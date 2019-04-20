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
	m_eventLDCfg = CreateEvent(NULL, false/*自动恢复*/, false/*无信号*/, NULL);
	m_bLdCfgWiatFlag = false;

	m_nRS = 0;
	m_eventRS = CreateEvent(NULL, false/*自动恢复*/, false/*无信号*/, NULL);
	m_RSNo = -1;

	m_nIOStatus = 0;
	m_eventIO = CreateEvent(NULL, false/*自动恢复*/, false/*无信号*/, NULL);
	m_bIOWaitFlag = false;

	// socket相关
	m_pRingBuf = NULL;  //环形缓冲区对象
	m_sock = INVALID_SOCKET;  //用于接收多播雷达信息的套接口
	m_hLidarCommThread = NULL;
	m_bLidarComm = false;		//=true，雷达通信线程在运行；=false，退出雷达通信线程

	// 雷达测量数据相关
	m_hLidarDataThread = NULL;
	m_bLidarData = false;		//=true，处理雷达数据线程在运行；=false，退出雷达数据线程
	m_callbackDataFunc = NULL;  //保存探测雷达回调函数指针
	m_callbackDataFuncParam = NULL;  //作为参数回传给回调函数

	// 雷达状态相关
	m_hLidarStateThread = NULL;
	m_bLidarState = false;	
	m_callbackStateFunc = NULL;  //保存探测雷达回调函数指针
	m_callbackStateFuncParam = NULL;  //作为参数回传给回调函数
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
	pLim->nCID = m_idLidar;  //用雷达ID替换
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
	} while (nSended < len); // 数据要发送完毕才结束，一次不一定能发送完毕
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
	{ // 等待查询结果
		ResetEvent(m_eventLDCfg);  // 不管事件是否有信号，都将其复位为无信号
	}
	ret = send_lim(id,(char*)lim, sizeof(LIM_HEAD));
	if (sizeof(LIM_HEAD) != ret)
	{
		LIM_Release(lim);
		return LIDARSDK_ERR_FAIL;
	}
	LIM_Release(lim);

	if (false == bWaitFlag)
		return LIDARSDK_ERR_SUCCESS; // 在回调函数中处理结果
	//等待雷达应答
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
	{ // 等待结果再返回
		m_RSNo = _port;
		ResetEvent(m_eventRS);  // 不管事件是否有信号，都将其复位为无信号
	}
	ret = send_lim(id, (char*)lim, sizeof(LIM_HEAD));
	if (sizeof(LIM_HEAD) != ret)
	{
		LIM_Release(lim);
		return LIDARSDK_ERR_FAIL;
	}
	LIM_Release(lim);
	if (false == bWaitFlag)
		return LIDARSDK_ERR_SUCCESS; // 在回调函数中处理结果
	//等待雷达应答
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
		ResetEvent(m_eventIO);  // 不管事件是否有信号，都将其复位为无信号
	}
	ret = send_lim(id, (char*)lim, sizeof(LIM_HEAD));
	if (sizeof(LIM_HEAD) != ret)
	{
		LIM_Release(lim);
		return LIDARSDK_ERR_FAIL;
	}
	LIM_Release(lim);

	if (false == bWaitFlag)
		return LIDARSDK_ERR_SUCCESS;  //在回调函数中处理查询结果

	//等待雷达应答
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
		ResetEvent(m_eventIO);  // 不管事件是否有信号，都将其复位为无信号
	}
	ret = send_lim(id, (char*)lim, sizeof(LIM_HEAD));
	if (sizeof(LIM_HEAD) != ret)
	{
		LIM_Release(lim);
		return LIDARSDK_ERR_FAIL;
	}
	LIM_Release(lim);

	if (false == bWaitFlag)
		return LIDARSDK_ERR_SUCCESS; // 在回调函数中处理设置结果
	//等待雷达应答
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
		ResetEvent(m_eventIO);  // 不管事件是否有信号，都将其复位为无信号
	}
	ret = send_lim(id, (char*)lim, sizeof(LIM_HEAD));
	if (sizeof(LIM_HEAD) != ret)
	{
		LIM_Release(lim);
		return LIDARSDK_ERR_FAIL;
	}
	LIM_Release(lim);
	if (false == bWaitFlag)
		return LIDARSDK_ERR_SUCCESS; // 在回调函数中处理结果
	//等待雷达应答
	DWORD dwRet = WaitForSingleObject(m_eventIO, 500);
	if (WAIT_OBJECT_0 == dwRet)
	{ // 检查雷达IO输出与设置的是否一致
		return m_nIOStatus;
	}
	return LIDARSDK_ERR_FAIL;
}

