#ifndef __DDR_LNN_LIB_NODEGROUP_INTERFACE_H_INCLUDED__
#define __DDR_LNN_LIB_NODEGROUP_INTERFACE_H_INCLUDED__

#ifdef DDR_LOCALNETWORKNODE_LIB
#define __API__DDRLNN__ __declspec(dllexport)   
#else
#define __API__DDRLNN__ __declspec(dllimport)
#ifdef _DEBUG
#ifndef _WIN64
#pragma comment (lib, "LocalNetworkNode/LocalNetworkNode_d.lib")
#else
#pragma comment (lib, "LocalNetworkNode/LocalNetworkNode_X64d.lib")
#endif
#else
#ifndef _WIN64
#pragma comment (lib, "LocalNetworkNode/LocalNetworkNode_r.lib")
#else
#pragma comment (lib, "LocalNetworkNode/LocalNetworkNode_X64r.lib")
#endif
#endif
#endif

#ifndef BYTE
typedef unsigned char BYTE;
#endif

namespace DDRLNN
{
	class LNNNodeGroupInterface
	{
	public:
		// *********************** �������ڵ㴴�������� *********************** //
		// *********************** Server node creation and configuration *********************** //

		// ���TCP�������ڵ�. ���ͬ���������ڵ��Ѵ����򷵻�false; ���򷵻�true.
		// Try to add a TCP server node. Return false if a server node with the same name exists.
		virtual bool _stdcall AddTCPServer(const char *pName,
								           BYTE authType // 0 - no auth.; 1 - Read Auth. (C->S); 2 - Write Auth. (S->C)
										   ) = 0;
		// ���UDP�������ڵ�. ���ͬ���������ڵ��Ѵ����򷵻�false; ���򷵻�true.
		// Try to add a UDP server node. Return false if a server node with the same node exist.
		virtual bool _stdcall AddUDPServer(const char *pName) = 0;
		// ���÷������ڵ㻺���С�����������(UDP�ڵ�������������)
		// �ɹ�����1; ����ʧ�ܷ���0; �������ڵ��������ڷ���-1
		// Try to set server buffer and max number of connections (latter ignored for UDP).
		// Return 1 for success; 0 for failure to set (node status may prohibit this setting);
		// -1 for failure to find a server node with specified name
		virtual int _stdcall SetServerOption(const char *pServerName, // name of the targeted server node
			                                 int szSndBuf, int szRcvBuf,
											 int nMaxClients = 10) = 0;
		
		// *********************** �ͻ��˽ڵ�Ĵ��������� *********************** //
		// *********************** Client node creation and configuration *********************** //

		// ���TCP�ͻ��˽ڵ�. ���ͬ���ͻ��˽ڵ��Ѵ����򷵻�false; ���򷵻�true.
		// Try to add a TCP client node. Return false if a server node with the same name exists.
		virtual bool _stdcall AddTCPClient(const char *pName,
			                               BYTE authType // 0 - no auth.; 1 - read auth. (C->S); 2 - write auth. (S->C)
										   ) = 0;
		// ���UDP�ͻ��˽ڵ�. ���ͬ���ͻ��˽ڵ��Ѵ����򷵻�false; ���򷵻�true.
		// Try to add a UDP client node. Return false if a server node with the same name exists.
		virtual bool _stdcall AddUDPClient(const char *pName) = 0;
		// ���ÿͻ��˽ڵ㻺���С
		// �ɹ�����1; ����ʧ�ܷ���0; �������ڵ��������ڷ���-1
		// Try to set server buffer and max number of connections (ignored for UDP).
		// Return 1 for success; 0 for failure to set (node status may prohibit this setting);
		// -1 for failure to find a client node with specified name
		virtual int _stdcall SetClientOption(const char *pClientName, // name of the targeted client node
			                                 int szSndBuf, int szRcvBuf) = 0;


		// *********************** �ص��������� *********************** //
		// *********************** CALLBACK settings *********************** //

		// _scb_��ͷ�������ڷ������ڵ�, _ccb_��ͷ�������ڿͻ��˽ڵ�.
		// ע����ຯ����void *pArg, ���û��Զ���Ļص������п����������⸴�ӵĲ�������
		// _scb_ for servers and _ccb for clients.
		// Note that "void *pArg" can be used by customed callback functions to pass arbitarily
		// complex parameters

