# Android的幾種多線程方式（AsyncTask,HandlerThread,IntentService,ThreadPool），使用場景以及注意事項


在程序開發的實踐當中，為了讓程序表現得更加流暢，我們肯定會需要使用到多線程來提升程序的併發執行性能。但是編寫多線程併發的代碼一直以來都是一個相對棘手的問題，所以想要獲得更佳的程序性能，我們非常有必要掌握多線程併發編程的基礎技能。

###引入多線程而可能伴隨而來的內存問題

雖然使用多線程可以提高程序的併發量，但是我們需要特別注意因為引入多線程而可能伴隨而來的內存問題。例如：在 Activity 內部定義的一個 AsyncTask，它屬於一個內部類，該類本身和外面的 Activity 是有引用關係的，如果 Activity 要銷燬的時候，AsyncTask 還仍然在運行，這會導致 Activity 沒有辦法完全釋放，從而引發內存洩漏。所以說，多線程是提升程序性能的有效手段之一，但是使用多線程卻需要十分謹慎小心，如果不瞭解背後的執行機制以及使用的注意事項，很可能引起嚴重的問題。

下面我們來一個一個分析下系統為我們提供的幾種多線程方式，包括：AsyncTask,HandlerThread,IntentService,ThreadPool

## 1. AsyncTask

###使用場景：
為 UI 線程與工作線程之間進行快速的切換提供一種簡單便捷的機制。適用於當下立即需要啟動，但是異步執行的生命週期短暫的使用場景。

###基本使用
AsyncTask是一個抽象方法，如果想使用它，需要先創建一個子類來繼承他，還需要為其指定三個泛型參數：


AsyncTask是一個抽象方法，如果想使用它，需要先創建一個子類來繼承他，還需要為其指定三個泛型參數：

- Params
在執行AsyncTask時需要傳入的參數，可用於在後臺任務中使用。
- Progress
後臺任務執行時，如果需要在界面上顯示當前的進度，則使用這裡指定的泛型作為進度單位。
- Result
當任務執行完畢後，如果需要對結果進行返回，則使用這裡指定的泛型作為返回值類型。


### 經常需要去重寫的方法有以下四個：

- onPreExecute()
這個方法會在後臺任務開始執行之間調用，用於進行一些界面上的初始化操作，比如顯示一個進度條對話框等。

- doInBackground(Params...)
這個方法中的所有代碼都會在子線程中運行，我們應該在這裡去處理所有的耗時任務。任務一旦完成就可以通過return語句來將任務的執行結果進行返回，如果AsyncTask的第三個泛型參數指定的是Void，就可以不返回任務執行結果。注意，在這個方法中是不可以進行UI操作的，如果需要更新UI元素，比如說反饋當前任務的執行進度，可以調用publishProgress(Progress...)方法來完成。

- onProgressUpdate(Progress...)
當在後臺任務中調用了publishProgress(Progress...)方法後，這個方法就很快會被調用，方法中攜帶的參數就是在後臺任務中傳遞過來的。在這個方法中可以對UI進行操作，利用參數中的數值就可以對界面元素進行相應的更新。

- onPostExecute(Result)
當後臺任務執行完畢並通過return語句進行返回時，這個方法就很快會被調用。返回的數據會作為參數傳遞到此方法中，可以利用返回的數據來進行一些UI操作，比如說提醒任務執行的結果，以及關閉掉進度條對話框等。
最後在需要的地方調用方法 new MyAsyncTask().execute(); 就可以運行了。


最後在需要的地方調用方法 new MyAsyncTask().execute(); 就可以運行了。

###注意事項：

- 首先，默認情況下，所有的 AsyncTask 任務都是被線性調度執行的，他們處在同一個任務隊列當中，按順序逐個執行。假設你按照順序啟動20個 AsyncTask，一旦其中的某個 AsyncTask 執行時間過長，隊列中的其他剩餘 AsyncTask 都處於阻塞狀態，必須等到該任務執行完畢之後才能夠有機會執行下一個任務。`為瞭解決上面提到的線性隊列等待的問題，我們可以使用 AsyncTask.executeOnExecutor()強制指定 AsyncTask 使用線程池併發調度任務。`

