# Android：遠程服務Service（含AIDL & IPC講解）


<hr>
<h1>前言</h1>
<ul>
<li>Service作為Android四大組件之一，應用非常廣泛</li>
<li>本文將介紹Service其中一種常見用法：遠程Service<blockquote><p>如果你對Service還未了解，建議先閱讀我寫的另外一篇文章：<br><a href="http://www.jianshu.com/p/d963c55c3ab9" target="_blank">Android四大組件：Service史上最全面解析</a></p></blockquote>
</li>
</ul>
<hr>
<h1>目錄</h1>
<div class="image-package">
<img src="images/944365-ea7ba2c6e52ca163.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-ea7ba2c6e52ca163.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">目錄</div>
</div>
<hr>
<h1>1. 遠程服務與本地服務的區別</h1>
<ul>
<li>遠程服務與本地服務最大的區別是：遠程Service與調用者不在同一個進程裡（即遠程Service是運行在另外一個進程）；而本地服務則是與調用者運行在同一個進程裡</li>
<li>二者區別的詳細區別如下圖：</li>
</ul>
<div class="image-package">
<img src="images/944365-843b2b4e2988ff66.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-843b2b4e2988ff66.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">按運行地點分類</div>
</div>
<hr>
<h1>2. 使用場景</h1>
<p>多個應用程序共享同一個後臺服務（遠程服務）</p>
<blockquote><p>即一個遠程Service與多個應用程序的組件（四大組件）進行跨進程通信</p></blockquote>
<div class="image-package">
<img src="images/944365-0e2a0b99bf323de8.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-0e2a0b99bf323de8.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">使用場景</div>
</div>
<hr>
<h1>3. 具體使用</h1>
<ul>
<li>
<p>為了讓遠程Service與多個應用程序的組件（四大組件）進行跨進程通信（IPC），需要使用AIDL</p>
<blockquote><ol>
<li>IPC：Inter-Process Communication，即跨進程通信</li>
<li>AIDL：Android Interface Definition Language，即Android接口定義語言；用於讓某個Service與多個應用程序組件之間進行跨進程通信，從而可以實現多個應用程序共享同一個Service的功能。</li>
</ol></blockquote>
</li>
<li>
<p>在多進程通信中，存在兩個進程角色（以最簡單的為例）：服務器端和客戶端</p>
</li>
<li>
<p>以下是兩個進程角色的具體使用步驟：<br><strong>服務器端（Service）</strong><br>步驟1：新建定義AIDL文件，並聲明該服務需要向客戶端提供的接口<br>步驟2：在Service子類中實現AIDL中定義的接口方法，並定義生命週期的方法（onCreat、onBind()、blabla）<br>步驟3：在AndroidMainfest.xml中註冊服務 &amp; 聲明為遠程服務</p>
<p><strong>客戶端（Client）</strong><br>步驟1：拷貝服務端的AIDL文件到目錄下<br>步驟2：使用Stub.asInterface接口獲取服務器的Binder，根據需要調用服務提供的接口方法<br>步驟3：通過Intent指定服務端的服務名稱和所在包，綁定遠程Service</p>
</li>
</ul>
<p>接下來，我將用一個具體實例來介紹遠程Service的使用</p>
<hr>
<h1>4. 具體實例</h1>
<ul>
<li>實例描述：客戶端遠程調用服務器端的遠程Service</li>
<li>具體使用：</li>
</ul>
<h3>4.1 服務器端（Service）</h3>
<p>新建一個服務器端的工程：Service - server</p>
<blockquote><p>先下Demo再看，效果會更好：<a href="https://github.com/Carson-Ho/Service_Server" target="_blank">Github_RemoteService_Server</a></p></blockquote>
<p>步驟1. 新建一個AIDL文件</p>
<div class="image-package">
<img src="images/944365-765bf5673bb721f4.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-765bf5673bb721f4.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">新建AIDL文件</div>
</div>
<p>步驟2. 在新建AIDL文件裡定義Service需要與Activity進行通信的內容（方法），並進行編譯（Make Project）</p>
<pre class="hljs java"><code class="java"><span class="hljs-comment">// 在新建的AIDL_Service1.aidl裡聲明需要與Activity進行通信的方法</span>
<span class="hljs-keyword">package</span> scut.carson_ho.demo_service;

