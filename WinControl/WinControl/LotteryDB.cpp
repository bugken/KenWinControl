#include "stdafx.h"
#include "LotteryDB.h"

bool LotteryDB::DBConnect()
{
	bool ret = false;
	char pStr[100] = "DRIVER={SQL Server Native Client 10.0};SERVER=127.0.0.1;DATABASE=9lottery;UID=sa;PWD=123456;";
	bool IsConnect = DriverConnect(pStr);
	if (IsConnect)
	{
		printf("���ӳɹ�\n");
		ret = true;
	}
	return ret;
}

bool LotteryDB::Ex_GetDrawLottery(DRAW_LOTTERY_PERIOD_QUEUE& queueDrawLotteryItems)
{

	return true;
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