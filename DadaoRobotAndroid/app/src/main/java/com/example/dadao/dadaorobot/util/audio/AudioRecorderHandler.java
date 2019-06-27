package com.example.dadao.dadaorobot.util.audio;

import android.content.Context;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.AsyncTask;
import android.util.Log;

import java.io.File;

/**
 * 作用:录制音频
 * Create by mei jia
 * create time:2018/12/7
 */
public class AudioRecorderHandler {

    /**
     * 录音数据单次回调数组最大为多少
     */
    private static int MAX_DATA_LENGTH = 160;

    private AudioRecord audioRecord;// 录音对象
    private boolean isRecording = false;// 标记是否正在录音中
    private int frequence = 16000;// 采样率 16000
    private int channelInConfig = AudioFormat.CHANNEL_CONFIGURATION_MONO;// 定义采样通道（过时，但是使用其他的又不行
    private int audioEncoding = AudioFormat.ENCODING_PCM_16BIT;// 定义音频编码（16位）
    private byte[] buffer = null;// 录制的缓冲数组



    public AudioRecorderHandler(Context context) {
        if (context == null) {
            throw new RuntimeException("Context could not be null!");
        }
    }

    /**
     * 开始录制音频
     *
     * @param audioRecordingCallback
     *            录制过程中的回调函数
     */
    public void startRecord(AudioRecordingCallback audioRecordingCallback) {
        RecordTask task = new RecordTask(audioRecordingCallback);
        task.execute();// 开始执行
    }

    /**
     * 停止录制
     */
    public void stoppRecord() {
        isRecording = false;
    }


    /**
     * 录制音频的任务类
     */
    private class RecordTask extends AsyncTask<String, Integer, String> {

        private AudioRecordingCallback audioRecordingCallback = null;

        public RecordTask(AudioRecordingCallback audioRecordingCallback) {
            this.audioRecordingCallback = audioRecordingCallback;
        }

        @Override
        protected void onPreExecute() {
            // 根据定义好的几个配置，来获取合适的缓冲大小
            // int bufferSize = 1280;
            int bufferSize = AudioRecord.getMinBufferSize(frequence,
                    channelInConfig, audioEncoding);
            Log.e("缓冲区大小：", String.valueOf(bufferSize));
            // 实例化AudioRecord
            audioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC,
                    frequence, channelInConfig, audioEncoding, bufferSize);
            // 定义缓冲数组
            buffer = new byte[bufferSize];
            MAX_DATA_LENGTH = bufferSize / 2;         //最大数据长度为缓冲区长度的一般
            audioRecord.startRecording();// 开始录制
            isRecording = true;// 设置录制标记为true
        }



        @Override
        protected String doInBackground(String... params) {

            // 开始录制
            while (isRecording) {
                int result = audioRecord.read(buffer, 0, buffer.length);
                if (audioRecordingCallback != null) {
                    int offset = result % MAX_DATA_LENGTH > 0 ? 1 : 0;         //
                    for (int i = 0; i < result / MAX_DATA_LENGTH + offset; i++) {
                        int length = MAX_DATA_LENGTH;
                        if ((i + 1) * MAX_DATA_LENGTH > result) {
                            length = result - i * MAX_DATA_LENGTH;
                        }
                        byte[]sBytes=onRecordingBytes(buffer,i*MAX_DATA_LENGTH,length);
                        audioRecordingCallback.onRecording(sBytes);

                    }
                }
            }
            if (audioRecord != null) {
                try {
                    audioRecord.stop();
                }catch (Exception e){
                    e.printStackTrace();
                }

            }
            return null;
        }
    }

    /**
     * 将一个大的字节数组分割成相应的小的字节数组
     * @param buffer
     * @param startIndes
     * @param length
     * @return
     */
    public byte[] onRecordingBytes(byte[]buffer,int startIndes,int length){
        byte[] reBytes=new byte[length];
        for (int i=0;i<length;i++){
            reBytes[i]=buffer[startIndes+i];
        }
        return reBytes;
    }

    /**
     * 监听录制过程，用于实时获取录音数据
     */
    public  interface AudioRecordingCallback {
        /**
         * 录音数据获取回调
         *
         * @param data
         *            数据数组对象
         *
         */
         void onRecording(byte[] data);

    }

    /**
     * 释放资源
     */
    public void release() {
        if (audioRecord != null) {
            audioRecord.release();
            audioRecord = null;
        }

    }

}

