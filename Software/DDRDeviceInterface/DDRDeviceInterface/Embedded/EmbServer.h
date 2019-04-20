#ifndef __DDR_EMBEDDED_TO_TCPSERVER_H_INCLUDED__
#define __DDR_EMBEDDED_TO_TCPSERVER_H_INCLUDED__

#include <vector>
#include "LocalNetworkNode/LNNFactory.h"
#include "SerialPortIOHandler.h"

namespace DDRDrivers
{
	class DDREmbeddedServer
	{
	public:
		DDREmbeddedServer();
		int GetVoiceChatCommID() const { return m_nVoiceChatCommID; }
		bool IsInitialized() const { return m_bInitialized; }
		bool IsGNSSModuleAvailable() const{ return (m_pGNSSUBLOX != nullptr); }
		void Close();
		~DDREmbeddedServer() { Close(); };
		bool WriteEmbMB(const void *pCommand, int nCommandLen);
		bool PurgeDevices();

	protected:
		bool m_bInitialized;

		// embedded device recognition and functional modules
		SerialPortIOHandler *m_pEmbedded_MB;
		SerialPortIOHandler *m_pGNSSUBLOX;
		int m_nVoiceChatCommID;

		void getAvailablePorts(std::vector<int> &ports);
		typedef bool(*_moduleDetectionFunc)(SerialPortIOHandler*,
			                                std::string&);
		void process();
		bool testOnePort(int portID, unsigned int baudrate,
			             std::string &devName,
						 std::vector<_moduleDetectionFunc> &testFuncs);
		static bool isEmbbededMB(SerialPortIOHandler *pSP, std::string &name);
		static bool isGNSSUBlox(SerialPortIOHandler *pSP, std::string &name);
		static bool isVoiceChatter(SerialPortIOHandler *pSP, std::string &name);

		std::shared_ptr<DDRLNN::LNNNodeGroupInterface> m_pLNG;
		static bool _myNDP4EMBMB(void *pVecBytes, void *pArg);
		struct _callStruct {
			SerialPortIOHandler *pSP;
			char *pBuf;
			int nBufCap;
		} _callArg[2];
		char m_tmpMBBuf[2048];
		char m_tmpUBLOXBuf[256];
	};
}

#endif // __DDR_EMBEDDED_TO_TCPSERVER_H_INCLUDED__