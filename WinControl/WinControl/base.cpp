#include "stdafx.h"
#include <io.h>
#include <atlconv.h>
#include <direct.h>
#include "ServerPublic.h"
//#include "../zip/zlib.h"
#include "base.h"
//#include "PacketID.h"
//#include "ErrorID.h"

//int EncodeInt(char **pstrEncode, unsigned int uiSrc)
//{
//	unsigned int uiTemp;
//
//	if (pstrEncode == nullptr || *pstrEncode == nullptr)
//	{
//		return 0;
//	}
//
//	uiTemp = htonl(uiSrc);
//
//	memcpy((void *)(*pstrEncode), (const void *)&uiTemp, sizeof(unsigned int));
//	*pstrEncode += sizeof(unsigned int);
//
//	return (int)sizeof(unsigned int);
//}

//int DecodeInt(char **pstrDecode, unsigned int *puiDest)
//{
//	unsigned int uiTemp;
//
//	if (pstrDecode == nullptr || *pstrDecode == nullptr || puiDest == nullptr)
//	{
//		return 0;
//	}
//
//	memcpy((void *)&uiTemp, (const void *)(*pstrDecode), sizeof(unsigned int));
//	*pstrDecode += sizeof(unsigned int);
//
//	*puiDest = ntohl(uiTemp);
//
//	return (int)sizeof(unsigned int);
//}

//int EncodeString(char **pstrEncode, char *strSrc, short sMaxStrLength)
//{
//	unsigned short usTemp;
//	unsigned short usTempLength;
//
//	if (pstrEncode == nullptr || *pstrEncode == nullptr || strSrc == nullptr || sMaxStrLength <= 0)
//	{
//		return 0;
//	}
//
//	usTempLength = (unsigned short)strlen(strSrc);
//	if (usTempLength > sMaxStrLength)
//	{
//		usTempLength = sMaxStrLength;
//	}
//
//	usTemp = htons(usTempLength);
//
//	memcpy((void *)(*pstrEncode), (const void *)&usTemp, sizeof(unsigned short));
//	*pstrEncode += sizeof(unsigned short);
//
//	memcpy((void *)(*pstrEncode), (const void *)strSrc, usTempLength);
//	*pstrEncode += usTempLength;
//
//	return (usTempLength + sizeof(unsigned short));
//}
//int DecodeString(char **pstrDecode, char *strDest, short sMaxStrLength)
//{
//	unsigned short usTemp;
//	unsigned short usTempLength;
//	unsigned short usRealLength;
//
//	if (pstrDecode == nullptr || *pstrDecode == nullptr || strDest == nullptr || sMaxStrLength <= 0)
//	{
//		return 0;
//	}
//
//	memcpy((void *)&usTemp, (const void *)(*pstrDecode), sizeof(unsigned short));
//	*pstrDecode += sizeof(unsigned short);
//
//	usTempLength = ntohs(usTemp);
//
//	if (usTempLength > sMaxStrLength)
//	{
//		usRealLength = sMaxStrLength;
//	}
//	else
//	{
//		usRealLength = usTempLength;
//	}
//	memcpy((void *)strDest, (const void *)(*pstrDecode), usRealLength);
//	*pstrDecode += usTempLength;
//
//	strDest[usRealLength] = '\0';
//	return (usTempLength + sizeof(unsigned short));
//}

//bool LoadCompressZipFile(const char* filename, unsigned char* outbuf, int& outlen)
//{
//	//unsigned char inbuf[MAXBUFFSIZE] = { 0, };
//	unsigned char inbuf[1024] = { 0, };
//	int inlen = sizeof(inbuf);
//	if (LoadBinaryFile(filename, inbuf, inlen) == false)
//	{
//		return false;
//	}
//	int err = compress(outbuf, (unsigned int *)&outlen, inbuf, inlen);
//	if (err == 0)
//	{
//		return true;
//	}
//	return false;
//}

//bool Compress(const char* inbuffer, int inlen, char* outbuffer, unsigned int& outlen)
//{	
//	int err = compress((unsigned char*)outbuffer, &outlen, (unsigned char*)inbuffer, inlen);
//	if (err == 0)
//	{
//		return true;
//	}
//	return false;
//}

unsigned char* CreateShareMem(char* name, size_t iSize)
{
#ifdef WIN32
	USES_CONVERSION;

	char lpszEngineName[1024];
	sprintf_s(lpszEngineName, "%s", name);

	int m_nIndex = 0;

	BOOL				bReturn = FALSE;
	int					nMemSize = 0;
	SECURITY_ATTRIBUTES	sa = { 0 };
	SECURITY_DESCRIPTOR	sd = { 0 };
	int					nRet = 0;
	int					i = 0;


	DWORD iBufferSize = (DWORD)iSize;

	bool bMemExist;
	HANDLE hFileMapping;


	// 1 �޸ı����̵ķ���Ȩ��
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, TRUE, NULL, TRUE);
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = FALSE;
	sa.lpSecurityDescriptor = &sd;

	// 2 �����������
	HANDLE hMutex;
	TCHAR sEngineName[MAX_PATH] = { 0 };

	printf_s(sEngineName, _T("%s_BBB"), A2T((LPSTR)lpszEngineName));
	hMutex = ::CreateMutex(&sa, FALSE, sEngineName);
	nRet = GetLastError();

	// 3 ���������ڴ�
	hFileMapping = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		&sa,
		PAGE_READWRITE,
		0,
		iBufferSize,
		A2T((LPSTR)lpszEngineName)
		);

	nRet = GetLastError();
	if ((INVALID_HANDLE_VALUE == hFileMapping) || (NULL == hFileMapping))
	{
		CloseHandle(hFileMapping);
		hFileMapping = INVALID_HANDLE_VALUE;
		return NULL;
	}

	unsigned char* pShareMem = (unsigned char*)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (NULL == pShareMem)
	{
		DWORD dwErr = GetLastError();
		return NULL;
	}
	#define LOCK_WAIT_TIME 1000L
	if (::WaitForSingleObject(hMutex, LOCK_WAIT_TIME) != WAIT_OBJECT_0)
	{
		::ReleaseMutex(hMutex);
		printf(("�ȴ��ź�[%llu]ʧ��:error=[%lld]\n"), (unsigned long long)hMutex, (long long)GetLastError());
		return NULL;
	}

	// 4 �����鹲���ڴ����´�����,�����ж�ָ������Ϊ-1,���Լ��Ķ�ָ��ָ��0λ����
	if (ERROR_SUCCESS == nRet)
	{
		memset(pShareMem, 0, iBufferSize);
		bMemExist = false;
	}
	// �������ڴ����Ѿ������ģ�����ֻ�Ǵ򿪣����Լ��Ķ�ָ������Ϊдָ��
	else if (ERROR_ALREADY_EXISTS == nRet)
	{
		bMemExist = true;
	}

	::ReleaseMutex(hMutex);
	return pShareMem;
