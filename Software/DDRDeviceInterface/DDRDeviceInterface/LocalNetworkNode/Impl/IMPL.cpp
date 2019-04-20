#include "stdafx.h"

#include <vector>
#include <string>
#include <unordered_map>
#include "TCPServer.h"
#include "UDPServer.h"
#include "TCPClient.h"
#include "UDPClient.h"
#include "LocalNodeGroup.h"
#include "CommonDef/DeleteUnwrapper.h"
#include "LocalNetworkNode/LNNNodeGroup.h"

namespace DDRLNN {

void _stdcall ResizeVecBytes(void *pVecBytes, unsigned int szNew) {
	((std::vector<BYTE>*)pVecBytes)->resize((size_t)szNew);
}

void* _stdcall GetVecBytesHead(void *pVecBytes) {
	if (!pVecBytes) { return nullptr; }
	std::vector<BYTE> *pVec = (std::vector<BYTE>*)pVecBytes;
	if (pVec->size() > 0) { return (void*)(&(*pVec)[0]); }
	else { return nullptr; }
}

class NodeGroupImpl : public DDRCommonDef::DefaultDelete<LNNNodeGroupInterface> {
public:
	NodeGroupImpl() {}
	~NodeGroupImpl() { StopRunning(); }

	bool _stdcall AddTCPServer(const char *pName, BYTE authType) override {
		auto findInd = m_serverMapper.find(pName);
		if (findInd != m_serverMapper.end()) { return false; }
		ServerNode_TCP *pTCPServer = new ServerNode_TCP;
		pTCPServer->SetTCPServerNode(pName, authType);
		m_serverMapper.insert({ pName, (int)m_servers.size() });
		m_servers.emplace_back(pTCPServer);
		return true;
	}
	bool _stdcall AddUDPServer(const char *pName) override {
		auto findInd = m_serverMapper.find(pName);
		if (findInd != m_serverMapper.end()) { return false; }
		ServerNode_UDP *pUDPServer = new ServerNode_UDP;
		pUDPServer->SetUDPServerNode(pName);
		m_serverMapper.insert({ pName, (int)m_servers.size() });
		m_servers.emplace_back(pUDPServer);
		return true;
	}
	virtual int _stdcall SetServerOption(const char *pServerName,
		                                 int szSndBuf, int szRcvBuf,
										 int nMaxClients) override {
		auto findInd = m_serverMapper.find(pServerName);
		if (m_serverMapper.end() == findInd) { return -1; }
		return (m_servers[findInd->second]->SetServerOption(szSndBuf, szRcvBuf, nMaxClients)) ? 1 : 0;
	}
	int _stdcall _scb_SetArg_NewDataPublish(const char *pServerName,
		                                    void *pArgNDP) override {
		auto findInd = m_serverMapper.find(pServerName);
		if (m_serverMapper.end() == findInd) { return -1; }
		return (m_servers[findInd->second]->SetNDPArg(pArgNDP)) ? 1 : 0;
	}
	int _stdcall _scb_SetFunc_NewDataPublish(const char *pServerName,
		                                     CALLBACK_VecBytesFill pFuncNDP) override {
		auto findInd = m_serverMapper.find(pServerName);
		if (m_serverMapper.end() == findInd) { return -1; }
		return (m_servers[findInd->second]->SetNDPFunc(pFuncNDP)) ? 1 : 0;
	}
	int _stdcall _scb_SetArg_IdlingGenerator(const char *pServerName,
		                                     void *pArgIG) override {
		auto findInd = m_serverMapper.find(pServerName);
		if (m_serverMapper.end() == findInd) { return -1; }
		return (m_servers[findInd->second]->SetIGArg(pArgIG)) ? 1 : 0;
	}
	int _stdcall _scb_SetParam_IdlingGenerator(const char *pServerName,
		                                       unsigned int idlingPeriodInMs) override {
		auto findInd = m_serverMapper.find(pServerName);
		if (m_serverMapper.end() == findInd) { return -1; }
		return (m_servers[findInd->second]->SetIdlingPeriod(idlingPeriodInMs)) ? 1 : 0;
	}
	int _stdcall _scb_SetFunc_IdlingGenerator(const char *pServerName,
		                                      CALLBACK_VecBytesFill pFuncIG) override {
		auto findInd = m_serverMapper.find(pServerName);
		if (m_serverMapper.end() == findInd) { return -1; }
		return (m_servers[findInd->second]->SetIGFunc(pFuncIG)) ? 1 : 0;
	}
	int _stdcall _scb_SetArg_WAGenerator(const char *pServerName,
		                                 void *pArgWAG) override {
		auto findInd = m_serverMapper.find(pServerName);
		if (m_serverMapper.end() == findInd) { return -1; }
		return (m_servers[findInd->second]->SetWAGArg(pArgWAG)) ? 1 : 0;
	}
	int _stdcall _scb_SetFunc_WAGenerator(const char *pServerName,
		                                  CALLBACK_VecBytesFill pFuncWAG) override {
		auto findInd = m_serverMapper.find(pServerName);
		if (m_serverMapper.end() == findInd) { return -1; }
		return (m_servers[findInd->second]->SetWAGFunc(pFuncWAG)) ? 1 : 0;
	}
	int _stdcall _scb_SetArg_RAValidator(const char *pServerName,
		                                 void *pArgRAV) override {
		auto findInd = m_serverMapper.find(pServerName);
		if (m_serverMapper.end() == findInd) { return -1; }
		return (m_servers[findInd->second]->SetRAVArg(pArgRAV)) ? 1 : 0;
	}
	int _stdcall _scb_SetFunc_RAValidator(const char *pServerName,
		                                  CALLBACK_AuthValidator pFuncRAV) override {
		auto findInd = m_serverMapper.find(pServerName);
		if (m_serverMapper.end() == findInd) { return -1; }
		return (m_servers[findInd->second]->SetRAVFunc(pFuncRAV)) ? 1 : 0;
	}

