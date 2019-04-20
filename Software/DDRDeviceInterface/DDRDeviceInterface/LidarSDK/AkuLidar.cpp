#include "AkuLidar.h"

#include <assert.h>

#if 1
#include "LidarSDK/lim.h"
#include "LidarSDK/LidarSDK_errno.h"

#ifndef _WIN64
#pragma comment(lib, "Drivers/LidarSDK/x86_r/LidarSDK.lib")
#else
#pragma comment(lib, "Drivers/LidarSDK/x64_r/LidarSDK.lib")
#endif

#endif
namespace DDRDrivers {

const int AKUSENSE_LIDAR_FILTEREDOUT_DISTANCE = 9999999; // range too far to be valid
const double AKUSENSE_LIDAR_TANTH_THRESHOLD = 0.2;
const int AKUSENSE_LIDAR_CLOSENESS2ORIGIN_THRESHOLD = 15; //cm
const float AKUSENSE_LIDAR_LINESEG_SIN_THRESHOLD = 0.2f;

bool Lidar_AkuSense::g_bStop = false;
std::mutex Lidar_AkuSense::g_Lidar_MUTEX;
int Lidar_AkuSense::g_nInstances = 0;
Lidar_AkuSense* Lidar_AkuSense::g_pSoleInstance = nullptr;

bool Lidar_AkuSense::_IsUnreliablePoint(float theta_2prev, float theta_2next,
	                                    float dist_prev, float dist_cur, float dist_next) {
	theta_2prev = fabsf(theta_2prev);
	theta_2next = fabsf(theta_2next);
	if (dist_cur * sinf(theta_2prev) > fabsf(dist_cur * cosf(theta_2prev))
		* AKUSENSE_LIDAR_TANTH_THRESHOLD) { return false; }
	float x1 = dist_cur * cosf(theta_2prev);
	float y1 = dist_cur * sinf(theta_2prev);
	float x2 = dist_next * cosf(theta_2prev + theta_2next);
	float y2 = dist_next * sinf(theta_2prev + theta_2next);
	float meanSQX = (dist_prev * dist_prev + x1 * x1 + x2 * x2) / 3.0f;
	float meanSQY = (y1 * y1 + y2 * y2) / 3.0f;
	float meanXY = (x1 * y1 + x2 * y2) / 3.0f;
	float meanX = (dist_prev + x1 + x2) / 3.0f;
	float meanY = (y1 + y2) / 3.0f;
	float D1 = (meanSQX - meanX * meanX + meanSQY - meanY * meanY) * 0.5f;
	float D2 = (meanSQX - meanX * meanX - meanSQY + meanY * meanY) * 0.5f;
	float D3 = meanXY - meanX * meanY;
	float theta = (atan2f(D3, D2) + PI) * 0.5f;
	float A = cosf(theta);
	float B = sinf(theta);
	float C = -meanX * A - meanY * B;
	return (fabsf(sinf(theta) - 1.0f) < AKUSENSE_LIDAR_LINESEG_SIN_THRESHOLD
		&& fabsf(C) < AKUSENSE_LIDAR_CLOSENESS2ORIGIN_THRESHOLD);
}


Lidar_AkuSense::Lidar_AkuSense() {
	m_bOkay = false;
	std::lock_guard<std::mutex> lock(g_Lidar_MUTEX);
	if (0 == g_nInstances) {
		m_lidarData.resize(0);
		m_bOkay = true;
		++g_nInstances;
		g_pSoleInstance = this;
	}
}

void Lidar_AkuSense::End() {
	if (m_bOkay) {
		_stop();
		--g_nInstances;
		g_pSoleInstance = nullptr;
		m_bOkay = false;
	}
}

void Lidar_AkuSense::_stop() {
	g_bStop = true;
	std::lock_guard<std::mutex> lock(g_Lidar_MUTEX);
	LIM_HEAD lh;
	lh.TAG = LIM_TAG;
	lh.VER = LIM_VER;
	lh.nCode = LIM_CODE_STOP_LMD;
	lh.nLIMLen = sizeof(LIM_HEAD);
	for (int i = 0; i < (int)m_AkuLidars.size(); ++i)
	{
		lh.nCID = m_AkuLidars[i]._nCID;
		Sleep(100);
	}
	m_AkuLidars.clear();
	m_CID2IND.clear();
	m_IP2IND.clear();
	close_lidar();
	m_lidar.close_lidar_probe();
}

bool Lidar_AkuSense::AddOneLidar(int nCID, char *ip) {
	g_Lidar_MUTEX.lock();
	if (g_pSoleInstance != nullptr) {
		if (!m_bOkay) {
			g_Lidar_MUTEX.unlock();
			return g_pSoleInstance->AddOneLidar(nCID, ip);
		} else {
			auto cid_pos = m_CID2IND.find(nCID);
			auto str_pos = m_IP2IND.find(std::string(ip));
			if (cid_pos != m_CID2IND.end() || str_pos != m_IP2IND.end()) {
				std::cout << "错误@Lidar_AkuSense::AddOneLidar() : cid或IP已关联激光雷达，请检查!\n";
				g_Lidar_MUTEX.unlock();
				return false;
			} else {
				g_Lidar_MUTEX.unlock();
				return _addLidar(nCID, ip);
			}
		}
	} else {
		g_Lidar_MUTEX.unlock();
		return false;
	}
}

bool Lidar_AkuSense::AddOneLidar(char *ip, int &OutID)
{
	g_Lidar_MUTEX.lock();
	if (g_pSoleInstance != nullptr) {
		if (!m_bOkay) {
			g_Lidar_MUTEX.unlock();
			return g_pSoleInstance->AddOneLidar(ip, OutID);
		} else {
			auto cid_pos = m_CID2IND.find(OutID);
			auto str_pos = m_IP2IND.find(std::string(ip));
			if (cid_pos != m_CID2IND.end() || str_pos != m_IP2IND.end()) {
				std::cout << "错误@Lidar_AkuSense::AddOneLidar() : cid或IP已关联激光雷达，请检查!\n";
				g_Lidar_MUTEX.unlock();
				return false;
			} else {
				g_Lidar_MUTEX.unlock();
				return _addLidar(ip, OutID);
			}
		}
	} else {
		g_Lidar_MUTEX.unlock();
		return false;
	}
}

bool Lidar_AkuSense::GetOneScan(int nCID, std::vector<DDRGeometry::APoint> &result,
	                            bool bRaw) {
	if (!g_Lidar_MUTEX.try_lock()) { return false; }
	if (g_pSoleInstance != nullptr) {
		if (!m_bOkay) {
			g_Lidar_MUTEX.unlock();
			return g_pSoleInstance->GetOneScan(nCID, result, bRaw);
		} else {
			auto cid_pos = m_CID2IND.find(nCID);
			if (cid_pos == m_CID2IND.end()) {
#if 1
				std::cout << "@Lidar_AkuSense::GetOneScan(nCID) : 输入CID未与激光雷达关联，请检查!\n";
#endif
				g_Lidar_MUTEX.unlock();
				return false;
			} else {
				g_Lidar_MUTEX.unlock();
				//std::cout << "GetOneScan() +++ cid_pos->second = " << cid_pos->second << std::endl;
				return _getScan(cid_pos->second, result, bRaw);
			}
		}
	} else {
		g_Lidar_MUTEX.unlock();
		return false;
	}
}

bool Lidar_AkuSense::GetOneScan(char *ip, std::vector<DDRGeometry::APoint> &result,
	                            bool bRaw) {
	if (!g_Lidar_MUTEX.try_lock()) { return false; }
	if (g_pSoleInstance != nullptr) {
		if (!m_bOkay) {
			g_Lidar_MUTEX.unlock();
			return g_pSoleInstance->GetOneScan(ip, result, bRaw);
		} else {
			auto str_pos = m_IP2IND.find(std::string(ip));
			if (str_pos == m_IP2IND.end()) {
#if 1
				std::cout << "@Lidar_AkuSense::GetOneScan(ip) : 输入IP未与激光雷达关联，请检查!\n";
#endif
				g_Lidar_MUTEX.unlock();
				return false;
			} else {
				g_Lidar_MUTEX.unlock();
				return _getScan(str_pos->second, result, bRaw);
			}
		}
	} else {
		g_Lidar_MUTEX.unlock();
		return false;
	}
}

bool Lidar_AkuSense::_addLidar(int nCID, char *ip) {
	if (!g_Lidar_MUTEX.try_lock()) { return false; }
	m_CID2IND.insert({ nCID, m_AkuLidars.size() });
	m_IP2IND.insert({ std::string(ip), m_AkuLidars.size() });
	m_AkuLidars.resize(m_AkuLidars.size() + 1);
	m_AkuLidars.back()._bConnected = false;
	m_AkuLidars.back()._bNewData = false;
	m_AkuLidars.back()._nCID = nCID;
	m_AkuLidars.back()._ip = std::string(ip);
	g_Lidar_MUTEX.unlock();

	std::cout << "@Lidar_AkuSense : 正在连接雷达(nCID=" << nCID << ", IP=" << ip << ")，请稍候...\n";
	return true;
}

bool Lidar_AkuSense::_addLidar(char *ip, int& nCID)
{
	if (!g_Lidar_MUTEX.try_lock()) { return false; }
#if 1
	nCID = -1;
	int id = m_lidar.open_lidar_probe();
	nCID = id;
	//m_CID2IND.insert({ nCID, m_AkuLidars.size() });
	m_IP2IND.insert({ std::string(ip), m_AkuLidars.size() });
	m_AkuLidars.resize(m_AkuLidars.size() + 1);
	m_AkuLidars.back()._bConnected = false;
	m_AkuLidars.back()._bNewData = false;
	//m_AkuLidars.back()._nCID = nCID;
	m_AkuLidars.back()._ip = std::string(ip);
	g_Lidar_MUTEX.unlock();

	std::cout << "@Lidar_AkuSenseEx : 正在连接雷达(ID=" << id << ", IP=" << ip << ")，请稍候...\n";
	m_AkuLidars.back()._nCID = id;
	int nTempID = open_lidar(ip); // 返回的是雷达ID
	if (nTempID >= 0)
	{
		std::cout << "@Lidar_AkuSenseEx new: 连接雷达(ID=" << nTempID << ", IP=" << ip << ")成功!\n";
		nCID = nTempID;
		m_CID2IND.insert({ nCID, m_AkuLidars.size() - 1 });
		m_AkuLidars.back()._nCID = nCID;
		m_AkuLidars.back()._bConnected = true;
		return true;
	}
	else
	{
		std::cout << "@Lidar_AkuSenseEx new: 连接雷达(nCID=" << nCID << ", IP=" << ip << ")失败，请确认连接和IP地址的正确性!\n";
		m_AkuLidars.back()._bConnected = false;
		//m_CID2IND.erase(nCID);
		m_IP2IND.erase(std::string(ip));
		m_AkuLidars.resize(m_AkuLidars.size() - 1);
		return false;
	}
#endif
}

void Lidar_AkuSense::ProcessRawData(std::vector<DDRGeometry::APoint> &points) {
	int prevID = 0;
	for (int i = 1; i < (int)(points.size() - 1); ++i) {
		if (_IsUnreliablePoint((points[i].angle - points[prevID].angle) * DEG2RAD,
			(points[i + 1].angle - points[i].angle) * DEG2RAD,
			points[prevID].distance,
			points[i].distance,
			points[i + 1].distance)) {
			points[i].distance = AKUSENSE_LIDAR_FILTEREDOUT_DISTANCE;
		} else {
			prevID = i;
		}
	}
}

bool Lidar_AkuSense::_getScan(int index, std::vector<DDRGeometry::APoint> &result,
	                          bool bRaw) {
	if (!g_Lidar_MUTEX.try_lock()) { return false; }
	assert(index >= 0 && index < (int)m_AkuLidars.size());
	if (m_AkuLidars[index]._data.empty() || !m_AkuLidars[index]._bNewData) {
		g_Lidar_MUTEX.unlock();
		return false;
	}

	result.resize(m_AkuLidars[index]._data.size());
	memcpy((void*)(&result[0]), (const void*)(&m_AkuLidars[index]._data[0]),
		sizeof(DDRGeometry::APoint) * m_AkuLidars[index]._data.size());
	if (!bRaw) {
		ProcessRawData(result);
	}
	m_AkuLidars[index]._bNewData = false;

	g_Lidar_MUTEX.unlock();
	return true;
}
#if 1
	// 雷达测量数据回调函数
	void LidarDataCallback(int id, LIM_HEAD *lim, void *data)
	{
		Lidar_AkuSense *pThis = (Lidar_AkuSense*)data;
		pThis->DealLidarData(id, lim);
	}
	// 雷达状态变化回调函数
	void LidarStateCallback(int id, LIM_HEAD *lim, void *data)
	{
		Lidar_AkuSense *pThis = (Lidar_AkuSense*)data;
		pThis->DealLidarState(id, lim);
	}

