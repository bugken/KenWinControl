#include "stdafx.h"
#include "LotteryDB.h"

bool LotteryDB::DBConnect()
{
	bool ret = false;
	char pStr[100] = "DRIVER={SQL Server Native Client 10.0};SERVER=47.56.134.27;DATABASE=9lottery;UID=sa;PWD=Jack7361;";
	bool IsConnect = DriverConnect(pStr);
	if (IsConnect)
	{
		printf("thread(%d) connect lottery db succes\n", GetCurrentThreadId());
		ret = true;
	}
	return ret;
}

bool LotteryDB::Ex_GetDrawLottery(DRAW_LOTTERY_PERIOD_QUEUE& queueDrawLotteryItems)
{
	CTicker timeLapser("sp_GetDrawLotteryInfo");
	bool bResult = false;
	DRAW_LOTTERY_PERIOD tagDrwaLotteryPeriod;

	ClearMoreResults();
	InitBindParam();
	bResult = ExecuteDirect(TEXT("{call dbo.sp_GetDrawLotteryInfo}"));
	if (!bResult)
	{
		printf("sp_GetDrawLotteryInfo failed ... \n");
		return false;
	}
	InitBindCol();
	BindCol(tagDrwaLotteryPeriod.iTypeID); 
	BindCol(tagDrwaLotteryPeriod.iUserControled); 
	BindCol(tagDrwaLotteryPeriod.iControlRate); 
	BindCol(tagDrwaLotteryPeriod.iPowerControl); 
	BindCol(tagDrwaLotteryPeriod.strCurrentIssueNumber, sizeof(tagDrwaLotteryPeriod.strCurrentIssueNumber));
	BindCol(tagDrwaLotteryPeriod.strLastIssueNumber, sizeof(tagDrwaLotteryPeriod.strLastIssueNumber));
	BindCol(tagDrwaLotteryPeriod.strBeginIssueNumber, sizeof(tagDrwaLotteryPeriod.strBeginIssueNumber));
	while (Fetch())
	{
		if ((tagDrwaLotteryPeriod.iTypeID != 0) && (strlen(tagDrwaLotteryPeriod.strCurrentIssueNumber) > 0))
		{
			queueDrawLotteryItems.push(tagDrwaLotteryPeriod);
			memset(&tagDrwaLotteryPeriod, 0, sizeof(tagDrwaLotteryPeriod));
		}
	}
	ClearMoreResults();

	return !queueDrawLotteryItems.empty();
}

bool LotteryDB::Ex_GetLotteryUserOrders(DRAW_LOTTERY_PERIOD drawLotteryInfo, LOTTERY_ORDER_DATA& lotteryOrderData)
{
	CTicker timeLapser("sp_GetLotteryUserOrders");
	bool bResult = false;

	ClearMoreResults();
	InitBindParam();
	BindParam(drawLotteryInfo.iUserControled);
	BindParam(drawLotteryInfo.iTypeID);
	BindParamVarChar(drawLotteryInfo.strBeginIssueNumber, ISSUE_NUMBER_LEN);
	BindParamVarChar(drawLotteryInfo.strCurrentIssueNumber, ISSUE_NUMBER_LEN);
	BindParamVarChar(drawLotteryInfo.strLastIssueNumber, ISSUE_NUMBER_LEN);
	bResult = ExecuteDirect(TEXT("{call dbo.sp_GetLotteryUserOrders(?,?,?,?,?)}"));
	if (!bResult)
	{
		printf("sp_GetLotteryUserOrders failed ... \n");
		return false;
	}
	InitBindCol();
	BindCol(lotteryOrderData.uiAllBet);
	BindCol(lotteryOrderData.uiAllBetAsOfLast);
	BindCol(lotteryOrderData.uiBonusAlready);
	BindCol(lotteryOrderData.fWinRateAsOfLast);
	Fetch();//if (IsFetchNoData())

	PLAYER_ORDERS tagPlayerOrders;
	if (Fetch())
	{
		InitBindCol();
		BindCol(tagPlayerOrders.iTypeID);
		BindCol(tagPlayerOrders.strIssueNumber, sizeof(tagPlayerOrders.strIssueNumber));
		BindCol(tagPlayerOrders.strSelectType, sizeof(tagPlayerOrders.strSelectType));
		BindCol(tagPlayerOrders.uiTotalBonus);
		while (Fetch())
		{
			lotteryOrderData.vecPlayerOrders.push_back(tagPlayerOrders);
			memset(&tagPlayerOrders, 0, sizeof(tagPlayerOrders));
		}
	}
	memset(&tagPlayerOrders, 0, sizeof(tagPlayerOrders));
	if (Fetch())
	{
		InitBindCol();
		BindCol(tagPlayerOrders.iTypeID);
		BindCol(tagPlayerOrders.strIssueNumber, sizeof(tagPlayerOrders.strIssueNumber));
		BindCol(tagPlayerOrders.strSelectType, sizeof(tagPlayerOrders.strSelectType));
		BindCol(tagPlayerOrders.uiTotalBonus);
		while (Fetch())
		{
			lotteryOrderData.vecControlUserOrders.push_back(tagPlayerOrders);
			memset(&tagPlayerOrders, 0, sizeof(tagPlayerOrders));
		}
	}

	ClearMoreResults();

	return true;
}

bool LotteryDB::Ex_UpdateGameResult(LOTTERY_RESULT lotteryResult)
{
	CTicker timeLapser("sp_UpdateLotteryResult");
	bool bResult = false;

	ClearMoreResults();
	InitBindParam();
	BindParam(lotteryResult.iTypeID);
	BindParamVarChar(lotteryResult.strIssueNumber, ISSUE_NUMBER_LEN);
	BindParamVarChar(lotteryResult.strLotteryNumber, NUMBER_LEN);
	BindParamVarChar(lotteryResult.strLotteryColor, COLOR_LEN);
	BindParam(lotteryResult.iControlType);
	bResult = ExecuteDirect(TEXT("{call dbo.sp_UpdateLotteryResult(?,?,?,?,?)}"));
	if (!bResult)
	{
		printf("sp_GetLotteryUserOrders failed ... \n");
		return false;
	}

	return true;
}
