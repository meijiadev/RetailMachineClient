package com.example.dadao.dadaorobot.Login.view;

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
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.example.dadao.dadaorobot.Socket.ITcpClient;
import com.example.dadao.dadaorobot.entity.RobotIDEntity;
import com.example.dadao.dadaorobot.protocobuf.CmdSchedule;
import com.example.dadao.dadaorobot.Login.presenter.LoginPresenter;
import com.example.dadao.dadaorobot.activity.NavigationActivity;
import com.example.dadao.dadaorobot.R;
import com.example.dadao.dadaorobot.Socket.TcpClient;
import com.example.dadao.dadaorobot.Socket.UdpClient;
import com.example.dadao.dadaorobot.adapter.RobotIDRecyclerAdapter;
import com.example.dadao.dadaorobot.base.DaDaoBaseActivity;
import com.example.dadao.dadaorobot.protocobuf.dispatcher.ClientMessageDispatcher;
import com.example.dadao.dadaorobot.util.ActivityManager;
import com.example.dadao.dadaorobot.util.Logger;
import com.example.dadao.dadaorobot.util.NetWorkUtil;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import DDRCommProto.RemoteCmd;
import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;

import static com.example.dadao.dadaorobot.protocobuf.CmdSchedule.broadcastServerIP;
import static com.example.dadao.dadaorobot.protocobuf.CmdSchedule.broadcastServerPort;

/**
 * 登录
 */
public class LoginActivity extends DaDaoBaseActivity implements ILoginView,ITcpClient {

    @BindView(R.id.account)
    EditText account;
    @BindView(R.id.password)
    EditText password;
    @BindView(R.id.login_in)
    Button loginIn;
    @BindView(R.id.toggle_left)
    Button toggleLeft;
    @BindView(R.id.toggle_right)
    Button toggleRight;
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
    @BindView(R.id.layout_toggle)
    LinearLayout layout_toggle;

    @BindView(R.id.cancel_login)
    TextView cancelLogin;

    @BindView(R.id.recycle_robotId)
    RecyclerView recycleRobotId;
    @BindView(R.id.layout_robot_list)
    RelativeLayout layoutRobotList;
    @BindView(R.id.tv_cancel)
    TextView tvCancel;
    @BindView(R.id.tv_rb)
    TextView tv_rb;
    @BindView(R.id.tv_sever)
    TextView tv_sever;


    private boolean isLeftToggle = true;
    private RobotIDRecyclerAdapter robotIDRecyclerAdapter;
    public static int tcpPort = 0;
    private String accountName = "", passwordName = "";
    private LoginPresenter loginPresenter;
    private SharedPreferences sharedPreferences;
    private SharedPreferences.Editor editor;
    private Context context;
    public static Handler mHandler;
    public TcpClient tcpClient;

    public static List<String> robotList=new ArrayList<>();
    public static List<RemoteCmd.rspRemoteServerList.RemoteServer> serverList=new ArrayList<>();
    public List<String>sServerList=new ArrayList<>();
    public static List<RemoteCmd.rspRemoteLogin.LSEntity> loginList=new ArrayList<>();
    public List<String>sLoginList=new ArrayList<>();




    public final int mRequestCode = 10000;          //权限请求码
    public UdpClient udpClient;
    private int port=28888;
    private int listStatus=0x01;   // 0x01:局域网情况下选择机器人ID   0x02:广域网情况下选择大区   0x03: 服务器连接成功后选择机器人

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
                    if (!isLeftToggle){
                        robotIDRecyclerAdapter.setList(getServerList());
                        robotIDRecyclerAdapter.notifyDataSetChanged();
                      Logger.e("------>>>>:"+getServerList().size());
                   }
                    break;
                case 2:
                    if (isLeftToggle){
                        robotIDRecyclerAdapter.setList(getList());
                        robotIDRecyclerAdapter.notifyDataSetChanged();
                        Logger.e("------>>>>:"+getList().size());
                    }
                    break;
                case 3:
                    listStatus=0x03;
                    robotIDRecyclerAdapter.setList(getLoginList());
                    robotIDRecyclerAdapter.notifyDataSetChanged();
                    layout_account.setVisibility(View.GONE);
                    layout_password.setVisibility(View.GONE);
                    layout_toggle.setVisibility(View.INVISIBLE);
                    cancelLogin.setVisibility(View.VISIBLE);
                    iv_robot.setImageResource(R.mipmap.robot_id);
                    tv_sever.setVisibility(View.VISIBLE);
                    robot_id.setText("选择机器人");
                    tv_rb.setText("机器人ID");
                    loginIn.setText("登 陆");
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
        ActivityManager.getInstance().addActivity(this);
        ActivityManager.getInstance().setIActivityManager(iActivityManager);
        mHandler=handler;
        loginPresenter = new LoginPresenter(this, this);
        sharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        editor = sharedPreferences.edit();
        account.setText(sharedPreferences.getString("account", ""));
        password.setText(sharedPreferences.getString("password", ""));
        //robot_id.setText(sharedPreferences.getString("ip","机器人ID"));
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



