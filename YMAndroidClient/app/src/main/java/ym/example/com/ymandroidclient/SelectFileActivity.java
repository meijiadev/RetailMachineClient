package ym.example.com.ymandroidclient;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Environment;
import android.preference.PreferenceManager;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;
import ym.example.com.ymandroidclient.adapter.RobotIDRecyclerAdapter;
import ym.example.com.ymandroidclient.utility.Logger;

public class SelectFileActivity extends DaDaoBaseActivity {

    @BindView(R.id.iv_back)
    ImageView ivBack;
    @BindView(R.id.title_name)
    TextView titleName;
    @BindView(R.id.recycle_file)
    RecyclerView recycleFile;

    public Context context;
    public List<String> nameList=new ArrayList<>();
    private RobotIDRecyclerAdapter robotIDRecyclerAdapter;
    private SharedPreferences sharedPreferences;
    private SharedPreferences.Editor editor;
    private boolean isFile;
    private boolean havePng=false;
    private String pngPath;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_select_file);
        ButterKnife.bind(this);
        sharedPreferences= PreferenceManager.getDefaultSharedPreferences(this);
        editor=sharedPreferences.edit();
        context=getApplicationContext();
        isDir();
        initRecycle();
    }

    /**
     * 判断打开的是？
     */
    public void isDir(){
        Intent intent=getIntent();
        isFile=intent.getBooleanExtra("isFile",false);
        if(!isFile){
            titleName.setText("选择地图");
            nameList=getFilesAllName(Environment.getExternalStorageDirectory().getPath()+"/"+"机器人");
        }else {
            titleName.setText("路径名称");
            String dirName=intent.getStringExtra("dirName");
            nameList=getFilesAllName(Environment.getExternalStorageDirectory().getPath()+"/"+"机器人"+"/"+dirName);
            for (int i=0;i<nameList.size();i++){
                if (nameList.get(i).contains(".png")){
                    havePng=true;
                    pngPath=Environment.getExternalStorageDirectory().getPath()+"/"+"机器人"+"/"+dirName+"/"+nameList.get(i);
                }
            }
        }
    }


    public void initRecycle(){
        LinearLayoutManager linearLayoutManager=new LinearLayoutManager(this);
        linearLayoutManager.setOrientation(LinearLayoutManager.VERTICAL);
        robotIDRecyclerAdapter=new RobotIDRecyclerAdapter(context);
        robotIDRecyclerAdapter.setList(nameList);
        robotIDRecyclerAdapter.setViewType(2);
        recycleFile.setAdapter(robotIDRecyclerAdapter);
        recycleFile.setLayoutManager(linearLayoutManager);
        onItemClick();

    }


    @OnClick(R.id.iv_back)
    public void onViewClicked(View view) {
        switch (view.getId()) {
            case R.id.iv_back:
                finish();
                startActivity(new Intent(SelectFileActivity.this,SettingActivity.class));
                break;
        }
    }

    private void onItemClick(){
        robotIDRecyclerAdapter.setOtemClickListener(new RobotIDRecyclerAdapter.OnItemClickListener() {
            @Override
            public void onItemClick(RobotIDRecyclerAdapter.ViewHolder view, int position) {
                Logger.e("点击子项");
                Intent intent=new Intent(SelectFileActivity.this,SettingActivity.class);
                String name=nameList.get(position);
                if (!isFile){
                    editor.putString("dirName",name);
                    editor.commit();
                    startActivity(intent);
                    finish();
                }else {
                    editor.putString("fileName",name);
                    editor.commit();
                    if (havePng){
                        intent.putExtra("isHavePng",havePng);
                        intent.putExtra("pngPath",pngPath);
                    }
                    if (!name.contains(".png")){
                        startActivity(intent);
                        finish();
                    }

                }

            }
        });
    }


    public  List<String> getFilesAllName(String path) {
        File file=new File(path);
        if (file.exists()){
            File[] files=file.listFiles();
            if (files == null){
                Logger.e("空目录");
                return null;
            }
            List<String> s = new ArrayList<>();
            for(int i =0;i<files.length;i++){
                s.add(files[i].getName());
                Logger.e(files[i].getName());
            }
            return s;
        }else {
            return nameList;
        }

    }

}
