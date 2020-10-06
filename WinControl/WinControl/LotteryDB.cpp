#include "stdafx.h"
#include "LotteryDB.h"

bool LotteryDB::DBConnect()
{
	bool ret = false;
	char pStr[100] = "DRIVER={SQL Server Native Client 10.0};SERVER=47.56.134.27;DATABASE=9lottery;UID=sa;PWD=Jack7361;";
	bool IsConnect = DriverConnect(pStr);
	if (IsConnect)
	{
		printf("连接成功\n");
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
	BindCol(tagDrwaLotteryPeriod.uiTypeID); 
	BindCol(tagDrwaLotteryPeriod.uiUserControled); 
	BindCol(tagDrwaLotteryPeriod.uiControlRate); 
	BindCol(tagDrwaLotteryPeriod.uiPowerControl); 
	BindCol(tagDrwaLotteryPeriod.strCurrentIssueNumber, sizeof(tagDrwaLotteryPeriod.strCurrentIssueNumber));
	BindCol(tagDrwaLotteryPeriod.strLastIssueNumber, sizeof(tagDrwaLotteryPeriod.strLastIssueNumber));
	BindCol(tagDrwaLotteryPeriod.strBeginIssueNumber, sizeof(tagDrwaLotteryPeriod.strBeginIssueNumber));
	while (Fetch())
	{
		if ((tagDrwaLotteryPeriod.uiTypeID != 0) && (strlen(tagDrwaLotteryPeriod.strCurrentIssueNumber) > 0))
		{
			queueDrawLotteryItems.push(tagDrwaLotteryPeriod);
			memset(&tagDrwaLotteryPeriod, 0, sizeof(tagDrwaLotteryPeriod));
		}
	}
	ClearMoreResults();

	return !queueDrawLotteryItems.empty();
}

bool LotteryDB::Ex_GetLotteryOrders(DRAW_LOTTERY_PERIOD tagDrawLotteryInfo, 
		PLAYER_ORDERS_VEC& tagPlayerOrdersVec, 
		PLAYER_ORDERS_VEC& tagControlUserOrdersVec)
{

	return true;
}

bool LotteryDB::Ex_UpdateGameResult()
{

	return true;
}

/*
bool LotteryDB::Ex_ReadOrder(ORDER_INFO_VEC& vecOrderInfo)
{
	CTicker timeLapser("xp_ReadOrder");
	bool bResult = false;
	ORDER_INFO tagOrderInfo = { 0 };
	ClearMoreResults();
	InitBindParam();

	bResult = ExecuteDirect(TEXT("{call dbo.sp_ReadOrder}"));
	if (!bResult)
	{
		printf("xp_ReadOrder failed ... \n");
		return false;
	}
	InitBindCol();
	BindCol(tagOrderInfo.uiUserID);
	BindCol(tagOrderInfo.cIssueNumber, sizeof(tagOrderInfo.cIssueNumber));
	BindCol(tagOrderInfo.uiTypeID);
	BindCol(tagOrderInfo.cSelectType, sizeof(tagOrderInfo.cSelectType));
	BindCol(tagOrderInfo.fRealAmount);
	while (Fetch())
	{
		vecOrderInfo.push_back(tagOrderInfo);
		memset(&tagOrderInfo, 0, sizeof(tagOrderInfo));
	}
	ClearMoreResults();

	return true;
}
*/