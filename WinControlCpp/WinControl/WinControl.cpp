#include "stdafx.h"
#include "Common.h"
#include "LotteryDB.h"
#include "LogFile.h"
#include "LotteryStatistic.h"
#include <iostream>
#include <thread>
#include <algorithm>

using namespace std;

extern CLotteryStatistic gLotteryStatistic;

MUTEX LotteryMutex;
DRAW_LOTTERY_PERIOD_QUEUE DrawLotteryQueue;
CONDITION_VARIABLE LotteryConditionVariable;

//TODO:对连续赢若干局的玩家进行单杀
/*
计算整体派彩的思路:彩票最终会生成10种结果,将所有下注的派彩整合到这个10个结果,然后按照赢率排序
计算单杀用户赢得最多派彩的思路:其与整体派彩思路相反,玩家下注已知,在玩家下注基础上,把下注对应的结果产生的派彩都加到下注上,这样就可以选出哪个下注赢钱最多,把赢钱最多的下注杀掉
计算单杀用户思路和计算整体派彩思路不同，如果按照整体派彩思路一样,那么会有这种情况:0下注300 5下注400 小下注200,然后5被杀,但实际上应该杀0,0这个结果上的派彩是300+200=500
*/
bool GetControlUserMostBonusBet(CONTROLED_USER_ORDERS_VEC vecControlUserOrders, char* strControlUserMostBonusBet)
{
	CTicker timeLapser("GetControlUserMostBonusBet");
	char strMaxSelectTypeCurr[BUFF64] = { 0 };
	UINT64 uiSumBonus = 0, uiBingoTotalBonus = 0;
	UINT64 uiMaxTotalBonusCurr = 0, uiMaxTotalBonusLast = 0;
	for (auto userOrder : vecControlUserOrders)
	{
		uiSumBonus = 0;
		uiMaxTotalBonusCurr = 0;
		memset(strMaxSelectTypeCurr, 0, sizeof(strMaxSelectTypeCurr));
		if (userOrder.strSelectType == "0")
		{
			FOR_CONTENT_BEGIN()
				IF_CONDITION_VAR4("0", "red", "violet", "small")
			FOR_CONTENT_END()
		}
		else if (userOrder.strSelectType == "1")
		{
			FOR_CONTENT_BEGIN()
				IF_CONDITION_VAR3("1", "green", "small")
			FOR_CONTENT_END()
		}
		else if (userOrder.strSelectType == "2")
		{
			FOR_CONTENT_BEGIN()
				IF_CONDITION_VAR3("2", "red", "small")
			FOR_CONTENT_END()
		}
		else if (userOrder.strSelectType == "3")
		{
			FOR_CONTENT_BEGIN()
				IF_CONDITION_VAR3("3", "green", "small")
			FOR_CONTENT_END()
		}
		else if (userOrder.strSelectType == "4")
		{
			FOR_CONTENT_BEGIN()
				IF_CONDITION_VAR3("4", "red", "small")
			FOR_CONTENT_END()
		}
		else if (userOrder.strSelectType == "5")
		{
			FOR_CONTENT_BEGIN()
				IF_CONDITION_VAR4("5", "green", "violet", "big")
			FOR_CONTENT_END()
		}
		else if (userOrder.strSelectType == "6")
		{
			FOR_CONTENT_BEGIN()
				IF_CONDITION_VAR3("6", "red", "big")
			FOR_CONTENT_END()
		}
		else if (userOrder.strSelectType == "7")
		{
			FOR_CONTENT_BEGIN()
				IF_CONDITION_VAR3("7", "green", "big")
			FOR_CONTENT_END()
		}
		else if (userOrder.strSelectType == "8")
		{
			FOR_CONTENT_BEGIN()
				IF_CONDITION_VAR3("8", "red", "big")
			FOR_CONTENT_END()
		}
		else if (userOrder.strSelectType == "9")
		{
			FOR_CONTENT_BEGIN()
				IF_CONDITION_VAR3("9", "green", "big")
			FOR_CONTENT_END()
		}
		else if (userOrder.strSelectType == "small")
		{
			FOR_CONTENT_BEGIN()
				IF_CONDITION_VAR6("small", "0", "1", "2", "3", "4")
			FOR_CONTENT_END()
		}
		else if (userOrder.strSelectType == "big")
		{
			FOR_CONTENT_BEGIN()
				IF_CONDITION_VAR6("big", "5", "6", "7", "8", "9")
			FOR_CONTENT_END()
		}
		else if (userOrder.strSelectType == "red")
		{//这里有待完善:@SumBonus应该算('0', '2', '4', '6', '8')与'red'组合中最大的一个,目前这个写法只会多杀,不会少杀
			FOR_CONTENT_BEGIN()
				IF_CONDITION_VAR6("red", "0", "2", "4", "6", "8")
			FOR_CONTENT_END()
		}
		else if (userOrder.strSelectType == "green")
		{//这里有待完善:@SumBonus应该算('1', '3', '5', '7', '9')与'green'组合中最大的一个,目前这个写法只会多杀,不会少杀
			FOR_CONTENT_BEGIN()
				IF_CONDITION_VAR6("green", "1", "3", "5", "7", "9")
			FOR_CONTENT_END()
		}
		else if (userOrder.strSelectType == "violet")
		{//紫色分红紫和绿紫
			UINT64 uiRedVioletBonus = 0, uiGreenVioletBonus = 0;
			UINT64 MaxBonusRedViolet = 0, MaxBonusGreenViolet = 0;
			for (auto order : vecControlUserOrders)
			{
				if (order.strSelectType == "red" || order.strSelectType == "0" || order.strSelectType == "violet")
				{
					uiRedVioletBonus += order.uiTotalBonus;
					if (order.uiTotalBonus > MaxBonusRedViolet)
					{
						MaxBonusRedViolet = order.uiTotalBonus;
						strcpy_s(strMaxSelectTypeCurr, order.strSelectType);
					}
				}
			}
			for (auto order : vecControlUserOrders)
			{
				if (order.strSelectType == "green" || order.strSelectType == "5" || order.strSelectType == "violet")
				{
					uiGreenVioletBonus += order.uiTotalBonus;
					if (order.uiTotalBonus > MaxBonusGreenViolet)
					{
						MaxBonusGreenViolet = order.uiTotalBonus;
						strcpy_s(strMaxSelectTypeCurr, order.strSelectType);
					}
				}
			}
			if (uiRedVioletBonus > uiGreenVioletBonus)
			{
				uiSumBonus = uiRedVioletBonus;
				uiMaxTotalBonusCurr = MaxBonusRedViolet;
				strcpy_s(strMaxSelectTypeCurr, "0");
			}
			else if (uiRedVioletBonus < uiGreenVioletBonus)
			{
				uiSumBonus = uiGreenVioletBonus;
				uiMaxTotalBonusCurr = MaxBonusGreenViolet;
				strcpy_s(strMaxSelectTypeCurr, "5");
			}
			else if (uiRedVioletBonus == uiGreenVioletBonus)
			{
				uiSumBonus = uiGreenVioletBonus;
				if (MaxBonusRedViolet > MaxBonusGreenViolet)
				{
					uiMaxTotalBonusCurr = MaxBonusRedViolet;
					strcpy_s(strMaxSelectTypeCurr, "0");
				}
				else if (MaxBonusRedViolet < MaxBonusGreenViolet)
				{
					uiMaxTotalBonusCurr = MaxBonusGreenViolet;
					strcpy_s(strMaxSelectTypeCurr, "5");
				}
				else if (MaxBonusRedViolet == MaxBonusGreenViolet)
				{
					uiMaxTotalBonusCurr = MaxBonusRedViolet;
					strcpy_s(strMaxSelectTypeCurr, "violet");
				}
			}
		}
		//每次循环判断一次选取最大的下注类型
		if (uiSumBonus > uiBingoTotalBonus)
		{
			uiBingoTotalBonus = uiSumBonus;
			uiMaxTotalBonusLast = uiMaxTotalBonusCurr;
			memcpy(strControlUserMostBonusBet, strMaxSelectTypeCurr, BUFF64);
		}
		else if (uiSumBonus == uiBingoTotalBonus)
		{
			if (uiMaxTotalBonusCurr > uiMaxTotalBonusLast)
			{
				memcpy(strControlUserMostBonusBet, strMaxSelectTypeCurr, BUFF64);
			}
		}
	}
	return true;
}

