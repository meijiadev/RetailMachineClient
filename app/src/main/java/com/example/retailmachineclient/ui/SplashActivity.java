package com.example.retailmachineclient.ui;

import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.widget.ImageView;


import com.example.retailmachineclient.R;
import com.example.retailmachineclient.base.BaseActivity;
import com.example.retailmachineclient.util.Logger;
import com.hjq.permissions.OnPermission;
import com.hjq.permissions.Permission;
import com.hjq.permissions.XXPermissions;

import java.util.List;

import butterknife.BindView;

public class SplashActivity extends BaseActivity implements Animation.AnimationListener, OnPermission {
    private static final int ANIM_TIME = 2000;
    @BindView(R.id.iv_splash)
    ImageView ivSplash;
    private String [] permission=new String[]{ Permission.READ_EXTERNAL_STORAGE,Permission.WRITE_EXTERNAL_STORAGE,Permission.SYSTEM_ALERT_WINDOW};


    @Override
    protected int getLayoutId() {
        return R.layout.activity_splash;
    }

    @Override
    protected void initView() {
        setStatusBarEnabled(true);
        AlphaAnimation alphaAnimation=new AlphaAnimation(0.4f,1.0f);
        alphaAnimation.setDuration(ANIM_TIME);
        alphaAnimation.setAnimationListener(this);
        ivSplash.startAnimation(alphaAnimation);

    }

    @Override
    protected void initData() {

    }


    /**
     * 请求权限
     */
    private void requestPermission(){
        Logger.e("请求权限！");
        XXPermissions.with(this)
                .permission(permission)
                .request(this);
    }


    /**
     * 权限通过
     * @param granted
     * @param isAll
     */
    @Override
    public void hasPermission(List<String> granted, boolean isAll) {
        Logger.d("权限请求成功");
        startActivityFinish(MainActivity.class);
    }

    @Override
    public void noPermission(List<String> denied, boolean quick) {
        if (quick){
            XXPermissions.gotoPermissionSettings(SplashActivity.this, true);
        }else {
            requestPermission();
        }
    }

    @Override
    public void onAnimationStart(Animation animation) {

    }

    @Override
    public void onAnimationEnd(Animation animation) {
        requestPermission();
    }

    @Override
    public void onAnimationRepeat(Animation animation) {

    }

    @Override
    protected void onRestart() {
        super.onRestart();

    }

    @Override
    public void onBackPressed() {
        //禁用返回键
        //super.onBackPressed();

    }
}
