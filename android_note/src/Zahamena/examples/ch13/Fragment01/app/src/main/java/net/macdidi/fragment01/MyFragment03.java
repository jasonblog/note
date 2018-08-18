package net.macdidi.fragment01;

import android.content.Context;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

public class MyFragment03 extends Fragment
{

    private static final String TAG = "MyFragment03";
    private static final boolean isLog = true;

    @Override
    public void onAttach(Context context)
    {
        super.onAttach(context);
        myLog("onAttach");
    }

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        myLog("onCreate");
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState)
    {
        myLog("onCreateView");
        return inflater.inflate(R.layout.fragment_myfragment03,
                                container, false);
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState)
    {
        super.onActivityCreated(savedInstanceState);
        myLog("onActivityCreated");
    }

    @Override
    public void onStart()
    {
        super.onStart();
        myLog("onStart");
    }

    @Override
    public void onResume()
    {
        super.onResume();
        myLog("onResume");
    }

    @Override
    public void onPause()
    {
        myLog("onPause");
        super.onPause();
    }

    @Override
    public void onStop()
    {
        myLog("onStop");
        super.onStop();
    }

    @Override
    public void onDestroyView()
    {
        myLog("onDestroyView");
        super.onDestroyView();
    }

    @Override
    public void onDestroy()
    {
        myLog("onDestroy");
        super.onDestroy();
    }

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