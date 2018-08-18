package net.macdidi.listview03;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.SimpleAdapter;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

public class ListView03Activity extends AppCompatActivity
{

    private TextView info;
    private ListView list_view;

    private List<HashMap<String, String>> items;
    private SimpleAdapter sa;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_list_view03);

        processViews();
        processControllers();

        // 建立每一個元素為Map物件的List物件
        items = new ArrayList<>();

        // 建立包裝一個項目資料的Map物件
        HashMap<String, String> record;

        record = new HashMap<>();
        // 設定項目資料的編號，第一個參數「id」為資料名稱
        record.put("id", "1");
        // 設定項目資料的名稱，第一個參數「name」為資料名稱
        record.put("name", "Strawberry");
        // 設定項目資料的說明，第一個參數「content」為資料名稱
        record.put("content", "Sweet fleshy red fruit");
        // 加入一個項目資料
        items.add(record);

        // 第二個項目資料
        record = new HashMap<>();
        record.put("id", "2");
        record.put("name", "Carrot");
        record.put("content",
                   "Edible root of the cultivated carrot plant");
        items.add(record);

        // 第三個項目資料
        record = new HashMap<>();
        record.put("id", "3");
        record.put("name", "Pumpkin");
        record.put("content", "Usually large pulpy deep-yellow round fruit ");
        items.add(record);

        // 建立一個資料名稱的字串陣列
        String[] keys = {"id", "name", "content"};
        // 與資料名稱對應的畫面元件資源編號陣列
        int[] viewIds = {R.id.id_text, R.id.name_text, R.id.content_text};

        // 建立Adapter物件
        // 第一個參數設定「this」，表示這個Activity元件
        // 第二個參數設定一個List物件，提供項目資料
        // 第三個參數設定自己設計的畫面配置資源
        // 第四個參數設定資料名稱的字串陣列
        // 第五個參數設定畫面元件資源編號陣列
        sa = new SimpleAdapter(this, items,
                               R.layout.listview_item, keys, viewIds);

        // 呼叫setAdapter方法指定使用的Adapter物件
        list_view.setAdapter(sa);
    }

    private void processViews()
    {
        info = (TextView) findViewById(R.id.info);
        list_view = (ListView) findViewById(R.id.list_view);
    }

    private void processControllers()
    {
        list_view.setOnItemClickListener(new MyItem());
    }

    // 選擇監聽類別
    private class MyItem implements AdapterView.OnItemClickListener
    {

        @Override
        public void onItemClick(AdapterView<?> parent,
                                View view,
                                int position,
                                long id)
        {
            HashMap<String, String> record = items.get(position);
            info.setText(record.get("name"));
        }

    }

}
