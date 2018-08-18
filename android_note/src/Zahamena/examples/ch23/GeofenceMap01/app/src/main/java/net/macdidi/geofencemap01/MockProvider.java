package net.macdidi.geofencemap01;

import android.content.Context;
import android.location.Criteria;
import android.location.Location;
import android.location.LocationManager;
import android.os.Build;
import android.os.SystemClock;

public class MockProvider
{

    private LocationManager locationManager;
    private String provider;

    // 測試用模擬位置陣列
    private final double coor[][] = { { 25.06676, 121.49997 },
        { 25.06681, 121.50019 }, { 25.06681, 121.50019 },
        { 25.06700, 121.50109 }, { 25.06700, 121.50109 },
        { 25.06630, 121.50187 }, { 25.06630, 121.50187 },
        { 25.06479, 121.50228 }, { 25.06391, 121.50260 },
        { 25.06391, 121.50260 }, { 25.06408, 121.50315 },
        { 25.06410, 121.50375 }, { 25.06340, 121.50823 },
        { 25.06300, 121.51051 }, { 25.06286, 121.51103 },
        { 25.06286, 121.51178 }, { 25.06298, 121.51400 },
        { 25.06293, 121.51581 }, { 25.06287, 121.51597 },
        { 25.06282, 121.51819 }
    };

    private Thread mockThread;
    private boolean isExit = false;

    public MockProvider(String provider, Context context)
    {
        this.provider = provider;

        // 取得 LocationManager 物件
        locationManager = (LocationManager)
                          context.getSystemService(Context.LOCATION_SERVICE);

        // 加入模擬位置測試, 需要的參數依序為
        // String, Location Provider名稱
        // boolean, 需要網路
        // boolean, 需要衛星定位
        // boolean, 需要行動網路
        // boolean, 需要花費成本
        // boolean, 支援海拔高度
        // boolean, 支援速度
        // boolean, 支援方向
        // int, 電力需求條件
        // int, 精確度條件
        locationManager.addTestProvider(provider,
                                        false, false, false, false, false, false, false,
                                        Criteria.POWER_HIGH,
                                        Criteria.ACCURACY_FINE);
        // 開啟模擬位置測試
        locationManager.setTestProviderEnabled(provider, true);

        // 在背景傳送模擬位置的執行緒
        mockThread = new Thread() {

            private int index = 0;
            private boolean isInc = true;

            @Override
            public void run() {
                while (!isExit) {
                    if (isInc) {
                        index++;

                        if (index >= coor.length) {
                            isInc = false;
                            index = coor.length - 2;
                        }
                    } else {
                        index--;

                        if (index < 0) {
                            isInc = true;
                            index = 1;
                        }
                    }

                    pushLocation(coor[index][0], coor[index][1]);

                    try {
                        sleep(2000);
                    } catch (InterruptedException e) {
                        //
                    }
                }
            }
        };
    }

    // 開始傳送模擬位置
    public void start()
    {
        // 啟動執行緒物件
        mockThread.start();
    }

    // 停止傳送模擬位置
    public void shutdown()
    {
        // 停止執行緒物件
        isExit = true;
        // 移除模擬位置測試
        locationManager.removeTestProvider(provider);
    }

    private void pushLocation(double lat, double lng)
    {
        // 建立與設定傳送用的模擬位置物件
        Location location = new Location(provider);
        location.setTime(System.currentTimeMillis());
        location.setAccuracy(1);
        location.setLatitude(lat);
        location.setLongitude(lng);

        if (Build.VERSION.SDK_INT > 16) {
            location.setElapsedRealtimeNanos(SystemClock.elapsedRealtimeNanos());
        }

        // 傳送模擬位置
        locationManager.setTestProviderLocation(provider, location);
    }

}
