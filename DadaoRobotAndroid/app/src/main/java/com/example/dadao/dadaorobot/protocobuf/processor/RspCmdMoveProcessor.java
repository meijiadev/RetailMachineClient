package com.example.dadao.dadaorobot.protocobuf.processor;

import android.content.Context;

import com.example.dadao.dadaorobot.util.Logger;
import com.google.protobuf.GeneratedMessageLite;

import DDRCommProto.BaseCmd;

public class RspCmdMoveProcessor extends BaseProcessor {
    @Override
    public void process(Context context, BaseCmd.CommonHeader commonHeader, GeneratedMessageLite msg) {
        super.process(context,commonHeader, msg);
        Logger.e("接受移动返回数据");
    }
}
