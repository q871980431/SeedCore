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

struct RedisConnectionConfig 
{
	std::string ip;
	s32 port;
	s32 connectNum;
	timeval	connectTV;
	std::string passWord;
};

class redisContext;
class redisReply;
typedef std::vector<redisContext*> VecRedisContext;
typedef std::vector<const RedisCmdBuilder *> VecRedisBuilder;
typedef std::vector<redisReply*> VecRedisReply;
class RedisMgr : public IRedisMgr
{
public:
    virtual ~RedisMgr(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);
public:
#include "RedisAsyncCall.h"
#include "RedisSyncCall.h"

public:
	virtual s32 GetAsyncConnectNum() override { return s_connectNum; };

	void TestPipeline();
private:
	int32_t execCmd(s32 threadIdx, const RedisCmdBuilder &cmdBuilder, redisReply *&reply);
	int32_t batchExecCmd(s32 threadIdx, const VecRedisBuilder &vecBuilder, VecRedisReply &vecReply);
	int32_t sendAndGetReply(redisContext *ct, const RedisCmdBuilder &cmdBuilder, redisReply *&reply);
	int32_t pipelineSendAndGetReply(redisContext *ct, const VecRedisBuilder &vecBuilder, VecRedisReply &vecReply);

private:
	redisContext* CreateRedisContext(const timeval &tv);
	redisContext & GetRedisContext(s32 theadIdx);
	bool LoadConfigFile();

private:
    static RedisMgr     * s_self;
    static IKernel  * s_kernel;
	static RedisConnectionConfig	s_connectionConfig;
	static VecRedisContext	s_vecRedisContext;
	static s32		  s_connectNum;
};
#endif