bool DeleteUserControlType(ORDERS_TEN_RESULTS_VEC& lottery10Results, const char* strControlUserMostBonusBet)
{
	CTicker timeLapser("DeleteUserControlType");
	//if中嵌套循环，效率稍高，编译器可对循环优化
	if (strControlUserMostBonusBet == "0" || strControlUserMostBonusBet == "1" || strControlUserMostBonusBet == "2"\
		|| strControlUserMostBonusBet == "3" || strControlUserMostBonusBet == "4" || strControlUserMostBonusBet == "5"\
		|| strControlUserMostBonusBet == "6" || strControlUserMostBonusBet == "7" || strControlUserMostBonusBet == "8"\
		|| strControlUserMostBonusBet == "9")
	{
		for (ORDERS_TEN_RESULTS_VEC_IT it = lottery10Results.begin(); it != lottery10Results.end(); ++it)
		{
			if (it->strIssueNumber == strControlUserMostBonusBet)
			{
				it = lottery10Results.erase(it);
			}
		}
	}
	else if (strControlUserMostBonusBet == "big")
	{
		for (ORDERS_TEN_RESULTS_VEC_IT it = lottery10Results.begin(); it != lottery10Results.end(); ++it)
		{
			if (it->strIssueNumber == "5" || it->strIssueNumber == "6" || it->strIssueNumber == "7"\
				|| it->strIssueNumber == "8" || it->strIssueNumber == "9")
			{
				it = lottery10Results.erase(it);
			}
		}
	}
	else if (strControlUserMostBonusBet == "small")
	{
		for (ORDERS_TEN_RESULTS_VEC_IT it = lottery10Results.begin(); it != lottery10Results.end(); ++it)
		{
			if (it->strIssueNumber == "0" || it->strIssueNumber == "1" || it->strIssueNumber == "2"\
				|| it->strIssueNumber == "3" || it->strIssueNumber == "4")
			{
				it = lottery10Results.erase(it);
			}
		}
	}
	else if (strControlUserMostBonusBet == "red")
	{
		for (ORDERS_TEN_RESULTS_VEC_IT it = lottery10Results.begin(); it != lottery10Results.end(); ++it)
		{
			if (it->strIssueNumber == "0" || it->strIssueNumber == "2" || it->strIssueNumber == "4"\
				|| it->strIssueNumber == "6" || it->strIssueNumber == "8")
			{
				it = lottery10Results.erase(it);
			}
		}
	}
	else if (strControlUserMostBonusBet == "green")
	{
		for (ORDERS_TEN_RESULTS_VEC_IT it = lottery10Results.begin(); it != lottery10Results.end(); ++it)
		{
			if (it->strIssueNumber == "1" || it->strIssueNumber == "3" || it->strIssueNumber == "5"\
				|| it->strIssueNumber == "7" || it->strIssueNumber == "9")
			{
				it = lottery10Results.erase(it);
			}
		}
	}
	else if (strControlUserMostBonusBet == "violet")
	{
		for (ORDERS_TEN_RESULTS_VEC_IT it = lottery10Results.begin(); it != lottery10Results.end(); ++it)
		{
			if (it->strIssueNumber == "0" || it->strIssueNumber == "5")
			{
				it = lottery10Results.erase(it);
			}
		}
	}

	return true;
}

