#include <iostream>
#include <windows.h>

//#include "../DDRDeviceInterface/Device/DDRDeviceCommData.h"
//#include "../DDRDeviceInterface/Device/DDRDeviceTypeBase.h"
#include "../DDRDeviceInterface/DDRDeviceInterface.h"
#include "../DDRDeviceInterface/DDRDeviceFactory.h"
#include "../DDRDeviceInterface/Device/DDRDeviceLidar.h"

#ifdef _DEBUG
#pragma comment (lib, "../x64/Debug/DDRDevice_x64d.lib")
#else
#pragma comment (lib, "../x64/Release/DDRDevice_x64r.lib")
#endif

int main()
{
	std::cout << "Main() +++\n";

	std::shared_ptr<DDRDevice::DDRDeviceInterface> ptr = DDRDevice::CreateDDRDeviceModule();
	ptr->AddDevice(DDRDevice::en_DeviceLidar);

	auto lidarMap = ptr->GetPtrMap(DDRDevice::en_DeviceLidar);
	std::cout << "Map size:" << lidarMap->size() << std::endl;


	auto it = lidarMap->begin();

	while (it != lidarMap->end())
	{
		auto pLidarData = it->second;

		DDRDevice::LidarBase* pData = (DDRDevice::LidarBase*)(pLidarData.get());
		DDRDevice::LidarInfo info;
		info.m_strIp = "192.168.0.82";
		info.m_strName = "Lidar";

		pData->Init(info);
		std::cout << it->first.c_str() << "   Name:" << pData->GetName().c_str() << std::endl;



		while (1)
		{
			auto lidarData = pData->GetData();
			if(lidarData.get())
				std::cout << "Recv lidar data count:" << lidarData->m_Data.size() << std::endl;
			::Sleep(20);
		}

		it++;
	}


	


	//auto pLidar = ptr->GetPtr(DDRDevice::en_DeviceLidar, "lidar");
	//DDRDevice::LidarBase* pData = (DDRDevice::LidarBase*)(pLidar);
	//pData->Init();
	//std::cout << "pLidar->mType:" << pData->mType << std::endl;


	system("pause");
	return 0;
}