package net.macdidi.material01;

import android.app.ActivityOptions;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

// 繼承支援套件的 AppCompatActivity 類別
public class Material01Activity extends AppCompatActivity
{

    // 提供給其它元件使用的圖形資源陣列
    public static final int[] IMAGE_ARRAY = {
        R.drawable.android03, R.drawable.android04,
        R.drawable.android05, R.drawable.android08,
        R.drawable.android09, R.drawable.android11,
        R.drawable.android14, R.drawable.android16,
        R.drawable.android19, R.drawable.android21,
        R.drawable.android23, R.drawable.android24,
        R.drawable.android26
    };

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_material01);
    }

    // 啟動元件按鈕
    public void clickButton(View view)
    {
        int id = view.getId();

        switch (id) {
        case R.id.explode_button:
            // 啟動示範 Explode 轉換效果的元件
            startActivityForVersion(
                new Intent(this, ApiLevelExplodeActivity.class));
            break;

        case R.id.slide_button:
            // 啟動示範 Slide 轉換效果的元件
            startActivityForVersion(
                new Intent(this, ApiLevelSlideActivity.class));
            break;

        case R.id.fade_button:
            // 啟動示範 Fade 轉換效果的元件
            startActivityForVersion(
                new Intent(this, ApiLevelFadeActivity.class));
            break;
        }
    }

    public void clickImage(View view)
    {
        int id = view.getId();

        // 建立啟動 ImageViewActivity 元件的 Intent 物件
        Intent intent = new Intent(this, ImageViewActivity.class);
        int resId = -1;

        // 根據選擇的圖形決定顯示的圖形資源
        switch (id) {
        case R.id.image_23:
            resId = R.drawable.android23;
            break;

        case R.id.image_24:
            resId = R.drawable.android24;
            break;

        case R.id.image_26:
            resId = R.drawable.android26;
            break;
        }

        // 設定傳送的圖形資源編號資料
        intent.putExtra("resId", resId);

        // 如果裝置是 LOLLIPOP 或更新的版本
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            // 建立轉換效果
            //   第一個參數為 Context 物件
            //   第二個參數為分享元素物件
            //   第三個參數為分享元素名稱
            ActivityOptions options = ActivityOptions
                                      .makeSceneTransitionAnimation(this, view, "androidimage");
            // 使用轉換效果啟動元件
            startActivity(intent, options.toBundle());
        } else {
            // 使用一般的方式啟動元件
            startActivity(intent);
        }
    }

    private void startActivityForVersion(Intent intent)
    {
        // 如果裝置是 LOLLIPOP 或更新的版本
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            // 啟動元件的時候加入畫面轉換設定
            startActivity(intent,
                          ActivityOptions.makeSceneTransitionAnimation(
                              Material01Activity.this).toBundle());
        } else {
            // 使用一般的方式啟動元件
            startActivity(intent);
        }
    }

}
