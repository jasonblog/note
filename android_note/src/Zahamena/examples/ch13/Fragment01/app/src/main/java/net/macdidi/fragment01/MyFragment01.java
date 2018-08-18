package net.macdidi.fragment01;

import android.content.Context;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

// 繼承自Support Library的Fragment類別
public class MyFragment01 extends Fragment
{

    private static final String TAG = "MyFragment01";
    private static final boolean isLog = true;

    // Fragment加入Activity元件，參數是使用Fragment的Activity元件
    @Override
    public void onAttach(Context context)
    {
        super.onAttach(context);
        myLog("onAttach");
    }

    // 準備建立Fragment元件
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        // 執行Fragment元件初始化的工作
        myLog("onCreate");
    }

    // 準備建立Fragment元件的畫面
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState)
    {
        // 載入與回傳Fragment元件的畫面物件
        myLog("onCreateView");
        // 第一個參數是Fragment元件使用的畫面配置資源
        // 第二個參數是這個方法的ViewGroup參數
        // 第三個參數設定為false
        return inflater.inflate(R.layout.fragment_myfragment01,
                                container, false);
    }

    // 使用Fragment的Activity元件已經建立完成
    @Override
    public void onActivityCreated(Bundle savedInstanceState)
    {
        super.onActivityCreated(savedInstanceState);
        // Fragment與Activity元件都已經建立完成
        // 從這裡開始可以與Activity元件物件互動，
        // 例如呼叫getActivity()取得Activity元件物件，使用它執行一些工作
        myLog("onActivityCreated");
    }

    @Override
    public void onStart()
    {
        super.onStart();
        // 適合在這裡設定畫面的內容
        myLog("onStart");
    }

    @Override
    public void onResume()
    {
        super.onResume();
        // 啟動或回復元件需要的工作與執行緒
        myLog("onResume");
    }

    @Override
    public void onPause()
    {
        myLog("onPause");
        // 停止執行中的工作與執行緒
        super.onPause();
    }

    @Override
    public void onStop()
    {
        myLog("onStop");
        // 停止執行中的工作與執行緒
        super.onStop();
    }

    @Override
    public void onDestroyView()
    {
        myLog("onDestroyView");
        // 清除與畫面元件相關的資源
        super.onDestroyView();
    }

    @Override
    public void onDestroy()
    {
        myLog("onDestroy");
        // 清除所有元件建立的資源
        super.onDestroy();
    }

    // Fragment從加入的Activity元件中移除
    @Override
    public void onDetach()
    {
        myLog("onDetach");
        super.onDetach();
    }

    private void myLog(String message)
    {
        if (isLog) {
            Log.d(TAG, message);
        }
    }
}