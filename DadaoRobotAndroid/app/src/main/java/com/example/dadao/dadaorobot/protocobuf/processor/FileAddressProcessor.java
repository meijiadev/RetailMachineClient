package com.example.dadao.dadaorobot.protocobuf.processor;

import android.content.Context;
import android.os.Environment;
import android.os.Message;
import android.util.Log;

import com.example.dadao.dadaorobot.activity.MainActivity;
import com.example.dadao.dadaorobot.entity.RobotStatusEntity;
import com.example.dadao.dadaorobot.util.download.DownLoadCallBack;
import com.example.dadao.dadaorobot.util.download.DownLoadImageService;
import com.example.dadao.dadaorobot.util.download.FileUtil;
import com.example.dadao.dadaorobot.util.Logger;
import com.example.dadao.dadaorobot.util.widget.CustomImageView;
import com.google.protobuf.ByteString;
import com.google.protobuf.GeneratedMessageLite;

import java.io.File;
import java.io.IOException;
import java.util.List;
import DDRCommProto.BaseCmd;
/**
 * 处理下载
 */
public class FileAddressProcessor extends BaseProcessor implements DownLoadCallBack {
    private Context context;
    FileUtil fileUtil;
    private boolean havePng;
    private int pngSize;
    private int n;
    @Override
    public void process(Context context, BaseCmd.CommonHeader commonHeader,GeneratedMessageLite msg) {
        this.context=context;
        havePng=false;
        Logger.e("接收到文件http地址");
        n=0;
        BaseCmd.rspFileAddress rspFileAddress= (BaseCmd.rspFileAddress) msg;
        String error=rspFileAddress.getError();
        Logger.e("error:"+error);
        List<ByteString> pathlist=rspFileAddress.getFileAddrListList();
        if (error.isEmpty()){
            Logger.e("----");
            if (pathlist.size()>0){
                Logger.e("http列表数量："+pathlist.size());
                for (int i=0;i<pathlist.size();i++){
                    ByteString bytes=pathlist.get(i);
                    String url=bytes.toStringUtf8();
                    //Logger.e("-----"+url);
                    String files=url.split("//")[1];
                    //Logger.e("----"+files);
                    String fileDir= files.split("/")[1];        //文件夹名
                    String fileName=files.split("/")[2];        //文件名
                    fileUtil=new FileUtil(fileDir);
                    File target = null;
                    if (url.contains(".png")||url.contains(".jpg")){
                        Logger.e("----使用图片专用下载工具-----");
                        havePng=true;
                        try {
                           target=fileUtil.createSdFile(fileName);
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                        new Thread(new DownLoadImageService(context,url,this,target)).start();

                    }else {
                        try {
                            fileUtil.writeToSdFromInput(url,fileName);
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }
                }
                if (havePng){
                    Logger.e("----图片下载完成");
                    Message message=new Message();
                    message.arg1=4;
                    MainActivity.getHandler().sendMessage(message);
                }
            }
        }
    }

    @Override
    public void onDownLoadSuccess(final File file, final File target) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                fileUtil.copy(file,target);
            }
        }).start();

    }

    @Override
    public void onDownLoadFailed() {
        Logger.e("----图片下载失败");
    }

}
