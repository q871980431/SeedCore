#include "RedisMgr.h"
#include "hiredis.h"


REDIS_RESULT RedisMgr::AsyncDel(s64 taskId, const std::string &strKey)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncSet(s64 taskId, const std::string &strKey, const std::string &strVal)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncSetEx(s64 taskId, const std::string &strKey, const std::string &strVal, const s32 ttlSeconds)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncSetNx(s64 taskId, const std::string &strKey, const std::string &strVal)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncIncr(s64 taskId, const std::string &strKey, s64 &iOutVal)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncIncrby(s64 taskId, const std::string &strKey, s64 iVal, s64 &iOutVal)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncDecr(s64 taskId, const std::string &strKey, s64 &iOutVal)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncDecrby(s64 taskId, const std::string &strKey, s64 iVal, s64 &iOutVal)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncHSet(s64 taskId, const std::string &strKey, const std::string &strFiled, const std::string &strVal)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncHSetNx(s64 taskId, const std::string &strKey, const std::string &strFiled, const std::string &strVal)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncHMSet(s64 taskId, const std::string &strKey, const StringMap &mapFiledVals)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncPushListBack(s64 taskId, const std::string &strKey, const std::string &strValue, u32 &iLen)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncPushListBackBatch(s64 taskId, const std::string &strKey, const std::vector<std::string> &vecValue, u32 &iLen)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncTrimList(s64 taskId, const std::string &strKey, s32 iStartIndex, s32 iStopIndex)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncSAdd(s64 taskId, const std::string &strKey, const std::string &member)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncSAdd(s64 taskId, const std::string &strKey, const StringVector &vecMembers)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncSRem(s64 taskId, const std::string &strKey, const std::string &strMember)
{
	return REDIS_RESULT_OK;
}
