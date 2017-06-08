# Android多線程全面解析：IntentService用法&源碼


<div class="show-content">
          <div class="image-package">
<img src="images/944365-207a738cb165a2da.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-207a738cb165a2da.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption"></div>
</div>
<hr>
<h1>前言</h1>
<ul>
<li>
<p>多線程的應用在Android開發中是非常常見的，常用方法主要有：</p>
<ol>
<li>繼承Thread類</li>
<li>實現Runnable接口</li>
<li>AsyncTask</li>
<li>Handler</li>
<li>HandlerThread</li>
<li>IntentService</li>
</ol>
</li>
<li>
<p>今天，我將全面解析多線程其中一種常見用法：IntentService</p>
</li>
</ul>
<hr>
<h1>目錄</h1>
<div class="image-package">
<img src="images/944365-e6645ea074978ab1.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-e6645ea074978ab1.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">目錄</div>
</div>
<hr>
<h1>1. 定義</h1>
<p>IntentService是Android裡面的一個封裝類，繼承自四大組件之一的Service。</p>
<hr>
<h1>2. 作用</h1>
<p>處理異步請求，實現多線程</p>
<hr>
<h1>3. 工作流程</h1>
<div class="image-package">
<img src="images/944365-fa5bfe6dffa531ce.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-fa5bfe6dffa531ce.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">工作流程</div>
</div>
<p>注意：若啟動IntentService 多次，那麼每個耗時操作則以隊列的方式在 IntentService的onHandleIntent回調方法中依次執行，執行完自動結束。</p>
<hr>
<h1>4. 實現步驟</h1>
<ul>
<li>步驟1：定義IntentService的子類：傳入線程名稱、複寫onHandleIntent()方法</li>
<li>步驟2：在Manifest.xml中註冊服務</li>
<li>步驟3：在Activity中開啟Service服務</li>
</ul>
<h1>5. 具體實例</h1>
<ul>
<li>步驟1：定義IntentService的子類：傳入線程名稱、複寫onHandleIntent()方法</li>
</ul>
<pre class="hljs aspectj"><code class="aspectj"><span class="hljs-keyword">package</span> com.example.carson_ho.demoforintentservice;

<span class="hljs-keyword">import</span> android.app.IntentService;
<span class="hljs-keyword">import</span> android.content.Intent;
<span class="hljs-keyword">import</span> android.util.Log;

<span class="hljs-comment">/**
 * Created by Carson_Ho on 16/9/28.
 */</span>
