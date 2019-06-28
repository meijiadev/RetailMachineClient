package ym.example.com.ymandroidclient.socket;

import android.os.Build;
import android.support.annotation.RequiresApi;

import java.util.concurrent.ConcurrentLinkedDeque;

import ym.example.com.ymandroidclient.utility.Logger;


/**
 * 双端队列
 * create by 2019/3/22
 */
@RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
public class StreamBuffer {
      public ConcurrentLinkedDeque<Byte> arrayDeque=new ConcurrentLinkedDeque<>();
      static StreamBuffer streamBuffer;

      /**
     *双重否定单例模式,保证线程安全
     * @return
     */
    public static StreamBuffer getInstance(){
          if (streamBuffer==null){
              synchronized (StreamBuffer.class){
                  if (streamBuffer==null){
                      streamBuffer=new StreamBuffer();
                  }
              }
          }

          return streamBuffer;
    }

    public StreamBuffer() {

    }

    /**
     * 接收数据放到该队列当中
     * @param buf
     * @param len
     */
    public void onDataReceived(byte[]buf,int len){
          for (int i=0;i<len;i++){
              arrayDeque.add(buf[i]);
          }
    }

    /**
     * 取出数据并删除队列中的元素
     */
    public synchronized byte[] pollData(int len)throws NullPointerException{
        byte [] data=new byte[len];
            for (int i=0;i<len;i++){
                try {
                    data[i]=arrayDeque.poll().byteValue();
                }catch (Exception e){
                    e.printStackTrace();
                }
            }
          return data;
      }

    /**
     * 取出数据但是不删除
     * @param
     * @return
     */
    public  byte[] peekData(int len){
        byte [] data=new byte[len];
        if (arrayDeque!=null){
            for (int i=0;i<len;i++){
                if (arrayDeque.peek()!=null){
                    try {
                        data[i]=arrayDeque.poll().byteValue();
                    }catch (NullPointerException e){
                        e.printStackTrace();
                    }
                }else {
                    Logger.w("数据为null");
                    data[i]='p';
                }
            }
            for (int i=0;i<len;i++){
                arrayDeque.addFirst(data[len-i-1]);

            }
        }
        return data;
    }

    /**
     *取出头部数据并删除
     * @return
     */
    public synchronized byte poll()throws NullPointerException{
        byte a=0;
        try {
            a=(arrayDeque.poll().byteValue());
        }catch (NullPointerException e){
            e.printStackTrace();
        }
        return a;
    }

    /**
     * 取出头部数据但不删除
     */
    public byte peek()throws NullPointerException{
        byte a=0;
        try {
            a=arrayDeque.peek().byteValue();
        }catch (NullPointerException e){
            e.printStackTrace();
        }
        return a;
    }

    /**
     * 清空链表
     */
    public void clearArray(){
        arrayDeque.clear();
    }

}
