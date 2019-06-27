package com.example.dadao.dadaorobot.activity;

import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.drawable.AnimationDrawable;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.preference.PreferenceManager;
import android.support.annotation.RequiresApi;
import android.support.constraint.ConstraintLayout;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.support.v4.view.ViewPager;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.example.dadao.dadaorobot.R;
import com.example.dadao.dadaorobot.Socket.ITcpClient;
import com.example.dadao.dadaorobot.Socket.TcpClient;
import com.example.dadao.dadaorobot.adapter.VideoRecyclerAdapter;
import com.example.dadao.dadaorobot.adapter.XFragmentPageAdapter;
import com.example.dadao.dadaorobot.entity.RobotIDEntity;
import com.example.dadao.dadaorobot.entity.RobotStatusEntity;
import com.example.dadao.dadaorobot.fragment.LaserMapFragment;
import com.example.dadao.dadaorobot.fragment.VideoFragment;
import com.example.dadao.dadaorobot.protocobuf.CmdSchedule;
import com.example.dadao.dadaorobot.protocobuf.dispatcher.ClientMessageDispatcher;
import com.example.dadao.dadaorobot.util.widget.CustomDialog;
import com.example.dadao.dadaorobot.util.widget.CustomImageView;
import com.example.dadao.dadaorobot.util.widget.NoScrollViewPager;
import com.example.dadao.dadaorobot.util.widget.RockerView;
import com.example.dadao.dadaorobot.util.ActivityManager;
import com.example.dadao.dadaorobot.util.Logger;

import com.example.dadao.dadaorobot.util.widget.NewStandardGSYVideoPlayer;
import com.google.protobuf.ByteString;
import com.shuyu.gsyvideoplayer.GSYVideoManager;
import com.shuyu.gsyvideoplayer.model.VideoOptionModel;

import java.io.File;
import java.io.IOException;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;
import tv.danmaku.ijk.media.player.IjkMediaPlayer;

import static com.example.dadao.dadaorobot.util.widget.RockerView.DirectionMode.DIRECTION_2_HORIZONTAL;
import static com.example.dadao.dadaorobot.util.widget.RockerView.DirectionMode.DIRECTION_2_VERTICAL;
import DDRCommProto.BaseCmd;


/**
 * 视频播放界面
 */
public class MainActivity extends FragmentActivity implements ITcpClient {
    public boolean isdisplay=false;   //右侧操作栏是否显示状态
    public boolean isRightLayout=false;   //右侧布局是否显示 默认 false
    public boolean activityLife;
    private List<String> list;
    private boolean isControl;

    //是否处于遥控状态
    @BindView(R.id.viewPage)
    NoScrollViewPager viewPager;
    @BindView(R.id.my_rocker)
    RockerView mRockerView;
    @BindView(R.id.my_rocker_zy)
    RockerView mRocker_zy;
    @BindView(R.id.back_from_control)
    ImageView backFromControl;
    @BindView(R.id.right_layout)
    ConstraintLayout rightLayout;
    @BindView(R.id.relative_layout)
    RelativeLayout relativeLayout;
    @BindView(R.id.status_bar)
    ConstraintLayout status_bar;
    @BindView(R.id.recycle_video)
    RecyclerView recycle_video;
    /**右侧栏*/
    @BindView(R.id.right_tv_model)
    TextView tv_model;
    @BindView(R.id.right_tv_control)
    TextView tv_control;
    @BindView(R.id.right_tv_log)
    TextView tv_log;
    @BindView(R.id.right_tv_setting)
    TextView tv_setting;

    @BindView(R.id.layout_model)
    LinearLayout layout_model;
    @BindView(R.id.layout_control)
    LinearLayout layout_control;
    @BindView(R.id.layout_shutdown)
    LinearLayout layout_shutdown;
    @BindView(R.id.layout_log)
    LinearLayout layout_log;
    @BindView(R.id.layout_setting)
    LinearLayout layout_setting;

    @BindView(R.id.tv_current_model)
    TextView tv_current_model;
    @BindView(R.id.tv_subschema_one)
    TextView tv_subschema_one;
    @BindView(R.id.tv_subschema_two)
    TextView tv_subSchema_two;
    @BindView(R.id.tv_subschema_exit)
    TextView tv_subschema_exit;

    @BindView(R.id.tv_collect)
    TextView tv_collect;
    @BindView(R.id.tv_await)
    TextView tv_await;
    @BindView(R.id.tv_charging)
    TextView tv_charging;
    @BindView(R.id.tv_go_static)
    TextView tv_go_static;
    @BindView(R.id.tv_go_dynamic)
    TextView tv_go_dynamic;
    @BindView(R.id.tv_auto_static)
    TextView tv_auto_static;
    @BindView(R.id.tv_auto_dynamic)
    TextView tv_auto_dynamic;
    @BindView(R.id.tv_exit)
    TextView tv_exit;
    @BindView(R.id.tv_modeMsg_bar)
    TextView tv_modelMsg_bar;
    @BindView(R.id.tv_energy_bar)
    TextView tv_energy_bar;
    @BindView(R.id.tv_temp_bar)
    TextView tv_temp_bar;
    @BindView(R.id.tv_wetness_bar)
    TextView tv_wetness_bar;
    @BindView(R.id.tv_line_bar)
    TextView tv_line_bar;
    @BindView(R.id.tv_plastance_bar)
    TextView tv_plastance_bar;
    @BindView(R.id.tv_pm_bar)
    TextView tv_pm_bar;

