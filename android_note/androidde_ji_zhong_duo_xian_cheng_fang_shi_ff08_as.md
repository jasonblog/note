# Android的几种多线程方式（AsyncTask,HandlerThread,IntentService,ThreadPool），使用场景以及注意事项


在程序开发的实践当中，为了让程序表现得更加流畅，我们肯定会需要使用到多线程来提升程序的并发执行性能。但是编写多线程并发的代码一直以来都是一个相对棘手的问题，所以想要获得更佳的程序性能，我们非常有必要掌握多线程并发编程的基础技能。

###引入多线程而可能伴随而来的内存问题

虽然使用多线程可以提高程序的并发量，但是我们需要特别注意因为引入多线程而可能伴随而来的内存问题。例如：在 Activity 内部定义的一个 AsyncTask，它属于一个内部类，该类本身和外面的 Activity 是有引用关系的，如果 Activity 要销毁的时候，AsyncTask 还仍然在运行，这会导致 Activity 没有办法完全释放，从而引发内存泄漏。所以说，多线程是提升程序性能的有效手段之一，但是使用多线程却需要十分谨慎小心，如果不了解背后的执行机制以及使用的注意事项，很可能引起严重的问题。

下面我们来一个一个分析下系统为我们提供的几种多线程方式，包括：AsyncTask,HandlerThread,IntentService,ThreadPool

## 1. AsyncTask

###使用场景：
为 UI 线程与工作线程之间进行快速的切换提供一种简单便捷的机制。适用于当下立即需要启动，但是异步执行的生命周期短暂的使用场景。

###基本使用
AsyncTask是一个抽象方法，如果想使用它，需要先创建一个子类来继承他，还需要为其指定三个泛型参数：


AsyncTask是一个抽象方法，如果想使用它，需要先创建一个子类来继承他，还需要为其指定三个泛型参数：

- Params
在执行AsyncTask时需要传入的参数，可用于在后台任务中使用。
- Progress
后台任务执行时，如果需要在界面上显示当前的进度，则使用这里指定的泛型作为进度单位。
- Result
当任务执行完毕后，如果需要对结果进行返回，则使用这里指定的泛型作为返回值类型。


### 经常需要去重写的方法有以下四个：

- onPreExecute()
这个方法会在后台任务开始执行之间调用，用于进行一些界面上的初始化操作，比如显示一个进度条对话框等。

- doInBackground(Params...)
这个方法中的所有代码都会在子线程中运行，我们应该在这里去处理所有的耗时任务。任务一旦完成就可以通过return语句来将任务的执行结果进行返回，如果AsyncTask的第三个泛型参数指定的是Void，就可以不返回任务执行结果。注意，在这个方法中是不可以进行UI操作的，如果需要更新UI元素，比如说反馈当前任务的执行进度，可以调用publishProgress(Progress...)方法来完成。

- onProgressUpdate(Progress...)
当在后台任务中调用了publishProgress(Progress...)方法后，这个方法就很快会被调用，方法中携带的参数就是在后台任务中传递过来的。在这个方法中可以对UI进行操作，利用参数中的数值就可以对界面元素进行相应的更新。

- onPostExecute(Result)
当后台任务执行完毕并通过return语句进行返回时，这个方法就很快会被调用。返回的数据会作为参数传递到此方法中，可以利用返回的数据来进行一些UI操作，比如说提醒任务执行的结果，以及关闭掉进度条对话框等。
最后在需要的地方调用方法 new MyAsyncTask().execute(); 就可以运行了。


最后在需要的地方调用方法 new MyAsyncTask().execute(); 就可以运行了。

###注意事项：

- 首先，默认情况下，所有的 AsyncTask 任务都是被线性调度执行的，他们处在同一个任务队列当中，按顺序逐个执行。假设你按照顺序启动20个 AsyncTask，一旦其中的某个 AsyncTask 执行时间过长，队列中的其他剩余 AsyncTask 都处于阻塞状态，必须等到该任务执行完毕之后才能够有机会执行下一个任务。`为了解决上面提到的线性队列等待的问题，我们可以使用 AsyncTask.executeOnExecutor()强制指定 AsyncTask 使用线程池并发调度任务。`

- 其次，如何才能够真正的取消一个 AsyncTask 的执行呢？我们知道 AsyncTaks 有提供 cancel()的方法，但是这个方法实际上做了什么事情呢？线程本身并不具备中止正在执行的代码的能力，为了能够让一个线程更早的被销毁，我们需要在 doInBackground()的代码中不断的添加程序是否被中止的判断逻辑。一旦任务被成功中止，AsyncTask 就不会继续调用 onPostExecute()，而是通过调用 onCancelled()的回调方法反馈任务执行取消的结果。我们可以根据任务回调到哪个方法（是 onPostExecute 还是 onCancelled）来决定是对 UI 进行正常的更新还是把对应的任务所占用的内存进行销毁等。

