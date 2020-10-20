#pragma  once

#include "stdafx.h"
#include <vector>
#include <string>
#include <queue>
#include <mutex>
#include <map>
#include <condition_variable>
#include <direct.h>
#include "win_linux.h"

using namespace std;

#ifdef WIN32
#include <io.h>
#include <direct.h>
#else
#ifndef MAX_PATH
#define MAX_PATH 260
#endif
#include <unistd.h>
#include <sys/time.h>
#endif

#ifdef WIN32
#define getcwd _getcwd // stupid MSFT "deprecation" warning
#define snprintf _snprintf_s
#define sprintf	sprintf_s
#define strncpy strncpy_s
#elif
#include <unistd.h>
#endif

#define ISSUE_NUMBER_LEN			20                         
#define COLOR_LEN					20 
#define NUMBER_LEN					10
#define BUFF64						64
#define WORKERS_THREAD_NUM_MODE1	0	//工作模式1线程数量
#define WORKERS_THREAD_NUM_MODE2	0	//工作模式2线程数量
#define LOTTERY_RESULT_NUM			10	//投注最终结果个数
#define LOG_FILE_NAME_LEN			100

#define WIN_RATE_NUMBER		9
#define WIN_RATE_VIOLET		5.5
#define WIN_RATE_SIZE		2
#define WIN_RATE_SMALL_BIG	2

#define GAME_TYPE_MAX 4

/*
控制类型
1:单杀,强拉,上拉	2:单杀,强拉,下拉	3:未单杀,强拉,上拉
4:未单杀,强拉,下拉	5:单杀,弱拉,上拉	6:未单杀,弱拉,上拉
7:单杀,弱拉,下拉	8:未单杀,弱拉,下拉	9:保持用户赢率为定值
*/
#define CONTROL_TYPE_USER_STRONG_UP			1
#define CONTROL_TYPE_USER_STRONG_DOWN		2
#define CONTROL_TYPE_NONE_USER_STRONG_UP	3
#define CONTROL_TYPE_NONE_USER_STRONG_DOWN	4
#define CONTROL_TYPE_USER_WEAK_UP			5
#define CONTROL_TYPE_NONE_USER_WEAK_UP		6
#define CONTROL_TYPE_USER_WEAK_DOWN			7
#define CONTROL_TYPE_NONE_USER_WEAK_DOWN	8
#define CONTROL_TYPE_FIX_WIN_RATE			9
/*
控制力度
0:赢率控制在ControlRate附近 1:强拉回 2:弱拉回
*/
#define CONTROL_POWER_FIX_WIN_RATE	0
#define CONTROL_POWER_STRONG		1
#define CONTROL_POWER_WEAK			2

#define FOR_CONTENT_BEGIN() for (auto order : vecControlUserOrders){
#define FOR_CONTENT_END()	}

#define IF_CONTENT()		uiSumBonus += order.uiTotalBonus; \
							if (order.uiTotalBonus > uiMaxTotalBonusCurr) \
							{ \
								uiMaxTotalBonusCurr = order.uiTotalBonus; \
								strcpy_s(strMaxSelectTypeCurr, order.strSelectType); \
							}
#define IF_CONDITION_VAR3(VAR1, VAR2, VAR3) if (strcmp(order.strSelectType, VAR1) == 0 \
							|| strcmp(order.strSelectType, VAR2) == 0 ||strcmp(order.strSelectType, VAR3) == 0) \
							{ \
								IF_CONTENT() \
							}
#define IF_CONDITION_VAR4(VAR1, VAR2, VAR3, VAR4) if (strcmp(order.strSelectType, VAR1) == 0 \
							|| strcmp(order.strSelectType, VAR2) == 0 || strcmp(order.strSelectType, VAR3) == 0  \
							|| strcmp(order.strSelectType, VAR4) == 0) \
							{ \
								IF_CONTENT() \
							}
#define IF_CONDITION_VAR6(VAR1, VAR2, VAR3, VAR4, VAR5, VAR6) if (strcmp(order.strSelectType, VAR1) == 0 \
							|| strcmp(order.strSelectType, VAR2) == 0|| strcmp(order.strSelectType, VAR3) == 0 \
							|| strcmp(order.strSelectType, VAR4) == 0 || strcmp(order.strSelectType, VAR5) == 0 \
							|| strcmp(order.strSelectType, VAR6) == 0) \
							{ \
								IF_CONTENT()\
							}

