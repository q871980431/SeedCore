/*
 * File:	UnitTest.cpp
 * Author:	XuPing
 * 
 * Created On 2019/5/16 16:19:20
 */

#include "UnitTest.h"
#include "XmlReader.h"

UnitTest * UnitTest::s_self = nullptr;
IKernel * UnitTest::s_kernel = nullptr;
GLOBAL_VAR_INT_MAX TestList UnitTest::s_testList;
GLOBAL_VAR_INT_MAX TestMap UnitTest::s_testMap;
std::vector<std::string> UnitTest::s_testModuleNames;
s32	UnitTest::s_nextId = 0;
std::set<s32> UnitTest::s_testIds;

bool UnitTest::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
	if (!LoadConfig(s_kernel))
		return false;
    
    return true;
}

bool UnitTest::Launched(IKernel *kernel)
{
	TEST_LOG("***************unit test begin********************");
	for (const auto &name : s_testModuleNames)
	{
		auto iter = s_testMap.find(name);
		if (iter != s_testMap.end())
		{
			TEST_LOG("module:%s test begin", name.c_str());
			iter->second->StartTest(kernel);
			TEST_LOG("module:%s test end", name.c_str());
		}
	}
	TEST_LOG("***************unit test end********************");

    return true;
}

bool UnitTest::Destroy(IKernel *kernel)
{
	for (auto test : s_testList)
	{
		DEL test;
	}
	s_testList.clear();
    DEL this;
    return true;
}

s32 UnitTest::GetUnitTestId()
{
	s_testIds.insert(++s_nextId);
	return s_nextId;
}

void UnitTest::CompleteTest(s32 testId)
{
	auto iter = s_testIds.find(testId);
	if (iter != s_testIds.end())
	{
		s_testIds.erase(iter);
		if (s_testIds.size() == 0)
		{
			//TO DO
		}
	}
}

bool UnitTest::LoadConfig(IKernel *kernel)
{
	const char *configPath = kernel->GetConfigFile();
	XmlReader reader;
	if (!reader.LoadFile(configPath))
	{
		ASSERT(false, "don't load config, path:%s", configPath);
		return false;
	}
	IXmlObject *unitTest = reader.Root()->GetFirstChrild("unittest");
	if (unitTest == nullptr)
	{
		ASSERT(false, "don't find chrild, name:%s", "unittest");
		return false;
	}
	IXmlObject *test = unitTest->GetFirstChrild("test");
	while (test)
	{
		const char *name = test->GetAttribute_Str("name");
		s_testModuleNames.emplace_back(name);
		test = test->GetNextSibling();
	}
	return true;
}


