package ym.example.com.ymandroidclient.view;

import android.content.Context;
import android.os.Message;
import android.util.AttributeSet;


import com.shuyu.gsyvideoplayer.video.StandardGSYVideoPlayer;

import ym.example.com.ymandroidclient.MainActivity;
import ym.example.com.ymandroidclient.utility.Logger;

/**
 * 自定义视频播放的控件
 * 重写点击事件的方法
 */
public class NewStandardGSYVideoPlayer extends StandardGSYVideoPlayer {

    public NewStandardGSYVideoPlayer(Context context, Boolean fullFlag) {
        super(context, fullFlag);
    }

    public NewStandardGSYVideoPlayer(Context context) {
        super(context);
    }

    public NewStandardGSYVideoPlayer(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    protected void onClickUiToggle() {
        Logger.e("点击屏幕");
        Message message=new Message();
        message.arg1=0;
        MainActivity.getHandler().sendMessage(message);

    }
}
