package ym.example.com.ymandroidclient;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.preference.PreferenceManager;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;

import ym.example.com.ymandroidclient.adapter.RobotIDRecyclerAdapter;
import ym.example.com.ymandroidclient.entity.RobotIDEntity;
import ym.example.com.ymandroidclient.protobuf.CmdSchedule;
import ym.example.com.ymandroidclient.protobuf.dispatcher.ClientMessageDispatcher;
import ym.example.com.ymandroidclient.socket.ITcpClient;
import ym.example.com.ymandroidclient.socket.TcpClient;
import ym.example.com.ymandroidclient.socket.UdpClient;
import ym.example.com.ymandroidclient.utility.Logger;

public class LoginActivity extends DaDaoBaseActivity implements ITcpClient {
    @BindView(R.id.account)
    EditText account;
    @BindView(R.id.password)
    EditText password;
    @BindView(R.id.login_in)
    Button loginIn;
    @BindView(R.id.layout_robot)
    RelativeLayout layoutRobot;
    @BindView(R.id.robot_id)
    TextView robot_id;
    @BindView(R.id.iv_robot)
    ImageView iv_robot;
    @BindView(R.id.layout_account)
    RelativeLayout layout_account;
    @BindView(R.id.layout_password)
    RelativeLayout layout_password;


    @BindView(R.id.recycle_robotId)
    RecyclerView recycleRobotId;
    @BindView(R.id.layout_robot_list)
    RelativeLayout layoutRobotList;
    @BindView(R.id.tv_cancel)
    TextView tvCancel;
    @BindView(R.id.tv_rb)
    TextView tv_rb;



    private boolean isLeftToggle = true;
    private RobotIDRecyclerAdapter robotIDRecyclerAdapter;
    public static int tcpPort = 0;
    private String accountName = "", passwordName = "";
    private SharedPreferences sharedPreferences;
    private SharedPreferences.Editor editor;
    private Context context;
    public static Handler mHandler;
    public TcpClient tcpClient;

    public static List<String> robotList=new ArrayList<>();



    public final int mRequestCode = 10000;          //权限请求码
    public UdpClient udpClient;
    private int port=28888;

    //需要请求的权限
    public String[] permissions = new String[]{
            Manifest.permission.RECORD_AUDIO,
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.READ_EXTERNAL_STORAGE
    };

