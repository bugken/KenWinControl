#include "stdafx.h"
#include "Common.h"
#include "LotteryDB.h"
#include "LogFile.h"
#include "LotteryStatistic.h"
#include "ErrorID.h"
#include <iostream>
#include <thread>
#include <algorithm>

using namespace std;

extern CLotteryStatistic gLotteryStatistic;

MUTEX LotteryMutex;
DRAW_LOTTERY_PERIOD_QUEUE DrawLotteryQueue;
CONDITION_VARIABLE LotteryConditionVariable;

//TODO:������Ӯ���ɾֵ���ҽ��е�ɱ
/*
���������ɲʵ�˼·:��Ʊ���ջ�����10�ֽ��,��������ע���ɲ����ϵ����10�����,Ȼ����Ӯ������
���㵥ɱ�û�Ӯ������ɲʵ�˼·:���������ɲ�˼·�෴,�����ע��֪,�������ע������,����ע��Ӧ�Ľ���������ɲʶ��ӵ���ע��,�����Ϳ���ѡ���ĸ���עӮǮ���,��ӮǮ������עɱ��
���㵥ɱ�û�˼·�ͼ��������ɲ�˼·��ͬ��������������ɲ�˼·һ��,��ô�����������:0��ע300 5��ע400 С��ע200,Ȼ��5��ɱ,��ʵ����Ӧ��ɱ0,0�������ϵ��ɲ���300+200=500
*/
bool GetControlUserMostBonusBet(const CONTROLED_USER_ORDERS_VEC& vecControlUserOrders, char* strControlUserMostBonusBet)
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
		if (strcmp("0", userOrder.strSelectType) == 0)
		{
			FOR_CONTENT_BEGIN()
				IF_CONDITION_VAR4("0", "red", "violet", "small")
			FOR_CONTENT_END()
		}
		else if (strcmp("1", userOrder.strSelectType) == 0)
		{
			FOR_CONTENT_BEGIN()
				IF_CONDITION_VAR3("1", "green", "small")
			FOR_CONTENT_END()
		}
		else if (strcmp("2", userOrder.strSelectType) == 0)
		{
			FOR_CONTENT_BEGIN()
				IF_CONDITION_VAR3("2", "red", "small")
			FOR_CONTENT_END()
		}
		else if (strcmp("3", userOrder.strSelectType) == 0)
		{
			FOR_CONTENT_BEGIN()
				IF_CONDITION_VAR3("3", "green", "small")
			FOR_CONTENT_END()
		}
		else if (strcmp("4", userOrder.strSelectType) == 0)
		{
			FOR_CONTENT_BEGIN()
				IF_CONDITION_VAR3("4", "red", "small")
			FOR_CONTENT_END()
		}
		else if (strcmp("5", userOrder.strSelectType) == 0)
		{
			FOR_CONTENT_BEGIN()
				IF_CONDITION_VAR4("5", "green", "violet", "big")
			FOR_CONTENT_END()
		}
		else if (strcmp("6", userOrder.strSelectType) == 0)
		{
			FOR_CONTENT_BEGIN()
				IF_CONDITION_VAR3("6", "red", "big")
			FOR_CONTENT_END()
		}
		else if (strcmp("7", userOrder.strSelectType) == 0)
		{
			FOR_CONTENT_BEGIN()
				IF_CONDITION_VAR3("7", "green", "big")
			FOR_CONTENT_END()
		}
		else if (strcmp("8", userOrder.strSelectType) == 0)
		{
			FOR_CONTENT_BEGIN()
				IF_CONDITION_VAR3("8", "red", "big")
			FOR_CONTENT_END()
		}
		else if (strcmp("9", userOrder.strSelectType) == 0)
		{
			FOR_CONTENT_BEGIN()
				IF_CONDITION_VAR3("9", "green", "big")
			FOR_CONTENT_END()
		}
		else if (strcmp("small", userOrder.strSelectType) == 0)
		{
			FOR_CONTENT_BEGIN()
				IF_CONDITION_VAR6("small", "0", "1", "2", "3", "4")
			FOR_CONTENT_END()
		}
		else if (strcmp("big", userOrder.strSelectType) == 0)
		{
			FOR_CONTENT_BEGIN()
				IF_CONDITION_VAR6("big", "5", "6", "7", "8", "9")
			FOR_CONTENT_END()
		}
		else if (strcmp("red", userOrder.strSelectType) == 0)
		{//�����д�����:@SumBonusӦ����('0', '2', '4', '6', '8')��'red'���������һ��,Ŀǰ���д��ֻ���ɱ,������ɱ
			FOR_CONTENT_BEGIN()
				IF_CONDITION_VAR6("red", "0", "2", "4", "6", "8")
			FOR_CONTENT_END()
		}
		else if (strcmp("green", userOrder.strSelectType) == 0)
		{//�����д�����:@SumBonusӦ����('1', '3', '5', '7', '9')��'green'���������һ��,Ŀǰ���д��ֻ���ɱ,������ɱ
			FOR_CONTENT_BEGIN()
				IF_CONDITION_VAR6("green", "1", "3", "5", "7", "9")
			FOR_CONTENT_END()
		}
		else if (strcmp("violet", userOrder.strSelectType) == 0)
		{//��ɫ�ֺ��Ϻ�����
			UINT64 uiRedVioletBonus = 0, uiGreenVioletBonus = 0;
			UINT64 MaxBonusRedViolet = 0, MaxBonusGreenViolet = 0;
			for (auto order : vecControlUserOrders)
			{
				if (strcmp("red", order.strSelectType) == 0 || strcmp("0", order.strSelectType) == 0 
					|| strcmp("violet", order.strSelectType) == 0)
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
				if (strcmp("green", order.strSelectType) == 0 || strcmp("5", order.strSelectType) == 0
					|| strcmp("violet", order.strSelectType) == 0)
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
		//ÿ��ѭ���ж�һ��ѡȡ������ע����
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
	//if��Ƕ��ѭ����Ч���Ըߣ��������ɶ�ѭ���Ż�
	if (strcmp("0", strControlUserMostBonusBet) == 0 || strcmp("1", strControlUserMostBonusBet) == 0 
		|| strcmp("2", strControlUserMostBonusBet) == 0 || strcmp("3", strControlUserMostBonusBet) == 0
		|| strcmp("4", strControlUserMostBonusBet) == 0 || strcmp("5", strControlUserMostBonusBet) == 0
		|| strcmp("6", strControlUserMostBonusBet) == 0 || strcmp("7", strControlUserMostBonusBet) == 0
		|| strcmp("8", strControlUserMostBonusBet) == 0 || strcmp("9", strControlUserMostBonusBet) == 0)
	{
		for (ORDERS_TEN_RESULTS_VEC_IT it = lottery10Results.begin(); it != lottery10Results.end(); ++it)
		{
			if (strcmp(it->strSelectNumber, strControlUserMostBonusBet) == 0)
			{
				it = lottery10Results.erase(it);
				break;
			}
		}
	}
	else if (strcmp("big", strControlUserMostBonusBet) == 0)
	{
		for (ORDERS_TEN_RESULTS_VEC_IT it = lottery10Results.begin(); it != lottery10Results.end(); ++it)
		{
			if (strcmp("5", it->strSelectNumber) == 0 || strcmp("6", it->strSelectNumber) == 0
				|| strcmp("7", it->strSelectNumber) == 0 || strcmp("8", it->strSelectNumber) == 0
				|| strcmp("9", it->strSelectNumber) == 0)
			{
				it = lottery10Results.erase(it);
				break;
			}
		}
	}
	else if (strcmp("small", strControlUserMostBonusBet) == 0)
	{
		for (ORDERS_TEN_RESULTS_VEC_IT it = lottery10Results.begin(); it != lottery10Results.end(); ++it)
		{
			if (strcmp("0", it->strSelectNumber) == 0 || strcmp("1", it->strSelectNumber) == 0
				|| strcmp("2", it->strSelectNumber) == 0 || strcmp("3", it->strSelectNumber) == 0
				|| strcmp("4", it->strSelectNumber) == 0)
			{
				it = lottery10Results.erase(it);
				break;
			}
		}
	}
	else if (strcmp("red", strControlUserMostBonusBet) == 0)
	{
		for (ORDERS_TEN_RESULTS_VEC_IT it = lottery10Results.begin(); it != lottery10Results.end(); ++it)
		{
			if (strcmp("0", it->strSelectNumber) == 0 || strcmp("2", it->strSelectNumber) == 0
				|| strcmp("4", it->strSelectNumber) == 0 || strcmp("6", it->strSelectNumber) == 0
				|| strcmp("8", it->strSelectNumber) == 0)
			{
				it = lottery10Results.erase(it);
				break;
			}
		}
	}
	else if (strcmp("green", strControlUserMostBonusBet) == 0)
	{
		for (ORDERS_TEN_RESULTS_VEC_IT it = lottery10Results.begin(); it != lottery10Results.end(); ++it)
		{
			if (strcmp("1", it->strSelectNumber) == 0 || strcmp("3", it->strSelectNumber) == 0
				|| strcmp("5", it->strSelectNumber) == 0 || strcmp("7", it->strSelectNumber) == 0
				|| strcmp("9", it->strSelectNumber) == 0)
			{
				it = lottery10Results.erase(it);
				break;
			}
		}
	}
	else if (strcmp("violet", strControlUserMostBonusBet) == 0)
	{
		for (ORDERS_TEN_RESULTS_VEC_IT it = lottery10Results.begin(); it != lottery10Results.end(); ++it)
		{
			if (strcmp("0", it->strSelectNumber) == 0 || strcmp("5", it->strSelectNumber) == 0)
			{
				it = lottery10Results.erase(it);
				break;
			}
		}
	}

	return true;
}

void SetLotteryResult(ORDERS_TEN_RESULTS& Result, LOTTERY_RESULT& lotteryResult)
{
	lotteryResult.iTypeID = Result.iTypeID;
	strcpy_s(lotteryResult.strIssueNumber, Result.strIssueNumber);
	strcpy_s(lotteryResult.strLotteryNumber, Result.strSelectNumber);
	strcpy_s(lotteryResult.strLotteryColor, Result.strSelectColor);
}
/*
��������
1:��ɱ,ǿ��,����	2:��ɱ,ǿ��,����	3:δ��ɱ,ǿ��,���� 
4:δ��ɱ,ǿ��,����	5:��ɱ,����,����	6:δ��ɱ,����,���� 
7:��ɱ,����,����	8:δ��ɱ,����,����	9:�����û�Ӯ��Ϊ��ֵ
��������
0:Ӯ�ʿ�����ControlRate���� 1:ǿ���� 2:������
*/
bool GetLotteryFinalResult(ORDERS_TEN_RESULTS_VEC& lottery10ResultsVec, float fWinRateAsOfLast, 
		bool bUserControled, UINT32 iControlRate, UINT32 iPowerControl, LOTTERY_RESULT& lotteryResult)
{
	CTicker timeLapser("GetLotteryFinalResult");
	UINT32 iSteps = 0;
	UINT32 iVecSize = lottery10ResultsVec.size();
	float fTargetWinRate = (float)(iControlRate * 1.0 / 10000);
	sort(lottery10ResultsVec.begin(), lottery10ResultsVec.end(), DescSort);

	if (CONTROL_POWER_STRONG == iPowerControl)//ǿ��
	{
		if (fWinRateAsOfLast <= fTargetWinRate)
		{//ǿ������ȡ��һ��Ԫ��
			SetLotteryResult(lottery10ResultsVec.front(), lotteryResult);
			bUserControled ? lotteryResult.iControlType = CONTROL_TYPE_USER_STRONG_UP:
				lotteryResult.iControlType = CONTROL_TYPE_NONE_USER_STRONG_UP;
		}
		else
		{//ǿ������ȡ���һ��Ԫ��
			SetLotteryResult(lottery10ResultsVec.back(), lotteryResult);
			bUserControled ? lotteryResult.iControlType = CONTROL_TYPE_USER_STRONG_DOWN 
				: lotteryResult.iControlType = CONTROL_TYPE_NONE_USER_STRONG_DOWN;
		}
	}
	else if (CONTROL_POWER_WEAK == iPowerControl)//����
	{
		if (fWinRateAsOfLast <= fTargetWinRate)//������
		{
			UINT32 iBingoCounts = 0;
			//��������ȡ��һ��ֵ����������Ӯ�ʶ���Ŀ��Ӯ��С�����
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
				//���������������ֵ�����Ϊֹ
				if ((3 == iBingoCounts) || (++iSteps == iVecSize))
					break;
			}
		}
		else//������
		{
			bool bIsFound = false;//�Ƿ������Сֵ��λ��
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
				//1.�ҵ���Сֵ,���������������� 2.û���ҵ���Сֵ,���������һ��
				if ((bIsFound && (iSteps == iVecSize - 2)) ||
					(!bIsFound && iSteps == iVecSize))
				{
					SetLotteryResult(result, lotteryResult);
					break;
				}
			}
		}
	}
	else if (CONTROL_POWER_FIX_WIN_RATE == iPowerControl)//����Ӯ�����趨ֵ
	{/*Ӧ��������������֮��:����ȡ���һ����Ŀ��Ӯ�ʴ�Ľ��������ȡ��һ��С��Ŀ��Ӯ�ʵĽ��
	 **��ǰ����Ӯ�����趨ֵֻȡ��һ�����ڻ���С���趨ֵ�Ľ��
	 */
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

bool ProcessControledUserOrder(const CONTROLED_USER_ORDERS_VEC& vecControlUserOrders, ORDERS_TEN_RESULTS_VEC& vecLottery10Results)
{
	CTicker timeLapser("ProcessControledUserOrder");
	char strControlUserMostBonusBet[BUFF64] = {0};
	if (vecControlUserOrders.size() > 0)
	{
		GetControlUserMostBonusBet(vecControlUserOrders, strControlUserMostBonusBet);
	}
	else
	{
		GetLogFileHandle().InfoLog("%s %d no controled user.\n", __FUNCTION__, __LINE__);
		return true;
	}

	DeleteUserControlType(vecLottery10Results, strControlUserMostBonusBet);
	GetLogFileHandle().InfoLog("%s %d ControlUserMostBonusBet %s deleted in lottery 10 results.\n",
		__FUNCTION__, __LINE__, strControlUserMostBonusBet);

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
			LockUnique LotteryLock(LotteryMutex);
			LotteryConditionVariable.wait(LotteryLock, []{return !DrawLotteryQueue.empty(); });
			tagDrawLotteryInfo = DrawLotteryQueue.front();
			DrawLotteryQueue.pop();
			LotteryLock.unlock();
		}
		{
			CTicker timeLapser("LotteryProcessWorker");
			GetLogFileHandle().InfoLog("%s %d lottery worker process(%d) begin\n", __FUNCTION__, __LINE__, GetCurrentThreadId());
			GetLogFileHandle().InfoLog("TypeID:%d, UserControled:%d, ControlRate:%d, PowerControl:%d, CurrentIssueNumber:%s, LastIssueNumber:%s, BeginIssueNumber:%s\n", \
				tagDrawLotteryInfo.iTypeID, tagDrawLotteryInfo.iUserControled, tagDrawLotteryInfo.iControlRate, \
				tagDrawLotteryInfo.iPowerControl, tagDrawLotteryInfo.strCurrentIssueNumber, \
				tagDrawLotteryInfo.strLastIssueNumber, tagDrawLotteryInfo.strBeginIssueNumber);

			UINT32 uiRetID = 0;
			float fTargetWinRate = 0.0;
			bool bUserControled = false;//�Ƿ񵥿�
			bool bReturn = false, bReStatistic = false;
			bool bRet10Results = false, bRetControledUserOrders = false;
			LOTTERY_ORDER_STAT tagLotteryOrderStat;
			ORDERS_TEN_RESULTS_VEC tagOrder10ResultsVec;
			CONTROLED_USER_ORDERS_VEC tagControledUserOrdersVec;
			LOTTERY_RESULT tagLotteryResult;

			//��ȡͳ����Ϣ 10�н�� �����û�����
			bReStatistic = lotteryDB.Ex_GetLotteryStatistic(tagDrawLotteryInfo, tagLotteryOrderStat);
			bRet10Results = lotteryDB.Ex_GetLottery10Results(tagDrawLotteryInfo, tagLotteryOrderStat.uiBonusAlready,
				tagLotteryOrderStat.uiAllBet, tagOrder10ResultsVec);
			bRetControledUserOrders = lotteryDB.Ex_GetControledUserOrders(tagDrawLotteryInfo, tagControledUserOrdersVec);
			if (!bReStatistic || !bRet10Results || !bRetControledUserOrders)
			{
				GetLogFileHandle().InfoLog("TypeID:%d, CurrentIssueNumber:%s get order statistic or ten results or controled user order failed\n", \
					tagDrawLotteryInfo.iTypeID, tagDrawLotteryInfo.strCurrentIssueNumber);
				GetLogFileHandle().InfoLog("%s %d lottery worker process(%d) end\n", __FUNCTION__, __LINE__, GetCurrentThreadId());
				continue;
			}
			if (tagLotteryOrderStat.uiUsersBetCounts < 1)
			{
				GetLogFileHandle().InfoLog("TypeID:%d, CurrentIssueNumber:%s bet users counts:%d < 5\n", \
					tagDrawLotteryInfo.iTypeID, tagDrawLotteryInfo.strCurrentIssueNumber, tagLotteryOrderStat.uiUsersBetCounts);
				GetLogFileHandle().InfoLog("%s %d lottery worker process(%d) end\n", __FUNCTION__, __LINE__, GetCurrentThreadId());
				continue;
			}

			//�������������
			if (tagDrawLotteryInfo.iUserControled > 0 && tagControledUserOrdersVec.size() > 0)
			{
				bUserControled = true;
				ProcessControledUserOrder(tagControledUserOrdersVec, tagOrder10ResultsVec);
			}
			else
			{
				GetLogFileHandle().InfoLog("%s %d no controled user.\n", __FUNCTION__, __LINE__);
			}

			//�õ����ս��
			fTargetWinRate = (float)(tagDrawLotteryInfo.iControlRate * 1.0 / 10000);
			GetLogFileHandle().InfoLog("%s %d TargetWinRate:%f, totally %d results as bellow:\n",
				__FUNCTION__, __LINE__, fTargetWinRate, tagOrder10ResultsVec.size());
			for (auto result : tagOrder10ResultsVec)
			{
				GetLogFileHandle().InfoLog("TypeID:%d, IssueNumber:%s, SelectNumber:%s, SelectColor:%s, AllTotalBonus:%I64u, WinRate:%f\n", \
					result.iTypeID, result.strIssueNumber, result.strSelectNumber, \
					result.strSelectColor, result.uiAllTotalBonus, result.fWinRate);
			}
			GetLotteryFinalResult(tagOrder10ResultsVec, tagLotteryOrderStat.fWinRateAsOfLast,
				bUserControled, tagDrawLotteryInfo.iControlRate, tagDrawLotteryInfo.iPowerControl, tagLotteryResult);

			//����Game��
			GetLogFileHandle().InfoLog("game final result as bellow:\n");
			GetLogFileHandle().InfoLog("TypeID:%d, IssueNumber:%s, LotteryNumber:%s, LotteryColor:%s, ControlType:%d\n", \
				tagLotteryResult.iTypeID, tagLotteryResult.strIssueNumber, tagLotteryResult.strLotteryNumber, \
				tagLotteryResult.strLotteryColor, tagLotteryResult.iControlType);
			lotteryDB.Ex_UpdateGameResult(tagLotteryResult, uiRetID);
			if (0 != uiRetID)
			{
				GetLogFileHandle().InfoLog("TypeID:%d, IssueNumber:%s, %s\n", \
					tagLotteryResult.iTypeID, tagLotteryResult.strIssueNumber, GetErrorString(uiRetID).c_str());
			}
			GetLogFileHandle().InfoLog("%s %d lottery worker process(%d) end\n", __FUNCTION__, __LINE__, GetCurrentThreadId());
		}
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
	printf("%s %d backup log files\n", __FUNCTION__, __LINE__);
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

	char szLogFileName[MAX_PATH] = { 0 };
	GetLogFileHandle().GetLogFileName(szLogFileName, 1);
	while (true)
	{
		//����Ƿ��ǿ�����ʱ�䣬ÿ���ӵĵ�50��
		if (IsDrawLotterySecond())
		{
			GetLogFileHandle().InfoLogToFileNoTime(szLogFileName, "\n");
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
				GetLogFileHandle().InfoLog("%s %d thread(%d) end new round check drawing lottery\n", __FUNCTION__, __LINE__, GetCurrentThreadId());
				GetLogFileHandle().InfoLogToFileNoTime(szLogFileName, "\n");
				LotteryConditionVariable.notify_all();
			}
		}
		else//����֧����Ӱ���ȡ���������Ϣ
		{	
			if (IsZeroOfDay())
			{
				ProcessLogFileOnZeroOfDay();//������־
			}
			gLotteryStatistic.OutputStats();//д��ͳ����Ϣ
			GetLogFileHandle().GetLogFileName(szLogFileName, 1);

			Sleep(10);
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	printf("main process id: %d\n", GetCurrentProcessId());
	printf("main thread id: %d\n", GetCurrentThreadId());
	//�����������߳���ѭ������Ƿ񿪽�
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

