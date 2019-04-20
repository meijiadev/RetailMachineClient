
#include "windows.h"

#include "LidarSDK_errno.h"
#include "RingBuf.h"

CRingBuf::CRingBuf()
{
	m_bufRing.pLim = NULL;
	m_bufRing.bufLim = NULL;
	m_bufRing.rp = 0;
	m_bufRing.wp = 0;
	m_bufRing.count = 0;
//	m_bufRing.mutex = NULL;
	m_bufRing.buf = NULL;

	initRingBuf();
}

CRingBuf::~CRingBuf()
{
	destroyRingBuf();
}

BOOL CRingBuf::isReady()  //判断环形缓冲区是否准备好
{
//	if ((NULL == m_bufRing.mutex) || (NULL == m_bufRing.buf) || (NULL == m_bufRing.bufLim))
	if ((NULL == m_bufRing.buf) || (NULL == m_bufRing.bufLim))
		return false;
	return true;
}
int CRingBuf::initRingBuf()  //初始化环形缓冲区
{
//	m_bufRing.mutex = ::CreateMutex(NULL, FALSE, nullptr);

//	if (NULL == m_bufRing.mutex)
//	{
//		return LIDARSDK_ERR_ALLOC_MUTEX;
//	}
	m_bufRing.buf = (unsigned char*)malloc(RING_BUF_SIZE);
	if (NULL == m_bufRing.buf)
	{
//		::CloseHandle(m_bufRing.mutex);
//		m_bufRing.mutex = NULL;
		return LIDARSDK_ERR_ALLOC_MEMORY;
	}
	m_bufRing.bufLim = (unsigned char*)malloc(MAX_LIDAR_FRAME_LEN);
	if (NULL == m_bufRing.bufLim)
	{
//		::CloseHandle(m_bufRing.mutex);
//		m_bufRing.mutex = NULL;
		free(m_bufRing.buf);
		m_bufRing.buf = NULL;
		return LIDARSDK_ERR_ALLOC_MEMORY;
	}
	m_bufRing.pLim = (LIM_HEAD*)m_bufRing.bufLim;
	m_bufRing.rp = 0;
	m_bufRing.wp = 0;
	m_bufRing.count = 0;
	return LIDARSDK_ERR_SUCCESS;
}
int CRingBuf::destroyRingBuf() // 销毁环形缓冲区
{
	if (NULL != m_bufRing.buf)
		free(m_bufRing.buf);
	m_bufRing.buf = NULL;
	if (NULL != m_bufRing.bufLim)
		free(m_bufRing.bufLim);
	m_bufRing.bufLim = NULL;
//	if (NULL != m_bufRing.mutex)
//		::CloseHandle(m_bufRing.mutex);
//	m_bufRing.mutex = NULL;
	return LIDARSDK_ERR_SUCCESS;
}
int CRingBuf::writeRingBuf(unsigned char* buf, int len) // 往环形缓冲区中写数据
{
//	::WaitForSingleObject(m_bufRing.mutex, INFINITE);
	if ((RING_BUF_SIZE - m_bufRing.count) < len)
	{
//		::ReleaseMutex(m_bufRing.mutex);
		return LIDARSDK_ERR_NOROOM;
	}
	if (m_bufRing.wp < m_bufRing.rp)
	{
		memcpy((void*)&m_bufRing.buf[m_bufRing.wp], buf, len);
		m_bufRing.wp = (m_bufRing.wp + len) % RING_BUF_SIZE; // modify write pointer;
		m_bufRing.count += len;
	}
	else
	{
		int nTailRoom = RING_BUF_SIZE - m_bufRing.wp;
		if (nTailRoom >= len)
		{
			memcpy((void*)&m_bufRing.buf[m_bufRing.wp], buf, len);
			m_bufRing.wp = (m_bufRing.wp + len) % RING_BUF_SIZE;
			m_bufRing.count += len;
		}
		else
		{
			memcpy((void*)&m_bufRing.buf[m_bufRing.wp], buf, nTailRoom);
			memcpy((void*)&m_bufRing.buf[0], &buf[nTailRoom], len - nTailRoom);
			m_bufRing.wp = len - nTailRoom;
			m_bufRing.count += len;
		}
	}
//	::ReleaseMutex(m_bufRing.mutex);
	return len;
}
// 在环形缓冲区中寻找LIM_HEAD.TAG，找到返回TAG的开始位置，否则返回-1
int CRingBuf::findLimTag()
{
	unsigned int nTag;
	unsigned char* p = (unsigned char*)&nTag;
	int i;

	while (sizeof(LIM_HEAD) <= m_bufRing.count)
	{
		for (i = 0; i<sizeof(nTag); i++)
			p[i] = m_bufRing.buf[(m_bufRing.rp + i) % RING_BUF_SIZE];
		if (LIM_TAG == nTag)
			return m_bufRing.rp;
		m_bufRing.count--;
		m_bufRing.rp = (m_bufRing.rp + 1) % RING_BUF_SIZE;
	}
	return LIDARSDK_ERR_FAIL;
}
//从环形缓冲区中读len字节，放入buf(不修改环形缓冲区的count和rp)
int CRingBuf::readRingBuf(unsigned char* buf, int len)
{
	if (m_bufRing.count < len)
		return -1; // 缓冲区中数据不足
	if (len <= (RING_BUF_SIZE - m_bufRing.rp))
	{
		memcpy((void*)buf, (void*)&m_bufRing.buf[m_bufRing.rp], len);
		return len;  //成功，返回读到的字节数
	}
	int nPart = RING_BUF_SIZE - m_bufRing.rp;
	memcpy((void*)buf, (void*)&m_bufRing.buf[m_bufRing.rp], nPart);
	memcpy((void*)&buf[nPart], (void*)&m_bufRing.buf[0], len - nPart);
	return len;
}
LIM_HEAD* CRingBuf::readLidarData() // 从环形缓冲区中读雷达数据
{
	int ret;
//	::WaitForSingleObject(m_bufRing.mutex, INFINITE);
	while (1)
	{
		ret = findLimTag();
		if (LIDARSDK_ERR_FAIL == ret)
		{ //  没有找到TAG
//			::ReleaseMutex(m_bufRing.mutex);
			return NULL;
		}
		// 找到 LIM_HEAD.TAG，把LIM_HEAD从环形缓冲区中读出，验证是否完整
		ret = readRingBuf(m_bufRing.bufLim, sizeof(LIM_HEAD));  // 把LIM_HEAD部分读出
		if (LIM_CheckSum(m_bufRing.pLim) == m_bufRing.pLim->CheckSum)
		{ // LIM_HEAD部分checksum校验成功
			if (sizeof(LIM_HEAD) == m_bufRing.pLim->nLIMLen)
			{  // 没有Extended Data
				m_bufRing.rp = (m_bufRing.rp + sizeof(LIM_HEAD)) % RING_BUF_SIZE;
				m_bufRing.count -= sizeof(LIM_HEAD);
//				::ReleaseMutex(m_bufRing.mutex);
				return m_bufRing.pLim;
			}
			else if ((int)m_bufRing.pLim->nLIMLen > m_bufRing.count)
			{ // 有Extended Data，此帧未接收完
//				::ReleaseMutex(m_bufRing.mutex);
				return NULL;
			}
			else if (m_bufRing.pLim->nLIMLen > MAX_LIDAR_FRAME_LEN)
			{ // Extended Data，此帧长度超出帧缓冲区长度，丢弃（不应该出现）
				m_bufRing.rp = (m_bufRing.rp + sizeof(LIM_HEAD)) % RING_BUF_SIZE;
				m_bufRing.count -= sizeof(LIM_HEAD);
			}
			else
			{ // 把Extended_Data部分读出
				m_bufRing.rp = (m_bufRing.rp + sizeof(LIM_HEAD)) % RING_BUF_SIZE;
				m_bufRing.count -= sizeof(LIM_HEAD);
				ret = readRingBuf(&m_bufRing.bufLim[sizeof(LIM_HEAD)], m_bufRing.pLim->nLIMLen - sizeof(LIM_HEAD));
				if (LIDARSDK_ERR_FAIL == ret)
				{
					m_bufRing.rp = (m_bufRing.rp + sizeof(LIM_HEAD)) % RING_BUF_SIZE;
					m_bufRing.count -= sizeof(LIM_HEAD);
//					::ReleaseMutex(m_bufRing.mutex);
					return NULL;
				}
				m_bufRing.rp = (m_bufRing.rp + m_bufRing.pLim->nLIMLen - sizeof(LIM_HEAD)) % RING_BUF_SIZE;
				m_bufRing.count -= (m_bufRing.pLim->nLIMLen - sizeof(LIM_HEAD));
//				::ReleaseMutex(m_bufRing.mutex);
				return m_bufRing.pLim;
			}
		}
		else
		{	// LIM_HEAD checksum校验失败，把rp指针加1，继续寻找TAG
			m_bufRing.rp = (m_bufRing.rp + 1) % RING_BUF_SIZE;
			m_bufRing.count--;
		}
	} // end while(1)
	// 不会执行到这
//	::ReleaseMutex(m_bufRing.mutex);
	return NULL;
}
