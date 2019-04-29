// DDRDeviceInterface.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "DDRDeviceInterface.h"
#include "DeleteUnwrapper.h"
#include "CommPublicFun/CommPublicFun.h"
#include <map>
#include <iostream>

namespace DDRDevice {

	#define MODULE_VERSION "V1.00"

	int g_cntDeviceModule = 0;
	static int g_VerPPOAC = 10003;

	class DDRDeviceImpl : public DefaultDelete<DDRDeviceInterface> 
	{
		std::string GetDeviceVersion()
		{
			return MODULE_VERSION;
		}
		
		std::string GetDeviceDate()
		{
			return DDRDevice::GetTimeNowstring();
		}

		bool AddDevice(EnDeviceType type, std::string strName)
		{
			bool bret = false;
			LevelLog(DDRFramework::Log::Level::INFO, "AddDevice Device Type:%d, Name:%s", type, strName.c_str());
			std::cout << "AddDevice Device Type:" << type << " Name:" << strName.c_str() << std::endl;
			if (en_DeviceLidar == type)
			{
				std::shared_ptr<LidarBase> plidar = std::make_shared<LidarBase>();
				std::shared_ptr<DeviceTypeMap> deviceMap = std::make_shared<DeviceTypeMap>();
				deviceMap->insert(std::pair<std::string, std::shared_ptr<DevicePtrContainer>>(strName, plidar));
				m_mapDevice.insert(std::pair<EnDeviceType, std::shared_ptr<DeviceTypeMap>>(en_DeviceLidar, deviceMap));
				bret = true;
			}
			else if (en_DeviceIMU == type)
			{
				std::shared_ptr<IMUBase> pDevice = std::make_shared<IMUBase>();
				std::shared_ptr<DeviceTypeMap> deviceMap = std::make_shared<DeviceTypeMap>();
				deviceMap->insert(std::pair<std::string, std::shared_ptr<DevicePtrContainer>>(strName, pDevice));
				m_mapDevice.insert(std::pair<EnDeviceType, std::shared_ptr<DeviceTypeMap>>(en_DeviceIMU, deviceMap));
				bret = true;
			}
			else if (en_DeviceMotor == type)
			{
				std::shared_ptr<MotorBase> pDevice = std::make_shared<MotorBase>();
				std::shared_ptr<DeviceTypeMap> deviceMap = std::make_shared<DeviceTypeMap>();
				deviceMap->insert(std::pair<std::string, std::shared_ptr<DevicePtrContainer>>(strName, pDevice));
				m_mapDevice.insert(std::pair<EnDeviceType, std::shared_ptr<DeviceTypeMap>>(en_DeviceMotor, deviceMap));
				bret = true;
			}
			else if (en_DeviceControlMoveNormal == type)
			{
				std::shared_ptr<ControlMoveNormalBase> pDevice = std::make_shared<ControlMoveNormalBase>();
				std::shared_ptr<DeviceTypeMap> deviceMap = std::make_shared<DeviceTypeMap>();
				deviceMap->insert(std::pair<std::string, std::shared_ptr<DevicePtrContainer>>(strName, pDevice));
				m_mapDevice.insert(std::pair<EnDeviceType, std::shared_ptr<DeviceTypeMap>>(en_DeviceControlMoveNormal, deviceMap));
				bret = true;
			}
			else if (en_DeviceEmbStatus == type)
			{
				std::shared_ptr<EmbStatusBase> pDevice = std::make_shared<EmbStatusBase>();
				std::shared_ptr<DeviceTypeMap> deviceMap = std::make_shared<DeviceTypeMap>();
				deviceMap->insert(std::pair<std::string, std::shared_ptr<DevicePtrContainer>>(strName, pDevice));
				m_mapDevice.insert(std::pair<EnDeviceType, std::shared_ptr<DeviceTypeMap>>(en_DeviceEmbStatus, deviceMap));
				bret = true;
			}
			else if (en_DeviceEnvironment == type)
			{
				std::shared_ptr<EnvironmentBase> pDevice = std::make_shared<EnvironmentBase>();
				std::shared_ptr<DeviceTypeMap> deviceMap = std::make_shared<DeviceTypeMap>();
				deviceMap->insert(std::pair<std::string, std::shared_ptr<DevicePtrContainer>>(strName, pDevice));
				m_mapDevice.insert(std::pair<EnDeviceType, std::shared_ptr<DeviceTypeMap>>(en_DeviceEnvironment, deviceMap));
				bret = true;
			}
			else if (en_DeviceChargingRelated == type)
			{
				// DDRDeviceChargingRelated
				std::shared_ptr<ChargingRelatedBase> pDevice = std::make_shared<ChargingRelatedBase>();
				std::shared_ptr<DeviceTypeMap> deviceMap = std::make_shared<DeviceTypeMap>();
				deviceMap->insert(std::pair<std::string, std::shared_ptr<DevicePtrContainer>>(strName, pDevice));
				m_mapDevice.insert(std::pair<EnDeviceType, std::shared_ptr<DeviceTypeMap>>(en_DeviceChargingRelated, deviceMap));
				bret = true;
			}
			else if (en_DeviceGNSS == type)
			{
				std::shared_ptr<GnssBase> pDevice = std::make_shared<GnssBase>();
				std::shared_ptr<DeviceTypeMap> deviceMap = std::make_shared<DeviceTypeMap>();
				deviceMap->insert(std::pair<std::string, std::shared_ptr<DevicePtrContainer>>(strName, pDevice));
				m_mapDevice.insert(std::pair<EnDeviceType, std::shared_ptr<DeviceTypeMap>>(en_DeviceGNSS, deviceMap));
				bret = true;
			}

			return bret;
		}

		bool RemoveDevice(EnDeviceType type, std::string strName)
		{
			std::cout << "RemoveDevice Device Type:" << type << " Name:" << strName.c_str() << std::endl;
			auto deviceMap = m_mapDevice[type]; // �ҵ� ��map �е� �豸map
			auto itMapPtr = m_mapDevice.find(type); // ��Ҫ���Ƴ���Ҫ�õ�
			
			if ("" == strName) // �Ƴ�һ���豸
			{
				if (itMapPtr != m_mapDevice.end())
				{
					deviceMap.reset();// �Ȱ�����ָ������ü�����1
					m_mapDevice.erase(itMapPtr);
				}
			} 
			else // �Ƴ�ĳһ���豸��
			{
				auto deviceOne = (*deviceMap)[strName];
				deviceOne.reset(); // �Ȱ�����ָ������ü�����1

				auto itDevice = (*deviceMap).find(strName);
				if (itDevice != (*deviceMap).end())
				{
					(*deviceMap).erase(itDevice);
				}
			}
			
			return true;
		}

		void _stdcall destroy()
		{
			delete this;
			--g_cntDeviceModule;
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
		if (0 == g_cntDeviceModule) {
			++g_cntDeviceModule;
			return (new DDRDeviceImpl);
		}
		else { return nullptr; }
	}
}



