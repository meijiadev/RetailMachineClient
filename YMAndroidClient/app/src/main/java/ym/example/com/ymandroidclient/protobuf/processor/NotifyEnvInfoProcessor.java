package ym.example.com.ymandroidclient.protobuf.processor;

import android.content.Context;

import com.google.protobuf.GeneratedMessageLite;

import DDRCommProto.BaseCmd;
import ym.example.com.ymandroidclient.entity.NotifyEnvInfo;

public class NotifyEnvInfoProcessor extends BaseProcessor {
    @Override
    public void process(Context context, BaseCmd.CommonHeader commonHeader, GeneratedMessageLite msg) {
        super.process(context, commonHeader, msg);
        BaseCmd.notifyEnvInfo notifyEnvInfo= (BaseCmd.notifyEnvInfo) msg;
        NotifyEnvInfo notifyEnvInfo1=NotifyEnvInfo.getInstance();
        notifyEnvInfo1.setBatt(notifyEnvInfo.getBatt());
        notifyEnvInfo1.setTemp(notifyEnvInfo.getTemp());
        notifyEnvInfo1.setCh2o(notifyEnvInfo.getCh2O());
        notifyEnvInfo1.setCo2(notifyEnvInfo.getCo2());
        notifyEnvInfo1.setHum(notifyEnvInfo.getHum());
        notifyEnvInfo1.setPm25(notifyEnvInfo.getPm25());
    }
}
