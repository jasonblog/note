package net.macdidi.location01;

import android.content.pm.PackageManager;
import android.location.Location;
import android.os.Build;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.widget.TextView;
import android.widget.Toast;

import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.location.LocationServices;

// GoogleApiClient.ConnectionCallbacks：連線狀態介面
// GoogleApiClient.OnConnectionFailedListener：連線失敗介面
public class Location01Activity extends AppCompatActivity
    implements GoogleApiClient.ConnectionCallbacks,
    GoogleApiClient.OnConnectionFailedListener
{

    // 顯示經、緯度的畫面元件
    private TextView longitude_text, latitude_text;

    // Google Play services API 用戶
    private GoogleApiClient googleApiClient;

    // 請求授權使用的請求代碼
    private static final int REQUEST_LOCATION_PERMISSION = 100;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_location01);

        processViews();

        // 建立 Google Play services API 用戶端物件
        configGoogleApiClient();
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
            // 中斷 Google Play services 用戶端服務連線
            googleApiClient.disconnect();
        }

        super.onStop();
    }

    private void processViews()
    {
        longitude_text = (TextView) findViewById(R.id.longitude_text);
        latitude_text = (TextView) findViewById(R.id.latitude_text);
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
        Log.d("Location01Activity", "onConnectionSuspended: " + i);
    }

    // 實作 OnConnectionFailedListener 介面的方法
    // Google Play services 連線失敗
    // ConnectionResult 參數是連線失敗的資訊
    @Override
    public void onConnectionFailed(ConnectionResult connectionResult)
    {
        Log.d("Location01Activity",
              "onConnectionFailed: " + connectionResult.getErrorMessage());
    }

    // 請求授權
    private void requestPermission()
    {
        // 如果裝置版本是6.0（包含）以上
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            // 取得讀取低精確度位置資訊授權狀態，參數是請求授權的名稱
            int hasPermission = checkSelfPermission(
                                    android.Manifest.permission.ACCESS_COARSE_LOCATION);

            // 如果未授權
            if (hasPermission != PackageManager.PERMISSION_GRANTED) {
                // 請求讀取低精確度位置資訊授權
                //     第一個參數是請求授權的名稱
                //     第二個參數是請求代碼
                requestPermissions(
                    new String[] {android.Manifest.permission.ACCESS_COARSE_LOCATION},
                    REQUEST_LOCATION_PERMISSION);
                return;
            }
        }

        // 如果裝置版本是6.0以下，
        // 或是裝置版本是6.0（包含）以上，使用者已經授權
        // 讀取與顯示裝置最後儲存的位置
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
                // 讀取與顯示裝置最後儲存的位置
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

    // 讀取與顯示裝置最後儲存的位置
    private void processLocation() throws SecurityException {
        // 讀取裝置最後儲存的位置
        Location lastLocation =
        LocationServices.FusedLocationApi.getLastLocation(
            googleApiClient);

        if (lastLocation != null)
        {
            // 讀取經、緯度
            double longitude = lastLocation.getLongitude();
            double latitude = lastLocation.getLatitude();

            // 設定經、緯度畫面元件
            longitude_text.setText(String.format("%.6f", longitude));
            latitude_text.setText(String.format("%.6f", latitude));
        }
    }

}

