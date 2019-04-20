#pragma once

#include "lim.h"

#include <time.h>
#include "windows.h"

#define MAX_LIDAR_PROBE_LIST_NUM		64   //最大可以探测的雷达数目
#define LIDAR_PROBE_LIST_TIMEOUT		5	//秒，探测到的雷达信息失效时间

typedef struct _LIDAR_NODE
{
	long ip;  // 把IP地址变为整数，方便比较。譬如IP为192.168.1.232,则变为整数为：192168001232
	time_t tm;  //收到雷达多播消息的时间，用于判断消息是否太老
	ULDINI_Type ldini;
	struct _LIDAR_NODE *prev;
	struct _LIDAR_NODE *next;
} LIDAR_NODE;

class CLidarList
{
public:
	CLidarList();
	~CLidarList();

	long get_ip(char *ip);  //字符串IP转为整数IP

	LIDAR_NODE* req_node(); //从空闲节点列表申请一个空闲节点
	void free_node(LIDAR_NODE* pNode); //释放一个节点到空闲链表

	LIDAR_NODE* find_node(long ip);  //根据IP在链表里查找
	void refresh_node_time(LIDAR_NODE* pNode);  //刷新雷达节点的时间

	void insert_node(LIDAR_NODE *pNode); //把节点插入链表
	void delete_node(long ip);  //根据IP删除节点，并释放到空闲链表

	LIDAR_NODE* find_timeout_node();  //查找超时节点，并把节点从链表中摘除

private:
	HANDLE m_mutex;  //用于对链表的互斥访问
	LIDAR_NODE *m_head;
	LIDAR_NODE *m_free;
	void free_list(LIDAR_NODE *list);  //释放链表内存

};

