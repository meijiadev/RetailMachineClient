package com.example.dadao.dadaorobot.protocobuf.processor;

import android.content.Context;
import android.os.Handler;
import android.os.Message;

import com.example.dadao.dadaorobot.activity.MainActivity;
import com.example.dadao.dadaorobot.util.Logger;
import com.google.protobuf.GeneratedMessageLite;
import DDRCommProto.BaseCmd;
public class RspCmdSetWorkPathProcessor extends BaseProcessor {

    @Override
    public void process(Context context, BaseCmd.CommonHeader commonHeader, GeneratedMessageLite msg) {
        super.process(context,commonHeader, msg);
        BaseCmd.rspCmdSetWorkPath rspCmdSetWorkPath= (BaseCmd.rspCmdSetWorkPath) msg;
        Logger.e("----"+rspCmdSetWorkPath.getType());
        switch (rspCmdSetWorkPath.getTypeValue()){
            case 0:
                Handler handler=MainActivity.getHandler();
                if (handler!=null){
                    Message message=new Message();
                    message.arg1=3;
                    message.obj=true;
                    handler.sendMessage(message);
                }
                Logger.e("------路径修改成功");
                break;
            case 1:
                Logger.e("------路径修改失败");
                Handler handler1=MainActivity.getHandler();
                if (handler1!=null){
                    Message message1=new Message();
                    message1.arg1=3;
                    message1.obj=false;
                    handler1.sendMessage(message1);
                }
                break;
        }
    }

}
