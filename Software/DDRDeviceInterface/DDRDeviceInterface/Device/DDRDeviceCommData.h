#ifndef __DDR_DEVICE_COMM_DATA_BASE_H__
#define __DDR_DEVICE_COMM_DATA_BASE_H__

#include <vector>
#include "DataFormat.h"

using namespace DDRData;

namespace DDRDevice
{
	enum EnDeviceType
	{
		// 1.Ƕ��ʽ״̬�����缱ͣ ң�� ������ѹ��
		en_DeviceEmbStatus = 1,

		// 2.IMU����
		en_DeviceIMU = 2,

		// 3.�ֻ�����
		en_DeviceMotor = 3,

		// 4.GPS����
		en_DeviceGNSS = 4,

		// 5.����������
		en_DeviceChargingRelated = 5,

		// 6.�������ݣ�������������EѲ����ı����õģ�
		en_DeviceUltrasound = 6,

		// 7.�������ݣ�ͬ�ϣ�
		en_DeviceInfrared = 7,

		// 8.������������ʪ��  �¶� PM2.5��
		en_DeviceEnvironment = 8,

		// 9.˫Ŀ
		en_DeviceStereo = 9,

		// 10.�״�
		en_DeviceLidar = 60,

		en_DeviceMAX = 255,
	};

	/*
		�豸��Ϣ�Ļ��ࡣ
	*/
	class DeviceInfoBase
	{
	public:
		std::string m_strName;
		EnDeviceType m_enType;
	};

	/*
		�״���Ϣ�ࡣ��Ҫ���÷����á�
	*/
	class LidarInfo:public DeviceInfoBase
	{
	public:
		LidarInfo()
		{
			m_enType = en_DeviceLidar;
		}
		std::string m_strIp;
	};

	/*
		�״������ࡣ���ظ����÷������ݽṹ
	*/
	class LidarData
	{
	public:
		LidarData()
		{
			m_enType = en_DeviceLidar;
			m_nTimeStamp = 0;
		}

		LidarScan_2D m_Data;
		EnDeviceType m_enType;
		U64_TIMESTAMP m_nTimeStamp;
	};

	class IMUInfo :public DeviceInfoBase
	{
	public:
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

	class MotorInfo :public DeviceInfoBase
	{
	public:
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

	class EmbStatusInfo :public DeviceInfoBase
	{
	public:
	};

	class EmbStatusData
	{
	public:

		EmbStatusData()
		{
			m_bStickConStatus = 0;
			m_bEmergencyStopStatus = 0;
			m_bIRStopStatus = 0;
			m_bUSStopStatus = 0;

			m_sBattStat = 0;
			m_sBattVoltageInMV = 0;
			m_sBattPercentageBy100 = 0;
			m_sBattTemp_2Bytes = 0;
			m_nTimeStamp = 0;
			m_enType = en_DeviceEmbStatus;
		}

		bool m_bStickConStatus;
		bool m_bEmergencyStopStatus;
		bool m_bIRStopStatus;
		bool m_bUSStopStatus;

		unsigned short m_sBattStat;
		unsigned short m_sBattVoltageInMV;
		unsigned short m_sBattPercentageBy100;
		short m_sBattTemp_2Bytes;

		EnDeviceType m_enType;
		unsigned int m_nTimeStamp;
	};

	class EnvironmentInfo :public DeviceInfoBase
	{
	public:
	};

	class EnvironmentData
	{
	public:
		EnvironmentData()
		{
			m_sEBREnvTempBy100 = 0;
			m_sEBREnvHumBy100 = 0;
			m_sEBREnvPM25 = 0;
			m_nTimeStamp = 0;
			m_enType = en_DeviceEnvironment;
		}

		short m_sEBREnvTempBy100;
		unsigned short m_sEBREnvHumBy100;
		unsigned short m_sEBREnvPM25;
		EnDeviceType m_enType;
		unsigned int m_nTimeStamp;
	};

	class ChargingRelatedInfo :public DeviceInfoBase
	{
	public:
		bool m_bEnterCharging; // true - Enter. false - quit
	};

	class ChargingRelatedData
	{
	public:
		ChargingRelatedData()
		{
			m_chRcgIsRcgMode = 0;
			m_chRcgState = 0;
			m_chRcgIsRcgCharging = 0;
			m_chRcgIsRcgDocked = 0;
			m_chRcgRcgRightReceived = 0;
			m_chRcgLeftReceived = 0;
			m_chRcgMiddleReceived = 0;
			m_nTimeStamp = 0;
			m_enType = en_DeviceChargingRelated;
		}

		unsigned char m_chRcgIsRcgMode;
		unsigned char m_chRcgState;
		unsigned char m_chRcgIsRcgCharging;
		unsigned char m_chRcgIsRcgDocked;
		unsigned char m_chRcgRcgRightReceived;
		unsigned char m_chRcgLeftReceived;
		unsigned char m_chRcgMiddleReceived;
		EnDeviceType m_enType;
		unsigned int m_nTimeStamp;
	};

	class GnssInfo :public DeviceInfoBase
	{
	public:
	};

	class GnssData
	{
	public:
		GnssData()
		{
			m_dstLng = 0;
			m_dstLat = 0;
			m_chCog = 0;
			m_fAltitude = 0;
			m_nTimeStamp = 0;
			m_enType = en_DeviceGNSS;
		}

		double m_dstLng;
		double m_dstLat;
		unsigned short m_chCog;
		float m_fAltitude;
		EnDeviceType m_enType;
		unsigned int m_nTimeStamp;
	};
}

#endif //  __DDR_DEVICE_COMM_DATA_BASE_H__

