/**
Frame Slice Generator of the DADAO Network Communication Libraries.
Mar 2018, Jianrui Long @ DADAOII

This module is used to generate size-limited data packets from cplete frames
to adapt for different network configurations

Usage:
	1. Construct with maximum slice length (in bytes) and frame numbering
	cap (e.g, 65536).
	2. Call FeedOneFrame() with a byte array to be sent.
	3. In a loop, call GetOneSlice().
	Example:
		char slice[MAX_SLICE_LENGTH];
		FooFSS.FeedOneFrame(pInputData, nInputDataLen);
		int n;
		while ((n = FooFSS.GetOneSlice(slice)) > 0) {
			...DoSomethingWith(slice, n);
		}

	4. This process (2-3) can be repeated, restarted, or terminated anyhow

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

#ifndef __DDCOMMLIB_FRAME_SLICE_GENERATOR_H_INCLUDED__
#define __DDCOMMLIB_FRAME_SLICE_GENERATOR_H_INCLUDED__

#include "CommLibInterface.h"
#include "CommonDef/DeleteUnwrapper.h"

namespace DDRCommLib
{
	class FrameSliceGeneratorImpl : public DDRCommonDef::DefaultDelete<FrameSliceGenerator>
	{
	public:
		FrameSliceGeneratorImpl(int maxSliceLength = DDCL_FSG_DEFAULT_MAX_SLICE_LENGTH,
			                    int frameNumberingCap = DDCL_FSG_DEFAULT_FRAME_NUMBERING_CAP) {
			setParam(maxSliceLength, frameNumberingCap);
		}
		bool _stdcall FeedOneFrame(const BYTE *pInputData, int nInputDataLen) override;
		// return -1 for error or INSUFFICIENT capacity; 0 for sending finished;
		// otherwise return actual length of next generated slice
		int _stdcall GetOneSlice(BYTE *pRcvBuf, unsigned int bufCapacity) override;
		void _stdcall destroy() override {
			delete this;
		}

	protected:
		void setParam(int maxSliceLength, int frameNumberingCap);
		int m_MaxSliceLen;
		int m_FrameNumberingCap;
		int m_curFrameNo;
		const BYTE *m_pFrameData;
		int m_nFrameDataLen;
		int m_frameStage; // 0 - received but nothing sent (mixed slice to be sent); 1 - sending data slices; 2 - finished sending or error
		int m_frameCurDataPos;
		int m_curSliceNo;
	};

	FrameSliceGenerator* _createFSG(int maxSliceLength, int frameNumberingCap) {
		return (new FrameSliceGeneratorImpl(maxSliceLength, frameNumberingCap));
	}
}

#endif
