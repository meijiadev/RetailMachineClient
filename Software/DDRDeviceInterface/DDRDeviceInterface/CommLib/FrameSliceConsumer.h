/**
Frame Slice Consumer of the DADAO Network Communication Libraries.
Mar 2018, Jianrui Long @ DADAOII 

This module is used to check and integrate size-limited data packets to form
complete frames.

Usage:
	1. Construct with maximum slice length (in bytes), frame numbering cap (e.g, 65536), and maximum
	number of in-process frame in the buffer.
	2. In a loop, call FeedOneSlice() with a slice of byte data and its length, and check if a
	complete frame is available by calling GetOneFrame().
	Example:
		std::vector<BYTE> frame;
		int frameInd;
		while (...AcquireNewSlice(pSlice, nSliceLen)) {
			FooFSC.FeedOneSlice(pSlice, nSliceLen);
			if (FooFSC.GetOneFrame(frame, &frameInd)) {
				...DoSomethingWith(frame, frameInd);
			}
		}

	3. This process (1-2) can be repeated, restarted, or terminated anyhow
	Frame numbering strategy:
		0, 1, 2, ..., nCap-1, 0, 1, ..., nCap-1, ...

Frame numbering order:
	0, 1, 2, ..., nCap-1, 0, 1, ..., nCap-1, ...
Slice format:
	1. Mixed slice - 'M' + 'M' + frame index (4 bytes, encoded) + # of slices within this frame
	(4 bytes, encoded) + # of total DATA bytes within this frame (4 bytes, encoded) + slice
	index (4 bytes, encoded) + # of DATA bytes within this slice (N, 4 bytes, encoded) + N
	bytes data + XOR check sum. SLICE LENGTH (23 + N) BYTES. Generally the slice index should
	be zero.
	2. Data slice - 'D' + 'D' + frame index (4 bytes, encoded) + slice index (4 bytes, encoded)
	+ # of DATA bytes within this slice (N, 4 bytes, encoded) + N bytes data + XOR check sum.
	SLICE LENGTH (15 + N) BYTES.
	3. For a single frame, a mixed slice is generated first, followed by data slices.
*/

#ifndef __DDCOMMLIB_FRAME_SLICE_CONSUMER_H_INCLUDED__
#define __DDCOMMLIB_FRAME_SLICE_CONSUMER_H_INCLUDED__

#include "CommLibInterface.h"

#include <vector>
#include "CommonDef/DeleteUnwrapper.h"

namespace DDRCommLib
{
	class SliceContainer
	{
	public:
		SliceContainer(int nReserveSliceLen = DDCL_FSG_DEFAULT_MAX_SLICE_LENGTH,
			           int nReserveSlices = 50);
		void Reset();
		bool FeedInfo(int frameInd, int nSlices, int nDataBytes);
		bool FeedData(int frameInd, int sliceInd, const BYTE *pData, int nDataLen);

		int GetFrameInd() const { return m_frameInd; }
		int GetSkippedSlices() const { return m_nSliceSkipped; }
		void IncSkippedSlice() { ++m_nSliceSkipped; }
		bool IsComplete() const;
		bool GetCompleteData(std::vector<BYTE> &dataVec) const;

		int m_frameInd;
		int m_nSlicesInFrame;
		int m_nDataBytesInFrame;
		int m_nSlicesReceived;
		int m_nDataBytesReceived;
		int m_reservedSliceDatalen;
		int m_nSliceSkipped;

	protected:
		typedef struct _oneSlice {
			int sliceInd;
			std::vector<BYTE> data;
		} oneSlice;
		std::vector<oneSlice> m_slices;

		// find a place to insert a unique index
		// return -1 if it has duplicate; otherwise, return the index to insert
		int findInsertionPos(int sliceInd2ins);
	};

	class FrameSliceConsumerImpl : public DDRCommonDef::DefaultDelete<FrameSliceConsumer>
	{
	public:
		FrameSliceConsumerImpl(int maxSliceLength = DDCL_FSG_DEFAULT_MAX_SLICE_LENGTH,
			                   int frameNumberingCap = DDCL_FSG_DEFAULT_FRAME_NUMBERING_CAP,
							   int numFramesInBuffer = DDCL_FSC_DEFAULT_NUM_FRAMES_IN_BUFFER) {
			setParam(maxSliceLength, frameNumberingCap, numFramesInBuffer);
		}
		~FrameSliceConsumerImpl();

		bool _stdcall FeedOneSlice(const BYTE *pSlice, int nSliceLen) override;
		int _stdcall GetCompleteFrame(BYTE *pFrameBuf, unsigned int bufCapacity,
			                          int *pFrameInd, unsigned int *pTotalBytes) override;
		void _stdcall destroy() override {
			delete this;
		}

	protected:
		int m_MaxSliceLen;
		int m_FrameNumberingCap;
		int m_numFramesInBuffer;
		std::vector<SliceContainer*> m_frames;
		std::vector<SliceContainer*> m_spareFrames;
		std::vector<BYTE> m_oneFrame;

		void setParam(int maxSliceLength, int frameNumberingCap, int numFramesInBuffer);
		int getDiffB2A(int aa, int bb);
		int try2find(int frameInd);
	};

	FrameSliceConsumer* _createFSC(int maxSliceLength,
		                           int frameNumberingCap,
								   int numFramesInBuffer) {
		return (new FrameSliceConsumerImpl(maxSliceLength, frameNumberingCap, numFramesInBuffer));
	}
}

#endif
