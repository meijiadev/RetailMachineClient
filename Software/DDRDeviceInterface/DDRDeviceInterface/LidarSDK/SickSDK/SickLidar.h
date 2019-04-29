#pragma once

#include <vector>
#include <mutex>

#include <WinSock2.h>
#include <Ws2tcpip.h>

#include "Geometry/RangerPoints.h"

#pragma comment(lib, "ws2_32.lib")

#define  LASER_SERVER_IP      "192.168.0.82"
#define  LASER_SERVER_PORT     2112


//namespace LidarData
//{
//	typedef struct _LidarPoint
//	{
//		// the coordinate system:
//		// front x, left y.
//		float x;
//		float y;
//	}Points_2d;
//
//}//namespace LidarData

namespace DDRDrivers
{
	/*!
	* @class scanCfg
	* @brief Structure containing scan configuration.
	*/
	typedef struct _scanCfg {
		/*!
		 * @brief Scanning frequency.
		 * 1/100 Hz
		 */
		int scaningFrequency;

		/*!
		 * @brief Scanning resolution.
		 * 1/10000 degree
		 */
		int angleResolution;

		/*!
		 * @brief Start angle. Change it on the data output config function.
		 * 1/10000 degree
		 */
		int startAngle = -450000;

		/*!
		 * @brief Stop angle. Change it on the data output config function.
		 * 1/10000 degree
		 */
		int stopAngle = 2250000;
	} scanCfg, outputRangeCfg;

	/*!
	* @class scanDataCfg
	* @brief Structure containing scan data configuration.
	*/
	typedef struct _scanDataCfg {

		/*!
		 * @brief Output channels.
		 * Defines which output channel is activated.
		 */
		int outputChannel;

		/*!
		 * @brief Remission.
		 * Defines whether remission values are output.
		 */
		bool remission;

		/*!
		 * @brief Remission resolution.
		 * Defines whether the remission values are output with 8-bit or 16bit resolution.
		 */
		int resolution;

		/*!
		 * @brief Encoders channels.
		 * Defines which output channel is activated.
		 */
		int encoder;

		/*!
		 * @brief Position.
		 * Defines whether position values are output.
		 */
		bool position;

		/*!
		 * @brief Device name.
		 * Determines whether the device name is to be output.
		 */
		bool deviceName;

		/*!
		 * @brief Time.
		 * Determines whether the Time is to be output.
		 */
		bool timestamp;

		/*!
		 * @brief Output interval.
		 * Defines which scan is output.
		 *
		 * 01 every scan\n
		 * 02 every 2nd scan\n
		 * ...\n
		 * 50000 every 50000th scan
		 */
		int outputInterval;
	} scanDataCfg;

	/*!
	* @class scanData
	* @brief Structure containing single scan message.
	*/
	typedef struct _scanData {


		/*!
		 * @brief Number of samples in dist1.
		 *
		 */
		uint32_t dist1TimeSinceStartUp;

		/*!
		 * @brief Number of samples in dist1.
		 *
		 */
		int dist_len1 = 0;

		/*!
		 * @brief Radial distance for the first reflected pulse
		 *
		 */
		uint16_t dist1[1082];

		/*!
		 * @brief Number of samples in dist2.
		 *
		 */
		int dist_len2;

		/*!
		 * @brief Radial distance for the second reflected pulse
		 *
		 */
		uint16_t dist2[1082];

		/*!
		 * @brief Number of samples in rssi1.
		 *
		 */
		int rssi_len1;

		/*!
		 * @brief Remission values for the first reflected pulse
		 *
		 */
		uint16_t rssi1[1082];

		/*!
		 * @brief Number of samples in rssi2.
		 *
		 */
		int rssi_len2;

		/*!
		 * @brief Remission values for the second reflected pulse
		 *
		 */
		uint16_t rssi2[1082];
	} scanData;

	typedef enum {
		undefined = 0,
		initialisation = 1,
		configuration = 2,
		idle = 3,
		rotated = 4,
		in_preparation = 5,
		ready = 6,
		ready_for_measurement = 7
	} status_t;