<span class="hljs-class"><span class="hljs-keyword">interface</span> <span class="hljs-title">AIDL_Service1</span> </span>{
    <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">AIDL_Service</span><span class="hljs-params">()</span></span>;
}
<span class="hljs-comment">//AIDL中支持以下的數據類型</span>
<span class="hljs-comment">//1. 基本數據類型</span>
<span class="hljs-comment">//2. String 和CharSequence</span>
<span class="hljs-comment">//3. List 和 Map ,List和Map 對象的元素必須是AIDL支持的數據類型;</span>
<span class="hljs-comment">//4. AIDL自動生成的接口（需要導入-import）</span>
<span class="hljs-comment">//5. 實現android.os.Parcelable 接口的類（需要導入-import)</span></code></pre>
<div class="image-package">
<img src="images/944365-b3c4011539256750.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-b3c4011539256750.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">編譯</div>
</div>
<p>步驟3：在Service子類中實現AIDL中定義的接口方法，並定義生命週期的方法（onCreat、onBind()、blabla）<br><em>MyService.java</em></p>
<pre class="hljs aspectj"><code class="aspectj"><span class="hljs-keyword">public</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">MyService</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">Service</span> </span>{

    <span class="hljs-comment">// 實例化AIDL的Stub類(Binder的子類)</span>
    AIDL_Service1.Stub mBinder = <span class="hljs-keyword">new</span> AIDL_Service1.Stub() {

        <span class="hljs-comment">//重寫接口裡定義的方法</span>
        <span class="hljs-meta">@Override</span>
        <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">AIDL_Service</span><span class="hljs-params">()</span> <span class="hljs-keyword">throws</span> RemoteException </span>{
            System.out.println(<span class="hljs-string">"客戶端通過AIDL與遠程後臺成功通信"</span>);
        }
    };

<span class="hljs-comment">//重寫與Service生命週期的相關方法</span>
    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">onCreate</span><span class="hljs-params">()</span> </span>{
        <span class="hljs-keyword">super</span>.onCreate();

        System.out.println(<span class="hljs-string">"執行了onCreat()"</span>);

    }

    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">int</span> <span class="hljs-title">onStartCommand</span><span class="hljs-params">(Intent intent, <span class="hljs-keyword">int</span> flags, <span class="hljs-keyword">int</span> startId)</span> </span>{
        System.out.println(<span class="hljs-string">"執行了onStartCommand()"</span>);
        <span class="hljs-function"><span class="hljs-keyword">return</span> <span class="hljs-keyword">super</span>.<span class="hljs-title">onStartCommand</span><span class="hljs-params">(intent, flags, startId)</span></span>;
    }

    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">onDestroy</span><span class="hljs-params">()</span> </span>{
        <span class="hljs-keyword">super</span>.onDestroy();
        System.out.println(<span class="hljs-string">"執行了onDestory()"</span>);
    }

    <span class="hljs-meta">@Nullable</span>
    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-function">IBinder <span class="hljs-title">onBind</span><span class="hljs-params">(Intent intent)</span> </span>{

        System.out.println(<span class="hljs-string">"執行了onBind()"</span>);
    <span class="hljs-comment">//在onBind()返回繼承自Binder的Stub類型的Binder，非常重要</span>
        <span class="hljs-keyword">return</span> mBinder;
    }

    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">boolean</span> <span class="hljs-title">onUnbind</span><span class="hljs-params">(Intent intent)</span> </span>{
        System.out.println(<span class="hljs-string">"執行了onUnbind()"</span>);
        <span class="hljs-function"><span class="hljs-keyword">return</span> <span class="hljs-keyword">super</span>.<span class="hljs-title">onUnbind</span><span class="hljs-params">(intent)</span></span>;
    }

}</code></pre>
<p>步驟4：在AndroidMainfest.xml中註冊服務 &amp; 聲明為遠程服務</p>
<pre class="hljs javascript"><code class="javascript">&lt;service
            android:name=<span class="hljs-string">".MyService"</span>
            android:process=<span class="hljs-string">":remote"</span>  <span class="hljs-comment">//將本地服務設置成遠程服務</span>
            android:exported=<span class="hljs-string">"true"</span>      <span class="hljs-comment">//設置可被其他進程調用</span>
            &gt;
            <span class="hljs-comment">//該Service可以響應帶有scut.carson_ho.service_server.AIDL_Service1這個action的Intent。</span>
            <span class="hljs-comment">//此處Intent的action必須寫成“服務器端包名.aidl文件名”</span>
            &lt;intent-filter&gt;
                <span class="xml"><span class="hljs-tag">&lt;<span class="hljs-name">action</span> <span class="hljs-attr">android:name</span>=<span class="hljs-string">"scut.carson_ho.service_server.AIDL_Service1"</span>/&gt;</span>
            <span class="hljs-tag">&lt;/<span class="hljs-name">intent-filter</span>&gt;</span>

        <span class="hljs-tag">&lt;/<span class="hljs-name">service</span>&gt;</span></span></code></pre>
