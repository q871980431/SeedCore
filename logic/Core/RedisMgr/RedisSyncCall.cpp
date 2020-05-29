#include "RedisMgr.h"
#include "hiredis.h"


REDIS_RESULT RedisMgr::Del(const std::string &strKey, bool &del, s32 threadIdx)
{
	del = false;
	RecordExecKey(threadIdx, strKey);
	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "DEL" << strKey;
	cmdBuilder.Commit();
	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		s64 val;
		if (result.GetInteger(val))
			del = val != 0;

	}
	return ret;
}

REDIS_RESULT RedisMgr::GetTTL(const std::string &strKey, s32 &seconds, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);
	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "TTL" << strKey;
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		s64 val;
		if (result.GetInteger(val))
			seconds = (s32)val;
	}

	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::SetExpire(const std::string &strKey, const s32 seconds, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);
	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "EXPIRE" << strKey;
	cmdBuilder.Commit();
	return (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, nullptr);
}

REDIS_RESULT RedisMgr::Exists(const std::string &strKey, bool &exists, s32 threadIdx)
{
	exists = false;
	RecordExecKey(threadIdx, strKey);
	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "DEL" << strKey;
	cmdBuilder.Commit();
	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		s64 val;
		if (result.GetInteger(val))
			exists = val != 0;
	}
	return ret;
}

REDIS_RESULT RedisMgr::Set(const std::string &strKey, const std::string &strVal, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);
	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "SET" << strKey << strVal;
	cmdBuilder.Commit();
	return(REDIS_RESULT)execCmd(threadIdx, cmdBuilder, nullptr);
}

REDIS_RESULT RedisMgr::SetStringWithOption(const std::string &strKey, const std::string &strValue, SetCmdOptionType iOption, u32 iExpireMS, bool &bSuccess, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);
	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "SET" << strKey;

	bSuccess = false;
	if (iExpireMS != 0)
	{
		cmdBuilder << "PX" << iExpireMS;
	}

	if (iOption == SetCmdOptionType_NX)
		cmdBuilder << "NX";

	if (iOption == SetCmdOptionType_XX)
		cmdBuilder << "EX";

	cmdBuilder.Commit();
	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		bSuccess = result.IsOK();
	}

	return ret;
}

REDIS_RESULT RedisMgr::Get(const std::string &strKey, std::string &strVal, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);
	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "GET" << strKey;
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		if (result.GetString(strVal))
			return REDIS_OK;
		return REDIS_RESULT_NODATA;
	}

	return ret;
}

REDIS_RESULT RedisMgr::GetSet(const std::string &strKey, const std::string &strVal, std::string &strOutVal, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);
	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "GETSET" << strKey;
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		if (result.GetString(strOutVal))
			return REDIS_OK;
		return REDIS_RESULT_NODATA;
	}

	return ret;
}

REDIS_RESULT RedisMgr::Incr(const std::string &strKey, s64 &iOutVal, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);
	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "INCR" << strKey;
	cmdBuilder.Commit();
	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply, false);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		if (!result.GetInteger(iOutVal))
		{
			return REDIS_ERR_OTHER;
		}
		return REDIS_OK;
	}

	return ret;
}

REDIS_RESULT RedisMgr::Incrby(const std::string &strKey, s64 iVal, s64 &iOutVal, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);
	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "INCRBY" << strKey << iVal;
	cmdBuilder.Commit();
	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply, false);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		if (!result.GetInteger(iOutVal))
		{
			return REDIS_ERR_OTHER;
		}
		return REDIS_OK;
	}

	return ret;
}

REDIS_RESULT RedisMgr::Decr(const std::string &strKey, s64 &iOutVal, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);
	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "DECR" << strKey;
	cmdBuilder.Commit();
	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply, false);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		if (!result.GetInteger(iOutVal))
		{
			return REDIS_ERR_OTHER;
		}
		return REDIS_OK;
	}

	return ret;
}

REDIS_RESULT RedisMgr::Decrby(const std::string &strKey, s64 iVal, s64 &iOutVal, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);
	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "DECRBy" << strKey << iVal;
	cmdBuilder.Commit();
	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply, false);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		if (!result.GetInteger(iOutVal))
		{
			return REDIS_ERR_OTHER;
		}
		return REDIS_OK;
	}

	return ret;
}