void SetLotteryResult(const ORDERS_TEN_RESULTS Result, LOTTERY_RESULT& lotteryResult)
{
	lotteryResult.iTypeID = Result.iTypeID;
	strcpy_s(lotteryResult.strIssueNumber, Result.strIssueNumber);
	strcpy_s(lotteryResult.strLotteryNumber, Result.strSelectNumber);
	strcpy_s(lotteryResult.strLotteryColor, Result.strSelectColor);
}
/*
控制类型
1:单杀,强拉,上拉	2:单杀,强拉,下拉	3:未单杀,强拉,上拉 
4:未单杀,强拉,下拉	5:单杀,弱拉,上拉	6:未单杀,弱拉,上拉 
7:单杀,弱拉,下拉	8:未单杀,弱拉,下拉	9:保持用户赢率为定值
控制力度
0:赢率控制在ControlRate附近 1:强拉回 2:弱拉回
*/
bool GetLotteryFinalResult(ORDERS_TEN_RESULTS_VEC lottery10ResultsVec, float fWinRateAsOfLast, 
		bool bUserControled, UINT32 iControlRate, UINT32 iPowerControl, LOTTERY_RESULT& lotteryResult)
{
	CTicker timeLapser("GetLotteryFinalResult");
	UINT32 iSteps = 0;
	UINT32 iVecSize = lottery10ResultsVec.size();
	float fTargetWinRate = (float)(iControlRate * 1.0 / 10000);
	sort(lottery10ResultsVec.begin(), lottery10ResultsVec.end(), DescSort);

	GetLogFileHandle().InfoLog("%s %d TargetWinRate:%f, totally %d results as bellow:\n", __FUNCTION__, __LINE__, fTargetWinRate, iVecSize);
	for (auto result : lottery10ResultsVec)
	{
		GetLogFileHandle().InfoLog("TypeID:%d, IssueNumber:%s, SelectNumber:%s, SelectColor:%s, AllTotalBonus:%I64u, WinRate:%f\n", \
			result.iTypeID, result.strIssueNumber, result.strSelectNumber, \
			result.strSelectColor, result.uiAllTotalBonus, result.fWinRate);
	}

	if (CONTROL_POWER_STRONG == iPowerControl)//强拉
	{
		if (fWinRateAsOfLast <= fTargetWinRate)
		{//强制上拉取第一个元素
			SetLotteryResult(lottery10ResultsVec.front(), lotteryResult);
			bUserControled ? lotteryResult.iControlType = CONTROL_TYPE_USER_STRONG_UP:
				lotteryResult.iControlType = CONTROL_TYPE_NONE_USER_STRONG_UP;
		}
		else
		{//强制下拉取最后一个元素
			SetLotteryResult(lottery10ResultsVec.back(), lotteryResult);
			bUserControled ? lotteryResult.iControlType = CONTROL_TYPE_USER_STRONG_DOWN 
				: lotteryResult.iControlType = CONTROL_TYPE_NONE_USER_STRONG_DOWN;
		}
	}
	else if (CONTROL_POWER_WEAK == iPowerControl)//弱拉
	{
		if (fWinRateAsOfLast <= fTargetWinRate)//弱上拉
		{
			UINT32 iBingoCounts = 0;
			//弱上拉先取第一个值，存在所有赢率都比目标赢率小的情况
			SetLotteryResult(lottery10ResultsVec.front(), lotteryResult);
			bUserControled ? lotteryResult.iControlType = CONTROL_TYPE_USER_WEAK_UP
				: lotteryResult.iControlType = CONTROL_TYPE_NONE_USER_WEAK_UP;
			for (auto result : lottery10ResultsVec)
			{
				if (result.fWinRate > fTargetWinRate)
				{
					iBingoCounts++;
					SetLotteryResult(result, lotteryResult);
				}
				//遍历到第三个大的值或结束为止
				if ((3 == iBingoCounts) || (++iSteps == iVecSize))
					break;
			}
		}
		else//弱下拉
		{
			bool bIsFound = false;//是否继续找小值的位置
			bUserControled ? lotteryResult.iControlType = CONTROL_TYPE_USER_WEAK_DOWN
				: lotteryResult.iControlType = CONTROL_TYPE_NONE_USER_WEAK_DOWN;
			for (auto result : lottery10ResultsVec)
			{
				++iSteps;
				if (!bIsFound && (result.fWinRate < fTargetWinRate))
				{
					bIsFound = true;
					if (iSteps >= iVecSize - 2)
					{
						SetLotteryResult(result, lotteryResult);
						break;
					}
				}
				//1.找到最小值,遍历到倒数第三个 2.没有找到最小值,遍历到最后一个
				if ((bIsFound && (iSteps == iVecSize - 2)) ||
					(!bIsFound && iSteps == iVecSize))
				{
					SetLotteryResult(result, lotteryResult);
					break;
				}
			}
		}
	}
	else if (CONTROL_POWER_FIX_WIN_RATE == iPowerControl)//保持赢率在设定值
	{
		lotteryResult.iControlType = CONTROL_TYPE_FIX_WIN_RATE;
		for (auto result : lottery10ResultsVec)
		{
			if ((++iSteps == iVecSize) || (result.fWinRate <= fTargetWinRate))
			{
				SetLotteryResult(result, lotteryResult);
				break;
			}
		}
	}

	return true;
}

