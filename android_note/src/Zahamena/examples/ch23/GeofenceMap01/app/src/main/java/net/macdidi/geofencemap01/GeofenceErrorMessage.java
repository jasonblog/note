package net.macdidi.geofencemap01;

import com.google.android.gms.location.GeofenceStatusCodes;

import java.util.HashMap;
import java.util.Map;

public class GeofenceErrorMessage
{

    // 儲存錯誤代號與對應的錯誤訊息
    private static Map<Integer, String> errors = new HashMap<>();

    static {
        errors.put(GeofenceStatusCodes.GEOFENCE_NOT_AVAILABLE,
                   "Geofence service is not available.");
        errors.put(GeofenceStatusCodes.GEOFENCE_TOO_MANY_GEOFENCES,
                   "App has registered too many geofences");
        errors.put(GeofenceStatusCodes.GEOFENCE_TOO_MANY_PENDING_INTENTS,
                   "Too many PendingIntents to the geofences");
    }

    private GeofenceErrorMessage() {}

    // 傳回對應的錯誤訊息
    public static String getErrorMessage(int errorCode)
    {
        String result = errors.get(errorCode);

        if (result == null) {
            result = "Unknown";
        }

        return result;
    }

}
