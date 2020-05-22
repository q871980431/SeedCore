#include "RedisTest.h"
#include "../UnitTest.h"

IRedisMgr * RedisTest::s_redisMgr;
void RedisTest::StartTest(core::IKernel *kernel)
{
	FIND_MODULE(s_redisMgr, RedisMgr);

}
