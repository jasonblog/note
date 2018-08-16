package net.macdidi.gui01;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.Button;

public class Gui01Activity extends AppCompatActivity
{

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        // 建立一個按鈕物件
        Button button = new Button(this);
        // 設定按鈕的文字
        button.setText("Hello! GUI!\nI am Button!");
        // 設定畫面為按鈕物件
        setContentView(button);
    }
}
