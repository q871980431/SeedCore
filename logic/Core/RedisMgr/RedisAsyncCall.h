//Key
public:
	virtual REDIS_RESULT AsyncDel(const std::string &strKey) override;

	//String
public:
	virtual REDIS_RESULT AsyncSet(const std::string &strKey, const std::string &strVal) override;
	virtual REDIS_RESULT AsyncSetEx(const std::string &strKey, const std::string &strVal, const s32 ttlSeconds) override;
	virtual REDIS_RESULT AsyncSetNx(const std::string &strKey, const std::string &strVal) override;
	virtual REDIS_RESULT AsyncIncr(const std::string &strKey, s64 &iOutVal) override;
	virtual REDIS_RESULT AsyncIncrby(const std::string &strKey, s64 iVal, s64 &iOutVal) override;
	virtual REDIS_RESULT AsyncDecr(const std::string &strKey, s64 &iOutVal) override;
	virtual REDIS_RESULT AsyncDecrby(const std::string &strKey, s64 iVal, s64 &iOutVal) override;

	//Hash
public:
	virtual REDIS_RESULT AsyncHSet(const std::string &strKey, const std::string &strFiled, const std::string &strVal) override;
	virtual REDIS_RESULT AsyncHSetNx(const std::string &strKey, const std::string &strFiled, const std::string &strVal) override;
	virtual REDIS_RESULT AsyncHMSet(const std::string &strKey, const StringMap &mapFiledVals) override;

	//List
public:
	virtual REDIS_RESULT AsyncPushListBack(const std::string &strKey, const std::string &strValue, u32 &iLen) override;
	virtual REDIS_RESULT AsyncPushListBackBatch(const std::string &strKey, const std::vector<std::string> &vecValue, u32 &iLen) override;
	virtual REDIS_RESULT AsyncTrimList(const std::string &strKey, s32 iStartIndex, s32 iStopIndex) override;

	//Set
public:
	virtual REDIS_RESULT AsyncSAdd(const std::string &strKey, const std::string &member) override;
	virtual REDIS_RESULT AsyncSAdd(const std::string &strKey, const StringVector &vecMembers) override;
	virtual REDIS_RESULT AsyncSRem(const std::string &strKey, const std::string &strMember) override;
