package net.macdidi.mapmarker03;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.CompoundButton;
import android.widget.Switch;
import android.widget.Toast;

import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.BitmapDescriptorFactory;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;

// OnMapReadyCallback：地圖元件建立完成介面
// GoogleMap.OnInfoWindowClickListener：
// GoogleMap.OnMarkerClickListener：
public class MapMarker03Activity extends AppCompatActivity
    implements OnMapReadyCallback,
    GoogleMap.OnInfoWindowClickListener,
    GoogleMap.OnMarkerClickListener
{

    // Google Map 元件
    private GoogleMap map;

    // 儲存兩組 Marker 的陣列
    private Marker[] stationMarkers;
    private Marker[] taipeiMarkers;

    // 切換是否顯示群組 Marker 的 Switch 元件
    private Switch station_switch, taipei_switch;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_map_marker03);

        processViews();
        processControllers();
    }

    private void processViews()
    {
        // 讀取型態為 SupportMapFragment 的 Google Map 物件
        SupportMapFragment mapFragment = (SupportMapFragment)
                                         getSupportFragmentManager().findFragmentById(R.id.map);
        // 註冊地圖元件建立完成監聽物件
        mapFragment.getMapAsync(this);

        station_switch = (Switch) findViewById(R.id.station_switch);
        taipei_switch = (Switch) findViewById(R.id.taipei_switch);
    }

    private void processControllers()
    {
        // 顯示或隱藏Marker的事件
        CompoundButton.OnCheckedChangeListener switchListener;
        switchListener = new CompoundButton.OnCheckedChangeListener() {

            @Override
            public void onCheckedChanged(CompoundButton buttonView,
                                         boolean isChecked) {

                if (buttonView == station_switch) {
                    // 顯示或隱藏第一組 Marker
                    switchMarker(stationMarkers, isChecked);
                } else if (buttonView == taipei_switch) {
                    // 顯示或隱藏第二組 Marker
                    switchMarker(taipeiMarkers, isChecked);
                }

            }

        };

        station_switch.setOnCheckedChangeListener(switchListener);
        taipei_switch.setOnCheckedChangeListener(switchListener);
    }

    @Override
    public void onInfoWindowClick(Marker marker)
    {
        // 取得訊息視窗的說明，內容是一個網址
        String url = marker.getSnippet();

        if (url != null && url.length() > 0) {
            // 啟動瀏覽器顯示指定的網頁
            Intent intent = new Intent(Intent.ACTION_VIEW,
                                       Uri.parse(url));
            startActivity(intent);
        }
    }

    @Override
    public boolean onMarkerClick(Marker marker)
    {
        String url = marker.getSnippet();

        if (url == null || url.equals("")) {
            Toast.makeText(MapMarker03Activity.this,
                           marker.getTitle(), Toast.LENGTH_SHORT).show();
            // 不會產生預設事件，也就是不會出現訊息視窗
            return true;
        } else {
            // 由地圖處理Marker點擊事件，顯示訊息視窗
            return false;
        }
    }

    // 顯示或隱藏 Marker
    private void switchMarker(Marker[] target, boolean display)
    {
        for (Marker marker : target) {
            marker.setVisible(display);
        }
    }

    // 加入第一組 Marker 到地圖
    private void addStationMarker()
    {
        // Marker 使用的座標
        double stationLatLng[][] = {{25.04622, 121.517451},
            {25.03335, 121.500027}, {25.042332, 121.508267},
            {25.052596, 121.520326}, {25.044626, 121.523631}
        };

        // 車站名稱
        String[] stationTitle =
        {"台北車站", "萬華車站", "西門站", "中山站", "善導寺站"};

        final int stationLength = stationLatLng.length;
        // 建立車站Marker陣列
        stationMarkers = new Marker[stationLength];

        for (int i = 0; i < stationLength; i++) {
            // 車站使用的Marker設定
            MarkerOptions markerOptions = new MarkerOptions()
            .position(new LatLng(stationLatLng[i][0], stationLatLng[i][1]))
            .title(stationTitle[i])
            .icon(BitmapDescriptorFactory.defaultMarker(
                      BitmapDescriptorFactory.HUE_GREEN))
            .visible(false);
            // 加入Marker到地圖
            stationMarkers[i] = map.addMarker(markerOptions);
        }
    }

    // 加入第二組 Marker 到地圖
    private void addTaipeiMarker()
    {
        // Marker 使用的座標
        double taipeiLatLng[][] = {
            {25.032883, 121.53883},
            {25.072617, 121.524346},
            {25.100057, 121.518917},
            {25.031454, 121.511226},
            {25.050724, 121.518974}
        };

        // 台北地點名稱
        String[] taipeiTitle = {
            "台北市立圖書館", "台北市立美術館",
            "台北市立天文館", "歷史博物館", "台北當代藝術館"
        };

        // 台北地點網址
        String[] taipeiUrl = {
            "http://www.tpml.edu.tw",
            "http://www.tfam.museum",
            "http://www.tam.gov.tw",
            "http://www.nmh.gov.tw",
            "http://www.mocataipei.org.tw"
        };

        final int taipeiLength = taipeiLatLng.length;
        taipeiMarkers = new Marker[taipeiLength];

        for (int i = 0; i < taipeiLength; i++) {
            // 台北市地點使用的Marker設定
            MarkerOptions markerOptions = new MarkerOptions()
            .position(new LatLng(taipeiLatLng[i][0], taipeiLatLng[i][1]))
            .title(taipeiTitle[i])
            .snippet(taipeiUrl[i])
            .icon(BitmapDescriptorFactory.defaultMarker(
                      BitmapDescriptorFactory.HUE_ROSE))
            .visible(false);
            // 加入Marker到地圖
            taipeiMarkers[i] = map.addMarker(markerOptions);
        }
    }

    // 地圖元件建立完成時，Android 自動呼叫這個方法
    // 參數是建立好的 Google Map 物件
    @Override
    public void onMapReady(GoogleMap googleMap)
    {
        // 設定 Google Map 物件
        map = googleMap;

        // 加入所有 Marker 到地圖
        addStationMarker();
        addTaipeiMarker();

        // 註冊訊息視窗點擊事件
        map.setOnInfoWindowClickListener(this);
        // 註冊 Marker 擊事件
        map.setOnMarkerClickListener(this);

        // 移動地圖到指定的地點
        map.moveCamera(CameraUpdateFactory.newLatLngZoom(
                           new LatLng(25.04622, 121.517451), 12));
    }

}
