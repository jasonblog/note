package net.macdidi.appwidget05;

import android.appwidget.AppWidgetManager;
import android.appwidget.AppWidgetProvider;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.widget.RemoteViews;

import java.util.Date;

public class AppWidget05 extends AppWidgetProvider
{

    @Override
    public void onEnabled(Context context)
    {
        super.onEnabled(context);

        // 啟動服務元件
        startService(context);
    }

    // 覆寫 onUpdate 方法，執行更新畫面的工作
    @Override
    public void onUpdate(Context context,
                         AppWidgetManager appWidgetManager,
                         int[] appWidgetIds)
    {

        for (int i = 0; i < appWidgetIds.length; i++) {
            // 建立 RemoteViews 物件
            RemoteViews views = new RemoteViews(context.getPackageName(),
                                                R.layout.appwidget_layout);
            // 取得現在的時間
            String now = String.format("%tR", new Date());
            // 設定現在的時間
            views.setTextViewText(R.id.now_txt, now);

            // 讀取指定小工具編號的文字顏色設定
            int textColor = ConfigActivity.getTextColorPref(context,
                            appWidgetIds[i]);
            // 設定文字元件的顏色
            views.setTextColor(R.id.now_txt, textColor);
            // 讀取指定小工具編號的背景資源設定
            int backgroundId = ConfigActivity.getBackgroundPref(context,
                               appWidgetIds[i]);
            // 設定指定畫面元件的背景圖形資源
            views.setInt(R.id.container, "setBackgroundResource",
                         backgroundId);

            // 更新畫面
            appWidgetManager.updateAppWidget(appWidgetIds[i], views);
        }

        super.onUpdate(context, appWidgetManager, appWidgetIds);
    }

    @Override
    public void onDisabled(Context context)
    {
        // 停止服務元件
        stopService(context);

        super.onDisabled(context);
    }

    // 啟動服務元件
    private void startService(Context context)
    {
        // 建立服務元件的Intent物件
        Intent intent = new Intent(context, MyService05.class);

        // 如果系統為 Android 8 Oreo, API Level 26 或更新的版本
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            // 啟動在背景工作的服務元件
            context.startForegroundService(intent);
        } else {
            // 啟動在背景工作的服務元件
            context.startService(intent);
        }
    }

    // 停止服務元件
    private void stopService(Context context)
    {
        // 建立服務元件的Intent物件
        Intent intent = new Intent(context, MyService05.class);
        // 停止服務元件
        context.stopService(intent);
    }

}
