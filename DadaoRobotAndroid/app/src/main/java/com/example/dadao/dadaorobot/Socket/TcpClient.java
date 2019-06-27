package com.example.dadao.dadaorobot.Socket;

import android.annotation.SuppressLint;
import android.content.Context;

import com.example.dadao.dadaorobot.protocobuf.GuIdInfo;
import com.example.dadao.dadaorobot.protocobuf.dispatcher.BaseMessageDispatcher;
import com.example.dadao.dadaorobot.protocobuf.dispatcher.ClientMessageDispatcher;
import com.example.dadao.dadaorobot.protocobuf.MessageRoute;
import com.example.dadao.dadaorobot.util.Logger;
import com.google.protobuf.GeneratedMessageLite;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.text.SimpleDateFormat;
import java.util.Date;

import DDRCommProto.BaseCmd;
/**
 * Tcp客户端
 */
public class TcpClient extends BaseSocketConnection {

    public static TcpClient mTcpClientConnector;
    private Socket mClient;
    private Thread mConnectThread;
    private InputStream in;
    private OutputStream out;
    private int bufferSize=1024;
    byte[] buffer=new byte[bufferSize];
    public  boolean isHeartBeat;
    private Context context;
    private StreamBuffer streamBuffer;
    private ITcpClient iTcpClient;



    public static TcpClient getInstance(Context context,BaseMessageDispatcher baseMessageDispatcher){
        if (mTcpClientConnector==null){
            mTcpClientConnector=new TcpClient(context,baseMessageDispatcher);
        }
        return mTcpClientConnector;
    }

    public TcpClient(Context context, BaseMessageDispatcher baseMessageDispatcher){
        this.context=context;
        mTcpClientConnector=this;
        m_MessageRoute=new MessageRoute(context,this,baseMessageDispatcher);
        streamBuffer=StreamBuffer.getInstance();
    }

    public void setiTcpClient(ITcpClient iTcpClient){
        this.iTcpClient=iTcpClient;
    }

    @Override
    Socket getSocket() {
        return mClient;
    }



    /**
     * 创建线程进行Tcp网络连接
     * @param mSerIP
     * @param mSerport
     */
    public void creatConnect(final String mSerIP,final int mSerport){
        Logger.e("--->>Jintulianjie");
        if (mConnectThread==null){
            mConnectThread=new Thread(new Runnable() {
                @Override
                public void run() {
                    Logger.e("------------------开启线程，进入与服务端连接的方法--------------------");
                    connectAndProcess(mSerIP,mSerport);

                }
            });
            mConnectThread.start();
        }
    }

    private void connectAndProcess(String mSerIP,int mSerPort)  {
        Logger.e("正在与服务端连接....");
        if (mClient==null){
            try {
                mClient=new Socket(mSerIP,mSerPort);
                mClient.setSoTimeout(5000);
                Logger.e("-------------服务器连接成功！---------------");
                in=mClient.getInputStream();
                out=mClient.getOutputStream();
                isHeartBeat=true;
                startHeartBeat();
                startReceiving();
                iTcpClient.tcpConnected();
            } catch (IOException e) {
                e.printStackTrace();
                Logger.e("-------------服务器连接失败！---------------");
                iTcpClient.connectFailure();
                mConnectThread=null;
            }
        }
    }

    public void startReceiving() {
        Logger.e("----------开始读取信息-----------");
        new Thread(new Runnable() {
            @SuppressLint("NewApi")
            @Override
            public void run() {
                while (isHeartBeat){
                    try {
                        int len=in.read(buffer);       // len 实际读取的长度
                        if (len==-1){
                            Logger.e("-------------服务器连接主动断开！---------------");
                            disConnect();
                            iTcpClient.tcpDisConnected();
                        }else {
                            streamBuffer.onDataReceived(buffer,len);
                        }
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        }).start();

    }


    public  void startHeartBeat(){
        Logger.e("开始发送心跳包");
        new Thread(new Runnable() {
            @Override
            public void run() {
                    BaseCmd.HeartBeat hb=BaseCmd.HeartBeat.newBuilder()
                            .setWhatever("hb")
                            .build();
                    while (isHeartBeat){
                        if (mClient.isConnected()&&!mClient.isClosed()){
                            try {
                                out.write(m_MessageRoute.serialize(null,hb));
                                Thread.sleep(2000);
                            }catch (IOException e) {
                                Logger.e("--------------socket连接断开-------------");
                                try {
                                    disConnect();
                                }catch (IOException e1) {
                                    e1.printStackTrace();
                                }
                                iTcpClient.tcpServerFinish();
                                e.printStackTrace();
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                        }else {
                            try {
                                disConnect();
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                        }
                    }
            }
        }).start();

    }


    /**
     * 异步发送消息
     * @param message
     */
    @Override
    public void sendData(final BaseCmd.CommonHeader commonHeader, final GeneratedMessageLite message){
        if (mClient!=null&&mClient.isConnected()){
            new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            out.write(m_MessageRoute.serialize(commonHeader,message));
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }
                }).start();
        }
    }

    GeneratedMessageLite msg;
    /**
     * 同步发送消息，需要等待返回信息才能再次发送
     * 这是一个耗时操作
     */
    public synchronized GeneratedMessageLite sendSync(final BaseCmd.CommonHeader commonHeader, final GeneratedMessageLite messageLite){
        if (mClient!=null){
            String guID=commonHeader.getGuid();
            try {
                out.write(m_MessageRoute.serialize(commonHeader,messageLite));
            } catch (IOException e){
                e.printStackTrace();
            }
            long startTime=getLongTime();
            msg=null;
            do {
                long endTime=getLongTime();
                long useTime=endTime-startTime;
                if (useTime>5000){
                    return msg ;
                }
            }while (GuIdInfo.getInstance().getGuId()==guID);
            msg=GuIdInfo.getInstance().getMessageLite();
        }
        return msg;
    }


    /**
     * 断开链接
     * @throws IOException
     *
     */
    @SuppressLint("NewApi")
    public  void disConnect() throws IOException {
        if (mClient!=null){
            m_MessageRoute.parseThread=null;
            streamBuffer.clearArray();
            Logger.e("断开连接");
            isHeartBeat=false;
            mClient.close();
            mClient=null;
            mConnectThread=null;

        }
    }

    /**
     * 根据String型时间，获取long型时间，单位毫秒
     * @return long型时间
     */
    public long getLongTime() {
        Date date = null;
        SimpleDateFormat inputFormat = new SimpleDateFormat("yyyy-MM-dd hh:mm:ss:SSS");
        try {
            date = inputFormat.parse(new SimpleDateFormat("yyyy-MM-dd hh:mm:ss:SSS").format(new Date()));
        } catch (Exception e) {
            e.printStackTrace();
        }
        return date.getTime();
    }



}