		// �������std::vector<BYTE>�Ļص�����
		// pVecBytesΪstd::vector<BYTE>ʵ����ָ��(ת��Ϊvoid*)
		// ע: ���û��Զ���Ļص�������, �����pVecBytes����resize, ����ص���DDRLNN::ResizeVecBytes()
		// ��ȷ����DLL�߽���õ��ڴ氲ȫ
		// Callback to fill up a vector of bytes
		// pVecBytes is a pointer to std::vector<BYTE> converted to void*
		// (resizing by DDRLNN::ResizeVecBytes() to ensure memory integrity across DLL boundary)
		typedef bool(*CALLBACK_VecBytesFill)(void *pVecBytes, void *pArg);
		// ���ڴ�����ջ������Ļص�����
		// pBufָ�򻺳���ͷ, nBufLen�����仺��������
		// ����ֵ�ƶ�������β����Ҫ������ֽ������� (��, ����ֵΪ2��������ǰ��
		// (nBufLen-2)�ֽ��Ѿ�������Ҫ������, β��2�ֽ���Ҫ������������ͷ������)
		// Callback to process a buffer of bytes (receving publications)
		// pBuf points to the head of the buffer and nBufLen is the length of buffer.
		// Return the length of the buffer tails to keep (e.g, return 2 means the first
		// (nBufLen-2) bytes have been processed and the last two bytes have to be preserved
		typedef unsigned int(*CALLBACK_BUF2PROCESS)(BYTE *pBuf, unsigned int nBufLen, void *pArg);
		// ���ڽ��ж�/д��֤�Ļص�����
		// pAuthStrָ��Ҫ��֤���ַ���(���԰����ɼ��򲻿ɼ��ַ�), nAuthLenΪ�䳤��,
		// pNextPosΪ�������ղ�����֤���������ָ��
		// �����֤ͨ��, *pNextPos��ֵ����������֤�ַ���֮��ĵ�һ���ֽ������pAuthStr��ƫ����
		// (�ر���������֤����ʣ������), ͬʱ��������true; �����ǰ��֤ʧ��, ��������false,
		// *pNextPos��ֵ����-1��ʾ�����Ƿ��к����ֽ���֤һ��ʧ��, ��Ϊ-1��ʾ������֤�������ȡ����
		// �����ֽڡ�
		// Callback to validate Read Auth.
		// Return true if validation is successful, and fill nextPos with next offset
		// after the complete validation string; return false for failed validatation,
		// and fill nextPos with -1 ONLY IF IT IS ALREADY IMPOSSIBLE to validate even
		// if more bytes are received, otherwise the final results depend on incoming bytes.
		typedef bool(*CALLBACK_AuthValidator)(const BYTE *pAuthStr, int nAuthLen, int *pNextPos, void *pArg);

