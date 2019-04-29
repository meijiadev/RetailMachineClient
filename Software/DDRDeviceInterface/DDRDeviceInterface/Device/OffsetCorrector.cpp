#include "OffsetCorrector.h"

//#include "Gadgets/ParamStorage.h"

namespace DDRDevice {

OffsetCorrector::OffsetCorrector() {
	m_bInitialized = false;
	m_pMQueue = nullptr;
	// load configurations
	m_nLen = 200;
	m_nMaxTimeGap = 100;

	//bool bRead[2];
	//bRead[0] = DDRGadgets::g_Params.ReadParam("IMUWheel_Params.NUM_OF_FRAMES_TO_CALIBRATE",
	//	                                      m_nLen);
	//bRead[1] = DDRGadgets::g_Params.ReadParam("IMUWheel_Params.MAX_AVG_INTERVAL_BETWEEN_FRAMES_MS",
	//	                                      m_nMaxTimeGap);
	//if (!bRead[0] || !bRead[1])	{
	//	std::cout << "致命错误@OffsetCorrector::OffsetCorrector(): 无法从配置文件中正确载入参数!\n";
	//	return;
	//}

	m_nMaxTimeGap *= m_nLen;
	m_nLenPlusOne = (m_nLen + 1);
	m_pMQueue = new OffsetMeasurement[m_nLenPlusOne];
	ClearCollectedData(true);

	m_bInitialized = true;
}

OffsetCorrector::~OffsetCorrector() {
	if (m_pMQueue) {
		delete[] m_pMQueue;
		m_pMQueue = nullptr;
	}
}

void OffsetCorrector::ClearCollectedData(bool bClearCalibResult) {
	if (bClearCalibResult) {
		m_bAvailable = false;
	}
	m_sumGyroZ = 0;
	m_nQueueHead = m_nQueueTail = 0;
}

void OffsetCorrector::AddMeasurement(const OffsetMeasurement &om) {
	int curLen = (m_nQueueTail - m_nQueueHead + m_nLenPlusOne) % m_nLenPlusOne;
	if (curLen == m_nLen) {
		m_sumGyroZ -= m_pMQueue[m_nQueueHead].gyro_z;
		m_nQueueHead = (m_nQueueHead + 1) % m_nLenPlusOne;
	}
	m_sumGyroZ += om.gyro_z;
	m_pMQueue[m_nQueueTail] = om;
	m_nQueueTail = (m_nQueueTail + 1) % m_nLenPlusOne;

	if (curLen >= m_nLen - 1 &&
		(int)(om.timeStamp - m_pMQueue[m_nQueueHead].timeStamp) <= m_nMaxTimeGap) {
		m_bAvailable = true;
		m_Offset_GyroZ = (m_sumGyroZ + 0.0f) / m_nLen;
	} else {
		m_bAvailable = false;
	}
}

void OffsetCorrector::AddMeasurement(int16_t gz, uint32_t timeStamp) {
	int curLen = (m_nQueueTail - m_nQueueHead + m_nLenPlusOne) % m_nLenPlusOne;
	if (curLen == m_nLen) {
		m_sumGyroZ -= m_pMQueue[m_nQueueHead].gyro_z;
		m_nQueueHead = (m_nQueueHead + 1) % m_nLenPlusOne;
	}
	m_sumGyroZ += gz;
	m_pMQueue[m_nQueueTail] = OffsetMeasurement(gz, timeStamp);
	m_nQueueTail = (m_nQueueTail + 1) % m_nLenPlusOne;

	if (curLen >= m_nLen - 1 &&
		(int)(timeStamp - m_pMQueue[m_nQueueHead].timeStamp) <= m_nMaxTimeGap) {
		m_bAvailable = true;
		m_Offset_GyroZ = (m_sumGyroZ + 0.0f) / m_nLen;
	} else {
		m_bAvailable = false;
	}
}


bool OffsetCorrector::GetOffset_GyroZ(float &offset) const {
	if (m_bAvailable) {
		offset = m_Offset_GyroZ;
		return true;
	} else { return false; }
}

}