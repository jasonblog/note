package net.macdidi.mapmarker02;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.BitmapDescriptorFactory;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;

// OnMapReadyCallback：地圖元件建立完成介面
// GoogleMap.InfoWindowAdapter：Marker 訊息框介面
public class MapMarker02Activity extends AppCompatActivity
    implements OnMapReadyCallback,
    GoogleMap.InfoWindowAdapter
{

    // Google Map 元件
    private GoogleMap map;

    // 建立 Marker 用的座標
    private static final LatLng center =
        new LatLng(25.044704, 121.537113);
    private static final LatLng taipei101 =
        new LatLng(25.033807, 121.56503);
    private static final LatLng taipeiStation =
        new LatLng(25.04622, 121.517451);

    // 建立 Marker 物件
    private Marker centerMarker, taipei101Marker, taipeiStationMarker;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_map_marker02);

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

    // 實作 GoogleMap.InfoWindowAdapter 介面的方法
    // 建立訊息視窗的時候呼叫這個方法
    @Override
    public View getInfoWindow(Marker marker)
    {
        if (!marker.equals(taipei101Marker)) {
            // 傳回null值，使用預設的訊息視窗，接著呼叫getInfoContents方法
            return null;
        }

        // 從指定的畫面配置檔建立訊息視窗畫面物件
        View view = getLayoutInflater().inflate(
                        R.layout.info_window, null);

        // 設定圖示、標題和說明
        ImageView badge = (ImageView) view.findViewById(R.id.badge);
        TextView title = (TextView) view.findViewById(R.id.title);
        TextView snippet = (TextView) view.findViewById(R.id.snippet);

        badge.setImageResource(R.drawable.ic_store_mall_directory_black_48dp);
        title.setText(R.string.title);
        snippet.setText(R.string.snippet);

        // 傳回 View 物件，使用自定的訊息視窗
        return view;
    }

    // 實作 GoogleMap.InfoWindowAdapter 介面的方法
    // 建立訊息視窗內容的時候呼叫這個方法
    @Override
    public View getInfoContents(Marker marker)
    {
        if (marker.equals(taipeiStationMarker)) {
            // 從指定的畫面配置檔建立訊息視窗內容畫面物件
            View view = getLayoutInflater().inflate(
                            R.layout.info_content, null);

            // 設定圖示和標題
            ImageView badge = ((ImageView) view.findViewById(R.id.badge));
            TextView title = ((TextView) view.findViewById(R.id.title));

            badge.setImageResource(R.drawable.ic_directions_railway_black_48dp);
            title.setText("Taipei Main Station");

            // 傳回 View 物件，使用自定的訊息內容
            return view;
        } else {
            // 傳回 null 值，使用預設的訊息內容
            return null;
        }
    }

    // 地圖元件建立完成時，Android 自動呼叫這個方法
    // 參數是建立好的 Google Map 物件
    @Override
    public void onMapReady(GoogleMap googleMap)
    {
        // 設定 Google Map 物件
        map = googleMap;

        // 移動地圖
        map.moveCamera(CameraUpdateFactory.newLatLngZoom(center, 12));

        // 設定地圖的 InfoWindowAdapter 監聽物件
        map.setInfoWindowAdapter(this);

        // 設定地圖的 Marker
        processMarker();
    }

    // 設定地圖的 Marker
    private void processMarker()
    {
        // 加入Marker到地圖並同時設定地點和圖示
        taipei101Marker = map.addMarker(new MarkerOptions()
                                        .position(taipei101)
                                        .icon(BitmapDescriptorFactory
                                              .defaultMarker(BitmapDescriptorFactory.HUE_YELLOW)));

        // 加入Marker到地圖並同時設定地點
        taipeiStationMarker = map.addMarker(new MarkerOptions()
                                            .position(taipeiStation)
                                            .icon(BitmapDescriptorFactory
                                                    .defaultMarker(BitmapDescriptorFactory.HUE_BLUE)));

        // 加入Marker到地圖並同時設定地點和標題
        centerMarker = map.addMarker(new MarkerOptions()
                                     .position(center)
                                     .title("Hello!")
                                     .icon(BitmapDescriptorFactory
                                           .defaultMarker(BitmapDescriptorFactory.HUE_GREEN)));;
    }

}
