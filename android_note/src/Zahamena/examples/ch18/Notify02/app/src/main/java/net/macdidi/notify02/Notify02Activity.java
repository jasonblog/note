package net.macdidi.notify02;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.support.v4.app.NotificationCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.RemoteViews;

public class Notify02Activity extends AppCompatActivity
{

    private NotificationManager manager;

    private static final int ACTION_ID = 0;
    private static final int CUSTOM_LAYOUT_ID = 1;

    // 通知頻道分類代碼
    private String channelId = "net.macdidi.notify02.notify.channel.default";

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_notify02);

        // 取得NotificationManager物件
        manager = (NotificationManager)
                  getSystemService(Context.NOTIFICATION_SERVICE);

        // 建立與設定Notify channel
        createChannel(channelId, "Default", "Zahamena default notify channel");
    }

    public void clickActionSend(View view)
    {
        // 建立NotificationCompat.Builder物件
        NotificationCompat.Builder builder =
            new NotificationCompat.Builder(this, channelId);

        // 設定小圖示、時間、標題和訊息
        builder.setSmallIcon(R.drawable.action_web_button)
        .setWhen(System.currentTimeMillis())
        .setContentTitle("Action Notification")
        .setContentText("Demo for notification action");

        // 建立點擊通知需要的PendingIntent物件
        // 第三個參數指定點擊以後要啟動的Activity元件，
        // 使用getIntent()設定為自己，就是Notify02Activity
        PendingIntent forwardIntent =
            PendingIntent.getActivity(this, 0, getIntent(), 0);
        // 設定點擊通知的PendingIntent物件
        builder.setContentIntent(forwardIntent);

        // 建立按鈕功能需要的Intent物件，點擊後開啟網頁
        Intent intentView = new Intent(Intent.ACTION_VIEW);
        // 設定網址
        intentView.setData(Uri.parse("http://www.codedata.com.tw/author/michael"));
        // 建立啟動瀏覽器元件需要的PendingIntent物件
        PendingIntent piWeb =
            PendingIntent.getActivity(this, 0, intentView, 0);
        // 設定開啟網頁的PendingIntent物件
        builder.addAction(R.drawable.action_web_button, "CODEDATA", piWeb);

        // 建立通知物件
        Notification notification = builder.build();
        // 使用ACTION_ID為編號發出通知
        manager.notify(ACTION_ID, notification);
    }

    public void clickCustomLayoutSend(View view)
    {
        // 建立NotificationCompat.Builder物件，
        // 因為需要在巢狀類別中使用，所以加入final關鍵字
        final NotificationCompat.Builder builder =
            new NotificationCompat.Builder(this, channelId);

        // 設定小圖示、時間和不可以清除
        builder.setSmallIcon(R.drawable.ic_timer_white_48dp)
        .setWhen(System.currentTimeMillis())
        .setOngoing(true);

        // 建立包裝畫面配置資源的RemoteViews物件
        RemoteViews remoteView = new RemoteViews(
            getPackageName(), R.layout.my_notification);
        // 設定通知使用自己設計的畫面
        builder.setContent(remoteView);

        // 建立通知物件，因為需要在巢狀類別中使用，所以加入final關鍵字
        final Notification notification = builder.build();

        // 設定RemoteViews物件中的畫面元件內容，開始倒數的時間
        notification.contentView.setTextViewText(
            R.id.start_text, "Start at " + getNow());

        // 建立與啟動倒數的執行緒
        new Thread() {
            @Override
            public void run() {
                // 倒數十六秒
                for (int seconds = 16; seconds > 0; seconds--) {
                    // 設定RemoteViews物件中的畫面元件內容，倒數的時間
                    notification.contentView.setTextViewText(
                        R.id.countdown_text, getTimeStr(seconds));
                    // 送出與更新CUSTOM_LAYOUT_ID編號的通知
                    manager.notify(CUSTOM_LAYOUT_ID, notification);

                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        Log.d("Notify02Activity", e.toString());
                    }
                }

                // 設定RemoteViews物件中的畫面元件內容，結束倒數的時間
                notification.contentView.setTextViewText(
                    R.id.end_text, "End at " + getNow());

                // 設定通知為可以清除
                builder.setOngoing(false);
                // 更新CUSTOM_LAYOUT_ID編號的通知
                manager.notify(CUSTOM_LAYOUT_ID, builder.build());
            }
        } .start();
    }

    // 傳回參數指定秒數的「時：分」格式字串
    private String getTimeStr(int seconds)
    {
        int minute = 0;
        seconds--;

        minute = seconds / 60;
        seconds = seconds - (minute * 60);

        return String.format("%02d", minute) + ":" +
               String.format("%02d", seconds);
    }

    // 傳回目前時間
    private String getNow()
    {
        return String.format("%tT", new java.util.Date());
    }

    // 建立與設定Notify channel
    // 加入裝置版本的判斷，應用程式就不用把最低版本設定為API level 26
    private void createChannel(String id, String name, String desc)
    {
        // 如果系統版本低於 Android 8.0 （API level 26）就不執行設定
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O) {
            return;
        }

        // 建立channel物件，參數依序為channel代碼、名稱與等級
        NotificationChannel nChannel = new NotificationChannel(
            id, name, NotificationManager.IMPORTANCE_DEFAULT);
        // 設定channel的說明
        nChannel.setDescription(desc);
        // 設定channel物件
        manager.createNotificationChannel(nChannel);
    }
}
