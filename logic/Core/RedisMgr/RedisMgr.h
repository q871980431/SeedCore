/*
 * File:	RedisMgr.h
 * Author:	xuping
 * 
 * Created On 2020/4/7 18:09:35
 */

#ifndef __RedisMgr_h__
#define __RedisMgr_h__

#include "IRedisMgr.h"
#include "RedisHelper.h"
#include <vector>
#include "Tools_time.h"
#include "PerformanceWatch.h"
#include <mutex>
#include <unordered_map>
#include <memory>
#include "AtomicIntrusiveLinkedList.h"

struct RedisConnectionConfig 
{
	std::string ip;
	s32 port;
	s32 connectNum;
	timeval	connectTV;
	std::string passWord;
	s32 reattemptTime;
};

class redisContext;
class redisReply;
typedef PerformanceWatch<tools::StopWatch<>> RedisExecWatch;
typedef std::unordered_map<std::string, int32_t> KeyProfileMap;
//typedef std::list<std::unique_ptr<RedisCmdBuilder>> RedisCmdList;

struct  RedisCmdNode
{
	RedisCmdBuilder *cmdBuilder;
	folly::AtomicIntrusiveLinkedListHook<RedisCmdNode> hook_;
};
typedef folly::AtomicIntrusiveLinkedList<RedisCmdNode, &RedisCmdNode::hook_> RedisCmdList;

class RedisTaskHandler;
struct RedisConnection 
{
	redisContext *ctx{nullptr};
	RedisCmdList *_threadCmdList{nullptr};
	s32			 _threadIdx{1};
	RedisTaskHandler *_redisTaskHandler{nullptr};
	bool		_postTask{false};
#ifdef _DEBUG
	s32	_count{0};
	RedisExecWatch _mainWatch;
	RedisExecWatch _threadWatch;
	std::mutex	   *_watchMutex{nullptr};
	KeyProfileMap  _mainKeyProfileMap;
	KeyProfileMap  _threadKeyProfileMap;
#endif
};

typedef std::vector<RedisConnection> VecConnection;
typedef std::vector<const RedisCmdBuilder *> VecRedisBuilder;
typedef std::vector<redisReply*> VecRedisReply;

class RedisMgr : public IRedisMgr, public core::IModuleProfile
{
public:
    virtual ~RedisMgr(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);

	virtual const char * Name() { return "RedisMgr"; };
	virtual std::string ProfileInfo();
public:
#include "RedisAsyncCall.h"
#include "RedisSyncCall.h"

public:
	virtual void SetAsyncQueue(core::IAsyncQueue *asyncQueue) { s_asyncQueue = asyncQueue; };
	virtual s32 GetAsyncConnectNum() override { return s_connectNum; };

	void TestPipeline();
public:
	static void OnFlushConnectionTimer();
	static void OnFlushConnection(s32 threadIdx);
	static void OnExecRedisTask(RedisConnection *redisConnection);

private:
	void addAsyncTask(s64 taskId, RedisCmdBuilder *cmdBuilderPtr);
	int32_t execCmd(s32 threadIdx, const RedisCmdBuilder &cmdBuilder, redisReply **reply, bool reSend = true);
	int32_t batchExecCmd(s32 threadIdx, const VecRedisBuilder &vecBuilder, VecRedisReply &vecReply, bool reSend = true);
	inline s32 GetThreadIdx(s64 taskId) { return ((s32)taskId == MAIN_THREAD_IDX) ? 1 : (s32)taskId; }

	static int32_t sendAndGetReply(redisContext *ct, const RedisCmdBuilder &cmdBuilder, redisReply **reply);
	static int32_t pipelineSendAndGetReply(redisContext *ct, const VecRedisBuilder &vecBuilder, VecRedisReply &vecReply);

private:
	static void	execTaskByTimeOut(RedisConnection & connection, const std::function<bool(RedisConnection &connetion)> &callFunc);
	static redisContext* CreateRedisContext(const timeval &tv);
	static RedisConnection & GetRedisConnection(s32 theadIdx);
	static RedisConnection & GetRedisConnectionInAsync(s64 taskId);
	void RecordExecKey(s32 threadIdx, const std::string &key);
	bool LoadConfigFile();

private:
    static RedisMgr     * s_self;
    static IKernel  * s_kernel;
	static RedisConnectionConfig	s_connectionConfig;
	static VecConnection	s_vecRedisContext;
	static s32		  s_connectNum;
	static core::IAsyncQueue *s_asyncQueue;
};

//刷新连接信息的定时器
class ConnetionProfileTimer : public core::ITimer
{
public:
	ConnetionProfileTimer() {};
	virtual ~ConnetionProfileTimer() {};

	virtual void OnStart(IKernel *kernel, s64 tick) { (void)kernel; (void)tick; };
	virtual void OnTime(IKernel *kernel, s64 tick) {
		RedisMgr::OnFlushConnectionTimer();
	};
	virtual void OnTerminate(IKernel *kernel, s64 tick, bool isKill) { DEL this; };
private:
};

//刷新异步连接的Handler
class FlushConnectionHandler : public core::IAsyncHandler
{
public:
	FlushConnectionHandler(s32 threadIdx) :_threadIdx(threadIdx) {};
	virtual ~FlushConnectionHandler() {};

	virtual bool OnExecute(IKernel * kernel, s32 queueId, s32 threadIdx)
	{
		ASSERT(_threadIdx == threadIdx, "thread error");
		RedisMgr::OnFlushConnection(threadIdx);
		return true;
	};

	virtual void OnSuccess(IKernel * kernel) { (void)kernel; };
	virtual void OnFailed(IKernel * kernel, bool isExecuted) { (void)kernel; (void)isExecuted; };
	virtual void OnRelease(IKernel * kernel) { (void)kernel; DEL this; };
private:
	s32 _threadIdx;
};

//执行异步Redis任务Handler
class RedisTaskHandler : public core::IAsyncHandler
{
public:
	RedisTaskHandler(RedisConnection *redisConnection):_redisConnection(redisConnection){}
	virtual ~RedisTaskHandler() {};

	virtual bool OnExecute(IKernel * kernel, s32 queueId, s32 threadIdx)
	{
		RedisMgr::OnExecRedisTask(_redisConnection);
		return true;
	};
	virtual void OnSuccess(IKernel * kernel) { (void)kernel; };
	virtual void OnFailed(IKernel * kernel, bool isExecuted) { (void)kernel; (void)isExecuted; };
	virtual void OnRelease(IKernel * kernel) { (void)kernel; };
protected:
private:
	RedisConnection *_redisConnection;
};
#endif