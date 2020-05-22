#ifndef _RedisTest_h_
#define _RedisTest_h_

#include "IUnitTest.h"
#include "IRedisMgr.h"

class RedisTest : public IUnitTestInstance
{
public:
	virtual void StartTest(core::IKernel *kernel) override;

protected:
private:
	static IRedisMgr	*s_redisMgr;
};

#endif //
