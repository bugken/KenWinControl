// WinControl.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include "ODBCQuery.h"

typedef struct _SYSDEFINE_STRINFO
{
	UINT32  uiSysID;
	UINT64  uiIntValue;
	float   fFloatValue;
}SYSDEFINE_STRINFO;

class CMiniContrlDB : public CODBCQuery
{
public:
	CMiniContrlDB(){};
	~CMiniContrlDB(){};
	bool DBConnect(){
		bool ret = false;
		char pStr[100] = "DRIVER={SQL Server Native Client 10.0};SERVER=127.0.0.1;DATABASE=GoldControlDB;UID=sa;PWD=123456;";
		bool IsConnect = DriverConnect(pStr);
		if (IsConnect)
		{
			ret = true;
			printf("连接成功\n");
		}
		return ret;
	}

	bool Ex_SystemDefLoad()
	{
		bool bResult = false;
		SYSDEFINE_STRINFO hinfo = { 0 };
		ClearMoreResults();

		InitBindParam();
		bResult = ExecuteDirect(TEXT("{call dbo.xp_LoadSystemDefInfo}"));
		if (!bResult)
		{
			printf("xp_LoadSystemDefInfo failed ... \n");
			return false;
		}

		InitBindCol();
		BindCol(hinfo.uiSysID);
		BindCol(hinfo.uiIntValue);
		BindCol(hinfo.fFloatValue);
		bool bOK = Fetch();
		ClearMoreResults();

		printf("hinfo.uiSysID:%d\n", hinfo.uiSysID);
		printf("hinfo.uiIntValue:%I64u\n", hinfo.uiIntValue);
		printf("hinfo.fFloatValue:%f\n", hinfo.fFloatValue);

		return true;
	}

};

int _tmain(int argc, _TCHAR* argv[])
{
	CMiniContrlDB db;
	db.DBConnect();

	db.Ex_SystemDefLoad();

	printf("this is main\n");
	system("pause");
	return 0;
}

