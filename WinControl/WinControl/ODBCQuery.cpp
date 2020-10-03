//#ifdef _DEBUG
//#include <io.h>
//#include <stdio.h>
//#endif

#include "stdafx.h"
#include "ServerPublic.h"
#include <tchar.h>
#include <time.h>
#include "odbcquery.h"
#include "LogFileEx.h"


#ifndef _WIN64
//#pragma comment(lib,"odbc32.lib")
#else
#pragma comment(lib,"odbc32.lib")
#endif

#define MAX_ERROR_SIZE 1024
#define BINDCHECKVARCHAR(X,Y)  if(Y == 0) X[0] = ' ';

CLogFileEx* CODBCQuery::mLogFile = NULL;
int CODBCQuery::msODBCVer = 6;

////////////////////////////////////////////
CODBCQuery::CODBCQuery() : m_henv(0), m_hdbc(0), m_hstmt(0), m_nBindParam(0), m_nBindCol(0), m_bConnected(false)
{
	memset(&m_cbValue,0,sizeof(m_cbValue));
	memset(m_szConnectionString, 0, sizeof(m_szConnectionString));	

	Startup();
}

CODBCQuery::~CODBCQuery()
{
	Disconnect();
	Cleanup();
}

void CODBCQuery::SetConnecionString(LPTSTR pSZCString)
{
	_tcsncpy_s(m_szConnectionString, pSZCString, MAX_PATH);	
}

bool CODBCQuery::ReConnect()
{
	if (_tcslen(m_szConnectionString) != 0)	
	{
		bool bConnected = false;
		//	那么先断开连接, 重试5次
		for (int i = 0; i < 5; i++)
		{
			Disconnect();
			Cleanup();

			//	如果启动不了，那么直接退出程序
			if (!Startup())
			{
				Disconnect();
				Cleanup();
				ExitProcess(-1);
			}

			if (DriverConnect(m_szConnectionString))
			{
				//	如果连接上了，那么直接退出
				bConnected = true;
				return true;
			}
			//	否则，暂停100ms，再重新连接
			Sleep(100);
		}	
		if (!bConnected)
		{
			Disconnect();
			Cleanup();
			ExitProcess(-1);
		}
	}
	return false;
}
bool CODBCQuery::Startup()
{
	// Allocate the ODBC environment and save handle.
	m_retCode = ::SQLAllocHandle (SQL_HANDLE_ENV, NULL, &m_henv);
	if(!SUCCEED_ODBC(m_retCode)) goto return_with_error;

	// Notify ODBC that this is an ODBC 3.0 app.
	m_retCode = ::SQLSetEnvAttr(m_henv, SQL_ATTR_ODBC_VERSION,(SQLPOINTER) SQL_OV_ODBC3, SQL_IS_INTEGER);
	if(!SUCCEED_ODBC(m_retCode)) goto return_with_error;

	// Allocate ODBC connection handle and connect.
	m_retCode = ::SQLAllocHandle(SQL_HANDLE_DBC, m_henv, &m_hdbc);
	if(!SUCCEED_ODBC(m_retCode)) goto return_with_error;


	TCHAR szResult[30];
	SWORD nResult;
	::SQLGetInfo (m_hdbc, SQL_ODBC_VER, szResult, sizeof(szResult), &nResult);

	m_nCursorErrorCount = 0;	//	每次连接的时候，将游标错误数量置为0

	return true;

return_with_error:
	GetError();
	return false;
}

bool CODBCQuery::Connect(LPSTR pszDSN,LPSTR pszID,LPSTR pszPW)
{
	m_retCode = ::SQLConnect(m_hdbc, (SQLCHAR*)pszDSN, SQL_NTS,(SQLCHAR*)pszID, SQL_NTS, (SQLCHAR*)pszPW, SQL_NTS);
	if(!SUCCEED_ODBC(m_retCode)) 
	{
		printf("%s %d SQLConnect() was failed\n", __FUNCTION__, __LINE__);
		goto return_with_error;
	}

	// Allocate statement handle.
	m_retCode = ::SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &m_hstmt);
	if(!SUCCEED_ODBC(m_retCode))
	{
		printf("%s %d SQLAllocHandle() was failed\n", __FUNCTION__, __LINE__);
		goto return_with_error;
	}

	m_bConnected = true;

	return true;

return_with_error:
	GetError();
	return false;
}

