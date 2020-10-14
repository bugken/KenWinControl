#include "stdafx.h"
#include "LogFile.h"

INTIALIZE_SINGLEOBJ(CLogFile);

CLogFile::CLogFile()
{
	memset(m_szLogPath, 0, sizeof(m_szLogPath));
	memset(m_szBakLogPath, 0, sizeof(m_szBakLogPath));
	memset(m_szLogName, 0, sizeof(m_szLogName));
	memset(m_szErrLogName, 0, sizeof(m_szErrLogName));
	memset(m_szInfoLogName, 0, sizeof(m_szInfoLogName));
	memset(m_szStatLogName, 0, sizeof(m_szStatLogName));
	memset(&m_currentTime, 0, sizeof(m_currentTime));
}

CLogFile::~CLogFile()
{
}

/*
snprintf:
GCC�еĲ���n��ʾ��str��д��n���ַ�������'\0'�ַ������ҷ���ʵ�ʵ��ַ������ȡ�
VC�еĲ���n��ʾ����str��д��n���ַ���������'\0'�ַ������Ҳ������ַ���ĩβ���'\0'����
*/
/*
char *strncpy(char *dest, const char *src, int n)
���n<src�ĳ��ȣ�ֻ�ǽ�src��ǰn���ַ����Ƶ�dest��ǰn���ַ������Զ����'\0'��Ҳ���ǽ��dest������'\0'��
��Ҫ���ֶ����һ��'\0'�����src�ĳ���С��n���ֽڣ�����NULL���destֱ��������n���ֽڡ�
*/
void CLogFile::SetLogPath(const char *pLogPath)
{
	if (!pLogPath)
	{
		return;
	}

	memset(m_szLogPath, 0, sizeof(m_szLogPath));
	strncpy(m_szLogPath, pLogPath, sizeof(m_szLogPath) - 2);
	int nLen = (int)strlen(m_szLogPath);
	for (int i = 0; i < nLen; i++)
	{
		if (m_szLogPath[i] == '\\')
		{
			m_szLogPath[i] = '/';
		}
	}
	if (m_szLogPath[nLen - 1] != '/')
	{
		m_szLogPath[nLen] = '/';
	}
	m_szLogPath[nLen + 1] = '\0';//�ֶ�����'\0'

	char szPath[MAX_PATH] = { 0 };
	snprintf(szPath, sizeof(szPath) - 1, "%s/LogBackupDir", m_szLogPath);
	SetBakLogPath(szPath);
}

void CLogFile::SetBakLogPath(const char *pBakLogPath)
{
	if (!pBakLogPath)
	{
		return;
	}
	memset(m_szBakLogPath, 0, sizeof(m_szBakLogPath));
	strncpy(m_szBakLogPath, pBakLogPath, sizeof(m_szBakLogPath) - 2);
	int nLen = (int)strlen(m_szBakLogPath);
	for (int i = 0; i < nLen; i++)
	{
		if (m_szBakLogPath[i] == '\\')
		{
			m_szBakLogPath[i] = '/';
		}
	}
	if (m_szBakLogPath[nLen - 1] != '/')
	{
		m_szBakLogPath[nLen] = '/';
	}
	m_szBakLogPath[nLen + 1] = '\0';
}

void CLogFile::SetLogName(const char* pLogName)
{
	if (!pLogName)
		return;

	memset(m_szErrLogName, 0, sizeof(m_szErrLogName));
	memset(m_szInfoLogName, 0, sizeof(m_szInfoLogName));
	strncpy(m_szLogName, pLogName, sizeof(m_szLogName) - 1);
	snprintf(m_szErrLogName, sizeof(m_szErrLogName) - 1, "%s_Error.log", m_szLogName);
	snprintf(m_szInfoLogName, sizeof(m_szInfoLogName) - 1, "%s_Info.log", m_szLogName);
}

void CLogFile::SetLogNameByDay(const char* pLogName)
{
	if (!pLogName)
		return;

	char m_szLogFileName[LOG_FILE_NAME_LEN] = {0};
	memset(m_szLogFileName, 0, sizeof(m_szLogFileName));
	SetCurrentTime();
	int nRet = snprintf(m_szLogFileName, sizeof(m_szLogFileName) - 1, "%04u-%02u-%02u_%s",
		m_currentTime.ulYear, m_currentTime.ulMonth, m_currentTime.ulDay, pLogName);
	if (nRet <= 0)
	{
		SetLogName(pLogName);
		return;
	}
	m_szLogFileName[nRet] = '\0';
	SetLogName(m_szLogFileName);
}

