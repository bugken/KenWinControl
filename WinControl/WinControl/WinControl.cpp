// WinControl.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "stdafx.h"
#include "LotteryDB.h"
#include <iostream>
#include <thread>

using namespace std;

MUTEX LotteryMutex;
DRAWLOTTERY_INFO_QUEUE DrawLotteryQueue;
CONDITION_VARIABLE LotteryConditionVariable;

bool LotteryOrderProcess()
{

	return true;
}

void LotteryProcessWorker()
{
	printf("Worker THread ID: %d\n", GetCurrentThreadId());
	LotteryDB lotteryDB;
	lotteryDB.DBConnect();
	while (true)
	{
		DRAWLOTTERY_INFO tagDrawLotteryInfo = {0};
		{
			std::unique_lock<std::mutex> LotteryLock(LotteryMutex);
			LotteryConditionVariable.wait(LotteryLock, []{return !DrawLotteryQueue.empty();});
			tagDrawLotteryInfo = DrawLotteryQueue.front();
			DrawLotteryQueue.pop();
			LotteryLock.unlock();
		}

		lotteryDB.Ex_GetLotteryOrders(tagDrawLotteryInfo);
		LotteryOrderProcess();
		lotteryDB.Ex_UpdateGameResult();
	}
}

bool IsDrawLotterySecond()
{
	return false;
}

void LoopCheckLottery()
{
	LotteryDB lotteryDB;
	lotteryDB.DBConnect();
	while (true)
	{
		printf("LoopCheckLottery every one second\n");
		//����Ƿ��ǿ�����ʱ�䣬ÿ���ӵĵ�50��
		if (IsDrawLotterySecond())
		{
			DRAWLOTTERY_INFO_QUEUE tagDrawLotteryQueue;
			if (lotteryDB.Ex_GetDrawLottery(tagDrawLotteryQueue))
			{
				{
					lock_guard<mutex> LotteryLock(LotteryMutex);
					DrawLotteryQueue = tagDrawLotteryQueue;
				}
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
	printf("Main Process ID: %d\n", GetCurrentProcessId());
	printf("Main THread ID: %d\n", GetCurrentThreadId());

	//�����������߳���ѭ������Ƿ񿪽�
	thread arrProcessWorkerThreads[4];
	for (int i = 0; i < WORKERS_THREAD_NUM; i++)
	{
		arrProcessWorkerThreads[i] = thread(LotteryProcessWorker);
	}
	for (auto &Thread : arrProcessWorkerThreads)
	{
		Thread.join();
	}

	LoopCheckLottery();

	return 0;
}

