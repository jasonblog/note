package net.macdidi.material02;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.helper.ItemTouchHelper;
import android.view.View;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;

public class Material02Activity extends AppCompatActivity
{

    // 顯示訊息的文字元件
    private TextView info;

    // 列表元件、資料來源物件與列表畫面配置元件
    private RecyclerView item_list;
    private FruitAdapter itemAdapter;
    private RecyclerView.LayoutManager rLayoutManager;

    // 列表元件使用的資料
    private List<Fruit> items;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_material02);

        processViews();

        // 建立RecyclerView元件使用的項目資料
        items = new ArrayList<>();

        // 加入三個Fruit物件
        items.add(new Fruit(1, "Strawberry",
                            "Sweet fleshy red fruit", true));
        items.add(new Fruit(2, "Carrot",
                            "Edible root of the cultivated carrot plant", false));
        items.add(new Fruit(3, "Pumpkin",
                            "Usually large pulpy deep-yellow round fruit", true));

        // 執行RecyclerView元件的設定
        item_list.setHasFixedSize(true);
        // 建立與設定RecyclerView元件的配置元件
        rLayoutManager = new LinearLayoutManager(this);
        item_list.setLayoutManager(rLayoutManager);

        // 建立RecyclerView元件的資料來源物件
        itemAdapter = new FruitAdapter(items, this) {
            @Override
            public void onBindViewHolder(final ViewHolder holder,
                                         final int position) {
                super.onBindViewHolder(holder, position);

                // 建立與註冊項目點擊監聽物件
                holder.rootView.setOnClickListener(
                new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        // 讀取選擇位置的項目物件
                        Fruit fruit = items.get(position);
                        // 顯示項目資訊
                        info.setText(fruit.getName() + ": " +
                                     fruit.isSelected());
                    }
                }
                );
            }
        };

        // 設定RecyclerView使用的資料來源物件
        item_list.setAdapter(itemAdapter);

        // 建立與設定項目操作物件
        ItemTouchHelper.Callback callback =
            new SimpleItemTouchHelperCallback(itemAdapter);
        ItemTouchHelper touchHelper = new ItemTouchHelper(callback);
        touchHelper.attachToRecyclerView(item_list);
    }

    // 新增一個項目
    public void clickAdd(View view)
    {
        // 決定新項目的編號
        int newId = items.size() + 1;
        // 建立新增項目物件
        Fruit fruit = new Fruit(
            newId, "Fruit#" + newId, "Content#" + newId, false);
        // 新增一個項目
        itemAdapter.add(fruit);
        // 控制列表元件移到最後一個項目
        item_list.scrollToPosition(items.size() - 1);
    }

    private void processViews()
    {
        info = (TextView) findViewById(R.id.info);
        item_list = (RecyclerView) findViewById(R.id.item_list);
    }

}