<span class="hljs-keyword">public</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">myIntentService</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">IntentService</span> </span>{

    <span class="hljs-comment">/*構造函數*/</span>
    <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-title">myIntentService</span><span class="hljs-params">()</span> </span>{
        <span class="hljs-comment">//調用父類的構造函數</span>
        <span class="hljs-comment">//構造函數參數=工作線程的名字</span>
        <span class="hljs-keyword">super</span>(<span class="hljs-string">"myIntentService"</span>);

    }

    <span class="hljs-comment">/*複寫onHandleIntent()方法*/</span>
    <span class="hljs-comment">//實現耗時任務的操作</span>
    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">protected</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">onHandleIntent</span><span class="hljs-params">(Intent intent)</span> </span>{
        <span class="hljs-comment">//根據Intent的不同進行不同的事務處理</span>
        String taskName = intent.getExtras().getString(<span class="hljs-string">"taskName"</span>);
        <span class="hljs-keyword">switch</span> (taskName) {
            <span class="hljs-keyword">case</span> <span class="hljs-string">"task1"</span>:
                Log.i(<span class="hljs-string">"myIntentService"</span>, <span class="hljs-string">"do task1"</span>);
                <span class="hljs-keyword">break</span>;
            <span class="hljs-keyword">case</span> <span class="hljs-string">"task2"</span>:
                Log.i(<span class="hljs-string">"myIntentService"</span>, <span class="hljs-string">"do task2"</span>);
                <span class="hljs-keyword">break</span>;
            <span class="hljs-keyword">default</span>:
                <span class="hljs-keyword">break</span>;
        }
    }


    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">onCreate</span><span class="hljs-params">()</span> </span>{
        Log.i(<span class="hljs-string">"myIntentService"</span>, <span class="hljs-string">"onCreate"</span>);
        <span class="hljs-keyword">super</span>.onCreate();
    }

    <span class="hljs-comment">/*複寫onStartCommand()方法*/</span>
    <span class="hljs-comment">//默認實現將請求的Intent添加到工作隊列裡</span>
    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">int</span> <span class="hljs-title">onStartCommand</span><span class="hljs-params">(Intent intent, <span class="hljs-keyword">int</span> flags, <span class="hljs-keyword">int</span> startId)</span> </span>{
        Log.i(<span class="hljs-string">"myIntentService"</span>, <span class="hljs-string">"onStartCommand"</span>);
        <span class="hljs-function"><span class="hljs-keyword">return</span> <span class="hljs-keyword">super</span>.<span class="hljs-title">onStartCommand</span><span class="hljs-params">(intent, flags, startId)</span></span>;
    }

    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">onDestroy</span><span class="hljs-params">()</span> </span>{
        Log.i(<span class="hljs-string">"myIntentService"</span>, <span class="hljs-string">"onDestroy"</span>);
        <span class="hljs-keyword">super</span>.onDestroy();
    }
}</code></pre>
<ul>
<li>步驟2：在Manifest.xml中註冊服務</li>
</ul>
<pre class="hljs xml"><code class="xml"><span class="hljs-tag">&lt;<span class="hljs-name">service</span> <span class="hljs-attr">android:name</span>=<span class="hljs-string">".myIntentService"</span>&gt;</span>
            <span class="hljs-tag">&lt;<span class="hljs-name">intent-filter</span> &gt;</span>
                <span class="hljs-tag">&lt;<span class="hljs-name">action</span> <span class="hljs-attr">android:name</span>=<span class="hljs-string">"cn.scu.finch"</span>/&gt;</span>
            <span class="hljs-tag">&lt;/<span class="hljs-name">intent-filter</span>&gt;</span>
        <span class="hljs-tag">&lt;/<span class="hljs-name">service</span>&gt;</span></code></pre>
<ul>
<li>步驟3：在Activity中開啟Service服務</li>
</ul>
<pre class="hljs scala"><code class="scala"><span class="hljs-keyword">package</span> com.example.carson_ho.demoforintentservice;

<span class="hljs-keyword">import</span> android.content.<span class="hljs-type">Intent</span>;
<span class="hljs-keyword">import</span> android.os.<span class="hljs-type">Bundle</span>;
<span class="hljs-keyword">import</span> android.support.v7.app.<span class="hljs-type">AppCompatActivity</span>;

public <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">MainActivity</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">AppCompatActivity</span> </span>{

    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">protected</span> void onCreate(<span class="hljs-type">Bundle</span> savedInstanceState) {
        <span class="hljs-keyword">super</span>.onCreate(savedInstanceState);
        setContentView(<span class="hljs-type">R</span>.layout.activity_main);

            <span class="hljs-comment">//同一服務只會開啟一個工作線程</span>
            <span class="hljs-comment">//在onHandleIntent函數裡依次處理intent請求。</span>

            <span class="hljs-type">Intent</span> i = <span class="hljs-keyword">new</span> <span class="hljs-type">Intent</span>(<span class="hljs-string">"cn.scu.finch"</span>);
            <span class="hljs-type">Bundle</span> bundle = <span class="hljs-keyword">new</span> <span class="hljs-type">Bundle</span>();
            bundle.putString(<span class="hljs-string">"taskName"</span>, <span class="hljs-string">"task1"</span>);
            i.putExtras(bundle);
            startService(i);

            <span class="hljs-type">Intent</span> i2 = <span class="hljs-keyword">new</span> <span class="hljs-type">Intent</span>(<span class="hljs-string">"cn.scu.finch"</span>);
            <span class="hljs-type">Bundle</span> bundle2 = <span class="hljs-keyword">new</span> <span class="hljs-type">Bundle</span>();
            bundle2.putString(<span class="hljs-string">"taskName"</span>, <span class="hljs-string">"task2"</span>);
            i2.putExtras(bundle2);
            startService(i2);

            startService(i);  <span class="hljs-comment">//多次啟動</span>
        }
    }</code></pre>
