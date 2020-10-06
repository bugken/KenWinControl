#pragma  once

#include "stdafx.h"
#include <windows.h>
#include <vector>
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>

using namespace std;

#define ISSUE_NUMBER_LEN	20                         
#define COLOR_LEN			10 
#define NUMBER_LEN			5
#define WORKERS_THREAD_NUM	4//工作线程数量

#define CONDITION_VARIABLE condition_variable
#define MUTEX mutex

class CTicker
{
public:
	CTicker(string name)
	{
		mName = name;
		mBeginTick = ::GetTickCount();
	}
	~CTicker()
	{
		unsigned int endTick = ::GetTickCount();
		//执行存储过程时间超过100毫秒，记录告警。超过500，记录错误日志
		unsigned int usedms = endTick - mBeginTick;
		printf("%s, The execution of stored procedures[%s] last %u milliseconds \n", __FUNCTION__, mName.c_str(), usedms);
	}
private:
	unsigned int mBeginTick;
	string mName;
};

//当前期号 开始计算的期号 上一期号 TypeID 单杀UserID 赢率 杀率类型
typedef struct _DRAWLOTTERY_INFO
{
	UINT32  uiTypeID;								//彩票类型
	UINT32  uiUserControled;						//单杀UserID
	UINT32  uiControlRate;							//杀率
	UINT32  uiPowerControl;							//控杀类型
	char	cCurrentIssueNumber[ISSUE_NUMBER_LEN];	//当前期号
	char	cBeginIssueNumber[ISSUE_NUMBER_LEN];	//区间开始期号
	char	cLastIssueNumber[ISSUE_NUMBER_LEN];		//上一期期号
}DRAWLOTTERY_INFO;
typedef std::queue<DRAWLOTTERY_INFO> DRAWLOTTERY_INFO_QUEUE;

typedef struct _ORDER_INFO
{
	UINT32  uiUserID;
	char	cIssueNumber[ISSUE_NUMBER_LEN];
	UINT32  uiTypeID;
	char	cSelectType[COLOR_LEN];
	float   fRealAmount;
}ORDER_INFO;
typedef std::vector<ORDER_INFO>  ORDER_INFO_VEC;

typedef struct  _LOTTERY_ORDER_INFO
{

}LOTTERY_ORDER_INFO;

typedef struct _Finally_Result
{
	char	cLotteryNumber[NUMBER_LEN];
	char	cLotteryColor[COLOR_LEN];
}Finally_Result;