		// ������Ӧ�����ݷ��������Ĳ���(void*)
		// �ɹ�����1; ����ʧ�ܷ���0; �������ڵ��������ڷ���-1
		// Set argument for new data publish function.
		// Return 1 for success; 0 for failure to set;
		// -1 for failure to find a server node with specified name
		virtual int _stdcall _scb_SetArg_NewDataPublish(const char *pServerName, // name of the targeted server node
			                                            void *pArgNDP) = 0;
		// ������Ӧ�����ݷ���������ָ��
		// �ɹ�����1; ����ʧ�ܷ���0; �������ڵ��������ڷ���-1
		// Set callback function pointer to publish new data.
		// The function pointer will be used to check if new data are available to publish.
		// Return 1 for success; 0 for failure to set;
		// -1 for failure to find a server node with specified name
		virtual int _stdcall _scb_SetFunc_NewDataPublish(const char *pServerName, // name of the targeted server node
			                                             CALLBACK_VecBytesFill pFuncNDP) = 0;
		// ������Ӧ���з��������Ĳ���(void*)
		// �ɹ�����1; ����ʧ�ܷ���0; �������ڵ��������ڷ���-1
		// Set argument for idling data generation function.
		// Return 1 for success; 0 for failure to set;
		// -1 for failure to find a server node with specified name
		virtual int _stdcall _scb_SetArg_IdlingGenerator(const char *pServerName, // name of the targeted server node
			                                             void *pArgIG) = 0;
		// ���ÿ�������, ��λ����
		// �ɹ�����1; ����ʧ�ܷ���0; �������ڵ��������ڷ���-1
		// Set idling period in milliseconds.
		// Return 1 for success; 0 for failure to set;
		// -1 for failure to find a server node with specified name
		virtual int _stdcall _scb_SetParam_IdlingGenerator(const char *pServerName, // name of the targeted server node
			                                               unsigned int idlingPeriodInMs) = 0;
		// ������Ӧ���з���������ָ��(��ʱ���������ݷ���ʱ�˺���������)
		// �ɹ�����1; ����ʧ�ܷ���0; �������ڵ��������ڷ���-1
		// Set callback function pointer to generate idling data. This functions pointer will
		// be used when certain period of time (idling period) has elapsed without publishing anything.
		// Return 1 for success; 0 for failure to set;
		// -1 for failure to find a server node with specified name
		virtual int _stdcall _scb_SetFunc_IdlingGenerator(const char *pServerName, // name of the targeted server node
			                                              CALLBACK_VecBytesFill pFuncIG) = 0;
		// ���÷�����д��֤���ɺ����Ĳ���(void*)
		// �ɹ�����1; ����ʧ�ܷ���0; �������ڵ��������ڷ���-1
		// Set argument for Write Auth. generation function.
		// Return 1 for success; 0 for failure to set;
		// -1 for failure to find a server node with specified name
		virtual int _stdcall _scb_SetArg_WAGenerator(const char *pServerName, // name of the targeted server node
			                                         void *pArgWAG) = 0;
		// ���÷�����д��֤���ɺ�����ָ��
		// �ɹ�����1; ����ʧ�ܷ���0; �������ڵ��������ڷ���-1
		// Set callback function to generate Write Auth. data (Server->Client, validated by clients).
		// Return 1 for success; 0 for failure to set;
		// -1 for failure to find a server node with specified name
		virtual int _stdcall _scb_SetFunc_WAGenerator(const char *pServerName, // name of the targeted server node
			                                          CALLBACK_VecBytesFill pFuncWAG) = 0;
		// ���÷���������֤�����Ĳ���(void*)
		// �ɹ�����1; ����ʧ�ܷ���0; �������ڵ��������ڷ���-1
		// set argument for Read Auth. validation function.
		// Return 1 for success; 0 for failure to set;
		// -1 for failure to find a server node with specified name
		virtual int _stdcall _scb_SetArg_RAValidator(const char *pServerName, // name of the targeted server node
			                                         void *pArgRAV) = 0;
		// ���÷���������֤������ָ��
		// �ɹ�����1; ����ʧ�ܷ���0; �������ڵ��������ڷ���-1
		// Set callback function to validate Read Auth. (Client->Server, validated by servers).
		// Return 1 for success; 0 for failure to set;
		// -1 for failure to find a server node with specified name
		virtual int _stdcall _scb_SetFunc_RAValidator(const char *pServerName, // name of the targeted server node
			                                          CALLBACK_AuthValidator pFuncRAV) = 0;

