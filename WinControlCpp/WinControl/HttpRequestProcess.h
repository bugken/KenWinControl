#pragma once
#include "stdafx.h"
#include <string>
#include "json/reader.h"

using namespace std;

enum HTTP_REQUEST_ID
{
	/*�ⲿʹ��Э��*/
	HTTP_USERSNUMONLINE = 100001,//��ѯ��������

	/*�ⲿʹ��Э��*/
};

class CHttpProcessor
{
public:
	CHttpProcessor(void);
	~CHttpProcessor(void);

public:
	//��ȡ��������
	static bool Proc_GetUsersNumOnline(Json::Value& jsonJarg, string& strReplyBuffer);
	//û�ж�ӦЭ�鴦����
	static bool Proc_HttpDefaultProcessor(Json::Value& jsonJarg, string& strReplyBuffer);
};