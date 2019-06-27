package com.example.dadao.dadaorobot.Login;

import android.content.Intent;
import android.os.Handler;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;

import com.example.dadao.dadaorobot.Login.view.LoginActivity;
import com.example.dadao.dadaorobot.R;
import com.example.dadao.dadaorobot.util.ActivityManager;

/**
 * 产品Logo加载页面
 */
public class LoadActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_load);
        ActivityManager.getInstance().addActivity(this);
        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                startActivity(new Intent(LoadActivity.this,LoginActivity.class));
                finish();
            }
        },1000);
    }
    @Override
    protected void onDestroy() {
        super.onDestroy();
        ActivityManager.getInstance().removeActivity(this);

    }

}
