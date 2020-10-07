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
#define WORKERS_THREAD_NUM	4//�����߳�����
#define LOTTERY_RESULT_NUM		10//Ͷע���ս������

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
		//ִ�д洢����ʱ�䳬��100���룬��¼�澯������500����¼������־
		unsigned int usedms = endTick - mBeginTick;
		printf("%s, The execution of stored procedures[%s] last %u milliseconds \n", __FUNCTION__, mName.c_str(), usedms);
	}
private:
	unsigned int mBeginTick;
	string mName;
};

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
}DRAW_LOTTERY_PERIOD;
typedef std::queue<DRAW_LOTTERY_PERIOD> DRAW_LOTTERY_PERIOD_QUEUE;

typedef struct _PLAYER_ORDERS
{
	UINT32  iTypeID;
	char	strIssueNumber[ISSUE_NUMBER_LEN];
	char	strSelectType[COLOR_LEN];
	UINT64  uiTotalBonus;
}PLAYER_ORDERS;
typedef std::vector<PLAYER_ORDERS>  PLAYER_ORDERS_VEC;

typedef struct  _LOTTERY_ORDER_INFO
{

}LOTTERY_ORDER_INFO;

typedef struct _LOTTERY_RESULT
{
	char	strLotteryNumber[NUMBER_LEN];
	char	strLotteryColor[COLOR_LEN];
}LOTTERY_RESULT;





