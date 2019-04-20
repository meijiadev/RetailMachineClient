
#include "LidarSDK_errno.h"
#include "LidarImpl.h"

#pragma comment(lib,"wsock32.lib")

CLidarImpl::CLidarImpl()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 3), &wsa);
}

CLidarImpl::~CLidarImpl()
{
	m_lidarProbe.stop_probe_lidar();
	int id = m_lidarComm.get_id();
	m_lidarComm.close_lidar(id);
	WSACleanup();
}

/**********************************************************/
int CLidarImpl::open_lidar_probe()
{
	return m_lidarProbe.start_probe_lidar();
}
int CLidarImpl::close_lidar_probe()
{
	return m_lidarProbe.stop_probe_lidar();
}
int CLidarImpl::reg_probe_callback(void *_func, void *_data)  //注册探测雷达回调函数
{
	return m_lidarProbe.reg_probe_callback(_func, _data);
}

/**********************************************************/
int CLidarImpl::get_id()
{
	return m_lidarComm.get_id();
}
int CLidarImpl::open_lidar(char *ip, int port,int flag)
{
	return m_lidarComm.open_lidar(ip, port,flag);
}
int CLidarImpl::close_lidar(int id)
{
	return m_lidarComm.close_lidar(id);
}
int CLidarImpl::start_lidar_config(int id)
{
	return m_lidarComm.start_lidar_config(id);
}
int CLidarImpl::stop_lidar_config(int id)
{
	return m_lidarComm.stop_lidar_config(id);
}
int CLidarImpl::get_lidar_config(int id, ULDINI_Type* _ldcfg, bool bWaitFlag)  
{
	return m_lidarComm.get_lidar_config(id,_ldcfg,bWaitFlag);
}
int CLidarImpl::query_fmsig(int id, int _port, bool bWaitFlag)
{
	return m_lidarComm.query_fmsig(id,_port,bWaitFlag);
}
int CLidarImpl::read_iostatus(int id, bool bWaitFlag)
{
	return m_lidarComm.read_iostatus(id,bWaitFlag);
}
int CLidarImpl::set_io_out(int id, int _port, int _status, bool bWaitFlag)
{
	return m_lidarComm.set_io_out(id, _port,_status,bWaitFlag);
}
int CLidarImpl::release_io_out(int id, bool bWaitFlag)
{
	return m_lidarComm.release_io_out(id,bWaitFlag);
}

int CLidarImpl::send_lim(int id, void *lim, int len)
{
	return m_lidarComm.send_lim(id,(char*)lim, len);
}
int CLidarImpl::reg_data_callback(int id, void *_func, void *_data)
{
	return m_lidarComm.reg_data_callback(id,_func, _data);
}
int CLidarImpl::reg_state_callback(int id, void *_func, void *_data)
{
	return m_lidarComm.reg_state_callback(id,_func, _data);
}
