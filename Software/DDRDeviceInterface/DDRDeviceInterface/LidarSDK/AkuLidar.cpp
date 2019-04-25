#include "AkuLidar.h"
#include "LidarSDK/CheveningSrc/lim.h"
#include "LidarSDK/CheveningSrc/LidarSDK_errno.h"
#include <iostream>

namespace DDRDrivers {

	Lidar_AkuSenseEx::Lidar_AkuSenseEx()
	{
		m_bNewData = false;
		m_bIsOpen = false;
	}

	void Lidar_AkuSenseEx::End()
	{
		close_lidar();
		m_bIsOpen = false;
	}

	// 雷达测量数据回调函数
	void LidarDataCallback(int id, LIM_HEAD *lim, void *data)
	{
		Lidar_AkuSenseEx *pThis = (Lidar_AkuSenseEx*)data;
		pThis->DealLidarData(id, lim);
	}

	// 雷达状态变化回调函数
	void LidarStateCallback(int id, LIM_HEAD *lim, void *data)
	{
		Lidar_AkuSenseEx *pThis = (Lidar_AkuSenseEx*)data;
		pThis->DealLidarState(id, lim);
	}

	int Lidar_AkuSenseEx::close_lidar()
	{
		int id = m_lidar.get_id();
		if (0 >= id)
			return -1;
		m_lidar.close_lidar(id);
		// 注册雷达回调函数
		m_lidar.reg_data_callback(id, (void*)NULL, (void*)this);
		m_lidar.reg_state_callback(id, (void*)NULL, (void*)this);

		return 0;
	}

	/*
		return:
			>0 - open lidar success. return lidar ID
			-1 - failed
	*/
	int Lidar_AkuSenseEx::open_lidar(char *ip)
	{
		if (m_bIsOpen)
		{
			std::cout << "Lidar_AkuSenseEx::open_lidar() error. lidar is open\n";
			return -1;
		}

		int id = 0;

		// 如果已连接雷达，先关闭
		//close_lidar();

		// 连接雷达
		id = m_lidar.open_lidar(ip, LIM_DT_PORT, 0);
		if (0 > id)
		{
			m_lidar.close_lidar(id);
			std::cout << "Lidar_AkuSenseEx::open_lidar() error. id = " << id << std::endl;
			return -1;
		}

		m_bIsOpen = true;
		// 注册雷达回调函数
		m_lidar.reg_data_callback(id, (void*)LidarDataCallback, (void*)this);
		m_lidar.reg_state_callback(id, (void*)LidarStateCallback, (void*)this);

		id = m_lidar.get_id();
		//m_lidar.read_iostatus(id, false);  // 目的是让回调函数刷新界面上IO口的状态显示

		return id;
	}

	bool Lidar_AkuSenseEx::AddOneLidar(char *ip, int &OutID)
	{
		OutID = open_lidar(ip);
		return OutID > 0;
	}

	bool Lidar_AkuSenseEx::GetOneScan(int nCID, std::vector<DDRGeometry::APoint> &result, bool bRaw)
	{
		if (!m_Lidar_MUTEX.try_lock()) { return false; }
		if (!m_bNewData)
		{
			m_Lidar_MUTEX.unlock();
			return false;
		}
		result.resize(m_lidarData.size());
		memcpy((void*)(&result[0]), (const void*)(&m_lidarData[0]), sizeof(DDRGeometry::APoint) * m_lidarData.size());
		m_bNewData = false;
		m_Lidar_MUTEX.unlock();
		return true;
	}

	void Lidar_AkuSenseEx::DealLidarData(int id, LIM_HEAD *_lim)
	{
		std::lock_guard<std::mutex> lock(m_Lidar_MUTEX);

		LMD_INFO* lmd_info = LMD_Info(_lim);
		LMD_D_Type* lmd = LMD_D(_lim);
		std::vector<DDRGeometry::APoint> &_pts = m_lidarData;
		_pts.resize(lmd_info->nMDataNum);

		for (int i = 0; i < (int)lmd_info->nMDataNum; i++)
		{
			_pts[i].angle = (lmd_info->nBAngle + i * (int)lmd_info->nAnglePrecision) * 0.001f - 90.0f;
			_pts[i].distance = lmd[i];
		}
		m_bNewData = true;

		return;
	}

	void Lidar_AkuSenseEx::DealLidarState(int id, LIM_HEAD *lim)
	{

		if (LIM_CODE_FMSIG == lim->nCode)
		{
			int nAlarmNO = lim->Data[0];
			if (0x10 & lim->Data[1])
			{ // 注意区报警
			}
			else
			{ // 注意区报警消失
			}

			if (0x04 & lim->Data[1])
			{ // 预警区报警
			}
			else
			{
			}

			if (0x01 & lim->Data[1])
			{ // 报警区报警
			}
			else
			{
			}
		}
		else if (LIM_CODE_IOSTATUS == lim->nCode)
		{
			//m_pIO->draw_io_status(lim->Data[0]);
		}
	}
}
