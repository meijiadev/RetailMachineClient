package com.hardware.SerialPort;

import android.os.ParcelFileDescriptor;

import com.example.retailmachineclient.util.Logger;
import com.example.retailmachineclient.base.BaseThread;

import java.io.IOException;
import java.nio.ByteBuffer;

/**
 * desc:串口通讯类
 * time:2021/1/7
 * author: MJ
 */
public class SerialPort {
    private String mcuName;   //串口名字
    private ParcelFileDescriptor mFileDescriptor;    // 原始文件描述符的一个复制,操作同一个底层文件流以及文件位置指针

    public SerialPort(String name){
        this.mcuName=name;
    }

    /**
     * 打开串口
     * @param devFile
     * @param speed
     * @return
     * @throws IOException
     */
    public boolean open(String devFile,int speed) throws IOException{
        mFileDescriptor=native_open(devFile,speed);
        return (mFileDescriptor!=null);
    }

    /**
     * 关闭串口
     * @throws IOException
     */
    public void close() throws IOException{
        if (mFileDescriptor != null) {
            mFileDescriptor.close();
            mFileDescriptor=null;
        }
        native_close();
    }

    /**
     * 返回串口是否已经打开
     * @return
     */
    public boolean isOpened(){
        return mFileDescriptor!=null;
    }

    public int read(ByteBuffer buffer)throws IOException {
        if (buffer.isDirect()){
            return native_read_direct(buffer,buffer.remaining());
        }else if (buffer.hasArray()){
            return native_read_array(buffer.array(),buffer.remaining());
        }else{
            throw new IllegalArgumentException("buffer is not direct and has no array");
        }
    }

    public void write(ByteBuffer buffer, int length) throws IOException {
        Logger.w(">> Write:" + Utils.byteBufferToHexString(buffer.array(),length));
        if (buffer.isDirect()) {
            native_write_direct(buffer, length);
        } else if (buffer.hasArray()) {
            native_write_array(buffer.array(), length);
        } else {
            throw new IllegalArgumentException("buffer is not direct and has no array");
        }
    }
    /**
     * 发送buffer里的指定长度到串口
     * @param buffer
     * @param length
     * @throws IOException
     */
    public void write(byte buffer[], int length) throws IOException {
        ByteBuffer buffer2 = ByteBuffer.wrap(buffer);
        write(buffer2, length);
    }

    /**
     * 发送buffer里全部内容到串口
     * @param buffer
     * @throws IOException
     */
    public void write(byte buffer[]) throws IOException {
        ByteBuffer buffer2 = ByteBuffer.wrap(buffer);
        write(buffer2, buffer.length);
    }


    public void sendBreak() {
        native_send_break();
    }

    //加载动态库
    static{
        System.loadLibrary("SerialPortExt");
    }

    private native ParcelFileDescriptor native_open(String path, int speed);
    private native void native_close();
    private native int native_read_array(byte[] buffer,int length)throws IOException;
    private native int native_read_direct(ByteBuffer buffer, int length) throws IOException;
    private native void native_write_array(byte[] buffer, int length) throws IOException;
    private native void native_write_direct(ByteBuffer buffer, int length) throws IOException;
    private native void native_send_break();

    /**
     * 数据读取线程
     */
    private class ReadThread extends BaseThread {
        private int ret;
        private boolean isReadStop;
        private ByteBuffer mCacheBuffer=ByteBuffer.allocateDirect(1024);
        public void setReadStop(boolean isReadStop){
            this.isReadStop=isReadStop;
            mCacheBuffer=ByteBuffer.allocateDirect(1024);
        }

        @Override
        public void run() {
            while(true){
                if (isReadStop){
                    break;
                }
                try {
                    //Log.d(TAG, "calling read >>" + this);
                    mCacheBuffer.clear();
                    ret = read(mCacheBuffer);
                    if (ret > 0) {
                        byte[] buf = new byte[ret];
                        mCacheBuffer.get(buf, 0, ret);

                        // 不能使用下面这句,将会把整个cache buffer都得到,如上会得到1024个字节的buf
                        //byte buf[] = mCacheBuffer.array();
                        Logger.e( "<< recei:" + Utils.byteBufferToHexString(buf));

                      /*  if (mSerialPortDataReceiver!=null){
                            mSerialPortDataReceiver.onDataReceived(buf);
                        }*/
                    }
                } catch (Exception e) {
                    Logger.e( "read Exception..."+e);
                    try {

                        Thread.sleep(100L);
                    } catch (Exception e1) {
                        Logger.e("read Exception,Thread sleep excetion! ..." + e1);
                    }
                } finally {
                }
            }
            super.run();
        }
    }
}