bool CODBCQuery::DriverConnect(LPSTR pszDSN)
{
	SetConnecionString(pszDSN);

	#define MAX_CONN_OUT 128

	SQLTCHAR szOutConn[MAX_CONN_OUT];
	short cbOutConn;

	if (CODBCQuery::msODBCVer == 6)
	{
		m_retCode = ::SQLDriverConnect(m_hdbc, NULL, (SQLCHAR*)pszDSN, SQL_NTS, szOutConn, MAX_CONN_OUT, &cbOutConn, SQL_DRIVER_COMPLETE);
	}
	else
	{
		m_retCode = ::SQLDriverConnect(m_hdbc, NULL, (SQLCHAR*)pszDSN, SQL_NTS, szOutConn, MAX_CONN_OUT, &cbOutConn, SQL_DRIVER_NOPROMPT);
	}
	if(!SUCCEED_ODBC(m_retCode)) 
	{
		GetError();
		return false;
	}

	// Allocate statement handle.
	m_retCode = ::SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &m_hstmt);
	if(!SUCCEED_ODBC(m_retCode)) return false;

	m_bConnected = true;

	return true;
}

void CODBCQuery::Disconnect()
{
	m_bConnected = false;

   if(m_hstmt) SQLFreeHandle(SQL_HANDLE_STMT, m_hstmt);
   if(m_hdbc) SQLDisconnect(m_hdbc);
   m_hstmt = 0;
   m_hdbc = 0;
}

void CODBCQuery::Cleanup()
{
   if(m_hdbc) SQLFreeHandle(SQL_HANDLE_DBC, m_hdbc);
   if(m_henv) SQLFreeHandle(SQL_HANDLE_ENV, m_henv);
   m_henv = m_hdbc = 0;
}

void CODBCQuery::InitBindParam()
{
	m_nBindParam = 0;
	memset(&m_cbValue,0,sizeof(m_cbValue));

#if (ODBCVER < 0x0300)
	m_retCode = ::SQLFreeStmt(m_hstmt,SQL_RESET_PARAMS);
#endif
}

SQLRETURN CODBCQuery::BindParam(INT8& nValue,SQLSMALLINT inoutType)
{
	m_cbValue[++m_nBindParam] = 0;
	assert(m_nBindParam < MAX_BIND_PARAM);
	return ::SQLBindParameter(m_hstmt,m_nBindParam,inoutType,SQL_C_TINYINT,SQL_TINYINT,0,0,&nValue,0,&m_cbValue[m_nBindParam]);
}


SQLRETURN CODBCQuery::BindParam(UINT8& nValue,SQLSMALLINT inoutType)
{
	m_cbValue[++m_nBindParam] = 0;
	assert(m_nBindParam < MAX_BIND_PARAM);
	return ::SQLBindParameter(m_hstmt,m_nBindParam,inoutType,SQL_C_UTINYINT,SQL_TINYINT,0,0,&nValue,0,&m_cbValue[m_nBindParam]);
}

SQLRETURN CODBCQuery::BindParam(int& nValue,SQLSMALLINT inoutType)
{
	m_cbValue[++m_nBindParam] = 0;
	assert(m_nBindParam < MAX_BIND_PARAM);
	return ::SQLBindParameter(m_hstmt,m_nBindParam,inoutType,SQL_C_SLONG,SQL_INTEGER,0,0,&nValue,0,&m_cbValue[m_nBindParam]);
}
SQLRETURN CODBCQuery::BindParam(short& nValue,SQLSMALLINT inoutType)
{
	m_cbValue[++m_nBindParam] = 0;
	assert(m_nBindParam < MAX_BIND_PARAM);
	return ::SQLBindParameter(m_hstmt,m_nBindParam,inoutType,SQL_C_SSHORT,SQL_SMALLINT,0,0,&nValue,0,&m_cbValue[m_nBindParam]);
}

SQLRETURN CODBCQuery::BindParam(__int64& nValue,SQLSMALLINT inoutType)
{
	m_cbValue[++m_nBindParam] = 0;
	assert(m_nBindParam < MAX_BIND_PARAM);
	return ::SQLBindParameter(m_hstmt,m_nBindParam,inoutType,SQL_C_SBIGINT,SQL_BIGINT,0,0,&nValue,0,&m_cbValue[m_nBindParam]);
} 

SQLRETURN CODBCQuery::BindParam(unsigned int& nValue,SQLSMALLINT inoutType)
{
	m_cbValue[++m_nBindParam] = 0;
	assert(m_nBindParam < MAX_BIND_PARAM);
	return ::SQLBindParameter(m_hstmt,m_nBindParam,inoutType,SQL_C_ULONG,SQL_INTEGER,0,0,&nValue,0,&m_cbValue[m_nBindParam]);
}
SQLRETURN CODBCQuery::BindParam(unsigned short& nValue,SQLSMALLINT inoutType)
{
	m_cbValue[++m_nBindParam] = 0;
	assert(m_nBindParam < MAX_BIND_PARAM);
	return ::SQLBindParameter(m_hstmt,m_nBindParam,inoutType,SQL_C_USHORT,SQL_SMALLINT,0,0,&nValue,0,&m_cbValue[m_nBindParam]);
}

