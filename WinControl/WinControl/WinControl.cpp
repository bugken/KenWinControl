// WinControl.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "ContrlDB.h"

int _tmain(int argc, _TCHAR* argv[])
{
	ContrlDB db;
	db.DBConnect();

	ORDER_INFO_VEC vecOrderInfo;
	db.Ex_ReadOrder(vecOrderInfo);

	printf("vecOrderInfo size:%d\n", vecOrderInfo.size());
	system("pause");
	return 0;
}