REDIS_RESULT RedisMgr::Append(const std::string &strKey, const std::string &strVal, s64 &iOutVal, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);
	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "APPEND" << strKey << strVal;
	cmdBuilder.Commit();
	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply, false);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		if (!result.GetInteger(iOutVal))
		{
			return REDIS_ERR_OTHER;
		}
		return REDIS_OK;
	}

	return ret;
}

REDIS_RESULT RedisMgr::MGet(const StringVector &vecFileds, StringVector &vecOutVal, s32 threadIdx)
{
	for (const auto &iter : vecFileds)
		RecordExecKey(threadIdx, iter);

	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "MGET";
	for (const auto &iter : vecFileds)
		cmdBuilder << iter;

	cmdBuilder.Commit();
	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		return result.ParseStringArray(vecOutVal) ? REDIS_OK : REDIS_ERR_OTHER;
	}

	return ret;
}

REDIS_RESULT RedisMgr::MSet(const StringMap &mapFileds, s32 threadIdx)
{
	for (const auto &iter : mapFileds)
		RecordExecKey(threadIdx, iter.first);

	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "MSET";
	for (const auto &iter : mapFileds)
		cmdBuilder << iter.first << iter.second;

	cmdBuilder.Commit();
	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		return result.IsOK() ? REDIS_OK : REDIS_ERR_OTHER;
	}

	return ret;
}



REDIS_RESULT RedisMgr::HSet(const std::string &strKey, const std::string &strFiled, const std::string &strVal, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);
	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "HSET" << strKey << strFiled << strVal;
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		s64 val = 0;
		if (result.GetInteger(val))
			return REDIS_OK;

		return REDIS_ERR_OTHER;
	}

	return ret;
}

REDIS_RESULT RedisMgr::HSetNx(const std::string &strKey, const std::string &strFiled, const std::string &strVal, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);
	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "HSETNX" << strKey << strFiled << strVal;
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		s64 val = 0;
		result.GetInteger(val);
		return val == 1 ? REDIS_OK : REDIS_ERR_OTHER;
	}

	return ret;
}

REDIS_RESULT RedisMgr::HMSet(const std::string &strKey, const StringMap &mapFiledVals, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);

	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "HMSET" << strKey;
	for (const auto &iter : mapFiledVals)
		cmdBuilder << iter.first << iter.second;
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		return result.IsOK() ? REDIS_OK : REDIS_ERR_OTHER;
	}

	return ret;
}

REDIS_RESULT RedisMgr::HGet(const std::string &strKey, const std::string &strFiled, std::string &strOutVal, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);

	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "HGET" << strKey << strFiled;
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		if (result.IsNil())
			return REDIS_RESULT_NODATA;
		result.GetString(strOutVal);
		return REDIS_OK;
	}

	return ret;
}

REDIS_RESULT RedisMgr::HMGet(const std::string &strKey, const StringVector &vecFileds, StringVector &vecOutVal, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);

	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "HMGET" << strKey;
	for (const auto &iter : vecFileds)
		cmdBuilder << iter;
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		return result.ParseStringArray(vecOutVal) ? REDIS_OK : REDIS_ERR_OTHER;
	}

	return ret;
}

REDIS_RESULT RedisMgr::HGetAll(const std::string &strKey, StringMap &mapOutVal, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);

	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "HGETALL" << strKey;
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		if (result.IsNil())
			return REDIS_RESULT_NODATA;

		return result.ParseStringMap(mapOutVal) ? REDIS_OK : REDIS_ERR_OTHER;
	}

	return ret;
}

REDIS_RESULT RedisMgr::HDel(const std::string &strKey, const std::string &strFiled, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);

	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "HDEL" << strKey << strFiled;
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		s64 count = 0;
		result.GetInteger(count);
		return count == 1 ? REDIS_OK : REDIS_ERR_OTHER;
	}

	return ret;
}

