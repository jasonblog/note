# （本地、可通信的、前臺、遠程）Service使用全面介紹


<hr>
<h1>前言</h1>
<ul>
<li>Service作為Android四大組件之一，應用非常廣泛</li>
<li>本文將介紹Service最基礎的知識：Service的生命週期<blockquote><p>如果你對Service還未了解，建議先閱讀我寫的文章：<br><a href="http://www.jianshu.com/p/d963c55c3ab9" target="_blank">Android四大組件：Service史上最全面解析</a></p></blockquote>
</li>
</ul>
<hr>
<h1>目錄</h1>
<div class="image-package">
<img src="images/944365-db081c31bd7b64c9.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-db081c31bd7b64c9.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">目錄</div>
</div>
<hr>
<h1>1. Service分類</h1>
<h3>1.1 Service的類型</h3>
<div class="image-package">
<img src="images/944365-d42fa78828930bdb.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-d42fa78828930bdb.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">分類</div>
</div>
<h3>1.2  特點</h3>
<div class="image-package">
<img src="images/944365-8855e3a5340bece5.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-8855e3a5340bece5.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">Service類型的詳細介紹</div>
</div>
<hr>
<h1>2.具體使用解析</h1>
<h3>2.1 本地Service</h3>
<p>這是最普通、最常用的後臺服務Service。</p>
<h3>2.1.1 使用步驟</h3>
<ul>
<li>步驟1：新建子類繼承Service類<blockquote><p>需重寫父類的onCreate()、onStartCommand()、onDestroy()和onBind()方法</p></blockquote>
</li>
<li>步驟2：構建用於啟動Service的Intent對象</li>
<li>步驟3：調用startService()啟動Service、調用stopService()停止服務</li>
<li>步驟4：在AndroidManifest.xml裡註冊Service</li>
</ul>
<h3>2.1.2 實例Demo</h3>
<p>接下來我將用一個實例Demo進行本地Service說明</p>
<blockquote><p>建議先下載Demo再進行閱讀：<a href="https://github.com/Carson-Ho/Demo_Service/tree/5e2a70cf2d75c56bbfa1abc0ead16c5ad8cae83f" target="_blank">（carson.ho的Github地址）Demo_for_Service</a></p></blockquote>
<ul>
<li>步驟1：新建子類繼承Service類<blockquote><p>需重寫父類的onCreate()、onStartCommand()、onDestroy()和onBind()</p></blockquote>
</li>
</ul>
<p><em>MyService.java</em></p>
<pre class="hljs aspectj"><code class="aspectj"><span class="hljs-keyword">public</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">MyService</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">Service</span> </span>{


<span class="hljs-comment">//啟動Service之後，就可以在onCreate()或onStartCommand()方法裡去執行一些具體的邏輯</span>
<span class="hljs-comment">//由於這裡作Demo用，所以只打印一些語句</span>
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
        <span class="hljs-keyword">return</span> <span class="hljs-keyword">null</span>;
    }
}</code></pre>
<ul>
<li>步驟2：在主佈局文件設置兩個Button分別用於啟動和停止Service<br><em>activity_main.xml</em>
</li>
</ul>
<pre class="hljs xml"><code class="xml"><span class="php"><span class="hljs-meta">&lt;?</span>xml version=<span class="hljs-string">"1.0"</span> encoding=<span class="hljs-string">"utf-8"</span><span class="hljs-meta">?&gt;</span></span>
<span class="hljs-tag">&lt;<span class="hljs-name">RelativeLayout</span> <span class="hljs-attr">xmlns:android</span>=<span class="hljs-string">"http://schemas.android.com/apk/res/android"</span>
    <span class="hljs-attr">xmlns:tools</span>=<span class="hljs-string">"http://schemas.android.com/tools"</span>
    <span class="hljs-attr">android:layout_width</span>=<span class="hljs-string">"match_parent"</span>
    <span class="hljs-attr">android:layout_height</span>=<span class="hljs-string">"match_parent"</span>
    <span class="hljs-attr">android:paddingBottom</span>=<span class="hljs-string">"@dimen/activity_vertical_margin"</span>
    <span class="hljs-attr">android:paddingLeft</span>=<span class="hljs-string">"@dimen/activity_horizontal_margin"</span>
    <span class="hljs-attr">android:paddingRight</span>=<span class="hljs-string">"@dimen/activity_horizontal_margin"</span>
    <span class="hljs-attr">android:paddingTop</span>=<span class="hljs-string">"@dimen/activity_vertical_margin"</span>
    <span class="hljs-attr">tools:context</span>=<span class="hljs-string">"scut.carson_ho.demo_service.MainActivity"</span>&gt;</span>

    <span class="hljs-tag">&lt;<span class="hljs-name">Button</span>
        <span class="hljs-attr">android:layout_centerInParent</span>=<span class="hljs-string">"true"</span>
        <span class="hljs-attr">android:id</span>=<span class="hljs-string">"@+id/startService"</span>
        <span class="hljs-attr">android:layout_width</span>=<span class="hljs-string">"wrap_content"</span>
        <span class="hljs-attr">android:layout_height</span>=<span class="hljs-string">"wrap_content"</span>
        <span class="hljs-attr">android:text</span>=<span class="hljs-string">"啟動服務"</span> /&gt;</span>

    <span class="hljs-tag">&lt;<span class="hljs-name">Button</span>
        <span class="hljs-attr">android:layout_centerInParent</span>=<span class="hljs-string">"true"</span>
        <span class="hljs-attr">android:layout_below</span>=<span class="hljs-string">"@+id/startService"</span>
        <span class="hljs-attr">android:id</span>=<span class="hljs-string">"@+id/stopService"</span>
        <span class="hljs-attr">android:layout_width</span>=<span class="hljs-string">"wrap_content"</span>
        <span class="hljs-attr">android:layout_height</span>=<span class="hljs-string">"wrap_content"</span>
        <span class="hljs-attr">android:text</span>=<span class="hljs-string">"停止服務"</span> /&gt;</span>
