/**************************************************************************

Copyright:Dadao Robot

Author: Michael

Date:2019-04-24

Description:Provide  DDRClient SDK functions  

**************************************************************************/

#ifndef DDRClientSDK_h__
#define DDRClientSDK_h__

#include <proto/BaseCmd.pb.h>
#include <proto/DDRModuleCmd.pb.h>

#if !defined(USE_DDRSDK_DLL) // inside DLL
#   define DDRSDK_API   __declspec(dllexport)
#else // outside DLL
#   define DDRSDK_API   __declspec(dllimport)
#endif 


namespace DDRSDK
{
	typedef std::string DDRHANDLE;


	//class to receive broadcast
	DDRSDK_API class DDRStatusListener : public std::enable_shared_from_this<DDRStatusListener>
	{
	public:
		
		virtual void OnConnected() {};
		virtual void OnDisconnected() {};
		virtual void OnConnectTimeout() {};
		virtual void OnConnectFailed() {};
		
		virtual void OnLoginSuccess() {};
		virtual void OnLoginFailed(std::string error) {};
	};




	//class to receive broadcast
	DDRSDK_API class DDRBroadcastReceiver : public std::enable_shared_from_this<DDRBroadcastReceiver>
	{
	public:
		virtual void OnBroadcastArrival(std::string name, std::string ip, std::string port) {};


	};

	//class to listen DDR message
	DDRSDK_API class DDRBaseListener : public std::enable_shared_from_this<DDRBaseListener>
	{
	public:
		virtual void OnMsgArrival(std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) {};


	};


	//create ddr handle
	DDRSDK_API DDRHANDLE CreateDDR(std::shared_ptr<DDRStatusListener> spListener);

	//release ddr handle
	DDRSDK_API void ReleaseDDR(DDRHANDLE h);

	//start receving broadcast from devices
	DDRSDK_API void StartReceivingBroadcast(DDRHANDLE h, std::shared_ptr<DDRBroadcastReceiver> spReceiver, int port = 28888);

	//stop receving broadcast from devices
	DDRSDK_API void StopReceivingBroadcast(DDRHANDLE h);


	//registe a listener to receive a proto type msg
	//spMsg : a proto msg ,it's type will be registed,and listener class will receive such type msg
	//listener : class to receive msg responsed
	DDRSDK_API bool RegisterListener(DDRHANDLE h, std::shared_ptr<google::protobuf::Message> spMsg, std::shared_ptr<DDRBaseListener> listener);

	//remove a msg type listener 
	DDRSDK_API bool UnregisterListener(DDRHANDLE h, std::shared_ptr<google::protobuf::Message> spMsg);



	DDRSDK_API enum eClietType
	{
		ECT_PC,
		ECT_ANDROID,
		ECT_LOCALSERVICE,
	};
	//connect device by ip and port to start communication with robot
	DDRSDK_API bool StartCommunication(DDRHANDLE h,std::string ip, std::string port, eClietType astype = eClietType::ECT_LOCALSERVICE,std::string username = "" ,std::string password = "");

	//stop communication
	DDRSDK_API bool StopCommunication(DDRHANDLE h);

	//send msg to robot connected
	DDRSDK_API void Send(DDRHANDLE h, std::shared_ptr<google::protobuf::Message> spMsg);



}





#endif // DDRClientSDK_h__
