﻿/*
 * File:	ScriptEngine.h
 * Author:	xuping
 * 
 * Created On 2017/8/1 21:33:16
 */

#ifndef __ScriptEngine_h__
#define __ScriptEngine_h__
#include "IScriptEngine.h"

class lua_State;
class LuaInterchangeCaller;
static const char *PACKAGE = "package";
static const char *PKG_PATH_INDEX = "path";
static const char *PKG_MODULE_INDEX = "loaded";
static const char *LUA_TRACE_NAME = "__G__TRACKBACK__";
static const char *LINK_FILE_NAME = "link";
static const s8    LUA_RESULT_COUNT = 8;

class ScriptEngine : public IScriptEngine
{
public:
    virtual ~ScriptEngine(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);
    virtual void RegModuleFunc(const char *module, const char *func, const IDataInterchangeFuncType &f);
	virtual bool CallScriptFunc(const char *module, const char *func, const IDataOutputFuncType &outPutFunc, const IDataCallBackFuncType &callBackFun);
	virtual void PrintLuaStack();
	virtual void ReloadScript(const char *module);

public:
    bool ExecGlobalFunction(const char *func, s8 argc, const IDataCallBackFuncType callback);

private:
    void TestCallLuaFunction();
    void SetSearchPath(const char *path);
    bool ExecScriptFile(const char *file);
    bool ExecFunction(s8 argc, const IDataCallBackFuncType callback);
    void ReplacePrint();
	void CallPtfMod(const std::string &modName);

private:
    static int LuaCall(lua_State *state);
    static int Log(lua_State *state);

private:
    static ScriptEngine     * s_self;
    static IKernel  * s_kernel;
    static lua_State * s_luaState;
	static s64			s_callId;
};
#endif