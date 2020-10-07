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
	bool Ex_GetDrawLottery(DRAW_LOTTERY_PERIOD_QUEUE& queueDrawLotteryItems);
	bool Ex_GetLotteryUserOrders(DRAW_LOTTERY_PERIOD drawLotteryInfo, LOTTERY_ORDER_DATA& lotteryOrderData);
	bool Ex_UpdateGameResult(LOTTERY_RESULT tagLotteryResult);
};