<span class="hljs-tag">&lt;/<span class="hljs-name">RelativeLayout</span>&gt;</span></code></pre>
<ul>
<li>步驟3：構建Intent對象，並調用startService()啟動Service、stopService停止服務</li>
</ul>
<p><em>MainActivity.java</em></p>
<pre class="hljs groovy"><code class="groovy"><span class="hljs-keyword">public</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">MainActivity</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">AppCompatActivity</span> <span class="hljs-keyword">implements</span> <span class="hljs-title">View</span>.<span class="hljs-title">OnClickListener</span> {</span>

    <span class="hljs-keyword">private</span> Button startService;
    <span class="hljs-keyword">private</span> Button stopService;

    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">protected</span> <span class="hljs-keyword">void</span> onCreate(Bundle savedInstanceState) {
        <span class="hljs-keyword">super</span>.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        startService = (Button) findViewById(R.id.startService);
        stopService = (Button) findViewById(R.id.stopService);

        startService.setOnClickListener(<span class="hljs-keyword">this</span>);
        startService.setOnClickListener(<span class="hljs-keyword">this</span>);
    }

    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> onClick(View v) {
        <span class="hljs-keyword">switch</span> (v.getId()) {

            <span class="hljs-comment">//點擊啟動Service Button</span>
            <span class="hljs-keyword">case</span> R.id.<span class="hljs-string">startService:</span>
                <span class="hljs-comment">//構建啟動服務的Intent對象</span>
                Intent startIntent = <span class="hljs-keyword">new</span> Intent(<span class="hljs-keyword">this</span>, MyService.<span class="hljs-keyword">class</span>);
                <span class="hljs-comment">//調用startService()方法-傳入Intent對象,以此啟動服務</span>
                startService(startIntent);

            <span class="hljs-comment">//點擊停止Service Button</span>
            <span class="hljs-keyword">case</span> R.id.<span class="hljs-string">stopService:</span>
                <span class="hljs-comment">//構建停止服務的Intent對象</span>
                Intent stopIntent = <span class="hljs-keyword">new</span> Intent(<span class="hljs-keyword">this</span>, MyService.<span class="hljs-keyword">class</span>);
                <span class="hljs-comment">//調用stopService()方法-傳入Intent對象,以此停止服務</span>
                stopService(stopIntent);

        }
    }
}</code></pre>
<ul>
<li>步驟4：在AndroidManifest.xml裡註冊Service<br><em>AndroidManifest.xml</em>
</li>
</ul>
<pre class="hljs xml"><code class="xml"><span class="php"><span class="hljs-meta">&lt;?</span>xml version=<span class="hljs-string">"1.0"</span> encoding=<span class="hljs-string">"utf-8"</span><span class="hljs-meta">?&gt;</span></span>
<span class="hljs-tag">&lt;<span class="hljs-name">manifest</span> <span class="hljs-attr">xmlns:android</span>=<span class="hljs-string">"http://schemas.android.com/apk/res/android"</span>
    <span class="hljs-attr">package</span>=<span class="hljs-string">"scut.carson_ho.demo_service"</span>&gt;</span>

    <span class="hljs-tag">&lt;<span class="hljs-name">application</span>
        <span class="hljs-attr">android:allowBackup</span>=<span class="hljs-string">"true"</span>
        <span class="hljs-attr">android:icon</span>=<span class="hljs-string">"@mipmap/ic_launcher"</span>
        <span class="hljs-attr">android:label</span>=<span class="hljs-string">"@string/app_name"</span>
        <span class="hljs-attr">android:supportsRtl</span>=<span class="hljs-string">"true"</span>
        <span class="hljs-attr">android:theme</span>=<span class="hljs-string">"@style/AppTheme"</span>&gt;</span>
        <span class="hljs-tag">&lt;<span class="hljs-name">activity</span> <span class="hljs-attr">android:name</span>=<span class="hljs-string">".MainActivity"</span>&gt;</span>

            <span class="hljs-tag">&lt;<span class="hljs-name">intent-filter</span>&gt;</span>
                <span class="hljs-tag">&lt;<span class="hljs-name">action</span> <span class="hljs-attr">android:name</span>=<span class="hljs-string">"android.intent.action.MAIN"</span> /&gt;</span>
                <span class="hljs-tag">&lt;<span class="hljs-name">category</span> <span class="hljs-attr">android:name</span>=<span class="hljs-string">"android.intent.category.LAUNCHER"</span> /&gt;</span>
            <span class="hljs-tag">&lt;/<span class="hljs-name">intent-filter</span>&gt;</span>

        <span class="hljs-tag">&lt;/<span class="hljs-name">activity</span>&gt;</span>

        //註冊Service服務
        <span class="hljs-tag">&lt;<span class="hljs-name">service</span> <span class="hljs-attr">android:name</span>=<span class="hljs-string">".MyService"</span>&gt;</span>
        <span class="hljs-tag">&lt;/<span class="hljs-name">service</span>&gt;</span>

    <span class="hljs-tag">&lt;/<span class="hljs-name">application</span>&gt;</span>

