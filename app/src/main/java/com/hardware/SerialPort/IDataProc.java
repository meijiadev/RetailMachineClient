package com.hardware.SerialPort;

/**
 * desc:数据回调接口
 */
public interface IDataProc {
    void onDataReceived(byte[] buffer);
}
