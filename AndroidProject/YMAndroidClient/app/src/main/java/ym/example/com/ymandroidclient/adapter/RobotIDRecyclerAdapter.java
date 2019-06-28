package ym.example.com.ymandroidclient.adapter;

import android.content.Context;
import android.support.annotation.NonNull;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import java.util.List;

import ym.example.com.ymandroidclient.R;
import ym.example.com.ymandroidclient.entity.XyEntity;

/**
 * 机器人ID列表适配器
 * 可用于 设置路径的列表适配器 换个子项布局
 */
public class RobotIDRecyclerAdapter extends RecyclerView.Adapter<RobotIDRecyclerAdapter.ViewHolder> {
    private Context context;
    private List<String> list;
    private List<XyEntity> pointList;
    private OnItemClickListener mOnItemClickListener;

    private int viewType;   //设置的子项Id

    public RobotIDRecyclerAdapter(Context context){
        this.context=context;
    }

    public void setViewType(int viewType){
        this.viewType=viewType;
    }

    public void setList(List<String> list){
        this.list=list;
    }

    public void setPointList(List<XyEntity> list){
        this.pointList=list;
    }


    @NonNull
    @Override
    public ViewHolder onCreateViewHolder(@NonNull ViewGroup viewGroup, int i) {
        View view = null;
        switch (viewType){
            case 1:
                view=LayoutInflater.from(context).inflate(R.layout.item_recycle_robot_id,viewGroup,false);
                break;
            case 2:
                view=LayoutInflater.from(context).inflate(R.layout.item_recycle_file,viewGroup,false);
                break;

        }
        ViewHolder viewHolder=new ViewHolder(view);
        return viewHolder;
    }

    @Override
    public void onBindViewHolder(@NonNull final ViewHolder viewHolder, final int i) {
        switch (viewType){
            case 1:
                viewHolder.tv_robotId.setText(list.get(i));
                viewHolder.tv_robotId.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {


                        mOnItemClickListener.onItemClick(viewHolder,i);
                    }
                });
                break;
            case 2:
                viewHolder.item_name.setText(list.get(i));
                viewHolder.item_name.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        mOnItemClickListener.onItemClick(viewHolder,i);
                    }
                });
                break;
        }


    }



    @Override
    public int getItemCount() {
        if (list!=null){
            return list.size();
        }else {
            return pointList.size();
        }
    }

    public class ViewHolder extends RecyclerView.ViewHolder {
        TextView tv_robotId;
        TextView item_name;
        public ViewHolder(@NonNull View itemView) {
            super(itemView);
            switch (viewType){
                case 1:
                    tv_robotId=itemView.findViewById(R.id.tv_robot_id);
                    break;
                case 2:
                    item_name=itemView.findViewById(R.id.item_name);
                    break;

            }

        }
    }

    public void setOtemClickListener(OnItemClickListener onItemClickListener){
        this.mOnItemClickListener=onItemClickListener;
    }

    public interface OnItemClickListener{
        void onItemClick(ViewHolder view, int position);
    }


}
