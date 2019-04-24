
//#include <DDRClientSDK.h>

#include "proto/BaseCmd.pb.h"


//
//DDRSDK::DDRHANDLE gDDRHandle;
//bool gQuit = false;
//
//class DemoBroadcastReceiver : public DDRSDK::DDRBroadcastReceiver
//{
//	virtual void OnBroadcastArrival(std::string name, std::string ip, std::string port) override
//	{
//		printf("DDRSDK Demo OnBroadcastArrival %s %s:%s", name.c_str(), ip.c_str(), port.c_str());
//	}
//};


int main()
{
	DDRCommProto::reqLogin req;
	//gDDRHandle = DDRSDK::CreateDDR();

	/*auto spReceiver = std::make_shared<DemoBroadcastReceiver>();
	DDRSDK::StartReceivingBroadcast(gDDRHandle, spReceiver->shared_ptr_base());


	

	while (!gQuit)
	{
		std::this_thread::sleep_for(std::chrono::seconds(100));
	}

	DDRSDK::ReleaseDDR(gDDRHandle);*/

	return 0;
}