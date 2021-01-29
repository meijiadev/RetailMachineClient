package com.example.retailmachineclient.mcuSdk;

import android.util.Log;

import com.example.retailmachineclient.util.CRC;
import com.hardware.SerialPort.Utils;

import java.util.zip.CRC32;

/**
 * desc:命令解析组包
 */
public class DataProtocol {
	// 一条命令最小长度是9个字节
	public static final int MIN_CMD_LEN = 20;
	/**
	 * 解析一条命令后得到 的字节
	 * @author Administrator
	 *
	 */
	public static class RecDataCls{
		public boolean isAndroidToMcu;     //是否是Android 发向mcu
		public McuService.Device device;  //设备
		public byte directiveId;         //指令
		public byte[] data;             //数据     16个字节
		public byte[] check;            //校验代码 两个字节
		@Override
		public String toString() {
			String ret="";
			if (isAndroidToMcu){
				ret += "And->Mcu [" +device.name() +"]";
			}else{
				ret += "Mcu [" +device.name() +"]->And";
			}

			ret += " directiveId:" +directiveId +  " Data:" + Utils.byteBufferToHexString(data)+"Check:"+Utils.byteBufferToHexString(check);
			return ret;
		}
	};

	/**
	 * 根据指定的参数打包一个要发送的数据包

	 * @return
	 */
	public static byte[] packSendData( McuService.Device device, byte directiveId, byte[] data){
		byte[] packData=new byte[20];
		packData[0]=(byte) device.getFlag();
		packData[1]=directiveId;
		Utils.putBytes(packData,2,data,data.length);
		byte[] checks= CRC.crc16(packData);
		return packData;
	}






}
