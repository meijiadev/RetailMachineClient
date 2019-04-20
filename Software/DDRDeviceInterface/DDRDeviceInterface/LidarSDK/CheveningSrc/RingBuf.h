#pragma once

#include "lim.h"

#define RING_BUF_SIZE	64000	// 环形缓冲区大小
#define MAX_LIDAR_FRAME_LEN  4800   // 雷达扫描一圈的数据是一帧，定义一帧最大的字节数

typedef struct _RING_BUF
{
	unsigned char *buf;
	int rp, wp;
	int count;
//	HANDLE mutex;

	unsigned char* bufLim;
	LIM_HEAD* pLim;
} RING_BUF;

class CRingBuf
{
public:
	CRingBuf();
	~CRingBuf();

	BOOL isReady();  //判断环形缓冲区是否准备好
	int writeRingBuf(unsigned char* buf, int len); // 往环形缓冲区中写数据
	LIM_HEAD* readLidarData(); // 从环形缓冲区中读雷达数据

private:
	RING_BUF m_bufRing;
	int initRingBuf();  //初始化环形缓冲区
	int destroyRingBuf(); // 销毁环形缓冲区

	// 在环形缓冲区中寻找LIM_HEAD.TAG，找到返回TAG的开始位置，否则返回-1
	int findLimTag();
	//从环形缓冲区中读len字节，放入buf(不修改环形缓冲区的count和rp)
	int readRingBuf(unsigned char* buf, int len);
};

