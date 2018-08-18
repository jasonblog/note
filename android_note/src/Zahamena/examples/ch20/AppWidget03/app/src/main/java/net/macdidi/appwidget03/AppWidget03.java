package net.macdidi.appwidget03;

import android.app.PendingIntent;
import android.appwidget.AppWidgetManager;
import android.appwidget.AppWidgetProvider;
import android.content.Context;
import android.content.Intent;
import android.widget.RemoteViews;
import android.widget.Toast;

import java.util.Date;

public class AppWidget03 extends AppWidgetProvider
{

    // 發送廣播事件的Action名稱
    public static final String ACTION_UPDATE =
        "net.macdidi.appwidget03.ACTION.UPDATE";

    @Override
    public void onReceive(Context context, Intent intent)
    {
        // 讀取Intent的Action名稱
        String action = intent.getAction();

        if (action.equals(ACTION_UPDATE)) {
            // 建立需要的AppWidgetManager物件，參數為Context物件
            AppWidgetManager appWidgetManager = AppWidgetManager
                                                .getInstance(context);

            // 讀取發出廣播事件的小工具元件編號
            // 小工具編號資料的名稱和讀取失敗的預設值，
            // 使用在AppWidgetManager類別中宣告好的變數
            // 包含小工具編號的資料名稱和讀取失敗的預設值
            int id = intent.getIntExtra(
                         AppWidgetManager.EXTRA_APPWIDGET_ID,
                         AppWidgetManager.INVALID_APPWIDGET_ID);

            Toast.makeText(context, "ACTION_UPDATE: " + id,
                           Toast.LENGTH_SHORT).show();

            // 如果成功讀取小工具的編號
            if (id != AppWidgetManager.INVALID_APPWIDGET_ID) {
                // 呼叫onUpdate方法更新指定編號的小工具元件
                onUpdate(context, appWidgetManager, new int[] {id});
            }
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
            // 加入小工具編號資料到Intent物件
            // 小工具編號資料的名稱使用在AppWidgetManager類別中宣告好的變數
            intent.putExtra(AppWidgetManager.EXTRA_APPWIDGET_ID,
                            appWidgetIds[i]);
            // 建立發送廣播事件的PendingIntent物件
            // 第一個參數是Context物件
            // 第二個參數設定為小工具編號
            // 第三個參數是建立與設定好的Intent物件
            // 第四個參數固定為FLAG_UPDATE_CURRENT
            PendingIntent pending = PendingIntent.getBroadcast(context,
                                    appWidgetIds[i], intent,
                                    PendingIntent.FLAG_UPDATE_CURRENT);
            // 設定元件點擊後發送指定的廣播事件
            views.setOnClickPendingIntent(R.id.now_txt, pending);

            // 更新畫面
            appWidgetManager.updateAppWidget(appWidgetIds[i], views);
        }

        super.onUpdate(context, appWidgetManager, appWidgetIds);
    }

}
