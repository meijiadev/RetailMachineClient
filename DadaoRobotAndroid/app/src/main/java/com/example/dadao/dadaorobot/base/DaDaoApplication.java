package com.example.dadao.dadaorobot.base;

import android.app.Application;
import android.content.Context;
import android.support.multidex.MultiDex;

import com.example.dadao.dadaorobot.util.Logger;
import com.tencent.bugly.crashreport.CrashReport;

public class DaDaoApplication extends Application {

    public DaDaoApplication() {
        super();
    }

    private static Context AppContext;

    public static Context getContext() {
        return AppContext;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        MultiDex.install(this);
        CrashReport.initCrashReport(getApplicationContext(), "fb779b17a3", false);
        AppContext = getApplicationContext();
        Logger.d("这里是DaDaoApplication");


    }


}

