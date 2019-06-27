package com.example.dadao.dadaorobot.protocobuf.processor;

import android.content.Context;
import android.os.Bundle;
import android.os.Message;

import com.example.dadao.dadaorobot.activity.NavigationActivity;
import com.example.dadao.dadaorobot.util.Logger;
import com.google.protobuf.GeneratedMessageLite;

import java.util.ArrayList;
import DDRCommProto.BaseCmd;
public class StreamAddrProcessor extends BaseProcessor {
    ArrayList<String> listAddress=new ArrayList<>();
    ArrayList<String>listAddressName=new ArrayList<>();
    @Override
    public void process(Context context, BaseCmd.CommonHeader commonHeader,GeneratedMessageLite msg) {
        BaseCmd.rspStreamAddr rspStreamAddr= (BaseCmd.rspStreamAddr) msg;
        Logger.e("语音对讲");
        String error=rspStreamAddr.getError();
        Logger.e("error:"+error);
        if (error.isEmpty()){
            listAddress.clear();
            listAddressName.clear();
            for (BaseCmd.AVStreamSrcChannel avStreamSrcChannel:rspStreamAddr.getChannelsList()){
                if (avStreamSrcChannel.getNetworkType()==BaseCmd.ChannelNetworkType.Local){
                    String address=avStreamSrcChannel.getSrcAddr();
                    String addressName=avStreamSrcChannel.getSrcname().toStringUtf8();
                    Logger.e("addressName:"+addressName);
                    if (address.contains("rtsp")){
                        listAddress.add(address);
                        listAddressName.add(addressName);
                        Logger.e("---------rtsp流视频-------"+address);
                    }
                    if (avStreamSrcChannel.getSrcPortList().size()>0){
                        String ip=avStreamSrcChannel.getSrcAddr();
                        int port=avStreamSrcChannel.getSrcPort(0);
                        Logger.e("语音对讲的IP和Port:"+ip+";"+port);
                        Message message=new Message();
                        message.arg1=1;
                        Bundle bundle=new Bundle();
                        bundle.putString("IP",ip);
                        bundle.putInt("Port",port);
                        message.setData(bundle);
                        NavigationActivity.getHandler().sendMessage(message);
                    }

                }
            }
            Message message=new Message();
            message.arg1=3;
            Bundle bundle=new Bundle();
            bundle.putStringArrayList("VideoAddress",listAddress);
            bundle.putStringArrayList("VideoAddressName",listAddressName);
            message.setData(bundle);
            NavigationActivity.getHandler().sendMessage(message);
      }else{
            Message message=new Message();
            message.arg1=3;
            Bundle bundle=new Bundle();
            bundle.putStringArrayList("VideoAddress",listAddress);
            bundle.putStringArrayList("VideoAddressName",listAddressName);
            message.setData(bundle);
            NavigationActivity.getHandler().sendMessage(message);
        }

    }
}
