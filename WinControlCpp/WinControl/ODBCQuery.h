#pragma once

#include <sql.h>
#include <sqlext.h>
#include "sqlncli.h"

#define MAX_BIND_PARAM	200	

#define SUCCEED_ODBC(X)		((SQL_SUCCESS == X)||(SQL_SUCCESS_WITH_INFO == X))
#define FAILED_ODBC(X)		((SQL_ERROR == X)||(SQL_INVALID_HANDLE == X))

class CODBCQuery
{
public:
	CODBCQuery();
	virtual ~CODBCQuery();

	void GetError();
	void GetError2(LPSTR sql, bool& bIsDisconnectted, char* errorStr = NULL);
	bool IsConnected(){return m_bConnected;};

	HENV GetHEnv(){return m_henv;};
	HSTMT GetHStmt(){return m_hstmt;};
	HDBC GetHDbc(){return m_hdbc;};

	bool Startup();
	bool Connect(LPSTR pszDSN,LPSTR pszID,LPSTR pszPW);
	bool DriverConnect(LPSTR pszDSN);
	void Disconnect();
	void Cleanup();
	void ClearMoreResults(bool bCloseCursor = false);
	void InitBindParam();

	SQLRETURN BindParam(int& nValue,SQLSMALLINT inoutType = SQL_PARAM_INPUT);
	SQLRETURN BindParam(short& nValue,SQLSMALLINT inoutType = SQL_PARAM_INPUT);
	SQLRETURN BindParam(__int64& nValue,SQLSMALLINT inoutType = SQL_PARAM_INPUT); 
	SQLRETURN BindParam(unsigned int& nValue,SQLSMALLINT inoutType = SQL_PARAM_INPUT);
	SQLRETURN BindParam(unsigned short& nValue,SQLSMALLINT inoutType = SQL_PARAM_INPUT);
	SQLRETURN BindParam(unsigned __int64& nValue,SQLSMALLINT inoutType = SQL_PARAM_INPUT); 
	SQLRETURN BindParam(char* pstr,int dbColumnLen,SQLSMALLINT inoutType = SQL_PARAM_INPUT);
	SQLRETURN BindParam(float& fValue,SQLSMALLINT inoutType = SQL_PARAM_INPUT);
	SQLRETURN BindParamVarChar(char* pstr,int dbColumnLen,SQLSMALLINT inoutType = SQL_PARAM_INPUT);
	SQLRETURN BindParamVarBinary(unsigned char* pBuf,int dbColumnLen,SQLSMALLINT inoutType = SQL_PARAM_INPUT);
	SQLRETURN BindParam(TIMESTAMP_STRUCT& dsDateTime,SQLSMALLINT inoutType = SQL_PARAM_INPUT);
	SQLRETURN BindParam(INT8& nValue,SQLSMALLINT inoutType = SQL_PARAM_INPUT);
	SQLRETURN BindParam(UINT8& nValue,SQLSMALLINT inoutType = SQL_PARAM_INPUT);

	void InitBindCol();
	SQLRETURN BindCol(INT8& nValue);	
	SQLRETURN BindCol(UINT8& nValue);	
	SQLRETURN BindCol(bool& nValue);		
	SQLRETURN BindCol(int& nValue);
	SQLRETURN BindCol(short& nValue);
	SQLRETURN BindCol(__int64& nValue);
	SQLRETURN BindCol(unsigned int& nValue);
	SQLRETURN BindCol(unsigned short& nValue);
	SQLRETURN BindCol(unsigned __int64& nValue);
	SQLRETURN BindCol(LPSTR str,int strBufMaxLen);		
	SQLRETURN BindCol(unsigned char * pbuf, int strBufMaxLen);
	SQLRETURN BindCol(float& nValue);
	SQLRETURN BindCol(DATE_STRUCT &dsDate);				
	SQLRETURN BindCol(TIME_STRUCT &dsTime);				
	SQLRETURN BindCol(TIMESTAMP_STRUCT &dsDateTime);	



	bool Prepare(LPSTR sql);
	bool Execute();
	bool ExecuteDirect(LPSTR sql, char* errorStr = NULL);
	bool Fetch();
	void CloseCursor();
	bool IsFetchNoData();

protected:
	bool m_bConnected;

	HENV m_henv;
	HDBC m_hdbc;
	HSTMT m_hstmt;
	SQLUSMALLINT m_nBindParam;
	SQLUSMALLINT m_nBindCol;

	SQLRETURN m_retCode;
	
#ifndef _WIN64
	SQLINTEGER m_cbValue[MAX_BIND_PARAM];	
#else
	INT64 m_cbValue[MAX_BIND_PARAM];
#endif


private:
	TCHAR m_szConnectionString[MAX_PATH + 1];
	int   m_nCursorErrorCount;
	void SetConnecionString(LPTSTR pSZCString);
	bool ReConnect();

public:
	static int msODBCVer;
};
