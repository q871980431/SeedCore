#ifndef __MysqlTest_h__
#define __MysqlTest_h__

#include "IUnitTest.h"
#include "IScriptEngine.h"

struct PlayerInfo
{
	std::string name;
	s64 id;
};

class ScriptEngineTest : public IUnitTestInstance
{
public:
	virtual void StartTest(core::IKernel *kernel);

protected:

private:
	void		testCallLuaFun();	//调用lua方法
	std::string testLuaCallEcho(std::string &content);

private:
	void		testInputPtrToLua();	//测试以指针传入lua
	int32_t		testLuaPtr(const PlayerInfo *playerInfo);
	static	void getPlayerId(IKernel *kernel, IDataInputStream &input, IDataOutputStream &out);

private:
	void testLoopCall();				//测试C++和Lua混合调用
	s64 FibNumber(u16 num);
	static s64 testFibNumber(s32 num);
	static void callFibNumer(IKernel *kernel, IDataInputStream &input, IDataOutputStream &out);

private:
	void testLuaReload();				//测试lua代码热更新重加载
	int32_t callTestReload(int32_t num);

private:
	void testErrorLuaFile();			//测试调用 有错误的lua文件
	void testExceptionLuaFunc();		//测试调用 运行时错误的lua 函数
	void testInnerExceptionLuaFunc();	//测试 嵌套调用中在内层lua函数异常时, 外层C++调用是否也异常
	static void callExceptionLuaFunc(IKernel *kernel, IDataInputStream &input, IDataOutputStream &out);

private:
	static IScriptEngine *_scriptEngine;
};

#endif
