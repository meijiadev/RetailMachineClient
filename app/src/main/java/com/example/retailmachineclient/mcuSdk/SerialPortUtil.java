package com.example.retailmachineclient.mcuSdk;

import com.example.retailmachineclient.util.Logger;
import com.example.retailmachineclient.util.Utils;

import com.kongqw.serialportlibrary.SerialPortManager;
import com.kongqw.serialportlibrary.listener.OnOpenSerialPortListener;
import com.kongqw.serialportlibrary.listener.OnSerialPortDataListener;

import java.io.File;

public class SerialPortUtil implements OnOpenSerialPortListener,OnSerialPortDataListener {
    private SerialPortManager mSerialPortManager;
    private static SerialPortUtil serialPortUtil;
    private IDataProcessor mDataProcessor;


    public static SerialPortUtil getInstance() {
        if (serialPortUtil==null){
            serialPortUtil=new SerialPortUtil();
        }
        return serialPortUtil;
    }

    public SerialPortUtil() {
        mSerialPortManager=new SerialPortManager();
        mSerialPortManager.setOnOpenSerialPortListener(this);
        mSerialPortManager.setOnSerialPortDataListener(this);
        mDataProcessor=new McuDataProcessor();
    }


    

    public boolean open(String devicePath,int portSpeed){
        return mSerialPortManager.openSerialPort(new File(devicePath), portSpeed);
    }

    public void close(){
        mSerialPortManager.closeSerialPort();
    }

    /**
     * 发送数据
     * @return
     */
    public boolean sendData(byte[]dataBytes){
        return mSerialPortManager.sendBytes(dataBytes);
    }


    @Override
    public void onSuccess(File file) {
        Logger.e("串口打开成功！"+file.getPath());
    }

    @Override
    public void onFail(File file, Status status) {
        Logger.e("串口打开失败！"+file.getPath());
    }

    @Override
    public void onDataReceived(byte[] bytes) {
        Logger.e("接受串口数据："+ Utils.byteBufferToHexString(bytes));
        mDataProcessor.onDataReceive(bytes);
    }

    @Override
    public void onDataSent(byte[] bytes) {
        Logger.e("发送串口数据："+ Utils.byteBufferToHexString(bytes));
    }
}
