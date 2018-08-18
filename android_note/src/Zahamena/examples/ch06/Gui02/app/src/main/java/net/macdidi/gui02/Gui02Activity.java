package net.macdidi.gui02;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

public class Gui02Activity extends AppCompatActivity
{

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        // 設定畫面配置資源
        // 指定的參數在「R.layout.」後面是檔案名稱，不包含附加檔名「.xml」
        setContentView(R.layout.activity_gui02);
    }
}