		// ���ý������ݴ������Ĳ���(void*)
		// �ɹ�����1; ����ʧ�ܷ���0; �ͻ��˽ڵ��������ڷ���-1
		// Set argument for receiving buffer processing function.
		// Return 1 for success; 0 for failure to set;
		// -1 for failure to find a client node with specified name
		virtual int _stdcall _ccb_SetArg_onRecvData(const char *pClientName, // name of the targeted client node
			                                        void *pArgORD) = 0;
		// ���ý������ݴ�������ָ��
		// �ɹ�����1; ����ʧ�ܷ���0; �ͻ��˽ڵ��������ڷ���-1
		// Set callback function pointer to process receiving buffer.
		// The function pointer will be used to process received content and retain unprocessed content.
		// Return 1 for success; 0 for failure to set;
		// -1 for failure to find a client node with specified name
		virtual int _stdcall _ccb_SetFunc_onRecvData(const char *pClientName, // name of the targeted client node
			                                         CALLBACK_BUF2PROCESS pFuncORD) = 0;
		// ���ÿͻ��˶���֤���ɺ����Ĳ���(void*)
		// �ɹ�����1; ����ʧ�ܷ���0; �ͻ��˽ڵ��������ڷ���-1
		// Set argument for Read Auth. generator.
		// Return 1 for success; 0 for failure to set;
		// -1 for failure to find a client node with specified name
		virtual int _stdcall _ccb_SetArg_RAGenerator(const char *pClientName, // name of the targeted client node
			void *pArgRAG) = 0;
		// ���ÿͻ��˶���֤���ɺ�����ָ��
		// �ɹ�����1; ����ʧ�ܷ���0; �ͻ��˽ڵ��������ڷ���-1
		// Set callback function pointer to generate Read Auth. (Client->Server, validated by servers).
		// Return 1 for success; 0 for failure to set;
		// -1 for failure to find a client node with specified name
		virtual int _stdcall _ccb_SetFunc_RAGenerator(const char *pClientName, // name of the targeted client node
			                                          CALLBACK_VecBytesFill pFuncRAG) = 0;
		// ���ÿͻ���д��֤�����Ĳ���(void*)
		// �ɹ�����1; ����ʧ�ܷ���0; �ͻ��˽ڵ��������ڷ���-1
		// Set argument for Write Auth. validator.
		// Return 1 for success; 0 for failure to set;
		// -1 for failure to find a client node with specified name
		virtual int _stdcall _ccb_SetArg_WAValidator(const char *pClientName, // name of the targeted client node
			                                         void *pArgWAV) = 0;
		// ���ÿͻ���д��֤������ָ��
		// �ɹ�����1; ����ʧ�ܷ���0; �ͻ��˽ڵ��������ڷ���-1
		// Set callback function pointer to valid Write Auth. (Server->Client, validated by clients).
		// Return 1 for success; 0 for failure to set;
		// -1 for failure to find a client node with specified name
		virtual int _stdcall _ccb_SetFunc_WAValidator(const char *pClientName, // name of the targeted client node
			                                          CALLBACK_AuthValidator pFuncWAV) = 0;

		// *********************** �ڵ������й��� *********************** //
		// *********************** Node group running functionalities *********************** //

		// ���ýڵ������н���
		// �ڵ���ÿ��ѭ����IO�ȴ�ʱ��(����)������ʱ��(����)���
		// Set running tempo of this node group. One iteration is composed of I/O time (millseconds)
		// plus sleep time (milliseconds).
		virtual bool _stdcall SetLoopTempo(int ioTime_ms, int sleepTime_ms) = 0;
		// �����߳��п�ʼ��ѭ���ڵ�ͨ��, Ҳ��DDRLNN����ĵĹ���
		// Start a big loop of socket communications in a sub-thread. The thread will periodically
		// check I/O status, publish/receive contents through server/client nodes, and call callback
		// functions when certain conditions are met.
		virtual bool _stdcall StartRunning() = 0;
		// ������ѭ�����ȴ����̷߳���
		// End the big loop and wait for the sub-thread to finish.
		virtual bool _stdcall StopRunning() = 0;
		// ͨ���������ڵ㷢������
		// �ɹ�����1; ����ʧ�ܷ���0; �������ڵ��������ڷ���-1
		// Try to pulish something through a certain server node.
		// Return 1 for success; 0 for failure to publish;
		// -1 for failure to find a server node with specified name
		virtual int _stdcall PublishContent(const char *pServerName, // name of the targeted server node
			                                const void *pContent, int nContentLen) = 0;
		// ��ȡ�ϴη������˳ɹ����������ʱ��(����)
		// ����������ڵ��������ڷ���-1
		// return time (in milliseconds) since last successful publication through a certain node;
		// return -1 for failure to find a server node with specified name
		virtual __int64 _stdcall GetTimeSinceLastPublication(const char *pServerName // name of the targeted server node
			                                                 ) const = 0; // in milliseconds
		// ͨ���ͻ��˽ڵ��ȡ��������
		// pDataBytesΪ���ڽ��յĻ�����ͷ, nBufCapacityΪ�������������
		// �ɹ���ȡ��pDataBytes�������Ϊ��ȡ������, *pnRemBytes���Ϊ�ڲ�������ʣ��
		// �ֽ���, �������ػ�ȡ���ݵĳ���(0������); ����ͻ��˽ڵ��������ڷ���-1
		// Try to get received data from a certain client node.
		// Return -1 for failure to find such a client node (by name);
		// otherwise fill the buffer with content and return actual
		// number of bytes filled (<= bufCapacity guaranteed), and 
		// the unsigned integer pointed by pnRemBytes will be assigned
		// the number of bytes left in the unprocessed internal buffer
		// When nothing is available, return 0.
		virtual int _stdcall GetData(const char *pClientName, // name of the targeted client node
			                         void *pDataBytes, unsigned int nBufCapacity,
									 unsigned int *pnRemBytes // to receive number of remaining bytes
									 ) = 0;
		// ��ȡ�ϴοͻ��˻�ȡ�������������ʱ��(����)
		// ����ͻ��˽ڵ��������ڷ���-1
		// return time (in milliseconds) since last successful reception of data through a certain
		//client node. Return -1 for failure to find a client node with specified name
		virtual __int64 _stdcall GetTimeSinceLastReception(const char *pClientName // name of the targeted client node
			                                              ) const = 0; // in milliseconds

