#ifndef __DDR_FUSION_NAVIGATION_CORE_SLAM_FUNC_H_INCLUDED__
#define __DDR_FUSION_NAVIGATION_CORE_SLAM_FUNC_H_INCLUDED__

#include <string>
#include "DataFormat.h"

namespace DDRNavigator {

// ��ȡ�汾��
int GetCurrentVersionNo();

// ��ȡ�汾����
std::string GetCurrentVersionDescription();

//��ģʽ
enum class MODE {
	IDLING, // ����ģʽ
	COLLECTION, // �ɼ���ͼģʽ
	AUTO // �Զ�����ģʽ
};

//�μ�ģʽ
enum class SUBMODE {
	IDLING_EXCEPTION, // ����ģʽ�쳣
	COLLECTION_EXCEPTION, // �ɼ�ģʽ�쳣
	AUTO_EXCEPTION, // �Զ�ģʽ�쳣

	IDLING_STILL, // ����ģʽ-��ֹ
	IDLING_MOVING, // ����ģʽ-�˶�

	COLLECTION_COLLECTING, // �ɼ�ģʽ-�ɼ���
	COLLECTION_GENERATING, // �ɼ�ģʽ-��ͼ������
	COLLECTION_FINISHED, // �ɼ�ģʽ-��ͼ���

	AUTO_RELOC, // �Զ�ģʽ-�ض�λ��
	AUTO_MOVING, // �Զ�ģʽ-�˶���
	AUTO_STOPPED, // �Զ�ģʽ-ֹͣ(�ɵ����߻����ģ��Ҫ��ֹͣ)
	AUTO_ENDPOINT_UNREACHABLE, // �Զ�ģʽ-·���յ��޷�����
	AUTO_FINISHED // �Զ�ģʽ-�ɹ������յ�
};

// �Զ��˶�״̬
enum class AUTOMOVINGSTATUS {
	STATIC_NORMAL, // ��̬����-��������
	STATIC_SLOWDOWN, // ��̬����-���ٱ���
	STATIC_STOPPED, // ��̬����-ֹͣ
	DYNAMIC_NORMAL, // ��̬����-��������
	DYNAMIC_DETOUR, // ��̬����-����
	DYNAMIC_NOWAYOUT, // ��̬����-Χ��
	DYNAMIC_VIRTUAL_COLLISION, // ��̬����-���������ϰ�
	PP_TURNING, // ԭ����ת
	PP_TEMP_SEGMENT // ִ����ʱ·��(�˹��̾�̬����)
};

// ��ģʽ�µ��쳣״̬
enum class MODEEXCEPTIONS {
	COLLECTION_EXISTINGROUTE, // ��ͼ�ɼ��ĵ�ͼ���Ѵ���
	COLLECTION_FILEIOERROR, // �ɼ�ģʽ���ļ�IO����
	AUTO_NONEXISTINGROUTE, // �Զ�ģʽѡ��ĵ�ͼ������
	AUTO_RELOCFAILED, // �Զ�ģʽ�ض�λʧ��
	AUTO_FILEIOERROR, // �Զ�ģʽ���ļ�IO����
	AUTO_PP_EXCEPTION, // �Զ�ģʽ��·���滮ģ���쳣
	AUTO_OA_EXCEPTION // �Զ�ģʽ�±���ģ���쳣
};

class CoreNavigator
{
public:
	CoreNavigator();
	virtual ~CoreNavigator();
	
	// ��ȡ��ǰģʽ
	MODE GetMode() const;
	// ��ȡ��ǰ�μ�ģʽ
	SUBMODE GetSubMode() const;
	// ��ȡ��һ���쳣
	MODEEXCEPTIONS GetLastError() const;
	// �����쳣״̬���ص�����ģʽ
	void ClearExceptions();
	// ��ȡ��ǰλ��
	bool GetCurrentPose(DDRData::Pose&) const;
	bool GetCurrentPose(DDRData::Pose_d&) const;