    @BindView(R.id.iv_cs)
    ImageView iv_cs;
    @BindView(R.id.iv_hw)
    ImageView iv_hw;
    @BindView(R.id.iv_jt)
    ImageView iv_jt;
    @BindView(R.id.iv_sb)
    ImageView iv_sb;
    @BindView(R.id.current_time)
    TextView current_time;
    private VideoRecyclerAdapter videoRecyclerAdapter;
    private CustomDialog customDialog,customDialog1;
    private RobotStatusEntity robotStatusEntity;
    private TcpClient tcpClient;
    private ByteString collectPath;
    private Handler customImageHandle;

    Animation statusHide;
    Animation statusShow;
    Animation rightLayoutHide;
    Animation rightLayoutShow;
    AnimationDrawable collect;

    public static Handler mHandler;
    public Handler handler=new Handler(){
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            int m=msg.arg1;
            switch (m){
                case 0:
                    if (isControl){

                    }else {
                        if (isdisplay){
                            isdisplay=false;
                            isRightLayout=false;
                            rightLayout.startAnimation(rightLayoutHide);
                            rightLayout.setVisibility(View.GONE);
                            relativeLayout.setVisibility(View.GONE);
                            /*status_bar.setVisibility(View.VISIBLE);
                            status_bar.startAnimation(statusShow);*/
                        }else {
                            isdisplay=true;
                            isRightLayout=false;
                            rightLayout.startAnimation(rightLayoutShow);
                            rightLayout.setVisibility(View.VISIBLE);
                            relativeLayout.setVisibility(View.GONE);
                        }
                    }
                    break;
                case 1:
                    getCurrentTime();
                    break;
                case 2:
                    tv_current_model.setVisibility(View.VISIBLE);
                    initModel();
                    initStatusBar();
                    break;
                case 3:
                    boolean isSucceed= (boolean) msg.obj;
                    if (isSucceed){
                        new Handler().postDelayed(new Runnable() {
                            @Override
                            public void run() {
                                Message message=new Message();
                                message.arg1=4;
                                customImageHandle.sendMessage(message);
                            }
                        },500);
                        Toast.makeText(MainActivity.this,"路径设置成功",Toast.LENGTH_LONG).show();
                    }else {
                        Toast.makeText(MainActivity.this,"路径设置失败",Toast.LENGTH_LONG).show();
                    }
                    break;
                case 4:
                    setPng();
                    break;
                case 5:
                    Toast.makeText(MainActivity.this,"该文件不存在，请重新下载或者检查服务中是否存在该文件",Toast.LENGTH_LONG).show();
                    break;

            }

        }
    };

    public static Handler getHandler(){
        return mHandler;
    }

    private SharedPreferences sharedPreferences;
    private SharedPreferences.Editor editor;


    @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        ActivityManager.getInstance().addActivity(this);
        ActivityManager.getInstance().setIActivityManager(iActivityManager);
        ButterKnife.bind(this);
        activityLife=true;
        mHandler=handler;
        initData();
        initRockerView();
        initRecycleView();
        initViewPageFragment();
        new TimeThread().start();
        tcpClient=TcpClient.getInstance(this, ClientMessageDispatcher.getInstance());
        tcpClient.setiTcpClient(this);
        rightLayoutHide=AnimationUtils.loadAnimation(this,R.anim.right_layout_hide);
        rightLayoutShow=AnimationUtils.loadAnimation(this,R.anim.right_layout_show);
        sharedPreferences=PreferenceManager.getDefaultSharedPreferences(this);
        editor=sharedPreferences.edit();
        //File file=new File(Environment.getExternalStorageDirectory().getPath()+"/"+"机器人");
        //deleteFile(file);
        requestFile();


    }

    @OnClick({R.id.layout_model,R.id.layout_control,R.id.layout_log,R.id.layout_setting,R.id.layout_shutdown,
              R.id.tv_collect,R.id.tv_await,R.id.tv_charging,R.id.tv_go_static,R.id.tv_go_dynamic,R.id.tv_auto_static,
              R.id.tv_auto_dynamic,R.id.tv_exit,R.id.back_from_control,R.id.tv_subschema_exit,R.id.tv_subschema_one,R.id.tv_subschema_two})
    public void onViewClicked(View view){
        switch (view.getId()){
            case R.id.layout_model:
                if (isRightLayout){
                    relativeLayout.setVisibility(View.GONE);
                    isRightLayout=false;
                }else {
                    relativeLayout.setVisibility(View.VISIBLE);
                    isRightLayout=true;
                }
                break;
            case R.id.layout_control:
                 isControl=true;
                 rightLayout.setVisibility(View.GONE);
                 backFromControl.setVisibility(View.VISIBLE);
                 mRockerView.setVisibility(View.VISIBLE);
                 mRocker_zy.setVisibility(View.VISIBLE);
                 initTimer();
                 break;
            case R.id.back_from_control:
                isControl=false;
                timer.cancel();
                mRockerView.setVisibility(View.GONE);
                mRocker_zy.setVisibility(View.GONE);
                backFromControl.setVisibility(View.GONE);
                rightLayout.setVisibility(View.VISIBLE);
                break;

            case R.id.layout_log:
                startActivity(new Intent(MainActivity.this,LogActivity.class));
                break;
            case R.id.layout_setting:
                Intent intent=new Intent(MainActivity.this,SettingActivity.class);
                intent.putExtra("MainToSet",0);
                startActivity(intent);
                break;
            case R.id.layout_shutdown:
                customDialog=new CustomDialog(this, "关机or重启", 3,"关机", new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        Logger.e("-----qqq");
                        reqCmdIpcMethod(BaseCmd.eCmdIPCMode.eShutDown);
                        customDialog.dismiss();

                    }
                }, "重启", new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        reqCmdIpcMethod(BaseCmd.eCmdIPCMode.eReStart);
                        customDialog.dismiss();
                    }
                });
                customDialog.setCancelable(true);
                customDialog.show();
                break;
            case R.id.tv_await:
                Toast.makeText(this,"点击退出，返回待命模式",Toast.LENGTH_SHORT).show();
                break;
            case R.id.tv_exit:
                //collect.stop();
                exitModel();
                break;
            case R.id.tv_subschema_exit:
                tv_subschema_exit.setVisibility(View.GONE);
                exitModel();
                break;
            case R.id.tv_subschema_one:
                startOrStop();
                break;
            case R.id.tv_subschema_two:
                switch (mModel){
                    case 1:
                        break;
                    case 4:
                        break;
                        default:
                            exitModel();
                            Logger.e("点击退出当前状态" );
                            break;
                }
                break;
            case R.id.tv_collect:
                restoreDefault();
                tv_model.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.collect_blue),null,null);
                tv_model.setText("采集模式");
                collectDialog();
                break;
            case R.id.tv_charging:
                restoreDefault();
                sendModel(BaseCmd.eCmdActionMode.eReCharging);
                tv_model.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.charging_blue),null,null);
                tv_model.setText("充电模式");
                break;
            case R.id.tv_go_static:
                restoreDefault();
                sendModel(BaseCmd.eCmdActionMode.eBlindWaldStatic);
                tv_model.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.go_static_blue),null,null);
                tv_model.setText("盲走静态");
                break;
            case R.id.tv_go_dynamic:
                restoreDefault();
                sendModel(BaseCmd.eCmdActionMode.eBlindWaldDynamic);
                tv_model.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.go_dynamic_blue),null,null);
                tv_model.setText("盲走动态");
                break;
            case R.id.tv_auto_static:
                restoreDefault();
                sendModel(BaseCmd.eCmdActionMode.eAutoStatic);
                tv_model.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.auto_static_blue),null,null);
                tv_model.setText("自动静态");
                break;
            case R.id.tv_auto_dynamic:
                restoreDefault();
                sendModel(BaseCmd.eCmdActionMode.eAutoDynamic);
                tv_model.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.auto_dynamic_blue),null,null);
                tv_model.setText("自动动态");
                break;
        }

    }

    private void exitModel(){
        restoreDefault();
        BaseCmd.reqCmdEndActionMode reqCmdEndActionMode=BaseCmd.reqCmdEndActionMode.newBuilder()
                .setError("noError")
                .build();
        BaseCmd.CommonHeader commonHeader=BaseCmd.CommonHeader.newBuilder()
                .setFromCltType(BaseCmd.eCltType.eLocalAndroidClient)
                .setToCltType(BaseCmd.eCltType.eLSMSlamNavigation)
                .addFlowDirection(BaseCmd.CommonHeader.eFlowDir.Forward)
                .build();
        tcpClient.sendData(commonHeader,reqCmdEndActionMode);
        tv_model.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.await_blue),null,null);
        tv_model.setText("待命模式");
    }

    private void sendModel(BaseCmd.eCmdActionMode eCmdActionMode){
        BaseCmd.reqCmdStartActionMode reqCmdStartActionMode=BaseCmd.reqCmdStartActionMode.newBuilder()
                .setMode(eCmdActionMode)
                .build();
        BaseCmd.CommonHeader commonHeader=BaseCmd.CommonHeader.newBuilder()
                .setFromCltType(BaseCmd.eCltType.eLocalAndroidClient)
                .setToCltType(BaseCmd.eCltType.eLSMSlamNavigation)
                .addFlowDirection(BaseCmd.CommonHeader.eFlowDir.Forward)
                .build();
        tcpClient.sendData(commonHeader,reqCmdStartActionMode);
    }

    /**
     * 机器人暂停或者重新运动
     * @param value
     */
    private void pauseOrResume(String value){
        BaseCmd.reqCmdPauseResume reqCmdPauseResume=BaseCmd.reqCmdPauseResume.newBuilder()
                .setError(value)
                .build();
        BaseCmd.CommonHeader commonHeader=BaseCmd.CommonHeader.newBuilder()
                .setFromCltType(BaseCmd.eCltType.eLocalAndroidClient)
                .setToCltType(BaseCmd.eCltType.eLSMSlamNavigation)
                .addFlowDirection(BaseCmd.CommonHeader.eFlowDir.Forward)
                .build();
        tcpClient.sendData(commonHeader,reqCmdPauseResume);
    }

    private void startOrStop(){
        if (mModel!=0&&mModel!=1&&mModel!=4){
            switch (sonModel){
                case 1:
                    pauseOrResume("Resume");
                    break;
                case 2:
                    pauseOrResume("pause");
                    break;
            }
        }

    }

    /**
     * 请求文件（txt、png) 刷新文件
     */
    private void requestFile(){
//        final ByteString path= ByteString.copyFromUtf8("OneRoute*/Path*.txt*");
//        final ByteString matPath=ByteString.copyFromUtf8("OneRoute*/affine_mat.txt");
//        final ByteString pngPath=ByteString.copyFromUtf8("OneRoute*/bkPic.png");
        final ByteString filePath=ByteString.copyFromUtf8("OneRoute*/*.*");

//        getHttpFile(path);
//        getHttpFile(matPath);
//        getHttpFile(pngPath);
        getHttpFile(filePath);
        Logger.e("请求文件中....");
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

    String route_Name;
    int mModel,oldMode;
    int sonModel,oldSonMode;
    private void initModel(){
        robotStatusEntity=RobotStatusEntity.getInstance();
        mModel=robotStatusEntity.getModel();
        sonModel=robotStatusEntity.getSonmodel();
        route_Name=robotStatusEntity.getCurroute();
        restoreDefault();
        switch (mModel){
            case 0 :
                showModel();
                if (oldMode==1&&oldSonMode==3){
                    requestFile();
                }
                oldMode=mModel;
                oldSonMode=sonModel;
                tv_model.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.await_blue),null,null);
                tv_model.setText("待命");
                tv_modelMsg_bar.setText("待命");
                tv_await.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.await_blue),null,null);
                tv_await.setTextColor(getResources().getColor(R.color.textselected));
                break;
            case 1:
                hideModel();
                tv_current_model.setVisibility(View.VISIBLE);
                tv_model.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.collect_blue),null,null);
                tv_model.setText("采集模式");
                tv_modelMsg_bar.setText("采集模式");
                tv_current_model.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.collect_blue),null,null);
                tv_current_model.setText("采集模式");
                tv_subschema_exit.setVisibility(View.VISIBLE);
                switch (sonModel){
                    case 1:
                        break;
                    case 2:
                        tv_subschema_one.setVisibility(View.VISIBLE);
                        tv_subschema_one.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.collecting_blue),null,null);
                        tv_subschema_one.setText("采集中");
                        tv_subSchema_two.setVisibility(View.VISIBLE);
                        tv_subSchema_two.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.generating_gray),null,null);
                        tv_subSchema_two.setText("生成中");
                        break;
                    case 3:
                        tv_subschema_one.setVisibility(View.VISIBLE);
                        tv_subschema_one.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.collecting_gray),null,null);
                        tv_subschema_one.setText("采集中");
                        tv_subSchema_two.setVisibility(View.VISIBLE);
                        tv_subSchema_two.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.generating_blue),null,null);
                        tv_subSchema_two.setText("生成中");
                        break;
                }
                oldMode=mModel;
                oldSonMode=sonModel;
                break;
            case 3:
                hideModel();
                tv_model.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.auto_dynamic_blue),null,null);
                tv_model.setText("自动动态");
                tv_modelMsg_bar.setText("自动动态");
                tv_current_model.setVisibility(View.VISIBLE);
                tv_current_model.setText("自动动态");
                tv_current_model.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.auto_dynamic_blue),null,null);
                switch (sonModel){
                    case 1:
                        tv_subschema_one.setVisibility(View.VISIBLE);
                        tv_subschema_one.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.start),null,null);
                        tv_subschema_one.setText("停止行走");
                        break;
                    case 2:
                        tv_subschema_one.setVisibility(View.VISIBLE);
                        tv_subschema_one.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.stop),null,null);
                        tv_subschema_one.setText("行走中");
                        if (customImageHandle!=null){
                            Message message=new Message();
                            message.arg1=2;
                            customImageHandle.sendMessage(message);
                        }
                        break;
                    default:
                        showModel();
                        tv_auto_dynamic.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.auto_dynamic_blue),null,null);
                        tv_auto_dynamic.setText("自动动态");
                }
                oldMode=mModel;
                oldSonMode=sonModel;
                break;
            case 2:
                hideModel();
                tv_model.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.auto_static_blue),null,null);
                tv_model.setText("自动静态");
                tv_modelMsg_bar.setText("自动静态");
                tv_current_model.setVisibility(View.VISIBLE);
                tv_current_model.setText("自动静态");
                tv_current_model.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.auto_static_blue),null,null);
                switch (sonModel){
                    case 1:
                        tv_subschema_one.setVisibility(View.VISIBLE);
                        tv_subschema_one.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.start),null,null);
                        tv_subschema_one.setText("停止行走");
                        break;
                    case 2:
                        tv_subschema_one.setVisibility(View.VISIBLE);
                        tv_subschema_one.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.stop),null,null);
                        tv_subschema_one.setText("行走中");
                        if (customImageHandle!=null){
                            Message message=new Message();
                            message.arg1=2;
                            customImageHandle.sendMessage(message);
                        }
                        break;
                    default:
                        showModel();
                        tv_auto_static.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.auto_static_blue),null,null);
                        tv_auto_static.setText("自动静态");
                        break;
                }
                oldMode=mModel;
                oldSonMode=sonModel;
                break;
            case 4:
                hideModel();
                tv_model.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.charging_blue),null,null);
                tv_model.setText("充电");
                tv_modelMsg_bar.setText("充电");
                tv_current_model.setVisibility(View.VISIBLE);
                tv_current_model.setText("充电");
                tv_current_model.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.charging_blue),null,null);
                switch (sonModel){
                    case 1:
                        tv_subschema_one.setVisibility(View.VISIBLE);
                        tv_subschema_one.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.return_charge_blue),null,null);
                        tv_subschema_one.setText("返回充电桩");
                        tv_subSchema_two.setVisibility(View.VISIBLE);
                        tv_subSchema_two.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.son_charging_gray),null,null);
                        tv_subSchema_two.setText("充电中");
                        break;
                    case 2:
                        tv_subschema_one.setVisibility(View.VISIBLE);
                        tv_subschema_one.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.return_charge_gray),null,null);
                        tv_subschema_one.setText("返回充电桩");
                        tv_subSchema_two.setVisibility(View.VISIBLE);
                        tv_subSchema_two.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.son_charging_blue),null,null);
                        tv_subSchema_two.setText("充电中");
                        break;
                        default:
                            showModel();
                            tv_charging.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.charging_blue),null,null);
                            tv_charging.setText("充电");
                }
                oldMode=mModel;
                oldSonMode=sonModel;
                break;
            case 5:
                hideModel();
                tv_model.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.go_static_blue),null,null);
                tv_model.setText("盲走静态");
                tv_modelMsg_bar.setText("盲走静态");
                tv_current_model.setVisibility(View.VISIBLE);
                tv_current_model.setText("盲走静态");
                tv_current_model.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.go_static_blue),null,null);
                switch (sonModel){
                    case 1:
                        tv_subschema_one.setVisibility(View.VISIBLE);
                        tv_subschema_one.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.start),null,null);
                        tv_subschema_one.setText("停止行走");
                        break;
                    case 2:
                        tv_subschema_one.setVisibility(View.VISIBLE);
                        tv_subschema_one.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.stop),null,null);
                        tv_subschema_one.setText("行走中");
                        break;
                        default:
                            showModel();
                            tv_go_static.setText("盲走静态");
                            tv_go_static.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.go_static_blue),null,null);
                            break;
                }
                oldMode=mModel;
                oldSonMode=sonModel;
                break;
            case 6:
                hideModel();
                tv_model.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.go_dynamic_blue),null,null);
                tv_model.setText("盲走动态");
                tv_modelMsg_bar.setText("盲走动态");
                tv_current_model.setVisibility(View.VISIBLE);
                tv_current_model.setText("盲走动态");
                tv_current_model.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.go_dynamic_blue),null,null);
                switch (sonModel){
                    case 1:
                        tv_subschema_one.setVisibility(View.VISIBLE);
                        tv_subschema_one.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.start),null,null);
                        tv_subschema_one.setText("停止行走");
                        break;
                    case 2:
                        tv_subschema_one.setVisibility(View.VISIBLE);
                        tv_subschema_one.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.stop),null,null);
                        tv_subschema_one.setText("行走中");
                        break;
                    default:
                        showModel();
                        tv_go_dynamic.setText("盲走动态");
                        tv_go_dynamic.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.go_dynamic_blue),null,null);
                        break;
                }
                oldMode=mModel;
                oldSonMode=sonModel;
                break;
                default:
                    break;
        }
        if (mModel!=1&&mModel!=4&&mModel!=0){
            if (sonModel==1|sonModel==2){
                tv_subSchema_two.setVisibility(View.VISIBLE);
                tv_subSchema_two.setText("退出");
                tv_subSchema_two.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.exit_gray),null,null);
            }
        }else if (mModel==4){
            if (sonModel==1|sonModel==2){
                tv_subschema_exit.setVisibility(View.VISIBLE);
                tv_subschema_exit.setText("退出");
            }
        }


    }

    private void initStatusBar(){
        robotStatusEntity=RobotStatusEntity.getInstance();
        DecimalFormat df = new DecimalFormat("0.0");
        tv_energy_bar.setText(df.format(robotStatusEntity.getBatt()));
        tv_temp_bar.setText(df.format(robotStatusEntity.getTemp()));
        tv_wetness_bar.setText(df.format(robotStatusEntity.getHum()));
        tv_plastance_bar.setText(df.format(robotStatusEntity.getPosangulauspeed()));
        tv_line_bar.setText(df.format(robotStatusEntity.getPoslinespeed()));
        tv_pm_bar.setText(df.format(robotStatusEntity.getPm25()));
        switch (robotStatusEntity.getStopstat()){
            case 0x01:
                iv_hw.setVisibility(View.GONE);
                iv_jt.setVisibility(View.GONE);
                iv_sb.setVisibility(View.GONE);
                iv_cs.setVisibility(View.VISIBLE);
                break;
            case 0x02:
                iv_jt.setVisibility(View.GONE);
                iv_sb.setVisibility(View.GONE);
                iv_cs.setVisibility(View.GONE);
                iv_hw.setVisibility(View.VISIBLE);
                break;
            case 0x04:
                iv_sb.setVisibility(View.GONE);
                iv_cs.setVisibility(View.GONE);
                iv_hw.setVisibility(View.GONE);
                iv_jt.setVisibility(View.VISIBLE);
                break;
            case 0x08:
                iv_cs.setVisibility(View.GONE);
                iv_hw.setVisibility(View.GONE);
                iv_jt.setVisibility(View.GONE);
                iv_sb.setVisibility(View.VISIBLE);
                break;
            case 0x00:
                iv_hw.setVisibility(View.GONE);
                iv_jt.setVisibility(View.GONE);
                iv_sb.setVisibility(View.GONE);
                iv_cs.setVisibility(View.GONE);
                break;
        }
    }

    /**
     * 恢复原始状态
     */
    private void restoreDefault(){
            tv_model.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.await_gray),null,null);
            tv_model.setText("待命");
            tv_await.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.await_gray),null,null);
            tv_await.setTextColor(getResources().getColor(R.color.textdefault));
            tv_collect.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.collect_gray),null,null);
            tv_collect.setTextColor(getResources().getColor(R.color.textdefault));
            tv_auto_dynamic.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.auto_dynamic_gray),null,null);
            tv_auto_dynamic.setTextColor(getResources().getColor(R.color.textdefault));
            tv_auto_static.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.auto_static_gray),null,null);
            tv_auto_static.setTextColor(getResources().getColor(R.color.textdefault));
            tv_charging.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.charging_gray),null,null);
            tv_charging.setTextColor(getResources().getColor(R.color.textdefault));
            tv_go_dynamic.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.go_dynamic_gray),null,null);
            tv_go_dynamic.setTextColor(getResources().getColor(R.color.textdefault));
            tv_go_static.setCompoundDrawablesWithIntrinsicBounds(null,getResources().getDrawable(R.mipmap.go_static_gray),null,null);
            tv_go_static.setTextColor(getResources().getColor(R.color.textdefault));
    }


    /**
     * 当处于除待命模式下任意模式，则隐藏
     */
    private void hideModel(){
        tv_collect.setVisibility(View.GONE);
        tv_await.setVisibility(View.GONE);
        tv_auto_dynamic.setVisibility(View.GONE);
        tv_auto_static.setVisibility(View.GONE);
        tv_charging.setVisibility(View.GONE);
        tv_go_dynamic.setVisibility(View.GONE);
        tv_go_static.setVisibility(View.GONE);
        tv_exit.setVisibility(View.GONE);

        tv_subschema_one.setVisibility(View.GONE);
        tv_subSchema_two.setVisibility(View.GONE);
        tv_current_model.setVisibility(View.GONE);
        tv_subschema_exit.setVisibility(View.GONE);
    }

    /**
     * 当处于待命模式下，所有模式都显示
     */
    private void showModel(){
        tv_collect.setVisibility(View.VISIBLE);
        tv_await.setVisibility(View.VISIBLE);
        tv_auto_dynamic.setVisibility(View.VISIBLE);
        tv_auto_static.setVisibility(View.VISIBLE);
        tv_charging.setVisibility(View.VISIBLE);
        tv_go_dynamic.setVisibility(View.VISIBLE);
        tv_go_static.setVisibility(View.VISIBLE);
        tv_exit.setVisibility(View.VISIBLE);

        tv_subschema_one.setVisibility(View.GONE);
        tv_subSchema_two.setVisibility(View.GONE);
        tv_current_model.setVisibility(View.GONE);
        tv_subschema_exit.setVisibility(View.GONE);
    }







    String source="rtsp://";    //1080p
    private void initData(){
        list=getIntent().getStringArrayListExtra("videoList");
        Logger.e("------"+list.size());
        source=getIntent().getStringExtra("videoAddress");
    }


    /**
     * 初始化列表布局
     */
    private void initRecycleView(){
        videoRecyclerAdapter=new VideoRecyclerAdapter(this,list);
        LinearLayoutManager linearLayoutManager=new LinearLayoutManager(this);
        linearLayoutManager.setOrientation(LinearLayoutManager.HORIZONTAL);
        recycle_video.setLayoutManager(linearLayoutManager);
        recycle_video.setAdapter(videoRecyclerAdapter);
        videoRecyclerAdapter.setOtemClickListener(new VideoRecyclerAdapter.OnItemClickListener() {
            @Override
            public void onItemClick(VideoRecyclerAdapter.ViewHolder view, int position) {
                source=list.get(position);
                GSYVideoManager.releaseAllVideos();
            }
        });
    }
    private List<Fragment> fragmentList=new ArrayList<>();
    private XFragmentPageAdapter xFragmentPageAdapter;

    /**
     * 初始化ViewPage
     */
    private void initViewPageFragment(){
        fragmentList=new ArrayList<>();
        fragmentList.add(new VideoFragment());
        fragmentList.add(new LaserMapFragment());
        xFragmentPageAdapter=new XFragmentPageAdapter(getSupportFragmentManager(),fragmentList);
        viewPager.setAdapter(xFragmentPageAdapter);
        viewPager.setScroll(true);
        viewPager.setCurrentItem(0);
        viewPager.addOnPageChangeListener(new ViewPager.OnPageChangeListener() {
            @Override
            public void onPageScrolled(int i, float v, int i1) {
                //Logger.e("onPageScrolled:" + i + "," + v + "," + i1);
            }

            @Override
            public void onPageSelected(int i) {

                Logger.e("当前页码：" + i);
            }

            @Override
            public void onPageScrollStateChanged(int i) {

            }
        });
    }


    private float lineSpeed,palstance;  //线速度 ，角速度
    private double maxSpeed=0.4;       //设置的最大速度
    private boolean isforward,isGoRight; //左右摇杆当前的方向

    /**
     * 自定义摇杆View的相关操作
     * 作用：监听摇杆的方向，角度，距离
     */
    private void initRockerView() {
        mRockerView.setOnShakeListener(DIRECTION_2_VERTICAL, new RockerView.OnShakeListener() {
            @Override
            public void onStart() {

            }
            @Override
            public void direction(RockerView.Direction direction) {
                if (direction == RockerView.Direction.DIRECTION_CENTER) {           // "当前方向：中心"
                    //Logger.e("---中心");
                    lineSpeed=0;
                    mRockerView.setmAreaBackground(R.mipmap.rocker_base_default);
                } else if (direction == RockerView.Direction.DIRECTION_DOWN) {     // 当前方向：下
                    isforward=false;
                    mRockerView.setmAreaBackground(R.mipmap.rocker_backward);
                    //Logger.e("下");
                } else if (direction == RockerView.Direction.DIRECTION_LEFT) {    //当前方向：左

                } else if (direction == RockerView.Direction.DIRECTION_UP) {      //当前方向：上
                    isforward=true;
                    mRockerView.setmAreaBackground(R.mipmap.rocker_forward);
                    //Logger.e("上");
                } else if (direction == RockerView.Direction.DIRECTION_RIGHT) {

                }
            }

            @Override
            public void onFinish() {

            }
        });

        mRocker_zy.setOnShakeListener(DIRECTION_2_HORIZONTAL, new RockerView.OnShakeListener() {
            @Override
            public void onStart() {

            }

            @Override
            public void direction(RockerView.Direction direction) {
                if (direction == RockerView.Direction.DIRECTION_CENTER) {           // "当前方向：中心"
                   // Logger.e("---中心");
                    mRocker_zy.setmAreaBackground(R.mipmap.rocker_default_zy);
                    palstance=0;
                } else if (direction == RockerView.Direction.DIRECTION_DOWN) {

                } else if (direction == RockerView.Direction.DIRECTION_LEFT) {    //当前方向：左
                    isGoRight=false;
                    mRocker_zy.setmAreaBackground(R.mipmap.rocker_go_left);
                   // Logger.e("左");
                } else if (direction == RockerView.Direction.DIRECTION_UP) {      //当前方向：上

                } else if (direction == RockerView.Direction.DIRECTION_RIGHT) {
                    // mTvShake.setText("当前方向：右");
                    //Logger.e("右");
                    isGoRight=true;
                    mRocker_zy.setmAreaBackground(R.mipmap.rocker_go_right);
                }
            }

            @Override
            public void onFinish() {

            }
        });


        mRocker_zy.setOnDistanceLevelListener(new RockerView.OnDistanceLevelListener() {
            @Override
            public void onDistanceLevel(int level) {
                DecimalFormat df = new DecimalFormat("#.00");
                palstance=Float.parseFloat(df.format(maxSpeed*level/10));
                if (isGoRight){
                    palstance=-palstance;
                }
            }
        });

        mRockerView.setOnDistanceLevelListener(new RockerView.OnDistanceLevelListener() {
            @Override
            public void onDistanceLevel(int level) {
                DecimalFormat df = new DecimalFormat("#.00");
                lineSpeed= Float.parseFloat(df.format(maxSpeed*level/10));
                if (!isforward){
                    lineSpeed=-lineSpeed;
                }



            }
        });
    }


    Timer timer;
    TimerTask task;
    int a=0;

    /**
     * 定时器，每300毫秒执行一次
     */
    private void initTimer(){
        timer=new Timer();
        task=new TimerTask() {
            @Override

            public void run() {
               // Logger.e("线速度，角速度："+lineSpeed+";"+palstance);
                if (lineSpeed==0&&palstance==0){
                    a++;
                    if (a<=5){
                        Logger.e("----a:"+a);
                        sendSpeed(lineSpeed,palstance);
                    }
                }else {
                    a=0;
                    sendSpeed(lineSpeed,palstance);
                }

            }
        };
        timer.schedule(task,0,300);
    }


    /**
     * 发送线速度，角速度
     * @param lineSpeed
     * @param palstance
     */
    private void sendSpeed(final float lineSpeed, final float palstance){
        BaseCmd.reqCmdMove reqCmdMove=BaseCmd.reqCmdMove.newBuilder()
                                             .setLineSpeed(lineSpeed)
                                             .setAngulauSpeed(palstance)
                                             .build();
        BaseCmd.CommonHeader commonHeader=BaseCmd.CommonHeader.newBuilder()
                .setFromCltType(BaseCmd.eCltType.eLocalAndroidClient)
                .setToCltType(BaseCmd.eCltType.eLSMSlamNavigation)
                .addFlowDirection(BaseCmd.CommonHeader.eFlowDir.Forward)
                .build();
        tcpClient.sendData(commonHeader,reqCmdMove);

    }



    /**
     * 采集弹窗
     */
    private void collectDialog(){
        customDialog=new CustomDialog(this, null,2, null, new View.OnClickListener() {
            @Override
            public void onClick(View v) {
            collectPath= ByteString.copyFromUtf8(customDialog.editCollect.getText().toString().trim());
            if (!collectPath.isEmpty()){
                Logger.e("采集的路径名："+collectPath);
                BaseCmd.reqCmdStartActionMode reqCmdStartActionMode=BaseCmd.reqCmdStartActionMode.newBuilder()
                        .setMode(BaseCmd.eCmdActionMode.eRec)
                        .setRouteName(collectPath)
                        .build();
                tcpClient.sendData(CmdSchedule.commonHeader(BaseCmd.eCltType.eLSMSlamNavigation),reqCmdStartActionMode);
                customDialog.dismiss();
            }else {
                Toast.makeText(MainActivity.this,"请输入采集路径名称",Toast.LENGTH_SHORT).show();
            }
            }
        }, null, new View.OnClickListener() {
            @Override
            public void onClick(View v) {
               customDialog.dismiss();
            }
        });
        customDialog.show();
    }


    private void reqCmdIpcMethod(BaseCmd.eCmdIPCMode eCmdIPCMode ){
        tcpClient.sendData(CmdSchedule.commonHeader(BaseCmd.eCltType.eLSMSlamNavigation),CmdSchedule.cmdIPC(eCmdIPCMode));
    }




    /**
     * 删除指定文件夹下所有文件
     * @param file
     */
    public void deleteFile(File file){
        if (file.isDirectory()) {
            File[] files = file.listFiles();
            if (files!=null){
                for (int i = 0; i < files.length; i++) {
                    File f = files[i];
                    deleteFile(f);
                }
                file.delete();//如要保留文件夹，只删除文件，请注释这行
            }
        } else if (file.exists()) {
            file.delete();
        }
    }
    private void setPng(){
        RobotStatusEntity robotStatusEntity=RobotStatusEntity.getInstance();
        customImageHandle=CustomImageView.getmHandler();
        final String pngPath=Environment.getExternalStorageDirectory().getPath()+"/"+"机器人"+"/"+robotStatusEntity.getCurroute()+"/"+"bkPic.png";
        Logger.e("-------当前的路径名："+robotStatusEntity.getCurroute()+"/"+robotStatusEntity.getCurrpath());
        if (pngPath!=null){
            new Handler().postDelayed(new Runnable() {
                @Override
                public void run() {
                    if (customImageHandle!=null){
                        Message message=new Message();
                        message.arg1=4;
                        customImageHandle.sendMessage(message);
                    }

                }
            },2000);
        }else {
            Logger.e("-----pngPath为空");
        }
    }


    @Override
    public void onPause() {
        super.onPause();
        if (timer!=null){
            timer.cancel();
        }
        Logger.d("onPause");
    }

    @Override
    protected void onStart() {
        super.onStart();
        DecimalFormat decimalFormat = new DecimalFormat("#.0");
        maxSpeed= Double.parseDouble(decimalFormat.format(Integer.valueOf(sharedPreferences.getInt("process",3)).doubleValue()/10));
        Logger.e("onStart--"+maxSpeed);
    }



    @Override
    protected void onDestroy() {
        super.onDestroy();
        Logger.e("onDestroy");
        activityLife=false;
        mHandler=null;
        ActivityManager.getInstance().removeActivity(this);

    }

    public ActivityManager.IActivityManager iActivityManager = new ActivityManager.IActivityManager() {
        @Override
        public void isFinishApp(boolean isFinsh) throws IOException {
            if (isFinsh) {
                tcpClient.disConnect();
            }
        }
    };



    @Override
    public void onBackPressed() {
        super.onBackPressed();
        Logger.e("进行系统返回操作 onBackPressed()");
        startActivity(new Intent(MainActivity.this, NavigationActivity.class));
    }


    private void getCurrentTime(){
        //日期操作
        Calendar calendar=Calendar.getInstance();
        int hour=calendar.get(Calendar.HOUR_OF_DAY);
        int minute=calendar.get(Calendar.MINUTE);
        if (minute>=10){
            current_time.setText(hour+":"+minute);
        }else {
            current_time.setText(hour+":0"+minute);
        }
    }

    @Override
    public void tcpConnected() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(MainActivity.this,"连接成功！",Toast.LENGTH_SHORT).show();
            }
        });
    }

    @Override
    public void tcpDisConnected() {
        tcpClient.creatConnect(RobotIDEntity.getInstance().getIp(),RobotIDEntity.getInstance().getPort());
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(MainActivity.this,"连接被断开，正在重新连接...",Toast.LENGTH_SHORT).show();
            }
        });
    }

    @Override
    public void tcpServerFinish() {
        tcpClient.creatConnect(RobotIDEntity.getInstance().getIp(),RobotIDEntity.getInstance().getPort());
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(MainActivity.this,"服务器断开，请检查网络",Toast.LENGTH_SHORT).show();
            }
        });
    }

    @Override
    public void connectFailure() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                customDialog=new CustomDialog(MainActivity.this, "", 4,"", new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        tcpClient.creatConnect(RobotIDEntity.getInstance().getIp(),RobotIDEntity.getInstance().getPort());
                        customDialog.dismiss();
                    }
                }, "", new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        customDialog.dismiss();
                    }
                });
                customDialog.setCancelable(false);
                customDialog.show();
            }
        });
    }

    public class TimeThread extends Thread {

        @Override
        public void run() {
            while (activityLife){
                Message message=new Message();
                message.arg1=1;
                handler.sendMessage(message);
                try {
                    Thread.sleep(10000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }
}
