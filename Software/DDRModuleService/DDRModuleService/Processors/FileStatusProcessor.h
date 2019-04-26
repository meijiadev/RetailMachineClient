#ifndef FileStatusProcessor_h__
#define FileStatusProcessor_h__


#include "src/Network/MessageSerializer.h"
#include "src/Network/BaseProcessor.h"


using namespace DDRFramework;
class FileStatusProcessor : public BaseProcessor
{
public:
	FileStatusProcessor(BaseMessageDispatcher& dispatcher);
	~FileStatusProcessor();

	virtual void Process(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override;
	virtual void AsyncProcess(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg) override;


	void ProcessAns(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg);
	void ProcessChk(std::shared_ptr<BaseSocketContainer> spSockContainer, std::shared_ptr<DDRCommProto::CommonHeader> spHeader, std::shared_ptr<google::protobuf::Message> spMsg);


	void OnGetDone(float f);
	void OnPostDone(float f);
private:

	CommonHeader_PassNode m_Passnode;
	std::shared_ptr<DDRCommProto::CommonHeader> m_spHeader;
};

#endif // FileStatusProcessor_h__

