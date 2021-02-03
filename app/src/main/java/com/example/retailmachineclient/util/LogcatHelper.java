package com.example.retailmachineclient.util;

import android.content.Context;
import android.os.Environment;


import com.example.retailmachineclient.base.BaseThread;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;

/**
 * desc：将调试信息输出到本地保存
 */
public class LogcatHelper {
    private static LogcatHelper INSTANCE = null;
    private static String PATH_LOGCAT;
    private LogDumper mLogDumper = null;
    private int mPId;
    private String cmdStartTime;           //程序开启时间
    private String fileCreateTime;         //写入时间
    private long lFileCreateTime;          // 文件创建时间

    /**
     * 初始化目录
     * */
    public void init(Context context) {
        if (Environment.getExternalStorageState().equals(
                Environment.MEDIA_MOUNTED)) {// 优先保存到SD卡中
            PATH_LOGCAT = Environment.getExternalStorageDirectory()
                    .getAbsolutePath() + File.separator + "adRobotLog";
        } else {// 如果SD卡不存在，就保存到本应用的目录下
            PATH_LOGCAT = context.getFilesDir().getAbsolutePath()
                    + File.separator + "adRobotLog";
        }
        File file = new File(PATH_LOGCAT);
        if (!file.exists()) {
            file.mkdirs();
        }
    }


    public static LogcatHelper getInstance(Context context) {
        if (INSTANCE == null) {
            INSTANCE = new LogcatHelper(context);
        }
        return INSTANCE;
    }

    private LogcatHelper(Context context) {
        init(context);
        cmdStartTime=TimeUtils.getCurrentTime4();
        mPId = android.os.Process.myPid();
    }

    public void start() {
        if (mLogDumper == null){
            mLogDumper = new LogDumper(String.valueOf(mPId));
            mLogDumper.createFile();
            mLogDumper.start();
        }
    }

    public void stop() {
        if (mLogDumper != null) {
            mLogDumper.stopLogs();
            mLogDumper = null;
            INSTANCE=null;
        }
    }

    /**
     * log输出线程
     */
    private class LogDumper extends BaseThread {
        private Process logcatProc;
        private BufferedReader mReader = null;
        private boolean mRunning = true;
        String cmds = null;
        private String mPID;
        private FileOutputStream out = null;

        public  LogDumper(String pid) {
            mPID = pid;
            /**
             *
             * 日志等级：*:v , *:d , *:w , *:e , *:f , *:s
             * 显示当前mPID程序的 E和W等级的日志.
             * */
            // cmds = "logcat *:e *:w | grep \"(" + mPID + ")\"";
            // cmds = "logcat  | grep \"(" + mPID + ")\"";//打印所有日志信息
            // cmds = "logcat -s way";//打印标签过滤信息
            cmds = "logcat *:e | grep \"(" + mPID + ")\"";
        }

        /**
         * 创建文件输出流
         *
         */
        public void createFile(){
            try {
                fileCreateTime=TimeUtils.getCurrentTime4();
                lFileCreateTime=System.currentTimeMillis();
                out = new FileOutputStream(new File(PATH_LOGCAT, getFileName() + ".log"));
            } catch (FileNotFoundException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }

        public void stopLogs() {
            mRunning = false;
        }

        @Override
        public void run() {
            try {
                logcatProc = Runtime.getRuntime().exec(cmds);
                mReader = new BufferedReader(new InputStreamReader(
                        logcatProc.getInputStream()), 1024);
                String line = null;
                while (mRunning && (line = mReader.readLine()) != null) {
                    if (System.currentTimeMillis()-lFileCreateTime>(30*60*1000)){
                        createFile();
                    }
                    if (!mRunning) {
                        break;
                    }
                    if (line.length() == 0) {
                        continue;
                    }
                    if (out != null && line.contains(mPID)) {
                        out.write((line + "\n").getBytes());
                    }
                }

            } catch (IOException e) {
                e.printStackTrace();
            } finally {
                if (logcatProc != null) {
                    logcatProc.destroy();
                    logcatProc = null;
                }
                if (mReader != null) {
                    try {
                        mReader.close();
                        mReader = null;
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
                if (out != null) {
                    try {
                        out.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    out = null;
                }

            }

        }

    }

    /**
     * 生成文件名
     * @return
     */
    public  String getFileName() {
        String fileName="AppStartTime:"+cmdStartTime+"-logCreate:"+fileCreateTime;
        return fileName;// 2012年10月03日 23:41:31
    }
}