<p>至此，服務器端（遠程Service）已經完成了。</p>
<h2>4.2 客戶端（Client）</h2>
<p>新建一個客戶端的工程：Service - Client</p>
<blockquote><p>先下Demo再看，效果會更好：<a href="https://github.com/Carson-Ho/Service_Client" target="_blank">Github_RemoteService_Client</a></p></blockquote>
<p>步驟1：將服務端的AIDL文件所在的包複製到客戶端目錄下（Project/app/src/main），並進行編譯</p>
<blockquote><p>注：記得要原封不動地複製！！什麼都不要改！</p></blockquote>
<div class="image-package">
<img src="images/944365-83eec5d94c1549dc.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-83eec5d94c1549dc.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">複製後的目錄</div>
</div>
<p>步驟2：在主佈局文件定義“綁定服務”的按鈕<br><em>MainActivity.xml</em></p>
<pre class="hljs xml"><code class="xml"><span class="php"><span class="hljs-meta">&lt;?</span>xml version=<span class="hljs-string">"1.0"</span> encoding=<span class="hljs-string">"utf-8"</span><span class="hljs-meta">?&gt;</span></span>
<span class="hljs-tag">&lt;<span class="hljs-name">RelativeLayout</span> <span class="hljs-attr">xmlns:android</span>=<span class="hljs-string">"http://schemas.android.com/apk/res/android"</span>
    <span class="hljs-attr">xmlns:tools</span>=<span class="hljs-string">"http://schemas.android.com/tools"</span>
    <span class="hljs-attr">android:layout_width</span>=<span class="hljs-string">"match_parent"</span>
    <span class="hljs-attr">android:layout_height</span>=<span class="hljs-string">"match_parent"</span>
    <span class="hljs-attr">android:paddingBottom</span>=<span class="hljs-string">"@dimen/activity_vertical_margin"</span>
    <span class="hljs-attr">android:paddingLeft</span>=<span class="hljs-string">"@dimen/activity_horizontal_margin"</span>
    <span class="hljs-attr">android:paddingRight</span>=<span class="hljs-string">"@dimen/activity_horizontal_margin"</span>
    <span class="hljs-attr">android:paddingTop</span>=<span class="hljs-string">"@dimen/activity_vertical_margin"</span>
    <span class="hljs-attr">tools:context</span>=<span class="hljs-string">"scut.carson_ho.service_client.MainActivity"</span>&gt;</span>

    <span class="hljs-tag">&lt;<span class="hljs-name">Button</span>
        <span class="hljs-attr">android:layout_centerInParent</span>=<span class="hljs-string">"true"</span>
        <span class="hljs-attr">android:id</span>=<span class="hljs-string">"@+id/bind_service"</span>
        <span class="hljs-attr">android:layout_width</span>=<span class="hljs-string">"match_parent"</span>
        <span class="hljs-attr">android:layout_height</span>=<span class="hljs-string">"wrap_content"</span>
        <span class="hljs-attr">android:text</span>=<span class="hljs-string">"綁定服務"</span>
        /&gt;</span>
