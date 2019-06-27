package com.example.dadao.dadaorobot.Socket;

import android.annotation.SuppressLint;
import android.content.Context;

import com.example.dadao.dadaorobot.protocobuf.MessageRoute;
import com.example.dadao.dadaorobot.protocobuf.dispatcher.BaseMessageDispatcher;
import com.example.dadao.dadaorobot.protocobuf.dispatcher.ClientMessageDispatcher;
import com.example.dadao.dadaorobot.util.Logger;

import java.io.IOException;

import java.io.OutputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;


public class UdpClient extends BaseSocketConnection {
    private Context context;
    public static UdpClient udpClient;
    public static DatagramSocket datagramSocket=null;
    public static DatagramPacket readPacket;
    public Thread readThread;

    public byte[] data=new byte[1024];
    public boolean udpLife;
    public StreamBuffer streamBuffer;

    public static UdpClient getInstance(Context context,BaseMessageDispatcher baseMessageDispatcher){
        if (udpClient==null){
            udpClient=new UdpClient(context,baseMessageDispatcher);
        }
        return udpClient;
    }

    @SuppressLint("NewApi")
    public UdpClient(Context context, BaseMessageDispatcher baseMessageDispatcher){
        this.context=context;
        m_MessageRoute=new MessageRoute(context,this,baseMessageDispatcher);
        streamBuffer=StreamBuffer.getInstance();
    }


    @Override
    public void close() {
        super.close();
        if (datagramSocket!=null){
            datagramSocket.close();
            udpLife=false;
            readThread=null;
        }

    }


    public boolean connect( int udpPort) throws IOException {
        if (!udpLife){
            try {
                datagramSocket=new DatagramSocket(udpPort);
                datagramSocket.setBroadcast(true);
                udpLife=true;
                readData();
                m_MessageRoute.parse();
            } catch (SocketException e) {
                e.printStackTrace();
                udpLife=false;
                Logger.e("udp连接失败");
                return false;
            }
        }
        return true;
    }

    public void readData(){
        if (readThread==null){
            readThread=new Thread(new Runnable() {
                @Override
                public void run() {
                    while (udpLife){
                        try {
                            readPacket=new DatagramPacket(data,data.length);
                            datagramSocket.receive(readPacket);
                            Logger.e("udp读取的长度"+readPacket.getLength());
                            streamBuffer.onDataReceived(data,readPacket.getLength());
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }
                }
            });
        }
        readThread.start();
    }















}