<span class="hljs-tag">&lt;/<span class="hljs-name">manifest</span>&gt;</span></code></pre>
<p><strong>Androidmanifest裡Service的常見屬性說明</strong></p>
<table>
<thead>
<tr>
<th>屬性</th>
<th style="text-align:center">說明</th>
<th style="text-align:right">備註</th>
</tr>
</thead>
<tbody>
<tr>
<td>android:name</td>
<td style="text-align:center">Service的類名</td>
<td style="text-align:right"></td>
</tr>
<tr>
<td>android:label</td>
<td style="text-align:center">Service的名字</td>
<td style="text-align:right">若不設置，默認為Service類名</td>
</tr>
<tr>
<td>android:icon</td>
<td style="text-align:center">Service的圖標</td>
<td style="text-align:right"></td>
</tr>
<tr>
<td>android:permission</td>
<td style="text-align:center">申明此Service的權限</td>
<td style="text-align:right">有提供了該權限的應用才能控制或連接此服務</td>
</tr>
<tr>
<td>android:process</td>
<td style="text-align:center">表示該服務是否在另一個進程中運行（遠程服務)</td>
<td style="text-align:right">不設置默認為本地服務；remote則設置成遠程服務</td>
</tr>
<tr>
<td>android:enabled</td>
<td style="text-align:center">系統默認啟動</td>
<td style="text-align:right">true：Service 將會默認被系統啟動；不設置則默認為false</td>
</tr>
<tr>
<td>android:exported</td>
<td style="text-align:center">該服務是否能夠被其他應用程序所控制或連接</td>
<td style="text-align:right">不設置默認此項為 false</td>
</tr>
</tbody>
</table>
<h1>2.1.3 測試結果</h1>
<div class="image-package">
<img src="images/944365-f4f659afdf0b4625.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-f4f659afdf0b4625.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">測試結果.png</div>
</div>
<h1>2.1.4 Demo地址</h1>
<p>Carson.ho的Github地址：<a href="https://github.com/Carson-Ho/Demo_Service/tree/5e2a70cf2d75c56bbfa1abc0ead16c5ad8cae83f" target="_blank">Demo_for_Service</a></p>
<h1>2.2 可通信的服務Service</h1>
<ul>
<li>上面介紹的Service是最基礎的，但只能單機使用，即無法與Activity通信</li>
<li>接下來將在上面的基礎用法上，增設“與Activity通信”的功能，即使用綁定Service服務（Binder類、bindService()、onBind(）、unbindService()、onUnbind()）</li>
</ul>
<h3>2.2.1 實例Demo</h3>
<p>接下來我將用一個實例Demo進行可通信的服務Service說明</p>
<blockquote><p>建議先下載Demo再進行閱讀：<a href="https://github.com/Carson-Ho/Demo_Service/tree/719e3b9ffd5017c334cdfdaf45b6a72776a2066a" target="_blank">（carson.ho的Github地址）Demo_for_Service</a></p></blockquote>
<ul>
<li>步驟1：在新建子類繼承Service類，並新建一個子類繼承自Binder類、寫入與Activity關聯需要的方法、創建實例</li>
</ul>
<pre class="hljs aspectj"><code class="aspectj"><span class="hljs-keyword">public</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">MyService</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">Service</span> </span>{

    <span class="hljs-keyword">private</span> MyBinder mBinder = <span class="hljs-keyword">new</span> MyBinder();

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
        <span class="hljs-comment">//返回實例</span>
        <span class="hljs-keyword">return</span> mBinder;
    }


    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">boolean</span> <span class="hljs-title">onUnbind</span><span class="hljs-params">(Intent intent)</span> </span>{
        System.out.println(<span class="hljs-string">"執行了onUnbind()"</span>);
        <span class="hljs-function"><span class="hljs-keyword">return</span> <span class="hljs-keyword">super</span>.<span class="hljs-title">onUnbind</span><span class="hljs-params">(intent)</span></span>;
    }

    <span class="hljs-comment">//新建一個子類繼承自Binder類</span>
    <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">MyBinder</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">Binder</span> </span>{

        <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">service_connect_Activity</span><span class="hljs-params">()</span> </span>{
            System.out.println(<span class="hljs-string">"Service關聯了Activity,並在Activity執行了Service的方法"</span>);

        }
    }
}</code></pre>
<ul>
<li>步驟2：在主佈局文件再設置兩個Button分別用於綁定和解綁Service</li>
</ul>
<pre class="hljs applescript"><code class="applescript">&lt;?xml <span class="hljs-built_in">version</span>=<span class="hljs-string">"1.0"</span> encoding=<span class="hljs-string">"utf-8"</span>?&gt;
&lt;RelativeLayout xmlns:android=<span class="hljs-string">"http://schemas.android.com/apk/res/android"</span>
    xmlns:tools=<span class="hljs-string">"http://schemas.android.com/tools"</span>
    android:layout_width=<span class="hljs-string">"match_parent"</span>
    android:layout_height=<span class="hljs-string">"match_parent"</span>
    android:paddingBottom=<span class="hljs-string">"@dimen/activity_vertical_margin"</span>
    android:paddingLeft=<span class="hljs-string">"@dimen/activity_horizontal_margin"</span>
    android:paddingRight=<span class="hljs-string">"@dimen/activity_horizontal_margin"</span>
    android:paddingTop=<span class="hljs-string">"@dimen/activity_vertical_margin"</span>
    tools:context=<span class="hljs-string">"scut.carson_ho.demo_service.MainActivity"</span>&gt;

    &lt;Button
        android:layout_centerInParent=<span class="hljs-string">"true"</span>
        android:<span class="hljs-built_in">id</span>=<span class="hljs-string">"@+id/startService"</span>
        android:layout_width=<span class="hljs-string">"wrap_content"</span>
        android:layout_height=<span class="hljs-string">"wrap_content"</span>
        android:<span class="hljs-built_in">text</span>=<span class="hljs-string">"啟動服務"</span> /&gt;

    &lt;Button
        android:layout_centerInParent=<span class="hljs-string">"true"</span>
        android:layout_below=<span class="hljs-string">"@+id/startService"</span>
        android:<span class="hljs-built_in">id</span>=<span class="hljs-string">"@+id/stopService"</span>
        android:layout_width=<span class="hljs-string">"wrap_content"</span>
        android:layout_height=<span class="hljs-string">"wrap_content"</span>
        android:<span class="hljs-built_in">text</span>=<span class="hljs-string">"停止服務"</span> /&gt;

    &lt;Button
        android:layout_centerInParent=<span class="hljs-string">"true"</span>
        android:layout_below=<span class="hljs-string">"@id/stopService"</span>
        android:<span class="hljs-built_in">id</span>=<span class="hljs-string">"@+id/bindService"</span>
        android:layout_width=<span class="hljs-string">"wrap_content"</span>
        android:layout_height=<span class="hljs-string">"wrap_content"</span>
        android:<span class="hljs-built_in">text</span>=<span class="hljs-string">"綁定服務"</span> /&gt;

    &lt;Button
        android:layout_centerInParent=<span class="hljs-string">"true"</span>
        android:layout_below=<span class="hljs-string">"@id/bindService"</span>
        android:<span class="hljs-built_in">id</span>=<span class="hljs-string">"@+id/unbindService"</span>
        android:layout_width=<span class="hljs-string">"wrap_content"</span>
        android:layout_height=<span class="hljs-string">"wrap_content"</span>
        android:<span class="hljs-built_in">text</span>=<span class="hljs-string">"解綁服務"</span>
        /&gt;
