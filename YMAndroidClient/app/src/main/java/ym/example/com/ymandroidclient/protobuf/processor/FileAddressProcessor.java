package ym.example.com.ymandroidclient.protobuf.processor;

import android.content.Context;
import android.os.Message;

import com.google.protobuf.ByteString;
import com.google.protobuf.GeneratedMessageLite;

import java.io.File;
import java.io.IOException;
import java.util.List;

import DDRCommProto.BaseCmd;

import ym.example.com.ymandroidclient.MainActivity;
import ym.example.com.ymandroidclient.utility.DownLoadCallBack;
import ym.example.com.ymandroidclient.utility.DownLoadImageService;
import ym.example.com.ymandroidclient.utility.FileUtil;
import ym.example.com.ymandroidclient.utility.Logger;

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
    public void process(Context context, BaseCmd.CommonHeader commonHeader, GeneratedMessageLite msg) {
        this.context=context;
        Logger.e("接收到文件http地址");
        havePng=false;
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
                    Logger.e("-----"+url);
                    String files=url.split("//")[1];
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
                            Logger.e("-----"+fileName);
                            fileUtil.writeToSdFromInput(url,fileName);
                        } catch (IOException e) {
                            e.printStackTrace();
                            Logger.e("-----e"+fileName);
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
