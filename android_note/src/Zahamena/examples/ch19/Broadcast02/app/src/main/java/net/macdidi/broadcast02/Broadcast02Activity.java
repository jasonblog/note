package net.macdidi.broadcast02;

import android.Manifest;
import android.app.ActivityManager;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Switch;
import android.widget.Toast;

import java.util.List;

public class Broadcast02Activity extends AppCompatActivity
{

    private Switch timetick_service_switch, boot_service_switch,
            phonestate_service_switch, wifistate_service_switch;

    private TimeTickReceiver timeTickReceiver;
    private IntentFilter filter;
    private Intent intentTimeTickService;
    private Intent intentBootCompletedService;
    private Intent intentPhoneStateService;
    private Intent intentWifiStateService;

    // 請求授權的代碼
    private static final int REQUEST_PHONE_STATE_PERMISSION = 100;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_broadcast02);

        processViews();
        // 準備註冊與移除廣播接收元件的IntentFilter物件
        filter = new IntentFilter(Intent.ACTION_TIME_TICK);

        // 建立啟動服務的Intent物件
        intentTimeTickService = new Intent(this, TimeTickService.class);
        intentBootCompletedService =
            new Intent(this, BootCompletedService.class);
        intentPhoneStateService =
            new Intent(this, PhoneStateService.class);
        intentWifiStateService =
            new Intent(this, WifiStateService.class);
    }

    @Override
    protected void onResume()
    {
        super.onResume();

        // 註冊廣播接收元件
        if (timeTickReceiver != null) {
            registerReceiver(timeTickReceiver, filter);
        }
    }

    @Override
    protected void onPause()
    {
        // 移除廣播接收元件
        if (timeTickReceiver != null) {
            unregisterReceiver(timeTickReceiver);
        }

        super.onPause();
    }

    public void clickTimeTickSwitch(View view)
    {
        Switch switchView = (Switch) view;

        if (switchView.isChecked()) {
            // 建立廣播接收元件物件
            if (timeTickReceiver == null) {
                timeTickReceiver = new TimeTickReceiver();
            }

            // 註冊廣播接收元件
            registerReceiver(timeTickReceiver, filter);
        } else {
            if (timeTickReceiver != null) {
                // 移除廣播接收元件
                unregisterReceiver(timeTickReceiver);
                timeTickReceiver = null;
            }
        }
    }

    public void clickTimeTickServiceSwitch(View view)
    {
        if (timetick_service_switch.isChecked()) {
            // 啟動服務
            startService(intentTimeTickService);
        } else {
            // 停止服務
            stopService(intentTimeTickService);
        }
    }

    public void clickBootServiceSwitch(View view)
    {
        if (boot_service_switch.isChecked()) {
            // 啟動服務
            startService(intentBootCompletedService);
        } else {
            // 停止服務
            stopService(intentBootCompletedService);
        }
    }

    public void clickPhoneStateServiceSwitch(View view)
    {
        if (phonestate_service_switch.isChecked()) {
            requestPhoneStatePermission();
        }
    }

    public void clickWifiStateServiceSwitch(View view)
    {
        if (wifistate_service_switch.isChecked()) {
            // 啟動服務
            startService(intentWifiStateService);
        } else {
            // 停止服務
            stopService(intentWifiStateService);
        }
    }

    private void processViews()
    {
        timetick_service_switch = (Switch) findViewById(R.id.timetick_service_switch);
        boot_service_switch = (Switch) findViewById(R.id.boot_service_switch);
        phonestate_service_switch = (Switch) findViewById(
                                        R.id.phonestate_service_switch);
        wifistate_service_switch = (Switch) findViewById(R.id.wifistate_service_switch);

        timetick_service_switch.setChecked(isRunning(TimeTickService.class));
        boot_service_switch.setChecked(isRunning(BootCompletedService.class));
        phonestate_service_switch.setChecked(isRunning(PhoneStateService.class));
        wifistate_service_switch.setChecked(isRunning(WifiStateService.class));
    }

    // 判斷參數指定的元件類別是否正在運作中
    private boolean isRunning(Class target)
    {
        // 取得系統ActivityManager物件
        ActivityManager am = (ActivityManager)
                             getSystemService(Context.ACTIVITY_SERVICE);

        // 取得正在運作中的服務元件
        List<ActivityManager.RunningServiceInfo> rs =
            am.getRunningServices(Integer.MAX_VALUE);

        // 比對與判斷服務元件是否在運作中
        for (ActivityManager.RunningServiceInfo rsi : rs) {
            if (target.getName().equals(rsi.service.getClassName())) {
                return true;
            }
        }

        return false;
    }

    private void requestPhoneStatePermission()
    {
        // 如果裝置版本是6.0（包含）以上
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            // 取得授權狀態，參數是請求授權的名稱
            int hasPermission = checkSelfPermission(
                                    Manifest.permission.READ_PHONE_STATE);

            // 如果未授權
            if (hasPermission != PackageManager.PERMISSION_GRANTED) {
                // 請求授權
                //     第一個參數是請求授權的名稱
                //     第二個參數是請求代碼
                requestPermissions(
                    new String[] {Manifest.permission.READ_PHONE_STATE},
                    REQUEST_PHONE_STATE_PERMISSION);
                return;
            }
        }

        // 如果裝置版本是6.0以下，
        // 或是裝置版本是6.0（包含）以上，使用者已經授權，
        processPhoneState();
    }

    // 使用者完成授權的選擇以後，Android會呼叫這個方法
    //     第一個參數：請求授權代碼
    //     第二個參數：請求的授權名稱
    //     第三個參數：使用者選擇授權的結果
    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           String[] permissions,
                                           int[] grantResults)
    {
        // 如果是撥電話授權請求
        if (requestCode == REQUEST_PHONE_STATE_PERMISSION) {
            // 如果在授權請求選擇「允許」
            if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {

                processPhoneState();
            }
            // 如果在授權請求選擇「拒絕」
            else {
                // 顯示沒有授權的訊息
                Toast.makeText(this, "沒有讀取電話狀態授權",
                               Toast.LENGTH_SHORT).show();
                phonestate_service_switch.setChecked(false);
            }
        } else {
            super.onRequestPermissionsResult(requestCode, permissions,
                                             grantResults);
        }
    }

    private void processPhoneState()
    {
        if (phonestate_service_switch.isChecked()) {
            // 啟動服務
            startService(intentPhoneStateService);
        } else {
            // 停止服務
            stopService(intentPhoneStateService);
        }
    }

}
