package net.macdidi.map04;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.Switch;
import android.widget.Toast;

import com.google.android.gms.maps.CameraUpdate;
import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.MapsInitializer;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.UiSettings;
import com.google.android.gms.maps.model.CameraPosition;
import com.google.android.gms.maps.model.LatLng;

// OnMapReadyCallback：地圖元件建立完成介面
public class Map04Activity extends AppCompatActivity
    implements OnMapReadyCallback
{

    // Google Map 元件
    private GoogleMap map;
    // Google Map UI 設定元件
    private UiSettings uiSettings;

    // 是否使用動畫移動地圖
    private Switch animate_switch;
    // 按鈕元件
    //     停止、移動到第一個地點(台北市立美術館)、移動到第二個地點(國立台灣美術館)、
    //     移動到第三個地點(高雄市立美術館) 、向上移動、向下移動、向左移動、向右移動、
    //     放大、縮小
    private Button stop_button, camera01_button, camera02_button,
            camera03_button, scroll_up_button, scroll_down_button,
            scroll_left_button, scroll_right_button, zoom_in_button,
            zoom_out_button;

    // 地圖動畫完成與停止介面
    private GoogleMap.CancelableCallback myCancelableCallback;

    // 第一個地點(台北市立美術館)
    private final CameraPosition position01 =
        new CameraPosition.Builder()
    .target(new LatLng(25.071742, 121.524256)) // 設定位置
    .zoom(15) // 設定縮放
    .bearing(300) // 設定方向
    .tilt(50) // 設定傾斜度
    .build(); // 建立設定好的CameraPosition物件
    private CameraUpdate cameraUpdate01;

    // 第二個地點(國立台灣美術館)
    private final CameraPosition position02 =
        new CameraPosition.Builder()
    .target(new LatLng(24.141369, 120.663246))
    .zoom(14)
    .bearing(0)
    .tilt(25)
    .build();
    private CameraUpdate cameraUpdate02;

    // 第三個地點(高雄市立美術館)
    private final CameraPosition position03 = new
    CameraPosition.Builder()
    .target(new LatLng(22.656076, 120.287071))
    .zoom(17)
    .bearing(180)
    .tilt(50)
    .build();
    private CameraUpdate cameraUpdate03;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_map04);

        processViews();
        processControllers();
        processCameraUpdate();
    }

    private void processCameraUpdate()
    {
        MapsInitializer.initialize(this);
        cameraUpdate01 =
            CameraUpdateFactory.newCameraPosition(position01);
        cameraUpdate02 =
            CameraUpdateFactory.newCameraPosition(position02);
        cameraUpdate03 =
            CameraUpdateFactory.newCameraPosition(position03);
    }

    private void processViews()
    {
        // 讀取型態為 SupportMapFragment 的 Google Map 物件
        SupportMapFragment mapFragment = (SupportMapFragment)
                                         getSupportFragmentManager().findFragmentById(R.id.map);
        // 註冊地圖元件建立完成監聽物件
        mapFragment.getMapAsync(this);

        // 動畫開啟或關閉
        animate_switch = (Switch) findViewById(R.id.animate_switch);

        // 操作功能按鈕元件
        stop_button = (Button) findViewById(R.id.stop_button);
        camera01_button = (Button) findViewById(R.id.camera01_button);
        camera02_button = (Button) findViewById(R.id.camera02_button);
        camera03_button = (Button) findViewById(R.id.camera03_button);
        scroll_up_button = (Button) findViewById(R.id.scroll_up_button);
        scroll_down_button = (Button) findViewById(R.id.scroll_down_button);
        scroll_left_button = (Button) findViewById(R.id.scroll_left_button);
        scroll_right_button = (Button) findViewById(R.id.scroll_right_button);
        zoom_in_button = (Button) findViewById(R.id.zoom_in_button);
        zoom_out_button = (Button) findViewById(R.id.zoom_out_button);
    }

    private void processControllers()
    {
        // 建立操作功能按鈕監聽物件
        View.OnClickListener buttonListener = new View.OnClickListener() {

            @Override
            public void onClick(View view) {
                if (view == stop_button) {
                    // 停止動畫
                    map.stopAnimation();
                } else if (view == camera01_button) {
                    // 移動地圖
                    processMap(cameraUpdate01, myCancelableCallback);
                } else if (view == camera02_button) {
                    // 移動地圖
                    processMap(cameraUpdate02, 3000, myCancelableCallback);
                } else if (view == camera03_button) {
                    // 移動地圖
                    processMap(cameraUpdate03, 10000, myCancelableCallback);
                } else if (view == scroll_up_button) {
                    // 向上移動地圖
                    processMap(CameraUpdateFactory.scrollBy(0, -120), null);
                } else if (view == scroll_down_button) {
                    // 向下移動地圖
                    processMap(CameraUpdateFactory.scrollBy(0, 120), null);
                } else if (view == scroll_left_button) {
                    // 向左移動地圖
                    processMap(CameraUpdateFactory.scrollBy(-120, 0), null);
                } else if (view == scroll_right_button) {
                    // 向右移動地圖
                    processMap(CameraUpdateFactory.scrollBy(120, 0), null);
                } else if (view == zoom_in_button) {
                    // 放大
                    processMap(CameraUpdateFactory.zoomIn(), null);
                } else if (view == zoom_out_button) {
                    // 縮小
                    processMap(CameraUpdateFactory.zoomOut(), null);
                }
            }

        };

        // 註冊操作功能按鈕監聽物件
        stop_button.setOnClickListener(buttonListener);
        camera01_button.setOnClickListener(buttonListener);
        camera02_button.setOnClickListener(buttonListener);
        camera03_button.setOnClickListener(buttonListener);
        scroll_up_button.setOnClickListener(buttonListener);
        scroll_down_button.setOnClickListener(buttonListener);
        scroll_left_button.setOnClickListener(buttonListener);
        scroll_right_button.setOnClickListener(buttonListener);
        zoom_in_button.setOnClickListener(buttonListener);
        zoom_out_button.setOnClickListener(buttonListener);

        // 建立地圖動畫停止監聽物件
        myCancelableCallback = new GoogleMap.CancelableCallback() {
            @Override
            public void onFinish() {
                // 動畫結束
                Toast.makeText(getBaseContext(), "Done!", Toast.LENGTH_SHORT).show();
            }

            @Override
            public void onCancel() {
                // 動畫取消
                Toast.makeText(getBaseContext(), "Canceled!", Toast.LENGTH_SHORT).show();
            }
        };
    }

    // 移動地圖
    //   CameraUpdate 參數：更新地圖的 CameraUpdate 物件
    //   int 參數：使用動畫效果更新地圖的時間
    //   GoogleMap.CancelableCallback 參數：地圖動畫完成與停止監聽物件

    private void processMap(CameraUpdate cameraUpdate, int duration,
                            GoogleMap.CancelableCallback cancelableCallback)
    {
        // 是否使用動畫效果
        boolean isAnimate = animate_switch.isChecked();

        if (isAnimate) {
            // 使用動畫效果
            map.animateCamera(cameraUpdate, duration, cancelableCallback);
        } else {
            // 直接顯示
            map.moveCamera(cameraUpdate);
        }
    }

    // 移動地圖
    //   CameraUpdate 參數：更新地圖的 CameraUpdate 物件
    //   GoogleMap.CancelableCallback 參數：地圖動畫完成與停止監聽物件
    private void processMap(CameraUpdate cameraUpdate,
                            GoogleMap.CancelableCallback cancelableCallback)
    {
        // 是否使用動畫效果
        boolean isAnimate = animate_switch.isChecked();

        if (isAnimate) {
            // 使用動畫效果
            map.animateCamera(cameraUpdate, cancelableCallback);
        } else {
            // 直接顯示
            map.moveCamera(cameraUpdate);
        }
    }

    // 地圖元件建立完成時，Android 自動呼叫這個方法
    // 參數是建立好的 Google Map 物件
    @Override
    public void onMapReady(GoogleMap googleMap)
    {
        // 設定 Google Map 物件
        map = googleMap;

        // 取得地圖 UI 設定物件
        uiSettings = map.getUiSettings();

        // 取消捲動按鈕
        uiSettings.setZoomControlsEnabled(false);

        // 禁止縮放、捲動、傾斜與旋轉操作
        uiSettings.setScrollGesturesEnabled(false);
        uiSettings.setZoomGesturesEnabled(false);
        uiSettings.setTiltGesturesEnabled(false);
        uiSettings.setRotateGesturesEnabled(false);

        // 建立準備要移動到台灣地理中心點的 CameraUpdate 物件
        LatLng center = new LatLng(23.975117, 120.981588);
        CameraUpdate cameraUpdate = CameraUpdateFactory.newLatLngZoom(
                                        center, 12);
        // 直接顯示指定位置的地圖
        map.moveCamera(cameraUpdate);
    }

}
