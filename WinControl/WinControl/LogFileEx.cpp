#include "stdafx.h"
#include "win_linux.h"
#include "ConfigFile.h"
#include "LogFileEx.h"


#ifndef WIN32
#include <unistd.h>
#else
#include <io.h>
#include <direct.h>
#endif

#ifndef  WIN32
#ifndef MAX_PATH
#define MAX_PATH 260
#endif
#include <sys/time.h>
#endif

#ifdef WIN32
#include "base.h"
#define snprintf _snprintf
#endif


CLogFileEx::CLogFileEx()
{
	mPreStr[0] = 0;
	m_szLogPath[0] = 0;
	SetLogName("");

	memset(m_szTraceNames, 0, sizeof(m_szTraceNames));
	memset(m_szLogPath, 0, sizeof(m_szLogPath));
	memset(m_szThreadModuleName, 0, sizeof(m_szThreadModuleName));
	memset(&m_curTime, 0, sizeof(m_curTime));
	memset(m_szBakLogPath, 0, sizeof(m_szBakLogPath));
	memset(m_szLogFileName, 0, sizeof(m_szLogFileName));
	strncpy(m_szLogFileName, "tcpsvrd.log", sizeof(m_szLogFileName)-1);

	m_bIsLockLog = FALSE;

	m_iDbgLogFlag = 0;
	m_iWarnLogFlag = 0;
	m_iInfoLogFlag = 1;
	m_iBinLogFlag = 0;

	m_iThreadDbgLogFlag = 0;
	m_iThreadWarnLogFlag = 0;
	m_iThreadInfoLogFlag = 0;

	m_iShowMs = 0;
	m_nTraceNum = 0;
	pthread_mutex_init(&m_stMutex, NULL);

	m_nMaxFileSize = 16 * 1024 * 1024;
	m_nMaxFileNum = 10;
}

CLogFileEx::~CLogFileEx()
{
}

void CLogFileEx::SetShowTimeFormat(int ishowms)
{
	m_iShowMs = ishowms;
}

void CLogFileEx::SetLogPath(const char *pLogPath)
{
	if (!pLogPath)
	{
		return;
	}

	memset(m_szLogPath, 0, sizeof(m_szLogPath));
	strncpy(m_szLogPath, pLogPath, sizeof(m_szLogPath)-2);
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

	char szPath[MAX_PATH] = { 0 };
	snprintf(szPath, sizeof(szPath)-1, "%sbaklogs", m_szLogPath);
	SetBakLogPath(szPath);
}

void CLogFileEx::SetLockFlag(int bIsLockLog)
{
	m_bIsLockLog = bIsLockLog;
}

void CLogFileEx::SetMaxLogFile(unsigned int nMaxSize, unsigned int nMaxFileNum)
{
	if (0 == nMaxSize || 0 == nMaxFileNum)
	{
		return;
	}
	m_nMaxFileSize = nMaxSize;
	m_nMaxFileNum = nMaxFileNum;
}

void CLogFileEx::SetLogName(const char* pLogName)
{
	if (!pLogName)
	{
		return;
	}

	memset(m_szLogName, 0, sizeof(m_szLogName));
	memset(m_szNormalLogName, 0, sizeof(m_szNormalLogName));
	memset(m_szWarnLogName, 0, sizeof(m_szWarnLogName));
	memset(m_szErrLogName, 0, sizeof(m_szErrLogName));
	memset(m_szInfoLogName, 0, sizeof(m_szInfoLogName));
	memset(m_szLogFileName, 0, sizeof(m_szLogFileName));

	strncpy(m_szLogName, pLogName, sizeof(m_szLogName)-1);
	snprintf(m_szNormalLogName, sizeof(m_szNormalLogName)-1, "%s%s_normal.log", m_szLogName, mPreStr);
	snprintf(m_szWarnLogName, sizeof(m_szWarnLogName)-1, "%s%s_warn.log", m_szLogName, mPreStr);
	snprintf(m_szErrLogName, sizeof(m_szErrLogName)-1, "%s%s_err.log", m_szLogName, mPreStr);
	snprintf(m_szInfoLogName, sizeof(m_szInfoLogName)-1, "%s%s_info.log", m_szLogName, mPreStr);
	snprintf(m_szLogFileName, sizeof(m_szLogFileName)-1, "%s%s.log", m_szLogName, mPreStr);
}