	int Lidar_AkuSense::close_lidar()
	{
		int id = m_lidar.get_id();
		if (0 >= id)
			return -1;
		m_lidar.close_lidar(id);
		// 注册雷达回调函数
		m_lidar.reg_data_callback(id, (void*)NULL, (void*)this);
		m_lidar.reg_state_callback(id, (void*)NULL, (void*)this);

		return 0;
	}

	/*
		return:
			>0 - open lidar success. return lidar ID
			-1 - failed
	*/
	int Lidar_AkuSense::open_lidar(char *ip)
	{
		int id = 0;

		// 如果已连接雷达，先关闭
		//close_lidar();

		// 连接雷达
		id = m_lidar.open_lidar(ip, LIM_DT_PORT, 0);
		if (0 > id)
		{
			m_lidar.close_lidar(id);
			printf("open_lidar()失败,id=%d\n", id);
			return -1;
		}

		// 注册雷达回调函数
		m_lidar.reg_data_callback(id, (void*)LidarDataCallback, (void*)this);
		m_lidar.reg_state_callback(id, (void*)LidarStateCallback, (void*)this);

		id = m_lidar.get_id();
		//m_lidar.read_iostatus(id, false);  // 目的是让回调函数刷新界面上IO口的状态显示

		return id;
	}

