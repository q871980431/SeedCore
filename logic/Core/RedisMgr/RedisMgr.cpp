/*
 * File:	RedisMgr.cpp
 * Author:	xuping
 * 
 * Created On 2020/4/7 18:09:35
 */

#include "RedisMgr.h"
#include "hiredis.h"
#include "XmlReader.h"
#include "RedisHelper.h"
#include "Tools.h"
#include "Tools_time.h"
#include <set>


const int32_t REATTEMPT_SLEEP_TIME = 1000;
const int32_t CONNECTION_TIMER_TIME =  60 * 1000;
const int32_t REDIS_KEY_TOP_COUNT = 20;
int32_t REDIS_ASYNC_BATCH_CMD_COUNT = 16;

RedisMgr * RedisMgr::s_self = nullptr;
IKernel * RedisMgr::s_kernel = nullptr;
RedisConnectionConfig RedisMgr::s_connectionConfig;
VecConnection RedisMgr::s_vecRedisContext;
s32 RedisMgr::s_connectNum;
core::IAsyncQueue * RedisMgr::s_asyncQueue = nullptr;

bool RedisMgr::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
	if (!LoadConfigFile())
		return false;

	for (int32_t i = 0; i < s_connectionConfig.connectNum+1; i++)
	{
		redisContext *ctx = CreateRedisContext(s_connectionConfig.connectTV);
		if (ctx == nullptr)
			return false;

		RedisConnection connection;
		connection.ctx = ctx;
		connection._threadCmdList = NEW RedisCmdList();
#ifdef _DEBUG
		connection._watchMutex = NEW std::mutex();
#endif
		s_vecRedisContext.push_back(connection);
	}
	s_connectNum = s_connectionConfig.connectNum;
	s_asyncQueue = s_kernel->GetMainAsyncQueue();
    
    return true;
}




bool RedisMgr::Launched(IKernel *kernel)
{	
	ConnetionProfileTimer *profileTimer = NEW ConnetionProfileTimer();
	ADD_TIMER(profileTimer, CONNECTION_TIMER_TIME, FOREVER, CONNECTION_TIMER_TIME);

    return true;
}

void RedisMgr::OnFlushConnectionTimer()
{
	s32 threadNum = 0;
	std::set<s32> threadIds;
	s_asyncQueue->GetQueueInfo(threadNum, threadIds);
	for (auto threadId : threadIds)
	{
		FlushConnectionHandler *flushConnectionHandler = NEW FlushConnectionHandler(threadId);
		s_asyncQueue->StartAsync(threadId, flushConnectionHandler, "", __LINE__);
	}
}

void RedisMgr::OnFlushConnection(s32 threadIdx)
{
	RedisConnection &connection = GetRedisConnection(threadIdx);
	{
		std::lock_guard<std::mutex> lockGurd(*connection._watchMutex);
		connection._mainWatch = connection._threadWatch;
		connection._mainKeyProfileMap.clear();
		connection._mainKeyProfileMap = connection._threadKeyProfileMap;
	}
}

void RedisMgr::OnExecRedisTask(RedisConnection *redisConnection)
{
	RedisCmdNode *head = redisConnection->_threadCmdList->cutList();
	VecRedisBuilder vecRedisBuilder;
	VecRedisReply vecRedisReply;

	while (head)
	{
		auto tmp = head;
		head = head->hook_.next;

		vecRedisBuilder.push_back(tmp->cmdBuilder);
		if (vecRedisBuilder.size() == REDIS_ASYNC_BATCH_CMD_COUNT || head == nullptr)
		{
			if (vecRedisBuilder.size() > 0)
				s_self->batchExecCmd(redisConnection->_threadIdx, vecRedisBuilder, vecRedisReply, true);

			for (auto &iter : vecRedisReply)
			{
				RedisResult tmpResult(iter);
			}
			for (auto iter : vecRedisBuilder)
			{
				DEL iter;
			}
			vecRedisBuilder.clear();
			vecRedisReply.clear();
		}
		DEL tmp;
	}

	redisConnection->_postTask = false;
}


void RedisMgr::TestPipeline()
{
	VecRedisBuilder vecBuilder;
	RedisCmdBuilder builder;
	builder << "GET" << "xuping";
	builder.Commit();
	vecBuilder.push_back(&builder);
	RedisCmdBuilder builder1;
	builder1 << "SET" << "xuping1" << "this is test";
	builder1.Commit();
	vecBuilder.push_back(&builder1);
	RedisCmdBuilder builder2;
	builder2 << "GET" << "xuping1";
	builder2.Commit();
	vecBuilder.push_back(&builder2);

	auto &connection = GetRedisConnection(0);
	VecRedisReply vecReply;
	int32_t ret = pipelineSendAndGetReply(connection.ctx, vecBuilder, vecReply);
	if (ret == REDIS_OK)
	{
		ECHO("pipeline ok");
	}
}

