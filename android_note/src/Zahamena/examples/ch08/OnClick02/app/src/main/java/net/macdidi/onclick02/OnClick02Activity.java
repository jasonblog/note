package net.macdidi.onclick02;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

public class OnClick02Activity extends AppCompatActivity
{

    // 宣告所有需要的畫面元件物件欄位變數
    private Button button01, button02, button03, button04, button05;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_on_click02);

        // 呼叫自己額外宣告的方法，執行所有取得畫面元件物件的工作
        processViews();
        // 呼叫自己額外宣告的方法，執行所有註冊的工作
        processControllers();
    }

    private void processViews()
    {
        // 在這個方法中，取得畫面元件物件後指定給欄位變數
        button01 = (Button) findViewById(R.id.button01);
        button02 = (Button) findViewById(R.id.button02);
        button03 = (Button) findViewById(R.id.button03);
        button04 = (Button) findViewById(R.id.button04);
        button05 = (Button) findViewById(R.id.button05);
    }

    private void processControllers()
    {
        // 在這個方法中，宣告或建立需要的監聽介面類別物件
        //    並執行所有需要的註冊工作
        button01.setOnClickListener(new MyListener01());

        View.OnClickListener myListener020304 =
        new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                int id = v.getId();

                if (id == R.id.button02) {
                    Toast.makeText(OnClick02Activity.this, "Button02",
                                   Toast.LENGTH_SHORT).show();
                } else if (id == R.id.button03) {
                    Toast.makeText(OnClick02Activity.this, "Button03",
                                   Toast.LENGTH_SHORT).show();
                } else if (id == R.id.button04) {
                    Toast.makeText(OnClick02Activity.this, "Button04",
                                   Toast.LENGTH_SHORT).show();
                }
            }
        };

        button02.setOnClickListener(myListener020304);
        button03.setOnClickListener(myListener020304);
        button04.setOnClickListener(myListener020304);

        button05.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Toast.makeText(OnClick02Activity.this, "Button05",
                               Toast.LENGTH_SHORT).show();
            }
        });
    }

    // 宣告需要的監聽介面類別
    private class MyListener01 implements View.OnClickListener
    {
        @Override
        public void onClick(View v)
        {
            Toast.makeText(OnClick02Activity.this, "Button01",
                           Toast.LENGTH_SHORT).show();
        }

    }

}
