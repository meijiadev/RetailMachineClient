package com.example.dadao.dadaorobot.util.widget;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.LinearInterpolator;
import android.view.animation.RotateAnimation;
import android.widget.AdapterView;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;

import com.example.dadao.dadaorobot.R;
import com.example.dadao.dadaorobot.util.Logger;

import butterknife.BindView;


/**
 * Todo: 可以嵌套lsitView 和GridView
 * Todo: 仿微信下拉头部小程序列表
 */

public class PullToRefrshLinearLayout extends LinearLayout {

    private static final int RELEASE_TO_XCX = 5;
    private static final int PULL_TO_REFRESH = 2;
    private static final int RELEASE_TO_REFRESH = 3;
    private static final int REFRESHING = 4;
    // pull state
    private static final int PULL_UP_STATE = 0;
    private static final int PULL_DOWN_STATE = 1;
    private static final int PULL_UP_XCX = 1;

    /**
     * 最近按下时手指位置
     */
    private int mLastMotionY;

    /**
     * 头布局（小程序和刷新提示）
     */
    private View mHeaderView;

    /**
     * list or grid
     */
    private AdapterView<?> mAdapterView;
    /**
     * scrollview
     */
    private ScrollView mScrollView;

    /**
     * 头布局高度，1:1
     */
    private int mHeaderViewHeight;


    /**
     * footer tip text
     */

    /**
     * layout inflater
     */
    private LayoutInflater mInflater;
    /**
     * 头布局状态，下拉刷新，松开刷新，在下拉显示xcx
     */
    private int mHeaderState;

    /**
     * 需要拦截的滑动状态，可下拉刷新，可上拉加载，上推隐藏小程序
     */
    private int mPullState;
    /**
     * 变为向下的箭头,改变箭头方向
     */
    private RotateAnimation mFlipAnimation;
    /**
     * 变为逆向的箭头,旋转
     */
    private RotateAnimation mReverseFlipAnimation;

    /**
     * footer refresh listener
     */
    private OnHeaderRefreshListener mOnHeaderRefreshListener;


    /**
     * last update time
     */
    // private String mLastUpdateTime;

    /**
     * 是否屏蔽下拉
     */
    private boolean mHideHeader;

