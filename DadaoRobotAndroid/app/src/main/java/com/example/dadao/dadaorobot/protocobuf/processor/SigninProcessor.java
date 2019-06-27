package com.example.dadao.dadaorobot.protocobuf.processor;

import android.content.Context;

import com.example.dadao.dadaorobot.Login.presenter.LoginPresenter;
import com.example.dadao.dadaorobot.util.Logger;
import com.google.protobuf.GeneratedMessageLite;
import DDRCommProto.BaseCmd;
/**
 * 处理登录模块返回的数据
 */
public class SigninProcessor extends BaseProcessor {

    @Override
    public void process(Context context, BaseCmd.CommonHeader commonHeader,GeneratedMessageLite msg) {
        BaseCmd.rspLogin rspLogin=(BaseCmd.rspLogin) msg;
        Logger.e("返回消息："+rspLogin.getRetcode());
        LoginPresenter.toLogined(true);

    }


}
