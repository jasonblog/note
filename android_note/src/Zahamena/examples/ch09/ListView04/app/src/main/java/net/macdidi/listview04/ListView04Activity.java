package net.macdidi.listview04;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;

public class ListView04Activity extends AppCompatActivity
{

    private TextView info;
    private ListView list_view;

    private List<Fruit> items;
    private FruitAdapter fa;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_list_view04);

        processViews();
        processControllers();

        // 建立ListView元件需要的項目資料，只能放Fruit的List物件
        items = new ArrayList<>();

        // 加入三個Fruit物件
        items.add(new Fruit(1, "Strawberry", "Sweet fleshy red fruit", true));
        items.add(new Fruit(2, "Carrot",
                            "Edible root of the cultivated carrot plant", false));
        items.add(new Fruit(3, "Pumpkin",
                            "Usually large pulpy deep-yellow round fruit", true));

        // 建立自己設計的Adapter物件
        fa = new FruitAdapter(this, R.layout.listview_item04, items);

        // 呼叫setAdapter方法指定使用的Adapter物件
        list_view.setAdapter(fa);
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
        public void onItemClick(AdapterView<?> parent, View view,
                                int position, long id)
        {
            // 取得選擇項目的Fruit物件
            Fruit fruit = fa.getItem(position);
            // 切換選擇狀態
            fruit.setSelected(!fruit.isSelected());
            // 設定切換選擇狀態後的Fruit物件
            fa.set(position, fruit);

            info.setText(fruit.getName() + ": "
                         + (fruit.isSelected() ? "selected" : "unselected"));
        }

    }

}