#else
	key_t iKey = keyValue;
	size_t iTempShmSize;
	int iShmID;

	iTempShmSize = iSize;

	iShmID = shmget(iKey, iTempShmSize, IPC_CREAT | IPC_EXCL | 0666);
	if (iShmID < 0)
	{
		//����ʧ�ܣ��������Ѿ��й����ڴ��ˣ����Կ�����ʲôԭ��ʧ��
		if (errno != EEXIST)
		{
			printf("Alloc share memory failed, %s\n", strerror(errno));
			return NULL;
		}
		//��������ڴ��Ѿ�����
		iShmID = shmget(iKey, iTempShmSize, 0666);
		if (iShmID < 0)
		{
			//���attachʧ�ܣ�������size��֮ǰ�Ĵ�������remove֮ǰ�Ĺ����ڴ�
			//LOG("Attach to share memory %d failed, %s. Now try to touch it\n", iShmID, strerror(errno));
			iShmID = shmget(iKey, 0, 0666);
			if (iShmID < 0)
			{
				printf("Fatel error, touch to shm failed, %s.\n", strerror(errno));
				return NULL;
			}
			else
			{
				//LOG("First remove the exist share memory %d\n", iShmID);
				if (shmctl(iShmID, IPC_RMID, NULL))
				{
					//LOG("Remove share memory failed, %s\n", strerror(errno));
					return NULL;
				}
				iShmID = shmget(iKey, iTempShmSize, IPC_CREAT | IPC_EXCL | 0666);
				if (iShmID < 0)
				{
					//LOG("Fatal error, alloc share memory failed, %s\n", strerror(errno));
					return NULL;
				}
				else
				{
					//remove�ɹ��󴴽������ڴ�ɹ�
					//bInitMode = true;
				}
			}
		}
	}
	else
	{
		//���������ڴ�ɹ�
		//bInitMode = true;
	}
	//LOG("Successfully alloced share memory block, key = %08X, id = %d, size = %d\n", iKey, iShmID, iTempShmSize);
	return (unsigned char *)shmat(iShmID, NULL, 0);
#endif	
}

void TrimStr(char *strInput)
{
	char *pb;
	char *pe;
	size_t iTempLength;

	if (strInput == NULL)
	{
		return;
	}

	iTempLength = strlen(strInput);
	if (iTempLength == 0)
	{
		return;
	}

	pb = strInput;

	while (((*pb == ' ') || (*pb == '\t') || (*pb == '\n') || (*pb == '\r')) && (*pb != 0))
	{
		pb++;
	}

	pe = &strInput[iTempLength - 1];
	while ((pe >= pb) && ((*pe == ' ') || (*pe == '\t') || (*pe == '\n') || (*pe == '\r')))
	{
		pe--;
	}

	pe++;
	*pe = '\0';

	size_t nMoveLen = pe - pb + 1;
	if (strInput != pb)
	{
		memmove(strInput, pb, nMoveLen);
	}
}

void SetExeCurrPath(char* pszExeName)
{
#ifdef WIN32
	char* pPath = strrchr(pszExeName, '\\');
	if (pPath != NULL)
	{
		*pPath = 0;
		SetCurrentDirectory(pszExeName);
		*pPath = '\\';
	}
#else
	char* pPath = strrchr(pszExeName, '/');
	if (pPath != NULL)
	{
		*pPath = 0;
		if (chdir((const char *)pszExeName))
		{
			printf("Can't change run dir to %s, exit.\n", pszExeName);
		}
		*pPath = '/';
	}
#endif
}

char* GetTimeString_r(time_t timer, char *strTime)
{
	struct tm stTempTm;
	localtime_s(&stTempTm, &timer);
	sprintf_s(strTime, 20, "%04d-%02d-%02d %02d:%02d:%02d", stTempTm.tm_year + 1900, stTempTm.tm_mon + 1, stTempTm.tm_mday, stTempTm.tm_hour, stTempTm.tm_min, stTempTm.tm_sec);
	return strTime;
}

UINT64 GetTimeDes(time_t timer)
{
	char strTime[20];
	struct tm stTempTm;
	localtime_s(&stTempTm, &timer);
	sprintf_s(strTime, "%04d%02d%02d%02d%02d%02d", stTempTm.tm_year + 1900, stTempTm.tm_mon + 1, stTempTm.tm_mday, stTempTm.tm_hour, stTempTm.tm_min, stTempTm.tm_sec);
    UINT64 timerdes = _atoi64(strTime);
	return timerdes;
}

