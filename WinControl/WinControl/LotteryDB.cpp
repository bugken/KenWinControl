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
	DRAW_LOTTERY_PERIOD tagDrwaLotteryPeriod = { 0 };

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

bool LotteryDB::Ex_GetLotteryUserOrders(DRAW_LOTTERY_PERIOD tagDrawLotteryInfo,
		UINT64& uiAllBet, UINT64& uiAllBetAsOfLast, UINT64& uiBonusAlready, float& fWinRateAsOfLast,
		PLAYER_ORDERS_VEC& PlayerOrdersVec, 
		PLAYER_ORDERS_VEC& ControlUserOrdersVec)
{
	CTicker timeLapser("sp_GetLotteryUserOrders");
	bool bResult = false;
	DRAW_LOTTERY_PERIOD tagDrwaLotteryPeriod = { 0 };

	ClearMoreResults();
	InitBindParam();
	BindParam(tagDrawLotteryInfo.iUserControled);
	BindParam(tagDrawLotteryInfo.iTypeID);
	BindParamVarChar(tagDrawLotteryInfo.strBeginIssueNumber, ISSUE_NUMBER_LEN);
	BindParamVarChar(tagDrawLotteryInfo.strCurrentIssueNumber, ISSUE_NUMBER_LEN);
	BindParamVarChar(tagDrawLotteryInfo.strLastIssueNumber, ISSUE_NUMBER_LEN);
	bResult = ExecuteDirect(TEXT("{call dbo.sp_GetLotteryUserOrders(?,?,?,?,?)}"));
	if (!bResult)
	{
		printf("sp_GetLotteryUserOrders failed ... \n");
		return false;
	}
	InitBindCol();
	BindCol(uiAllBet);
	BindCol(uiAllBetAsOfLast);
	BindCol(uiBonusAlready);
	BindCol(fWinRateAsOfLast);
	Fetch();//if (IsFetchNoData())

	PLAYER_ORDERS tagPlayerOrders = {0};
	if (Fetch())
	{
		InitBindCol();
		BindCol(tagPlayerOrders.iTypeID);
		BindCol(tagPlayerOrders.strIssueNumber, sizeof(tagPlayerOrders.strIssueNumber));
		BindCol(tagPlayerOrders.strSelectType, sizeof(tagPlayerOrders.strSelectType));
		BindCol(tagPlayerOrders.uiTotalBonus);
		while (Fetch())
		{
			PlayerOrdersVec.push_back(tagPlayerOrders);
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
			ControlUserOrdersVec.push_back(tagPlayerOrders);
			memset(&tagPlayerOrders, 0, sizeof(tagPlayerOrders));
		}
	}

	ClearMoreResults();

	return true;
}

bool LotteryDB::Ex_UpdateGameResult(LOTTERY_RESULT tagLotteryResult)
{

	return true;
}
