#ifndef __FRAMESLICERS_ABSTRACT_INTERFACE_H_INCLUDED__
#define __FRAMESLICERS_ABSTRACT_INTERFACE_H_INCLUDED__

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <ws2ipdef.h>
#pragma comment (lib,"ws2_32.lib")

#ifndef BYTE
typedef unsigned char BYTE;
#endif

namespace DDRCommLib
{
	// encryption / decryption
	bool Txt_Encrypt(const void *pSrc, int lenSrc,
		                                 const void *pTar, int lenTar);
	bool Txt_Encrypt2(const void *pSrc1, int lenSrc1,
		                                  const void *pSrc2, int lenSrc2,
										  void *pTar, int lenTar);
	bool Txt_Decrypt(const void *pSrc, int lenSrc,
		                                 void *pTar, int lenTar);
	__int64 EncryptDataLen_64(unsigned int len);
	bool VerifyDataLen_64(const void *pDataHead,
		                                       unsigned int *plen);
	inline int SizeOfELen_64() { return 8; }
	int EncryptDataLen_32(int len);
	bool VerifyDataLen_32(const void *pDataHead,
		                                      int *plen);
	inline int SizeOfELen_32() { return 4; }

	// pStr points to an array holding at least 16 elements to be safe
	void ConvertUIP2Str_IPv4(unsigned long ip, char *pStr, int *pEndingPos = nullptr);
	bool ConvertStr2UIP_IPv4(const char *pStr, unsigned long &ip, char cDelimiter = '\0', int *pEndingPos = nullptr);
	bool ConvertStr2Int(const char *pStr, int &x, char cDelimiter = '\0', int *pEndingPos = nullptr);
	unsigned short ConvertHTONS(unsigned short u16val);
	
	const int DDCL_FSG_DEFAULT_MAX_SLICE_LENGTH = 1450;
	const int DDCL_FSG_DEFAULT_FRAME_NUMBERING_CAP = 65536;
	const int DDCL_FSC_DEFAULT_NUM_FRAMES_IN_BUFFER = 5;
	const int DDCL_FSC_DEFAULT_FRAME_IGNORED_FOR_NUM_SLICES_SKIPPED = 20;
	class FrameSliceGenerator {
	public:
		virtual bool _stdcall FeedOneFrame(const BYTE *pInputData, int nInputDataLen) = 0;

		// return -1 for error or INSUFFICIENT capacity; 0 for sending finished;
		// otherwise return actual length of next generated slice
		virtual int _stdcall GetOneSlice(BYTE *pRcvBuf, unsigned int bufCapacity) = 0;
		virtual void _stdcall destroy() = 0;
	};	
	FrameSliceGenerator* _createFSG(int maxSliceLength = DDCL_FSG_DEFAULT_MAX_SLICE_LENGTH,
	                                int frameNumberingCap = DDCL_FSG_DEFAULT_FRAME_NUMBERING_CAP);
	
	class FrameSliceConsumer {
	public:
		virtual bool _stdcall FeedOneSlice(const BYTE *pSlice, int nSliceLen) = 0;
		// return -1 for INSUFFICIENT capacity; 0 - for no complete frame available;
		// otherwise return actual length of a complete frame
		virtual int _stdcall GetCompleteFrame(BYTE *pFrameBuf, unsigned int bufCapacity,
			                                  int *pFrameInd = nullptr,
											  unsigned int *pTotalBytes = nullptr) = 0;
		virtual void _stdcall destroy() = 0;
	};
	FrameSliceConsumer* _createFSC(int maxSliceLength = DDCL_FSG_DEFAULT_MAX_SLICE_LENGTH,
		                           int frameNumberingCap = DDCL_FSG_DEFAULT_FRAME_NUMBERING_CAP,
		                           int numFramesInBuffer = DDCL_FSC_DEFAULT_NUM_FRAMES_IN_BUFFER);

}


#endif // __FRAMESLICERS_ABSTRACT_INTERFACE_H_INCLUDED__