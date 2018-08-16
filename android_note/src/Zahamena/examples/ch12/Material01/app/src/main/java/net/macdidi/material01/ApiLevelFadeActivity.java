package net.macdidi.material01;

import android.os.Build;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.transition.Fade;
import android.transition.Transition;
import android.widget.ImageView;
import android.widget.LinearLayout;

public class ApiLevelFadeActivity extends AppCompatActivity
{

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        // 如果裝置是 LOLLIPOP 或更新的版本
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            // 設定使用轉換效果
            getWindow().requestFeature(
                android.view.Window.FEATURE_CONTENT_TRANSITIONS);
            // 建立 Fade 轉換效果物件
            Transition fade = new Fade();
            // 設定轉換效果持續的時間，單位是毫秒（千分之一秒）
            fade.setDuration(2000);
            // 設定進入與離開的轉換效果
            getWindow().setEnterTransition(fade);
            getWindow().setExitTransition(fade);
        }

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_api_level);

        // 建立與設定顯示圖片的畫面配置元件
        LinearLayout container = (LinearLayout) findViewById(R.id.container);
        LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.MATCH_PARENT,
            LinearLayout.LayoutParams.MATCH_PARENT);
        params.setMargins(6, 6, 6, 6);

        // 加入所有圖形元件
        for (int resId : Material01Activity.IMAGE_ARRAY) {
            ImageView image = new ImageView(this);
            image.setImageResource(resId);
            container.addView(image, params);
        }
    }

}
