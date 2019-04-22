#ifndef __DDR_DEVICE_TYPE_BASE_H__
#define __DDR_DEVICE_TYPE_BASE_H__

#include <memory>

namespace DDRDevice
{
	template <typename T>
	class DeviceTypeBase : public T
	{
	public:
		virtual bool Init() = 0;
		virtual bool DeInit() = 0;
		virtual std::shared_ptr<T> GetData() = 0;
	};
}

#endif //  __DDR_DEVICE_DATA_BASE_H__
