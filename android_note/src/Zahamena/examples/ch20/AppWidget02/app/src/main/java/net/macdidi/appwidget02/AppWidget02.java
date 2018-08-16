package net.macdidi.appwidget02;

import android.app.PendingIntent;
import android.appwidget.AppWidgetManager;
import android.appwidget.AppWidgetProvider;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.widget.RemoteViews;
import android.widget.Toast;

import java.util.Date;

public class AppWidget02 extends AppWidgetProvider
{

    // 發送廣播事件的 Action 名稱
    public static final String ACTION_UPDATE =
        "net.macdidi.appwidget02.ACTION.UPDATE";

    @Override
    public void onReceive(Context context, Intent intent)
    {
        // 讀取Intent的Action名稱
        String action = intent.getAction();

        if (action.equals(ACTION_UPDATE)) {
            Toast.makeText(context, "ACTION_UPDATE",
                           Toast.LENGTH_SHORT).show();

            // 如果需要更新小工具的畫面
            // 建立需要的AppWidgetManager物件，參數為Context物件
            AppWidgetManager appWidgetManager = AppWidgetManager
                                                .getInstance(context);
            // 建立小工具元件的ComponentName物件
            ComponentName cn = new ComponentName(context, AppWidget02.class);
            // 取得所有安裝在桌面的小工具編號
            int[] appWidgetIds = appWidgetManager.getAppWidgetIds(cn);
            // 呼叫onUpdate方法更新所有小工具畫面
            onUpdate(context, appWidgetManager, appWidgetIds);
        } else {
            // 執行其它操作
            super.onReceive(context, intent);
        }
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

            // 建立指定Actoin名稱的Intent物件
            Intent intent = new Intent(ACTION_UPDATE);
            // 建立發送廣播事件的PendingIntent物件
            // 第一個參數是Context物件
            // 第二個參數固定為0
            // 第三個參數是建立與設定好的Intent物件
            // 第四個參數固定為FLAG_UPDATE_CURRENT
            PendingIntent pending = PendingIntent.getBroadcast(
                                        context, 0, intent, PendingIntent.FLAG_UPDATE_CURRENT);
            // 設定元件點擊後發送指定的廣播事件
            views.setOnClickPendingIntent(R.id.now_txt, pending);

            // 建立啟動瀏覽器的Intent物件
            Intent intentWeb = new Intent(Intent.ACTION_VIEW);
            // 設定網址
            intentWeb.setData(
                Uri.parse("http://tw.linkedin.com/in/macdidi5/zh-tw"));
            // 建立啟動Activity元件的PendingIntent物件
            // 第一個參數是Context物件
            // 第二和第四個參數固定為0
            // 第三個參數是建立與設定好的Intent物件
            PendingIntent pendingWeb = PendingIntent.getActivity(
                                           context, 0, intentWeb, 0);
            // 設定元件點擊後啟動瀏覽器
            // 第一個參數指定操作的元件資源編號
            // 第二個參數是建立好的PendingIntent物件
            views.setOnClickPendingIntent(R.id.app_name_txt, pendingWeb);

            // 更新畫面
            appWidgetManager.updateAppWidget(appWidgetIds[i], views);
        }

        super.onUpdate(context, appWidgetManager, appWidgetIds);
    }

}
