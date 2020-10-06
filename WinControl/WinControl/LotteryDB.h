#pragma once

#include "stdafx.h"
#include <windows.h>
#include "ODBCQuery.h"
#include "Common.h"

class LotteryDB : public CODBCQuery
{
public:
	LotteryDB(void){};
	~LotteryDB(void){};

	bool DBConnect();
	bool Ex_ReadOrder(ORDER_INFO_VEC& vecOrderInfo);
	bool Ex_GetDrawLottery(DRAWLOTTERY_INFO_QUEUE& queueDrawLotteryItems);
	bool Ex_GetLotteryOrders(DRAWLOTTERY_INFO tagDrawLotteryInfo);
	bool Ex_UpdateGameResult();

};
