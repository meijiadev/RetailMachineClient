#ifndef __OFFSET_CORRECTOR_H_INCLUDED__
#define __OFFSET_CORRECTOR_H_INCLUDED__

#include "CommonDef/Types.h"

namespace DDRDevice
{
	struct OffsetMeasurement
	{
		int16_t gyro_z;
		uint32_t timeStamp;
		OffsetMeasurement(int16_t gz, uint32_t ts)
			: gyro_z(gz), timeStamp(ts) {}
		OffsetMeasurement() {}
	};

	class OffsetCorrector
	{
	public:
		OffsetCorrector();
		~OffsetCorrector();
		void ClearCollectedData(bool bClearCalibResult);
		bool GetOffset_GyroZ(float &offset) const;
		void AddMeasurement(const OffsetMeasurement &om);
		void AddMeasurement(int16_t gz, uint32_t timeStamp);
		inline bool IsInitialized() const { return m_bInitialized; }

	protected:
		int m_nLen;
		int m_nLenPlusOne;
		int m_nMaxTimeGap;

		int m_sumGyroZ;

		bool m_bInitialized;
		bool m_bAvailable;
		float m_Offset_GyroZ;

		OffsetMeasurement *m_pMQueue;
		int m_nQueueHead;
		int m_nQueueTail;
	};
}

#endif