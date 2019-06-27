package com.example.dadao.dadaorobot.util.widget;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.util.AttributeSet;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.Transformation;

import com.example.dadao.dadaorobot.R;
import com.example.dadao.dadaorobot.util.DpOrPxUtils;

/**
 *自定义圆形进度条
 */
public class CircleBarView extends View {
    private RectF mRectF;//绘制圆弧的矩形区域
    private Paint bgPaint;//绘制背景圆弧的画笔
    private Paint progressPaint;//绘制圆弧的画笔
    private float progressNum;  //可以更新的进度条数值
    private float MaxNum;       //进度条最大值

    private float progressSweepAngle;//进度条圆弧扫过的角度
    private float startAngle;  //背景圆弧的起始角度
    private float sweepAngle;   //背景圆弧扫过的角度

    private float barWidth;    // 圆弧进度条宽度
    private int defaultSize;   //自定义的View默认的宽高

    private int progressColor=Color.GREEN;  //设置进度条的颜色


    private CircleBarAnim circleBarAnim;

    private Bitmap centerBg;
    private Paint centerPaint;



    public CircleBarView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init(context,attrs);
    }

    private void init(Context context,AttributeSet attrs){
        TypedArray typedArray = context.obtainStyledAttributes(attrs, R.styleable.CircleBarView);
        Drawable imageBackground=typedArray.getDrawable(R.styleable.CircleBarView_centerBackground);
        if (null!=imageBackground){
            if (imageBackground instanceof BitmapDrawable){
                centerBg=((BitmapDrawable) imageBackground).getBitmap();
            }
        }else {
            //如何没有设置背景图片
        }

        defaultSize = DpOrPxUtils.dip2px(context,100);
        barWidth = DpOrPxUtils.dip2px(context,2);
        mRectF = new RectF();

        progressPaint = new Paint();
        progressPaint.setStyle(Paint.Style.STROKE);//只描边，不填充
        progressPaint.setAntiAlias(true);//设置抗锯齿
        progressPaint.setStrokeWidth(barWidth);      //设置一个画笔的宽度
        circleBarAnim=new CircleBarAnim();

        bgPaint=new Paint();
        bgPaint.setStyle(Paint.Style.STROKE);
        bgPaint.setColor(Color.GRAY);
        bgPaint.setAntiAlias(true);
        bgPaint.setStrokeWidth(barWidth);

        centerPaint=new Paint();
        centerPaint.setAntiAlias(true);


        progressNum=0;
        MaxNum=100;
        startAngle=0;
        sweepAngle=360;
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        //RectF rectF = new RectF(x,y,x+300,y+300);//建一个大小为300 * 300的正方形区域
        int measuredWidth = getMeasuredWidth();          //获取View原始大小
        int measuredHeight = getMeasuredHeight();
        // Logger.e("view的宽："+measuredWidth);
        int cx = measuredWidth / 2;
        int cy = measuredHeight / 2;
        // 中心点
        Rect src=new Rect(0,0,centerBg.getWidth(),centerBg.getHeight());
        Rect dst=new Rect(cx-centerBg.getWidth()/2,cy-centerBg.getHeight()/2,cx+centerBg.getWidth()/2,cy+centerBg.getHeight()/2);
        canvas.drawBitmap(centerBg,src,dst,centerPaint);
        progressPaint.setColor(progressColor);
        canvas.drawArc(mRectF,startAngle,sweepAngle,false,bgPaint);//这里角度0对应的是三点钟方向，顺时针方向递增   背景圆弧
        canvas.drawArc(mRectF,startAngle,progressSweepAngle,false,progressPaint);      //进度圆弧
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        int height=measureSize(defaultSize,heightMeasureSpec);

        int width=measureSize(defaultSize,widthMeasureSpec);
        int min=Math.min(width,height);    //获取View最短的边
        setMeasuredDimension(min,min);

        if(min >= barWidth*2){//这里简单限制了圆弧的最大宽度
            mRectF.set(barWidth/2,barWidth/2,min-barWidth/2,min-barWidth/2);
        }
    }

    private int measureSize(int defaultSize,int measureSpec) {
        int result = defaultSize;
        int specMode = MeasureSpec.getMode(measureSpec);
        int specSize = MeasureSpec.getSize(measureSpec);

        if (specMode == MeasureSpec.EXACTLY) {
            result = specSize;
        } else if (specMode == MeasureSpec.AT_MOST) {
            result = Math.min(result, specSize);
        }
        return result;
    }

    public class CircleBarAnim extends Animation{
        public CircleBarAnim(){

        }
        @Override
        protected void applyTransformation(float interpolatedTime, Transformation t) {
            super.applyTransformation(interpolatedTime, t);
            progressSweepAngle=interpolatedTime*sweepAngle*progressNum/MaxNum;
            postInvalidate();
        }
    }
    

    public void setProgress(float progressNum,int time,int progressColor){
        circleBarAnim.setDuration(time);
        this.startAnimation(circleBarAnim);
        this.progressNum=progressNum;
        this.progressColor=progressColor;
    }


}