void CreatePath(char szLogPath[MAX_PATH])
{
	if (0 == szLogPath[0])
	{
		printf("CreatePath Error!\n");
		return;
	}
	char szTempLogPath[MAX_PATH];
	memset(szTempLogPath, 0, sizeof(szTempLogPath));
	strncpy_s(szTempLogPath, szLogPath, sizeof(szTempLogPath)-1);
	int iTempLen = strlen(szTempLogPath);

	if ('/' == szTempLogPath[iTempLen - 1])
	{
		szTempLogPath[iTempLen - 1] = 0;
	}
	char *pTemp = szTempLogPath;
	char *pTemp1;
	char chTemp;
	while (1)
	{
		pTemp1 = strchr(pTemp, '/');
		if (0 == pTemp1)
		{
			_mkdir(szTempLogPath);
			break;
		}
		if (0 == *(pTemp1 + 1))
		{
			break;
		}

		chTemp = *(pTemp1 + 1);
		*(pTemp1 + 1) = 0;
		if (0 != strcmp("../", szTempLogPath) && 0 != strcmp("./", szTempLogPath))
		{
			_mkdir(szTempLogPath);
		}
		*(pTemp1 + 1) = chTemp;
		pTemp = pTemp1 + 1;
	}
}

int GetDateString(char *strTime)
{
	struct tm *pTempTm = NULL;
	time_t timer;

	if (!strTime)
	{
		return -1;
	}

	time(&timer);

	strTime[0] = '\0';

	localtime_s(pTempTm, &timer);

	if (!pTempTm)
	{
		return -1;
	}

	sprintf_s(strTime, 15, "%04d-%02d-%02d", pTempTm->tm_year + 1900, pTempTm->tm_mon + 1, pTempTm->tm_mday);

	return 0;
}

int GetTimeStringOfDay(char *strTime)
{
	struct tm *pTempTm = NULL;
	time_t timer;

	if (!strTime)
	{
		return -1;
	}

	time(&timer);

	strTime[0] = '\0';

	localtime_s(pTempTm, &timer);

	if (!pTempTm)
	{
		return -1;
	}

	sprintf_s(strTime, 15, "%02d:%02d:%02d", pTempTm->tm_hour, pTempTm->tm_min, pTempTm->tm_sec);

	return 0;
}

int GetStructTime(time_t tTime, TStruTime *pstTime)
{	
	struct tm stTempTm = {0};
	localtime_s(&stTempTm, &tTime);

	pstTime->m_iYear = stTempTm.tm_year + 1900;
	pstTime->m_iMon = stTempTm.tm_mon + 1;
	pstTime->m_iDay = stTempTm.tm_mday;
	pstTime->m_iHour = stTempTm.tm_hour;
	pstTime->m_iMin = stTempTm.tm_min;
	pstTime->m_iSec = stTempTm.tm_sec;
	pstTime->m_iMSec = 0;

	return 0;
}


int GetCurStructTime(TStruTime *pstTime)
{
	time_t tTempNow;

	time(&tTempNow);

	return GetStructTime(tTempNow, pstTime);
}

int GetTimeStruct(time_t tTime, TStruTime *pstTime)
{
	time_t tTempNow;
	tTempNow = tTime;
	return GetStructTime(tTempNow, pstTime);
}

//const char* GetServerTypeName(int servertype)
//{
//	assert(SVRTP_MAX == 15);
//	switch (servertype)
//	{	
//	case SVRTP_CTR:
//		return "SVRTP_CTR";
//	case SVRTP_DB:
//		return "SVRTP_DB";
//	case SVRTP_NAT:
//		return "SVRTP_NAT";
//	case SVRTP_LOGIN:
//		return "SVRTP_LOGIN";
//	case SVRTP_LOBBY:
//		return "SVRTP_LOBBY";
//	case SVRTP_PROXY:
//		return "SVRTP_PROXY";
//	case SVRTP_TOOL:
//		return "SVRTP_TOOL";
//	case SVRTP_GAME:
//		return "SVRTP_GAME";
//	case SVRTP_LGDB:
//		return "SVRTP_LGDB";
//	case SVRTP_BKDB:
//		return "SVRTP_BKDB";
//	case SVRTP_WEB:
//		return "SVRTP_WEB";
//	case SVRTP_SAFEKEY:
//		return "SVRTP_SAFEKEY";
//	case SVRTP_RECORD: //ս��������
//		return "SVRTP_RECORD";	
//	case SVRTP_GWCTRL: //���ط���������������������
//		return "SVRTP_GWCTRL";
//	default:
//		return "DEFAULT_SERVER";
//	}
//}

//char* GetIPString(unsigned int ip)
//{
//	struct in_addr sin_addr;
//	sin_addr.S_un.S_addr = htonl(ip);	
//	return inet_ntoa(sin_addr);
//}

static bool sAlloc32GuidInit = false;
static CRITICAL_SECTION sAlloc32GuidLock;
static unsigned int sGuidValue = 0;

void Alloc32GuidInit()
{
	sAlloc32GuidInit = true;
	InitializeCriticalSection(&sAlloc32GuidLock);
}

unsigned int Alloc32GuidEx()
{
	assert(sAlloc32GuidInit);
	EnterCriticalSection(&sAlloc32GuidLock);	
	sGuidValue++;
	if (sGuidValue == 0)
	{
		sGuidValue++;
	}
	LeaveCriticalSection(&sAlloc32GuidLock);
	return sGuidValue;
}

