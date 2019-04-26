#include <iostream>
#include <windows.h>

//#include "../DDRDeviceInterface/Device/DDRDeviceCommData.h"
//#include "../DDRDeviceInterface/Device/DDRDeviceTypeBase.h"
#include "../DDRDeviceInterface/DDRDeviceInterface.h"
#include "../DDRDeviceInterface/DDRDeviceFactory.h"
#include "../DDRDeviceInterface/Device/DDRDeviceLidar.h"

#include "opencv2/opencv.hpp"

#ifdef _DEBUG
#pragma comment (lib, "../x64/Debug/DDRDevice_x64d.lib")
#else
#pragma comment (lib, "../x64/Release/DDRDevice_x64r.lib")
#endif

void lidarTest()
{
	std::shared_ptr<DDRDevice::DDRDeviceInterface> ptr = DDRDevice::CreateDDRDeviceModule();

	std::cout << "Device Version:" << ptr->GetDeviceVersion().c_str() << " Date:" << ptr->GetDeviceDate().c_str() << std::endl;

	std::string strLidarName("LidarYoung");
	ptr->AddDevice(DDRDevice::en_DeviceLidar, strLidarName);
	
	auto lidar = ptr->GetPtr(DDRDevice::en_DeviceLidar, strLidarName);

	DDRDevice::LidarBase* pData = (DDRDevice::LidarBase*)(lidar);
	DDRDevice::LidarInfo info;
	info.m_strIp = "192.168.0.100";
	info.m_strName = strLidarName;

	if (pData->Init(info))
	{
		std::cout << "Init lidar success ...\n";
		while (1)
		{
			auto lidarData = pData->GetData();
			if (lidarData.get())
				std::cout << "Recv lidar data count:" << lidarData->m_Data.pts.size() << std::endl;
			::Sleep(20);
		}
		pData->DeInit();
	}
	else
	{
		std::cout << "Init lidar failed ...\n";
	}

	ptr->RemoveDevice(DDRDevice::en_DeviceLidar);
}

int main()
{
	std::cout << "Main() +++\n";
	{
		//std::shared_ptr<DDRDevice::DDRDeviceInterface> ptr = DDRDevice::CreateDDRDeviceModule();

		//std::cout << "Device Version:" << ptr->GetDeviceVersion().c_str() << " Date:" << ptr->GetDeviceDate().c_str() << std::endl;

		//std::string strLidarName("LidarYoung");
		//ptr->AddDevice(DDRDevice::en_DeviceLidar, strLidarName);
		//ptr->AddDevice(DDRDevice::en_DeviceLidar, "1111");
		//ptr->AddDevice(DDRDevice::en_DeviceLidar, "222");
		//ptr->AddDevice(DDRDevice::en_DeviceLidar, "333");
		//ptr->RemoveDevice(DDRDevice::en_DeviceLidar);
	}

	std::shared_ptr<DDRDevice::DDRDeviceInterface> ptr = DDRDevice::CreateDDRDeviceModule();

	std::cout << "Device Version:" << ptr->GetDeviceVersion().c_str() << " Date:" << ptr->GetDeviceDate().c_str() << std::endl;

	std::string strLidarName("LidarYoung");
	ptr->AddDevice(DDRDevice::en_DeviceLidar, strLidarName);
	auto lidarMap = ptr->GetPtrMap(DDRDevice::en_DeviceLidar);
	std::cout << "Map size:" << lidarMap->size() << std::endl;


	auto it = lidarMap->begin();

	while (it != lidarMap->end())
	{
		auto pLidarData = it->second;

		DDRDevice::LidarBase* pData = (DDRDevice::LidarBase*)(pLidarData.get());
		DDRDevice::LidarInfo info;
		info.m_strIp = "192.168.0.100";
		info.m_strName = strLidarName;

		pData->Init(info);
		std::cout << it->first.c_str() << "   Name:" << pData->GetName().c_str() << std::endl;

		int nCount = 0;

		while (1)
		{
			auto lidarData = pData->GetData();
			if(lidarData.get())
				std::cout << "Recv lidar data count:" << lidarData->m_Data.pts.size() << std::endl;
			::Sleep(20);

			if (nCount++ > 20)
			{
				//pData->DeInit();
				//break;
			}
		}

		//it++;
	}

	


	//auto pLidar = ptr->GetPtr(DDRDevice::en_DeviceLidar, "lidar");
	//DDRDevice::LidarBase* pData = (DDRDevice::LidarBase*)(pLidar);
	//pData->Init();
	//std::cout << "pLidar->mType:" << pData->mType << std::endl;


	system("pause");
	return 0;
}