    public PullToRefrshLinearLayout(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public PullToRefrshLinearLayout(Context context) {
        super(context);
        init();
    }

    /**
     * init
     */
    private void init() {
        // 需要设置成vertical
        setOrientation(LinearLayout.VERTICAL);
        // Load all of the animations we need in code rather than through XML
        mFlipAnimation = new RotateAnimation(0, -180,
                RotateAnimation.RELATIVE_TO_SELF, 0.5f,
                RotateAnimation.RELATIVE_TO_SELF, 0.5f);
        mFlipAnimation.setInterpolator(new LinearInterpolator());
        mFlipAnimation.setDuration(250);
        mFlipAnimation.setFillAfter(true);

        mReverseFlipAnimation = new RotateAnimation(-180, 0,
                RotateAnimation.RELATIVE_TO_SELF, 0.5f,
                RotateAnimation.RELATIVE_TO_SELF, 0.5f);
        mReverseFlipAnimation.setInterpolator(new LinearInterpolator());
        mReverseFlipAnimation.setDuration(250);
        mReverseFlipAnimation.setFillAfter(true);


        mInflater = LayoutInflater.from(getContext());
        // header view 在此添加,保证是第一个添加到linearlayout的最上端
        addHeaderView();
    }

    private void addHeaderView() {
        // header view
        mHeaderView = mInflater.inflate(R.layout.refresh_header, this, false);

        measureView(mHeaderView);

        mHeaderViewHeight = mHeaderView.getMeasuredHeight();
        LayoutParams params = new LayoutParams(LayoutParams.MATCH_PARENT,
                mHeaderViewHeight);
        // 设置topMargin的值为负的header View高度,即将其隐藏在最上方
        params.topMargin = -(mHeaderViewHeight);
        // mHeaderView.setLayoutParams(params1);
        addView(mHeaderView, params);

    }


    @Override
    protected void onFinishInflate() {
        super.onFinishInflate();
        initContentAdapterView();

    }

    /**
     * init AdapterView like ListView,GridView and so on;or init ScrollView
     */
    private void initContentAdapterView() {
        int count = getChildCount();
        Log.e("------", "----count:" + count);
        if (count < 2) {
            throw new IllegalArgumentException(
                    "This layout must contain 2 child views,and AdapterView or ScrollView must in the second position!");
        }
        View view = null;
        for (int i = 0; i <= count; ++i) {
            view = getChildAt(i);
            if (view instanceof AdapterView<?>) {
                mAdapterView = (AdapterView<?>) view;
            }
            if (view instanceof ScrollView) {
                // finish later
                mScrollView = (ScrollView) view;
            }
        }
        if (mAdapterView == null && mScrollView == null) {
            throw new IllegalArgumentException(
                    "must contain a AdapterView or ScrollView in this layout!");
        }
    }

    private void measureView(View child) {
        ViewGroup.LayoutParams p = child.getLayoutParams();
        if (p == null) {
            p = new ViewGroup.LayoutParams(ViewGroup.LayoutParams.FILL_PARENT,
                    ViewGroup.LayoutParams.WRAP_CONTENT);
        }

        int childWidthSpec = ViewGroup.getChildMeasureSpec(0, 0 + 0, p.width);
        int lpHeight = p.height;
        int childHeightSpec;
        if (lpHeight > 0) {
            childHeightSpec = MeasureSpec.makeMeasureSpec(lpHeight,
                    MeasureSpec.EXACTLY);
        } else {
            childHeightSpec = MeasureSpec.makeMeasureSpec(0,
                    MeasureSpec.UNSPECIFIED);
        }
        child.measure(childWidthSpec, childHeightSpec);
    }

    @Override
    public boolean onInterceptTouchEvent(MotionEvent e) {
        int y = (int) e.getRawY();
        switch (e.getAction()) {
            case MotionEvent.ACTION_DOWN:
                // 首先拦截down事件,记录y坐标
                mLastMotionY = y;
                break;
            case MotionEvent.ACTION_MOVE:
                // deltaY > 0 是向下运动,< 0是向上运动
                int deltaY = y - mLastMotionY;
                if (isRefreshViewScroll(deltaY)) {
                    return true;
                }
                break;
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_CANCEL:
                break;
        }
        return false;
    }

    /*
     * 如果在onInterceptTouchEvent()方法中没有拦截(即onInterceptTouchEvent()方法中 return
     * false)则由PullToRefrshLinearLayout 的子View来处理;否则由下面的方法来处理(即由PullToRefrshLinearLayout自己来处理)
     */
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        // if (mLock) {
        // return true;
        // }
        int y = (int) event.getRawY();
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                // onInterceptTouchEvent已经记录
                // mLastMotionY = y;
                break;
            case MotionEvent.ACTION_MOVE:
                int deltaY = y - mLastMotionY;
                if (mPullState == PULL_DOWN_STATE) {// 执行下拉
                    if (!mHideHeader)
                        headerPrepareToRefresh(deltaY);
                    // setHeaderPadding(-mHeaderViewHeight);
                } else if (mPullState == PULL_UP_STATE) {// 执行上拉
                    //Logger.e("执行上拉");
                    //setHeaderTopMargin(-mHeaderViewHeight);
                    headerPrepareToRefresh(deltaY);
                } else if (mPullState == PULL_UP_XCX) {
                    changingHeaderViewTopMargin(deltaY);
                }
                mLastMotionY = y;
                break;
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_CANCEL:
                int topMargin = getHeaderTopMargin();
                if (mPullState == PULL_DOWN_STATE) {
                    Logger.e("下拉"+topMargin);
                   if (-topMargin<=2*mHeaderViewHeight/3){
                       setHeaderTopMargin(0);//显示
                       headerRefreshing();
                       mHeaderState = RELEASE_TO_XCX;
                   }else {
                       setHeaderTopMargin(-mHeaderViewHeight);//隐藏
                   }
                } else if (mPullState == PULL_UP_STATE) {
                    Logger.e("上推"+topMargin+"---"+mHeaderViewHeight);
                   if (-topMargin>=mHeaderViewHeight/3){
                       setHeaderTopMargin(-mHeaderViewHeight);  //隐藏
                   }else {
                       setHeaderTopMargin(0);
                       mHeaderState = RELEASE_TO_XCX;
                   }

                }
                break;
        }
        return super.onTouchEvent(event);
    }

    /**
     * 是否应该到了父View,即PullToRefrshLinearLayout滑动
     *
     * @param deltaY , deltaY > 0 是向下运动,< 0是向上运动
     * @return
     */
    private boolean isRefreshViewScroll(int deltaY) {
       /* if (mHeaderState == REFRESHING ) {
            return false;
        }*/
        // 对于ListView和GridView
        if (mAdapterView != null) {
            // 子view(ListView or GridView)滑动到最顶端
            if (deltaY > 0) {

                View child = mAdapterView.getChildAt(0);
                if (child == null) {
                    // 只有当列表在顶部的时候才拦截数据
                    return false;
                }
                if (mAdapterView.getFirstVisiblePosition() == 0
                        && child.getTop() == 0) {
                    mPullState = PULL_DOWN_STATE;
                    return true;
                }
                int top = child.getTop();
                int padding = mAdapterView.getPaddingTop();
                if (mAdapterView.getFirstVisiblePosition() == 0
                        && Math.abs(top - padding) <= 8) {// 这里之前用3可以判断,但现在不行,还没找到原因
                    mPullState = PULL_DOWN_STATE;
                    return true;
                }

            } else if (deltaY < 0) {        //上推
                if (mHeaderState == RELEASE_TO_XCX) {          //如果小程序栏在显示中
                    View child = mAdapterView.getChildAt(0);
                    if (child == null) {
                        // 只要小程序栏在显示 就拦截，只有当小程序栏隐藏了才继续滑动列表
                        return true;
                    }
                    if (mAdapterView.getFirstVisiblePosition() == 0
                            && child.getTop() == 0) {
                        mPullState = PULL_UP_STATE;
                        return true;
                    }
                    int top = child.getTop();
                    int padding = mAdapterView.getPaddingTop();
                    if (mAdapterView.getFirstVisiblePosition() == 0
                            && Math.abs(top - padding) <= 8) {// 这里之前用3可以判断,但现在不行,还没找到原因
                        mPullState = PULL_DOWN_STATE;
                        return true;
                    }
                }

                View lastChild = mAdapterView.getChildAt(mAdapterView
                        .getChildCount() - 1);
                if (lastChild == null) {
                    // 如果mAdapterView中没有数据,不拦截
                    Logger.e("不拦截？");
                    return false;
                }
                // 最后一个子view的Bottom小于父View的高度说明mAdapterView的数据没有填满父view,
                // 等于父View的高度说明mAdapterView已经滑动到最后
                if (lastChild.getBottom() <= getHeight()
                        && mAdapterView.getLastVisiblePosition() == mAdapterView
                        .getCount() - 1) {
                    mPullState = PULL_UP_STATE;
                    return true;
                }

            }
        }
        // 对于ScrollView
        if (mScrollView != null) {
            // 子scroll view滑动到最顶端
            View child = mScrollView.getChildAt(0);
            if (deltaY > 0 && mScrollView.getScrollY() == 0) {
                mPullState = PULL_DOWN_STATE;
                return true;
            } else if (deltaY < 0
                    && child.getMeasuredHeight() <= getHeight()
                    + mScrollView.getScrollY()) {
                mPullState = PULL_UP_STATE;
                return true;
            }
        }
        return false;
    }

    /**
     * header 准备刷新,手指移动过程,还没有释放
     *
     * @param deltaY ,手指滑动的距离
     */
    private void headerPrepareToRefresh(int deltaY) {
        int newTopMargin = changingHeaderViewTopMargin(deltaY);
      //  Logger.e(":"+newTopMargin);
        // 当header view的topMargin>=0时，说明已经完全显示出来了,修改header view 的提示状态
       /* if (newTopMargin >= -mHeaderViewHeight / 2 && newTopMargin < -mHeaderViewHeight / 3 && mHeaderState != RELEASE_TO_REFRESH) {
            mHeaderState = RELEASE_TO_REFRESH;
        } else if (newTopMargin < -mHeaderViewHeight / 2 && newTopMargin > -mHeaderViewHeight) {// 拖动时没有释放
            mHeaderState = PULL_TO_REFRESH;
        } else if (newTopMargin >= -mHeaderViewHeight / 3) {
            mHeaderState = RELEASE_TO_XCX;
        }*/
    }


    /**
     * 修改Header view top margin的值
     *
     * @param deltaY
     */
    private int changingHeaderViewTopMargin(int deltaY) {
        LayoutParams params = (LayoutParams) mHeaderView.getLayoutParams();
        float newTopMargin = params.topMargin + deltaY * 0.3f;
        // 这里对上拉做一下限制,因为当前上拉后然后不释放手指直接下拉,会把下拉刷新给触发了,感谢网友yufengzungzhe的指出
        // 表示如果是在上拉后一段距离,然后直接下拉
        if (deltaY > 0 && mPullState == PULL_UP_STATE
                && Math.abs(params.topMargin) <= mHeaderViewHeight) {
            return params.topMargin;
        }
        // 同样地,对下拉做一下限制,避免出现跟上拉操作时一样的bug
        if (deltaY < 0 && mPullState == PULL_DOWN_STATE
                && Math.abs(params.topMargin) >= mHeaderViewHeight) {
            return params.topMargin;
        }
        params.topMargin = (int) newTopMargin;
        mHeaderView.setLayoutParams(params);
        invalidate();
        return params.topMargin;
    }


    /**
     * header refreshing
     */
    private void headerRefreshing() {
        // mHeaderState = REFRESHING;
        setHeaderTopMargin(0);
        mHeaderState = RELEASE_TO_XCX;
        //mHeaderTextView.setText("正在刷新...");
        if (mOnHeaderRefreshListener != null) {
            mOnHeaderRefreshListener.onHeaderRefresh(this);
        }
    }


    /**
     * 设置header view 的topMargin的值
     *
     * @param topMargin ，为0时，说明header view 刚好完全显示出来； 为-mHeaderViewHeight时，说明完全隐藏了
     */
    private void setHeaderTopMargin(int topMargin) {
        if (topMargin == 0) {
            mOnHeaderRefreshListener.hasShowHeader();      //当头部完全显示时，通过回调函数通知界面刷新数据
        }
        LayoutParams params = (LayoutParams) mHeaderView.getLayoutParams();
        params.topMargin = topMargin;
        mHeaderView.setLayoutParams(params);
        invalidate();
    }

    /**
     * header view 完成更新后恢复初始状态
     */
    public void onHeaderRefreshComplete() {
        setHeaderTopMargin(-mHeaderViewHeight);
        //mHeaderTextView.setText("下拉刷新");
        mHeaderState = PULL_TO_REFRESH;

    }

    /**
     * Resets the list to a normal state after a refresh.
     *
     * @param lastUpdated Last updated at.
     */
    public void onHeaderRefreshComplete(CharSequence lastUpdated) {
        setLastUpdated(lastUpdated);
        onHeaderRefreshComplete();
    }


    /**
     * Set a text to represent when the list was last updated.
     *
     * @param lastUpdated Last updated at.
     */
    public void setLastUpdated(CharSequence lastUpdated) {
        if (lastUpdated != null) {

        } else {

        }
    }

    /**
     * 获取当前header view 的topMargin
     */
    private int getHeaderTopMargin() {
        LayoutParams params = (LayoutParams) mHeaderView.getLayoutParams();
        return params.topMargin;
    }

    /**
     * set headerRefreshListener
     *
     * @param headerRefreshListener
     */
    public void setOnHeaderRefreshListener(
            OnHeaderRefreshListener headerRefreshListener) {
        mOnHeaderRefreshListener = headerRefreshListener;
    }


    /**
     * Interface definition for a callback to be invoked when list/grid footer
     * view should be refreshed.
     */
    public interface OnFooterRefreshListener {
        public void onFooterRefresh(PullToRefrshLinearLayout view);
    }

    /**
     * Interface definition for a callback to be invoked when list/grid header
     * view should be refreshed.
     */
    public interface OnHeaderRefreshListener {
        public void onHeaderRefresh(PullToRefrshLinearLayout view);

        void hasShowHeader();
    }


    public void hideHeaderView() {
        mHideHeader = true;
    }


    public void showHeaderView() {
        mHideHeader = false;
    }


}
