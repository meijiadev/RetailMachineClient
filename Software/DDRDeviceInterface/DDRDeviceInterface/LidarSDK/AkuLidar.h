#ifndef __LIDAR_AKUSENSE_H_INCLUDED__
#define __LIDAR_AKUSENSE_H_INCLUDED__

#include "Geometry/RangerPoints.h"
#include "LidarSDK/CheveningSrc/Lidar.h"
#include <vector>
#include <mutex>


namespace DDRDrivers
{
	class Lidar_AkuSenseEx
	{
	public:
		Lidar_AkuSenseEx();
		~Lidar_AkuSenseEx() { End(); }
		void End();
		bool AddOneLidar(char *ip, int &OutID);
		bool GetOneScan(int nCID, std::vector<DDRGeometry::APoint> &result, bool bRaw = true);

	private:

		int open_lidar(char *ip);
		int close_lidar();

		CLidar m_lidar;  // �����״����
		std::mutex m_Lidar_MUTEX;
		bool m_bNewData;
		bool m_bIsOpen;
		std::vector<DDRGeometry::APoint> m_lidarData;

	public:
		//void FindLidar(int flag, ULDINI_Type* ldini); //�ɻص��������ã�������⵽���״�
		void DealLidarData(int id, LIM_HEAD *lim); //�ɻص��������ã������״�����
		void DealLidarState(int id, LIM_HEAD *lim); //�ɻص��������ã������״�״̬�仯
	};
}

#endif