	class Lidar_SickLMS
	{
	public:
		Lidar_SickLMS();
		~Lidar_SickLMS();

		bool InitLidarParams();

		/*!
		* @brief Connect to LMS1xx.
		* @param host LMS1xx host name or ip address.
		* @param port LMS1xx port number.
		*/
		bool Connect(std::string host, int port = 2112);

		/*!
		* @brief Disconnect from LMS1xx device.
		*/
		void Disconnect();

		/*!
		* @brief Get status of connection.
		* @returns connected or not.
		*/
		bool IsConnected();

		/*!
		* @brief Start measurements.
		* After receiving this command LMS1xx unit starts spinning laser and measuring.
		*/
		void StartMeas();

		/*!
		* @brief Stop measurements.
		* After receiving this command LMS1xx unit stop spinning laser and measuring.
		*/
		void StopMeas();

		/*!
		* @brief Get current status of LMS1xx device.
		* @returns status of LMS1xx device.
		*/
		status_t QueryStatus();

		/*!
		* @brief Log into LMS1xx unit.
		* Increase privilege level, giving ability to change device configuration.
		*/
		void Login();

		/*!
		* @brief Get current scan configuration.
		* Get scan configuration :
		* - scanning frequency.
		* - scanning resolution.
		* - start angle.
		* - stop angle.
		* @returns scanCfg structure.
		*/
		scanCfg GetScanCfg() const;

		/*!
		* @brief Set scan configuration.
		* Get scan configuration :
		* - scanning frequency.
		* - scanning resolution.
		* - start angle.
		* - stop angle.
		* @param cfg structure containing scan configuration.
		*/
		void SetScanCfg(const scanCfg &cfg);

		/*!
		* @brief Set scan data configuration.
		* Set format of scan message returned by device.
		* @param cfg structure containing scan data configuration.
		*/
		void SetScanDataCfg(const scanDataCfg &cfg);

		/*!
		* @brief Get current data output configuration.
		* Get  data output configuration :
		* - scanning resolution.
		* - start angle.
		* - stop angle.
		* @returns outputRangeCfg structure.
		*/
		outputRangeCfg GetDataOutputCfg() const;

		/*!
		* @brief Set data output configuration.
		* Set scan angle.
		* @param cfg structure containing data output configuration.
		*/
		void SetDataOutputCfg(const outputRangeCfg &cfg);

		/*!
		* @brief Start or stop continuous data acquisition.
		* After reception of this command device start or stop continuous data stream containing scan messages.
		* @param start 1 : start 0 : stop
		*/
		void ScanContinous(int start);

		/*!
		* @brief Receive single scan message.
		*
		* @param data pointer to scanData buffer structure.
		*/
		void GetData(scanData& data);


		/*!
		* @brief Receive single scan data.
		*
		* @param Output: A vector of lidar 2d points, an uint64_t of timestamp.
		*/
		bool GetOneScan(std::vector<DDRGeometry::Points_2d> &result, uint64_t &time);

		/*!
		* @brief Connect to lidar with giving IP and status.
		* @param host LMS1xx host name or ip address.
		* @param port LMS1xx port number.
		* @param init status, false for fisrt connect.
		*/
		bool AddLidar(std::string host, int port = 2112, bool initstatus = false);

		/*!
		* @brief Get lidar ready status.
		*/
		bool GetInitStatus() { return m_initDone; }

	private:

		const double PI_TEMP = 3.1415926;

		float m_fAngleRes;
		
		uint64_t m_TimeStampCounter = 0;
		uint64_t m_PreviousTimeStamp = 0;
		uint64_t m_InitialTime = 0;

		bool m_bTimestampInit = true;

		std::mutex m_lidarPointMutex;

		SOCKET sockDesc;

		bool connected = false;
		bool debug = false;

		bool m_initDone = false;
	};

}//namespace LidarDriver


