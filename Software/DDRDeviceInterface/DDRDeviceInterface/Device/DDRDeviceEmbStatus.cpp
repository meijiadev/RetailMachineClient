#include "include/DDRDeviceEmbStatus.h"
#include "DeviceManager.h"

#include <iostream>

namespace DDRDevice
{
	bool EmbStatusBase::Init(EmbStatusInfo info)
	{
		std::cout << "EmbStatusBase::Init() +++\n";
		return true;
	}

	bool EmbStatusBase::DeInit()
	{
		return true;
	}

	std::shared_ptr<EmbStatusData> EmbStatusBase::GetData()
	{
		std::shared_ptr<EmbStatusData> ptr = std::make_shared<EmbStatusData>();

		if (DDRDevicedManager::GetInstance()->GetEmbStatusData(*ptr))
		{
			std::shared_ptr<EmbStatusData> ptr = std::make_shared<EmbStatusData>();
			return ptr;
		}
		ptr.reset();
		return nullptr;
	}

	EmbStatusInfo EmbStatusBase::GetDeviceInfo()
	{
		EmbStatusInfo info;
		return m_info;
	}

	bool EmbStatusBase::SendData(EmbStatusData data)
	{
		return true;
	}

	std::string EmbStatusBase::GetName()
	{
		return m_info.m_strName;
	}

}


