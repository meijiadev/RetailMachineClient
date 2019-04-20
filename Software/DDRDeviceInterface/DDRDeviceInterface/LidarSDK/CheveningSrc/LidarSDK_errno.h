
#ifndef __LIDARSDK_ERR_NO_
#define __LIDARSDK_ERR_NO_

// 错误代码定义
#define LIDARSDK_ERR_SUCCESS			0		//成功，无错误
#define LIDARSDK_ERR_FAIL				-1		//失败

#define LIDARSDK_ERR_ALLOC_MEMORY		-101    //申请内存失败
#define LIDARSDK_ERR_ALLOC_MUTEX		-102	//创建互斥量失败
#define LIDARSDK_ERR_NOROOM				-103	//无空间
#define LIDARSDK_ERR_STARTTHREAD		-104	//启动线程失败

#define LIDARSDK_ERR_CONNFAIL			-105	//网络连接失败
#define LIDARSDK_ERR_CONNCLOSE			-106    //网络关闭
#define LIDARSDK_ERR_SENDFAIL			-107    //向雷达发送数据失败

#define LIDARSDK_ERR_TIMEOUT			-108	//超时
#endif
