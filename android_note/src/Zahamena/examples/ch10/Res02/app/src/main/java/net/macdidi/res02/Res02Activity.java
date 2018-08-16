package net.macdidi.res02;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.ContextMenu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView.AdapterContextMenuInfo;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.Toast;

public class Res02Activity extends AppCompatActivity
{

    private ListView list_view;
    private ArrayAdapter<CharSequence> aa;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_res02);

        processViews();

        // 使用Android為ListView內建的畫面配置資源
        int layoutId = android.R.layout.simple_list_item_1;

        // 建立一個項目為陣列資源的Adapter物件
        // 第一個參數設定「this」，表示這個Activity元件
        // 第二個參數設定陣列資源
        // 第三個參數設定畫面配置資源
        aa = ArrayAdapter.createFromResource(
                 this, R.array.listview_array, layoutId);

        // 呼叫setAdapter方法指定使用的Adapter物件
        list_view.setAdapter(aa);
    }

    // 建立選單
    @Override
    public void onCreateContextMenu(ContextMenu menu, View v,
                                    ContextMenu.ContextMenuInfo menuInfo)
    {
        super.onCreateContextMenu(menu, v, menuInfo);
        // 取得載入選單用的MenuInflater物件
        MenuInflater inflater = getMenuInflater();
        // 呼叫inflate方法載入指定的選單資源，第二個參數是這個方法的ContextMenu物件
        inflater.inflate(R.menu.listview_context, menu);
    }

    // 使用者選擇選項
    @Override
    public boolean onContextItemSelected(MenuItem item)
    {
        // 呼叫MenuItem的getMenuInfo取得AdapterContextMenuInfo
        AdapterContextMenuInfo info =
            (AdapterContextMenuInfo) item.getMenuInfo();

        // 呼叫MenuItem的getItemId方法取得選項資源編號，用來判斷使用者選擇哪一個項目
        if (item.getItemId() == R.id.main_menu_view) {
            // AdapterContextMenuInfo物件的position是使用者選擇的項目編號
            CharSequence selected = aa.getItem(info.position);
            Toast.makeText(this, selected, Toast.LENGTH_SHORT).show();
        }

        return super.onContextItemSelected(item);
    }

    private void processViews()
    {
        list_view = (ListView) findViewById(R.id.list_view);
        // 為ListView元件註冊Context Menu
        registerForContextMenu(list_view);
    }

}
