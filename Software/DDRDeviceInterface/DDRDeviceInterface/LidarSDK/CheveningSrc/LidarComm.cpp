//#include "stdafx.h"
#include "LidarComm.h"

#include <stdio.h>
#include <process.h>
#include <time.h>
#include <Winsock2.h>

#include "LidarSDK_errno.h"
#include "lim.h"
#include "common.h"

unsigned int CLidarComm::m_idSDKBase = 1;

CLidarComm::CLidarComm()
{
	m_idSDK = 0;
	m_idLidar = 0;

	memset((void*)&m_ldcfg, 0, sizeof(ULDINI_Type));
	m_eventLDCfg = CreateEvent(NULL, false/*�Զ��ָ�*/, false/*���ź�*/, NULL);
	m_bLdCfgWiatFlag = false;

	m_nRS = 0;
	m_eventRS = CreateEvent(NULL, false/*�Զ��ָ�*/, false/*���ź�*/, NULL);
	m_RSNo = -1;

	m_nIOStatus = 0;
	m_eventIO = CreateEvent(NULL, false/*�Զ��ָ�*/, false/*���ź�*/, NULL);
	m_bIOWaitFlag = false;

	// socket���
	m_pRingBuf = NULL;  //���λ���������
	m_sock = INVALID_SOCKET;  //���ڽ��նಥ�״���Ϣ���׽ӿ�
	m_hLidarCommThread = NULL;
	m_bLidarComm = false;		//=true���״�ͨ���߳������У�=false���˳��״�ͨ���߳�

	// �״�����������
	m_hLidarDataThread = NULL;
	m_bLidarData = false;		//=true�������״������߳������У�=false���˳��״������߳�
	m_callbackDataFunc = NULL;  //����̽���״�ص�����ָ��
	m_callbackDataFuncParam = NULL;  //��Ϊ�����ش����ص�����

	// �״�״̬���
	m_hLidarStateThread = NULL;
	m_bLidarState = false;	
	m_callbackStateFunc = NULL;  //����̽���״�ص�����ָ��
	m_callbackStateFuncParam = NULL;  //��Ϊ�����ش����ص�����
}

CLidarComm::~CLidarComm()
{
	close_lidar(m_idSDK);
	if (NULL != m_eventLDCfg)
		::CloseHandle(m_eventLDCfg);
	m_eventLDCfg = NULL;
	if (NULL != m_eventRS)
		::CloseHandle(m_eventRS);
	m_eventRS = NULL;
	if (NULL != m_eventIO)
		::CloseHandle(m_eventIO);
	m_eventIO = NULL;
}

int CLidarComm::connect_lidar(char *ip, int port)
{
	int ret;

	if (INVALID_SOCKET != m_sock)
	{
		stop_lidar_data(m_idSDK);
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		Sleep(500);
	}
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sock == INVALID_SOCKET)
	{
		printf("Create Socket Failed::%s\n", GetLastError());
		return LIDARSDK_ERR_CONNFAIL;
	}
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(LIM_USER_PORT);
	servaddr.sin_addr.s_addr = inet_addr(ip);
	memset((void*)servaddr.sin_zero, 0x00, 8);
	printf("to link %s:%d\n", ip, LIM_USER_PORT);
	if (SOCKET_ERROR == connect(m_sock, (struct sockaddr *)&servaddr, sizeof(servaddr)))
	{
		ret = closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		perror("connect");
		return LIDARSDK_ERR_CONNFAIL;
	}
	return LIDARSDK_ERR_SUCCESS;
}

int CLidarComm::get_id()
{
	return m_idSDK;
}

int CLidarComm::send_lim(int id, char *lim, int len)
{
	int ret, nSended = 0;
	if (INVALID_SOCKET == m_sock)
		return LIDARSDK_ERR_CONNCLOSE;
	LIM_HEAD *pLim = (LIM_HEAD*)lim;
	pLim->nCID = m_idLidar;  //���״�ID�滻
	do
	{
		ret = send(m_sock, &lim[nSended], len - nSended, 0);
		if (SOCKET_ERROR == ret)
		{
			if (0 == nSended)
				return LIDARSDK_ERR_SENDFAIL;
			else
				return nSended;
		}
		nSended += ret;
	} while (nSended < len); // ����Ҫ������ϲŽ�����һ�β�һ���ܷ������
	return nSended;
}

