package com.example.dadao.dadaorobot.activity;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.os.SystemClock;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Chronometer;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.example.dadao.dadaorobot.Login.view.LoginActivity;
import com.example.dadao.dadaorobot.R;
import com.example.dadao.dadaorobot.Socket.AudioTcpClient;
import com.example.dadao.dadaorobot.Socket.IAudioTcpClient;
import com.example.dadao.dadaorobot.Socket.ITcpClient;
import com.example.dadao.dadaorobot.Socket.TcpClient;
import com.example.dadao.dadaorobot.adapter.VideoListAdapter;
import com.example.dadao.dadaorobot.base.DaDaoBaseActivity;
import com.example.dadao.dadaorobot.entity.RobotIDEntity;
import com.example.dadao.dadaorobot.entity.RobotStatusEntity;
import com.example.dadao.dadaorobot.protocobuf.CmdSchedule;
import com.example.dadao.dadaorobot.protocobuf.dispatcher.ClientMessageDispatcher;
import com.example.dadao.dadaorobot.util.ActivityManager;
import com.example.dadao.dadaorobot.util.Logger;
import com.example.dadao.dadaorobot.util.audio.AudioPlayerHandler;
import com.example.dadao.dadaorobot.util.audio.AudioRecorderHandler;
import com.example.dadao.dadaorobot.util.widget.CircleBarView;
import com.example.dadao.dadaorobot.util.widget.CustomDialog;
import com.example.dadao.dadaorobot.util.widget.PullToRefrshLinearLayout;
import com.google.protobuf.ByteString;

import java.io.File;
import java.io.IOException;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.List;
import DDRCommProto.BaseCmd;
import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;




public class NavigationActivity extends DaDaoBaseActivity implements IAudioTcpClient,ITcpClient {


    @BindView(R.id.list_video)
    ListView listVideo;
    @BindView(R.id.pull_extend)
    PullToRefrshLinearLayout pullExtend;
    @BindView(R.id.relative_log)
    RelativeLayout relativeLog;
    @BindView(R.id.video)
    ImageView video;
    @BindView(R.id.voice)
    ImageView voice;
    @BindView(R.id.log)
    ImageView log;
    @BindView(R.id.account_switch)
    ImageView account_switch;

    @BindView(R.id.frameLayout_voice)
    FrameLayout frameLayout_voice;
    @BindView(R.id.voice_status)
    TextView voice_status;
    @BindView(R.id.chronometer)
    Chronometer chronometer;


    /**头部布局**/
    @BindView(R.id.iv_video_status)
    ImageView ivVideoStatus;
    @BindView(R.id.iv_robot_status)
    ImageView ivRobotStatus;
    @BindView(R.id.iv_log_status)
    ImageView ivLogStatus;
    @BindView(R.id.iv_energy_status)
    CircleBarView ivEnergyStatus;
    @BindView(R.id.tv_energy_status)
    TextView tvEnergyStatus;
    @BindView(R.id.iv_temp_status)
    CircleBarView ivTempStatus;
    @BindView(R.id.tv_temp_status)
    TextView tvTempStatus;
    @BindView(R.id.iv_wetness_status)
    CircleBarView ivWetnessStatus;
    @BindView(R.id.tv_wetness_status)
    TextView tvWetnessStatus;
    @BindView(R.id.iv_line_status)
    CircleBarView ivLineStatus;
    @BindView(R.id.tv_line_status)
    TextView tvLineStatus;
    @BindView(R.id.iv_plastance_status)
    CircleBarView ivPlastanceStatus;
    @BindView(R.id.tv_plastance_status)
    TextView tvPlastanceStatus;
    @BindView(R.id.iv_pm_status)
    CircleBarView ivPmStatus;
    @BindView(R.id.tv_pm_status)
    TextView tvPmStatus;


    private TcpClient tcpClient;
    private AudioTcpClient audioTcpClient;
    private AudioRecorderHandler audioRecorderHandler;
    private AudioPlayerHandler audioPlayerHandler;
    public  boolean isCalling;           //是否正在通话中
    public  String ip="";
    public  int port=0;

    VideoListAdapter videoListAdapter;
    ArrayList<String> mList;
    ArrayList<String> listAddress=new ArrayList<>();   //rtsp视频流地址
    ArrayList<String>listAddressName=new ArrayList<>(); //视频流地址名字