bool ProcessControledUserOrder(LOTTERY_ORDER_DATA& lotteryOrderData, bool& bUserControled)
{
	CTicker timeLapser("ProcessControledUserOrder");
	char strControlUserMostBonusBet[BUFF64] = {0};
	if (lotteryOrderData.vecControlUserOrders.size() > 0)
	{
		bUserControled = GetControlUserMostBonusBet(lotteryOrderData.vecControlUserOrders, strControlUserMostBonusBet);
	}
	else
	{
		GetLogFileHandle().InfoLog("%s %d no controled user.\n", __FUNCTION__, __LINE__);
		return true;
	}

	if (bUserControled)
	{
		GetLogFileHandle().InfoLog("%s %d ControlUserMostBonusBet %s deleted in lottery 10 results.\n",
				__FUNCTION__, __LINE__, strControlUserMostBonusBet);
		DeleteUserControlType(lotteryOrderData.vecLottery10Results, strControlUserMostBonusBet);
	}

	return true;
}

void SetLogConf()
{
	char szWorkDir[MAX_PATH] = { 0 };
	char szLogBackupDir[MAX_PATH] = { 0 };
	GetCurrentWorkDir(szWorkDir, MAX_PATH);
	UINT32 iRet = snprintf(szLogBackupDir, sizeof(szLogBackupDir) - 1, "%s\\LogBackupDir", szWorkDir);
	szLogBackupDir[iRet] = '\0';
	GetLogFileHandle().SetLogNameByDay("Lottery");
	GetLogFileHandle().SetStatNameByDay("Statistic");
	GetLogFileHandle().SetLogPath(szWorkDir); 
	GetLogFileHandle().SetBakLogPath(szLogBackupDir);
}

