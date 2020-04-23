#include "MysqlTest.h"
#include "UnitTest.h"
#include "Game_tools.h"
#include "MysqlBuilder.h"

UnitTestRegister<MysqlTest> mysqlTest;
void MysqlTest::StartTest(core::IKernel *kernel)
{
	core::IKernel *s_kernel = kernel;
	FIND_MODULE(_mysqlMgr, MysqlMgr);
	MainThreadTest();
	MainThreadSQLBuilder(kernel);
}

void MysqlTest::DoMysqlTestTask(int32_t iTaskId)
{
	if (iTaskId >= 10000)
	{
		return;
	}

	InsertRoleHandler *insertHandler = NEW InsertRoleHandler(iTaskId);
	_mysqlMgr->PushMysqlHandler(iTaskId, insertHandler);
	QueryRoleHandler *queryHandler = NEW QueryRoleHandler(this, iTaskId);
	_mysqlMgr->PushMysqlHandler(iTaskId, queryHandler);
}

void MysqlTest::MainThreadTest()
{
	for (s32 i = 0; i < 10; i++)
	{
		MainThreadTestHandler *mysqlHandler = NEW MainThreadTestHandler(i);
		_mysqlMgr->PushMysqlHandler(i, mysqlHandler);
		//MainThreadErrorHandler *testHandler = NEW MainThreadErrorHandler(i);
		//_mysqlMgr->PushMysqlHandler(i, testHandler);
	}
	ClearTableHandler *clearTableHandler = NEW ClearTableHandler(this);
	_mysqlMgr->PushMysqlHandler(0, clearTableHandler);
}

struct TestBinData 
{
	s32 id;
	s32 level;
	char name[5];
};

void MysqlTest::MainThreadSQLBuilder(core::IKernel *kernel)
{
	IEscapeStringHandler *hander = _mysqlMgr->GetEscapeStringHandlerInMainThread();
	if (hander)
	{
		MYSQL_QUERY(query, hander, "t_role");
		query.Select("accountid").Select("platform", "username").Where("username", SQLCommand::EQ, "dfgew").And("username", SQLCommand::EQ, "dfgew");
		query.OrderBy("username", false, "platform", "accountid", true).Limit(10);
		const char *sql = query.ToStr().c_str();
		TRACE_LOG("SQL:%s", sql);
		MYSQL_UPDATE(update, hander, "t_role");
		update.SetFiled("name", "xuping1", "level", 1).Where("roleid", SQLCommand::EQ, 1);
		const char *updateSql = update.ToStr().c_str();
		MYSQL_INSERT(insertCmd, hander, "t_role");
		insertCmd.SetFiled("roleid", 2, "accountid", 3, "level", 2);
		const char *insertSql = insertCmd.ToStr().c_str();
		MYSQL_SAVE(saveCmd, hander, "t_role");
		saveCmd.SetKey("roleid", 3).SetFiled("name", "xuping", "level", 2, "accountid", 4);
		TestBinData testBinData;
		testBinData.id = 1;
		testBinData.level = 0;
		testBinData.name[0] = 'x';
		testBinData.name[1] = 'u';
		testBinData.name[2] = 0;
		saveCmd.SetBlobFiled("bin_data", &testBinData, (s32)sizeof(testBinData));
		const char *saveSql = saveCmd.ToStr().c_str();
		ECHO("Save sql:%s", saveSql);
		query.clear();
		//query.Select("roleid", "accountid", "bin_data").Where("roleid", SQLCommand::EQ, 2);
	}
}

void MainThreadTestHandler::OnSuccess(core::IKernel *kernel, IDBResult *dbResult)
{
	s32 count = dbResult->RowCount();
	for (s32 i = 0; i < count; i++)
	{
		IRowData *row = dbResult->GetRow(i);
		for (s32 j = 0; j < row->FiledCount(); j++)
		{
			const char *content = row->GetString(j);
			TRACE_LOG("Filed[%d][%d], content:%s", i, j, content);
		}
	}

}

void MainThreadTestHandler::OnFail(core::IKernel *kernel, const s32 errorCode, const char *errorInfo)
{
	TRACE_LOG("id:%d, Exec Failed, ErrorCode:%d, Info:%s", _id, errorCode, errorInfo);
}

