package net.macdidi.geofence01;

import android.Manifest;
import android.app.PendingIntent;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.location.Location;
import android.os.Build;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.common.api.GoogleApiClient.ConnectionCallbacks;
import com.google.android.gms.common.api.GoogleApiClient.OnConnectionFailedListener;
import com.google.android.gms.common.api.ResultCallback;
import com.google.android.gms.common.api.Status;
import com.google.android.gms.location.Geofence;
import com.google.android.gms.location.GeofencingRequest;
import com.google.android.gms.location.LocationListener;
import com.google.android.gms.location.LocationRequest;
import com.google.android.gms.location.LocationServices;

// ConnectionCallbacks：連線狀態介面
// OnConnectionFailedListener：連線失敗介面
// ResultCallback<Status>：加入或移除 Geofence 介面
// LocationListener：位置資訊更新介面
public class Geofence01Activity extends AppCompatActivity
    implements ConnectionCallbacks, OnConnectionFailedListener,
    ResultCallback<Status>, LocationListener
{

    private static final String TAG = "Geofence01Activity";

    // Google Play services API 用戶端物件
    private GoogleApiClient googleApiClient;
    // 加入或移除 Geofence 需要的 PendingIntent 物件
    private PendingIntent pendingIntent;
    // 位置資訊更新物件
    private LocationRequest locationRequest;

    // 顯示經、緯度的畫面元件
    private TextView longitude_text, latitude_text;
    // 加入、移除 Geofence 的元件
    private ToggleButton geofence_switch;

    // 請求授權使用的請求代碼
    private static final int REQUEST_LOCATION_PERMISSION = 100;

    // 儲存是否加入 Geofence 的 Preference 資料名稱
    private static final String ADDED_KEY = "GEOFENCE_ADDED";

    // 是否已加入 Geofence
    private boolean isAdded;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_geofence01);

        processViews();

        // 讀取是否已加入 Geofence 的 Preference 資料
        isAdded = PreferenceManager.getDefaultSharedPreferences(this)
                  .getBoolean(ADDED_KEY, false);
        // 設定加入、移除 Geofence 的畫面元件
        geofence_switch.setChecked(isAdded);

        // 建立 Google Play services API 用戶端物件
        configGoogleApiClient();

        // 建立 Location 請求物件
        configLocationRequest();
    }

    @Override
    public void onStart()
    {
        super.onStart();

        // 連線到 Google Play services 用戶端
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
        geofence_switch = (ToggleButton) findViewById(R.id.geofence_switch);
    }

    // 建立 Google services 用戶端物件
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
    // 已經連線到 Google Play services API 用戶端服務
    @Override
    public void onConnected(Bundle bundle)
    {
        // 請求授權
        requestPermission();
    }

    // 實作 ConnectionCallbacks 介面的方法
    // Google Play services API 用戶端服務連線中斷
    @Override
    public void onConnectionSuspended(int i)
    {
        Log.d(TAG, "onConnectionSuspended: " + i);
    }

    // 實作 OnConnectionFailedListener 介面的方法
    // Google Play services API 用戶端服務連線失敗
    // ConnectionResult 參數是連線失敗的資訊
    @Override
    public void onConnectionFailed(ConnectionResult connectionResult)
    {
        Log.d(TAG, "onConnectionFailed: " + connectionResult);
    }

    // 實作 LocationListener 介面的方法
    // 裝置接收到位置更新的資訊
    @Override
    public void onLocationChanged(Location location)
    {
        // 讀取目前位置的經、緯度
        double longitude = location.getLongitude();
        double latitude = location.getLatitude();

        // 設定經、緯度畫面元件
        longitude_text.setText(String.format("%.6f", longitude));
        latitude_text.setText(String.format("%.6f", latitude));
    }

    // 請求位置授權
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
            googleApiClient, locationRequest, Geofence01Activity.this);
    }

    // 加入或移除 Geofence
    public void clickToggleButton(View view)
    {
        ToggleButton tb = (ToggleButton) view;

        if (tb.isChecked()) {
            // 加入 Geofence
            addGeofence();
        } else {
            // 移除 Geofence
            removeGeofence();
        }
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
        .setCircularRegion(25.04622, 121.517451, 100)
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
        // 如果加入或移除成功
        if (status.isSuccess()) {
            // 切換是否已加入 Geofence
            isAdded = !isAdded;
            // 儲存是否已加入 Geofence 的 Preference 資料
            SharedPreferences.Editor editor =
                PreferenceManager.getDefaultSharedPreferences(this).edit();
            editor.putBoolean(ADDED_KEY, isAdded);
            editor.apply();
        } else {
            Log.e(TAG, GeofenceErrorMessage.getErrorMessage(
                      status.getStatusCode()));
        }
    }

}
