package ym.example.com.ymandroidclient.adapter;

import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentPagerAdapter;


import java.util.List;

import ym.example.com.ymandroidclient.utility.Logger;

/**
 * ViewPager+Fragment的适配器
 */
public class XFragmentPageAdapter extends FragmentPagerAdapter {
    FragmentManager fragmentManager;
    List<Fragment> mList;

    public XFragmentPageAdapter(FragmentManager fm, List<Fragment>list) {
        super(fm);
        this.mList=list;
    }

    @Override
    public Fragment getItem(int i) {
        Logger.i("getItem当生成Fragment时调用？");
        return mList.get(i);
    }

    @Override
    public int getCount() {
        return mList.size();
    }
}