&lt;/RelativeLayout&gt;</code></pre>
<ul>
<li>步驟3：在Activity通過調用MyBinder類中的public方法來實現Activity與Service的聯繫<blockquote><p>即實現了Activity指揮Service幹什麼Service就去幹什麼的功能</p></blockquote>
</li>
</ul>
<p><em>MainActivity.java</em></p>
<pre class="hljs groovy"><code class="groovy"><span class="hljs-keyword">public</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">MainActivity</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">AppCompatActivity</span> <span class="hljs-keyword">implements</span> <span class="hljs-title">View</span>.<span class="hljs-title">OnClickListener</span> {</span>

    <span class="hljs-keyword">private</span> Button startService;
    <span class="hljs-keyword">private</span> Button stopService;
    <span class="hljs-keyword">private</span> Button bindService;
    <span class="hljs-keyword">private</span> Button unbindService;

    <span class="hljs-keyword">private</span> MyService.MyBinder myBinder;


    <span class="hljs-comment">//創建ServiceConnection的匿名類</span>
    <span class="hljs-keyword">private</span> ServiceConnection connection = <span class="hljs-keyword">new</span> ServiceConnection() {

        <span class="hljs-comment">//重寫onServiceConnected()方法和onServiceDisconnected()方法</span>
        <span class="hljs-comment">//在Activity與Service建立關聯和解除關聯的時候調用</span>
        <span class="hljs-meta">@Override</span>
        <span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> onServiceDisconnected(ComponentName name) {
        }

        <span class="hljs-comment">//在Activity與Service解除關聯的時候調用</span>
        <span class="hljs-meta">@Override</span>
        <span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> onServiceConnected(ComponentName name, IBinder service) {
            <span class="hljs-comment">//實例化Service的內部類myBinder</span>
            <span class="hljs-comment">//通過向下轉型得到了MyBinder的實例</span>
            myBinder = (MyService.MyBinder) service;
            <span class="hljs-comment">//在Activity調用Service類的方法</span>
            myBinder.service_connect_Activity();
        }
    };

    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">protected</span> <span class="hljs-keyword">void</span> onCreate(Bundle savedInstanceState) {
        <span class="hljs-keyword">super</span>.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        startService = (Button) findViewById(R.id.startService);
        stopService = (Button) findViewById(R.id.stopService);

        startService.setOnClickListener(<span class="hljs-keyword">this</span>);
        stopService.setOnClickListener(<span class="hljs-keyword">this</span>);

        bindService = (Button) findViewById(R.id.bindService);
        unbindService = (Button) findViewById(R.id.unbindService);

        bindService.setOnClickListener(<span class="hljs-keyword">this</span>);
        unbindService.setOnClickListener(<span class="hljs-keyword">this</span>);

    }

    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> onClick(View v) {
        <span class="hljs-keyword">switch</span> (v.getId()) {

            <span class="hljs-comment">//點擊啟動Service</span>
            <span class="hljs-keyword">case</span> R.id.<span class="hljs-string">startService:</span>
                <span class="hljs-comment">//構建啟動服務的Intent對象</span>
                Intent startIntent = <span class="hljs-keyword">new</span> Intent(<span class="hljs-keyword">this</span>, MyService.<span class="hljs-keyword">class</span>);
                <span class="hljs-comment">//調用startService()方法-傳入Intent對象,以此啟動服務</span>
                startService(startIntent);
                <span class="hljs-keyword">break</span>;

            <span class="hljs-comment">//點擊停止Service</span>
            <span class="hljs-keyword">case</span> R.id.<span class="hljs-string">stopService:</span>
                <span class="hljs-comment">//構建停止服務的Intent對象</span>
                Intent stopIntent = <span class="hljs-keyword">new</span> Intent(<span class="hljs-keyword">this</span>, MyService.<span class="hljs-keyword">class</span>);
                <span class="hljs-comment">//調用stopService()方法-傳入Intent對象,以此停止服務</span>
                stopService(stopIntent);
                <span class="hljs-keyword">break</span>;

            <span class="hljs-comment">//點擊綁定Service</span>
            <span class="hljs-keyword">case</span> R.id.<span class="hljs-string">bindService:</span>
                <span class="hljs-comment">//構建綁定服務的Intent對象</span>
                Intent bindIntent = <span class="hljs-keyword">new</span> Intent(<span class="hljs-keyword">this</span>, MyService.<span class="hljs-keyword">class</span>);
                <span class="hljs-comment">//調用bindService()方法,以此停止服務</span>

                bindService(bindIntent,connection,BIND_AUTO_CREATE);
                <span class="hljs-comment">//參數說明</span>
                <span class="hljs-comment">//第一個參數:Intent對象</span>
                <span class="hljs-comment">//第二個參數:上面創建的Serviceconnection實例</span>
                <span class="hljs-comment">//第三個參數:標誌位</span>
                <span class="hljs-comment">//這裡傳入BIND_AUTO_CREATE表示在Activity和Service建立關聯後自動創建Service</span>
                <span class="hljs-comment">//這會使得MyService中的onCreate()方法得到執行，但onStartCommand()方法不會執行</span>
                <span class="hljs-keyword">break</span>;

            <span class="hljs-comment">//點擊解綁Service</span>
            <span class="hljs-keyword">case</span> R.id.<span class="hljs-string">unbindService:</span>
                <span class="hljs-comment">//調用unbindService()解綁服務</span>
                <span class="hljs-comment">//參數是上面創建的Serviceconnection實例</span>
                unbindService(connection);
                <span class="hljs-keyword">break</span>;
<span class="hljs-symbol">
                default:</span>
                    <span class="hljs-keyword">break</span>;

        }
    }
}</code></pre>
<h3>2.2.2 測試結果</h3>
<div class="image-package">
<img src="images/944365-31165a1f2064a06a.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-31165a1f2064a06a.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">測試結果11.png</div>
</div>
<h3>2.2.3 Demo</h3>
<p>carson.ho的Github地址：<a href="https://github.com/Carson-Ho/Demo_Service/tree/719e3b9ffd5017c334cdfdaf45b6a72776a2066a" target="_blank">Demo_for_Service</a></p>
<h1>2.3 前臺Service</h1>
<p>前臺Service和後臺Service（普通）最大的區別就在於：</p>
<ul>
<li>前臺Service在下拉通知欄有顯示通知（如下圖），但後臺Service沒有；</li>
</ul>
<div class="image-package">
<img src="images/944365-95cd200fe14bac8e.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-95cd200fe14bac8e.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">TT9$TN8IK1SAPDT%~0IRLS2.png</div>
</div>
<ul>
<li>前臺Service優先級較高，不會由於系統內存不足而被回收；後臺Service優先級較低，當系統出現內存不足情況時，很有可能會被回收</li>
</ul>
<h1>2.3.1 具體使用</h1>
<p>用法很簡單，只需要在原有的Service類對onCreate()方法進行稍微修改即可，如下圖：</p>
<pre class="hljs d"><code class="d"><span class="hljs-keyword">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> onCreate() {
        <span class="hljs-keyword">super</span>.onCreate();
        System.<span class="hljs-keyword">out</span>.println(<span class="hljs-string">"執行了onCreat()"</span>);

        <span class="hljs-comment">//添加下列代碼將後臺Service變成前臺Service</span>
        <span class="hljs-comment">//構建"點擊通知後打開MainActivity"的Intent對象</span>
        Intent notificationIntent = <span class="hljs-keyword">new</span> Intent(<span class="hljs-keyword">this</span>,MainActivity.<span class="hljs-keyword">class</span>);
        PendingIntent pendingIntent = PendingIntent.getActivity(<span class="hljs-keyword">this</span>,<span class="hljs-number">0</span>,notificationIntent,<span class="hljs-number">0</span>);

        <span class="hljs-comment">//新建Builer對象</span>
        Notification.Builder builer = <span class="hljs-keyword">new</span> Notification.Builder(<span class="hljs-keyword">this</span>);
        builer.setContentTitle(<span class="hljs-string">"前臺服務通知的標題"</span>);<span class="hljs-comment">//設置通知的標題</span>
        builer.setContentText(<span class="hljs-string">"前臺服務通知的內容"</span>);<span class="hljs-comment">//設置通知的內容</span>
        builer.setSmallIcon(R.mipmap.ic_launcher);<span class="hljs-comment">//設置通知的圖標</span>
        builer.setContentIntent(pendingIntent);<span class="hljs-comment">//設置點擊通知後的操作</span>

        Notification notification = builer.getNotification();<span class="hljs-comment">//將Builder對象轉變成普通的notification</span>
        startForeground(<span class="hljs-number">1</span>, notification);<span class="hljs-comment">//讓Service變成前臺Service,並在系統的狀態欄顯示出來</span>

    }</code></pre>
