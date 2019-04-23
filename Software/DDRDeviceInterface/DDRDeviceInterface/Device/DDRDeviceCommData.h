#ifndef __DDR_DEVICE_COMM_DATA_BASE_H__
#define __DDR_DEVICE_COMM_DATA_BASE_H__

#include <vector>

namespace DDRDevice
{
	enum EnDeviceType
	{
		en_DeviceIMU = 1,
		en_DeviceMotor = 2,

		en_DeviceLidar = 60,

		en_DeviceMAX = 255,
	};

	class LidarData
	{
	public:
		LidarData()
		{
			m_enType = en_DeviceLidar;
			m_Data.resize(0);
		}

		struct Data 
		{
			Data()
			{
				angle = 0;
				distance = 0;
			}
			float angle; // in DEGREE and COUNTER-CLOCKWISE (0 defined as directly forward)
			float distance; // in CENTIMETERS
		};
		std::vector<Data> m_Data;
		EnDeviceType m_enType;
	};

	class IMUData 
	{
	public:
		IMUData()
		{
			m_sAccX = 0;
			m_sAccY = 0;
			m_sAccZ = 0;
			m_sGX = 0;
			m_sGY = 0;
			m_sGZ = 0;
			m_sIMUTempBy100 = 0;
			m_nTimeStamp = 0;
			m_enType = en_DeviceIMU;
		}
	
		short m_sAccX;
		short m_sAccY;
		short m_sAccZ;
		short m_sGX;
		short m_sGY;
		short m_sGZ;
		short m_sIMUTempBy100;
		EnDeviceType m_enType;
		unsigned int m_nTimeStamp;
	};

	class MotorData
	{
	public:

		MotorData()
		{
			m_sLeftMotorSpeed = 0;
			m_sRightMotorSpeed = 0;
			m_nTimeStamp = 0;
			m_enType = en_DeviceMotor;
		}

		short m_sLeftMotorSpeed;
		short m_sRightMotorSpeed;
		EnDeviceType m_enType;
		unsigned int m_nTimeStamp;
	};

}

#endif //  __DDR_DEVICE_COMM_DATA_BASE_H__