    @Override
    public void LoginResult(boolean isLogin) {
        if (isLogin) {
            UdpClient.getInstance(context,ClientMessageDispatcher.getInstance()).close();
            editor.putString("account", accountName);
            editor.putString("password", passwordName);
            editor.commit();
            startActivity(new Intent(LoginActivity.this, NavigationActivity.class));
            finish();
        }else {

        }

    }

    private int num;
    @OnClick({R.id.login_in, R.id.toggle_left, R.id.toggle_right, R.id.layout_robot,R.id.tv_cancel,R.id.cancel_login})
    public void onViewClicked(View view) {
        switch (view.getId()) {
            case R.id.login_in:
                if (listStatus==0x03){
                    if (!robot_id.getText().equals("选择机器人")){
                        RemoteCmd.reqSelectLS reqSelectLS=RemoteCmd.reqSelectLS.newBuilder()
                                .setUdid(loginList.get(num).getUdid())
                                .build();
                        tcpClient.sendData(null,reqSelectLS);
                        Logger.e("广域网选择服务器登录");
                    }else {
                        Toast.makeText(this,"请选择机器人",Toast.LENGTH_SHORT).show();
                    }
                }else {
                    accountName = account.getText().toString().trim();
                    passwordName = password.getText().toString().trim();
                    if (tcpClient.isHeartBeat) {
                        loginPresenter.doLogin(accountName, passwordName,isLeftToggle);              //
                        Logger.e("登录");
                    } else {
                        Toast.makeText(LoginActivity.this, "服务器尚未连接，无法登陆", Toast.LENGTH_SHORT).show();
                    }
                }
                break;
            case R.id.toggle_left:
                receiveBroadcast();
                isLeftToggle = true;
                listStatus=0x01;
                robotIDRecyclerAdapter.setList(getList());
                robotIDRecyclerAdapter.notifyDataSetChanged();
                toggleLeft.setBackgroundResource(R.drawable.toggle_button_left_blue);
                toggleRight.setBackgroundResource(R.drawable.toggle_button_right_gray);
                layoutRobot.setVisibility(View.VISIBLE);
                iv_robot.setImageResource(R.mipmap.robot_id);
                robot_id.setText("机器人ID");
                tv_rb.setText("机器人ID");
                loginIn.setText("登 陆");
                break;
            case R.id.toggle_right:
                UdpClient.getInstance(context,ClientMessageDispatcher.getInstance()).close();
                robotIDRecyclerAdapter.setList(getServerList());
                robotIDRecyclerAdapter.notifyDataSetChanged();
                iv_robot.setImageResource(R.mipmap.server);
                toggleLeft.setBackgroundResource(R.drawable.toggle_button_left_gray);
                toggleRight.setBackgroundResource(R.drawable.toggle_button_right_blue);
                listStatus=0x02;
                isLeftToggle = false;
                //layoutRobot.setVisibility(View.GONE);
                robot_id.setText("请选择大区");
                tv_rb.setText("服务大区");
                loginIn.setText("下一步");
                try {
                    tcpClient.disConnect();     //在切换服务器时，要先关闭当前服务器再连接新的服务器
                } catch (IOException e) {
                    e.printStackTrace();
                }
                Logger.e("----连接");
                tcpClient.creatConnect(broadcastServerIP,broadcastServerPort);
                break;
            case R.id.layout_robot:
                if (isLeftToggle){
                    layoutRobotList.setVisibility(View.VISIBLE);
                    loginIn.setVisibility(View.INVISIBLE);
                }else {
                    if (tcpClient.isHeartBeat){
                        Logger.e("-----本机IP:"+NetWorkUtil.getIPAddress(this));
                        tcpClient.sendData(null, CmdSchedule.remoteServerList(NetWorkUtil.getIPAddress(this)));
                    }
                    layoutRobotList.setVisibility(View.VISIBLE);
                    loginIn.setVisibility(View.GONE);
                }
                break;

            case R.id.tv_cancel:
                layoutRobotList.setVisibility(View.GONE);
                loginIn.setVisibility(View.VISIBLE);
                break;
            case R.id.cancel_login:
                listStatus=0x02;
                layout_toggle.setVisibility(View.VISIBLE);
                layout_account.setVisibility(View.VISIBLE);
                layout_password.setVisibility(View.VISIBLE);
                cancelLogin.setVisibility(View.GONE);
                tv_sever.setVisibility(View.GONE);
                iv_robot.setImageResource(R.mipmap.server);
                robot_id.setText("请选择大区");
                tv_rb.setText("服务大区");
                robotIDRecyclerAdapter.setList(getServerList());
                robotIDRecyclerAdapter.notifyDataSetChanged();
                break;
        }
    }

