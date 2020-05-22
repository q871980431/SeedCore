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
#include "win32.h"

RedisMgr * RedisMgr::s_self = nullptr;
IKernel * RedisMgr::s_kernel = nullptr;
RedisConnectionConfig RedisMgr::s_connectionConfig;
VecRedisContext RedisMgr::s_vecRedisContext;
s32 RedisMgr::s_connectNum;

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

		s_vecRedisContext.push_back(ctx);
	}
	s_connectNum = s_connectionConfig.connectNum;
    
    return true;
}

void __redisSetErrorInner(redisContext *c, int type, const char *str) {
	size_t len;

	c->err = type;
	if (str != NULL) {
		len = strlen(str);
		len = len < (sizeof(c->errstr) - 1) ? len : (sizeof(c->errstr) - 1);
		memcpy(c->errstr, str, len);
		c->errstr[len] = '\0';
	}
	else {
		/* Only REDIS_ERR_IO may lack a description! */
		assert(type == REDIS_ERR_IO);
		strerror_r(errno, c->errstr, sizeof(c->errstr));
	}
}

int redisBufferWriteInner(redisContext *c) {

	/* Return early when the context has seen an error. */
	if (c->err)
		return REDIS_ERR;

	int32_t *len = ((int32_t *)c->obuf - 1);
	while (*len > 0)
	{
		int32_t nwritten = send(c->fd, c->obuf, *len, 0);
		if (nwritten < 0) {
			if ((errno == EWOULDBLOCK && !(c->flags & REDIS_BLOCK)) || (errno == EINTR)) {
				/* Try again later */
			}
			else {
				__redisSetErrorInner(c, REDIS_ERR_IO, NULL);
				return REDIS_ERR;
			}
		}
		*len -= nwritten;
		c->obuf = c->obuf + nwritten;
	}
	return REDIS_OK;
}


int redisGetReplyInner(redisContext *c, void **reply) {
	void *aux = NULL;
	if (redisBufferWriteInner(c) == REDIS_ERR)
		return REDIS_ERR;

	/* Read until there is a reply */
	do {
		if (redisBufferRead(c) == REDIS_ERR)
			return REDIS_ERR;
		if (redisGetReplyFromReader(c, &aux) == REDIS_ERR)
			return REDIS_ERR;
	} while (aux == NULL);

	/* Set reply or free it if we were passed NULL */
	if (reply != NULL) {
		*reply = aux;
	}
	else {
		freeReplyObject(aux);
	}

	return REDIS_OK;
}

bool RedisMgr::Launched(IKernel *kernel)
{
	RedisCmdBuilder builder;
	builder << "keys" << "*";
	builder.Commit();
	redisContext &ctx = GetRedisContext(0);
	char *newCmd;
	int len;
	len = redisFormatCommand(&newCmd, "GET xuping");
	redisReply *reply = nullptr;
	int32_t ret2 = sendAndGetReply(&ctx, builder, reply);
	if (ret2 == REDIS_OK && reply != nullptr)
	{
		ECHO("OK");
		RedisResult result(reply);
		bool ret = result.ForeachArray([](RedisResult &tmp) {
			const char *val;
			int32_t len;
			bool ret = tmp.GetStr(val, len);
			if (ret)
			{
				ECHO("Key:%s", val);
			}
		});
		if (ret)
		{
			ECHO("keys * ok");
		}
	}
	TestPipeline();

    return true;
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

	redisContext &ctx = GetRedisContext(0);
	VecRedisReply vecReply;
	int32_t ret = pipelineSendAndGetReply(&ctx, vecBuilder, vecReply);
	if (ret == REDIS_OK)
	{
		ECHO("pipeline ok");
	}
}

int32_t RedisMgr::execCmd(s32 threadIdx, const RedisCmdBuilder &cmdBuilder, redisReply *&reply)
{
	redisContext &ctx = GetRedisContext(0);
	int32_t ret = sendAndGetReply(&ctx, cmdBuilder, reply);
	if (ret == REDIS_ERR_IO)
	{
		redis
	}

}

int32_t RedisMgr::batchExecCmd(s32 threadIdx, const VecRedisBuilder &vecBuilder, VecRedisReply &vecReply)
{

}


bool RedisMgr::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}

int32_t RedisMgr::sendAndGetReply(redisContext *ct, const RedisCmdBuilder &cmdBuilder, redisReply *&reply)
{
#ifdef _DEBUG
	if (!cmdBuilder.IsCommit())
	{
		ASSERT(false, "builder uncommit");
		return REDIS_ERR;
	}
#endif

	ct->obuf = (char*)cmdBuilder.GetSendBuff();
	return redisGetReplyInner(ct, (void**)&reply);
}

int32_t RedisMgr::pipelineSendAndGetReply(redisContext *ct, VecRedisBuilder &vecBuilder, VecRedisReply &vecReply)
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
		if (redisBufferWriteInner(ct) == REDIS_ERR)
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

redisContext & RedisMgr::GetRedisContext(s32 threadIdx)
{
	if (threadIdx == MAIN_THREAD_IDX)
	{
		return *s_vecRedisContext[0];
	}
	threadIdx = ((u32)threadIdx % s_connectNum) + 1;
	return *s_vecRedisContext[threadIdx];
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