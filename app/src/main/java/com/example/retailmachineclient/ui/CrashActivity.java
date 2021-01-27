package com.example.retailmachineclient.ui;

import android.annotation.SuppressLint;
import android.os.Environment;


import com.example.retailmachineclient.R;
import com.example.retailmachineclient.base.BaseActivity;
import com.example.retailmachineclient.util.Logger;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

import cat.ereza.customactivityoncrash.CustomActivityOnCrash;
import cat.ereza.customactivityoncrash.config.CaocConfig;

public class CrashActivity extends BaseActivity {
    @SuppressLint("SimpleDateFormat")
    private DateFormat dateFormat=new SimpleDateFormat("yyyy-MM-dd-HH-mm-ss"); //用于格式化日期，作为日志文件名的一部分
    @Override
    protected int getLayoutId() {
        return R.layout.activity_splash;
    }

    @Override
    protected void initView() {

    }

    @Override
    protected void initData() {
        CaocConfig mConfig = CustomActivityOnCrash.getConfigFromIntent(getIntent());
        if (mConfig == null) {
            // 这种情况永远不会发生，只要完成该活动就可以避免递归崩溃。
            finish();
        }
        //saveCrashInfo2File(CustomActivityOnCrash.getAllErrorDetailsFromIntent(CrashActivity.this, getIntent()));
        Logger.e("进入重启程序");
        CustomActivityOnCrash.restartApplication(CrashActivity.this, mConfig);
    }


   /* *//**
     * errorMessage
     * @param  errorMessage
     * @return
     *//*
    private String saveCrashInfo2File(String errorMessage){
        File dir=new File(GlobalParameter.ROBOT_FOLDER_LOG);
        if (dir.exists()){
            //  Logger.e("文件夹已存在，无须创建");
        }else {
            Logger.e("创建文件");
            dir.mkdirs();
         }
        StringBuffer sb=new StringBuffer();
        sb.append(errorMessage);
        //存到文件
        String time=dateFormat.format(new Date());
        String fileName = "crash-" + time + ".txt";
        if (Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)){
            try {
                Logger.e("建立log文件");
                File path = new File(GlobalParameter.ROBOT_FOLDER_LOG);
                FileOutputStream fos = new FileOutputStream(path +"/"+ fileName);
                fos.write(sb.toString().getBytes());
                fos.close();
            }catch (IOException e){
                e.printStackTrace();
            }
        }
        return fileName;
    }*/
}