	bool _stdcall AddTCPClient(const char *pName, BYTE authType) override {
		auto findInd = m_clientMapper.find(pName);
		if (findInd != m_clientMapper.end()) { return false; }
		ClientNode_TCP *pTCPClient = new ClientNode_TCP;
		pTCPClient->SetTCPClientNode(pName, authType);
		m_clientMapper.insert({ pName, (int)m_clients.size() });
		m_clients.emplace_back(pTCPClient);
		m_rcvBuf.emplace_back(std::vector<BYTE>());
		return true;
	}
	bool _stdcall AddUDPClient(const char *pName) override {
		auto findInd = m_clientMapper.find(pName);
		if (findInd != m_clientMapper.end()) { return false; }
		ClientNode_UDP *pUDPClient = new ClientNode_UDP;
		pUDPClient->SetUDPClientNode(pName);
		m_clientMapper.insert({ pName, (int)m_clients.size() });
		m_clients.emplace_back(pUDPClient);
		m_rcvBuf.emplace_back(std::vector<BYTE>());
		return true;
	}
	int _stdcall SetClientOption(const char *pClientName,
		                         int szSndBuf, int szRcvBuf) override {
		auto findInd = m_clientMapper.find(pClientName);
		if (m_clientMapper.end() == findInd) { return -1; }
		return (m_clients[findInd->second]->SetClientOption(szSndBuf, szRcvBuf)) ? 1 : 0;
	}
	int _stdcall _ccb_SetArg_onRecvData(const char *pClientName,
	                                    void *pArgORD) override {
		auto findInd = m_clientMapper.find(pClientName);
		if (m_clientMapper.end() == findInd) { return -1; }
		return (m_clients[findInd->second]->SetORDArg(pArgORD)) ? 1 : 0;
	}
	int _stdcall _ccb_SetFunc_onRecvData(const char *pClientName,
		                                 CALLBACK_BUF2PROCESS pFuncORD) {
		auto findInd = m_clientMapper.find(pClientName);
		if (m_clientMapper.end() == findInd) { return -1; }
		return (m_clients[findInd->second]->SetORDFunc(pFuncORD)) ? 1 : 0;
	}
	int _stdcall _ccb_SetArg_RAGenerator(const char *pClientName,
		                                 void *pArgRAG) override {
		auto findInd = m_clientMapper.find(pClientName);
		if (m_clientMapper.end() == findInd) { return -1; }
		return (m_clients[findInd->second]->SetRAGArg(pArgRAG)) ? 1 : 0;
	}
	int _stdcall _ccb_SetFunc_RAGenerator(const char *pClientName,
		                                  CALLBACK_VecBytesFill pFuncRAG) override {
		auto findInd = m_clientMapper.find(pClientName);
		if (m_clientMapper.end() == findInd) { return -1; }
		return (m_clients[findInd->second]->SetRAGFunc(pFuncRAG)) ? 1 : 0;
	}
	int _stdcall _ccb_SetArg_WAValidator(const char *pClientName,
		                                 void *pArgWAV) override {
		auto findInd = m_clientMapper.find(pClientName);
		if (m_clientMapper.end() == findInd) { return -1; }
		return (m_clients[findInd->second]->SetWAVArg(pArgWAV)) ? 1 : 0;
	}
	int _stdcall _ccb_SetFunc_WAValidator(const char *pClientName,
		                                  CALLBACK_AuthValidator pFuncWAV) override {
		auto findInd = m_clientMapper.find(pClientName);
		if (m_clientMapper.end() == findInd) { return -1; }
		return (m_clients[findInd->second]->SetWAVFunc(pFuncWAV)) ? 1 : 0;
	}