SQLRETURN CODBCQuery::BindParam(unsigned __int64& nValue,SQLSMALLINT inoutType)
{
	m_cbValue[++m_nBindParam] = 0;
	assert(m_nBindParam < MAX_BIND_PARAM);
	return ::SQLBindParameter(m_hstmt,m_nBindParam,inoutType,SQL_C_UBIGINT,SQL_BIGINT,0,0,&nValue,0,&m_cbValue[m_nBindParam]);
} 

SQLRETURN CODBCQuery::BindParam(char* pstr,int dbColumnLen,SQLSMALLINT inoutType)
{
	BINDCHECKVARCHAR(pstr,dbColumnLen);
	m_cbValue[++m_nBindParam] = SQL_NTS;
	assert(m_nBindParam < MAX_BIND_PARAM);
	return ::SQLBindParameter(m_hstmt,m_nBindParam,inoutType,SQL_C_CHAR,SQL_CHAR,dbColumnLen,0,pstr,0,&m_cbValue[m_nBindParam]);
}

SQLRETURN CODBCQuery::BindParamVarChar(char* pstr,int dbColumnLen,SQLSMALLINT inoutType)
{
	m_cbValue[++m_nBindParam] = SQL_NTS;
	assert(m_nBindParam < MAX_BIND_PARAM);
	return ::SQLBindParameter(m_hstmt, m_nBindParam, inoutType, SQL_C_CHAR, SQL_VARCHAR, dbColumnLen, 0, pstr, 0, &m_cbValue[m_nBindParam]);
}

SQLRETURN CODBCQuery::BindParamVarBinary(unsigned char* pBuf,int dbColumnLen,SQLSMALLINT inoutType)
{
	m_cbValue[++m_nBindParam] = dbColumnLen;
	assert(m_nBindParam < MAX_BIND_PARAM);
	return ::SQLBindParameter(m_hstmt, m_nBindParam, inoutType, SQL_C_BINARY, SQL_LONGVARBINARY, dbColumnLen, 0, pBuf, 0, &m_cbValue[m_nBindParam]);

}

SQLRETURN CODBCQuery::BindParam(float& fValue,SQLSMALLINT inoutType)
{
	m_cbValue[++m_nBindParam] = 0;
	assert(m_nBindParam < MAX_BIND_PARAM);
	return ::SQLBindParameter(m_hstmt,m_nBindParam,inoutType,SQL_C_FLOAT,SQL_FLOAT,0,0,&fValue,0,&m_cbValue[m_nBindParam]);
}

SQLRETURN CODBCQuery::BindParam(TIMESTAMP_STRUCT& dsDateTime,SQLSMALLINT inoutType)
{
	m_cbValue[++m_nBindParam] = 0;
	assert(m_nBindParam < MAX_BIND_PARAM);
	return ::SQLBindParameter(m_hstmt,m_nBindParam,inoutType, SQL_C_TYPE_TIMESTAMP,SQL_TYPE_TIMESTAMP,SQL_TIMESTAMP_LEN,0,&dsDateTime,0,&m_cbValue[m_nBindParam]); 
}

void CODBCQuery::InitBindCol()
{
	m_nBindCol= 0;
	memset(&m_cbValue,0,sizeof(m_cbValue));

#if (ODBCVER < 0x0300)
	m_retCode = ::SQLFreeStmt(m_hstmt,SQL_UNBIND);
#endif
}

SQLRETURN CODBCQuery::BindCol(INT8& nValue)
{
	m_nBindCol++;
	return ::SQLBindCol(m_hstmt,m_nBindCol,SQL_C_TINYINT,&nValue,sizeof(INT8),&m_cbValue[m_nBindParam]);
}

SQLRETURN CODBCQuery::BindCol(UINT8 & nValue)
{
	m_nBindCol++;
	return ::SQLBindCol(m_hstmt,m_nBindCol,SQL_C_UTINYINT,&nValue,sizeof(UINT8),&m_cbValue[m_nBindParam]);
}

SQLRETURN CODBCQuery::BindCol(bool& nValue)
{
	m_nBindCol++;
	return ::SQLBindCol(m_hstmt,m_nBindCol,SQL_C_BIT,&nValue,sizeof(SQL_C_BIT),&m_cbValue[m_nBindParam]);
}


