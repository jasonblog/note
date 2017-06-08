package com.archer.rainbow;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

public class MyService extends Service
{
    private static final String TAG = "MyService";

    @Override
    public void onCreate()
    {
        super.onCreate();
        MyServiceActivity.updateLog(TAG + " ----> onCreate()");
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        MyServiceActivity.updateLog(TAG + " ----> onStartCommand()");
        return START_STICKY;
    }

    @Override
    public IBinder onBind(Intent intent)
    {
        MyServiceActivity.updateLog(TAG + " ----> onBind()");
        return null;
    }

    @Override
    public void onDestroy()
    {
        super.onDestroy();
        MyServiceActivity.updateLog(TAG + " ----> onDestroy()");
    }

    @Override
    public void onRebind(Intent intent)
    {
        super.onRebind(intent);
        MyServiceActivity.updateLog(TAG + " ----> onRebind()");
    }

    @Override
    public void onStart(Intent intent, int startId)
    {
        super.onStart(intent, startId);
        MyServiceActivity.updateLog(TAG + " ----> onStart()");
    }

    @Override
    public boolean onUnbind(Intent intent)
    {
        MyServiceActivity.updateLog(TAG + " ----> onUnbind()");
        return super.onUnbind(intent);
    }

}
