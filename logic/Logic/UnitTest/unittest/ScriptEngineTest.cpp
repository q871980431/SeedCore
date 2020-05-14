#include "ScriptEngineTest.h"
#include "../UnitTest.h"
#include "Game_tools.h"

UNIT_TEST_REGISTER(ScriptEngineTest);
IScriptEngine * ScriptEngineTest::_scriptEngine = nullptr;
void ScriptEngineTest::StartTest(core::IKernel *kernel)
{
	FIND_MODULE(_scriptEngine, ScriptEngine);
	_scriptEngine->RegModuleFunc("scripttest", "callFibNumer", callFibNumer);
	_scriptEngine->RegModuleFunc("scripttest", "getPlayerId", getPlayerId);
	_scriptEngine->RegModuleFunc("scripttest", "callExceptionLuaFunc", callExceptionLuaFunc);

	testCallLuaFun();
	testLoopCall();
	testInputPtrToLua();
	testErrorLuaFile();
	testExceptionLuaFunc();
	testInnerExceptionLuaFunc();
	//testLuaReload();
}

void ScriptEngineTest::testCallLuaFun()
{
	std::string content{ "this is test echo" };
	std::string echo = testLuaCallEcho(content);
	if (echo == content)
	{
		IMPORTANT_LOG("LUATEST", "call echo success");
	}
	else {
		ASSERT(false, "content != echo");
	}
}


std::string ScriptEngineTest::testLuaCallEcho(std::string &content)
{
	auto outPutFun = [&content](IKernel *kernel, IDataOutputStream &output)
	{
		output.WriteStr(content.c_str());
	};
	std::string ret;
	auto readFun = [&ret](IKernel *kernel, IDataInputStream &input)
	{
		const char *content = nullptr;
		input.ReadStr(content);
		if (content != nullptr)
		{
			ret = content;
		}
	};
	_scriptEngine->CallScriptFunc("test.func", "testEcho", outPutFun, readFun);
	return ret;
}

void ScriptEngineTest::testInputPtrToLua()
{
	int32_t playerId = 10;
	PlayerInfo playerInfo{ "xuping", playerId };
	int32_t retId = testLuaPtr(&playerInfo);
	if (retId == playerId)
	{
		IMPORTANT_LOG("LUATEST", "luaptr  success");
	}
	else {
		ASSERT(retId == playerId, "id error");
		ERROR_LOG("luaptr: %d != %d", playerId, retId);
	}
}


int32_t	ScriptEngineTest::testLuaPtr(const PlayerInfo *playerInfo)
{
	auto outPutFun = [playerInfo](IKernel *kernel, IDataOutputStream &output)
	{
		output.WritePtr(playerInfo, "PlayerInfo");
	};
	int32_t ret;
	auto readFun = [&ret](IKernel *kernel, IDataInputStream &input)
	{
		input.ReadInt32(ret);
	};
	_scriptEngine->CallScriptFunc("test.func", "testPoint", outPutFun, readFun);
	return ret;
}

void ScriptEngineTest::getPlayerId(IKernel *kernel, IDataInputStream &input, IDataOutputStream &out)
{
	void *tmp = nullptr;
	input.ReadPtr(tmp, "PlayerInfo");
	if (tmp != nullptr)
	{
		out.WriteInt32(((const PlayerInfo*)tmp)->id);
	}
}

void ScriptEngineTest::testLoopCall()
{
	u16 num = 10;
	for (u16 i = 1; i < num; i++)
	{
		s64 tmp = FibNumber(i);
		s64 tmpLua = testFibNumber(i);
		if (tmp == tmpLua)
		{
			IMPORTANT_LOG("LUATEST", "fibnumer:%d, %ld == %ld success", i, tmp, tmpLua);
		}
		else {
			ASSERT(false, "fibnumer:%d, %ld != %ld success", i, tmp, tmpLua);
			ERROR_LOG("fibnumer:%d, %ld != %ld", i, tmp, tmpLua);
		}
	}
}

s64 ScriptEngineTest::FibNumber(u16 num)
{
	if (num <= 2)
		return 1;
	return FibNumber(num - 1) + FibNumber(num - 2);
}

s64 ScriptEngineTest::testFibNumber(s32 num)
{
	if (num <= 2)
		return 1;

	auto outPutFun = [&num](IKernel *kernel, IDataOutputStream &output)
	{
		output.WriteInt32(num);
	};
	s64 ret = 0;
	auto readFun = [&ret](IKernel *kernel, IDataInputStream &input)
	{
		input.ReadInt64(ret);
	};
	_scriptEngine->CallScriptFunc("test.func", "testFibNum", outPutFun, readFun);
	return ret;
}

