#pragma  once
#include "stdafx.h"
#include <windows.h>
#include <vector>
#include <string>

using namespace std;

#define ISSUE_NUMBER_LEN	20                         
#define COLOR_LEN			10   
#define WORKERS_THREAD_NUM	4//工作线程数量

class CDBLock
{
public:
	CDBLock(string name)
	{
		mName = name;
		mbeginTick = ::GetTickCount();
	}
	~CDBLock()
	{
		unsigned int endTick = ::GetTickCount();
		//执行存储过程时间超过100毫秒，记录告警。超过500，记录错误日志
		unsigned int usedms = endTick - mbeginTick;
		printf("%s, The execution of stored procedures[%s] last %u milliseconds \n", __FUNCTION__, mName.c_str(), usedms);
	}

private:
	unsigned int mbeginTick;
	string mName;
};


typedef struct _ORDER_INFO
{
	UINT32  uiUserID;
	char	cIssueNumber[ISSUE_NUMBER_LEN];
	UINT32  uiTypeID;
	char	cSelectType[COLOR_LEN];
	float   fRealAmount;
}ORDER_INFO;
typedef std::vector<ORDER_INFO>  ORDER_INFO_VEC;



