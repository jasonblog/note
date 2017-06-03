# Android之Service總結


<div class="show-content">
          <h3>1. 簡介</h3>
<p>與前一篇<a href="http://www.jianshu.com/p/c2c2ee4eb48a" target="_blank">Android之Activity總結</a>是同一系列的文章，是自己在學習和研發過程中，對Service的一些知識點的總結，彙總得到這篇文章。</p>
<p>這篇文章會從Service的一些小知識點，延伸到Android中幾種常用進程間通信方法。</p>
<h3>2. 進程</h3>
<p>Service是一種不提供用戶交互頁面但是可以在後臺長時間運行的組件，可以通過在AndroidManifest.xml設置Service的<code>android:process=":remote"</code>屬性，讓Service運行另一個進程中，也就是說，雖然你是在當前應用啟動的這個Service，但是這個Service和這個應用並不是同一個進程。</p>
<p><strong>四大組件都支持<code>android:process=":remote"</code>這個屬性。</strong></p>
<p>因為Service可以運行在不同的進程，這裡說一下Android中幾種進程的優先級，當系統內存不足時候，系統會從優先級低的進程開始回收，下面根據優先級由高到低列出Android中幾種進程。</p>
<ul>
<li>
<p><strong>前臺進程</strong>，當前用戶操作所需要的進程</p>
<ul>
<li>用戶正在交互的Activity（Activity執行了onResume方法）</li>
<li>與正在交互的Activity綁定的Service</li>
<li>設置為前臺權限的Service（Service調用startForeground()方法）</li>
<li>正在執行某些生命週期回調的Service，onCreate()、onStart()、onDestroy()</li>
<li>
<p>正在執行onReceive()的BroadcastReceiver</p>
<p>這種進程基本不會被回收，只有當內存不足以支持前臺進程同時運行時候，系統才回回收它們，主要關注前三個。</p>
</li>
</ul>
</li>
<li>
<p><strong>可見進程</strong>，沒有與用戶交互所必須的組件，但是在屏幕上仍然可見其內容的進程</p>
<ul>
<li>調用了onPause()方法但仍對用戶可見的Activity</li>
<li>與上面這種Activity綁定的Service</li>
</ul>
</li>
<li>
<p><strong>服務進程</strong>，使用startService()啟動的Service且不屬於上面兩種類別進程的進程，雖然這個進程與用戶交互沒有直接關係，但是一般會在後臺執行一些耗時操作，所以，只有當內存不足以維持所有前臺進程和可見進程同時運行，系統才回回收這個類別的進程。</p>
</li>
<li>
<p><strong>後臺進程</strong>，對用戶不可見的Activity進程，已調用了onStop()方法的Activity</p>
</li>
<li>
<strong>空進程</strong>，不包含任何活動應用組件的進程，保留這種進程唯一目的是作為緩存，縮短引用組件下次啟動時間。通常系統會最優先回收這類進程。</li>
</ul>
<p>此外，一個進程的級別可能會因為其他進程對它的依賴而有所提高，即進程A服務於進程B（B依賴A），那麼A的進程級別至少是和B一樣高的。</p>
<h3>3. Service配置</h3>
<p>和其他組件(Activity/ContentProvider/BroadcastReceiver)一樣，Service需要在Androidmanifest.xml中聲明。</p>
<pre class="hljs xml"><code class="xml"><span class="hljs-tag">&lt;<span class="hljs-name">manifest</span> <span class="hljs-attr">...</span> &gt;</span>
  ...
  <span class="hljs-tag">&lt;<span class="hljs-name">application</span> <span class="hljs-attr">...</span> &gt;</span>
      <span class="hljs-tag">&lt;<span class="hljs-name">service</span> <span class="hljs-attr">android:name</span>=<span class="hljs-string">".DemoService"</span> /&gt;</span>
      ...
  <span class="hljs-tag">&lt;/<span class="hljs-name">application</span>&gt;</span>
