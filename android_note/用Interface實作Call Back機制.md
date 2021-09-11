## 用Interface實作Call Back機制



當你有一些程式需要在背景執行時

總是會需要它通知你完成的時候

這時你就可以去規範那個背景程式你需要哪一些狀態通知


例如：

例如你大概知道某個下載程式會有成功與失敗的可能

並且你也需要知道到底是成功還是失敗

那你就可以先定義Interface給該下載程式使用



OnDownLoadCallback.java

```java
public interface OnDownLoadCallback {
    void onComplete(String message);
    void onFail();
}
```

在Interface定義有下載成功與失敗

DownLoadService.java

```java
public class DownLoadService {
    private OnDownLoadCallback mDownLoadcallback;
    public DownLoadService(OnDownLoadCallback callback) {
        this.mDownLoadcallback = callback;
    }
    //模擬一個八秒的下載
    public void startDownLoad() {
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    Thread.sleep(8000);
                    mDownLoadcallback.onComplete("完成囉！");
                } catch (InterruptedException e) {
                    mDownLoadcallback. onFail();
                }
            }
        });
        thread.start();
    }
}
```

在該下載程式一開始註冊這個CallBack來使用

MainActivity.java

```java
public class MainActivity extends AppCompatActivity implements OnDownLoadCallback {
    private static final String TAG = "MainActivity";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        DownLoadService downloadservice = new DownLoadService(this);
        downloadservice.startDownLoad();
    }

    @Override
    public void onComplete(String message) {
        Log.i(TAG, message);
    }

    @Override
    public void onFail() {
        Log.e(TAG, "下載失敗");
    }
}
```

最後在Main裡面去實作這個CallBack得知觸發的時候以及UI該做什麼事情

