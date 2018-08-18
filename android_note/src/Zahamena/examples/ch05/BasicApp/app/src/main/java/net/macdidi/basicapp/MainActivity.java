package net.macdidi.basicapp;

// 使用Android提供的API
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

// 繼承自AppCompatActivity類別
public class MainActivity extends AppCompatActivity
{

    // 最好加入這個覆寫檢查的標記
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        // 呼叫父類別被覆寫的方法
        super.onCreate(savedInstanceState);
        // 指定元件使用的畫面配置檔
        // 參數是存放在res\layout下的畫面配置檔，
        //     「R.layout.」是固定的，後面接著是畫面配置檔的檔名
        setContentView(R.layout.activity_main);
    }
}
