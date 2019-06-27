package com.example.dadao.dadaorobot.protocobuf.processor;

import android.content.Context;

import com.example.dadao.dadaorobot.Login.presenter.LoginPresenter;
import com.example.dadao.dadaorobot.util.Logger;
import com.google.protobuf.GeneratedMessageLite;

import DDRCommProto.BaseCmd;
import DDRCommProto.RemoteCmd;

public class RspSelectLSProcessor extends BaseProcessor {
    @Override
    public void process(Context context, BaseCmd.CommonHeader commonHeader, GeneratedMessageLite msg) {
        super.process(context,commonHeader, msg);
        RemoteCmd.rspSelectLS rspSelectLS= (RemoteCmd.rspSelectLS) msg;
        Logger.e(rspSelectLS.getError());
        LoginPresenter.toLogined(true);
    }
}
