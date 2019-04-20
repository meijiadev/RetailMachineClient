
using namespace std;

#include <vector>
#include <string>

#include "LidarList.h"

CLidarList::CLidarList()
{
	LIDAR_NODE *pNode = NULL;
	m_head = NULL;
	m_free = NULL;
	m_mutex = ::CreateMutex(NULL, FALSE, nullptr);
	if (NULL == m_mutex)
	{
	}
	for (int i = 0; i < MAX_LIDAR_PROBE_LIST_NUM; i++)
	{
		pNode = new LIDAR_NODE;
		if (NULL != pNode)
		{
			pNode->next = m_free;
			pNode->prev = NULL;
			if (NULL != m_free)
			{
				m_free->prev = pNode;
			}
			m_free = pNode;
		}
	}
}
CLidarList::~CLidarList()
{
	free_list(m_free);
	m_free = NULL;
	free_list(m_head);
	m_head = NULL;
	if (NULL != m_mutex)
		CloseHandle(m_mutex);
	m_mutex = NULL;
}
void CLidarList::free_list(LIDAR_NODE *list)  //�ͷ������ڴ�
{
	LIDAR_NODE *pNode = NULL;
	::WaitForSingleObject(m_mutex, INFINITE);
	while (NULL != list)
	{
		pNode = list;
		list = pNode->next;
		delete pNode;
	}
	::ReleaseMutex(m_mutex);
}
long CLidarList::get_ip(char *ip)  //�ַ���IPתΪ����IP
{
	string s = ip;
	string::size_type pos1, pos2;
	long nip = 0;
	pos1 = s.find('.')+1;
	pos2 = s.find('.',pos1);
	while (string::npos != pos2)
	{
		nip *= 1000;
		nip += std::stoi(s.substr(pos1, pos2 - pos1));
		pos1 = pos2 + 1;
		pos2 = s.find('.', pos1);
	}
	nip *= 1000;
	int si = s.size();
	nip += std::stoi(s.substr(pos1, s.size()-pos1));
	return nip;
}

LIDAR_NODE* CLidarList::req_node() //�ӿ��нڵ��б�����һ�����нڵ�
{
	LIDAR_NODE *pNode = NULL;
	if (NULL == m_free)
		return NULL;
	::WaitForSingleObject(m_mutex, INFINITE);
	pNode = m_free;
	m_free = m_free->next;
	if (NULL != m_free)
		m_free->prev = NULL;

	pNode->prev = NULL;
	pNode->next = NULL;
	::ReleaseMutex(m_mutex);
	return pNode;
}
void CLidarList::free_node(LIDAR_NODE* pNode) //�ͷ�һ���ڵ㵽�����б�
{
	::WaitForSingleObject(m_mutex, INFINITE);
	pNode->prev = NULL;
	pNode->next = m_free;
	if (NULL != m_free)
		m_free->prev = pNode;
	m_free = pNode;
	::ReleaseMutex(m_mutex);
}
LIDAR_NODE* CLidarList::find_node(long ip)  //����IP�����������
{
	LIDAR_NODE *pNode = m_head;
	if (NULL == pNode)
		return NULL;
	::WaitForSingleObject(m_mutex, INFINITE);
	while (NULL != pNode)
	{
		if (ip == pNode->ip)
		{
			::ReleaseMutex(m_mutex);
			return pNode;
		}
		pNode = pNode->next;
	};
	::ReleaseMutex(m_mutex);
	return pNode;
}
void CLidarList::refresh_node_time(LIDAR_NODE* pNode)  //ˢ���״�ڵ��ʱ��
{
	::WaitForSingleObject(m_mutex, INFINITE);
	pNode->tm = time(NULL);
	::ReleaseMutex(m_mutex);
}
void CLidarList::insert_node(LIDAR_NODE *pNode)
{
	::WaitForSingleObject(m_mutex, INFINITE);
	pNode->next = m_head;
	pNode->prev = NULL;
	if (NULL != m_head)
		m_head->prev = pNode;
	m_head = pNode;
	::ReleaseMutex(m_mutex);
}
void CLidarList::delete_node(long ip)  //����IPɾ���ڵ�
{
	LIDAR_NODE *pNode = find_node(ip);
	if (NULL == pNode)
		return;
	// �Ѵ˽ڵ������ժ��
	::WaitForSingleObject(m_mutex, INFINITE);
	if (m_head == pNode)
	{
		m_head = pNode->next;
		if (NULL != pNode->next)
			pNode->next->prev = NULL;
	}
	else
	{
		pNode->prev->next = pNode->next;
		if (NULL != pNode->next)
			pNode->next->prev = pNode->prev;
	}
	::ReleaseMutex(m_mutex);
	//�ͷŵ���������
	free_node(pNode);
}
LIDAR_NODE* CLidarList::find_timeout_node()  //���ҳ�ʱ�ڵ㣬���ѽڵ��������ժ��
{
	time_t ct = time(NULL);
	LIDAR_NODE *pNode = NULL;
	::WaitForSingleObject(m_mutex, INFINITE);
	pNode = m_head;
	while (NULL != pNode)
	{
		if (LIDAR_PROBE_LIST_TIMEOUT < (ct - pNode->tm))
		{
			// ��pNode������ժ��
			if (NULL != pNode->prev)
				pNode->prev->next = pNode->next;
			if (NULL != pNode->next)
				pNode->next->prev = pNode->prev;
			if (m_head == pNode)
				m_head = pNode->next;

			pNode->prev = NULL;
			pNode->next = NULL;
			break;
		}
		pNode = pNode->next;
	}
	::ReleaseMutex(m_mutex);
	return pNode;
}
