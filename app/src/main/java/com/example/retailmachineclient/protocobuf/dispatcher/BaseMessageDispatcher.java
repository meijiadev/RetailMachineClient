package com.example.retailmachineclient.protocobuf.dispatcher;

import android.content.Context;

import com.example.retailmachineclient.protocobuf.GuIdInfo;
import com.example.retailmachineclient.protocobuf.processor.BaseProcessor;
import com.google.protobuf.GeneratedMessageLite;

import java.util.HashMap;
import java.util.Map;

import DDRCommProto.BaseCmd;


/**
 * 事件分发基类
 */
public class BaseMessageDispatcher {
    protected Map<String, BaseProcessor> m_ProcessorMap=new HashMap<>();

    public void dispatcher(Context context, BaseCmd.CommonHeader commonHeader, String typeName, GeneratedMessageLite msg){
        if (m_ProcessorMap.containsKey(typeName)){
            m_ProcessorMap.get(typeName).process(context,commonHeader,msg);
            GuIdInfo.getInstance().setGuId(commonHeader.getGuid());
            GuIdInfo.getInstance().setMessageLite(msg);
        }
    }




}