int SplitMapStr(const char *pMapStr, int iMaxItemNum, int *pItemArray, char szGap /* = ',' */)
{
	char szTempStr[10240];
	char *pTemp1 = NULL;
	char *pTemp2 = NULL;
	char *pTemp3 = NULL;
	char *pTemp4 = NULL;
	int iStart;
	int iStop, i;
	int iTempCount = 0;
	size_t mapstrlen;

	if (!pMapStr || !pItemArray || iMaxItemNum < 0)
	{
		return -1;
	}

	mapstrlen = strlen(pMapStr);
	if (mapstrlen >= 10240)
	{
		return -1;
	}

	int iLength = 0;
	memset(szTempStr, '\0', sizeof(szTempStr));
	for (size_t i = 0; i < mapstrlen; i++)
	{
		if (*(pMapStr + i) == ' ' || *(pMapStr + i) == '\t')
		{
			continue;
		}
		szTempStr[iLength] = *(pMapStr + i);
		iLength++;
	}
	pTemp1 = &szTempStr[0];
	if (strchr(pTemp1, ' '))
	{
		printf("Error in put map str, contained tab space\n");
		return -1;
	}
	
	for (int i = 0; i < iLength; i++)
	{
		if ((pTemp1[i] > '9' || pTemp1[i] < '0') && pTemp1[i] != szGap && pTemp1[i] != '-')
		{
			printf("Error in put map str, contained error char\n");
			return -1;
		}
	}
	while (pTemp2 = strchr(pTemp1, szGap))
	{
		if (iTempCount == iMaxItemNum)
		{
			return iTempCount;
		}
		*pTemp2 = '\0';
		if (strlen(pTemp1) == 0)
		{
			printf("Error:Split Map Str Failed, str in null or space\n");
			return -1;
		}

		if (pTemp3 = strchr(pTemp1, '-'))
		{
			*pTemp3 = '\0';
			iStart = atoi(pTemp1);
			iStop = atoi(pTemp3 + 1);
			if (iStop < iStart)
			{
				printf("Error:Split Map Str Failed, Range start big(%d) than stop(%d)\n", iStart, iStop);
				return -1;
			}

			for (i = iStart; i <= iStop && iTempCount < iMaxItemNum; i++)
			{
				pItemArray[iTempCount] = i;
				//LOG("Item[%d]= %d\n", iTempCount, i);
				iTempCount++;

			}
		}
		else
		{
			pItemArray[iTempCount] = atoi(pTemp1);
			//LOG("Item[%d]= %d\n", iTempCount, pItemArray[iTempCount] );
			iTempCount++;

		}
		pTemp1 = pTemp2 + 1;
	}
	if (pTemp1 && iTempCount < iMaxItemNum)
	{
		if (pTemp3 = strchr(pTemp1, '-'))
		{
			*pTemp3 = '\0';
			iStart = atoi(pTemp1);
			iStop = atoi(pTemp3 + 1);
			if (iStop < iStart)
			{
				printf("Error:Split Map Str Failed, Range start big(%d) than stop(%d)\n", iStart, iStop);
				return -1;
			}

			for (i = iStart; i <= iStop && iTempCount < iMaxItemNum; i++)
			{
				pItemArray[iTempCount] = i;
				//LOG("Item[%d]= %d\n", iTempCount, i);
				iTempCount++;
			}
		}
		else
		{
			pItemArray[iTempCount] = atoi(pTemp1);
			//LOG("Item[%d]= %d\n", iTempCount, pItemArray[iTempCount] );
			iTempCount++;

		}
	}
	return iTempCount;
}


//bool LoadBinaryFile(const char* pszFileName, unsigned char* pbyFileBuf, int& iFileLen)
//{
//	if (!pszFileName || !pbyFileBuf)
//	{
//		iFileLen = 0;
//		return false;
//	}
//	int iMaxFileLen = iFileLen;
//	FILE *fp = NULL;
//	fopen_s(&fp, pszFileName, "rb");
//	if (!fp)
//	{
//		//printf("open file %s failed!\n", pszFileName);
//		iFileLen = 0;
//		return false;
//	}
//	fseek(fp, 0, SEEK_END);
//	int iTempFileSize = ftell(fp);
//	fseek(fp, 0, SEEK_SET);
//	if (iTempFileSize > iMaxFileLen)
//	{
//		fclose(fp);
//		printf("unzip buf len is so short filename is %s\n", pszFileName);
//		iFileLen = 0;
//		return false;
//	}
//	fread(pbyFileBuf, iTempFileSize, 1, fp);
//	fclose(fp);
//	iFileLen = iTempFileSize;
//	return true;
//}

//bool WriteBinaryFile(const char* pszFileName, unsigned char* pbyFileBuf, int iFileLen, bool createpath)
//{
//	if (!pszFileName || !pbyFileBuf)
//	{	
//		return false;
//	}
//	int iMaxFileLen = iFileLen;
//	FILE *fp = NULL;
//	fopen_s(&fp, pszFileName, "wb");
//	if (!fp)
//	{
//		if (createpath == false)
//		{
//			return false;
//		}
//		char tempCh = '/';
//		char* tempStr = (char*)strrchr((const char*)pszFileName, tempCh);
//		if (tempStr == NULL)
//		{
//			tempCh = '\\';
//			tempStr = (char*)strrchr((const char*)pszFileName, tempCh);
//		}
//		if (tempStr == NULL)
//		{
//			return false;
//		}
//		*tempStr = 0;
//		CreatePath((char*)pszFileName);
//		*tempStr = tempCh;
//		fopen_s(&fp, pszFileName, "wb");
//		if (!fp)
//		{
//			printf("open file %s failed!\n", pszFileName);			
//			return false;
//		}
//	}
//	fwrite(pbyFileBuf, iFileLen, 1, fp);
//	fclose(fp);	
//	return true;
//}

time_t ParseTimeStr(const char* szTime)
{
	tm tmParser;
	int num = 0;
	if (0 == szTime)
	{
		return 0;
	}
	tmParser.tm_hour = tmParser.tm_min = tmParser.tm_sec = 0;
	num = sscanf_s(szTime, "%d%*1c%d%*1c%d%*1c%d%*1c%d%*1c%d",
		&tmParser.tm_year, &tmParser.tm_mon, &tmParser.tm_mday,
		&tmParser.tm_hour, &tmParser.tm_min, &tmParser.tm_sec);

	if (tmParser.tm_year <= 0 || tmParser.tm_mon <= 0 || tmParser.tm_mday <= 0)
	{
		return 0;
	}

	if (num >= 3)
	{
		tmParser.tm_year -= 1900;
		tmParser.tm_mon--;
		time_t result = mktime(&tmParser);
		return result;
	}
	return 0;
}

