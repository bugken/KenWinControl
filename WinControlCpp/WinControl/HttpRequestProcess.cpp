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

//��ȡ��������
bool CHttpProcessor::Proc_GetUsersNumOnline(Json::Value& jsonJarg, string& strReplyBuffer)
{
	Json::Value jsonValue;
	jsonValue["ret"] = 0;
	jsonValue["users_online"] = 100;

	strReplyBuffer = jsonValue.toStyledString();
	return true;
}

//û�ж�ӦЭ�鴦����
bool CHttpProcessor::Proc_HttpDefaultProcessor(Json::Value& jsonJarg, string& strReplyBuffer)
{
	Json::Value jsonValue;
	jsonValue["ret"] = -1;
	jsonValue["error_message"] = string_To_UTF8("message id not exist").c_str();;

	strReplyBuffer = jsonValue.toStyledString();
	return true;
}
