package com.example.dadao.dadaorobot.protocobuf.processor;

import android.content.Context;
import android.os.Message;

import com.example.dadao.dadaorobot.Login.view.LoginActivity;
import com.example.dadao.dadaorobot.util.Logger;
import com.google.protobuf.GeneratedMessageLite;
import DDRCommProto.BaseCmd;
/**
 * 当解析出IP和端口就关闭Udp连接并连接tcp
 */
public class ServerInformationProcessor extends BaseProcessor {
    @Override
    public void process(Context context, BaseCmd.CommonHeader commonHeader,GeneratedMessageLite msg) {
        Logger.w("udp广播处理程序");
        BaseCmd.bcLSAddr bcLSAddr=(BaseCmd.bcLSAddr) msg;
        for (BaseCmd.bcLSAddr.ServerInfo serverInfo:bcLSAddr.getLSInfosList()) {
            for (String ip:serverInfo.getIpsList()){
                    LoginActivity.addList(ip);
                    LoginActivity.tcpPort=serverInfo.getPort();
            }
            Message message=new Message();
            message.arg1=2;
            LoginActivity.getInstance().sendMessage(message);
        }
    }
}
