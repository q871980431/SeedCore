/*
 * File:	IScriptEngine.h
 * Author:	xuping
 * 
 * Created On 2017/8/1 21:33:16
 */

#ifndef  __IScriptEngine_h__
#define __IScriptEngine_h__

#include "IModule.h"
#include "IDataInterchange.h"
#include <functional>

class IScriptEngine : public IModule
{
public:
    virtual ~IScriptEngine(){};
    virtual void RegModuleFunc(const char *module, const char *func, const IDataInterchangeFuncType &f) = 0;
	//在返回ret 为false时 不触发callBackFun 回调
	virtual bool CallScriptFunc(const char *module, const char *func, const IDataOutputFuncType &outPutFunc, const IDataCallBackFuncType &callBackFun) = 0;
	virtual void PrintLuaStack() = 0;
	virtual void ReloadScript(const char *module) = 0;
};

#endif