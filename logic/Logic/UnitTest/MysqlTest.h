#ifndef __MysqlTest_h__
#define __MysqlTest_h__
#include "IUnitTest.h"
#include "IMysqlMgr.h"
class MysqlTest : public IUnitTestInstance
{
public:
	virtual void StartTest(core::IKernel *kernel);

	void DoMysqlTestTask(int32_t iTaskId);
protected:

private:
	void MainThreadTest();
	void MainThreadSQLBuilder(core::IKernel *kernel);
private:
	IMysqlMgr *_mysqlMgr;
};

class MainThreadTestHandler : public IMysqlHandler
{
public:
	MainThreadTestHandler(s32 id) :_id(id) {};
	virtual void ExecSql(IEscapeStringHandler *pEscapeStrHandler, const SqlExecCallBackFun &callBackFun) { 
		const char *pSql = "select * from account;";
		callBackFun(pSql); 
	};

public:
	//Work In Main Thread
	virtual void OnSuccess(core::IKernel *kernel, IDBResult *dbResult);
	virtual void OnFail(core::IKernel *kernel, const s32 errorCode, const char *errorInfo);
	virtual void OnRelease() { DEL this; };

protected:
	s32 _id;
};

class MainThreadErrorHandler : public MainThreadTestHandler
{
public:
	MainThreadErrorHandler(s32 id) :MainThreadTestHandler(id) {};
	virtual void ExecSql(IEscapeStringHandler *pEscapeStrHandler, const SqlExecCallBackFun &callBackFun) { 
		const char *pSql = "select * from accoun;";
		callBackFun(pSql);
	};
	virtual void OnFail(core::IKernel *kernel, const s32 errorCode, const char *errorInfo);
protected:
private:
};

class ClearTableHandler : public IMysqlHandler
{
public:
	ClearTableHandler(MysqlTest *mysqlTest):_mysqlTest(mysqlTest) {};

	virtual void ExecSql(IEscapeStringHandler *pEscapeStrHandler, const SqlExecCallBackFun &callBackFun) {
		const char *pSql = "delete  from `t_role`;";
		callBackFun(pSql);
	};

	virtual void OnSuccess(core::IKernel *kernel, IDBResult *dbResult);
	virtual void OnFail(core::IKernel *kernel, const s32 errorCode, const char *errorInfo);
	virtual void OnRelease();
protected:
private:
	MysqlTest	*_mysqlTest;
};

class InsertRoleHandler : public IMysqlHandler
{
public:
	InsertRoleHandler(s32 id) :_id(id) {};
	~InsertRoleHandler(){}

	//Work In Work Thread
	virtual void ExecSql(IEscapeStringHandler *pEscapeStrHandler, const SqlExecCallBackFun &callBackFun) override;
	//Work In Main Thread
	virtual void OnSuccess(core::IKernel *kernel, IDBResult *dbResult) override;
	virtual void OnFail(core::IKernel *kernel, const s32 errorCode, const char *errorInfo) override;
	virtual void OnRelease() override;

protected:
private:
	s32 _id;
};

class QueryRoleHandler : public IMysqlHandler
{
public:
	QueryRoleHandler(MysqlTest *mysqlTest, s32 id) : _mysqlTest(mysqlTest), _id(id) {};
	~QueryRoleHandler() {}

	//Work In Work Thread
	virtual void ExecSql(IEscapeStringHandler *pEscapeStrHandler, const SqlExecCallBackFun &callBackFun) override;
	//Work In Main Thread
	virtual void OnSuccess(core::IKernel *kernel, IDBResult *dbResult) override;
	virtual void OnFail(core::IKernel *kernel, const s32 errorCode, const char *errorInfo) override;
	virtual void OnRelease() override;
protected:
private:
	s32 _id;
	MysqlTest *_mysqlTest;
};

#endif
