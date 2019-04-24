#ifndef __DDR_DEVICE_TYPE_BASE_H__
#define __DDR_DEVICE_TYPE_BASE_H__


#include "DDRDeviceCommData.h"

#include <memory>
#include <map>

namespace DDRDevice
{
	template <class T, class U>
	class DeviceTypeBase : public T
	{
	public:
		virtual bool Init(U) = 0;
		virtual bool DeInit() = 0;
		virtual std::shared_ptr<T> GetData() = 0;
		virtual std::string GetName() = 0;
	};

	class DevicePtrContainer
	{
	};

	typedef std::map<std::string, std::shared_ptr<DevicePtrContainer>> DeviceTypeMap;
}

#endif //  __DDR_DEVICE_DATA_BASE_H__