REDIS_RESULT RedisMgr::HDel(const std::string &strKey, const StringVector &strFileds, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);
	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "HDEL" << strKey;
	for (const auto &iter : strFileds)
	{
		cmdBuilder << iter;
	}
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		s64 count = 0;
		result.GetInteger(count);
		return count > 0 ? REDIS_OK : REDIS_ERR_OTHER;
	}

	return ret;
}
REDIS_RESULT RedisMgr::HLen(const std::string &strKey, s64 &iOutVal, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);
	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "HLEN" << strKey;
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		if (result.GetInteger(iOutVal))
			return REDIS_OK;

		return REDIS_ERR_OTHER;
	}

	return ret;
}

REDIS_RESULT RedisMgr::HExists(const std::string &strKey, const std::string &strFiled, bool &exists, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);
	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "HEXISTS" << strKey << strFiled;
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		s64 val = 0;
		if (result.GetInteger(val))
		{
			exists = val == 1 ? true : false;
			return REDIS_OK;
		}

		return REDIS_ERR_OTHER;
	}

	return ret;
}

REDIS_RESULT RedisMgr::HIncrby(const std::string &strKey, const std::string &strFiled, s64 iVal, s64 &iOutVal, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);
	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "HINCRBY" << strKey << strFiled << iVal;
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		if (result.GetInteger(iOutVal))
			return REDIS_OK;

		return REDIS_ERR_OTHER;
	}

	return ret;
}

REDIS_RESULT RedisMgr::HKeys(const std::string &strKey, StringVector &vecOutFileds, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);
	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "HKEYS" << strKey;
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		if (result.ParseStringArray(vecOutFileds))
			return REDIS_OK;

		return REDIS_ERR_OTHER;
	}

	return ret;
}

REDIS_RESULT RedisMgr::HVals(const std::string &strKey, StringVector &vecOutVals, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);
	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "HVALS" << strKey;
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		if (result.ParseStringArray(vecOutVals))
			return REDIS_OK;

		return REDIS_ERR_OTHER;
	}

	return ret;
}

REDIS_RESULT RedisMgr::PushListTail(const std::string &strKey, const std::string &strValue, u32 &iLen, s32 threadIdx)
{
	return RPush(strKey, strValue, iLen, threadIdx);
}

REDIS_RESULT RedisMgr::PushListTailBatch(const std::string &strKey, const std::vector<std::string> &vecValue, u32 &iLen, s32 threadIdx)
{
	return RPush(strKey, vecValue, iLen, threadIdx);
}

REDIS_RESULT RedisMgr::GetListByRange(const std::string &strKey, s32 iStartIndex, s32 iStopIndex, std::vector<std::string> &vecValue, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);

	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "LRANGE" << strKey << iStartIndex << iStopIndex;
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		
		return result.ParseStringArray(vecValue) ? REDIS_OK : REDIS_ERR_OTHER;
	}

	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::TrimList(const std::string &strKey, s32 iStartIndex, s32 iStopIndex, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);

	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "LPUSH" << strKey << iStartIndex << iStopIndex;
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		return result.IsOK() ? REDIS_OK : REDIS_ERR_OTHER;
	}

	return ret;
}

REDIS_RESULT RedisMgr::LPush(const std::string &strKey, const std::string &strValue, u32 &iLen, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);

	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "LPUSH" << strKey << strValue;
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		s64 count = 0;
		if (result.GetInteger(count))
		{
			iLen = (u32)count;
			return REDIS_OK;
		}
		return REDIS_ERR_OTHER;
	}

	return ret;
}

REDIS_RESULT RedisMgr::LPush(const std::string &strKey, const StringVector &vecValue, u32 &iLen, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);

	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "LPUSH" << strKey;
	for (const auto &iter : vecValue)
	{
		cmdBuilder << iter;
	}
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		s64 count = 0;
		if (result.GetInteger(count))
		{
			iLen = (u32)count;
			return REDIS_OK;
		}
		return REDIS_ERR_OTHER;
	}

	return ret;
}

REDIS_RESULT RedisMgr::LPushX(const std::string &strKey, const std::string &strValue, u32 &iLen, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);

	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "LPUSHX" << strKey << strValue;
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		s64 count = 0;
		if (result.GetInteger(count))
		{
			iLen = (u32)count;
			return REDIS_OK;
		}
		return REDIS_ERR_OTHER;
	}

	return ret;
}

