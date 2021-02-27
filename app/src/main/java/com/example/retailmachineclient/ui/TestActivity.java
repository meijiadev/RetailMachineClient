package com.example.retailmachineclient.ui;

import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.example.retailmachineclient.R;
import com.example.retailmachineclient.base.BaseActivity;
import com.example.retailmachineclient.mcuSdk.DataProtocol;
import com.example.retailmachineclient.mcuSdk.MCU;
import com.example.retailmachineclient.mcuSdk.SerialPortUtil;
import com.example.retailmachineclient.util.LogcatHelper;
import com.example.retailmachineclient.util.Logger;
import com.example.retailmachineclient.util.Utils;
import com.kongqw.serialportlibrary.SerialPortManager;
import com.kongqw.serialportlibrary.listener.OnOpenSerialPortListener;
import com.kongqw.serialportlibrary.listener.OnSerialPortDataListener;

import java.io.File;

import butterknife.BindView;
import butterknife.OnClick;

public class TestActivity extends BaseActivity {

    @BindView(R.id.tvPushUp)
    Button tvPushUp;
    @BindView(R.id.tvResult)
    TextView tvResult;
    @BindView(R.id.etNumber)
    EditText etNumber;;
    private String result="";
    private SerialPortManager mSerialPortManager;
    private SerialPortUtil serialPortUtil;
    @Override
    protected int getLayoutId() {
        return R.layout.activity_test;
    }




    @Override
    protected void initView() {
        setStatusBarEnabled(true);
    }

    @Override
    protected void initData() {
        //mcuSDK=McuSDK.initSDK(BaseApplication.context);
        /*serialPortUtil=new SerialPortUtil();
        serialPortUtil.open(Mcu.PortMCU.MCU1.getPath(),Mcu.PORT_RATE);*/
        mSerialPortManager=new SerialPortManager();
        mSerialPortManager.setOnOpenSerialPortListener(new OnOpenSerialPortListener() {
            @Override
            public void onSuccess(File file) {
                Logger.e("串口打开成功！"+file.getPath());
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        result=result+"\n"+"串口打开成功";
                        tvResult.setText(result);
                    }
                });
            }

            @Override
            public void onFail(File file, Status status) {
                Logger.e("串口打开失败！"+file.getPath());
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        result=result+"\n"+"串口打开失败";
                        tvResult.setText(result);
                    }
                });
            }
        });

        mSerialPortManager.setOnSerialPortDataListener(new OnSerialPortDataListener() {
            @Override
            public void onDataReceived(byte[] bytes) {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        result=result+"\n"+"接受数据:"+Utils.byteBufferToHexString(bytes);
                        tvResult.setText(result);
                    }
                });


            }

            @Override
            public void onDataSent(byte[] bytes) {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        byte[] check=Utils.getBytes(bytes,18,2);
                        result=result+"\n"+"发送数据:"+Utils.byteBufferToHexString(bytes)+"-----:"+Utils.byteBufferToHexString(check);
                        tvResult.setText(result);
                    }
                });

            }
        });
        mSerialPortManager.openSerialPort(new File(MCU.PortMCU.MCU1.getPath()), MCU.PORT_RATE);
    }

    @Override
    protected void onRestart() {
        super.onRestart();
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onStart() {
        super.onStart();
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    protected void onStop() {
        super.onStop();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        LogcatHelper.getInstance(context).stop();
    }


    @OnClick({R.id.tvPushUp,R.id.btPullDown,R.id.btcx,R.id.btCXLifter,R.id.btStart,R.id.btClear})
    public void onViewClicked(View view) {
        switch(view.getId()){
            case R.id.tvPushUp:
                //serialPortUtil.sendData(DataProtocol.packSendData(Mcu.PortMCU.MCU1,(byte)0x14,(byte)0x01));
               mSerialPortManager.sendBytes(DataProtocol.packSendData(MCU.PortMCU.MCU1,(byte)0x14,(byte)0x01));
                break;
            case R.id.btPullDown:
                //serialPortUtil.sendData(DataProtocol.packSendData(Mcu.PortMCU.MCU1,(byte)0x14,(byte)0x01));
                mSerialPortManager.sendBytes(DataProtocol.packSendData(MCU.PortMCU.MCU1,(byte)0x14,(byte)0x00));
                break;
            case R.id.btcx:
                mSerialPortManager.sendBytes(DataProtocol.packSendData(MCU.PortMCU.MCU1,(byte)0x15,(byte)0x00));
                break;
            case R.id.btCXLifter:
                mSerialPortManager.sendBytes(DataProtocol.packSendData(MCU.PortMCU.MCU1,(byte)0x13,(byte)0x00));
                break;
            case R.id.btStart:
                String sEt=etNumber.getText().toString();
                int number = Integer.parseInt(sEt);
                byte data;
                switch (number){
                    case 0:
                        data=0x00;
                        break;
                    case 1:
                        data=0x01;
                        break;
                    case 2:
                        data=0x02;
                        break;
                    case 3:
                        data=0x03;
                        break;
                    case 4:
                        data=0x04;
                        break;
                    case 5:
                        data=0x05;
                        break;
                    case 6:
                        data=0x06;
                        break;
                    case 7:
                        data=0x07;
                        break;
                    case 8:
                        data=0x08;
                        break;
                    default:
                        data=0x00;
                        break;

                }
                mSerialPortManager.sendBytes(DataProtocol.packSendData(MCU.PortMCU.MCU1,(byte)0x12,(byte)data));
                break;
            case R.id.btClear:
                result="";
                break;

        }


    }
}