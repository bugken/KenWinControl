#pragma once

/////////////////////////////////
#include <winsock2.h>
#include <windows.h>
#include <assert.h>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <string>
#include <hash_map>
#include <algorithm>
#include <functional>
#include <iostream>
#include "time.h"
#include "tchar.h"
//#include "logprintf.h"

using namespace std;

#define SAFE_MMCPY(X,Y) \
	memset(X, 0, sizeof(X)); \
	memcpy(X, Y, min(sizeof(X)-1,strlen(Y)))

#define DECLARE_ARRAY(T,N,S)  T N[S + 1]; memset(N,0,sizeof(N));
#define MMCPY(X,Y)            memcpy(X,Y,min(sizeof(X)-1,strlen(Y)))
#define SAFE_DELETE(X)        { if(X != 0) {delete   X; X = 0;} };
#define SAFE_DELETE_ARRAY(X)  { if(X != 0) {delete []X; X = 0;} };
#define SAFE_CLOSEHANDLE(X)   { if(X != 0) {::CloseHandle(X); X = 0;} };
#define MAX_CLIENT_SESSION    5000

#define PACKETLENTYPE         UINT16

#define MAX_IOPACK_SIZE        (32000 - 100)
#define MAX_IOPACKEX_SIZE    (1024 * 32)

#define MAX_IOWBUFF_SIZE      1024 * 64
#define COLLECTSERVDATATM     2

#define LOGIN_VISITOR    1 //手机游客和pc游客都是这个
#define LOGIN_WEIXIN      2 //手机微信
#define LOGIN_USER         3 //注册的账号登录

// 3：大厅滚动消息；4：游戏滚动消息; 6：游戏公告消息；7：弹窗提示消息
#define CLIENT_LOBBY_MSG	3
#define CLIENT_GAME_MSG		4
#define CLIENT_MAINTAIN_MSG	5 
#define CLIENT_PROMPT_MSG	6 
#define CLIENT_NOTICE_MSG	7 

typedef std::vector<std::string> ClientShowMessageVec;
typedef std::map<int/*msgtype*/, ClientShowMessageVec> ClientShowMessageMap;

typedef enum _SERVER_MODE
{
	SERVERMODE_DEBUG,
	SERVERMODE_SERVICE,
	MAX_SERVERMODE
} SERVER_MODE;

struct CharLess : public binary_function<const char*, const char*, bool>
{
public:
    result_type operator()(const first_argument_type& _Left, const second_argument_type& _Right) const
    {
        return(_stricmp(_Left, _Right) < 0 ? true : false);
    }
};

extern const char* GetServerTypeName(int servertype);

extern int EncodeInt(char **pstrEncode, unsigned int uiSrc);
extern int DecodeInt(char **pstrDecode, unsigned int *puiDest);
extern int EncodeString(char **pstrEncode, char *strSrc, short sMaxStrLength);
extern int DecodeString(char **pstrDecode, char *strDest, short sMaxStrLength);


#define SERVERWARNLOG ("serverwarn.log")

#define DBG_LOG(...) \
	if(pLogFile) pLogFile->DbgLog(__VA_ARGS__);

#define CWARN_LOG(...) \
	if(pLogFile) pLogFile->WarnLog(__VA_ARGS__); \
	if(pLogFile) pLogFile->DbgLog(__VA_ARGS__);

#define SWARN_LOG(...) \
	if(pLogFile) pLogFile->DbgLog(__VA_ARGS__); \
	if(pLogFile) pLogFile->LogToFile(SERVERWARNLOG, __VA_ARGS__);

#define SWARN_PC_LOG(...) \
	pPC->WriteLog(__VA_ARGS__); \
	if(pLogFile) pLogFile->DbgLog(__VA_ARGS__); \
	if(pLogFile) pLogFile->LogToFile(SERVERWARNLOG, __VA_ARGS__);

#define CWARN_PC_LOG(...) \
	if(pLogFile) pLogFile->WarnLog(__VA_ARGS__); \
	if(pLogFile) pLogFile->DbgLog(__VA_ARGS__); \
	pPC->WriteLog(__VA_ARGS__);


