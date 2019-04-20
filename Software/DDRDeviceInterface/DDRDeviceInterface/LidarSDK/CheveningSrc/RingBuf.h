#pragma once

#include "lim.h"

#define RING_BUF_SIZE	64000	// ���λ�������С
#define MAX_LIDAR_FRAME_LEN  4800   // �״�ɨ��һȦ��������һ֡������һ֡�����ֽ���

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

	BOOL isReady();  //�жϻ��λ������Ƿ�׼����
	int writeRingBuf(unsigned char* buf, int len); // �����λ�������д����
	LIM_HEAD* readLidarData(); // �ӻ��λ������ж��״�����

private:
	RING_BUF m_bufRing;
	int initRingBuf();  //��ʼ�����λ�����
	int destroyRingBuf(); // ���ٻ��λ�����

	// �ڻ��λ�������Ѱ��LIM_HEAD.TAG���ҵ�����TAG�Ŀ�ʼλ�ã����򷵻�-1
	int findLimTag();
	//�ӻ��λ������ж�len�ֽڣ�����buf(���޸Ļ��λ�������count��rp)
	int readRingBuf(unsigned char* buf, int len);
};

