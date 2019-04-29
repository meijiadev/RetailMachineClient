#include "AkuLidar.h"

#if LIDAR_DEVICE_CHOICE == AKUSENSE_LIDAR
#include "LidarSDK/CheveningSrc/lim.h"
#include "LidarSDK/CheveningSrc/LidarSDK_errno.h"
#endif

#include <iostream>
//#define SHOW_LIDAR_DATA
#ifdef SHOW_LIDAR_DATA
#include "opencv2/opencv.hpp"
void showData(std::vector<DDRGeometry::Points_2d> ret_Point_2d)
{
	cv::Mat black_img = cv::Mat::zeros(cv::Size(1600, 1024), CV_8UC3);//黑色图像
	const int n = ret_Point_2d.size();
	for (int k = 0; k < n; k++)
	{
		cv::Point2f pt;
		//the unit of x and y is mm
		pt.x = -ret_Point_2d[k].y / 10 + 800;
		pt.y = -ret_Point_2d[k].x / 10 + 600;
		if (pt.x < black_img.cols && pt.y < black_img.rows)
		{
			cv::circle(black_img, pt, 3, cv::Scalar(255, 255, 255), 2);
		}
	}

	cv::imshow("lidar", black_img);
	cv::waitKey(10);
}
#endif
namespace DDRDrivers {

	Lidar_AkuSense::Lidar_AkuSense()
	{
		m_bNewData = false;
		m_bIsOpen = false;
	}

	bool Lidar_AkuSense::AddOneLidar(char *ip, int &OutID)
	{
#if LIDAR_DEVICE_CHOICE == AKUSENSE_LIDAR
		OutID = open_lidar(ip);
		return OutID > 0;
#elif LIDAR_DEVICE_CHOICE == SICK_LIDAR
		m_Lidar.Connect(ip, LASER_SERVER_PORT);

		if (!m_Lidar.IsConnected())
		{
			std::cout << "connection failend" << std::endl;
			return false;
		}

		std::cout << "Connected to laser" << std::endl;
		std::cout << "Loging in ..." << std::endl;

		m_Lidar.Login();
		m_Lidar.StopMeas();

		// nesscessary
		if (!m_Lidar.InitLidarParams())
		{
			std::cout << "Set Lidar Params Failed." << ::std::endl;
		}

		std::cout << "Start measurements ..." << std::endl;

		m_Lidar.StartMeas();

		std::cout << "Wait for ready status ..." << std::endl;

		int ret = 0;
		while (ret != 7)
		{
			// wait several seconds
			ret = m_Lidar.QueryStatus();
		}

		std::cout << "Laser ready ret = " << ret << std::endl;

		std::cout << "Start continuous data transmission ..." << std::endl;

		m_Lidar.ScanContinous(1);
		return ret;
#endif
	}

	bool Lidar_AkuSense::GetOneScan(int nCID, std::vector<DDRGeometry::APoint> &result, bool bRaw)
	{
#if LIDAR_DEVICE_CHOICE == AKUSENSE_LIDAR
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
#elif LIDAR_DEVICE_CHOICE == SICK_LIDAR
		std::vector<DDRGeometry::Points_2d> ret_Point_2d;
		uint64_t times;
		if (!m_Lidar.GetOneScan(ret_Point_2d, times))
		{
			return false;
		}
#ifdef SHOW_LIDAR_DATA
		showData(ret_Point_2d);
#endif
		result.resize(0);
		for (int i = 0; i < ret_Point_2d.size(); i++)
		{
			DDRGeometry::APoint point;
			float x2 = (ret_Point_2d[i].x / 10) * (ret_Point_2d[i].x / 10);
			float y2 = (ret_Point_2d[i].y / 10) * (ret_Point_2d[i].y / 10);
			point.distance = sqrt(x2 + y2);
			point.angle = atan2(ret_Point_2d[i].y, ret_Point_2d[i].x) * (180 / 3.14159265358979f);
			result.push_back(point);
		}
		return true;
#endif
	}

	void Lidar_AkuSense::End()
	{
		std::cout << "Lidar_AkuSense::End() close lidar ...\n";
#if LIDAR_DEVICE_CHOICE == AKUSENSE_LIDAR
		close_lidar();
#elif LIDAR_DEVICE_CHOICE == SICK_LIDAR
		m_Lidar.Disconnect();
#endif
		m_bIsOpen = false;
	}

#if LIDAR_DEVICE_CHOICE == AKUSENSE_LIDAR
	// 雷达测量数据回调函数
	void LidarDataCallback(int id, LIM_HEAD *lim, void *data)
	{
		Lidar_AkuSense *pThis = (Lidar_AkuSense*)data;
		pThis->DealLidarData(id, lim);
	}

	// 雷达状态变化回调函数
	void LidarStateCallback(int id, LIM_HEAD *lim, void *data)
	{
		Lidar_AkuSense *pThis = (Lidar_AkuSense*)data;
		pThis->DealLidarState(id, lim);
	}

	int Lidar_AkuSense::close_lidar()
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
	int Lidar_AkuSense::open_lidar(char *ip)
	{
		if (m_bIsOpen)
		{
			std::cout << "Lidar_AkuSense::open_lidar() error. lidar is open\n";
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
			std::cout << "Lidar_AkuSense::open_lidar() error. id = " << id << std::endl;
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

	void Lidar_AkuSense::DealLidarData(int id, LIM_HEAD *_lim)
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

	void Lidar_AkuSense::DealLidarState(int id, LIM_HEAD *lim)
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

#elif LIDAR_DEVICE_CHOICE == SICK_LIDAR

#endif
}