void CLogFile::SetStatNameByDay(const char* pLogName)
{
	if (!pLogName)
		return;

	char m_szStatFileName[LOG_FILE_NAME_LEN];
	memset(m_szStatFileName, 0, sizeof(m_szStatFileName));
	SetCurrentTime();
	snprintf(m_szStatFileName, sizeof(m_szStatFileName) - 1, "%04u-%02u-%02u_%s",
		m_currentTime.ulYear, m_currentTime.ulMonth, m_currentTime.ulDay, pLogName);
	snprintf(m_szStatLogName, sizeof(m_szStatLogName) - 1, "%s.log", m_szStatFileName);
}

void CLogFile::GetLogFileName(char* pFileName, int iFileType)
{
	if (iFileType == 1)//InfoLog
	{
		memcpy(pFileName, m_szInfoLogName, sizeof(m_szInfoLogName) - 1);
	}
	else if (iFileType == 2)//ErrorLog
	{
		memcpy(pFileName, m_szErrLogName, sizeof(m_szErrLogName) - 1);
	}
	else if (iFileType == 3)//StatLog
	{
		memcpy(pFileName, m_szStatLogName, sizeof(m_szStatLogName) - 1);
	}
}

void CLogFile::SetCurrentTime()
{
	struct tm tmpTime = {0};
	struct timeval tmNow;

#ifndef WIN32
	gettimeofday(&tmNow, NULL);
#else
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	tmNow.tv_sec = (long)time(NULL);
	tmNow.tv_usec = sysTime.wMilliseconds * 1000;
#endif
	time_t tNow = tmNow.tv_sec;
	m_currentTime.ulMSecond = tmNow.tv_usec / 1000;
	localtime_s(&tmpTime, &tNow);

	m_currentTime.ulYear = tmpTime.tm_year + 1900;
	m_currentTime.ulMonth = tmpTime.tm_mon + 1;
	m_currentTime.ulDay = tmpTime.tm_mday;
	m_currentTime.ulHour = tmpTime.tm_hour;
	m_currentTime.ulMinute = tmpTime.tm_min;
	m_currentTime.ulSecond = tmpTime.tm_sec;
}

void CLogFile::AddDayOnFileName(const char* pFileName, char* pDayOnFileName, const char* pLogType)
{
	char m_szLogToFileName[LOG_FILE_NAME_LEN];
	memset(m_szLogToFileName, 0, sizeof(m_szLogToFileName));

	//SetCurrentTime();//�����Ѿ������˸���ʱ��ĺ���
	snprintf(m_szLogToFileName, sizeof(m_szLogToFileName) - 1, "%04u-%02u-%02u_%s_%s.log",
		m_currentTime.ulYear, m_currentTime.ulMonth, m_currentTime.ulDay, pFileName, pLogType);
	memcpy(pDayOnFileName, m_szLogToFileName, sizeof(m_szLogToFileName) - 1);
}

void CLogFile::BackupFile(const char* pSrcFile, const char* pDstFile)
{
	char szSrcFile[MAX_PATH] = { 0 };
	char szDstFile[MAX_PATH] = { 0 };
	snprintf(szSrcFile, sizeof(szSrcFile) - 1, "%s\\%s", m_szLogPath, pSrcFile);
	snprintf(szDstFile, sizeof(szDstFile) - 1, "%s\\%s", m_szBakLogPath, pDstFile);
	if (_access(szSrcFile, 0) != -1)
	{
		char szBakLogPath[MAX_PATH] = { 0 };
		strncpy(szBakLogPath, szDstFile, MAX_PATH - 1);
		char* pEnd = strrchr(szBakLogPath, '/');
		if (pEnd)
		{
			*pEnd = 0;
		}
		if (_access(szBakLogPath, 0) != 0)
		{
			CreatePath(szBakLogPath);
		}

		_unlink(szDstFile);
		rename(szSrcFile, szDstFile);
		_unlink(szSrcFile);
	}
}

void CLogFile::ErrorLog(const char* msg, ...)
{
	SetCurrentTime();
	va_list args;

	va_start(args, msg);
	WriteToLogFile(m_szErrLogName, msg, args);
	va_end(args);
}

void CLogFile::ErrorLogToFile(const char* pFileName, const char* msg, ...)
{
	char szDayOnFileName[LOG_FILE_NAME_LEN] = { 0 };
	SetCurrentTime();
	AddDayOnFileName(pFileName, szDayOnFileName, "_Error");

	va_list args;
	va_start(args, msg);
	WriteToLogFile(szDayOnFileName, msg, args);
	va_end(args);
}

void CLogFile::InfoLog(const char* msg, ...)
{
	if (0 == m_szInfoLogName[0])
	{
		return;
	}
	SetCurrentTime();

	va_list args;
	va_start(args, msg);
	WriteToLogFile(m_szInfoLogName, msg, args);
	va_end(args);
}

void CLogFile::InfoLogToFile(const char* pFileName, const char* msg, ...)
{
	char szDayOnFileName[LOG_FILE_NAME_LEN] = { 0 };
	SetCurrentTime();
	AddDayOnFileName(pFileName, szDayOnFileName, "_Info");
	
	va_list args;
	va_start(args, msg);
	WriteToLogFile(szDayOnFileName, msg, args);
	va_end(args);
}