	void Lidar_AkuSense::DealLidarData(int id, LIM_HEAD *_lim)
	{
#if 1
		if (g_bStop) { return; }

		g_Lidar_MUTEX.lock();
		auto cid_pos = m_CID2IND.find(id);
		if (cid_pos == m_CID2IND.end()) 
		{
			std::cout << "DealLidarData step1 --- id:" << id << "  m_CID2IND size:" << m_CID2IND.size() << std::endl;
			g_Lidar_MUTEX.unlock();
			return; 
		}
		g_Lidar_MUTEX.unlock();
		std::lock_guard<std::mutex> lock(g_Lidar_MUTEX);
		
		if (cid_pos->second >= m_AkuLidars.size())
		{
			std::cout << "Error m_AkuLidars size:" << m_AkuLidars.size() << "  cid_pos->second = " << cid_pos->second << std::endl;
			return;
		}
		
		LMD_INFO* lmd_info = LMD_Info(_lim);
		LMD_D_Type* lmd = LMD_D(_lim);
		std::vector<DDRGeometry::APoint> &_pts = m_AkuLidars[cid_pos->second]._data;
		_pts.resize(lmd_info->nMDataNum);

		for (int i = 0; i < (int)lmd_info->nMDataNum; i++)
		{
			_pts[i].angle = (lmd_info->nBAngle + i * (int)lmd_info->nAnglePrecision) * 0.001f - 90.0f;
			_pts[i].distance = lmd[i];

			if (300 == i)
			{
			//	std::cout << "pts" << i << ": angle = " << _pts[i].angle << " distance = " << _pts[i].distance << std::endl;
			}
		}
		m_AkuLidars[cid_pos->second]._bNewData = true;
		return;		
#endif
	}

	void Lidar_AkuSense::DealLidarState(int id, LIM_HEAD *lim)
	{
#if 1
		if (LIM_CODE_FMSIG == lim->nCode)
		{
			int nAlarmNO = lim->Data[0];
			if (0x10 & lim->Data[1])
			{ // 注意区报警
			}
			else
			{ // 注意区报警消失
			}

			if (0x04 & lim->Data[1])
			{ // 预警区报警
			}
			else
			{
			}

			if (0x01 & lim->Data[1])
			{ // 报警区报警
			}
			else
			{
			}
		}
		else if (LIM_CODE_IOSTATUS == lim->nCode)
		{
			//m_pIO->draw_io_status(lim->Data[0]);
		}
#endif
	}
#endif
}
