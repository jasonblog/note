package net.macdidi.map02;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Spinner;

import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.MarkerOptions;

// 如果應用程式需要 ActionBar，也可以讓使用地圖的元件繼承 AppCompatActivity
// OnMapReadyCallback： 地圖元件建立完成介面
public class Map02Activity extends AppCompatActivity
    implements OnMapReadyCallback
{

    // Google Map 元件
    private GoogleMap map;
    // 地圖種類選單
    private Spinner type_spinner;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_map02);

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

        // 讀取地圖種類選單畫面元件
        type_spinner = (Spinner) findViewById(R.id.type_spinner);

        // 設定選單項目
        ArrayAdapter<CharSequence> adapter =
            ArrayAdapter.createFromResource(
                this, R.array.type_array,
                android.R.layout.simple_spinner_item);
        adapter.setDropDownViewResource(
            android.R.layout.simple_spinner_dropdown_item);
        type_spinner.setAdapter(adapter);
    }

    private void processControllers()
    {
        // 註冊選單元件監聽物件
        type_spinner.setOnItemSelectedListener(
        new AdapterView.OnItemSelectedListener() {

            @Override
            public void onItemSelected(AdapterView<?> parent,
                                       View view, int position, long id) {
                if (position == 0) {
                    // 不顯示地圖
                    map.setMapType(GoogleMap.MAP_TYPE_NONE);
                } else if (position == 1) {
                    // 一般
                    map.setMapType(GoogleMap.MAP_TYPE_NORMAL);
                } else if (position == 2) {
                    // 衛星空照
                    map.setMapType(GoogleMap.MAP_TYPE_SATELLITE);
                } else if (position == 3) {
                    // 地形
                    map.setMapType(GoogleMap.MAP_TYPE_TERRAIN);
                } else if (position == 4) {
                    // 衛星空照加道路
                    map.setMapType(GoogleMap.MAP_TYPE_HYBRID);
                }

            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {
            }

        });
    }

    // 地圖元件建立完成時，Android 自動呼叫這個方法
    // 參數是建立好的 Google Map 物件
    @Override
    public void onMapReady(GoogleMap googleMap)
    {
        // 設定 Google Map 物件
        map = googleMap;

        // 建立座標物件
        LatLng ts = new LatLng(25.04622F, 121.517451);
        // 加入標記
        map.addMarker(new MarkerOptions().position(ts).title("Taipei station"));
        // 移動地圖到指定的座標
        map.moveCamera(CameraUpdateFactory.newLatLng(ts));

        // 設定地圖種類為一般地圖
        map.setMapType(GoogleMap.MAP_TYPE_NORMAL);

        // 取得目前地圖的種類，並設定好 Spinner 元件
        type_spinner.setSelection(map.getMapType());
    }

}
