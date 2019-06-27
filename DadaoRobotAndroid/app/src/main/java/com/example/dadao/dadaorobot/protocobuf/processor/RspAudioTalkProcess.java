package com.example.dadao.dadaorobot.protocobuf.processor;

import android.content.Context;
import android.os.Message;

import com.example.dadao.dadaorobot.activity.NavigationActivity;
import com.example.dadao.dadaorobot.util.Logger;
import com.google.protobuf.GeneratedMessageLite;
import DDRCommProto.BaseCmd;
public class RspAudioTalkProcess extends BaseProcessor {
    @Override
    public void process(Context context, BaseCmd.CommonHeader commonHeader,GeneratedMessageLite msg) {
        super.process(context,commonHeader, msg);
        BaseCmd.rspAudioTalk rspAudioTalk= (BaseCmd.rspAudioTalk) msg;
        if (rspAudioTalk.getStatusValue()== 0){
            Logger.e("正在通话...");
            Message message=new Message();
            message.arg1=2;
            NavigationActivity.getHandler().sendMessage(message);
        }else if (rspAudioTalk.getStatusValue()==2){
            Logger.e("-----返回停止对讲命令");
        }
    }
}
