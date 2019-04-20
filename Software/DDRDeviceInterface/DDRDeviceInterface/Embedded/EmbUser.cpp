#include "EmbUser.h"

#include <iostream>
#include "CommonDef/CommonDefFunc.h"

#define __NEW_EMBEDDED_PROTOCOL_V3_DEFINED__

namespace DDRDrivers {

EmbUser::EmbUser() {
	m_CommandBuf[0] = 0x55;
	m_CommandBuf[1] = 0xFF;
	m_CommandBuf[8] = 0xFF;
	m_CommandBuf[9] = 0xFE;
	m_pEmbServer = nullptr;
	m_maxReading4StillMotors = 50;
	for (int i = 0; i < 32; ++i) {
		m_rangingDist[i] = 65535;
	}
	m_rangingTriggerStat = 0;
}

bool EmbUser::IsGNSSDeviceAvailable() const
{
	return (m_pEmbServer && m_pEmbServer->IsGNSSModuleAvailable());
}

void EmbUser::SetMaxReading4StillMotors(int maxReading) {
	if (maxReading < 0) {
		maxReading = -maxReading;
	}
	if (maxReading > 50) {
		maxReading = 50;
	}
	m_maxReading4StillMotors = maxReading;
}

void EmbUser::SetMBControlPtr(DDREmbeddedServer *pEmbServer) {
	m_pEmbServer = pEmbServer;
}

bool EmbUser::FlushRx() {
	if (m_pEmbServer) {
		return m_pEmbServer->PurgeDevices();
	} else { return false; }
}

void EmbUser::ResetEmbMB() {
	if (m_pEmbServer) {
		m_CommandBuf[2] = 0xFF;
		m_CommandBuf[3] = 0xA5;
		m_CommandBuf[4] = 0xA5;
		m_CommandBuf[5] = 0xA5;
		m_CommandBuf[6] = 0xA5;
		m_CommandBuf[7] = (BYTE)((m_CommandBuf[2] +
			                      m_CommandBuf[3] + m_CommandBuf[4] +
								  m_CommandBuf[5] + m_CommandBuf[6]) & 0xff);
		m_pEmbServer->WriteEmbMB(m_CommandBuf, 10);
	}
}

bool EmbUser::SetIMUTargetTemp(float fTemp) {
	if (!m_pEmbServer) {
		std::cout << "未正确连接至嵌入式主板控制模块..." << std::endl;
		return false;
	}
	if (fTemp < 20.0f || fTemp > 70.0f) {
		std::cout << "设置嵌入式IMU工作温度("
			<< fTemp << "摄氏度)超出合理范围, 请检查..." << std::endl;
		return false;
	}
	std::cout << "正在设置嵌入式IMU工作温度("
		<< fTemp << "摄氏度)..." << std::endl;
	m_TargetTempBy100 = (int)round(fTemp * 100);
	m_CommandBuf[2] = 0x05;
	m_CommandBuf[3] = (BYTE)(m_TargetTempBy100 & 0xff);
	m_CommandBuf[4] = (BYTE)((m_TargetTempBy100 >> 8) & 0xff);
	m_CommandBuf[5] = 0x00;
	m_CommandBuf[6] = 0x00;
	m_CommandBuf[7] = (BYTE)((m_CommandBuf[2] +
		                      m_CommandBuf[3] + m_CommandBuf[4] +
							  m_CommandBuf[5] + m_CommandBuf[6]) & 0xff);
	m_pEmbServer->WriteEmbMB(m_CommandBuf, 10);

	// wait for actual temperature to reach the target
	while (ReadEmbMB()) {};
	bool bSuccess = false;
	int nFramesRead = 0;
	for (int nFramesRead = 0;;)
	{
		while (!ReadEmbMB()) {
			DDRCommonDef::Sleep(5);
		}
		if (GetFrameTypeCode() != MB_FRAMETYPE_IMUMOTOR) {
			continue;
		}
		++nFramesRead; // IMU-wheel only
		if (isIMUAroundWorkingTemperature()) {
			std::cout << "IMU工作温度已成功到达设定值!" << std::endl;
			bSuccess = true;
			break;
		}
		if (nFramesRead >= 500) {
			std::cout << "IMU工作温度长时间未能到达设定值（"
				<< fTemp << "摄氏度），请检查机器人连接及温度状况..." << std::endl;
			break;
		}
		if (nFramesRead % 50 == 0) {
			std::cout << "IMU正在调整工作温度，当前温度为"
				<< GetIMUTempBy100() * 0.01f << "摄氏度..." << std::endl;
			m_pEmbServer->WriteEmbMB(m_CommandBuf, 10);
		}
	}

	return bSuccess;
}

void EmbUser::UpdateRANGINGObs() {
	if (GetFrameTypeCode() == MB_FRAMETYPE_RANGING) {
		memcpy(m_rangingDist + GetRANGINGGroupNo() * 8,
			   m_UserInterface.GetEmbMBBuffer() + MB_POS_RNGRAME_RANGINGDATA_HEADER_U16,
			   sizeof(unsigned short) * 8);
		m_rangingTriggerStat |= 0xFF << (GetRANGINGGroupNo() * 8);
		m_rangingTriggerStat &= (m_UserInterface.GetEmbMBBuffer()[MB_POS_RNGRAME_TRIGGER_STAT_HEADER_U8])
				                << (GetRANGINGGroupNo() * 8);
	}
}

unsigned short EmbUser::GetRANGINGDist(int nominalSensorID) const {
	if (nominalSensorID <= 0 || nominalSensorID > 32) {
		return 65535;
	} else {
		return m_rangingDist[nominalSensorID - 1];
	}
}

bool EmbUser::IsRANGINGTriggered(int nominalSensorID) const {
	if (nominalSensorID <= 0 || nominalSensorID > 32) {
		return false;
	} else {
		return ((m_rangingTriggerStat & (1 << nominalSensorID)) != 0);
	}
}

bool EmbUser::isIMUAroundWorkingTemperature() const {
	return (abs(GetIMUTempBy100() - m_TargetTempBy100) <= 50); // within 0.5 celsius
}

bool EmbUser::AreMotorsStill() const {
	return (abs(GetLeftMotorSpeed()) <= m_maxReading4StillMotors &&
		    abs(GetRightMotorSpeed()) <= m_maxReading4StillMotors);
}

// xcy add 20180816
void EmbUser::SendGetEmbSysInfo()
{
	// 0x55,0xFF,0x09,0x00,0x00,0x00,0x00,0x09,0xFF,0xFE
	m_CommandBuf[2] = 0x09;
	m_CommandBuf[3] = 0x00;
	m_CommandBuf[4] = 0x00;
	m_CommandBuf[5] = 0x00;
	m_CommandBuf[6] = 0x00;
	m_CommandBuf[7] = 0x09;
	m_pEmbServer->WriteEmbMB(m_CommandBuf, 10);
	Sleep(20);
}

void EmbUser::SendSpeedInstruction(int16_t leftSpeed, int16_t rightSpeed) {
	if (m_pEmbServer) {
		m_CommandBuf[2] = 0x01;
		m_CommandBuf[3] = (BYTE)(leftSpeed >> 8);
		m_CommandBuf[4] = (BYTE)(leftSpeed & 0xff);
		m_CommandBuf[5] = (BYTE)(rightSpeed >> 8);
		m_CommandBuf[6] = (BYTE)(rightSpeed & 0xff);
		m_CommandBuf[7] = (BYTE)((m_CommandBuf[2] +
			m_CommandBuf[3] + m_CommandBuf[4] +
			m_CommandBuf[5] + m_CommandBuf[6]) & 0xff);
		m_pEmbServer->WriteEmbMB(m_CommandBuf, 10);
	}
}

void EmbUser::EnableEmbeddedSafetyWatcher() {
	m_CommandBuf[2] = 0x02;
	m_CommandBuf[3] = 0x00;
	m_CommandBuf[4] = 0x01;
	m_CommandBuf[5] = 0x00;
	m_CommandBuf[6] = 0x01;
	m_CommandBuf[7] = 0x04;
	m_pEmbServer->WriteEmbMB(m_CommandBuf, 10);
	Sleep(20);
}

void EmbUser::DisableEmbeddedSafetyWatcher() {
	m_CommandBuf[2] = 0x02;
	m_CommandBuf[3] = 0x00;
	m_CommandBuf[4] = 0x02;
	m_CommandBuf[5] = 0x00;
	m_CommandBuf[6] = 0x02;
	m_CommandBuf[7] = 0x06;
	m_pEmbServer->WriteEmbMB(m_CommandBuf, 10);
	Sleep(20);
}

void EmbUser::SetCASensor(int SensorID,
	                      int distSetting,
						  int exeTime) {
	m_CommandBuf[2] = 0x03;
	if (SensorID <= 0 || SensorID > 32) { return; }
	m_CommandBuf[4] = (BYTE)(SensorID & 0xFF);
	if (distSetting > 0) {
		m_CommandBuf[3] = 0x00;
		m_CommandBuf[5] = (BYTE)(distSetting & 0xff);
		m_CommandBuf[6] = (BYTE)((distSetting >> 8) & 0xff);
	} else {
		m_CommandBuf[3] = 0x01;
		m_CommandBuf[5] = 0x00;
		m_CommandBuf[6] = 0x00;
	}
	m_CommandBuf[7] = (BYTE)((m_CommandBuf[2] +
		                      m_CommandBuf[3] + m_CommandBuf[4] +
							  m_CommandBuf[5] + m_CommandBuf[6]) & 0xff);
	m_pEmbServer->WriteEmbMB(m_CommandBuf, 10);
	if (exeTime < 0) {
		exeTime = 0;
	} else if (exeTime > 100) {
		exeTime = 100;
	}
	DDRCommonDef::Sleep(exeTime);
}

void EmbUser::DisableAllCASensors() {
	m_CommandBuf[2] = 0x03;
	m_CommandBuf[3] = 0x00;
	m_CommandBuf[4] = 0x00;
	m_CommandBuf[5] = 0x00;
	m_CommandBuf[6] = 0x00;
	m_CommandBuf[7] = 0x03;
	m_pEmbServer->WriteEmbMB(m_CommandBuf, 10);
}

void EmbUser::SwitchRechargingStatus(bool bRecharging) {
	m_CommandBuf[2] = 0x04;
	if (bRecharging) {
		m_CommandBuf[3] = 0xA5;
		m_CommandBuf[4] = 0xA5;
		m_CommandBuf[5] = 0xA5;
		m_CommandBuf[6] = 0xA5;
	} else {
		m_CommandBuf[3] = 0xB6;
		m_CommandBuf[4] = 0xB6;
		m_CommandBuf[5] = 0xB6;
		m_CommandBuf[6] = 0xB6;
	}
	m_CommandBuf[7] = (BYTE)((m_CommandBuf[2] +
		                      m_CommandBuf[3] + m_CommandBuf[4] +
							  m_CommandBuf[5] + m_CommandBuf[6]) & 0xff);
	m_pEmbServer->WriteEmbMB(m_CommandBuf, 10);
}

}