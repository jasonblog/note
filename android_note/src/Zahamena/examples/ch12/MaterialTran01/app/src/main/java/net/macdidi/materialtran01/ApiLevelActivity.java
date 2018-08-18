package net.macdidi.materialtran01;

import android.app.Activity;
import android.os.Bundle;
import android.widget.ImageView;
import android.widget.LinearLayout;

public class ApiLevelActivity extends Activity
{

    // 圖形資源陣列
    public static final int[] IMAGE_ARRAY = {
        R.drawable.android03, R.drawable.android04,
        R.drawable.android05, R.drawable.android08,
        R.drawable.android09, R.drawable.android11,
        R.drawable.android14, R.drawable.android16,
        R.drawable.android19, R.drawable.android21,
        R.drawable.android23
    };

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_api_level);

        // 建立與設定顯示圖片的畫面配置元件
        LinearLayout container = (LinearLayout) findViewById(R.id.container);
        LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.MATCH_PARENT,
            LinearLayout.LayoutParams.MATCH_PARENT);
        params.setMargins(6, 6, 6, 6);

        // 加入所有圖形元件
        for (int resId : IMAGE_ARRAY) {
            ImageView image = new ImageView(this);
            image.setImageResource(resId);
            container.addView(image, params);
        }
    }

}