void  CLogFileEx::SetDbgLogFlag(int iLogFlag)
{
	m_iDbgLogFlag = iLogFlag;
}

void  CLogFileEx::SetWarnLogFlag(int iErrLogFlag)
{
	m_iWarnLogFlag = iErrLogFlag;
}

void  CLogFileEx::SetInfoLogFlag(int iInfoLogFlag)
{
	m_iInfoLogFlag = iInfoLogFlag;
}

void  CLogFileEx::SetBinLogFlag(int iBinLogFlag)
{
	m_iBinLogFlag = iBinLogFlag;
}

void CLogFileEx::SetTraceNames(int nNameTraceNum, TPlayerName pszNames[MAX_TRACENUM])
{
	if (nNameTraceNum > MAX_TRACENUM)
	{
		nNameTraceNum = MAX_TRACENUM;
	}
	m_nTraceNameNum = nNameTraceNum;

	for (int i = 0; i < m_nTraceNameNum; i++)
	{
		strcpy(m_allTraceNames[i], pszNames[i]);
	}

}

void CLogFileEx::SetTraceUins(int nUinTraceNum, unsigned long long pszNames[MAX_TRACENUM])
{
	if (nUinTraceNum > MAX_TRACENUM)
	{
		nUinTraceNum = MAX_TRACENUM;
	}
	m_nTraceNum = nUinTraceNum;

	for (int i = 0; i < nUinTraceNum; i++)
	{
		m_szTraceNames[i] = pszNames[i];
	}
}

void CLogFileEx::Lock()
{
	if (m_bIsLockLog)
	{
		pthread_mutex_lock(&m_stMutex);
	}
}

void CLogFileEx::Unlock()
{
	if (m_bIsLockLog)
	{
		pthread_mutex_unlock(&m_stMutex);
	}
}

void CLogFileEx::SetCurrentTime()
{
	struct tm *tmpTime = NULL;

	if (m_iShowMs)
	{
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
		m_curTime.ulMSecond = tmNow.tv_usec / 1000;
		tmpTime = localtime(&tNow);
	}
	else
	{
		time_t tNow = time(NULL);
		tmpTime = localtime(&tNow);
	}

	m_curTime.ulYear = tmpTime->tm_year + 1900;
	m_curTime.ulMonth = tmpTime->tm_mon + 1;
	m_curTime.ulDay = tmpTime->tm_mday;
	m_curTime.ulHour = tmpTime->tm_hour;
	m_curTime.ulMinute = tmpTime->tm_min;
	m_curTime.ulSecond = tmpTime->tm_sec;
}

void CLogFileEx::BackupFile(const char* pSrcFile, const char* pDstFile)
{
	if (access(pSrcFile, 0) != -1)
	{
		char szBakLogPath[MAX_PATH] = { 0 };
		strncpy(szBakLogPath, pDstFile, MAX_PATH - 1);
		char* pEnd = strrchr(szBakLogPath, '/');
		if (pEnd)
		{
			*pEnd = 0;
		}
		if (access(szBakLogPath, 0) != 0)
		{
			CreatePath(szBakLogPath);
		}

		unlink(pDstFile);
		rename(pSrcFile, pDstFile);
		unlink(pSrcFile);
	}
}

void CLogFileEx::GetBakFileName(const char* pFileName, char szBakFileName[MAX_PATH])
{
	time_t tmOld = 0xFFFFFFFF;
	memset(szBakFileName, 0, MAX_PATH);
	for (unsigned int i = 0; i < m_nMaxFileNum; i++)
	{
		time_t tmTemp = 0;
		char szTempName[MAX_PATH];
		memset(szTempName, 0, sizeof(szTempName));
		snprintf(
			szTempName, sizeof(szTempName)-1,
			"%s%s_%d.bak",
			m_szBakLogPath, pFileName, i + 1
			);
		struct stat buf;
		if (stat(szTempName, &buf) != 0)
		{
			tmTemp = 0;
			strncpy(szBakFileName, szTempName, MAX_PATH - 1);
			return;
		}
		else
		{
			tmTemp = buf.st_mtime;
		}
		if (tmTemp < tmOld)
		{
			tmOld = tmTemp;
			strncpy(szBakFileName, szTempName, MAX_PATH - 1);
		}
	}
}

