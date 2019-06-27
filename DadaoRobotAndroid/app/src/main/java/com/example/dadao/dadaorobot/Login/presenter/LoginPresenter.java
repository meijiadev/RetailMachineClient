package com.example.dadao.dadaorobot.Login.presenter;

import android.content.Context;
import android.widget.Toast;

import com.example.dadao.dadaorobot.protocobuf.CmdSchedule;
import com.example.dadao.dadaorobot.Login.view.ILoginView;
import com.example.dadao.dadaorobot.Socket.TcpClient;
import com.example.dadao.dadaorobot.protocobuf.dispatcher.ClientMessageDispatcher;
import com.example.dadao.dadaorobot.util.Logger;

/**
 *presenter层的实体类
 */
public class LoginPresenter implements ILoginPresenter {
    public static ILoginView mILoginView;
    private Context mContext;
    private TcpClient tcpClient;

    public LoginPresenter(ILoginView iLoginView,Context context) {
        mContext=context;
        this.mILoginView=iLoginView;
        tcpClient=TcpClient.getInstance(context, ClientMessageDispatcher.getInstance());
    }

    @Override
    public void doLogin(final String account, final String password,boolean isLocalNetwork) {
       if (account.equals("")&password.equals("")){
           Toast.makeText(mContext,"用户名和密码不能为空",Toast.LENGTH_SHORT).show();
       }else {
           if (isLocalNetwork){
               tcpClient.sendData(null, CmdSchedule.localLogin(account,password));
               Logger.e("登陆局域网服务器");
           }else {
               tcpClient.sendData(null,CmdSchedule.remoteLogin(account,password));
               Logger.e("登陆远程服务器");
           }
           Logger.e("发送登陆信息");
       }
    }

    public static void toLogined(boolean isLoging){
        mILoginView.LoginResult(isLoging);
    }


}
