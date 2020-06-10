#include "RedisTest.h"
#include "../UnitTest.h"
#include "PerformanceWatch.h"
#include "Tools.h"
#include "Tools_time.h"

const char *REDSI_TEST_PREFIX = "redistest:";

UNIT_TEST_REGISTER(RedisTest);
IRedisMgr * RedisTest::s_redisMgr;
void RedisTest::StartTest(core::IKernel *kernel)
{
	FIND_MODULE(s_redisMgr, RedisMgr);
	TestSyncCall();
}


void RedisTest::TestSyncCall()
{
	StringUnitTest();
	HashUnitTest();
	ListUnitTest();
	SetUnitTest();
}

void RedisTest::StringUnitTest()
{
	std::string key1(buildTestKey("key1"));
	std::string key2(buildTestKey("key2"));
	std::string val1(buildTestKey("val1"));
	std::string val2(buildTestKey("val2"));
	bool exists = false;
	bool success = false;
	s32 ret = s_redisMgr->Exists(key1, exists);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	if (exists)
	{
		bool del = false;
		ret = s_redisMgr->Del(key1, del);
		TEST_EQ(ret, REDIS_RESULT_OK);
		TEST_EQ(del, true);
	}
	else {
		bool del = false;
		ret = s_redisMgr->Del(key1, del);
	}
	ret = s_redisMgr->Set(key1, val1);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	ret = s_redisMgr->Exists(key1, exists);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(exists, true);
	std::string tmp;
	ret = s_redisMgr->Get(key1, tmp);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(val1, tmp);
	bool del = false;
	ret = s_redisMgr->Del(key1, del);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(del, true);
	ret = s_redisMgr->Get(key1, tmp);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_NODATA);
	ret = s_redisMgr->SetStringWithOption(key1, val1, SetCmdOptionType::SetCmdOptionType_NX, 0, success);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(success, true);
	ret = s_redisMgr->SetStringWithOption(key1, val1, SetCmdOptionType::SetCmdOptionType_NX, 0, success);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(success, false);
	ret = s_redisMgr->SetStringWithOption(key1, val2, SetCmdOptionType::SetCmdOptionType_XX, 0, success);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(success, true);
	ret = s_redisMgr->Get(key1, tmp);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(val2, tmp);
	ret = s_redisMgr->SetStringWithOption(key2, val2, SetCmdOptionType::SetCmdOptionType_NX, 3 * 1000, success);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(success, true);
	ret = s_redisMgr->Get(key2, tmp);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(val2, tmp);
	Sleep(4 * 1000);
	ret = s_redisMgr->Get(key2, tmp);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_NODATA);
	s_redisMgr->Set(key1, val1);
	s64 outLen;
	ret = s_redisMgr->Append(key1, val2, outLen);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(outLen, val1.size() + val2.size());
	ret = s_redisMgr->Get(key1, tmp);
	std::string tmp2(val1);
	tmp2.append(val2);
	TEST_EQ(tmp, tmp2);
	s64 crTmp;
	ret = s_redisMgr->Incr(key1, crTmp);
	s_redisMgr->Del(key1, success);
	ret = s_redisMgr->Incr(key1, crTmp);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(1, crTmp);
	ret = s_redisMgr->Incr(key1, crTmp);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(2, crTmp);
	ret = s_redisMgr->Incrby(key1, 10, crTmp);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(12, crTmp);
	ret = s_redisMgr->Decrby(key1, 10, crTmp);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(2, crTmp);
	ret = s_redisMgr->Decr(key1,crTmp);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(1, crTmp);
	ret = s_redisMgr->Decr(key1, crTmp);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(0, crTmp);
	ret = s_redisMgr->Decr(key1, crTmp);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(-1, crTmp);
	s_redisMgr->Del(key1, del);
	TEST_EQ(del, true);
}

