#pragma once

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>

#include "RingBuf.h"
#include "LidarList.h"

#define CLOSE_OPEN_INTERVAL_TIME	500  //ms

typedef void(*LIDAR_PROBE_CALLBACK)(int flag, ULDINI_Type* ldini, void *_data);  //�״�̽��ص���������

class CLidarProbe
{
public:
	CLidarProbe();
	~CLidarProbe();

	/*************** ̽���״���غ��� ***************************/
	int start_probe_lidar();  //����̽��lidar
	int stop_probe_lidar();  //ֹͣ̽���״�
	int reg_probe_callback(void *_func,void *_data);  //ע��̽���״�ص�����

private:
	// ���������������ಥ�̣߳��������״�����߳��ղ����״�Ķಥ����
	// ����ץ������ץ���������������յ��״�Ķಥ��Ϣ�ġ�ͨ��ʵ�飬�����ж����״�Ķಥ���������⡣
	// ����İ취�ǣ��ڼ����ಥ�߳��У������ʱû���յ��״�Ķಥ��Ϣ�������³�ʼ������
	//ע�⣺ �ڹر�SOCKET�󣬲�Ҫ���ϴ򿪣�Ҫ�ӳ�һ�ᣨƩ��500ms����
	int open_socket();
	int close_socket();

	/************** ���״�̽����ر��������� ********************/
	CRingBuf *m_pRingBuf;  //���λ���������
	CLidarList *m_pLidarList;  // ̽�⵽���״��б�

	HANDLE m_hLidarProbeThread;
	bool	m_bLidarProbe;		//=true���״�̽���߳������У�=false���˳��״�̽���߳�
	int	LidarProbeThread(void);  //�״�̽���߳�
//	static void __cdecl LidarProbeThreadEntry(void* lpParameter);  //�״�̽���߳����
	static unsigned __stdcall LidarProbeThreadEntry(void* lpParameter);

	SOCKET m_sock;  //���ڽ��նಥ�״���Ϣ���׽ӿ�
	struct sockaddr_in m_LocalAddr;
	struct ip_mreq m_mreq;/*����ಥ��*/

	LIDAR_PROBE_CALLBACK m_callbackProbeFunc;  //����̽���״�ص�����ָ��
	void *m_callbackProbeFuncParam;  //��Ϊ�����ش����ص�����

};