<ul>
<li>結果<br><div class="image-package">
<img src="images/944365-fadf671e3671b52a.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-fadf671e3671b52a.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">運行結果</div>
</div>
</li>
</ul>
<h1>6. 源碼分析</h1>
<p>接下來，我們會通過源碼分析解決以下問題：</p>
<ul>
<li>IntentService如何單獨開啟一個新的工作線程；</li>
<li>IntentService如何通過onStartCommand()傳遞給服務intent被<strong>依次</strong>插入到工作隊列中</li>
</ul>
<p>問題1：IntentService如何單獨開啟一個新的工作線程</p>
<pre class="hljs java"><code class="java"><span class="hljs-comment">// IntentService源碼中的 onCreate() 方法</span>
<span class="hljs-meta">@Override</span>
<span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">onCreate</span><span class="hljs-params">()</span> </span>{
    <span class="hljs-keyword">super</span>.onCreate();
    <span class="hljs-comment">// HandlerThread繼承自Thread，內部封裝了 Looper</span>
    <span class="hljs-comment">//通過實例化andlerThread新建線程並啟動</span>
    <span class="hljs-comment">//所以使用IntentService時不需要額外新建線程</span>
    HandlerThread thread = <span class="hljs-keyword">new</span> HandlerThread(<span class="hljs-string">"IntentService["</span> + mName + <span class="hljs-string">"]"</span>);
    thread.start();

    <span class="hljs-comment">//獲得工作線程的 Looper，並維護自己的工作隊列</span>
    mServiceLooper = thread.getLooper();
    <span class="hljs-comment">//將上述獲得Looper與新建的mServiceHandler進行綁定</span>
    <span class="hljs-comment">//新建的Handler是屬於工作線程的。</span>
    mServiceHandler = <span class="hljs-keyword">new</span> ServiceHandler(mServiceLooper);
}

<span class="hljs-keyword">private</span> <span class="hljs-keyword">final</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">ServiceHandler</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">Handler</span> </span>{
    <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-title">ServiceHandler</span><span class="hljs-params">(Looper looper)</span> </span>{
        <span class="hljs-keyword">super</span>(looper);
    }

<span class="hljs-comment">//IntentService的handleMessage方法把接收的消息交給onHandleIntent()處理</span>
<span class="hljs-comment">//onHandleIntent()是一個抽象方法，使用時需要重寫的方法</span>
    <span class="hljs-meta">@Override</span>
    <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">handleMessage</span><span class="hljs-params">(Message msg)</span> </span>{
        <span class="hljs-comment">// onHandleIntent 方法在工作線程中執行，執行完調用 stopSelf() 結束服務。</span>
        onHandleIntent((Intent)msg.obj);
      <span class="hljs-comment">//onHandleIntent 處理完成後 IntentService會調用 stopSelf() 自動停止。</span>
        stopSelf(msg.arg1);
    }
}

<span class="hljs-comment">////onHandleIntent()是一個抽象方法，使用時需要重寫的方法</span>
<span class="hljs-meta">@WorkerThread</span>
<span class="hljs-function"><span class="hljs-keyword">protected</span> <span class="hljs-keyword">abstract</span> <span class="hljs-keyword">void</span> <span class="hljs-title">onHandleIntent</span><span class="hljs-params">(Intent intent)</span></span>;</code></pre>
<p><strong>問題2：IntentService如何通過onStartCommand()傳遞給服務intent被</strong>依次<strong>插入到工作隊列中</strong></p>
<pre class="hljs fortran"><code class="fortran"><span class="hljs-keyword">public</span> <span class="hljs-built_in">int</span> onStartCommand(<span class="hljs-keyword">Intent</span> <span class="hljs-keyword">intent</span>, <span class="hljs-built_in">int</span> flags, <span class="hljs-built_in">int</span> startId) {
    onStart(<span class="hljs-keyword">intent</span>, startId);
    <span class="hljs-keyword">return</span> mRedelivery ? START_REDELIVER_INTENT : START_NOT_STICKY;
}

