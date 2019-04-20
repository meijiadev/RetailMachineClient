#ifndef __DDR_COMM_SOCKET_WRAPPER_H_INCLUDED__
#define __DDR_COMM_SOCKET_WRAPPER_H_INCLUDED__

#include <WinSock2.h>

namespace DDRLNN
{
	class SocketWrapper
	{
	public:
		SocketWrapper(SOCKET soc = INVALID_SOCKET);
		SocketWrapper(const SocketWrapper &oriSW);
		SocketWrapper& operator= (const SocketWrapper &oriSW);
		SocketWrapper& operator= (SOCKET soc);
		~SocketWrapper() { Close(); }
		void Close();
		bool IsValid() const;
		SOCKET Socket();

	private:
		struct __refInfo {
			SOCKET soc;
			int nRefCnt;
		};
		__refInfo *m_pRef;
	};
}

#endif // __DDR_COMM_SOCKET_WRAPPER_H_INCLUDED__
