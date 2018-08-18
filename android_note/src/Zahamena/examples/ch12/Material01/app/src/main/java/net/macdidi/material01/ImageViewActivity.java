package net.macdidi.material01;

import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.ImageView;

public class ImageViewActivity extends AppCompatActivity
{

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_image_view);

        // 讀取圖形元件
        ImageView image_view = (ImageView) findViewById(R.id.image_view);

        // 讀取圖形資源
        Intent intent = getIntent();
        int resId = intent.getIntExtra("resId", -1);

        if (resId != -1) {
            // 設定圖形資源
            image_view.setImageResource(resId);
        }
    }

    public void clickImage(View view)
    {
        // 如果裝置的版本是LOLLIPOP
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            // 使用轉換效果結束元件
            finishAfterTransition();
        } else {
            // 使用一般的方式結束元件
            finish();
        }
    }
}