- 其次，如何才能夠真正的取消一個 AsyncTask 的執行呢？我們知道 AsyncTaks 有提供 cancel()的方法，但是這個方法實際上做了什麼事情呢？線程本身並不具備中止正在執行的代碼的能力，為了能夠讓一個線程更早的被銷燬，我們需要在 doInBackground()的代碼中不斷的添加程序是否被中止的判斷邏輯。一旦任務被成功中止，AsyncTask 就不會繼續調用 onPostExecute()，而是通過調用 onCancelled()的回調方法反饋任務執行取消的結果。我們可以根據任務回調到哪個方法（是 onPostExecute 還是 onCancelled）來決定是對 UI 進行正常的更新還是把對應的任務所佔用的內存進行銷燬等。

- 最後，使用 AsyncTask 很容易導致內存洩漏，一旦把 AsyncTask 寫成 Activity 的內部類的形式就很容易因為 AsyncTask 生命週期的不確定而導致 Activity 發生洩漏。


綜上所述，AsyncTask 雖然提供了一種簡單便捷的異步機制，但是我們還是很有必要特別關注到他的缺點，避免出現因為使用錯誤而導致的嚴重系統性能問題。


## 2. HandlerThread

###使用場景：
為某些回調方法或者等待某些任務的執行設置一個專屬的線程，並提供線程任務的調度機制。

大多數情況下，AsyncTask 都能夠滿足多線程併發的場景需要（在工作線程執行任務並返回結果到主線程），但是它並不是萬能的。例如打開相機之後的預覽幀數據是通過 onPreviewFrame()的方法進行回調的，onPreviewFrame()和 open()相機的方法是執行在同一個線程的。如果使用 AsyncTask，會因為 AsyncTask 默認的線性執行的特性(即使換成併發執行)會導致因為無法把任務及時傳遞給工作線程而導致任務在主線程中被延遲，直到工作線程空閒，才可以把任務切換到工作線程中進行執行。所以我們需要的是一個執行在工作線程，同時又能夠處理隊列中的複雜任務的功能，而 HandlerThread 的出現就是為了實現這個功能的，它組合了 Handler，MessageQueue，Looper 實現了一個長時間運行的線程，不斷的從隊列中獲取任務進行執行的功能。

### 基本用法：
HandlerThread 繼承於 Thread,它本質上是一個線程，只不過是 Android 為我們封裝好了 Looper 和 MessageQueue的線程，簡化了操作。使用方法很簡單：

```java
// 創建一個線程，線程名字 : handlerThreadTest
mHandlerThread = new HandlerThread("handlerThreadTest");
mHandlerThread.start();

// Handler 接收消息
final Handler mHandler = new Handler(mHandlerThread.getLooper()) {
    @Override
    public void handleMessage(Message msg) {
        Log.e("Test", "收到 " + msg.obj.toString() + " 在 "
              + Thread.currentThread().getName());
    }
};
mTextView = (TextView) findViewById(R.id.text_view);
// 主線程發出消息
mTextView.setOnClickListener(new View.OnClickListener() {
    @Override
    public void onClick(View v) {
        Message msg = new Message();
        msg.obj = "第一條信息";
        mHandler.sendMessage(msg);
        Log.e("Test", "發出 " + msg.obj.toString() + " 在 "
              + Thread.currentThread().getName());
    }
});
// 子線程發出消息
new Thread(new Runnable() {
    @Override
    public void run() {
        Message msg = new Message();
        msg.obj = "第二條信息";
        mHandler.sendMessage(msg);
        Log.e("Test", "發出 " + msg.obj.toString() + " 在 "
              + Thread.currentThread().getName());
    }
}).start()；
```

最後在不需要的時候記得調用quit();

```java
@Override
protected void onDestroy() {
    super.onDestroy();
    //停止消息循環
    mHandlerThread.quit();
}
```

###注意事項：
HandlerThread 比較合適處理那些在工作線程執行，需要花費時間偏長的任務。我們只需要把任務發送給 HandlerThread，然後就只需要等待任務執行結束的時候通知返回到主線程就好了。
另外很重要的一點是，一旦我們使用了 HandlerThread，需要特別注意給 HandlerThread 設置不同的線程優先級，CPU 會根據設置的不同線程優先級對所有的線程進行調度優化。


##3. IntentSerice

默認的 Service 是執行在主線程的，可是通常情況下，這很容易影響到程序的繪製性能(搶佔了主線程的資源)。除了前面介紹過的 AsyncTask 與 HandlerThread，我們還可以選擇使用 IntentService 來實現異步操作。IntentService 繼承自普通 Service 同時又在內部創建了一個 HandlerThread，在 onHandlerIntent()的回調裡面處理扔到 IntentService 的任務，`在執行完任務後會自動停止。所以 IntentService 就不僅僅具備了異步線程的特性，還同時保留了 Service 不受主頁面生命週期影響，優先級比較高，適合執行高優先級的後臺任務,不容易被殺死的特點。`

