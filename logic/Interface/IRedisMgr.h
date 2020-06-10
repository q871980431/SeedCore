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

//copy form redis read.h
//#define REDIS_ERR_IO 1 /* Error in read or write */
//#define REDIS_ERR_EOF 3 /* End of file */
//#define REDIS_ERR_PROTOCOL 4 /* Protocol error */
//#define REDIS_ERR_OOM 5 /* Out of memory */
//#define REDIS_ERR_TIMEOUT 6 /* Timed out */
//#define REDIS_ERR_OTHER 2 /* Everything else... */


enum REDIS_RESULT_CODE
{
	REDIS_RESULT_NODATA = -999,
	REDIS_RESULT_TIMEOUT = -1,
	REDIS_RESULT_OK = 0,
};

typedef s32 REDIS_RESULT;
enum SetCmdOptionType
{
	SetCmdOptionType_None,
	SetCmdOptionType_XX,
	SetCmdOptionType_NX,
};

typedef std::map<std::string, std::string> StringMap;
typedef std::vector<std::string> StringVector;

/*对Sync接口所有命令会支持自动重发, 即如果和远端连接断开, 导致的错误.在一定时间范围内模块会主动重连远端, 
并重发该命令。不自动重发的命令会标注出来, 主要时非覆盖性指令。
不自动重发指令:
Incr
Incrby
Decr
Decrby
Append

*/
//threadIdx 为【0，max(s32)) 其中0 为主线程id
//可用于主线程和后台线程中使用，后台线程中使用，需要和该模块属于同一AsyncQueue中, 否则由于内部自身
//不采用锁的方式, 会导致线程安全相关的问题。
class IRedisSyncCall
{
	//Key
public:
	virtual REDIS_RESULT Del(const std::string &strKey, bool &del, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT GetTTL(const std::string &strKey, s32 &seconds, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT SetExpire(const std::string &strKey, const s32 seconds, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT Exists(const std::string &strKey, bool &exists, s32 threadIdx = MAIN_THREAD_IDX) = 0;

	//String
public:
	virtual REDIS_RESULT Set(const std::string &strKey, const std::string &strVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT SetStringWithOption(const std::string &strKey, const std::string &strValue, SetCmdOptionType iOption, u32 iExpireMS, bool &bSuccess, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT Get(const std::string &strKey, std::string &strVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT GetSet(const std::string &strKey, const std::string &strVal, std::string &strOutVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;	//Abase暂不支持
	virtual REDIS_RESULT Incr(const std::string &strKey, s64 &iOutVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT Incrby(const std::string &strKey, s64 iVal, s64 &iOutVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT Decr(const std::string &strKey, s64 &iOutVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT Decrby(const std::string &strKey, s64 iVal, s64 &iOutVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT Append(const std::string &strKey, const std::string &strVal, s64 &iOutVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT MGet(const StringVector &vecFileds, StringVector &vecOutVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT MSet(const StringMap &mapFileds, s32 threadIdx = MAIN_THREAD_IDX) = 0;

	//Hash
public:
	virtual REDIS_RESULT HSet(const std::string &strKey, const std::string &strFiled, const std::string &strVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT HSetNx(const std::string &strKey, const std::string &strFiled, const std::string &strVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT HMSet(const std::string &strKey, const StringMap &mapFiledVals, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT HGet(const std::string &strKey, const std::string &strFiled, std::string &strOutVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT HMGet(const std::string &strKey, const StringVector &vecFileds, StringVector &vecOutVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT HGetAll(const std::string &strKey, StringMap &mapOutVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT HDel(const std::string &strKey, const std::string &strFiled, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT HDel(const std::string &strKey, const StringVector &strFileds, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT HLen(const std::string &strKey, s64 &iOutVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT HExists(const std::string &strKey, const std::string &strFiled, bool &exists, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT HIncrby(const std::string &strKey, const std::string &strFiled, s64 iVal, s64 &iOutVal, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT HKeys(const std::string &strKey, StringVector &vecOutFileds, s32 threadIdx = MAIN_THREAD_IDX) = 0;	
	virtual REDIS_RESULT HVals(const std::string &strKey, StringVector &vecOutVals, s32 threadIdx = MAIN_THREAD_IDX) = 0;	

	//List
public:
	virtual REDIS_RESULT PushListTail(const std::string &strKey, const std::string &strValue, u32 &iLen, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT PushListTailBatch(const std::string &strKey, const std::vector<std::string> &vecValue, u32 &iLen, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT GetListByRange(const std::string &strKey, s32 iStartIndex, s32 iStopIndex, std::vector<std::string> &vecValue, s32 threadIdx = MAIN_THREAD_IDX) = 0;//	//istarIndex 从0 开始计数[iStartIndex, iStopIndex]
	virtual REDIS_RESULT TrimList(const std::string &strKey, s32 iStartIndex, s32 iStopIndex, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT LPush(const std::string &strKey, const std::string &strValue, u32 &iLen, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT LPush(const std::string &strKey, const StringVector &vecValue, u32 &iLen, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT LPushX(const std::string &strKey, const std::string &strValue, u32 &iLen, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT LPushX(const std::string &strKey, const StringVector &vecValue, u32 &iLen, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT RPush(const std::string &strKey, const std::string &strValue, u32 &iLen, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT RPush(const std::string &strKey, const StringVector &vecValue, u32 &iLen, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT RPushX(const std::string &strKey, const std::string &strValue, u32 &iLen, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT RPushX(const std::string &strKey, const StringVector &vecValue, u32 &iLen, s32 threadIdx = MAIN_THREAD_IDX) = 0;



	//Set
public:
	virtual REDIS_RESULT SAdd(const std::string &strKey, const std::string &member, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT SAdd(const std::string &strKey, const StringVector &vecMembers, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT SMembers(const std::string &strKey, StringVector &vecResult, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT SRem(const std::string &strKey, const std::string &strMember, s32 threadIdx = MAIN_THREAD_IDX) = 0;
	virtual REDIS_RESULT SRem(const std::string &strKey, const StringVector &strMembers, s32 threadIdx = MAIN_THREAD_IDX) = 0;
};

//用于主线程中使用, 相同taskid 会在同一执行线程 线性执行, 并会采用pipline的方式提交给redis
class IRedisAsyncCall
{
public:
	//Key
public:
	virtual REDIS_RESULT AsyncDel(s64 taskId, const std::string &strKey) = 0;

	//String
public:
	virtual REDIS_RESULT AsyncSet(s64 taskId, const std::string &strKey, const std::string &strVal) = 0;
	virtual REDIS_RESULT AsyncSetStringWithOption(s64 taskId, const std::string &strKey, const std::string &strValue, SetCmdOptionType iOption, u32 iExpireMS) = 0;
	virtual REDIS_RESULT AsyncIncr(s64 taskId, const std::string &strKey, s64 &iOutVal) = 0;
	virtual REDIS_RESULT AsyncIncrby(s64 taskId, const std::string &strKey, s64 iVal, s64 &iOutVal) = 0;
	virtual REDIS_RESULT AsyncDecr(s64 taskId, const std::string &strKey, s64 &iOutVal) = 0;
	virtual REDIS_RESULT AsyncDecrby(s64 taskId, const std::string &strKey, s64 iVal, s64 &iOutVal) = 0;

	//Hash
public:
	virtual REDIS_RESULT AsyncHSet(s64 taskId, const std::string &strKey, const std::string &strFiled, const std::string &strVal) = 0;
	virtual REDIS_RESULT AsyncHSetNx(s64 taskId, const std::string &strKey, const std::string &strFiled, const std::string &strVal) = 0;
	virtual REDIS_RESULT AsyncHMSet(s64 taskId, const std::string &strKey, const StringMap &mapFiledVals) = 0;

	//List
public:
	virtual REDIS_RESULT AsyncPushListBack(s64 taskId, const std::string &strKey, const std::string &strValue, u32 &iLen) = 0;
	virtual REDIS_RESULT AsyncPushListBackBatch(s64 taskId, const std::string &strKey, const std::vector<std::string> &vecValue, u32 &iLen) = 0;
	virtual REDIS_RESULT AsyncTrimList(s64 taskId, const std::string &strKey, s32 iStartIndex, s32 iStopIndex) = 0;

	//Set
public:
	virtual REDIS_RESULT AsyncSAdd(s64 taskId, const std::string &strKey, const std::string &member) = 0;
	virtual REDIS_RESULT AsyncSAdd(s64 taskId, const std::string &strKey, const StringVector &vecMembers) = 0;
	virtual REDIS_RESULT AsyncSRem(s64 taskId, const std::string &strKey, const std::string &strMember) = 0;
};

class IRedisMgr : public IModule, public IRedisSyncCall, public IRedisAsyncCall
{
public:
    virtual ~IRedisMgr(){};
	virtual void SetAsyncQueue(core::IAsyncQueue *asyncQueue) = 0;
	virtual s32 GetAsyncConnectNum() = 0;	//线程threadIdx = random(0, GetAsyncConnectNum()) + 1 即可
};
#endif