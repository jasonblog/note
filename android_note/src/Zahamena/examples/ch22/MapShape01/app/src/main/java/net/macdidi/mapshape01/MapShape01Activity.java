package net.macdidi.mapshape01;

import android.graphics.Color;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.CompoundButton;
import android.widget.SeekBar;
import android.widget.Switch;

import com.google.android.gms.maps.CameraUpdate;
import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.Circle;
import com.google.android.gms.maps.model.CircleOptions;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Polygon;
import com.google.android.gms.maps.model.PolygonOptions;
import com.google.android.gms.maps.model.Polyline;
import com.google.android.gms.maps.model.PolylineOptions;

import java.util.ArrayList;
import java.util.List;

// OnMapReadyCallback：地圖元件建立完成介面
public class MapShape01Activity extends AppCompatActivity
    implements OnMapReadyCallback
{

    // Google Map 元件
    private GoogleMap map;

    private Switch polyline_switch, polygon_switch, circle_switch;
    private SeekBar ployline_seekbar, ploygon_seekbar, circle_seekbar;

    private Polyline polyline;
    private Polygon polygon;
    private Circle circle;

    // 地圖中心點，繪製圓形用的座標
    private static final LatLng mapCenter =
        new LatLng(25.066772584973673, 121.53889048844576);

    // 繪製線條用的座標
    private static final LatLng station01 = new LatLng(25.04657, 121.51763);
    private static final LatLng station02 = new LatLng(25.044937, 121.523037);
    private static final LatLng station03 = new LatLng(25.042293, 121.532907);
    private static final LatLng station04 = new LatLng(25.051702, 121.532907);
    private static final LatLng station05 = new LatLng(25.05971, 121.533251);

    // 繪製區塊用的座標
    private static final LatLng station06 = new LatLng(25.062354, 121.541061);
    private static final LatLng station07 = new LatLng(25.041671, 121.5378);
    private static final LatLng station08 = new LatLng(25.04136, 121.557713);
    private static final LatLng station09 = new LatLng(25.063054, 121.552048);

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_map_shape01);

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

        //
        polyline_switch = (Switch) findViewById(R.id.polyline_switch);
        ployline_seekbar = (SeekBar) findViewById(R.id.ployline_seekbar);
        polygon_switch = (Switch) findViewById(R.id.polygon_switch);
        ploygon_seekbar = (SeekBar) findViewById(R.id.ploygon_seekbar);
        circle_switch = (Switch) findViewById(R.id.circle_switch);
        circle_seekbar = (SeekBar) findViewById(R.id.circle_seekbar);

    }

    private void processControllers()
    {
        // 註冊控制圖形是否顯示的事件
        CompoundButton.OnCheckedChangeListener switchListener;
        switchListener = new CompoundButton.OnCheckedChangeListener() {

            @Override
            public void onCheckedChanged(CompoundButton buttonView,
                                         boolean isChecked) {
                if (buttonView == polyline_switch) {
                    // 設定線條是否顯示
                    polyline.setVisible(isChecked);
                } else if (buttonView == polygon_switch) {
                    // 設定區塊是否顯示
                    polygon.setVisible(isChecked);
                } else if (buttonView == circle_switch) {
                    // 設定圓形是否顯示
                    circle.setVisible(isChecked);
                }
            }

        };

        polyline_switch.setOnCheckedChangeListener(switchListener);
        polygon_switch.setOnCheckedChangeListener(switchListener);
        circle_switch.setOnCheckedChangeListener(switchListener);

        // 註冊改變寬度的事件
        SeekBar.OnSeekBarChangeListener seekBarListener;
        seekBarListener = new SeekBar.OnSeekBarChangeListener() {

            @Override
            public void onProgressChanged(SeekBar seekBar,
                                          int progress, boolean fromUser) {
                if (seekBar == ployline_seekbar) {
                    // 設定線條的寬度
                    polyline.setWidth(progress);
                } else if (seekBar == ploygon_seekbar) {
                    // 設定區塊的線條寬度
                    polygon.setStrokeWidth(progress);
                } else if (seekBar == circle_seekbar) {
                    // 設定圓形的線條寬度
                    circle.setStrokeWidth(progress);
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        };

        ployline_seekbar.setOnSeekBarChangeListener(seekBarListener);
        ploygon_seekbar.setOnSeekBarChangeListener(seekBarListener);
        circle_seekbar.setOnSeekBarChangeListener(seekBarListener);
    }

    // 地圖元件建立完成時，Android 自動呼叫這個方法
    // 參數是建立好的 Google Map 物件
    @Override
    public void onMapReady(GoogleMap googleMap)
    {
        // 設定 Google Map 物件
        map = googleMap;

        // 移動地圖
        CameraUpdate cameraUpdate =
            CameraUpdateFactory.newLatLngZoom(mapCenter, 13);
        map.moveCamera(cameraUpdate);

        // 加入繪圖物件到地圖
        processShape();
    }

    // 加入繪圖物件到地圖
    private void processShape()
    {
        // 線條設定物件
        PolylineOptions polylineOptions = new PolylineOptions();

        // 加入線條座標
        polylineOptions.add(station01, station02, station03,
                            station04, station05);

        // 設定線條的寬度、顏色
        polylineOptions.width(20);
        polylineOptions.color(Color.BLUE);

        // 加入線條到地圖
        polyline = map.addPolyline(polylineOptions);

        // 設定區塊中未填滿區域的座標
        List<LatLng> points = new ArrayList<>();
        points.add(new LatLng(25.05178, 121.545267));
        points.add(new LatLng(25.043148, 121.545353));
        points.add(new LatLng(25.044665, 121.550546));
        points.add(new LatLng(25.051585, 121.550288));

        // 繪製區塊，直接設定座標與未填滿區域
        polygon = map.addPolygon((new PolygonOptions())
                                 .add(station06, station07, station08, station09)
                                 .addHole(points));

        // 設定區塊的線條寬度、線條顏色、填滿的顏色與指定繪製順序
        polygon.setStrokeWidth(5);
        polygon.setStrokeColor(Color.rgb(102, 153, 0));
        polygon.setFillColor(Color.argb(200, 255, 255, 0));
        polygon.setZIndex(1);

        // 繪製圓形，設定圓心座標、半徑、繪製順序、線條寬度、線條顏色與填滿的顏色
        circle = map.addCircle((new CircleOptions())
                               .center(mapCenter)
                               .radius(2500).zIndex(2)
                               .strokeWidth(0)
                               .strokeColor(Color.argb(200, 255, 0, 0))
                               .fillColor(Color.argb(50, 255, 0, 0)));
    }

}
