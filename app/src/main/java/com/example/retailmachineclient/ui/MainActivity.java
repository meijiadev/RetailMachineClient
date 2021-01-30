package com.example.retailmachineclient.ui;


import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import com.example.retailmachineclient.R;
import com.example.retailmachineclient.base.BaseActivity;

import androidx.recyclerview.widget.RecyclerView;
import butterknife.BindView;
import butterknife.OnClick;

public class MainActivity extends BaseActivity {

    @BindView(R.id.btEnglish)
    TextView btEnglish;
    @BindView(R.id.btFrench)
    TextView btFrench;
    @BindView(R.id.tvTelephone)
    TextView tvTelephone;
    @BindView(R.id.btChinese)
    TextView btChinese;
    @BindView(R.id.recyclerCommodity)
    RecyclerView recyclerCommodity;
    @BindView(R.id.recyclerShopping)
    RecyclerView recyclerShopping;
    @BindView(R.id.tvCommodityName)
    TextView tvCommodityName;

    @Override
    protected int getLayoutId() {
        return R.layout.activity_main;
    }

    @Override
    protected void initView() {
        setStatusBarEnabled(true);
    }

    @Override
    protected void initData() {

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
    }


    @OnClick({R.id.btEnglish, R.id.btFrench, R.id.btChinese})
    public void onViewClicked(View view) {
        switch (view.getId()) {
            case R.id.btEnglish:

                break;
            case R.id.btFrench:

                break;
            case R.id.btChinese:

                break;
        }
    }
}