<span class="hljs-tag">&lt;/<span class="hljs-name">manifest</span>&gt;</span></code></pre>
<p>Service是運行在主線程中的，如果有什麼耗時的操作，建議新建子線程去處理，避免阻塞主線程，降低ANR的風險。</p>
<p>在另外一篇文章中<a href="http://www.jianshu.com/p/a7535c3f33df" target="_blank">Intent以及IntentFilter詳解</a>提到過，為了確保應用的安全，不要為Service設置intent-filter，因為如果使用隱式Intent去啟動Service時候，手機裡面那麼多應用，並不能確定哪一個Service響應了這個Intent，所以在項目中儘量使用顯式Intent去啟動Service。在Android 5.0(API LEVEL 21)版本後的，如果傳入隱式Intent去調用<code>bindService()</code>方法，系統會拋出異常。</p>
<p>可以通過設置<code>android:exported=false</code>來確保這個Service僅能在本應用中使用。</p>
<h3>4. 服務啟動方式</h3>
<p>服務可以由其他組件啟動，而且如果用戶切換到其他應用，這個服務可能會繼續在後臺執行。到目前為止，Android中Service總共有三種啟動方式。</p>
<ul>
<li>Scheduled，可定時執行的Service，是Android 5.0（API LEVEL 21）版本中新添加的一個Service，名為JobService，繼承Service類，使用JobScheduler類調度它並且設置JobService運行的一些配置。具體文檔可以參考<a href="https://developer.android.com/reference/android/app/job/JobScheduler.html" target="_blank">JobScheduler</a>，如果你的應用最低支持版本是21，官方建議使用JobService。</li>
<li>Started，通過<code>startService()</code>啟動的Service。通過這種方式啟動的Service會獨立的運行在後臺，即使啟動它的組件已經銷燬了。例如Activity A使用startService()啟動了Service B，過了會兒，Activity A執行onDestroy()被銷燬了，如果Service B任務沒有執行完畢，它仍然會在後臺執行。這種啟動方式啟動的Service需要主動調用<code>StopService()</code>停止服務。</li>
<li>Bound，通過<code>bindService()</code>啟動的Service。通過這種方式啟動Service時候，會返回一個客戶端交互接口，用戶可以通過這個接口與服務進行交互，如果這個服務是在另一個進程中，那麼就實現了進程間通信，也就是Messenger和AIDL，這個會是下篇文章的重點。多個組件可以同時綁定同一個Service，如果所有的組件都調用<code>unbindService()</code>解綁後，Service會被銷燬。</li>
</ul>
<p><strong>startService和bindService可以同時使用</strong></p>
<h3>5. 主要方法</h3>
<p>Service是一個抽象類，使用需要我們去實現它的抽象方法<code>onBind()</code>，Service有且僅有這一個抽象方法，還有一些其他的生命週期回調方法需要複寫幫助我們實現具體的功能。</p>
<ul>
<li>onCreate()，在創建服務時候，可以在這個方法中執行一些的初始化操作，它在<code>onStartCommand()</code>和<code>onBind()</code>之前被調用。如果服務已經存在，調用<code>startService()</code>啟動服務時候這個方法不會調用，只會調用<code>onStartCommand()</code>方法。</li>
<li>onStartCommand()，其他組件通過<code>startService()</code>啟動服務時候會回調這個方法，這個方法執行後，服務會啟動被在後臺運行，需要調用<code>stopSelf()</code>或者<code>stopService()</code>停止服務。</li>
<li>onBind()，其他組件通過<code>bindService()</code>綁定服務時候會回調的方法，這是Service的一個抽象方法，如果客戶端需要與服務交互，需要在這個方法中返回一個<code>IBinder</code>實現類實例化對象，如果不想其他客戶端與服務綁定，直接返回null。</li>
<li>onDestroy()，當服務不在還是用且即將被銷燬時，會回調這個方法，可以在這個方法中做一些釋放資源操作，這是服務生命週期的最後一個回調。</li>
</ul>
<p>如果組件僅通過<code>startService()</code>啟動服務，不論服務是否已經啟動，都會回調<code>onStartCommand()</code>方法，而且服務會一直運行，需要調用<code>stopSelf</code>和<code>stopService</code>方法關閉服務。</p>
<p>如果組件僅通過<code>bindService()</code>綁定服務，則服務只有在與組件綁定時候運行，一旦所有的客戶端全部取消綁定<code>unbindService</code>，系統才會銷燬該服務。</p>
<p>多次啟動同一個服務，只有在服務初次啟動時候會回調<code>onCreate</code>方法，但是每次都會回調<code>onStartCommand</code>，可以利用這個向服務傳遞一些信息。</p>
<p><strong>onStartCommand()的回調是在UI主線程，如果有什麼耗時的操作，建議新啟線程去處理。</strong></p>
<h3>6. 啟動和關閉服務</h3>
<p>啟動服務：</p>
<ul>
<li><code>JobScheduler.schedule()</code></li>
<li><code>startService(Intent)</code></li>
<li><code>bindService(Intent service, ServiceConnection conn, int flags)</code></li>
</ul>
<p>關閉服務：</p>
<ul>
<li>
<code>JobScheduler.cancel()</code>或者<code>JobScheduler.cancelAll()</code>，對應<code>JobScheduler.schedule()</code>
</li>
<li>Service自身的<code>stopSelf()</code>方法，組件的<code>stopService(Intent)</code>方法，對應<code>startService</code>啟動方法</li>
<li>
<code>unbindService(ServiceConnection conn)</code>，對應<code>bindService</code>
</li>
</ul>
<p>示例：</p>
<pre class="hljs pony"><code class="pony"><span class="hljs-comment">// 啟動服務</span>
<span class="hljs-type">Intent</span> intent = <span class="hljs-function"><span class="hljs-keyword">new</span> <span class="hljs-title">Intent</span>(this, <span class="hljs-type">DemoService</span>.class);
<span class="hljs-title">startService</span>(intent);

