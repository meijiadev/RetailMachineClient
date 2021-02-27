package com.example.retailmachineclient.socket;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.view.Gravity;
import android.widget.TextView;

import com.example.retailmachineclient.protocobuf.CmdSchedule;
import com.example.retailmachineclient.protocobuf.MessageRoute;
import com.example.retailmachineclient.protocobuf.dispatcher.BaseMessageDispatcher;
import com.example.retailmachineclient.util.ActivityStackManager;
import com.example.retailmachineclient.util.Logger;
import com.google.protobuf.ByteString;
import com.google.protobuf.GeneratedMessageLite;
import com.google.protobuf.InvalidProtocolBufferException;
import com.hjq.xtoast.OnClickListener;
import com.hjq.xtoast.XToast;
import com.xuhao.didi.core.iocore.interfaces.ISendable;
import com.xuhao.didi.core.pojo.OriginalData;
import com.xuhao.didi.core.protocol.IReaderProtocol;
import com.xuhao.didi.socket.client.sdk.OkSocket;
import com.xuhao.didi.socket.client.sdk.client.ConnectionInfo;
import com.xuhao.didi.socket.client.sdk.client.OkSocketOptions;
import com.xuhao.didi.socket.client.sdk.client.action.SocketActionAdapter;
import com.xuhao.didi.socket.client.sdk.client.connection.IConnectionManager;

import org.greenrobot.eventbus.EventBus;

import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;

import DDRCommProto.BaseCmd;


/**
 * 基于OkSocket库的TCP客户端
 * create by ezreal.mei 2019/10/16
 */
public class TcpClient extends BaseSocketConnection {
    public Context context;
    public static TcpClient tcpClient;
    private ConnectionInfo info;
    public IConnectionManager manager;
    private boolean isConnected; //是否连接
    private SocketCallBack socketCallBack;
    private byte[] heads=new byte[4];  //存储头部长度信息的字节数组
    private byte [] bodyLenths=new byte[4];        //存储body体的信息长度
    private XToast xToast;

    /**
     * 获取客户端
     * @param context
     * @param baseMessageDispatcher
     * @return
     */
    public static TcpClient getInstance(Context context, BaseMessageDispatcher baseMessageDispatcher){
        if (tcpClient==null){
            synchronized (TcpClient.class){
                if (tcpClient==null){
                    tcpClient=new TcpClient(context,baseMessageDispatcher);
                }
            }
        }
        return tcpClient;
    }


    private TcpClient(Context context, BaseMessageDispatcher baseMessageDispatcher) {
        this.context=context.getApplicationContext();         //使用Application的context 避免造成内存泄漏
        m_MessageRoute=new MessageRoute(context,this,baseMessageDispatcher);
    }

    /**
     * 创建连接通道
     * @param ip
     * @param port
     */
    public synchronized void createConnect(String ip, int port){
        Logger.e("连接tcp:"+ip+";"+port);
        info=new ConnectionInfo(ip,port);
        manager=OkSocket.open(info);
        OkSocketOptions.Builder clientOptions=new OkSocketOptions.Builder();
        clientOptions.setPulseFeedLoseTimes(100);
        clientOptions.setReaderProtocol(new ReaderProtocol());
        manager.option(clientOptions.build());
        socketCallBack=new SocketCallBack();
        manager.registerReceiver(socketCallBack);
        manager.connect();
    }




    /**
     * 连接的状态信息
     */
    public class SocketCallBack extends SocketActionAdapter{


        public SocketCallBack() {
            super();
        }

        /**
         * 当客户端连接成功会回调这个方法
         * @param info
         * @param action
         */
        @Override
        public void onSocketConnectionSuccess(ConnectionInfo info, String action) {
            isConnected=true;
            Logger.e("--------连接成功---------");
            Activity activity= ActivityStackManager.getInstance().getTopActivity();
            sendHeartBeat();
        }

        /**
         * 当客户端连接失败会调用
         * @param info
         * @param action
         * @param e
         */
        @Override
        public void onSocketConnectionFailed(ConnectionInfo info, String action, Exception e) {
            isConnected=false;
        }

        /**
         * 当连接断开时会调用此方法
         * @param info
         * @param action
         * @param e
         */
        @Override
        public void onSocketDisconnection(ConnectionInfo info, String action, Exception e) {
            isConnected=false;
            Activity activity=ActivityStackManager.getInstance().getTopActivity();
            if (activity!=null){
                if (activity.getLocalClassName().contains("LoginActivity")){
                    disConnect();
                }else {
                    Logger.e("网络连接断开，当前处于"+activity.getLocalClassName());
                }
            }
        }

