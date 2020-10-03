#pragma once

#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <stdarg.h>

#define MAX_TRACENUM       10

enum KGLOG_PRIORITY
{
	KGLOG_DEBUG = 1,  // debug-level messages
	KGLOG_WARNING = 2,  // warning conditions
	KGLOG_ERR = 3,  // error conditions
	KGLOG_INFO = 4,  // informational 
	KGLOG_PRIORITY_MAX = 4
};

typedef struct _TTime/*当前时间*/
{
	unsigned int      ulYear;           /*0000-9999*/
	unsigned int      ulMonth;          /*00-12*/
	unsigned int      ulDay;            /*01-31*/
	unsigned int      ulHour;           /*00-23*/
	unsigned int      ulMinute;         /*00-59*/
	unsigned int      ulSecond;         /*00-59*/
	unsigned int      ulMSecond;        /*000-999*/
}TTime;

typedef char TPlayerName[64];

typedef struct tagLogCfg
{
	int DumpLogFlag;
	int DdbLogFlag;
	int WarnLogFlag;
	int InfoLogFlag;
	int BinLogFlag;
	int MsgLogFlag;
	int AssingLogFlag;
	int AllTraceFlag;
	int ShowMs;
	char m_szLogName[MAX_PATH];
	unsigned int unMaxFileSize;
	unsigned int unMaxFileNum;
	int TraceNum;
	unsigned long long Traceuins[MAX_TRACENUM];
	int TraceNameNum;
	TPlayerName Tracenames[MAX_TRACENUM];
	int LockFlag;
}tagZoneLogCfg;

#define MAX_BIN_LEN       4096

class CLogFileEx
{
public:
	CLogFileEx();
	~CLogFileEx();

	void SetLogPath(const char *pLogPath);
	void SetLogName(const char* pLogName);
	void SetMaxLogFile(unsigned int nMaxSize, unsigned int nMaxFileNum);
	void SetDbgLogFlag(int iLogFlag);
	void SetWarnLogFlag(int iErrLogFlag);
	void SetInfoLogFlag(int iInfoLogFlag);
	void SetBinLogFlag(int iBinLogFlag);
	void SetLockFlag(int bLogFlag);
	void SetShowTimeFormat(int ishowms);

	void WriteLogFile(int nPriority, const char* msg, ...);
	void DbgLog(const char* msg, ...);        /*记录普通日志*/
	void WarnLog(const char* msg, ...);     /*记录错误日志*/
	void ErrLog(const char* msg, ...);   /*记录致命日志*/
	void BinLog(const char* pszFileName, char *pBuffer, unsigned int iLength);
	void InfoLog(const char* msg, ...); /*指定文件名打印日志*/
	void DbgBinLog(char *pBuffer, unsigned int iLength);        /*记录binlog到普通日志*/
	void LogToFile(const char* pszLogFile, const char* msg, ...);
	void LogToFileByDay(const char* pszLogFile, const char* msg, ...);
	void TraceUinLog(unsigned long long pszName, const char* msg, ...);
	void TraceNameLog(const char* pszName, const char* msg, ...);

	void SetTraceUins(int nUinTraceNum, unsigned long long pszNames[MAX_TRACENUM]);
	void SetTraceNames(int nNameTraceNum, TPlayerName pszNames[MAX_TRACENUM]);

	virtual	void SetBakLogPath(const char *pBakLogPath);   /* 设置日志备份的路径，可选项默认为：日志路径/baklogs/ */

	void InitLogCfg(const char* pszCfgFile, char* section, char* preStr, char* path);//初始化日志模块yzs.

	void ReadCfg();//初始化日志模块yzs.

	void ReloadCfg();//初始化日志模块yzs.

	bool IsDbgLog();

private:
	void  Lock();
	void  Unlock();
	void  SetCurrentTime();
	void  WriteNormalLog(const char* msg, va_list& args, char* pStrAdd = NULL);
	void  WriteToLogFile(const char* szFileName, const char* msg, va_list& args, char* pStrAdd = NULL);
	void  WriteLog(FILE* pFile, const char *msg, va_list& args, char* pAddStr = NULL);
	void  BakLogFile(const char* pFileName);
	void  GetBakFileName(const char* pFileName, char szBakFileName[MAX_PATH]);
	void  BackupFile(const char* pSrcFile, const char* pDstFile);
	void  WriteThreadNormalLog(int nThreadIndex, const char* msg, va_list& args, char* pStrAdd = NULL);

private:
	int             m_iThreadDbgLogFlag;
	int             m_iThreadWarnLogFlag;
	int             m_iThreadInfoLogFlag;

	int               m_bIsLockLog;
	CRITICAL_SECTION  m_stMutex;
	TTime             m_curTime;
	int             m_nTraceNum;
	int             m_nTraceNameNum;

	int             m_iShowMs;
	int             m_iDbgLogFlag;              /*普通日志的打印标志*/
	int             m_iWarnLogFlag;             /*错误日志的打印标志*/
	int             m_iInfoLogFlag;             /*信息日志的打印标志*/
	int             m_iBinLogFlag;              /*缓冲区日志的打印标志*/
	char            m_szLogPath[MAX_PATH];        /*日志的路径*/
	char            m_szBakLogPath[MAX_PATH];        /*日志备份的路径*/
	char            m_szThreadModuleName[MAX_PATH];
	unsigned long long    m_szTraceNames[MAX_TRACENUM];
	TPlayerName    m_allTraceNames[MAX_TRACENUM];
	char            m_szLogName[MAX_PATH];
	char            m_szLogFileName[MAX_PATH];
	char            m_szNormalLogName[MAX_PATH];
	char            m_szWarnLogName[MAX_PATH];
	char            m_szErrLogName[MAX_PATH];
	char            m_szInfoLogName[MAX_PATH];
	char            m_szLogToFileName[MAX_PATH];
	char            mLogCfgFile[MAX_PATH];
	unsigned int    m_nMaxFileSize;
	unsigned int    m_nMaxFileNum;
	char            mPreStr[20];
	char            mSection[128];

	tagLogCfg          m_ZoneLogCfg;
};

extern CLogFileEx gLogFile;
extern CLogFileEx* pLogFile;