bool CheckIdentify(std::string& identyfy)
{
	return true;
}


void gettimeofday(TTimeVal *tp, void *tzp)
{
	time_t clock;
	struct tm tm;
	SYSTEMTIME wtm;

	GetLocalTime(&wtm);
	tm.tm_year = wtm.wYear - 1900;
	tm.tm_mon = wtm.wMonth - 1;
	tm.tm_mday = wtm.wDay;
	tm.tm_hour = wtm.wHour;
	tm.tm_min = wtm.wMinute;
	tm.tm_sec = wtm.wSecond;
	tm.tm_isdst = -1;
	clock = mktime(&tm);
	tp->tv_sec = clock;
	tp->tv_usec = wtm.wMilliseconds * 1000;
}

unsigned long long RDTSC()  // ReaD Time Stamp Counter
{
	TTimeVal time;
	gettimeofday(&time, NULL);
	return ((time.tv_sec * 1000000 + time.tv_usec) / 1000);
}

int CheckUserNameLegal(const char* pText)
{
	int str_len = (int)strlen(pText);
	if (str_len < 4 || str_len > 12)
	{
		return ERR_NAME_LEN;
	}

	bool alldigit = true;
	for (int i = 0; i < str_len; i++)
	{
		char ch = pText[i];
		if ( (ch >= '0' && ch <= '9') ||
			 (ch >= 'a' && ch <= 'z') ||
			 (ch >= 'A' && ch <= 'Z') )
		{
			if (ch < '0' || ch > '9')
			{
				alldigit = false;
			}
			continue;
		}
		return ERR_NAME_TEXT;
	}

	//if (alldigit)
	//{
	//	return ERR_ALL_DIGIT;
	//}

	return 0;

#if 0
	UINT32 i = 0;
	char ch = pText[i];
	bool allDigit = true;

	while (ch && i < uiLen)
	{
		//˫�ֽڣ����壬����
		if (IsDBCSLeadByteEx(936,ch) || IsDBCSLeadByteEx(950,ch))
		{
			if ( (BYTE)ch >= 0xa1 && (BYTE)ch <= 0xa9)//���ֻ����
			{
				return false;
			}

			//������5��������ʾ���ַ�
 			if ((0xd7 == (BYTE)ch) && 
				((BYTE)pText[i+1] >= 0xfa) && ((BYTE)pText[i+1] <= 0xfe))
			{
				return false;
			}

			i+=2;
			ch = pText[i];//
		}//Ӣ�Ļ�����
		else if( (ch >= 'a' && ch <= 'z') ||
			(ch >= 'A' && ch <= 'Z') ||
			(ch >= '0' && ch <= '9')) 
		{
			i++;
			ch = pText[i];			
		}
		else
		{
			return false;
		}
		
		//BEGIN: Added by Johnny, 2018/1/8
		if (ch < '0' || ch > '9')
		{
			allDigit = false;
		}
		//END: Added by Johnny, 2018/1/8
	}
	
	//BEGIN: Added by Johnny, 2018/1/8
	if (allDigit)
	{
		//���������ֵ��û���
		return false;
	}
	//END: Added by Johnny, 2018/1/8

	return true;
#endif
}