void MainThreadErrorHandler::OnFail(core::IKernel *kernel, const s32 errorCode, const char *errorInfo)
{
	TRACE_LOG("Exec Failed, Id:%d, ErrorCode:%d, Info:%s", _id, errorCode, errorInfo);
}

void ClearTableHandler::OnSuccess(core::IKernel *kernel, IDBResult *dbResult)
{
	TRACE_LOG("clear table success");
}

void ClearTableHandler::OnFail(core::IKernel *kernel, const s32 errorCode, const char *errorInfo)
{
	(void)kernel;
	(void)errorCode;
	(void)errorInfo;
}

void ClearTableHandler::OnRelease()
{
	TRACE_LOG("clear table release");
	_mysqlTest->DoMysqlTestTask(1);
	DEL this;
}

void InsertRoleHandler::ExecSql(IEscapeStringHandler *pEscapeStrHandler, const SqlExecCallBackFun &callBackFun)
{
	TestBinData testBinData;
	testBinData.id = _id;
	testBinData.level = _id;
	testBinData.name[0] = 'x';
	testBinData.name[1] = 'u';
	testBinData.name[2] = 0;
	MYSQL_INSERT(insertCmd, pEscapeStrHandler, "t_role");
	insertCmd.SetFiled("roleid", _id, "accountid", 3, "level", 2);
	insertCmd.SetBlobFiled("bin_data", &testBinData, (s32)sizeof(testBinData));
	insertCmd.SetFiled("online_total_time", _id);
	THREAD_LOG("sqltest","insert sql:%s", insertCmd.ToStr().c_str());
	std::string &insertSql = insertCmd.ToStr();
	callBackFun(insertSql.c_str());
}

void InsertRoleHandler::OnSuccess(core::IKernel *kernel, IDBResult *dbResult)
{
	TRACE_LOG("insert taskId: %d exec success",_id);
}

void InsertRoleHandler::OnFail(core::IKernel *kernel, const s32 errorCode, const char *errorInfo)
{
	TRACE_LOG("insert taskId: %d exec fialed, errorInfo:%s", _id, errorInfo);
}

void InsertRoleHandler::OnRelease()
{
	TRACE_LOG("insert taskid: %d release", _id);
	DEL this;
}

void QueryRoleHandler::ExecSql(IEscapeStringHandler *pEscapeStrHandler, const SqlExecCallBackFun &callBackFun)
{
	MYSQL_QUERY(queryCmd, pEscapeStrHandler, "t_role");
	queryCmd.Select("roleid", "accountid", "level", "bin_data", "online_total_time");
	queryCmd.Where("roleid", SQLCommand::EQ, _id);
	THREAD_LOG("sqltest", "query taskid: %d sql:%s", _id, queryCmd.ToStr().c_str());
	callBackFun(queryCmd.ToStr().c_str());
}

void QueryRoleHandler::OnSuccess(core::IKernel *kernel, IDBResult *dbResult)
{
	s32 count = dbResult->RowCount();
	if (count == 1)
	{
		IRowData *row = dbResult->GetRow(0);
		s32 filedCount = row->FiledCount();
		ASSERT(filedCount == 5, "filed count error");
		s32 roleId = row->GetDataS32(0);
		s32 accountid = row->GetDataS32(1);
		s32 level = row->GetDataS32(2);
		TestBinData testBinData;
		testBinData.id = _id;
		testBinData.level = _id;
		testBinData.name[0] = 'x';
		testBinData.name[1] = 'u';
		testBinData.name[2] = 0;
		s32 binSize = 0;
		const char *binData = row->GetBuff(3, binSize);
		ASSERT(binSize == sizeof(TestBinData), "data size error");
		if (memcmp(&testBinData, binData, binSize) == 0)
		{
			TRACE_LOG("bin data is ok");
		}
		s32 onlineTime = row->GetDataS32(4);
		ASSERT(onlineTime == _id, "online time error");
	}
	TRACE_LOG("query taskid: %d exec success", _id);
}

void QueryRoleHandler::OnFail(core::IKernel *kernel, const s32 errorCode, const char *errorInfo)
{
	TRACE_LOG("query taskid: %d exec fialed", _id, errorInfo);
}

void QueryRoleHandler::OnRelease()
{
	TRACE_LOG("query taskid: %d release", _id);
	_mysqlTest->DoMysqlTestTask(_id + 1);
	DEL this;
}