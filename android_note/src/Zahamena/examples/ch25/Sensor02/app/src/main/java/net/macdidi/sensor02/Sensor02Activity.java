package net.macdidi.sensor02;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.widget.SeekBar;
import android.widget.TextView;

public class Sensor02Activity extends AppCompatActivity
{

    // 顯示方向的文字元件
    private TextView direction_text;
    // 顯示裝置傾斜度的元件
    private SeekBar horizontal_value, vertical_value;

    // 感應器設備管理員物件
    private SensorManager manager;
    // 加速度與磁場感應器設備
    private Sensor sMagentic, sAccelerometer;
    private MySensorEventListener listener;

    // 記錄地磁與加速度資訊的陣列物件
    private float[] magenticValues = new float[3];
    // 感應器設備改變監聽物件
    private float[] accelerometerValues = new float[3];

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sensor02);

        processViews();
        processControllers();

        // 取得感應器設備管理員物件
        manager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        // 取得加速度感應器設備
        sMagentic = manager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);
        // 取得磁場感應器設備
        sAccelerometer = manager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        // 建立感應器設備改變監聽物件
        listener = new MySensorEventListener();
    }

    private void processViews()
    {
        direction_text = (TextView) findViewById(R.id.direction_text);
        horizontal_value = (SeekBar) findViewById(R.id.horizontal_value);
        vertical_value = (SeekBar) findViewById(R.id.vertical_value);
    }

    private void processControllers()
    {
        // 讓SeekBar不能操作的觸控監聽類別
        class NoTouchListener implements OnTouchListener
        {
            @Override
            public boolean onTouch(View view, MotionEvent event)
            {
                return true;
            }
        }

        // 註冊觸控監聽事件，讓 SeekBar 不能操作
        NoTouchListener noTouchListener = new NoTouchListener();
        horizontal_value.setOnTouchListener(noTouchListener);
        vertical_value.setOnTouchListener(noTouchListener);
    }

    @Override
    protected void onResume()
    {
        super.onResume();

        // 註冊感應器設備改變監聽事件，兩種感應器共用同一個監聽物件
        manager.registerListener(listener,
                                 sMagentic, SensorManager.SENSOR_DELAY_NORMAL);
        manager.registerListener(listener,
                                 sAccelerometer, SensorManager.SENSOR_DELAY_NORMAL);
    }

    @Override
    protected void onPause()
    {
        // 移除感應器設備改變監聽事件
        manager.unregisterListener(listener);

        super.onPause();
    }

    // 感應設備改變監聽類別
    private class MySensorEventListener implements SensorEventListener
    {

        @Override
        public void onAccuracyChanged(Sensor sensor, int value)
        {

        }

        @Override
        public void onSensorChanged(SensorEvent event)
        {
            // 取得感應設備種類
            int type = event.sensor.getType();

            switch (type) {
            // 如果是加速度感應器
            case Sensor.TYPE_MAGNETIC_FIELD:
                // 取得加速度感應器資訊
                magenticValues = event.values.clone();
                break;

            // 如果是磁場感應器
            case Sensor.TYPE_ACCELEROMETER:
                // 取得磁場感應器資訊
                accelerometerValues = event.values.clone();
                break;
            }

            // 準備轉換資料用的陣列變數
            float[] ra = new float[9];
            float[] ia = new float[9];

            // 儲存轉換後資料
            float[] values = new float[3];

            // 儲存轉換為角度的資料
            int[] result = new int[3];

            // 先取得裝置旋轉資訊
            SensorManager.getRotationMatrix(ra, ia,
                                            accelerometerValues, magenticValues);

            // 轉換為方向與傾斜度資訊
            SensorManager.getOrientation(ra, values);

            // 轉換為角度整數
            for (int i = 0; i < result.length; i++) {
                result[i] = (int) Math.toDegrees(values[i]);
            }

            // 顯示方向，0：北、90：東、180與-180：南、-90：西
            direction_text.setText("Orientation: " + result[0]);

            // 設定 SeekBar 元件顯示前後傾斜度
            vertical_value.setProgress(result[1] + 90);
            // 設定 SeekBar 元件顯示左右傾斜度
            horizontal_value.setProgress(result[2] + 90);
        }
    }

}
