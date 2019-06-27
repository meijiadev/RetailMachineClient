package com.example.dadao.dadaorobot.protocobuf.processor;

import android.content.Context;

import com.example.dadao.dadaorobot.util.Logger;
import com.google.protobuf.GeneratedMessageLite;
import DDRCommProto.BaseCmd;

public class CmdStartActionModel extends BaseProcessor {
    @Override
    public void process(Context context,BaseCmd.CommonHeader commonHeader, GeneratedMessageLite msg) {
        super.process(context, commonHeader,msg);
        BaseCmd.rspCmdStartActionMode rspCmdStartActionMode= (BaseCmd.rspCmdStartActionMode) msg;
        Logger.e("--------》》》"+rspCmdStartActionMode.getMode()+"----->>"+rspCmdStartActionMode.getType());
    }
}
