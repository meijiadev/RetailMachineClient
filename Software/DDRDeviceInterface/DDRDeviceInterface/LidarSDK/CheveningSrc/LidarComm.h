#pragma once

#ifndef __LIDAR_COMM_H__
#define __LIDAR_COMM_H__

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>

#include "RingBuf.h"
#include "BlockRingBuf.h"

#define HB_INTERVAL_TIME	5	//�������������ʱ��(s)

typedef void(*LIDAR_DATA_CALLBACK)(int _id,void* _lim, void *_data);	//�״����ݻص���������
typedef void(*LIDAR_STATE_CALLBACK)(int _id, void* _lim, void *_data);	//�״�״̬�仯ʱ�ص���������

class CLidarComm
{
private:
	static unsigned int m_idSDKBase;  // ��ID��open_lidar()ʱ��ÿ����������ϼ�һ

	unsigned int m_idLidar; //�״�ID,�����״��Ӧ�ı������ID�����״�ͨ�������ID
	int m_idSDK;	// SDK�����ID����m_idSDKBase�����Ϸ��䣬��APPͨ��ʱ��ʹ�����ID

	char m_ip[64];
	int m_port;

	ULDINI_Type m_ldcfg; //�����״���Ϣ
	HANDLE m_eventLDCfg;  //���״﷢��LIM_CODE_GET_LDBCONFIG�����ͨ��m_eventLDCfg�¼��ȴ��״��Ӧ�� 
	bool m_bLdCfgWiatFlag;  //=true,����LIM_CODE_GET_LDBCONFIGʱ�ȴ���ѯ�����=false,��ʾ�ڻص������д���

	// ��������״̬��Regionals surveillance status)
	// m_nRS���ֽڣ���������
	// m_nRS���ֽ�,
	//		Bit0:�澯�ź�״̬
	//		Bit1:�澯�ź���״̬
	//		Bit2 : Ԥ���ź�״̬
	//		Bit3 : Ԥ���ź���״̬
	//		Bit4 : ע���ź�״̬
	//		Bit5 : ע���ź���״̬
	//		Bit6 ~bit7 : 0
	unsigned short m_nRS; // ������״̬
	HANDLE m_eventRS;  // ���״﷢��LIM_CODE_FMSIG_QUERY�����ͨ��m_eventRS�¼��ȴ��״��Ӧ��
	int m_RSNo; //������-1����ʾ�û���ѯ��m_RSNo��������״̬��Ҫ�ȴ���ѯ���;=-1����ʾ���ȴ���ѯ������ڻص������д���

	// IO��״̬
	//bit0 - 3��OUT1 - OUT4
	//bit4 - 7 ��IN1 - IN4
	int m_nIOStatus;  
	HANDLE m_eventIO; // ���״﷢��LIM_CODE_IOREAD�����ͨ��m_eventIO�¼��ȴ��״��Ӧ��
	bool m_bIOWaitFlag;  //=true����ʾ��ѯ/����IO�󣬵ȴ�����ٷ��أ�=false����ʾ���ȴ�ִ�н�����ڻص������д���

	// socket��ص��̡߳�������������
	CRingBuf *m_pRingBuf;  //���λ���������
	SOCKET m_sock;  //�׽ӿ�
	struct sockaddr_in m_LocalAddr;
	HANDLE m_hLidarCommThread;
	bool	m_bLidarComm;		//=true���״�ͨ���߳������У�=false���˳��״�ͨ���߳�

	int	LidarCommThread(void);  //�״�ͨ���߳�
	static unsigned __stdcall LidarCommThreadEntry(void* lpParameter);

	// �״����������ص� ���������̡߳��ص�����������
	CBlockRingBuf m_bufLMD; // �״�������ݻ�����
	HANDLE m_hLidarDataThread;
	bool	m_bLidarData;		//=true�������״������߳������У�=false���˳��״������߳�
	LIDAR_DATA_CALLBACK m_callbackDataFunc;  //����̽���״�ص�����ָ��
	void *m_callbackDataFuncParam;  //��Ϊ�����ش����ص�����

	int	LidarDataThread(void);  //�״������̣߳���ѭ������ȡ�״�������ݣ�����APPע��Ļص�����
	static unsigned __stdcall LidarDataThreadEntry(void* lpParameter);

	// �״�״̬�仯ʱ��ص�  ���������̡߳��ص�����
	CBlockRingBuf m_bufState; // �״�״̬�仯�������ݻ�����
	HANDLE m_hLidarStateThread;
	bool	m_bLidarState;		//=true�������״�״̬�߳������У�=false���˳��״�״̬�߳�
	LIDAR_STATE_CALLBACK m_callbackStateFunc;  //����̽���״�ص�����ָ��
	void *m_callbackStateFuncParam;  //��Ϊ�����ش����ص�����

	int	LidarStateThread(void);  //�״�״̬�̣߳���ѭ������ȡ�״�״̬�仯LIM���İ�������APPע��Ļص�����
	static unsigned __stdcall LidarStateThreadEntry(void* lpParameter);

public:
	CLidarComm();
	~CLidarComm();

	int get_id();  // ��ȡ�״�ID
	int send_lim(int id, char *lim, int len);

	int start_lidar_config(int id);
	int stop_lidar_config(int id);
	int get_lidar_config(int id, ULDINI_Type* _ldcfg, bool bWaitFlag); 
	int query_fmsig(int id, int _port, bool bWaitFlag);
	int read_iostatus(int id, bool bWaitFlag);
	int set_io_out(int id, int _port, int _status, bool bWaitFlag);
	int release_io_out(int id, bool bWaitFlag);

	int open_lidar(char *ip, int port, int flag);  //����lidar���������״��TCP����,�����״��������
	int close_lidar(int id);  //ֹͣ�״�������ݣ�����clear()�����̣߳����������׽ӿ�

	int reg_data_callback(int id, void *_func, void *_data);  //ע��̽���״�ص�����
	int reg_state_callback(int id, void *_func, void *_data);  //ע��̽���״�ص�����

private:
	void clear();  //�ر��̣߳�ͨ�š����ݡ�״̬�������ȴ��߳��˳����ͷŻ��λ��������رմ򿪵��׽ӿ�
	int connect_lidar(char *ip, int port);  //����SOCKET�����״�����
	int reopen_lidar();  //���������ʱ����Ҫ�رվɵ��׽ӿڡ�Ȼ�����״����½����µ����ӣ������״��������,�����״�ID����ͨ���̣߳�������������
	int req_lidar_data(int id); //�����״��������
	int stop_lidar_data(int id); //ֹͣ�״��������
};

#endif

