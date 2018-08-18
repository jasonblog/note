package net.macdidi.fragment01;

import android.os.Bundle;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.FrameLayout;

public class Fragment01Activity extends AppCompatActivity
{

    private static final String TAG = "Fragment01Activity";
    private static final boolean isLog = true;

    private boolean added = false;

    private FragmentManager manager;
    private MyFragment03 myFragment03;
    private FrameLayout container;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_fragment01);
        myLog("onCreate");

        // 讀取放置MyFragment03的畫面元件
        container = (FrameLayout) findViewById(R.id.container);

        // 為了支援Support Library，
        // 所以呼叫getSupportFragmentManager方法，
        // 取得FragmentManager物件
        manager = getSupportFragmentManager();

        // 建立MyFragment03物件
        myFragment03 = new MyFragment03();
    }

    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        if (event.getAction() == MotionEvent.ACTION_DOWN) {

            // 取得Support Library的FragmentTransaction物件
            FragmentTransaction transaction = manager.beginTransaction();

            // 如果已經加入
            if (added) {
                // 移除MyFragment03
                transaction.remove(myFragment03);
                // 讓元件消失
                container.setVisibility(View.GONE);
            }
            // 如果還沒有加入
            else {
                // 加入MyFragment03到指定的畫面元件
                transaction.add(R.id.container, myFragment03);
                // 顯示元件
                container.setVisibility(View.VISIBLE);
            }

            // 確認執行
            transaction.commit();

            // 設定為已加入與未加入
            added = !added;
        }

        return true;
    }

    @Override
    protected void onRestart()
    {
        super.onRestart();
        myLog("onRestart");
    }

    @Override
    protected void onStart()
    {
        super.onStart();
        myLog("onStart");
    }

    @Override
    protected void onResume()
    {
        super.onResume();
        myLog("onResume");
    }

    @Override
    protected void onPause()
    {
        myLog("onPause");

        super.onPause();
    }

    @Override
    protected void onStop()
    {
        myLog("onStop");
        super.onStop();
    }

    @Override
    protected void onDestroy()
    {
        myLog("onDestroy");
        super.onDestroy();
    }

    private void myLog(String message)
    {
        if (isLog) {
            Log.d(TAG, message);
        }
    }

}
