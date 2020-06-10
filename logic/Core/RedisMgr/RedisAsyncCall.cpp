#include "RedisMgr.h"
#include "hiredis.h"

REDIS_RESULT RedisMgr::AsyncDel(s64 taskId, const std::string &strKey)
{
	RedisCmdBuilder *cmdBuilder = NEW RedisCmdBuilder();
	(*cmdBuilder) << "DEL" << strKey;
	cmdBuilder->Commit();

	addAsyncTask(taskId, cmdBuilder);
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncSet(s64 taskId, const std::string &strKey, const std::string &strVal)
{
	RedisCmdBuilder *cmdBuilder = NEW RedisCmdBuilder();
	(*cmdBuilder) << "SET" << strKey << strVal;
	cmdBuilder->Commit();

	addAsyncTask(taskId, cmdBuilder);
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncSetStringWithOption(s64 taskId, const std::string &strKey, const std::string &strValue, SetCmdOptionType iOption, u32 iExpireMS)
{
	RedisCmdBuilder *cmdBuilderPtr = NEW RedisCmdBuilder();
	RedisCmdBuilder &cmdBuilder = *cmdBuilderPtr;
	cmdBuilder << "SET" << strKey;

	if (iExpireMS != 0)
	{
		cmdBuilder << "PX" << iExpireMS;
	}

	if (iOption == SetCmdOptionType_NX)
		cmdBuilder << "NX";

	if (iOption == SetCmdOptionType_XX)
		cmdBuilder << "EX";

	cmdBuilder.Commit();
	addAsyncTask(taskId, cmdBuilderPtr);

	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncIncr(s64 taskId, const std::string &strKey, s64 &iOutVal)
{
	RedisCmdBuilder *cmdBuilder = NEW RedisCmdBuilder();
	(*cmdBuilder) << "INCR" << strKey;
	cmdBuilder->Commit();

	addAsyncTask(taskId, cmdBuilder);
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncIncrby(s64 taskId, const std::string &strKey, s64 iVal, s64 &iOutVal)
{
	RedisCmdBuilder *cmdBuilder = NEW RedisCmdBuilder();
	(*cmdBuilder) << "INCRBY" << strKey << iVal;
	cmdBuilder->Commit();

	addAsyncTask(taskId, cmdBuilder);
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncDecr(s64 taskId, const std::string &strKey, s64 &iOutVal)
{
	RedisCmdBuilder *cmdBuilder = NEW RedisCmdBuilder();
	(*cmdBuilder) << "DECR" << strKey;
	cmdBuilder->Commit();

	addAsyncTask(taskId, cmdBuilder);
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncDecrby(s64 taskId, const std::string &strKey, s64 iVal, s64 &iOutVal)
{
	RedisCmdBuilder *cmdBuilder = NEW RedisCmdBuilder();
	(*cmdBuilder) << "DECRBy" << strKey << iVal;
	cmdBuilder->Commit();

	addAsyncTask(taskId, cmdBuilder);
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncHSet(s64 taskId, const std::string &strKey, const std::string &strFiled, const std::string &strVal)
{
	RedisCmdBuilder *cmdBuilder = NEW RedisCmdBuilder();
	(*cmdBuilder) << "HSET" << strKey << strFiled << strVal;
	cmdBuilder->Commit();

	addAsyncTask(taskId, cmdBuilder);
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncHSetNx(s64 taskId, const std::string &strKey, const std::string &strFiled, const std::string &strVal)
{
	RedisCmdBuilder *cmdBuilder = NEW RedisCmdBuilder();
	(*cmdBuilder) << "HSETNX" << strKey << strFiled << strVal;
	cmdBuilder->Commit();

	addAsyncTask(taskId, cmdBuilder);
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncHMSet(s64 taskId, const std::string &strKey, const StringMap &mapFiledVals)
{
	RedisCmdBuilder *cmdBuilder = NEW RedisCmdBuilder();
	(*cmdBuilder) << "HMSET" << strKey;
	for (const auto &iter : mapFiledVals)
		(*cmdBuilder) << iter.first << iter.second;
	cmdBuilder->Commit();

	addAsyncTask(taskId, cmdBuilder);
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncPushListBack(s64 taskId, const std::string &strKey, const std::string &strValue, u32 &iLen)
{
	RedisCmdBuilder *cmdBuilder = NEW RedisCmdBuilder();
	(*cmdBuilder) << "RPUSH" << strKey << strValue;
	cmdBuilder->Commit();

	addAsyncTask(taskId, cmdBuilder);
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncPushListBackBatch(s64 taskId, const std::string &strKey, const std::vector<std::string> &vecValue, u32 &iLen)
{
	RedisCmdBuilder *cmdBuilder = NEW RedisCmdBuilder();
	(*cmdBuilder) << "RPUSH" << strKey;
	for (const auto &iter : vecValue)
	{
		(*cmdBuilder) << iter;
	}
	cmdBuilder->Commit();

	addAsyncTask(taskId, cmdBuilder);
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncTrimList(s64 taskId, const std::string &strKey, s32 iStartIndex, s32 iStopIndex)
{
	RedisCmdBuilder *cmdBuilder = NEW RedisCmdBuilder();
	*cmdBuilder << "LPUSH" << strKey << iStartIndex << iStopIndex;
	cmdBuilder->Commit();
	
	addAsyncTask(taskId, cmdBuilder);
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncSAdd(s64 taskId, const std::string &strKey, const std::string &member)
{
	RedisCmdBuilder *cmdBuilder = NEW RedisCmdBuilder();
	(*cmdBuilder) << "SADD" << strKey << member;
	cmdBuilder->Commit();

	addAsyncTask(taskId, cmdBuilder);
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncSAdd(s64 taskId, const std::string &strKey, const StringVector &vecMembers)
{
	RedisCmdBuilder *cmdBuilder = NEW RedisCmdBuilder();
	(*cmdBuilder) << "SADD" << strKey;
	for (const auto &iter : vecMembers)
	{
		(*cmdBuilder) << iter;
	}
	cmdBuilder->Commit();

	addAsyncTask(taskId, cmdBuilder);
	return REDIS_RESULT_OK;
}

REDIS_RESULT RedisMgr::AsyncSRem(s64 taskId, const std::string &strKey, const std::string &strMember)
{
	RedisCmdBuilder *cmdBuilder = NEW RedisCmdBuilder();
	*cmdBuilder << "SREM" << strKey << strMember;
	cmdBuilder->Commit();

	addAsyncTask(taskId, cmdBuilder);
	return REDIS_RESULT_OK;
}