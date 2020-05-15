/*
 * File:	RedisMgr.h
 * Author:	xuping
 * 
 * Created On 2020/4/7 18:09:35
 */

#ifndef __RedisMgr_h__
#define __RedisMgr_h__

#include "IRedisMgr.h"
#include <vector>

struct RedisConnectionConfig 
{
	std::string ip;
	s32 port;
	s32 connectNum;
	s32 connectTimeOut;		//s
	std::string passWord;
};

class redisContext;
typedef std::vector<redisContext*> VecRedisContext;
class RedisMgr : public IRedisMgr
{
public:
    virtual ~RedisMgr(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);
private:
	redisContext* CreateRedisContext();

	bool LoadConfigFile();
private:
    static RedisMgr     * s_self;
    static IKernel  * s_kernel;
	static RedisConnectionConfig	s_connectionConfig;
	static VecRedisContext	s_vecRedisContext;
	static s32		  s_connectNum;
};
#endif