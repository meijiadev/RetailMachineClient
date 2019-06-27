package com.example.dadao.dadaorobot.fragment;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v4.app.Fragment;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.example.dadao.dadaorobot.R;
import com.example.dadao.dadaorobot.Socket.TcpClient;
import com.example.dadao.dadaorobot.activity.MainActivity;
import com.example.dadao.dadaorobot.adapter.RobotIDRecyclerAdapter;
import com.example.dadao.dadaorobot.entity.RobotStatusEntity;
import com.example.dadao.dadaorobot.entity.XyEntity;
import com.example.dadao.dadaorobot.protocobuf.CmdSchedule;
import com.example.dadao.dadaorobot.protocobuf.dispatcher.ClientMessageDispatcher;
import com.example.dadao.dadaorobot.util.Logger;
import com.example.dadao.dadaorobot.util.widget.CustomDialog;
import com.example.dadao.dadaorobot.util.widget.CustomImageView;
import com.google.protobuf.ByteString;
import java.util.ArrayList;
import java.util.List;

import DDRCommProto.BaseCmd;
import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;
import butterknife.Unbinder;


public class LaserMapFragment extends Fragment {
    @BindView(R.id.bt_edit_map)
    Button bt_edit_map;
    @BindView(R.id.bt_add_point)
    Button bt_add_point;
    @BindView(R.id.bt_save)
    Button bt_save;
    @BindView(R.id.bt_cancel)
    Button bt_cancel;
    @BindView(R.id.point_recycle)
    RecyclerView point_recycle;
    @BindView(R.id.laser_image)
    CustomImageView customImageView;
    @BindView(R.id.map_name)
    TextView mapName;

    private static Handler mhandler;
    private Handler customHandler;
    private boolean isAddPoint=false;
    private boolean isEditMap=false;
    private Unbinder unbinder;
    private RobotIDRecyclerAdapter robotIDRecyclerAdapter;
    private Context context;
    private TcpClient tcpClient;
    RobotStatusEntity robotStatusEntity;


    public static Handler getHandler() {
        return mhandler;
    }

