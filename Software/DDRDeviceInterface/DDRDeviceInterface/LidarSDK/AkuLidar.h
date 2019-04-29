#ifndef __LIDAR_AKUSENSE_H_INCLUDED__
#define __LIDAR_AKUSENSE_H_INCLUDED__

#define AKUSENSE_LIDAR 1
#define SICK_LIDAR 2
#define LIDAR_DEVICE_CHOICE SICK_LIDAR

#if LIDAR_DEVICE_CHOICE == AKUSENSE_LIDAR
#include "LidarSDK/CheveningSrc/Lidar.h"
#elif LIDAR_DEVICE_CHOICE == SICK_LIDAR
#include "SickSDK/SickLidar.h"
#endif

#include "Geometry/RangerPoints.h"
#include <vector>
#include <mutex>

namespace DDRDrivers
{
	class Lidar_AkuSense
	{
	public:
		Lidar_AkuSense();
		~Lidar_AkuSense() { End(); }
		void End();
		bool AddOneLidar(char *ip, int &OutID);
		bool GetOneScan(int nCID, std::vector<DDRGeometry::APoint> &result, bool bRaw = true);

	private:

		std::mutex m_Lidar_MUTEX;
		bool m_bNewData;
		bool m_bIsOpen;
		std::vector<DDRGeometry::APoint> m_lidarData;

#if LIDAR_DEVICE_CHOICE == AKUSENSE_LIDAR
		int open_lidar(char *ip);
		int close_lidar();
		CLidar m_lidar;  // �����״����
	public:
		//void FindLidar(int flag, ULDINI_Type* ldini); //�ɻص��������ã�������⵽���״�
		void DealLidarData(int id, LIM_HEAD *lim); //�ɻص��������ã������״�����
		void DealLidarState(int id, LIM_HEAD *lim); //�ɻص��������ã������״�״̬�仯
#elif  LIDAR_DEVICE_CHOICE == SICK_LIDAR
		Lidar_SickLMS m_Lidar;
		static void  GetDataSubThread(void *param);
		void GetLidarData();
		std::thread m_subThread;
		bool m_bQuitSubThread;
#endif
	};
}

#endif