<span class="hljs-comment">// 停止服務</span>
<span class="hljs-title">stopService</span>(intent)

<span class="hljs-comment">// 綁定服務</span>
<span class="hljs-title">ServiceConnection</span> <span class="hljs-title">mConnection</span> = <span class="hljs-title">ServiceConnection</span>() { ... };
<span class="hljs-title">Intent</span> <span class="hljs-title">intent</span> = <span class="hljs-title">new</span> <span class="hljs-title">Intent</span>(this, <span class="hljs-type">DemoService</span>.class);
<span class="hljs-title">bindService</span>(intent, mConnection, <span class="hljs-type">Context</span>.<span class="hljs-type">BIND_AUTO_CREATE</span>);

<span class="hljs-comment">// 解除綁定</span>
<span class="hljs-title">unbindService</span>(mConnection);</span></code></pre>
<p>綁定服務<code>bindService()</code>第三個參數數值：</p>
<ul>
<li>0，如果不想設置任何值，就設置成0</li>
<li>
<code>Context.BIND_AUTO_CREATE</code>，綁定服務時候，如果服務尚未創建，服務會自動創建，在API LEVEL 14以前的版本不支持這個標誌，使用<code>Context.BIND_WAIVE_PRIORITY</code>可以達到同樣效果</li>
<li>
<code>Context.BIND_DEBUG_UNBIND</code>，通常用於Debug，在<code>unbindService</code>時候，會將服務信息保存並打印出來，這個標記很容易造成內存洩漏。</li>
<li>
<code>Context.BIND_NOT_FOREGROUND</code>，不會將被綁定的服務提升到前臺優先級，但是這個服務也至少會和客戶端在內存中優先級是相同的。</li>
<li>
<code>Context.BIND_ABOVE_CLIENT</code>，設置服務的進程優先級高於客戶端的優先級，只有當需要服務晚於客戶端被銷燬這種情況才這樣設置。</li>
<li>
<code>Context.BIND_ALLOW_OOM_MANAGEMENT</code>，保持服務受默認的服務管理器管理，當內存不足時候，會銷燬服務</li>
<li>
<code>Context.BIND_WAIVE_PRIORITY</code>，不會影響服務的進程優先級，像通用的應用進程一樣將服務放在一個LRU表中</li>
<li>
<code>Context.BIND_IMPORTANT</code>，標識服務對客戶端是非常重要的，會將服務提升至前臺進程優先級，通常情況下，即時客戶端是前臺優先級，服務最多也只能被提升至可見進程優先級，</li>
<li>
<code>BIND_ADJUST_WITH_ACTIVITY</code>，如果客戶端是Activity，服務優先級的提高取決於Activity的進程優先級，使用這個標識後，會無視其他標識。</li>
</ul>
<h3>7. onStartCommand()返回值</h3>
<p><code>onStartCommand()</code>方法有一個int的返回值，這個返回值標識服務關閉後系統的後續操作。</p>
<p>返回值有以下幾種：</p>
<ul>
<li>
<code>Service.START_STICKY</code>，啟動後的服務被殺死，系統會自動重建服務並調用<code>on onStartCommand()</code>，但是不會傳入最後一個Intent(Service可能多次執行onStartCommand)，會傳入一個空的Intent，使用這個標記要注意對Intent的判空處理。這個標記適用於太依靠外界數據Intent，在特定的時間，有明確的啟動和關閉的服務，例如後臺運行的音樂播放。</li>
<li>
<code>Service.START_NOT_STICKY</code>，啟動後的服務被殺死，系統不會自動重新創建服務。這個標記是最安全的，適用於依賴外界數據Intent的服務，需要完全執行的服務。</li>
<li>
<code>Service.START_REDELIVER_INTENT</code>，啟動後的服務被殺死，系統會重新創建服務並調用<code>onStartCommand()</code>，同時會傳入最後一個Intent。這個標記適用於可恢復繼續執行的任務，比如說下載文件。</li>
<li>
<code>Service.START_STICKY_COMPATIBILITY</code>，啟動後的服務被殺死，不能保證系統一定會重新創建Service。</li>
</ul>
<h3>8. Service生命週期</h3>
<p>Service生命週期（從創建到銷燬）跟它被啟動的方式有關係，這裡只介紹<code>startService</code>和<code>bindService</code>兩種啟動方法時候Service的生命週期。</p>
<ul>
<li>
<code>startService</code>啟動方式，其他組件用這種方式啟動服務，服務會在後臺一直運行，只有服務調用本身的<code>stopSelf()</code>方法或者其他組件調用<code>stopService()</code>才能停止服務。</li>
<li>
<code>bindService</code>啟動方式，其他組件用這種方法綁定服務，服務通過IBinder與客戶端通信，客戶端通過<code>unbindService</code>接觸對服務的綁定，當沒有客戶端綁定到服務，服務會被系統銷燬。</li>
</ul>
<p>這兩種生命週期不是獨立的，組件可以同時用<code>startService</code>啟動服務同時用<code>bindService</code>綁定服務，例如跨頁面的音樂播放器，就可以在多個頁面同時綁定同一個服務，這種情況下需要調用<code>stopService()</code>或者服務本身的<code>stopSelf()</code>並且沒有客戶端綁定到服務，服務才會被銷燬。</p>
<div class="image-package">
<img src="images/354846-1d3012753d0e93c5.png" data-original-src="images/354846-1d3012753d0e93c5.png" style="cursor: zoom-in;"><br><div class="image-caption">Service生命週期圖</div>
</div>
<p>左圖是使用<code>startService()</code>所創建的服務的生命週期，右圖是使用<code>bindService()</code>所創建的服務的生命週期。</p>
<h3>9. 在前臺運行服務</h3>
<p>服務可以通過startForeground來使服務變成前臺優先級。</p>
<pre class="hljs aspectj"><code class="aspectj"><span class="hljs-keyword">public</span> <span class="hljs-keyword">final</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">startForeground</span><span class="hljs-params">(<span class="hljs-keyword">int</span> id, Notification notification)</span> </span>{
    <span class="hljs-keyword">try</span> {
        mActivityManager.setServiceForeground(
                <span class="hljs-keyword">new</span> ComponentName(<span class="hljs-keyword">this</span>, mClassName), mToken, id,
                notification, <span class="hljs-keyword">true</span>);
    } <span class="hljs-keyword">catch</span> (RemoteException ex) {
    }
}</code></pre>
<p>第一個參數用於標識你<strong>應用中唯一的通知標識id</strong>，不能設為0，最終會傳入<code>NotificationManager.notify(int id, Notification notification)</code>取消通知需要用到，第二個參數是通知具體內容。</p>
<p>前臺服務需要在狀態欄中添加通知，例如，將音樂播放器的服務設置為前臺服務，狀態欄通知顯示正在播放的歌曲，並允許其他組件與其交互。</p>
<pre class="hljs cpp"><code class="cpp"><span class="hljs-comment">// 設置Notification屬性</span>
Notification notification = <span class="hljs-keyword">new</span> Notification(R.drawable.icon, getText(R.<span class="hljs-built_in">string</span>.ticker_text),System.currentTimeMillis());
Intent notificationIntent = <span class="hljs-keyword">new</span> Intent(<span class="hljs-keyword">this</span>, ExampleActivity.<span class="hljs-keyword">class</span>);
PendingIntent pendingIntent = PendingIntent.getActivity(<span class="hljs-keyword">this</span>, <span class="hljs-number">0</span>, notificationIntent, <span class="hljs-number">0</span>);
notification.setLatestEventInfo(<span class="hljs-keyword">this</span>, getText(R.<span class="hljs-built_in">string</span>.notification_title), getText(R.<span class="hljs-built_in">string</span>.notification_message), pendingIntent);
startForeground(ONGOING_NOTIFICATION_ID, notification);</code></pre>
<p>要將服務從前臺移除，需要調用stopForeground(boolean removeNotification)，參數是一個布爾值，用來標識服務從前臺服務移除時候，是否需要移除狀態欄的通知。如果服務在前臺運行時候被停止，也會移除狀態欄的通知。</p>
<h3>10. 與服務通信</h3>
<h4>10.1 廣播</h4>
<p>不多說，萬能的通信。</p>
<h4>10.2 內存共享</h4>
<p>不多說，萬能的通信</p>
<h4>10.3 本地數據共享</h4>
<p>不多說，萬能的通信，例如ContentProvider/SharePreference等等。</p>
<h4>10.4 startService()</h4>
<p>使用這個方法啟動的服務，再次調用<code>startService()</code>傳入Intent即可與服務通信，因為這種方式啟動的服務在完整的生命週期內<code>onCreate()</code>只會執行一次，而<code>onStartCommand()</code>會執行多次，我們再次調用<code>startService()</code>時候，可以在<code>oonStartCommand()</code>去處理。</p>
<h4>10.5 bindService()</h4>
<p>使用這種方法啟動的服務，組件有三種與服務通信的方式。 </p>
<ul>
<li>Service中實現IBinder</li>
<li>Messenger(AIDL的簡化版)</li>
<li>AIDL</li>
</ul>
<p>下一篇文章具體介紹Messenger、AIDL，因為它們是屬於Android進程間通信。</p>
<p>如果一個服務Service只需要在本應用的進程中使用，不提供給其他進程，推薦使用第一種方法。</p>
<p>使用示例：</p>
<p>Service：</p>
<pre class="hljs scala"><code class="scala"><span class="hljs-comment">/**
 * 本地服務
 * &lt;br/&gt;
 * 和啟動應用屬於同一進程
 */</span>