/*  
std::string GetErrorString(int errcode, bool return_utf8)
{
	std::map<int, std::string> mapErrStr;

	mapErrStr[MI_NO_ERROR] = "";	
	mapErrStr[MI_ERR_DBERROR] = "DB��������";
	mapErrStr[MI_ERR_VALIDCODE_NOT_REQ] = "��û�з��ͻ�ȡ��֤�������";
	mapErrStr[MI_ERR_VALIDCODE_INCORRECT] = "��֤�벻��ȷ";
	mapErrStr[MI_ERR_PHONENUMBER_USED] = "�ֻ����ѱ�ע��";
	mapErrStr[MI_ERR_ACCOUNT_NOT_EXIST] = "�˺Ų�����";
	mapErrStr[MI_ERR_USERNAME_EXIST] = "�û����Ѵ���";
	mapErrStr[MI_ERR_USERNAME_INVALID] = "���Ϸ����û�����";
	mapErrStr[MI_ERR_USERNAME_INCORRECT] = "�û�������ȷ";
	mapErrStr[MI_ERR_PASSWORD_INCORRECT] = "���벻��ȷ";
	mapErrStr[MI_ERR_PHONENUMBER_INCORRECT] = "�ֻ��Ų���ȷ";
	mapErrStr[MI_ERR_PHONENUMBER_BIND_ALREADY] = "���Ѱ��ֻ��ţ����Ƚ����";
	mapErrStr[MI_ERR_VALIDCODE_SENDSMS_FAILED] = "������֤��ʧ��";
	mapErrStr[MI_ERR_VALIDCODE_SEND_LATER] = "������֤�����Ƶ�������Ժ�����";
	mapErrStr[MI_ERR_HASDISABLEDSTRING] = "�������дʻ�";
	mapErrStr[MI_ERROR_WRONGPASSWORD] = "���벻��ȷ";
	mapErrStr[MI_ERROR_ACCOUNTNOTEXIST] = "�˺Ų�����";
	mapErrStr[MI_ERROR_NOWEIBOSALES] = "���Ƚ����ٷ�����������";
	

	mapErrStr[MI_ERROR_LOGIN_MAC] = "��¼ʧ�ܣ���������ð󶨵��ֻ���¼��";
	mapErrStr[MI_ERROR_ACCOUNTLOCKED] = "��¼ʧ�ܣ���ұ���ţ�";
	mapErrStr[MI_ERROR_REGFAIL] = "���ע��ʧ�ܣ�";	
	mapErrStr[MI_ERROR_MAC_UNBIND] = "����ֻ���¼������ԭ�����ֻ���";
	mapErrStr[MI_ERROR_REGUSER_BIND] = "ֻ��ע���û����ܰ��ֻ���¼��";
	mapErrStr[MI_ERROR_REPEAT_BIND] = "�Ѿ������ֻ���¼�������ظ��󶨣�";
	mapErrStr[MI_ERROR_NOMAC_UNBIND] = "δ���ֻ���¼��������";
	//BEGIN AA BY  chenrujiang 20180425
	mapErrStr[MI_ERROR_NOTALLOW_MODIFY_HEADURL] = "��ע���˺�,�������޸�ͷ��ͼƬ��ַ��";
	//END AA BY  chenrujiang 20180425
	//BEGIN AA BY  chenrujiang 20180502
	mapErrStr[MI_ERROR_NOTALLOW_MODIFY_SEX] = "΢���˺�,�������޸��Ա�";
	//END AA BY  chenrujiang 20180502

	//BEGIN ADD BY  chenrujiang 20180529
	mapErrStr[MI_ERROR_REPEAT_APPLY_INVITECODE] = "���Ѿ�������Ƽ��룬�����ٴ�����";		//�ظ������Ƽ���
	mapErrStr[MI_ERROR_INVITECODE_NOT_EXIST] = "�Ƽ������,�����º˶�";		//�Ƽ��벻����
	mapErrStr[MI_ERROR_BIND_MYSELF_INVITECODE] = "�����ܰ��Լ����Ƽ���";		//���Լ����Ƽ���
	mapErrStr[MI_ERROR_REPEAT_BIND_INVITECODE] = "���Ѿ��󶨹��Ƽ��룬�����ٴΰ�";		//�ظ����Ƽ���
	mapErrStr[MI_ERROR_BIND_NOTALLOW_APPLY] = "���Ѿ��󶨹��Ƽ��룬���������Ƽ���";		//�Ѿ��󶨹����˵��Ƽ��룬���������Ƽ���
	mapErrStr[MI_ERROR_NOT_BIND_INVITECODE] = "����δ���Ƽ���";					//û�а��Ƽ���
	//END AA ADD  chenrujiang 20180529
	mapErrStr[MI_ERROR_USERGAMEINFO_NOT_EXIST] = "�û���Ϸ��Ϣ������",
		mapErrStr[MI_ERROR_INVITEUSER_NOT_EXIST] = "�Ƽ��û�������",
		mapErrStr[MI_ERROR_MUTUAL_BIND_INVITECODE] = "�����໥���Ƽ���",
		mapErrStr[MI_ERROR_IP_VOER_LIMIT] = "������ע����û����࣬���������ԣ�",

	//BEGIN add  by mhx
	mapErrStr[MI_ERR_BANK_PASS_ERROR] = "��¼ʧ��,���벻��ȷ��";
	mapErrStr[MI_ERR_BANK_SENDUSER_FORBIDDEN] = "�ʺ�״̬�쳣��";
	mapErrStr[MI_ERR_BANK_SENDUSER_PASS_ERROR] = "�û�У��ʧ�ܣ�";
	mapErrStr[MI_ERR_BANK_RECVUSER_FORBIDDEN] = "�Է��û�״̬�쳣��";
	mapErrStr[MI_ERR_BANK_SENDUSER_NOT_ENOUGH] = "���չ��Ҳ��㣡";
	mapErrStr[MI_ERR_BANK_PASS_TRYOUT] = "���볢�Դ������࣡";
	mapErrStr[MI_ERR_BANK_MODIFYPASS_SAME] = "�¾�����һ����";
	mapErrStr[MI_ERR_BANK_NOT_TRANS_RIGHT] = "�޷����ͣ�";
	mapErrStr[MI_ERR_BANK_NOT_TRANS_VISITOR] = "�οͽ�ֹ���ͣ�";
	mapErrStr[MI_ERR_VISITOR_NOT_EXIST] = "�ο��˺Ų����ڣ�";
	mapErrStr[MI_ERR_VISITOR_ALREADY_REG] = "���ο��ѳ�Ϊ����ʽ�˺ţ�";
	mapErrStr[MI_ERR_BANK_RECORD_END] = "�Ѿ����׿���";
	mapErrStr[MI_ERR_CHANNELNAMETOOLONG] = "������������";
	mapErrStr[MI_ERR_CHANNELNAMEEXIST] = "�������Ѿ����ڣ�";
	mapErrStr[MI_ERR_OLDPASSWORD] = "�����벻��ȷ��";
	mapErrStr[MI_ERR_RECHARGE_INVALIDCODE] = "����У���벻�ԣ�";
	mapErrStr[MI_ERR_SALERECHARGE_PASSWORD] = "�������벻�ԣ�";
	mapErrStr[MI_ERR_PARENTSALESID] = "��������id����";
	mapErrStr[MI_ERR_MODIFYSALESMONEY] = "�����޸����۵Ľ�ң�";
	mapErrStr[MI_ERR_SALESTAXRATIOTOOLARGE] = "�¼����۵ı������ܳ����ϼ���";
	mapErrStr[MI_ERR_SETSALESTAXRATIO] = "���۷ֳɱ�����1��100֮ǰ��";
	mapErrStr[MI_ERR_TOPSALESTAXRATIO] = "һ�����۷ֳɱ���������80��";	
	mapErrStr[MI_ERR_REALNAMENOTEQUAL] = " ��ʵ����������п�������ͬ�����������֣�������Ҳ�����޸�������Ϣ��";
	mapErrStr[MI_ERR_MONEYCARDNAME] = "���п����������ʵ����֤��������ͬ��";
	mapErrStr[MI_ERR_LUCKLESSSCORE] = "���ֲ��㣬���ܳ齱��";
	mapErrStr[MI_ERR_PARENTID] = "parentid����";
	mapErrStr[MI_ERR_GETLOCKMONEY] = "��ȡ�����ʱ�ʧ�ܣ�";
	mapErrStr[MI_ERR_FINDMONEYID] = "û���ҵ���Ӧ��¼��";
	mapErrStr[MI_ERR_CANNOTGETMONEY] = "��Լ�����ʱ�ʧ�ܣ�����첻��Ǯ��";
	mapErrStr[MI_ERR_CONTROLRRMONEY] = "��Ӧ�ļ�¼״̬���ԣ�";
	mapErrStr[MI_ERR_ALREADYGETLOCKMONEY] = "�����ʱ������Ѿ��������ˣ�";
	mapErrStr[MI_ERR_INLOCKTIME] = "�����ʱ����𻹴����������У�";	
	mapErrStr[MI_ERR_ADDRRMONEYINGAME] = "���������Ϸ�У��ý�ҹ��������ʱ�ʧ�ܣ������˳���Ϸ��";
	mapErrStr[MI_ERR_BUGRRMONEYLESSMONEY] = "��ҵĽ�Ҳ��㹺��ʧ�ܣ�";
	mapErrStr[MI_ERR_JIEYUEFREEGET] = "������͵������ʱ����ܽ�Լ��";
	mapErrStr[MI_ERR_LINGQUFREEGET] = "������͵������ʱ����ڲ�����ȡ��";
	mapErrStr[MI_ERR_REPEATFREEGETRR] = "�Ѿ������������ʱ��������ظ����ͣ�";
	mapErrStr[MI_ERR_RRZIUSER] = "�����ʱ��û����ܹ�������ͣ�UserID����";
	//END

	string errmsg;
	std::map<int, std::string>::iterator it = mapErrStr.find(errcode);
	if (it != mapErrStr.end())
	{
		errmsg = it->second;
	}
	else
	{ 
		errmsg = "δ֪������";
		errmsg += to_string(errcode);
	}

	//���ظ�client��errmsgת��Ϊutf-8��ʽ
	if (return_utf8 == true)
	{
		errmsg = G2U(errmsg.c_str());
	}

	return errmsg;
}
*/