void CLogFileEx::BakLogFile(const char* pFileName)
{
	char szLogFile[MAX_PATH];
	sprintf(szLogFile, "%s%s", m_szLogPath, pFileName);

	time_t tmNow = time(NULL);
	unsigned int unFileSize = 0;
	struct stat buf;
	if (stat(szLogFile, &buf) != 0)
	{
		return;
	}
	if (buf.st_size < (int)m_nMaxFileSize)
	{
		return;
	}
	char szBakFileName[MAX_PATH];
	GetBakFileName(pFileName, szBakFileName);
	BackupFile(szLogFile, szBakFileName);
}

void CLogFileEx::WriteLog(FILE* pFile, const char *msg, va_list& args, char* pAddStr)
{
	if (m_iShowMs)
	{
		fprintf(pFile, "[%.4u-%.2u-%.2u,%.2u:%.2u:%.2u:%.3u] ",
			m_curTime.ulYear,
			m_curTime.ulMonth,
			m_curTime.ulDay,
			m_curTime.ulHour,
			m_curTime.ulMinute,
			m_curTime.ulSecond,
			m_curTime.ulMSecond);
	}
	else
	{
		fprintf(pFile, "[%.4u-%.2u-%.2u,%.2u:%.2u:%.2u] ",
			m_curTime.ulYear,
			m_curTime.ulMonth,
			m_curTime.ulDay,
			m_curTime.ulHour,
			m_curTime.ulMinute,
			m_curTime.ulSecond);
	}
	if (pAddStr)
	{
		fprintf(pFile, " %s ", pAddStr);
	}

	vfprintf(pFile, msg, args);
	//fprintf(pFile,"\n");
}

void CLogFileEx::WriteNormalLog(const char* msg, va_list& args, char* pStrAdd)
{
	if (m_iDbgLogFlag == 0)
	{
		return;
	}
	WriteToLogFile(m_szNormalLogName, msg, args, pStrAdd);
}

void CLogFileEx::WriteToLogFile(const char* pFileName, const char* msg, va_list& args, char *pAddStr)
{
	//write之前先考虑备份，备份机制统一考虑。
	Lock();
	char szLogFile[MAX_PATH];
	sprintf(szLogFile, "%s%s", m_szLogPath, pFileName);

	BakLogFile(pFileName);

	FILE *pFile = fopen(szLogFile, "a+");
	if (NULL == pFile)
	{
		char szTempLogFile[MAX_PATH];
		strncpy(szTempLogFile, szLogFile, sizeof(szTempLogFile)-1);
		char *pTemp = strrchr(szTempLogFile, '/');
		if (pTemp == NULL)
		{
			Unlock();
			return;
		}
		*pTemp = 0;
		if (access(szTempLogFile, 0) != 0)
		{
			CreatePath(szTempLogFile);
		}
		pFile = fopen(szLogFile, "a+");
	}
	if (pFile)
	{
		WriteLog(pFile, msg, args, pAddStr);
		fclose(pFile);
	}
	Unlock();
}

void CLogFileEx::WriteLogFile(int nPriority, const char* msg, ...)
{
	char* pAddStr = NULL;
	switch (nPriority)
	{
	case KGLOG_DEBUG:
		if (m_iDbgLogFlag == 0)
		{
			return;
		}
		break;
	case KGLOG_WARNING:
		if (m_iWarnLogFlag == 0)
		{
			return;
		}
		pAddStr = (char*)"#warn";
		break;
	case KGLOG_INFO:
		if (m_iInfoLogFlag == 0)
		{
			return;
		}
		pAddStr = (char*)"#info#";
		break;
	case KGLOG_ERR:
		pAddStr = (char*)"#error#";
		break;
	default:
		return;
	}
	SetCurrentTime();

	//write之前先考虑备份，备份机制统一考虑。
	Lock();
	char szLogFile[MAX_PATH];
	snprintf(szLogFile, sizeof(szLogFile)-1, "%s%s", m_szLogPath, m_szNormalLogName);
	szLogFile[sizeof(szLogFile)-1] = 0;
	BakLogFile(m_szLogFileName);
	FILE *pFile = fopen(szLogFile, "a+");
	if (NULL == pFile)
	{
		char szTempLogFile[MAX_PATH];
		strncpy(szTempLogFile, szLogFile, sizeof(szTempLogFile)-1);
		char *pTemp = strrchr(szTempLogFile, '/');
		if (pTemp == NULL)
		{
			Unlock();
			return;
		}
		*pTemp = 0;
		if (access(szTempLogFile, 0) != 0)
		{
			CreatePath(szTempLogFile);
		}
		pFile = fopen(szLogFile, "a+");
	}
	if (pFile)
	{
		va_list   args;
		va_start(args, msg);
		WriteLog(pFile, msg, args, pAddStr);
		va_end(args);
		fclose(pFile);
	}
	Unlock();

}

