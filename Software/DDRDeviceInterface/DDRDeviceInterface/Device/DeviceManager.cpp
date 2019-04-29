#include "DeviceManager.h"
#include "CommPublicFun/CommPublicFun.h"
namespace DDRDevice
{
	DDRDevicedManager * DDRDevicedManager::GetInstance()
	{
		static DDRDevicedManager *ptr = nullptr;
		if (!ptr)
		{
			ptr = new DDRDevicedManager();
		}
		return ptr;
	}

	DDRDevicedManager::DDRDevicedManager()
		: m_EmbServer(),
		m_EmbUser(),
		m_bQuit(false)
	{
		// 大于INFO的会被输出
		DDRFramework::Log::getInstance()->setLevel(DDRFramework::Log::Level::INFO);

		// 设定输出的地方
		DDRFramework::Log::getInstance()->setTarget(/*DDRFramework::Log::Target::STDOUT |*/
			DDRFramework::Log::Target::LOG_FILE);

		// 输出Log
		//LevelLog(DDRFramework::Log::Level::INFO, "Debug level %s", "ssssssss");
		std::thread t2(&DDRDevicedManager::ParseEmbSubThread, this);
		t2.detach();
	}

	DDRDevicedManager::~DDRDevicedManager()
	{
		m_bQuit = true;
		m_EmbUser.Quit();
		m_EmbServer.Close();
	}

	void  DDRDevicedManager::ParseEmbSubThread(void *param)
	{
		DDRDevicedManager *pManager = (DDRDevicedManager *)param;
		if (pManager)
		{
			pManager->ParseEmbData();
		}
	}

	void DDRDevicedManager::ParseEmbData()
	{
		while (true)
		{
			if (m_bQuit)
			{
				break;
			}

			if (m_EmbUser.ReadGNSS())
			{
				ParseGNSSData();
			}

			if (!m_EmbUser.ReadEmbMB())
			{
				DDRDevice::Sleep(2);
				continue;
			}
			std::lock_guard<std::mutex> lock(m_mtxDealEmbData);
			switch (m_EmbUser.GetFrameTypeCode())
			{
				case DDRDrivers::MB_FRAMETYPE_IMUMOTOR:
				{
					if (m_EmbUser.IsOkay4Calibration()) 
					{
						m_OffsetCorrector.AddMeasurement(m_EmbUser.GetGZ(), m_EmbUser.GetTimeStamp());
					}

					m_bStickConStatus = (m_EmbUser.GetStickConStatus() == 1);
					m_bEmergencyStopStatus = (m_EmbUser.GetEmergencyStopStatus() == 1);
					m_bIRStopStatus = (m_EmbUser.GetIRStopStatus() == 1);
					m_bUSStopStatus = (m_EmbUser.GetUSStopStatus() == 1);

					m_sAccX = m_EmbUser.GetAccX();
					m_sAccY = m_EmbUser.GetAccY();
					m_sAccZ = m_EmbUser.GetAccZ();
					m_sGX = m_EmbUser.GetGX();
					m_sGY = m_EmbUser.GetGY();
					m_sGZ = m_EmbUser.GetGZ();
					m_sIMUTempBy100 = m_EmbUser.GetIMUTempBy100();

					m_bIMUNewData = true;
					m_sLeftMotorSpeed = m_EmbUser.GetLeftMotorSpeed();
					m_sRightMotorSpeed = m_EmbUser.GetRightMotorSpeed();
					m_bMotorNewData = true;
					break;
				}
				case DDRDrivers::MB_FRAMETYPE_EBR:
				{
					m_bStickConStatus = (1 == m_EmbUser.GetStickConStatus());
					m_bEmergencyStopStatus = (1 == m_EmbUser.GetEmergencyStopStatus());
					m_bIRStopStatus = (1 == m_EmbUser.GetIRStopStatus());
					m_bUSStopStatus = (1 == m_EmbUser.GetUSStopStatus());
					m_sBattStat = m_EmbUser.GetEBRBattStat();
					m_sBattVoltageInMV = m_EmbUser.GetEBRBattVoltageInMV();
					m_sBattPercentageBy100 = m_EmbUser.GetEBRBattPercentageBy100();
					m_sBattTemp_2Bytes = m_EmbUser.GetEBRBattTemp_2Bytes();
					m_bEmbNewData = true;

					m_sEBREnvTempBy100 = m_EmbUser.GetEBREnvTempBy100();
					m_sEBREnvHumBy100 = m_EmbUser.GetEBREnvHumBy100();
					m_sEBREnvPM25 = m_EmbUser.GetEBREnvPMIndex();
					m_bEnvironmentNewData = true;

					m_chRcgState = m_EmbUser.GetEBRRcgState();
					m_chRcgIsRcgDocked = m_EmbUser.GetEBRRcgIsRcgDocked();
					m_chRcgIsRcgCharging = m_EmbUser.GetEBRRcgIsRcgCharging();
					m_chRcgIsRcgMode = m_EmbUser.GetEBRRcgIsRcgMode();
					m_chRcgLeftReceived = m_EmbUser.GetEBRRcgIsRcgLeftReceived();
					m_chRcgMiddleReceived = m_EmbUser.GetEBRRcgIsRcgMiddleReceived();
					m_chRcgRcgRightReceived = m_EmbUser.GetEBRRcgIsRcgRightReceived();
					m_bChargingRelatedNewData = true;
					break;
				}
				case DDRDrivers::MB_FRAMETYPE_RANGING: // 传感器数据帧
				{
					m_EmbUser.UpdateRANGINGObs();
					break;
				}
				case DDRDrivers::MB_FRAMETYPE_GETVERSION:// Get Emb Version
				{
					BYTE byData[28] = { 0 };
					memcpy(byData, m_EmbUser.GetFrameDataHead(), 28);
					if (0x09 == byData[3])
					{
						char strTemp[28] = { 0 };
						memcpy((void*)&strTemp[0], byData + 4, 17);
						std::string strEmbInfo(strTemp);
					}
					break;
				}
				case 0xBB:
				{
					break;
				}
				default:
					break;
			}
		}
	}

