#include "stdafx.h"
#include "LotteryStatistic.h"
#include "LogFile.h"

CLotteryStatistic gLotteryStatistic;

//��ʱ��
CTicker::CTicker(string name)
{
	mName = name;
	mBeginTick = ::GetTickCount();
}
CTicker::~CTicker()
{
	unsigned int endTick = ::GetTickCount();
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
	if (gap < 60)
	{
		return;
	}
	s_time = nowtime;

	char fname[260];
	CurrentTime stTempNow;
	char szCurDate[32];
	char szCurTime[64];
	char szStatisticDir[MAX_PATH];
	QueryCurrentTime(&stTempNow);
	GetCurrentWorkDir(szStatisticDir, MAX_PATH);
	sprintf(szCurDate, "%04u-%02u-%02u", stTempNow.ulYear, stTempNow.ulMonth, stTempNow.ulDay);
	sprintf(szCurTime, "[%.4u-%.2u-%.2u,%.2u:%.2u:%.2u:%.3u]", stTempNow.ulYear, stTempNow.ulMonth, stTempNow.ulDay, \
		stTempNow.ulHour, stTempNow.ulMinute, stTempNow.ulSecond, stTempNow.ulMSecond);

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
			GetLogFileHandle().ErrorLog("Open statistic file %s failed.\n", fname);
			return;
		}

	}

	fprintf(fpStatistic, "******************************new statistic begin************************************\n");
	for (map<string, TExecInfo>::iterator it = mAllStats.begin(); it != mAllStats.end(); ++it)
	{
		string name = it->first;
		TExecInfo& info = it->second;
		fprintf(fpStatistic, "%s name = %s count %u usedms %u avgms %.2f maxusedtime %u \n", szCurTime, name.c_str(),
				info.mCount, info.mExecTime, (float)info.mExecTime / (1.0*info.mCount), info.mMaxTime);
	}
	fprintf(fpStatistic, "******************************new statistic end**************************************\n\n");
	fclose(fpStatistic);

	mAllStats.clear();
}

