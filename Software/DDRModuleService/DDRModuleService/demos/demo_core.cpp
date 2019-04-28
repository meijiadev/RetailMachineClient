#include <iostream>
#include "DataFormat.h"
#include "CoreNavigator.h"

int main()
{
	DDRData::AngularVelocity av;
	DDRData::LinearVelocity lv;
	DDRData::LidarScan_2D lidarScan;
	DDRData::StereoImages stereoImg;
	DDRData::Pose_d robotPose;
	DDRNavigator::CoreNavigator cn;

	DDRData::Pose_d p2;
	p2.orie.yaw = 0.3;
	p2.orie.pitch = 0.22;
	p2.orie.roll = -0.1;
	p2.pos.x = 0.1;
	p2.pos.y = -0.3;
	p2.pos.z = 0.5;
	DDRData::Pose p1 = p2;
	p1 = p2;

	while (1) {
		if (0/*�û�Ҫ���˳�*/) {
			break;
		}

		if (0/*!DeviceManager::GetGyro(&av)*/ &&
			0/*!DeviceManager::GetWheel(&lv)*/) {
			//Sleep(1);
			continue;
		}

		// ��õ�ǰ����ģʽ
		auto mode = cn.GetMode();
		auto subMode = cn.GetSubMode();

		lidarScan.Validate(false);
		/*DeviceManager::GetLidarScan(&lidarScan);*/

		// ����ϴ�˫Ŀ�������, ��Update_Start()��Update_Finish()��֮ǰ����
		// EndStereoCap()��ȷ֪ͨ�Զ�ģʽ/�ɼ�ģʽ
		stereoImg.Validate(false);
		int lastCapStat = 0;/*DeviceManager::GetLastStereoCapStatus();*/
		if (0/*SUCCESSFUL == lastCapStat*/) {
			if (0/*DeviceManager::GetStereoImages(&stereoImg);*/) {
				if (DDRNavigator::MODE::AUTO == mode) {
					cn.AUTO_EndStereoCap(true);
				} else if (DDRNavigator::MODE::COLLECTION == mode) {
					cn.COLLECTION_EndStereoCap(true);
				}
			}
		} else if (0/*FAILURE == lastCapStat*/) {
			if (DDRNavigator::MODE::AUTO == mode) {
				cn.AUTO_EndStereoCap(false);
			} else if (DDRNavigator::MODE::COLLECTION == mode) {
				cn.COLLECTION_EndStereoCap(false);
			}
		} else if (0/*STILLBUSY == lastCapStat*/) {
			; // do nothing
		}

		if (0/*�û�Ҫ�����ɼ�ģʽ*/) {
			/* ����DeviceManager���Լ졢�Ա궨�Ȼ������� */
			std::string routeName = std::string(); //�û�Ҫ����µ�ͼ��
			cn.COLLECTION_Start(routeName);
		}

		if (0/*�û�Ҫ������Զ�ģʽ*/) {
			/* ����DeviceManager���Լ졢�Ա궨�Ȼ������� */
			std::string routeName = std::string(); //�û�Ҫ������ߵ�ͼ��
			std::string pathFileName = std::string(); //�û�Ҫ�������·���ļ���
			if (0/*!isRouteNotExisting(str)*/) {
				cn.AUTO_Start(true, routeName, pathFileName);
			}
		}

		if (0/*�û�Ҫ������ɼ�ģʽ*/) {
			cn.COLLECTION_Finish();
		}

		// ���µ�ǰ����ģʽ
		mode = cn.GetMode();
		subMode = cn.GetSubMode();

		if (DDRNavigator::MODE::AUTO == mode &&
			DDRNavigator::SUBMODE::AUTO_EXCEPTION == subMode &&
			DDRNavigator::MODEEXCEPTIONS::AUTO_RELOCFAILED == cn.GetLastError()) {
			std::cout << "����! �Զ�ģʽ�ض�λʧ��!" << std::endl;
			cn.ClearExceptions();
		}
		if (DDRNavigator::MODE::COLLECTION == mode &&
			DDRNavigator::SUBMODE::COLLECTION_EXCEPTION == subMode &&
			DDRNavigator::MODEEXCEPTIONS::COLLECTION_EXISTINGROUTE == cn.GetLastError()) {
			std::cout << "����! ��ͼ����ɼ�ģʽʧ��(ͬ����ͼ�Ѵ���)!" << std::endl;
			cn.ClearExceptions();
		}
		if (DDRNavigator::MODE::AUTO == mode &&
			DDRNavigator::SUBMODE::AUTO_EXCEPTION == subMode &&
			DDRNavigator::MODEEXCEPTIONS::AUTO_NONEXISTINGROUTE == cn.GetLastError()) {
			std::cout << "����! ��ͼ�����Զ�ģʽʧ��(��ͼ��·��������)!" << std::endl;
			cn.ClearExceptions();
		}
		if (DDRNavigator::MODE::COLLECTION == mode) {
			if (DDRNavigator::SUBMODE::COLLECTION_GENERATING == subMode) {
				std::cout << "��ʾ: �ɼ�ֹͣ, ���ڽ�����ͼ..." << std::endl;
			} else if (DDRNavigator::SUBMODE::COLLECTION_FINISHED == subMode) {
				std::cout << "��ʾ: �ɼ���ͼ���, ���ش���ģʽ..." << std::endl;
				cn.COLLECTION_Back2Idling();
			}
		}
		if (DDRNavigator::MODE::AUTO == mode) {
			if (0/*�û�Ҫ������Զ�����*/) {
				/*DeviceManager::SendControl()���ٶ�ָ��*/
				std::cout << "��ʾ: �û�Ҫ������Զ�����, ����ֹͣ..." << std::endl;
				cn.AUTO_Back2Idling();
			} else if (DDRNavigator::SUBMODE::AUTO_FINISHED == subMode) {
				std::cout << "��ʾ: ����Զ�����ȫ��·��, ����ֹͣ..." << std::endl;
				cn.AUTO_Back2Idling();
			} else if (DDRNavigator::SUBMODE::AUTO_ENDPOINT_UNREACHABLE == subMode) {
				std::cout << "����: �Զ������յ��޷��ִ�, ����!" << std::endl;
				cn.COLLECTION_Back2Idling();
			}
		}
		
		// ͨ�����������ݸ��»�����λ�˺�״̬
		cn.Update_Start();
		do {
			cn.FeedSensorData(av);
			cn.FeedSensorData(lv);
		} while (0/*DeviceManager::GetGyro(&av)*/ &&
			     0/*DeviceManager::GetWheel(&lv)*/);
		cn.FeedSensorData(lidarScan);
		cn.FeedSensorData(stereoImg);
		cn.Update_Finish();

		DDRData::Pose_d pose;
		cn.GetCurrentPose(pose);
		if (DDRNavigator::MODE::AUTO == mode) {
			DDRData::AngularVelocity avCmd;
			DDRData::LinearVelocity lvCmd;
			cn.AUTO_GetControl(lvCmd, avCmd);
			/*DeviceManager::SendControl(lvCmd, avCmd);*/
			DDRData::U64_TIMESTAMP t;
			cn.AUTO_GetTimeElapsedSinceLastPC(t);
			std::cout << "��ǰ�����Զ�ģʽ, λ��(X="
				<< pose.pos.x << ", Y="
				<< pose.pos.y << ", th="
				<< pose.orie.yaw * 180 / 3.14159265359
				<< "), �ϴ���������" << t
				<< "����..." << std::endl;
			std::vector<DDRData::Position> lidarPts, visualPts;
			if (cn.AUTO_GetMatchedLidarPts(lidarPts)) {
				/*����ƥ��ɹ��ļ������*/
			}
			if (cn.AUTO_GetMatchedVisualFeatures(visualPts)) {
				/*����ƥ��ɹ����Ӿ���������*/
			}
		}

		// ���Զ�ģʽ/�ɼ�ģʽ��, ˫Ŀ���������������������״̬(�����ڵ�
		// ���ع�). ÿ�ɹ���ʼһ�β���ʱ, ֪ͨ�Զ�ģʽ/�ɼ�ģʽ�������ж�
		// Ӧ�߼�.
		if ((DDRNavigator::MODE::AUTO == mode ||
			DDRNavigator::MODE::COLLECTION == mode) &&
			0/*!DeviceManger::IsStereoCamBusy()*/) {
			if (0/*DeviceManager::StartStereoCamCapture()*/) {
				if (DDRNavigator::MODE::AUTO == mode) {
					cn.AUTO_BeginStereoCap();
				} else {
					cn.COLLECTION_BeginStereoCap();
				}
			}
		}
	}

	return 0;
}