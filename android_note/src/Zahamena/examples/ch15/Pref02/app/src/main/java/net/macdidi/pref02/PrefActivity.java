package net.macdidi.pref02;

import android.preference.PreferenceActivity;

import java.util.List;

// 繼承自PreferenceActivity類別
public class PrefActivity extends PreferenceActivity
{

    // 載入Preference Header
    @Override
    public void onBuildHeaders(List<Header> target)
    {
        super.onBuildHeaders(target);
        // 載入指定的Preference Header資源
        loadHeadersFromResource(R.xml.pref_headers, target);
    }

    // Android 4.4、API level 19加入的方法
    // API level 19以後的版本必須覆寫這個方法，
    //    檢查使用的Fragment是否有效
    @Override
    protected boolean isValidFragment(String fragmentName)
    {
        return PrefFragment.class.getName().equals(fragmentName);
    }

}