		// *********************** Node info and status *********************** //

		// Return -1 for failure to find such a node;
		// Return 0 for structured data; 1 for streamed data.
		virtual int _stdcall IsStreamed(const char *pNodeName, bool bServer) const = 0;
		// Return -1 for failure to find such a node;
		// Return 0 for UDP node; 1 for TCP node.
		virtual int _stdcall IsTCPBased(const char *pNodeName, bool bServer) const = 0;
		// Return -1 for failure to find such a node;
		// Return 0 for no authorization; 1 for Read Auth.; 2 for Write Auth..
		virtual int _stdcall GetAuthType(const char *pNodeName, bool bServer) const = 0;
		// Return -1 for failure to find such a server node;
		// Return 0 if server has not been established; return 1 otherwise.
		virtual int _stdcall IsServerEstablished(const char *pServerNode) const = 0;
		// Return -1 for UDP servers;
		// return actual connected clients for TCP servers.
		virtual int _stdcall GetNumOfConnections(const char *pServerNode) const = 0;
		// Return -1 for failure to find such a client node;
		// Return 0 if client is not connected; return 1 otherwise.
		virtual int _stdcall IsClientConnected(const char *pClientNode) const = 0;

		virtual void _stdcall destroy() = 0;
		void operator delete(void *p) {
			if (p != nullptr) {
				((LNNNodeGroupInterface*)p)->destroy();
			}
		}
	};

	// ���ô˺����ڶ��ϴ���LNN�ڵ������, ע���䷵�ص�����ָ��, �������
	// "LNNFactory.h"�ļ�ʹ������ָ���װ
	// Call this factory function to create a node group object (raw pointer
	// on heap). Recommend to include "LNNFactory.h" to use smare pointer
	// to wrap around it.
	extern "C" __API__DDRLNN__ LNNNodeGroupInterface* _stdcall _createLNNNodeGroup();

	// ���ô˺����ı�std::vector<BYTE>�Ĵ�С, ע��˺�����Ҫ���ô�����
	// �û��Զ���Ļص�������, ȷ��std::vector�����İ�ȫ
	// Call this function to resize std::vector<BYTE> (in form of its pointer converted to void*)
	// to ensure safety across DLL boundary
	extern "C" __API__DDRLNN__ void _stdcall ResizeVecBytes(void *pVecBytes, unsigned int szNew);
	// ���ô˺�����ȡstd::vector<BYTE>��������ͷ��ַ, ע��˺�����Ҫ���ô�����
	// �û��Զ���Ļص�������, ȷ��std::vector�����İ�ȫ
	// Call this function to get head address of the buffer managed by std::vector<BYTE>
	// (in form of its pointer converted to void*) to ensure safety across DLL boundary
	extern "C" __API__DDRLNN__ void* _stdcall GetVecBytesHead(void *pVecBytes);
}

#endif // __DDR_LNN_LIB_NODEGROUP_INTERFACE_H_INCLUDED__