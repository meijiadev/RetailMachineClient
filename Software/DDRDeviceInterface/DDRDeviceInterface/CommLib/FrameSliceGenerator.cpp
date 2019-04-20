#include "stdafx.h"

#include "FrameSliceGenerator.h"

#include <string>
#include "CommLibInterface.h"

namespace DDRCommLib {

void FrameSliceGeneratorImpl::setParam(int maxSliceLength, int frameNumberingCap) {
	if (maxSliceLength < 100) {
		maxSliceLength = 100;
	}
	if (frameNumberingCap < 1024) {
		frameNumberingCap = 1024;
	} else if (frameNumberingCap > 0xFFFFFF) {
		frameNumberingCap = 0xFFFFFF;
	}
	m_MaxSliceLen = maxSliceLength;
	m_FrameNumberingCap = frameNumberingCap;
	m_curFrameNo = -1;
	m_frameStage = 2;
}

bool FrameSliceGeneratorImpl::FeedOneFrame(const BYTE *pInputData,
	                                       int nInputDataLen) {
	if (nInputDataLen <= 0 ||
		nInputDataLen >= 255 * 65536 ||
		nullptr == pInputData) {
		return false;
	}
	m_frameStage = 0;
	m_frameCurDataPos = 0;
	m_curFrameNo = (m_curFrameNo + 1) % m_FrameNumberingCap;
	m_pFrameData = pInputData;
	m_nFrameDataLen = nInputDataLen;
	return true;
}

int FrameSliceGeneratorImpl::GetOneSlice(BYTE *pRcvBuf, unsigned int bufCapacity) {
	int nDataBytesThisSlice, nSlices;
	BYTE chkS = 0;
	switch (m_frameStage) {
	case 0: // frame pointed, nothing sent
		if (m_nFrameDataLen + 23 > m_MaxSliceLen) {
			// (m_MaxSliceLen-23) + (m_MaxSliceLen-15) * (m-2) <
			// m_nFrameDataLen <=
			// (m_MaxSliceLen-23) + (m_MaxSliceLen-15) * (m-1)
			// ==>> (m_MaxSliceLen-15) * (m-1) <
			// m_nFrameDataLen + 8 <=
			// (m_MaxSliceLen-15) * m
			nSlices = (m_nFrameDataLen + 8 + (m_MaxSliceLen - 16))
				      / (m_MaxSliceLen - 15);
			nDataBytesThisSlice = m_MaxSliceLen - 23;
			m_frameStage = 1;
		} else {
			nSlices = 1;
			nDataBytesThisSlice = m_nFrameDataLen;
			m_frameStage = 2;
		}
		m_curSliceNo = 1;
		m_frameCurDataPos = nDataBytesThisSlice;
		if (23 + nDataBytesThisSlice > (int)bufCapacity) { return -1; }
		pRcvBuf[0] = pRcvBuf[1] = (BYTE)'M';
		*((int*)(pRcvBuf + 2)) = DDRCommLib::EncryptDataLen_32(m_curFrameNo);
		*((int*)(pRcvBuf + 6)) = DDRCommLib::EncryptDataLen_32(nSlices);
		*((int*)(pRcvBuf + 10)) = DDRCommLib::EncryptDataLen_32(m_nFrameDataLen);
		*((int*)(pRcvBuf + 14)) = DDRCommLib::EncryptDataLen_32(0);
		*((int*)(pRcvBuf + 18)) = DDRCommLib::EncryptDataLen_32(nDataBytesThisSlice);
		memcpy(pRcvBuf + 22, m_pFrameData, nDataBytesThisSlice);
		for (int i = 0; i < 22 + nDataBytesThisSlice; ++i) {
			chkS ^= (BYTE)pRcvBuf[i];
		}
		pRcvBuf[22 + nDataBytesThisSlice] = chkS;
		return (23 + nDataBytesThisSlice);

	case 1: // in process of sending data slices
		if (m_frameCurDataPos + (m_MaxSliceLen - 15) < m_nFrameDataLen) {
			nDataBytesThisSlice = m_MaxSliceLen - 15;
		} else {
			nDataBytesThisSlice = m_nFrameDataLen - m_frameCurDataPos;
			m_frameStage = 2;
		}
		m_frameCurDataPos += nDataBytesThisSlice;
		if (15 + nDataBytesThisSlice > (int)bufCapacity) { return -1; }
		pRcvBuf[0] = pRcvBuf[1] = 'D';
		*((int*)(pRcvBuf + 2)) = DDRCommLib::EncryptDataLen_32(m_curFrameNo);
		*((int*)(pRcvBuf + 6)) = DDRCommLib::EncryptDataLen_32(m_curSliceNo);
		*((int*)(pRcvBuf + 10)) = DDRCommLib::EncryptDataLen_32(nDataBytesThisSlice);
		memcpy(pRcvBuf + 14,
			   m_pFrameData + m_frameCurDataPos - nDataBytesThisSlice,
			   nDataBytesThisSlice);
		for (int i = 0; i < 14 + nDataBytesThisSlice; ++i) {
			chkS ^= pRcvBuf[i];
		}
		pRcvBuf[14 + nDataBytesThisSlice] = chkS;
		++m_curSliceNo;
		return (15 + nDataBytesThisSlice);

	case 2: // sending finished
		return 0;
	}
	return -1;
}

}