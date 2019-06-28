package ym.example.com.ymandroidclient.protobuf.processor;

import android.content.Context;
import android.os.Handler;
import android.os.Message;

import com.google.protobuf.GeneratedMessageLite;

import DDRCommProto.BaseCmd;
import ym.example.com.ymandroidclient.MainActivity;
import ym.example.com.ymandroidclient.entity.NotifyBaseStatusEx;

public class NotifyBaseStatusExProcessor extends BaseProcessor {
    @Override
    public void process(Context context, BaseCmd.CommonHeader commonHeader, GeneratedMessageLite msg) {
        super.process(context, commonHeader, msg);
        BaseCmd.notifyBaseStatusEx notifyBaseStatusEx= (BaseCmd.notifyBaseStatusEx) msg;
        NotifyBaseStatusEx notifyBaseStatusEx1=NotifyBaseStatusEx.getInstance();
        notifyBaseStatusEx1.setCurroute(notifyBaseStatusEx.getCurrroute().toStringUtf8());
        notifyBaseStatusEx1.setCurrpath(notifyBaseStatusEx.getCurrpath());
        notifyBaseStatusEx1.setMode(notifyBaseStatusEx.getModeValue());
        notifyBaseStatusEx1.setSonMode(notifyBaseStatusEx.getSonmodeValue());
        notifyBaseStatusEx1.setStopStat(notifyBaseStatusEx.getStopstat());
        notifyBaseStatusEx1.setPosAngulauspeed(notifyBaseStatusEx.getPosangulauspeed());
        notifyBaseStatusEx1.setPosDirection(notifyBaseStatusEx.getPosdirection());
        notifyBaseStatusEx1.setPosLinespeed(notifyBaseStatusEx.getPoslinespeed());
        notifyBaseStatusEx1.setPosX(notifyBaseStatusEx.getPosx());
        notifyBaseStatusEx1.setPosY(notifyBaseStatusEx.getPosy());

        Handler mHandler=MainActivity.getHandler();
        if (mHandler!=null){
            Message message=new Message();
            message.arg1=2;
            mHandler.sendMessage(message);
        }
    }
}
