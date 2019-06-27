package com.example.dadao.dadaorobot.adapter;


import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;


import com.bumptech.glide.Glide;
import com.bumptech.glide.load.engine.bitmap_recycle.BitmapPool;
import com.example.dadao.dadaorobot.R;

import java.util.List;

import jp.wasabeef.glide.transformations.BlurTransformation;
import jp.wasabeef.glide.transformations.ColorFilterTransformation;

public class VideoListAdapter extends BaseAdapter {
    private List<String> mList;
    private Context context;

    public VideoListAdapter(Context context) {
        super();
        this.context=context;
    }
    public void setList(List<String> list){
        this.mList=list;
    }

    @Override
    public int getCount() {
        return mList.size();
    }

    @Override
    public Object getItem(int position) {
        return mList.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        View view=LayoutInflater.from(context).inflate(R.layout.item_list_video,null);
        TextView tv_name=view.findViewById(R.id.tv_name);
        ImageView imageView=view.findViewById(R.id.image_video);
        Glide.with(context)
                .load(R.mipmap.timg)
               // .bitmapTransform(new ColorFilterTransformation(context,0x50000000))
                .bitmapTransform(new BlurTransformation(context,40))
                .into(imageView);
        tv_name.setText(mList.get(position));
        return view;
    }

}