void CLidarComm::clear()
{
	// 关闭socket通信线程
	m_bLidarComm = false;
	if (NULL != m_hLidarCommThread)
	{
		WaitForSingleObject(m_hLidarCommThread, INFINITE);  //等待线程退出
		CloseHandle(m_hLidarCommThread);  //关闭线程
		m_hLidarCommThread = NULL;
	}
	// 关闭雷达状态处理线程
	m_bLidarState = false;
	if (NULL != m_hLidarStateThread)
	{
		WaitForSingleObject(m_hLidarStateThread, INFINITE);  //等待线程退出
		CloseHandle(m_hLidarStateThread);  //关闭线程
		m_hLidarStateThread = NULL;
	}
	// 关闭雷达数据处理线程
	m_bLidarData = false;
	if (NULL != m_hLidarDataThread)
	{
		WaitForSingleObject(m_hLidarDataThread,INFINITE);  //等待线程退出
		CloseHandle(m_hLidarDataThread);  //关闭线程
		m_hLidarDataThread = NULL;
	}
	// 关闭tcp socket
	if (NULL != m_sock)
		closesocket(m_sock);
	m_sock = INVALID_SOCKET;
	Sleep(500);
	// 删除环形缓冲区
	if (NULL != m_pRingBuf)
		delete m_pRingBuf;
	m_pRingBuf = NULL;
}

