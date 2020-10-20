#include "stdafx.h"
#include <string>
#include "HttpRequestProcess.h"
#include "json/reader.h"
#include "Common.h"

using namespace std;

CHttpProcessor::CHttpProcessor(void)
{
}
CHttpProcessor::~CHttpProcessor(void)
{
}

//获取在线人数
bool CHttpProcessor::Proc_GetUsersNumOnline(Json::Value& jsonJarg, string& strReplyBuffer)
{
	Json::Value jsonValue;
	jsonValue["ret"] = 0;
	jsonValue["users_online"] = 100;

	strReplyBuffer = jsonValue.toStyledString();
	return true;
}

//没有对应协议处理函数
bool CHttpProcessor::Proc_HttpDefaultProcessor(Json::Value& jsonJarg, string& strReplyBuffer)
{
	Json::Value jsonValue;
	jsonValue["ret"] = -1;
	jsonValue["error_message"] = string_To_UTF8("message id not exist").c_str();;

	strReplyBuffer = jsonValue.toStyledString();
	return true;
}
