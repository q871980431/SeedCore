/*
 * File:	RedisMgr.h
 * Author:	xuping
 * 
 * Created On 2020/4/7 18:09:35
 */

#ifndef __RedisMgr_h__
#define __RedisMgr_h__
#include "IRedisMgr.h"

class RedisMgr : public IRedisMgr
{
public:
    virtual ~RedisMgr(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);
protected:
private:
    static RedisMgr     * s_self;
    static IKernel  * s_kernel;
};
#endif