package net.macdidi.appwidget05;

import android.app.Activity;
import android.appwidget.AppWidgetManager;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.TextView;

public class ConfigActivity extends AppCompatActivity
{

    // 五種可以選擇的文字顏色
    private static final String[] TEXT_COLORS =
    {"#AED581", "#FFF176", "#E57373", "#E0E0E0", "#4FC3F7"};

    // 文字顏色的Preference名稱
    public static final String TEXT_COLOR_KEY = "TEXT_COLOR_KEY";
    // 背景資源的Preference名稱
    public static final String BACKGROUND_KEY = "BACKGROUND_KEY";

    private TextView now_text;
    private int colorIndex = 0;
    private int backgroundId = R.drawable.appwidget_drawable;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_config);

        processViews();
    }

    // 選擇文字顏色
    public void clickColor(View view)
    {
        int id = view.getId();

        switch (id) {
        case R.id.color01:
            colorIndex = 0;
            break;

        case R.id.color02:
            colorIndex = 1;
            break;

        case R.id.color03:
            colorIndex = 2;
            break;

        case R.id.color04:
            colorIndex = 3;
            break;

        case R.id.color05:
            colorIndex = 4;
            break;
        }

        // 設定預覽文字顏色
        now_text.setTextColor(Color.parseColor(TEXT_COLORS[colorIndex]));
    }

    // 設定背景
    public void clickStroke(View view)
    {
        int id = view.getId();

        switch (id) {
        case R.id.stroke01:
            backgroundId = R.drawable.appwidget_drawable;
            break;

        case R.id.stroke02:
            backgroundId = R.drawable.appwidget_drawable_stroke;
            break;
        }

        // 設定預覽背景
        now_text.setBackgroundResource(backgroundId);
    }

    // 使用者選擇確定按鈕
    public void clickOK(View view)
    {
        // 儲存小工具編號的變數
        int appWidgetId = 0;
        // 取得由小工具傳送過來的Intent物件
        Intent intent = getIntent();
        // 取得包裝所有資料的Bundle物件
        Bundle extras = intent.getExtras();

        if (extras != null) {
            // 讀取小工具編號
            // 小工具編號資料的名稱和讀取失敗的預設值，
            // 都使用在AppWidgetManager類別中宣告好的變數
            appWidgetId = extras.getInt(
                              AppWidgetManager.EXTRA_APPWIDGET_ID,
                              AppWidgetManager.INVALID_APPWIDGET_ID);
        }

        // 取得SharedPreferences物件
        SharedPreferences pref =
            PreferenceManager.getDefaultSharedPreferences(this);
        // 取得儲存SharedPreferences資料物件
        SharedPreferences.Editor editor = pref.edit();

        // 儲存小工具需要的資料，每個小工具依照編號使用自己的設定值
        // 儲存文字顏色資料，名稱為TEXT_COLOR_KEY接小工具編號
        editor.putString(TEXT_COLOR_KEY + appWidgetId, TEXT_COLORS[colorIndex]);
        // 儲存背景資料，名稱為BACKGROUND_KEY接小工具編號
        editor.putInt(BACKGROUND_KEY + appWidgetId, backgroundId);
        // 寫入資料
        editor.commit();

        // 設定元件要負責執行小工具第一次更新畫面的工作
        // 取得AppWidgetManager物件
        AppWidgetManager appWidgetManager =
            AppWidgetManager.getInstance(ConfigActivity.this);
        // 建立AppWidget05小工具物件並呼叫onUpdate方法更新畫面
        new AppWidget05().onUpdate(ConfigActivity.this,
                                   appWidgetManager, new int[] {appWidgetId});
        // 加入小工具編號資料到Intent物件
        // 小工具編號資料的名稱使用在AppWidgetManager類別中宣告好的變數
        intent.putExtra(AppWidgetManager.EXTRA_APPWIDGET_ID, appWidgetId);
        // 回傳確定結果給小工具，它才會安裝到桌面上
        setResult(Activity.RESULT_OK, intent);
        // 結束
        finish();
    }

    private void processViews()
    {
        now_text = (TextView) findViewById(R.id.config_text);
    }

    // 讀取與傳回指定小工具編號的文字顏色設定
    public static int getTextColorPref(Context context, int appWidgetId)
    {
        // 取得讀取資料用的SharedPreferences物件
        SharedPreferences pref =
            PreferenceManager.getDefaultSharedPreferences(context);
        // 讀取儲存的資料並傳回處理後的顏色
        // 因為每個小工具有自己的設定值，所以資料名稱要接上小工具編號
        String textColorStr = pref.getString(
                                  TEXT_COLOR_KEY + appWidgetId, "#000000");
        return Color.parseColor(textColorStr);
    }

    // 讀取與傳回指定小工具編號的背景設定
    public static int getBackgroundPref(Context context, int appWidgetId)
    {
        // 取得讀取資料用的SharedPreferences物件
        SharedPreferences pref =
            PreferenceManager.getDefaultSharedPreferences(context);
        // 讀取儲存的資料並傳回背景資源
        // 因為每個小工具有自己的設定值，所以資料名稱要接上小工具編號
        return pref.getInt(BACKGROUND_KEY + appWidgetId,
                           R.drawable.appwidget_drawable);
    }

}