void RedisMgr::addAsyncTask(s64 taskId, RedisCmdBuilder *cmdBuilderPtr)
{
	auto &connection = GetRedisConnectionInAsync(taskId);
	RedisCmdNode *cmdNode =  NEW RedisCmdNode();
	cmdNode->cmdBuilder = cmdBuilderPtr;
	connection._threadCmdList->insertHead(cmdNode);
	if (connection._redisTaskHandler == nullptr)
		connection._redisTaskHandler = NEW RedisTaskHandler(&connection);
	if (connection._postTask = false)
	{
		s_asyncQueue->StartAsync(connection._threadIdx, connection._redisTaskHandler, "async task", __LINE__);
		connection._postTask = true;
	}
}

int32_t RedisMgr::execCmd(s32 threadIdx, const RedisCmdBuilder &cmdBuilder, redisReply **reply, bool reSend)
{
	auto &connection = GetRedisConnection(threadIdx);
	int32_t ret = sendAndGetReply(connection.ctx, cmdBuilder, reply);
	if (reSend && ret == REDIS_ERR_IO)
	{
		auto callFun = [&cmdBuilder, &reply](RedisConnection &redisConnetion)->bool
		{
			int32_t tmpRet = sendAndGetReply(redisConnetion.ctx, cmdBuilder, reply);
			return tmpRet != REDIS_ERR_IO;
		};
		execTaskByTimeOut(connection, callFun);
	}
	return 0;
}

int32_t RedisMgr::batchExecCmd(s32 threadIdx, const VecRedisBuilder &vecBuilder, VecRedisReply &vecReply, bool reSend)
{
	auto &connection = GetRedisConnection(threadIdx);
	int32_t ret = pipelineSendAndGetReply(connection.ctx, vecBuilder, vecReply);
	if (reSend && ret == REDIS_ERR_IO)
	{
		auto callFun = [&vecBuilder, &vecReply](RedisConnection &redisConnetion)->bool
		{
			int32_t tmpRet = pipelineSendAndGetReply(redisConnetion.ctx, vecBuilder, vecReply);
			return tmpRet != REDIS_ERR_IO;
		};
		execTaskByTimeOut(connection, callFun);
	}
	return REDIS_OK;
}


bool RedisMgr::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}

struct KeyProfileNode
{
	int32_t iCount;
	std::string strKey;
	KeyProfileNode() {};
	KeyProfileNode(const KeyProfileNode &stVal) : iCount(stVal.iCount), strKey(stVal.strKey) {};
	KeyProfileNode(KeyProfileNode &&stVal)
	{
		iCount = stVal.iCount;
		strKey.swap(stVal.strKey);
	}
	bool operator<(const KeyProfileNode &stVal) const
	{
		if (iCount == stVal.iCount)
		{
			return strKey > stVal.strKey;
		}
		return iCount > stVal.iCount;
	}
};

std::string GetProfileAndReset(KeyProfileMap mapKeyProfile)
{
	typedef std::set<KeyProfileNode> KeyProfileSet;
	KeyProfileSet setKeyProfile;
	std::string strProfile;
	int64_t iCount = 0;
	for (auto &iter : mapKeyProfile)
	{
		iCount += iter.second;

		KeyProfileNode stProfileNode;
		stProfileNode.iCount = iter.second;
		stProfileNode.strKey = iter.first;
		setKeyProfile.insert(std::move(stProfileNode));
	}
	auto iter = setKeyProfile.begin();
	for (int32_t i = 0; (i < REDIS_KEY_TOP_COUNT) && (iter != setKeyProfile.end()); i++, iter++)
	{
		strProfile.append("\n Key:");
		strProfile.append(iter->strKey);
		strProfile.append(" Use Count:");
		strProfile.append(std::to_string(iter->iCount));
	}
	strProfile.append("\nTotal Key Count:");
	strProfile.append(std::to_string(iCount));
	return strProfile;
}

std::string RedisMgr::ProfileInfo()
{
	std::stringstream ostream;
	for (size_t i = 0; i < s_vecRedisContext.size(); i++)
	{
		auto &connection = s_vecRedisContext[i];
		RedisExecWatch watch;
		KeyProfileMap profileMap;
		{
			std::lock_guard<std::mutex> lockGuard(*connection._watchMutex);
			watch = connection._mainWatch;
			profileMap = connection._mainKeyProfileMap;
		}

		ostream << "connection " << i << " info:";
		ostream << "\nexpends:\n" << watch.CreatePerformanceInfo();
		ostream << "\nkey profile:\n" << GetProfileAndReset(profileMap) << "\n";
	}

	return ostream.str();
}

int32_t RedisMgr::sendAndGetReply(redisContext *ct, const RedisCmdBuilder &cmdBuilder, redisReply **reply)
{
#ifdef _DEBUG
	if (!cmdBuilder.IsCommit())
	{
		ASSERT(false, "builder uncommit");
		return REDIS_ERR;
	}
#endif

	ct->obuf = (char*)cmdBuilder.GetSendBuff();
	return RedisWraaper::redisGetReplyInner(ct, (void**)reply);
}

