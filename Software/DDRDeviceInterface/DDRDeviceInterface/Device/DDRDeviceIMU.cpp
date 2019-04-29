#include "include/DDRDeviceIMU.h"
#include "DeviceManager.h"

namespace DDRDevice
{
	bool IMUBase::Init(IMUInfo info)
	{
		m_info.m_enType = info.m_enType;
		m_info.m_fIMUTemp = info.m_fIMUTemp;
		m_info.m_strName = info.m_strName;
		return true;
	}

	bool IMUBase::DeInit()
	{
		return true;
	}

	std::shared_ptr<IMUData> IMUBase::GetData()
	{
		std::shared_ptr<IMUData> ptr = std::make_shared<IMUData>();
		if (DDRDevicedManager::GetInstance()->GetIMUData(*ptr))
		{
			return ptr;
		}
		ptr.reset();
		return nullptr;
	}

	IMUInfo IMUBase::GetDeviceInfo()
	{
		//IMUInfo m_info;
		return m_info;
	}

	bool IMUBase::SendData(IMUData data)
	{
		DDRDevicedManager::GetInstance()->SetIMUTargetTemp(data.m_fIMUTemp);
		return true;
	}

	std::string IMUBase::GetName()
	{
		return m_info.m_strName;
	}
}