//#ifndef WIN32
//int SetNBlock(int iSock)
//{
//	int iFlags;
//	iFlags = fcntl(iSock, F_GETFL, 0);
//	iFlags |= O_NONBLOCK;
//	iFlags |= O_NDELAY;
//	fcntl(iSock, F_SETFL, iFlags);
//	return 0;
//}
//#else
//int SetNBlock(SOCKET iSock)
//{
//	unsigned long ul = 1;
//	int ret = ioctlsocket(iSock, FIONBIO, (unsigned long*)&ul);
//	if (ret == SOCKET_ERROR)
//	{
//		return -1;
//	}
//	return 0;
//}
//#endif

//bool InitNetwork()
//{
//#ifdef WIN32
//	WORD wVersionRequested;
//	WSADATA wsaData;
//	int err;
//
//	wVersionRequested = MAKEWORD(2, 2);
//
//	err = WSAStartup(wVersionRequested, &wsaData);
//	if (err != 0)
//	{
//		/* Tell the user that we could not find a usable */
//		/* WinSock DLL.                                  */
//		return false;
//	}
//#endif
//
//	return true;
//}


//char* IPValue2Str(unsigned int uIPValue, char szIPStr[IP_LENGTH])
//{
//	memset(szIPStr, 0, IP_LENGTH);
//	_snprintf(szIPStr, IP_LENGTH - 1, "%u.%u.%u.%u",
//		uIPValue & 0x000000FF, (uIPValue & 0x0000FF00) >> 8,
//		(uIPValue & 0x00FF0000) >> 16, (uIPValue & 0xFF000000) >> 24);
//	szIPStr[IP_LENGTH - 1] = 0;
//
//	return szIPStr;
//}

//const char* SockAddrToString(sockaddr_in *pstSockAddr, char *szResult)
//{
//	char *pcTempIP = NULL;
//	unsigned short nTempPort = 0;
//
//	pcTempIP = inet_ntoa(pstSockAddr->sin_addr);
//	if (!pcTempIP)
//	{
//		return "";
//	}
//	nTempPort = ntohs(pstSockAddr->sin_port);
//	sprintf(szResult, "%s:%d", pcTempIP, nTempPort);
//	return (const char*)szResult;
//}

//���buffer��33���ֽڵģ����������� pszDigestString[32]��ֵΪ0
//void  Md5ToDigestString(char* pszDataString, char pszDigestString[33])
//{
//	//WCHAR wcUnicodeData[MAX_PATH] = { 0 };
//	//int nUnicodeLen = ::MultiByteToWideChar(CP_ACP, 0, pszDataString, -1, wcUnicodeData,
//	//		sizeof(wcUnicodeData) / sizeof(WCHAR));
//	//assert(nUnicodeLen >= 1);
//	//int nMultiByteLen = (nUnicodeLen - 1)/*ȥ��NULL��δ���ַ���*/ * sizeof(WCHAR);
//	//    	int size = sizeof(unsigned long int);
//	int size = (int)strlen(pszDataString);
//	unsigned char* btStr = new unsigned char[size * 2];
//	for (int i = 0; i < size * 2; i++)
//	{
//		if ((i % 2) == 0)
//		{
//			int index = i / 2;
//			btStr[i] = pszDataString[index];
//		}
//		else
//		{
//			btStr[i] = 0;
//		}
//	}
//	unsigned char btDigestData[16] = { 0 };
//	MD5_CTX md5;
//	md5.MD5Update((unsigned char*)btStr, size * 2);
//	md5.MD5Final(btDigestData);
//	delete[] btStr;
//	//assert(pszDigestString != NULL);
//	pszDigestString[0] = '\0';
//	for (int nIndex = 0; nIndex < sizeof(btDigestData); nIndex++)
//	{
//		::sprintf(pszDigestString + (nIndex * sizeof(char) * 2), "%02X", btDigestData[nIndex]);
//	}
//}

