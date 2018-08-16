package net.macdidi.service01;

import android.app.Service;
import android.content.Intent;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.widget.Toast;

public class MyService02 extends Service
{

    public static final String ACTION_MYSERVICE02 =
        "net.macdidi.service01.action.MYSERVICE02";

    private Handler handlerHello;
    private int period;

    // 提供別的元件取得這個服務元件物件，使用它執行一些工作
    // 參數是執行Binding工作的Intent物件
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
                               "Hello! Service 02", Toast.LENGTH_SHORT).show();
                handlerHello.sendEmptyMessageDelayed(0, period);
            }
        };
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        period = intent.getIntExtra("period", 3000);
        handlerHello.sendEmptyMessage(0);
        return Service.START_STICKY;
    }

    @Override
    public void onDestroy()
    {
        handlerHello.removeMessages(0);
    }

}
