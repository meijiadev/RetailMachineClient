package com.example.dadao.dadaorobot.activity;

import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Build;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.annotation.RequiresApi;
import android.view.View;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import com.example.dadao.dadaorobot.R;
import com.example.dadao.dadaorobot.Socket.TcpClient;
import com.example.dadao.dadaorobot.base.DaDaoBaseActivity;
import com.example.dadao.dadaorobot.entity.RobotStatusEntity;
import com.example.dadao.dadaorobot.protocobuf.dispatcher.ClientMessageDispatcher;
import com.example.dadao.dadaorobot.util.Logger;
import com.example.dadao.dadaorobot.util.widget.CustomImageView;
import com.google.protobuf.ByteString;


import java.text.DecimalFormat;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;
import DDRCommProto.BaseCmd;
public class SettingActivity extends DaDaoBaseActivity {
    @BindView(R.id.iv_back)
    ImageView iv_back;
    @BindView(R.id.speed_seekbar)
    SeekBar speed_SeekBar;
    @BindView(R.id.tv_speed_setting)
    TextView tv_speed_setting;
    @BindView(R.id.saved)
    TextView saved;

    @BindView(R.id.layout_current_path)
    RelativeLayout current_path;
    @BindView(R.id.layout__path_name)
    RelativeLayout path_name;

    @BindView(R.id.tv_current_path)
    TextView tv_current_path;
    @BindView(R.id.tv_path_name)
    TextView tv_path_name;


    private TcpClient tcpClient;
    private String currentPath;

    private SharedPreferences sharedPreferences;
    private SharedPreferences.Editor editor;

    private int mProcess;  //进度条
    private String dirName="";  //文件夹名字
    private String fileName=""; //文件名字
    Intent intent;

    @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_setting);
        ButterKnife.bind(this);
        sharedPreferences= PreferenceManager.getDefaultSharedPreferences(this);
        editor=sharedPreferences.edit();
        mProcess=sharedPreferences.getInt("process",3);
        dirName=sharedPreferences.getString("dirName","");
        fileName=sharedPreferences.getString("fileName","");
        tcpClient=TcpClient.getInstance(this, ClientMessageDispatcher.getInstance());
        onSeekBarChanger();
        intent=getIntent();
        int set=intent.getIntExtra("MainToSet",1);
        if (set==0){
            RobotStatusEntity robotStatusEntity=RobotStatusEntity.getInstance();
            dirName=robotStatusEntity.getCurroute();
            fileName=robotStatusEntity.getCurrpath();
            editor.putString("dirName",dirName);
            editor.putString("fileName",fileName);
            editor.commit();
            tv_current_path.setText(dirName);
            tv_path_name.setText(fileName);
        }else {
            tv_current_path.setText(dirName);
            tv_path_name.setText(fileName);
        }
        initCustomImageView();
    }

    public void initCustomImageView(){
        final String txtPath,matPath;
        boolean havePng=intent.getBooleanExtra("isHavePng",false);
        if (havePng){
            String pngpath=intent.getStringExtra("pngPath");
            txtPath= Environment.getExternalStorageDirectory().getPath()+"/"+"机器人"+"/"+dirName+"/"+fileName;
            matPath=Environment.getExternalStorageDirectory().getPath()+"/"+"机器人"+"/"+dirName+"/"+"affine_mat.txt";
            Logger.e("-----png的路径："+pngpath);
            Message message=new Message();
            message.obj=pngpath;
            message.arg1=1;
            CustomImageView.getmHandler().sendMessage(message);
            new Handler().postDelayed(new Runnable() {
                @Override
                public void run() {
                    Message message=new Message();
                    Bundle bundle=new Bundle();
                    bundle.putString("txtPath",txtPath);
                    bundle.putString("matPath",matPath);
                    message.setData(bundle);
                    message.arg1=0;
                    CustomImageView.getmHandler().sendMessage(message);
                }
            },1000);
        }
    }




    @OnClick({R.id.iv_back,R.id.layout_current_path,R.id.layout__path_name,R.id.saved})
    public void onViewClicked(View view){
        switch (view.getId()){
            case R.id.iv_back:
                Logger.e("点击返回");
                finish();
                break;
            case R.id.layout_current_path:
                Intent intent=new Intent(SettingActivity.this,SelectFileActivity.class);
                intent.putExtra("isFile",false);
                startActivity(intent);
                finish();
                break;
            case R.id.layout__path_name:
                if (!tv_current_path.getText().equals("")){
                    Intent intent1=new Intent(SettingActivity.this,SelectFileActivity.class);
                    intent1.putExtra("isFile",true);
                    intent1.putExtra("dirName",tv_current_path.getText());
                    startActivity(intent1);
                    finish();
                }else {
                    Toast.makeText(SettingActivity.this,"当前路径为空，请先选择路径",Toast.LENGTH_SHORT).show();
                }
                break;
            case R.id.saved:
                saveSetting();
                finish();
                break;
        }

    }

    private void saveSetting(){
        if (!tv_path_name.getText().equals("")){
            BaseCmd.reqCmdSetWorkPath reqCmdSetWorkPath= BaseCmd.reqCmdSetWorkPath.newBuilder()
                    .setRouteName(ByteString.copyFromUtf8(dirName))
                    .setPathName(fileName)
                    .setSpeed((float) maxSpeed)
                    .setWalkStyle("0")
                    .build();
            BaseCmd.CommonHeader commonHeader=BaseCmd.CommonHeader.newBuilder()
                    .setFromCltType(BaseCmd.eCltType.eLocalAndroidClient)
                    .setToCltType(BaseCmd.eCltType.eLSMSlamNavigation)
                    .addFlowDirection(BaseCmd.CommonHeader.eFlowDir.Forward)
                    .build();
            tcpClient.sendData(commonHeader,reqCmdSetWorkPath);
        }
            Logger.e("-----process:"+mProcess);
            editor.putInt("process",mProcess);
            editor.commit();
    }
    double maxSpeed;
    private void onSeekBarChanger(){
        final DecimalFormat decimalFormat = new DecimalFormat("#.0");
        maxSpeed= Double.parseDouble(decimalFormat.format(Integer.valueOf(mProcess).doubleValue()/10));
        tv_speed_setting.setText(maxSpeed+" m/s");
        speed_SeekBar.setProgress(mProcess);
        speed_SeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                Logger.e("当前速度："+progress);
                mProcess=progress;
                maxSpeed= Double.parseDouble(decimalFormat.format(Integer.valueOf(progress).doubleValue()/10));
                tv_speed_setting.setText(maxSpeed+" m/s");

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }






}
