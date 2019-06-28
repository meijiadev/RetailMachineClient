package com.example.dadao.dadaorobot.activity;

import android.os.Bundle;
import android.widget.ImageView;

import com.example.dadao.dadaorobot.R;
import com.example.dadao.dadaorobot.base.DaDaoBaseActivity;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;

/**
 * 日志界面
 */
public class LogActivity extends DaDaoBaseActivity {

    @BindView(R.id.back)
    ImageView back;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_log);
        ButterKnife.bind(this);

    }

    @OnClick(R.id.back)
    public void onViewClicked() {
        finish();
    }
}