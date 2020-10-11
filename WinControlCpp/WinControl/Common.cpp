#include "stdafx.h"
#include "Common.h"
#include <direct.h>//for mkdir function

//½µÐòÅÅÐòº¯Êý
bool DescSort(const ORDERS_TEN_RESULTS& V1, const ORDERS_TEN_RESULTS& V2)
{
	return V1.uiAllTotalBonus >= V2.uiAllTotalBonus;
}
//ÉýÐòÅÅÐòº¯Êý
bool AscSort(const ORDERS_TEN_RESULTS& V1, const ORDERS_TEN_RESULTS& V2)
{
	return V1.uiAllTotalBonus < V2.uiAllTotalBonus;
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
	strncpy_s(szTempLogPath, szLogPath, sizeof(szTempLogPath) - 1);
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