package net.macdidi.intent03;

import android.app.SearchManager;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.ListView;

public class WebsActivity extends AppCompatActivity
{

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
                this, R.array.webs_menu_array,
                android.R.layout.simple_list_item_1);

        // 設定ArrayAdapter物件給ListView物件
        listview.setAdapter(adapterActions);

        // 項目選擇事件
        listview.setOnItemClickListener(new OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view,
                                    int position, long id) {
                Intent intent = new Intent();
                Uri uri = null;

                switch (position) {
                // 瀏覽網頁
                case 0:
                    // 設定Action名稱為查詢
                    intent.setAction(Intent.ACTION_VIEW);

                    // 設定網頁
                    uri = Uri.parse("http://www.codedata.com.tw/kotlin/kt01/");

                    // 設定Uri物件
                    intent.setData(uri);
                    break;

                // 搜尋
                case 1:
                    // 設定Action名稱為搜尋
                    intent.setAction(Intent.ACTION_WEB_SEARCH);

                    // 設定查詢關鍵字
                    intent.putExtra(SearchManager.QUERY, "kotlin");
                    break;
                }

                // 啟動元件
                startActivity(intent);
            }

        });
    }
}
