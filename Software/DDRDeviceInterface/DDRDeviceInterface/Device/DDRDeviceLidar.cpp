#include "DDRDeviceLidar.h"
#include "DeviceManager.h"
#include "CommPublicFun.h"

#include <iostream>
namespace DDRDevice
{
	LidarBase::LidarBase()
	{
		std::cout << "LidarBase::LidarBase() +++ " << std::endl;
	}

	LidarBase::~LidarBase()
	{
		std::cout << "LidarBase::~LidarBase() +++ " << std::endl;
		DeInit();
	}
	bool LidarBase::Init(LidarInfo info)
	{
		std::cout << "LidarBase::Init() +++ Name:" << info.m_strName.c_str() << std::endl;
		m_info.m_enType = info.m_enType;
		m_info.m_strIp = info.m_strIp;
		m_info.m_strName = info.m_strName;

		bool bret = false;
		char *ip = (char*)info.m_strIp.c_str();
		if (DDRDevicedManager::GetInstance()->AddLidar(ip, m_info.m_strName))
		{
			std::cout << "LidarBase::Init() +++ success\n";
			bret = true;
		}
		else
		{
			std::cout << "LidarBase::Init() +++ failed\n";
		}

		return true;
	}

	bool LidarBase::DeInit()
	{
		std::cout << "LidarBase::DeInit() +++ Name:" << m_info.m_strName.c_str() << std::endl;
		return DDRDevicedManager::GetInstance()->RemoveLidar(m_info.m_strName);
	}

	std::string LidarBase::GetName()
	{
		return m_info.m_strName;
	}

	std::shared_ptr<LidarData> LidarBase::GetData()
	{
		bool bret = true;
		std::vector<DDRGeometry::APoint> result;
		if (!DDRDevicedManager::GetInstance()->GetLidarData(m_info.m_strName, result))
		{
			return nullptr;
		}

		std::shared_ptr<LidarData> pData = std::make_shared<LidarData>();
		pData->m_nTimeStamp = getSystemTimeMillitm();
		for (int i = 0; i < result.size(); i++)
		{
			LidarData::Data data;
			data.angle = result[i].angle;
			data.distance = result[i].distance;
			pData->m_Data.push_back(data);
		}

		return pData;
	}

	LidarInfo LidarBase::GetDeviceInfo()
	{
		return m_info;
	}

	bool LidarBase::SendData(LidarData data)
	{
		return true;
	}
}