###使用場景：
適合於執行由 UI 觸發的後臺 Service 任務，並可以把後臺任務執行的情況通過一定的機制反饋給 UI。

### 基本用法：


```java
public class MyIntentService extends IntentService {

    public static UpdateUI updateUI;
    /**
     * Creates an IntentService.  Invoked by your subclass's constructor.
     *
     * @param name Used to name the worker thread, important only for debugging.
     */
    public MyIntentService(String name) {
        super(name);
    }

    public interface UpdateUI {
        void updateUI(Message message);
    }

    public static void setUpdateUI(UpdateUI updateUIInterface) {
        updateUI = updateUIInterface;
    }

    @Override
    protected void onHandleIntent(@Nullable Intent intent) {
        // 執行耗時操作
        Message msg1 = new Message();
        msg1.obj ="我是耗時操作";
        // 調用回調 （也可以通過廣播機制完成）
        if(updateUI != null) {
            updateUI.updateUI(msg1);
        }
    }
}
```

最後 Activity 通過 Handler 獲取數據並刷新UI。

###注意事項：
使用 IntentService 需要特別留意以下幾點：
首先，因為 IntentService 內置的是 HandlerThread 作為異步線程，所以每一個交給 IntentService 的任務都將以隊列的方式逐個被執行到，一旦隊列中有某個任務執行時間過長，那麼就會導致後續的任務都會被延遲處理。

其次，通常使用到 IntentService 的時候，我們會結合使用 BroadcastReceiver 把工作線程的任務執行結果返回給主 UI 線程。使用廣播容易引起性能問題，我們可以使用 LocalBroadcastManager 來發送只在程序內部傳遞的廣播，從而提升廣播的性能。我們也可以使用 runOnUiThread() 快速回調到主 UI 線程。

最後，包含正在運行的 IntentService 的程序相比起純粹的後臺程序更不容易被系統殺死，該程序的優先級是介於前臺程序與純後臺程序之間的。

## 4. ThreadPool

系統為我們提供了 ThreadPoolExecutor 來實現多線程併發執行任務。

###使用場景：
把任務分解成不同的單元，分發到各個不同的線程上，進行同時併發處理。

###基本用法：
線程池有四個構造方法，這四個構造方法咋一看，前面三個都是調用第四個構造方法實現的，每一個構造方法都特別複雜，參數很多，使用起來比較麻煩。

下面列出是四種構造方法，從簡單到複雜：

- 第一種


```java
ThreadPoolExecutor(
int corePoolSize, 
int maximumPoolSize, 
long keepAliveTime, 
TimeUnit unit, 
BlockingQueue<Runnable> workQueue
);
```

- 第二種

```java
ThreadPoolExecutor(
int corePoolSize, 
int maximumPoolSize, 
long keepAliveTime, 
TimeUnit unit, 
BlockingQueue<Runnable> workQueue, 
RejectedExecutionHandler handler
);
```

- 第三種

```java
ThreadPoolExecutor(
int corePoolSize, 
int maximumPoolSize, 
long keepAliveTime, 
TimeUnit unit, 
BlockingQueue<Runnable> workQueue,
ThreadFactory threadFactory
);
```

- 第四種

```java
ThreadPoolExecutor(
int corePoolSize, 
int maximumPoolSize, 
long keepAliveTime,
TimeUnit unit, 
BlockingQueue<Runnable> workQueue, 
ThreadFactory threadFactory, 
RejectedExecutionHandler handler
);
```

###參數作用：
- corePoolSize:
池中所保存的線程數，包括空閒線程。

- maximumPoolSize:
池中允許的最大線程數。

- keepAliveTime:
當線程數大於核心時，此為終止前多餘的空閒線程等待新任務的最長時間。

- unit:
keepAliveTime參數的時間單位

- workQueue:
執行前用於保持任務的隊列。此隊列僅保持由 execute 方法提交的 Runnable 任務。

- threadFactory
執行程序創建新線程時使用的工廠。

- **handler **
由於超出線程範圍和隊列容量而使執行被阻塞時所使用的處理程序。

這裡用複雜的一個構造方法說明如何手動創建一個線程池。