bool CLogFileEx::IsDbgLog()
{
	return m_iDbgLogFlag != 0;
}

void CLogFileEx::DbgLog(const char* msg, ...)
{
	if (0 == m_iDbgLogFlag) /*如果无需记录则返回*/
	{
		return;
	}

	SetCurrentTime();
	va_list   args;
	va_start(args, msg);
	WriteNormalLog(msg, args);
	va_end(args);
}

void CLogFileEx::WarnLog(const char* msg, ...)
{
	if (0 == m_iWarnLogFlag) /*如果WarnLog标志没有打开，则返回*/
	{
		return;
	}
	SetCurrentTime();

	va_list   args;

	va_start(args, msg);
	WriteNormalLog(msg, args, (char*)"#warn#");
	va_end(args);

	va_start(args, msg);
	WriteToLogFile(m_szWarnLogName, msg, args);
	va_end(args);
}

void CLogFileEx::ErrLog(const char* msg, ...)
{
	SetCurrentTime();
	va_list args;

	va_start(args, msg);
	WriteNormalLog(msg, args, (char*)"#error#");
	va_end(args);

	va_start(args, msg);
	WriteToLogFile(m_szErrLogName, msg, args);
	va_end(args);
}

void CLogFileEx::BinLog(const char* pszFileName, char *pBuffer, unsigned int iLength)
{
	if (0 == m_iBinLogFlag) /*如果BinLog标志没有打开，则返回*/
	{
		return;
	}
	SetCurrentTime();

	if (pszFileName == NULL || pBuffer == NULL)
	{
		return;
	}
	if (iLength >= MAX_BIN_LEN)
	{
		iLength = MAX_BIN_LEN;
	}
	char tmpBuffer[2 * MAX_BIN_LEN + 1];
	char strTemp[64];

	char szLogFile[MAX_PATH] = { 0 };
	sprintf(szLogFile, "%sbinlog/%04u_%02u_%02u"
		"/%02u_%s",
		m_szLogPath, m_curTime.ulYear, m_curTime.ulMonth, m_curTime.ulDay
		, m_curTime.ulHour, pszFileName);

	Lock();

	FILE *pFile = fopen(szLogFile, "a+");
	if (pFile == NULL)
	{
		char szPath[MAX_PATH] = { 0 };
		sprintf(szPath, "%sbinlog/%04u_%02u_%02u",
			m_szLogPath, m_curTime.ulYear, m_curTime.ulMonth, m_curTime.ulDay);
		CreatePath(szPath);

		pFile = fopen(szLogFile, "a+");
		if (pFile == NULL)
		{
			Unlock();
			return;
		}
	}

	tmpBuffer[0] = 0;
	for (unsigned int i = 0; i < iLength; i++)
	{
		if (!(i % 16))
		{
			sprintf(strTemp, "\n%04d>    ", i / 16 + 1);
			strcat(tmpBuffer, strTemp);
		}
		sprintf(strTemp, "%02X ", (unsigned char)pBuffer[i]);
		strcat(tmpBuffer, strTemp);
	}
	strcat(tmpBuffer, "\n");
	fprintf(pFile, "\n***************[time %02u:%02u:%02u] bufferlen %u***************",
		m_curTime.ulHour, m_curTime.ulMinute, m_curTime.ulSecond, iLength);
	fprintf(pFile, tmpBuffer);

	fclose(pFile);
	Unlock();
}


