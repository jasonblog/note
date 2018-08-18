package net.macdidi.map01;

import android.support.v4.app.FragmentActivity;
import android.os.Bundle;

import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.MarkerOptions;

// OnMapReadyCallback： 地圖元件建立完成介面
public class MapsActivity extends FragmentActivity
    implements OnMapReadyCallback
{

    // Google Map 元件
    private GoogleMap mMap;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_maps);
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
        mMap = googleMap;
        // 建立座標物件
        LatLng sydney = new LatLng(-34, 151);
        // 加入標記
        mMap.addMarker(new MarkerOptions().position(sydney)
                       .title("Marker in Sydney"));
        // 移動地圖到指定的座標
        mMap.moveCamera(CameraUpdateFactory.newLatLng(sydney));
    }
}
