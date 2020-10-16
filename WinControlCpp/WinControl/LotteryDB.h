#pragma once

#include "stdafx.h"
#include "Common.h"
#include "ODBCQuery.h"
#include "LogFile.h"

class LotteryDB : public CODBCQuery
{
public:
	LotteryDB(void){};
	~LotteryDB(void){};

	bool DBConnect();
	bool Ex_GetDrawLottery(DRAW_LOTTERY_PERIOD_QUEUE& queueDrawLotteryItems);
	bool Ex_GetLotteryStatistic(DRAW_LOTTERY_PERIOD& drawLotteryInfo, LOTTERY_ORDER_STAT& lotteryOrderStat);
	bool Ex_GetLottery10Results(DRAW_LOTTERY_PERIOD& drawLotteryInfo, UINT64 ulBonusAlready, UINT64 ulAllBet, ORDERS_TEN_RESULTS_VEC& order10ResultsVec);
	bool Ex_GetControledUserOrders(DRAW_LOTTERY_PERIOD& drawLotteryInfo, CONTROLED_USER_ORDERS_VEC& controledUserOrdersVec);
	bool Ex_GetLotteryUserOrders(DRAW_LOTTERY_PERIOD& drawLotteryInfo, LOTTERY_ORDER_DATA& lotteryOrderData);
	bool Ex_UpdateGameResult(LOTTERY_RESULT& tagLotteryResult, UINT32& uiRetID);

private:
	static CLogFile* pLogFile;
};
