package com.example.retailmachineclient.protocobuf;

import android.annotation.SuppressLint;
import android.content.Context;
import android.os.Build;

import com.example.retailmachineclient.protocobuf.dispatcher.BaseMessageDispatcher;
import com.example.retailmachineclient.socket.BaseSocketConnection;
import com.example.retailmachineclient.socket.StreamBuffer;
import com.example.retailmachineclient.util.Logger;
import com.google.protobuf.GeneratedMessageLite;
import com.google.protobuf.InvalidProtocolBufferException;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

import DDRCommProto.BaseCmd;
import androidx.annotation.RequiresApi;


/**
 * 负责解析和包装数据（序列化）
 */
public class MessageRoute {
    private Context context;
    public String headString="pbh\0";
    private BaseSocketConnection m_BaseSocketConnection;
    private BaseMessageDispatcher m_MessageDispatcher=null;
    public  static StreamBuffer streamBuffer;


    @SuppressLint("NewApi")
    public MessageRoute(Context context, BaseSocketConnection bc, BaseMessageDispatcher bmd){
        this.context=context;
        m_BaseSocketConnection=bc;
        m_MessageDispatcher=bmd;
        streamBuffer=StreamBuffer.getInstance();
    }

    public static String javaClass2ProtoTypeName(String className){
        if (className.contains("BaseCmd")){
            String sType=className.replaceAll("class DDRCommProto.BaseCmd\\$","DDRCommProto.");
            return sType;
        }else if (className.contains("RemoteCmd")){
            String sType=className.replaceAll("class DDRCommProto.RemoteCmd\\$","DDRCommProto.");
            return sType;
        }else if (className.contains("DDRVLNMap")){
            String sType=className.replaceAll("class DDRVLNMapProto.DDRVLNMap\\$","DDRVLNMapProto.");
            return sType;
        }else if (className.contains("DDRModuleCmd")){
            String sType=className.replaceAll("class DDRModuleProto.DDRModuleCmd\\$","DDRModuleProto.");
            return sType;
        } else if (className.contains("DDRAIServiceCmd")){
            String sType=className.replaceAll("class DDRAIServiceProto.DDRAIServiceCmd\\$","DDRAIServiceProto.");
            return sType;
        }
        return null;
    }

    public static String protoTypeName2JavaClassName(String typeName){
        if (typeName.contains("DDRCommProto")){
            if (typeName.contains("Remote")|typeName.contains("rspSelectLS")){
                String className=typeName.replaceAll("DDRCommProto\\.","class DDRCommProto.RemoteCmd\\$");
                return className;
            }else {
                String className=typeName.replaceAll("DDRCommProto\\.","class DDRCommProto.BaseCmd\\$");
                return className;
            }
        }else if (typeName.contains("DDRVLNMapProto")){
            String className=typeName.replaceAll("DDRVLNMapProto\\.","class DDRVLNMapProto.DDRVLNMap\\$");
            return className;
        }else if (typeName.contains("DDRModuleProto")){
            String className=typeName.replaceAll("DDRModuleProto\\.","class DDRModuleProto.DDRModuleCmd\\$");
            return className;
        }else if (typeName.contains("DDRAIServiceProto")){
            String className=typeName.replaceAll("DDRAIServiceProto\\.","class DDRAIServiceProto.DDRAIServiceCmd\\$");
            return className;
        }
        return null;
    }

    /**
     * 处理解析的数据
     * @param
     * @throws IOException
     */
    public void processReceive(BaseCmd.CommonHeader commonHeader, Object msg) {
        if (msg!=null){
            m_MessageDispatcher.dispatcher(context,commonHeader,msg.getClass().toString(), (GeneratedMessageLite) msg);
        }else {
            Logger.e("-----msg为空");
        }
    }



    public  byte []head=new byte[4];
    public  byte[] bTotalLen=new byte[4];
    public  byte[]bHeadLen=new byte[4];
    public Thread parseThread;
    public int where=0x00;   //标志位