int CLidarComm::start_lidar_config(int id)
{
	LIM_HEAD *lim;
	int ret;

	if (false == LIM_Pack(lim, m_idLidar, LIM_CODE_START_LDBCONFIG))
		return LIDARSDK_ERR_FAIL;
	ret = send_lim(id, (char*)lim, sizeof(LIM_HEAD));
	if (sizeof(LIM_HEAD) != ret)
	{
		LIM_Release(lim);
		return LIDARSDK_ERR_FAIL;
	}
	LIM_Release(lim);
	return LIDARSDK_ERR_SUCCESS;
}
int CLidarComm::stop_lidar_config(int id)
{
	LIM_HEAD *lim;
	int ret;

	if (false == LIM_Pack(lim, m_idLidar, LIM_CODE_STOP_LDBCONFIG))
		return LIDARSDK_ERR_FAIL;
	ret = send_lim(id, (char*)lim, sizeof(LIM_HEAD));
	if (sizeof(LIM_HEAD) != ret)
	{
		LIM_Release(lim);
		return LIDARSDK_ERR_FAIL;
	}
	LIM_Release(lim);
	return LIDARSDK_ERR_SUCCESS;
}

int CLidarComm::get_lidar_config(int id, ULDINI_Type* _ldcfg, bool bWaitFlag) 
{
	LIM_HEAD *lim;
	int ret;

	if (false == LIM_Pack(lim, m_idLidar, LIM_CODE_GET_LDBCONFIG))
		return LIDARSDK_ERR_FAIL;
	m_bLdCfgWiatFlag = bWaitFlag;
	if (bWaitFlag)
	{ // �ȴ���ѯ���
		ResetEvent(m_eventLDCfg);  // �����¼��Ƿ����źţ������临λΪ���ź�
	}
	ret = send_lim(id,(char*)lim, sizeof(LIM_HEAD));
	if (sizeof(LIM_HEAD) != ret)
	{
		LIM_Release(lim);
		return LIDARSDK_ERR_FAIL;
	}
	LIM_Release(lim);

	if (false == bWaitFlag)
		return LIDARSDK_ERR_SUCCESS; // �ڻص������д�����
	//�ȴ��״�Ӧ��
	DWORD dwRet = WaitForSingleObject(m_eventLDCfg, 500);
	if (WAIT_OBJECT_0 == dwRet)
	{
		memcpy_s((void*)_ldcfg, sizeof(ULDINI_Type), (void*)&m_ldcfg, sizeof(ULDINI_Type));
		return LIDARSDK_ERR_SUCCESS;
	}

	return LIDARSDK_ERR_FAIL;
}
int CLidarComm::query_fmsig(int id, int _port, bool bWaitFlag)
{
	LIM_HEAD *lim;
	int ret;

	if (false == LIM_Pack(lim, m_idLidar, LIM_CODE_FMSIG_QUERY))
		return LIDARSDK_ERR_FAIL;
	lim->Data[0] = _port;
	lim->CheckSum = LIM_CheckSum(lim);
	if (bWaitFlag)
	{ // �ȴ�����ٷ���
		m_RSNo = _port;
		ResetEvent(m_eventRS);  // �����¼��Ƿ����źţ������临λΪ���ź�
	}
	ret = send_lim(id, (char*)lim, sizeof(LIM_HEAD));
	if (sizeof(LIM_HEAD) != ret)
	{
		LIM_Release(lim);
		return LIDARSDK_ERR_FAIL;
	}
	LIM_Release(lim);
	if (false == bWaitFlag)
		return LIDARSDK_ERR_SUCCESS; // �ڻص������д�����
	//�ȴ��״�Ӧ��
	DWORD dwRet = WaitForSingleObject(m_eventRS, 500);
	if (WAIT_OBJECT_0 == dwRet)
	{
		return m_nRS;
	}
	return LIDARSDK_ERR_FAIL;
}
int CLidarComm::read_iostatus(int id, bool bWaitFlag)
{
	LIM_HEAD *lim;
	int ret;

	if (false == LIM_Pack(lim, m_idLidar, LIM_CODE_IOREAD))
		return LIDARSDK_ERR_FAIL;
	m_bIOWaitFlag = bWaitFlag;
	if (bWaitFlag)
	{
		ResetEvent(m_eventIO);  // �����¼��Ƿ����źţ������临λΪ���ź�
	}
	ret = send_lim(id, (char*)lim, sizeof(LIM_HEAD));
	if (sizeof(LIM_HEAD) != ret)
	{
		LIM_Release(lim);
		return LIDARSDK_ERR_FAIL;
	}
	LIM_Release(lim);

	if (false == bWaitFlag)
		return LIDARSDK_ERR_SUCCESS;  //�ڻص������д����ѯ���

	//�ȴ��״�Ӧ��
	DWORD dwRet = WaitForSingleObject(m_eventIO, 500);
	if (WAIT_OBJECT_0 == dwRet)
	{
		return m_nIOStatus;
	}
	return LIDARSDK_ERR_FAIL;
}
int CLidarComm::set_io_out(int id, int _port, int _status,bool bWaitFlag)
{
	LIM_HEAD *lim;
	int ret;

	if (false == LIM_Pack(lim, m_idLidar, LIM_CODE_IOSET,NULL,0,NULL))
		return LIDARSDK_ERR_FAIL;
	_status = (0 != _status) ? 1 : 0;
	lim->Data[0] = (unsigned int)_status;
	lim->Data[1] = (unsigned int)_port;
	lim->CheckSum = LIM_CheckSum(lim);
	m_bIOWaitFlag = bWaitFlag;
	if (bWaitFlag)
	{
		ResetEvent(m_eventIO);  // �����¼��Ƿ����źţ������临λΪ���ź�
	}
	ret = send_lim(id, (char*)lim, sizeof(LIM_HEAD));
	if (sizeof(LIM_HEAD) != ret)
	{
		LIM_Release(lim);
		return LIDARSDK_ERR_FAIL;
	}
	LIM_Release(lim);

	if (false == bWaitFlag)
		return LIDARSDK_ERR_SUCCESS; // �ڻص������д������ý��
	//�ȴ��״�Ӧ��
	DWORD dwRet = WaitForSingleObject(m_eventIO, 500);
	if (WAIT_OBJECT_0 == dwRet)
	{ 
		return m_nIOStatus & 0x0F;
	}
	return LIDARSDK_ERR_FAIL;
}
int CLidarComm::release_io_out(int id, bool bWaitFlag)
{
	LIM_HEAD *lim;
	int ret;

	if (false == LIM_Pack(lim, m_idLidar, LIM_CODE_IOSET_RELEASE))
		return LIDARSDK_ERR_FAIL;
	m_bIOWaitFlag = bWaitFlag;
	if (bWaitFlag)
	{
		ResetEvent(m_eventIO);  // �����¼��Ƿ����źţ������临λΪ���ź�
	}
	ret = send_lim(id, (char*)lim, sizeof(LIM_HEAD));
	if (sizeof(LIM_HEAD) != ret)
	{
		LIM_Release(lim);
		return LIDARSDK_ERR_FAIL;
	}
	LIM_Release(lim);
	if (false == bWaitFlag)
		return LIDARSDK_ERR_SUCCESS; // �ڻص������д�����
	//�ȴ��״�Ӧ��
	DWORD dwRet = WaitForSingleObject(m_eventIO, 500);
	if (WAIT_OBJECT_0 == dwRet)
	{ // ����״�IO��������õ��Ƿ�һ��
		return m_nIOStatus;
	}
	return LIDARSDK_ERR_FAIL;
}

