# Android多线程全面解析：IntentService用法&源码


<div class="show-content">
          <div class="image-package">
<img src="images/944365-207a738cb165a2da.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-207a738cb165a2da.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption"></div>
</div>
<hr>
<h1>前言</h1>
<ul>
<li>
<p>多线程的应用在Android开发中是非常常见的，常用方法主要有：</p>
<ol>
<li>继承Thread类</li>
<li>实现Runnable接口</li>
<li>AsyncTask</li>
<li>Handler</li>
<li>HandlerThread</li>
<li>IntentService</li>
</ol>
</li>
<li>
<p>今天，我将全面解析多线程其中一种常见用法：IntentService</p>
</li>
</ul>
<hr>
<h1>目录</h1>
<div class="image-package">
<img src="images/944365-e6645ea074978ab1.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-e6645ea074978ab1.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">目录</div>
</div>
<hr>
<h1>1. 定义</h1>
<p>IntentService是Android里面的一个封装类，继承自四大组件之一的Service。</p>
<hr>
<h1>2. 作用</h1>
<p>处理异步请求，实现多线程</p>
<hr>
<h1>3. 工作流程</h1>
<div class="image-package">
<img src="images/944365-fa5bfe6dffa531ce.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-fa5bfe6dffa531ce.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">工作流程</div>
</div>
<p>注意：若启动IntentService 多次，那么每个耗时操作则以队列的方式在 IntentService的onHandleIntent回调方法中依次执行，执行完自动结束。</p>
<hr>
<h1>4. 实现步骤</h1>
<ul>
<li>步骤1：定义IntentService的子类：传入线程名称、复写onHandleIntent()方法</li>
<li>步骤2：在Manifest.xml中注册服务</li>
<li>步骤3：在Activity中开启Service服务</li>
</ul>
<h1>5. 具体实例</h1>
<ul>
<li>步骤1：定义IntentService的子类：传入线程名称、复写onHandleIntent()方法</li>
</ul>
<pre class="hljs aspectj"><code class="aspectj"><span class="hljs-keyword">package</span> com.example.carson_ho.demoforintentservice;

<span class="hljs-keyword">import</span> android.app.IntentService;
<span class="hljs-keyword">import</span> android.content.Intent;
<span class="hljs-keyword">import</span> android.util.Log;

<span class="hljs-comment">/**
 * Created by Carson_Ho on 16/9/28.
 */</span>
<span class="hljs-keyword">public</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">myIntentService</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">IntentService</span> </span>{

    <span class="hljs-comment">/*构造函数*/</span>
    <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-title">myIntentService</span><span class="hljs-params">()</span> </span>{
        <span class="hljs-comment">//调用父类的构造函数</span>
        <span class="hljs-comment">//构造函数参数=工作线程的名字</span>
        <span class="hljs-keyword">super</span>(<span class="hljs-string">"myIntentService"</span>);

    }

    <span class="hljs-comment">/*复写onHandleIntent()方法*/</span>
    <span class="hljs-comment">//实现耗时任务的操作</span>
    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">protected</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">onHandleIntent</span><span class="hljs-params">(Intent intent)</span> </span>{
        <span class="hljs-comment">//根据Intent的不同进行不同的事务处理</span>
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

    <span class="hljs-comment">/*复写onStartCommand()方法*/</span>
    <span class="hljs-comment">//默认实现将请求的Intent添加到工作队列里</span>
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
<li>步骤2：在Manifest.xml中注册服务</li>
</ul>
<pre class="hljs xml"><code class="xml"><span class="hljs-tag">&lt;<span class="hljs-name">service</span> <span class="hljs-attr">android:name</span>=<span class="hljs-string">".myIntentService"</span>&gt;</span>
            <span class="hljs-tag">&lt;<span class="hljs-name">intent-filter</span> &gt;</span>
                <span class="hljs-tag">&lt;<span class="hljs-name">action</span> <span class="hljs-attr">android:name</span>=<span class="hljs-string">"cn.scu.finch"</span>/&gt;</span>
            <span class="hljs-tag">&lt;/<span class="hljs-name">intent-filter</span>&gt;</span>
        <span class="hljs-tag">&lt;/<span class="hljs-name">service</span>&gt;</span></code></pre>
<ul>
<li>步骤3：在Activity中开启Service服务</li>
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

            <span class="hljs-comment">//同一服务只会开启一个工作线程</span>
            <span class="hljs-comment">//在onHandleIntent函数里依次处理intent请求。</span>

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

            startService(i);  <span class="hljs-comment">//多次启动</span>
        }
    }</code></pre>
