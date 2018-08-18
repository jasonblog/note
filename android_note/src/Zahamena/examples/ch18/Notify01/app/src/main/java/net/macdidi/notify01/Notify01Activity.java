package net.macdidi.notify01;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.provider.Settings;
import android.support.v4.app.NotificationCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Switch;

public class Notify01Activity extends AppCompatActivity
{

    private Switch max_priority_switch;
    private NotificationManager manager;

    // 通知頻道分類代碼
    private String channelIdBasic = "net.macdidi.notify01.notify.channel.basic";
    private String channelIdOther = "net.macdidi.notify01.notify.channel.other";

    private Bitmap bigPicture;

    private static int downloadId = 10;
    private static final int BASIC_ID = 0;
    private static final int BIG_PICTURE_ID = 1;
    private static final int BIG_TEXT_ID = 2;
    private static final int INBOX_ID = 3;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_notify01);

        processViews();

        // 建立大型圖片通知需要的Bitmap物件
        bigPicture = BitmapFactory.decodeResource(
                         getResources(), R.drawable.notify_big_picture);
        // 取得NotificationManager物件
        manager = (NotificationManager)
                  getSystemService(Context.NOTIFICATION_SERVICE);

        // 建立與設定Notify channel
        createChannel(channelIdBasic, "Basic", "Zahamena basic notify channel");
        createChannel(channelIdOther, "Other", "Zahamena other notify channel");
    }

    public void clickPref(View view)
    {
        // 如果系統版本低於 Android 8.0 （API level 26）就不執行設定
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O) {
            return;
        }

        Intent intent = new Intent(
            Settings.ACTION_CHANNEL_NOTIFICATION_SETTINGS);

        switch (view.getId()) {
        case R.id.button_basic_pref:
            intent.putExtra(Settings.EXTRA_CHANNEL_ID, channelIdBasic);
            break;

        case R.id.button_other_pref:
            intent.putExtra(Settings.EXTRA_CHANNEL_ID, channelIdOther);
            break;
        }

        intent.putExtra(Settings.EXTRA_APP_PACKAGE, getPackageName());
        startActivity(intent);
    }

    public void clickBasicSend(View view)
    {
        // 建立NotificationCompat.Builder物件
        NotificationCompat.Builder builder =
            new NotificationCompat.Builder(this, channelIdBasic);

        // 設定圖示、時間、標題、訊息和額外資訊
        builder.setSmallIcon(R.drawable.ic_android_white_48dp)
        .setWhen(System.currentTimeMillis())
        .setContentTitle("Basic Notification")
        .setContentText("Demo for basic notification control.")
        .setContentInfo("3");

        // 設定通知圖示與應用程式名稱的顏色
        builder.setColor(Color.BLUE);

        // 設定通知分類目錄
        setCategory(builder, "recommendation");

        // 設定通知的優先順序
        if (max_priority_switch.isChecked()) {
            builder.setPriority(Notification.PRIORITY_MAX);
        }

        // 建立通知物件
        Notification notification = builder.build();
        // 使用BASIC_ID為編號發出通知
        manager.notify(BASIC_ID, notification);
    }

    public void clickBasicCancel(View view)
    {
        // 清除BASIC_ID編號的通知
        manager.cancel(BASIC_ID);
    }

    public void clickProgressSend(View view)
    {
        // 建立NotificationCompat.Builder物件
        final NotificationCompat.Builder builder
            = new NotificationCompat.Builder(this, channelIdOther);

        // 設定圖示、時間、標題、訊息、不可清除與顏色
        builder.setSmallIcon(R.drawable.ic_file_download_white_48dp)
        .setWhen(System.currentTimeMillis())
        .setContentTitle("Download mypicture" + downloadId + ".jpg")
        .setContentText("Download in progress...")
        .setOngoing(true)
        .setColor(Color.BLUE);

        // 建立測試用的執行緒物件
        new Thread() {

            // 設定編號
            private int id = downloadId++;

            @Override
            public void run() {
                int incr;

                for (incr = 0; incr <= 100; incr += 5) {
                    // 設定進度
                    // 參數依序為最大值、目前進度與是否不確定
                    builder.setProgress(100, incr, false);
                    // 使用id為編號發出或更新通知
                    manager.notify(id, builder.build());

                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        Log.d("Notify01Activity", e.toString());
                    }
                }

                // 設定內容訊息為下載完成、進度為0和可以清除
                builder.setContentText("Download complete!")
                .setProgress(0, 0, false)
                .setOngoing(false);
                // 使用id為編號發出通知
                manager.notify(id, builder.build());
            }
        } .start();
    }

    public void clickBigPictureSend(View view)
    {
        // 建立NotificationCompat.Builder物件
        final NotificationCompat.Builder builder
            = new NotificationCompat.Builder(this, channelIdOther);

        // 設定圖示、時間、標題與顏色
        builder.setSmallIcon(R.drawable.ic_insert_photo_white_48dp)
        .setWhen(System.currentTimeMillis())
        .setContentTitle("Big picture notification")
        .setColor(Color.BLUE);

        // 建立大型圖片樣式物件
        NotificationCompat.BigPictureStyle bigPictureStyle =
            new NotificationCompat.BigPictureStyle();

        // 設定圖片與簡介
        bigPictureStyle.bigPicture(bigPicture)
        .setSummaryText("The flowers");

        // 設定樣式為大型圖片
        builder.setStyle(bigPictureStyle);

        // 使用BIG_PICTURE_ID為編號發出通知
        manager.notify(BIG_PICTURE_ID, builder.build());
    }

    public void clickBigTextSend(View view)
    {
        // 建立NotificationCompat.Builder物件
        final NotificationCompat.Builder builder
            = new NotificationCompat.Builder(this, channelIdOther);

        // 設定圖示、時間、標題與顏色
        builder.setSmallIcon(R.drawable.ic_description_white_48dp)
        .setWhen(System.currentTimeMillis())
        .setContentTitle("Big text notification")
        .setColor(Color.BLUE);

        // 建立大型文字樣式物件
        NotificationCompat.BigTextStyle bigTextStyle =
            new NotificationCompat.BigTextStyle();
        // 設定文字與簡介
        bigTextStyle.bigText(getString(R.string.big_text))
        .setSummaryText("About notification");
        // 設定樣式為大型文字
        builder.setStyle(bigTextStyle);
        // 使用BIG_TEXT_ID為編號發出通知
        manager.notify(BIG_TEXT_ID, builder.build());
    }

    public void clickInboxSend(View view)
    {
        // 建立NotificationCompat.Builder物件
        final NotificationCompat.Builder builder
            = new NotificationCompat.Builder(this, channelIdOther);

        // 設定圖示、時間、標題與顏色
        builder.setSmallIcon(R.drawable.ic_view_list_white_48dp)
        .setWhen(System.currentTimeMillis())
        .setContentTitle("Inbox notification")
        .setColor(Color.BLUE);

        // 建立列表樣式物件
        NotificationCompat.InboxStyle inboxStyle =
            new NotificationCompat.InboxStyle();
        // 加入三行列表
        inboxStyle.addLine("You got a message from Ali");
        inboxStyle.addLine("You got a message from Andras");
        inboxStyle.addLine("You got a message from Megan");
        // 設定簡介
        inboxStyle.setSummaryText("Total 3 messages");
        // 設定樣式為列表
        builder.setStyle(inboxStyle);
        // 使用INBOX_ID為編號發出通知
        manager.notify(INBOX_ID, builder.build());
    }

    private void processViews()
    {
        max_priority_switch = (Switch) findViewById(R.id.max_priority_switch);
    }

    // 設定通知分類目錄
    // 加入裝置版本的判斷，應用程式就不用把最低版本設定為API level 21
    private void setCategory(NotificationCompat.Builder builder, String category)
    {
        // setCategory 方法從 Android 5.0（API level 21）開始提供
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            builder.setCategory(category);
        }
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
