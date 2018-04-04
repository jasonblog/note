# 如何使用IntentService


IntentService提供了Thread跟Handler、Looper機制, 讓你在操作Service更加方便,
而且確保了執行緒的安全。

跟操作Service相同, 必須在AndroidManifest.xml開啟權限。

```sh
<service android:name=".IntentServiceDemo"/>
```

接著宣告一個IntentService的類別。

```java
 public class IntentServiceDemo extends IntentService{
    public IntentServiceDemo(String name){
        super(name);
    }
    
    @Override
    protected void onHandleIntent(Intent intent){
        //這邊是背景執行緒

    }
}
```

這邊用法跟HandlerThread相同, 必須傳入名稱到父類別,
而且只要覆寫onHandleIntent方法, 就可以將長任務寫進這個方法。
如果你想跟Service一樣使用重啟的選項, IntentService提供了兩種方法,

- START_NOT_STICKY
- START_STICKY 預設是後者, 如果你想要調整為前者, 則可以使用以下方法。

```java 
setIntentRedelivery(true);
```

如果在Activity想要使用它, 跟啟動的Service相同。

```java
public class MainActivity extends Activity{
    public void onCreate(Bundle bundle){
        Intent intent = new Intent(this, IntentServiceDemo.class);
        startService(intent);
    }
}
```
你不需要自行停止service, 因為它會自動判斷是否有在使用, 而自我停止。