int CLidarComm::open_lidar(char *ip, int port,int flag)  //启动探测lidar
{
	strcpy(m_ip, ip);
	m_port = port;

	// 分配缓冲区
	if (NULL == m_pRingBuf)
		m_pRingBuf = new CRingBuf;
	if (NULL == m_pRingBuf)
	{
		clear();
		return LIDARSDK_ERR_ALLOC_MEMORY;
	}
	// 连接雷达
	int ret = connect_lidar(ip, port);
	if (LIDARSDK_ERR_SUCCESS != ret)
	{
		clear();
		return ret;
	}
	// 启动雷达数据处理线程
	m_bLidarData = true;
	if (0 == (m_hLidarDataThread = (HANDLE)_beginthreadex(NULL, 0, LidarDataThreadEntry, this, 0, NULL)))
	{
		clear();
		return LIDARSDK_ERR_STARTTHREAD;
	}

	// 启动雷达状态线程
	m_bLidarState = true;
	if (0 == (m_hLidarStateThread = (HANDLE)_beginthreadex(NULL, 0, LidarStateThreadEntry, this, 0, NULL)))
	{
		clear();
		return LIDARSDK_ERR_STARTTHREAD;
	}

	// 启动雷达通信线程
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
int CLidarComm::reopen_lidar()  //网络出问题时，需要关闭旧的套接口。然后与雷达重新建立新的连接，启动雷达测量数据。但通信线程，缓冲区保留。
{
	int ret;
	ret = connect_lidar(m_ip, m_port);
	if (LIDARSDK_ERR_SUCCESS != ret)
		return ret;
	return req_lidar_data(m_idLidar);
}
int CLidarComm::close_lidar(int id)  //停止探测雷达
{
	if (0 == id)
		printf("CLidarComm::close_lidar()-----error ----- id=%d,m_idSDK=%d,m_idLidar=%d\n", id, m_idSDK, m_idLidar);
	stop_lidar_data(m_idLidar);
	clear();
	m_idLidar = 0;
	m_idSDK = 0;
	return LIDARSDK_ERR_SUCCESS;
}

int CLidarComm::req_lidar_data(int id) //请求雷达测量数据
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

int CLidarComm::stop_lidar_data(int id) //停止雷达测量数据
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

int CLidarComm::reg_data_callback(int id, void *_func, void *_data)  //注册雷达测量数据回调函数
{
	m_callbackDataFunc = (LIDAR_DATA_CALLBACK)_func;
	m_callbackDataFuncParam = _data;
	return 0;
}
int CLidarComm::reg_state_callback(int id, void *_func, void *_data)  //注册雷达状态变化回调函数
{
	m_callbackStateFunc = (LIDAR_STATE_CALLBACK)_func;
	m_callbackStateFuncParam = _data;
	return 0;
}

unsigned __stdcall CLidarComm::LidarCommThreadEntry(void* lpParameter)
{
	CLidarComm	*pThread = NULL;
	pThread = static_cast<CLidarComm *>(lpParameter);   //编译器隐式执行的任何类型转换都可以由static_cast显式完成
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
	LIM_HEAD lim_hb; //心跳包
	time_t tm,tmLastHB;  //上次心跳时间
	char recv_buf[MAX_LIDAR_FRAME_LEN];  //临时接收缓冲区
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
		// 发送心跳包
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

		//  接收来自雷达的数据,放入循环缓冲区中
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
					{  // 重连
						printf("comm socket fail(recv),reconnect .....\n");
						reopen_lidar();
						continue;
					}
					break;
				}
				if (0 >= len)
					continue;
				// 把数据放入环形缓冲区
				ret = m_pRingBuf->writeRingBuf((unsigned char*)recv_buf, len);
				if (LIDARSDK_ERR_FAIL == ret)
				{
					printf("write lidar to ring buffer fail(no enought room)\n");
				}
			}
		}

		// 分析雷达数据包，按类别处理
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
				case LIM_CODE_LDBCONFIG:  // 雷达配置信息
					if (m_bLdCfgWiatFlag)
					{
						uldini = (ULDINI_Type*)LIM_ExData(lim);
						memcpy_s((void*)&m_ldcfg, sizeof(ULDINI_Type), (void*)uldini, sizeof(ULDINI_Type));
						::SetEvent(m_eventLDCfg);
						m_bLdCfgWiatFlag = false;
					}
					else
					{
						m_bufState.put((char*)lim, lim->nLIMLen, 0); // 在回调函数处理
						printf("LDBCONFIG info\n");
					}
					break;
				case LIM_CODE_FMSIG: // 区域监测信号
					if (0 > m_RSNo)
					{
						m_bufState.put((char*)lim, lim->nLIMLen, 0);
						//printf("alarm region %d: %x\n",lim->Data[0],lim->Data[1]); // young add. 
					}
					else
					{
						m_nRS = lim->Data[0]; //区域组
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
		}// 分析雷达数据包
	} // 主循环

	return 0;
}

unsigned __stdcall CLidarComm::LidarDataThreadEntry(void* lpParameter)
{
	CLidarComm	*pThread = NULL;
	pThread = static_cast<CLidarComm *>(lpParameter);   //编译器隐式执行的任何类型转换都可以由static_cast显式完成
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
		// 从循环缓冲区取数据
		lim = (LIM_HEAD*)m_bufLMD.get_getbuffer(500);
		if (NULL == lim)
			continue;

		// 调用注册的回调函数处理
		if (NULL != m_callbackDataFunc)
			m_callbackDataFunc(m_idSDK, lim, m_callbackDataFuncParam); 

		// 释放循环缓冲区的数据块
		m_bufLMD.get_finish();
	}
	return 0;
}

unsigned __stdcall CLidarComm::LidarStateThreadEntry(void* lpParameter)
{
	CLidarComm	*pThread = NULL;
	pThread = static_cast<CLidarComm *>(lpParameter);   //编译器隐式执行的任何类型转换都可以由static_cast显式完成
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
		// 从循环缓冲区取数据
		lim = (LIM_HEAD*)m_bufState.get_getbuffer(500);
		if (NULL == lim)
			continue;

		// 调用注册的回调函数处理
		if (NULL != m_callbackStateFunc)
			m_callbackStateFunc(m_idSDK, lim, m_callbackStateFuncParam); 

		// 释放循环缓冲区的数据块
		m_bufState.get_finish();
	}
	return 0;
}

