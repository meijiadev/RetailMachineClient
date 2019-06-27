package com.example.dadao.dadaorobot.protocobuf.processor;

import android.content.Context;
import android.os.Message;

import com.example.dadao.dadaorobot.Login.view.LoginActivity;
import com.example.dadao.dadaorobot.util.Logger;
import com.google.protobuf.GeneratedMessageLite;

import DDRCommProto.BaseCmd;
import DDRCommProto.RemoteCmd;

public class RspRemoteServerListProcessor extends BaseProcessor {
    @Override
    public void process(Context context, BaseCmd.CommonHeader commonHeader,GeneratedMessageLite msg) {
        super.process(context,commonHeader, msg);
        RemoteCmd.rspRemoteServerList rspRemoteServerList= (RemoteCmd.rspRemoteServerList) msg;
        for (RemoteCmd.rspRemoteServerList.RemoteServer remoteServer:rspRemoteServerList.getServersList()){
            LoginActivity.addServerList(remoteServer);
        }
        Message message=new Message();
        message.arg1=1;
        LoginActivity.getInstance().sendMessage(message);
        Logger.e("----接受服务区信息");

    }
}