- 最后，使用 AsyncTask 很容易导致内存泄漏，一旦把 AsyncTask 写成 Activity 的内部类的形式就很容易因为 AsyncTask 生命周期的不确定而导致 Activity 发生泄漏。


综上所述，AsyncTask 虽然提供了一种简单便捷的异步机制，但是我们还是很有必要特别关注到他的缺点，避免出现因为使用错误而导致的严重系统性能问题。


## 2. HandlerThread

###使用场景：
为某些回调方法或者等待某些任务的执行设置一个专属的线程，并提供线程任务的调度机制。

大多数情况下，AsyncTask 都能够满足多线程并发的场景需要（在工作线程执行任务并返回结果到主线程），但是它并不是万能的。例如打开相机之后的预览帧数据是通过 onPreviewFrame()的方法进行回调的，onPreviewFrame()和 open()相机的方法是执行在同一个线程的。如果使用 AsyncTask，会因为 AsyncTask 默认的线性执行的特性(即使换成并发执行)会导致因为无法把任务及时传递给工作线程而导致任务在主线程中被延迟，直到工作线程空闲，才可以把任务切换到工作线程中进行执行。所以我们需要的是一个执行在工作线程，同时又能够处理队列中的复杂任务的功能，而 HandlerThread 的出现就是为了实现这个功能的，它组合了 Handler，MessageQueue，Looper 实现了一个长时间运行的线程，不断的从队列中获取任务进行执行的功能。

### 基本用法：
HandlerThread 继承于 Thread,它本质上是一个线程，只不过是 Android 为我们封装好了 Looper 和 MessageQueue的线程，简化了操作。使用方法很简单：

```java
// 创建一个线程，线程名字 : handlerThreadTest
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
// 主线程发出消息
mTextView.setOnClickListener(new View.OnClickListener() {
    @Override
    public void onClick(View v) {
        Message msg = new Message();
        msg.obj = "第一条信息";
        mHandler.sendMessage(msg);
        Log.e("Test", "发出 " + msg.obj.toString() + " 在 "
              + Thread.currentThread().getName());
    }
});
// 子线程发出消息
new Thread(new Runnable() {
    @Override
    public void run() {
        Message msg = new Message();
        msg.obj = "第二条信息";
        mHandler.sendMessage(msg);
        Log.e("Test", "发出 " + msg.obj.toString() + " 在 "
              + Thread.currentThread().getName());
    }
}).start()；
```

最后在不需要的时候记得调用quit();

```java
@Override
protected void onDestroy() {
    super.onDestroy();
    //停止消息循环
    mHandlerThread.quit();
}
```

###注意事项：
HandlerThread 比较合适处理那些在工作线程执行，需要花费时间偏长的任务。我们只需要把任务发送给 HandlerThread，然后就只需要等待任务执行结束的时候通知返回到主线程就好了。
另外很重要的一点是，一旦我们使用了 HandlerThread，需要特别注意给 HandlerThread 设置不同的线程优先级，CPU 会根据设置的不同线程优先级对所有的线程进行调度优化。


##3. IntentSerice

默认的 Service 是执行在主线程的，可是通常情况下，这很容易影响到程序的绘制性能(抢占了主线程的资源)。除了前面介绍过的 AsyncTask 与 HandlerThread，我们还可以选择使用 IntentService 来实现异步操作。IntentService 继承自普通 Service 同时又在内部创建了一个 HandlerThread，在 onHandlerIntent()的回调里面处理扔到 IntentService 的任务，`在执行完任务后会自动停止。所以 IntentService 就不仅仅具备了异步线程的特性，还同时保留了 Service 不受主页面生命周期影响，优先级比较高，适合执行高优先级的后台任务,不容易被杀死的特点。`

