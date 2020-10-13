#include "stdafx.h"
#include "LotteryStatistic.h"

CLotteryStatistic gLotteryStatistic;

//计时类
CTicker::CTicker(string name)
{
	mName = name;
	mBeginTick = ::GetTickCount();
}
CTicker::~CTicker()
{
	unsigned int endTick = ::GetTickCount();
	//执行存储过程时间超过100毫秒，记录告警。超过500，记录错误日志
	unsigned int usedms = endTick - mBeginTick;
	gLotteryStatistic.AddStatistic(mName, usedms);
}

CLotteryStatistic::CLotteryStatistic()
{
}

CLotteryStatistic::~CLotteryStatistic()
{
}

void CLotteryStatistic::AddStatistic(string& strName, unsigned int ulUsedMS)
{
	LockGuard lockGuard(mMutex);
	map<string, TExecInfo>::iterator it = mAllStats.find(strName);
	if (it == mAllStats.end())
	{
		TExecInfo tempInfo;
		tempInfo.AddStat(ulUsedMS);
		mAllStats.insert(make_pair(strName, tempInfo));
	}
	else
	{
		TExecInfo& info = it->second;
		info.AddStat(ulUsedMS);
	}
}

void CLotteryStatistic::OutputStats()
{
	LockGuard lockGuard(mMutex);
	WriteStats();
}

void CLotteryStatistic::WriteStats()
{
	static unsigned int s_time = (unsigned int)time(NULL);
	unsigned int nowtime = (unsigned int)time(NULL);
	if (nowtime < s_time)
	{
		s_time = nowtime;
	}
	int gap = (int)(nowtime - s_time);
	if (gap < 10*60*1000)
	{
		return;
	}
	s_time = nowtime;

	char fname[260];
	CurrentTime stTempNow;
	char szCurDate[32];
	char szStatisticDir[MAX_PATH];
	QueryCurrentTime(&stTempNow);
	GetCurrentWorkDir(szStatisticDir, MAX_PATH);
	sprintf(szCurDate, "%04u-%02u-%02u", stTempNow.ulYear, stTempNow.ulMonth, stTempNow.ulDay);

	sprintf(fname, "%s\\%s_Statistic.log", szStatisticDir, szCurDate);
	FILE* fpStatistic = NULL;
	fopen_s(&fpStatistic, fname, "a+");
	if (!fpStatistic)
	{
		CreatePath(szStatisticDir);
		fopen_s(&fpStatistic, fname, "a+");
		if (!fpStatistic)
		{
			printf("Open statistic file %s failed.\n", fname);
			return;
		}

	}

	fprintf(fpStatistic, "******************************new statistic begin************************************\n");
	for (map<string, TExecInfo>::iterator it = mAllStats.begin(); it != mAllStats.end(); ++it)
	{
		string name = it->first;
		TExecInfo& info = it->second;
		fprintf(fpStatistic, "name = %s count %u usedms %u avgms %.2f maxusedtime %u \n", name.c_str(), 
				info.mCount, info.mExecTime, (float)info.mExecTime / (1.0*info.mCount), info.mMaxTime);
	}
	fprintf(fpStatistic, "******************************new statistic end************************************\n");
	fclose(fpStatistic);

	mAllStats.clear();
}