public <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">LocalService</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">Service</span> </span>{
    <span class="hljs-comment">/**
     * 自定的IBinder
     */</span>
    <span class="hljs-keyword">private</span> <span class="hljs-keyword">final</span> <span class="hljs-type">IBinder</span> mBinder = <span class="hljs-keyword">new</span> <span class="hljs-type">LocalBinder</span>();

    <span class="hljs-meta">@Override</span>
    public <span class="hljs-type">IBinder</span> onBind(<span class="hljs-type">Intent</span> intent) {
        <span class="hljs-keyword">return</span> mBinder;
    }

    <span class="hljs-comment">/**
     * 提供給客戶端的方法
     *
     * @return
     */</span>
    public <span class="hljs-type">String</span> getServiceInfo() {
        <span class="hljs-keyword">return</span> <span class="hljs-keyword">this</span>.getPackageName() + <span class="hljs-string">" "</span> + <span class="hljs-keyword">this</span>.getClass().getSimpleName();
    }

    <span class="hljs-comment">/**
     * 自定義的IBinder
     */</span>
    public <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">LocalBinder</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">Binder</span> </span>{
        public <span class="hljs-type">LocalService</span> getService() {
            <span class="hljs-keyword">return</span> <span class="hljs-type">LocalService</span>.<span class="hljs-keyword">this</span>;
        }
    }
}</code></pre>
<p>Activity：</p>
<pre class="hljs java"><code class="java"><span class="hljs-comment">/**
 * 綁定本地服務的組件
 *
 * Created by KyoWang.
 */</span>
