package com.example.dadao.dadaorobot.protocobuf.processor;

import android.content.Context;
import android.os.Handler;
import android.os.Message;

import com.example.dadao.dadaorobot.entity.RobotStatusEntity;
import com.example.dadao.dadaorobot.activity.MainActivity;
import com.example.dadao.dadaorobot.util.Logger;
import com.google.protobuf.GeneratedMessageLite;
import DDRCommProto.BaseCmd;

public class NotifyBaseStatusProcessor extends BaseProcessor {

    @Override
    public void process(Context context, BaseCmd.CommonHeader commonHeader, GeneratedMessageLite msg) {
        super.process(context, commonHeader,msg);
        BaseCmd.notifyBaseStatus notifyBaseStatus=(BaseCmd.notifyBaseStatus)msg;
        RobotStatusEntity robotStatusEntity=RobotStatusEntity.getInstance();
        robotStatusEntity.setModel(notifyBaseStatus.getMode());
        robotStatusEntity.setSonmodel(notifyBaseStatus.getSonmode());
        robotStatusEntity.setBatt(notifyBaseStatus.getBatt());
        robotStatusEntity.setTemp(notifyBaseStatus.getTemp());
        robotStatusEntity.setHum(notifyBaseStatus.getHum());
        robotStatusEntity.setPm25(notifyBaseStatus.getPm25());
        robotStatusEntity.setStopstat(notifyBaseStatus.getStopstat());
        robotStatusEntity.setFuncstat(notifyBaseStatus.getFuncstat());
        robotStatusEntity.setVoiceloop(notifyBaseStatus.getVoiceloop());
        robotStatusEntity.setCurroute(notifyBaseStatus.getCurrroute().toStringUtf8());
        robotStatusEntity.setCurrpath(notifyBaseStatus.getCurrpath());
        robotStatusEntity.setCurrspeed(notifyBaseStatus.getCurrspeed());
        robotStatusEntity.setCurrwalkstyle(notifyBaseStatus.getCurrwalkstyle());
        robotStatusEntity.setPosx(notifyBaseStatus.getPosx());
        robotStatusEntity.setPosy(notifyBaseStatus.getPosy());
        robotStatusEntity.setPoslinespeed(notifyBaseStatus.getPoslinespeed());
        robotStatusEntity.setPosangulauspeed(notifyBaseStatus.getPosangulauspeed());
        robotStatusEntity.setLongitude(notifyBaseStatus.getLongitude());
        robotStatusEntity.setLatitude(notifyBaseStatus.getLatitude());
        robotStatusEntity.setTimestamp(notifyBaseStatus.getTimestamp());
        robotStatusEntity.setThermalmaxX(notifyBaseStatus.getThermalmaxx());
        robotStatusEntity.setThermalmaxY(notifyBaseStatus.getThermalmaxy());
        robotStatusEntity.setThermalmaxvalue(notifyBaseStatus.getThermalmaxvalue());
        //if (robotStatusEntity.getModel()!=0)
        Logger.w("机器人状态信息:"+robotStatusEntity.getCurroute());
        Handler mHandler=MainActivity.getHandler();
        if (mHandler!=null){
            Message message=new Message();
            message.arg1=2;
            mHandler.sendMessage(message);
           // Logger.e("-----发送机器人信息");
        }

    }
}
