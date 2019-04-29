#ifndef __DDR_DEVICE_TYPE_BASE_H__
#define __DDR_DEVICE_TYPE_BASE_H__

#include "DDRDeviceCommData.h"
#include <memory>
#include <map>

namespace DDRDevice
{
	/*
		设备接口类。
		T：该设备返回的数据类型。
		U：该设备的一些信息。比如名字。雷达ip，传感器朝向等。
	*/
	template <class T, class U>
	class DeviceTypeBase : public T
	{
	public:

		/*
		* @brief: 初始化设备
		* @param U[输入参数]:设备信息
		*
		* @retval: true-成功,  false-失败
		*/
		virtual bool Init(U) = 0;

		/*
		* @brief: 释放设备
		*
		* @retval: true-成功,  false-失败
		*/
		virtual bool DeInit() = 0;

		/*
		* @brief: 获取设备数据
		*
		* @retval: nullptr-失败。成功返回具体的数据。
		*/
		virtual std::shared_ptr<T> GetData() = 0;

		/*
		* @brief: 获取设备名字
		*
		* @retval: 设备名字
		*/
		virtual std::string GetName() = 0;

		/*
		* @brief: 获取设备信息。就是Init传入时的信息。
		*
		* @retval: 设备信息
		*/
		virtual U GetDeviceInfo() = 0;

		/*
		* @brief: 往设备下发数据
		* @param T[输入参数]:要下发的数据。
		*
		* @retval: true-成功,  false-失败
		*/
		virtual bool SendData(T) = 0;
	};

	class DevicePtrContainer
	{
	};

	/*
		设备类型。（有的设备可能有多个，所以这里用了map）
		前面是名字，后面就是具体某个设备了。
	*/
	typedef std::map<std::string, std::shared_ptr<DevicePtrContainer>> DeviceTypeMap;
}

#endif //  __DDR_DEVICE_DATA_BASE_H__

