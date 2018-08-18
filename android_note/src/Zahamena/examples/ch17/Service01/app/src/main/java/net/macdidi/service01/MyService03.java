package net.macdidi.service01;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.widget.Toast;

public class MyService03 extends Service
{

    private Handler handlerHello;
    private int countDown = 10;

    // 提供別的元件取得這個服務元件物件，使用它執行一些工作
    // 參數是執行Binding工作的Intent物件
    @Override
    public IBinder onBind(Intent intent)
    {
        // 傳回可以取得服務元件物件的IBinder物件
        return new MyBinder();
    }

    // 實作自己需要的IBinder物件，Binder是實作IBinder介面的類別
    public class MyBinder extends Binder
    {
        // 加入自己宣告的方法，傳回這個服務元件的物件
        public MyService03 getService()
        {
            return MyService03.this;
        }
    }

    @Override
    public void onCreate()
    {
        handlerHello = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                Toast.makeText(getApplicationContext(),
                               "Hello! Service 03: " + countDown, Toast.LENGTH_SHORT).show();
                countDown--;

                if (countDown >= 0) {
                    handlerHello.sendEmptyMessageDelayed(0, 3000);
                } else {
                    Toast.makeText(getApplicationContext(),
                                   "Hello! Service 03: Stop", Toast.LENGTH_SHORT).show();
                    stopSelf();
                }
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

    public void setCountDown(int countDown)
    {
        this.countDown = countDown;
    }
}
