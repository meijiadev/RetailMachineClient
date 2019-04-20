#include "stdafx.h"

#include "SocketWrapper.h"

#pragma comment (lib,"ws2_32.lib")

namespace DDRLNN {

SocketWrapper::SocketWrapper(SOCKET soc) {
	if (soc != INVALID_SOCKET) {
		m_pRef = new __refInfo;
		m_pRef->soc = soc;
		m_pRef->nRefCnt = 1;
	} else {
		m_pRef = nullptr;
	}
}

SocketWrapper::SocketWrapper(const SocketWrapper &oriSW) {
	m_pRef = oriSW.m_pRef;
	if (m_pRef) {
		++(m_pRef->nRefCnt);
	}
}

SocketWrapper& SocketWrapper::operator= (const SocketWrapper &oriSW) {
	Close();
	m_pRef = oriSW.m_pRef;
	if (m_pRef) {
		++(m_pRef->nRefCnt);
	}
	return (*this);
}

SocketWrapper& SocketWrapper::operator= (SOCKET soc) {
	Close();
	if (soc != INVALID_SOCKET) {
		m_pRef = new __refInfo;
		m_pRef->soc = soc;
		m_pRef->nRefCnt = 1;
	}
	return (*this);
}

void SocketWrapper::Close() {
	if (m_pRef) {
		if (--(m_pRef->nRefCnt) == 0) {
			closesocket(m_pRef->soc);
			delete m_pRef;
		}
		m_pRef = nullptr;
	}
}

bool SocketWrapper::IsValid() const {
	return (m_pRef != nullptr);
}

SOCKET SocketWrapper::Socket() {
	if (m_pRef) {
		return (m_pRef->soc);
	} else { return INVALID_SOCKET; }
}

}