#pragma once

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>

#include "RingBuf.h"
#include "LidarList.h"

#define CLOSE_OPEN_INTERVAL_TIME	500  //ms

typedef void(*LIDAR_PROBE_CALLBACK)(int flag, ULDINI_Type* ldini, void *_data);  //雷达探测回调函数类型

class CLidarProbe
{
public:
	CLidarProbe();
	~CLidarProbe();

	/*************** 探测雷达相关函数 ***************************/
	int start_probe_lidar();  //启动探测lidar
	int stop_probe_lidar();  //停止探测雷达
	int reg_probe_callback(void *_func,void *_data);  //注册探测雷达回调函数

private:
	// 发现先启动监听多播线程，再启动雷达，监听线程收不到雷达的多播包，
	// 但用抓包程序抓包，发现网卡是收到雷达的多播消息的。通过实验，初步判断是雷达的多播程序有问题。
	// 解决的办法是，在监听多播线程中，如果超时没有收到雷达的多播消息，就重新初始化网络
	//注意： 在关闭SOCKET后，不要马上打开，要延迟一会（譬如500ms）。
	int open_socket();
	int close_socket();

	/************** 与雷达探测相关变量、函数 ********************/
	CRingBuf *m_pRingBuf;  //环形缓冲区对象
	CLidarList *m_pLidarList;  // 探测到的雷达列表

	HANDLE m_hLidarProbeThread;
	bool	m_bLidarProbe;		//=true，雷达探测线程在运行；=false，退出雷达探测线程
	int	LidarProbeThread(void);  //雷达探测线程
//	static void __cdecl LidarProbeThreadEntry(void* lpParameter);  //雷达探测线程入口
	static unsigned __stdcall LidarProbeThreadEntry(void* lpParameter);

	SOCKET m_sock;  //用于接收多播雷达信息的套接口
	struct sockaddr_in m_LocalAddr;
	struct ip_mreq m_mreq;/*加入多播组*/

	LIDAR_PROBE_CALLBACK m_callbackProbeFunc;  //保存探测雷达回调函数指针
	void *m_callbackProbeFuncParam;  //作为参数回传给回调函数

};

