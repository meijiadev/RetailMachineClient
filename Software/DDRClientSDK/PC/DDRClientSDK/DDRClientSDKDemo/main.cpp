

//引用SDK头文件
#include <DDRClientSDK.h>


#include "utils.h"






DDRSDK::DDRHANDLE gDDRHandle;
bool gQuit = false;

std::string gIP;
std::string gPort;



//状态变化监听回调类
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

//广播消息接收器类，收到有设备存在的广播消息，会回调OnBroadcastArrival
class DemoBroadcastReceiver : public DDRSDK::DDRBroadcastReceiver
{
	virtual void OnBroadcastArrival(std::string name, std::string ip, std::string port) override
	{
		printf("\nDDRSDK Demo OnBroadcastArrival %s %s:%s", name.c_str(), ip.c_str(), port.c_str());

		gIP = ip;
		gPort = port;

		//todo 收到设备广播，记录网络中存在的设备，实现具体逻辑
	}
};



//移动消息监听器类，收到回包后会回调到 OnMsgArrival
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


//连接到设备，收到广播设备信息后，连接到指定设备IP,端口，连接成功后，会回调DemoStatusListener的OnConnected
void Connect()
{
	DDRSDK::StartCommunication(gDDRHandle, gIP, gPort,DDRSDK::eClietType::ECT_PC,"admin","admin");
}

//断开设备连接
void Disconnect()
{
	DDRSDK::StopCommunication(gDDRHandle);
}

//发送指令。这里以reqRobotVersionInfo为例。指令详见proto文件
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

	//创建SDK实例，返回实例的句柄，创建一个状态回调监听类，作为参数输入。
	//状态回调监听类，负责监听与设备通讯的状态变化
	gDDRHandle = DDRSDK::CreateDDR(std::make_shared<DemoStatusListener>());

	//
	DDRSDK::RegisterListener(gDDRHandle, std::make_shared<DDRModuleProto::rspRobotVersionInfo>(), std::make_shared<DemoRobotVersionInfoListener>());
	DDRSDK::RegisterListener(gDDRHandle, std::make_shared<DDRModuleProto::rspSetParameter>(), std::make_shared<DemoSetParameterListener>());


	//创建广播接收器
	auto spReceiver = std::make_shared<DemoBroadcastReceiver>();
	//开始监听广播
	DDRSDK::StartReceivingBroadcast(gDDRHandle, spReceiver);





	//此处是消息循环，在你的程序里使用你自己的方案，用来响应输入设备(键盘鼠标及其它输入),执行相应方法
	//输入quit退出程序
	ConsoleUtility consoleUtility;
	consoleUtility.AddCommand("connect", std::bind(Connect));//命令行输入connect执行Connect方法，下同
	consoleUtility.AddCommand("disconnect", std::bind(Disconnect));
	consoleUtility.AddCommand("reqRobotVersionInfo", std::bind(RobotVersionInfo));
	consoleUtility.AddCommand("reqSetParameter", std::bind(SetParameter));
	consoleUtility.ConsoleLoop();



	//释放SDK实例
	DDRSDK::ReleaseDDR(gDDRHandle);

	return 0;
}