    public Handler handler=new Handler(){
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.arg1){
                case 0:
                    robotIDRecyclerAdapter.setPointList(customImageView.getPointList());
                    robotIDRecyclerAdapter.notifyDataSetChanged();
                    break;
            }
        }
    };


    public LaserMapFragment() {
        // Required empty public constructor


    }


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

    }
    View view;
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        view=inflater.inflate(R.layout.fragment_laser_map,container,false);
        unbinder=ButterKnife.bind(this,view);
        tcpClient=TcpClient.getInstance(getActivity(),ClientMessageDispatcher.getInstance());
        mhandler=handler;
        initView(view);
        return view;
    }

    private void initView(View view){
        RelativeLayout fragment=view.findViewById(R.id.fragment);
        robotStatusEntity=RobotStatusEntity.getInstance();
        mapName.setText(robotStatusEntity.getCurroute()+"/"+robotStatusEntity.getCurrpath());
        fragment.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Message message=new Message();
                message.arg1=0;
                MainActivity.getHandler().sendMessage(message);
            }
        });
        customHandler=CustomImageView.getmHandler();
        initRecycler();
    }

    /**
     *初始化列表
     */
    public void initRecycler(){
        LinearLayoutManager linearLayoutManager=new LinearLayoutManager(getActivity());
        linearLayoutManager.setOrientation(LinearLayoutManager.VERTICAL);
        robotIDRecyclerAdapter=new RobotIDRecyclerAdapter(getActivity());
        robotIDRecyclerAdapter.setViewType(3);
        robotIDRecyclerAdapter.setPointList(customImageView.getPointList());
        point_recycle.setAdapter(robotIDRecyclerAdapter);
        point_recycle.setLayoutManager(linearLayoutManager);
        onItemClick();
    }

    CustomDialog customDialog;
    String editType="";
    private void onItemClick(){
        robotIDRecyclerAdapter.setOtemClickListener(new RobotIDRecyclerAdapter.OnItemClickListener() {
            @Override
            public void onItemClick(RobotIDRecyclerAdapter.ViewHolder view, final int position) {
                 customDialog=new CustomDialog(getActivity(), 5, new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                    customImageView.getPointList().get(position).setePathPointType(0);
                    EditText editText=customDialog.findViewById(R.id.edit_type);
                    editType=editText.getText().toString();
                        if (editType==""){
                            editType="无";
                        }
                    customImageView.getPointList().get(position).setPointInfo(editType);
                    customDialog.dismiss();
                    Logger.e("--------"+editType);
                    }
                }, new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        customImageView.getPointList().get(position).setePathPointType(1);
                        EditText editText=customDialog.findViewById(R.id.edit_type);
                        editType=editText.getText().toString();
                        if (editType==""){
                            editType="无";
                        }
                        customImageView.getPointList().get(position).setPointInfo(editType);
                        customDialog.dismiss();
                    }
                }, new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        customImageView.getPointList().get(position).setePathPointType(2);
                        EditText editText=customDialog.findViewById(R.id.edit_type);
                        editType=editText.getText().toString();
                        if (editType==""){
                            editType="无";
                        }
                        customImageView.getPointList().get(position).setPointInfo(editType);
                        Logger.e("--------"+editType);
                        customDialog.dismiss();
                    }
                }, new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        customImageView.getPointList().get(position).setePathPointType(3);
                        EditText editText=customDialog.findViewById(R.id.edit_type);
                        editType=editText.getText().toString();
                        if (editType==""){
                            editType="无";
                        }
                        customImageView.getPointList().get(position).setPointInfo(editType);
                        customDialog.dismiss();
                    }
                }, new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        Message message=new Message();
                        message.arg1=7;
                        message.arg2=position;
                        customHandler.sendMessage(message);
                        customDialog.dismiss();
                    }
                });
                customDialog.setCancelable(true);
                customDialog.show();
            }
        });
    }

    @OnClick({R.id.bt_edit_map,R.id.bt_add_point,R.id.bt_save,R.id.bt_cancel})
    public void onViewClicked(View view) {
        switch (view.getId()){
            case R.id.bt_edit_map:
                if (isEditMap){
                    isEditMap=false;
                    bt_edit_map.setBackground(getResources().getDrawable(R.mipmap.edit_default));
                    bt_add_point.setVisibility(View.INVISIBLE);
                    bt_save.setVisibility(View.INVISIBLE);
                    bt_cancel.setVisibility(View.INVISIBLE);
                }else{
                    isEditMap=true;
                    bt_edit_map.setBackground(getResources().getDrawable(R.mipmap.edit_seleted));
                    bt_add_point.setVisibility(View.VISIBLE);
                    bt_save.setVisibility(View.VISIBLE);
                    bt_cancel.setVisibility(View.VISIBLE);
                }
                break;
            case R.id.bt_add_point:
                if (!isAddPoint){
                    editDialog();
                }
                break;
            case R.id.bt_save:
                bt_edit_map.setBackground(getResources().getDrawable(R.mipmap.edit_default));
                bt_add_point.setBackground(getResources().getDrawable(R.mipmap.edit_bg_gray));
                bt_add_point.setVisibility(View.INVISIBLE);
                bt_save.setVisibility(View.INVISIBLE);
                bt_cancel.setVisibility(View.INVISIBLE);
                sendPoint();
                Message message1=new Message();
                message1.arg1=6;
                customHandler.sendMessage(message1);
                requestFile();
                break;
            case R.id.bt_cancel:
                bt_edit_map.setBackground(getResources().getDrawable(R.mipmap.edit_default));
                bt_add_point.setBackground(getResources().getDrawable(R.mipmap.edit_bg_gray));
                bt_add_point.setVisibility(View.INVISIBLE);
                bt_save.setVisibility(View.INVISIBLE);
                bt_cancel.setVisibility(View.INVISIBLE);
                Message message=new Message();
                message.arg1=6;
                customHandler.sendMessage(message);
                break;
        }

    }

    List<BaseCmd.PathPoint> pathPointList=new ArrayList<>();
    List<XyEntity>pointList;
    private void sendPoint(){
        pointList=customImageView.getPointList();
        pathPointList.clear();   //清空上次保存的点
        RobotStatusEntity robotStatusEntity=RobotStatusEntity.getInstance();
        for (int i=0;i<pointList.size();i++){
            BaseCmd.PathPoint.Builder builder=BaseCmd.PathPoint.newBuilder();
            builder.setPointX(pointList.get(i).getX())
                    .setPointY(pointList.get(i).getY())
                    .setTypeValue(pointList.get(i).getePathPointType())
                    .setPointInfo(ByteString.copyFromUtf8(pointList.get(i).getPointInfo()))
                    .setSpeed(robotStatusEntity.getCurrspeed());
            BaseCmd.PathPoint pathPoint=builder.build();
            pathPointList.add(pathPoint);
        }
        BaseCmd.reqCmdSetPathPoint setPathPoint=BaseCmd.reqCmdSetPathPoint.newBuilder()
                    .setPathName(ByteString.copyFromUtf8(collectPath))
                    .setRouteName(ByteString.copyFromUtf8(RobotStatusEntity.getInstance().getCurroute()))
                    .setEndAng(0)
                    .addAllSPath(pathPointList)
                    .build();
        BaseCmd.CommonHeader commonHeader=BaseCmd.CommonHeader.newBuilder()
                    .setFromCltType(BaseCmd.eCltType.eLocalAndroidClient)
                    .setToCltType(BaseCmd.eCltType.eLSMSlamNavigation)
                    .addFlowDirection(BaseCmd.CommonHeader.eFlowDir.Forward)
                    .build();
        tcpClient.sendData(commonHeader,setPathPoint);
        Logger.e("发送path路径点");

    }


    /**
     * 请求文件（txt、png) 刷新文件
     */
    private void requestFile(){
//        final ByteString path= ByteString.copyFromUtf8("OneRoute*/Path*.txt");
//        final ByteString matPath=ByteString.copyFromUtf8("OneRoute*/affine_mat.txt");
//        final ByteString pngPath=ByteString.copyFromUtf8("OneRoute*/bkPic.png");

        final ByteString filePath=ByteString.copyFromUtf8("OneRoute*/*.*");
        getHttpFile(filePath);
//        getHttpFile(path);
//        getHttpFile(matPath);
//        getHttpFile(pngPath);
        Logger.e("请求http文件下载");
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
     String collectPath;
    /**
     * 编辑文件名称的弹窗
     */
    private void editDialog(){
        collectPath="";
        customDialog=new CustomDialog(getActivity(), null,6, null, new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                collectPath=customDialog.editCollect.getText().toString().trim();
                if (!collectPath.isEmpty()){
                    collectPath="Path_auto_"+collectPath+".txt";
                    bt_add_point.setBackground(getResources().getDrawable(R.mipmap.edit_bg_blue));
                    Message message=new Message();
                    message.arg1=5;
                    customHandler.sendMessage(message);
                    customDialog.dismiss();
                    Logger.e("----"+collectPath);
                }else {
                    Toast.makeText(getActivity(),"请输入采集路径名称",Toast.LENGTH_SHORT).show();
                }
            }
        }, null, new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                customDialog.dismiss();
            }
        });
        customDialog.setCancelable(false);
        customDialog.show();
    }

    @Override
    public void onResume() {
        super.onResume();
        mapName.setText(robotStatusEntity.getCurroute()+"/"+robotStatusEntity.getCurrpath());
    }

    @Override
    public void onPause() {
        super.onPause();
        Logger.e("------onPause");
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        unbinder.unbind();
    }
}
