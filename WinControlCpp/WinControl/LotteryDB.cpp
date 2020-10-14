#include "stdafx.h"
#include "LotteryDB.h"
#include "LogFile.h"
#include "LotteryStatistic.h"

bool LotteryDB::DBConnect()
{
	bool ret = false;
	char pStr[100] = "DRIVER={SQL Server Native Client 10.0};SERVER=47.242.25.101;DATABASE=92lottery;UID=sa;PWD=Jack7361;";
	bool IsConnect = DriverConnect(pStr);
	if (IsConnect)
	{
		GetLogFileHandle().InfoLog("thread(%d) connect lottery db succes\n", GetCurrentThreadId());
		printf("thread(%d) connect lottery db succes\n", GetCurrentThreadId());
		ret = true;
	}
	else
	{
		GetLogFileHandle().ErrorLog("thread(%d) connect lottery db failed\n", GetCurrentThreadId());
		printf("thread(%d) connect lottery db failed\n", GetCurrentThreadId());
		ret = false;
	}
	return ret;
}

bool LotteryDB::Ex_GetDrawLottery(DRAW_LOTTERY_PERIOD_QUEUE& queueDrawLotteryItems)
{
	CTicker timeLapser("Ex_GetDrawLottery");
	bool bResult = false;
	int iError = 0;
	char errstr[2048] = { 0 };
	DRAW_LOTTERY_PERIOD tagDrwaLotteryPeriod;

	ClearMoreResults();
	InitBindParam();
	BindParam(iError, SQL_PARAM_OUTPUT);
	bResult = ExecuteDirect(TEXT("{? = call dbo.sp_GetDrawLotteryInfo}"), errstr);
	if (iError || !bResult)
	{
		GetLogFileHandle().ErrorLog("%s %d Error[%d] Result[%d] sp_GetDrawLotteryInfo error [%s]\n", \
			__FUNCTION__, __LINE__, iError, bResult, errstr);
		return false;
	}
	//if (IsFetchNoData())//有数据的时候再去取数据
	{
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
	}
	ClearMoreResults();

	return !queueDrawLotteryItems.empty();
}

bool LotteryDB::Ex_GetLotteryUserOrders(DRAW_LOTTERY_PERIOD drawLotteryInfo, LOTTERY_ORDER_DATA& lotteryOrderData)
{
	CTicker timeLapser("Ex_GetLotteryUserOrders");
	bool bResult = false;
	int iError = 0;
	char errstr[2048] = { 0 };

	ClearMoreResults();
	InitBindParam();
	BindParam(iError, SQL_PARAM_OUTPUT);
	BindParam(drawLotteryInfo.iUserControled);
	BindParam(drawLotteryInfo.iTypeID);
	BindParamVarChar(drawLotteryInfo.strBeginIssueNumber, ISSUE_NUMBER_LEN);
	BindParamVarChar(drawLotteryInfo.strCurrentIssueNumber, ISSUE_NUMBER_LEN);
	BindParamVarChar(drawLotteryInfo.strLastIssueNumber, ISSUE_NUMBER_LEN);
	bResult = ExecuteDirect(TEXT("{? = call dbo.sp_GetLotteryUserOrders(?,?,?,?,?)}"), errstr);
	if (iError || !bResult)
	{
		GetLogFileHandle().ErrorLog("%s %d Error[%d] Result[%d] sp_GetLotteryUserOrders error [%s]\n", \
			__FUNCTION__, __LINE__, iError, bResult, errstr);
		return false;
	}
	//if (IsFetchNoData())//有数据的时候再去取数据
	{
		InitBindCol();
		BindCol(lotteryOrderData.uiUsersBetCounts);
		BindCol(lotteryOrderData.uiAllBet);
		BindCol(lotteryOrderData.uiAllBetAsOfLast);
		BindCol(lotteryOrderData.uiBonusAlready);
		BindCol(lotteryOrderData.fWinRateAsOfLast);
		Fetch();

		CONTROLED_USER_ORDERS tagControledUserOrders;
		ORDERS_TEN_RESULTS tagOrdersTenResults;
		if (Fetch())
		{
			InitBindCol();
			BindCol(tagOrdersTenResults.iTypeID);
			BindCol(tagOrdersTenResults.strIssueNumber, sizeof(tagOrdersTenResults.strIssueNumber));
			BindCol(tagOrdersTenResults.strSelectNumber, sizeof(tagOrdersTenResults.strSelectNumber));
			BindCol(tagOrdersTenResults.strSelectColor, sizeof(tagOrdersTenResults.strSelectColor));
			BindCol(tagOrdersTenResults.uiAllTotalBonus);
			BindCol(tagOrdersTenResults.fWinRate);
			while (Fetch())
			{
				lotteryOrderData.vecLottery10Results.push_back(tagOrdersTenResults);
				memset(&tagOrdersTenResults, 0, sizeof(tagOrdersTenResults));
			}
		}
		if (drawLotteryInfo.iUserControled > 0)
		{
			if (Fetch())
			{
				InitBindCol();
				BindCol(tagControledUserOrders.iTypeID);
				BindCol(tagControledUserOrders.strIssueNumber, sizeof(tagControledUserOrders.strIssueNumber));
				BindCol(tagControledUserOrders.strSelectType, sizeof(tagControledUserOrders.strSelectType));
				BindCol(tagControledUserOrders.uiTotalBonus);
				while (Fetch())
				{
					lotteryOrderData.vecControlUserOrders.push_back(tagControledUserOrders);
					memset(&tagControledUserOrders, 0, sizeof(tagControledUserOrders));
				}
			}
		}
	}
	ClearMoreResults();

	return true;
}

bool LotteryDB::Ex_UpdateGameResult(LOTTERY_RESULT lotteryResult)
{
	CTicker timeLapser("Ex_UpdateGameResult");
	bool bResult = false;
	int iError = 0;
	char errstr[2048] = { 0 };

	ClearMoreResults();
	InitBindParam();
	BindParam(iError, SQL_PARAM_OUTPUT);
	BindParam(lotteryResult.iTypeID);
	BindParamVarChar(lotteryResult.strIssueNumber, ISSUE_NUMBER_LEN);
	BindParamVarChar(lotteryResult.strLotteryNumber, NUMBER_LEN);
	BindParamVarChar(lotteryResult.strLotteryColor, COLOR_LEN);
	BindParam(lotteryResult.iControlType);
	bResult = ExecuteDirect(TEXT("? = {call dbo.sp_UpdateLotteryResult(?,?,?,?,?)}"), errstr);
	if (iError || !bResult)
	{
		GetLogFileHandle().ErrorLog("%s %d Error[%d] Result[%d] sp_GetLotteryUserOrders error [%s]\n",\
			__FUNCTION__, __LINE__, iError, bResult, errstr);
		return false;
	}

	return true;
}
