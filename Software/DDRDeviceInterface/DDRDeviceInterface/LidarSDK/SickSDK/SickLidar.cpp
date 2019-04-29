#include "SickLidar.h"

#include <windows.h>

#include <iostream>
#include <string>

#include <time.h>
#include <math.h>  


namespace DDRDrivers
{
	Lidar_SickLMS::Lidar_SickLMS()
	{
		//debug = true;
		m_TimeStampCounter = 0;
		m_PreviousTimeStamp = 0;
		m_fAngleRes = 5000;//default frequency 50hz, angle resolution 0.5deg
		m_initDone = false;
	}

	Lidar_SickLMS::~Lidar_SickLMS()
	{
	}

	bool Lidar_SickLMS::InitLidarParams()
	{
		// the number of frequency and angle resolution, they should be same.
		// change this number will cost more time compare with unchanged.
		// bug will occur at the first time when num is changed from 2500 to 5000.
		// 2500: 25Hz, 0.25 degree per point, totally 1081 points.
		// 5000: 50Hz, 0.5 degree per point, totally 541 points.
		int iFreRes = 5000;
		m_fAngleRes = iFreRes / 10000.0;

		scanCfg lastScanConfig = GetScanCfg();

		if (debug) {
			std::cout << "Geting scan configuration ..." << ::std::endl;
			std::cout << "\nScan configuration: \nScanning Frequency :" << lastScanConfig.scaningFrequency / 100.0
			<< "Hz AngleResolution :" << lastScanConfig.angleResolution / 10000.0
			<< "deg StartAngle :" << lastScanConfig.startAngle / 10000.0
			<< "deg StopAngle :" << lastScanConfig.stopAngle / 10000.0
			<< "deg" << std::endl << std::endl;
		}

		// 2500: 25Hz, 0.25 degree per point
		// 5000: 50Hz, 0.5 degree per point
		// bug: from 2500 to 5000 dosen't work well at first time
		lastScanConfig.angleResolution = iFreRes;
		lastScanConfig.scaningFrequency = iFreRes;

		SetScanCfg(lastScanConfig);

		scanDataCfg cc;
		cc.outputChannel = 1;
		cc.remission = true;
		cc.resolution = 1;
		cc.encoder = 0;
		cc.position = false;
		cc.deviceName = false;
		cc.timestamp = 0;
		cc.outputInterval = 1;

		SetScanDataCfg(cc);

		outputRangeCfg lastOutputRange = GetDataOutputCfg();

		if (debug) {
			std::cout << "Geting OutputRange configuration ..." << ::std::endl;
			std::cout << "\nOutputRange: \nAngleResolution :" << lastOutputRange.angleResolution / 10000.0
				<< "deg StartAngle :" << lastOutputRange.startAngle / 10000.0
				<< "deg StopAngle :" << lastOutputRange.stopAngle / 10000.0
				<< "deg" << std::endl << std::endl;
		}

		//maxium: 270 degree, from -45 to 225
		lastOutputRange.angleResolution = iFreRes;
		lastOutputRange.startAngle = -45 * 10000.0;
		lastOutputRange.stopAngle = 225 * 10000.0;

		SetDataOutputCfg(lastOutputRange);

		return true;
	}

	bool Lidar_SickLMS::Connect(std::string host, int port)
	{
		if (!connected) {

			//初始化套接字
			WORD wVerRequest = MAKEWORD(1, 1);
			WSADATA wsaData;
			WSAStartup(wVerRequest, &wsaData);
			//需要WSACleanup

			sockDesc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			//非阻塞
			int iMode = 1;
			ioctlsocket(sockDesc, FIONBIO, (u_long FAR*)&iMode);

			if (sockDesc) {
				
				struct sockaddr_in stSockAddr;
				int Res;
				stSockAddr.sin_family = AF_INET;
				stSockAddr.sin_port = htons(port);
				Res = inet_pton(AF_INET, host.c_str(), &stSockAddr.sin_addr);


				//::表示全局
				int ret = ::connect(sockDesc, (struct sockaddr *) &stSockAddr, sizeof stSockAddr);
				if (ret == 0) {
					connected = true;
				}
				else{
					struct timeval tv;
					tv.tv_sec = 5;
					tv.tv_usec = 0;

					fd_set rfds;
					FD_ZERO(&rfds);
					FD_SET(sockDesc, &rfds);

					if (select(-1, NULL, &rfds, NULL, &tv) <= 0)
					{
						ret = -1; // 有错误(select错误或者超时)
						connected = false;
					}
					else
					{
						int error = -1;
						int optLen = sizeof(int);
						getsockopt(sockDesc, SOL_SOCKET, SO_ERROR, (char*)&error, &optLen);

						if (0 != error)
						{
							ret = -1; // 有错误
							connected = false;
						}
						else
						{
							ret = 1;  // 无错误
							connected = true;
						}
					}

				}
			}

			iMode = 0;
			ioctlsocket(sockDesc, FIONBIO, (u_long FAR*)&iMode); //设置为阻塞模式
		}

		return connected;
	}

