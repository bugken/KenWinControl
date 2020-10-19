#include "stdafx.h"
#include "win_linux.h"
#include <stdlib.h>
#include "codequeue.h"

CCodeQueue::CCodeQueue(int nTotalSize)
{
	m_stQueueHead.m_nSize = nTotalSize;
	m_stQueueHead.m_nBegin = 0;
	m_stQueueHead.m_nEnd = 0;
	mCodeBuf = (unsigned char *)malloc(nTotalSize);
}

CCodeQueue::~CCodeQueue()
{
}

void CCodeQueue::GetCriticalData(int *piBeginIdx, int *piEndIdx)
{
	if( piBeginIdx )
	{
		*piBeginIdx = m_stQueueHead.m_nBegin;
	}
	if( piEndIdx )
	{
		*piEndIdx = m_stQueueHead.m_nEnd;
	}
}

void CCodeQueue::SetCriticalData(int iBeginIdx, int iEndIdx)
{
	if( iBeginIdx >= 0 && iBeginIdx < m_stQueueHead.m_nSize )
	{
		m_stQueueHead.m_nBegin = iBeginIdx;
	}
	if( iEndIdx >= 0 && iEndIdx < m_stQueueHead.m_nSize )
	{
		m_stQueueHead.m_nEnd = iEndIdx;
	}
}

int CCodeQueue::GetUsedLen(int &iTotalSize)
{
	int nUsedLen = 0;
	int nTempBegin = -1;
	int nTempEnd = -1;

	GetCriticalData( &nTempBegin, &nTempEnd );

	if( nTempBegin > nTempEnd )
	{
		nUsedLen = m_stQueueHead.m_nSize - (nTempBegin - nTempEnd);
	}
	else
	{
		nUsedLen = nTempEnd - nTempBegin;
	}
	iTotalSize = m_stQueueHead.m_nSize;
    return nUsedLen;
}

bool CCodeQueue::IsQueueFull()
{
	int totalLen;
	int useLen = GetUsedLen(totalLen);

	if (totalLen < useLen + QUEUERESERVELENGTH)
	{
		return true;
	}

	return false;
}

//Error code: -1 invalid para; -2 not enough; -3 data crashed
//This function only set the end index of code queue
int CCodeQueue::AppendOneCode(const unsigned char *pInCode, int nInLength)
{
    int nTempMaxLength = 0;
    int nTempBegin = -1;
    int nTempEnd = -1;
    
    unsigned char *pTempSrc = NULL;
    unsigned char *pTempDst = NULL;
    int i;
	int nTempRet;

	if (!pInCode || nInLength <= 0)
    {
        return FAILED_SYSTEM;
    }
	
    //首先判断是否队列已满
    if (IsQueueFull())  
    {
        return FAILED_QUEUE_FULL;
    }

    GetCriticalData(&nTempBegin, &nTempEnd);

    if( nTempBegin < 0 || nTempBegin >= m_stQueueHead.m_nSize
        || nTempEnd < 0 || nTempEnd >= m_stQueueHead.m_nSize )
    {
        nTempEnd = nTempBegin;
        SetCriticalData( -1, nTempEnd );

        return FAILED_SYSTEM;
    }

    if( nTempBegin == nTempEnd )
    {
        nTempMaxLength = m_stQueueHead.m_nSize;
    }
    else if( nTempBegin > nTempEnd )
    {
        nTempMaxLength = nTempBegin - nTempEnd;        
    }
    else
    {
        nTempMaxLength = (m_stQueueHead.m_nSize - nTempEnd) + nTempBegin;
    }

    //重要：最大长度应该减去预留部分长度，保证首尾不会相接
    nTempMaxLength -= QUEUERESERVELENGTH;

    if( (int)(nInLength + sizeof(int)) > nTempMaxLength )
    {
        return FAILED_QUEUE_FULL;
    }

    nTempRet = nTempEnd;

    pTempDst = &mCodeBuf[0];
    pTempSrc = (unsigned char *)&nInLength;

    for( i = 0; i < sizeof(nInLength); i++ )
    {
        pTempDst[nTempEnd] = pTempSrc[i];
        nTempEnd = (nTempEnd + 1) % m_stQueueHead.m_nSize;
    }

    if( nTempBegin > nTempEnd )
    {
        memcpy((void *)&mCodeBuf[nTempEnd], (const void *)pInCode, (size_t)nInLength);
    }
    else
    {
        if( (int)nInLength > (m_stQueueHead.m_nSize - nTempEnd) )
        {
            memcpy((void *)&mCodeBuf[nTempEnd], (const void *)&pInCode[0], (size_t)(m_stQueueHead.m_nSize - nTempEnd) );
            memcpy((void *)&mCodeBuf[0],(const void *)&pInCode[(m_stQueueHead.m_nSize - nTempEnd)], (size_t)(nInLength + nTempEnd - m_stQueueHead.m_nSize) );
        }
        else
        {
            memcpy((void *)&mCodeBuf[nTempEnd], (const void *)&pInCode[0], (size_t)nInLength);
        }
    }
    nTempEnd = (nTempEnd + nInLength) % m_stQueueHead.m_nSize;

    SetCriticalData( -1, nTempEnd );

    return nTempRet;
}