	void DDRDevicedManager::ParseGNSSData()
	{
		if (m_EmbUser.GetGNSSData(m_dGnssLng, m_dGnssLat, m_nGnssCog, m_fGnssAlt, m_nGnssLng))
		{
			m_bGnssNewData = true;
		}
	}

	bool DDRDevicedManager::GetIMUData(IMUData &data)
	{
		std::lock_guard<std::mutex> lock(m_mtxDealEmbData);
		if(!m_bIMUNewData)
			return false;
		data.m_sAccX = m_sAccX;
		data.m_sAccY = m_sAccY;
		data.m_sAccZ = m_sAccZ;
		data.m_sGX = m_sGX;
		data.m_sGY = m_sGY;
		data.m_sGZ = m_sGZ;
		data.m_fIMUTemp = (float)m_sIMUTempBy100 / 100.0f;
		m_bIMUNewData = false;
		return true;
	}

	bool DDRDevicedManager::GetMotorData(MotorData &data)
	{
		std::lock_guard<std::mutex> lock(m_mtxDealEmbData);
		if (!m_bMotorNewData)
			return false;
		data.m_sLeftMotorSpeed = m_sLeftMotorSpeed;
		data.m_sRightMotorSpeed = m_sRightMotorSpeed;
		m_bMotorNewData = false;
		return true;
	}

	bool DDRDevicedManager::GetGnssData(GnssData &data)
	{
		std::lock_guard<std::mutex> lock(m_mtxDealEmbData);
		if (!m_bGnssNewData)
			return false;
		data.m_dstLng = m_dGnssLng;
		data.m_dstLat = m_dGnssLat;
		data.m_chCog = m_nGnssCog;
		data.m_fAltitude = m_fGnssAlt;
		data.m_nTimeStamp = m_nGnssLng;
		m_bGnssNewData = false;
		return true;
	}
	
	bool DDRDevicedManager::GetEnvironmentData(EnvironmentData &data)
	{
		std::lock_guard<std::mutex> lock(m_mtxDealEmbData);
		if (!m_bEnvironmentNewData)
			return false;
		data.m_sEBREnvHumBy100 = m_sEBREnvHumBy100;
		data.m_sEBREnvPM25 = m_sEBREnvPM25;
		data.m_sEBREnvTempBy100 = m_sEBREnvTempBy100;
		m_bEnvironmentNewData = false;
		return true;
	}

	bool DDRDevicedManager::GetEmbStatusData(EmbStatusData &data)
	{
		std::lock_guard<std::mutex> lock(m_mtxDealEmbData);
		if (!m_bEmbNewData)
			return false;
		data.m_bEmergencyStopStatus = m_bEmergencyStopStatus;
		data.m_bIRStopStatus = m_bIRStopStatus;
		data.m_bStickConStatus = m_bStickConStatus;
		data.m_bUSStopStatus = m_bUSStopStatus;
		data.m_sBattPercentageBy100 = m_sBattPercentageBy100;
		data.m_sBattStat = m_sBattStat;
		data.m_sBattTemp_2Bytes = m_sBattTemp_2Bytes;
		data.m_sBattVoltageInMV = m_sBattVoltageInMV;
		m_bEmbNewData = false;
		return true;
	}