    private void onItemClick(){
        robotIDRecyclerAdapter.setOtemClickListener(new RobotIDRecyclerAdapter.OnItemClickListener() {
            @Override
            public void onItemClick(RobotIDRecyclerAdapter.ViewHolder view, int position) {
                Logger.e("点击子项");
                switch (listStatus){
                    case 0x01:
                        String ip=robotList.get(position);
                        try {
                            tcpClient.disConnect();     //在切换服务器时，要先关闭当前服务器再连接新的服务器
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                        Logger.e("----连接");
                        tcpClient.creatConnect(ip,tcpPort);
                        robot_id.setText(ip);
                        layoutRobotList.setVisibility(View.GONE);
                        loginIn.setVisibility(View.VISIBLE);
                        RobotIDEntity.getInstance().setIp(ip);
                        RobotIDEntity.getInstance().setPort(tcpPort);
                        break;
                    case 0x02:
                        try {
                            tcpClient.disConnect();     //在切换服务器时，要先关闭当前服务器再连接新的服务器
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                        tcpClient.creatConnect(serverList.get(position).getIp(),serverList.get(position).getPort());
                        RobotIDEntity.getInstance().setIp(serverList.get(position).getIp());
                        RobotIDEntity.getInstance().setPort(serverList.get(position).getPort());
                        robot_id.setText(sServerList.get(position));
                        layoutRobotList.setVisibility(View.GONE);
                        loginIn.setVisibility(View.VISIBLE);
                        break;
                    case 0x03:
                        robot_id.setText(sLoginList.get(position));
                        layoutRobotList.setVisibility(View.GONE);
                        loginIn.setVisibility(View.VISIBLE);
                        num=position;
                        break;
                }


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

    public static void addServerList(RemoteCmd.rspRemoteServerList.RemoteServer remoteServer){
        if (!serverList.contains(remoteServer)){
            serverList.add(remoteServer);
        }
    }

    public static void addLoginList(List<RemoteCmd.rspRemoteLogin.LSEntity> list){
        loginList=list;
    }

    public List<String> getList(){
        return robotList;
    }

    public List<String> getServerList(){
        for (int i=0;i<serverList.size();i++){
            if (!sServerList.contains(serverList.get(i).getName())){
                sServerList.add(serverList.get(i).getName());
            }
        }
        return sServerList;
    }



    public List<String> getLoginList(){
        for (int i=0;i<loginList.size();i++){
            if (!sLoginList.contains(loginList.get(i).getName())){
                sLoginList.add(loginList.get(i).getName());
            }
        }
        return sLoginList;
    }




    @Override
    protected void onDestroy() {
        super.onDestroy();
        ActivityManager.getInstance().removeActivity(this);
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

    /**
     * 监听app是否即将退出的回调函数
     */
    private ActivityManager.IActivityManager iActivityManager = new ActivityManager.IActivityManager() {
        @Override
        public void isFinishApp(boolean isFinsh) throws IOException {
            if (isFinsh) {
                tcpClient.disConnect();

            }
        }
    };

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

    }

    @Override
    public void tcpDisConnected() {

    }

    @Override
    public void tcpServerFinish() {

    }

    @Override
    public void connectFailure() {

    }
}
