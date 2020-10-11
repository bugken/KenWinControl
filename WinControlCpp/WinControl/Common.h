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
#define BUFF64				64
#define WORKERS_THREAD_NUM	4//�����߳�����
#define LOTTERY_RESULT_NUM	10//Ͷע���ս������

#define WIN_RATE_NUMBER 9
#define WIN_RATE_VIOLET 5.5
#define WIN_RATE_SIZE 2
#define WIN_RATE_SMALL_BIG 2

/*
��������
1:��ɱ,ǿ��,����	2:��ɱ,ǿ��,����	3:δ��ɱ,ǿ��,����
4:δ��ɱ,ǿ��,����	5:��ɱ,����,����	6:δ��ɱ,����,����
7:��ɱ,����,����	8:δ��ɱ,����,����	9:�����û�Ӯ��Ϊ��ֵ
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
��������
0:Ӯ�ʿ�����ControlRate���� 1:ǿ���� 2:������
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
#define IF_CONDITION_VAR3(VAR1, VAR2, VAR3) if (order.strSelectType == VAR1 \
							|| order.strSelectType == VAR2 || order.strSelectType == VAR3) \
							{ \
								IF_CONTENT() \
							}
#define IF_CONDITION_VAR4(VAR1, VAR2, VAR3, VAR4) if (order.strSelectType == VAR1 || order.strSelectType == VAR2 \
							|| order.strSelectType == VAR3 || order.strSelectType == VAR4) \
							{ \
								IF_CONTENT() \
							}
#define IF_CONDITION_VAR6(VAR1, VAR2, VAR3, VAR4, VAR5, VAR6) if (order.strSelectType == VAR1 \
							|| order.strSelectType == VAR2 || order.strSelectType == VAR3 \
							|| order.strSelectType == VAR4 || order.strSelectType == VAR5 \
							|| order.strSelectType == VAR6) \
							{ \
								IF_CONTENT()\
							}

#define CONDITION_VARIABLE condition_variable
#define MUTEX mutex
#define LockGuard lock_guard<mutex> 

//��ʱ��
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
		//ִ�д洢����ʱ�䳬��100���룬��¼�澯������500����¼������־
		unsigned int usedms = endTick - mBeginTick;
		printf("%s %d, The %s execution last %u milliseconds \n", __FUNCTION__, __LINE__, mName.c_str(), usedms);
	}
private:
	unsigned int mBeginTick;
	string mName;
};
//��ǰʱ��
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

//��ǰ�ں� ��ʼ������ں� ��һ�ں� TypeID ��ɱUserID Ӯ�� ɱ������
typedef struct _DRAW_LOTTERY_PERIOD
{
	UINT32  iTypeID;								//��Ʊ����
	UINT32  iUserControled;						//��ɱUserID
	UINT32  iControlRate;							//ɱ��
	UINT32  iPowerControl;							//��ɱ����
	char	strCurrentIssueNumber[ISSUE_NUMBER_LEN];	//��ǰ�ں�
	char	strBeginIssueNumber[ISSUE_NUMBER_LEN];	//���俪ʼ�ں�
	char	strLastIssueNumber[ISSUE_NUMBER_LEN];		//��һ���ں�
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
	bool operator < (const _ORDERS_TEN_RESULTS &that)const {
		return fWinRate < that.fWinRate;
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

typedef struct  _LOTTERY_ORDER_DATA
{
	UINT64 uiAllBet = 0;//������ע
	UINT64 uiAllBetAsOfLast = 0;//��ֹ������ע
	UINT64 uiBonusAlready = 0;//�Ѿ����ŵĲʽ�
	float fWinRateAsOfLast = 0;//��ֹ����Ӯ��
	ORDERS_TEN_RESULTS_VEC vecLottery10Results;//��ע��10�ֿ��ܽ��
	CONTROLED_USER_ORDERS_VEC vecControlUserOrders;//�ܿ������ע
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

//����������
bool DescSort(const ORDERS_TEN_RESULTS& V1, const ORDERS_TEN_RESULTS& V2);
//����������
bool AscSort(const ORDERS_TEN_RESULTS& V1, const ORDERS_TEN_RESULTS& V2);
//�����ļ�·��
void CreatePath(char szLogPath[MAX_PATH]);
