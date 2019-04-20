#include "stdafx.h"

#include "FrameSliceConsumer.h"

#include "CommLibInterface.h"

namespace DDRCommLib {

SliceContainer::SliceContainer(int nReserveSliceLen, int nReserveSlices) {
	if (nReserveSliceLen < 100)
		nReserveSliceLen = 100;
	if (nReserveSlices < 10)
		nReserveSlices = 10;
	m_reservedSliceDatalen = nReserveSliceLen;
	m_slices.resize(nReserveSlices);
	for (int i = 0; i < nReserveSlices; ++i) {
		m_slices[i].data.reserve(nReserveSliceLen);
	}
	Reset();
}

void SliceContainer::Reset() {
	m_frameInd = -1;
	m_nSliceSkipped = 0;
}

bool SliceContainer::FeedInfo(int frameInd, int nSlices, int nDataBytes) {
	if (m_frameInd == -1) {
		// new frame, new info
		m_frameInd = frameInd;
		m_nSlicesInFrame = nSlices;
		m_nDataBytesInFrame = nDataBytes;
		m_nSlicesReceived = 0;
		m_nDataBytesReceived = 0;
	} else {
		// frame index already set
		if (frameInd != m_frameInd) { return false; } // different frame index
		if (m_nSlicesInFrame != -1) { return false; } // info already received
		// data frame came before info
		m_nSlicesInFrame = nSlices;
		m_nDataBytesInFrame = nDataBytes;
		m_nDataBytesReceived = 0;
		for (int i = m_nSlicesReceived - 1; i >= 0; --i) {
			if (m_slices[i].sliceInd < nSlices)	{
				m_nDataBytesReceived += (int)m_slices[i].data.size();
			} else {
				// this slice out of index
				m_slices[i].sliceInd = -1;
				--m_nSlicesReceived;
			}
		}
	}
	return true;
}

bool SliceContainer::FeedData(int frameInd, int sliceInd,
	                          const BYTE *pData, int nDataLen) {
	if (m_frameInd != -1) {
		// frame index already set
		if (frameInd != m_frameInd) { return false; } // different frame index
		if (m_nSlicesInFrame != -1 && sliceInd >= m_nSlicesInFrame) {
			return false; // out of index
		}
		int ind2ins = findInsertionPos(sliceInd);
		if (-1 == ind2ins) { return false; }
		if (m_nSlicesReceived + 1 > (int)m_slices.size()) {
			// need to add more slots
			int oldSz = (int)m_slices.size();
			int newSz = oldSz + (oldSz >> 1);
			if (newSz < m_nSlicesReceived + 5) {
				newSz = m_nSlicesReceived + 5;
			}
			m_slices.resize(newSz);
		}
		// move [ind2ins, nReceived-1] to [ind2ins + 1, nReceived]
		for (int i = m_nSlicesReceived; i > ind2ins; --i) {
			m_slices[i].sliceInd = m_slices[i - 1].sliceInd;
			m_slices[i].data.swap(m_slices[i-1].data);
		}
		// insert new slice at ind2ins
		m_slices[ind2ins].sliceInd = sliceInd;
		m_slices[ind2ins].data.resize(nDataLen);
		memcpy(&(m_slices[ind2ins].data[0]), pData, nDataLen);
		++m_nSlicesReceived;
		if (m_nSlicesInFrame != -1) {
			m_nDataBytesReceived += nDataLen;
		}
	} else {
		// no previous data slice or info
		m_frameInd = frameInd;
		m_nSlicesInFrame = -1;
		m_slices[0].sliceInd = sliceInd;
		m_slices[0].data.resize(nDataLen);
		memcpy(&(m_slices[0].data[0]), pData, nDataLen);
		m_nSlicesReceived = 1;
	}
	return true;
}

bool SliceContainer::IsComplete() const {
	return (m_frameInd != -1 && m_nSlicesInFrame != -1 &&
		m_nSlicesReceived == m_nSlicesInFrame &&
		m_nDataBytesReceived == m_nDataBytesInFrame);
}

bool SliceContainer::GetCompleteData(std::vector<BYTE> &dataVec) const {
	if (!IsComplete()) { return false; }
	dataVec.resize(m_nDataBytesInFrame);
	int pos = 0;
	for (int i = 0; i < m_nSlicesInFrame; ++i) {
		memcpy(&dataVec[pos], &(m_slices[i].data[0]), m_slices[i].data.size());
		pos += m_slices[i].data.size();
	}
	return true;
}

int SliceContainer::findInsertionPos(int sliceInd2ins) {
	if (0 == m_nSlicesReceived) { return 0; }
	if (sliceInd2ins > m_slices[m_nSlicesReceived - 1].sliceInd) {
		return m_nSlicesReceived; }
	if (sliceInd2ins == m_slices[0].sliceInd ||
		sliceInd2ins == m_slices[m_nSlicesReceived - 1].sliceInd) {
		return -1;
	}
	int aa = 0, bb = m_nSlicesReceived - 1;
	while (1) {
		int cc = (aa + bb) >> 1;
		if (sliceInd2ins == m_slices[cc].sliceInd) { return -1; }
		if (sliceInd2ins > m_slices[cc].sliceInd) {
			aa = cc;
		} else {
			bb = cc;
		}
		if (aa + 1 == bb) { return bb; }
	}
	return -1;
}

void FrameSliceConsumerImpl::setParam(int maxSliceLength, int frameNumberingCap,
	                                  int numFramesInBuffer) {
	if (maxSliceLength < 100) {
		maxSliceLength = 100;
	}
	if (frameNumberingCap < 1024) {
		frameNumberingCap = 1024;
	} else if (frameNumberingCap > 0xFFFFFF) {
		frameNumberingCap = 0xFFFFFF;
	}
	numFramesInBuffer = (numFramesInBuffer <= 0 ? 1 :
		                 (numFramesInBuffer > 100 ? 100 : numFramesInBuffer));
	m_MaxSliceLen = maxSliceLength;
	m_FrameNumberingCap = frameNumberingCap;
	m_numFramesInBuffer = numFramesInBuffer;
	m_spareFrames.resize(numFramesInBuffer);
	for (int i = 0; i < numFramesInBuffer; ++i)	{
		m_spareFrames[i] = new SliceContainer();
	}
	m_oneFrame.reserve(1000);
	m_oneFrame.resize(0);
}

bool FrameSliceConsumerImpl::FeedOneSlice(const BYTE *pSlice, int nSliceLen) {
	if (nullptr == pSlice || nSliceLen <= 15 || nSliceLen > m_MaxSliceLen) {
		return false;
	}
	if (!((pSlice[0] == (BYTE)'M' && pSlice[1] == (BYTE)'M') ||
		 (pSlice[0] == (BYTE)'D' && pSlice[1] == (BYTE)'D'))) {
		return false;
	}
	BYTE cs = 0;
	for (int i = 0; i < nSliceLen - 1; ++i)	{
		cs ^= pSlice[i];
	}
	if (pSlice[nSliceLen - 1] != cs) { return false; }
	if (pSlice[0] == 'M') {
		int nDataLen = nSliceLen - 23;
		if (nDataLen <= 0) { return false; }
		int fId, nSlicesInFrame, nBytesInFrame, sId, sBytes;
		if (!DDRCommLib::VerifyDataLen_32(pSlice + 2, &fId)) { return false; }
		if (!DDRCommLib::VerifyDataLen_32(pSlice + 6, &nSlicesInFrame)) { return false; }
		if (!DDRCommLib::VerifyDataLen_32(pSlice + 10, &nBytesInFrame)) { return false; }
		if (!DDRCommLib::VerifyDataLen_32(pSlice + 14, &sId)) { return false; }
		if (!DDRCommLib::VerifyDataLen_32(pSlice + 18, &sBytes)) { return false; }
		if (sBytes != nDataLen) { return false; }
		int indInVec = try2find(fId);
		for (int i = 0; i < indInVec; ++i) {
			m_frames[i]->IncSkippedSlice();
		}
		for (int i = indInVec + 1; i < (int)m_frames.size(); ++i) {
			m_frames[i]->IncSkippedSlice();
		}
		if (indInVec == -1) { return false; }
		if (!m_frames[indInVec]->FeedInfo(fId, nSlicesInFrame,
			                              nBytesInFrame)) {
			return false;
		}
		if (!m_frames[indInVec]->FeedData(fId, sId,
			                              pSlice + 22, nDataLen)) {
			return false;
		}
	} else if (pSlice[0] == 'D') {
		int nDataLen = nSliceLen - 15;
		if (nDataLen <= 0) { return false; }
		int fId, sId, sBytes;
		if (!DDRCommLib::VerifyDataLen_32(pSlice + 2, &fId)) { return false; }
		if (!DDRCommLib::VerifyDataLen_32(pSlice + 6, &sId)) { return false; }
		if (!DDRCommLib::VerifyDataLen_32(pSlice + 10, &sBytes)) { return false; }
		if (sBytes != nDataLen) { return false; }
		int indInVec = try2find(fId);
		if (indInVec == -1) { return false; }
		if (!m_frames[indInVec]->FeedData(fId, sId, pSlice + 14, nDataLen)) {
			return false;
		}
	}
	return true;
}

int FrameSliceConsumerImpl::getDiffB2A(int aa, int bb) {
	int diff = bb - aa;
	if (diff >= (m_FrameNumberingCap >> 1)) {
		diff -= m_FrameNumberingCap;
	}
	return diff;
}

int FrameSliceConsumerImpl::try2find(int frameInd) {
	if (m_frames.empty()) {
		SliceContainer *pSC = m_spareFrames.back();
		m_spareFrames.pop_back();
		pSC->Reset();
		m_frames.push_back(pSC);
		return 0;
	}
	for (int i = (int)m_frames.size() - 1; i >= 0; --i)	{
		int diff = getDiffB2A(m_frames[i]->GetFrameInd(), frameInd);
		if (diff == 0) { return i; }
		if (diff > 0) {
			if ((int)m_frames.size() == m_numFramesInBuffer) {
				SliceContainer *pSC = m_frames[0];
				pSC->Reset();
				for (int j = 0; j < i; ++j)	{
					m_frames[j] = m_frames[j + 1];
				}
				m_frames[i] = pSC;
				return i;
			} else {
				m_frames.push_back(nullptr);
				for (int j = (int)m_frames.size() - 1; j > i + 1; --j) {
					m_frames[j] = m_frames[j - 1];
				}
				SliceContainer *pSC = m_spareFrames.back();
				m_spareFrames.pop_back();
				pSC->Reset();
				m_frames[i + 1] = pSC;
				return (i + 1);
			}
		}
	}
	return (-1);
}

int FrameSliceConsumerImpl::GetCompleteFrame(BYTE *pFrameBuf, unsigned int bufCapacity,
	                                         int *pFrameInd, unsigned int *pTotalBytes) {
	if (pTotalBytes) {
		*pTotalBytes = 0;
	}
	if (m_oneFrame.size() > 0) {
		if (pTotalBytes) {
			*pTotalBytes = m_oneFrame.size();
		}
		if (bufCapacity < m_oneFrame.size()) {
			return -1;
		} else {
			size_t sz = m_oneFrame.size();
			memcpy(pFrameBuf, &m_oneFrame[0], sz);
			m_oneFrame.resize(0);
			return (int)sz;
		}
	}
	for (int i = (int)m_frames.size() - 1; i >= 0; --i) {
		if (m_frames[i]->GetCompleteData(m_oneFrame)) {
			if (pTotalBytes) {
				*pTotalBytes = m_oneFrame.size();
			}
			if (bufCapacity < m_oneFrame.size()) {
				return -1;
			}
			if (pFrameInd != nullptr) {
				*pFrameInd = m_frames[i]->GetFrameInd();
			}
			for (int j = 0; j <= i; ++j) {
				m_frames[j]->Reset();
				m_spareFrames.push_back(m_frames[j]);
			}
			for (int j = 0; j < (int)m_frames.size() - i - 1; ++j) {
				m_frames[j] = m_frames[j + i + 1];
			}
			m_frames.resize((int)m_frames.size() - i - 1);
			size_t sz = m_oneFrame.size();
			memcpy(pFrameBuf, &m_oneFrame[0], sz);
			m_oneFrame.resize(0);
			return (int)sz;
		}
		if (m_frames[i]->GetSkippedSlices() > DDCL_FSC_DEFAULT_FRAME_IGNORED_FOR_NUM_SLICES_SKIPPED) {
			m_frames[i]->Reset();
			m_spareFrames.push_back(m_frames[i]);
			for (int j = i + 1; j < (int)m_frames.size(); ++j) {
				m_frames[j - 1] = m_frames[j];
			}
			m_frames.pop_back();
		}
	}
	return 0;
}

FrameSliceConsumerImpl::~FrameSliceConsumerImpl() {
	for (int i = 0; i < (int)m_frames.size(); ++i) {
		delete m_frames[i];
	}
	for (int i = 0; i < (int)m_spareFrames.size(); ++i) {
		delete m_spareFrames[i];
	}
}

}