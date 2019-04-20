/**
Local Subscription Client Node of the DADAO Network Communication Libraries.
Mar 2018, Jianrui Long @ DADAOII

A client node is a node that receives data from a single server. It navigates
to the corresponding server by parsing formatted node info of servers being
braocasted at DDRLNB_NODEINFO_BROADCASTING_ADDRESS_IP:DDRLNB_NODEINFO_BROADCASTING_ADDRESS_PORT.
Once the node's name, data type, transmission layer protocol type, and
authorization type are all equal, the client will try to connect to the server
using the server's IP and its port value in the node info being broadcasted.

class ClientNode_Base0 will handle logic state switching and node info parsing,
Connetion establishment and its closing will be extended by its derived
classes.

General steps to use objects derived from this abstract clas are:
1. SetNode()
2. SetServerOption(). Optionally set size of receiving and sending buffers.
For TCP servers, set maximum number of clients allowed.
3*. User defines his own way to read node info being broadcasted at
DDRLNB_NODEINFO_BROADCASTING_ADDRESS_IP:DDRLNB_NODEINFO_BROADCASTING_ADDRESS_PORT
till a string is received.
4. Try2Launch() using the received node info
5. addFD()
6. select()
7. processIO()
Stage 3-7 are generally repeated in a loop in a one thread. In another
data-feeding thread, do:
8. GetData()
Statge 8 is generally repeated in a loop.

To extend this abstract class, derive it and implement following functions:
addFD() - mandatory. Add server's socket file descriptor to a read set and
a write set. NOTE THAT THE WRITE SET AVAILIBILITY IS USED TO CHECK IF AN
INITIATED CONNECTION IS ACCEPTED (so this only applied for TCP)
close() - mandatory. Closing client.
_establishClient() - mandatory. Establish a socket client.
_respond2write() - optional. Only applies to TCP when checking whether the
initiated connection is accpeted from the other peer.
_respond2read() - mandatory. Process raw data received from the server.
_updateConnection() - optional. If the initiated connection is not accepted,
or authorized within certain time window, the connection is closed. Only
applies to TCP.
_sendHB() - optional. Send keep-alive signal from clients to servers. Only
applies to TCP.
*/

#ifndef __DDR_LOCAL_CLIENT_BASE_H_INCLUDED__
#define __DDR_LOCAL_CLIENT_BASE_H_INCLUDED__

#include <vector>
#include <mutex>
#include "DefaultConfig.h"
#include "SocketWrapper.h"

namespace DDRLNN
{
	class ClientNode_Base0
	{
	public:
		ClientNode_Base0();
		virtual ~ClientNode_Base0();

		BYTE GetStatus() const { return m_nState; }
		const char* GetName() const { return m_Name.c_str(); }
		BYTE GetDataType() const { return m_nDataType; }
		BYTE GetTransProtoType() const { return m_transProtoID; }

		bool IsStreamed() const { return (DDRLNB_DATATYPE_ID_STREAM == m_nDataType); }
		bool IsTCPBased() const { return (DDRLNB_PROTO_ID_TCP == m_transProtoID); }
		BYTE GetAuthType() const { return m_authType; }
		virtual bool IsConnected() const { return (4 == m_nState); }

		bool SetClientOption(int szSndBuf, int szRcvBuf);
		bool Try2Launch(unsigned long srcIP_n, unsigned short tarPort_n,
			            const char *pNodeName,
						BYTE dataTypeID, BYTE transProtoID,
						BYTE authTypeID);

		// return TRUE if data successully obtained (stream buffer for STREAM data type,
		// and serialized structured data for STRUCTURED data type);
		// return FALSE if locked by other threads, or not in connected state
		bool GetData(std::vector<BYTE> &dataVec);
		__int64 GetTimeSinceLastContentReception() const; // in milliseconds
		
		virtual bool addFD(fd_set *pSet2Read, fd_set *pSet2Write) = 0;
		void processIO(fd_set *pSetRead, fd_set *pSetWritten);
		virtual void close() = 0;

		typedef unsigned int(*CALLBACK_BUF2PROCESS)(BYTE *pBuf, unsigned int nBufLen, void *pArg);
		typedef bool(*CALLBACK_VecBytesFill)(void *pVecBytes, void *pArg);
		typedef bool(*CALLBACK_WAValidation)(const BYTE *pAuthStr, int nAuthLen, int *pNextPos, void *pArg);

		bool SetORDArg(void *pArgORD) {
			m_pArgORD = pArgORD;
			return true;
		}
		bool SetORDFunc(CALLBACK_BUF2PROCESS pFuncORD) {
			m_pFuncORD = pFuncORD;
			return true;
		}
		bool SetRAGArg(void *pArgRAG) {
			if (IsTCPBased() && GetAuthType() == 1) {
				m_pArgRAG = pArgRAG;
				return true;
			} else { return false; }
		}
		bool SetRAGFunc(CALLBACK_VecBytesFill pFuncWAG) {
			if (IsTCPBased() && GetAuthType() == 1) {
				m_pFuncRAG = pFuncWAG;
				return true;
			} else { return false; }
		}
		bool SetWAVArg(void *pArgWAV) {
			if (IsTCPBased() && GetAuthType() == 2) {
				m_pArgWAV = pArgWAV;
				return true;
			} else { return false; }
		}
		bool SetWAVFunc(CALLBACK_WAValidation pFuncWAV) {
			if (IsTCPBased() && GetAuthType() == 2) {
				m_pFuncWAV = pFuncWAV;
				return true;
			} else { return false; }
		}

	protected:
		static const int MAXTIME_FOR_INACTIVE_CONNECTION = 10000;

		// 0 - uninitialized; 1 - data type set; 2 - data name set;
		// 3 - trans. prot. & auth. type set, ready to launch; 4 - launched
		BYTE m_nState;
		std::string m_Name;
		BYTE m_nDataType;
		BYTE m_transProtoID;
		BYTE m_authType;
		int m_szSndBuf;
		int m_szRcvBuf;
		sockaddr_in m_curTarAddr;
		SocketWrapper m_soc;
		std::chrono::system_clock::time_point m_lastAnyReceptionTime;
		std::chrono::system_clock::time_point m_lastContentReceptionTime;

		std::timed_mutex m_dataLock;
		void *m_pData;
		std::vector<BYTE> m_rcvBuf;
		std::vector<BYTE> m_tmpBuf;

		void *m_pArgORD;
		CALLBACK_BUF2PROCESS m_pFuncORD;
		void *m_pArgRAG;
		CALLBACK_VecBytesFill m_pFuncRAG;
		void *m_pArgWAV;
		CALLBACK_WAValidation m_pFuncWAV;

		static unsigned int onReceivedData(BYTE *pBuf, unsigned int nBufLen, void *pArg) { return nBufLen; }
		static bool _generateReadAuth(void *pVecBytes, void *pArg) { return false; }
		static bool _validateWriteAuth(const BYTE *pAuthStr, int nAuthLen, int *pNextPos, void *pArg) {
			if (pNextPos) {
				*pNextPos = nAuthLen;
			}
			return true;
		}

		bool SetName(const char *pName);
		virtual bool _establishClient(unsigned long bcIP_n,
			                          unsigned short serPort_n) = 0;
		virtual bool _respond2write() {	return true; } // return if the connection is still active
		virtual bool _respond2read(std::vector<BYTE> &rcvVec) = 0; // return if the connection is still active
		virtual bool _updateConnection(); // return if the connection is still active
		void _processRcvBuf();
		virtual void _sendHB() {} // heat-beat signals. Do nothing over UDP
	};
}

#endif