```java
import java.util.Collection;
import java.util.Iterator;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.Executors;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

/**
 * @auther MaxLiu
 * @time 2017/2/23
 */

public class ThreadPoolTest {
    private static final int CORE_POOL_SIZE = 5;
    private static final int MAX_POOL_SIZE = 10;
    private static final int BLOCK_POOL_SIZE = 2;
    private static final int ALIVE_POOL_SIZE = 2;
    private static ThreadPoolExecutor executor;

    public static void main(String args[]) {
        executor = new ThreadPoolExecutor(
            CORE_POOL_SIZE,// 核心線程數 最小
            MAX_POOL_SIZE,// 最大執行線程數
            ALIVE_POOL_SIZE,// 空閒線程超時
            TimeUnit.SECONDS,// 超時時間單位
            // 當線程池達到corePoolSize時，新提交任務將被放入workQueue中，
            // 等待線程池中任務調度執行
            new ArrayBlockingQueue<Runnable>(BLOCK_POOL_SIZE),// 阻塞隊列大小
            // 線程工廠，為線程池提供創建新線程的功能，它是一個接口，
            // 只有一個方法：Thread newThread(Runnable r)
            Executors.defaultThreadFactory(),
            // 線程池對拒絕任務的處理策略。一般是隊列已滿或者無法成功執行任務，
            // 這時ThreadPoolExecutor會調用handler的rejectedExecution
            // 方法來通知調用者
            new ThreadPoolExecutor.AbortPolicy()
        );
        executor.allowCoreThreadTimeOut(true);
        /*
         * ThreadPoolExecutor默認有四個拒絕策略：
         *
         * 1、ThreadPoolExecutor.AbortPolicy()   直接拋出異常RejectedExecutionException
         * 2、ThreadPoolExecutor.CallerRunsPolicy()    直接調用run方法並且阻塞執行
         * 3、ThreadPoolExecutor.DiscardPolicy()   直接丟棄後來的任務
         * 4、ThreadPoolExecutor.DiscardOldestPolicy()  丟棄在隊列中隊首的任務
         */

        for (int i = 0; i < 10; i++) {
            try {
                executor.execute(new WorkerThread("線程 --> " + i));
                LOG();
            } catch (Exception e) {
                System.out.println("AbortPolicy...");
            }
        }
        executor.shutdown();

        // 所有任務執行完畢後再次打印日誌
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    Thread.sleep(1000);
                    System.out.println("\n\n---------執行完畢---------\n");
                    LOG();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }).start();
    }

    /**
     * 打印 Log 信息
     */
    private static void LOG() {
        System.out.println(" ==============線程池===============\n"
                           + "   線程池中線程數 : " + executor.getPoolSize()
                           + "   等待執行線程數 : " + executor.getQueue().size()
                           + "   所有的任務數 : " + executor.getTaskCount()
                           + "   執行任務的線程數 : " + executor.getActiveCount()
                           + "   執行完畢的任務數 : " + executor.getCompletedTaskCount()

                          );
    }

    // 模擬線程任務
    public static class WorkerThread implements Runnable {
        private String threadName;

        public WorkerThread(String threadName) {
            this.threadName = threadName;
        }

        @Override
        public synchronized void run() {

            int i = 0;
            boolean flag = true;
            try {
                while (flag) {
                    i++;
                    if (i > 2) flag = false;
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        public String getThreadName() {
            return threadName;
        }
    }

}
```

###注意事項：
- 使用線程池需要特別注意同時併發線程數量的控制，理論上來說，我們可以設置任意你想要的併發數量，但是這樣做非常的不好。因為 CPU 只能同時執行固定數量的線程數，一旦同時併發的線程數量超過 CPU 能夠同時執行的閾值，CPU 就需要花費精力來判斷到底哪些線程的優先級比較高，需要在不同的線程之間進行調度切換。一旦同時併發的線程數量達到一定的量級，這個時候 CPU 在不同線程之間進行調度的時間就可能過長，反而導致性能嚴重下降。

- 另外需要關注的一點是，每開一個新的線程，都會耗費至少 64K+ 的內存。為了能夠方便的對線程數量進行控制，ThreadPoolExecutor 為我們提供了初始化的併發線程數量，以及最大的併發數量進行設置。

- 另外需要關注的一個問題是：Runtime.getRuntime().availableProcesser()方法並不可靠，他返回的值並不是真實的 CPU 核心數，因為 CPU 會在某些情況下選擇對部分核心進行睡眠處理，在這種情況下，返回的數量就只能是激活的 CPU 核心數。