int32_t RedisMgr::pipelineSendAndGetReply(redisContext *ct, const VecRedisBuilder &vecBuilder, VecRedisReply &vecReply)
{
	int32_t iSize = (int32_t)vecBuilder.size();
	for (const auto &iter : vecBuilder)
	{
#ifdef _DEBUG
		if (!iter->IsCommit())
		{
			ASSERT(false, "builder uncommit");
			return REDIS_ERR;
		}
#endif
		ct->obuf = (char *)iter->GetSendBuff();
		if (RedisWraaper::redisBufferWriteInner(ct) == REDIS_ERR)
			return REDIS_ERR;
	}

	for (int32_t i = 0; i < iSize; i++)
	{
		void *aux = nullptr;
		/* Read until there is a reply */
		if (redisGetReplyFromReader(ct, &aux) == REDIS_ERR)
			return REDIS_ERR;
		if (aux != nullptr)
		{
			vecReply.push_back((redisReply*)aux);
			continue;
		}

		do {
			if (redisBufferRead(ct) == REDIS_ERR)
				return REDIS_ERR;
			if (redisGetReplyFromReader(ct, &aux) == REDIS_ERR)
				return REDIS_ERR;
		} while (aux == NULL);
		vecReply.push_back((redisReply*)aux);
	}

	return REDIS_OK;
}

void RedisMgr::execTaskByTimeOut(RedisConnection & connection, const std::function<bool(RedisConnection &connetion)> &callFunc)
{
	s64 enterTime = tools::GetTimeMillisecond();
	do 
	{
		redisContext *ctx = CreateRedisContext(s_connectionConfig.connectTV);
		if (ctx != nullptr)
		{
			redisFree(connection.ctx);
			connection.ctx = ctx;
			if (callFunc(connection))
				break;
		}
		s64 now = tools::GetTimeMillisecond();
		if (now - enterTime > s_connectionConfig.reattemptTime)
		{
			break;
		}
		Sleep(REATTEMPT_SLEEP_TIME);
	} while (true);
}


redisContext * RedisMgr::CreateRedisContext(const timeval &tv)
{
	redisContext *ctx = redisConnectWithTimeout(s_connectionConfig.ip.c_str(), s_connectionConfig.port, tv);
	if (ctx == nullptr || ctx->err)
	{
		ERROR_LOG("redis connect failed, ip:%s, port:%d", s_connectionConfig.ip.c_str(), s_connectionConfig.port);
		return nullptr;
	}

	if (!s_connectionConfig.passWord.empty())
	{
		redisReply *reply = (redisReply *)redisCommand(ctx, "AUTH %s", s_connectionConfig.passWord.c_str());
		if (reply == nullptr || reply->type != REDIS_REPLY_ERROR)
		{
			ERROR_LOG("redis auth failed, ip:%s, port:%d, password:%s", s_connectionConfig.ip.c_str(),
				s_connectionConfig.port, s_connectionConfig.passWord.c_str());
			redisFree(ctx);
			return nullptr;
		}
	}

	return ctx;
}

RedisConnection & RedisMgr::GetRedisConnection(s32 threadIdx)
{
	if (threadIdx == MAIN_THREAD_IDX)
	{
		return s_vecRedisContext[0];
	}
	threadIdx = ((u32)threadIdx % s_connectNum) + 1;
	return s_vecRedisContext[threadIdx];
}

RedisConnection & RedisMgr::GetRedisConnectionInAsync(s64 taskId)
{
	taskId = ((u64)taskId % s_connectNum) + 1;
	return s_vecRedisContext[taskId];
}


void RedisMgr::RecordExecKey(s32 threadIdx, const std::string &key)
{
	(void)threadIdx;
	(void)key;
#ifdef _DEBUG

#endif
}


bool RedisMgr::LoadConfigFile()
{
	const char *configFile = s_kernel->GetConfigFile();
	XmlReader reader;
	if (!reader.LoadFile(configFile))
	{
		ASSERT(false, "load config error, config file:%s", configFile);
		return false;
	}
	IXmlObject *root = reader.Root();
	IXmlObject *redisObj = root->GetFirstChrild("redis");
	if (!redisObj)
	{
		ASSERT(false, "wtf");
		return false;
	}

	s_connectionConfig.ip = redisObj->GetAttribute_Str("ip");
	s_connectionConfig.port = redisObj->GetAttribute_S32("port");
	s_connectionConfig.connectNum = redisObj->GetAttribute_S32("connectnum");
	s_connectionConfig.connectTV.tv_sec = redisObj->GetAttribute_S32("connecttime");
	s_connectionConfig.connectTV.tv_usec = 0;
	s_connectionConfig.passWord = redisObj->GetAttribute_Str("password");
	if (s_connectionConfig.connectTV.tv_sec < 0)
		s_connectionConfig.connectTV.tv_sec = 5;

	return true;
}