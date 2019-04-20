
#include <stdio.h>

#include "LidarSDK_errno.h"
#include "lim.h"
#include "common.h"
#include <process.h>
#include "LidarProbe.h"
#include "LidarList.h"

CLidarProbe::CLidarProbe()
{
	m_pRingBuf = NULL;  //���λ���������
	m_pLidarList = NULL;  // ̽�⵽���״��б�
	m_bLidarProbe = false;
	m_hLidarProbeThread = NULL;
	m_sock = INVALID_SOCKET;
	m_callbackProbeFunc = NULL;  //����̽���״�ص�����ָ��
	m_callbackProbeFuncParam = NULL;  //��Ϊ�����ش����ص�����
}
CLidarProbe::~CLidarProbe()
{
	stop_probe_lidar();
}

int CLidarProbe::open_socket()
{
	int err;

	if (INVALID_SOCKET != m_sock)
		return LIDARSDK_ERR_FAIL;

	m_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (INVALID_SOCKET == m_sock)
		return LIDARSDK_ERR_FAIL;

	const int on = 1; //�������Ķ��ʵ��������ͬһ̨������  
	/*	����setsockopt()����Ϊ�׽�������SO_REUSEADDRѡ��������׽��ְ�����һ������ʹ�õĵ�ַ�ϡ������׽��ֵ�ѡ��	*/
	err = setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	if (err == SOCKET_ERROR)
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		return LIDARSDK_ERR_FAIL;
	}

	memset(&m_LocalAddr, 0, sizeof(m_LocalAddr));
	m_LocalAddr.sin_family = AF_INET;
	m_LocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);  //ok
	m_LocalAddr.sin_port = htons(LIM_DT_PORT);
	err = bind(m_sock, (struct sockaddr *)&m_LocalAddr, sizeof(m_LocalAddr));
	if (SOCKET_ERROR == err)
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		return LIDARSDK_ERR_FAIL;
	}

	/*����㲥��*/
	m_mreq.imr_multiaddr.s_addr = inet_addr(LIM_DT_IP);//�ಥ��ַ
	m_mreq.imr_interface.s_addr = htonl(INADDR_ANY); //����ӿ�ΪĬ��
	/*����������ಥ��*/
	err = setsockopt(m_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&m_mreq, sizeof(m_mreq));
	if (SOCKET_ERROR == err)
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		return LIDARSDK_ERR_FAIL;
	}
	printf("CLidarProbe::open_socket()\n");
	return LIDARSDK_ERR_SUCCESS;
}
int CLidarProbe::close_socket()
{
	if (INVALID_SOCKET != m_sock)
	{
		setsockopt(m_sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&m_mreq, sizeof(m_mreq));
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		printf("CLidarProbe::close_socket()\n");
	}
	return LIDARSDK_ERR_SUCCESS;
}

int CLidarProbe::start_probe_lidar()  //����̽��lidar
{
	int ret;
	ret = open_socket();
//	if (LIDARSDK_ERR_SUCCESS != ret)
//		return ret;

	// ���仺����
	if (NULL == m_pRingBuf)
		m_pRingBuf = new CRingBuf;
	if (NULL == m_pLidarList)
		m_pLidarList = new CLidarList;
	if ((NULL == m_pRingBuf) || (NULL == m_pLidarList))
	{
		if (NULL != m_pRingBuf)
			delete m_pRingBuf;
		m_pRingBuf = NULL;
		if (NULL != m_pLidarList)
			delete m_pLidarList;
		m_pLidarList = NULL;
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		return LIDARSDK_ERR_FAIL;
	}

	// ����̽���״��߳�
	m_bLidarProbe = true;
	if (0 == (m_hLidarProbeThread = (HANDLE)_beginthreadex(NULL, 0, LidarProbeThreadEntry, this, 0, NULL)))
	{
		m_bLidarProbe = false;
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		if (NULL != m_pRingBuf)
			delete m_pRingBuf;
		m_pRingBuf = NULL;
		if (NULL != m_pLidarList)
			delete m_pLidarList;
		m_pLidarList = NULL;
		printf("probe start thread fail\n");
		return LIDARSDK_ERR_FAIL;
	}

	return LIDARSDK_ERR_SUCCESS;
}
int CLidarProbe::stop_probe_lidar()  //ֹͣ̽���״�
{
	// ֪ͨ̽���߳��˳�
	if (NULL != m_hLidarProbeThread)
	{
		m_bLidarProbe = false;  //֪ͨ̽���״��߳��˳�
		WaitForSingleObject(m_hLidarProbeThread, INFINITE);  //�ȴ��߳��˳�
		CloseHandle(m_hLidarProbeThread);  //�ر��߳�
		m_hLidarProbeThread = NULL;
	}

	// �ر�SOCKET
	close_socket();

	// �ͷŻ�����
	if (NULL != m_pRingBuf)
		delete m_pRingBuf;
	m_pRingBuf = NULL;
	if (NULL != m_pLidarList)
		delete m_pLidarList;
	m_pLidarList = NULL;

	return 0;

}

int CLidarProbe::reg_probe_callback(void *_func, void *_data)  //ע��̽���״�ص�����
{
	m_callbackProbeFunc = (LIDAR_PROBE_CALLBACK)_func;
	m_callbackProbeFuncParam = _data;
	return 0;
}

