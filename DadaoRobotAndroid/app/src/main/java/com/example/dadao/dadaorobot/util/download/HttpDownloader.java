package com.example.dadao.dadaorobot.util.download;

import com.example.dadao.dadaorobot.util.Logger;

import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;

/**
 * create by Ezreal Mei 2018/12/27
 * http下载工具类
 */
public class HttpDownloader {
    public static URL url=null;
    public static int MaxLength=1024;
    public static byte[] downloadBytes=new byte[MaxLength];
    public static byte[] getbytesFromUrl(String urlStr) throws IOException {
        url=new URL(urlStr);      //新建一个URL对象
        HttpURLConnection httpURLConnection= (HttpURLConnection) url.openConnection();  //打开一个HttpURLConnection 的连接
        httpURLConnection.setUseCaches(true);
        InputStream inputStream=httpURLConnection.getInputStream();
        int len=httpURLConnection.getContentLength();
        Logger.e("文件长度："+len);
        if (len>MaxLength){
            int count=len/MaxLength;        //取商
            int remainder=len%MaxLength;    //取余数
            for (int i=0;i<count;i++){
                }
        }
        inputStream.read(downloadBytes);
        return downloadBytes;
    }

}