<span class="hljs-keyword">public</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">BindLocalServiceActivity</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">AppCompatActivity</span> <span class="hljs-keyword">implements</span> <span class="hljs-title">View</span>.<span class="hljs-title">OnClickListener</span> </span>{

    <span class="hljs-keyword">private</span> Button mShowServiceNameBtn;

    <span class="hljs-keyword">private</span> LocalService mService;

    <span class="hljs-keyword">private</span> <span class="hljs-keyword">boolean</span> mBound = <span class="hljs-keyword">false</span>;

    <span class="hljs-keyword">public</span> ServiceConnection mConnection = <span class="hljs-keyword">new</span> ServiceConnection() {
        <span class="hljs-meta">@Override</span>
        <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">onServiceConnected</span><span class="hljs-params">(ComponentName name, IBinder service)</span> </span>{
            LocalService.LocalBinder binder = (LocalService.LocalBinder) service;
            mService = binder.getService();
            mBound = <span class="hljs-keyword">true</span>;
        }

        <span class="hljs-meta">@Override</span>
        <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">onServiceDisconnected</span><span class="hljs-params">(ComponentName name)</span> </span>{
            mBound = <span class="hljs-keyword">false</span>;
        }
    };

    <span class="hljs-meta">@Override</span>
    <span class="hljs-function"><span class="hljs-keyword">protected</span> <span class="hljs-keyword">void</span> <span class="hljs-title">onCreate</span><span class="hljs-params">(@Nullable Bundle savedInstanceState)</span> </span>{
        <span class="hljs-keyword">super</span>.onCreate(savedInstanceState);
        setContentView(R.layout.a_bind_local_service);
        mShowServiceNameBtn = (Button) findViewById(R.id.bind_local_service_btn);
        mShowServiceNameBtn.setOnClickListener(<span class="hljs-keyword">this</span>);
    }

    <span class="hljs-meta">@Override</span>
    <span class="hljs-function"><span class="hljs-keyword">protected</span> <span class="hljs-keyword">void</span> <span class="hljs-title">onStart</span><span class="hljs-params">()</span> </span>{
        <span class="hljs-keyword">super</span>.onStart();
        Intent intent = <span class="hljs-keyword">new</span> Intent(<span class="hljs-keyword">this</span>, LocalService.class);
        bindService(intent, mConnection, Context.BIND_AUTO_CREATE);
    }

    <span class="hljs-meta">@Override</span>
    <span class="hljs-function"><span class="hljs-keyword">protected</span> <span class="hljs-keyword">void</span> <span class="hljs-title">onStop</span><span class="hljs-params">()</span> </span>{
        <span class="hljs-keyword">super</span>.onStop();
        <span class="hljs-keyword">if</span>(mBound) {
            unbindService(mConnection);
            mBound = <span class="hljs-keyword">false</span>;
        }
    }

    <span class="hljs-meta">@Override</span>
    <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">onClick</span><span class="hljs-params">(View v)</span> </span>{
        <span class="hljs-keyword">int</span> id = v.getId();
        <span class="hljs-keyword">if</span>(id == R.id.bind_local_service_btn) {
            <span class="hljs-keyword">if</span>(mBound) {
                String info = mService.getServiceInfo();
                Toast.makeText(BindLocalServiceActivity.<span class="hljs-keyword">this</span>, info, Toast.LENGTH_SHORT).show();
            }
        }
    }
}</code></pre>
<h3>11. 服務長存後臺</h3>
<p>關於<strong>網上通用</strong>的提升服務優先級以保證服務長存後臺，即保證服務不輕易被系統殺死的方法有以下幾種。</p>
<ul>
<li>
<p>設置<code>android:persistent="true"</code>，這是application的一個屬性，官方都不建議使用。</p>
<pre class="hljs applescript"><code class="applescript">  Whether <span class="hljs-keyword">or</span> <span class="hljs-keyword">not</span> <span class="hljs-keyword">the</span> <span class="hljs-built_in">application</span> should remain <span class="hljs-built_in">running</span> <span class="hljs-keyword">at</span> all <span class="hljs-keyword">times</span>，
  <span class="hljs-string">"true"</span> <span class="hljs-keyword">if</span> <span class="hljs-keyword">it</span> should, <span class="hljs-keyword">and</span> <span class="hljs-string">"false"</span> <span class="hljs-keyword">if</span> <span class="hljs-keyword">not</span>. 
  The default value <span class="hljs-keyword">is</span> <span class="hljs-string">"false"</span>. 
  Applications should <span class="hljs-keyword">not</span> normally <span class="hljs-keyword">set</span> this flag; 
  persistence mode <span class="hljs-keyword">is</span> intended only <span class="hljs-keyword">for</span> certain system applications.</code></pre>
