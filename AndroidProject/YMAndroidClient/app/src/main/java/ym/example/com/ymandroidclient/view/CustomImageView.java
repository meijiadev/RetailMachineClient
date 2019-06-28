package ym.example.com.ymandroidclient.view;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.preference.PreferenceManager;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;



import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.List;

import ym.example.com.ymandroidclient.MainActivity;
import ym.example.com.ymandroidclient.R;
import ym.example.com.ymandroidclient.entity.NotifyBaseStatusEx;
import ym.example.com.ymandroidclient.entity.XyEntity;
import ym.example.com.ymandroidclient.fragment.LaserMapFragment;
import ym.example.com.ymandroidclient.utility.Logger;


/**
 * 将激光图作为背景在背景中画线
 */
public class CustomImageView extends View {
    private boolean isRealTimeDraw=false;
    private boolean isAddPoint=false;
    private float posX,posY;   //计算之后实时的X,Y坐标
    //private RobotStatusEntity robotStatusEntity;
    private NotifyBaseStatusEx notifyBaseStatusEx;
    private Paint linePaint;   //直线画笔
    private Paint imagePaint;  //图片画笔
    private Paint pointPaint,pointPaint1;  //路径点画笔
    private List<XyEntity> list=new ArrayList<>();
    private List<Float>mList=new ArrayList<>();
    private List<XyEntity> pointToSList=new ArrayList<>();   //发送给服务端的点信息
    private List<XyEntity> pointCanvasList=new ArrayList<>();

    private Bitmap mAreaBitmap;
    Bitmap bitmap=null;
    private double r00=0;
    private double r01=-61.5959;
    private double t0=375.501;
    private double r10=-61.6269;
    private double r11=0;
    private double t1=410.973;
    private Context context;

    private SharedPreferences sharedPreferences;
    private SharedPreferences.Editor editor;

    public CustomImageView(Context context) {
        super(context);
        this.context=context;
    }

    public static Handler mHandler;

    public static Handler getmHandler(){
        return mHandler;
    }

