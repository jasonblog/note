package net.macdidi.appwidget01;

import android.appwidget.AppWidgetManager;
import android.appwidget.AppWidgetProvider;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.widget.RemoteViews;

import java.util.Date;

// 繼承自AppWidgetProvider類別
public class AppWidget01 extends AppWidgetProvider
{

    private final static String TAG = "AppWidget01";

    @Override
    public void onEnabled(Context context)
    {
        super.onEnabled(context);
        // 使用者安裝第一個小工具時呼叫這個方法，執行小工具元件初始化的工作
        Log.d(TAG, "onEnabled");
    }

    @Override
    public void onUpdate(Context context,
                         AppWidgetManager appWidgetManager,
                         int[] appWidgetIds)
    {
        // 使用者安裝每一個小工具到桌面或執行更新的時候呼叫這個方法
        // Android 會幫每一個安裝到桌面的小工具編製一個號碼
        // 第三個參數 appWidgetIds 是小工具的編號
        Log.d(TAG, "onUpdate: " + getIds(appWidgetIds));

        // 使用迴圈處理所有小工具編號，使用者安裝小工具的時候只會有一個元素
        for (int i = 0; i < appWidgetIds.length; i++) {
            // 建立 RemoteViews 物件
            RemoteViews views = new RemoteViews(
                context.getPackageName(), R.layout.appwidget_layout);
            // 取得現在的時間
            String now = String.format("%tR", new Date());
            // 設定現在的時間
            views.setTextViewText(R.id.now_txt, now);
            // 更新小工具畫面
            appWidgetManager.updateAppWidget(appWidgetIds[i], views);
        }

        super.onUpdate(context, appWidgetManager, appWidgetIds);
    }

    @Override
    public void onDeleted(Context context, int[] appWidgetIds)
    {
        super.onDeleted(context, appWidgetIds);
        // 使用者刪除小工具的時候呼叫這個方法
        // 第二個參數 appWidgetIds 是被刪除小工具的編號

        Log.d(TAG, "onDeleted: " + getIds(appWidgetIds));
    }

    @Override
    public void onDisabled(Context context)
    {
        super.onDisabled(context);
        // 使用者移除桌面上最後一個小工具時呼叫這個方法，執行清除資源的工作
        Log.d(TAG, "onDisabled");
    }

    @Override
    public void onReceive(Context context, Intent intent)
    {
        super.onReceive(context, intent);
        // 接收到廣播事件的時候呼叫這個方法
        Log.d(TAG, "onReceive");
    }

    private String getIds(int[] appWidgetIds)
    {
        String result = "";

        for (int i = 0; i < appWidgetIds.length; i++) {
            result += appWidgetIds[i] + "-";
        }

        return result;
    }

}
