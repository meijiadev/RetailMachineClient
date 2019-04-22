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
		* @brief: ����һ���״һ���ȡ�״�����֮ǰҪ�������״�
		* @param strLidarIP[�������]:Ҫ���ӵ��״�IP������:192.168.0.83
		* @param nLidarID[�������]:�����״�ID
		*
		* @retval: true-�ɹ�,  false-ʧ��
		*/
		virtual bool AddOneLidar(DDR_IN char* strLidarIP, DDR_OUT int *nLidarID) = 0;

		/**
		* @brief: ��Ƕ��ʽ�·��ٶ�
		* @param fSpeedA[�������]:���ٶ�
		* @param fSpeedL[�������]:���ٶ�
		*
		* @retval: true-�ɹ�,  false-ʧ��
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