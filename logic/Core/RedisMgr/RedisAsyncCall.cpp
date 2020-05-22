#include "RedisMgr.h"
#include "hiredis.h"


REDIS_RESULT RedisMgr::AsyncDel(const std::string &strKey)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncSet(const std::string &strKey, const std::string &strVal)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncSetEx(const std::string &strKey, const std::string &strVal, const s32 ttlSeconds)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncSetNx(const std::string &strKey, const std::string &strVal)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncIncr(const std::string &strKey, s64 &iOutVal)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncIncrby(const std::string &strKey, s64 iVal, s64 &iOutVal)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncDecr(const std::string &strKey, s64 &iOutVal)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncDecrby(const std::string &strKey, s64 iVal, s64 &iOutVal)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncHSet(const std::string &strKey, const std::string &strFiled, const std::string &strVal)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncHSetNx(const std::string &strKey, const std::string &strFiled, const std::string &strVal)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncHMSet(const std::string &strKey, const StringMap &mapFiledVals)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncPushListBack(const std::string &strKey, const std::string &strValue, u32 &iLen)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncPushListBackBatch(const std::string &strKey, const std::vector<std::string> &vecValue, u32 &iLen)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncTrimList(const std::string &strKey, s32 iStartIndex, s32 iStopIndex)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncSAdd(const std::string &strKey, const std::string &member)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncSAdd(const std::string &strKey, const StringVector &vecMembers)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncSRem(const std::string &strKey, const std::string &strMember)
{
	return REDIS_RESULT_OK;
}
