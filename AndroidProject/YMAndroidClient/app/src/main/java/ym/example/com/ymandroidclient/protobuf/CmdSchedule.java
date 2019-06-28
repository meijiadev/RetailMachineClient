package ym.example.com.ymandroidclient.protobuf;

import DDRCommProto.BaseCmd;


/**
 * 所有命令集合
 */
public class CmdSchedule {
    public static String broadcastServerIP="111.230.250.213";  //广域网远程服务器的Ip和端口
    public static int broadcastServerPort=8800;


    /**
     * 心跳包的数据
     * @return
     */
    public static BaseCmd.HeartBeat heartBeat(){
        BaseCmd.HeartBeat hb=BaseCmd.HeartBeat.newBuilder()
                .setWhatever("hb")
                .build();
        return hb;
    }

    /**
     * 局域网登录
     * @param password
     * @param account
     * @return
     */
    public static BaseCmd.reqLogin localLogin(String account, String password){
        BaseCmd.reqLogin mreqLogin=BaseCmd.reqLogin.newBuilder()
                .setUsername(account)
                .setUserpwd(password)
                .setType(BaseCmd.eCltType.eLocalAndroidClient)
                .build();
        return mreqLogin;
    }





    /**
     *获取rtmp 和语音对讲的IP 和端口
     * @return
     */
    public static BaseCmd.reqStreamAddr streamAddr(){
        BaseCmd.reqStreamAddr reqStreamAddr=BaseCmd.reqStreamAddr.newBuilder()
                .setNetworkType(BaseCmd.ChannelNetworkType.Local)
                .build();
        return reqStreamAddr;
    }


    /**
     * 命令的额外头部信息，决定命令发往那个服务
     * @return
     */
    public static BaseCmd.CommonHeader commonHeader(BaseCmd.eCltType eCltType){
        BaseCmd.CommonHeader header=BaseCmd.CommonHeader.newBuilder()
                .setFromCltType(BaseCmd.eCltType.eLocalAndroidClient)
                .setToCltType(eCltType)
                .addFlowDirection(BaseCmd.CommonHeader.eFlowDir.Forward)
                .build();
        return header;
    }

    /**
     * 语音对讲
     * @return
     */
    public static BaseCmd.reqAudioTalk audioTalk(BaseCmd.reqAudioTalk.eOpMode eOpMode){
        BaseCmd.reqAudioTalk reqAudioTalk=BaseCmd.reqAudioTalk.newBuilder()
                .setNetType(BaseCmd.reqAudioTalk.eNetType.eLocal)
                .setOpType(eOpMode)
                .build();
        return reqAudioTalk;
    }

    /**
     * 关机重启
     * @param eCmdIPCMode
     * @return
     */
    public static BaseCmd.reqCmdIPC cmdIPC(BaseCmd.eCmdIPCMode eCmdIPCMode ){
        BaseCmd.reqCmdIPC reqCmdIPC=BaseCmd.reqCmdIPC.newBuilder()
                .setMode(eCmdIPCMode)
                .build();
        return reqCmdIPC;
    }

}
