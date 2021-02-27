package com.example.retailmachineclient.protocobuf;

import com.google.protobuf.GeneratedMessageLite;

/**
 * 用来存放返回的GuId
 */
public class GuIdInfo {
    public static GuIdInfo guIdInfo;
    private String guId;
    private GeneratedMessageLite messageLite;

    public static GuIdInfo getInstance(){
        if (guIdInfo==null){
            synchronized (GuIdInfo.class){
                if (guIdInfo==null){
                    guIdInfo=new GuIdInfo();
                }
            }
        }
        return guIdInfo;
    }
    public String getGuId() {
        return guId;
    }

    public void setGuId(String guId) {
        this.guId = guId;
    }

    public GeneratedMessageLite getMessageLite() {
        return messageLite;
    }

    public void setMessageLite(GeneratedMessageLite messageLite) {
        this.messageLite = messageLite;
    }
}
