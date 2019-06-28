package ym.example.com.ymandroidclient.fragment;


import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;


import com.shuyu.gsyvideoplayer.GSYVideoManager;
import com.shuyu.gsyvideoplayer.model.VideoOptionModel;

import java.util.ArrayList;
import java.util.List;

import tv.danmaku.ijk.media.player.IjkMediaPlayer;
import ym.example.com.ymandroidclient.R;
import ym.example.com.ymandroidclient.utility.Logger;
import ym.example.com.ymandroidclient.view.NewStandardGSYVideoPlayer;

public class VideoFragment extends Fragment {
    private NewStandardGSYVideoPlayer videoPlayer;
    public VideoFragment() {
        // Required empty public constructor
    }



    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Logger.d("onCreate：------------Fragment创建");
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View view=inflater.inflate(R.layout.fragment_video,container,false);
        Logger.d("onCreateView：------------------View创建");
        initView(view);
        return view;
    }

    private String source="";
    private void initView(View view){
        videoPlayer =  view.findViewById(R.id.videoPlayer);
        videoPlayer.getBackButton().setVisibility(View.GONE);
        /**如果不加上这些内容，在非局域网的情况下会一直卡*/
        List<VideoOptionModel> list = new ArrayList<>();
        VideoOptionModel videoOptionModel = new VideoOptionModel(IjkMediaPlayer.OPT_CATEGORY_FORMAT, "rtsp_transport", "tcp");
        list.add(videoOptionModel);
        /*videoOptionModel=new VideoOptionModel(IjkMediaPlayer.OPT_CATEGORY_PLAYER,"mediacodec",1);   //打开硬解码
        list.add(videoOptionModel);
        videoOptionModel=new VideoOptionModel(IjkMediaPlayer.OPT_CATEGORY_PLAYER,"videotoolbox",0);  //关闭软解码
        list.add(videoOptionModel);*/
        videoOptionModel = new VideoOptionModel(IjkMediaPlayer.OPT_CATEGORY_FORMAT, "rtsp_flags", "prefer_tcp");
        list.add(videoOptionModel);
        videoOptionModel = new VideoOptionModel(IjkMediaPlayer.OPT_CATEGORY_FORMAT, "allowed_media_types", "video"); //根据媒体类型来配置
        list.add(videoOptionModel);
        videoOptionModel = new VideoOptionModel(IjkMediaPlayer.OPT_CATEGORY_FORMAT, "timeout", 20000);
        list.add(videoOptionModel);
        videoOptionModel = new VideoOptionModel(IjkMediaPlayer.OPT_CATEGORY_FORMAT, "buffer_size", 1316);
        list.add(videoOptionModel);
        videoOptionModel = new VideoOptionModel(IjkMediaPlayer.OPT_CATEGORY_FORMAT, "infbuf", 1);  // 无限读
        list.add(videoOptionModel);
        videoOptionModel = new VideoOptionModel(IjkMediaPlayer.OPT_CATEGORY_FORMAT, "analyzemaxduration", 100);
        list.add(videoOptionModel);
        videoOptionModel = new VideoOptionModel(IjkMediaPlayer.OPT_CATEGORY_FORMAT, "probesize", 10240);
        list.add(videoOptionModel);
        videoOptionModel = new VideoOptionModel(IjkMediaPlayer.OPT_CATEGORY_FORMAT, "flush_packets", 1);
        list.add(videoOptionModel);
        //  关闭播放器缓冲，这个必须关闭，否则会出现播放一段时间后，一直卡主，控制台打印 FFP_MSG_BUFFERING_START
        videoOptionModel = new VideoOptionModel(IjkMediaPlayer.OPT_CATEGORY_PLAYER, "packet-buffering", 0);
        list.add(videoOptionModel);
        GSYVideoManager.instance().setOptionModelList(list);
        Logger.e("------播放的视频源："+source);
        videoPlayer.setUp(source, true, "");
        //是否可以滑动调整
        videoPlayer.setIsTouchWiget(true);
        videoPlayer.startPlayLogic();

    }

    @Override
    public void onPause() {
        super.onPause();
        videoPlayer.onVideoPause();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        GSYVideoManager.releaseAllVideos();
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        GSYVideoManager.releaseAllVideos();
    }
}
