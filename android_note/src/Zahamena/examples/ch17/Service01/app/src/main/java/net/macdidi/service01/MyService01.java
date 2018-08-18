package net.macdidi.service01;

import android.app.Service;
import android.content.Intent;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.widget.Toast;

public class MyService01 extends Service
{

    private Handler handlerHello;

    @Override
    public IBinder onBind(Intent intent)
    {
        return null;
    }

    @Override
    public void onCreate()
    {
        handlerHello = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                Toast.makeText(getApplicationContext(),
                               "Hello! Service 01", Toast.LENGTH_SHORT).show();
                handlerHello.sendEmptyMessageDelayed(0, 5000);
            }
        };
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        handlerHello.sendEmptyMessage(0);
        return Service.START_STICKY;
    }

    @Override
    public void onDestroy()
    {
        handlerHello.removeMessages(0);
    }
}
