#include "stdafx.h"

#include "BlockRingBuf.h"
#include "LidarSDK_errno.h"

CBlockRingBuf::CBlockRingBuf()
{
	m_nBlockSize = 4800;  // 每块缓冲区大小
	m_nBlockNum = 16;	// 块数
	m_rp = m_wp = 0;
	m_buf = new char[m_nBlockSize*m_nBlockNum];
	m_hSemRead = CreateSemaphore(NULL, 0, m_nBlockNum, NULL);
	m_hSemWrite = CreateSemaphore(NULL, m_nBlockNum, m_nBlockNum, NULL);
}
CBlockRingBuf::CBlockRingBuf(int nBlockNum = 16, int nBlockSize = 4800)
{
	m_nBlockSize = nBlockSize;  // 每块缓冲区大小
	m_nBlockNum = nBlockNum;	// 块数
	m_rp = m_wp = 0;
	m_buf = new char[m_nBlockSize*m_nBlockNum];
	m_hSemRead = CreateSemaphore(NULL, 0, m_nBlockNum, NULL);
	m_hSemWrite = CreateSemaphore(NULL, m_nBlockNum, m_nBlockNum, NULL);
}

CBlockRingBuf::~CBlockRingBuf()
{
	if (NULL != m_hSemRead)
		CloseHandle(m_hSemRead);
	m_hSemRead = NULL;
	if (NULL != m_hSemWrite)
		CloseHandle(m_hSemWrite);
	m_hSemWrite = NULL;
	if (NULL != m_buf)
		delete m_buf;
	m_buf = NULL;
}

int CBlockRingBuf::get_block_size()
{
	return m_nBlockSize;
}
int CBlockRingBuf::get_block_num()
{
	return m_nBlockNum;
}

int CBlockRingBuf::put(char* buf, int len, int dwMilliseconds)
{
	if (len > m_nBlockSize)
		return LIDARSDK_ERR_FAIL;

	DWORD dwErr;
	if (-1 == dwMilliseconds)
		dwErr = WaitForSingleObject(m_hSemWrite, INFINITE);
	else
		dwErr = WaitForSingleObject(m_hSemWrite, dwMilliseconds);
	if (WAIT_TIMEOUT == dwErr)
		return LIDARSDK_ERR_TIMEOUT;
	if (WAIT_OBJECT_0 == dwErr)
	{
		memcpy((void*)(m_buf + m_nBlockSize * m_wp), (void*)buf, len);
		m_wp = (m_wp + 1) % m_nBlockNum;
		if (FALSE == ReleaseSemaphore(m_hSemRead, 1, NULL))
			return LIDARSDK_ERR_FAIL;
		return LIDARSDK_ERR_SUCCESS;
	}
	return LIDARSDK_ERR_FAIL;
}

char* CBlockRingBuf::get_getbuffer(int dwMilliseconds)
{
	DWORD dwErr = WaitForSingleObject(m_hSemRead, dwMilliseconds); //不等待，无论有无信号，立刻返回
	if (WAIT_OBJECT_0 != dwErr)
		return NULL;
	return m_buf + m_nBlockSize * m_rp;
}
int CBlockRingBuf::get_finish()
{
	m_rp = (m_rp + 1) % m_nBlockNum;
	if (FALSE == ReleaseSemaphore(m_hSemWrite, 1, NULL))
		return LIDARSDK_ERR_FAIL;
	return LIDARSDK_ERR_SUCCESS;
}



