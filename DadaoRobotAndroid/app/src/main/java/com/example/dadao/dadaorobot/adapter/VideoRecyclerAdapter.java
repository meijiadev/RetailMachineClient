package com.example.dadao.dadaorobot.adapter;

import android.content.Context;
import android.support.annotation.NonNull;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import com.example.dadao.dadaorobot.R;

import java.util.List;


/**
 * 视频切换列表 横向滑动
 */
public class VideoRecyclerAdapter extends RecyclerView.Adapter<VideoRecyclerAdapter.ViewHolder> {
    private Context context;
    private List<String> list;
    private OnItemClickListener mOnItemClickListener;

    public VideoRecyclerAdapter(Context context, List<String> list) {
        this.context = context;
        this.list = list;
    }

    @NonNull
    @Override
    public ViewHolder onCreateViewHolder(@NonNull ViewGroup viewGroup, int i) {
        View view = LayoutInflater.from(context).inflate(R.layout.item_recycle_video, viewGroup, false);
        ViewHolder viewHolder = new ViewHolder(view);
        return viewHolder;
    }

    @Override
    public void onBindViewHolder(@NonNull final ViewHolder viewHolder, final int i) {
        viewHolder.tvVideoName.setText(list.get(i));
        viewHolder.ivVideo.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mOnItemClickListener.onItemClick(viewHolder,i);
            }
        });

    }

    @Override
    public int getItemCount() {
        return list.size();
    }

    public class ViewHolder extends RecyclerView.ViewHolder {
        ImageView ivVideo;
        TextView tvVideoName;
        public ViewHolder(@NonNull View itemView) {
            super(itemView);
            tvVideoName=itemView.findViewById(R.id.tv_video_name);
            ivVideo=itemView.findViewById(R.id.iv_video);
        }

    }

    public void setOtemClickListener(OnItemClickListener onItemClickListener){
        this.mOnItemClickListener=onItemClickListener;
    }


    public interface OnItemClickListener{
        void onItemClick(ViewHolder view, int position);
    }
}
