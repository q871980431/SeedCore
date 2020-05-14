/*
 * File:	ScriptEngine.cpp
 * Author:	xuping
 * 
 * Created On 2017/8/1 21:33:16
 */

#include "ScriptEngine.h"
#include "lua.hpp"
#include "LuaInterchange.h"
ScriptEngine * ScriptEngine::s_self = nullptr;
IKernel * ScriptEngine::s_kernel = nullptr;
lua_State * ScriptEngine::s_luaState = nullptr;
s64 ScriptEngine::s_callId = 0;

bool ScriptEngine::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;

    s_luaState = luaL_newstate();
    luaL_openlibs(s_luaState);

    char scriptPath[MAX_PATH];
    const char *envi = s_kernel->GetEnvirPath();
    SafeSprintf(scriptPath, sizeof(scriptPath), "%s/script", envi);
	ECHO("LUA PATH:%s", scriptPath);
    SetSearchPath(scriptPath);
	ReplacePrint();
    ExecScriptFile(LINK_FILE_NAME);

    //lua_getglobal(s_luaState, LUA_TRACE_NAME);

    return true;
}

bool ScriptEngine::Launched(IKernel *kernel)
{
    return true;
}

bool ScriptEngine::Destroy(IKernel *kernel)
{
    lua_close(s_luaState);
    DEL this;
    return true;
}

void ScriptEngine::RegModuleFunc(const char *module, const char *func, const IDataInterchangeFuncType &f)
{
    char path[MAX_PATH];
    SafeSprintf(path, sizeof(path), "serverd.%s", module);

    lua_getglobal(s_luaState, PACKAGE);                 // pkg
    lua_getfield(s_luaState, -1, PKG_MODULE_INDEX);     // pkg modules 
    lua_pushstring(s_luaState, path);                   // pkg modules path
    lua_rawget(s_luaState, -2);                         // pkg modules moudules[path]
    if (lua_isnil(s_luaState, -1))
    {
        lua_pop(s_luaState, 1);
        lua_pushstring(s_luaState, path);               // pkg modules path
        lua_newtable(s_luaState);                       // pkg modules path table
        lua_rawset(s_luaState, -3);                     // pkg modules

        lua_pushstring(s_luaState, path);
        lua_rawget(s_luaState, -2);
    }
    // pkg modules modules[path]
    lua_pushstring(s_luaState, func);
    void *userdata = lua_newuserdata(s_luaState, sizeof(IDataInterchangeFuncType));
    IDataInterchangeFuncType *funType = NEW(userdata)IDataInterchangeFuncType();
    *funType = f;

    lua_pushcclosure(s_luaState, LuaCall, 1);
    lua_rawset(s_luaState, -3);
    lua_pop(s_luaState, 3);
}

bool ScriptEngine::CallScriptFunc(const char *module, const char *func, const IDataOutputFuncType &outPutFunc, const IDataCallBackFuncType &callBackFun)
{
	//s64 callId = ++s_callId;
	//s32 top = lua_gettop(s_luaState);
	//TRACE_LOG("call script %ld start, top = %d", callId, top);
	//PrintLuaStack();
	LuaInterchangeCaller caller(this, s_luaState);
	caller.PreCall(module, func);
	if (outPutFunc != nullptr)
	{
		IDataOutputStream *outStream = caller.GetOutputStream();
		outPutFunc(s_kernel, *outStream);
	}
	bool ret = caller.Call(s_kernel, callBackFun);
	//PrintLuaStack();
	//s32 nowTop = lua_gettop(s_luaState);
	//TRACE_LOG("call script %ld end, top = %d", callId, nowTop);
	//ASSERT(top == nowTop, "stack error");
	return ret;
}

void ScriptEngine::TestCallLuaFunction()
{
    lua_getglobal(s_luaState, "test");
    lua_pushnumber(s_luaState, 1);
    lua_pushnumber(s_luaState, 2);
    lua_call(s_luaState, 2, 0);
}

void ScriptEngine::SetSearchPath(const char *path)
{
    lua_getglobal(s_luaState, PACKAGE);
    lua_getfield(s_luaState, -1, PKG_PATH_INDEX);
    const char *cur_path = lua_tostring(s_luaState, -1);
    ECHO("CUR_PATH:%s", cur_path);
    char buff[1024];
    //SafeSprintf(buff, sizeof(buff), "%s;%s/?.lua", cur_path, path);
    SafeSprintf(buff, sizeof(buff), "%s/?.lua", path);
    lua_pushstring(s_luaState, buff);
    lua_setfield(s_luaState, -3, PKG_PATH_INDEX);
    lua_pop(s_luaState, 2);
}

bool ScriptEngine::ExecScriptFile(const char *file)
{
    char buff[512];
    SafeSprintf(buff, sizeof(buff), "return require \"%s\"", file);
    int32_t loaded = luaL_loadstring(s_luaState, buff);
	if (LUA_OK == loaded)
	{
		TRACE_LOG("loaded ok");
	}
	else {
		TRACE_LOG("loaded error:%d", loaded);
	}

    return ExecFunction(0, nullptr);
}

int ScriptEngine::LuaCall(lua_State *state)
{
    IDataInterchangeFuncType func = *(IDataInterchangeFuncType *)lua_touserdata(s_luaState, lua_upvalueindex(1));
    ASSERT(func != nullptr, "error");
    LuaInputStream input(s_luaState);
    LuaOutputStream output(s_luaState);
    func(s_kernel, input, output);

    return output.Count();
}

