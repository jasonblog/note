package net.macdidi.intent03;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.Toast;

public class PhonesActivity extends AppCompatActivity
{

    // 請求授權使用的請求代碼
    private static final int REQUEST_CALL_PHONE_PERMISSION = 100;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_intent03);

        // 取得ListView物件
        ListView listview = (ListView) findViewById(R.id.listview);

        // 建立指定陣列資源的ArrayAdapter物件
        ArrayAdapter<CharSequence> adapterActions =
            ArrayAdapter.createFromResource(
                this, R.array.phones_menu_array,
                android.R.layout.simple_list_item_1);

        // 設定ArrayAdapter物件給ListView物件
        listview.setAdapter(adapterActions);

        // 項目選擇事件
        listview.setOnItemClickListener(new OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view,
                                    int position, long id) {
                switch (position) {
                // 準備撥電話
                case 0:
                    // 建立撥電話的Intent物件
                    Intent intent = new Intent(Intent.ACTION_DIAL);
                    // 設定電話號碼
                    Uri uri = Uri.parse("tel:0933111222");
                    // 設定Uri物件
                    intent.setData(uri);
                    // 啟動元件
                    startActivity(intent);

                    break;

                // 直接撥打電話
                case 1:
                    // 執行使用者授權工作
                    requestCallPhonePermission();
                    break;
                }
            }
        });
    }

    private void requestCallPhonePermission()
    {
        // 如果裝置版本是6.0（包含）以上
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            // 取得授權狀態，參數是請求授權的名稱
            int hasPermission = checkSelfPermission(
                                    Manifest.permission.CALL_PHONE);

            // 如果未授權
            if (hasPermission != PackageManager.PERMISSION_GRANTED) {
                // 請求授權
                //     第一個參數是請求授權的名稱
                //     第二個參數是請求代碼
                requestPermissions(
                    new String[] {Manifest.permission.CALL_PHONE},
                    REQUEST_CALL_PHONE_PERMISSION);
                return;
            }
        }

        // 如果裝置版本是6.0以下，
        // 或是裝置版本是6.0（包含）以上，使用者已經授權，
        // 就直接執行撥電話的工作
        callPhone();
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
        // 如果是撥電話授權請求
        if (requestCode == REQUEST_CALL_PHONE_PERMISSION) {
            // 如果在授權請求選擇「允許」
            if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                // 撥電話
                callPhone();
            }
            // 如果在授權請求選擇「拒絕」
            else {
                // 顯示沒有授權的訊息
                Toast.makeText(this, "沒有撥電話的授權",
                               Toast.LENGTH_SHORT).show();
            }
        } else {
            super.onRequestPermissionsResult(requestCode, permissions,
                                             grantResults);
        }
    }

    private void callPhone()
    {
        // 建立與設定直接撥電話的Intent物件
        Intent intent = new Intent();
        intent.setAction(Intent.ACTION_CALL);

        // 設定電話號碼
        Uri uri = Uri.parse("tel:0933111222");
        intent.setData(uri);

        // 啟動元件
        startActivity(intent);
    }

}
