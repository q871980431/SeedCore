#ifndef _RedisTest_h_
#define _RedisTest_h_

#include "IUnitTest.h"
#include "IRedisMgr.h"
#include "AtomicIntrusiveLinkedList.h"
#include <thread>


class RedisTest : public IUnitTestInstance
{
public:
	virtual void StartTest(core::IKernel *kernel) override;

private:
	void TestSyncCall();
	void StringUnitTest();
	void HashUnitTest();
	void ListUnitTest();
	void SetUnitTest();


private:
	std::string buildTestKey(const std::string &oldKey);
protected:
private:
	static IRedisMgr	*s_redisMgr;
};


#endif //
