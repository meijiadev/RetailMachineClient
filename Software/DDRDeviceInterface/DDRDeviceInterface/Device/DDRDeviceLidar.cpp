#include "DDRDeviceLidar.h"
#include "DeviceManager.h"
#include <iostream>
namespace DDRDevice
{
	bool LidarBase::Init(LidarInfo info)
	{
		m_strName = info.m_strName;
		//m_strIp
		m_nLidarID = 0;
		bool bret = false;
		char *ip = (char*)info.m_strIp.c_str();
		int  lidarID = 0;
		if (DDRDevicedManager::GetInstance()->AddOneLidar(ip, lidarID))
		{
			std::cout << "LidarBase::Init() +++ success\n";
			bret = true;
			m_nLidarID = lidarID;
		}
		else
		{
			std::cout << "LidarBase::Init() +++ failed\n";
		}

		return true;
	}

	bool LidarBase::DeInit()
	{
		std::cout << "LidarBase::DeInit() +++\n";
		return true;
	}

	std::string LidarBase::GetName()
	{
		return m_strName;
	}

	std::shared_ptr<LidarData> LidarBase::GetData()
	{
		bool bret = true;
		std::vector<DDRGeometry::APoint> result;
		if (!DDRDevicedManager::GetInstance()->GetOneScan(m_nLidarID, result))
		{
			return nullptr;
		}

		std::shared_ptr<LidarData> pData = std::make_shared<LidarData>();
		for (int i = 0; i < result.size(); i++)
		{
			LidarData::Data data;
			data.angle = result[i].angle;
			data.distance = result[i].distance;
			pData->m_Data.push_back(data);
		}

		return pData;
	}
}