void CLidarComm::clear()
{
	// �ر�socketͨ���߳�
	m_bLidarComm = false;
	if (NULL != m_hLidarCommThread)
	{
		WaitForSingleObject(m_hLidarCommThread, INFINITE);  //�ȴ��߳��˳�
		CloseHandle(m_hLidarCommThread);  //�ر��߳�
		m_hLidarCommThread = NULL;
	}
	// �ر��״�״̬�����߳�
	m_bLidarState = false;
	if (NULL != m_hLidarStateThread)
	{
		WaitForSingleObject(m_hLidarStateThread, INFINITE);  //�ȴ��߳��˳�
		CloseHandle(m_hLidarStateThread);  //�ر��߳�
		m_hLidarStateThread = NULL;
	}
	// �ر��״����ݴ����߳�
	m_bLidarData = false;
	if (NULL != m_hLidarDataThread)
	{
		WaitForSingleObject(m_hLidarDataThread,INFINITE);  //�ȴ��߳��˳�
		CloseHandle(m_hLidarDataThread);  //�ر��߳�
		m_hLidarDataThread = NULL;
	}
	// �ر�tcp socket
	if (NULL != m_sock)
		closesocket(m_sock);
	m_sock = INVALID_SOCKET;
	Sleep(500);
	// ɾ�����λ�����
	if (NULL != m_pRingBuf)
		delete m_pRingBuf;
	m_pRingBuf = NULL;
}