        /**
         * 当接收tcp服务端数据时调用此方法
         * @param info
         * @param action
         * @param data
         */
        @Override
        public void onSocketReadResponse(ConnectionInfo info, String action, OriginalData data) {
            byte[] headBytes=data.getHeadBytes();
            System.arraycopy(headBytes,8,heads,0,4);
            int headLength=bytesToIntLittle(heads,0);
            try {
                m_MessageRoute.parseBody(data.getBodyBytes(),headLength);
            } catch (InvalidProtocolBufferException e) {
                e.printStackTrace();
            }

        }

        @Override
        public void onSocketWriteResponse(ConnectionInfo info, String action, ISendable data) {
            Logger.d("---------"+action);
        }
    }

    /**
     * 自定义解析头
     */
    public class ReaderProtocol implements IReaderProtocol{

        /**
         * 返回固定的头部长度
         * @return
         */
        @Override
        public int getHeaderLength() {
            return 12;
        }

        /**
         * 返回不固定长的body包长度
         * @param header
         * @param byteOrder
         * @return
         */
        @Override
        public int getBodyLength(byte[] header, ByteOrder byteOrder) {
            if (header == null || header.length < getHeaderLength()) {
                return 0;
            }
            System.arraycopy(header,4,bodyLenths,0,4);
            return bytesToIntLittle(bodyLenths,0)-8;
        }
    }




    /**
     * 以小端模式将byte[]转成int
     */
    public int bytesToIntLittle(byte[] src, int offset) {
        int value;
        value = (int) ((src[offset] & 0xFF)
                | ((src[offset + 1] & 0xFF) << 8)
                | ((src[offset + 2] & 0xFF) << 16)
                | ((src[offset + 3] & 0xFF) << 24));
        return value;
    }

    /**
     * 喂狗操作，否则当超过一定次数的心跳发送,未得到喂狗操作后,狗将会将此次连接断开重连.
     */
    public void feedDog(){
        if (manager!=null){
            manager.getPulseManager().feed();
           Logger.d("---喂狗");
        }
    }

    /**
     * 断开连接
     */
    public void disConnect(){
        if (manager!=null){
            manager.unRegisterReceiver(socketCallBack);
            manager.disconnect();
            isConnected=false;
            manager=null;
            Logger.e("断开tcp连接");
        }
    }


    /**
     * 发送消息
     * @param commonHeader
     * @param message
     */
    public void sendData(BaseCmd.CommonHeader commonHeader, GeneratedMessageLite message){
        if (manager!=null){
            byte[] data=m_MessageRoute.serialize(commonHeader,message);
            Logger.d("--------sendData");
            manager.send(new SendData(data));
        }
    }


