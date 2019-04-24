// DDRDeviceInterface.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "DDRDeviceInterface.h"
#include "DeleteUnwrapper.h"
#include "Device/Device.h"
#include "Device/DDRDeviceCommData.h"
#include "Device/DDRDeviceTypeBase.h"
#include <map>
#include <iostream>

namespace DDRDevice {

	int g_cntPPOACModule = 0;
	static int g_VerPPOAC = 10003;

	class DDRDeviceImpl : public DefaultDelete<DDRDeviceInterface> 
	{
		bool AddDevice(EnDeviceType type)
		{
			if (en_DeviceLidar == type)
			{
				std::cout << "Add Lidar Success...\n";
				std::shared_ptr<LidarBase> plidar = std::make_shared<LidarBase>();
				std::shared_ptr<DeviceTypeMap> deviceMap = std::make_shared<DeviceTypeMap>();
				deviceMap->insert(std::pair<std::string, std::shared_ptr<DevicePtrContainer>>("lidar", plidar));
				m_mapDevice.insert(std::pair<EnDeviceType, std::shared_ptr<DeviceTypeMap>>(en_DeviceLidar, deviceMap));
			}
			return true;
		}

		bool RemoveDevice(EnDeviceType type)
		{
			if (en_DeviceLidar == type)
			{
				std::cout << "Remove Lidar Success...\n";
			}

			return true;
		}
		void _stdcall destroy()
		{
			delete this;
			--g_cntPPOACModule;
		}

		DeviceTypeMap* GetPtrMap(EnDeviceType type)
		{
			auto p = m_mapDevice[type];
			return p.get();
		}

		DevicePtrContainer* GetPtr(EnDeviceType type, std::string strName)
		{
			auto ptrDeviceMap = GetPtrMap(type);
			DeviceTypeMap map = *((DeviceTypeMap*)(ptrDeviceMap));
			return map[strName].get();
		}

	private:
		
		std::map<EnDeviceType, std::shared_ptr<DeviceTypeMap>> m_mapDevice;
	};

	DDRDeviceInterface* _stdcall _createDDRDeviceModule() {
		if (0 == g_cntPPOACModule) {
			++g_cntPPOACModule;
			return (new DDRDeviceImpl);
		}
		else { return nullptr; }




	}
}



