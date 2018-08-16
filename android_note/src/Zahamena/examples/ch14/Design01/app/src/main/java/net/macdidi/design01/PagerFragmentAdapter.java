package net.macdidi.design01;

import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentStatePagerAdapter;

import java.util.List;
// ViewPager 分頁容器使用的 Adapter 類別
public class PagerFragmentAdapter extends FragmentStatePagerAdapter
{

    // 儲存分頁 Fragment 的 List 物件
    private List<Fragment> fragments;
    // 儲存分頁名稱的 List 物件
    private List<String> titles;

    public PagerFragmentAdapter(FragmentManager fm, List<Fragment> fragments,
                                List<String> titles)
    {
        super(fm);
        this.fragments = fragments;
        this.titles = titles;
    }

    @Override
    public Fragment getItem(int position)
    {
        return fragments.get(position);
    }

    @Override
    public int getCount()
    {
        return fragments.size();
    }

    @Override
    public CharSequence getPageTitle(int position)
    {
        return titles.get(position);
    }
}
