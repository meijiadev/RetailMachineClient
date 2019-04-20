// Lidar.cpp : 定义 DLL 应用程序的导出函数。
//


#include "Lidar.h"
#include "LidarImpl.h"

CLidar::CLidar()
{
	m_pLidarImpl = new CLidarImpl;
}
CLidar::~CLidar()
{
	if (NULL != m_pLidarImpl)
		delete m_pLidarImpl;
	m_pLidarImpl = NULL;
}

void CLidar::get_sdk_version(char *strVer)
{
	strcpy_s(strVer, 16, LIDAR_SDK_VERSION);
}
/***************************************************************/

int CLidar::open_lidar_probe()
{
	return m_pLidarImpl->open_lidar_probe();
}
int CLidar::close_lidar_probe()
{
	return m_pLidarImpl->close_lidar_probe();
}

int CLidar::reg_probe_callback(void *_func, void *_data)  //注册探测雷达回调函数
{
	return m_pLidarImpl->reg_probe_callback(_func,_data);
}
/***************************************************************/
int CLidar::get_id()
{
	return m_pLidarImpl->get_id();
}
int CLidar::open_lidar(char *ip, int port, int flag)
{
	return m_pLidarImpl->open_lidar(ip, port, flag);
}
int CLidar::close_lidar(int id)
{
	return m_pLidarImpl->close_lidar(id);
}
inline int CLidar::send_lim(int id, void *lim, int len)
{
	return m_pLidarImpl->send_lim(id,lim, len);
}

int CLidar::query_region(int id, int _port, bool bWaitFlag)
{
	return m_pLidarImpl->query_fmsig(id,_port,bWaitFlag);
}
int CLidar::start_lidar_config(int id)
{
	return m_pLidarImpl->start_lidar_config(id);
}
int CLidar::stop_lidar_config(int id)
{
	return m_pLidarImpl->stop_lidar_config(id);
}

int CLidar::get_lidar_config(int id, ULDINI_Type* _ldcfg, bool bWaitFlag) 
{
	return m_pLidarImpl->get_lidar_config(id,_ldcfg,bWaitFlag);
}
int CLidar::read_iostatus(int id, bool bWaitFlag)
{
	return m_pLidarImpl->read_iostatus(id,bWaitFlag);
}
int CLidar::set_io_out(int id, int _port, int _status, bool bWaitFlag)
{
	return m_pLidarImpl->set_io_out(id,_port,_status,bWaitFlag);
}
int CLidar::release_io_out(int id, bool bWaitFlag)
{
	return m_pLidarImpl->release_io_out(id,bWaitFlag);
}

int CLidar::reg_data_callback(int id, void *_func, void *_data)
{
	return m_pLidarImpl->reg_data_callback(id,_func, _data);
}
int CLidar::reg_state_callback(int id, void *_func, void *_data)
{
	return m_pLidarImpl->reg_state_callback(id,_func, _data);
}
