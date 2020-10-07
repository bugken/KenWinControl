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
#define COLOR_LEN			20 
#define NUMBER_LEN			10
#define WORKERS_THREAD_NUM	4//工作线程数量
#define LOTTERY_RESULT_NUM	10//投注最终结果个数

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
typedef struct _DRAW_LOTTERY_PERIOD
{
	UINT32  iTypeID;								//彩票类型
	UINT32  iUserControled;						//单杀UserID
	UINT32  iControlRate;							//杀率
	UINT32  iPowerControl;							//控杀类型
	char	strCurrentIssueNumber[ISSUE_NUMBER_LEN];	//当前期号
	char	strBeginIssueNumber[ISSUE_NUMBER_LEN];	//区间开始期号
	char	strLastIssueNumber[ISSUE_NUMBER_LEN];		//上一期期号
	_DRAW_LOTTERY_PERIOD()
	{
		memset(this, 0, sizeof(*this));
	}
}DRAW_LOTTERY_PERIOD;
typedef std::queue<DRAW_LOTTERY_PERIOD> DRAW_LOTTERY_PERIOD_QUEUE;

typedef struct _PLAYER_ORDERS
{
	UINT32  iTypeID;
	char	strIssueNumber[ISSUE_NUMBER_LEN];
	char	strSelectType[COLOR_LEN];
	UINT64  uiTotalBonus;
	_PLAYER_ORDERS()
	{
		memset(this, 0, sizeof(*this));
	}
}PLAYER_ORDERS;
typedef std::vector<PLAYER_ORDERS>  PLAYER_ORDERS_VEC;

typedef struct  _LOTTERY_ORDER_DATA
{
	UINT64 uiAllBet = 0;//所有下注
	UINT64 uiAllBetAsOfLast = 0;//截止上期下注
	UINT64 uiBonusAlready = 0;//已经发放的彩金
	float fWinRateAsOfLast = 0;//截止上期赢率
	PLAYER_ORDERS_VEC vecPlayerOrders;//所有玩家下注
	PLAYER_ORDERS_VEC vecControlUserOrders;//受控玩家下注
	_LOTTERY_ORDER_DATA()
	{
		memset(this, 0, sizeof(*this));
	}
}LOTTERY_ORDER_DATA;

typedef struct _LOTTERY_RESULT
{
	UINT32	iTypeID;
	char	strIssueNumber[ISSUE_NUMBER_LEN];
	UINT32	iControlType;
	char	strLotteryNumber[NUMBER_LEN];
	char	strLotteryColor[COLOR_LEN];
	_LOTTERY_RESULT()
	{
		memset(this, 0, sizeof(*this));
	}
}LOTTERY_RESULT;





