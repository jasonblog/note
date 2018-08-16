package net.macdidi.mapmarker01;

import android.location.Address;
import android.location.Geocoder;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.google.android.gms.maps.CameraUpdate;
import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.BitmapDescriptor;
import com.google.android.gms.maps.model.BitmapDescriptorFactory;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;

import java.io.IOException;
import java.util.List;
import java.util.Locale;

// OnMapReadyCallback：地圖元件建立完成介面
// GoogleMap.OnMarkerClickListener：點擊 Marker 介面
// GoogleMap.OnMarkerDragListener：拖拉 Marker 介面
public class MapMarker01Activity extends AppCompatActivity
    implements OnMapReadyCallback,
    GoogleMap.OnMarkerClickListener,
    GoogleMap.OnMarkerDragListener
{

    // Google Map 元件
    private GoogleMap map;

    private LinearLayout info_panel;
    private TextView info;

    // 宣告與建立 Marker 用座標
    private static final LatLng station01 = new LatLng(25.04622, 121.517451);
    private static final LatLng station02 = new LatLng(25.03335, 121.500027);
    private static final LatLng station03 = new LatLng(25.042332, 121.508267);
    private static final LatLng station04 = new LatLng(25.052596, 121.520326);
    private static final LatLng station05 = new LatLng(25.044626, 121.523631);

    // 宣告需要的 Marker 物件
    private Marker marker01, marker02, marker03, marker04, marker05;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_map_marker01);

        processViews();
    }

    private void processViews()
    {
        // 讀取型態為 SupportMapFragment 的 Google Map 物件
        SupportMapFragment mapFragment = (SupportMapFragment)
                                         getSupportFragmentManager().findFragmentById(R.id.map);
        // 註冊地圖元件建立完成監聽物件
        mapFragment.getMapAsync(this);

        // 顯示拖拉 Marker 訊息
        info_panel = (LinearLayout) findViewById(R.id.info_panel);
        info = (TextView) findViewById(R.id.info);
    }

    // 實作 GoogleMap.OnMarkerClickListener 介面的方法
    // 點擊 Marker 時呼叫這個方法
    @Override
    public boolean onMarkerClick(Marker marker)
    {
        // 如果是 marker01 物件
        if (marker.equals(marker01)) {
            Toast.makeText(MapMarker01Activity.this, marker.getTitle(),
                           Toast.LENGTH_SHORT).show();
            // 回傳 true 處理點擊事件
            return true;
        } else {
            // 回傳 false 不處理點擊事件，執行預設的點擊工作，顯示訊息視窗
            return false;
        }
    }

    // 實作 GoogleMap.OnMarkerDragListener 介面的方法
    // 正在拖拉 Marker 時呼叫這個方法
    @Override
    public void onMarkerDrag(Marker marker)
    {
        // 如果是 marker05 物件
        // Marker 正在移動，參數是操作中的Marker物件，
        // 可以經由它取得最新的資訊，例如位置
        if (marker.equals(marker05)) {
            // 顯示目前位置
            LatLng position = marker.getPosition();
            info.setText(marker.getTitle() + ": " + position.latitude
                         + "," + position.longitude);
        }
    }

    // 實作 GoogleMap.OnMarkerDragListener 介面的方法
    // 結束拖拉 Marker 時呼叫這個方法
    @Override
    public void onMarkerDragEnd(Marker marker)
    {
        // 如果是 marker05 物件
        // Marker 結束拖拉，參數是操作的Marker物件
        if (marker.equals(marker05)) {
            // 關閉顯示訊息元件
            info_panel.setVisibility(View.INVISIBLE);
            marker05.setTitle("New Place");
            // 取得 Marker 目前的經緯度座標
            LatLng point = marker.getPosition();
            // 取得目前的地址
            marker05.setSnippet(getAddress(point));
        }
    }

    // 實作 GoogleMap.OnMarkerDragListener 介面的方法
    // 開始拖拉 Marker 時呼叫這個方法
    @Override
    public void onMarkerDragStart(Marker marker)
    {
        // 如果是 marker05 物件
        // Marker 開始拖拉，參數是操作的Marker物件
        if (marker.equals(marker05)) {
            // 開啟顯示訊息元件
            info_panel.setVisibility(View.VISIBLE);
            info.setText("Drag Start...");
        }
    }

    private String getAddress(LatLng point)
    {
        String result = "Unknown";
        List<Address> addresses = null;
        // 建立 Geocoder 物件
        Geocoder geocoder = new Geocoder(MapMarker01Activity.this,
                                         Locale.getDefault());

        try {
            // 查詢地址
            addresses = geocoder.getFromLocation(point.latitude,
                                                 point.longitude, 1);

            if (addresses.size() > 0) {
                // 取得第一個地址物件
                Address addr = addresses.get(0);
                // 設定回傳內容為地址
                result = addr.getAddressLine(0);
            }
        } catch (IOException e) {
            Log.d("Geocoder01", e.toString());
        }

        return result;
    }

    // 地圖元件建立完成時，Android 自動呼叫這個方法
    // 參數是建立好的 Google Map 物件
    @Override
    public void onMapReady(GoogleMap googleMap)
    {
        // 設定 Google Map 物件
        map = googleMap;

        // 移動地圖
        CameraUpdate cameraUpdate = CameraUpdateFactory.newLatLngZoom(
                                        station01, 13);
        map.moveCamera(cameraUpdate);

        // 在地圖加入 Marker
        processMarker();

        // 註冊點擊 Marker 監聽物件
        map.setOnMarkerClickListener(this);
        // 註冊拖拉 Marker 監聽物件
        map.setOnMarkerDragListener(this);
    }

    // 在地圖加入 Marker
    private void processMarker()
    {
        // 先建立一個設定 Marker 用的 MarkerOptions 物件
        MarkerOptions markerOptions = new MarkerOptions();
        // 建立設定 Marker 圖示用的物件
        BitmapDescriptor bitmapDescriptor = BitmapDescriptorFactory
                                            .fromResource(R.drawable.ic_directions_railway_black_48dp);
        // 設定 Marker 的地點、標題、說明和圖示
        markerOptions.position(station01)
        .title("台北車站")
        .snippet(station01.latitude + "," + station01.longitude)
        .icon(bitmapDescriptor);
        // 加入 Marker 到地圖並取得傳回的 Marker 物件
        marker01 = map.addMarker(markerOptions);

        // 加入 Marker 到地圖並設定地點、標題、說明和圖示
        marker02 = map.addMarker(new MarkerOptions()
                                 .position(station02)
                                 .title("萬華車站")
                                 .snippet(station02.latitude + "," + station02.longitude)
                                 .icon(BitmapDescriptorFactory.fromResource(
                                           R.drawable.ic_directions_railway_black_48dp)));

        // 加入 Marker 到地圖並設定地點、標題和說明
        marker03 = map.addMarker(new MarkerOptions()
                                 .position(station03)
                                 .title("西門站")
                                 .snippet(station03.latitude + "," + station03.longitude));

        // 加入 Marker 到地圖並設定地點、標題、說明和橘色預設圖示
        marker04 = map.addMarker(new MarkerOptions()
                                 .position(station04)
                                 .title("中山站")
                                 .snippet(station04.latitude + "," + station04.longitude)
                                 .icon(BitmapDescriptorFactory
                                       .defaultMarker(BitmapDescriptorFactory.HUE_ORANGE)));

        // 加入 Marker 到地圖並設定地點、標題、說明、黃色預設圖示和可以拖拉
        marker05 = map.addMarker(new MarkerOptions()
                                 .position(station05)
                                 .title("善導寺站")
                                 .snippet(station05.latitude + "," + station05.longitude)
                                 .icon(BitmapDescriptorFactory
                                       .defaultMarker(BitmapDescriptorFactory.HUE_YELLOW))
                                 .draggable(true));
    }

}