//日志文件
//NOTE:需要每个类中保存一个pLogFile的静态变量，这个宏才会有效
#define ERROR_LOG(...)	if(pLogFile) pLogFile->ErrorLog(__VA_ARGS__);
#define INFO_LOG(...)	if(pLogFile) pLogFile->InfoLog(__VA_ARGS__);
#define ERROR_LOG_TO_FILE(szFileName, ...)	if(pLogFile) pLogFile->ErrorLogToFile(szFileName, __VA_ARGS__);
#define INFO_LOG_TO_FILE(szFileName, ...)	if(pLogFile) pLogFile->InfoLogToFile(szFileName, __VA_ARGS__);

#define CONDITION_VARIABLE std::condition_variable
#define MUTEX std::mutex
#define LockGuard std::lock_guard<std::mutex> 
#define LockUnique std::unique_lock<std::mutex>

//当前时间
typedef struct _CurrentTime
{
	unsigned int      ulYear;           /*0000-9999*/
	unsigned int      ulMonth;          /*00-12*/
	unsigned int      ulDay;            /*01-31*/
	unsigned int      ulHour;           /*00-23*/
	unsigned int      ulMinute;         /*00-59*/
	unsigned int      ulSecond;         /*00-59*/
	unsigned int      ulMSecond;        /*000-999*/
}CurrentTime;

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

typedef struct _ORDERS_TEN_RESULTS
{
	UINT32  iTypeID;
	char	strIssueNumber[ISSUE_NUMBER_LEN];
	char	strSelectNumber[NUMBER_LEN];
	char	strSelectColor[COLOR_LEN];
	UINT64  uiAllTotalBonus;
	float	fWinRate;
	_ORDERS_TEN_RESULTS()
	{
		memset(this, 0, sizeof(*this));
	}
}ORDERS_TEN_RESULTS;
typedef std::vector<ORDERS_TEN_RESULTS>  ORDERS_TEN_RESULTS_VEC;
typedef std::vector<ORDERS_TEN_RESULTS>::iterator ORDERS_TEN_RESULTS_VEC_IT;

typedef struct _CONTROLED_USER_ORDERS
{
	UINT32  iTypeID;
	char	strIssueNumber[ISSUE_NUMBER_LEN];
	char	strSelectType[COLOR_LEN];
	UINT64  uiTotalBonus;
	_CONTROLED_USER_ORDERS()
	{
		memset(this, 0, sizeof(*this));
	}
}CONTROLED_USER_ORDERS;
typedef std::vector<CONTROLED_USER_ORDERS>  CONTROLED_USER_ORDERS_VEC;

typedef struct  _LOTTERY_ORDER_STAT
{
	UINT32 uiUsersBetCounts;//下注人数
	UINT64 uiAllBet;//所有下注
	UINT64 uiAllBetAsOfLast;//截止上期下注
	UINT64 uiBonusAlready;//已经发放的彩金
	float fWinRateAsOfLast;//截止上期赢率
	_LOTTERY_ORDER_STAT()
	{
		memset(this, 0, sizeof(*this));
	}
}LOTTERY_ORDER_STAT;

typedef struct  _LOTTERY_ORDER_DATA
{
	UINT32 uiUsersBetCounts;//下注人数
	UINT64 uiAllBet;//所有下注
	UINT64 uiAllBetAsOfLast;//截止上期下注
	UINT64 uiBonusAlready;//已经发放的彩金
	float fWinRateAsOfLast;//截止上期赢率
	CONTROLED_USER_ORDERS_VEC vecControlUserOrders;
	ORDERS_TEN_RESULTS_VEC vecLottery10Results;
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

//降序排序函数
bool DescSort(const ORDERS_TEN_RESULTS& V1, const ORDERS_TEN_RESULTS& V2);
//升序排序函数
bool AscSort(const ORDERS_TEN_RESULTS& V1, const ORDERS_TEN_RESULTS& V2);
//创建文件路径
void CreatePath(char szLogPath[MAX_PATH]);
//获取当前工作目录
bool GetCurrentWorkDir(char* pPath, UINT32 iSize);
//获取当前时间
void QueryCurrentTime(CurrentTime* stCurrentTime);
//string to UTF8
std::string string_To_UTF8(const std::string & str);
//UTF8 to string
std::string UTF8_To_string(const std::string & str);