    /**
     * 解析数据
     * @param  （用于解析广播）
     * @return
     * @throws IOException
     */
    public  void parse() {
        if (parseThread==null){
            parseThread=new Thread(new Runnable() {
                @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
                @Override
                public void run() {
                    while (true){
                        switch (where){
                            case 0x00:
                                try {
                                    parsePbhState();
                                } catch (UnsupportedEncodingException e) {
                                    e.printStackTrace();
                                } catch (InvalidProtocolBufferException e) {
                                    e.printStackTrace();
                                }
                                break;
                            case 0x01:
                                parsePbhState2();
                                break;
                            case 0x02:
                                try {
                                    parseLengthState();
                                } catch (InvalidProtocolBufferException e) {
                                    e.printStackTrace();
                                }
                                break;
                            case 0x03:
                                try {
                                    parseHeadState();
                                } catch (InvalidProtocolBufferException e) {
                                    e.printStackTrace();
                                }
                                break;
                            case 0x04:
                                try {
                                    parseBodyState();
                                } catch (InvalidProtocolBufferException e) {
                                    e.printStackTrace();
                                }
                                break;
                        }
                    }
                }
            });
            parseThread.start();
        }

    }

    /**
     * 解析包体
     */
    public void parseBody(byte[] bodys,int headLength) throws InvalidProtocolBufferException {
        byte[] bheadData=new byte[headLength];
        byte[] bbodyData=new byte[bodys.length-headLength];
        System.arraycopy(bodys,0,bheadData,0,headLength);
        System.arraycopy(bodys,headLength,bbodyData,0,bbodyData.length);
        BaseCmd.CommonHeader headData=null;
        Object bodyDataMsg=null;
        boolean needEncrypt=true;
        if (needEncrypt)
        {
            byte[]bHeadDataDE=new byte[bheadData.length-5];
            if (Encrypt.Txt_Decrypt(bheadData,bheadData.length,bHeadDataDE,bHeadDataDE.length)){
            }else {
                Logger.e("Txt_Decrypt Error ");
                processReceive(null,null);
            }
            headData=BaseCmd.CommonHeader.parseFrom(bHeadDataDE);
            if (bbodyData.length>5){
                byte[] bbodyDataDE=new byte[bbodyData.length-5];
                if (Encrypt.Txt_Decrypt(bbodyData,bbodyData.length,bbodyDataDE,bbodyDataDE.length)){
                    bodyDataMsg=parseDynamic(headData.getBodyType(),bbodyDataDE);
                    processReceive(headData,bodyDataMsg);
                    // Logger.e("------");
                }else {
                    Logger.e("Txt_Decrypt Error");
                    processReceive(null,null);
                }
            }else {
                //Logger.e("bodyDataMsg:"+headData.getBodyType());
                bodyDataMsg=parseDynamic(headData.getBodyType(),null);
                processReceive(headData,bodyDataMsg);
            }
        }else {
            headData=BaseCmd.CommonHeader.parseFrom(bheadData);
            bodyDataMsg=parseDynamic(headData.getBodyType(),bbodyData);
            Logger.e("bodyDataMsg:"+headData.getBodyType());
            processReceive(headData,bodyDataMsg);
        }
    }


    /**
     * 验证加上的phb头部
     * @throws UnsupportedEncodingException
     * @throws InvalidProtocolBufferException
     */
    @SuppressLint("NewApi")
    private void parsePbhState() throws UnsupportedEncodingException, InvalidProtocolBufferException {
        if (streamBuffer.arrayDeque.size()>=4){
            head=streamBuffer.peekData(4);
            String shead=new String(head,"UTF-8");
            if (shead.equals(headString)){
                streamBuffer.pollData(4);
                parseLengthState();
            }else {
                where=0x01;
                Logger.e("验证失败");
            }
        }else {
            where=0x00;
        }
    }

    /**
     * 如果验证失败 则丢掉第一个字节 再取四个进行比较 直到遇到对的头部
     * @throws UnsupportedEncodingException
     * @throws InvalidProtocolBufferException
     */
    @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
    private void parsePbhState2(){
        streamBuffer.poll();
        where=0x00;
    }

    public int totalLen;

