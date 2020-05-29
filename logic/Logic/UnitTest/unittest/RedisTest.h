#ifndef _RedisTest_h_
#define _RedisTest_h_

#include "IUnitTest.h"
#include "IRedisMgr.h"
#include "AtomicIntrusiveLinkedList.h"
#include <thread>

struct TestNode 
{
	s32 id;
	folly::AtomicIntrusiveLinkedListHook<TestNode> hook_;
};
typedef folly::AtomicIntrusiveLinkedList<TestNode, &TestNode::hook_> TestNodeList;

class RedisTest : public IUnitTestInstance
{
public:
	virtual void StartTest(core::IKernel *kernel) override;

protected:
private:
	static IRedisMgr	*s_redisMgr;
	std::thread		s_producter;
	std::thread		s_consumer;
	TestNodeList	s_nodeList;
	bool			_close{false};
};


#endif //
