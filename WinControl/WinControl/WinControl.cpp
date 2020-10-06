#include "stdafx.h"
#include "LotteryDB.h"
#include <iostream>
#include <thread>

using namespace std;

MUTEX LotteryMutex;
DRAW_LOTTERY_PERIOD_QUEUE DrawLotteryQueue;
CONDITION_VARIABLE LotteryConditionVariable;

bool LotteryOrderProcess(PLAYER_ORDERS_VEC tagPlayerOrdersVec, PLAYER_ORDERS_VEC tagControlUserOrdersVec)
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
		DRAW_LOTTERY_PERIOD tagDrawLotteryInfo = { 0 };
		{
			std::unique_lock<std::mutex> LotteryLock(LotteryMutex);
			LotteryConditionVariable.wait(LotteryLock, []{return !DrawLotteryQueue.empty();});
			tagDrawLotteryInfo = DrawLotteryQueue.front();
			DrawLotteryQueue.pop();
			LotteryLock.unlock();
		}

		PLAYER_ORDERS_VEC tagPlayerOrdersVec, tagControlUserOrdersVec;
		lotteryDB.Ex_GetLotteryOrders(tagDrawLotteryInfo, tagPlayerOrdersVec, tagControlUserOrdersVec);
		LotteryOrderProcess(tagPlayerOrdersVec, tagControlUserOrdersVec);
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
		//检查是否是开奖的时间，每分钟的第50秒
		if (IsDrawLotterySecond())
		{
			DRAW_LOTTERY_PERIOD_QUEUE tagDrawLotteryQueue;
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

	//先启动工作线程再循环检查是否开奖
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