#define ERR_PC_LOG(...) \
	if(pLogFile) pLogFile->ErrLog(__VA_ARGS__); \
	if(pLogFile) pLogFile->DbgLog(__VA_ARGS__); \
	pPC->WriteLog(__VA_ARGS__);

#define DBG_PC_LOG(...) \
	if(pLogFile) pLogFile->DbgLog(__VA_ARGS__); \
	pPC->WriteLog(__VA_ARGS__);

#define DBG_PC_TB_LOG(...) \
	if(pLogFile) pLogFile->DbgLog(__VA_ARGS__); \
	pPC->WriteLog(__VA_ARGS__); \
	pTable->WriteLog(__VA_ARGS__);

#define PC_AND_LOG(...) \
	pPC->WriteLog(__VA_ARGS__); \
	WriteLog(__VA_ARGS__);

#define PC_ANDLOGTOFILE(szFile, ...) \
	if(pLogFile) pLogFile->LogToFile(szFile, __VA_ARGS__); \
	pPC->WriteLog(__VA_ARGS__); 

#define DBG_TB_LOG(...) \
	if(pLogFile) pLogFile->DbgLog(__VA_ARGS__); \
	pTable->WriteLog(__VA_ARGS__);

#define DBG_WRITE_LOG(...) \
	if(pLogFile) pLogFile->DbgLog(__VA_ARGS__); \
	WriteLog(__VA_ARGS__);

#define CWARN_WRITE_LOG(...) \
	if(pLogFile) pLogFile->WarnLog(__VA_ARGS__); \
	WriteLog(__VA_ARGS__);

#define ERR_WRITE_LOG(...) \
	if(pLogFile) pLogFile->ErrLog(__VA_ARGS__); \
	WriteLog(__VA_ARGS__);

#define SWARN_WRITE_LOG(...) \
	SWARN_LOG(__VA_ARGS__); \
	WriteLog(__VA_ARGS__);

#define ERR_LOG(...) \
	if(pLogFile) pLogFile->ErrLog(__VA_ARGS__); \
	if(pLogFile) pLogFile->DbgLog(__VA_ARGS__);

#define M_ERR_LOG(...) \
if (mLogFile) mLogFile->ErrLog(__VA_ARGS__); 

#define INFO_LOG(...) \
	if(pLogFile) pLogFile->InfoLog(__VA_ARGS__);

#define ERR_INFO_LOG(...) \
	if(pLogFile) pLogFile->ErrLog(__VA_ARGS__); \
	if(pLogFile) pLogFile->InfoLog(__VA_ARGS__);

#define LOG_TOFILE(szFile, ...) \
	if(pLogFile) pLogFile->LogToFile(szFile, __VA_ARGS__);

#define LOG_TOFILE_EX(szFile, ...) \
	if(pLogFile) pLogFile->LogToFile(szFile, __VA_ARGS__); \
	if(pLogFile) pLogFile->DbgLog(__VA_ARGS__);

#define BIN_LOG(...) \
	if(pLogFile) pLogFile->DbgBinLog(__VA_ARGS__);

#define TRACE_LOG(pszName, ...) \
	if(pLogFile) pLogFile->DbgLog(__VA_ARGS__); \
	if(pLogFile) pLogFile->TraceLog(pszName, __VA_ARGS__);

struct PLAYERONTABLEINFO //alice
{
	PLAYERONTABLEINFO()
	{
		memset(this, 0, sizeof(*this));
	}
	int       mIsOnTable;
	UINT32    mTablePwd;
	int       mServerID;
	int       mGameID;
	int       mRoomID;
};

struct INGAMEUSERINFO 
{
	INGAMEUSERINFO()
	{
		memset(this, 0, sizeof(*this));
	}

	INGAMEUSERINFO(int iServerID, int iGameID, int iRoomID) : mServerID(iServerID), mGameID(iGameID), mRoomID(iRoomID)
	{
		
	}

	int       mServerID;
	int       mGameID;
	int       mRoomID;
};

#define OS_IOS                  1
#define OS_ANDROID        2
#define OS_OTHERS          3

#pragma warning(disable:4267)