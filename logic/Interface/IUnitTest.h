/*
 * File:	IUnitTest.h
 * Author:	XuPing
 * 
 * Created On 2019/5/16 16:19:20
 */

#ifndef  __IUnitTest_h__
#define __IUnitTest_h__
#include "IModule.h"

#define TEST_EQ(A, B) if ((A) != (B))\
{\
	constexpr static const char * name = getFileName(__FILE__, sizeof(__FILE__) - 1);\
	std::stringstream ostream;\
	ostream << (A) << " don't eq " << (B);\
	std::string tmp(ostream.str());\
    char log[LOG_BUFF_SIZE] = { 0 }; \
    SafeSprintf(log, sizeof(log), "[%s]: %s:%d:%s | TEST EQ FAILED:%s", "Error", name, __LINE__, __FUNCTION__, tmp.c_str()); \
	PrintToConsel(core::G_KERNEL::g_stdHandle, 0x02, log);\
    core::G_KERNEL::g_kernel->ThreadLog("UnitTest", log); \
}

#define TEST_LOG(format, ...)\
{\
	constexpr static const char * tmpFileName = getFileName(__FILE__, sizeof(__FILE__) - 1);\
    char log[LOG_BUFF_SIZE] = { 0 }; \
    SafeSprintf(log, sizeof(log), "[UnitTest]: %s:%d:%s | "#format, tmpFileName, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
	PrintToConsel(core::G_KERNEL::g_stdHandle, 0x02, log);\
    core::G_KERNEL::g_kernel->ThreadLog("UnitTest", log); \
}


class IUnitTest : public IModule
{
public:
    virtual ~IUnitTest(){};
	virtual s32 GetUnitTestId() = 0;
	virtual void CompleteTest(s32 testId) = 0;
};

class IUnitTestInstance
{
public:
	virtual ~IUnitTestInstance() {};
	virtual void StartTest(core::IKernel *kernel) = 0;
};

#endif