<span class="hljs-keyword">public</span> void onStart(<span class="hljs-keyword">Intent</span> <span class="hljs-keyword">intent</span>, <span class="hljs-built_in">int</span> startId) {
    Message msg = mServiceHandler.obtainMessage();
    msg.arg1 = startId;
//把 <span class="hljs-keyword">intent</span> 參數包裝到 message 的 obj 中，然後發送消息，即添加到消息隊列裡
//這裡的<span class="hljs-keyword">Intent</span> 就是啟動服務時startService(<span class="hljs-keyword">Intent</span>) 裡的 <span class="hljs-keyword">Intent</span>。
    msg.obj = <span class="hljs-keyword">intent</span>;
    mServiceHandler.sendMessage(msg);
}

//清除消息隊列中的消息
@Override
<span class="hljs-keyword">public</span> void onDestroy() {
    mServiceLooper.quit();
}</code></pre>
<ul>
<li>總結<br>從上面源碼可以看出，IntentService本質是採用Handler &amp; HandlerThread方式：<ol>
<li>通過HandlerThread單獨開啟一個名為<strong>IntentService</strong>的線程</li>
<li>創建一個名叫ServiceHandler的內部Handler</li>
<li>把內部Handler與HandlerThread所對應的子線程進行綁定</li>
<li>通過onStartCommand()傳遞給服務intent，<strong>依次</strong>插入到工作隊列中，並逐個發送給onHandleIntent()</li>
<li>通過onHandleIntent()來依次處理所有Intent請求對象所對應的任務</li>
</ol>
</li>
</ul>
<p>因此我們通過複寫方法onHandleIntent()，再在裡面根據Intent的不同進行不同的線程操作就可以了</p>
<p><strong>注意事項1. 工作任務隊列是順序執行的。</strong></p>
<blockquote><p>如果一個任務正在IntentService中執行，此時你再發送一個新的任務請求，這個新的任務會一直等待直到前面一個任務執行完畢才開始執行</p></blockquote>
<p>  原因：</p>
<ol>
<li>由於onCreate() 方法只會調用一次，所以只會創建一個工作線程；</li>
<li>當多次調用 startService(Intent) 時（onStartCommand也會調用多次）其實並不會創建新的工作線程，只是把消息加入消息隊列中等待執行，<strong>所以，多次啟動 IntentService 會按順序執行事件</strong>
</li>
<li>如果服務停止，會清除消息隊列中的消息，後續的事件得不到執行。</li>
</ol>
<p><strong>注意事項2：不建議通過 bindService() 啟動 IntentService</strong><br>原因：</p>
<pre class="hljs aspectj"><code class="aspectj"><span class="hljs-meta">@Override</span>
<span class="hljs-keyword">public</span> <span class="hljs-function">IBinder <span class="hljs-title">onBind</span><span class="hljs-params">(Intent intent)</span> </span>{
    <span class="hljs-keyword">return</span> <span class="hljs-keyword">null</span>;
}</code></pre>
<p>在IntentService中，onBind()是默認返回null的，而採用bindService() 啟動 IntentService的生命週期是：<strong>onCreate() —&gt;onBind()—&gt;onunbind()—&gt;onDestory()</strong><br>並不會調用onstart()或者onstartcommand()方法，所以不會將消息發送到消息隊列，那麼onHandleIntent()將不會回調，即無法實現多線程的操作。</p>
<blockquote><p>此時，你使用的是Service，而不是IntentService</p></blockquote>
<h1>7. 使用場景</h1>
<ul>
<li>線程任務需要<strong>按順序</strong>、<strong>在後臺執行</strong>的使用場景<blockquote><p>最常見的場景：離線下載</p></blockquote>
</li>
<li>由於所有的任務都在同一個Thread looper裡面來做，所以不符合多個數據同時請求的場景。</li>
</ul>
<h1>8. 對比</h1>
<h3>8.1 IntentService與Service的區別</h3>
<ul>
<li>
<p>從屬性 &amp; 作用上來說<br>Service：依賴於應用程序的主線程（不是獨立的進程 or 線程）</p>
<blockquote><p>不建議在Service中編寫耗時的邏輯和操作，否則會引起ANR；</p></blockquote>
<p>IntentService：創建一個工作線程來處理多線程任務 　　</p>
</li>
<li>Service需要主動調用stopSelft()來結束服務，而IntentService不需要（在所有intent被處理完後，系統會自動關閉服務）<blockquote>
<p>此外：</p>
<ol>
<li>IntentService為Service的onBingd()方式提供了默認實現：返回null</li>
<li>IntentService為Service的onStartCommand（）方法提供了默認實現：將請求的intent添加到隊列中</li>
</ol>
</blockquote>
</li>
</ul>
<h3>8.2 IntentService與其他線程的區別</h3>
<ul>
<li>IntentService內部採用了HandlerThread實現，作用類似於後臺線程；</li>
<li>與後臺線程相比，<strong>IntentService是一種後臺服務</strong>，優勢是：優先級高（不容易被系統殺死），從而保證任務的執行<blockquote><p>對於後臺線程，若進程中沒有活動的四大組件，則該線程的優先級非常低，容易被系統殺死，無法保證任務的執行</p></blockquote>
</li>
</ul>
<h1>9. 總結</h1>
<ul>
<li>本文主要對多線程IntentService用法&amp;源碼進行了全面介紹</li>
<li>接下來，我會繼續講解Android開發中關於多線程的知識，包括繼承Thread類、實現Runnable接口、Handler等等，有興趣可以繼續關注<a href="http://www.jianshu.com/users/383970bef0a0/latest_articles" target="_blank">Carson_Ho的安卓開發筆記</a>
</li>
</ul>
<hr>
<h1>請點贊！因為你的鼓勵是我寫作的最大動力！</h1>
<blockquote><p><strong>相關文章閱讀</strong><br><a href="http://www.jianshu.com/p/6e5eda3a51af" target="_blank">1分鐘全面瞭解“設計模式”</a><br><a href="http://www.jianshu.com/p/ec5a1a30694b" target="_blank">Android開發：最全面、最易懂的Android屏幕適配解決方案</a><br><a href="http://www.jianshu.com/p/9fe944ee02f7" target="_blank">Android開發：Handler異步通信機制全面解析（包含Looper、Message Queue）</a><br><a href="http://www.jianshu.com/p/ce1d060573ba" target="_blank">Android開發：頂部Tab導航欄實現（TabLayout+ViewPager+Fragment）</a><br><a href="http://www.jianshu.com/p/a663803b2a44" target="_blank">Android開發：底部Tab菜單欄實現（FragmentTabHost+ViewPager）</a><br><a href="http://www.jianshu.com/p/b87fee2f7a23" target="_blank">Android開發：JSON簡介及最全面解析方法!</a><br><a href="http://www.jianshu.com/p/e636f4f8487b" target="_blank">Android開發：XML簡介及DOM、SAX、PULL解析對比</a></p></blockquote>
<hr>
<h3>歡迎關注<a href="http://www.jianshu.com/users/383970bef0a0/latest_articles" target="_blank">Carson_Ho</a>的簡書！</h3>
<p>不定期分享關於<strong>安卓開發</strong>的乾貨，追求<strong>短、平、快</strong>，但<strong>卻不缺深度</strong>。<br></p><div class="image-package">
<img src="images/944365-9b76fa3c52d478a7.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-9b76fa3c52d478a7.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption"></div>
</div>

        </div>