int CLidarComm::open_lidar(char *ip, int port,int flag)  //����̽��lidar
{
	strcpy(m_ip, ip);
	m_port = port;

	// ���仺����
	if (NULL == m_pRingBuf)
		m_pRingBuf = new CRingBuf;
	if (NULL == m_pRingBuf)
	{
		clear();
		return LIDARSDK_ERR_ALLOC_MEMORY;
	}
	// �����״�
	int ret = connect_lidar(ip, port);
	if (LIDARSDK_ERR_SUCCESS != ret)
	{
		clear();
		return ret;
	}
	// �����״����ݴ����߳�
	m_bLidarData = true;
	if (0 == (m_hLidarDataThread = (HANDLE)_beginthreadex(NULL, 0, LidarDataThreadEntry, this, 0, NULL)))
	{
		clear();
		return LIDARSDK_ERR_STARTTHREAD;
	}

	// �����״�״̬�߳�
	m_bLidarState = true;
	if (0 == (m_hLidarStateThread = (HANDLE)_beginthreadex(NULL, 0, LidarStateThreadEntry, this, 0, NULL)))
	{
		clear();
		return LIDARSDK_ERR_STARTTHREAD;
	}

	// �����״�ͨ���߳�
	m_bLidarComm = true;
	if (0 == (m_hLidarCommThread = (HANDLE)_beginthreadex(NULL, 0, LidarCommThreadEntry, this, 0, NULL)))
	{
		clear();
		return LIDARSDK_ERR_STARTTHREAD;
	}

	if (LIDARSDK_ERR_FAIL == req_lidar_data(m_idLidar))
	{
		clear();
		return LIDARSDK_ERR_FAIL;
	}

	m_idSDKBase++;
	m_idSDK = m_idSDKBase;
	return m_idSDK;
}
int CLidarComm::reopen_lidar()  //���������ʱ����Ҫ�رվɵ��׽ӿڡ�Ȼ�����״����½����µ����ӣ������״�������ݡ���ͨ���̣߳�������������
{
	int ret;
	ret = connect_lidar(m_ip, m_port);
	if (LIDARSDK_ERR_SUCCESS != ret)
		return ret;
	return req_lidar_data(m_idLidar);
}
int CLidarComm::close_lidar(int id)  //ֹͣ̽���״�
{
	if (0 == id)
		printf("CLidarComm::close_lidar()-----error ----- id=%d,m_idSDK=%d,m_idLidar=%d\n", id, m_idSDK, m_idLidar);
	stop_lidar_data(m_idLidar);
	clear();
	m_idLidar = 0;
	m_idSDK = 0;
	return LIDARSDK_ERR_SUCCESS;
}

int CLidarComm::req_lidar_data(int id) //�����״��������
{
	LIM_HEAD *lim;
	int ret;

	if (LIM_Pack(lim, m_idLidar, LIM_CODE_START_LMD))
	{
		ret = send_lim(id,(char*)lim, sizeof(LIM_HEAD));
		if (ret != sizeof(LIM_HEAD))
		{
			LIM_Release(lim);
			return LIDARSDK_ERR_FAIL;
		}
		LIM_Release(lim);
		return LIDARSDK_ERR_SUCCESS;
	}
	return LIDARSDK_ERR_FAIL;
}

int CLidarComm::stop_lidar_data(int id) //ֹͣ�״��������
{
	LIM_HEAD *lim;
	int ret;

	if (LIM_Pack(lim, m_idLidar, LIM_CODE_STOP_LMD))
	{
		ret = send_lim(id,(char*)lim, sizeof(LIM_HEAD));
		if (ret != sizeof(LIM_HEAD))
		{
			LIM_Release(lim);
			return LIDARSDK_ERR_FAIL;
		}
		LIM_Release(lim);
		return LIDARSDK_ERR_SUCCESS;
	}
	return LIDARSDK_ERR_FAIL;
}