SQLRETURN CODBCQuery::BindCol(int& nValue)
{
	m_nBindCol++;
	return ::SQLBindCol(m_hstmt,m_nBindCol,SQL_C_SLONG,&nValue,sizeof(int),&m_cbValue[m_nBindParam]);
}

SQLRETURN CODBCQuery::BindCol(unsigned int& nValue)
{
	m_nBindCol++;
	return ::SQLBindCol(m_hstmt,m_nBindCol,SQL_C_ULONG,&nValue,sizeof(unsigned int),&m_cbValue[m_nBindParam]);
}

SQLRETURN CODBCQuery::BindCol(short& nValue)
{
	m_nBindCol++;
	return ::SQLBindCol(m_hstmt,m_nBindCol,SQL_C_SSHORT,&nValue,sizeof(short),&m_cbValue[m_nBindParam]);
}

SQLRETURN CODBCQuery::BindCol(unsigned short& nValue)
{
	m_nBindCol++;
	return ::SQLBindCol(m_hstmt,m_nBindCol,SQL_C_USHORT,&nValue,sizeof(unsigned short),&m_cbValue[m_nBindParam]);
}

SQLRETURN CODBCQuery::BindCol(__int64& nValue) // by rowid
{
	m_nBindCol++;
	return ::SQLBindCol(m_hstmt,m_nBindCol,SQL_C_SBIGINT,&nValue,sizeof(__int64),&m_cbValue[m_nBindParam]);
}

SQLRETURN CODBCQuery::BindCol(unsigned __int64& nValue) // by rowid
{
	m_nBindCol++;
	return ::SQLBindCol(m_hstmt,m_nBindCol,SQL_C_UBIGINT,&nValue,sizeof(unsigned __int64),&m_cbValue[m_nBindParam]);
}

SQLRETURN CODBCQuery::BindCol(float& fValue)
{
	m_nBindCol++;
	return ::SQLBindCol(m_hstmt,m_nBindCol,SQL_C_FLOAT,&fValue,sizeof(float),&m_cbValue[m_nBindParam]);
}

SQLRETURN CODBCQuery::BindCol(DATE_STRUCT &dsDate)
{
	m_nBindCol++;
	return ::SQLBindCol(m_hstmt,m_nBindCol,SQL_C_TYPE_DATE,&dsDate,sizeof(DATE_STRUCT),&m_cbValue[m_nBindParam]);
}

SQLRETURN CODBCQuery::BindCol(TIME_STRUCT &dsTime)
{
	m_nBindCol++;
	return ::SQLBindCol(m_hstmt,m_nBindCol,SQL_C_TYPE_TIME,&dsTime,sizeof(TIME_STRUCT),&m_cbValue[m_nBindParam]);
}

SQLRETURN CODBCQuery::BindCol(TIMESTAMP_STRUCT &dsDateTime)
{
	m_nBindCol++;
	return ::SQLBindCol(m_hstmt,m_nBindCol,SQL_C_TIMESTAMP,&dsDateTime,sizeof(TIMESTAMP_STRUCT),&m_cbValue[m_nBindParam]);
}


SQLRETURN CODBCQuery::BindCol(LPSTR str,int strBufMaxLen)
{
	m_nBindCol++;
	return ::SQLBindCol(m_hstmt,m_nBindCol,SQL_C_CHAR,str,strBufMaxLen,&m_cbValue[m_nBindParam]);
}

SQLRETURN CODBCQuery::BindCol(unsigned char * pbuf, int strBufMaxLen)
{
	m_nBindCol++;
	return ::SQLBindCol(m_hstmt,m_nBindCol,SQL_C_BINARY,pbuf,strBufMaxLen,&m_cbValue[m_nBindParam]);
}

bool CODBCQuery::Prepare(LPSTR sql)
{
	m_retCode = ::SQLPrepare(m_hstmt,(SQLCHAR*)sql,SQL_NTS);
	if(!SUCCEED_ODBC(m_retCode)) return false;
	return true;
}

bool CODBCQuery::Execute()
{
	ClearMoreResults(true);

	if(!SUCCEED_ODBC(::SQLExecute(m_hstmt))) 
	{
		GetError();
		ClearMoreResults(true);
		return false;
	}
	return true;
}