    public Handler handler=new Handler(){
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.arg1){
                case 1:
                    UdpClient.getInstance(context,ClientMessageDispatcher.getInstance()).close();
                    editor.putString("account", accountName);
                    editor.putString("password", passwordName);
                    editor.commit();
                    startActivity(new Intent(LoginActivity.this, MainActivity.class));
                    finish();
                    break;
                case 2:
                    //Logger.e("------>>>>:"+getList().size());
                    robotIDRecyclerAdapter.setList(getList());
                    robotIDRecyclerAdapter.notifyDataSetChanged();
                    break;
            }
        }
    };

    public static Handler getInstance(){
        return mHandler;
    }



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);
        ButterKnife.bind(this);
        receiveBroadcast();
        mHandler=handler;
        robotList.clear();
        sharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        editor = sharedPreferences.edit();
        account.setText(sharedPreferences.getString("account", ""));
        password.setText(sharedPreferences.getString("password", ""));
        isFirstRun();
        context = this;
        tcpClient=TcpClient.getInstance(context,ClientMessageDispatcher.getInstance());
        tcpClient.setiTcpClient(this);
        initRecycler();
    }

    /**
     *初始化列表
     */
    public void initRecycler(){
        LinearLayoutManager linearLayoutManager=new LinearLayoutManager(this);
        linearLayoutManager.setOrientation(LinearLayoutManager.VERTICAL);
        robotIDRecyclerAdapter=new RobotIDRecyclerAdapter(context);
        robotIDRecyclerAdapter.setViewType(1);
        robotIDRecyclerAdapter.setList(getList());
        recycleRobotId.setAdapter(robotIDRecyclerAdapter);
        recycleRobotId.setLayoutManager(linearLayoutManager);
        onItemClick();
    }




    @OnClick({R.id.login_in,  R.id.layout_robot,R.id.tv_cancel})
    public void onViewClicked(View view) {
        switch (view.getId()) {
            case R.id.login_in:
                    accountName = account.getText().toString().trim();
                    passwordName = password.getText().toString().trim();
                    if (tcpClient.isHeartBeat) {
                       doLogin(accountName,passwordName);
                        Logger.e("登录");
                    } else {
                        Toast.makeText(LoginActivity.this, "服务器尚未连接，无法登陆", Toast.LENGTH_SHORT).show();
                    }

                break;
            case R.id.layout_robot:
                layoutRobotList.setVisibility(View.VISIBLE);
                loginIn.setVisibility(View.INVISIBLE);
                break;
            case R.id.tv_cancel:
                layoutRobotList.setVisibility(View.GONE);
                loginIn.setVisibility(View.VISIBLE);
                break;

        }
    }

    private void doLogin(String accountName,String passwordName){
        if (accountName.equals("")&passwordName.equals("")){
            Toast.makeText(LoginActivity.this,"用户名和密码不能为空",Toast.LENGTH_SHORT).show();
        }else {
            tcpClient.sendData(null, CmdSchedule.localLogin(accountName,passwordName));
            Logger.e("登陆局域网服务器");

        }
    }

    private void onItemClick(){
        robotIDRecyclerAdapter.setOtemClickListener(new RobotIDRecyclerAdapter.OnItemClickListener() {
            @Override
            public void onItemClick(RobotIDRecyclerAdapter.ViewHolder view, int position) {
                Logger.e("点击子项");
                String ip=robotList.get(position);
                try {
                    tcpClient.disConnect();     //在切换服务器时，要先关闭当前服务器再连接新的服务器
                } catch (IOException e) {
                    e.printStackTrace();
                }
                Logger.e("----连接");
                tcpClient.creatConnect(ip,tcpPort);
                RobotIDEntity.getInstance().setIp(ip);
                RobotIDEntity.getInstance().setPort(tcpPort);
                robot_id.setText(ip);
                layoutRobotList.setVisibility(View.GONE);
                loginIn.setVisibility(View.VISIBLE);

            }
        });
    }


    /**
     * 当udp广播接收到ip就往里面添加
     * @param ip
     */
    public static void addList(String ip){
        if (!robotList.contains(ip)){
            robotList.add(ip);
        }

    }



    public List<String> getList(){
        return robotList;
    }





    @Override
    protected void onDestroy() {
        super.onDestroy();
        Logger.e("----退出登录界面");

    }
    @Override
    public void onBackPressed() {
        super.onBackPressed();
        Logger.e("进行系统返回操作 onBackPressed()");
        try {
            tcpClient.disConnect();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }


    /**
     * 判断app是否是第一次运行，第一次运行要请求相关权限
     */
    private void isFirstRun() {
        boolean isFirst = sharedPreferences.getBoolean("isFirstRun", true);
        if (isFirst) {
            Logger.e("app第一次运行");
            editor.putBoolean("isFirstRun", false);
            editor.commit();
            requestPermission(this);
        } else {

        }
    }

    public void requestPermission(Activity activity) {
        for (int i = 0; i < permissions.length; i++) {
            if (ContextCompat.checkSelfPermission(activity, permissions[i]) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(activity, permissions, mRequestCode);
            }
        }
    }



    private void receiveBroadcast(){
        udpClient= UdpClient.getInstance(this,ClientMessageDispatcher.getInstance());
        try {
            udpClient.connect(port);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }


    @Override
    public void tcpConnected() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(LoginActivity.this,"连接成功！",Toast.LENGTH_SHORT).show();
            }
        });
    }

    @Override
    public void tcpDisConnected() {
        tcpClient.creatConnect(RobotIDEntity.getInstance().getIp(),RobotIDEntity.getInstance().getPort());
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(LoginActivity.this,"连接被断开，正在重新连接...",Toast.LENGTH_SHORT).show();
            }
        });
    }

    @Override
    public void tcpServerFinish() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(LoginActivity.this,"服务器断开，请检查网络",Toast.LENGTH_SHORT).show();
            }
        });
    }

    @Override
    public void connectFailure() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(LoginActivity.this,"连接失败，请选择机器人ID重新连接！",Toast.LENGTH_SHORT).show();
            }
        });
    }
}
