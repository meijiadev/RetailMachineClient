
#ifndef __LIDAR_H__
#define __LIDAR_H__

#include "lim.h"

#define LIDAR_SDK_VERSION		"Ver1.21"	

class CLidarImpl;


/* ��class �������м����һ��������_declspec(dllexport)����ʾ�����༰�������������Ա������ */
#ifdef USE_LIDAR_SDK_LIBRARY
typedef void (*LIDAR_PROBE_CALLBACK)(int flag, ULDINI_Type* ldini,void *_data);  //�״�̽��ص���������
typedef void(*LIDAR_DATA_CALLBACK)(int _id,void* _lim, void *_data);	//�״����ݻص���������
typedef void (*LIDAR_STATE_CALLBACK)(int _id, void* _lim, void *_data);	//�״�״̬�ص���������

class  _declspec(dllimport) CLidar
#else
class  _declspec(dllexport) CLidar
#endif
{
public:
	CLidar();
	~CLidar();

	void get_sdk_version(char *strVer);

	/************* �״�̽��ӿں��� ********************/
	int open_lidar_probe();
	int close_lidar_probe();
	//ע��̽���״�ص�����
	// �ص��������壺 typedef void (*PROBE_LIDAR_CALLBACK)(int flag, ULDINI_Type* ldini,void *_data);
	// _data���û�ע��ص�����ʱ�ṩ��ָ�룬SDK���ûص�����ʱ���������������ݻظ��ص�����
	int reg_probe_callback(void *_func,void *_data);  

	/************* �״�����&״̬�ӿں��� ********************/
	int get_id();  // ��ȡ�״�ID
	//	���״﷢��LIM����
	//	lim��LIM ����ָ��
	//	len��LIM ���ĳ��ȣ���LIM_HEAD.nLIMLen
	int send_lim(int id, void *lim, int len);

	int start_lidar_config(int id);
	int stop_lidar_config(int id);

	//��ȡ�״�����
	//	id : SDK�����ID��
	//	bWaitFlag: �Ƿ�ȴ������=1���ȴ�����ٷ��أ�=0��ֱ�ӷ��أ�Ȼ���ٻص������д����ѯ�����
	// �ɹ�����LIDARSDK_ERR_SUCCESS,_ldcfg����ULDINI_Type��Ϣ;ʧ�ܷ���LIDARSDK_ERR_FAIL
	int get_lidar_config(int id, ULDINI_Type* _ldcfg, bool bWaitFlag);  

	//��ѯ���������鱨��״̬
	//	id : SDK�����ID��
	//	_port : Ҫ��ѯ����������(0-15)
	//	bWaitFlag: �Ƿ�ȴ������=1���ȴ�����ٷ��أ�=0��ֱ�ӷ��أ�Ȼ���ٻص������д����ѯ�����
	// �ɹ�����������ѯ�����򱨾�״̬
	//		Bit0:�澯�ź�״̬
	//		Bit1:�澯�ź���״̬
	//		Bit2 : Ԥ���ź�״̬
	//		Bit3 : Ԥ���ź���״̬
	//		Bit4 : ע���ź�״̬
	//		Bit5 : ע���ź���״̬
	//      Bit7 - Bit15 : ��������
	//		����λ: 0
	//	ʧ�ܣ�����LIDARSDK_ERR_FAIL
	int query_region(int id, int _port,bool bWaitFlag);

	// ��IO״̬
	//		id : SDK�����ID��
	//		bWaitFlag: �Ƿ�ȴ������=1���ȴ�����ٷ��أ�=0��ֱ�ӷ���LIDARSDK_ERR_SUCESS��Ȼ���ٻص������д���IO�Ĳ�ѯ�����
	// �ɹ�����IO״̬
	//		bit0��OUT1
	//		bit1��OUT2
	//		bit2��OUT3
	//		bit3��OUT4
	//		bit4��IN1
	//		bit5��IN2
	//		bit6��IN3
	//		bit7��IN4
	// ʧ�ܷ���LIDARSDK_ERR_FAIL
	int read_iostatus(int id, bool bWaitFlag);

	// ����IO���״̬:��ʱ�������⹦����ͣͨ��I/O ����˿��������ź�
	//		id : SDK�����ID��
	//		_port : ��ӦbitΪ1����ʾ���ö�Ӧ��IO�����
	//			bit0��OUT1
	//			bit1��OUT2
	//			bit2��OUT3
	//			bit3��OUT4
	//		_status : 0/1
	//		bWaitFlag: �Ƿ�ȴ������=1���ȴ�����ٷ��أ�=0��ֱ�ӷ���LIDARSDK_ERR_SUCESS��Ȼ���ٻص�������ȡ���õĽ����
	// �ɹ��������õ�״̬,��Ҫ�û���֤�Ƿ������õ�һ��
	// ʧ�ܷ���LIDARSDK_ERR_FAIL
	int set_io_out(int id, int _port, int _status, bool bWaitFlag);

	// �ͷ�IO������ã�����set_io_out()��Ҫ����release_io_out()�ͷ�IO����˿�(�����⹦�ָܻ�ͨ��I/O ����˿��������ź�)
	//		id : SDK�����ID��
	//		bWaitFlag: �Ƿ�ȴ������=1���ȴ�����ٷ��أ�=0��ֱ�ӷ���LIDARSDK_ERR_SUCESS��Ȼ���ٻص�������ȡ���õĽ����
	// �ɹ�����IO��״̬,ʧ�ܷ���LIDARSDK_ERR_FAIL
	int release_io_out(int id, bool bWaitFlag);

	int open_lidar(char *ip, int port, int flag);  //���״�ɹ��������״�ID������0����ʧ�ܷ��ش�����루С��0��
	int close_lidar(int id); //�ر��״�

	int reg_data_callback(int id, void *_func, void *_data); //ע���״����ݻص�����
	int reg_state_callback(int id, void *_func, void *_data); //ע���״�״̬�ص�����

private:
	CLidarImpl* m_pLidarImpl;

};

#endif


