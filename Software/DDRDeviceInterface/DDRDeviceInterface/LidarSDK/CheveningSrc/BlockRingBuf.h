#pragma once

#include <WinBase.h>

// ѭ���黺��������m_nBlockNum�����ݿ���ɣ�ÿ�����ݿ��Сm_nBlockSize)
// ��������LIM���ݰ�

class CBlockRingBuf
{
public:
	CBlockRingBuf( );  
	CBlockRingBuf(int nBlockNum, int nBlockSize);  //��������ʱ��ָ�����ݿ��С���Ϳ���
	~CBlockRingBuf();

	int get_block_size();
	int get_block_num();

	/******************************
	�����������λ�ã������ݷ��뻺������
	���û�пռ䣬�ȴ�dwMilliseconds���룬���dwMilliseconds = -1�����޵ȴ���
	���أ� LIDARSDK_ERR_SUCCESS ,�ɹ���
	LIDARSDK_ERR_TIMEOUT����ʱ��
	LIDARSDK_ERR_FAIL����������len���ȳ���m_nBlockSize��
	******************************/
	int put(char* buf, int len, int dwMilliseconds);

	// ���Ի�ȡ��ָ��
	// ���û�����ݣ��ȴ�dwMilliseconds���룬���dwMilliseconds =-1 �����޵ȴ���
	// �ɹ����ػ�����ָ�룬���򷵻�NULL
	char* get_getbuffer(int dwMilliseconds);
	// �������get_getbuffer()���ص�ָ��ǿգ���Ҫ����get_finish()ȥ���ʣ��Ĺ���
	int get_finish();

protected:
	int m_nBlockSize;  // ÿ�����ݿ��С
	int m_nBlockNum;	// ���ݿ�����
	char* m_buf;  // ����m_nBlockSize*m_nBlockNum��С
	int m_rp, m_wp;	// ѭ����������дָ��
	HANDLE m_hSemRead;  // ���ڶ����ź�������ʾ�������ж��ٿ����ݿ��ã���ʼֵΪ0
	HANDLE m_hSemWrite;	// ����д���ź�������ʾ�������ж��ٿ����ݿ�д����ʼֵΪm_nBlockNum
};

