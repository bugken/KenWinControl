#pragma once

#include <WinSock2.h>
#include <string>
#include <assert.h>

#ifndef IP_LENGTH
#define IP_LENGTH 20
#endif

#define False 0
#define True 1

#define SAFE_snprintf(a, b, c) \
	_snprintf( (a), (b), "%s", (c) ); \
	(a)[(b)] = 0;

#define SAFE_strcpy(a, b) \
	SAFE_snprintf( (a), sizeof(a)-1, (b) )

typedef struct
{
	int m_iYear;
	int m_iMon;
	int m_iDay;
	int m_iHour;
	int m_iMin;
	int m_iSec;
	int m_iMSec;
} TStruTime;

static std::string G2U(const char* pszBuffer)
{	
	int len = MultiByteToWideChar(CP_ACP, 0, pszBuffer, -1, NULL, 0);
	
	wchar_t stack_wstr[10240];
	wchar_t* wstr = stack_wstr;
	if (len > sizeof(stack_wstr))
	{
		wstr = new wchar_t[len + 1];
	}

	MultiByteToWideChar(CP_ACP, 0, pszBuffer, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL) - 1;

	std::string utf8;
	utf8.resize(len);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, (char*)utf8.c_str(), len, NULL, NULL);

	if (wstr != stack_wstr)
	{
		delete[] wstr;
	}

	return utf8;
}

static char* G2U(const char* pszBuffer, char* utf8, int inputlen)
{
	int len = MultiByteToWideChar(CP_ACP, 0, pszBuffer, -1, NULL, 0);

	wchar_t wstr[1024];
	assert(sizeof(wstr) >= len + 1);

	MultiByteToWideChar(CP_ACP, 0, pszBuffer, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL) - 1;
	
	assert(inputlen > len);
	utf8[len] = 0;
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, (char*)utf8, len, NULL, NULL);

	return utf8;
}


static char* U2G_300(const char* utf8, char gb2312[4096])
{
#ifdef WIN32

	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t wstr[30 * 1024] = { 0 };;
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);

	memset(gb2312, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, gb2312, len, NULL, NULL);
	return gb2312;
#else
	strcpy(gb2312, utf8);
	return gb2312;
#endif
}

bool WriteBinaryFile(const char* pszFileName, unsigned char* pbyFileBuf, int iFileLen, bool createpath = true);
bool LoadBinaryFile(const char* pszFileName, unsigned char* pbyFileBuf, int& iFileLen);
bool LoadCompressZipFile(const char* pszFileName, unsigned char* pbyFileBuf, int& iFileLen);
bool Compress(const char* inbuffer, int inlen, char* outbuffer, unsigned int& outlen);

unsigned char* CreateShareMem(char* name, size_t iSize);

void TrimStr(char *strInput);

void SetExeCurrPath(char* pszExeName);

char* GetTimeString_r(time_t timer, char *strTime);

UINT64 GetTimeDes(time_t timer);

void CreatePath(char szLogPath[MAX_PATH]);

int GetDateString(char *strTime);

int GetTimeStringOfDay(char *strTime);

int GetCurStructTime(TStruTime *pstTime);

char* GetIPString(unsigned int ip);

void Alloc32GuidInit();
unsigned int Alloc32GuidEx();

int SplitMapStr(const char *pMapStr, int iMaxItemNum, int *pItemArray, char szGap  = ',');

time_t ParseTimeStr(const char* szTime);

bool CheckIdentify(std::string& identyfy);

struct TTimeVal
{
	unsigned long long  tv_sec;         /* seconds */
	unsigned long long  tv_usec;        /* and microseconds */
};
void gettimeofday(TTimeVal *tp, void *tzp);
unsigned long long RDTSC();

//0 :成功  1:长度不对 2:全部都是数字 3:其它错误
#define ERR_NAME_LEN     1
#define ERR_ALL_DIGIT       2
#define ERR_NAME_TEXT    3
int CheckUserNameLegal(const char* pText);

#ifdef WIN32
extern int SetNBlock(SOCKET iSock);
#else
extern int SetNBlock(int iSock);
#endif

extern bool InitNetwork();
extern char* IPValue2Str(unsigned int uIPValue, char szIPStr[IP_LENGTH]);

extern const char* SockAddrToString(sockaddr_in *pstSockAddr, char *szResult);

//输出buffer是33个字节的，这个函数会对 pszDigestString[32]赋值为0
extern void  Md5ToDigestString(char* pszDataString, char pszDigestString[33]);
