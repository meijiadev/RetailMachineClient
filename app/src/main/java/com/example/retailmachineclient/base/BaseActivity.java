package com.example.retailmachineclient.base;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.view.WindowManager;


import com.example.retailmachineclient.util.ActivityStackManager;
import com.example.retailmachineclient.util.EventBusManager;
import com.example.retailmachineclient.util.Logger;

import androidx.annotation.Nullable;
import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;
import butterknife.ButterKnife;
import butterknife.Unbinder;

/**
 * desc:正常Activity基类
 * time:2020/08/07
 */
public abstract class BaseActivity extends AppCompatActivity {
    private Unbinder mButterKnife;
    protected boolean isStatusBarEnabled;
    protected Context context;
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Logger.d("-------------"+this.getClass().getSimpleName());
        context= BaseApplication.context;
        if (getLayoutId()>0){
            setContentView(getLayoutId());
        }
        mButterKnife= ButterKnife.bind(this);
        EventBusManager.register(this);
        ActivityStackManager.getInstance().onCreated(this);
        initView();
        initData();
        if (isStatusBarEnabled){
            initState(this);
        }
    }

    /**
     * 是否启用沉浸式状态栏
     * @param statusBarEnabled true启动
     */
    public void setStatusBarEnabled(boolean statusBarEnabled) {
        isStatusBarEnabled = statusBarEnabled;
    }

    /**
     * 沉浸式状态栏（已适配 ）
     */
    private  void initState(Activity activity) {
        Logger.i("启动沉浸式状态栏");
        if (Build.VERSION.SDK_INT >= 21) {
            View decorView = getWindow().getDecorView();
            int option = View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                    | View.SYSTEM_UI_FLAG_LAYOUT_STABLE;
            decorView.setSystemUiVisibility(option);
            getWindow().setStatusBarColor(Color.TRANSPARENT);
            getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                    WindowManager.LayoutParams.FLAG_FULLSCREEN);
        }
        ActionBar actionBar = getSupportActionBar();
        //assert仅在Debug版本中有用
        assert actionBar != null;
        actionBar.hide();
    }




    /**
     * 获取布局
     * @return 布局ID
     */
    protected abstract int getLayoutId();

    /**
     * 初始化控件
     */
    protected abstract void initView();

    /**
     * 初始化参数
     */
    protected abstract void initData();


    @Override
    protected void onRestart() {
        super.onRestart();
        Logger.d("-------------"+this.getClass().getSimpleName());
    }

    @Override
    protected void onResume() {
        super.onResume();
        ActivityStackManager.getInstance().onResume(this);
        Logger.d("-------------"+this.getClass().getSimpleName());
    }

    @Override
    protected void onStart() {
        super.onStart();
        Logger.d("-------------"+this.getClass().getSimpleName());
    }

    @Override
    protected void onPause() {
        super.onPause();
        Logger.d("-------------"+this.getClass().getSimpleName());
    }

    @Override
    protected void onStop() {
        super.onStop();
        Logger.d("-------------"+this.getClass().getSimpleName());
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (mButterKnife!=null)
            mButterKnife.unbind();
        EventBusManager.unregister(this);
        ActivityStackManager.getInstance().onDestroyed(this);
        Logger.d("-------------"+this.getClass().getSimpleName());
    }

    /**
     * startActivity 方法优化
     */
    public void startActivity(Class<? extends Activity> cls) {
        startActivity(new Intent(this, cls));
    }

    /**
     * 跳转并销毁当前activity
     * @param cls
     */
    public void startActivityFinish(Class<? extends Activity> cls) {
        startActivityFinish(new Intent(this, cls));
    }

    private void startActivityFinish(Intent intent) {
        startActivity(intent);
        finish();
    }
}