<span class="hljs-tag">&lt;/<span class="hljs-name">RelativeLayout</span>&gt;</span></code></pre>
<p>步驟3：在MainActivity.java裡</p>
<ul>
<li>使用Stub.asInterface接口獲取服務器的Binder；</li>
<li>通過Intent指定服務端的服務名稱和所在包，進行Service綁定；</li>
<li>根據需要調用服務提供的接口方法。</li>
</ul>
<p><em>MainActivity.java</em></p>
<pre class="hljs java"><code class="java"><span class="hljs-keyword">public</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">MainActivity</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">AppCompatActivity</span> </span>{

        <span class="hljs-keyword">private</span> Button bindService;

        <span class="hljs-comment">//定義aidl接口變量</span>
        <span class="hljs-keyword">private</span> AIDL_Service1 mAIDL_Service;

        <span class="hljs-comment">//創建ServiceConnection的匿名類</span>
        <span class="hljs-keyword">private</span> ServiceConnection connection = <span class="hljs-keyword">new</span> ServiceConnection() {

            <span class="hljs-comment">//重寫onServiceConnected()方法和onServiceDisconnected()方法</span>
            <span class="hljs-comment">//在Activity與Service建立關聯和解除關聯的時候調用</span>
            <span class="hljs-meta">@Override</span>
            <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">onServiceDisconnected</span><span class="hljs-params">(ComponentName name)</span> </span>{
            }

            <span class="hljs-comment">//在Activity與Service建立關聯時調用</span>
            <span class="hljs-meta">@Override</span>
            <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">onServiceConnected</span><span class="hljs-params">(ComponentName name, IBinder service)</span> </span>{

                <span class="hljs-comment">//使用AIDLService1.Stub.asInterface()方法獲取服務器端返回的IBinder對象</span>
                <span class="hljs-comment">//將IBinder對象傳換成了mAIDL_Service接口對象</span>
                mAIDL_Service = AIDL_Service1.Stub.asInterface(service);

                <span class="hljs-keyword">try</span> {

                    <span class="hljs-comment">//通過該對象調用在MyAIDLService.aidl文件中定義的接口方法,從而實現跨進程通信</span>
                    mAIDL_Service.AIDL_Service();

                } <span class="hljs-keyword">catch</span> (RemoteException e) {
                    e.printStackTrace();
                }
            }
        };


        <span class="hljs-meta">@Override</span>
        <span class="hljs-function"><span class="hljs-keyword">protected</span> <span class="hljs-keyword">void</span> <span class="hljs-title">onCreate</span><span class="hljs-params">(Bundle savedInstanceState)</span> </span>{
            <span class="hljs-keyword">super</span>.onCreate(savedInstanceState);
            setContentView(R.layout.activity_main);

            bindService = (Button) findViewById(R.id.bind_service);

            <span class="hljs-comment">//設置綁定服務的按鈕</span>
            bindService.setOnClickListener(<span class="hljs-keyword">new</span> View.OnClickListener() {
                <span class="hljs-meta">@Override</span>
                <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">onClick</span><span class="hljs-params">(View v)</span> </span>{

                    <span class="hljs-comment">//通過Intent指定服務端的服務名稱和所在包，與遠程Service進行綁定</span>
                    <span class="hljs-comment">//參數與服務器端的action要一致,即"服務器包名.aidl接口文件名"</span>
                    Intent intent = <span class="hljs-keyword">new</span> Intent(<span class="hljs-string">"scut.carson_ho.service_server.AIDL_Service1"</span>);

                    <span class="hljs-comment">//Android5.0後無法只通過隱式Intent綁定遠程Service</span>
                    <span class="hljs-comment">//需要通過setPackage()方法指定包名</span>
                    intent.setPackage(<span class="hljs-string">"scut.carson_ho.service_server"</span>);

                    <span class="hljs-comment">//綁定服務,傳入intent和ServiceConnection對象</span>
                    bindService(intent, connection, Context.BIND_AUTO_CREATE);

                }
            });
        }

    }</code></pre>
