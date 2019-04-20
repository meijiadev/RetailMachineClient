#pragma once

#include <WinBase.h>

// 循环块缓冲区，由m_nBlockNum块数据块组成（每块数据块大小m_nBlockSize)
// 缓冲区放LIM数据包

class CBlockRingBuf
{
public:
	CBlockRingBuf( );  
	CBlockRingBuf(int nBlockNum, int nBlockSize);  //创建对象时，指定数据块大小，和块数
	~CBlockRingBuf();

	int get_block_size();
	int get_block_num();

	/******************************
	如果缓冲区有位置，把数据放入缓冲区；
	如果没有空间，等待dwMilliseconds毫秒，如果dwMilliseconds = -1，无限等待；
	返回： LIDARSDK_ERR_SUCCESS ,成功；
	LIDARSDK_ERR_TIMEOUT，超时；
	LIDARSDK_ERR_FAIL，出错，包括len长度超过m_nBlockSize；
	******************************/
	int put(char* buf, int len, int dwMilliseconds);

	// 尝试获取读指针
	// 如果没有数据，等待dwMilliseconds毫秒，如果dwMilliseconds =-1 ，无限等待；
	// 成功返回缓冲块的指针，否则返回NULL
	char* get_getbuffer(int dwMilliseconds);
	// 如果调用get_getbuffer()返回的指针非空，需要调用get_finish()去完成剩余的工作
	int get_finish();

protected:
	int m_nBlockSize;  // 每块数据块大小
	int m_nBlockNum;	// 数据块数量
	char* m_buf;  // 分配m_nBlockSize*m_nBlockNum大小
	int m_rp, m_wp;	// 循环缓冲区读写指针
	HANDLE m_hSemRead;  // 用于读的信号量，表示缓冲区有多少块数据可用，初始值为0
	HANDLE m_hSemWrite;	// 用于写的信号量，表示缓冲区有多少块数据可写，初始值为m_nBlockNum
};