<h1>2.3.2 測試結果</h1>
<p>運行後，當點擊Start Service或Bind Service按鈕，Service就會以前臺Service的模式啟動（通知欄上有通知），如下圖<br></p><div class="image-package">
<img src="images/944365-caa89c41afed8e78.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-caa89c41afed8e78.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">點擊啟動服務</div>
</div>
<h1>2.4 遠程Service</h1>
<p>具體請看我寫的另外一篇文章：<a href="Servicehttp://www.jianshu.com/p/34326751b2c6" target="_blank">Android：遠程服務Service（含AIDL &amp; IPC講解）</a></p>
<h1>3. 使用場景</h1>
<ul>
<li>通過上述描述，你應該對Service類型及其使用非常瞭解；</li>
<li>那麼，我們該什麼時候用哪種類型的Service呢？</li>
<li>各種Service的使用場景請看下圖：<br><div class="image-package">
<img src="images/944365-8a3cef8a174ae4b8.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-8a3cef8a174ae4b8.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">使用場景</div>
</div>
</li>
</ul>
<h1>4. 總結</h1>
<ul>
<li>本文對Service的使用進行了全面解析（本地、可通信、前臺和遠程Service）</li>
<li>如果你還想了解關於Service的其他知識，請瀏覽以下文章：<blockquote><p><a href="http://www.jianshu.com/p/d963c55c3ab9" target="_blank">Android四大組件：Service史上最全面解析</a><br><a href="http://www.jianshu.com/p/8d0cde35eb10" target="_blank">Android：Service生命週期最全面解析</a><br><a href="http://www.jianshu.com/p/e04c4239b07e" target="_blank">Android：（本地、可通信的、前臺、遠程）Service使用全面介紹</a><br><a href="http://www.jianshu.com/p/34326751b2c6" target="_blank">Android：遠程服務Service（含AIDL &amp; IPC講解）</a><br><a href="http://www.jianshu.com/p/8a3c44a9173a" target="_blank">Android多線程全面解析：IntentService用法&amp;源碼</a></p></blockquote>
</li>
</ul>
<ul>
<li>接下來會介紹繼續介紹Android開發中的相關知識，有興趣可以繼續關注<a href="http://www.jianshu.com/users/383970bef0a0/latest_articles" target="_blank">Carson_Ho的安卓開發筆記</a>
</li>
</ul>
<hr>
<h4>請點贊！因為你們的贊同/鼓勵是我寫作的最大動力！</h4>
<blockquote><p><strong>相關文章閱讀</strong><br><a href="http://www.jianshu.com/p/ec5a1a30694b" target="_blank">Android開發：最全面、最易懂的Android屏幕適配解決方案</a><br><a href="http://www.jianshu.com/p/9fe944ee02f7" target="_blank">Android開發：Handler異步通信機制全面解析（包含Looper、Message Queue）</a><br><a href="http://www.jianshu.com/p/3c94ae673e2a" target="_blank">Android開發：最全面、最易懂的Webview詳解</a><br><a href="http://www.jianshu.com/p/b87fee2f7a23" target="_blank">Android開發：JSON簡介及最全面解析方法!</a><br><a href="http://www.jianshu.com/p/ce1d060573ba" target="_blank">Android開發：頂部Tab導航欄實現（TabLayout+ViewPager+Fragment）</a><br><a href="http://www.jianshu.com/p/a663803b2a44" target="_blank">Android開發：底部Tab菜單欄實現（FragmentTabHost+ViewPager）</a></p></blockquote>
<hr>
<h3>歡迎關注<a href="http://www.jianshu.com/users/383970bef0a0/latest_articles" target="_blank">Carson_Ho</a>的簡書！</h3>
<p>不定期分享關於<strong>安卓開發</strong>的乾貨，追求<strong>短、平、快</strong>，但<strong>卻不缺深度</strong>。</p>
<div class="image-package">
<img src="images/944365-9b76fa3c52d478a7.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-9b76fa3c52d478a7.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption"></div>
</div>

        </div>