void LotteryProcessWorker()
{
	printf("work thread id: %d\n", GetCurrentThreadId());
	LotteryDB lotteryDB;
	lotteryDB.DBConnect();
	while (true)
	{
		DRAW_LOTTERY_PERIOD tagDrawLotteryInfo;
		{
			std::unique_lock<std::mutex> LotteryLock(LotteryMutex);
			LotteryConditionVariable.wait(LotteryLock, []{return !DrawLotteryQueue.empty();});
			tagDrawLotteryInfo = DrawLotteryQueue.front();
			DrawLotteryQueue.pop();
			LotteryLock.unlock();
		}
		GetLogFileHandle().InfoLog("%s %d lottery worker process(%d) begin\n", __FUNCTION__, __LINE__, GetCurrentThreadId());
		GetLogFileHandle().InfoLog("TypeID:%d, UserControled:%d, ControlRate:%d, PowerControl:%d, CurrentIssueNumber:%s, LastIssueNumber:%s, BeginIssueNumber:%s\n", \
			tagDrawLotteryInfo.iTypeID, tagDrawLotteryInfo.iUserControled, tagDrawLotteryInfo.iControlRate, \
			tagDrawLotteryInfo.iPowerControl, tagDrawLotteryInfo.strCurrentIssueNumber, \
			tagDrawLotteryInfo.strLastIssueNumber, tagDrawLotteryInfo.strBeginIssueNumber);

		LOTTERY_ORDER_DATA tagLotteryOrderData;
		LOTTERY_RESULT tagLotteryResult;
		bool bUserControled = false;//是否单控
		UINT32 uiRetID = 0;

		bool bResult = lotteryDB.Ex_GetLotteryUserOrders(tagDrawLotteryInfo, tagLotteryOrderData);
		if (bResult && tagLotteryOrderData.uiUsersBetCounts >= 1)
		{
			bResult = ProcessControledUserOrder(tagLotteryOrderData, bUserControled);
		}
		else
		{
			GetLogFileHandle().InfoLog("TypeID:%d, CurrentIssueNumber:%s bet users counts:%d < 5\n", \
				tagDrawLotteryInfo.iTypeID, tagDrawLotteryInfo.strCurrentIssueNumber, tagLotteryOrderData.uiUsersBetCounts);
			GetLogFileHandle().InfoLog("%s %d lottery worker process(%d) end\n", __FUNCTION__, __LINE__, GetCurrentThreadId());
			continue;
		}
			
		if (bResult)
		{
			bResult = GetLotteryFinalResult(tagLotteryOrderData.vecLottery10Results,
				tagLotteryOrderData.fWinRateAsOfLast, bUserControled, tagDrawLotteryInfo.iControlRate,
				tagDrawLotteryInfo.iPowerControl, tagLotteryResult);
		}
		if (bResult)
		{
			GetLogFileHandle().InfoLog("game final result as bellow:\n");
			GetLogFileHandle().InfoLog("TypeID:%d, IssueNumber:%s, LotteryNumber:%s, LotteryColor:%s, ControlType:%d\n", \
				tagLotteryResult.iTypeID, tagLotteryResult.strIssueNumber, tagLotteryResult.strLotteryNumber, \
				tagLotteryResult.strLotteryColor, tagLotteryResult.iControlType);
			lotteryDB.Ex_UpdateGameResult(tagLotteryResult, uiRetID);
			if (1 == uiRetID)
			{
				GetLogFileHandle().InfoLog("TypeID:%d, IssueNumber:%s, %s", \
					tagLotteryResult.iTypeID, tagLotteryResult.strIssueNumber, "控制开关未开启");
			}
			else if (2 == uiRetID)
			{
				GetLogFileHandle().InfoLog("TypeID:%d, IssueNumber:%s, %s", \
					tagLotteryResult.iTypeID, tagLotteryResult.strIssueNumber, "单控开关未开启");
			}
			else if (3 == uiRetID)
			{
				GetLogFileHandle().InfoLog("TypeID:%d, IssueNumber:%s, %s", \
					tagLotteryResult.iTypeID, tagLotteryResult.strIssueNumber, "未在更新时间内或已经预设或已经开奖");
			}
		}
		GetLogFileHandle().InfoLog("%s %d lottery worker process(%d) end\n", __FUNCTION__, __LINE__, GetCurrentThreadId());
	}
}

