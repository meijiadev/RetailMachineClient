package com.example.retailmachineclient.base;


import android.app.Application;
import android.content.Context;
import android.os.Build;
import android.util.Log;
import android.widget.Toast;

import com.example.retailmachineclient.glide.ImageLoader;
import com.example.retailmachineclient.ui.CrashActivity;
import com.example.retailmachineclient.ui.SplashActivity;
import com.example.retailmachineclient.util.EventBusManager;
import com.hjq.toast.ToastInterceptor;
import com.hjq.toast.ToastUtils;
import com.hjq.toast.style.ToastWhiteStyle;

import cat.ereza.customactivityoncrash.config.CaocConfig;


/**
 * time :  2019/10/28
 * desc :  项目中的 application 基类
 */
public class BaseApplication extends Application {
    public static Context context;

    @Override
    public void onCreate() {
        super.onCreate();
        context=this;

    }

    public  void initSDK(Application application) {
        // 这个过程专门用于堆分析的 leak 金丝雀
        // 你不应该在这个过程中初始化你的应用程序
       /* if (LeakCanary.isInAnalyzerProcess(application)) {
            return;
        }*/
        // 图片加载器
        ImageLoader.init(application);

        // 内存泄漏检测
        //LeakCanary.install(application);
        // 设置 Toast 拦截器
        ToastUtils.setToastInterceptor(new ToastInterceptor() {
            @Override
            public boolean intercept(Toast toast, CharSequence text) {
                boolean intercept = super.intercept(toast, text);
                if (intercept) {
                    Log.e("Toast", "空 Toast");
                } else {
                    Log.i("Toast", text.toString());
                }
                return intercept;
            }
        });
        // 吐司工具类
        ToastUtils.init(application,new ToastWhiteStyle(application));

        // EventBus 事件总线
        EventBusManager.init();
        // Crash 捕捉界面
       // Crash 捕捉界面
        CaocConfig.Builder.create()
                .backgroundMode(CaocConfig.BACKGROUND_MODE_SHOW_CUSTOM)
                .enabled(true)
                .trackActivities(true)
                .minTimeBetweenCrashesMs(2000)
                // 重启的 Activity
                .restartActivity(SplashActivity.class)
                .errorActivity(CrashActivity.class)
                // 设置监听器
                //.eventListener(new YourCustomEventListener())
                .apply();

    }





    public static Context getContext() {
        return context;
    }





}




