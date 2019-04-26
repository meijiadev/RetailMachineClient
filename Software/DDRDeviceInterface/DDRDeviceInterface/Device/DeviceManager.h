#ifndef __DDR_DEVICE_MANAGER_H_INCLUDED__
#define __DDR_DEVICE_MANAGER_H_INCLUDED__

#include "Embedded/EmbServer.h"
#include "Embedded/EmbUser.h"
#include "Device/DDRDeviceCommData.h"
#include "LidarSDK/AkuLidar.h"
#include <map>
#include <mutex>

namespace DDRDevice
{
	/*
		设备管理的类（单例）
		主要管理所有的设备。获取具体的设备数据。
		每个DeviceTypeBase获取数据的来源都是从这里。
	*/
	class DDRDevicedManager
	{
		DDRDevicedManager();

	public:
		static DDRDevicedManager * GetInstance();
		~DDRDevicedManager();
	
		// Lidar
		bool AddLidar(char* ip, std::string strName);
		bool GetLidarData(std::string strName, std::vector<DDRGeometry::APoint> &result);
		bool RemoveLidar(std::string strName);

		// Embedded
		bool GetIMUData(IMUData &data);
		bool GetMotorData(MotorData &data);
		bool GetGnssData(GnssData &data);
		bool GetEnvironmentData(EnvironmentData &data);
		bool GetEmbStatusData(EmbStatusData &data);
		bool GetChargingRelatedData(ChargingRelatedData &data);

		bool SetIMUTargetTemp(float fTemp);
		bool SwitchRechargingStatus(bool bData);// true - enter recharging. false - quit recharging
		// Stereo

		static void  ParseEmbSubThread(void *param);
		void ParseEmbData();
		void ParseGNSSData();
	protected:
		DDRDrivers::DDREmbeddedServer m_EmbServer;
		DDRDrivers::EmbUser m_EmbUser;
		std::map<std::string, std::shared_ptr<DDRDrivers::Lidar_AkuSense>> m_mapLidar;
		bool m_bQuit;
		std::mutex m_mtxDealEmbData;

		// IMU
		short m_sAccX;
		short m_sAccY;
		short m_sAccZ;
		short m_sGX;
		short m_sGY;
		short m_sGZ;
		short m_sIMUTempBy100;
		bool m_bIMUNewData;

		// motor
		short m_sLeftMotorSpeed;
		short m_sRightMotorSpeed;
		bool m_bMotorNewData;

		// environment
		short m_sEBREnvTempBy100;
		unsigned short m_sEBREnvHumBy100;
		unsigned short m_sEBREnvPM25;
		bool m_bEnvironmentNewData;

		// emb status
		bool m_bStickConStatus;
		bool m_bEmergencyStopStatus;
		bool m_bIRStopStatus;
		bool m_bUSStopStatus;
		unsigned short m_sBattStat;
		unsigned short m_sBattVoltageInMV;
		unsigned short m_sBattPercentageBy100;
		short m_sBattTemp_2Bytes;
		bool m_bEmbNewData;

		// Gnss
		double m_dGnssLng;
		double m_dGnssLat;
		unsigned short m_nGnssCog;
		float m_fGnssAlt;
		unsigned int m_nGnssLng;
		bool m_bGnssNewData;

		// Charging Related
		unsigned char m_chRcgIsRcgMode;
		unsigned char m_chRcgState;
		unsigned char m_chRcgIsRcgCharging;
		unsigned char m_chRcgIsRcgDocked;
		unsigned char m_chRcgRcgRightReceived;
		unsigned char m_chRcgLeftReceived;
		unsigned char m_chRcgMiddleReceived;
		bool m_bChargingRelatedNewData;
	};
}

#endif // __DDR_DEVICE_MANAGER_H_INCLUDED__
