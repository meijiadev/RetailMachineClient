package com.example.retailmachineclient.util;

import android.util.Log;

import com.example.retailmachineclient.BuildConfig;

/**
 * 日志工具
 */
public class Logger {

    public static void v(String msg){
        if (BuildConfig.LOG_DEBUG){
            StackTraceElement traceElement = ((new Exception()).getStackTrace())[1];
            String TAG = "[" +
                    traceElement.getFileName() + " | " +
                    traceElement.getLineNumber() + " | " +
                    traceElement.getMethodName() + "()" + "]";
            Log.v(TAG,msg);
        }
    }

    public static void d(String msg){
        if (BuildConfig.LOG_DEBUG){
            StackTraceElement traceElement = ((new Exception()).getStackTrace())[1];
            String TAG = "[" +
                    traceElement.getFileName() + " | " +
                    traceElement.getLineNumber() + " | " +
                    traceElement.getMethodName() + "()" + "]";
            Log.d(TAG,msg);
        }
    }
    public static void w(String msg){
        if (BuildConfig.LOG_DEBUG){
            StackTraceElement traceElement = ((new Exception()).getStackTrace())[1];
            String TAG = "[" +
                    traceElement.getFileName() + " | " +
                    traceElement.getLineNumber() + " | " +
                    traceElement.getMethodName() + "()" + "]";
            Log.w(TAG,msg);
        }
    }

    public static void e(String msg){
        if (BuildConfig.LOG_DEBUG){
            StackTraceElement traceElement = ((new Exception()).getStackTrace())[1];
            String TAG = "[" +
                    traceElement.getFileName() + " | " +
                    traceElement.getLineNumber() + " | " +
                    traceElement.getMethodName() + "()" + "]";
            Log.e(TAG,msg);
        }
    }

    public static void i(String msg) {
        if (BuildConfig.LOG_DEBUG){
            StackTraceElement traceElement = ((new Exception()).getStackTrace())[1];
            String TAG = "[" +
                    traceElement.getFileName() + " | " +
                    traceElement.getLineNumber() + " | " +
                    traceElement.getMethodName() + "()" + "]";
            Log.i( TAG,msg);
        }
    }






}
