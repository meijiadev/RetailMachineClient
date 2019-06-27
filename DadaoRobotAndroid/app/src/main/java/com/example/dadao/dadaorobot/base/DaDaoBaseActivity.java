package com.example.dadao.dadaorobot.base;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.annotation.RequiresApi;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.view.View;


import com.example.dadao.dadaorobot.util.Logger;
import com.example.dadao.dadaorobot.util.widget.CustomDialog;

public class DaDaoBaseActivity extends AppCompatActivity{
    private CustomDialog customDialog;

    public DaDaoBaseActivity() {
        super();
    }


    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        initState(this);

    }

    /**
     * 沉浸式状态栏（已适配 ）
     */
    public  void initState(Activity activity) {
        //Logger.e("启动沉浸式状态栏");
        if (Build.VERSION.SDK_INT >= 21) {
            View decorView = getWindow().getDecorView();
            int option = View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                    | View.SYSTEM_UI_FLAG_LAYOUT_STABLE;
            decorView.setSystemUiVisibility(option);
            getWindow().setStatusBarColor(Color.TRANSPARENT);
        }
        ActionBar actionBar = getSupportActionBar();
        actionBar.hide();

    }

    @SuppressLint("NewApi")
    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
       //initState(this);
    }

    /**
     * 来电语音提示
     */
    public void callReminder(String title,int show,String buttonConfirm,String buttonCancel){
        if (title.isEmpty()){
            customDialog=new CustomDialog(this, "邀请语音通话...",1, "接听", new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    Logger.e("---------接听来电");
                }
            }, "拒绝", new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    customDialog.dismiss();
                    Logger.e("---------拒绝接听");
                }
            });
            customDialog.setCancelable(false);
            customDialog.show();
        }else {
            customDialog=new CustomDialog(this, title,1, buttonConfirm, new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    customDialog.dismiss();
                    Logger.e("---------接听来电");
                }
            }, buttonCancel, new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    customDialog.dismiss();
                    Logger.e("---------拒绝接听");
                }
            });
            customDialog.setCancelable(false);
            customDialog.show();
        }

    }


}


