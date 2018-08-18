package net.macdidi.onclick01;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

public class OnClick01Activity extends AppCompatActivity
{

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_on_click01);

        // 呼叫findViewById取得畫面配置檔中的畫面元件物件
        //    傳回的畫面元件型態是View，所以要轉型為對應的型態
        Button button01 = (Button) findViewById(R.id.button01);
        Button button02 = (Button) findViewById(R.id.button02);
        Button button03 = (Button) findViewById(R.id.button03);
        Button button04 = (Button) findViewById(R.id.button04);
        Button button05 = (Button) findViewById(R.id.button05);

        // 建立監聽物件
        MyListener01 myListener01 = new MyListener01();
        // 註冊給按鈕元件
        button01.setOnClickListener(myListener01);

        // 同時宣告與建立一個需要的監聽物件
        View.OnClickListener myListener020304 =
        new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // 取得畫面元件的id
                int id = v.getId();

                // 判斷是哪一個畫面元件再執行各自的工作
                if (id == R.id.button02) {
                    Toast.makeText(OnClick01Activity.this, "Button02",
                                   Toast.LENGTH_SHORT).show();
                } else if (id == R.id.button03) {
                    Toast.makeText(OnClick01Activity.this, "Button03",
                                   Toast.LENGTH_SHORT).show();
                } else if (id == R.id.button04) {
                    Toast.makeText(OnClick01Activity.this, "Button04",
                                   Toast.LENGTH_SHORT).show();
                }
            }
        };

        // 把一個監聽物件註冊給三個畫面元件
        button02.setOnClickListener(myListener020304);
        button03.setOnClickListener(myListener020304);
        button04.setOnClickListener(myListener020304);

        // 呼叫註冊方法，在參數中同時宣告與建立一個需要的監聽物件
        button05.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Toast.makeText(OnClick01Activity.this, "Button05",
                               Toast.LENGTH_SHORT).show();
            }
        });
    }

    // 一個宣告在Activity裡面的巢狀類別
    private class MyListener01 implements View.OnClickListener
    {
        // 根據介面的規定實作需要的方法
        @Override
        public void onClick(View v)
        {
            // 在方法中填入發生事件的時候要執行的工作
            // 參數的View型態物件是發生事件的畫面元件
            Toast.makeText(OnClick01Activity.this, "Button01",
                           Toast.LENGTH_SHORT).show();
        }

    }
}
