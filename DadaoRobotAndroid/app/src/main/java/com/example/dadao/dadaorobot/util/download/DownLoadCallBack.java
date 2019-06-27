package com.example.dadao.dadaorobot.util.download;

import java.io.File;

/**
 * create by Ezreal mei
 * 下载结果的回调接口
 */
public interface DownLoadCallBack {
    void onDownLoadSuccess(File file, File target);
    void onDownLoadFailed();
}