	bool _stdcall SetLoopTempo(int ioTime_ms, int sleepTime_ms) override {
		m_LNG.SetLoopTempo(ioTime_ms, sleepTime_ms);
		return true;
	}
	bool _stdcall StartRunning() override {
		for (int i = 0; i < (int)m_servers.size(); ++i) {
			m_LNG.AddServerNode(m_servers[i]);
		}
		for (int i = 0; i < (int)m_clients.size(); ++i) {
			m_LNG.AddClientNode(m_clients[i]);
		}
		m_LNG.FinishAddingNodes();
		return m_LNG.Launch();
	}
	bool _stdcall StopRunning() override {
		m_LNG.Stop();
		for (int i = 0; i < (int)m_servers.size(); ++i) {
			delete m_servers[i];
		}
		for (int i = 0; i < (int)m_clients.size(); ++i) {
			delete m_clients[i];
		}
		m_servers.clear();
		m_clients.clear();
		return true;
	}
	int _stdcall PublishContent(const char *pServerName,
		                        const void *pContent, int nContentLen) override {
		auto findInd = m_serverMapper.find(pServerName);
		if (m_serverMapper.end() == findInd) { return -1; }
		return (m_servers[findInd->second]->PublishContent((const BYTE*)pContent, nContentLen)) ? 1 : 0;
	}
	__int64 _stdcall GetTimeSinceLastPublication(const char *pServerName) const override {
		auto findInd = m_serverMapper.find(pServerName);
		if (m_serverMapper.end() == findInd) { return -1; }
		return (m_servers[findInd->second]->GetTimeSinceLastPublication());
	}
	int _stdcall GetData(const char *pClientName,
		                 void *pDataBytes, unsigned int nBufCapacity,
						 unsigned int *pnRemBytes) override {
		auto findInd = m_clientMapper.find(pClientName);
		if (m_clientMapper.end() == findInd) { return -1; }
		m_tmpBuf.resize(0);
		if (m_clients[findInd->second]->GetData(m_tmpBuf) &&
			m_tmpBuf.size() > 0) {
			size_t pos = m_rcvBuf[findInd->second].size();
			m_rcvBuf[findInd->second].resize(pos + m_tmpBuf.size());
			memcpy(&(m_rcvBuf[findInd->second])[pos], &m_tmpBuf[0], m_tmpBuf.size());
		}
		std::vector<BYTE> &vec = m_rcvBuf[findInd->second];
		if (vec.size() == 0) {
			if (pnRemBytes) {
				*pnRemBytes = 0;
			}
			return 0;
		} else if (vec.size() <= nBufCapacity) {
			memcpy(pDataBytes, &vec[0], vec.size());
			int sz = (int)vec.size();
			vec.resize(0);
			if (pnRemBytes) {
				*pnRemBytes = 0;
			}
			return sz;
		} else {
			if (nBufCapacity > 0) {
				memcpy(pDataBytes, &vec[0], nBufCapacity);
				memmove(&vec[0], &vec[nBufCapacity], vec.size() - nBufCapacity);
			}
			if (pnRemBytes) {
				*pnRemBytes = vec.size() - nBufCapacity;
			}
			vec.resize(vec.size() - nBufCapacity);
			return nBufCapacity;
		}
	}
	__int64 _stdcall GetTimeSinceLastReception(const char *pClientName) const override {
		auto findInd = m_clientMapper.find(pClientName);
		if (m_clientMapper.end() == findInd) { return -1; }
		return (m_clients[findInd->second]->GetTimeSinceLastContentReception());
	}
	int _stdcall IsStreamed(const char *pNodeName, bool bServer) const override {
		if (bServer) {
			auto findInd = m_serverMapper.find(pNodeName);
			if (m_serverMapper.end() == findInd) { return -1; }
			return (m_servers[findInd->second]->IsStreamed()) ? 1 : 0;
		} else {
			auto findInd = m_clientMapper.find(pNodeName);
			if (m_clientMapper.end() == findInd) { return -1; }
			return (m_clients[findInd->second]->IsStreamed()) ? 1 : 0;
		}
	}
	int _stdcall IsTCPBased(const char *pNodeName, bool bServer) const override {
		if (bServer) {
			auto findInd = m_serverMapper.find(pNodeName);
			if (m_serverMapper.end() == findInd) { return -1; }
			return (m_servers[findInd->second]->IsTCPBased()) ? 1 : 0;
		} else {
			auto findInd = m_clientMapper.find(pNodeName);
			if (m_clientMapper.end() == findInd) { return -1; }
			return (m_clients[findInd->second]->IsTCPBased()) ? 1 : 0;
		}
	}
	int _stdcall GetAuthType(const char *pNodeName, bool bServer) const override {
		if (bServer) {
			auto findInd = m_serverMapper.find(pNodeName);
			if (m_serverMapper.end() == findInd) { return -1; }
			return (int)m_servers[findInd->second]->GetAuthType();
		} else {
			auto findInd = m_clientMapper.find(pNodeName);
			if (m_clientMapper.end() == findInd) { return -1; }
			return (int)m_clients[findInd->second]->GetAuthType();
		}
	}
	int _stdcall IsServerEstablished(const char *pServerNode) const override {
		auto findInd = m_serverMapper.find(pServerNode);
		if (m_serverMapper.end() == findInd) { return -1; }
		return (m_servers[findInd->second]->IsEstablished()) ? 1 : 0;
	}
	int _stdcall GetNumOfConnections(const char *pServerNode) const override {
		auto findInd = m_serverMapper.find(pServerNode);
		if (m_serverMapper.end() == findInd) { return -1; }
		return m_servers[findInd->second]->GetNumOfConnections();
	}
	int _stdcall IsClientConnected(const char *pClientNode) const override {
		auto findInd = m_clientMapper.find(pClientNode);
		if (m_clientMapper.end() == findInd) { return -1; }
		return (m_clients[findInd->second]->IsConnected()) ? 1 : 0;
	}
	void _stdcall destroy() override {
		delete this;
	}

protected:
	std::vector<ServerNode_Base0*> m_servers;
	std::vector<ClientNode_Base0*> m_clients;
	std::unordered_map<std::string, int> m_serverMapper;
	std::unordered_map<std::string, int> m_clientMapper;
	LocalNodeGroup m_LNG;
	std::vector<BYTE> m_tmpBuf;
	std::vector<std::vector<BYTE>> m_rcvBuf;
};

LNNNodeGroupInterface* _stdcall _createLNNNodeGroup() {
	return (new NodeGroupImpl);
}

}
