public:
	virtual REDIS_RESULT Del(const std::string &strKey, s32 threadIdx = MAIN_THREAD_IDX) override;
	virtual REDIS_RESULT GetTTL(const std::string &strKey, s32 &seconds, s32 threadIdx = MAIN_THREAD_IDX) override;
	virtual REDIS_RESULT SetExpire(const std::string &strKey, const s32 seconds, s32 threadIdx = MAIN_THREAD_IDX) override;
	virtual REDIS_RESULT Exists(const std::string &strKey, bool &exists, s32 threadIdx = MAIN_THREAD_IDX) override;

	//String
public:
	virtual REDIS_RESULT Set(const std::string &strKey, const std::string &strVal, s32 threadIdx = MAIN_THREAD_IDX) override;
	virtual REDIS_RESULT SetEx(const std::string &strKey, const std::string &strVal, const s32 ttlSeconds, s32 threadIdx = MAIN_THREAD_IDX) override;
	virtual REDIS_RESULT SetNx(const std::string &strKey, const std::string &strVal, s32 threadIdx = MAIN_THREAD_IDX) override;
	virtual REDIS_RESULT Get(const std::string &strKey, std::string &strVal, s32 threadIdx = MAIN_THREAD_IDX) override;
	virtual REDIS_RESULT GetSet(const std::string &strKey, const std::string &strVal, std::string &strOutVal, s32 threadIdx = MAIN_THREAD_IDX) override;	//Abase暂不支持
	virtual REDIS_RESULT Incr(const std::string &strKey, s64 &iOutVal, s32 threadIdx = MAIN_THREAD_IDX) override;
	virtual REDIS_RESULT Incrby(const std::string &strKey, s64 iVal, s64 &iOutVal, s32 threadIdx = MAIN_THREAD_IDX) override;
	virtual REDIS_RESULT Decr(const std::string &strKey, s64 &iOutVal, s32 threadIdx = MAIN_THREAD_IDX) override;
	virtual REDIS_RESULT Decrby(const std::string &strKey, s64 iVal, s64 &iOutVal, s32 threadIdx = MAIN_THREAD_IDX) override;
	virtual REDIS_RESULT Append(const std::string &strKey, const std::string &strVal, s64 &iOutVal, s32 threadIdx = MAIN_THREAD_IDX) override;
	virtual REDIS_RESULT MGet(const StringVector &vecFileds, StringVector &vecOutVal, s32 threadIdx = MAIN_THREAD_IDX) override;
	virtual REDIS_RESULT SetStringWithOption(const std::string &strKey, const std::string &strValue, SetCmdOptionType iOption, u32 iExpireMS, bool &bSuccess, s32 threadIdx = MAIN_THREAD_IDX) override;

	//Hash
public:
	virtual REDIS_RESULT HSet(const std::string &strKey, const std::string &strFiled, const std::string &strVal, s32 threadIdx = MAIN_THREAD_IDX) override;
	virtual REDIS_RESULT HSetNx(const std::string &strKey, const std::string &strFiled, const std::string &strVal, s32 threadIdx = MAIN_THREAD_IDX) override;
	virtual REDIS_RESULT HMSet(const std::string &strKey, const StringMap &mapFiledVals, s32 threadIdx = MAIN_THREAD_IDX) override;
	virtual REDIS_RESULT HGet(const std::string &strKey, const std::string &strFiled, std::string &strOutVal, s32 threadIdx = MAIN_THREAD_IDX) override;
	virtual REDIS_RESULT HMGet(const std::string &strKey, const StringVector &vecFileds, StringVector &vecOutVal, s32 threadIdx = MAIN_THREAD_IDX) override;
	virtual REDIS_RESULT HGetAll(const std::string &strKey, StringMap &mapOutVal, s32 threadIdx = MAIN_THREAD_IDX) override;
	virtual REDIS_RESULT HDel(const std::string &strKey, const std::string &strFiled, s32 threadIdx = MAIN_THREAD_IDX) override;
	virtual REDIS_RESULT HLen(const std::string &strKey, s64 &iOutVal, s32 threadIdx = MAIN_THREAD_IDX) override;
	virtual REDIS_RESULT HExists(const std::string &strKey, const std::string &strFiled, s32 threadIdx = MAIN_THREAD_IDX) override;
	virtual REDIS_RESULT HIncrby(const std::string &strKey, const std::string &strFiled, s64 iVal, s64 &iOutVal, s32 threadIdx = MAIN_THREAD_IDX) override;
	virtual REDIS_RESULT HKeys(const std::string &strKey, StringVector &vecOutFileds, s32 threadIdx = MAIN_THREAD_IDX) override;	//Abase 暂不支持
	virtual REDIS_RESULT HVals(const std::string &strKey, StringVector &vecOutVals, s32 threadIdx = MAIN_THREAD_IDX) override;	//Abase 暂不支持

																															//List
public:
	virtual REDIS_RESULT PushListBack(const std::string &strKey, const std::string &strValue, u32 &iLen, s32 threadIdx = MAIN_THREAD_IDX) override;
	virtual REDIS_RESULT PushListBackBatch(const std::string &strKey, const std::vector<std::string> &vecValue, u32 &iLen, s32 threadIdx = MAIN_THREAD_IDX) override;
	virtual REDIS_RESULT GetListByRange(const std::string &strKey, s32 iStartIndex, s32 iStopIndex, std::vector<std::string> &vecValue, s32 threadIdx = MAIN_THREAD_IDX) override;
	virtual REDIS_RESULT TrimList(const std::string &strKey, s32 iStartIndex, s32 iStopIndex, s32 threadIdx = MAIN_THREAD_IDX) override;


	//Set
public:
	virtual REDIS_RESULT SAdd(const std::string &strKey, const std::string &member, s32 threadIdx = MAIN_THREAD_IDX) override;
	virtual REDIS_RESULT SAdd(const std::string &strKey, const StringVector &vecMembers, s32 threadIdx = MAIN_THREAD_IDX) override;
	virtual REDIS_RESULT SMembers(const std::string &strKey, StringVector &vecResult, s32 threadIdx = MAIN_THREAD_IDX) override;
	virtual REDIS_RESULT SRem(const std::string &strKey, const std::string &strMember, s32 threadIdx = MAIN_THREAD_IDX) override;