<h2>4.3 測試結果</h2>
<div class="image-package">
<img src="images/944365-90d3da78fdab4acd.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-90d3da78fdab4acd.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">點擊綁定服務按鈕</div>
</div>
<p>從上面測試結果可以看出：</p>
<ul>
<li>打印的語句分別運行在不同進程（看語句前面的包名）；</li>
<li>客戶端調用了服務端Service的方法</li>
</ul>
<p>即<strong>客戶端和服務端進行了跨進程通信</strong></p>
<h1>4.4 Demo地址</h1>
<ul>
<li>客戶端：<a href="https://github.com/Carson-Ho/Service_Client" target="_blank">Github_RemoteService_Client</a>
</li>
<li>服務端：<a href="https://github.com/Carson-Ho/Service_Server" target="_blank">Github_RemoteService_Server</a>
</li>
</ul>
<hr>
<h1>5. 總結</h1>
<ul>
<li>本文對Android組件Service中的遠程Service進行了全面介紹</li>
<li>如果你還想了解關於Service的其他知識，請瀏覽以下文章：<blockquote><p><a href="http://www.jianshu.com/p/d963c55c3ab9" target="_blank">Android四大組件：Service史上最全面解析</a><br><a href="http://www.jianshu.com/p/8d0cde35eb10" target="_blank">Android：Service生命週期最全面解析</a><br><a href="http://www.jianshu.com/p/e04c4239b07e" target="_blank">Android：（本地、可通信的、前臺、遠程）Service使用全面介紹</a><br><a href="http://www.jianshu.com/p/34326751b2c6" target="_blank">Android：遠程服務Service（含AIDL &amp; IPC講解）</a><br><a href="http://www.jianshu.com/p/8a3c44a9173a" target="_blank">Android多線程全面解析：IntentService用法&amp;源碼</a></p></blockquote>
</li>
<li>接下來會介紹繼續介紹Android開發中的相關知識，有興趣可以繼續關注<a href="http://www.jianshu.com/users/383970bef0a0/latest_articles" target="_blank">Carson_Ho的安卓開發筆記</a>
</li>
</ul>
<hr>
<h4>請點贊！因為你們的贊同/鼓勵是我寫作的最大動力！</h4>
<blockquote><p><strong>相關文章閱讀</strong><br><a href="http://www.jianshu.com/p/ec5a1a30694b" target="_blank">Android開發：最全面、最易懂的Android屏幕適配解決方案</a><br><a href="http://www.jianshu.com/p/9fe944ee02f7" target="_blank">Android開發：Handler異步通信機制全面解析（包含Looper、Message Queue）</a><br><a href="http://www.jianshu.com/p/ce1d060573ba" target="_blank">Android開發：頂部Tab導航欄實現（TabLayout+ViewPager+Fragment）</a><br><a href="http://www.jianshu.com/p/a663803b2a44" target="_blank">Android開發：底部Tab菜單欄實現（FragmentTabHost+ViewPager）</a><br><a href="http://www.jianshu.com/p/b87fee2f7a23" target="_blank">Android開發：JSON簡介及最全面解析方法!</a><br><a href="http://www.jianshu.com/p/e636f4f8487b" target="_blank">Android開發：XML簡介及DOM、SAX、PULL解析對比</a></p></blockquote>
<hr>
