#include "DDRDeviceLidar.h"

namespace DDRDevice
{
	bool LidarBase::Init()
	{
		return true;
	}

	bool LidarBase::DeInit()
	{
		return true;
	}

	std::shared_ptr<LidarData> LidarBase::GetData()
	{
		return nullptr;
	}
}


