#ifndef __DDR_DEVICE_MANAGER_H_INCLUDED__
#define __DDR_DEVICE_MANAGER_H_INCLUDED__

#include "Embedded/EmbServer.h"
#include "Embedded/EmbUser.h"
#include "Device/DDRDeviceCommData.h"
#include "LidarSDK/AkuLidar.h"

namespace DDRDevice
{
	class DDRDevicedManager
	{
		DDRDevicedManager();
	public:
		static DDRDevicedManager * GetInstance();
		~DDRDevicedManager();
		
		bool AddOneLidar(char *ip, int &OutID);
		bool GetOneScan(int nCID, std::vector<DDRGeometry::APoint> &result);
		bool GetIMUData(IMUData &data);
		bool GetMotorData(MotorData &data);


	protected:
		DDRDrivers::DDREmbeddedServer m_EmbServer;
		DDRDrivers::EmbUser m_EmbUser;
		DDRDrivers::Lidar_AkuSense m_Lidar;
	private:
		
	};
}

#endif // __DDR_DEVICE_MANAGER_H_INCLUDED__