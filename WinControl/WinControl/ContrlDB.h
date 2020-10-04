#pragma once

#include "stdafx.h"
#include <windows.h>
#include "ODBCQuery.h"
#include "Common.h"

class ContrlDB : public CODBCQuery
{
public:
	ContrlDB(void){};
	~ContrlDB(void){};

	bool DBConnect();
	bool Ex_ReadOrder(ORDER_INFO_VEC& vecOrderInfo);
};
