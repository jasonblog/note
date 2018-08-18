package net.macdidi.mapgroundoverlay01;

import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;

import com.google.android.gms.maps.CameraUpdate;
import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.UiSettings;
import com.google.android.gms.maps.model.BitmapDescriptor;
import com.google.android.gms.maps.model.BitmapDescriptorFactory;
import com.google.android.gms.maps.model.CameraPosition;
import com.google.android.gms.maps.model.GroundOverlay;
import com.google.android.gms.maps.model.GroundOverlayOptions;
import com.google.android.gms.maps.model.LatLng;

import java.lang.ref.WeakReference;

// OnMapReadyCallback：地圖元件建立完成介面
// GoogleMap.OnMapClickListener：
public class MapGroundOverlay01Activity extends AppCompatActivity
    implements OnMapReadyCallback,
    GoogleMap.OnMapClickListener
{

    // Google Map 元件
    private GoogleMap map;

    // Google Map UI 設定元件
    private UiSettings uiSettings;

    // GroundOverlay 物件
    private GroundOverlay groundOverlay;
    // 地圖顯示位置的座標
    private LatLng center;

    // GroundOverlay 物件是否正在旋轉
    private boolean isRotate = false;
    // 控制 GroundOverlay 物件旋轉的執行緒物件
    private MyRotateTask myRotateTask;

    // GroundOverlay 物件的角度
    private int angle = 0;

    // 地圖啟始畫面延遲用的 Handler 物件
    private Handler handlerDelay = new HandlerDelay(this);

    private static class HandlerDelay extends Handler
    {

        private final WeakReference<MapGroundOverlay01Activity> hActivity;

        public HandlerDelay(MapGroundOverlay01Activity activity)
        {
            hActivity = new WeakReference<>(activity);
        }

        @Override
        public void handleMessage(Message msg)
        {
            MapGroundOverlay01Activity activity = hActivity.get();

            if (activity != null) {
                // 建立 CameraPosition 物件
                // 設定位置與縮放
                CameraPosition position =
                    new CameraPosition.Builder()
                .target(activity.center)
                .zoom(12.5F)
                .build();
                // 使用動畫的方式更新地圖畫面
                activity.map.animateCamera(
                    CameraUpdateFactory.newCameraPosition(position),
                    3000, null);
            }
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_map_ground_overlay01);

        processViews();
    }

    private void processViews()
    {
        // 讀取型態為 SupportMapFragment 的 Google Map 物件
        SupportMapFragment mapFragment = (SupportMapFragment)
                                         getSupportFragmentManager().findFragmentById(R.id.map);
        // 註冊地圖元件建立完成監聽物件
        mapFragment.getMapAsync(this);
    }

    // 地圖元件建立完成時，Android 自動呼叫這個方法
    // 參數是建立好的 Google Map 物件
    @Override
    public void onMapReady(GoogleMap googleMap)
    {
        // 設定 Google Map 物件
        map = googleMap;

        // 設定地圖類型
        map.setMapType(GoogleMap.MAP_TYPE_TERRAIN);

        // 建立準備要移動地圖的 CameraUpdate 物件
        center = new LatLng(23.975117, 120.981588);
        CameraUpdate cameraUpdate = CameraUpdateFactory.newLatLngZoom(
                                        center, 6);

        // 直接顯示指定位置的地圖
        map.moveCamera(cameraUpdate);

        // 取得地圖UI設定物件
        uiSettings = map.getUiSettings();
        // 設定可以控制地圖縮放
        uiSettings.setZoomControlsEnabled(true);

        // 加入 GroundOverlay 到地圖
        processGroundOverlay();

        // 註冊地圖點擊事件
        map.setOnMapClickListener(this);
    }

    @Override
    public void onMapClick(LatLng latLng)
    {
        // 如果 GroundOverlay 物件正在旋轉
        if (!isRotate) {
            // 建立與啟動控制 GroundOverlay 物件旋轉的執行緒物件
            myRotateTask = new MyRotateTask();
            myRotateTask.execute();
        }
        // 如果 GroundOverlay 物件沒有在旋轉
        else {
            // 停止控制 GroundOverlay 物件旋轉的執行緒物件
            myRotateTask.cancel(true);
            myRotateTask = null;
        }

        isRotate = !isRotate;
    }

    public void processGroundOverlay()
    {
        // 建立指定給 GroundOverlay 使用的圖形物件
        BitmapDescriptor androidGround =
            BitmapDescriptorFactory.fromResource(
                R.drawable.android_oreo);

        // 準備建立 GroundOverlay 物件
        //   設定圖形、透明度與位置
        GroundOverlayOptions newarkMap = new GroundOverlayOptions()
        .image(androidGround)
        .transparency(0.1F)
        .position(center, 8000F);

        // 加入 GroundOverlay 物件到地圖
        groundOverlay = map.addGroundOverlay(newarkMap);

        // 一秒鐘後開始移動地圖
        handlerDelay.sendEmptyMessageDelayed(0, 1000);
    }

    // 控制 GroundOverlay 物件旋轉的執行緒類別
    private class MyRotateTask extends AsyncTask<Void, Void, Void>
    {

        @Override
        protected Void doInBackground(Void... args)
        {
            while (!isCancelled()) {
                // 設定 GroundOverlay 的角度
                publishProgress();

                // 旋轉角度加 2
                angle += 2;

                // 如果已經旋轉到 360 度，角度就設為 0
                if (angle >= 360) {
                    angle = 0;
                }

                try {
                    Thread.sleep(10);
                } catch (InterruptedException e) {
                    Log.d("MapGroundOverlay01", e.toString());
                }
            }

            return null;
        }

        @Override
        protected void onProgressUpdate(Void... values)
        {
            // 設定 GroundOverlay 的角度
            groundOverlay.setBearing(angle);
        }
    }

}