###使用场景：
适合于执行由 UI 触发的后台 Service 任务，并可以把后台任务执行的情况通过一定的机制反馈给 UI。

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
        // 执行耗时操作
        Message msg1 = new Message();
        msg1.obj ="我是耗时操作";
        // 调用回调 （也可以通过广播机制完成）
        if(updateUI != null) {
            updateUI.updateUI(msg1);
        }
    }
}
```

最后 Activity 通过 Handler 获取数据并刷新UI。

###注意事项：
使用 IntentService 需要特别留意以下几点：
首先，因为 IntentService 内置的是 HandlerThread 作为异步线程，所以每一个交给 IntentService 的任务都将以队列的方式逐个被执行到，一旦队列中有某个任务执行时间过长，那么就会导致后续的任务都会被延迟处理。

其次，通常使用到 IntentService 的时候，我们会结合使用 BroadcastReceiver 把工作线程的任务执行结果返回给主 UI 线程。使用广播容易引起性能问题，我们可以使用 LocalBroadcastManager 来发送只在程序内部传递的广播，从而提升广播的性能。我们也可以使用 runOnUiThread() 快速回调到主 UI 线程。

最后，包含正在运行的 IntentService 的程序相比起纯粹的后台程序更不容易被系统杀死，该程序的优先级是介于前台程序与纯后台程序之间的。

## 4. ThreadPool

系统为我们提供了 ThreadPoolExecutor 来实现多线程并发执行任务。

###使用场景：
把任务分解成不同的单元，分发到各个不同的线程上，进行同时并发处理。

###基本用法：
线程池有四个构造方法，这四个构造方法咋一看，前面三个都是调用第四个构造方法实现的，每一个构造方法都特别复杂，参数很多，使用起来比较麻烦。

下面列出是四种构造方法，从简单到复杂：

- 第一种


```java
ThreadPoolExecutor(
int corePoolSize, 
int maximumPoolSize, 
long keepAliveTime, 
TimeUnit unit, 
BlockingQueue<Runnable> workQueue
);
```

- 第二种

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

- 第三种

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

- 第四种

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

###参数作用：
- corePoolSize:
池中所保存的线程数，包括空闲线程。

- maximumPoolSize:
池中允许的最大线程数。

- keepAliveTime:
当线程数大于核心时，此为终止前多余的空闲线程等待新任务的最长时间。

- unit:
keepAliveTime参数的时间单位

- workQueue:
执行前用于保持任务的队列。此队列仅保持由 execute 方法提交的 Runnable 任务。

- threadFactory
执行程序创建新线程时使用的工厂。

- **handler **
由于超出线程范围和队列容量而使执行被阻塞时所使用的处理程序。

这里用复杂的一个构造方法说明如何手动创建一个线程池。


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
            CORE_POOL_SIZE,// 核心线程数 最小
            MAX_POOL_SIZE,// 最大执行线程数
            ALIVE_POOL_SIZE,// 空闲线程超时
            TimeUnit.SECONDS,// 超时时间单位
            // 当线程池达到corePoolSize时，新提交任务将被放入workQueue中，
            // 等待线程池中任务调度执行
            new ArrayBlockingQueue<Runnable>(BLOCK_POOL_SIZE),// 阻塞队列大小
            // 线程工厂，为线程池提供创建新线程的功能，它是一个接口，
            // 只有一个方法：Thread newThread(Runnable r)
            Executors.defaultThreadFactory(),
            // 线程池对拒绝任务的处理策略。一般是队列已满或者无法成功执行任务，
            // 这时ThreadPoolExecutor会调用handler的rejectedExecution
            // 方法来通知调用者
            new ThreadPoolExecutor.AbortPolicy()
        );
        executor.allowCoreThreadTimeOut(true);
        /*
         * ThreadPoolExecutor默认有四个拒绝策略：
         *
         * 1、ThreadPoolExecutor.AbortPolicy()   直接抛出异常RejectedExecutionException
         * 2、ThreadPoolExecutor.CallerRunsPolicy()    直接调用run方法并且阻塞执行
         * 3、ThreadPoolExecutor.DiscardPolicy()   直接丢弃后来的任务
         * 4、ThreadPoolExecutor.DiscardOldestPolicy()  丢弃在队列中队首的任务
         */

        for (int i = 0; i < 10; i++) {
            try {
                executor.execute(new WorkerThread("线程 --> " + i));
                LOG();
            } catch (Exception e) {
                System.out.println("AbortPolicy...");
            }
        }
        executor.shutdown();

        // 所有任务执行完毕后再次打印日志
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    Thread.sleep(1000);
                    System.out.println("\n\n---------执行完毕---------\n");
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
        System.out.println(" ==============线程池===============\n"
                           + "   线程池中线程数 : " + executor.getPoolSize()
                           + "   等待执行线程数 : " + executor.getQueue().size()
                           + "   所有的任务数 : " + executor.getTaskCount()
                           + "   执行任务的线程数 : " + executor.getActiveCount()
                           + "   执行完毕的任务数 : " + executor.getCompletedTaskCount()

                          );
    }

    // 模拟线程任务
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

###注意事项：
- 使用线程池需要特别注意同时并发线程数量的控制，理论上来说，我们可以设置任意你想要的并发数量，但是这样做非常的不好。因为 CPU 只能同时执行固定数量的线程数，一旦同时并发的线程数量超过 CPU 能够同时执行的阈值，CPU 就需要花费精力来判断到底哪些线程的优先级比较高，需要在不同的线程之间进行调度切换。一旦同时并发的线程数量达到一定的量级，这个时候 CPU 在不同线程之间进行调度的时间就可能过长，反而导致性能严重下降。

- 另外需要关注的一点是，每开一个新的线程，都会耗费至少 64K+ 的内存。为了能够方便的对线程数量进行控制，ThreadPoolExecutor 为我们提供了初始化的并发线程数量，以及最大的并发数量进行设置。

- 另外需要关注的一个问题是：Runtime.getRuntime().availableProcesser()方法并不可靠，他返回的值并不是真实的 CPU 核心数，因为 CPU 会在某些情况下选择对部分核心进行睡眠处理，在这种情况下，返回的数量就只能是激活的 CPU 核心数。