int CLidarComm::reg_data_callback(int id, void *_func, void *_data)  //ע���״�������ݻص�����
{
	m_callbackDataFunc = (LIDAR_DATA_CALLBACK)_func;
	m_callbackDataFuncParam = _data;
	return 0;
}
int CLidarComm::reg_state_callback(int id, void *_func, void *_data)  //ע���״�״̬�仯�ص�����
{
	m_callbackStateFunc = (LIDAR_STATE_CALLBACK)_func;
	m_callbackStateFuncParam = _data;
	return 0;
}

unsigned __stdcall CLidarComm::LidarCommThreadEntry(void* lpParameter)
{
	CLidarComm	*pThread = NULL;
	pThread = static_cast<CLidarComm *>(lpParameter);   //��������ʽִ�е��κ�����ת����������static_cast��ʽ���
	if (pThread == NULL)
	{
		_endthreadex(0);
		return 0;
	}
	pThread->LidarCommThread();

	printf("exit comm thread,m_idSDK=%d,m_idLidar=%d\n", pThread->m_idSDK,pThread->m_idLidar);
	_endthreadex(0);
	return 0;
}

int	CLidarComm::LidarCommThread(void)
{
	int ret,len;
	LIM_HEAD lim_hb; //������
	time_t tm,tmLastHB;  //�ϴ�����ʱ��
	char recv_buf[MAX_LIDAR_FRAME_LEN];  //��ʱ���ջ�����
	LIM_HEAD* lim = NULL;
	fd_set rfds, efds;
	struct timeval tv;

	tmLastHB = time(NULL)-(time_t)2 * HB_INTERVAL_TIME;

	lim_hb.TAG = LIM_TAG;
	lim_hb.VER = LIM_VER;
	lim_hb.nCID = m_idLidar;
	memset((void*)lim_hb.Data, 0, sizeof(unsigned int)*LIM_DATA_LEN);
	lim_hb.nLIMLen = sizeof(LIM_HEAD);
	lim_hb.nCode = LIM_CODE_HB;
	lim_hb.CheckSum = LIM_CheckSum(&lim_hb);

	while (m_bLidarComm)
	{
		// ����������
		tm = time(NULL);
		if ((time_t)HB_INTERVAL_TIME < (tm - tmLastHB))
		{
			tmLastHB = tm;
			ret = send_lim(0,(char*)&lim_hb, sizeof(LIM_HEAD));
			if (LIDARSDK_ERR_SENDFAIL == ret)
			{
				reopen_lidar();
				continue;
			}
		}

		//  ���������״������,����ѭ����������
		FD_ZERO(&rfds);
		FD_ZERO(&efds);
		if (INVALID_SOCKET != m_sock)
		{
			FD_SET(m_sock, &rfds);
			FD_SET(m_sock, &efds);
		}
		tv.tv_sec = 0;
		tv.tv_usec = 35000; //35ms
		ret = ::select(-1, &rfds, NULL, &efds, &tv);
		if (SOCKET_ERROR == ret)
		{
			ret = WSAGetLastError();
			reopen_lidar();
			continue;
		}
		else if (0 != ret)
		{
			if (FD_ISSET(m_sock, &efds))
			{
				printf("error(efds) comm socket...\n");
				reopen_lidar();
				continue;
			}
			if (FD_ISSET(m_sock, &rfds))
			{
				len = recv(m_sock, recv_buf, MAX_LIDAR_FRAME_LEN, 0);
				if (SOCKET_ERROR == len)
				{
					ret = WSAGetLastError();
					if (WSAEWOULDBLOCK != ret)
					{  // ����
						printf("comm socket fail(recv),reconnect .....\n");
						reopen_lidar();
						continue;
					}
					break;
				}
				if (0 >= len)
					continue;
				// �����ݷ��뻷�λ�����
				ret = m_pRingBuf->writeRingBuf((unsigned char*)recv_buf, len);
				if (LIDARSDK_ERR_FAIL == ret)
				{
					printf("write lidar to ring buffer fail(no enought room)\n");
				}
			}
		}

		// �����״����ݰ����������
		while (NULL != (lim = m_pRingBuf->readLidarData()))
		{
			ULDINI_Type *uldini = NULL;
			unsigned short nRS;
			m_idLidar = lim->nCID;
			switch (lim->nCode)
			{
				case LIM_CODE_LMD:
				case LIM_CODE_LMD_RSSI:
					m_bufLMD.put((char*)lim, lim->nLIMLen, 0);
					break;
				case LIM_CODE_LDBCONFIG:  // �״�������Ϣ
					if (m_bLdCfgWiatFlag)
					{
						uldini = (ULDINI_Type*)LIM_ExData(lim);
						memcpy_s((void*)&m_ldcfg, sizeof(ULDINI_Type), (void*)uldini, sizeof(ULDINI_Type));
						::SetEvent(m_eventLDCfg);
						m_bLdCfgWiatFlag = false;
					}
					else
					{
						m_bufState.put((char*)lim, lim->nLIMLen, 0); // �ڻص���������
						printf("LDBCONFIG info\n");
					}
					break;
				case LIM_CODE_FMSIG: // �������ź�
					if (0 > m_RSNo)
					{
						m_bufState.put((char*)lim, lim->nLIMLen, 0);
						//printf("alarm region %d: %x\n",lim->Data[0],lim->Data[1]); // young add. 
					}
					else
					{
						m_nRS = lim->Data[0]; //������
						m_nRS <<= 8;
						nRS = lim->Data[1];
						nRS &= 0x00FF;
						m_nRS |= nRS;
						::SetEvent(m_eventRS);
						m_RSNo = -1;
					}
					break;
				case LIM_CODE_IOSTATUS:
					m_nIOStatus = lim->Data[0];
					if (m_bIOWaitFlag)
					{
						::SetEvent(m_eventIO);
						m_bIOWaitFlag = false;
					}
					else
					{
						m_bufState.put((char*)lim, lim->nLIMLen, 0);
						// printf("IO status : %x\n", m_nIOStatus); // young add. 
					}
					break;
				case LIM_CODE_ALARM:
				case LIM_CODE_DISALARM:
					m_bufState.put((char*)lim, lim->nLIMLen, 0);
					break;
				case LIM_CODE_HBACK:
					break;
				default:
					printf("unknow code(%d)!\n",lim->nCode);
					break;
			}
		}// �����״����ݰ�
	} // ��ѭ��

	return 0;
}

