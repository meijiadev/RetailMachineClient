#ifndef __LOCAL_SERVER_BASE_H_INCLUDED__
#define __LOCAL_SERVER_BASE_H_INCLUDED__

#include <vector>
#include <string>
#include <mutex>
#include "DefaultConfig.h"
#include "SocketWrapper.h"

namespace DDRLNN
{
	class ServerNode_Base0
	{
	public:
		ServerNode_Base0();
		virtual ~ServerNode_Base0() {}
		BYTE GetStatus() const { return m_nState; }
		const char* GetName() const { return m_Name.c_str(); }
		BYTE GetTransProtoType() const { return m_transProtoID; }

		bool IsStreamed() const { return true; }
		bool IsTCPBased() const { return (DDRLNB_PROTO_ID_TCP == m_transProtoID); }
		BYTE GetAuthType() const { return m_authType; }
		virtual bool IsEstablished() const { return (4 == m_nState); }
		virtual int GetNumOfConnections() const { return 0; }

		bool SetServerOption(int szSndBuf, int szRcvBuf, int nMaxClients = 10);
		bool Try2Launch();

		bool PublishContent(const BYTE *pContent, int nContentLen);
		__int64 GetTimeSinceLastPublication() const; // in milliseconds
		
		// for non connection-oriented trans. protocol (e.g, UDP), no way to receive (C->S)
		virtual bool addReadFD(fd_set *pSet2Read) { return true; };
		void processIO(fd_set *pSetRead);
		virtual bool attachNodeInfo(std::vector<BYTE> &vec2append);
		virtual void close() = 0;

		typedef bool(*CALLBACK_VecFill)(void *pVecBytes, void *pArg);
		typedef bool(*CALLBACK_RAValidator)(const BYTE *pAuthStr, int nAuthLen, int *pNextPos, void *pArg);

		bool SetNDPArg(void *pArgNDP) {
			m_pArgNDP = pArgNDP;
			return true;
		}
		bool SetNDPFunc(CALLBACK_VecFill pFuncNDP) {
			m_pFuncNDP = pFuncNDP;
			return true;
		}
		bool SetIdlingPeriod(int idlingPeriod) {
			if (idlingPeriod < 0) {
				idlingPeriod = 0;
			} else if (idlingPeriod > 3600000) {
				idlingPeriod = 3600000;
			}
			m_IdlingPeriod = idlingPeriod;
			return true;
		}
		bool SetIGArg(void *pArgIG) {
			m_pArgIG = pArgIG;
			return true;
		}
		bool SetIGFunc(CALLBACK_VecFill pFuncIG) {
			m_pFuncIG = pFuncIG;
			return true;
		}
		bool SetWAGArg(void *pArgWAG) {
			if (IsTCPBased() && GetAuthType() == 2) {
				m_pArgWAG = pArgWAG;
				return true;
			}
			else { return false; }
		}
		bool SetWAGFunc(CALLBACK_VecFill pFuncWAG) {
			if (IsTCPBased() && GetAuthType() == 2) {
				m_pFuncWAG = pFuncWAG;
				return true;
			} else { return false; }
		}
		bool SetRAVArg(void *pArgRAV) {
			if (IsTCPBased() && GetAuthType() == 1) {
				m_pArgRAV = pArgRAV;
				return true;
			}
			else { return false; }
		}
		bool SetRAVFunc(CALLBACK_RAValidator pFuncRAV) {
			if (IsTCPBased() && GetAuthType() == 1) {
				m_pFuncRAV = pFuncRAV;
				return true;
			}
			else { return false; }
		}
		
	protected:
		static const int HEART_BEAT_PERIOD_IN_MS = 1000;

		// 0 - uninitialized; 1 - name set; 2 - data type set;
		// 3 - trans. prot. & auth. type set, ready to launch; 4 - launched
		BYTE m_nState;
		std::string m_Name;
		BYTE m_dataType;
		BYTE m_transProtoID;
		BYTE m_authType;
		int m_szSndBuf;
		int m_szRcvBuf;
		int m_nMaxClients; // ignored under UDP

		unsigned short m_port;
		std::timed_mutex m_bufLock;
		std::vector<BYTE> m_sndBuffer;
		std::vector<BYTE> m_tmpBuf;
		std::chrono::system_clock::time_point m_lastContentTime;
		std::chrono::system_clock::time_point m_lastHeartBeatTime;

		void *m_pArgNDP;
		CALLBACK_VecFill m_pFuncNDP;
		void *m_pArgIG;
		int m_IdlingPeriod; // in milliseconds
		CALLBACK_VecFill m_pFuncIG;
		void *m_pArgWAG;
		CALLBACK_VecFill m_pFuncWAG;
		void *m_pArgRAV;
		CALLBACK_RAValidator m_pFuncRAV;

		static bool _hasNewDataToPublish(void*, void*) { return false; }
		static bool _generateIdlingString(void*, void*) { return false; }
		static bool _generateWriteAuth(void*, void*) { return false; }
		static bool _validateReadAuth(const BYTE*, int nAuthLen, int *pNextPos, void*) {
			if (pNextPos) {
				*pNextPos = nAuthLen;
			}
			return true;
		}

		bool SetName(const char *pName);
		void _try2send(); // send content or protocol if necessary. return if something is sent
		void _sendHB(); // heat-beat signals
		virtual void _processRecv(fd_set *pSetRead) {} // for non connection-oriented trans. protocol (e.g, UDP), no way to receive (C->S)
		virtual void _updateConnections() {} // for non connection-oriented trans. protocol (e.g, UDP), no need to manage connections
		virtual bool _establishServer() = 0;
		virtual void _send2all(const BYTE *pData, int nDataLen) = 0;
	};
}

#endif
