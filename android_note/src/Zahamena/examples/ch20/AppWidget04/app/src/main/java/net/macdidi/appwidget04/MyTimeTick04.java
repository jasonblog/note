package net.macdidi.appwidget04;

import android.appwidget.AppWidgetManager;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;

public class MyTimeTick04 extends BroadcastReceiver
{

    @Override
    public void onReceive(Context context, Intent intent)
    {
        // 建立需要的AppWidgetManager物件，參數為Context物件
        AppWidgetManager appWidgetManager =
            AppWidgetManager.getInstance(context);
        // 建立小工具元件的ComponentName物件
        ComponentName cn = new ComponentName(context, AppWidget04.class);
        // 取得所有安裝在桌面的小工具編號
        int[] ids = appWidgetManager.getAppWidgetIds(cn);
        // 建立小工具物件後呼叫onUpdate方法更新所有小工具畫面
        new AppWidget04().onUpdate(context, appWidgetManager, ids);
    }

}
