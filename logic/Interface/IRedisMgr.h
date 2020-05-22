/*
 * File:	IRedisMgr.h
 * Author:	xuping
 * 
 * Created On 2020/4/7 18:09:35
 */

#ifndef  __IRedisMgr_h__
#define __IRedisMgr_h__
#include "IModule.h"
#include <map>
#include <vector>

enum REDIS_RESULT
{
	REDIS_RESULT_NODATA = -1,
	REDIS_RESULT_OK = 0,
	REDIS_RESULT_TIMEOUT = 1
};

enum SetCmdOptionType
{
	SetCmdOptionType_None,
	SetCmdOptionType_XX,
	SetCmdOptionType_NX,
};

typedef std::map<std::string, std::string> StringMap;
typedef std::vector<std::string> StringVector;

//threadIdx 为【0，max(s32)) 其中0 为主线程id
//可用于主线程和后台线程中使用
class IRedisSyncCall
{
	//Key
public:
	virtual REDIS_RESULT Del(const std::string &strKey, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT GetTTL(const std::string &strKey, s32 &seconds, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT SetExpire(const std::string &strKey, const s32 seconds, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT Exists(const std::string &strKey, bool &exists, s32 threadIdx = MAIN_THREAD_IDX) = 0;

	//String
public:
	virtual REDIS_RESULT Set(const std::string &strKey, const std::string &strVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT SetEx(const std::string &strKey, const std::string &strVal, const s32 ttlSeconds, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT SetNx(const std::string &strKey, const std::string &strVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT Get(const std::string &strKey, std::string &strVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT GetSet(const std::string &strKey, const std::string &strVal, std::string &strOutVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;	//Abase暂不支持
	virtual REDIS_RESULT Incr(const std::string &strKey, s64 &iOutVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT Incrby(const std::string &strKey, s64 iVal, s64 &iOutVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT Decr(const std::string &strKey, s64 &iOutVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT Decrby(const std::string &strKey, s64 iVal, s64 &iOutVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT Append(const std::string &strKey, const std::string &strVal, s64 &iOutVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT MGet(const StringVector &vecFileds, StringVector &vecOutVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT SetStringWithOption(const std::string &strKey, const std::string &strValue, SetCmdOptionType iOption, u32 iExpireMS, bool &bSuccess, s32 threadIdx = MAIN_THREAD_IDX) = 0;

	//Hash
public:
	virtual REDIS_RESULT HSet(const std::string &strKey, const std::string &strFiled, const std::string &strVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT HSetNx(const std::string &strKey, const std::string &strFiled, const std::string &strVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT HMSet(const std::string &strKey, const StringMap &mapFiledVals, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT HGet(const std::string &strKey, const std::string &strFiled, std::string &strOutVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT HMGet(const std::string &strKey, const StringVector &vecFileds, StringVector &vecOutVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT HGetAll(const std::string &strKey, StringMap &mapOutVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT HDel(const std::string &strKey, const std::string &strFiled, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT HLen(const std::string &strKey, s64 &iOutVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT HExists(const std::string &strKey, const std::string &strFiled, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT HIncrby(const std::string &strKey, const std::string &strFiled, s64 iVal, s64 &iOutVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT HKeys(const std::string &strKey, StringVector &vecOutFileds, s32 threadIdx = MAIN_THREAD_IDX) = 0;	//Abase 暂不支持
	virtual REDIS_RESULT HVals(const std::string &strKey, StringVector &vecOutVals, s32 threadIdx = MAIN_THREAD_IDX) = 0;	//Abase 暂不支持

	//List
public:
	virtual REDIS_RESULT PushListBack(const std::string &strKey, const std::string &strValue, u32 &iLen, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT PushListBackBatch(const std::string &strKey, const std::vector<std::string> &vecValue, u32 &iLen, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT GetListByRange(const std::string &strKey, s32 iStartIndex, s32 iStopIndex, std::vector<std::string> &vecValue, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT TrimList(const std::string &strKey, s32 iStartIndex, s32 iStopIndex, s32 threadIdx = MAIN_THREAD_IDX) = 0;


	//Set
public:
	virtual REDIS_RESULT SAdd(const std::string &strKey, const std::string &member, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT SAdd(const std::string &strKey, const StringVector &vecMembers, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT SMembers(const std::string &strKey, StringVector &vecResult, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT SRem(const std::string &strKey, const std::string &strMember, s32 threadIdx = MAIN_THREAD_IDX) = 0;
};

//用于主线程中使用
class IRedisAsyncCall
{
public:
	//Key
public:
	virtual REDIS_RESULT AsyncDel(const std::string &strKey) = 0;

	//String
public:
	virtual REDIS_RESULT AsyncSet(const std::string &strKey, const std::string &strVal) = 0;
	virtual REDIS_RESULT AsyncSetEx(const std::string &strKey, const std::string &strVal, const s32 ttlSeconds) = 0;
	virtual REDIS_RESULT AsyncSetNx(const std::string &strKey, const std::string &strVal) = 0;
	virtual REDIS_RESULT AsyncIncr(const std::string &strKey, s64 &iOutVal) = 0;
	virtual REDIS_RESULT AsyncIncrby(const std::string &strKey, s64 iVal, s64 &iOutVal) = 0;
	virtual REDIS_RESULT AsyncDecr(const std::string &strKey, s64 &iOutVal) = 0;
	virtual REDIS_RESULT AsyncDecrby(const std::string &strKey, s64 iVal, s64 &iOutVal) = 0;

	//Hash
public:
	virtual REDIS_RESULT AsyncHSet(const std::string &strKey, const std::string &strFiled, const std::string &strVal) = 0;
	virtual REDIS_RESULT AsyncHSetNx(const std::string &strKey, const std::string &strFiled, const std::string &strVal) = 0;
	virtual REDIS_RESULT AsyncHMSet(const std::string &strKey, const StringMap &mapFiledVals) = 0;

	//List
public:
	virtual REDIS_RESULT AsyncPushListBack(const std::string &strKey, const std::string &strValue, u32 &iLen) = 0;
	virtual REDIS_RESULT AsyncPushListBackBatch(const std::string &strKey, const std::vector<std::string> &vecValue, u32 &iLen) = 0;
	virtual REDIS_RESULT AsyncTrimList(const std::string &strKey, s32 iStartIndex, s32 iStopIndex) = 0;

	//Set
public:
	virtual REDIS_RESULT AsyncSAdd(const std::string &strKey, const std::string &member) = 0;
	virtual REDIS_RESULT AsyncSAdd(const std::string &strKey, const StringVector &vecMembers) = 0;
	virtual REDIS_RESULT AsyncSRem(const std::string &strKey, const std::string &strMember) = 0;
};

class IRedisMgr : public IModule, public IRedisSyncCall, public IRedisAsyncCall
{
public:
    virtual ~IRedisMgr(){};
	virtual s32 GetAsyncConnectNum() = 0;	//线程threadIdx = random(0, GetAsyncConnectNum()) + 1 即可
};
#endif