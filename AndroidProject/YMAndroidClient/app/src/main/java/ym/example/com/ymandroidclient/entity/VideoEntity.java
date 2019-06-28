package ym.example.com.ymandroidclient.entity;

import android.graphics.drawable.Drawable;

/**
 * 监控视频信息实体类
 * url name image status
 */
public class VideoEntity {
    private String url;
    private String name;
    private Drawable videoIv;
    private int status;       //监控摄像头状态

    public String getUrl() {
        return url;
    }

    public void setUrl(String url) {
        this.url = url;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public Drawable getVideoIv() {
        return videoIv;
    }

    public void setVideoIv(Drawable videoIv) {
        this.videoIv = videoIv;
    }

    public int getStatus() {
        return status;
    }

    public void setStatus(int status) {
        this.status = status;
    }
}