    public Handler handler=new Handler(){
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.arg1){
                case 0:
                    Bundle bundle=msg.getData();
                    String matPath=bundle.getString("matPath");
                    read(matPath);
                    break;
                case 1:
                    String pngPath= (String) msg.obj;
                    initPng(pngPath);
                    break;
                case 2:
                    isRealTimeDraw=true;
                    realTimeDraw();
                    break;
                case 3:
                    isRealTimeDraw=false;
                    break;
                case 4:
                    String matPath1=Environment.getExternalStorageDirectory().getPath()+"/"+"机器人"+"/"+notifyBaseStatusEx.getCurroute()+"/"+"affine_mat.txt";
                    //String path1=Environment.getExternalStorageDirectory().getPath()+"/"+"机器人"+"/"+notifyBaseStatusEx.getCurroute()+"/"+notifyBaseStatusEx.getCurrpath();
                    String pngPath1=Environment.getExternalStorageDirectory().getPath()+"/"+"机器人"+"/"+notifyBaseStatusEx.getCurroute()+"/"+"bkPic.png";
                    Logger.e("-------当前的路径名："+notifyBaseStatusEx.getCurroute()+"/"+notifyBaseStatusEx.getCurrpath());
                    bitmap=null;
                    initPng(pngPath1);
                    read(matPath1);
                    break;
                case 5:              //添加point
                    isAddPoint=true;
                    break;
                case 6:             //取消编辑
                    isAddPoint=false;
                    pointToSList.clear();
                    pointCanvasList.clear();
                    invalidate();
                    Message message=new Message();
                    message.arg1=0;
                    LaserMapFragment.getHandler().sendMessage(message);
                    index=0;
                    break;
                case 7:   // 删除该点
                    int index1=msg.arg2;
                    removePointList(index1);
                    break;
                case 8:
                    String matPath2=Environment.getExternalStorageDirectory().getPath()+"/"+"机器人"+"/"+sharedPreferences.getString("dirName","")+"/"+"affine_mat.txt";
                    String pngPath2=Environment.getExternalStorageDirectory().getPath()+"/"+"机器人"+"/"+sharedPreferences.getString("dirName","")+"/"+"bkPic.png";
                    read(matPath2);
                    initPng(pngPath2);
                    break;
            }

        }
    };


    public CustomImageView(Context context, AttributeSet attrs) {
        super(context, attrs);
        initAttribute(context,attrs);
        this.context=context;
        linePaint=new Paint();
        linePaint.setAntiAlias(true);
        linePaint.setColor(Color.GREEN);
        linePaint.setStrokeWidth(2);

        imagePaint=new Paint();
        imagePaint.setAntiAlias(true);
        mHandler=handler;
        notifyBaseStatusEx=NotifyBaseStatusEx.getInstance();
        sharedPreferences= PreferenceManager.getDefaultSharedPreferences(context);
        editor=sharedPreferences.edit();

        pointPaint=new Paint();
        pointPaint.setAntiAlias(true);
        pointPaint.setColor(Color.RED);
        pointPaint.setStrokeWidth(10);

        pointPaint1=new Paint();
        pointPaint1.setAntiAlias(true);
        pointPaint1.setColor(Color.BLUE);
        pointPaint1.setStrokeWidth(9);


    }

    private void initAttribute(Context context, AttributeSet attrs){
        TypedArray typedArray = context.obtainStyledAttributes(attrs, R.styleable.CustomImageView);
         Drawable imageBackground=typedArray.getDrawable(R.styleable.CustomImageView_imageBackground);
         if (bitmap==null){
             Logger.e("png为空");
             if (null!=imageBackground){
                 if (imageBackground instanceof BitmapDrawable){
                     mAreaBitmap=((BitmapDrawable) imageBackground).getBitmap();
                 }
             }
         }else{
             Logger.e("png不为空");
             mAreaBitmap=bitmap;
         }
    }

    /**
     * 测量
     * @param widthMeasureSpec
     * @param heightMeasureSpec
     */
    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        int measureWidth, measureHeight;
        int widthMode = MeasureSpec.getMode(widthMeasureSpec);
        int heightMode = MeasureSpec.getMode(heightMeasureSpec);
        int widthSize = MeasureSpec.getSize(widthMeasureSpec);
        int heightSize = MeasureSpec.getSize(heightMeasureSpec);
        if (bitmap!=null){
            mAreaBitmap=bitmap;
        }else {
            Logger.e("设置的图片为空？");
        }
        if (widthMode == MeasureSpec.EXACTLY) {
            // 具体的值和match_parent
            measureWidth = widthSize;
        } else {
            // wrap_content
            measureWidth = mAreaBitmap.getWidth();
        }
        if (heightMode == MeasureSpec.EXACTLY) {
            measureHeight = heightSize;
        } else {
            measureHeight = mAreaBitmap.getHeight();
        }
        setMeasuredDimension(measureWidth, measureHeight);
        Logger.e("view:"+measureWidth+";"+measureHeight);
    }


    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        int measuredWidth = getMeasuredWidth();          //获取View原始大小
        int measuredHeight = getMeasuredHeight();
        if (bitmap!=null){
            mAreaBitmap=bitmap;
        }
        int imageWidth=mAreaBitmap.getWidth();
        int imageHeight=mAreaBitmap.getHeight();
        //Logger.e("png的宽和高："+imageWidth+";"+imageHeight);
        Rect src=new Rect(0,0,imageWidth,imageHeight);           //要绘制到画布上的图片大小
        Rect dst=new Rect(0,0,measuredWidth,measuredHeight);     //绘制到画布上的位置
        canvas.drawBitmap(mAreaBitmap,src,dst,imagePaint);
        /*if (list.size()>1&&list!=null){
            for (int i=0;i<list.size()-1;i++){
                canvas.drawLine(list.get(i).getX(),list.get(i).getY(),list.get(i+1).getX(),list.get(i+1).getY(),linePaint);
                Logger.e("绘制行走路径");

            }
        }*/
        if (isRealTimeDraw){
            Logger.e("描绘实时位置");
            canvas.drawPoint(posX,posY,pointPaint);
        }
        if (isAddPoint){
            for (int i=0;i<pointCanvasList.size();i++){
                canvas.drawPoint(pointCanvasList.get(i).getX(),pointCanvasList.get(i).getY(),pointPaint1
                );
            }

        }

    }
    int index=0;
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (isAddPoint){
            x = event.getX();
            y = event.getY();
            index++;
            Log.e("index:"+index+"x:" + x, "y:" + y);
            pointCanvasList.add(new XyEntity(x,y));
            pointToSList.add(toPathXy(index,x,y));
            invalidate();
            Message message=new Message();
            message.arg1=0;
            LaserMapFragment.getHandler().sendMessage(message);
        }
        return super.onTouchEvent(event);
    }

    public List<XyEntity> getPointList(){
        return pointToSList;
    }

    public void removePointList(int index){
        pointToSList.remove(index);
        pointCanvasList.remove(index);
        invalidate();
        Message message=new Message();
        message.arg1=0;
        LaserMapFragment.getHandler().sendMessage(message);
    }

    private void read(String path) {
        Logger.e("------"+path);
        mList.clear();
        list.clear();
        File file=new File(path);
        if (file.exists()){
            Logger.e("file不为空");
            FileReader fileReader= null;
             try {
                fileReader = new FileReader(file);
                } catch (FileNotFoundException e) {
                    e.printStackTrace();
                }
                BufferedReader bufferedReader = null;
                bufferedReader=new BufferedReader(fileReader);   //字符读取流
                String readline="";
                StringBuffer sb=new StringBuffer();
                try {
                    while ((readline=bufferedReader.readLine())!=null){
                        sb.append(readline+" ");
                    }
                    bufferedReader.close();
                }catch (IOException e){
                    e.printStackTrace();
                }
                String s[]=sb.toString().split("[\\s]+");
                if (s.length>1){
                    if (path.contains("affine_mat")){
                        if (s.length==6){
                            r00= Double.parseDouble(s[0]);
                            r01= Double.parseDouble(s[1]);
                            t0= Double.parseDouble(s[2]);
                            r10= Double.parseDouble(s[3]);
                            r11= Double.parseDouble(s[4]);
                            t1= Double.parseDouble(s[5]);
                            invalidate();
                        }
                    }else {
                        for (int i=0;i<s.length;i++){
                            mList.add(Float.valueOf(s[i]));
                        }
                        int a=mList.size()/3;
                        for (int i=0;i<a;i++){
                            list.add(toXorY(mList.get(i+i*2),mList.get(i+1+i*2),mList.get(i+2+i*2)));
                        }
                        invalidate();
                    }
                }
            }else {
            Message message=new Message();
            message.arg1=5;
            MainActivity.getHandler().sendMessage(message);
        }
    }


    /**
     * 将本地图片变成Bitmap
     * @param pngPath
     */
    public void initPng(String pngPath){
        if (pngPath!=null){
            FileInputStream fis = null;
            try {
                fis = new FileInputStream(pngPath);
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            }
            bitmap= BitmapFactory.decodeStream(fis);
            Logger.e("更新图片");
            requestLayout();
        }

    }

    private float x1,y1;
    private XyEntity toXorY(float x, float y, float speed){
        x1=(float)( r00*x+r01*y+t0);
        y1=(float) (r10*x+r11*y+t1);
        return new XyEntity(x1,y1,speed);
    }

    float k;
    float j;
    private XyEntity toPathXy(int index,float x,float y){
        k= (float) (r00*r11-r10*r01);
        j= (float) (r10*r01-r00*r11);
        float ax= (float) (r11*x-r01*y+r01*t1-r11*t0);
        float ay= (float) (r10*x-r00*y+r00*t1-r10*t0);
        float sX=txfloat(ax,k);
        float sY=txfloat(ay,j);
        return new XyEntity(index,sX,sY);
    }

    private float txfloat(float a,float b) {
        // TODO 自动生成的方法存根
        DecimalFormat df=new DecimalFormat("0.0000");//设置保留位数
        return Float.parseFloat(df.format((float)a/b));

    }

    float x,y;  //当前机器人的位置坐标
    /**
     * 实时绘制
     */
    private void realTimeDraw(){
        x=notifyBaseStatusEx.getPosX();
        y=notifyBaseStatusEx.getPosY();
        posX=(float) (r00*x+r01*y+t0);
       // Logger.e("-----:"+x+";"+y);
        posY=(float) (r10*x+r11*y+t1);
        //Logger.e("-----:"+posX+";"+posY);
        invalidate();
    }
}
