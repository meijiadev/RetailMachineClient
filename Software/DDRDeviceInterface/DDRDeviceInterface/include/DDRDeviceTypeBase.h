#ifndef __DDR_DEVICE_TYPE_BASE_H__
#define __DDR_DEVICE_TYPE_BASE_H__

#include "DDRDeviceCommData.h"
#include <memory>
#include <map>

namespace DDRDevice
{
	/*
		�豸�ӿ��ࡣ
		T�����豸���ص��������͡�
		U�����豸��һЩ��Ϣ���������֡��״�ip������������ȡ�
	*/
	template <class T, class U>
	class DeviceTypeBase : public T
	{
	public:

		/*
		* @brief: ��ʼ���豸
		* @param U[�������]:�豸��Ϣ
		*
		* @retval: true-�ɹ�,  false-ʧ��
		*/
		virtual bool Init(U) = 0;

		/*
		* @brief: �ͷ��豸
		*
		* @retval: true-�ɹ�,  false-ʧ��
		*/
		virtual bool DeInit() = 0;

		/*
		* @brief: ��ȡ�豸����
		*
		* @retval: nullptr-ʧ�ܡ��ɹ����ؾ�������ݡ�
		*/
		virtual std::shared_ptr<T> GetData() = 0;

		/*
		* @brief: ��ȡ�豸����
		*
		* @retval: �豸����
		*/
		virtual std::string GetName() = 0;

		/*
		* @brief: ��ȡ�豸��Ϣ������Init����ʱ����Ϣ��
		*
		* @retval: �豸��Ϣ
		*/
		virtual U GetDeviceInfo() = 0;

		/*
		* @brief: ���豸�·�����
		* @param T[�������]:Ҫ�·������ݡ�
		*
		* @retval: true-�ɹ�,  false-ʧ��
		*/
		virtual bool SendData(T) = 0;
	};

	class DevicePtrContainer
	{
	};

	/*
		�豸���͡����е��豸�����ж����������������map��
		ǰ�������֣�������Ǿ���ĳ���豸�ˡ�
	*/
	typedef std::map<std::string, std::shared_ptr<DevicePtrContainer>> DeviceTypeMap;
}

#endif //  __DDR_DEVICE_DATA_BASE_H__

