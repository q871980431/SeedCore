#include "RedisTest.h"
#include "../UnitTest.h"
#include "PerformanceWatch.h"
#include "Tools.h"
#include "Tools_time.h"

UNIT_TEST_REGISTER(RedisTest);
IRedisMgr * RedisTest::s_redisMgr;
void RedisTest::StartTest(core::IKernel *kernel)
{
	FIND_MODULE(s_redisMgr, RedisMgr);
	PerformanceWatch<tools::StopWatch<>> watch;
	PerformanceWatch<tools::RtsWatch> rtsWatch;
	for (int32_t i = 0; i < 10000; i++)
	{
		int32_t time = tools::GetRandom(1, 10);
		watch.Start();
		rtsWatch.Start();
		for (int32_t j = 0; j < time; j++)
		{
			int32_t size = tools::GetRandom(10 * 1024, 100 * 1024);
			int32_t *pBuff = new int32_t[size];
			delete[] pBuff;
		}
		watch.Record();
		rtsWatch.Record();
	}
	std::string strContent = watch.CreatePerformanceInfo();
	std::string rtsContent = rtsWatch.CreatePerformanceInfo();
	IMPORTANT_LOG("Performance", "ms %s", strContent.c_str());
	IMPORTANT_LOG("Performance", "rts %s", rtsContent.c_str());
	s_producter = std::thread([this]() {
		for (size_t i = 0; i < 1000000; i++)
		{
			TestNode *testNode = NEW TestNode();
			testNode->id = i;
			s_nodeList.insertHead(testNode);
		}
	});
	s_consumer = std::thread([this]() {
		TestNode *head = nullptr;
		while (!_close || head != nullptr)
		{
			if (head == nullptr)
				head = s_nodeList.cutList();

			int32_t i = 0;
			while (head != nullptr && i < 1000)
			{
				auto t = head;
				head = head->hook_.next;
				THREAD_LOG("CONSUMER", "test node id:%d", t->id);
				DEL t;
				i++;
			}
			//s_nodeList.sweep([](TestNode *testNode) {
			//	THREAD_LOG("CONSUMER", "test node id:%d", testNode->id);
			//	DEL testNode;
			//});
			Sleep(10);
		}
	});
	s_producter.join();
	Sleep(200);
	_close = true;
	s_consumer.join();
}