void CLogFileEx::DbgBinLog(char *pBuffer, unsigned int iLength)
{
	if (0 == m_iBinLogFlag) /*如果BinLog标志没有打开，则返回*/
	{
		return;
	}
	SetCurrentTime();

	if (pBuffer == NULL)
	{
		return;
	}
	if (iLength >= MAX_BIN_LEN)
	{
		iLength = MAX_BIN_LEN;
	}
	char tmpBuffer[4 * MAX_BIN_LEN + 1];
	char strTemp[64];

	char szLogFile[MAX_PATH] = { 0 };


	sprintf(szLogFile, "%s%s", m_szLogPath, m_szNormalLogName);


	Lock();

	FILE *pFile = fopen(szLogFile, "a+");
	if (pFile == NULL)
	{
		CreatePath(m_szLogPath);

		pFile = fopen(szLogFile, "a+");
		if (pFile == NULL)
		{
			Unlock();
			return;
		}
	}

	tmpBuffer[0] = 0;
	for (unsigned int i = 0; i < iLength; i++)
	{
		if (!(i % 16))
		{
			sprintf(strTemp, "\n%04d>    ", i / 16 + 1);
			strcat(tmpBuffer, strTemp);
		}
		sprintf(strTemp, "%02X ", (unsigned char)pBuffer[i]);
		strcat(tmpBuffer, strTemp);
	}
	strcat(tmpBuffer, "\n");
	//fprintf(pFile, "\n***************[time %02u:%02u:%02u] bufferlen %u***************", 
	//	m_curTime.ulHour, m_curTime.ulMinute, m_curTime.ulSecond, iLength);
	fprintf(pFile, tmpBuffer);

	fclose(pFile);
	Unlock();
}


