#ifndef __DDR_DEVICE_MANAGER_H_INCLUDED__
#define __DDR_DEVICE_MANAGER_H_INCLUDED__

#include "Embedded/EmbServer.h"
#include "Embedded/EmbUser.h"
#include "Device/DDRDeviceCommData.h"
#include "LidarSDK/AkuLidar.h"
#include <map>

namespace DDRDevice
{
	/*
		�豸������ࣨ������
		��Ҫ�������е��豸����ȡ������豸���ݡ�
		ÿ��DeviceTypeBase��ȡ���ݵ���Դ���Ǵ����
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

		// Emb
		bool GetIMUData(IMUData &data);
		bool GetMotorData(MotorData &data);

		// Stereo

		static void  ParseEmbSubThread(void *param);
		void ParseEmbData();
		void ParseGNSSData();
	protected:
		DDRDrivers::DDREmbeddedServer m_EmbServer;
		DDRDrivers::EmbUser m_EmbUser;
		std::map<std::string, std::shared_ptr<DDRDrivers::Lidar_AkuSense>> m_mapLidar;
		bool m_bQuit;
	};
}

#endif // __DDR_DEVICE_MANAGER_H_INCLUDED__