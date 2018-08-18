package net.macdidi.geofencemap01;

import android.Manifest;
import android.app.PendingIntent;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.location.Location;
import android.location.LocationManager;
import android.os.Build;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.widget.Toast;

import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.common.api.ResultCallback;
import com.google.android.gms.common.api.Status;
import com.google.android.gms.location.Geofence;
import com.google.android.gms.location.GeofencingRequest;
import com.google.android.gms.location.LocationListener;
import com.google.android.gms.location.LocationRequest;
import com.google.android.gms.location.LocationServices;
import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.BitmapDescriptorFactory;
import com.google.android.gms.maps.model.Circle;
import com.google.android.gms.maps.model.CircleOptions;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;

// OnMapReadyCallback：地圖元件建立完成介面
// GoogleApiClient.ConnectionCallbacks：連線狀態介面
// GoogleApiClient.OnConnectionFailedListener：連線失敗介面
// LocationListener：位置資訊更新介面
// ResultCallback<Status>：加入或移除 Geofence 介面
public class GeofenceMap01Activity extends AppCompatActivity
    implements OnMapReadyCallback,
    GoogleApiClient.ConnectionCallbacks,
    GoogleApiClient.OnConnectionFailedListener,
    LocationListener,
    ResultCallback<Status>
{

    // Google Map 元件
    private GoogleMap map;

    // Google Play services API 用戶
    private GoogleApiClient googleApiClient;

    // Location 請求物件
    private LocationRequest locationRequest;

    // 加入或移除 Geofence 需要的 PendingIntent 物件
    private PendingIntent pendingIntent;

    // 地圖元件是否已建立完成
    private boolean mapReady = false;

    // 請求授權使用的請求代碼
    private static final int REQUEST_LOCATION_PERMISSION = 100;

    // 顯示目前位置的 Marker 物件
    private Marker marker;

    // 顯示 Geofence 範圍
    private Circle circle;

    // Geofence 中心位置座標
    private static final LatLng center = new LatLng(25.06408, 121.50315);

    // 傳送模擬位置到實體裝置的物件
    private MockProvider mockProvider;

    private static final String TAG = "GeofenceMap01Activity";

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_geofence_map01);

        processViews();

        // 建立 Google Play services API 用戶端物件
        configGoogleApiClient();

        // 建立 Location 請求物件
        configLocationRequest();

        // 建立傳送模擬位置到實體裝置的物件
        mockProvider = new MockProvider(
            LocationManager.GPS_PROVIDER, this);

    }

    @Override
    public void onStart()
    {
        super.onStart();

        // 連線到 Google Play services 用戶端
        googleApiClient.connect();

        // 啟動傳送模擬位置到實體裝置
        mockProvider.start();
    }

    @Override
    public void onPause()
    {
        // 移除 Geofence
        removeGeofence();

        super.onPause();
    }

    @Override
    public void onStop()
    {
        if (googleApiClient.isConnected()) {
            // 中斷 Google Play services 用戶端連線
            googleApiClient.disconnect();
        }

        // 停止傳送模擬位置到實體裝置
        mockProvider.shutdown();

        super.onStop();
    }

    // 建立 Google Play services API 用戶端物件
    private synchronized void configGoogleApiClient()
    {
        if (googleApiClient == null) {
            // addConnectionCallbacks 設定連線狀態介面
            // addOnConnectionFailedListener 設定連線失敗介面
            // addApi 設定加入位置 API
            googleApiClient = new GoogleApiClient.Builder(this)
            .addConnectionCallbacks(this)
            .addOnConnectionFailedListener(this)
            .addApi(LocationServices.API)
            .build();
        }
    }

    // 建立 Location 請求物件
    private void configLocationRequest()
    {
        locationRequest = new LocationRequest();
        // 設定讀取位置資訊的間隔時間為一秒（1000ms）
        locationRequest.setInterval(1000);
        // 設定讀取位置資訊最快的間隔時間為一秒（1000ms）
        locationRequest.setFastestInterval(1000);
        // 設定優先讀取高精確度的位置資訊（GPS）
        locationRequest.setPriority(LocationRequest.PRIORITY_HIGH_ACCURACY);
    }

    // 實作 ConnectionCallbacks 介面的方法
    // 已經連線到 Google Play services
    @Override
    public void onConnected(Bundle bundle)
    {
        // 請求授權
        requestPermission();
    }

    // 實作 ConnectionCallbacks 介面的方法
    // Google Play services 連線中斷
    // int 參數是連線中斷的代號
    @Override
    public void onConnectionSuspended(int i)
    {
        Log.d(TAG, "onConnectionSuspended: " + i);
    }

    // 實作 OnConnectionFailedListener 介面的方法
    // Google Play services 連線失敗
    // ConnectionResult 參數是連線失敗的資訊
    @Override
    public void onConnectionFailed(ConnectionResult connectionResult)
    {
        Log.d(TAG, "onConnectionFailed: " + connectionResult.getErrorMessage());
    }

    // 實作 LocationListener 介面的方法
    // 位置資訊更新的時候，Android 會自動呼叫這個方法
    // Location 參數是目前的位置
    @Override
    public void onLocationChanged(Location location)
    {
        // 如果地圖元件還沒有建立完成就返回
        if (!mapReady) {
            return;
        }

        // 讀取目前位置座標
        LatLng now = new LatLng(
            location.getLatitude(), location.getLongitude());
        // 設定地圖標記的位置座標
        marker.setPosition(now);
        // 移動地圖到目前位置
        map.animateCamera(CameraUpdateFactory.newLatLngZoom(now, 17));
    }

    // 請求授權
    private void requestPermission()
    {
        // 如果裝置版本是6.0（包含）以上
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            // 取得讀取高精確度位置資訊授權狀態，參數是請求授權的名稱
            int hasPermission = checkSelfPermission(
                                    Manifest.permission.ACCESS_FINE_LOCATION);

            // 如果未授權
            if (hasPermission != PackageManager.PERMISSION_GRANTED) {
                // 請求讀取高精確度位置資訊授權
                //     第一個參數是請求授權的名稱
                //     第二個參數是請求代碼
                requestPermissions(
                    new String[] {Manifest.permission.ACCESS_FINE_LOCATION},
                    REQUEST_LOCATION_PERMISSION);
                return;
            }
        }

        // 如果裝置版本是6.0以下，
        // 或是裝置版本是6.0（包含）以上，使用者已經授權
        // 啟動位置更新服務
        processLocation();
    }

    // 使用者完成授權的選擇以後，Android會呼叫這個方法
    //     第一個參數：請求授權代碼
    //     第二個參數：請求的授權名稱
    //     第三個參數：使用者選擇授權的結果
    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           String[] permissions,
                                           int[] grantResults)
    {
        // 如果是讀取位置授權請求
        if (requestCode == REQUEST_LOCATION_PERMISSION) {
            // 如果在授權請求選擇「允許」
            if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                // 啟動位置更新服務
                processLocation();
            }
            // 如果在授權請求選擇「拒絕」
            else {
                // 顯示沒有授權的訊息
                Toast.makeText(this, "沒有讀取位置授權",
                               Toast.LENGTH_SHORT).show();
            }
        } else {
            super.onRequestPermissionsResult(requestCode, permissions,
                                             grantResults);
        }
    }

    // 啟動位置更新服務
    private void processLocation() throws SecurityException {
        // 位置資訊更新的時候，應用程式會自動呼叫 LocationListener.onLocationChanged
        LocationServices.FusedLocationApi.requestLocationUpdates(
            googleApiClient, locationRequest, GeofenceMap01Activity.this);

        // 加入 Geofence
        addGeofence();
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
        // 地圖元件已建立完成
        mapReady = true;

        // 繪製 Geofence 範圍的圓形
        // 繪製圓形，設定圓心座標、半徑、繪製順序、線條寬度、線條顏色與填滿的顏色
        circle = map.addCircle((new CircleOptions())
                               .center(center)
                               .radius(150)
                               .strokeWidth(0)
                               .strokeColor(Color.argb(200, 255, 0, 0))
                               .fillColor(Color.argb(50, 255, 0, 0)));

        // 顯示指定位置的地圖
        map.moveCamera(CameraUpdateFactory.newLatLngZoom(center, 17));

        // 加入 Marker 到地圖並設定地點和圖示
        marker = map.addMarker(new MarkerOptions()
                               .position(new LatLng(23.975117, 120.981588))
                               .icon(BitmapDescriptorFactory.fromResource(
                                         R.drawable.ic_directions_walk_black_48dp)));
    }

    // 加入 Geofence
    private void addGeofence()
    {
        // 如果沒有連線到 Google services 用戶端就返回
        if (!googleApiClient.isConnected()) {
            Log.d(TAG, "Google service is not available");
            return;
        }

        // 建立 Geofence 物件
        //   setRequestId 設定名稱（ID）
        //   setCircularRegion 設定中心座標與半徑（公尺）
        //   setExpirationDuration 設定有效時間（微秒）
        //   setTransitionTypes 設定事件的種類（進入與離開）
        Geofence geofence = new Geofence.Builder()
        .setRequestId("Center")
        .setCircularRegion(center.latitude, center.longitude, 150)
        .setExpirationDuration(6 * 60 * 60 * 1000)
        .setTransitionTypes(Geofence.GEOFENCE_TRANSITION_ENTER |
                            Geofence.GEOFENCE_TRANSITION_EXIT)
        .build();

        // 建立 Geofence 請求物件
        //   setInitialTrigger 設定第一次需要處理的事件種類
        //   addGeofence 設定 Geofence 物件
        GeofencingRequest request = new GeofencingRequest.Builder()
        .setInitialTrigger(GeofencingRequest.INITIAL_TRIGGER_ENTER)
        .addGeofence(geofence)
        .build();

        try {
            // 加入 Geofence
            LocationServices.GeofencingApi.addGeofences(
                googleApiClient, request, getPendingIntent()
            ).setResultCallback(this);
        } catch (SecurityException securityException) {
            Log.d(TAG, securityException.toString());
        }
    }

    // 移除 Geofence
    private void removeGeofence()
    {
        // 如果沒有連線到 Google services 用戶端就返回
        if (!googleApiClient.isConnected()) {
            Log.d(TAG, "Google service is not available");
            return;
        }

        try {
            // 移除 Geofence
            LocationServices.GeofencingApi.removeGeofences(
                googleApiClient,
                getPendingIntent()
            ).setResultCallback(this);
        } catch (SecurityException securityException) {
            Log.d(TAG, securityException.toString());
        }
    }

    // 建立 Geofence 需要的 PendingIntent 物件
    private PendingIntent getPendingIntent()
    {
        if (pendingIntent != null) {
            return pendingIntent;
        }

        // 建立啟動 GeofenceIntentService 服務元件的 Intent 物件
        Intent intent = new Intent(
            this, GeofenceIntentService.class);
        // 建立與指定 GeofenceIntentService 服務元件
        pendingIntent = PendingIntent.getService(this, 0, intent,
                        PendingIntent.FLAG_UPDATE_CURRENT);
        return pendingIntent;
    }

    // 實作 ResultCallback<Status> 介面的方法
    // 加入或移除 Geofence 後自動呼叫這個方法
    public void onResult(Status status)
    {
        // 如果加入或移除失敗
        if (!status.isSuccess()) {
            Log.e(TAG, GeofenceErrorMessage.getErrorMessage(
                      status.getStatusCode()));
        }
    }

}