	void Lidar_SickLMS::Disconnect()
	{
		ScanContinous(0);
		StopMeas();
		if (connected) {
			char buf[100];
			sprintf_s(buf, "%c%s%c", 0x02, "sMN Run", 0x03);

			send(sockDesc, buf, strlen(buf), 0);

			int len = recv(sockDesc, buf, 100, 0);

			closesocket(sockDesc);
			connected = false;
			std::cout << "Disconnect from laser" << std::endl;
		}
	}

	bool Lidar_SickLMS::IsConnected()
	{
		return connected;
	}

	void Lidar_SickLMS::StartMeas()
	{
		char buf[100];
		sprintf_s(buf, "%c%s%c", 0x02, "sMN LMCstartmeas", 0x03);

		send(sockDesc, buf, strlen(buf), 0);

		int len = recv(sockDesc, buf, 100, 0);
	}

	void Lidar_SickLMS::StopMeas()
	{
		char buf[100];
		sprintf_s(buf, "%c%s%c", 0x02, "sMN LMCstopmeas", 0x03);

		send(sockDesc, buf, strlen(buf), 0);

		int len = recv(sockDesc, buf, 100, 0);
	}

	status_t Lidar_SickLMS::QueryStatus()
	{
		char buf[100];
		sprintf_s(buf, "%c%s%c", 0x02, "sRN STlms", 0x03);

		send(sockDesc, buf, strlen(buf), 0);

		int len = recv(sockDesc, buf, 100, 0);
		//	if (buf[0] != 0x02)
		//		std::cout << "invalid packet recieved" << std::endl;
		//	if (debug) {
		//		buf[len] = 0;
		//		std::cout << buf << std::endl;
		//	}
		int ret;
		sscanf_s((buf + 10), "%d", &ret, sizeof(ret));

		return (status_t)ret;
	}

	void Lidar_SickLMS::Login()
	{
		char buf[100];
		sprintf_s(buf, "%c%s%c", 0x02, "sMN SetAccessMode 03 F4724744", 0x03);

		send(sockDesc, buf, strlen(buf), 0);

		int len = recv(sockDesc, buf, 100, 0);
		if (buf[0] != 0x02)
			std::cout << "invalid packet recieved" << std::endl;
		//if (debug) {
		//	buf[len] = 0;
		//	std::cout << buf << std::endl;
		//}
	}

	scanCfg Lidar_SickLMS::GetScanCfg() const
	{
		scanCfg cfg;
		char buf[100];
		sprintf_s(buf, "%c%s%c", 0x02, "sRN LMPscancfg", 0x03);

		send(sockDesc, buf, strlen(buf), 0);

		int len = recv(sockDesc, buf, 100, 0);

		sscanf_s(buf + 1, "%*s %*s %X %*d %X %X %X", &cfg.scaningFrequency,
			&cfg.angleResolution, &cfg.startAngle, &cfg.stopAngle);
		return cfg;
	}

	void Lidar_SickLMS::SetScanCfg(const scanCfg & cfg)
	{
		char buf[100];
		sprintf_s(buf, "%c%s %X 1 %X %X %X%c", 0x02, "sMN mLMPsetscancfg",
			cfg.scaningFrequency, cfg.angleResolution, cfg.startAngle,
			cfg.stopAngle, 0x03);

		if (debug)
			printf("Set ScanCfg: %s\n", buf);

		send(sockDesc, buf, strlen(buf), 0);

		int len = recv(sockDesc, buf, 100, 0);

		buf[len - 1] = 0;
	}

