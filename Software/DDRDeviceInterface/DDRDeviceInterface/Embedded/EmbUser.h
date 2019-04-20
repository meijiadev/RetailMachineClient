#ifndef __DDR_EMBEDDED_DEVICE_DATA_WRAPPER_H_INCLUDED__
#define __DDR_EMBEDDED_DEVICE_DATA_WRAPPER_H_INCLUDED__

#include "DDREmbConfig.h"
#include "EmbUserBase.h"

namespace DDRDrivers
{
	class EmbUser
	{
	public:
		EmbUser();
		bool IsGNSSDeviceAvailable() const;
		void Quit() {
			m_UserInterface.Stop();
		}
		~EmbUser() { Quit(); }
		void SetMaxReading4StillMotors(int maxReading);

		void SetMBControlPtr(DDREmbeddedServer *pEmbServer);
		bool ReadEmbMB() { return m_UserInterface.ReadEmbData(); }
		bool ReadGNSS() { return m_UserInterface.ReadGNSSData(); }
		bool FlushRx();
		
#if 1
		inline uint8_t GetFrameTypeCode() const { return (m_UserInterface.GetEmbMBBuffer()[MB_POS_FUNCCODE_U8] >> MB_POS_FRAMETYPE_4B); }
		inline uint32_t GetTimeStamp() const { return *((uint32_t*)(m_UserInterface.GetEmbMBBuffer() + MB_POS_TIMESTAMP_U32)); }
		// status flags
		inline uint8_t GetStickConStatus() const { return ((m_UserInterface.GetEmbMBBuffer()[MB_POS_FUNCCODE_U8] >> MB_BPOS_STICKCONTROLLER_1B) & 0x01); }
		inline uint8_t GetEmergencyStopStatus() const { return ((m_UserInterface.GetEmbMBBuffer()[MB_POS_FUNCCODE_U8] >> MB_BPOS_EMGSTOP_1B) & 0x01); }
		inline uint8_t GetIRStopStatus() const { return ((m_UserInterface.GetEmbMBBuffer()[MB_POS_FUNCCODE_U8] >> MB_BPOS_IRSTOP_1B) & 0x01); }
		inline uint8_t GetUSStopStatus() const { return ((m_UserInterface.GetEmbMBBuffer()[MB_POS_FUNCCODE_U8] >> MB_BPOS_USSTOP_1B) & 0x01); }
		// imu-wheels
		inline int16_t GetAccX() const { return *((int16_t*)(m_UserInterface.GetEmbMBBuffer() + MB_POS_IMFRAME_ACCX_16)); }
		inline int16_t GetAccY() const { return *((int16_t*)(m_UserInterface.GetEmbMBBuffer() + MB_POS_IMFRAME_ACCY_16)); }
		inline int16_t GetAccZ() const { return *((int16_t*)(m_UserInterface.GetEmbMBBuffer() + MB_POS_IMFRAME_ACCZ_16)); }
		inline int16_t GetGX() const { return *((int16_t*)(m_UserInterface.GetEmbMBBuffer() + MB_POS_IMFRAME_GX_16)); }
		inline int16_t GetGY() const { return *((int16_t*)(m_UserInterface.GetEmbMBBuffer() + MB_POS_IMFRAME_GY_16)); }
		inline int16_t GetGZ() const { return *((int16_t*)(m_UserInterface.GetEmbMBBuffer() + MB_POS_IMFRAME_GZ_16)); }
		inline int16_t GetIMUTempBy100() const { return *((int16_t*)(m_UserInterface.GetEmbMBBuffer() + MB_POS_IMFRAME_IMUTEMP_16)); }
		inline int16_t GetLeftMotorSpeed() const { return *((int16_t*)(m_UserInterface.GetEmbMBBuffer() + MB_POS_IMFRAME_LEFT_MOTORSPEED_16)); }
		inline int16_t GetRightMotorSpeed() const { return *((int16_t*)(m_UserInterface.GetEmbMBBuffer() + MB_POS_IMFRAME_RIGHT_MOTORSPEED_16)); }
		// environment-battery-recharging
		inline int16_t GetEBREnvTempBy100() const { return *((int16_t*)(m_UserInterface.GetEmbMBBuffer() + MB_POS_EBRFRAME_ENVTEMP_16)); }
		inline uint16_t GetEBREnvHumBy100() const { return *((uint16_t*)(m_UserInterface.GetEmbMBBuffer() + MB_POS_EBRFRAME_ENVHUM_U16)); }
		inline uint16_t GetEBRBattStat() const { return *((uint16_t*)(m_UserInterface.GetEmbMBBuffer() + MB_POS_EBRFRAME_BATTSTAT_U16)); }
		inline uint16_t GetEBRBattVoltageInMV() const { return *((uint16_t*)(m_UserInterface.GetEmbMBBuffer() + MB_POS_EBRFRAME_BATTVOLT_U16)); }
		inline uint16_t GetEBRBattPercentageBy100() const { return *((uint16_t*)(m_UserInterface.GetEmbMBBuffer() + MB_POS_EBRFRAME_BATTPERCENT_U16)); }
		inline int16_t GetEBRBattTemp_2Bytes() const { return *((int16_t*)(m_UserInterface.GetEmbMBBuffer() + MB_POS_EBRFRAME_BATTTEMP_16)); }
		inline uint16_t GetEBREnvPMIndex() const { return *((uint16_t*)(m_UserInterface.GetEmbMBBuffer() + MB_POS_EBRFRAME_ENVPMINDEX_U16)); }
		// recharging related
		inline uint8_t GetEBRRcgIsRcgMode() const { return m_UserInterface.GetEmbMBBuffer()[MB_POS_EBRFRAME_RCGMODE_U8]; }
		inline uint8_t GetEBRRcgState() const { return m_UserInterface.GetEmbMBBuffer()[MB_POS_EBRFRAME_RCGSTAT_U8]; }
		inline uint8_t GetEBRRcgIsRcgCharging() const { return ((m_UserInterface.GetEmbMBBuffer()[MB_POS_EBRFRAME_RCGSTAT_U8] >> MB_BPOS_EBRFRAME_RCGSTAT_ISCHARGING_1B) & 0x01); }
		inline uint8_t GetEBRRcgIsRcgDocked() const { return ((m_UserInterface.GetEmbMBBuffer()[MB_POS_EBRFRAME_RCGSTAT_U8] >> MB_BPOS_EBRFRAME_RCGSTAT_ISDOCKED_1B) & 0x01); }
		inline uint8_t GetEBRRcgIsRcgRightReceived() const { return ((m_UserInterface.GetEmbMBBuffer()[MB_POS_EBRFRAME_RCGSTAT_U8] >> MB_BPOS_EBRFRAME_RCGSTAT_RIGHTIR_1B) & 0x01); }
		inline uint8_t GetEBRRcgIsRcgLeftReceived() const { return ((m_UserInterface.GetEmbMBBuffer()[MB_POS_EBRFRAME_RCGSTAT_U8] >> MB_BPOS_EBRFRAME_RCGSTAT_MIDIR_1B) & 0x01); }
		inline uint8_t GetEBRRcgIsRcgMiddleReceived() const { return ((m_UserInterface.GetEmbMBBuffer()[MB_POS_EBRFRAME_RCGSTAT_U8] >> MB_BPOS_EBRFRAME_RCGSTAT_LEFTIR_1B) & 0x01); }
		// ranging related
		void UpdateRANGINGObs(); 
		inline int GetRANGINGGroupNo() const { return (int)(m_UserInterface.GetEmbMBBuffer()[MB_POS_EBRFRAME_ENVTEMP_16]); }
		unsigned short GetRANGINGDist(int nominalSensorID) const; // 1, 2, ..., 32
		bool IsRANGINGTriggered(int nominalSensorID) const; // 1, 2, ..., 32

