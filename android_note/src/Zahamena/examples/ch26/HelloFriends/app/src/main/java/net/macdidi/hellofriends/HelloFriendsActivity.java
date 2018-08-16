package net.macdidi.hellofriends;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.location.Location;
import android.os.Build;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;

import com.firebase.ui.auth.AuthUI;
import com.firebase.ui.auth.ErrorCodes;
import com.firebase.ui.auth.IdpResponse;
import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.location.LocationListener;
import com.google.android.gms.location.LocationRequest;
import com.google.android.gms.location.LocationServices;
import com.google.android.gms.maps.CameraUpdate;
import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.UiSettings;
import com.google.android.gms.maps.model.BitmapDescriptorFactory;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;
import com.google.firebase.database.ChildEventListener;
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;

import java.util.Arrays;
import java.util.List;

// OnMapReadyCallback：地圖元件建立完成介面
// GoogleApiClient.ConnectionCallbacks：連線狀態介面
// GoogleApiClient.OnConnectionFailedListener：連線失敗介面
// LocationListener：位置資訊更新介面
public class HelloFriendsActivity extends AppCompatActivity
    implements OnMapReadyCallback, GoogleApiClient.ConnectionCallbacks,
    GoogleApiClient.OnConnectionFailedListener,
    LocationListener
{

    // 地圖元件
    private GoogleMap map;
    // 地圖 UI 設定物件
    private UiSettings uiSettings;
    // 顯示位置的 Marker
    private Marker friendMarker;

    // Google Play services API 用戶
    private GoogleApiClient googleApiClient;
    // 位置資訊更新物件
    private LocationRequest locationRequest;
    // 請求授權使用的請求代碼
    private static final int REQUEST_LOCATION_PERMISSION = 100;

    // 列表元件與資料來源物件
    private RecyclerView item_list;
    private FriendAdapter itemAdapter;

    // 是否分享我的位置
    private boolean isShare = false;
    // 登入、啟動與關閉分享的選單項目物件
    private MenuItem menu_login, menu_share;

    // Firebase節點物件
    private DatabaseReference databaseRef;

    // 我的位置物件
    private Friend me;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_hello_friends);

        processViews();

        // 建立 RecyclerView 元件的資料來源物件
        itemAdapter = new FriendAdapter(this) {
            @Override
            public void onBindViewHolder(final ViewHolder holder,
                                         final int position) {
                super.onBindViewHolder(holder, position);

                // 建立與註冊項目點擊監聽事件
                holder.rootView.setOnClickListener(
                new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        // 點擊項目
                        clickItem(position);
                    }
                }
                );
            }
        };

        // 設定 RecyclerView 使用的資料來源物件
        item_list.setAdapter(itemAdapter);

        // 建立 Google Play services API 用戶端物件
        configGoogleApiClient();
        // 建立位置資訊更新物件
        configLocationRequest();
    }

    // 點擊項目
    private void clickItem(int position)
    {
        // 讀取選擇位置的項目物件
        Friend friend = itemAdapter.get(position);

        // 如果點擊的項目是分享狀態
        if (friend.isOnline()) {
            // 建立項目的座標物件
            LatLng where = new LatLng(
                friend.getLatitude(), friend.getLongitude());

            // 如果還沒有加入 Marker
            if (friendMarker == null) {
                // 加入 Marker
                friendMarker = map.addMarker(new MarkerOptions()
                                             .position(where)
                                             .title(friend.getEmail())
                                             .icon(BitmapDescriptorFactory.fromResource(
                                                       R.drawable.ic_person_pin_black_48dp)));
            } else  {
                // 設定 Marker 座標與標題
                friendMarker.setPosition(where);
                friendMarker.setTitle(friend.getEmail());
            }

            // 移動地圖
            CameraUpdate cameraUpdate = CameraUpdateFactory.newLatLngZoom(
                                            where, 13);

            map.animateCamera(cameraUpdate, new GoogleMap.CancelableCallback() {
                @Override
                public void onFinish() {
                    // 動畫結束時顯示 Marker 訊息框
                    friendMarker.showInfoWindow();
                }

                @Override
                public void onCancel() { }
            });
        } else {
            Toast.makeText(HelloFriendsActivity.this, "Sharing Disabled",
                           Toast.LENGTH_SHORT).show();
        }
    }

    // 設定 Firebase
    private void configFirebase()
    {
        // 建立 FirebaseDatabase 物件
        FirebaseDatabase database = FirebaseDatabase.getInstance();
        // Firebase節點名稱
        final String FIREBASE_CHILD = "hellofriends";
        // 建立Firebase節點物件
        databaseRef = database.getReference(FIREBASE_CHILD);
        // 建立儲存分享位置的 Firebase 物件
        DatabaseReference myLocation = databaseRef.child(me.emailToNode());
        // 儲存位置資訊到 Firebase
        myLocation.setValue(me);

        // 註冊 Firebase 節點監聽事件
        databaseRef.addChildEventListener(new ChildEventListener() {

            // 新增節點
            @Override
            public void onChildAdded(DataSnapshot dataSnapshot, String s) {

                // 讀取節點儲存的物件
                Friend friend = dataSnapshot.getValue(Friend.class);

                // 如果不是自己儲存的節點
                if (!friend.getEmail().equals(me.getEmail())) {
                    // 加入節點儲存的項目
                    itemAdapter.add(friend);
                }
            }

            // 節點異動
            @Override
            public void onChildChanged(DataSnapshot dataSnapshot, String s) {
                // 讀取節點儲存的物件
                Friend friend = dataSnapshot.getValue(Friend.class);
                // 修改節點儲存的項目
                itemAdapter.update(friend);
            }

            // 節點刪除
            @Override
            public void onChildRemoved(DataSnapshot dataSnapshot) {
                // 讀取節點儲存的物件
                Friend friend = dataSnapshot.getValue(Friend.class);
                // 刪除節點儲存的項目
                itemAdapter.remove(friend);
            }

            @Override
            public void onChildMoved(DataSnapshot dataSnapshot, String s) {

            }

            @Override
            public void onCancelled(DatabaseError databaseError) {

            }
        });
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
        // 設定優先讀取高精確度的位置資訊（GPS）
        locationRequest.setPriority(LocationRequest.PRIORITY_HIGH_ACCURACY);
        // 設定只更新位置一次
        locationRequest.setNumUpdates(1);
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
        super.onStop();

        // 如果已分享我的位置
        if (isShare) {
            // 設定為沒有分享我的位置並儲存到 Firebase
            me.setOnline(false);
            databaseRef.child(me.emailToNode()).setValue(me);
        }

        // 如果已經連線到 Google Play services 用戶端服務
        if (googleApiClient.isConnected()) {
            // 中斷 Google Play services 用戶端連線
            googleApiClient.disconnect();
        }
    }

    // 實作 ConnectionCallbacks 介面的方法
    // 已經連線到 Google Play services
    @Override
    public void onConnected(Bundle bundle)
    {
        Log.d("FindMyFriendActivity", "onConnected");
    }

    // 實作 ConnectionCallbacks 介面的方法
    // Google Play services 連線中斷
    // int 參數是連線中斷的代號
    @Override
    public void onConnectionSuspended(int i)
    {
        Log.d("FindMyFriendActivity", "onConnectionSuspended: " + i);
    }

    // 實作 OnConnectionFailedListener 介面的方法
    // Google Play services 連線失敗
    // ConnectionResult 參數是連線失敗的資訊
    @Override
    public void onConnectionFailed(ConnectionResult connectionResult)
    {
        Log.d("FindMyFriendActivity",
              "onConnectionFailed: " + connectionResult.getErrorMessage());
    }

    // 實作 LocationListener 介面的方法
    // 位置資訊更新的時候，Android 會自動呼叫這個方法
    // Location 參數是目前的位置
    @Override
    public void onLocationChanged(Location location)
    {
        // 設定座標與已分享狀態
        me.setLatitude(location.getLatitude());
        me.setLongitude(location.getLongitude());
        me.setOnline(true);

        // 儲存到 Firebase
        databaseRef.child(me.emailToNode()).setValue(me);

        // 移動地圖
        LatLng myLocation = new LatLng(
            location.getLatitude(), location.getLongitude());
        CameraUpdate cameraUpdate = CameraUpdateFactory.newLatLngZoom(
                                        myLocation, 13);
        map.animateCamera(cameraUpdate);
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
        // 分享我的位置
        processShare();
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
                // 分享我的位置
                processShare();
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

    // 分享位置
    private void processShare() throws SecurityException {
        // 設定是否分享我的位置狀態
        isShare = !isShare;

        // 設定選單圖示
        menu_share.setIcon(isShare ?
                           R.drawable.ic_verified_user_white_48dp :
                           R.drawable.ic_cloud_white_48dp);

        // 顯示是否分享我的位置狀態訊息框
        Toast.makeText(this, (isShare ? "Shared" : "Disabled"),
                       Toast.LENGTH_SHORT).show();

        // 如果是分享我的位置
        if (isShare)
        {
            // 啟動位置更新服務
            LocationServices.FusedLocationApi.requestLocationUpdates(
                googleApiClient, locationRequest, HelloFriendsActivity.this);
        } else
        {
            // 設定為不分享我的位置
            me.setOnline(false);
            // 儲存到 Firebase
            databaseRef.child(me.emailToNode()).setValue(me);
        }

        // 設定我的位置圖層與我的位置按鈕
        map.setMyLocationEnabled(isShare);
        uiSettings.setMyLocationButtonEnabled(isShare);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu)
    {
        // 取得載入選單用的 MenuInflater 物件
        MenuInflater menuInflater = getMenuInflater();
        // 呼叫 inflate 方法載入指定的選單資源，第二個參數是這個方法的 Menu 物件
        menuInflater.inflate(R.menu.menu_hello_friends, menu);
        // 讀取啟動與關閉分享的選單項目物件
        menu_share = menu.findItem(R.id.menu_share);
        menu_login = menu.findItem(R.id.menu_login);

        menu_share.setEnabled(false);
        menu_login.setEnabled(true);

        // 回傳 true 選單才會顯示
        return true;
    }

    // 參數 MenuItem 是使用者選擇的選單項目物件
    @Override
    public boolean onOptionsItemSelected(MenuItem item)
    {
        // 取得選單項目的資源編號
        int id = item.getItemId();

        switch (id) {
        case R.id.menu_login:
            processLogin();
            break;

        case R.id.menu_share:
            // 請求授權
            requestPermission();
            break;
        }

        return super.onOptionsItemSelected(item);
    }

    private void processViews()
    {
        SupportMapFragment mapFragment = (SupportMapFragment)
                                         getSupportFragmentManager().findFragmentById(R.id.map);
        mapFragment.getMapAsync(this);

        item_list = (RecyclerView) findViewById(R.id.item_list);
        // 執行 RecyclerView 元件的設定
        item_list.setHasFixedSize(true);
        // 建立與設定 RecyclerView 元件的配置元件
        item_list.setLayoutManager(new LinearLayoutManager(this));
    }

    // Firebase登入功能的請求代碼
    private static final int FIREBASE_SIGN_IN = 123;

    private void processLogin()
    {
        // 加入電子郵件登入項目
        List<AuthUI.IdpConfig> providers = Arrays.asList(
                                               new AuthUI.IdpConfig.Builder(AuthUI.EMAIL_PROVIDER).build());

        // 建立啟動Firebase登入功能的Intent物件
        Intent loginIntent = AuthUI.getInstance()
                             .createSignInIntentBuilder()
                             .setAvailableProviders(providers)
                             .build();
        // 啟動Firebase登入功能
        startActivityForResult(loginIntent, FIREBASE_SIGN_IN);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode,
                                    Intent data)
    {
        super.onActivityResult(requestCode, resultCode, data);

        // 如果請求代碼是Firebase登入功能
        if (requestCode == FIREBASE_SIGN_IN) {
            // 讀取Firebase登入功能的結果
            IdpResponse response = IdpResponse.fromResultIntent(data);

            // 如果使用者完成登入
            if (resultCode == RESULT_OK) {
                // 讀取使用者輸入的電子郵件
                String email = response.getEmail();
                // 建立我的位置物件
                me = new Friend(email, 0, 0, false);

                // 設定 Firebase
                configFirebase();

                // 設定功能表
                menu_login.setIcon(R.drawable.ic_check_circle_white_48dp);
                menu_share.setEnabled(true);
                menu_login.setEnabled(false);
            } else {
                // 使用者取消登入
                if (response == null) {
                    Log.d("==========", "使用者取消登入");
                }
                // 沒有網際網路連線
                else if (response.getErrorCode() == ErrorCodes.NO_NETWORK) {
                    Log.d("==========", "沒有網際網路連線");
                }
                // 其它錯誤
                else if (response.getErrorCode() == ErrorCodes.UNKNOWN_ERROR) {
                    Log.d("==========", "發生錯誤");
                }
            }
        }
    }

    @Override
    public void onMapReady(GoogleMap googleMap)
    {
        map = googleMap;

        // 取得地圖 UI 設定物件
        uiSettings = map.getUiSettings();
    }

}
