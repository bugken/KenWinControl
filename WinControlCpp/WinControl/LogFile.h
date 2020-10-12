#pragma once
#include "stdafx.h"
#include "Common.h"
#include "SingleObject.h"

class CLogFile : public CSingleObject<CLogFile>
{
public:
	CLogFile();
	~CLogFile();

	void SetLogPath(const char *pLogPath);
	void SetBakLogPath(const char *pBakLogPath);
	void SetLogName(const char* pLogName);
	void SetLogNameByDay(const char* pLogName);

	void ErrorLog(const char* msg, ...);
	void ErrorLogToFile(const char* pFileName, const char* msg, ...);
	void InfoLog(const char* msg, ...);
	void InfoLogToFile(const char* pFileName, const char* msg, ...);
	void WriteLogFile(int nPriority, const char* msg, ...);//更具优先级写入日志
	void LogToFile(const char* pszLogFile, const char* msg, ...);
	void LogToFileByDay(const char* pszLogFile, const char* msg, ...);
	void InitLogCfg(const char* pszCfgFile, char* section, char* preStr, char* path);
	void BackupFile(const char* pSrcFile, const char* pDstFile);
	void ReadCfg();
	void ReloadCfg();

private:
	void SetCurrentTime();
	void AddDayOnFileName(const char* pFileName, char* pDayOnFileName, const char* pLogType);
	void WriteToLogFile(const char* szFileName, const char* msg, va_list& args, char* pStrAdd = NULL);
	void WriteLog(FILE* pFile, const char *msg, va_list& args, char* pAddStr = NULL);

private:
	char m_szLogPath[MAX_PATH];  
	char m_szBakLogPath[MAX_PATH];  
	char m_szLogName[MAX_PATH];
	char m_szErrLogName[MAX_PATH];
	char m_szInfoLogName[MAX_PATH];

	CurrentTime m_currentTime;
	MUTEX	m_Mutex;
};

#define GetLogFileHandle()  CLogFile::Instance()
