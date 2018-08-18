package net.macdidi.intent02;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.ListView;

public class PlanetActivity extends Activity
{

    private ListView listview;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_planet);

        processViews();
        processControllers();
    }

    private void processViews()
    {
        listview = (ListView) findViewById(R.id.listview);

        ArrayAdapter<CharSequence> adapter =
            ArrayAdapter.createFromResource(
                this, R.array.planet_array,
                android.R.layout.simple_list_item_1);
        listview.setAdapter(adapter);
    }

    private void processControllers()
    {
        // 項目選擇事件
        listview.setOnItemClickListener(new OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view,
                                    int position, long id) {
                // 取得回應結果用的Intent物件
                Intent result = getIntent();
                // 取得使用者選擇的項目內容
                String planet = parent.getItemAtPosition(position).toString();
                // 把項目內容設定為Intent物件的資料
                result.putExtra("planet", planet);
                // 設定執行結果代碼為RESULT_OK，第二個參數是包裝資料的Intent物件
                setResult(Activity.RESULT_OK, result);
                // 完成工作，結束Activity元件
                finish();
            }
        });
    }
}