    /**
     * 持续发送心跳
     */
    public void sendHeartBeat(){
        final BaseCmd.HeartBeat hb=BaseCmd.HeartBeat.newBuilder()
                .setWhatever("hb")
                .build();
        new Thread(new Runnable() {
            @Override
            public void run() {
                while (isConnected&&manager!=null){
                    try {
                        manager.getPulseManager().setPulseSendable(new PulseData(m_MessageRoute.serialize(null,hb))).pulse();
                        Logger.d("发送心跳包");
                        Thread.sleep(3000);
                    }catch (NullPointerException e){
                        e.printStackTrace();
                    }catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        }).start();
    }


    public void setConnected(boolean connected) {
        isConnected = connected;
    }

    public boolean isConnected() {
        return isConnected;
    }





    /**
     * 还没采集完时退出采集模式，再进入请求之前采集过的栅格图
     */
    public void getAllLidarMap(){
        Logger.e("获取所有栅格地图");
        BaseCmd.reqGetAllLidarCurSubMap reqGetAllLidarCurSubMap= BaseCmd.reqGetAllLidarCurSubMap.newBuilder()
                .build();
        sendData(CmdSchedule.commonHeader(BaseCmd.eCltType.eModuleServer),reqGetAllLidarCurSubMap);
    }
    /**
     * 请求文件（txt、png) 刷新文件列表
     */
    public void requestFile() {
        //final ByteString currentFile = ByteString.copyFromUtf8("OneRoute_*" + "/bkPic.png");
        BaseCmd.reqClientGetMapInfo reqClientGetMapInfo=BaseCmd.reqClientGetMapInfo.newBuilder()
                .build();
        sendData(CmdSchedule.commonHeader(BaseCmd.eCltType.eModuleServer),reqClientGetMapInfo);
        Logger.e("请求文件中....");
    }


    /**
     * 发送线速度，角速度
     * @param lineSpeed
     * @param palstance
     */
    public void sendSpeed(final float lineSpeed, final float palstance) {
        BaseCmd.reqCmdMove reqCmdMove = BaseCmd.reqCmdMove.newBuilder()
                .setLineSpeed(lineSpeed)
                .setAngulauSpeed(palstance)
                .build();
        BaseCmd.CommonHeader commonHeader = BaseCmd.CommonHeader.newBuilder()
                .setFromCltType(BaseCmd.eCltType.eLocalAndroidClient)
                .setToCltType(BaseCmd.eCltType.eModuleServer)
                .addFlowDirection(BaseCmd.CommonHeader.eFlowDir.Forward)
                .build();
        sendData(commonHeader, reqCmdMove);

    }



    /**
     * 退出当前模式
     */
    public void exitModel() {
        BaseCmd.reqCmdEndActionMode reqCmdEndActionMode = BaseCmd.reqCmdEndActionMode.newBuilder()
                .setError("noError")
                .build();
        BaseCmd.CommonHeader commonHeader = BaseCmd.CommonHeader.newBuilder()
                .setFromCltType(BaseCmd.eCltType.eLocalAndroidClient)
                .setToCltType(BaseCmd.eCltType.eModuleServer)
                .addFlowDirection(BaseCmd.CommonHeader.eFlowDir.Forward)
                .build();
        sendData(commonHeader, reqCmdEndActionMode);
    }




    /**
     * 处理虚拟墙（该命令会修改bkPic_obs.png的地图）
     * type=6 ->添加虚拟墙，由多个线段组成。就是 reqEditorLidarMap 中的 vlSet。
     * type=7 -> 移除虚拟墙，由多个线段组成。就是 reqEditorLidarMap 中的 vlSet。
     */
    public void reqEditMapVirtual(int type, List<BaseCmd.reqEditorLidarMap.VirtualLineItem> virtualLineItems, String mapName){
        BaseCmd.reqEditorLidarMap reqEditorLidarMap=BaseCmd.reqEditorLidarMap.newBuilder()
                .setTypeValue(type)
                .addAllVlSet(virtualLineItems)
                .setOneroutename(ByteString.copyFromUtf8(mapName))
                .build();
        sendData(CmdSchedule.commonHeader(BaseCmd.eCltType.eModuleServer),reqEditorLidarMap);
    }

    /**
     * 获取上位机版本信息
     */
    public void getHostComputerEdition() {
        BaseCmd.reqGetSysVersion reqGetSysVersion = BaseCmd.reqGetSysVersion.newBuilder()
                .build();
        sendData(CmdSchedule.commonHeader(BaseCmd.eCltType.eModuleServer), reqGetSysVersion);
    }




    /**
     * 关机or重启
     * @param eCmdIPCMode
     */
    public void reqCmdIpcMethod(BaseCmd.eCmdIPCMode eCmdIPCMode ){
        BaseCmd.reqCmdIPC reqCmdIPC=BaseCmd.reqCmdIPC.newBuilder()
                .setMode(eCmdIPCMode)
                .build();
        sendData(CmdSchedule.commonHeader(BaseCmd.eCltType.eModuleServer),reqCmdIPC);
    }

    /**
     * 去充电
     */
    public void goToCharge(){
        BaseCmd.reqCmdStartActionMode reqCmdStartActionMode= BaseCmd.reqCmdStartActionMode.newBuilder()
                .setMode(BaseCmd.eCmdActionMode.eReCharging)
                .build();
        sendData(CmdSchedule.commonHeader(BaseCmd.eCltType.eModuleServer),reqCmdStartActionMode);
    }

    /**
     * 以下是广域网临时使用
     *
     */
    /**
     * 获取机器人状态信息
     */
    private void postgetRobotStatus(int type){
        BaseCmd.reqGetRobotStatusAndEnv reqGetRobotStatusAndEnv=BaseCmd.reqGetRobotStatusAndEnv.newBuilder()
                .setNType(type)
                .build();
        tcpClient.sendData(CmdSchedule.commonHeader(BaseCmd.eCltType.eModuleServer),reqGetRobotStatusAndEnv);
    }

    /**
     * 获取基础状态信息
     * @param num
     */
    private boolean isRobotStatus=true;
    public void requestNotifyBase (int num){
        new Thread(()->{
            try {
                while (isRobotStatus) {
                    switch (num){
                        case 0:
//                            Logger.d("单台广域网获取机器状态信息");
                            //单台广域网
                            postgetRobotStatus(4);
                            break;
                    }
                    Thread.sleep(500);
//                    Logger.d("---------请求的基础信息");
                }
            }catch (Exception e){
                e.printStackTrace();
            }
        }).start();
    }
}
