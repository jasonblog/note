package net.macdidi.geofencemap01;

import android.app.IntentService;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.support.v4.app.NotificationCompat;
import android.support.v4.app.TaskStackBuilder;
import android.text.TextUtils;
import android.util.Log;

import com.google.android.gms.location.Geofence;
import com.google.android.gms.location.GeofencingEvent;

import java.util.ArrayList;
import java.util.List;

// 繼承 android.app.IntentService
public class GeofenceIntentService extends IntentService
{

    protected static final String TAG = "GeofenceIntentService";

    public GeofenceIntentService()
    {
        super(TAG);
    }

    @Override
    public void onCreate()
    {
        super.onCreate();
    }

    @Override
    protected void onHandleIntent(Intent intent)
    {
        // 讀取 Geofence 傳送的事件
        GeofencingEvent geofencingEvent = GeofencingEvent.fromIntent(intent);

        // 如果發生錯誤就返回
        if (geofencingEvent.hasError()) {
            Log.e(TAG, GeofenceErrorMessage.getErrorMessage(
                      geofencingEvent.getErrorCode()));
            return;
        }

        // 讀取事件的種類
        int geofenceTransition = geofencingEvent.getGeofenceTransition();

        // 如果是進入或離開
        if (geofenceTransition == Geofence.GEOFENCE_TRANSITION_ENTER ||
            geofenceTransition == Geofence.GEOFENCE_TRANSITION_EXIT) {
            // 讀取所有的事件
            List<Geofence> geofences =
                geofencingEvent.getTriggeringGeofences();
            // 產生事件的訊息
            String details = getDetails(geofenceTransition, geofences);
            // 發出通知
            processNotification(details);
        } else {
            Log.e(TAG, "Invalid type: " + geofenceTransition);
        }
    }

    // 產生事件的訊息
    private String getDetails(int geofenceTransition,
                              List<Geofence> triggeringGeofences)
    {
        String type = "Unknown";

        // 判斷進入或離開
        switch (geofenceTransition) {
        case Geofence.GEOFENCE_TRANSITION_ENTER:
            type = "Entered";
            break;

        case Geofence.GEOFENCE_TRANSITION_EXIT:
            type = "Exited";
            break;
        }

        // 讀取所有請求的 ID
        ArrayList ids = new ArrayList();

        for (Geofence geofence : triggeringGeofences) {
            ids.add(geofence.getRequestId());
        }

        return type + ": " + TextUtils.join(", ", ids);
    }

    // 發出通知
    private void processNotification(String details)
    {
        // 選擇通知啟動的元件
        Intent notificationIntent =
            new Intent(getApplicationContext(), GeofenceMap01Activity.class);
        TaskStackBuilder stackBuilder = TaskStackBuilder.create(this);
        stackBuilder.addParentStack(GeofenceMap01Activity.class);
        stackBuilder.addNextIntent(notificationIntent);
        PendingIntent notificationPendingIntent =
            stackBuilder.getPendingIntent(0,
                                          PendingIntent.FLAG_UPDATE_CURRENT);

        // 建立通知
        NotificationCompat.Builder builder =
            new NotificationCompat.Builder(this);
        builder.setSmallIcon(R.drawable.ic_track_changes_white_48dp)
        .setColor(Color.DKGRAY)
        .setContentTitle(details)
        .setContentText("Click notification to GeofenceMap01")
        .setContentIntent(notificationPendingIntent)
        .setDefaults(Notification.DEFAULT_SOUND)
        .setAutoCancel(true);

        // 發出通知
        NotificationManager manager = (NotificationManager)
                                      getSystemService(Context.NOTIFICATION_SERVICE);
        manager.notify(0, builder.build());
    }

}
