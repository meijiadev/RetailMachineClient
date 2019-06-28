package ym.example.com.ymandroidclient.protobuf.processor;

import android.content.Context;
import android.os.Handler;
import android.os.Message;

import com.google.protobuf.GeneratedMessageLite;

import DDRCommProto.BaseCmd;
import ym.example.com.ymandroidclient.LoginActivity;


public class RspLoginProcessor extends BaseProcessor {
    @Override
    public void process(Context context, BaseCmd.CommonHeader commonHeader, GeneratedMessageLite msg) {
        super.process(context, commonHeader, msg);
        BaseCmd.rspLogin rspLogin= (BaseCmd.rspLogin) msg;
        Handler handler=LoginActivity.getInstance();
        Message message=new Message();
        message.arg1=1;
        handler.sendMessage(message);

    }
}
