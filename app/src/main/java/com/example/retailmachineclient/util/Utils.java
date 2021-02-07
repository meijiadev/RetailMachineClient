package com.example.retailmachineclient.util;

public class Utils {
	public static byte[] getBytes(byte[] buffer, int offset, int length) {
		int num = 0;
		byte[] dstbyte = new byte[length];
		for (num = 1; num <= length; num++) {
			dstbyte[num - 1] = buffer[offset + num - 1];
		}
		return dstbyte;
	}

	public static int putBytes(byte[] dst, int offset,byte[] source, int length) {
		int num = 0;
		for (num = 0; num < length; num++) {
			dst[offset + num] = source[num];
		}
		return offset+num;
	}

	public static byte[] getBytes(Byte[] buffer, int offset, int length) {
		int num = 0;
		byte[] dstbyte = new byte[length];
		for (num = 1; num <= length; num++) {
			dstbyte[num - 1] = buffer[offset + num - 1];
		}
		return dstbyte;
	}

	/**
	 * 把数组中指定长度的内容转换为字符丿
	 * @param buffer
	 * @param length
	 * @return
	 */
	public static String byteBufferToHexString(byte[] buffer, int length) {
		StringBuilder mBuilder = new StringBuilder();
		int startIndex = 0;
		for (int i = startIndex; i < length; i++) {
			int v = buffer[i] & 0xFF;
			String hv = Integer.toHexString(v);
			if (hv.length() < 2) {
				mBuilder.append(0);
			}
			mBuilder.append(hv);
		}
		return "len:" + length + " [ " + mBuilder.toString().toUpperCase()
				+ " ] ";
	}

	/**
	 * 数组转为16进制字符串输凿
	 *
	 * @param buffer
	 * @return
	 */
	public static String byteBufferToHexString(byte[] buffer) {
		return byteBufferToHexString(buffer,buffer.length);
	}

	/**
	 * 把一个整数变化为丿个小敿
	 * 妿115-> 0.115
	 * 50->0.5
	 * @param v
	 * @return
	 */
	public static double intToDec(int v){
		if (v < 10){
			return v / 10.0;
		}else if (v < 100){
			return v/100.0;
		}else if (v < 1000){
			return v / 1000.0;
		}else {
			return v/ 10000.0;
		}
	}


	//byte ԫ int քРۥתۻ
	public static byte intToByte(int x) {
		return (byte) x;
	}

	public static int byteToInt(byte b) {
		return b & 0xFF;
	}


}