	void Lidar_SickLMS::SetScanDataCfg(const scanDataCfg & cfg)
	{
		char buf[100];
		sprintf_s(buf, "%c%s %02X 00 %d %d 0 %02X 00 %d %d 0 %d 0 %d%c", 0x02,
			"sWN LMDscandatacfg", cfg.outputChannel, cfg.remission ? 1 : 0,
			cfg.resolution, cfg.encoder, cfg.position ? 1 : 0,
			cfg.deviceName ? 1 : 0, cfg.timestamp ? 1 : 0, cfg.outputInterval, 0x03);
		
		if (debug)
			printf("Set ScanDataCfg: %s\n", buf);
		send(sockDesc, buf, strlen(buf), 0);

		int len = recv(sockDesc, buf, 100, 0);
		buf[len - 1] = 0;
	}

	outputRangeCfg Lidar_SickLMS::GetDataOutputCfg() const
	{
		scanCfg cfg;
		char buf[100];
		sprintf_s(buf, "%c%s%c", 0x02, "sRN LMPoutputRange", 0x03);

		send(sockDesc, buf, strlen(buf), 0);

		int len = recv(sockDesc, buf, 100, 0);

		sscanf_s(buf + 1, "%*s %*s %*d %X %X %X",
			&cfg.angleResolution, &cfg.startAngle, &cfg.stopAngle);

		return cfg;
	}

	void Lidar_SickLMS::SetDataOutputCfg(const outputRangeCfg &cfg)
	{
		char buf[100];
		sprintf_s(buf, "%c%s 1 %X %X %X%c", 0x02, "sWN LMPoutputRange",
			cfg.angleResolution, cfg.startAngle, cfg.stopAngle, 0x03);

		if (debug)
			printf("Set DataOutputCfg: %s\n", buf);

		send(sockDesc, buf, strlen(buf), 0);

		int len = recv(sockDesc, buf, 100, 0);

		buf[len - 1] = 0;
	}

	void Lidar_SickLMS::ScanContinous(int start)
	{
		char buf[100];
		sprintf_s(buf, "%c%s %d%c", 0x02, "sEN LMDscandata", start, 0x03);

		send(sockDesc, buf, strlen(buf), 0);

		int len = recv(sockDesc, buf, 100, 0);

		if (buf[0] != 0x02)
			printf("invalid packet recieved\n");

		if (start = 0) {
			for (int i = 0; i < 10; i++)
				recv(sockDesc, buf, 100, 0);
		}
	}

