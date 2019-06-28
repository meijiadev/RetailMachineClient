package ym.example.com.ymandroidclient.protobuf.processor;

import android.content.Context;

import com.google.protobuf.GeneratedMessageLite;

import DDRCommProto.BaseCmd;
import ym.example.com.ymandroidclient.utility.Logger;

public class RspCmdMoveProcessor extends BaseProcessor {
    @Override
    public void process(Context context, BaseCmd.CommonHeader commonHeader, GeneratedMessageLite msg) {
        super.process(context,commonHeader, msg);
        Logger.e("接受移动返回数据");
    }
}
