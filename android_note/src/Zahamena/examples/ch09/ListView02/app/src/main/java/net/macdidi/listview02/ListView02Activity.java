package net.macdidi.listview02;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import java.util.ArrayList;
import java.util.List;

public class ListView02Activity extends AppCompatActivity
{

    private ListView list_view;
    private ArrayAdapter<String> aa;
    private List<String> data;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_list_view02);

        processViews();

        // 建立一個給ListView使用的項目資料
        data = new ArrayList<>();

        // 加入5個項目
        for (int i = 0; i < 5; i++) {
            data.add("Item " + i);
        }

        // 使用Android為ListView內建的畫面配置資源
        int layoutId = android.R.layout.simple_list_item_1;

        // 建立一個項目為String的Adapter物件
        // 第一個參數設定「this」，表示這個Activity元件
        // 第二個參數設定畫面配置資源
        // 第三個參數設定一個List物件，提供項目資料
        aa = new ArrayAdapter<>(this, layoutId, data);

        // 呼叫setAdapter方法指定使用的Adapter物件
        list_view.setAdapter(aa);
    }

    private void processViews()
    {
        list_view = (ListView) findViewById(R.id.list_view);
    }

    public void addItem(View view)
    {
        // 增加一個項目
        data.add("Item " + data.size());
        // 通知項目資料已經改變
        aa.notifyDataSetChanged();
    }

    public void removeItem(View view)
    {
        if (data.size() > 0) {
            // 移除一個項目
            data.remove(data.size() - 1);
            // 通知項目資料已經改變
            aa.notifyDataSetChanged();
        }
    }

}