	void Lidar_SickLMS::GetData(scanData & data)
	{
		if (!connected) {
			std::cout << "lidar is not connected!" << std::endl;
			return;
		}
			
		char buf[20000];
		char *buftmp;
		fd_set rfds;
		struct timeval tv;
		int retval, len;
		len = 0;
		//each telegram size with 0.5 resolution and 270 degree is 2798(5498 for 0.25 resolution)
		int len_fix = 2798;
		bool bOK = false;

		do {
			FD_ZERO(&rfds);
			FD_SET(sockDesc, &rfds);

			tv.tv_sec = 0;
			tv.tv_usec = 50000;
			retval = select(sockDesc + 1, &rfds, NULL, NULL, &tv);
			if (retval) {
				len += recv(sockDesc, buf + len, 20000 - len, 0);
				if (len > len_fix){
					for (size_t i = 0; i < len; i++){
						if (buf[i] == 0x02){
							if ((i + len_fix) < len){
								if (buf[i + len_fix] == 0x03) {
									bOK = true;
									break;
								}		
							}
							else{
								continue;
							}
						}
					}
					if (!bOK){
						// no stx
						memset(buf, 0, len);
						len = 0;
					}
				}
				else{
					continue; //length is not match
				}
			}
		} while ((buf[0] != 0x02) || (buf[len - 1] != 0x03));
		buf[len - 1] = 0;
		char* tok = strtok_s(buf, " ", &buftmp); //Type of command 3
		tok = strtok_s(NULL, " ", &buftmp); //Command 11 
		tok = strtok_s(NULL, " ", &buftmp); //VersionNumber 2
		tok = strtok_s(NULL, " ", &buftmp); //DeviceNumber 2
		tok = strtok_s(NULL, " ", &buftmp); //Serial number 4
		tok = strtok_s(NULL, " ", &buftmp); //DeviceStatus 2x1
		tok = strtok_s(NULL, " ", &buftmp); 
		tok = strtok_s(NULL, " ", &buftmp); //TelegramCounter 2
		tok = strtok_s(NULL, " ", &buftmp); //ScanCounter 2
		tok = strtok_s(NULL, " ", &buftmp); //PowerUpDuration 4
		uint32_t TimeSinceStartUp;
		sscanf_s(tok, "%x", &TimeSinceStartUp, sizeof(TimeSinceStartUp));
		data.dist1TimeSinceStartUp = TimeSinceStartUp;

		tok = strtok_s(NULL, " ", &buftmp); //TransmissionDuration 4 
		tok = strtok_s(NULL, " ", &buftmp); //InputStatus 2x1
		tok = strtok_s(NULL, " ", &buftmp);
		tok = strtok_s(NULL, " ", &buftmp); //OutputStatus 2x1
		tok = strtok_s(NULL, " ", &buftmp); 
		tok = strtok_s(NULL, " ", &buftmp); //ReservedByteA 2
		tok = strtok_s(NULL, " ", &buftmp); //ScanningFrequency 4
		tok = strtok_s(NULL, " ", &buftmp); //MeasurementFrequency 4
		tok = strtok_s(NULL, " ", &buftmp); //NumberEncoders 2
		int NumberEncoders;
		sscanf_s(tok, "%d", &NumberEncoders, sizeof(NumberEncoders));
		for (int i = 0; i < NumberEncoders; i++) {
			tok = strtok_s(NULL, " ", &buftmp); //EncoderPosition 2
			tok = strtok_s(NULL, " ", &buftmp); //EncoderSpeed 2
		}

		tok = strtok_s(NULL, " ", &buftmp); //NumberChannels16Bit 2
		int NumberChannels16Bit;
		sscanf_s(tok, "%d", &NumberChannels16Bit, sizeof(NumberChannels16Bit));
		if (debug)
			printf("NumberChannels16Bit : %d\n", NumberChannels16Bit);
		for (int i = 0; i < NumberChannels16Bit; i++) {
			int type = -1; // 0 DIST1 1 DIST2 2 RSSI1 3 RSSI2
			char content[6];
			tok = strtok_s(NULL, " ", &buftmp); //MeasuredDataContent 5
			sscanf_s(tok, "%s", content, sizeof(content));
			if (!strcmp(content, "DIST1")) {
				type = 0;
			}
			else if (!strcmp(content, "DIST2")) {
				type = 1;
			}
			else if (!strcmp(content, "RSSI1")) {
				type = 2;
			}
			else if (!strcmp(content, "RSSI2")) {
				type = 3;
			}
			tok = strtok_s(NULL, " ", &buftmp); //ScalingFactor
			tok = strtok_s(NULL, " ", &buftmp); //ScalingOffset
			tok = strtok_s(NULL, " ", &buftmp); //Starting angle

			int StartAngle;
			sscanf_s(tok, "%X", &StartAngle, sizeof(StartAngle));
			if (debug)
				printf("StartAngle : %d\n", StartAngle);

			tok = strtok_s(NULL, " ", &buftmp); //Angular step width
			tok = strtok_s(NULL, " ", &buftmp); //NumberData
			int NumberData = 0;
			sscanf_s(tok, "%X", &NumberData, sizeof(NumberData));

			if (debug)
				printf("NumberData : %d\n", NumberData);

			if (type == 0) {
				data.dist_len1 = NumberData;
			}
			else if (type == 1) {
				data.dist_len2 = NumberData;
			}
			else if (type == 2) {
				data.rssi_len1 = NumberData;
			}
			else if (type == 3) {
				data.rssi_len2 = NumberData;
			}

			for (int i = 0; i < NumberData; i++) {
				int dat;
				tok = strtok_s(NULL, " ", &buftmp); //data
				sscanf_s(tok, "%X", &dat, sizeof(dat));

				if (type == 0) {
					data.dist1[i] = dat;
				}
				else if (type == 1) {
					data.dist2[i] = dat;
				}
				else if (type == 2) {
					data.rssi1[i] = dat;
				}
				else if (type == 3) {
					data.rssi2[i] = dat;
				}

			}
		}

		/*
		tok = strtok_s(NULL, " ", &buftmp); //NumberChannels8Bit
		int NumberChannels8Bit;
		sscanf_s(tok, "%d", &NumberChannels8Bit);
		if (debug)
			printf("NumberChannels8Bit : %d\n", NumberChannels8Bit);
		for (int i = 0; i < NumberChannels8Bit; i++) {
			int type = -1;
			char content[6];
			tok = strtok_s(NULL, " ", &buftmp); //MeasuredDataContent
			sscanf_s(tok, "%s", content);
			if (!strcmp(content, "DIST1")) {
				type = 0;
			}
			else if (!strcmp(content, "DIST2")) {
				type = 1;
			}
			else if (!strcmp(content, "RSSI1")) {
				type = 2;
			}
			else if (!strcmp(content, "RSSI2")) {
				type = 3;
			}
			tok = strtok_s(NULL, " ", &buftmp); //ScalingFactor
			tok = strtok_s(NULL, " ", &buftmp); //ScalingOffset
			tok = strtok_s(NULL, " ", &buftmp); //Starting angle
			tok = strtok_s(NULL, " ", &buftmp); //Angular step width
			tok = strtok_s(NULL, " ", &buftmp); //NumberData
			int NumberData;
			sscanf_s(tok, "%X", &NumberData);

			if (debug)
				printf("NumberData : %d\n", NumberData);

			if (type == 0) {
				data.dist_len1 = NumberData;
			}
			else if (type == 1) {
				data.dist_len2 = NumberData;
			}
			else if (type == 2) {
				data.rssi_len1 = NumberData;
			}
			else if (type == 3) {
				data.rssi_len2 = NumberData;
			}
			for (int i = 0; i < NumberData; i++) {
				int dat;
				tok = strtok_s(NULL, " ", &buftmp); //data
				sscanf_s(tok, "%X", &dat);

				if (type == 0) {
					data.dist1[i] = dat;
				}
				else if (type == 1) {
					data.dist2[i] = dat;
				}
				else if (type == 2) {
					data.rssi1[i] = dat;
				}
				else if (type == 3) {
					data.rssi2[i] = dat;
				}
			}

		}
		*/

	}

