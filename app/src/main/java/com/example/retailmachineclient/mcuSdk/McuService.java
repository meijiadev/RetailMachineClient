
package com.example.retailmachineclient.mcuSdk;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;

import com.example.retailmachineclient.util.Logger;
import com.hardware.SerialPort.SerialPort;

import java.io.IOException;

import androidx.annotation.NonNull;

public class McuService extends Service {
    private static SerialPort mPortMcu1 = new SerialPort(PortMCU.MCU1.getName());
    private Context mContext;
    private static McuDataProcessor mDataProcessor;
    public enum Device{
        MCU1(0x11,mPortMcu1);
        private int flag;
        private SerialPort serialPort;

        Device(int flag,SerialPort sp){
            this.flag=flag;
            this.serialPort=sp;
        }

        @Override
        public String toString() {
            return "Device{" +
                    "flag=" + flag +
                    ", serialPort=" + serialPort +
                    '}';
        }

        public int getFlag() {
            return flag;
        }

        public SerialPort getSerialPort() {
            return serialPort;
        }

        public McuDataProcessor getSerialPortDataProc() {
            if (serialPort.equals(mPortMcu1)) {
                return mDataProcessor;
            }
            return null;
        }
    }

    /**
     * 串口枚举，
     */
    public enum PortMCU{
        //出货串口
        MCU1("/dev/ttyS1", "MCU1"),
        //按键串口
        MCU2("/dev/ttyS0", "MCU2"),
        //会员卡串口
        MCU3("/dev/ttyS2", "MCU3"),
        //pos机串口
        MCU4("/dev/ttyS3", "MCU4");
        private String path;
        private String name;

        @NonNull
        @Override
        public String toString() {
            return name+"["+path+"]";
        }
        PortMCU(String path,String  name){
            this.path = path;
            this.name=name;
        }

        public String getName() {
            return name;
        }

        public String getPath() {
            return path;
        }

        public SerialPort getSerialPort() {
            switch (name) {
                case "MCU1":
                    return mPortMcu1;
                default:
                    break;
            }
            return null;
        }
    }


    /**
     * sdk初始化
     *
     * @param context
     * @return
     */
    public void init(Context context) {
        mContext = context;

        if (openSerPortRate(PortMCU.MCU1)) {
            Logger.e( "open " + PortMCU.MCU1.getPath() + " success!");
        } else {
            Logger.e( "open " + PortMCU.MCU1.getPath() + " error!");
        }
        mDataProcessor = new McuDataProcessor(mContext);
        //注册数据接受的回调函数
        mPortMcu1.setDataReceiver(mDataProcessor);
        // mPortMcu1.startReceived();-- 在open时已经启动
    }


    /**
     * 打开指定串口并设置波特率
     * @return
     */
    private boolean openSerPortRate(PortMCU portMCU) {
        boolean ret = false;
        try {
            if (portMCU.getSerialPort().isOpened()) {
                Logger.e("Open " + portMCU.toString() + ",ignore already opened!");
            } else {
                Logger.e( "Open " + portMCU.toString() + "...");
                ret = portMCU.getSerialPort().open(portMCU.path, McuSDK.PORT_RATE);
                Logger.e("Open " + portMCU.toString() + " ret=" + ret + " and startReceived thread!");
                portMCU.getSerialPort().startReceived();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return ret;
    }

    /**
     * 关闭指定端口
     * @param port
     */
    private void closeSerPort(PortMCU port) {
        try {
            if (port.getSerialPort().isOpened() == false) {
                Logger.e( "Close " + port.toString() + "...");
                port.getSerialPort().close();
            } else {
                Logger.e( "Close " + port.toString() + ",ignore already closed!");
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    /**
     * 直接向MCU发送字节数组，不经过组包
     */
    public boolean sendSerPortOrignalData(Device device, byte[] buf) {
        try {
            device.getSerialPort().write(buf);
            return true;
        } catch (IOException e) {
            e.printStackTrace();
        }
        return false;
    }

    /**
     * 向mcu发送命令,并等待返回的结果
     * @param mes
     * @param buf
     * @return
     */
    public boolean sendBufAndGetResult(String mes, Device device, byte[] buf) {
        McuDataProcessor dpm = device.getSerialPortDataProc();
        boolean rst = false;
        if (sendSerPortOrignalData(device, buf)){
            dpm.waitResult();
            rst = dpm.getCmdResult();
            Logger.e(mes + " result:" + rst);
        }else{
            Logger.e(mes + " -- send data fail!");
            rst = false;
        }
        return rst;
    }





    @Override
    public void onCreate() {
        super.onCreate();

    }

    public class LocalBinder extends Binder {
        public McuService getService() {
            return McuService.this;
        }
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        return START_NOT_STICKY;
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
        return mBinder;
    }
    private final IBinder mBinder = new LocalBinder();
}