//static bool IsInGapVec(char inputchar, std::vector<char>& vecGap)
//{
//	for (int i = 0; i < (int)vecGap.size(); i++)
//	{
//		if (inputchar == vecGap[i])
//		{
//			return true;
//		}
//	}
//	return false;
//}

//static char* GetFirstCharPos(char* inputstr, std::vector<char>& vecGap)
//{
//	assert(sizeof(char*) == 4);
//
//	bool find = false;
//	UINT32 minpos = 0xFFFFFFFF;
//	for (int i = 0; i < (int)vecGap.size(); i++)
//	{
//		char szGap = vecGap[i];
//		char* temppos = strchr(inputstr, szGap);
//		if (temppos != NULL)
//		{
//			find = true;
//			if ((UINT32)temppos < minpos)
//			{
//				minpos = (UINT32)temppos;
//			}
//		}
//	}
//	if (find == false)
//	{
//		return NULL;
//	}
//	return (char*)minpos;
//}

//int SplitMapStr(const char *pMapStr, int iMaxItemNum, int *pItemArray, std::vector<char>& vecGap) 
//{
//	char szTempStr[10240];
//	char *pTemp1 = NULL;
//	char *pTemp2 = NULL;
//	char *pTemp3 = NULL;
//	char *pTemp4 = NULL;
//	int iStart;
//	int iStop, i;
//	int iTempCount = 0;
//	size_t mapstrlen;
//
//	if (!pMapStr || !pItemArray || iMaxItemNum < 0)
//	{
//		return -1;
//	}
//
//	mapstrlen = strlen(pMapStr);
//	if (mapstrlen >= 10240)
//	{
//		return -1;
//	}
//
//	int iLength = 0;
//	memset(szTempStr, '\0', sizeof(szTempStr));
//	for (size_t i = 0; i < mapstrlen; i++)
//	{
//		if (*(pMapStr + i) == ' ' || *(pMapStr + i) == '\t')
//		{
//			continue;
//		}
//		szTempStr[iLength] = *(pMapStr + i);
//		iLength++;
//	}
//	pTemp1 = &szTempStr[0];
//	if (strchr(pTemp1, ' '))
//	{
//		printf("Error in put map str, contained tab space\n");
//		return -1;
//	}
//
//	for (int i = 0; i < iLength; i++)
//	{
//		if ((pTemp1[i] > '9' || pTemp1[i] < '0') && IsInGapVec(pTemp1[i], vecGap) == false && pTemp1[i] != '-')
//		{
//			printf("Error in put map str, contained error char\n");
//			return -1;
//		}
//	}
//	while (pTemp2 = GetFirstCharPos(pTemp1, vecGap))
//	{
//		if (iTempCount == iMaxItemNum)
//		{
//			return iTempCount;
//		}
//		*pTemp2 = '\0';
//		if (strlen(pTemp1) == 0)
//		{
//			printf("Error:Split Map Str Failed, str in null or space\n");
//			return -1;
//		}
//
//		if (pTemp3 = strchr(pTemp1, '-'))
//		{
//			*pTemp3 = '\0';
//			iStart = atoi(pTemp1);
//			iStop = atoi(pTemp3 + 1);
//			if (iStop < iStart)
//			{
//				printf("Error:Split Map Str Failed, Range start big(%d) than stop(%d)\n", iStart, iStop);
//				return -1;
//			}
//
//			for (i = iStart; i <= iStop && iTempCount < iMaxItemNum; i++)
//			{
//				pItemArray[iTempCount] = i;
//				//LOG("Item[%d]= %d\n", iTempCount, i);
//				iTempCount++;
//
//			}
//		}
//		else
//		{
//			pItemArray[iTempCount] = atoi(pTemp1);
//			//LOG("Item[%d]= %d\n", iTempCount, pItemArray[iTempCount] );
//			iTempCount++;
//
//		}
//		pTemp1 = pTemp2 + 1;
//	}
//	if (pTemp1 && iTempCount < iMaxItemNum)
//	{
//		if (pTemp3 = strchr(pTemp1, '-'))
//		{
//			*pTemp3 = '\0';
//			iStart = atoi(pTemp1);
//			iStop = atoi(pTemp3 + 1);
//			if (iStop < iStart)
//			{
//				printf("Error:Split Map Str Failed, Range start big(%d) than stop(%d)\n", iStart, iStop);
//				return -1;
//			}
//
//			for (i = iStart; i <= iStop && iTempCount < iMaxItemNum; i++)
//			{
//				pItemArray[iTempCount] = i;
//				//LOG("Item[%d]= %d\n", iTempCount, i);
//				iTempCount++;
//			}
//		}
//		else
//		{
//			pItemArray[iTempCount] = atoi(pTemp1);
//			//LOG("Item[%d]= %d\n", iTempCount, pItemArray[iTempCount] );
//			iTempCount++;
//
//		}
//	}
//	return iTempCount;
//}

UINT32 GetYestodayDay()
{
	time_t yestodaytime = time(NULL) - 86400;
	tm* tmtmp = nullptr;
	localtime_s(tmtmp, &yestodaytime);
	char buff[20];
	ZeroMemory(buff, sizeof(buff));
	strftime(buff, sizeof(buff), "%Y%m%d", tmtmp);
	UINT32 nDate = atoi(buff);
	return nDate;
}

time_t GetTodayZeroTime()
{
	char timestr[64];
	char daystr[64];
	GetDateString(daystr);
	sprintf_s(timestr, "%s 00:00:00", daystr);
	time_t todayzerotime = ParseTimeStr(timestr);
	return todayzerotime;
}

