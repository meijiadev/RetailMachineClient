#include "EmbServer.h"

#include <string>
#include <chrono>
#include <iostream>
#include <fstream>
#include "CommonDef/CommonDefFunc.h"
//#include "Gadgets/TTSQueuePlayer.h"

#define SERIALPORT_RECORD_FILE "config/EmbPortRec.txt"

//extern bool PlayVoiceManager(std::string strData, int nLevel);

namespace DDRDrivers {

bool DDREmbeddedServer::_myNDP4EMBMB(void *pVecBytes, void *pArg) {
	if (pArg) {
		_callStruct *pARG = (_callStruct*)pArg;
		int nRead;
		if (pARG->pSP->Read(pARG->pBuf, pARG->nBufCap, nRead)) {
			DDRLNN::ResizeVecBytes(pVecBytes, nRead);
			memcpy(DDRLNN::GetVecBytesHead(pVecBytes), pARG->pBuf, nRead);
			return true;
		}
	}
	return false;
}

DDREmbeddedServer::DDREmbeddedServer() {
	m_bInitialized = false;
	m_pEmbedded_MB = m_pGNSSUBLOX = nullptr;
	m_nVoiceChatCommID = 0;
	process();
}

void DDREmbeddedServer::process() {
	// stage I - device detection
	std::vector<_moduleDetectionFunc> testFuncs{ isEmbbededMB, isGNSSUBlox, isVoiceChatter };
	std::vector<unsigned int> baudRates{ 9600, 115200, 230400 };
	std::vector<int> ports;
	getAvailablePorts(ports);
	
	std::ofstream ofs(SERIALPORT_RECORD_FILE);

	std::string devName;
	for (int ii = 0; ii < (int)ports.size(); ++ii) {
		for (int jj = 0; jj < (int)baudRates.size(); ++jj) {
			if (testOnePort(ports[ii], baudRates[jj], devName, testFuncs)) {
				ofs << ports[ii] << std::endl;
				break;
			}
		} // end for (jj) iterating through all baud rates
		if (testFuncs.size() == 0) { break; }
	} // end for (ii) iterating through all ports

	if (!m_pEmbedded_MB) {
		std::cout << "未发现嵌入式主板, 请确认设备连接..." << std::endl;
		return;
	}

	//PlayVoiceManager("嵌入式设备服务已成功启动", 1); // error
	std::cout << "************************** 标签: **************************\n";
	std::cout << "===================== 嵌入式主板\n";
	if (m_pGNSSUBLOX) {
		std::cout << "===================== 卫星定位模块\n";
	}
	if (m_nVoiceChatCommID > 0) {
		std::cout << "===================== 语音交互模块\n";
	}
	std::cout << "***********************************************************" << std::endl;
	
	// stage II - launch LNN for SP2TCP service
	m_pLNG = DDRLNN::CreateNodeGroup();

	_callArg[0] = { m_pEmbedded_MB, m_tmpMBBuf, 2048 };
	m_pLNG->AddTCPServer("EMBEDDED_MB_DATA", 0);
	m_pLNG->SetServerOption("EMBEDDED_MB_DATA", 32768, 512, 20);
	m_pLNG->_scb_SetArg_NewDataPublish("EMBEDDED_MB_DATA", &_callArg[0]);
	m_pLNG->_scb_SetFunc_NewDataPublish("EMBEDDED_MB_DATA", _myNDP4EMBMB);
	if (m_pGNSSUBLOX) {
		_callArg[1] = { m_pGNSSUBLOX, m_tmpUBLOXBuf, 256 };
		m_pLNG->AddTCPServer("GNSSUBLOX_DATA", 0);
		m_pLNG->SetServerOption("GNSSUBLOX_DATA", 2048, 128, 20);
		m_pLNG->_scb_SetArg_NewDataPublish("GNSSUBLOX_DATA", &_callArg[1]);
		m_pLNG->_scb_SetFunc_NewDataPublish("GNSSUBLOX_DATA", _myNDP4EMBMB);
	}

	m_pLNG->SetLoopTempo(20, 1);
	if (!m_pLNG->StartRunning()) {
		std::cout << "@EmbServer::process() : 启动节点组失败, 请检查..." << std::endl;
	} else {
		std::cout << "@EmbServer::process() : 成功启动串口设备转TCP服务..." << std::endl;
		m_bInitialized = true;
	}
}

void DDREmbeddedServer::getAvailablePorts(std::vector<int> &ports) {
	ports.resize(0);
	std::vector<char> lineStr;
	char buffer[128];
	FILE *pOut = _popen("C:\\Windows\\System32\\reg query HKEY_LOCAL_MACHINE\\Hardware\\DeviceMap\\SerialComm", "rt");
	if (nullptr == pOut) { return; }
	while (!feof(pOut)) {
		if (fgets(buffer, 128, pOut) != nullptr) {
			int i;
			for (i = 0; buffer[i] != '\0'; ++i);
			int pp = (int)lineStr.size();
			lineStr.resize(pp + i);
			if (i > 0) {
				memcpy(&lineStr[pp], buffer, i);
				if ((char)0x0A == buffer[i - 1]) {
					// a line integrated
					int ii = (int)lineStr.size() - 1;
					for (; ii >= 0; --ii) {
						if (' ' == lineStr[ii]) {
							break;
						}
					}
					if (ii >= 0 && strncmp(&lineStr[ii + 1], "COM", 3) == 0) {
						int ss = 0;
						for (ii += 4; lineStr[ii] >= '0' && lineStr[ii] <= '9'; ++ii) {
							ss = ss * 10 + (lineStr[ii] - '0');
						}
						ports.push_back(ss);
					}
					lineStr.resize(0);
				}
			}
		}
	}
	std::ifstream ifs(SERIALPORT_RECORD_FILE);
	if (!ifs.is_open()) { return; }
	int x;
	while (ifs >> x) {
		int occInd = -1;
		for (int i = 0; i < (int)ports.size(); ++i) {
			if (ports[i] == x) {
				occInd = i;
				break;
			}
		}
		if (-1 == occInd) {
			ports.insert(ports.begin(), x);
		} else {
			for (int i = occInd; i > 0; --i) {
				ports[i] = ports[i - 1];
			}
			ports[0] = x;
		}
	}
}

bool DDREmbeddedServer::testOnePort(int portID, unsigned int baudrate,
	                                std::string &devName,
									std::vector<_moduleDetectionFunc> &testFuncs) {
	if (testFuncs.size() == 0) { return false; }

	std::cout << "测试串口设备 (COM" << portID << " @ "
		<< baudrate << "bps)..." << std::endl;
	
	SerialPortIOHandler spioh;
	spioh.SetSerialPort(portID, baudrate);
	if (!spioh.Open()) { return false; }

	for (int kk = 0; kk < (int)testFuncs.size(); ++kk) {
		if ((testFuncs[kk])(&spioh, devName)) {
			if (devName.compare("EMBEDDED_MB") == 0 &&
				nullptr == m_pEmbedded_MB) {
				spioh.Close();
				m_pEmbedded_MB = new SerialPortIOHandler;
				m_pEmbedded_MB->SetSerialPort(portID, baudrate);
				if (m_pEmbedded_MB->Open()) {
					std::cout << "成功识别出嵌入式设备'" << devName
						<< "' (COM" << portID << " @ " << baudrate
						<< "bps)并打开!" << std::endl;
					for (int i = 0; i < (int)testFuncs.size(); ++i) {
						if (isEmbbededMB == testFuncs[i]) {
							testFuncs.erase(testFuncs.begin() + i);
							break;
						}
					}
					return true;
				}
			} // if device recognized as MB and MB slot available
			if (devName.compare("GNSSUBLOX") == 0 &&
				nullptr == m_pGNSSUBLOX) {
				spioh.Close();
				m_pGNSSUBLOX = new SerialPortIOHandler;
				m_pGNSSUBLOX->SetSerialPort(portID, baudrate);
				if (m_pGNSSUBLOX->Open()) {
					std::cout << "成功识别出嵌入式设备'" << devName
						<< "' (COM" << portID << " @ " << baudrate
						<< "bps)!" << std::endl;
					for (int i = 0; i < (int)testFuncs.size(); ++i) {
						if (isGNSSUBlox == testFuncs[i]) {
							testFuncs.erase(testFuncs.begin() + i);
							break;
						}
					}
					return true;
				}
			} // if device recognized as UBLOX and UBLOX slot available
			if (115200 == baudrate &&
				devName.compare("VOICECHATTER") == 0 &&
				0 == m_nVoiceChatCommID) {
				spioh.Close();
				m_nVoiceChatCommID = portID;
				std::cout << "成功识别出嵌入式设备'" << devName
					<< "' (COM" << portID << " @ " << baudrate
					<< "bps)!" << std::endl;
				for (int i = 0; i < (int)testFuncs.size(); ++i) {
					if (isVoiceChatter == testFuncs[i]) {
						testFuncs.erase(testFuncs.begin() + i);
						break;
					}
				}
				return true;
			}
		} // if test passed
	} // end for (kk) iterating through all device tests

	return false;
}

bool DDREmbeddedServer::isEmbbededMB(SerialPortIOHandler *pSP,
	                                 std::string &name) {
	int sFrame = 0;
	auto _tic0 = std::chrono::system_clock::now();
	char buffer[1024];
	int bufPos = 0;
	while (1)
	{
		auto _tic1 = std::chrono::system_clock::now();
		__int64 timediff = std::chrono::duration_cast
			<std::chrono::milliseconds>(_tic1 - _tic0).count();
		if (timediff >= 500) {
			break;
		}
		else if (timediff > 80 && sFrame == 0) {
			break;
		}

		int n;
		if (!pSP->Read(buffer + bufPos, 128, n)) {
			return false;
		}
		if (0 == n) {
			continue;
		}
		bufPos += n;
		bool bIncomplete = false;
		int ss;
		for (ss = 0; ss < bufPos;) {
			if ((char)0x55 == buffer[ss]) {
				int remLen = bufPos - ss;
				if (remLen >= 28) {
					if ((char)0xFF == buffer[ss + 1]
						&& (char)0xFF == buffer[ss + 26]
						&& (char)0xFE == buffer[ss + 27]) {
						int cs = 0;
						for (int i = 2; i < 25; ++i) {
							cs += (unsigned char)buffer[ss + i];
						}
						if ((unsigned char)(cs & 0xFF) ==
							(unsigned char)buffer[ss + 25]) {
							++sFrame;
						}
					}
					ss += 28; // enough for a frame
				} else {
					memmove(buffer, buffer + ss, remLen);
					bufPos = remLen;
					bIncomplete = true;
					break;
				} // not enough for a frame
			} else {
				++ss;
			}
		}
		if (!bIncomplete) {
			bufPos = 0;
		}
	}
	// condition A. starting with 0x55 and frame length 28
	//			 B. [1][26][27] == '0xFF 0xFF 0xFE'
	//			 C. [25] == check sume [2, 24]
	//			 D. at least one frame within first 80 milliseconds
	//			 E. at least 12 frames within 500 milliseconds
	if (sFrame >= 12) {
		name = "EMBEDDED_MB";
		return true;
	} else { return false; }
}

bool DDREmbeddedServer::isGNSSUBlox(SerialPortIOHandler *pSP,
	std::string &name) {
	auto _tic0 = std::chrono::system_clock::now();
	char str[15], lastChar = (char)0x0A;
	int sChar = 0;
	while (1) {
		auto _tic1 = std::chrono::system_clock::now();
		__int64 timediff = std::chrono::duration_cast
			<std::chrono::milliseconds>(_tic1 - _tic0).count();
		if (timediff >= 500) {
			break;
		}

		int n;
		if (!pSP->Read(str, 15, n)) {
			return false;
		}
		if (0 == n) {
			DDRCommonDef::Sleep(1);
			continue;
		}
		if (str[0] == '$' && lastChar != (char)0x0A) {
			return false;
		}
		for (int i = 0; i < n; ++i)	{
			if ((str[i] < ' ' || str[i] > '~') &&
				str[i] != (char)0x0A &&
				str[i] != (char)0x0D) {
				return false;
			}
		}
		lastChar = str[n - 1];
		sChar += n;
	}
	// condition A. all bytes (but 0x0A and ox0D) are printable ASCII characters
	//			 B. every line ends with 0x0A and starts with '$'s
	//			 C. at least 25 characters within 1000 milliseconds
	if (sChar >= 10) {
		name = "GNSSUBLOX";
		return true;
	} else { return false; }
}


bool DDREmbeddedServer::isVoiceChatter(SerialPortIOHandler *pSP,
	                                   std::string &name) {
	for (int nTrial = 0; nTrial < 5; ++nTrial) {
		pSP->Write("set 0", 5);
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		char rcv[4];
		int rcvLen;
		pSP->Read(rcv, 4, rcvLen);
		if (rcvLen >= 2) {
			if ('O' == rcv[0] && 'K' == rcv[1]) {
				name = "VOICECHATTER";
				return true;
			}
		}
	}
	return false;
}

void DDREmbeddedServer::Close() {
	if (m_pLNG) {
		m_pLNG->StopRunning();
	}
	CLEAR_HEAP_PTR(m_pEmbedded_MB);
	CLEAR_HEAP_PTR(m_pGNSSUBLOX);
}

bool DDREmbeddedServer::WriteEmbMB(const void *pCommand, int nCommandLen) {
	if (m_pEmbedded_MB && nCommandLen > 0 && pCommand) {
		return m_pEmbedded_MB->Write((const char*)pCommand, nCommandLen);
	} else { return false; }
}

bool DDREmbeddedServer::PurgeDevices() {
	bool bRet = false;
	if (m_pEmbedded_MB) {
		m_pEmbedded_MB->FlushRx();
		bRet = true;
	}
	if (m_pGNSSUBLOX) {
		m_pGNSSUBLOX->FlushRx();
		bRet = true;
	}
	return true;
}

}