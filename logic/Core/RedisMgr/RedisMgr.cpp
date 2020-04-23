/*
 * File:	RedisMgr.cpp
 * Author:	xuping
 * 
 * Created On 2020/4/7 18:09:35
 */

#include "RedisMgr.h"
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

    return true;
}

bool RedisMgr::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}


