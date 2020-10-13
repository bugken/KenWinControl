#pragma once
#include "stdafx.h"
#include "Common.h"
#include "LogFile.h"

using namespace std;

//保存统计信息
struct TExecInfo
{
	TExecInfo()
	{
		mCount = 0;
		mExecTime = 0;
		mMaxTime = 0;
	}

	void AddStat(unsigned int ulExecTime)
	{
		mCount++;
		mExecTime += ulExecTime;
		if (ulExecTime > mMaxTime)
		{
			mMaxTime = ulExecTime;
		}
	}

	unsigned int mCount;
	unsigned int mExecTime;
	unsigned int mMaxTime;
};


class CTicker
{
public:
	CTicker(string name);
	~CTicker();
private:
	unsigned int mBeginTick;
	string mName;
};

class CLotteryStatistic 
{
public:
	CLotteryStatistic();
	~CLotteryStatistic();

	void OutputStats();
	void WriteStats();
	void AddStatistic(string& strName, unsigned int ulUsedMS);
private:
	map<string, TExecInfo> mAllStats;
	MUTEX mMutex;
	static CLogFile* pLogfile;
};

extern CLotteryStatistic gLotteryStatistic;