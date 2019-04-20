
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

BOOL CRingBuf::isReady()  //�жϻ��λ������Ƿ�׼����
{
//	if ((NULL == m_bufRing.mutex) || (NULL == m_bufRing.buf) || (NULL == m_bufRing.bufLim))
	if ((NULL == m_bufRing.buf) || (NULL == m_bufRing.bufLim))
		return false;
	return true;
}
int CRingBuf::initRingBuf()  //��ʼ�����λ�����
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
int CRingBuf::destroyRingBuf() // ���ٻ��λ�����
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
int CRingBuf::writeRingBuf(unsigned char* buf, int len) // �����λ�������д����
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
// �ڻ��λ�������Ѱ��LIM_HEAD.TAG���ҵ�����TAG�Ŀ�ʼλ�ã����򷵻�-1
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
//�ӻ��λ������ж�len�ֽڣ�����buf(���޸Ļ��λ�������count��rp)
int CRingBuf::readRingBuf(unsigned char* buf, int len)
{
	if (m_bufRing.count < len)
		return -1; // �����������ݲ���
	if (len <= (RING_BUF_SIZE - m_bufRing.rp))
	{
		memcpy((void*)buf, (void*)&m_bufRing.buf[m_bufRing.rp], len);
		return len;  //�ɹ������ض������ֽ���
	}
	int nPart = RING_BUF_SIZE - m_bufRing.rp;
	memcpy((void*)buf, (void*)&m_bufRing.buf[m_bufRing.rp], nPart);
	memcpy((void*)&buf[nPart], (void*)&m_bufRing.buf[0], len - nPart);
	return len;
}
LIM_HEAD* CRingBuf::readLidarData() // �ӻ��λ������ж��״�����
{
	int ret;
//	::WaitForSingleObject(m_bufRing.mutex, INFINITE);
	while (1)
	{
		ret = findLimTag();
		if (LIDARSDK_ERR_FAIL == ret)
		{ //  û���ҵ�TAG
//			::ReleaseMutex(m_bufRing.mutex);
			return NULL;
		}
		// �ҵ� LIM_HEAD.TAG����LIM_HEAD�ӻ��λ������ж�������֤�Ƿ�����
		ret = readRingBuf(m_bufRing.bufLim, sizeof(LIM_HEAD));  // ��LIM_HEAD���ֶ���
		if (LIM_CheckSum(m_bufRing.pLim) == m_bufRing.pLim->CheckSum)
		{ // LIM_HEAD����checksumУ��ɹ�
			if (sizeof(LIM_HEAD) == m_bufRing.pLim->nLIMLen)
			{  // û��Extended Data
				m_bufRing.rp = (m_bufRing.rp + sizeof(LIM_HEAD)) % RING_BUF_SIZE;
				m_bufRing.count -= sizeof(LIM_HEAD);
//				::ReleaseMutex(m_bufRing.mutex);
				return m_bufRing.pLim;
			}
			else if ((int)m_bufRing.pLim->nLIMLen > m_bufRing.count)
			{ // ��Extended Data����֡δ������
//				::ReleaseMutex(m_bufRing.mutex);
				return NULL;
			}
			else if (m_bufRing.pLim->nLIMLen > MAX_LIDAR_FRAME_LEN)
			{ // Extended Data����֡���ȳ���֡���������ȣ���������Ӧ�ó��֣�
				m_bufRing.rp = (m_bufRing.rp + sizeof(LIM_HEAD)) % RING_BUF_SIZE;
				m_bufRing.count -= sizeof(LIM_HEAD);
			}
			else
			{ // ��Extended_Data���ֶ���
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
		{	// LIM_HEAD checksumУ��ʧ�ܣ���rpָ���1������Ѱ��TAG
			m_bufRing.rp = (m_bufRing.rp + 1) % RING_BUF_SIZE;
			m_bufRing.count--;
		}
	} // end while(1)
	// ����ִ�е���
//	::ReleaseMutex(m_bufRing.mutex);
	return NULL;
}
