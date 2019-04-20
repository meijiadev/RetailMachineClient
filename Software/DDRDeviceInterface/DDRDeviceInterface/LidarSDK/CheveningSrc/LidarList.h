#pragma once

#include "lim.h"

#include <time.h>
#include "windows.h"

#define MAX_LIDAR_PROBE_LIST_NUM		64   //������̽����״���Ŀ
#define LIDAR_PROBE_LIST_TIMEOUT		5	//�룬̽�⵽���״���ϢʧЧʱ��

typedef struct _LIDAR_NODE
{
	long ip;  // ��IP��ַ��Ϊ����������Ƚϡ�Ʃ��IPΪ192.168.1.232,���Ϊ����Ϊ��192168001232
	time_t tm;  //�յ��״�ಥ��Ϣ��ʱ�䣬�����ж���Ϣ�Ƿ�̫��
	ULDINI_Type ldini;
	struct _LIDAR_NODE *prev;
	struct _LIDAR_NODE *next;
} LIDAR_NODE;

class CLidarList
{
public:
	CLidarList();
	~CLidarList();

	long get_ip(char *ip);  //�ַ���IPתΪ����IP

	LIDAR_NODE* req_node(); //�ӿ��нڵ��б�����һ�����нڵ�
	void free_node(LIDAR_NODE* pNode); //�ͷ�һ���ڵ㵽��������

	LIDAR_NODE* find_node(long ip);  //����IP�����������
	void refresh_node_time(LIDAR_NODE* pNode);  //ˢ���״�ڵ��ʱ��

	void insert_node(LIDAR_NODE *pNode); //�ѽڵ��������
	void delete_node(long ip);  //����IPɾ���ڵ㣬���ͷŵ���������

	LIDAR_NODE* find_timeout_node();  //���ҳ�ʱ�ڵ㣬���ѽڵ��������ժ��

private:
	HANDLE m_mutex;  //���ڶ�����Ļ������
	LIDAR_NODE *m_head;
	LIDAR_NODE *m_free;
	void free_list(LIDAR_NODE *list);  //�ͷ������ڴ�

};

