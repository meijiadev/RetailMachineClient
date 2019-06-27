package com.example.dadao.dadaorobot.Socket;



import android.os.Handler;
import android.os.Message;
import android.util.Log;
import com.example.dadao.dadaorobot.activity.NavigationActivity;
import com.example.dadao.dadaorobot.util.Logger;


import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;


/**
 *socket连接
 * 专用于语音对讲的socket
 */
public class AudioTcpClient extends BaseSocketConnection{

    private static final String TAG = "AudioTcpClient";
    public static AudioTcpClient audioTcpClient;
    public Socket mClient;
    public Thread mConnectThread;
    public byte[]bytes;
    public boolean isConnected;          //通话是否连接

    public OutputStream outputStream;
    public InputStream inputStream;

    private IAudioTcpClient iAudioTcpClient;

    public static AudioTcpClient getInstance(IAudioTcpClient iAudioTcpClient){
        if (audioTcpClient==null){
            audioTcpClient=new AudioTcpClient(iAudioTcpClient);
        }
        return audioTcpClient;
    }

    public AudioTcpClient(IAudioTcpClient iAudioTcpClient){
        this.iAudioTcpClient=iAudioTcpClient;
    }

    /**
     * 创建连接服务器方法
     * @param IP
     * @param port
     * @return 返回连接结果
     */
    public void createConnect(final String IP, final int port) throws InterruptedException {
        if (mConnectThread==null){                           //避免重复创建线程
            mConnectThread=new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        connectProcess(IP,port);
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    Log.e("返回结果0","----"+isConnected);
                }
            });
        }
        mConnectThread.start();
        Log.e("返回结果：","----"+isConnected);
    }

    /**
     * 发送音频数据
     * @param data
     */
    public void sendData(final byte[]data){
        if (outputStream!=null){
            try {
                Logger.i("-----发送语音");
                outputStream.write(data);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }


    /**
     * 读取音频数据
     * @return
     */
    public void  readData() throws IOException {
        Handler handler=NavigationActivity.getHandler();
        while (isConnected){
            if (inputStream!=null){
                int a=inputStream.available();
                Logger.w("-----a:"+a);
                bytes=new byte[a];
                inputStream.read(bytes,0,a);
                a=inputStream.available();
                if (handler!=null){
                    Message message=new Message();
                    message.arg1=0;
                    message.obj=bytes;
                    handler.sendMessage(message);
                }
            }
        }
    }


    /**
     * 连接并验证
     * @param ip
     * @param port
     * @return
     */
    private void connectProcess(String ip,int port) throws IOException {
        if (mClient==null){
            try {
                mClient=new Socket(ip,port);          //与服务端连接
                Log.e(TAG,"服务器连接成功");
                outputStream=mClient.getOutputStream();
                inputStream=mClient.getInputStream();
                iAudioTcpClient.isConnected(true);
            } catch (IOException e) {
                e.printStackTrace();
                iAudioTcpClient.isConnected(false);
                Logger.e("连接失败");
                mConnectThread=null;
                disConnect();
                isConnected=false;
            }
        }
       isConnected=true;
    }

    /**
     * 断开链接
     * @throws IOException
     */
    public void disConnect() throws IOException {
        if (mClient!=null){
            Logger.e("----------断开连接");
            mClient.close();
            mClient=null;
            isConnected=false;
        }
        mConnectThread=null;
        audioTcpClient=null;
    }

}