<ul>
<li>结果<br><div class="image-package">
<img src="images/944365-fadf671e3671b52a.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-fadf671e3671b52a.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">运行结果</div>
</div>
</li>
</ul>
<h1>6. 源码分析</h1>
<p>接下来，我们会通过源码分析解决以下问题：</p>
<ul>
<li>IntentService如何单独开启一个新的工作线程；</li>
<li>IntentService如何通过onStartCommand()传递给服务intent被<strong>依次</strong>插入到工作队列中</li>
</ul>
<p>问题1：IntentService如何单独开启一个新的工作线程</p>
<pre class="hljs java"><code class="java"><span class="hljs-comment">// IntentService源码中的 onCreate() 方法</span>
<span class="hljs-meta">@Override</span>
<span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">onCreate</span><span class="hljs-params">()</span> </span>{
    <span class="hljs-keyword">super</span>.onCreate();
    <span class="hljs-comment">// HandlerThread继承自Thread，内部封装了 Looper</span>
    <span class="hljs-comment">//通过实例化andlerThread新建线程并启动</span>
    <span class="hljs-comment">//所以使用IntentService时不需要额外新建线程</span>
    HandlerThread thread = <span class="hljs-keyword">new</span> HandlerThread(<span class="hljs-string">"IntentService["</span> + mName + <span class="hljs-string">"]"</span>);
    thread.start();

    <span class="hljs-comment">//获得工作线程的 Looper，并维护自己的工作队列</span>
    mServiceLooper = thread.getLooper();
    <span class="hljs-comment">//将上述获得Looper与新建的mServiceHandler进行绑定</span>
    <span class="hljs-comment">//新建的Handler是属于工作线程的。</span>
    mServiceHandler = <span class="hljs-keyword">new</span> ServiceHandler(mServiceLooper);
}

<span class="hljs-keyword">private</span> <span class="hljs-keyword">final</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">ServiceHandler</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">Handler</span> </span>{
    <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-title">ServiceHandler</span><span class="hljs-params">(Looper looper)</span> </span>{
        <span class="hljs-keyword">super</span>(looper);
    }

<span class="hljs-comment">//IntentService的handleMessage方法把接收的消息交给onHandleIntent()处理</span>
<span class="hljs-comment">//onHandleIntent()是一个抽象方法，使用时需要重写的方法</span>
    <span class="hljs-meta">@Override</span>
    <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">handleMessage</span><span class="hljs-params">(Message msg)</span> </span>{
        <span class="hljs-comment">// onHandleIntent 方法在工作线程中执行，执行完调用 stopSelf() 结束服务。</span>
        onHandleIntent((Intent)msg.obj);
      <span class="hljs-comment">//onHandleIntent 处理完成后 IntentService会调用 stopSelf() 自动停止。</span>
        stopSelf(msg.arg1);
    }
}