bool CODBCQuery::ExecuteDirect(LPSTR sql, char* errorStr)
{
	SQLRETURN ret;
	
	ClearMoreResults(true);
	ret = ::SQLExecDirect(m_hstmt,(SQLCHAR*)sql,SQL_NTS);
	if(!SUCCEED_ODBC(ret))
	{
		bool bIsDisconnectted = false;
		GetError2(sql, bIsDisconnectted, errorStr);
		ClearMoreResults(true);

		if (bIsDisconnectted)	//	连接已经断开
		{
			if (ReConnect())	//	重连成功
			{
				//	连接上了，再执行一下本次数据操作
				ret = ::SQLExecDirect(m_hstmt,(SQLCHAR*)sql,SQL_NTS);
				if(!SUCCEED_ODBC(ret))
				{
					GetError2(sql, bIsDisconnectted);
					ClearMoreResults(true);
					return false;
				}
				else
				{
					return true;
				}
			}
		}
		return false;
	}
	return true;
}

bool CODBCQuery::Fetch()
{
	m_retCode = ::SQLFetch(m_hstmt);
	if(SUCCEED_ODBC(m_retCode)) return true;
	else if(m_retCode == SQL_NO_DATA)
	{
		m_retCode = ::SQLMoreResults(m_hstmt);
		if(SUCCEED_ODBC(m_retCode)) return true;
	}
	else
	{
		GetError();
		ClearMoreResults(true);
		return false;
	}

	return false;
}

bool CODBCQuery::IsFetchNoData()
{
	return m_retCode == SQL_NO_DATA;
}

void CODBCQuery::CloseCursor()
{
	if(!SUCCEED_ODBC(::SQLCloseCursor(m_hstmt)))
	{
		//GetError();
	}
}

void CODBCQuery::ClearMoreResults(bool bCloseCursor)
{
   // Clear any result sets generated.
	while (1)
	{
		m_retCode = SQLMoreResults(m_hstmt);
		if (m_retCode == SQL_NO_DATA)
		{
			break;
		}
	}

   if(bCloseCursor)
   {
	   m_retCode = ::SQLCloseCursor(m_hstmt);
   }
}

void CODBCQuery::GetError()
{
	char buf[MAX_ERROR_SIZE]; 
	char szState[100]; 
	RETCODE rc=SQL_SUCCESS;  

	int nCount = 0;

	for(rc = ::SQLError(m_henv, m_hdbc, m_hstmt, (unsigned char *)szState, NULL, (unsigned char *)buf, MAX_ERROR_SIZE, NULL) ;;
		rc = ::SQLError(m_henv, NULL, NULL, (unsigned char*)szState, NULL, (unsigned char *)buf, MAX_ERROR_SIZE, NULL))
	{ 
//#ifdef _DEBUG
//		if (access("c:/finddb.err", 0) != -1)
//		{
//			assert(false); //高峰此处是测试的代码
//		}		
//#endif
//
		printf("%s %d Error=[%s] %s\n", __FUNCTION__, __LINE__, szState, buf);

		if (rc==SQL_SUCCESS || rc == SQL_NO_DATA) break;
		if(++nCount > 10)
		{
			assert(false && "CODBCQuery::GetError() : Infinite loop!!!");
			return;
		}
	}
}


void CODBCQuery::GetError2(LPSTR sql, bool& bIsDisconnectted, char *errorStr)
{
	char buf[MAX_ERROR_SIZE]; 
	char szState[2048]; 
	RETCODE rc=SQL_SUCCESS;  

	int nCount = 0;
	memset(szState, 0, sizeof(szState));

	for(rc = ::SQLError(m_henv, m_hdbc, m_hstmt, (unsigned char *)szState, NULL, (unsigned char *)buf, MAX_ERROR_SIZE, NULL) ;;
		rc = ::SQLError(m_henv, NULL, NULL, (unsigned char*)szState, NULL, (unsigned char *)buf, MAX_ERROR_SIZE, NULL))
	{ 	
		printf("%s %d Error=[%s] [%s] sql=[%s]\n", __FUNCTION__, __LINE__, szState, buf, sql);

		if (errorStr)
		{
			strcpy_s(errorStr,1024, buf);
		}

		//	连接断开
		char szDisConnection[] = {"08S01"};		
		if (strncmp(szState, szDisConnection, sizeof(szDisConnection)) == 0)
		{
			bIsDisconnectted = true;
		}
		//	游标错误
		char szErrorCursor[] = {"24000"};
		if (strncmp(szState, szErrorCursor, sizeof(szErrorCursor)) == 0)
		{
			//	bIsDisconnectted = true;
			//	加入统计，记录出现的次数
			m_nCursorErrorCount++;
		}

		if (rc==SQL_SUCCESS || rc == SQL_NO_DATA) break;
		if(++nCount > 10)
		{
			assert(false && "CODBCQuery::GetError() : Infinite loop!!!");
			return;
		}
		memset(szState, 0, sizeof(szState));
	}
}

