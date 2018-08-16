package net.macdidi.spinner01;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;

public class Spinner01Activity extends AppCompatActivity
{

    private TextView info;
    private Spinner spinner01, spinner02;
    private ArrayAdapter<String> aa01, aa02;
    private List<String> listData;

    // 在這個Activity元件中的Spinner元件，而且會註冊選擇監聽事件的數量
    private int spinnerCount = 2;

    // 給Spinner當作選項用的字串陣列
    private static final String[] arrayData = { "ONE", "TWO", "THREE" };

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_spinner01);

        processViews();
        processControllers();

        // 使用Android為Spinner內建的畫面配置資源
        int layoutId01 = android.R.layout.simple_spinner_item;

        // 建立一個選項為String的Adapter物件
        // 第一個參數設定「this」，表示這個Activity元件
        // 第二個參數設定畫面配置資源，決定元件的外觀樣式
        // 第三個參數設定一個字串陣列，提供選項項目資料
        aa01 = new ArrayAdapter<>(this, layoutId01, arrayData);

        // 呼叫setAdapter方法指定使用的Adapter物件
        spinner01.setAdapter(aa01);

        // 建立選項的List物件，並加入需要的項目
        listData = new ArrayList<>();
        listData.add("ALPHA");
        listData.add("BETA");
        listData.add("DELTA");

        // 使用自己設計的畫面配置資源
        int layoutId02 = R.layout.spinner_dropdown;

        // 建立一個選項為String的Adapter物件
        // 第一個參數設定「this」，表示這個Activity元件
        // 第二個參數設定畫面配置資源
        // 第三個參數設定一個List物件，提供選項項目資料
        aa02 = new ArrayAdapter<>(this, layoutId02, listData);

        // 設定選項為自己設計的畫面資源
        aa02.setDropDownViewResource(layoutId02);

        // 呼叫setAdapter方法指定使用的Adapter物件
        spinner02.setAdapter(aa02);
    }

    private void processViews()
    {
        info = (TextView) findViewById(R.id.info);
        spinner01 = (Spinner) findViewById(R.id.spinner01);
        spinner02 = (Spinner) findViewById(R.id.spinner02);
    }

    private void processControllers()
    {
        // 建立選擇監聽物件
        MyItem myItem = new MyItem();
        // 註冊選擇監聽物件
        spinner01.setOnItemSelectedListener(myItem);
        spinner02.setOnItemSelectedListener(myItem);
    }

    // 選擇監聽類別
    private class MyItem implements AdapterView.OnItemSelectedListener
    {

        // 第一個參數是使用者操作的Spinner元件
        // 第二個參數是使用者選擇的項目
        // 第三個參數是使用者選擇的項目編號，第一個是0
        // 第四個參數在這裡沒有用途
        @Override
        public void onItemSelected(AdapterView<?> parent, View view,
                                   int position, long id)
        {

            // 因為剛建立好Spinner元件的時候，Android會設定第一個為預設的項目，
            // 就會呼叫這個方法一次
            if (spinnerCount > 0) {
                spinnerCount--;
                // 第一次呼叫，不是使用者選擇事件，不要執行任何工作
                return;
            }

            String message = "";
            int vid = parent.getId();

            if (vid == R.id.spinner01) {
                message = arrayData[position];
                // 增加一個選項
                listData.add(message);
                // 通知選項資料已經改變
                aa02.notifyDataSetChanged();
            } else if (vid == R.id.spinner02) {
                message = listData.get(position);
            }

            info.setText(message);
        }

        @Override
        public void onNothingSelected(AdapterView<?> parent)
        {
            // 因為特殊情況沒有選擇任何項目，或是選項數量為0
        }

    }

}
