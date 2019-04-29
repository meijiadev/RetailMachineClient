#ifndef __DDR_DEVICE_INTERFACE_H_INCLUDED__
#define __DDR_DEVICE_INTERFACE_H_INCLUDED__

#ifdef DDR_DEVICE_LIB
#define __API__DDRDEVICE__ __declspec(dllexport)   
#else
#define __API__DDRDEVICE__ __declspec(dllimport)
//#ifdef _DEBUG
//#ifndef _WIN64
//#pragma comment (lib, "DDRDevice/DDRDevice_d.lib")
//#else
//#pragma comment (lib, "DDRDevice/DDRDevice_x64d.lib")
//#endif
//#else
//#ifndef _WIN64
//#pragma comment (lib, "DDRDevice/DDRDevice_r.lib")
//#else
//#pragma comment (lib, "DDRDevice/DDRDevice_x64r.lib")
//#endif
//#endif
#endif

#include "Device/DDRDeviceCommData.h"
#include "Device/DDRDeviceTypeBase.h"
#include "Device/DDRDeviceLidar.h"
#include "Device/DDRDeviceIMU.h"
#include "Device/DDRDeviceMotor.h"
#include "Device/DDRDeviceControlMoveNormal.h"
#include <memory>

#define DDR_OUT // Output parameter
#define DDR_IN // Input parameter

namespace DDRDevice
{
	class DDRDeviceInterface
	{
	public:
		/*
		* @brief: 获取版本信息
		*
		* @retval: true-成功,  false-失败
		*/
		virtual std::string GetDeviceVersion() = 0;

		/*
		* @brief: 获取日期
		*
		* @retval: true-成功,  false-失败
		*/
		virtual std::string GetDeviceDate() = 0;

		/*
		* @brief: 添加设备
		* @param type[输入参数]:设备类型
		* @param strName[输入参数]:设备名字。每个设备有个唯一的名字。
		*
		* @retval: true-成功,  false-失败
		*/
		virtual bool AddDevice(DDR_IN EnDeviceType type, DDR_IN std::string strName) = 0;

		/*
		* @brief: 移除设备
		* @param type[输入参数]:设备类型
		* @param strName[输入参数]:设备名字
		*
		* @retval: true-成功,  false-失败
		*/
		virtual bool RemoveDevice(DDR_IN EnDeviceType type, DDR_IN std::string strName = "") = 0;

		/*
		* @brief: 根据类型获取设备。有的设备可能是同种类型，但是有多个。
		* @param type[输入参数]:设备类型
		*
		* @retval: nullptr-失败。成功返回设备类型的指针。
		*/
		virtual DeviceTypeMap* GetPtrMap(DDR_IN EnDeviceType type) = 0;

		/*
		* @brief: 根据类型和名字查找设备。这里找的是某个特定的设备了。
		* @param type[输入参数]:设备类型
		* @param strName[输入参数]:设备名字。每个设备有个唯一的名字。
		*
		* @retval: nullptr-失败。成功返回设备指针。
		*/
		virtual DevicePtrContainer* GetPtr(DDR_IN EnDeviceType type, DDR_IN std::string strName) = 0;

		virtual void _stdcall destroy() = 0;
		void operator delete(void *p) {
			if (p != nullptr) {
				((DDRDeviceInterface*)p)->destroy();
			}
		}
	};

	extern "C" __API__DDRDEVICE__ DDRDeviceInterface* _stdcall _createDDRDeviceModule();
}

#endif // __DDR_PPOAC_INTERFACE_H_INCLUDED__