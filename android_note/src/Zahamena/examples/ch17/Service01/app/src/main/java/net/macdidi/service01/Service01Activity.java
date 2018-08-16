package net.macdidi.service01;

import android.app.ActivityManager;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.Switch;

import java.util.List;

public class Service01Activity extends AppCompatActivity
{

    private Switch service01_switch, service02_switch;

    private MyService03 myService03;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_service01);

        processViews();
        processControllers();

        startService(new Intent(Service01Activity.this, MyService03.class));

        // 建立與實作需要的ServiceConnection物件
        ServiceConnection connection = new ServiceConnection() {

            // 建立連結，讀取與設定服務元件物件
            // 第一個參數是服務元件
            // 第二個參數是服務元件在onBind傳回的IBinder物件
            @Override
            public void onServiceConnected(ComponentName className,
                                           IBinder service) {
                // 把IBinder參數轉型為自己在服務元件類別中實作的型態
                MyService03.MyBinder myBinder = (MyService03.MyBinder) service;
                // 取得服務元件物件
                myService03 = myBinder.getService();
            }

            // 失去連結
            @Override
            public void onServiceDisconnected(ComponentName className) {
                myService03 = null;
            }

        };

        // 建立連結服務元件用的Intent物件
        Intent bindIntent = new Intent(Service01Activity.this, MyService03.class);
        // 呼叫bindService方法，指定Intent、ServiceConnection與連結方式
        // Android會呼叫ServiceConnection物件的onServiceConnected方法
        bindService(bindIntent, connection, Context.BIND_AUTO_CREATE);
    }

    public void clickRestart(View view)
    {
        // 呼叫宣告在服務元件類別的方法
        myService03.setCountDown(10);
    }

    private void processViews()
    {
        service01_switch = (Switch) findViewById(R.id.service01_switch);
        service02_switch = (Switch) findViewById(R.id.service02_switch);
        configSwitch();
    }

    private void processControllers()
    {
        CompoundButton.OnCheckedChangeListener listener =
        new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView,
                                         boolean isChecked) {
                int id = buttonView.getId();
                Intent intent = null;

                if (id == R.id.service01_switch) {
                    // 建立啟動MyService01服務元件的Intent物件
                    intent = new Intent(Service01Activity.this,
                                        MyService01.class);
                } else if (id == R.id.service02_switch) {
                    // 使用指定Action名稱的方式，建立啟動MyService02服務元件的Intent物件
                    intent = new Intent(MyService02.ACTION_MYSERVICE02);
                    // 加入需要傳送給服務元件的資料
                    intent.putExtra("period", 10000);
                }

                // 從Android 5、API level 21開始，
                //    啟動服務元件必須加入下列的敘述
                intent.setPackage("net.macdidi.service01");

                if (isChecked) {
                    // 啟動服務
                    startService(intent);
                } else {
                    // 停止服務
                    stopService(intent);
                }
            }
        };

        service01_switch.setOnCheckedChangeListener(listener);
        service02_switch.setOnCheckedChangeListener(listener);
    }

    private void configSwitch()
    {
        // 根據服務元件是否運作中設定Switch元件
        service01_switch.setChecked(isRunning(MyService01.class));
        service02_switch.setChecked(isRunning(MyService02.class));
    }

    // 判斷參數指定的元件類別是否正在運作中
    private boolean isRunning(Class target)
    {
        boolean result = false;

        // 取得系統ActivityManager物件
        ActivityManager am = (ActivityManager)
                             getSystemService(Context.ACTIVITY_SERVICE);

        // 取得正在運作中的服務元件
        List<ActivityManager.RunningServiceInfo> rs =
            am.getRunningServices(20);

        // 建立參數指定服務元件的ComponentName物件
        ComponentName cn = new ComponentName(this, target);

        // 比對與判斷服務元件是否在運作中
        for (ActivityManager.RunningServiceInfo rsi : rs) {
            if (rsi.service.equals(cn)) {
                result = true;
                break;
            }
        }

        return result;
    }

}
