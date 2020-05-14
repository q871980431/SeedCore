/*
 * File:	RedisMgr.cpp
 * Author:	xuping
 * 
 * Created On 2020/4/7 18:09:35
 */

#include "RedisMgr.h"
#include "hiredis.h"

RedisMgr * RedisMgr::s_self = nullptr;
IKernel * RedisMgr::s_kernel = nullptr;
bool RedisMgr::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
    
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


