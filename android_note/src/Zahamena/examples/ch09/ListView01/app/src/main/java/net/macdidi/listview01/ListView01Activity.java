package net.macdidi.listview01;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.AbsListView;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

public class ListView01Activity extends AppCompatActivity
{

    private TextView info;
    private ListView list_view;
    private ArrayAdapter<String> aa;
    private String[] data = new String[20];

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_list_view01);

        processViews();
        processControllers();

        // 加入20個項目
        for (int i = 0; i < 20; i++) {
            data[i] = "Item " + i;
        }

        // 使用Android為ListView內建的畫面配置資源
        int layoutId = android.R.layout.simple_list_item_1;

        // 建立一個項目為String的Adapter物件
        // 第一個參數設定「this」，表示這個Activity元件
        // 第二個參數設定畫面配置資源
        // 第三個參數設定一個字串陣列，提供項目資料
        aa = new ArrayAdapter<String>(this, layoutId, data);

        // 呼叫setAdapter方法指定使用的Adapter物件
        list_view.setAdapter(aa);
    }

    private void processViews()
    {
        info = (TextView) findViewById(R.id.info);
        list_view = (ListView) findViewById(R.id.list_view);
    }

    private void processControllers()
    {
        // 建立監聽物件，它實作兩種監聽介面
        MyItem myItem = new MyItem();
        // 註冊點擊項目事件
        list_view.setOnItemClickListener(myItem);
        // 註冊長按項目事件
        list_view.setOnItemLongClickListener(myItem);

        // 註冊捲動事件
        list_view.setOnScrollListener(new MyScroll());
    }

    // 點擊與長按監聽類別
    private class MyItem implements AdapterView.OnItemClickListener,
                AdapterView.OnItemLongClickListener
    {

        // 它是長按監聽介面的方法
        // 第一個參數是使用者操作的ListView物件
        // 第二個參數是使用者選擇的項目
        // 第三個參數是使用者選擇的項目編號，第一個是0
        // 第四個參數在這裡沒有用途
        @Override
        public boolean onItemLongClick(AdapterView<?> parent, View view,
                                       int position, long id)
        {
            info.setText("LongClick: " + data[position]);
            // 移動選擇的項目為畫面的第一筆
            parent.setSelection(position);
            return false;
        }

        // 它是點擊監聽介面的方法
        // 第一個參數是使用者操作的ListView物件
        // 第二個參數是使用者選擇的項目
        // 第三個參數是使用者選擇的項目編號，第一個是0
        // 第四個參數在這裡沒有用途
        @Override
        public void onItemClick(AdapterView<?> parent, View view,
                                int position, long id)
        {
            info.setText("Click: " + data[position]);
        }

    }

    // 捲動項目監聽類別
    private class MyScroll implements AbsListView.OnScrollListener
    {

        // 是否已捲動到最後一筆
        private boolean isBottom;

        // 捲動中事件
        // 第一個參數是使用者操作的ListView物件
        // 第二個參數是第一筆項目的編號
        // 第三個參數是畫面中的項目數量
        // 第四個參數是全部項目的數量
        @Override
        public void onScroll(AbsListView view, int firstVisibleItem,
                             int visibleItemCount, int totalItemCount)
        {
            // 判斷是否已捲動到最後一筆
            isBottom = (firstVisibleItem + visibleItemCount) >= totalItemCount;

            info.setText("First: " + firstVisibleItem +
                         " Visible: " + visibleItemCount);
        }

        // 捲動狀態事件
        // 第一個參數是使用者操作的ListView物件
        // 第二個參數是捲動狀態
        @Override
        public void onScrollStateChanged(AbsListView view, int scrollState)
        {
            switch (scrollState) {
            // 自行捲動中
            case AbsListView.OnScrollListener.SCROLL_STATE_FLING:
                break;

            // 結束捲動
            case AbsListView.OnScrollListener.SCROLL_STATE_IDLE:

                // 結束捲動以後再判斷是否已經捲動到最後一筆
                if (isBottom) {
                    Toast.makeText(ListView01Activity.this, "Bottom",
                                   Toast.LENGTH_SHORT).show();
                }

                break;

            // 開始捲動
            case AbsListView.OnScrollListener.SCROLL_STATE_TOUCH_SCROLL:
                break;
            }
        }

    }

}
