#include "DeviceManager.h"
#include "CommPublicFun.h"
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

			switch (m_EmbUser.GetFrameTypeCode())
			{
				case DDRDrivers::MB_FRAMETYPE_IMUMOTOR:
				{
					break;
				}
				case DDRDrivers::MB_FRAMETYPE_EBR:
				{
					break;
				}
				case DDRDrivers::MB_FRAMETYPE_RANGING:
				{
					m_EmbUser.UpdateRANGINGObs();
					break;
				}
				case DDRDrivers::MB_FRAMETYPE_GETVERSION:// Get Emb Version
				{

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
	
	}

	bool DDRDevicedManager::GetIMUData(IMUData &data)
	{
		return false;
	}

	bool DDRDevicedManager::GetMotorData(MotorData &data)
	{
		return false;
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

}
