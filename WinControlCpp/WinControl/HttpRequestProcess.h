#pragma once
#include "stdafx.h"
#include <string>
#include "json/reader.h"

using namespace std;

enum HTTP_REQUEST_ID
{
	/*外部使用协议*/
	HTTP_USERSNUMONLINE = 100001,//查询在线人数

	/*外部使用协议*/
};

class CHttpProcessor
{
public:
	CHttpProcessor(void);
	~CHttpProcessor(void);

public:
	//获取在线人数
	static bool Proc_GetUsersNumOnline(Json::Value& jsonJarg, string& strReplyBuffer);
	//没有对应协议处理函数
	static bool Proc_HttpDefaultProcessor(Json::Value& jsonJarg, string& strReplyBuffer);
};