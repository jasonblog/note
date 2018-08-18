package net.macdidi.sensor01;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.hardware.camera2.CameraAccessException;
import android.os.Build;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.Toast;

public class Sensor01Activity extends AppCompatActivity
    implements FlashControl.ReadyCallBack
{

    // 啟動與關閉偵測、閃光燈亮度開關與目前亮度
    private Switch detect;
    private SeekBar sensor_value;
    private ProgressBar light_value;

    // 閃光燈開、關圖示
    private ImageView flash_switch;

    // 感應設備管理員物件
    private SensorManager manager;
    // 環境亮度感應設備
    private Sensor lightSensor;
    // 環境亮度感應監聽物件
    private LightListener lightListener;

    // 閃光燈控制物件
    private FlashControl flashControl;

    // 是否在偵測狀態
    private boolean isDetect = false;

    // 請求授權使用的請求代碼
    private static final int REQUEST_CAMERA_PERMISSION = 100;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sensor01);

        processViews();
        processControllers();

        // 取得感應設備管理員物件
        manager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        // 取得環境亮度感應設備
        lightSensor = manager.getDefaultSensor(Sensor.TYPE_LIGHT);
        // 建立感應設備改變監聽物件
        lightListener = new LightListener();
    }

    private void processViews()
    {
        // 啟動與關閉偵測元件
        detect = (Switch) findViewById(R.id.detect);
        // 設定自動開啟或關閉閃光燈亮度用的SeekBar元件
        sensor_value = (SeekBar) findViewById(R.id.sensor_value);
        // 顯示目前環境亮度
        light_value = (ProgressBar) findViewById(R.id.light_value);
        // 閃光燈開、關圖示
        flash_switch = (ImageView) findViewById(R.id.flash_switch);
    }

    private void processControllers()
    {
        // 註冊啟動與關閉偵測元件監聽事件
        detect.setOnCheckedChangeListener(
        new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView,
                                         boolean isChecked) {
                // 設定是否在偵測狀態
                isDetect = isChecked;

                // 如果設定為偵測狀態
                if (isDetect) {
                    // 請求授權
                    requestCameraPermission();
                } else {
                    // 移除感應設備改變監聽事件
                    manager.unregisterListener(lightListener);

                    if (flashControl != null) {
                        // 關閉閃光燈
                        flashControl.turnOff();
                    }

                    // 設定閃光燈圖示為關閉
                    setFlashSwitch(false);
                }
            }
        });
    }

    @Override
    protected void onPause()
    {
        // 移除感應設備改變監聽事件
        manager.unregisterListener(lightListener);

        if (flashControl != null) {
            // 關閉閃光燈
            flashControl.turnOff();
        }

        super.onPause();
    }

    @Override
    protected void onDestroy()
    {
        if (flashControl != null) {
            // 關閉與清除資源
            flashControl.close();
        }

        super.onDestroy();
    }

    @Override
    public void ready()
    {
        // 註冊感應設備改變監聽事件
        manager.registerListener(lightListener, lightSensor,
                                 SensorManager.SENSOR_DELAY_UI);
    }

    // 感應設備資訊改變監聽類別
    private class LightListener implements SensorEventListener
    {

        @Override
        public void onAccuracyChanged(Sensor sensor, int value)
        {

        }

        @Override
        public void onSensorChanged(SensorEvent event)
        {
            // 讀取環境亮度數值
            float light = event.values[0];

            // 顯示目前環境亮度
            light_value.setProgress((int) light);

            if (flashControl != null) {
                // 如果目前亮度小於使用者設定的亮度
                if (light < sensor_value.getProgress()) {
                    // 開啟閃光燈
                    flashControl.turnOn();
                    // 設定閃光燈圖示為開啟
                    setFlashSwitch(true);
                } else {
                    // 關閉閃光燈
                    flashControl.turnOff();
                    // 設定閃光燈圖示為關閉
                    setFlashSwitch(false);
                }
            }
        }
    }

    // 設定閃光燈圖示狀態
    private void setFlashSwitch(boolean isTurnOn)
    {
        flash_switch.setImageResource(isTurnOn ?
                                      R.drawable.android_oreo_light :
                                      R.drawable.android_oreo_dark
                                     );
    }

    private void requestCameraPermission()
    {
        // 如果裝置版本是6.0（包含）以上
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            // 取得授權狀態，參數是請求授權的名稱
            int hasPermission = checkSelfPermission(
                                    Manifest.permission.CAMERA);

            // 如果未授權
            if (hasPermission != PackageManager.PERMISSION_GRANTED) {
                // 請求授權
                //     第一個參數是請求授權的名稱
                //     第二個參數是請求代碼
                requestPermissions(
                    new String[] {Manifest.permission.CAMERA},
                    REQUEST_CAMERA_PERMISSION);
                return;
            }
        }

        // 如果裝置版本是6.0以下，
        // 或是裝置版本是6.0（包含）以上，使用者已經授權，
        // 開啟偵測
        processDetect();
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
        if (requestCode == REQUEST_CAMERA_PERMISSION) {
            // 如果在授權請求選擇「允許」
            if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                // 開啟偵測
                processDetect();
            }
            // 如果在授權請求選擇「拒絕」
            else {
                detect.setChecked(false);

                // 顯示沒有授權的訊息
                Toast.makeText(this, "沒有使用相機的授權",
                               Toast.LENGTH_SHORT).show();
            }
        } else {
            super.onRequestPermissionsResult(requestCode, permissions,
                                             grantResults);
        }
    }

    // 開啟偵測
    private void processDetect()
    {
        if (flashControl == null) {
            try {
                // 建立閃光燈控制物件
                flashControl = new FlashControl(this, this);
            } catch (CameraAccessException e) {
                Log.d("Sensor01Activity", e.toString());
            }
        }
    }

}
