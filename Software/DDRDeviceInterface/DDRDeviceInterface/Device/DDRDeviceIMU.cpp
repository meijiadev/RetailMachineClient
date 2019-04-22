#include "DDRDeviceIMU.h"

namespace DDRDevice
{
	bool IMUBase::Init()
	{
		return true;
	}

	bool IMUBase::DeInit()
	{
		return true;
	}

	std::shared_ptr<IMUData> IMUBase::GetData()
	{
		return nullptr;
	}
}