</li>
<li>
<p>設置<code>android:priority</code>優先級，這個並不是Service的屬性。這個屬性是在<code>intent-filter</code>中設置的。<a href="https://developer.android.com/guide/topics/manifest/intent-filter-element.html" target="_blank">官方解釋</a>，這個屬性只對活動和廣播有用，而且這個是接受Intent的優先級，並不是在內存中的優先級，呵呵。</p>
<pre class="hljs livecodeserver"><code class="livecodeserver">  android:priority
  The priority that should be given <span class="hljs-built_in">to</span> <span class="hljs-keyword">the</span> parent component <span class="hljs-keyword">with</span> regard <span class="hljs-built_in">to</span> 
  handling intents <span class="hljs-keyword">of</span> <span class="hljs-keyword">the</span> type described <span class="hljs-keyword">by</span> <span class="hljs-keyword">the</span> <span class="hljs-built_in">filter</span>. 
  This attribute has meaning <span class="hljs-keyword">for</span> both activities <span class="hljs-keyword">and</span> broadcast receivers。</code></pre>
</li>
<li>
<p>在Service的<code>onDestroy</code>中發送廣播，然後重啟服務，就目前我知道的，會出現Service的<code>onDestroy</code>不調用的情況。</p>
</li>
<li>
<p><code>startForeground</code>，這個上面提到過，是通過<code>Notification</code>提升優先級。</p>
</li>
<li>設置<code>onStartCommand()</code>返回值，讓服務被殺死後，系統重新創建服務，上面提到過。</li>
</ul>
<p>五個裡面就兩個能稍微有點用，所以啊，網絡謠傳害死人。</p>
<h3>12. IntentService</h3>
<p>敲黑板時間，重點來了，官方強力推薦。</p>
<p>前面提到兩點。</p>
<ul>
<li>因為Service中幾個方法的回調都是在主線程中，如果使用Service執行特別耗時的操作，建議單獨新建線程去操作，避免阻塞主線程（UI線程）</li>
<li>啟動服務和停止服務是成對出現的，需要手動停止服務</li>
</ul>
<p>IntentService完美的幫我們解決了這個問題，在內部幫我們新建的線程，不需要我們手動新建，執行完畢任務後會自動關閉。IntentService也是一個抽象類，裡面有一個<code>onHandleIntent(Intent intent)</code>抽象方法，這個方法是在非UI線程調用的，在這裡執行耗時的操作。</p>
<p>IntentService使用非UI線程逐一處理所有的啟動需求，它在內部使用Handler，將所有的請求放入隊列中，依次處理，關於Handler可以看<a href="http://www.jianshu.com/p/e72aba99012a" target="_blank">這篇文章</a>，也就是說IntentService不能同時處理多個請求，如果不要求服務同時處理多個請求，可以考慮使用IntentService。</p>
<p>IntentService在內部使用<code>HandlerThread</code>配合<code>Handler</code>來處理耗時操作。</p>
<pre class="hljs java"><code class="java"><span class="hljs-keyword">private</span> <span class="hljs-keyword">final</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">ServiceHandler</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">Handler</span> </span>{
    <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-title">ServiceHandler</span><span class="hljs-params">(Looper looper)</span> </span>{
        <span class="hljs-keyword">super</span>(looper);
    }

    <span class="hljs-meta">@Override</span>
    <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">handleMessage</span><span class="hljs-params">(Message msg)</span> </span>{
        onHandleIntent((Intent)msg.obj);
        stopSelf(msg.arg1);
    }
}

