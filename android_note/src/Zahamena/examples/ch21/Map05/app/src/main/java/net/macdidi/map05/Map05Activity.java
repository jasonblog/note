package net.macdidi.map05;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.TextView;

import com.google.android.gms.maps.CameraUpdate;
import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.CameraPosition;
import com.google.android.gms.maps.model.LatLng;

// OnMapReadyCallback：地圖元件建立完成介面
// GoogleMap.OnMapClickListener：點擊地圖介面
// GoogleMap.OnMapLongClickListener：長按地圖介面
public class Map05Activity extends AppCompatActivity
    implements OnMapReadyCallback,
    GoogleMap.OnMapClickListener,
    GoogleMap.OnMapLongClickListener
{

    // Google Map 元件
    private GoogleMap map;

    // 顯示資訊的元件
    private TextView info;

    // 移動地圖用的CameraPosition物件
    private final CameraPosition positionMove =
        new CameraPosition.Builder()
    .target(new LatLng(25.071742, 121.524256))
    .zoom(15)
    .bearing(300)
    .tilt(50)
    .build();
    private CameraUpdate cameraUpdateMove;


    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_map05);

        processViews();
    }

    private void processViews()
    {
        // 讀取型態為 SupportMapFragment 的 Google Map 物件
        SupportMapFragment mapFragment = (SupportMapFragment)
                                         getSupportFragmentManager().findFragmentById(R.id.map);
        // 註冊地圖元件建立完成監聽物件
        mapFragment.getMapAsync(this);

        info = (TextView) findViewById(R.id.info);
    }

    // 實作 GoogleMap.OnMapClickListener 介面的方法
    // 點擊地圖
    @Override
    public void onMapClick(LatLng point)
    {
        // 參數 LatLng 物件是使用者點擊的位置
        info.setText(String.format("Map Click: %.4f,%.4f",
                                   point.latitude, point.longitude));
    }

    // 實作 GoogleMap.OnMapLongClickListener 介面的方法
    // 長按地圖
    @Override
    public void onMapLongClick(LatLng point)
    {
        // 參數 LatLng 物件是使用者長按的位置
        info.setText(String.format("Map Long Click: %.4f,%.4f",
                                   point.latitude, point.longitude));
    }

    // 地圖元件建立完成時，Android 自動呼叫這個方法
    // 參數是建立好的 Google Map 物件
    @Override
    public void onMapReady(GoogleMap googleMap)
    {
        // 設定 Google Map 物件
        map = googleMap;

        // 註冊地圖點擊、長按與地圖位置改變監聽事件
        map.setOnMapClickListener(this);
        map.setOnMapLongClickListener(this);

        // 移動地圖到台灣地理中心點
        LatLng center = new LatLng(23.975117, 120.981588);
        CameraUpdate cameraUpdate = CameraUpdateFactory.newLatLngZoom(
                                        center, 12);
        map.moveCamera(cameraUpdate);

        // 建立移動地圖用的CameraUpdate物件
        cameraUpdateMove =
            CameraUpdateFactory.newCameraPosition(positionMove);

        // 註冊Camera事件
        processCameraListener();
    }

    // 開始移動
    public void clickMove(View view)
    {
        map.animateCamera(cameraUpdateMove, 10000, null);
    }

    // 取消移動
    public void clickCancel(View view)
    {
        map.stopAnimation();
    }

    private enum REASON {
        // 使用者操作、初始移動、使用API移動
        REASON_GESTURE, REASON_DEVELOPER_ANIMATION, REASON_API_ANIMATION;

        public static String getName(int value)
        {
            return REASON.values()[value - 1].name();
        }
    }

    // 註冊Camera事件
    private void processCameraListener()
    {
        // 開始移動
        map.setOnCameraMoveStartedListener(
        new GoogleMap.OnCameraMoveStartedListener() {
            @Override
            public void onCameraMoveStarted(int i) {
                // 參數為移動的原因
                info.setText("Camera Move Started: " + REASON.getName(i));
            }
        });

        // 移動
        map.setOnCameraMoveListener(
        new GoogleMap.OnCameraMoveListener() {
            @Override
            public void onCameraMove() {
                // 目前位置
                LatLng point = map.getCameraPosition().target;
                info.setText(String.format("Camera Move: %.4f,%.4f",
                                           point.latitude, point.longitude));
            }
        });

        // 取消移動
        map.setOnCameraMoveCanceledListener(
        new GoogleMap.OnCameraMoveCanceledListener() {
            @Override
            public void onCameraMoveCanceled() {
                // 目前位置
                LatLng point = map.getCameraPosition().target;
                info.setText(String.format(
                                 "Camera Move Canceled: %.4f,%.4f",
                                 point.latitude, point.longitude));
            }
        });

        // 停止移動
        map.setOnCameraIdleListener(
        new GoogleMap.OnCameraIdleListener() {
            @Override
            public void onCameraIdle() {
                // 目前位置
                LatLng point = map.getCameraPosition().target;
                info.setText(String.format("Camera Idle: %.4f,%.4f",
                                           point.latitude, point.longitude));
            }
        });
    }

}