<span class="hljs-comment">////onHandleIntent()是一个抽象方法，使用时需要重写的方法</span>
<span class="hljs-meta">@WorkerThread</span>
<span class="hljs-function"><span class="hljs-keyword">protected</span> <span class="hljs-keyword">abstract</span> <span class="hljs-keyword">void</span> <span class="hljs-title">onHandleIntent</span><span class="hljs-params">(Intent intent)</span></span>;</code></pre>
<p><strong>问题2：IntentService如何通过onStartCommand()传递给服务intent被</strong>依次<strong>插入到工作队列中</strong></p>
<pre class="hljs fortran"><code class="fortran"><span class="hljs-keyword">public</span> <span class="hljs-built_in">int</span> onStartCommand(<span class="hljs-keyword">Intent</span> <span class="hljs-keyword">intent</span>, <span class="hljs-built_in">int</span> flags, <span class="hljs-built_in">int</span> startId) {
    onStart(<span class="hljs-keyword">intent</span>, startId);
    <span class="hljs-keyword">return</span> mRedelivery ? START_REDELIVER_INTENT : START_NOT_STICKY;
}

<span class="hljs-keyword">public</span> void onStart(<span class="hljs-keyword">Intent</span> <span class="hljs-keyword">intent</span>, <span class="hljs-built_in">int</span> startId) {
    Message msg = mServiceHandler.obtainMessage();
    msg.arg1 = startId;
//把 <span class="hljs-keyword">intent</span> 参数包装到 message 的 obj 中，然后发送消息，即添加到消息队列里
//这里的<span class="hljs-keyword">Intent</span> 就是启动服务时startService(<span class="hljs-keyword">Intent</span>) 里的 <span class="hljs-keyword">Intent</span>。
    msg.obj = <span class="hljs-keyword">intent</span>;
    mServiceHandler.sendMessage(msg);
}

