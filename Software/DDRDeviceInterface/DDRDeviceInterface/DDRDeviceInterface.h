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
		* @brief: ��ȡ�汾��Ϣ
		*
		* @retval: true-�ɹ�,  false-ʧ��
		*/
		virtual std::string GetDeviceVersion() = 0;

		/*
		* @brief: ��ȡ����
		*
		* @retval: true-�ɹ�,  false-ʧ��
		*/
		virtual std::string GetDeviceDate() = 0;

		/*
		* @brief: ����豸
		* @param type[�������]:�豸����
		* @param strName[�������]:�豸���֡�ÿ���豸�и�Ψһ�����֡�
		*
		* @retval: true-�ɹ�,  false-ʧ��
		*/
		virtual bool AddDevice(DDR_IN EnDeviceType type, DDR_IN std::string strName) = 0;

		/*
		* @brief: �Ƴ��豸
		* @param type[�������]:�豸����
		* @param strName[�������]:�豸����
		*
		* @retval: true-�ɹ�,  false-ʧ��
		*/
		virtual bool RemoveDevice(DDR_IN EnDeviceType type, DDR_IN std::string strName = "") = 0;

		/*
		* @brief: �������ͻ�ȡ�豸���е��豸������ͬ�����ͣ������ж����
		* @param type[�������]:�豸����
		*
		* @retval: nullptr-ʧ�ܡ��ɹ������豸���͵�ָ�롣
		*/
		virtual DeviceTypeMap* GetPtrMap(DDR_IN EnDeviceType type) = 0;

		/*
		* @brief: �������ͺ����ֲ����豸�������ҵ���ĳ���ض����豸�ˡ�
		* @param type[�������]:�豸����
		* @param strName[�������]:�豸���֡�ÿ���豸�и�Ψһ�����֡�
		*
		* @retval: nullptr-ʧ�ܡ��ɹ������豸ָ�롣
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