<span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">int</span> <span class="hljs-title">onStartCommand</span><span class="hljs-params">(Intent intent, <span class="hljs-keyword">int</span> flags, <span class="hljs-keyword">int</span> startId)</span> </span>{
    onStart(intent, startId);
    <span class="hljs-keyword">return</span> mRedelivery ? START_REDELIVER_INTENT : START_NOT_STICKY;
}

<span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">onStart</span><span class="hljs-params">(Intent intent, <span class="hljs-keyword">int</span> startId)</span> </span>{
    Message msg = mServiceHandler.obtainMessage();
    msg.arg1 = startId;
    msg.obj = intent;
    mServiceHandler.sendMessage(msg);
}</code></pre>
<p>注意<code>msg.arg1</code>它是請求的唯一標識，每發送一個請求，會生成一個唯一標識，然後將請求放入<code>Handler</code>處理隊列中，從源代碼裡面可以看見，在執行完畢<code>onHandleIntent</code>方法後，會執行<code>stopSelf</code>來關閉本身，同時IntentService中<code>onBind()</code>方法默認返回<code>null</code>，這說明<strong>啟動IntetService的方式最好是用<code>startService</code>方法</strong>，這樣在服務執行完畢後才會自動停止；如果使用<code>bindService</code>來啟動服務，還是需要調用<code>unbindService</code>來解綁服務的，也需要複寫<code>onBind()</code>方法。</p>
<blockquote><p>小盆宇：在<code>onHandleIntent</code>中，執行<code>onHandleIntent</code>後緊接著執行stopSelf(int startId)，把服務就給停止了，那第一個請求執行完畢服務就停止了，後續的請求怎麼會執行？</p></blockquote>
<p>注意stopSelf(int startID)方法作用是在其參數startId跟最後啟動該service時生成的id相等時才會執行停止服務，當有多個請求時候，如果發現當前請求的startId不是最後一個請求的id，那麼不會停止服務，所以只有當最後一個請求執行完畢後，才回停止服務。</p>
<h3>13. 總結</h3>
<p>在年前寫了出來，比Activity中的坑少了太多，希望對大家有幫助。下一篇是關於Android中進程通信Messenger/AIDL的，是本篇的補充，但是也屬於單獨的知識點。</p>

        </div>