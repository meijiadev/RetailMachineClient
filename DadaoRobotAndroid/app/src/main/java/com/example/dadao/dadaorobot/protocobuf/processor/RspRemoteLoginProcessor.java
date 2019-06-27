package com.example.dadao.dadaorobot.protocobuf.processor;

import android.content.Context;
import android.os.Message;

import com.example.dadao.dadaorobot.Login.view.LoginActivity;
import com.example.dadao.dadaorobot.util.Logger;
import com.google.protobuf.GeneratedMessageLite;

import java.util.List;

import DDRCommProto.BaseCmd;
import DDRCommProto.RemoteCmd;

public class RspRemoteLoginProcessor extends BaseProcessor {
    List<RemoteCmd.rspRemoteLogin.LSEntity> list;
    @Override
    public void process(Context context, BaseCmd.CommonHeader commonHeader,GeneratedMessageLite msg) {
        super.process(context,commonHeader, msg);
        RemoteCmd.rspRemoteLogin rspRemoteLogin= (RemoteCmd.rspRemoteLogin) msg;
        switch (rspRemoteLogin.getRetcode().getNumber()){
            case 0:
                list=rspRemoteLogin.getLslistList();
                LoginActivity.addLoginList(list);
               // Logger.e("广域网登陆成功"+list.size()+list.get(0).getUdid());
                Message message=new Message();
                message.arg1=3;
                LoginActivity.getInstance().sendMessage(message);
                break;
            case 1:
                Logger.e("服务器繁忙");
                Message message1=new Message();
                message1.arg1=4;
                LoginActivity.getInstance().sendMessage(message1);
                break;
            case 2:
                Logger.e("服务器连接达到上限");


        }
    }
}