REDIS_RESULT RedisMgr::LPushX(const std::string &strKey, const StringVector &vecValue, u32 &iLen, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);

	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "LPUSHX" << strKey;
	for (const auto &iter : vecValue)
	{
		cmdBuilder << iter;
	}
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		s64 count = 0;
		if (result.GetInteger(count))
		{
			iLen = (u32)count;
			return REDIS_OK;
		}
		return REDIS_ERR_OTHER;
	}

	return ret;
}

REDIS_RESULT RedisMgr::RPush(const std::string &strKey, const std::string &strValue, u32 &iLen, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);

	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "RPUSH" << strKey << strValue;
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		s64 count = 0;
		if (result.GetInteger(count))
		{
			iLen = (u32)count;
			return REDIS_OK;
		}
		return REDIS_ERR_OTHER;
	}

	return ret;
}

REDIS_RESULT RedisMgr::RPush(const std::string &strKey, const StringVector &vecValue, u32 &iLen, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);

	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "RPUSH" << strKey;
	for (const auto &iter : vecValue)
	{
		cmdBuilder << iter;
	}
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		s64 count = 0;
		if (result.GetInteger(count))
		{
			iLen = (u32)count;
			return REDIS_OK;
		}
		return REDIS_ERR_OTHER;
	}

	return ret;
}

REDIS_RESULT RedisMgr::RPushX(const std::string &strKey, const std::string &strValue, u32 &iLen, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);

	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "RPUSHX" << strKey << strValue;
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		s64 count = 0;
		if (result.GetInteger(count))
		{
			iLen = (u32)count;
			return REDIS_OK;
		}
		return REDIS_ERR_OTHER;
	}

	return ret;
}

REDIS_RESULT RedisMgr::RPushX(const std::string &strKey, const StringVector &vecValue, u32 &iLen, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);

	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "LPUSHX" << strKey;
	for (const auto &iter : vecValue)
	{
		cmdBuilder << iter;
	}
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		s64 count = 0;
		if (result.GetInteger(count))
		{
			iLen = (u32)count;
			return REDIS_OK;
		}
		return REDIS_ERR_OTHER;
	}

	return ret;
}


REDIS_RESULT RedisMgr::SAdd(const std::string &strKey, const std::string &member, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);

	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "SADD" << strKey << member;
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		return REDIS_OK;
	}

	return ret;
}

REDIS_RESULT RedisMgr::SAdd(const std::string &strKey, const StringVector &vecMembers, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);

	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "SADD" << strKey;
	for (const auto &iter : vecMembers)
	{
		cmdBuilder << iter;
	}
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		return REDIS_OK;
	}

	return ret;
}

REDIS_RESULT RedisMgr::SMembers(const std::string &strKey, StringVector &vecResult, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);

	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "SMEMBERS" << strKey;
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		return result.ParseStringArray(vecResult) ?  REDIS_OK : REDIS_ERR_OTHER;
	}

	return ret;
}

REDIS_RESULT RedisMgr::SRem(const std::string &strKey, const std::string &strMember, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);

	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "SREM" << strKey << strMember;
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		s64 count = 0;
		if (result.GetInteger(count))
			return count == 0 ? REDIS_RESULT_NODATA : REDIS_OK;

		return REDIS_ERR_OTHER;
	}

	return ret;
}

REDIS_RESULT RedisMgr::SRem(const std::string &strKey, const StringVector &strMembers, s32 threadIdx)
{
	RecordExecKey(threadIdx, strKey);

	RedisCmdBuilder cmdBuilder;
	cmdBuilder << "SREM" << strKey;
	for (const auto &iter : strMembers)
	{
		cmdBuilder << iter;
	}
	cmdBuilder.Commit();

	redisReply *reply = nullptr;
	auto ret = (REDIS_RESULT)execCmd(threadIdx, cmdBuilder, &reply);
	if (reply != nullptr)
	{
		RedisResult result(reply);
		s64 count = 0;
		return result.GetInteger(count) ? REDIS_OK: REDIS_ERR_OTHER;
	}

	return ret;
}
