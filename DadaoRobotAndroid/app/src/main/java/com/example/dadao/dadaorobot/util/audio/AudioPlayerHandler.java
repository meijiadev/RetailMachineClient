package com.example.dadao.dadaorobot.util.audio;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.util.Log;

import java.util.concurrent.LinkedBlockingDeque;
import java.util.concurrent.Semaphore;

public class AudioPlayerHandler implements Runnable {
    private static final String TAG = "AudioPlayerHandler";
    private AudioTrack track = null;// 录音文件播放对象
    private boolean isPlaying = false;// 标记是否正在录音中
    private int frequence = 16000;// 采样率 16000
    private int channelInConfig = AudioFormat.CHANNEL_OUT_MONO;// 定义采样为双声道（过时，但是使用其他的又不行
    private int audioEncoding = AudioFormat.ENCODING_PCM_16BIT;// 定义音频编码（16位）
    private int bufferSize = -1;// 播放缓冲大小
    private LinkedBlockingDeque<Object> dataQueue = new LinkedBlockingDeque<>();    //双向阻塞队列，可以从两头移除和添加
    // 互斥信号量
    private Semaphore semaphore = new Semaphore(1);
    // 是否释放资源的标志位
    private boolean release = false;
     static AudioPlayerHandler audioPlayerHandler;
    public static AudioPlayerHandler getInstance(){
        if (audioPlayerHandler==null){
            synchronized (AudioPlayerHandler.class){
                if (audioPlayerHandler==null){
                    audioPlayerHandler = new AudioPlayerHandler();
                }
            }
        }
        return audioPlayerHandler;

    }

    public AudioPlayerHandler() {
        // 获取缓冲 大小
        bufferSize = AudioTrack.getMinBufferSize(frequence, channelInConfig,
                audioEncoding);

        // 实例AudioTrack
        track = new AudioTrack(AudioManager.STREAM_MUSIC, frequence,
                channelInConfig, audioEncoding, bufferSize,
                AudioTrack.MODE_STREAM);
        track.setStereoVolume(AudioTrack.getMaxVolume(),
                AudioTrack.getMaxVolume());
        try {
            // 默认需要抢占一个信号量。防止播放进程执行
            semaphore.acquire();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        // 开启播放线程
        new Thread(this).start();
    }

    /**
     * 播放，当有新数据传入时
     * @param data
     *        语音byte数组
     */
    public synchronized void onPlaying(byte[] data) {
        if (AudioTrack.ERROR_BAD_VALUE == bufferSize) {// 初始化错误
            Log.e("播放初始化错误","<<<------");
            return;
        }
        try {
            dataQueue.putLast(data);   //在队列的尾端添加数据
            semaphore.release();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    /**
     * 准备播放
     */
    public void prepare() {
        if (track != null && !isPlaying) {
            track.play();
            isPlaying = true;
        }

    }

    /**
     * 停止播放
     */
    public void stop() {
        if (track != null) {
            track.stop();
            isPlaying = false;
        }
    }

    /**
     * 释放资源
     */
    public void release() {
        release = true;
        semaphore.release();
        if (track != null) {
            track.release();
            track = null;
        }
    }

    @Override
    public void run() {
        while (true) {
            if (release) {
                return;
            }
            if (dataQueue.size() > 0) {
                byte[] data = (byte[]) dataQueue.pollFirst();   //从头开始出，保证了语音数据的顺序
                track.write(data, 0, data.length);

            } else {
                try {
                    semaphore.acquire();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }



}
