#pragma once

#include <string>
#include <map>

#define MAX_ERRSTR_LEN    128

enum
{
	MI_NO_ERROR = 0,
	MI_MASTER_SWITCH_CLOSE = 100001,	//总开关未开启
	MI_BRANCH_SWITCH_CLOSE = 100002,	//单控开关未开启
	MI_TIMEOUT_PRESET_DRAWED = 100003,	//未在规定时间内更新或已预设或已开奖
	MI_ERR_BANK_RECORD_END,				//没有记录
	MAX_MI_ERROR
};

std::string GetErrorString(int errcode, bool return_utf8 = true);
static std::string GetErrorString(int errcode, bool return_utf8)
{
	std::map<int, std::string> mapErrStr;

	/*Begin*/
	mapErrStr[MI_NO_ERROR] = "";
	mapErrStr[MI_MASTER_SWITCH_CLOSE] = "总开关未开启";
	mapErrStr[MI_BRANCH_SWITCH_CLOSE] = "单控开关未开启";
	mapErrStr[MI_TIMEOUT_PRESET_DRAWED] = "未在规定时间内更新或已预设或已开奖";
	mapErrStr[MI_ERR_BANK_RECORD_END] = "已经到底咯！";
	/*End*/

	std::string errmsg;
	std::map<int, std::string>::iterator it = mapErrStr.find(errcode);
	if (it != mapErrStr.end())
	{
		errmsg = it->second;
	}
	else
	{
		errmsg = "未知错误码";
		errmsg += std::to_string(errcode);
	}
	return errmsg;
}
