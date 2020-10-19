#ifndef _CODEQUEUE_H_
#define _CODEQUEUE_H_

#define QUEUERESERVELENGTH		8	//预留部分的长度


#define CODE_SUCC               0
#define ERR_SYSTEM              -9999
#define ERR_FOR_NOPACKET        -1
#define ERR_FOR_NO_ONE_PACKET   -2
#define ERR_FOR_SMALLBUFF       -3
#define ERR_APPEND_NOBUFF       -4

#define FAILED_SYSTEM           ERR_SYSTEM
#define FAILED_QUEUE_FULL       ERR_APPEND_NOBUFF  

class CCodeQueue
{
public:
	CCodeQueue(int nTotalSize);
	~CCodeQueue();

	int AppendOneCode(const unsigned char *pInCode, int nInLength);
	int GetHeadCode(unsigned char *pOutCode, unsigned int nOutMaxBufLen);
	int GetUsedLen(int &iTotalSize);

	bool IsQueueEmpty();

private:
	bool IsQueueFull();
	
	void GetCriticalData(int *piBeginIdx, int *piEndIdx);
	void SetCriticalData(int iBeginIdx, int iEndIdx);

private:
	struct _tagHead
	{
		int m_nSize;		
		int m_nBegin;
		int m_nEnd;
	} m_stQueueHead;

	unsigned char* mCodeBuf;
};

#endif
