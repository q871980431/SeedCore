/*
 * File:	IRedisMgr.h
 * Author:	xuping
 * 
 * Created On 2020/4/7 18:09:35
 */

#ifndef  __IRedisMgr_h__
#define __IRedisMgr_h__
#include "IModule.h"

enum REDIS_STAT
{
	REDIS_STAT_OK = 0,
	REDIS_STAT_NODATA = 1,
	REDIS_STAT_TIMEOUT = 2,
};
class IRedisMgr : public IModule
{
public:
    virtual ~IRedisMgr(){};
    
};
#endif