		inline const BYTE* GetFrameDataHead() const { return m_UserInterface.GetEmbMBBuffer(); }
#endif
		bool GetGNSSData(double &dstLng, double &dstLat,
			unsigned short &cog, float &altitude,
			unsigned int &UNIXTimeStamp) {
			return m_UserInterface.GetGNSSData(dstLng, dstLat, cog,
				                               altitude, UNIXTimeStamp);
		}
		bool IsOkay4Calibration() const {
			return (isIMUAroundWorkingTemperature() &&
				    AreMotorsStill());
		}
		void SendSpeedInstruction(int16_t leftSpeed, int16_t rightSpeed);		
		void StopMotors() {
			SendSpeedInstruction(0, 0);
		}
		bool SetIMUTargetTemp(float fTemp); // temp is in Celsius
		void EnableEmbeddedSafetyWatcher();
		void DisableEmbeddedSafetyWatcher();
		void SetCASensor(int SensorID,
			int distSetting, // -1 to disable, positive to enable (with trigger distance in centimeters)
			int exeTime = 100 // execution time in milliseconds
			);
		void DisableAllCASensors();
		void SwitchRechargingStatus(bool bRecharging);
		void ResetEmbMB();

		bool AreMotorsStill() const;
		void SendGetEmbSysInfo();
		
	protected:
		EmbUserBase m_UserInterface;
		DDREmbeddedServer *m_pEmbServer;
		BYTE m_CommandBuf[10];
		int m_TargetTempBy100;
		int m_maxReading4StillMotors;

		unsigned short m_rangingDist[32];
		unsigned int m_rangingTriggerStat;
		
		bool isIMUAroundWorkingTemperature() const;
	};
}

#endif