//void __cdecl CLidarProbe::LidarProbeThreadEntry(void* lpParameter)
unsigned __stdcall CLidarProbe::LidarProbeThreadEntry(void* lpParameter)
{
	CLidarProbe	*pThread = NULL;
	pThread = static_cast<CLidarProbe *>(lpParameter);   //��������ʽִ�е��κ�����ת����������static_cast��ʽ���
	if (pThread == NULL)
	{
		_endthreadex(0);
		return 0;
	}
	pThread->LidarProbeThread();

	_endthreadex(0);
	return 0;
}

int	CLidarProbe::LidarProbeThread(void)
{
	char recv_buf[MAX_LIDAR_FRAME_LEN];  //��ʱ���ջ�����
	int len,err;
	LIM_HEAD* lim = NULL;
	int addr_len = 0;
	fd_set rfds,efds;
	struct timeval tv;
	LIDAR_NODE *pNode = NULL;

	time_t tmLastProbe = time(NULL);
	while (m_bLidarProbe)
	{
		if (5 < (time(NULL) - tmLastProbe))
		{  // ��ʱû�з����״���³�ʼ��SOCKET
#if 0 // 20190314 to meiya
#else
			close_socket();
			Sleep(CLOSE_OPEN_INTERVAL_TIME);
			open_socket();
			tmLastProbe = time(NULL);
#endif
		}

		// ���һ���״��������Ƿ��г�ʱû��ˢ�µ��״�ڵ�
		// �ѳ�ʱûˢ�µ��״�ڵ��������ɾ�����ûص�����֪ͨӦ���߳�
		pNode = m_pLidarList->find_timeout_node();
		if (NULL != pNode)
		{
			if (NULL != m_callbackProbeFunc)
				m_callbackProbeFunc(-1, &pNode->ldini, m_callbackProbeFuncParam);  //���ûص�������flag=-1����ʾ�˽ڵ���ʧЧ
			m_pLidarList->free_node(pNode);
		}

		// ���նಥ�˿����ݣ�����ѭ����������
		FD_ZERO(&rfds);
		FD_ZERO(&efds);
		if (INVALID_SOCKET != m_sock)
		{
			FD_SET(m_sock, &rfds);
			FD_SET(m_sock, &efds);
		}
		tv.tv_sec = 0;
		tv.tv_usec = 500000; //500ms
		err = ::select(-1, &rfds, NULL, &efds, &tv);
		if (SOCKET_ERROR == err)
		{
			err = WSAGetLastError();
			printf("probe thread : select fail,errno = %d\n", err);
			close_socket();
			Sleep(CLOSE_OPEN_INTERVAL_TIME);
			open_socket();
			continue;
		}
		else if (0 != err)
		{
			if (FD_ISSET(m_sock, &efds))
			{
				printf("error(efds) probe socket...\n");
				close_socket();
				Sleep(CLOSE_OPEN_INTERVAL_TIME);
				open_socket();
				continue;
			}
			if (FD_ISSET(m_sock, &rfds))
			{
				//�ӹ㲥��ַ������Ϣ��ע�������󶨵ĵ�ַ�ͽ�����Ϣ�ĵ�ַ�ǲ�һ����
				addr_len = sizeof(m_LocalAddr);
				len = recvfrom(m_sock, recv_buf, MAX_LIDAR_FRAME_LEN, 0, (struct sockaddr FAR *)&m_LocalAddr, (int FAR *)&addr_len);
				if (SOCKET_ERROR == len)
				{
					err = WSAGetLastError();
					if (WSAEWOULDBLOCK != err)
					{  // ����
						printf("probe socket fail(recvfrom),reconnect .....\n");
						close_socket();
						Sleep(CLOSE_OPEN_INTERVAL_TIME);
						open_socket();
					}
					break;
				}
				if (0 >= len)
					continue;
				// �����ݷ��뻷�λ�����
				err = m_pRingBuf->writeRingBuf((unsigned char*)recv_buf, len);
				if (LIDARSDK_ERR_FAIL == err)
				{
					printf("write lidar to ring buffer fail(no enought room)\n");
				}
			}
		}

		// ����ѭ���������е��״�����֡
		while (NULL != (lim = m_pRingBuf->readLidarData()))
		{
			long ip;
			ULDINI_Type *uldini;
			switch (lim->nCode)
			{
				case LIM_CODE_LDBCONFIG:
					tmLastProbe = time(NULL);  // ˢ��̽�⵽�״��ʱ��

					uldini = (ULDINI_Type*)LIM_ExData(lim);
					ip = m_pLidarList->get_ip(uldini->szIP);
					pNode = m_pLidarList->find_node(ip);
					if (NULL != pNode)
					{
						m_pLidarList->refresh_node_time(pNode);  //�״�ڵ��Ѵ��ڣ�ֻˢ��ʱ��
					}
					else
					{
						pNode = m_pLidarList->req_node(); // ����һ���нڵ�
						if (NULL != pNode)
						{
							pNode->tm = time(NULL);
							pNode->ip = m_pLidarList->get_ip(uldini->szIP);
							memcpy((void*)&pNode->ldini, (void*)uldini, sizeof(ULDINI_Type));
							m_pLidarList->insert_node(pNode); // ���½ڵ��������
						}
						if (NULL != m_callbackProbeFunc)
							m_callbackProbeFunc(1, uldini, m_callbackProbeFuncParam);  //���ûص�������flag=1����ʾ�״����·��ֵ�
					}
					break;
				default:
					printf("probe thread : unknow lim packet(nCode=%d)\n",lim->nCode);
					break;
			}
		}
	}

	return 0;
}
