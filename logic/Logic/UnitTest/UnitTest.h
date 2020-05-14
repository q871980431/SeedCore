/*
 * File:	UnitTest.h
 * Author:	XuPing
 * 
 * Created On 2019/5/16 16:19:20
 */

#ifndef __UnitTest_h__
#define __UnitTest_h__
#include "IUnitTest.h"
#include <vector>
#include <map>

typedef std::vector<IUnitTestInstance *> TestList;
typedef std::map<std::string, IUnitTestInstance *> TestMap;
class UnitTest : public IUnitTest
{
public:
    virtual ~UnitTest(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);
	virtual s32 GetUnitTestId();
	virtual void CompleteTest(s32 testId);

	static TestList &GetTestList() { return s_testList; };
	static TestMap  &GetTestMap() { return s_testMap; };
private:
	bool LoadConfig(IKernel *kernel);

protected:
private:
    static UnitTest     * s_self;
    static IKernel  * s_kernel;
	static TestList		s_testList;
	static TestMap		s_testMap;
	static std::vector<std::string> s_testModuleNames;
	static s32			s_nextId;
	static std::set<s32>	s_testIds;
};

template<typename T>
struct UnitTestRegister
{
	UnitTestRegister()
	{
		//printf("this UnitTest Register 1\n");
		TestList &list = UnitTest::GetTestList();
		list.push_back(NEW T());
		//printf("list size:%ld", list.size());
	}

	UnitTestRegister(const std::string &testName)
	{
		//printf("this UnitTest Register %s\n", testName.c_str());
		TestMap &testMap = UnitTest::GetTestMap();
		testMap.insert(std::make_pair(testName, NEW T()));
		//printf("list size:%ld", testMap.size());
	}
};

#define UNIT_TEST_REGISTER(T) UnitTestRegister<T> unitTest##T(#T)


#endif
