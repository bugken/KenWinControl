#include "stdafx.h"
#include "LotteryDB.h"
#include <iostream>
#include <thread>

using namespace std;

MUTEX LotteryMutex;
DRAW_LOTTERY_PERIOD_QUEUE DrawLotteryQueue;
CONDITION_VARIABLE LotteryConditionVariable;

//TODO:������Ӯ���ɾֵ���ҽ��е�ɱ
/*
���������ɲʵ�˼·:��Ʊ���ջ�����10�ֽ��,��������ע���ɲ����ϵ����10�����,Ȼ����Ӯ������
���㵥ɱ�û�Ӯ������ɲʵ�˼·:���������ɲ�˼·�෴,�����ע��֪,�������ע������,����ע��Ӧ�Ľ���������ɲʶ��ӵ���ע��,�����Ϳ���ѡ���ĸ���עӮǮ���,��ӮǮ������עɱ��
���㵥ɱ�û�˼·�ͼ��������ɲ�˼·��ͬ��������������ɲ�˼·һ��,��ô�����������:0��ע300 5��ע400 С��ע200,Ȼ��5��ɱ,��ʵ����Ӧ��ɱ0,0�������ϵ��ɲ���300+200=500
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
		{//�����д�����:@SumBonusӦ����('0', '2', '4', '6', '8')��'red'���������һ��,Ŀǰ���д��ֻ���ɱ,������ɱ
			FOR_CONTENT_BEGIN()
				IF_CONDITION_VAR6("red", "0", "2", "4", "6", "8")
			FOR_CONTENT_END()
		}
		else if (userOrder.strSelectType == "green")
		{//�����д�����:@SumBonusӦ����('1', '3', '5', '7', '9')��'green'���������һ��,Ŀǰ���д��ֻ���ɱ,������ɱ
			FOR_CONTENT_BEGIN()
				IF_CONDITION_VAR6("green", "1", "3", "5", "7", "9")
			FOR_CONTENT_END()
		}
		else if (userOrder.strSelectType == "violet")
		{//��ɫ�ֺ��Ϻ�����
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

bool GetLotteryFinalResult(ORDERS_TEN_RESULTS_VEC lottery10Results, float fWinRateAsOfLast, 
		bool bUserControled, UINT32 iControlRate, UINT32 iPowerControl, LOTTERY_RESULT& lotteryResult)
{
	CTicker timeLapser("GetLotteryFinalResult");

	return true;
}

bool ProcessLotteryOrder(LOTTERY_ORDER_DATA& lotteryOrderData, bool& bUserControled)
{
	CTicker timeLapser("ProcessLotteryOrder");
	char strControlUserMostBonusBet[BUFF64] = {0};
	if (lotteryOrderData.vecControlUserOrders.size() > 0)
	{
		bUserControled = GetControlUserMostBonusBet(lotteryOrderData.vecControlUserOrders, strControlUserMostBonusBet);
	}
	else
	{
		printf("No Controled User.\n");
	}

	if (bUserControled)
	{
		DeleteUserControlType(lotteryOrderData.vecLottery10Results, strControlUserMostBonusBet);
	}

	return true;
}

void LotteryProcessWorker()
{
	printf("worker thread id: %d\n", GetCurrentThreadId());
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

		LOTTERY_ORDER_DATA tagLotteryOrderData;
		LOTTERY_RESULT tagLotteryResult;
		bool bUserControled = false;//�Ƿ񵥿�
		bool bResult = lotteryDB.Ex_GetLotteryUserOrders(tagDrawLotteryInfo, tagLotteryOrderData);
		if (bResult)
			bResult = ProcessLotteryOrder(tagLotteryOrderData, bUserControled);
		if (bResult)
			bResult = GetLotteryFinalResult(tagLotteryOrderData.vecLottery10Results, 
				tagLotteryOrderData.fWinRateAsOfLast,bUserControled, tagDrawLotteryInfo.iControlRate,
				tagDrawLotteryInfo.iPowerControl, tagLotteryResult);
		if (bResult)
			lotteryDB.Ex_UpdateGameResult(tagLotteryResult);
	}
}

bool IsDrawLotterySecond()
{
	bool bIsDrawing = false;
	struct tm stTempTm;
	char cSecond[20];

	time_t nowTime = time(NULL);
	localtime_s(&stTempTm, &nowTime);
	sprintf_s(cSecond, "%02d", stTempTm.tm_sec);
	if (strcmp("50", cSecond) == 0)
	{
		bIsDrawing = true;
		printf("...................50s...............\n");
	}

	return bIsDrawing;
}

void LoopCheckLottery()
{
	LotteryDB lotteryDB;
	lotteryDB.DBConnect();
	while (true)
	{
		printf("loop check lottery every 100 miliseconds\n");
		//����Ƿ��ǿ�����ʱ�䣬ÿ���ӵĵ�50��
		if (IsDrawLotterySecond())
		{
			DRAW_LOTTERY_PERIOD_QUEUE tagDrawLotteryQueue;
			if (lotteryDB.Ex_GetDrawLottery(tagDrawLotteryQueue))
			{
				{
					lock_guard<mutex> LotteryLock(LotteryMutex);
					while (!tagDrawLotteryQueue.empty())
					{
						DrawLotteryQueue.push(tagDrawLotteryQueue.front());
						tagDrawLotteryQueue.pop();
					}
				}
				//printf("DrawLotteryQueue:%d\n", DrawLotteryQueue.size());
				//while (!DrawLotteryQueue.empty())
				//{
				//	DRAW_LOTTERY_PERIOD tag = DrawLotteryQueue.front();
				//	DrawLotteryQueue.pop();
				//	printf("uiTypeID:%d\n", tag.uiTypeID);
				//	printf("uiUserControled:%d\n", tag.uiUserControled);
				//	printf("uiControlRate:%d\n", tag.uiControlRate);
				//	printf("uiPowerControl:%d\n", tag.uiPowerControl);
				//	printf("strCurrentIssueNumber:%s\n", tag.strCurrentIssueNumber);
				//	printf("strBeginIssueNumber:%s\n", tag.strBeginIssueNumber);
				//	printf("strLastIssueNumber:%s\n", tag.strLastIssueNumber);
				//}
				LotteryConditionVariable.notify_all();
			}
		}
		else
		{
			Sleep(100);
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	printf("main process id: %d\n", GetCurrentProcessId());
	printf("main thread id: %d\n", GetCurrentThreadId());

	//�����������߳���ѭ������Ƿ񿪽�
	thread arrProcessWorkerThreads[4];
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

