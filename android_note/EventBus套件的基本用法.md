## EventBus 套件的基本用法



**《簡單介紹》**

Eventbus 是一個針對 Android 端優化的事件管理平台，它以事件驅動的方式來簡化事件傳遞的邏輯，一般 Android 開發是以callback、handler、[broadcast](https://xnfood.com.tw/android-broadcastreceiver/)、Listener 等來達到資訊傳遞效果，而透過 EventBus，我們可以將以上所有資料傳遞的方式以簡單的方法達成。

 

**《用法介紹》**

在使用 EventBus 之前，首先我們必須在 build.gradle 中加入依賴庫 –

```
apply plugin: 'com.android.application'
 
android {
    ...
 
    defaultConfig {
        ...
    }
 
    buildTypes {
        release {
        ....   
        }
    }
}
 
dependencies {
     compile 'org.greenrobot:eventbus:3.0.0'
}
```

接著設定需要透過 EventBus 傳遞的參數，我們需宣告一個自訂類別來當作參數傳遞，這邊我們宣告了一個名為 MyEvent 的自訂類別，程式碼如下 –

- **MyEvent.java**

```java
public class MyEvent {

    //此類別用來當作 EventBus 中傳遞的參數物件型別，可在這裡面定義要傳遞的資料。
    private String message;

    public String getMessage() {
        return message;
    }

    public void setMessage(String message) {
        this.message = message;
    }
}
```

第三個步驟，我們在要接收資料的地方(以 broadcast 來說，就是我們註冊識別碼的地方)，註冊 EventBus，程式碼如下 –

- **MainActivity.java**

```java
public class MainActivity extends Activity {
    private EventBus eventBus;
    private TextView textView;
 
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        .
        .
        .
        //註冊 Eventbus
        eventBus = EventBus.getDefault();
        eventBus.register(this);
    }
 
    //Thread mode 有 ASYN, MAIN, POSTING, BACKGROUND 三種選擇，主線程才能修改UI，因此這裡選 MAIN
    //這 onEvent 方法，當有人呼叫 EventBus.getDefault().post(Object event)方法時，就會觸發，並把附帶的資料傳入
    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onEvent(MyEvent event){
        textView.setText(event.getMyEventString());
    }
 
    @Override
    protected void onDestroy() {
        super.onDestroy();
        //一定要記得取消註冊釋放資源
        eventBus.unregister(this);
    }
}
```

我們在 MainActivity 中註冊了 EventBus，並定義好 onEvent(Object event) 方法，如此一來當在同一支程式其他地方呼叫 EventBus.getDefault().post(Object event) 方法時，就會觸發 onEvent(Object event)，並取得所傳遞來的參數。

其中要注意的是 onEvent(Object event) 方法有四種 Thread Mode，分別說明如下 –

**ThreadMode.MAIN :**

表示無論事件是從哪個執行緒 post 出來的，onEvent() 都會在主執行緒執行。

**ThreadMode.POSTING :**

表示事件是從哪個執行緒 post 出來的，onEvent() 就會在哪個執行緒執行。

**ThreadMode.BACKGROUND :**

表示如果事件是從主執行緒 post 出來的，onEvent() 就會創建新的子執行緒執行，如果事件是從子執行緒 post 出來的，onEvent() 就會在該子執行中緒執行。

**ThreadMode.AYSNC :**

表示無論事件是從哪個執行緒 post 出來的，onEvent() 都會創建新的子執行緒執行。

以上就是 EventBus 的基本用法，這種寫法十分方便，在同一隻程式裡幾乎可以取代所有資料之間傳遞的方式。

