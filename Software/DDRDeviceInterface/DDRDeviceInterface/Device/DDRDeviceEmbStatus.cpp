#include "DDRDeviceEmbStatus.h"
#include "DeviceManager.h"

namespace DDRDevice
{
	bool EmbStatusBase::Init(EmbStatusInfo info)
	{
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
		return info;
	}

	bool EmbStatusBase::SendData(EmbStatusInfo data)
	{
		return true;
	}

	std::string EmbStatusBase::GetName()
	{
		return m_info.m_strName;
	}

}


