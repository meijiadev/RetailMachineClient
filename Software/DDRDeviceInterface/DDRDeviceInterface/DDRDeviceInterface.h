#ifndef __DDR_DEVICE_INTERFACE_H_INCLUDED__
#define __DDR_DEVICE_INTERFACE_H_INCLUDED__

#ifdef DDR_DEVICE_LIB
#define __API__DDRDEVICE__ __declspec(dllexport)   
#else
#define __API__DDRDEVICE__ __declspec(dllimport)
#ifdef _DEBUG
#ifndef _WIN64
#pragma comment (lib, "DDRDevice/DDRDevice_d.lib")
#else
#pragma comment (lib, "DDRDevice/DDRDevice_x64d.lib")
#endif
#else
#ifndef _WIN64
#pragma comment (lib, "DDRDevice/DDRDevice_r.lib")
#else
#pragma comment (lib, "DDRDevice/DDRDevice_x64r.lib")
#endif
#endif
#endif

#define DDR_OUT // Output parameter
#define DDR_IN // Input parameter

namespace DDRDevice
{
	class DDRDeviceInterface
	{
	public:

		/**
		* @brief: 添加一个雷达。一般获取雷达数据之前要先添加雷达
		* @param strLidarIP[输入参数]:要连接的雷达IP。比如:192.168.0.83
		* @param nLidarID[输出参数]:返回雷达ID
		*
		* @retval: true-成功,  false-失败
		*/
		virtual bool AddOneLidar(DDR_IN char* strLidarIP, DDR_OUT int *nLidarID) = 0;

		/**
		* @brief: 向嵌入式下发速度
		* @param fSpeedA[输入参数]:角速度
		* @param fSpeedL[输入参数]:线速度
		*
		* @retval: true-成功,  false-失败
		*/
		virtual bool SendSpeed(DDR_IN float fSpeedA, DDR_IN float fSpeedL) = 0;

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