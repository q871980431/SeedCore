//Key
public:
	virtual REDIS_RESULT AsyncDel(s64 taskId, const std::string &strKey) override;

	//String
public:
	virtual REDIS_RESULT AsyncSet(s64 taskId, const std::string &strKey, const std::string &strVal) override;
	virtual REDIS_RESULT AsyncSetEx(s64 taskId, const std::string &strKey, const std::string &strVal, const s32 ttlSeconds) override;
	virtual REDIS_RESULT AsyncSetNx(s64 taskId, const std::string &strKey, const std::string &strVal) override;
	virtual REDIS_RESULT AsyncIncr(s64 taskId, const std::string &strKey, s64 &iOutVal) override;
	virtual REDIS_RESULT AsyncIncrby(s64 taskId, const std::string &strKey, s64 iVal, s64 &iOutVal) override;
	virtual REDIS_RESULT AsyncDecr(s64 taskId, const std::string &strKey, s64 &iOutVal) override;
	virtual REDIS_RESULT AsyncDecrby(s64 taskId, const std::string &strKey, s64 iVal, s64 &iOutVal) override;

	//Hash
public:
	virtual REDIS_RESULT AsyncHSet(s64 taskId, const std::string &strKey, const std::string &strFiled, const std::string &strVal) override;
	virtual REDIS_RESULT AsyncHSetNx(s64 taskId, const std::string &strKey, const std::string &strFiled, const std::string &strVal) override;
	virtual REDIS_RESULT AsyncHMSet(s64 taskId, const std::string &strKey, const StringMap &mapFiledVals) override;

	//List
public:
	virtual REDIS_RESULT AsyncPushListBack(s64 taskId, const std::string &strKey, const std::string &strValue, u32 &iLen) override;
	virtual REDIS_RESULT AsyncPushListBackBatch(s64 taskId, const std::string &strKey, const std::vector<std::string> &vecValue, u32 &iLen) override;
	virtual REDIS_RESULT AsyncTrimList(s64 taskId, const std::string &strKey, s32 iStartIndex, s32 iStopIndex) override;

	//Set
public:
	virtual REDIS_RESULT AsyncSAdd(s64 taskId, const std::string &strKey, const std::string &member) override;
	virtual REDIS_RESULT AsyncSAdd(s64 taskId, const std::string &strKey, const StringVector &vecMembers) override;
	virtual REDIS_RESULT AsyncSRem(s64 taskId, const std::string &strKey, const std::string &strMember) override;