void CLogFileEx::InfoLog(const char* msg, ...)
{
	if (0 == m_iInfoLogFlag) /*如果InfoLog标志没有打开，则返回*/
	{
		return;
	}
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

void CLogFileEx::LogToFile(const char* pszLogFile, const char* msg, ...)
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

void CLogFileEx::LogToFileByDay(const char* pszLogFile, const char* msg, ...)
{
	if (msg == NULL || pszLogFile == NULL)
	{
		return;
	}

	SetCurrentTime();
	int nRet = snprintf(
		m_szLogToFileName, sizeof(m_szLogToFileName)-1, "%04u-%02u-%02u/%s",
		m_curTime.ulYear, m_curTime.ulMonth, m_curTime.ulDay, pszLogFile
		);
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

void CLogFileEx::TraceUinLog(unsigned long long pszName, const char* msg, ...)
{
	if (m_nTraceNum >= 0)
	{
		int i;
		for (i = 0; i < m_nTraceNum; i++)
		{
			if (pszName == m_szTraceNames[i])
			{
				break;
			}
		}
		if (i == m_nTraceNum)
		{
			return;
		}
	}

	SetCurrentTime();
	va_list args;
	va_start(args, msg);
	char szLogFile[MAX_PATH];
	memset(szLogFile, 0, sizeof(szLogFile));
	snprintf(szLogFile, sizeof(szLogFile)-1,
		"player/trace_%llu_%04u_%02u_%02u.log", pszName, m_curTime.ulYear, m_curTime.ulMonth, m_curTime.ulDay);
	WriteToLogFile(szLogFile, msg, args);
	va_end(args);
}

void CLogFileEx::TraceNameLog(const char* pszName, const char* msg, ...)
{
	if (m_nTraceNum >= 0)
	{
		int i;
		for (i = 0; i < m_nTraceNum; i++)
		{
			if (strcmp(pszName, m_allTraceNames[i]) == 0)
			{
				break;
			}
		}
		if (i == m_nTraceNum)
		{
			return;
		}
	}

	SetCurrentTime();
	va_list args;
	va_start(args, msg);
	char szLogFile[MAX_PATH];
	memset(szLogFile, 0, sizeof(szLogFile));
	snprintf(szLogFile, sizeof(szLogFile)-1,
		"player/trace_%s_%04u_%02u_%02u.log", pszName, m_curTime.ulYear, m_curTime.ulMonth, m_curTime.ulDay);
	WriteToLogFile(szLogFile, msg, args);
	va_end(args);
}

void CLogFileEx::SetBakLogPath(const char *pBakLogPath)
{
	if (!pBakLogPath)
	{
		return;
	}
	memset(m_szBakLogPath, 0, sizeof(m_szBakLogPath));
	strncpy(m_szBakLogPath, pBakLogPath, sizeof(m_szBakLogPath)-2);
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
}

void CLogFileEx::InitLogCfg(const char* pszCfgFile, char* section, char* preStr, char* path)
{
	SAFE_strcpy(mSection, section);
	if (preStr != NULL)
	{
		SAFE_strcpy(mPreStr, preStr);
	}
	if (path != NULL)
	{
		SetLogPath(path);
	}
	SAFE_strcpy(mLogCfgFile, pszCfgFile);
	ReadCfg();
}

void CLogFileEx::ReloadCfg()
{
	pthread_mutex_lock(&m_stMutex);
	ReadCfg();
	pthread_mutex_unlock(&m_stMutex);
}

void CLogFileEx::ReadCfg()
{
	char* section = mSection;

	//初始化配置
	CConfigFile configwrap(mLogCfgFile);
	configwrap.GetInt(section, "dbgflag", m_ZoneLogCfg.DdbLogFlag, 0);
	configwrap.GetInt(section, "dumpflag", m_ZoneLogCfg.DumpLogFlag, 0);
	configwrap.GetInt(section, "warnflag", m_ZoneLogCfg.WarnLogFlag, 0);
	configwrap.GetInt(section, "infoflag", m_ZoneLogCfg.InfoLogFlag, 0);
	configwrap.GetInt(section, "binflag", m_ZoneLogCfg.BinLogFlag, 0);
	configwrap.GetInt(section, "ansignflag", m_ZoneLogCfg.AssingLogFlag, 0);
	configwrap.GetInt(section, "lockflag", m_ZoneLogCfg.LockFlag, 0);
	configwrap.GetInt(section, "showms", m_ZoneLogCfg.ShowMs, 0);
	configwrap.GetString(section, "logname", m_ZoneLogCfg.m_szLogName, sizeof(m_ZoneLogCfg.m_szLogName), "");
	configwrap.GetInt(section, "maxfilesize", (int&)m_ZoneLogCfg.unMaxFileSize, 1024 * 1024);
	configwrap.GetInt(section, "maxfilenum", (int&)m_ZoneLogCfg.unMaxFileNum, 8);
	configwrap.GetInt(section, "tracenum", m_ZoneLogCfg.TraceNum, 0);
	for (int i = 0; i < m_ZoneLogCfg.TraceNum; i++)
	{
		char szTemp[100] = { 0 };
		sprintf(szTemp, "trace%d", i + 1);
		configwrap.GetLongLong(section, szTemp, (long long&)m_ZoneLogCfg.Traceuins[i], 0);
	}

	configwrap.GetInt(section, "tracenamenum", m_ZoneLogCfg.TraceNameNum, 0);
	for (int i = 0; i < m_ZoneLogCfg.TraceNameNum; i++)
	{
		char szTemp[100] = { 0 };
		sprintf(szTemp, "tracename%d", i + 1);
		configwrap.GetString(section, szTemp, m_ZoneLogCfg.Tracenames[i], sizeof(m_ZoneLogCfg.Tracenames[i]), "");
	}

	SetMaxLogFile(m_ZoneLogCfg.unMaxFileSize, m_ZoneLogCfg.unMaxFileNum);
	SetDbgLogFlag(m_ZoneLogCfg.DdbLogFlag);
	SetWarnLogFlag(m_ZoneLogCfg.WarnLogFlag);
	SetInfoLogFlag(m_ZoneLogCfg.InfoLogFlag);
	SetBinLogFlag(m_ZoneLogCfg.BinLogFlag);
	SetShowTimeFormat(m_ZoneLogCfg.ShowMs);
	SetLockFlag(m_ZoneLogCfg.LockFlag);
	SetLogName(m_ZoneLogCfg.m_szLogName);
	SetTraceUins(m_ZoneLogCfg.TraceNum, m_ZoneLogCfg.Traceuins);

	if (m_szLogPath[0] == 0)
	{
		char logPath[2 * MAX_PATH];
		configwrap.GetString(section, "LogPath", logPath, sizeof(logPath)-1, "../log/defaultlog");
		SetLogPath(logPath);
	}

	InfoLog(
		"========logconfig========\n dbgflag %d dumpflag %d warnflag %d infoflag %d assingflag %d maxfilesize %d maxfilenum %d lockflag %d tracenum %d \n",
		m_ZoneLogCfg.DdbLogFlag, m_ZoneLogCfg.DumpLogFlag, m_ZoneLogCfg.WarnLogFlag,
		m_ZoneLogCfg.InfoLogFlag, m_ZoneLogCfg.AssingLogFlag, m_ZoneLogCfg.unMaxFileSize,
		m_ZoneLogCfg.unMaxFileNum, m_ZoneLogCfg.LockFlag, m_ZoneLogCfg.TraceNum
		);
	for (int i = 0; i < m_ZoneLogCfg.TraceNum; i++)
	{
		char szTemp[100] = { 0 };
		sprintf(szTemp, "trace%d", i + 1);
		InfoLog("%s is %u \n", szTemp, m_ZoneLogCfg.Traceuins[i]);
	}
}


