

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



//ע����Ϣ�������࣬�յ��ذ����ص��� OnMsgArrival
class DemoLoginListener : public DDRSDK::DDRBaseListener
{
	virtual void OnMsgArrival(std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override
	{
		auto spRsp = std::dynamic_pointer_cast<DDRCommProto::rspLogin>(spMsg);
		if (spRsp)
		{
			printf("\nDemo Login Listener:%i", spRsp->yourrole());

			//to do ������д�յ���¼��Ϣ��ľ����߼�
		}
	}
};

//�ƶ���Ϣ�������࣬�յ��ذ����ص��� OnMsgArrival
class DemoCmdMoveListener : public DDRSDK::DDRBaseListener
{
	virtual void OnMsgArrival(std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override
	{
		auto spRsp = std::dynamic_pointer_cast<DDRCommProto::rspCmdMove>(spMsg);
		if (spRsp)
		{
			printf("\nDemo CmdMove Listener:%i", spRsp->type());
		}
	}
};


//���ӵ��豸���յ��㲥�豸��Ϣ�����ӵ�ָ���豸IP,�˿ڣ����ӳɹ��󣬻�ص�DemoStatusListener��OnConnected
void Connect()
{
	DDRSDK::StartCommunication(gDDRHandle, gIP, gPort);
}

//�Ͽ��豸����
void Disconnect()
{
	DDRSDK::StopCommunication(gDDRHandle);
}

//��¼�豸
void Login()
{
	auto spReq = std::make_shared<DDRCommProto::reqLogin>();
	spReq->set_username("admin");
	spReq->set_userpwd("admin");;
	spReq->set_type(DDRCommProto::eCltType::eLocalPCClient);
	DDRSDK::Send(gDDRHandle,spReq);
}

//����ָ�������reqCmdMoveΪ����ָ�����proto�ļ�
void CmdMove()
{
	auto spReq = std::make_shared<DDRCommProto::reqCmdMove>();
	spReq->set_line_speed(123);
	spReq->set_angulau_speed(456);
	DDRSDK::Send(gDDRHandle, spReq);
}

int main()
{

	//����SDKʵ��������ʵ���ľ��������һ��״̬�ص������࣬��Ϊ�������롣
	//״̬�ص������࣬����������豸ͨѶ��״̬�仯
	gDDRHandle = DDRSDK::CreateDDR(std::make_shared<DemoStatusListener>());

	//ע����Ӧ��Ϣ�ļ�����
	DDRSDK::RegisterListener(gDDRHandle, std::make_shared<DDRCommProto::rspLogin>(), std::make_shared<DemoLoginListener>());
	DDRSDK::RegisterListener(gDDRHandle, std::make_shared<DDRCommProto::rspCmdMove>(), std::make_shared<DemoCmdMoveListener>());


	//�����㲥������
	auto spReceiver = std::make_shared<DemoBroadcastReceiver>();
	//��ʼ�����㲥
	DDRSDK::StartReceivingBroadcast(gDDRHandle, spReceiver);





	//�˴�����Ϣѭ��������ĳ�����ʹ�����Լ��ķ�����������Ӧ�����豸(������꼰��������),ִ����Ӧ����
	//����quit�˳�����
	ConsoleUtility consoleUtility;
	consoleUtility.AddCommand("connect", std::bind(Connect));//����������connectִ��Connect��������ͬ
	consoleUtility.AddCommand("disconnect", std::bind(Disconnect));
	consoleUtility.AddCommand("login", std::bind(Login));
	consoleUtility.AddCommand("move", std::bind(CmdMove));
	consoleUtility.ConsoleLoop();



	//�ͷ�SDKʵ��
	DDRSDK::ReleaseDDR(gDDRHandle);

	return 0;
}