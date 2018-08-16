package net.macdidi.design01;

import android.os.Bundle;
import android.support.design.widget.CollapsingToolbarLayout;
import android.support.design.widget.Snackbar;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.View;

public class ShoppingCartActivity extends AppCompatActivity
{

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_shopping_cart);

        // 建立與設定 Toolbar 物件
        Toolbar toolbar = (Toolbar) this.findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        // 取得 ActionBar 物件
        ActionBar actionBar = getSupportActionBar();

        if (actionBar != null) {
            // 設定返回按鈕
            actionBar.setDisplayHomeAsUpEnabled(true);
        }

        // 讀取折疊 Toolbar 物件
        CollapsingToolbarLayout collapsingToolbar =
            (CollapsingToolbarLayout) findViewById(R.id.collapsing_toolbar);
        // 設定折疊 Toolbar 的標題
        collapsingToolbar.setTitle("Shopping Cart");
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item)
    {
        // 如果選擇 app bar 左側的返回鍵
        if (item.getItemId() == android.R.id.home) {
            // 結束元件
            finish();
            return true;
        } else {
            return super.onOptionsItemSelected(item);
        }
    }

    public void checkout(View view)
    {
        Snackbar.make(view, "Checkout!", Snackbar.LENGTH_SHORT).show();
    }

}