	bool DDRDevicedManager::GetChargingRelatedData(ChargingRelatedData &data)
	{
		std::lock_guard<std::mutex> lock(m_mtxDealEmbData);
		if (!m_bChargingRelatedNewData)
			return false;
		data.m_chRcgState = m_chRcgState;
		data.m_chRcgIsRcgDocked = m_chRcgIsRcgDocked;
		data.m_chRcgIsRcgCharging = m_chRcgIsRcgCharging;
		data.m_chRcgIsRcgMode = m_chRcgIsRcgMode;
		data.m_chRcgLeftReceived = m_chRcgLeftReceived;
		data.m_chRcgMiddleReceived = m_chRcgMiddleReceived;
		data.m_chRcgRcgRightReceived = m_chRcgRcgRightReceived;
		m_bChargingRelatedNewData = false;
		return true;
	}

	// 获取机器人线速度角速度
	bool DDRDevicedManager::GetControlMoveNormalData(ControlMoveNormalData &data)
	{
		std::lock_guard<std::mutex> lock(m_mtxDealEmbData);
		if (!m_bIMUNewData)
			return false;
#if CURR_SYSTEM == DDR_EMBEDDED
		float line = 0, ang = 0;
		MotorConverterSpeed(line, ang);
		data.m_Lin.x_lv = (double)line;
		data.m_Ang.x_av = (double)ang;
#else

#endif
		return true;
	}

	// 下发线速度角速度
	bool DDRDevicedManager::SendControlMoveNormalData(ControlMoveNormalData &data)
	{
		bool bret = false;
#if CURR_SYSTEM == DDR_EMBEDDED
		float line = data.m_Lin.x_lv, ang = data.m_Ang.x_av;
		
		short leftCommand = 0;
		short rightCommand = 0;

		m_IMUWheelConverter.ComputeControlSignal(line, ang, leftCommand, rightCommand);
		m_EmbUser.SendSpeedInstruction(leftCommand, rightCommand);
		bret = true;
#else

#endif
		return bret;
	}

	bool DDRDevicedManager::SetIMUTargetTemp(float fTemp)
	{
		m_EmbUser.SetIMUTargetTemp(fTemp);
		return true;
	}

	bool DDRDevicedManager::SwitchRechargingStatus(bool bData)
	{
		m_EmbUser.SwitchRechargingStatus(bData);
		return true;
	}

	bool DDRDevicedManager::AddLidar(char* ip, std::string strName)
	{
		bool bret = false;
		std::shared_ptr<DDRDrivers::Lidar_AkuSense> pLidar = std::make_shared<DDRDrivers::Lidar_AkuSense>();
		int OutID = 0;
		if (pLidar->AddOneLidar(ip, OutID))
		{
			bret = true;
			m_mapLidar.insert(std::pair<std::string, std::shared_ptr<DDRDrivers::Lidar_AkuSense>>(strName, pLidar));
		}

		return bret;
	}

	bool DDRDevicedManager::GetLidarData(std::string strName, std::vector<DDRGeometry::APoint> &result)
	{
		auto it = m_mapLidar.begin();
		while (it != m_mapLidar.end())
		{
			if (strName == it->first)
			{
				auto pLidarData = it->second;
				return pLidarData->GetOneScan(0, result);
			}
		}
		return false;
	}
	bool DDRDevicedManager::RemoveLidar(std::string strName)
	{
		bool bret = false;
		auto it = m_mapLidar.find(strName);

		if (it != m_mapLidar.end())
		{
			auto pLidarData = it->second;
			pLidarData->End();
			pLidarData.reset();
			m_mapLidar.erase(it);
			bret = true;
		}
		return bret;
	}

	bool DDRDevicedManager::InitMotor(MotorInfo& info)
	{
		m_MotorInfo.m_dLeftRadius = info.m_dLeftRadius;
		m_MotorInfo.m_dRightRadius = info.m_dRightRadius;
		m_MotorInfo.m_dReading2AR_gyro = info.m_dReading2AR_gyro;
		m_MotorInfo.m_dReading2AR_wheel = info.m_dReading2AR_wheel;
		m_MotorInfo.m_dWheelBase = info.m_dWheelBase;
		m_MotorInfo.m_enType = info.m_enType;
		m_MotorInfo.m_strName = info.m_strName;

		m_IMUWheelConverter.Init(m_MotorInfo.m_dLeftRadius, m_MotorInfo.m_dRightRadius
			, m_MotorInfo.m_dWheelBase, m_MotorInfo.m_dReading2AR_wheel
			, m_MotorInfo.m_dReading2AR_gyro);

		return true;
	}
	
	/*
		将左右轮转速和IMU的Gz转换成线速度 角速度
	*/
	bool DDRDevicedManager::MotorConverterSpeed(float &fSpeeedL, float &sSpeedA)
	{
		fSpeeedL = m_IMUWheelConverter.GetLinVel(m_sLeftMotorSpeed, m_sRightMotorSpeed);

		// m_GZOffset 需要机器人静止一段时间算出来。还没想好自标定这个要怎么搞
		sSpeedA = m_IMUWheelConverter.GetAngVel_IMU((float)m_sGZ,(double)m_GZOffset);

		return true;
	}
}