    public static Handler mHandler;
    public static Handler getHandler() {
        return mHandler;
    }

    @SuppressLint("HandlerLeak")
    public Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.arg1) {
                case 0:
                    byte[] data = (byte[]) msg.obj;
                    AudioPlayerHandler.getInstance().onPlaying(data);
                    break;
                case 1:
                    Logger.e("开始连接语音对讲的IP和端口-----");
                    Bundle bundle = msg.getData();
                     ip = bundle.getString("IP");
                     port  = bundle.getInt("Port");
                    break;
                case 2:
                    Logger.e("正在通话中...");
                    voice_status.setVisibility(View.GONE);
                    chronometer.setVisibility(View.VISIBLE);
                    isCalling=true;
                    chronometer.setBase(SystemClock.elapsedRealtime());
                    chronometer.start();
                    break;
                case 3:
                    Bundle bundle1=msg.getData();
                    listAddress=bundle1.getStringArrayList("VideoAddress");
                    listAddressName=bundle1.getStringArrayList("VideoAddressName");
                    Logger.e("-------"+listAddress.size());
                    videoListAdapter.setList(initList());
                    videoListAdapter.notifyDataSetChanged();
                    break;
            }
        }
    };



    public Context context;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_navigation);
        ButterKnife.bind(this);
        context=getApplicationContext();
        mHandler = handler;
        tcpClient = TcpClient.getInstance(this, ClientMessageDispatcher.getInstance());
        tcpClient.setiTcpClient(this);
        getNetworkAddress();
        audioTcpClient=AudioTcpClient.getInstance(this);
        ActivityManager.getInstance().addActivity(this);
        ActivityManager.getInstance().setIActivityManager(iActivityManager);
        initEvent();
        initRecycle();

    }

    CustomDialog customDialog;

    @OnClick({R.id.video, R.id.voice, R.id.log,R.id.account_switch,R.id.frameLayout_voice})
    public void onViewClicked(View view) {
        switch (view.getId()) {
            case R.id.video:
                listVideo.setVisibility(View.VISIBLE);
                relativeLog.setVisibility(View.GONE);
                break;
            case R.id.voice:
                if (!isCalling){
                    frameLayout_voice.setVisibility(View.VISIBLE);
                    voice_status.setVisibility(View.VISIBLE);
                    voice_status.setText("正在连接...");
                    try {
                        startConnect();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
                break;
            case R.id.log:
                listVideo.setVisibility(View.GONE);
                relativeLog.setVisibility(View.VISIBLE);
                break;
            case R.id.account_switch:
                try {
                    tcpClient.disConnect();
                } catch (IOException e) {
                    e.printStackTrace();
                }
                startActivity(new Intent(NavigationActivity.this,LoginActivity.class));
                finish();
                break;
            case R.id.frameLayout_voice:
                if (isCalling){
                    customDialog=new CustomDialog(this, (String) chronometer.getText(), 1,"挂断", new View.OnClickListener() {
                        @Override
                        public void onClick(View v) {
                            Logger.e("-----qqq");
                            disConnectedAudio();
                            isCalling=false;
                            chronometer.stop();
                            frameLayout_voice.setVisibility(View.GONE);
                            chronometer.setVisibility(View.GONE);
                            customDialog.dismiss();

                        }
                    }, "取消", new View.OnClickListener() {
                        @Override
                        public void onClick(View v) {
                            customDialog.dismiss();
                        }
                    });
                    customDialog.setCancelable(false);
                    customDialog.show();
                }else {
                    customDialog=new CustomDialog(this, "是否取消连接", 1,"是", new View.OnClickListener() {
                        @Override
                        public void onClick(View v) {
                            try {
                                audioTcpClient.disConnect();
                                frameLayout_voice.setVisibility(View.GONE);
                                customDialog.dismiss();
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                        }
                    }, "否", new View.OnClickListener() {
                        @Override
                        public void onClick(View v) {
                            customDialog.dismiss();
                        }
                    });
                    customDialog.setCancelable(false);
                    customDialog.show();
                }

                break;
        }
    }



    private void initRecycle() {
        videoListAdapter = new VideoListAdapter(this);
        videoListAdapter.setList(initList());
        listVideo.setAdapter(videoListAdapter);
        listVideo.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                disConnectedAudio();
                isCalling=false;
                Intent intent = new Intent(NavigationActivity.this, MainActivity.class);
                intent.putStringArrayListExtra("videoList", mList);
                intent.putExtra("videoAddress",mList.get(0));
                startActivity(intent);
                finish();
            }
        });
    }

    /**
     * 获取rtmp 和语音对讲的IP 和端口
     */
    private void getNetworkAddress(){
        new Thread(new Runnable() {
            @Override
            public void run() {
                tcpClient.sendData(null,CmdSchedule.streamAddr());
            }
        }).start();
    }

    /**
     * 点击连接
     */
    public  void startConnect() throws InterruptedException {
        if (!ip.isEmpty()&&port!=0){
            audioTcpClient.createConnect(ip, port);
            Logger.e("ip:"+ip+";"+"port:"+port);
        }else {
        Toast.makeText(context,"未检测到机器人IP，请刷新",Toast.LENGTH_SHORT).show();
    }
    }

    @Override
    public void isConnected(boolean isConnected) {
        if (isConnected){
               reqStartTalk();
               startRecord();
               player();
        }else {
            runOnUiThread(new Runnable() {      //必须切换到主线程更新UI
                @Override
                public void run() {
                    Toast.makeText(NavigationActivity.this,"连接失败",Toast.LENGTH_SHORT).show();
                    frameLayout_voice.setVisibility(View.GONE);
                    try {
                        audioTcpClient.disConnect();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            });

        }

    }


    public  void startRecord() {
        audioRecorderHandler = new AudioRecorderHandler(this);
        audioRecorderHandler.startRecord(new AudioRecorderHandler.AudioRecordingCallback() {
            @Override
            public void onRecording(byte[] data) {
               audioTcpClient.sendData(data);          //发送
            }
        });

    }
    /**
     * 发送语音通话命令
     */
    public  void reqStartTalk(){
        tcpClient.sendData(CmdSchedule.commonHeader(BaseCmd.eCltType.eLSMStreamRelay),CmdSchedule.audioTalk(BaseCmd.reqAudioTalk.eOpMode.eStart));
        Logger.e("发送请求语音通话命令");
    }



    /**
     * 语音通话终止命令
     */
    public  void reqStopTalk(){
        tcpClient.sendData(CmdSchedule.commonHeader(BaseCmd.eCltType.eLSMStreamRelay),CmdSchedule.audioTalk(BaseCmd.reqAudioTalk.eOpMode.eStop));
    }

    public void player() {
        audioPlayerHandler = AudioPlayerHandler.getInstance();
        audioPlayerHandler.prepare();
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    audioTcpClient.readData();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }).start();

    }

    /**
     * 断开通话连接
     */
    public  void disConnectedAudio( ){
        if (isCalling){
            reqStopTalk();
           if (audioRecorderHandler!=null){
               audioRecorderHandler.stoppRecord();
               audioRecorderHandler.release();
           }
           if (audioPlayerHandler!=null){
               audioPlayerHandler.stop();
               audioPlayerHandler.release();
           }
            try {
                audioTcpClient.disConnect();
            } catch (IOException e) {
                e.printStackTrace();
            }

        }
    }

    /**
     * 通过回调函数监听头部的状态
     */
    private void initEvent(){
        pullExtend.setOnHeaderRefreshListener(new PullToRefrshLinearLayout.OnHeaderRefreshListener() {
            @Override
            public void onHeaderRefresh(PullToRefrshLinearLayout view) {
                Logger.e("头部刷新");
                RobotStatusEntity robotStatusEntity=RobotStatusEntity.getInstance();
                DecimalFormat df = new DecimalFormat("0.0");
                ivEnergyStatus.setProgress(robotStatusEntity.getBatt(),2000,(robotStatusEntity.getBatt()>10)?Color.GREEN:Color.RED);
                tvEnergyStatus.setText(df.format(robotStatusEntity.getBatt()));
                ivTempStatus.setProgress(robotStatusEntity.getTemp(),2000,(robotStatusEntity.getTemp()>40)?Color.RED:Color.GREEN);
                tvTempStatus.setText(df.format(robotStatusEntity.getTemp()));
                ivWetnessStatus.setProgress(robotStatusEntity.getHum(),2000,Color.GREEN);
                tvWetnessStatus.setText(df.format(robotStatusEntity.getHum()));
                ivLineStatus.setProgress(robotStatusEntity.getPoslinespeed(),2000,Color.GREEN);
                tvLineStatus.setText(df.format(robotStatusEntity.getPoslinespeed()));
                ivPlastanceStatus.setProgress(robotStatusEntity.getPosangulauspeed(),2000,Color.GREEN);
                tvPlastanceStatus.setText(df.format(robotStatusEntity.getPosangulauspeed()));
                ivPmStatus.setProgress(robotStatusEntity.getPm25(),2000,Color.RED);
                tvPmStatus.setText(df.format(robotStatusEntity.getPm25()));

            }

            @Override
            public void hasShowHeader() {


            }
        });

    }


    private List initList() {
        mList = new ArrayList<>();
        if (listAddress.size()>0){
            Logger.e("-------返回列表");
            mList.clear();
            mList=listAddress;
        }else {
            mList.add("一号监控");
            mList.add("二号监控");
            mList.add("三号监控");
            mList.add("四号监控");
            mList.add("五号监控");
            mList.add("六号监控");
            mList.add("七号监控");
        }
        return mList;
    }

    /**
     * 请求文件（txt、png)
     */
    private void requestFile(){
        File file=new File(Environment.getExternalStorageDirectory().getPath()+"/"+"机器人");
        deleteFile(file);
        final ByteString path= ByteString.copyFromUtf8("OneRoute*/Path*.*");
        final ByteString matPath=ByteString.copyFromUtf8("OneRoute*/affine_mat.txt");
        final ByteString pngPath=ByteString.copyFromUtf8("OneRoute*/bkPic.png");
        getHttpFile(path);
        getHttpFile(matPath);
        getHttpFile(pngPath);
    }

    public void getHttpFile(final ByteString path){
        new Thread(new Runnable() {
            @Override
            public void run() {
                BaseCmd.reqFileAddress reqFileAddress = BaseCmd.reqFileAddress.newBuilder()
                        .setTarServiceType(BaseCmd.eCltType.eLSMSlamNavigation)
                        .setFileType(BaseCmd.eFileTypes.FileHttpAddress)
                        .addFileNames(path)
                        .build();
                BaseCmd.CommonHeader commonHeader=BaseCmd.CommonHeader.newBuilder()
                        .setFromCltType(BaseCmd.eCltType.eLocalAndroidClient)
                        .setToCltType(BaseCmd.eCltType.eLSMSlamNavigation)
                        .addFlowDirection(BaseCmd.CommonHeader.eFlowDir.Forward)
                        .build();
                tcpClient.sendData(commonHeader,reqFileAddress);
            }
        }).start();

    }
    /**
     * 删除指定文件夹下所有文件
     * @param file
     */
    public void deleteFile(File file){
        if (file.isDirectory()) {
            File[] files = file.listFiles();
            for (int i = 0; i < files.length; i++) {
                File f = files[i];
                deleteFile(f);
            }
            file.delete();//如要保留文件夹，只删除文件，请注释这行
        } else if (file.exists()) {
            file.delete();
        }
    }




    @Override
    protected void onDestroy() {
        super.onDestroy();
        ActivityManager.getInstance().removeActivity(this);

    }



    public ActivityManager.IActivityManager iActivityManager = new ActivityManager.IActivityManager() {
        @Override
        public void isFinishApp(boolean isFinsh) throws IOException {
            if (isFinsh) {
                isCalling=false;
                tcpClient.disConnect();
                disConnectedAudio();
            }
        }
    };


    @Override
    public void tcpConnected() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(NavigationActivity.this,"连接成功！",Toast.LENGTH_SHORT).show();
            }
        });
    }

    @Override
    public void tcpDisConnected() {
        tcpClient.creatConnect(RobotIDEntity.getInstance().getIp(),RobotIDEntity.getInstance().getPort());
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(NavigationActivity.this,"连接被断开，正在重新连接...",Toast.LENGTH_SHORT).show();
            }
        });
    }

    @Override
    public void tcpServerFinish() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(NavigationActivity.this,"服务器断开，请检查网络",Toast.LENGTH_SHORT).show();
            }
        });
    }

    @Override
    public void connectFailure() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(NavigationActivity.this,"连接失败,请检查网络或者服务器是否正常",Toast.LENGTH_SHORT).show();
            }
        });
    }
}
