package ym.example.com.ymandroidclient.view;


import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;


import ym.example.com.ymandroidclient.R;
import ym.example.com.ymandroidclient.utility.Logger;

public class CustomDialog extends Dialog {
    private String title;

    private String buttonConfirm;
    private String buttonCancel;
    private View.OnClickListener confirmClickListener;
    private View.OnClickListener cancelClickListener;
    private int showWhich=0;

    View.OnClickListener v1,v2,v3,v4,deleteOnClick;

    public EditText editCollect;


    public CustomDialog(Context context, String title, int showWhich,
                        String buttonConfirm,
                        View.OnClickListener confirmClickListener, String buttonCancel, View.OnClickListener cancelClickListener) {
        super(context, R.style.Dialog);
        this.title = title;
        this.showWhich=showWhich;
        this.buttonConfirm = buttonConfirm;
        this.buttonCancel = buttonCancel;
        this.confirmClickListener = confirmClickListener;
        this.cancelClickListener = cancelClickListener;
    }

    public CustomDialog(Context context, int showWhich, View.OnClickListener onClickListener1, View.OnClickListener onClickListener2
            , View.OnClickListener onClickListener3, View.OnClickListener onClickListener4, View.OnClickListener deleteOnClickListener){
        super(context, R.style.Dialog);
        this.showWhich=showWhich;
        this.v1=onClickListener1;
        this.v2=onClickListener2;
        this.v3=onClickListener3;
        this.v4=onClickListener4;
        this.deleteOnClick=deleteOnClickListener;

    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        switch (showWhich){
           case 1:
               break;
           case 2:
               initCollectLayout();
               break;
           case 3:
               initShutdownLayout();
               break;
            case 4:
                initShowTcpStutus();
                break;
            case 5:
                initShowPointType();
                break;
            case 6:
                initShowEditPath();
                break;

       }

    }


    private void initCollectLayout() {
        setContentView(R.layout.collect_dialog);
        editCollect = findViewById(R.id.edit_collect);
        Button cancel_collect = findViewById(R.id.cancel_collect);
        Button confirm_collect = findViewById(R.id.confirm_collect);
        if (null != confirmClickListener) {
            confirm_collect.setOnClickListener(confirmClickListener);
        }
        if (null != cancelClickListener) {
            cancel_collect.setOnClickListener(cancelClickListener);
        }
    }

    private void initShutdownLayout(){
        setContentView(R.layout.shutdown_dialog);
        Button shutdown=findViewById(R.id.shutdown);
        Button restart=findViewById(R.id.restart);
        if (null != confirmClickListener) {
            shutdown.setOnClickListener(confirmClickListener);
        }
        if (null != cancelClickListener) {
            restart.setOnClickListener(cancelClickListener);
        }
        Logger.e("------shutdownLayout");

    }

    private void initShowTcpStutus(){
        setContentView(R.layout.tcp_status_dialog);
        Button i_know=findViewById(R.id.i_know);
        Button cancel=findViewById(R.id.cancel);
        if (null!=confirmClickListener){
            i_know.setOnClickListener(confirmClickListener);
        }
        if (null!=cancelClickListener){
            cancel.setOnClickListener(cancelClickListener);
        }
    }

    private void initShowPointType(){
        setContentView(R.layout.point_type_dialog);
        TextView view1=findViewById(R.id.type1);
        TextView view2=findViewById(R.id.type2);
        TextView view3=findViewById(R.id.type3);
        TextView view4=findViewById(R.id.type4);
        Button delete=findViewById(R.id.delete_point);

        view1.setOnClickListener(v1);
        view2.setOnClickListener(v2);
        view3.setOnClickListener(v3);
        view4.setOnClickListener(v4);
        delete.setOnClickListener(deleteOnClick);
    }

    private void initShowEditPath(){
        setContentView(R.layout.edit_path_dialog);
        editCollect = findViewById(R.id.edit_collect);
        Button cancel_collect = findViewById(R.id.cancel_collect);
        Button confirm_collect = findViewById(R.id.confirm_collect);
        if (null != confirmClickListener) {
            confirm_collect.setOnClickListener(confirmClickListener);
        }
        if (null != cancelClickListener) {
            cancel_collect.setOnClickListener(cancelClickListener);
        }
    }





}

