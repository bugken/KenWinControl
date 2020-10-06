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
	bool Ex_GetLotteryOrders(DRAW_LOTTERY_PERIOD, PLAYER_ORDERS_VEC&, PLAYER_ORDERS_VEC&);
	bool Ex_UpdateGameResult();

};
