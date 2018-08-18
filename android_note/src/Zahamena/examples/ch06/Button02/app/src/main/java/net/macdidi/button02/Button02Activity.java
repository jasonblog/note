package net.macdidi.button02;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Toast;

public class Button02Activity extends AppCompatActivity
{

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_button02);
    }

    // 兩個按鈕元件都使用這個方法
    //   點擊button01，參數view的id就是button01
    //   點擊button02，參數view的id就是button02
    public void sayHello(View view)
    {
        // 取得元件的id
        int id = view.getId();

        // 判斷是哪一個按鈕
        if (id == R.id.button01) {
            Toast.makeText(this,
                           "Hello! Button01!", Toast.LENGTH_SHORT).show();
        } else if (id == R.id.button02) {
            Toast.makeText(this,
                           "Hello! Button02!", Toast.LENGTH_SHORT).show();
        }
    }

}
