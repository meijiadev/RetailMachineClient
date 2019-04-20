#ifndef __DDR_EMBEDDED_DATA_PROCESSOR_H_INCLUDED__
#define __DDR_EMBEDDED_DATA_PROCESSOR_H_INCLUDED__

#include <vector>
#include <string>
#include "LocalNetworkNode/LNNFactory.h"
#include "DDREmbConfig.h"
#include "GNSSAnalyzer.h"
#include "EmbServer.h"

namespace DDRDrivers
{
	class EmbUserBase
	{
	public:
		EmbUserBase();
		bool ReadEmbData();
		const BYTE *GetEmbMBBuffer() const { return m_MBFrame; }
		bool ReadGNSSData();
		bool GetGNSSData(double &dstLng, double &dstLat,
			             unsigned short &cog, float &altitude,
						 unsigned int &UNIXTimeStamp) {
			return m_UBloxData.GetGNSSData(dstLng, dstLat,
				                           cog, altitude,
										   UNIXTimeStamp);
		}
		void Stop() {
			m_pNodeGroup->StopRunning();
		}

	protected:
		int m_nState; // 0 - uninitialized; 1 - initialized and running
		
		std::shared_ptr<DDRLNN::LNNNodeGroupInterface> m_pNodeGroup;
		std::vector<BYTE> m_tmpVec;
		std::vector<BYTE> m_MBRcvBuf;
		std::vector<BYTE> m_UBloxRcvBuf;
		std::vector<std::vector<BYTE>*> m_bufPtrs;

		BYTE m_MBFrame[EMBEDDED_MB_FRAME_LENGTH];
		GNSSAnalyzer m_UBloxData;
		DDREmbeddedServer *m_pEmbServer;

		bool _receive(int clientID);
	};
}

#endif // __DDR_EMBEDDED_DATA_PROCESSOR_H_INCLUDED__