void ScriptEngineTest::callFibNumer(IKernel *kernel, IDataInputStream &input, IDataOutputStream &out)
{
	s32 num = 0;
	input.ReadInt32(num);
	if (num <= 2)
	{
		out.WriteInt64(1);
	}
	else {
		out.WriteInt64(testFibNumber(num));
	}
}

void ScriptEngineTest::testLuaReload()
{
	int32_t num = 10;
	int32_t ret = callTestReload(10);
	_scriptEngine->ReloadScript("test.func");
	int32_t ret2 = callTestReload(10);
	if (ret != ret2)
	{
		IMPORTANT_LOG("LUATEST", "reload:old %d != new %d success", ret, ret2);
	}else {
		ASSERT(false, "reload:%d, %ld == %ld error", ret, ret2);
		ERROR_LOG("reload:%d, %ld == %ld error", ret, ret2);
	}
}

int32_t ScriptEngineTest::callTestReload(int32_t num)
{
	auto outPutFun = [&num](IKernel *kernel, IDataOutputStream &output)
	{
		output.WriteInt32(num);
	};
	s32 ret = 0;
	auto readFun = [&ret](IKernel *kernel, IDataInputStream &input)
	{
		input.ReadInt32(ret);
	};
	_scriptEngine->CallScriptFunc("test.func", "testReload", outPutFun, readFun);
	return ret;
}

void ScriptEngineTest::testErrorLuaFile()
{
	auto outPutFun = [](IKernel *kernel, IDataOutputStream &output)
	{
		output.WriteInt32(1);
	};
	s32 ret = 0;
	auto readFun = [&ret](IKernel *kernel, IDataInputStream &input)
	{
		ERROR_LOG("exec error func call back");
		input.ReadInt32(ret);
	};
	bool exeRet = _scriptEngine->CallScriptFunc("test.errorfunc", "testEcho", outPutFun, readFun);
	if (!exeRet)
	{
		TRACE_LOG("error func exec failed");
	}
	else {
		ASSERT(false, "error");
		ERROR_LOG("error func exec success");
	}
}

void ScriptEngineTest::testExceptionLuaFunc()
{
	auto outPutFun = [](IKernel *kernel, IDataOutputStream &output)
	{
		output.WriteInt32(1);
	};
	s32 ret = 0;
	auto readFun = [&ret](IKernel *kernel, IDataInputStream &input)
	{
		input.ReadInt32(ret);
	};
	bool exeRet = _scriptEngine->CallScriptFunc("test.exceptionfunc", "testEcho", outPutFun, readFun);
	if (!exeRet)
	{
		TRACE_LOG("exception func exec failed");
	}
	else {
		ASSERT(false, "error");
		ERROR_LOG("exception func exec success");
	}
}

void ScriptEngineTest::testInnerExceptionLuaFunc()
{
	auto outPutFun = [](IKernel *kernel, IDataOutputStream &output)
	{
		output.WriteInt32(1);
	};
	s32 ret = 0;
	auto readFun = [&ret](IKernel *kernel, IDataInputStream &input)
	{
		input.ReadInt32(ret);
	};
	bool exeRet = _scriptEngine->CallScriptFunc("test.exceptionfunc", "testCallExcptionLuaFunc", outPutFun, readFun);
	if (exeRet)
	{
		TRACE_LOG("func exec success ret:%d", ret);
	}
	else {
		ASSERT(false, "error");
		ERROR_LOG("func exec failed");
	}
}

void ScriptEngineTest::callExceptionLuaFunc(IKernel *kernel, IDataInputStream &input, IDataOutputStream &out)
{
	s32 num = 0;
	input.ReadInt32(num);
	auto outPutFun = [](IKernel *kernel, IDataOutputStream &output)
	{
		output.WriteInt32(1);
	};
	s32 ret = 0;
	auto readFun = [&ret](IKernel *kernel, IDataInputStream &input)
	{
		input.ReadInt32(ret);
	};
	bool exeRet = _scriptEngine->CallScriptFunc("test.exceptionfunc", "testEcho", outPutFun, readFun);
	if (!exeRet)
	{
		TRACE_LOG("exception func exec failed");
	}
	out.WriteInt32(ret);
}