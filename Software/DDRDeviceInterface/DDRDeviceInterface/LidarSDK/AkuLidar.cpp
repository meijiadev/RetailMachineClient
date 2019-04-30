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
	cv::Mat black_img = cv::Mat::zeros(cv::Size(1600, 1024), CV_8UC3);//��ɫͼ��
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
		bool bStatus = m_Lidar.GetInitStatus();

		if (!m_Lidar.AddLidar(ip, LASER_SERVER_PORT, bStatus))
		{
			std::cout << "Connection fail." << std::endl;
			return false;
		}
		m_bIsOpen = true;
		m_bQuitSubThread = false;

		// �������߳�ȥѭ����ȡ����
		m_subThread = std::thread(&Lidar_AkuSense::GetDataSubThread, this);
		return true;

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
#endif
	}

	void Lidar_AkuSense::End()
	{
		if (!m_bIsOpen)
		{
			return;
		}
		std::cout << "Lidar_AkuSense::End() close lidar +++ \n";
#if LIDAR_DEVICE_CHOICE == AKUSENSE_LIDAR
		close_lidar();
#elif LIDAR_DEVICE_CHOICE == SICK_LIDAR
		
		m_Lidar_MUTEX.lock();
		m_bQuitSubThread = true;
		m_Lidar_MUTEX.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		m_subThread.join(); // ���˳����߳�

		m_Lidar.Disconnect(); // �Ͽ�����

#endif
		m_bIsOpen = false;

		std::cout << "Lidar_AkuSense::End() close lidar --- \n";
	}

#if LIDAR_DEVICE_CHOICE == AKUSENSE_LIDAR
	// �״�������ݻص�����
	void LidarDataCallback(int id, LIM_HEAD *lim, void *data)
	{
		Lidar_AkuSense *pThis = (Lidar_AkuSense*)data;
		pThis->DealLidarData(id, lim);
	}

	// �״�״̬�仯�ص�����
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
		// ע���״�ص�����
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

		// ����������״�ȹر�
		//close_lidar();

		// �����״�
		id = m_lidar.open_lidar(ip, LIM_DT_PORT, 0);
		if (0 > id)
		{
			m_lidar.close_lidar(id);
			std::cout << "Lidar_AkuSense::open_lidar() error. id = " << id << std::endl;
			return -1;
		}

		m_bIsOpen = true;
		// ע���״�ص�����
		m_lidar.reg_data_callback(id, (void*)LidarDataCallback, (void*)this);
		m_lidar.reg_state_callback(id, (void*)LidarStateCallback, (void*)this);

		id = m_lidar.get_id();
		//m_lidar.read_iostatus(id, false);  // Ŀ�����ûص�����ˢ�½�����IO�ڵ�״̬��ʾ

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
			{ // ע��������
			}
			else
			{ // ע����������ʧ
			}

			if (0x04 & lim->Data[1])
			{ // Ԥ��������
			}
			else
			{
			}

			if (0x01 & lim->Data[1])
			{ // ����������
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

	void  Lidar_AkuSense::GetDataSubThread(void *param)
	{
		Lidar_AkuSense *pManager = (Lidar_AkuSense *)param;
		if (pManager)
		{
			pManager->GetLidarData();
		}
	}

	void Lidar_AkuSense::GetLidarData()
	{
		try
		{
			while (1)
			{
				bool bQuit = false;
				if (m_Lidar_MUTEX.try_lock())
				{
					bQuit = m_bQuitSubThread;

					m_Lidar_MUTEX.unlock();
				}

				if (bQuit)
				{
					std::cout << "Quit lidar sub thread\n";
					break;
				}

				if (!m_Lidar.IsConnected())
				{
					std::cout << "Quit lidar sub thread. lidar is not connect\n";
					break;
				}

				std::vector<DDRGeometry::Points_2d> ret_Point_2d;
				uint64_t times;

				//std::cout << "m_Lidar.GetOneScan() +++\n";
				// ������Ȼ������ѭ��
				if (!m_Lidar.GetOneScan(ret_Point_2d, times))
				{
					//std::cout << "m_Lidar.GetOneScan() 11 ---\n";
					continue;
				}
				//std::cout << "m_Lidar.GetOneScan() 22 ---\n";

				std::lock_guard<std::mutex> lock(m_Lidar_MUTEX);
				m_lidarData.resize(0);
				for (int i = 0; i < ret_Point_2d.size(); i++)
				{
					DDRGeometry::APoint point;
					float x2 = (ret_Point_2d[i].x / 10) * (ret_Point_2d[i].x / 10);
					float y2 = (ret_Point_2d[i].y / 10) * (ret_Point_2d[i].y / 10);
					point.distance = sqrt(x2 + y2);
					point.angle = atan2(ret_Point_2d[i].y, ret_Point_2d[i].x) * (180 / 3.14159265358f);
					m_lidarData.push_back(point);
				}
				m_bNewData = true;

			}
			std::cout << "Sick lidar sub thread end ... m_bQuitSubThread:" << m_bQuitSubThread << std::endl;
		}
		catch (const std::exception& e)
		{
			std::cout << "Sick lidar sub thread occer exception:" << e.what() << std::endl;
		}
	}
#endif
}