unsigned __stdcall CLidarComm::LidarDataThreadEntry(void* lpParameter)
{
	CLidarComm	*pThread = NULL;
	pThread = static_cast<CLidarComm *>(lpParameter);   //��������ʽִ�е��κ�����ת����������static_cast��ʽ���
	if (pThread == NULL)
	{
		_endthreadex(0);
		return 0;
	}
	pThread->LidarDataThread();

	printf("exit data thread,m_idSDK=%d,m_idLidar=%d\n", pThread->m_idSDK,pThread->m_idLidar);
	_endthreadex(0);
	return 0;
}

int	CLidarComm::LidarDataThread(void)
{
	LIM_HEAD* lim = NULL;

	while (m_bLidarData)
	{
		// ��ѭ��������ȡ����
		lim = (LIM_HEAD*)m_bufLMD.get_getbuffer(500);
		if (NULL == lim)
			continue;

		// ����ע��Ļص���������
		if (NULL != m_callbackDataFunc)
			m_callbackDataFunc(m_idSDK, lim, m_callbackDataFuncParam); 

		// �ͷ�ѭ�������������ݿ�
		m_bufLMD.get_finish();
	}
	return 0;
}

unsigned __stdcall CLidarComm::LidarStateThreadEntry(void* lpParameter)
{
	CLidarComm	*pThread = NULL;
	pThread = static_cast<CLidarComm *>(lpParameter);   //��������ʽִ�е��κ�����ת����������static_cast��ʽ���
	if (pThread == NULL)
	{
		_endthreadex(0);
		return 0;
	}
	pThread->LidarStateThread();

	printf("exit state thread,m_idSDK=%d,m_idLidar=%d\n", pThread->m_idSDK,pThread->m_idLidar);
	_endthreadex(0);
	return 0;
}

int	CLidarComm::LidarStateThread(void)
{
	LIM_HEAD* lim = NULL;

	while (m_bLidarState)
	{
		// ��ѭ��������ȡ����
		lim = (LIM_HEAD*)m_bufState.get_getbuffer(500);
		if (NULL == lim)
			continue;

		// ����ע��Ļص���������
		if (NULL != m_callbackStateFunc)
			m_callbackStateFunc(m_idSDK, lim, m_callbackStateFuncParam); 

		// �ͷ�ѭ�������������ݿ�
		m_bufState.get_finish();
	}
	return 0;
}

