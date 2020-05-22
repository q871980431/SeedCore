#include "RedisMgr.h"
#include "hiredis.h"

REDIS_RESULT RedisMgr::Del(const std::string &strKey, s32 threadIdx)
{
	redisContext &ctx = GetRedisContext(threadIdx);
	redisReply *reply = (redisReply *)redisCommand(&ctx, "DEL %b", strKey.c_str(), strKey.length());
	if (reply == nullptr)
	{
		;
	}
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::GetTTL(const std::string &strKey, s32 &seconds, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::SetExpire(const std::string &strKey, const s32 seconds, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::Exists(const std::string &strKey, bool &exists, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::Set(const std::string &strKey, const std::string &strVal, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::SetEx(const std::string &strKey, const std::string &strVal, const s32 ttlSeconds, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::SetNx(const std::string &strKey, const std::string &strVal, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::Get(const std::string &strKey, std::string &strVal, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::GetSet(const std::string &strKey, const std::string &strVal, std::string &strOutVal, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::Incr(const std::string &strKey, s64 &iOutVal, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::Incrby(const std::string &strKey, s64 iVal, s64 &iOutVal, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::Decr(const std::string &strKey, s64 &iOutVal, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::Decrby(const std::string &strKey, s64 iVal, s64 &iOutVal, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::Append(const std::string &strKey, const std::string &strVal, s64 &iOutVal, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::MGet(const StringVector &vecFileds, StringVector &vecOutVal, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::SetStringWithOption(const std::string &strKey, const std::string &strValue, SetCmdOptionType iOption, u32 iExpireMS, bool &bSuccess, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::HSet(const std::string &strKey, const std::string &strFiled, const std::string &strVal, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::HSetNx(const std::string &strKey, const std::string &strFiled, const std::string &strVal, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::HMSet(const std::string &strKey, const StringMap &mapFiledVals, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::HGet(const std::string &strKey, const std::string &strFiled, std::string &strOutVal, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::HMGet(const std::string &strKey, const StringVector &vecFileds, StringVector &vecOutVal, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::HGetAll(const std::string &strKey, StringMap &mapOutVal, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::HDel(const std::string &strKey, const std::string &strFiled, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::HLen(const std::string &strKey, s64 &iOutVal, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::HExists(const std::string &strKey, const std::string &strFiled, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::HIncrby(const std::string &strKey, const std::string &strFiled, s64 iVal, s64 &iOutVal, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::HKeys(const std::string &strKey, StringVector &vecOutFileds, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::HVals(const std::string &strKey, StringVector &vecOutVals, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::PushListBack(const std::string &strKey, const std::string &strValue, u32 &iLen, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::PushListBackBatch(const std::string &strKey, const std::vector<std::string> &vecValue, u32 &iLen, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::GetListByRange(const std::string &strKey, s32 iStartIndex, s32 iStopIndex, std::vector<std::string> &vecValue, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::TrimList(const std::string &strKey, s32 iStartIndex, s32 iStopIndex, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::SAdd(const std::string &strKey, const std::string &member, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::SAdd(const std::string &strKey, const StringVector &vecMembers, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::SMembers(const std::string &strKey, StringVector &vecResult, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::SRem(const std::string &strKey, const std::string &strMember, s32 threadIdx)
{
	return REDIS_RESULT_OK;
}
