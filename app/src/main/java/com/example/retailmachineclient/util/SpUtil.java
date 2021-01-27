package com.example.retailmachineclient.util;

import android.content.Context;
import android.content.SharedPreferences;


/**
 * desc：用于保存设置的参数
 */
public class SpUtil {
    public static final String LANGUAGE = "language";
    private static final String SP_NAME = "poemTripSpref";
    public static final String CHARGE_STATUS="chargeStatus";
    public static final String LOGIN_PASSWORD="password";  //登录密码
    public static final String LOGIN_ACCOUNT="account";    //登录账号
    public static final String TCP_PORT="tcpPort";       //tcp 端口
    public static final String TCP_IP="tcpIp";           //tcp IP
    private static SpUtil spUtil;
    private static SharedPreferences sharedPreferences;
    private static SharedPreferences.Editor editor;
    public static final String HW_ID="io.device.dadao.robot";//华为ID
    public static final String HW_APP_KEY="rnjiKsIkJOh/wVj17AItCw==";//华为APPKEY

    private SpUtil(Context context) {
        context=context.getApplicationContext();
        sharedPreferences = context.getApplicationContext().getSharedPreferences(SP_NAME, Context.MODE_PRIVATE);
        editor = sharedPreferences.edit();
    }

    public static SpUtil getInstance(Context context) {
        if (spUtil == null) {
            synchronized (SpUtil.class) {
                if (spUtil == null) {
                    spUtil = new SpUtil(context);
                }
            }
        }
        return spUtil;
    }

    public void putString(String key, String value) {
        editor.putString(key, value);
        editor.commit();
        Logger.e("设置的语言："+value);
    }

    public void putBoolean(String key, boolean value){
        editor.putBoolean(key,value);
        editor.commit();
    }

    public void putInt(String key, int value){
        editor.putInt(key,value);
        editor.commit();
    }


    public String getString(String key) {
        return sharedPreferences.getString(key,"");
    }

    public boolean getBoolean(String key){
        return sharedPreferences.getBoolean(key,true);
    }

    public int getInt(String key){
        return sharedPreferences.getInt(key,88);
    }
}