bool IsDrawLotterySecond()
{
	bool bIsDrawing = false;
	struct tm stTempTm;
	char cSecond[20];
	static time_t tLastTimeSeconds = 0;

	time_t tNowTime = time(NULL);
	localtime_s(&stTempTm, &tNowTime);
	sprintf_s(cSecond, "%02d", stTempTm.tm_sec);
	if ((strcmp("50", cSecond) == 0) && (tNowTime - tLastTimeSeconds) > 1)
	{
		tLastTimeSeconds = tNowTime;
		bIsDrawing = true;
	}

	return bIsDrawing;
}

bool IsZeroOfDay()
{
	bool bIsZeroOfDay = false;
	struct tm stTempTm;
	char szHourMins[20];
	static time_t tLastTimeSeconds = 0;

	time_t tNowTime = time(NULL);
	localtime_s(&stTempTm, &tNowTime);
	sprintf_s(szHourMins, "%02d-%02d", stTempTm.tm_hour, stTempTm.tm_min);
	if ((strcmp("00-00", szHourMins) == 0) && (tNowTime - tLastTimeSeconds) > 60)
	{
		tLastTimeSeconds = tNowTime;
		bIsZeroOfDay = true;
	}

	return bIsZeroOfDay;
}

void ProcessLogFileOnZeroOfDay()
{
	GetLogFileHandle().InfoLog("%s %d backup log files\n", __FUNCTION__, __LINE__);
#if 0
	char szTargetFile[LOG_FILE_NAME_LEN] = { 0 };
	strncpy(szTargetFile, "CheckLotteryDrawing", sizeof(szTargetFile) - 1);
	GetLogFileHandle().BackupFile(szTargetFile, szTargetFile);
	for (UINT32 iTypeID = 1; iTypeID <= GAME_TYPE_MAX; iTypeID++)
	{
		snprintf(szTargetFile, sizeof(szTargetFile) - 1, "LotteryProcessType%d", iTypeID);
		GetLogFileHandle().BackupFile(szTargetFile, szTargetFile);
	}
#else
	char szTargetFile[MAX_PATH] = { 0 };
	GetLogFileHandle().GetLogFileName(szTargetFile, 1);
	GetLogFileHandle().BackupFile(szTargetFile, szTargetFile);
	GetLogFileHandle().GetLogFileName(szTargetFile, 2);
	GetLogFileHandle().BackupFile(szTargetFile, szTargetFile);
	GetLogFileHandle().GetLogFileName(szTargetFile, 3);
	GetLogFileHandle().BackupFile(szTargetFile, szTargetFile);
	GetLogFileHandle().SetLogNameByDay("Lottery");
	GetLogFileHandle().SetStatNameByDay("Statistic");
#endif
}

