#pragma once

#ifndef __LIDAR_COMM_H__
#define __LIDAR_COMM_H__

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>

#include "RingBuf.h"
#include "BlockRingBuf.h"

#define HB_INTERVAL_TIME	5	//发送心跳包间隔时间(s)

typedef void(*LIDAR_DATA_CALLBACK)(int _id,void* _lim, void *_data);	//雷达数据回调函数类型
typedef void(*LIDAR_STATE_CALLBACK)(int _id, void* _lim, void *_data);	//雷达状态变化时回调函数类型

class CLidarComm
{
private:
	static unsigned int m_idSDKBase;  // 基ID，open_lidar()时，每次在这基础上加一

	unsigned int m_idLidar; //雷达ID,保存雷达回应的报文里的ID，与雷达通信用这个ID
	int m_idSDK;	// SDK分配的ID，在m_idSDKBase基础上分配，与APP通信时，使用这个ID

	char m_ip[64];
	int m_port;

	ULDINI_Type m_ldcfg; //保存雷达信息
	HANDLE m_eventLDCfg;  //向雷达发送LIM_CODE_GET_LDBCONFIG命令后，通过m_eventLDCfg事件等待雷达的应答 
	bool m_bLdCfgWiatFlag;  //=true,调用LIM_CODE_GET_LDBCONFIG时等待查询结果；=false,表示在回调函数中处理；

	// 区域组监测状态（Regionals surveillance status)
	// m_nRS高字节，区域组编号
	// m_nRS低字节,
	//		Bit0:告警信号状态
	//		Bit1:告警信号总状态
	//		Bit2 : 预警信号状态
	//		Bit3 : 预警信号总状态
	//		Bit4 : 注意信号状态
	//		Bit5 : 注意信号总状态
	//		Bit6 ~bit7 : 0
	unsigned short m_nRS; // 报警区状态
	HANDLE m_eventRS;  // 向雷达发送LIM_CODE_FMSIG_QUERY命令后，通过m_eventRS事件等待雷达的应答
	int m_RSNo; //不等于-1，表示用户查询了m_RSNo的区域组状态，要等待查询结果;=-1，表示不等待查询结果，在回调函数中处理

	// IO口状态
	//bit0 - 3：OUT1 - OUT4
	//bit4 - 7 ：IN1 - IN4
	int m_nIOStatus;  
	HANDLE m_eventIO; // 向雷达发送LIM_CODE_IOREAD命令后，通过m_eventIO事件等待雷达的应答
	bool m_bIOWaitFlag;  //=true，表示查询/设置IO后，等待结果再返回；=false，表示不等待执行结果，在回调函数中处理

	// socket相关的线程、缓冲区、变量
	CRingBuf *m_pRingBuf;  //环形缓冲区对象
	SOCKET m_sock;  //套接口
	struct sockaddr_in m_LocalAddr;
	HANDLE m_hLidarCommThread;
	bool	m_bLidarComm;		//=true，雷达通信线程在运行；=false，退出雷达通信线程

	int	LidarCommThread(void);  //雷达通信线程
	static unsigned __stdcall LidarCommThreadEntry(void* lpParameter);

	// 雷达测量数据相关的 缓冲区、线程、回调函数、变量
	CBlockRingBuf m_bufLMD; // 雷达测量数据缓冲区
	HANDLE m_hLidarDataThread;
	bool	m_bLidarData;		//=true，处理雷达数据线程在运行；=false，退出雷达数据线程
	LIDAR_DATA_CALLBACK m_callbackDataFunc;  //保存探测雷达回调函数指针
	void *m_callbackDataFuncParam;  //作为参数回传给回调函数

	int	LidarDataThread(void);  //雷达数据线程，从循环区中取雷达测量数据，调用APP注册的回调函数
	static unsigned __stdcall LidarDataThreadEntry(void* lpParameter);

	// 雷达状态变化时相关的  缓冲区、线程、回调函数
	CBlockRingBuf m_bufState; // 雷达状态变化报文数据缓冲区
	HANDLE m_hLidarStateThread;
	bool	m_bLidarState;		//=true，处理雷达状态线程在运行；=false，退出雷达状态线程
	LIDAR_STATE_CALLBACK m_callbackStateFunc;  //保存探测雷达回调函数指针
	void *m_callbackStateFuncParam;  //作为参数回传给回调函数

	int	LidarStateThread(void);  //雷达状态线程，从循环区中取雷达状态变化LIM报文包，调用APP注册的回调函数
	static unsigned __stdcall LidarStateThreadEntry(void* lpParameter);

public:
	CLidarComm();
	~CLidarComm();

	int get_id();  // 获取雷达ID
	int send_lim(int id, char *lim, int len);

	int start_lidar_config(int id);
	int stop_lidar_config(int id);
	int get_lidar_config(int id, ULDINI_Type* _ldcfg, bool bWaitFlag); 
	int query_fmsig(int id, int _port, bool bWaitFlag);
	int read_iostatus(int id, bool bWaitFlag);
	int set_io_out(int id, int _port, int _status, bool bWaitFlag);
	int release_io_out(int id, bool bWaitFlag);

	int open_lidar(char *ip, int port, int flag);  //启动lidar，建立与雷达的TCP连接,启动雷达测量数据
	int close_lidar(int id);  //停止雷达测量数据，调用clear()清理线程，缓冲区，套接口

	int reg_data_callback(int id, void *_func, void *_data);  //注册探测雷达回调函数
	int reg_state_callback(int id, void *_func, void *_data);  //注册探测雷达回调函数

private:
	void clear();  //关闭线程（通信、数据、状态），并等待线程退出；释放环形缓冲区，关闭打开的套接口
	int connect_lidar(char *ip, int port);  //创建SOCKET，与雷达连接
	int reopen_lidar();  //网络出问题时，需要关闭旧的套接口。然后与雷达重新建立新的连接，启动雷达测量数据,分配雷达ID。但通信线程，缓冲区保留。
	int req_lidar_data(int id); //请求雷达测量数据
	int stop_lidar_data(int id); //停止雷达测量数据
};

#endif