//清除消息队列中的消息
@Override
<span class="hljs-keyword">public</span> void onDestroy() {
    mServiceLooper.quit();
}</code></pre>
<ul>
<li>总结<br>从上面源码可以看出，IntentService本质是采用Handler &amp; HandlerThread方式：<ol>
<li>通过HandlerThread单独开启一个名为<strong>IntentService</strong>的线程</li>
<li>创建一个名叫ServiceHandler的内部Handler</li>
<li>把内部Handler与HandlerThread所对应的子线程进行绑定</li>
<li>通过onStartCommand()传递给服务intent，<strong>依次</strong>插入到工作队列中，并逐个发送给onHandleIntent()</li>
<li>通过onHandleIntent()来依次处理所有Intent请求对象所对应的任务</li>
</ol>
</li>
</ul>
<p>因此我们通过复写方法onHandleIntent()，再在里面根据Intent的不同进行不同的线程操作就可以了</p>
<p><strong>注意事项1. 工作任务队列是顺序执行的。</strong></p>
<blockquote><p>如果一个任务正在IntentService中执行，此时你再发送一个新的任务请求，这个新的任务会一直等待直到前面一个任务执行完毕才开始执行</p></blockquote>
<p>  原因：</p>
<ol>
<li>由于onCreate() 方法只会调用一次，所以只会创建一个工作线程；</li>
<li>当多次调用 startService(Intent) 时（onStartCommand也会调用多次）其实并不会创建新的工作线程，只是把消息加入消息队列中等待执行，<strong>所以，多次启动 IntentService 会按顺序执行事件</strong>
</li>
<li>如果服务停止，会清除消息队列中的消息，后续的事件得不到执行。</li>
</ol>
<p><strong>注意事项2：不建议通过 bindService() 启动 IntentService</strong><br>原因：</p>
<pre class="hljs aspectj"><code class="aspectj"><span class="hljs-meta">@Override</span>
<span class="hljs-keyword">public</span> <span class="hljs-function">IBinder <span class="hljs-title">onBind</span><span class="hljs-params">(Intent intent)</span> </span>{
    <span class="hljs-keyword">return</span> <span class="hljs-keyword">null</span>;
}</code></pre>
<p>在IntentService中，onBind()是默认返回null的，而采用bindService() 启动 IntentService的生命周期是：<strong>onCreate() —&gt;onBind()—&gt;onunbind()—&gt;onDestory()</strong><br>并不会调用onstart()或者onstartcommand()方法，所以不会将消息发送到消息队列，那么onHandleIntent()将不会回调，即无法实现多线程的操作。</p>
<blockquote><p>此时，你使用的是Service，而不是IntentService</p></blockquote>
<h1>7. 使用场景</h1>
<ul>
<li>线程任务需要<strong>按顺序</strong>、<strong>在后台执行</strong>的使用场景<blockquote><p>最常见的场景：离线下载</p></blockquote>
</li>
<li>由于所有的任务都在同一个Thread looper里面来做，所以不符合多个数据同时请求的场景。</li>
</ul>
<h1>8. 对比</h1>
<h3>8.1 IntentService与Service的区别</h3>
<ul>
<li>
<p>从属性 &amp; 作用上来说<br>Service：依赖于应用程序的主线程（不是独立的进程 or 线程）</p>
<blockquote><p>不建议在Service中编写耗时的逻辑和操作，否则会引起ANR；</p></blockquote>
<p>IntentService：创建一个工作线程来处理多线程任务 　　</p>
</li>
<li>Service需要主动调用stopSelft()来结束服务，而IntentService不需要（在所有intent被处理完后，系统会自动关闭服务）<blockquote>
<p>此外：</p>
<ol>
<li>IntentService为Service的onBingd()方式提供了默认实现：返回null</li>
<li>IntentService为Service的onStartCommand（）方法提供了默认实现：将请求的intent添加到队列中</li>
</ol>
</blockquote>
</li>
</ul>
<h3>8.2 IntentService与其他线程的区别</h3>
<ul>
<li>IntentService内部采用了HandlerThread实现，作用类似于后台线程；</li>
<li>与后台线程相比，<strong>IntentService是一种后台服务</strong>，优势是：优先级高（不容易被系统杀死），从而保证任务的执行<blockquote><p>对于后台线程，若进程中没有活动的四大组件，则该线程的优先级非常低，容易被系统杀死，无法保证任务的执行</p></blockquote>
</li>
</ul>
<h1>9. 总结</h1>
<ul>
<li>本文主要对多线程IntentService用法&amp;源码进行了全面介绍</li>
<li>接下来，我会继续讲解Android开发中关于多线程的知识，包括继承Thread类、实现Runnable接口、Handler等等，有兴趣可以继续关注<a href="http://www.jianshu.com/users/383970bef0a0/latest_articles" target="_blank">Carson_Ho的安卓开发笔记</a>
</li>
</ul>
<hr>
<h1>请点赞！因为你的鼓励是我写作的最大动力！</h1>
<blockquote><p><strong>相关文章阅读</strong><br><a href="http://www.jianshu.com/p/6e5eda3a51af" target="_blank">1分钟全面了解“设计模式”</a><br><a href="http://www.jianshu.com/p/ec5a1a30694b" target="_blank">Android开发：最全面、最易懂的Android屏幕适配解决方案</a><br><a href="http://www.jianshu.com/p/9fe944ee02f7" target="_blank">Android开发：Handler异步通信机制全面解析（包含Looper、Message Queue）</a><br><a href="http://www.jianshu.com/p/ce1d060573ba" target="_blank">Android开发：顶部Tab导航栏实现（TabLayout+ViewPager+Fragment）</a><br><a href="http://www.jianshu.com/p/a663803b2a44" target="_blank">Android开发：底部Tab菜单栏实现（FragmentTabHost+ViewPager）</a><br><a href="http://www.jianshu.com/p/b87fee2f7a23" target="_blank">Android开发：JSON简介及最全面解析方法!</a><br><a href="http://www.jianshu.com/p/e636f4f8487b" target="_blank">Android开发：XML简介及DOM、SAX、PULL解析对比</a></p></blockquote>
<hr>
<h3>欢迎关注<a href="http://www.jianshu.com/users/383970bef0a0/latest_articles" target="_blank">Carson_Ho</a>的简书！</h3>
<p>不定期分享关于<strong>安卓开发</strong>的干货，追求<strong>短、平、快</strong>，但<strong>却不缺深度</strong>。<br></p><div class="image-package">
<img src="images/944365-9b76fa3c52d478a7.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-9b76fa3c52d478a7.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption"></div>
</div>

        </div>