void CLogFile::WriteToLogFile(const char* pFileName, const char* msg, va_list& args, char *pAddStr)
{
	LockGuard lockGuard(m_Mutex);
	char szLogFile[MAX_PATH];
	snprintf(szLogFile, sizeof(szLogFile) - 1, "%s%s", m_szLogPath, pFileName);

	FILE *pFile = NULL;
	fopen_s(&pFile, szLogFile, "a+");
	if (NULL == pFile)
	{
		char szTempLogFile[MAX_PATH];
		strncpy(szTempLogFile, szLogFile, sizeof(szTempLogFile) - 1);
		char *pTemp = strrchr(szTempLogFile, '/');
		if (pTemp == NULL)
		{
			return;
		}
		*pTemp = 0;
		if (_access(szTempLogFile, 0) != 0)
		{
			CreatePath(szTempLogFile);
		}
		fopen_s(&pFile, szLogFile, "a+");
	}
	if (pFile)
	{
		WriteLog(pFile, msg, args, pAddStr);
		fclose(pFile);
	}
}

void CLogFile::WriteLog(FILE* pFile, const char *msg, va_list& args, char* pAddStr)
{
	fprintf(pFile, "[%.4u-%.2u-%.2u,%.2u:%.2u:%.2u:%.3u] ",
		m_currentTime.ulYear, m_currentTime.ulMonth, m_currentTime.ulDay,
		m_currentTime.ulHour, m_currentTime.ulMinute,
		m_currentTime.ulSecond, m_currentTime.ulMSecond);
	if (pAddStr)
	{
		fprintf(pFile, "[%s]", pAddStr);
	}
	vfprintf(pFile, msg, args);
	//fprintf(pFile,"\n");
}

//����Log���ȼ�д���ļ�,Ĭ��д��m_szLogName
void CLogFile::WriteLogFile(int nPriority, const char* msg, ...)
{
	char* pAddStr = NULL;
	switch (nPriority)
	{
	case 1:
		pAddStr = (char*)"[info]";
		break;
	default:
		return;
	}

	SetCurrentTime();
	LockGuard lockGuard(m_Mutex);
	char szLogFile[MAX_PATH];
	snprintf(szLogFile, sizeof(szLogFile) - 1, "%s%s", m_szLogPath, m_szLogName);
	szLogFile[sizeof(szLogFile) - 1] = 0;
	FILE* pFile = NULL;
	fopen_s(&pFile, szLogFile, "a+");
	if (NULL == pFile)
	{
		char szTempLogFile[MAX_PATH];
		strncpy(szTempLogFile, szLogFile, sizeof(szTempLogFile) - 1);
		char *pTemp = strrchr(szTempLogFile, '/');
		if (pTemp == NULL)
		{
			return;
		}
		*pTemp = 0;
		if (_access(szTempLogFile, 0) != 0)
		{
			CreatePath(szTempLogFile);
		}
		fopen_s(&pFile, szLogFile, "a+");
	}
	if (pFile)
	{
		va_list   args;
		va_start(args, msg);
		WriteLog(pFile, msg, args, pAddStr);
		va_end(args);
		fclose(pFile);
	}
}

void CLogFile::LogToFile(const char* pszLogFile, const char* msg, ...)
{
	if (msg == NULL || pszLogFile == NULL)
	{
		return;
	}

	SetCurrentTime();
	va_list args;
	va_start(args, msg);
	WriteToLogFile(pszLogFile, msg, args);
	va_end(args);
}

void CLogFile::LogToFileByDay(const char* pszLogFile, const char* msg, ...)
{
	if (msg == NULL || pszLogFile == NULL)
	{
		return;
	}

	char m_szLogToFileName[MAX_PATH];
	memset(m_szLogToFileName, 0, sizeof(m_szLogToFileName));
	SetCurrentTime();
	int nRet = snprintf(m_szLogToFileName, sizeof(m_szLogToFileName) - 1, "%04u-%02u-%02u/%s",
			m_currentTime.ulYear, m_currentTime.ulMonth, m_currentTime.ulDay, pszLogFile);
	if (nRet <= 0)
	{
		return;
	}
	m_szLogToFileName[nRet] = '\0';

	va_list args;
	va_start(args, msg);
	WriteToLogFile(m_szLogToFileName, msg, args);
	va_end(args);
}

void CLogFile::InitLogCfg(const char* pszCfgFile, char* section, char* preStr, char* path)
{
	ReadCfg();
}

void CLogFile::ReadCfg()
{

}

void CLogFile::ReloadCfg()
{
	LockGuard lockGuard(m_Mutex);
	ReadCfg();
}

