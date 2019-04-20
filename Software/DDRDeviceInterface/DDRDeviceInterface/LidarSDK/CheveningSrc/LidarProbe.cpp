
#include <stdio.h>

#include "LidarSDK_errno.h"
#include "lim.h"
#include "common.h"
#include <process.h>
#include "LidarProbe.h"
#include "LidarList.h"

CLidarProbe::CLidarProbe()
{
	m_pRingBuf = NULL;  //环形缓冲区对象
	m_pLidarList = NULL;  // 探测到的雷达列表
	m_bLidarProbe = false;
	m_hLidarProbeThread = NULL;
	m_sock = INVALID_SOCKET;
	m_callbackProbeFunc = NULL;  //保存探测雷达回调函数指针
	m_callbackProbeFuncParam = NULL;  //作为参数回传给回调函数
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

	const int on = 1; //允许程序的多个实例运行在同一台机器上  
	/*	调用setsockopt()函数为套接字设置SO_REUSEADDR选项，以允许套接字绑扎到一个已在使用的地址上。设置套接字的选项	*/
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

	/*加入广播组*/
	m_mreq.imr_multiaddr.s_addr = inet_addr(LIM_DT_IP);//多播地址
	m_mreq.imr_interface.s_addr = htonl(INADDR_ANY); //网络接口为默认
	/*将本机加入多播组*/
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

int CLidarProbe::start_probe_lidar()  //启动探测lidar
{
	int ret;
	ret = open_socket();
//	if (LIDARSDK_ERR_SUCCESS != ret)
//		return ret;

	// 分配缓冲区
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

	// 启动探测雷达线程
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
int CLidarProbe::stop_probe_lidar()  //停止探测雷达
{
	// 通知探测线程退出
	if (NULL != m_hLidarProbeThread)
	{
		m_bLidarProbe = false;  //通知探测雷达线程退出
		WaitForSingleObject(m_hLidarProbeThread, INFINITE);  //等待线程退出
		CloseHandle(m_hLidarProbeThread);  //关闭线程
		m_hLidarProbeThread = NULL;
	}

	// 关闭SOCKET
	close_socket();

	// 释放缓冲区
	if (NULL != m_pRingBuf)
		delete m_pRingBuf;
	m_pRingBuf = NULL;
	if (NULL != m_pLidarList)
		delete m_pLidarList;
	m_pLidarList = NULL;

	return 0;

}

int CLidarProbe::reg_probe_callback(void *_func, void *_data)  //注册探测雷达回调函数
{
	m_callbackProbeFunc = (LIDAR_PROBE_CALLBACK)_func;
	m_callbackProbeFuncParam = _data;
	return 0;
}

//void __cdecl CLidarProbe::LidarProbeThreadEntry(void* lpParameter)
unsigned __stdcall CLidarProbe::LidarProbeThreadEntry(void* lpParameter)
{
	CLidarProbe	*pThread = NULL;
	pThread = static_cast<CLidarProbe *>(lpParameter);   //编译器隐式执行的任何类型转换都可以由static_cast显式完成
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
	char recv_buf[MAX_LIDAR_FRAME_LEN];  //临时接收缓冲区
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
		{  // 超时没有发现雷达，重新初始化SOCKET
#if 0 // 20190314 to meiya
#else
			close_socket();
			Sleep(CLOSE_OPEN_INTERVAL_TIME);
			open_socket();
			tmLastProbe = time(NULL);
#endif
		}

		// 检查一下雷达链表中是否有超时没有刷新的雷达节点
		// 把超时没刷新的雷达节点从链表中删除，用回调函数通知应用线程
		pNode = m_pLidarList->find_timeout_node();
		if (NULL != pNode)
		{
			if (NULL != m_callbackProbeFunc)
				m_callbackProbeFunc(-1, &pNode->ldini, m_callbackProbeFuncParam);  //调用回调函数，flag=-1，表示此节点已失效
			m_pLidarList->free_node(pNode);
		}

		// 接收多播端口数据，放入循环缓冲区中
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
				//从广播地址接收消息，注意用来绑定的地址和接收消息的地址是不一样的
				addr_len = sizeof(m_LocalAddr);
				len = recvfrom(m_sock, recv_buf, MAX_LIDAR_FRAME_LEN, 0, (struct sockaddr FAR *)&m_LocalAddr, (int FAR *)&addr_len);
				if (SOCKET_ERROR == len)
				{
					err = WSAGetLastError();
					if (WSAEWOULDBLOCK != err)
					{  // 重连
						printf("probe socket fail(recvfrom),reconnect .....\n");
						close_socket();
						Sleep(CLOSE_OPEN_INTERVAL_TIME);
						open_socket();
					}
					break;
				}
				if (0 >= len)
					continue;
				// 把数据放入环形缓冲区
				err = m_pRingBuf->writeRingBuf((unsigned char*)recv_buf, len);
				if (LIDARSDK_ERR_FAIL == err)
				{
					printf("write lidar to ring buffer fail(no enought room)\n");
				}
			}
		}

		// 处理循环缓冲区中的雷达数据帧
		while (NULL != (lim = m_pRingBuf->readLidarData()))
		{
			long ip;
			ULDINI_Type *uldini;
			switch (lim->nCode)
			{
				case LIM_CODE_LDBCONFIG:
					tmLastProbe = time(NULL);  // 刷新探测到雷达的时间

					uldini = (ULDINI_Type*)LIM_ExData(lim);
					ip = m_pLidarList->get_ip(uldini->szIP);
					pNode = m_pLidarList->find_node(ip);
					if (NULL != pNode)
					{
						m_pLidarList->refresh_node_time(pNode);  //雷达节点已存在，只刷新时间
					}
					else
					{
						pNode = m_pLidarList->req_node(); // 申请一空闲节点
						if (NULL != pNode)
						{
							pNode->tm = time(NULL);
							pNode->ip = m_pLidarList->get_ip(uldini->szIP);
							memcpy((void*)&pNode->ldini, (void*)uldini, sizeof(ULDINI_Type));
							m_pLidarList->insert_node(pNode); // 将新节点插入链表
						}
						if (NULL != m_callbackProbeFunc)
							m_callbackProbeFunc(1, uldini, m_callbackProbeFuncParam);  //调用回调函数，flag=1，表示雷达是新发现的
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
