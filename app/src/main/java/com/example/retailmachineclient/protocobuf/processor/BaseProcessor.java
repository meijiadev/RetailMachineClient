package com.example.retailmachineclient.protocobuf.processor;

import android.content.Context;

import com.google.protobuf.GeneratedMessageLite;

import DDRCommProto.BaseCmd;

/**
 * 作用：事件处理基类
 */
public class BaseProcessor {
    public static String MessageFilter="BaseProcessorFilter";

    public void process(Context context, BaseCmd.CommonHeader commonHeader, GeneratedMessageLite msg){

    }

}
