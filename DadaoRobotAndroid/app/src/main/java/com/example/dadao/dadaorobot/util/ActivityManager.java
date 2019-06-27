package com.example.dadao.dadaorobot.util;

import android.app.Activity;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * 作用：活动管理的类,（观察者模式的思想）
 * 判断列表中的活动数量，当列表为空时通过回调函数终止当前所有正在进行的操作
 */
public class ActivityManager {
    static ActivityManager activityManager;
    IActivityManager mIActivityManager;
    List<Activity> mList;


    /**
     * 双重判断的单例模式
     * @return
     */
    public static ActivityManager getInstance(){
        if (activityManager==null){
            synchronized (ActivityManager.class){
                if (activityManager==null){
                    activityManager=new ActivityManager();
                }
            }
        }
        return activityManager;
    }

    public ActivityManager() {
        mList=new ArrayList<>();
    }

    public void addActivity(Activity activity){
        mList.add(activity);
    }


    public void removeActivity(Activity activity){
        mList.remove(activity);
        if (mList.size()==0){
            try {
                mIActivityManager.isFinishApp(true);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }else {
            try {
                mIActivityManager.isFinishApp(false);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }


    public void removeAllActivity(){
        mList.clear();

    }

    /**
     * 回调
     * @param iActivityManager
     */
    public void setIActivityManager(IActivityManager iActivityManager){
        this.mIActivityManager=iActivityManager;
    }


    /**
     * 当列表中的活动为0时表示当前app被销毁，结束一切正在操作的网络请求和线程活动
     */
    public interface IActivityManager {
        void isFinishApp(boolean isFinsh) throws IOException;
    }



}