int ScriptEngine::Log(lua_State *state)
{
	//ECHO("Enter Script Engine Log");
    tlib::TString<4096> content;
    luaL_where(state, 1);
    content << lua_tostring(state, -1);
    lua_pop(state, 1);
    switch (lua_type(state, 1))
    {
    case LUA_TNIL: content << "nil"; break;
    case LUA_TNUMBER:
    {
        if (lua_isinteger(state, 1))
            content << (s64)lua_tointeger(state, 1);
        else
            content << (float)lua_tonumber(state, 1);
    }
        break;
    case LUA_TBOOLEAN:content << ((lua_toboolean(state, 1) ? "true" : "false")); break;
    case LUA_TSTRING: content << lua_tostring(state, 1); break;
    default:content << luaL_typename(state, 1) << ":" << (s64)lua_topointer(state, 1);break;
    }
    core::IKernel *kernel = s_kernel;
	LABEL_LOG(core::LOG_LEVEL_DEBUG, "LUA", "%s", content.GetString());
    return 0;
}

bool ScriptEngine::ExecGlobalFunction(const char *func, s8 argc, const IDataCallBackFuncType callback)
{
	IKernel *kernel = s_kernel;
	lua_getglobal(s_luaState, func);				//将func 函数压入栈中
	if (!lua_isfunction(s_luaState, -1))
	{
		ASSERT(false, "error, don't find function: %s", func);
		lua_pop(s_luaState, 1);
		return false;
	}
	//PrintLuaStack();
	//将func移动到距离栈顶 argc+1的位置
	if (argc > 0)
	{
		lua_insert(s_luaState, -(argc + 1)); 
	}
	//PrintLuaStack();
	return ExecFunction(argc, callback);
}

void ScriptEngine::PrintLuaStack()
{
	IKernel *kernel = s_kernel;
	lua_State* pLuaState = s_luaState;
	s32 stackTop = lua_gettop(pLuaState);//获取栈顶的索引值
	TRACE_LOG(" element count: %d", stackTop); 
	TRACE_LOG("--栈顶(v)(%d)--", stackTop);
	//显示栈中的元素	
	for(s32 nIdx = stackTop; nIdx > 0; --nIdx)
	{		
		s32 nType = lua_type(pLuaState, nIdx);
		switch (nType)
		{
		case LUA_TBOOLEAN:
		case LUA_TNUMBER: {
			if (lua_isinteger(pLuaState, nIdx))
			{
				s64 val = lua_tonumber(pLuaState, nIdx);
				TRACE_LOG("(i:%d) %s(%ld)", nIdx, lua_typename(pLuaState, nType), val);
			}
			else
			{
				TRACE_LOG("(i:%d) %s(%f)", nIdx, lua_typename(pLuaState, nType), lua_tonumber(pLuaState, nIdx));
			}

		}
			break;
		default:
			TRACE_LOG("(i:%d) %s(%s)", nIdx, lua_typename(pLuaState, nType), lua_tostring(pLuaState, nIdx));
		}
	} 
	TRACE_LOG("--栈底--");
}

void ScriptEngine::ReloadScript(const char *module)
{
	lua_pushstring(s_luaState, module);
	s32 ret = 0;
	auto func = [&ret](IKernel *kernel, IDataInputStream &input)
	{
		input.ReadInt32(ret);
	};
	ExecGlobalFunction("reload", 1, nullptr);
	TRACE_LOG("Reload Mod:%s", module);
}

bool ScriptEngine::ExecFunction(s8 argc, const IDataCallBackFuncType callback)
{
    core::IKernel *kernel = s_kernel;
    s8 findex = -(argc + 1);
    if (!lua_isfunction(s_luaState, findex))
    {
        lua_pop(s_luaState, -findex);
        return false;
    }
    s32 tarceIndex = 0;
    lua_getglobal(s_luaState, LUA_TRACE_NAME);
    if (!lua_isfunction(s_luaState, -1))
    {
        lua_pop(s_luaState, 1);
    }
    else
    {
        tarceIndex = findex - 1;
        lua_insert(s_luaState, tarceIndex);
    }
    s32 error = lua_pcall(s_luaState, argc, LUA_RESULT_COUNT, tarceIndex);
    if (error != LUA_OK)
    {
        if (tarceIndex == 0)
        {
            ERROR_LOG("Lua error:%s", lua_tostring(s_luaState, -1));
            lua_pop(s_luaState, 1);
        }
		else
		{
			ERROR_LOG("%s", lua_tostring(s_luaState, -1))
			lua_pop(s_luaState, 2);
		}
        return false;
    }
    LuaInputStream ack(s_luaState, true);
    if (callback != nullptr)
        callback(s_kernel, ack);
    lua_pop(s_luaState, LUA_RESULT_COUNT);
    if (tarceIndex != 0)
        lua_pop(s_luaState, 1);

    return true;
}

void ScriptEngine::ReplacePrint()
{
    lua_register(s_luaState, "print", Log);
    //lua_pushcclosure(s_luaState, Log, 0);//lua_setglobal(s_luaState,)
}

void ScriptEngine::CallPtfMod(const std::string &modName)
{
	lua_pushstring(s_luaState, modName.c_str());
	s32 ret = 0;
	auto func = [&ret](IKernel *kernel, IDataInputStream &input)
	{
		input.ReadInt32(ret);
	};
	ExecGlobalFunction("ptfmod", 1, nullptr);
	TRACE_LOG("CallPtf Mod:%s", modName.c_str());
	return ;
}
