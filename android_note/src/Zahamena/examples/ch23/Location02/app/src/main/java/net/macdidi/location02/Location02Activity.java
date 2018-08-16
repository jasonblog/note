package net.macdidi.location02;

import android.Manifest;
import android.content.pm.PackageManager;
import android.location.Location;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.location.LocationListener;
import com.google.android.gms.location.LocationRequest;
import com.google.android.gms.location.LocationServices;

import java.lang.ref.WeakReference;

// GoogleApiClient.ConnectionCallbacks：連線狀態介面
// GoogleApiClient.OnConnectionFailedListener：連線失敗介面
// LocationListener：位置資訊更新介面
public class Location02Activity extends AppCompatActivity
    implements GoogleApiClient.ConnectionCallbacks,
    GoogleApiClient.OnConnectionFailedListener,
    LocationListener
{

    // 顯示經、緯度的畫面元件
    private TextView longitude_text, latitude_text;
    // 讀取位置狀態圖示
    private ImageView my_location;

    // Google Play services API 用戶
    private GoogleApiClient googleApiClient;

    // 位置資訊更新物件
    private LocationRequest locationRequest;

    // 請求授權使用的請求代碼
    private static final int REQUEST_LOCATION_PERMISSION = 100;

    // 宣告控制讀取位置狀態圖示的 Handler 物件
    private Handler locationHandler = new LocationHandler(this);

    // 宣告控制讀取位置狀態圖示的 Handler 類別
    private static class LocationHandler extends Handler
    {

        private final WeakReference<Location02Activity> hActivity;

        public LocationHandler(Location02Activity activity)
        {
            hActivity = new WeakReference<>(activity);
        }

        @Override
        public void handleMessage(Message msg)
        {
            Location02Activity activity = hActivity.get();

            if (activity != null) {
                super.handleMessage(msg);
                // 隱藏讀取位置狀態圖示
                activity.my_location.setVisibility(View.INVISIBLE);
            }
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_location02);

        processViews();

        // 建立 Google Play services API 用戶端物件
        configGoogleApiClient();

        // 建立位置資訊更新物件
        configLocationRequest();
    }

    @Override
    public void onStart()
    {
        super.onStart();

        // 連線到 Google Play services 用戶端服務
        googleApiClient.connect();
    }

    @Override
    public void onStop()
    {
        // 如果已經連線到 Google Play services 用戶端服務
        if (googleApiClient.isConnected()) {
            // 中斷 Google Play services 用戶端連線
            googleApiClient.disconnect();
        }

        super.onStop();
    }

    private void processViews()
    {
        longitude_text = (TextView) findViewById(R.id.longitude_text);
        latitude_text = (TextView) findViewById(R.id.latitude_text);
        my_location = (ImageView) findViewById(R.id.my_location);
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

    // 建立與設定位置資訊更新物件
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
        Log.d("Location02Activity", "onConnectionSuspended: " + i);
    }

    // 實作 OnConnectionFailedListener 介面的方法
    // Google Play services 連線失敗
    // ConnectionResult 參數是連線失敗的資訊
    @Override
    public void onConnectionFailed(ConnectionResult connectionResult)
    {
        Log.d("Location02Activity",
              "onConnectionFailed: " + connectionResult.getErrorMessage());
    }

    // 實作 LocationListener 介面的方法
    // 位置資訊更新的時候，Android 會自動呼叫這個方法
    // Location 參數是目前的位置
    @Override
    public void onLocationChanged(Location location)
    {
        // 顯示讀取位置狀態圖示
        my_location.setVisibility(View.VISIBLE);
        // 0.8 秒以後執行 Handler
        locationHandler.sendEmptyMessageDelayed(0, 800);

        // 讀取目前位置的經、緯度
        double longitude = location.getLongitude();
        double latitude = location.getLatitude();

        // 設定經、緯度畫面元件
        longitude_text.setText(String.format("%.6f", longitude));
        latitude_text.setText(String.format("%.6f", latitude));
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
            googleApiClient, locationRequest, Location02Activity.this);
    }

}