    /**
     * 验证信息长度
     * @throws InvalidProtocolBufferException
     */
    @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
    private void parseLengthState() throws InvalidProtocolBufferException {
        if (streamBuffer.arrayDeque.size()>=4){
            bTotalLen=streamBuffer.pollData(4);
            totalLen=bytesToIntLittle(bTotalLen,0);  //获取总长度信息
            if (totalLen>0){
                parseHeadState();
            }else {
                where=0x00;
            }
        }else {
            where=0x02;
        }
    }

    public int headLen;
    @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
    private void parseHeadState() throws InvalidProtocolBufferException {
        if (streamBuffer.arrayDeque.size()>=4){
            bHeadLen=streamBuffer.pollData(4);
            headLen=bytesToIntLittle(bHeadLen,0);     //获取头部长度信息
            if (headLen>0){
                parseBodyState();
            }else {
                where=0x00;
            }
        }else {
            where=0x03;
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
    private void parseBodyState() throws InvalidProtocolBufferException {
        if (streamBuffer.arrayDeque.size()>=totalLen-8){
            byte[] bHeadData=streamBuffer.pollData(headLen);
            byte[] bBodyData=streamBuffer.pollData(totalLen-headLen-8);
            BaseCmd.CommonHeader headData=null;
            Object bodyDataMsg=null;
            boolean needEncrypt=true;
            if (needEncrypt)
            {
                byte[]bHeadDataDE=new byte[bHeadData.length-5];
                if (Encrypt.Txt_Decrypt(bHeadData,bHeadData.length,bHeadDataDE,bHeadDataDE.length)){
                }else {
                    Logger.e("Txt_Decrypt Error ");
                    processReceive(null,null);
                    where=0x00;
                }
                headData=BaseCmd.CommonHeader.parseFrom(bHeadDataDE);
                if (bBodyData.length>5){
                    byte[] bbodyDataDE=new byte[bBodyData.length-5];
                    if (Encrypt.Txt_Decrypt(bBodyData,bBodyData.length,bbodyDataDE,bbodyDataDE.length)){
                        bodyDataMsg=parseDynamic(headData.getBodyType(),bbodyDataDE);
                        processReceive(headData,bodyDataMsg);
                        where=0x00;
                    }else {
                        Logger.e("Txt_Decrypt Error");
                        processReceive(null,null);
                        where=0x00;
                    }
                }else {
                    //Logger.e("bodyDataMsg:"+headData.getBodyType());
                    bodyDataMsg=parseDynamic(headData.getBodyType(),null);
                    processReceive(headData,bodyDataMsg);
                    where=0x00;

                }
            }else {
                headData=BaseCmd.CommonHeader.parseFrom(bHeadData);
                bodyDataMsg=parseDynamic(headData.getBodyType(),bBodyData);
                Logger.e("bodyDataMsg:"+headData.getBodyType());
                processReceive(headData,bodyDataMsg);
                where=0x00;
            }
        }else {
            where=0x04;
        }
    }

    /**
     * 序列化要发送的信息
     * @param msg
     * @return
     */
    public  byte[] serialize(BaseCmd.CommonHeader commonHeader, GeneratedMessageLite msg){
        byte[]bBody=msg.toByteArray();
        String sType=msg.getClass().toString();
//         Logger.e("未转换的stype:"+sType);
        sType=javaClass2ProtoTypeName(sType);
//         Logger.e("转换后的stype:"+sType);
        int bBodyLength=bBody.length;
        BaseCmd.CommonHeader headData;
        if (commonHeader!=null){
            headData=BaseCmd.CommonHeader.newBuilder().setBodyType(sType)
                    .setFromCltType(commonHeader.getFromCltType())
                    .setToCltType(commonHeader.getToCltType())
                    .addFlowDirection(commonHeader.getFlowDirection(0))
                    .setGuid(commonHeader.getGuid())
                    .build();      //设置头部类型
        }else{
            headData= BaseCmd.CommonHeader.newBuilder().setBodyType(sType). build();      //设置头部类型
        }
        byte[]bsHead=headString.getBytes();  //头部标识
        byte[] bHead=headData.toByteArray();//头部信息
        int bHeadLength=bHead.length;
        int totalLen=8+bHeadLength+bBodyLength;
        byte[]bytes=new byte[totalLen+4+10];    //要发送出去的数组总信息
        System.arraycopy(bsHead,0,bytes,0,4);
        System.arraycopy(intToBytesLittle(totalLen+10),0,bytes,4,4);
        System.arraycopy(intToBytesLittle(bHeadLength+5),0,bytes,8,4);
        byte[]bHeadE=new byte[bHeadLength+5];
        if (Encrypt.Txt_Encrypt(bHead,bHeadLength,bHeadE,bHeadE.length)){
            bHeadE=Encrypt.getTxt_Encrypt();
            try {
                System.arraycopy(bHeadE,0,bytes,12,bHeadE.length);
            }catch (ArrayIndexOutOfBoundsException e){
                e.printStackTrace();
                return null;
            }
        }else {
            Logger.e("Txt_Encrypt Error");
            return null;
        }
        if (bBodyLength>0)
        {
            byte[]bBodyE=new byte[bBodyLength+5];
            if (Encrypt.Txt_Encrypt(bBody,bBodyLength,bBodyE,bBodyE.length)){
                try {
                    System.arraycopy(bBodyE,0,bytes,12+bHeadE.length,bBodyE.length);
                }catch (ArrayIndexOutOfBoundsException a){
                    a.printStackTrace();
                    Logger.e("----------数组越界");
                    return null;
                }
            }else {
                Logger.e("Txt_Encrypt Error");
                return null;
            }
        }
        return bytes;


    }

    /**
     * 通过反射获取到返回的相应的类对象
     * @param type
     * @param bytes
     * @return
     */
    public static Object parseDynamic(String type, byte[]bytes){
        //Logger.e("------"+type);
        type = protoTypeName2JavaClassName(type);
        type = type.replace("class ", "");
        //Logger.e("------"+type);
        try {
            Class<?> clazz= Class.forName(type);
            if (bytes!=null){
                Method method=clazz.getDeclaredMethod("parseFrom",byte[].class);
                return method.invoke(null,bytes);
            }else {
                Method method=clazz.getMethod("getDefaultInstance",
                         null);
                return method.invoke(null,null);
            }
        } catch (ClassNotFoundException | NoSuchMethodException | IllegalAccessException e) {
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
            Logger.e("bytes为空！");
        }
        return null;
    }




    /**
     * 以大端模式将int转成byte[]
     *
     */
    public static byte[] intToBytesBig(int value) {
        byte[] src = new byte[4];
        src[0] = (byte) ((value >> 24) & 0xFF);
        src[1] = (byte) ((value >> 16) & 0xFF);
        src[2] = (byte) ((value >> 8) & 0xFF);
        src[3] = (byte) (value & 0xFF);
        return src;
    }

    /**
     * 以小端模式将int转成byte[]
     * @param value
     * @return
     * '
     */
    private static byte[] intToBytesLittle(int value) {
        byte[] src = new byte[4];
        src[3] = (byte) ((value >> 24) & 0xFF);
        src[2] = (byte) ((value >> 16) & 0xFF);
        src[1] = (byte) ((value >> 8) & 0xFF);
        src[0] = (byte) (value & 0xFF);
        return src;
    }

    /**
     * 以大端模式将byte[]转成int
     */
    public static int bytesToIntBig(byte[] src, int offset) {
        int value;
        value = (int) (((src[offset] & 0xFF) << 24)
                | ((src[offset + 1] & 0xFF) << 16)
                | ((src[offset + 2] & 0xFF) << 8)
                | (src[offset + 3] & 0xFF));
        return value;
    }

    /**
     * 以小端模式将byte[]转成int
     */
    private static int bytesToIntLittle(byte[] src, int offset) {
        int value;
        value = (int) ((src[offset] & 0xFF)
                | ((src[offset + 1] & 0xFF) << 8)
                | ((src[offset + 2] & 0xFF) << 16)
                | ((src[offset + 3] & 0xFF) << 24));
        return value;
    }
}
