package net.macdidi.fragment03;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.database.Cursor;
import android.os.Bundle;
import android.support.v4.app.ListFragment;
import android.support.v4.widget.SimpleCursorAdapter;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.CursorAdapter;
import android.widget.ListView;
import android.widget.Toast;

// 繼承自Support Library的ListFragment類別
public class PlaceFragment extends ListFragment
{

    private static final int[] IDS =
    {R.id.id_listview, R.id.datetime_listview, R.id.note_listview};

    public static final int INSERT_REQUEST_CODE = 0;
    public static final int UPDATE_REQUEST_CODE = 1;
    public static final int SEARCH_REQUEST_CODE = 2;

    private PlaceDAO placeDAO;

    // 儲存使用這個Fragment的Activity元件
    private Context parent;

    // 加入Activity元件
    @Override
    public void onAttach(Context context)
    {
        super.onAttach(context);
        // 設定使用這個Fragment的Activity元件
        parent = context;
    }

    // 建立Fragment畫面
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState)
    {
        // 載入與回傳指定的畫面配置資源
        return inflater.inflate(R.layout.fragment_place, container, false);
    }

    // Activity元件已建立
    @Override
    public void onActivityCreated(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        // 為ListView物件註冊Context Menu
        registerForContextMenu(getListView());
        // 建立資料存取物件
        placeDAO = new PlaceDAO(parent);
        // 處理範例資料
        placeDAO.sampleData(parent);
        // 讀取與顯示資料
        refresh();

        // 設定為可加入ActionBar
        setHasOptionsMenu(true);
    }

    @Override
    public void onCreateOptionsMenu(Menu menu, MenuInflater inflater)
    {
        super.onCreateOptionsMenu(menu, inflater);
        // 呼叫inflate方法載入指定的選單資源，第二個參數是這個方法的Menu物件
        inflater.inflate(R.menu.menu_fragment03, menu);
    }

    // 參數MenuItem是使用者選擇的選單項目物件
    @Override
    public boolean onOptionsItemSelected(MenuItem item)
    {
        // 取得選單項目的資源編號
        int id = item.getItemId();

        switch (id) {
        // 讀取全部資料與顯示
        case R.id.refresh_menu:
            refresh();
            break;

        // 新增
        case R.id.insert_menu:
            Intent intentInsert =
                new Intent(parent, InsertActivity.class);
            // 先把Context轉型為Activity再呼叫startActivityForResult方法
            ((Activity)parent).startActivityForResult(
                intentInsert, INSERT_REQUEST_CODE);
            break;

        // 搜尋
        case R.id.search_menu:
            Intent intentSearch =
                new Intent(parent, SearchActivity.class);
            // 先把Context轉型為Activity再呼叫startActivityForResult方法
            ((Activity)parent).startActivityForResult(
                intentSearch, SEARCH_REQUEST_CODE);
            break;
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onCreateContextMenu(ContextMenu menu, View view,
                                    ContextMenuInfo menuInfo)
    {
        // 取得讀取選項資訊的AdapterContextMenuInfo物件
        AdapterView.AdapterContextMenuInfo info =
            (AdapterView.AdapterContextMenuInfo) menuInfo;
        // 使用AdapterContextMenuInfo物件的編號取得資料
        Place selected = placeDAO.get(info.id);
        // 設定選單標題
        menu.setHeaderTitle(selected.getNote());

        // 取得載入選單用的MenuInflater物件
        // 先把Context轉型為Activity再呼叫getMenuInflater()方法
        MenuInflater menuInflater = ((Activity)parent).getMenuInflater();
        // 呼叫inflate方法載入指定的選單資源，第二個參數是這個方法的Menu物件
        menuInflater.inflate(R.menu.menu_fragment03_context, menu);
    }

    @Override
    public boolean onContextItemSelected(MenuItem item)
    {
        // 取得讀取選項資訊的AdapterContextMenuInfo物件
        AdapterView.AdapterContextMenuInfo info =
            (AdapterView.AdapterContextMenuInfo) item.getMenuInfo();
        // 使用AdapterContextMenuInfo物件的編號取得資料
        final Place selected = placeDAO.get(info.id);

        // 取得選單項目的資源編號
        int id = item.getItemId();

        switch (id) {
        // 修改資料
        case R.id.update_menu:
            Intent intentUpdate =
                new Intent(parent, UpdateActivity.class);
            // 設定資料編號
            intentUpdate.putExtra("id", selected.getId());
            // 先把Context轉型為Activity再呼叫startActivityForResult方法
            ((Activity)parent).startActivityForResult(
                intentUpdate, UPDATE_REQUEST_CODE);
            break;

        // 刪除資料
        case R.id.delete_menu:
            // 建立詢問是否刪除的對話框
            AlertDialog.Builder d = new AlertDialog.Builder(parent);
            // 設定標題、訊息與不可取消
            d.setTitle("DELETE?")
            .setMessage("Delete " + selected.getNote() + "?");

            // 加入按鈕
            d.setPositiveButton(android.R.string.ok,
            new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int which) {
                    // 刪除
                    placeDAO.delete(selected.getId());
                    // 重新讀取與顯示
                    refresh();
                }
            });

            d.setNegativeButton(android.R.string.cancel, null);

            // 顯示對話框
            d.show();
            break;
        }

        return true;
    }

    @Override
    public void onListItemClick(ListView l, View v, int position, long id)
    {
        Place place = placeDAO.get(id);
        // 第一個參數是Context物件
        Toast.makeText(parent, place.getNote(), Toast.LENGTH_SHORT).show();
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode,
                                 Intent data)
    {
        // 如果是搜尋資料
        if (requestCode == SEARCH_REQUEST_CODE) {
            // 取得搜尋日期
            String dateValue = data.getStringExtra("dateValue");
            // 查詢指定的日期
            Cursor cursor = placeDAO.getSearchCursor(dateValue);
            // 建立給ListView元件使用的Adapter物件
            // 第一個參數是Context物件
            // 第二個參數是項目使用的畫面配置資源
            // 第三個參數是資料查詢後的Cursor物件
            // 第四個參數是項目顯示資料的欄位名稱陣列
            // 第五個參數是項目顯示資料的元件資源編號陣列
            // 第六個參數指定為FLAG_REGISTER_CONTENT_OBSERVER, , API Level 11
            SimpleCursorAdapter sca = new SimpleCursorAdapter(
                parent, R.layout.listview_place,
                cursor, PlaceDAO.SHOW_COLUMNS, IDS,
                CursorAdapter.FLAG_REGISTER_CONTENT_OBSERVER);
            // 設定Adapter物件
            setListAdapter(sca);
        } else {
            refresh();
        }
    }

    private void refresh()
    {
        // 查詢全部資料
        Cursor cursor = placeDAO.getAllCursor();
        // 建立給ListView元件使用的Adapter物件
        // 第一個參數是Context物件
        // 第二個參數是項目使用的畫面配置資源
        // 第三個參數是資料查詢後的Cursor物件
        // 第四個參數是項目顯示資料的欄位名稱陣列
        // 第五個參數是項目顯示資料的元件資源編號陣列
        // 第六個參數指定為FLAG_REGISTER_CONTENT_OBSERVER, API Level 11
        SimpleCursorAdapter sca = new SimpleCursorAdapter(
            parent, R.layout.listview_place,
            cursor, PlaceDAO.SHOW_COLUMNS, IDS,
            CursorAdapter.FLAG_REGISTER_CONTENT_OBSERVER);
        // 設定Adapter物件
        setListAdapter(sca);
    }

}