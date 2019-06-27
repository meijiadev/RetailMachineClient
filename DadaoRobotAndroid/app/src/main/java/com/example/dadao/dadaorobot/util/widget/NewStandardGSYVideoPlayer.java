package com.example.dadao.dadaorobot.util.widget;

import android.content.Context;
import android.os.Message;
import android.util.AttributeSet;

import com.example.dadao.dadaorobot.activity.MainActivity;
import com.example.dadao.dadaorobot.util.Logger;
import com.shuyu.gsyvideoplayer.video.StandardGSYVideoPlayer;

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