void LoopCheckLottery()
{
	SetLogConf();
	LotteryDB lotteryDB;
	if (!lotteryDB.DBConnect())
	{
		return;
	}

	while (true)
	{
		//检查是否是开奖的时间，每分钟的第50秒
		if (IsDrawLotterySecond())
		{
			GetLogFileHandle().InfoLog("%s %d thread(%d) begin new round check drawing lottery\n",
				__FUNCTION__, __LINE__, GetCurrentThreadId());
			DRAW_LOTTERY_PERIOD_QUEUE tagDrawLotteryQueue;
			DRAW_LOTTERY_PERIOD tagDrawLotteryPeriod;
			if (lotteryDB.Ex_GetDrawLottery(tagDrawLotteryQueue))
			{
				GetLogFileHandle().InfoLog("drawing lottery queue size:%d\n", tagDrawLotteryQueue.size());
				{
					lock_guard<mutex> LotteryLock(LotteryMutex);
					while (!tagDrawLotteryQueue.empty())
					{
						memset(&tagDrawLotteryPeriod, 0, sizeof(tagDrawLotteryPeriod));
						tagDrawLotteryPeriod = tagDrawLotteryQueue.front();
						DrawLotteryQueue.push(tagDrawLotteryPeriod);
						tagDrawLotteryQueue.pop();
						GetLogFileHandle().InfoLog("TypeID:%d, UserControled:%d, ControlRate:%d, PowerControl:%d, CurrentIssueNumber:%s, LastIssueNumber:%s, BeginIssueNumber:%s\n", \
							tagDrawLotteryPeriod.iTypeID, tagDrawLotteryPeriod.iUserControled, tagDrawLotteryPeriod.iControlRate, \
							tagDrawLotteryPeriod.iPowerControl, tagDrawLotteryPeriod.strCurrentIssueNumber, \
							tagDrawLotteryPeriod.strLastIssueNumber, tagDrawLotteryPeriod.strBeginIssueNumber);
					}
				}
				GetLogFileHandle().InfoLog("%s %d thread(%d) end new round check drawing lottery\n\n", __FUNCTION__, __LINE__, GetCurrentThreadId());
				LotteryConditionVariable.notify_all();
			}
		}
		else//本分支不会影响获取开奖结果信息
		{	
			if (IsZeroOfDay())
			{
				ProcessLogFileOnZeroOfDay();//备份日志
			}
			gLotteryStatistic.OutputStats();//写入统计信息

			Sleep(50);
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	printf("main process id: %d\n", GetCurrentProcessId());
	printf("main thread id: %d\n", GetCurrentThreadId());
	//先启动工作线程再循环检查是否开奖
	thread arrProcessWorkerThreads[WORKERS_THREAD_NUM];
	for (int i = 0; i < WORKERS_THREAD_NUM; i++)
	{
		arrProcessWorkerThreads[i] = thread(LotteryProcessWorker);
	}
	for (auto &Thread : arrProcessWorkerThreads)
	{
		Thread.detach();
	}

	LoopCheckLottery();

	return 0;
}

