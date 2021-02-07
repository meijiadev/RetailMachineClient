package com.example.retailmachineclient.mcuSdk;

import com.example.retailmachineclient.util.CRC;
import com.example.retailmachineclient.util.Logger;
import com.example.retailmachineclient.util.Utils;

/**
 * desc:命令解析组包
 */
public class DataProtocol {
	// 一条命令最小长度是9个字节
	public static final int MIN_CMD_LEN = 20;

	/**
	 * 解析一条命令后得到 的字节
	 * @author Administrator
	 */
	public static class RecDataCls{
		public boolean isAndroidToMcu;     //是否是Android 发向mcu
		public byte directiveId;         //指令
		public byte[] data;             //数据     16个字节
		public byte[] check;            //校验代码 两个字节

		@Override
		public String toString() {
			String ret="";
			ret += " directiveId:" +directiveId +  " Data:" + Utils.byteBufferToHexString(data)+"Check:"+Utils.byteBufferToHexString(check);
			return ret;
		}
	};

	/**
	 * 从buf里面解析出一条命令
	 * @param buf
	 * @return
	 */
	public static RecDataCls createRecDataCls(byte[] buf){
		if(buf.length<MIN_CMD_LEN){
			return null;
		}
		RecDataCls dp=new RecDataCls();
		if (buf[0]==0x00){
			dp.isAndroidToMcu=false;
		}else{
			dp.isAndroidToMcu=true ;
		}
		dp.directiveId=buf[1];
		dp.data= Utils.getBytes(buf,2,16);
		dp.check=Utils.getBytes(buf,18,2);
		return dp;
	}

	/**
	 * 判断一个字节数组的校验码是否正确
	 * @return
	 */
	public static boolean isCheckRight(byte[] buffer){
		byte[] bytes=Utils.getBytes(buffer,0,18);
		byte[] oldCheck=Utils.getBytes(buffer,18,2);
		byte[] newCheck=CRC.crc16(bytes);
		for (int i=0;i<2;i++){
			if (oldCheck[i]!=newCheck[i]){
				return false;
			}
		}
		return true;
	}

	/**
	 * 组包
	 * @param portMCU 发给哪个mcu
	 * @param directiveId 指令
	 * @param data        发送的数据
	 * @return
	 */
	public static byte[] packSendData(MCU.PortMCU portMCU, byte directiveId, byte data){
		byte[] sData=new byte[18];
		sData[0]=(byte) portMCU.getFlag();
		sData[1]=directiveId;
		byte[] nData=new byte[16];
		nData[0]=data;
		for (int i=1;i<nData.length;i++){
			nData[i]=0x00;
		}
		Utils.putBytes(sData,2,nData,nData.length);
		byte[] checks= CRC.crc16(sData);
		byte[] packData=new byte[20];
		if (checks.length==2){
			Utils.putBytes(packData,0,sData,18);
			Utils.putBytes(packData,18,checks,2);
		}else{
			Logger.e("check result is error！");
		}
		Logger.e("-------组包的数据："+Utils.byteBufferToHexString(packData));
		return packData;
	}

	/**
	 * 只用补充校验码
	 * @param data(18字节)
	 * @return
	 */
	public static byte[] packSendData(byte[] data){
		byte[] packData=new byte[20];
		if (data.length==18){
			byte[] checkData=CRC.crc16(data);
			Utils.putBytes(packData,0,data,18);
			Utils.putBytes(packData,18,checkData,2);
		}
		return packData;
	}













}
