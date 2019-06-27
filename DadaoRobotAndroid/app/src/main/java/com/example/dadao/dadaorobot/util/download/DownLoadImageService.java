package com.example.dadao.dadaorobot.util.download;

import android.content.Context;

import com.bumptech.glide.Glide;

import java.io.File;

/**
 * Glide图片下载
 */
public class DownLoadImageService implements Runnable {
    private String url;
    private File target;
    private Context context;
    private DownLoadCallBack downLoadCallBack;
    public DownLoadImageService(Context context,String url,DownLoadCallBack callBack,File target){
        this.context=context;
        this.downLoadCallBack=callBack;
        this.url=url;
        this.target=target;
    }

    @Override
    public void run() {
        File file=null;
        try {
            file = Glide.with(context)
                    .load(url)
                    .downloadOnly(0,0)
                    .get();
        }catch (Exception e){
            e.printStackTrace();
        }finally {
            if (file!=null){
                downLoadCallBack.onDownLoadSuccess(file,target);
            }else {
                downLoadCallBack.onDownLoadFailed();
            }
        }
    }
}