	/*
	���»������˶�״̬��λ��
			1. Update_start()
			2. FeedSensorData() (ʹ�ø�������ι�벻ͬ���ʹ���������)
			3. Update_finish()
	����: ���ҽ���һ��Update_Start()��Update_finish()֮��(�³�"�˿ռ�")
	���԰�ȫ����FeedSensorData(). ����ȡģʽ��λ�ˡ���ȡ����ָ���ʼ˫
	Ŀ����Ȳ������ڴ˿ռ�֮����Ч, �ڴ˿ռ��ڵ��ÿ�������쳣�������.
	*/
	bool Update_Start();
	bool Update_Finish();
	// ι����ٶ�����(ͨ���������ǻ�ȡ)
	bool FeedSensorData(const DDRData::AngularVelocity&);
	// ι����ٶ�����(ͨ�����ֻ���ȡ)
	bool FeedSensorData(const DDRData::LinearVelocity&);
	// ι��2D����(ͨ����ƽ�漤���״��ȡ)
	bool FeedSensorData(const DDRData::LidarScan_2D&);
	// ι��˫Ŀͼ��(ͨ����˫Ŀ�����ȡ)
	bool FeedSensorData(const DDRData::StereoImages&);

	// �����ض�ģʽ����ͳһ����ֵΪbool��, ��ʾ�˴ε����Ƿ���Ч

	// ========================= collection mode ========================= 
	// ��ʼ�ɼ�ģʽ, routeNameΪ��Ҫ����ĵ�ͼ����
	bool COLLECTION_Start(const std::string &routeName);
	// ֪ͨ�ɼ�ģʽ�ոտ�ʼ��һ��˫Ŀͼ��׽
	bool COLLECTION_BeginStereoCap();
	// ֪ͨ�ɼ�ģʽ��ǰ�ִε�˫Ŀ��׽���, bSuccess��ʾ�Ƿ�ɹ�
	bool COLLECTION_EndStereoCap(bool bSuccess);
	// ��ȡ������״����(��������)��������vector��
	bool COLLECTION_GetLidarPts(std::vector<DDRData::Position>&);
	// ��ȡ������Ӿ�����(��������)��������vector��
	bool COLLECTION_GetVisualFeatures(std::vector<DDRData::Position>&);
	// �����ɼ�ģʽ. �ɼ�ģʽ�������ͼ���ɽ׶�(SUBMODE::COLLECTION_GENERATING),
	// ��ɺ����SUBMODE::COLLECTION_FINISHED
	bool COLLECTION_Finish();
	// �ɼ�ģʽ��SUBMODE::COLLECTION_FINISHED��ʾ�ɼ���ͼ��ȫ�����, ���ش���ģʽ
	bool COLLECTION_Back2Idling();
	// ========================= collection mode ========================= 

	// ============================== auto mode ============================== 
	// ��ʼ�Զ�ģʽ, bDynamicOA�Ƿ�Ϊ��̬����ģʽ(����Ϊ��̬����),
	// routeNameΪҪ����ĵ�ͼ����, pathFileNameΪҪ���е�·���ļ���
	bool AUTO_Start(bool bDynamicOA, const std::string &routeName,
		            const std::string &pathFileName);
	// �л���̬/��̬����ģʽ
	bool AUTO_SwitchOAMode(bool bDynamicOA);
	// ֪ͨ�Զ�ģʽ�ոտ�ʼ��һ��˫Ŀͼ��׽
	bool AUTO_BeginStereoCap();
	// ֪ͨ�Զ�ģʽ��ǰ�ִε�˫Ŀ��׽���, bSuccess��ʾ�Ƿ�ɹ�
	bool AUTO_EndStereoCap(bool bSuccess);
	// ��ȡ�Զ�ģʽ�µ��Զ��˶�״̬����������
	bool AUTO_GetMovingStatus(AUTOMOVINGSTATUS&);
	// ��ȡ�Զ�ģʽ�µĿ���ָ��(�������ٶ�ָ��ͽ��ٶ�ָ��)
	bool AUTO_GetControl(DDRData::LinearVelocity&,
		                 DDRData::AngularVelocity&);
	// ��ȡ���ƥ��ɹ����״����(��������)��������vector��
	bool AUTO_GetMatchedLidarPts(std::vector<DDRData::Position>&);
	// ��ȡ���ƥ��ɹ����Ӿ�����(��������)��������vector��
	bool AUTO_GetMatchedVisualFeatures(std::vector<DDRData::Position>&);
	// ������ϴ�λ������(������Ӿ�)�����ʱ��, ��λ����
	bool AUTO_GetTimeElapsedSinceLastPC(DDRData::U64_TIMESTAMP&);
	// �����Զ�ģʽ. ���쳣�����дμ�ģʽ�¶�����ֱ�ӽ����Զ�ģʽ���ش���ģʽ
	bool AUTO_Back2Idling();
	// ============================== auto mode ============================== 

private:
	struct IMPL;
	IMPL *m_pImp;
};

}

#endif // __DDR_FUSION_NAVIGATION_CORE_SLAM_FUNC_H_INCLUDED__

