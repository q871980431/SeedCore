/*
 * File:	RedisMgr.cpp
 * Author:	xuping
 * 
 * Created On 2020/4/7 18:09:35
 */

#include "RedisMgr.h"
#include "hiredis.h"
#include "XmlReader.h"

RedisMgr * RedisMgr::s_self = nullptr;
IKernel * RedisMgr::s_kernel = nullptr;

bool RedisMgr::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
	if (!LoadConfigFile())
		return false;

	struct timeval timeout = { s_connectionConfig.connectTimeOut, 0 };
	for (int32_t i = 0; i < s_connectionConfig.connectNum+1; i++)
	{
		redisContext *ctx = CreateRedisContext();
		if (ctx == nullptr)
			return false;

		s_vecRedisContext.push_back(ctx);
	}
	s_connectNum = s_connectionConfig.connectNum;
    
    return true;
}

bool RedisMgr::Launched(IKernel *kernel)
{
	redisContext *c;
	redisReply *reply;

	struct timeval timeout = { 1, 500000 };
	c = redisConnectWithTimeout((char *)"10.94.28.21", 6380, timeout);
	if (c->err)
	{
		printf("Connection error:%s\n", c->errstr);
		exit(1);
	}

    return true;
}

bool RedisMgr::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}

redisContext * RedisMgr::CreateRedisContext()
{
	redisContext *ctx = redisConnectWithTimeout(s_connectionConfig.ip.c_str(), s_connectionConfig.port, timeout);
	if (ctx == nullptr || ctx->err)
	{
		ERROR_LOG("redis connect failed, ip:%s, port:%d", s_connectionConfig.ip.c_str(), s_connectionConfig.port);
		return false;
	}

	if (!s_connectionConfig.passWord.empty())
	{
		redisReply *reply = (redisReply *)redisCommand(ctx, "AUTH %s", s_connectionConfig.passWord.c_str());
		if (reply == nullptr || reply->type != REDIS_REPLY_ERROR)
		{
		}
	}

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
	s_connectionConfig.connectTimeOut = redisObj->GetAttribute_S32("connecttime");
	s_connectionConfig.passWord = redisObj->GetAttribute_Str("password");
	if (s_connectionConfig.connectTimeOut < 0)
		s_connectionConfig.connectTimeOut = 5;

	return true;
}