void RedisTest::HashUnitTest()
{
	std::string key(buildTestKey("hashtest"));
	std::string filed1(buildTestKey("filed1"));
	std::string filed2(buildTestKey("filed2"));
	std::string val1(buildTestKey("val1"));
	std::string val2(buildTestKey("val2"));
	std::string tmp;
	bool del = false;
	s32 ret = s_redisMgr->Del(key, del);
	ret = s_redisMgr->HSet(key, filed1, val1);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	ret = s_redisMgr->HGet(key, filed1, tmp);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(tmp, val1);
	ret = s_redisMgr->HGet(key, filed2, tmp);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_NODATA);
	ret = s_redisMgr->HSetNx(key, filed1, val2);
	TEST_EQ(ret != REDIS_RESULT_CODE::REDIS_RESULT_OK, true);
	ret = s_redisMgr->HGet(key, filed1, tmp);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(tmp, val1);
	ret = s_redisMgr->HSetNx(key, filed2, val2);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	ret = s_redisMgr->HGet(key, filed2, tmp);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(tmp, val2);
	ret = s_redisMgr->Del(key, del);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(del, true);
	ret = s_redisMgr->HMSet(key, { { filed1, val1 },{ filed2, val2 } });
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	ret = s_redisMgr->HGet(key, filed1, tmp);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(tmp, val1);
	ret = s_redisMgr->HGet(key, filed2, tmp);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(tmp, val2);
	StringVector vals;
	ret = s_redisMgr->HMGet(key, { filed1, filed2 }, vals);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(vals[0], val1);
	TEST_EQ(vals[1], val2);
	ret = s_redisMgr->Del(key, del);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(del, true);
	ret = s_redisMgr->HMSet(key, { { filed1, val1 }});
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	ret = s_redisMgr->HMGet(key, { filed1, filed2 }, vals);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(vals.size(), 2);
	TEST_EQ(vals[1].size(), 0);
	StringMap mapFiledVal;
	ret = s_redisMgr->HGetAll(key, mapFiledVal);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(mapFiledVal.size(), 1);
	s64 filedLen = 0;
	ret = s_redisMgr->HLen(key, filedLen);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(filedLen, 1);
	s_redisMgr->Del(key, del);
	ret = s_redisMgr->HLen(key, filedLen);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(filedLen, 0);
	ret = s_redisMgr->HMSet(key, { { filed1, val1 }});
	ret = s_redisMgr->HLen(key, filedLen);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(filedLen, 1);
	bool bExists = false;
	ret = s_redisMgr->HExists(key, filed2, bExists);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(bExists, false);
	ret = s_redisMgr->HExists(key, filed1, bExists);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(bExists, true);
	s64 iFiledVal = 0;
	ret = s_redisMgr->HIncrby(key, filed2, 10, iFiledVal);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(iFiledVal, 10);
	ret = s_redisMgr->HIncrby(key, filed2, 10, iFiledVal);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(iFiledVal, 20);
	ret = s_redisMgr->HIncrby(key, filed2, -10, iFiledVal);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(iFiledVal, 10);
	ret = s_redisMgr->HKeys(key, vals);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(vals.size(), 2);
	TEST_EQ(vals[0], filed1);
	TEST_EQ(vals[1], filed2);
}

void RedisTest::ListUnitTest()
{
	std::string key(buildTestKey("listkey"));
	StringVector vecContent;
	bool del;
	uint32_t iNum = 10;
	for (uint32_t i = 0; i < iNum; i++)
	{
		vecContent.push_back(std::to_string(i));
	}
	auto ret = s_redisMgr->Del(key, del);
	uint32_t iLen = 0;
	ret = s_redisMgr->PushListTailBatch(key, vecContent, iLen);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(iLen, iNum);

	StringVector vecRet;
	ret = s_redisMgr->GetListByRange(key, 0, iNum - 1, vecRet);
	TEST_EQ(vecContent.size(), vecRet.size());
	for (uint32_t i = 0; i < iNum; i++)
	{
		TEST_EQ(vecContent[i], vecRet[i]);
	}

	ret = s_redisMgr->TrimList(key, 1, -1);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	vecRet.clear();
	ret = s_redisMgr->GetListByRange(key, 0, iNum - 1, vecRet);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(vecRet.size(), vecContent.size() - 1);
}

void RedisTest::SetUnitTest()
{
	std::string key(buildTestKey("settest"));
	std::string member1(buildTestKey("member1"));
	std::string member2(buildTestKey("member2"));
	bool del;
	auto ret = s_redisMgr->Del(key, del);

	ret = s_redisMgr->SAdd(key, member1);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	ret = s_redisMgr->SAdd(key, member1);
	TEST_EQ(ret != REDIS_RESULT_CODE::REDIS_RESULT_OK, true);
	ret = s_redisMgr->SAdd(key, member2);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	StringVector members;
	ret = s_redisMgr->SMembers(key, members);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(members.size(), 2);
	ret = s_redisMgr->SRem(key, member1);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	ret = s_redisMgr->SRem(key, member1);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_NODATA);
	ret = s_redisMgr->SRem(key, member2);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	ret = s_redisMgr->SMembers(key, members);
	TEST_EQ(ret, REDIS_RESULT_CODE::REDIS_RESULT_OK);
	TEST_EQ(members.size(), 0);
}

std::string RedisTest::buildTestKey(const std::string &oldKey)
{
	std::string ret(REDSI_TEST_PREFIX);
	ret.append(oldKey);
	return ret;
}