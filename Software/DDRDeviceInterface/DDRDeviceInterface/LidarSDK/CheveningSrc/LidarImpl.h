#pragma once

// 这个类真正实现CLidar接口的代码，让CLidar成为一个接口类，不要暴露实现细节
#ifndef __LIDAR_IMPL_H__
#define __LIDAR_IMPL_H__

#include "lim.h"
#include "LidarProbe.h"
#include "LidarComm.h"

class CLidarImpl
{
public:
	CLidarImpl();
	~CLidarImpl();

	int open_lidar_probe();
	int close_lidar_probe();
	int reg_probe_callback(void *_func, void *_data);  

	int get_id();
	int send_lim(int id, void *lim, int len);

	int start_lidar_config(int id);
	int stop_lidar_config(int id);
	int get_lidar_config(int id, ULDINI_Type* _ldcfg, bool bWaitFlag); 
	int query_fmsig(int id, int _port, bool bWaitFlag);
	int read_iostatus(int id, bool bWaitFlag);
	int set_io_out(int id, int _port, int _status, bool bWaitFlag);
	int release_io_out(int id, bool bWaitFlag);

	int open_lidar(char *ip, int port, int flag);
	int close_lidar(int id);
	int reg_data_callback(int id, void *_func, void *_data);
	int reg_state_callback(int id, void *_func, void *_data);

private:
	CLidarProbe m_lidarProbe;
	CLidarComm m_lidarComm;
};

#endif