	//interface
	bool Lidar_SickLMS::GetOneScan(std::vector<DDRGeometry::Points_2d>& result, uint64_t &time)
	{

		scanData data;
		GetData(data);

		if (!(data.dist_len1 > 0)) { return false; }
			
		if (m_bTimestampInit)
		{
			m_InitialTime = data.dist1TimeSinceStartUp;
			m_bTimestampInit = false;
		}

		if (data.dist1TimeSinceStartUp < m_PreviousTimeStamp)
			m_TimeStampCounter++;

		time = (((uint64_t)data.dist1TimeSinceStartUp + m_TimeStampCounter * 4294967295) - m_InitialTime);


		//the angle resolution should be changed when the frequency is changed.
		float angleRes = m_fAngleRes;//outputrange.angleResolution/10000.0;
		float angleStart = -45;//outputrange.startAngle/10000.0;
		float angleStop = 225;//outputrange.stopAngle/10000.0;

		if (!m_lidarPointMutex.try_lock()) { return false; }
			
		for (int j = 0; j < data.dist_len1; j++)
		{
			DDRGeometry::Points_2d P;
			P.x = data.dist1[j] * sin((j*angleRes + angleStart)*PI_TEMP / 180);
			P.y = -data.dist1[j] * cos((j*angleRes + angleStart)*PI_TEMP / 180);
			result.push_back(P);
		}
		m_lidarPointMutex.unlock();
		return true;
	}

	bool Lidar_SickLMS::AddLidar(std::string host, int port, bool initstatus)
	{
		if (!Connect(host, port))
		{
			std::cout << "Connection fail" << std::endl;
			return false;
		}

		std::cout << "Connected to laser" << std::endl;
		std::cout << "Loging in ..." << std::endl;

		Login();

		if (!initstatus)
		{
			StopMeas();
			if (!InitLidarParams())
			{
				std::cout << "Set Lidar Params Failed." << ::std::endl;
				return false;
			}

			std::cout << "Start measurements ..." << std::endl;

			StartMeas();

			std::cout << "Wait for ready status, it may take several seconds..." << std::endl;

			int ret = 0;
			while (ret != 7)
			{
				// wait several seconds
				ret = QueryStatus();
			}

			std::cout << "Laser ready" << std::endl;
			m_initDone = true;
		}
		std::cout << "Start continuous data transmission ..." << std::endl;
		ScanContinous(1);

		return true;
	}


}//namespace LidarDriver

