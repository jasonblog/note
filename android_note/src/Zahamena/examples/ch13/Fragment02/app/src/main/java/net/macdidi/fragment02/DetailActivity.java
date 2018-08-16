package net.macdidi.fragment02;

import android.content.res.Configuration;
import android.os.Bundle;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.support.v7.app.AppCompatActivity;

public class DetailActivity extends AppCompatActivity
{

    // 負責顯示詳細資料的Fragment
    private DetailFragment detailFragment;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        // 如果裝置的方向是橫式
        if (getResources().getConfiguration().orientation
            == Configuration.ORIENTATION_LANDSCAPE) {
            // 結束後回到主畫面元件
            finish();
            return;
        }

        setContentView(R.layout.activity_detail);
        processViews();
    }

    @Override
    protected void onResume()
    {
        super.onResume();
        // 讀取選擇項目的編號
        int position = getIntent().getIntExtra("position", -1);

        if (position != -1) {
            // 設定元件的標題
            setTitle(DataSet.rpis[position]);
            // 通知DetailFragment元件更新內容
            detailFragment.updateDetail(position);
        }
    }

    private void processViews()
    {
        // 為了支援Support Library，
        // 所以呼叫getSupportFragmentManager方法，
        // 取得FragmentManager物件
        FragmentManager manager = getSupportFragmentManager();
        // 取得Support Library的FragmentTransaction物件
        FragmentTransaction transaction = manager.beginTransaction();
        // 建立DetailFragment物件
        detailFragment = new DetailFragment();
        // 加入DetailFragment到指定的畫面元件
        transaction.add(R.id.detail_container, detailFragment);
        // 確認執行
        transaction.commit();
    }

}