//This function just change the begin index of code queue
int CCodeQueue::GetHeadCode(unsigned char *pOutCode, unsigned int nOutMaxBufLen)
{	
    int nTempMaxLength = 0;
    int nTempBegin = -1;
    int nTempEnd = -1;
    unsigned char *pTempSrc;
    unsigned char *pTempDst;
    int i;
    
    if(!pOutCode)
    {	
        return ERR_SYSTEM;
    }

    GetCriticalData(&nTempBegin, &nTempEnd);

    if( nTempBegin == nTempEnd )
    {   
        return 0;
    }

    if( nTempBegin < nTempEnd )
    {
        nTempMaxLength = nTempEnd - nTempBegin;
    }
    else
    {
        nTempMaxLength = m_stQueueHead.m_nSize - nTempBegin + nTempEnd;
    }

    if( nTempMaxLength < sizeof(int) )
    {   
        nTempBegin = nTempEnd;
        SetCriticalData(nTempBegin, -1);
        return ERR_SYSTEM;
    }

	unsigned int nReturnLength;
	pTempDst = (unsigned char *)(&nReturnLength);

    pTempSrc = (unsigned char *)&mCodeBuf[0];
    for( i = 0; i < sizeof(int); i++ )
    {
        pTempDst[i] = pTempSrc[nTempBegin];
        nTempBegin = (nTempBegin+1) % m_stQueueHead.m_nSize; 
    }

	if (nReturnLength >(int)(nTempMaxLength - sizeof(int)))
    {   
		//已经写入的buffer长度不够
        nTempBegin = nTempEnd;
        SetCriticalData(nTempBegin, -1);
		return ERR_FOR_NO_ONE_PACKET;
    }
	if (nReturnLength > nOutMaxBufLen)
	{	
		//传进来的空间不够用
		return ERR_FOR_SMALLBUFF;
	}

    pTempDst = (unsigned char *)&pOutCode[0];

    if( nTempBegin < nTempEnd )
    {
        memcpy((void *)pTempDst, (const void *)&pTempSrc[nTempBegin], nReturnLength);
    }
    else
    {
        //如果出现分片，则分段拷贝
		if (m_stQueueHead.m_nSize - nTempBegin < (int)(nReturnLength))
        {
            memcpy((void *)pTempDst, (const void *)&pTempSrc[nTempBegin], (size_t)(m_stQueueHead.m_nSize - nTempBegin));
            pTempDst += (m_stQueueHead.m_nSize - nTempBegin);
			memcpy((void *)pTempDst, (const void *)&pTempSrc[0], (size_t)(nReturnLength + nTempBegin - m_stQueueHead.m_nSize));
        }
        else    //否则，直接拷贝
        {
			memcpy((void *)pTempDst, (const void *)&pTempSrc[nTempBegin], (size_t)nReturnLength);
        }
    }
	nTempBegin = (nTempBegin + nReturnLength) % m_stQueueHead.m_nSize;

    SetCriticalData(nTempBegin, -1);

	return nReturnLength;
}

bool CCodeQueue::IsQueueEmpty()
{
	int iTempBegin;
	int iTempEnd;
	int nTempFullFlag = -1;

	GetCriticalData(&iTempBegin, &iTempEnd);
	if( iTempBegin == iTempEnd )
	{
		return true;
	}

	return false;
}

