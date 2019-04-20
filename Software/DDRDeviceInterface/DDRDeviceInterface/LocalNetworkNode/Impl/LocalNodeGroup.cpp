#include "stdafx.h"

#include "LocalNodeGroup.h"

#include <string>
#include <unordered_map>
#include <thread>
#include <iostream>
#include "MIMC.h"

namespace DDRLNN {

LocalNodeGroup::LocalNodeGroup()
	: m_nState((BYTE)0), m_bSubThreadStarted(false) {
	SetLoopTempo(5, 1);
}

void LocalNodeGroup::SetLoopTempo(int ioTime_ms, int sleepTime_ms) {
	if (ioTime_ms <= 0) {
		ioTime_ms = 1;
	} else if (ioTime_ms > 1000) {
		ioTime_ms = 1000;
	}
	if (sleepTime_ms <= 0) {
		sleepTime_ms = 1;
	} else if (sleepTime_ms > 1000) {
		sleepTime_ms = 1000;
	}
	m_ioTV = { ioTime_ms / 1000, (ioTime_ms % 1000) * 1000 };
	m_sleepTimeMs = sleepTime_ms;
}

bool LocalNodeGroup::AddServerNode(ServerNode_Base0 *pRawServer) {
	if (m_nState != (BYTE)0 &&
		m_nState != (BYTE)1) {
		return false;
	}
	for (int i = 0; i < (int)m_rawServerNodes.size(); ++i) {
		if (strcmp(m_rawServerNodes[i]->GetName(), pRawServer->GetName()) == 0) {
			std::cout << "´íÎó@LocalNodeGroup::AddServerNode() : ½ÚµãÃû³Æ³åÍ», Ìí¼ÓÊ§°Ü, Çë¼ì²é...\n";
			return false;
		}
	}
	for (int i = 0; i < (int)m_rawClientNodes.size(); ++i) {
		if (strcmp(m_rawClientNodes[i]->GetName(), pRawServer->GetName()) == 0) {
			std::cout << "´íÎó@LocalNodeGroup::AddServerNode() : ½ÚµãÃû³Æ³åÍ», Ìí¼ÓÊ§°Ü, Çë¼ì²é...\n";
			return false;
		}
	}

	m_rawServerNodes.push_back(pRawServer);
	m_nState = (BYTE)1;
	return true;
}

bool LocalNodeGroup::AddClientNode(ClientNode_Base0 *pRawClient) {
	if (m_nState != (BYTE)0 && m_nState != (BYTE)1) { return false; }

	for (int i = 0; i < (int)m_rawServerNodes.size(); ++i)
	{
		if (strcmp(m_rawServerNodes[i]->GetName(), pRawClient->GetName()) == 0) {
			std::cout << "´íÎó@LocalNodeGroup::AddServerNode() : ½ÚµãÃû³Æ³åÍ», Ìí¼ÓÊ§°Ü, Çë¼ì²é...\n";
			return false;
		}
	}
	for (int i = 0; i < (int)m_rawClientNodes.size(); ++i) {
		if (strcmp(m_rawClientNodes[i]->GetName(), pRawClient->GetName()) == 0) {
			std::cout << "´íÎó@LocalNodeGroup::AddServerNode() : ½ÚµãÃû³Æ³åÍ», Ìí¼ÓÊ§°Ü, Çë¼ì²é...\n";
			return false;
		}
	}

	m_rawClientNodes.push_back(pRawClient);
	m_nState = (BYTE)1;
	return true;
}

bool LocalNodeGroup::FinishAddingNodes() {
	if (m_nState != (BYTE)1) { return false; }
	else {
		m_nState = (BYTE)2;
		return true;
	}
}

bool LocalNodeGroup::Launch() {
	if (m_nState != (BYTE)2) { return false; }

	m_bRequested2Quit = (BYTE)0;
	m_bSubThreadStarted = true;
	m_subThread = std::thread(_threadFunc, (void*)this);

	return true;
}

bool LocalNodeGroup::Publish(int serverNodeInd,
	                         const BYTE *pContent,
	                         int nContentLen) {
	if (m_nState != (BYTE)3) { return false; }
	bool bRet = false;
	if (serverNodeInd >= 0 && serverNodeInd <
		(int)m_rawServerNodes.size()) {
		bRet = m_rawServerNodes[serverNodeInd]->PublishContent(pContent,
				                                               nContentLen);
	}
	return bRet;
}

bool LocalNodeGroup::GetData(int clientNodeInd,
	                         std::vector<BYTE> &dataVec) {
	if (m_nState != (BYTE)3) { return false; }
	bool bRet = false;
	if (clientNodeInd >= 0 && clientNodeInd <
		(int)m_rawClientNodes.size()) {
		bRet = m_rawClientNodes[clientNodeInd]->GetData(dataVec);
	}
	return bRet;
}

void LocalNodeGroup::Stop() {
	if (m_bSubThreadStarted) {
		m_bRequested2Quit = (BYTE)1;
		m_subThread.join();
		m_bSubThreadStarted = false;
		m_rawServerNodes.resize(0);
		m_rawClientNodes.resize(0);
	}
	m_nState = (BYTE)0;
}

int LocalNodeGroup::_runThis() {
	const int rcvTmpBufSz = 1400;
	BYTE rcvTmpBuf[rcvTmpBufSz];

	sockaddr_in bcAddr;
	bcAddr.sin_family = AF_INET;
	inet_pton(AF_INET, DDRLNB_NODEINFO_BROADCASTING_ADDRESS_IP,
		      &(bcAddr.sin_addr.s_addr));
	bcAddr.sin_port = htons(DDRLNB_NODEINFO_BROADCASTING_ADDRESS_PORT);
	MIMCClient *pNodeInfoReceiver = nullptr;
	if (m_rawClientNodes.size() > 0) {
		pNodeInfoReceiver = new MIMCClient;
		pNodeInfoReceiver->JoinGroup(&bcAddr);
	}
	MIMCServer *pNodeInfoServer = nullptr;
	if (m_rawServerNodes.size() > 0) {
		pNodeInfoServer = new MIMCServer;
	}

	for (int i = 0; i < (int)m_rawServerNodes.size(); ++i) {
		if (!m_rawServerNodes[i]->Try2Launch()) {
			std::cout << "@LocalNodeGroup::_runThis() : µÚ" << (i + 1)
				<< "ºÅ·þÎñÆ÷Î´ÄÜ³É¹¦Æô¶¯, Çë¼ì²é!\n";
			return 4; // server not started
		}
	}

	std::vector<BYTE> broadcastInfo;
	for (int i = 0; i < (int)m_rawServerNodes.size(); ++i) {
		m_rawServerNodes[i]->attachNodeInfo(broadcastInfo);
	}
	auto _ticBC = std::chrono::system_clock::now()
		- std::chrono::milliseconds(NODEINFO_BROADCASTING_PERIOD_IN_MS * 2);

	m_nState = (BYTE)3;

	// ---------------------------- BIG LOOP ----------------------------
	while (1) {
		// checking quitting request
		if ((BYTE)1 == m_bRequested2Quit) { break; }

		// broadcast node info if any
		if (pNodeInfoServer) {
			auto _ticNow = std::chrono::system_clock::now();
			__int64 timediff = std::chrono::duration_cast
				<std::chrono::milliseconds>(_ticNow - _ticBC).count();
			if (timediff >= NODEINFO_BROADCASTING_PERIOD_IN_MS) {
				pNodeInfoServer->Broadcast(&bcAddr,
					                       (const char*)(&broadcastInfo[0]),
										   (int)broadcastInfo.size());
				_ticBC = _ticNow;
			} // end if it's time to broadcast device info
		}
		
		// IO
		fd_set rset, wset;
		FD_ZERO(&rset);
		FD_ZERO(&wset);
		if (pNodeInfoReceiver) {
			pNodeInfoReceiver->AddToSet(&rset);
		}
		for (int i = 0; i < (int)m_rawServerNodes.size(); ++i) {
			m_rawServerNodes[i]->addReadFD(&rset);
		}
		for (int i = 0; i < (int)m_rawClientNodes.size(); ++i) {
			m_rawClientNodes[i]->addFD(&rset, &wset);
		}				
		select(0, &rset, &wset, nullptr, &m_ioTV);

		if (pNodeInfoReceiver && pNodeInfoReceiver->IsSet(&rset)) {
			while (1) {
				sockaddr_in remoteAddr;
				int len = sizeof(sockaddr_in);
				int n = pNodeInfoReceiver->recv((char*)rcvTmpBuf, rcvTmpBufSz,
					                            (sockaddr*)&remoteAddr, &len);
				if (n <= 0) { break; } // UNEXPECTED
				int pos = 0;
				std::string name;
				BYTE dtID, tpID, aID;
				unsigned short port;
				while (pos < n) {
					int nextOffset;
					if (_getNodeInfo(rcvTmpBuf + pos, n - pos, nextOffset, name,
						dtID, tpID, aID, port)) {
#if 0
						char iipp[20];
						DDRCommLib::ConvertUIP2Str_IPv4(remoteAddr.sin_addr.s_addr, iipp);
						std::cout << "Received node broadcasting! IP=" << iipp
							<< ", tarPort=" << port << ", name=" << name << std::endl;
#endif
						for (int ii = 0; ii < (int)m_rawClientNodes.size(); ++ii) {
							m_rawClientNodes[ii]->Try2Launch(remoteAddr.sin_addr.s_addr,
								                             htons(port),
															 name.c_str(), dtID, tpID, aID);
						} // end trials for all client nodes
					} // end if cur position starts a correct node info format
					pos += nextOffset;
				} // end while (still other left-over to process)
			} // end while new stuff to receive
		}

		// rset/wset on or off, IO processing (sending queued publication
		// included) has to be done for timely responsiveness
		for (int i = 0; i < (int)m_rawServerNodes.size(); ++i) {
			m_rawServerNodes[i]->processIO(&rset);
		} // for each server node process IO
		for (int i = 0; i < (int)m_rawClientNodes.size(); ++i) {
			m_rawClientNodes[i]->processIO(&rset, &wset);
		} // for each client node process IO

		std::this_thread::sleep_for(std::chrono::milliseconds(m_sleepTimeMs));
	} // end BIG WHILE()
	// ++++++++++++++++++++++++++++ BIG LOOP ++++++++++++++++++++++++++++
	
	// closing servers
	for (int i = 0; i < (int)m_rawServerNodes.size(); ++i) {
		m_rawServerNodes[i]->close();
	}
	// closing client
	for (int i = 0; i < (int)m_rawClientNodes.size(); ++i) {
		m_rawClientNodes[i]->close();
	}
	if (pNodeInfoReceiver) {
		delete pNodeInfoReceiver;
	}
	if (pNodeInfoServer) {
		delete pNodeInfoServer;
	}
	return 0;
}

bool LocalNodeGroup::_getNodeInfo(const BYTE *pInfo, int nMaxInfoLen,
						    	  int &nextPossibleOffset,
								  std::string &name, BYTE &dataTypeID,
								  BYTE &transProtoID, BYTE &authTypeID,
								  unsigned short &port) {
	if (nMaxInfoLen < 11) {
		nextPossibleOffset = nMaxInfoLen;
		return false;
	}
	int nlen;
	if (!DDRCommLib::VerifyDataLen_32(pInfo, &nlen) ||
		nlen + 4 > nMaxInfoLen) {
		nextPossibleOffset = 1;
		return false;
	}
	const BYTE *pC = pInfo + 4;
	for (; pC < pInfo + 4 + nlen; ++pC) {
		if ((BYTE)'\0' == *pC) { break; }
	}
	if (pInfo + 4 + nlen - 6 == pC &&
		((pInfo[4] >= (BYTE)'A' && pInfo[4] <= (BYTE)'Z') ||
		(pInfo[4] >= (BYTE)'a' && pInfo[4] <= (BYTE)'z'))) {
		nextPossibleOffset = 4 + nlen;
		name = std::string((const char*)pInfo + 4);
		dataTypeID = pC[1];
		transProtoID = pC[2];
		authTypeID = pC[3];
		port = *((unsigned short*)(pC + 4));
		return true;
	} else {
		nextPossibleOffset = 1;
		return false;
	}
}

}

