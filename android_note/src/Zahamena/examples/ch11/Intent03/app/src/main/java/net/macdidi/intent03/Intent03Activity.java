package net.macdidi.intent03;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;

public class Intent03Activity extends AppCompatActivity
{

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_intent03);

        // 取得ListView物件
        ListView listview = (ListView) findViewById(R.id.listview);

        // 建立指定陣列資源的ArrayAdapter物件
        ArrayAdapter<CharSequence> adapterActions =
            ArrayAdapter.createFromResource(
                this, R.array.main_menu_array,
                android.R.layout.simple_list_item_1);

        // 設定ArrayAdapter物件給ListView物件
        listview.setAdapter(adapterActions);

        // 項目選擇事件
        listview.setOnItemClickListener(new AdapterView.OnItemClickListener() {

            @Override
            public void onItemClick(AdapterView<?> parent, View view,
                                    int position, long id) {

                // 依照功能表順序建立Activity元件的Class陣列
                Class[] target = {
                    PhonesActivity.class, MmsActivity.class,
                    WebsActivity.class, MapsActivity.class
                };
                // 用使用者選擇項目編號的Activity元件建立Intent物件
                Intent intent = new Intent(
                    Intent03Activity.this, target[position]);
                // 啟動指定的Activity元件
                startActivity(intent);
            }

        });
    }

}
