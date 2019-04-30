

//����SDKͷ�ļ�
#include <DDRClientSDK.h>


#include "utils.h"






DDRSDK::DDRHANDLE gDDRHandle;
bool gQuit = false;

std::string gIP;
std::string gPort;



//״̬�仯�����ص���
class DemoStatusListener : public DDRSDK::DDRStatusListener
{
public:
	virtual void OnConnected() override 
	{
		printf("\nDemoStatusListener OnConnected");


		DDRSDK::StopReceivingBroadcast(gDDRHandle);
	}
	virtual void OnDisconnected() override
	{
		printf("\nDemoStatusListener OnDisconnected");
	}
	virtual void OnConnectTimeout() override
	{
		printf("\nDemoStatusListener OnConnectTimeout");
	}
	virtual void OnConnectFailed() override
	{
		printf("\nDemoStatusListener OnConnectFailed");
	}



	virtual void OnLoginSuccess() override
	{
		printf("\nDemoStatusListener OnLoginSuccess");

	}
	virtual void OnLoginFailed(std::string error)  override
	{
		printf("\nDemoStatusListener OnLoginFailed");

	}
};

//�㲥��Ϣ�������࣬�յ����豸���ڵĹ㲥��Ϣ����ص�OnBroadcastArrival
class DemoBroadcastReceiver : public DDRSDK::DDRBroadcastReceiver
{
	virtual void OnBroadcastArrival(std::string name, std::string ip, std::string port) override
	{
		printf("\nDDRSDK Demo OnBroadcastArrival %s %s:%s", name.c_str(), ip.c_str(), port.c_str());

		gIP = ip;
		gPort = port;

		//todo �յ��豸�㲥����¼�����д��ڵ��豸��ʵ�־����߼�
	}
};



//�ƶ���Ϣ�������࣬�յ��ذ����ص��� OnMsgArrival
class DemoRobotVersionInfoListener : public DDRSDK::DDRBaseListener
{
	virtual void OnMsgArrival(std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override
	{
		auto spRsp = std::dynamic_pointer_cast<DDRModuleProto::rspRobotVersionInfo>(spMsg);
		if (spRsp)
		{
			printf("\nDemo RobotVersionInfo Listener RobotID:%s", spRsp->robotid().c_str());
			printf("\nDemo RobotVersionInfo Listener Hardware Version:%s", spRsp->hardwareversion().c_str());
		}
	}
};

class DemoSetParameterListener : public DDRSDK::DDRBaseListener
{
	virtual void OnMsgArrival(std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override
	{
		auto spRsp = std::dynamic_pointer_cast<DDRModuleProto::rspSetParameter>(spMsg);
		if (spRsp)
		{
			printf("\nDemo SetParameter Listener:%i", spRsp->ret());
		}
	}
};


//���ӵ��豸���յ��㲥�豸��Ϣ�����ӵ�ָ���豸IP,�˿ڣ����ӳɹ��󣬻�ص�DemoStatusListener��OnConnected
void Connect()
{
	DDRSDK::StartCommunication(gDDRHandle, gIP, gPort,DDRSDK::eClietType::ECT_PC,"admin","admin");
}

//�Ͽ��豸����
void Disconnect()
{
	DDRSDK::StopCommunication(gDDRHandle);
}

//����ָ�������reqRobotVersionInfoΪ����ָ�����proto�ļ�
void RobotVersionInfo()
{
	auto spReq = std::make_shared<DDRModuleProto::reqRobotVersionInfo>();
	DDRSDK::Send(gDDRHandle, spReq);
}

void SetParameter()
{
	auto spReq = std::make_shared<DDRModuleProto::reqSetParameter>();
	spReq->set_posangulauspeed(12);
	spReq->set_poslinespeed(99);
	DDRSDK::Send(gDDRHandle, spReq);
}

int main()
{

	//����SDKʵ��������ʵ���ľ��������һ��״̬�ص������࣬��Ϊ�������롣
	//״̬�ص������࣬����������豸ͨѶ��״̬�仯
	gDDRHandle = DDRSDK::CreateDDR(std::make_shared<DemoStatusListener>());

	//
	DDRSDK::RegisterListener(gDDRHandle, std::make_shared<DDRModuleProto::rspRobotVersionInfo>(), std::make_shared<DemoRobotVersionInfoListener>());
	DDRSDK::RegisterListener(gDDRHandle, std::make_shared<DDRModuleProto::rspSetParameter>(), std::make_shared<DemoSetParameterListener>());


	//�����㲥������
	auto spReceiver = std::make_shared<DemoBroadcastReceiver>();
	//��ʼ�����㲥
	DDRSDK::StartReceivingBroadcast(gDDRHandle, spReceiver);





	//�˴�����Ϣѭ��������ĳ�����ʹ�����Լ��ķ�����������Ӧ�����豸(������꼰��������),ִ����Ӧ����
	//����quit�˳�����
	ConsoleUtility consoleUtility;
	consoleUtility.AddCommand("connect", std::bind(Connect));//����������connectִ��Connect��������ͬ
	consoleUtility.AddCommand("disconnect", std::bind(Disconnect));
	consoleUtility.AddCommand("reqRobotVersionInfo", std::bind(RobotVersionInfo));
	consoleUtility.AddCommand("reqSetParameter", std::bind(SetParameter));
	consoleUtility.ConsoleLoop();



	//�ͷ�SDKʵ��
	DDRSDK::ReleaseDDR(gDDRHandle);

	return 0;
}