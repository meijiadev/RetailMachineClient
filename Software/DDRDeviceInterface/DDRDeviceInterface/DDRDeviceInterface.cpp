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
				std::shared_ptr<DeviceType> pdevice = std::make_shared<DeviceType>();
				std::shared_ptr<LidarBase> plidar = std::make_shared<LidarBase>();
				pdevice->insert(std::pair<unsigned int, std::shared_ptr<DevicePtrContainer>>((int)en_DeviceLidar, plidar));
				m_mapDevice.insert(std::pair<EnDeviceType, std::shared_ptr<DeviceType>>(en_DeviceLidar, pdevice));

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

		template <class T>
		T GetPtr(EnDeviceType type)
		{
			auto p = m_mapDevice[type];
			return  (T*)p;
		}

		bool LidarTest()
		{
			auto p = m_mapDevice[en_DeviceLidar];
			std::cout << "p size " << p->size() << std::endl;
			
			//auto pContainer = p[en_DeviceLidar];

			//auto lidar = dynamic_cast<LidarBase*>(pContainer);


			//lidar->Init();
			//std::cout << lidar->mType << std::endl;
			//auto p = GetPtr<en_DeviceLidar>(en_DeviceLidar);
			return true;
		}

	private:
		typedef std::map<unsigned int, std::shared_ptr<DevicePtrContainer>> DeviceType;
		std::map<EnDeviceType, std::shared_ptr<DeviceType>> m_mapDevice;

		//m_mapDevice.insert(pair<int, string>(1, "student_one"));
	};

	DDRDeviceInterface* _stdcall _createDDRDeviceModule() {
		if (0 == g_cntPPOACModule) {
			++g_cntPPOACModule;
			return (new DDRDeviceImpl);
		}
		else { return nullptr; }




	}
}



