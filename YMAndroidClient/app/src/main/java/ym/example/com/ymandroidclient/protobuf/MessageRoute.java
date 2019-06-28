package ym.example.com.ymandroidclient.protobuf;

import android.annotation.SuppressLint;
import android.content.Context;

import com.google.protobuf.GeneratedMessageLite;
import com.google.protobuf.InvalidProtocolBufferException;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

import DDRCommProto.BaseCmd;
import ym.example.com.ymandroidclient.protobuf.dispatcher.BaseMessageDispatcher;
import ym.example.com.ymandroidclient.socket.BaseSocketConnection;
import ym.example.com.ymandroidclient.socket.StreamBuffer;
import ym.example.com.ymandroidclient.utility.Logger;

/**
 * 负责解析和包装数据（序列化）
 */
public class MessageRoute {
    private Context context;
    public  String headString="pbh\0";

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
        }else {
            String sType=className.replaceAll("class DDRCommProto.RemoteCmd\\$","DDRCommProto.");
            return sType;
        }
    }

    public static String protoTypeName2JavaClassName(String typeName){
        String className=typeName.replaceAll("DDRCommProto\\.","class DDRCommProto.BaseCmd\\$");
        return className;
    }

    /**
     * 处理解析的数据
     * @param
     * @throws IOException
     */
    public void processReceive(BaseCmd.CommonHeader commonHeader, Object msg) {
        if (msg!=null){
            m_MessageDispatcher.dispatcher(context,commonHeader,msg.getClass().toString(), (GeneratedMessageLite) msg);
        }

    }


    public static byte []head=new byte[4];
    public static byte[] btotallen=new byte[4];
    public static byte[]bheadlen=new byte[4];
    public Thread parseThread;
    public int where=0x00;  //

    /**
     * 解析数据
     * @param
     * @return
     * @throws IOException
     */
     public  void parse() {
         if (parseThread==null){
             parseThread=new Thread(new Runnable() {
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
     * 验证加上的phb头部
     * @throws UnsupportedEncodingException
     * @throws InvalidProtocolBufferException
     */
   @SuppressLint("NewApi")
   private void parsePbhState() throws UnsupportedEncodingException, InvalidProtocolBufferException {
         if (streamBuffer.arrayDeque.size()>=4){
             head=streamBuffer.peekData(4);
             String shead=new String(head,"UTF-8");
             //  Logger.e(shead);
             if (shead.equals(headString)){
                 //Logger.e("------验证成功");
                 streamBuffer.pollData(4);
                 parseLengthState();
             }else {
                 where=0x01;
                 Logger.e("验证失败");
             }
         }
   }

    /**
     * 如果验证失败 则丢掉第一个字节 再取四个进行比较 直到遇到对的头部
     *
     * @throws UnsupportedEncodingException
     * @throws InvalidProtocolBufferException
     */
   @SuppressLint("NewApi")
   private void parsePbhState2(){
        streamBuffer.poll();
        where=0x00;
   }

   public int totallen;

    /**
     * 验证信息长度
     * @throws InvalidProtocolBufferException
     */
   @SuppressLint("NewApi")
   private void parseLengthState() throws InvalidProtocolBufferException {
         //Logger.e("-------可读长度:"+streamBuffer.arrayDeque.size());
         if (streamBuffer.arrayDeque.size()>=4){
             btotallen=streamBuffer.pollData(4);
             totallen=bytesToIntLittle(btotallen,0);  //获取总长度信息
             if (totallen>0){
                 parseHeadState();
             }else {
                 where=0x00;
             }
             //Logger.e("---可读长度，信息长度"+streamBuffer.arrayDeque.size()+";"+totallen);
         }else {
             where=0x02;
         }
   }

    public int headlen;
   @SuppressLint("NewApi")
   private void parseHeadState() throws InvalidProtocolBufferException {
       //Logger.e("-------av:"+streamBuffer.arrayDeque.size());
         if (streamBuffer.arrayDeque.size()>=4){
            // Logger.e("验证头部信息");
             bheadlen=streamBuffer.pollData(4);
             headlen=bytesToIntLittle(bheadlen,0);     //获取头部长度信息
             if (headlen>0){
                 parseBodyState();
             }else {
                 where=0x00;
             }
         }else {
             where=0x03;
         }
   }

   @SuppressLint("NewApi")
   private void parseBodyState() throws InvalidProtocolBufferException {
        if (streamBuffer.arrayDeque.size()>=totallen-8){
            byte[] bheadData=streamBuffer.pollData(headlen);
            byte[] bbodyData=streamBuffer.pollData(totallen-headlen-8);
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
                    where=0x00;
                }
                headData=BaseCmd.CommonHeader.parseFrom(bHeadDataDE);
                if (bbodyData.length>5){
                    byte[] bbodyDataDE=new byte[bbodyData.length-5];
                    if (Encrypt.Txt_Decrypt(bbodyData,bbodyData.length,bbodyDataDE,bbodyDataDE.length)){
                        bodyDataMsg=parseDynamic(headData.getBodyType(),bbodyDataDE);
                        processReceive(headData,bodyDataMsg);
                        where=0x00;
                    }else {
                        Logger.e("Txt_Decrypt Error");
                        processReceive(null,null);
                        where=0x00;
                    }
                }else {
                    Logger.e("bodyDataMsg:"+headData.getBodyType());
                    bodyDataMsg=parseDynamic(headData.getBodyType(),null);
                    processReceive(headData,bodyDataMsg);
                    where=0x00;

                }
            }else {
                headData=BaseCmd.CommonHeader.parseFrom(bheadData);
                bodyDataMsg=parseDynamic(headData.getBodyType(),bbodyData);
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
        byte[]bbody=msg.toByteArray();
        String stype=msg.getClass().toString();
        //Logger.e("未转换的stype:"+stype);
        stype=javaClass2ProtoTypeName(stype);
        //Logger.e("转换后的stype:"+stype);
        int bbodylen=bbody.length;
        BaseCmd.CommonHeader headData;
        if (commonHeader!=null){
             headData=BaseCmd.CommonHeader.newBuilder().setBodyType(stype)
                     .setFromCltType(commonHeader.getFromCltType())
                     .setToCltType(commonHeader.getToCltType())
                     .addFlowDirection(commonHeader.getFlowDirection(0))
                     .setGuid(commonHeader.getGuid())
                     .build();      //设置头部类型
        }else{
             headData=BaseCmd.CommonHeader.newBuilder().setBodyType(stype). build();      //设置头部类型
        }
        byte[]bshead=headString.getBytes();  //头部标识
        byte[] bhead=headData.toByteArray();//头部信息
        int bheadlen=bhead.length;
        int totalLen=8+bheadlen+bbodylen;
        byte[]bytes=new byte[totalLen+4+10];    //要发送出去的数组总信息
        System.arraycopy(bshead,0,bytes,0,4);
        boolean needEncrypt=true;
        if (needEncrypt)
        {
            System.arraycopy(intToBytesLittle(totalLen+10),0,bytes,4,4);
            System.arraycopy(intToBytesLittle(bheadlen+5),0,bytes,8,4);
            byte[]bheadE=new byte[bheadlen+5];
            if (Encrypt.Txt_Encrypt(bhead,bheadlen,bheadE,bheadE.length)){
                bheadE=Encrypt.getTxt_Encrypt();
                System.arraycopy(bheadE,0,bytes,12,bheadE.length);
            }else {
                Logger.e("Txt_Encrypt Error");
                return null;
            }
            if (bbodylen>0)
            {
                byte[]bbodyE=new byte[bbodylen+5];
                if (Encrypt.Txt_Encrypt(bbody,bbodylen,bbodyE,bbodyE.length)){
                    System.arraycopy(bbodyE,0,bytes,12+bheadE.length,bbodyE.length);
                }else {
                    Logger.e("Txt_Encrypt Error");
                    return null;
                }
            }
        }else {
            System.arraycopy(intToBytesLittle(totalLen+10),0,bytes,4,4);
            System.arraycopy(intToBytesLittle(bheadlen+5),0,bytes,8,4);
            System.arraycopy(bhead,0,bytes,12,bheadlen);
            System.arraycopy(bbody,0,bytes,12+bheadlen,bbody.length);
            return bytes;
        }
        return bytes;

    }

    /**
     * 通过反射获取到返回的相应的类对象
     * @param type
     * @param bytes
     * @return
     */
    public static Object parseDynamic(String type,byte[]bytes){
        type = protoTypeName2JavaClassName(type);
        type = type.replace("class ", "");
        //Logger.e("------"+type);
        try {
            Class<?> clazz=Class.forName(type);
            if (bytes!=null){
                Method method=clazz.getDeclaredMethod("parseFrom",byte[].class);
                return method.invoke(null,bytes);
            }else {
                Method method=clazz.getMethod("getDefaultInstance",null);
                return method.invoke(null,null);
            }
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
            Logger.e("bytes为空！");
        }
        return null;
    }






    /**
     * 以大端模式将int转成byte[]
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
     *
     * @param value
     * @return
     */
    public static byte[] intToBytesLittle(int value) {
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
    public static int bytesToIntLittle(byte[] src, int offset) {
        int value;
        value = (int) ((src[offset] & 0xFF)
                | ((src[offset + 1] & 0xFF) << 8)
                | ((src[offset + 2] & 0xFF) << 16)
                | ((src[offset + 3] & 0xFF) << 24));
        return value;
    }







}
