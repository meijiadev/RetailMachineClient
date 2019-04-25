#ifndef __LIDAR_AKUSENSE_H_INCLUDED__
#define __LIDAR_AKUSENSE_H_INCLUDED__

#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <mutex>
#include "CommonDef/CommonDef.h"
#include "Geometry/RangerPoints.h"
#include "LidarSDK/CheveningSrc/Lidar.h"

namespace DDRDrivers
{
	struct OneAkuSenseLidar
	{
		std::string _ip;
		int _nCID;
		bool _bConnected;
		bool _bNewData;
		std::vector<DDRGeometry::APoint> _data;
	};
#if 0
	class Lidar_AkuSense
	{
	public:
		Lidar_AkuSense();
		~Lidar_AkuSense() { End(); }
		void End();

		bool AddOneLidar(int nCID, char *ip);
		bool AddOneLidar(char *ip, int &OutID);
		bool GetOneScan(int nCID, std::vector<DDRGeometry::APoint> &result, bool bRaw);
		bool GetOneScan(char *ip, std::vector<DDRGeometry::APoint> &result, bool bRaw);

		inline Lidar_AkuSense* GetInstancePointer() const {
			return g_pSoleInstance;
		}
		inline void Stop() {
			if (g_pSoleInstance != NULL)
				g_pSoleInstance->_stop();
		}
		static void ProcessRawData(std::vector<DDRGeometry::APoint> &points);

	protected:
		bool m_bOkay;
		std::vector<OneAkuSenseLidar> m_AkuLidars;
		std::unordered_map<std::string, int> m_IP2IND;
		std::unordered_map<int, int> m_CID2IND;

		void _stop();
		bool _getScan(int index, std::vector<DDRGeometry::APoint> &result, bool bRaw);
		bool _addLidar(int nCID, char *ip);
		bool _addLidar(char *ip, int& nCID);

		static bool _IsUnreliablePoint(float theta_2prev, float theta_2next, float dist_prev, float dist_cur, float dist_next);

		static bool g_bStop;
		static std::mutex g_Lidar_MUTEX;
		static int g_nInstances;
		static Lidar_AkuSense *g_pSoleInstance;

	private:
		std::mutex m_mutexDBuf;
		char m_dbuf[4800]; //回调函数把雷达数据拷贝到这个区域
		CLidar m_lidar;  // 激光雷达对象

		std::vector<DDRGeometry::APoint> m_lidarData;

		int open_lidar(char *ip);
		int close_lidar();

	public:
		//void FindLidar(int flag, ULDINI_Type* ldini); //由回调函数调用，处理侦测到的雷达
		void DealLidarData(int id, LIM_HEAD *lim); //由回调函数调用，处理雷达数据
		void DealLidarState(int id, LIM_HEAD *lim); //由回调函数调用，处理雷达状态变化
	};
#endif
	class Lidar_AkuSenseEx
	{
	public:
		Lidar_AkuSenseEx();
		~Lidar_AkuSenseEx() { End(); }
		void End();

		bool AddOneLidar(char *ip, int &OutID);
		bool GetOneScan(int nCID, std::vector<DDRGeometry::APoint> &result, bool bRaw);

	private:
		std::mutex m_mutexDBuf;
		char m_dbuf[4800]; //回调函数把雷达数据拷贝到这个区域
		CLidar m_lidar;  // 激光雷达对象

		std::vector<DDRGeometry::APoint> m_lidarData;

		int open_lidar(char *ip);
		int close_lidar();

	public:
		//void FindLidar(int flag, ULDINI_Type* ldini); //由回调函数调用，处理侦测到的雷达
		void DealLidarData(int id, LIM_HEAD *lim); //由回调函数调用，处理雷达数据
		void DealLidarState(int id, LIM_HEAD *lim); //由回调函数调用，处理雷达状态变化
	};
}

#endif