#pragma once

#include <string>
#include <map>

#define MAX_ERRSTR_LEN    128

enum
{
	MI_NO_ERROR = 0,
	MI_MASTER_SWITCH_CLOSE = 100001,	//�ܿ���δ����
	MI_BRANCH_SWITCH_CLOSE = 100002,	//���ؿ���δ����
	MI_TIMEOUT_PRESET_DRAWED = 100003,	//δ�ڹ涨ʱ���ڸ��»���Ԥ����ѿ���
	MI_ERR_BANK_RECORD_END,				//û�м�¼
	MAX_MI_ERROR
};

std::string GetErrorString(int errcode, bool return_utf8 = true);
static std::string GetErrorString(int errcode, bool return_utf8)
{
	std::map<int, std::string> mapErrStr;

	/*Begin*/
	mapErrStr[MI_NO_ERROR] = "";
	mapErrStr[MI_MASTER_SWITCH_CLOSE] = "�ܿ���δ����";
	mapErrStr[MI_BRANCH_SWITCH_CLOSE] = "���ؿ���δ����";
	mapErrStr[MI_TIMEOUT_PRESET_DRAWED] = "δ�ڹ涨ʱ���ڸ��»���Ԥ����ѿ���";
	mapErrStr[MI_ERR_BANK_RECORD_END] = "�Ѿ����׿���";
	/*End*/

	std::string errmsg;
	std::map<int, std::string>::iterator it = mapErrStr.find(errcode);
	if (it != mapErrStr.end())
	{
		errmsg = it->second;
	}
	else
	{
		errmsg = "δ֪������";
		errmsg += std::to_string(errcode);
	}
	return errmsg;
}
