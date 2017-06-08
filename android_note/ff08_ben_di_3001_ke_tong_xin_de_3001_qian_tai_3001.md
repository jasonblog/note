# （本地、可通信的、前台、远程）Service使用全面介绍


<hr>
<h1>前言</h1>
<ul>
<li>Service作为Android四大组件之一，应用非常广泛</li>
<li>本文将介绍Service最基础的知识：Service的生命周期<blockquote><p>如果你对Service还未了解，建议先阅读我写的文章：<br><a href="http://www.jianshu.com/p/d963c55c3ab9" target="_blank">Android四大组件：Service史上最全面解析</a></p></blockquote>
</li>
</ul>
<hr>
<h1>目录</h1>
<div class="image-package">
<img src="images/944365-db081c31bd7b64c9.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-db081c31bd7b64c9.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">目录</div>
</div>
<hr>
<h1>1. Service分类</h1>
<h3>1.1 Service的类型</h3>
<div class="image-package">
<img src="images/944365-d42fa78828930bdb.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-d42fa78828930bdb.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">分类</div>
</div>
<h3>1.2  特点</h3>
<div class="image-package">
<img src="images/944365-8855e3a5340bece5.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-8855e3a5340bece5.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">Service类型的详细介绍</div>
</div>
<hr>
<h1>2.具体使用解析</h1>
<h3>2.1 本地Service</h3>
<p>这是最普通、最常用的后台服务Service。</p>
<h3>2.1.1 使用步骤</h3>
<ul>
<li>步骤1：新建子类继承Service类<blockquote><p>需重写父类的onCreate()、onStartCommand()、onDestroy()和onBind()方法</p></blockquote>
</li>
<li>步骤2：构建用于启动Service的Intent对象</li>
<li>步骤3：调用startService()启动Service、调用stopService()停止服务</li>
<li>步骤4：在AndroidManifest.xml里注册Service</li>
</ul>
<h3>2.1.2 实例Demo</h3>
<p>接下来我将用一个实例Demo进行本地Service说明</p>
<blockquote><p>建议先下载Demo再进行阅读：<a href="https://github.com/Carson-Ho/Demo_Service/tree/5e2a70cf2d75c56bbfa1abc0ead16c5ad8cae83f" target="_blank">（carson.ho的Github地址）Demo_for_Service</a></p></blockquote>
<ul>
<li>步骤1：新建子类继承Service类<blockquote><p>需重写父类的onCreate()、onStartCommand()、onDestroy()和onBind()</p></blockquote>
</li>
</ul>
<p><em>MyService.java</em></p>
<pre class="hljs aspectj"><code class="aspectj"><span class="hljs-keyword">public</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">MyService</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">Service</span> </span>{


<span class="hljs-comment">//启动Service之后，就可以在onCreate()或onStartCommand()方法里去执行一些具体的逻辑</span>
<span class="hljs-comment">//由于这里作Demo用，所以只打印一些语句</span>
    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">onCreate</span><span class="hljs-params">()</span> </span>{
        <span class="hljs-keyword">super</span>.onCreate();
        System.out.println(<span class="hljs-string">"执行了onCreat()"</span>);
    }

    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">int</span> <span class="hljs-title">onStartCommand</span><span class="hljs-params">(Intent intent, <span class="hljs-keyword">int</span> flags, <span class="hljs-keyword">int</span> startId)</span> </span>{
        System.out.println(<span class="hljs-string">"执行了onStartCommand()"</span>);
        <span class="hljs-function"><span class="hljs-keyword">return</span> <span class="hljs-keyword">super</span>.<span class="hljs-title">onStartCommand</span><span class="hljs-params">(intent, flags, startId)</span></span>;


    }

    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">onDestroy</span><span class="hljs-params">()</span> </span>{
        <span class="hljs-keyword">super</span>.onDestroy();
        System.out.println(<span class="hljs-string">"执行了onDestory()"</span>);
    }

    <span class="hljs-meta">@Nullable</span>
    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-function">IBinder <span class="hljs-title">onBind</span><span class="hljs-params">(Intent intent)</span> </span>{
        <span class="hljs-keyword">return</span> <span class="hljs-keyword">null</span>;
    }
}</code></pre>
<ul>
<li>步骤2：在主布局文件设置两个Button分别用于启动和停止Service<br><em>activity_main.xml</em>
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
        <span class="hljs-attr">android:text</span>=<span class="hljs-string">"启动服务"</span> /&gt;</span>

    <span class="hljs-tag">&lt;<span class="hljs-name">Button</span>
        <span class="hljs-attr">android:layout_centerInParent</span>=<span class="hljs-string">"true"</span>
        <span class="hljs-attr">android:layout_below</span>=<span class="hljs-string">"@+id/startService"</span>
        <span class="hljs-attr">android:id</span>=<span class="hljs-string">"@+id/stopService"</span>
        <span class="hljs-attr">android:layout_width</span>=<span class="hljs-string">"wrap_content"</span>
        <span class="hljs-attr">android:layout_height</span>=<span class="hljs-string">"wrap_content"</span>
        <span class="hljs-attr">android:text</span>=<span class="hljs-string">"停止服务"</span> /&gt;</span>
<span class="hljs-tag">&lt;/<span class="hljs-name">RelativeLayout</span>&gt;</span></code></pre>
<ul>
<li>步骤3：构建Intent对象，并调用startService()启动Service、stopService停止服务</li>
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

            <span class="hljs-comment">//点击启动Service Button</span>
            <span class="hljs-keyword">case</span> R.id.<span class="hljs-string">startService:</span>
                <span class="hljs-comment">//构建启动服务的Intent对象</span>
                Intent startIntent = <span class="hljs-keyword">new</span> Intent(<span class="hljs-keyword">this</span>, MyService.<span class="hljs-keyword">class</span>);
                <span class="hljs-comment">//调用startService()方法-传入Intent对象,以此启动服务</span>
                startService(startIntent);

            <span class="hljs-comment">//点击停止Service Button</span>
            <span class="hljs-keyword">case</span> R.id.<span class="hljs-string">stopService:</span>
                <span class="hljs-comment">//构建停止服务的Intent对象</span>
                Intent stopIntent = <span class="hljs-keyword">new</span> Intent(<span class="hljs-keyword">this</span>, MyService.<span class="hljs-keyword">class</span>);
                <span class="hljs-comment">//调用stopService()方法-传入Intent对象,以此停止服务</span>
                stopService(stopIntent);

        }
    }
}</code></pre>
<ul>
<li>步骤4：在AndroidManifest.xml里注册Service<br><em>AndroidManifest.xml</em>
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

        //注册Service服务
        <span class="hljs-tag">&lt;<span class="hljs-name">service</span> <span class="hljs-attr">android:name</span>=<span class="hljs-string">".MyService"</span>&gt;</span>
        <span class="hljs-tag">&lt;/<span class="hljs-name">service</span>&gt;</span>

    <span class="hljs-tag">&lt;/<span class="hljs-name">application</span>&gt;</span>

<span class="hljs-tag">&lt;/<span class="hljs-name">manifest</span>&gt;</span></code></pre>
<p><strong>Androidmanifest里Service的常见属性说明</strong></p>
<table>
<thead>
<tr>
<th>属性</th>
<th style="text-align:center">说明</th>
<th style="text-align:right">备注</th>
</tr>
</thead>
<tbody>
<tr>
<td>android:name</td>
<td style="text-align:center">Service的类名</td>
<td style="text-align:right"></td>
</tr>
<tr>
<td>android:label</td>
<td style="text-align:center">Service的名字</td>
<td style="text-align:right">若不设置，默认为Service类名</td>
</tr>
<tr>
<td>android:icon</td>
<td style="text-align:center">Service的图标</td>
<td style="text-align:right"></td>
</tr>
<tr>
<td>android:permission</td>
<td style="text-align:center">申明此Service的权限</td>
<td style="text-align:right">有提供了该权限的应用才能控制或连接此服务</td>
</tr>
<tr>
<td>android:process</td>
<td style="text-align:center">表示该服务是否在另一个进程中运行（远程服务)</td>
<td style="text-align:right">不设置默认为本地服务；remote则设置成远程服务</td>
</tr>
<tr>
<td>android:enabled</td>
<td style="text-align:center">系统默认启动</td>
<td style="text-align:right">true：Service 将会默认被系统启动；不设置则默认为false</td>
</tr>
<tr>
<td>android:exported</td>
<td style="text-align:center">该服务是否能够被其他应用程序所控制或连接</td>
<td style="text-align:right">不设置默认此项为 false</td>
</tr>
</tbody>
</table>
<h1>2.1.3 测试结果</h1>
<div class="image-package">
<img src="images/944365-f4f659afdf0b4625.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-f4f659afdf0b4625.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">测试结果.png</div>
</div>
<h1>2.1.4 Demo地址</h1>
<p>Carson.ho的Github地址：<a href="https://github.com/Carson-Ho/Demo_Service/tree/5e2a70cf2d75c56bbfa1abc0ead16c5ad8cae83f" target="_blank">Demo_for_Service</a></p>
<h1>2.2 可通信的服务Service</h1>
<ul>
<li>上面介绍的Service是最基础的，但只能单机使用，即无法与Activity通信</li>
<li>接下来将在上面的基础用法上，增设“与Activity通信”的功能，即使用绑定Service服务（Binder类、bindService()、onBind(）、unbindService()、onUnbind()）</li>
</ul>
<h3>2.2.1 实例Demo</h3>
<p>接下来我将用一个实例Demo进行可通信的服务Service说明</p>
<blockquote><p>建议先下载Demo再进行阅读：<a href="https://github.com/Carson-Ho/Demo_Service/tree/719e3b9ffd5017c334cdfdaf45b6a72776a2066a" target="_blank">（carson.ho的Github地址）Demo_for_Service</a></p></blockquote>
<ul>
<li>步骤1：在新建子类继承Service类，并新建一个子类继承自Binder类、写入与Activity关联需要的方法、创建实例</li>
</ul>
<pre class="hljs aspectj"><code class="aspectj"><span class="hljs-keyword">public</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">MyService</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">Service</span> </span>{

    <span class="hljs-keyword">private</span> MyBinder mBinder = <span class="hljs-keyword">new</span> MyBinder();

    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">onCreate</span><span class="hljs-params">()</span> </span>{
        <span class="hljs-keyword">super</span>.onCreate();
        System.out.println(<span class="hljs-string">"执行了onCreat()"</span>);
    }

    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">int</span> <span class="hljs-title">onStartCommand</span><span class="hljs-params">(Intent intent, <span class="hljs-keyword">int</span> flags, <span class="hljs-keyword">int</span> startId)</span> </span>{
        System.out.println(<span class="hljs-string">"执行了onStartCommand()"</span>);
        <span class="hljs-function"><span class="hljs-keyword">return</span> <span class="hljs-keyword">super</span>.<span class="hljs-title">onStartCommand</span><span class="hljs-params">(intent, flags, startId)</span></span>;


    }

    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">onDestroy</span><span class="hljs-params">()</span> </span>{
        <span class="hljs-keyword">super</span>.onDestroy();
        System.out.println(<span class="hljs-string">"执行了onDestory()"</span>);
    }

    <span class="hljs-meta">@Nullable</span>
    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-function">IBinder <span class="hljs-title">onBind</span><span class="hljs-params">(Intent intent)</span> </span>{
        System.out.println(<span class="hljs-string">"执行了onBind()"</span>);
        <span class="hljs-comment">//返回实例</span>
        <span class="hljs-keyword">return</span> mBinder;
    }


    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">boolean</span> <span class="hljs-title">onUnbind</span><span class="hljs-params">(Intent intent)</span> </span>{
        System.out.println(<span class="hljs-string">"执行了onUnbind()"</span>);
        <span class="hljs-function"><span class="hljs-keyword">return</span> <span class="hljs-keyword">super</span>.<span class="hljs-title">onUnbind</span><span class="hljs-params">(intent)</span></span>;
    }

    <span class="hljs-comment">//新建一个子类继承自Binder类</span>
    <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">MyBinder</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">Binder</span> </span>{

        <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">service_connect_Activity</span><span class="hljs-params">()</span> </span>{
            System.out.println(<span class="hljs-string">"Service关联了Activity,并在Activity执行了Service的方法"</span>);

        }
    }
}</code></pre>
<ul>
<li>步骤2：在主布局文件再设置两个Button分别用于绑定和解绑Service</li>
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
        android:<span class="hljs-built_in">text</span>=<span class="hljs-string">"启动服务"</span> /&gt;

    &lt;Button
        android:layout_centerInParent=<span class="hljs-string">"true"</span>
        android:layout_below=<span class="hljs-string">"@+id/startService"</span>
        android:<span class="hljs-built_in">id</span>=<span class="hljs-string">"@+id/stopService"</span>
        android:layout_width=<span class="hljs-string">"wrap_content"</span>
        android:layout_height=<span class="hljs-string">"wrap_content"</span>
        android:<span class="hljs-built_in">text</span>=<span class="hljs-string">"停止服务"</span> /&gt;

    &lt;Button
        android:layout_centerInParent=<span class="hljs-string">"true"</span>
        android:layout_below=<span class="hljs-string">"@id/stopService"</span>
        android:<span class="hljs-built_in">id</span>=<span class="hljs-string">"@+id/bindService"</span>
        android:layout_width=<span class="hljs-string">"wrap_content"</span>
        android:layout_height=<span class="hljs-string">"wrap_content"</span>
        android:<span class="hljs-built_in">text</span>=<span class="hljs-string">"绑定服务"</span> /&gt;

    &lt;Button
        android:layout_centerInParent=<span class="hljs-string">"true"</span>
        android:layout_below=<span class="hljs-string">"@id/bindService"</span>
        android:<span class="hljs-built_in">id</span>=<span class="hljs-string">"@+id/unbindService"</span>
        android:layout_width=<span class="hljs-string">"wrap_content"</span>
        android:layout_height=<span class="hljs-string">"wrap_content"</span>
        android:<span class="hljs-built_in">text</span>=<span class="hljs-string">"解绑服务"</span>
        /&gt;
&lt;/RelativeLayout&gt;</code></pre>
<ul>
<li>步骤3：在Activity通过调用MyBinder类中的public方法来实现Activity与Service的联系<blockquote><p>即实现了Activity指挥Service干什么Service就去干什么的功能</p></blockquote>
</li>
</ul>
<p><em>MainActivity.java</em></p>
<pre class="hljs groovy"><code class="groovy"><span class="hljs-keyword">public</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">MainActivity</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">AppCompatActivity</span> <span class="hljs-keyword">implements</span> <span class="hljs-title">View</span>.<span class="hljs-title">OnClickListener</span> {</span>

    <span class="hljs-keyword">private</span> Button startService;
    <span class="hljs-keyword">private</span> Button stopService;
    <span class="hljs-keyword">private</span> Button bindService;
    <span class="hljs-keyword">private</span> Button unbindService;

    <span class="hljs-keyword">private</span> MyService.MyBinder myBinder;


    <span class="hljs-comment">//创建ServiceConnection的匿名类</span>
    <span class="hljs-keyword">private</span> ServiceConnection connection = <span class="hljs-keyword">new</span> ServiceConnection() {

        <span class="hljs-comment">//重写onServiceConnected()方法和onServiceDisconnected()方法</span>
        <span class="hljs-comment">//在Activity与Service建立关联和解除关联的时候调用</span>
        <span class="hljs-meta">@Override</span>
        <span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> onServiceDisconnected(ComponentName name) {
        }

        <span class="hljs-comment">//在Activity与Service解除关联的时候调用</span>
        <span class="hljs-meta">@Override</span>
        <span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> onServiceConnected(ComponentName name, IBinder service) {
            <span class="hljs-comment">//实例化Service的内部类myBinder</span>
            <span class="hljs-comment">//通过向下转型得到了MyBinder的实例</span>
            myBinder = (MyService.MyBinder) service;
            <span class="hljs-comment">//在Activity调用Service类的方法</span>
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

            <span class="hljs-comment">//点击启动Service</span>
            <span class="hljs-keyword">case</span> R.id.<span class="hljs-string">startService:</span>
                <span class="hljs-comment">//构建启动服务的Intent对象</span>
                Intent startIntent = <span class="hljs-keyword">new</span> Intent(<span class="hljs-keyword">this</span>, MyService.<span class="hljs-keyword">class</span>);
                <span class="hljs-comment">//调用startService()方法-传入Intent对象,以此启动服务</span>
                startService(startIntent);
                <span class="hljs-keyword">break</span>;

            <span class="hljs-comment">//点击停止Service</span>
            <span class="hljs-keyword">case</span> R.id.<span class="hljs-string">stopService:</span>
                <span class="hljs-comment">//构建停止服务的Intent对象</span>
                Intent stopIntent = <span class="hljs-keyword">new</span> Intent(<span class="hljs-keyword">this</span>, MyService.<span class="hljs-keyword">class</span>);
                <span class="hljs-comment">//调用stopService()方法-传入Intent对象,以此停止服务</span>
                stopService(stopIntent);
                <span class="hljs-keyword">break</span>;

            <span class="hljs-comment">//点击绑定Service</span>
            <span class="hljs-keyword">case</span> R.id.<span class="hljs-string">bindService:</span>
                <span class="hljs-comment">//构建绑定服务的Intent对象</span>
                Intent bindIntent = <span class="hljs-keyword">new</span> Intent(<span class="hljs-keyword">this</span>, MyService.<span class="hljs-keyword">class</span>);
                <span class="hljs-comment">//调用bindService()方法,以此停止服务</span>

                bindService(bindIntent,connection,BIND_AUTO_CREATE);
                <span class="hljs-comment">//参数说明</span>
                <span class="hljs-comment">//第一个参数:Intent对象</span>
                <span class="hljs-comment">//第二个参数:上面创建的Serviceconnection实例</span>
                <span class="hljs-comment">//第三个参数:标志位</span>
                <span class="hljs-comment">//这里传入BIND_AUTO_CREATE表示在Activity和Service建立关联后自动创建Service</span>
                <span class="hljs-comment">//这会使得MyService中的onCreate()方法得到执行，但onStartCommand()方法不会执行</span>
                <span class="hljs-keyword">break</span>;

            <span class="hljs-comment">//点击解绑Service</span>
            <span class="hljs-keyword">case</span> R.id.<span class="hljs-string">unbindService:</span>
                <span class="hljs-comment">//调用unbindService()解绑服务</span>
                <span class="hljs-comment">//参数是上面创建的Serviceconnection实例</span>
                unbindService(connection);
                <span class="hljs-keyword">break</span>;
<span class="hljs-symbol">
                default:</span>
                    <span class="hljs-keyword">break</span>;

        }
    }
}</code></pre>
<h3>2.2.2 测试结果</h3>
<div class="image-package">
<img src="images/944365-31165a1f2064a06a.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-31165a1f2064a06a.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">测试结果11.png</div>
</div>
<h3>2.2.3 Demo</h3>
<p>carson.ho的Github地址：<a href="https://github.com/Carson-Ho/Demo_Service/tree/719e3b9ffd5017c334cdfdaf45b6a72776a2066a" target="_blank">Demo_for_Service</a></p>
<h1>2.3 前台Service</h1>
<p>前台Service和后台Service（普通）最大的区别就在于：</p>
<ul>
<li>前台Service在下拉通知栏有显示通知（如下图），但后台Service没有；</li>
</ul>
<div class="image-package">
<img src="images/944365-95cd200fe14bac8e.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-95cd200fe14bac8e.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">TT9$TN8IK1SAPDT%~0IRLS2.png</div>
</div>
<ul>
<li>前台Service优先级较高，不会由于系统内存不足而被回收；后台Service优先级较低，当系统出现内存不足情况时，很有可能会被回收</li>
</ul>
<h1>2.3.1 具体使用</h1>
<p>用法很简单，只需要在原有的Service类对onCreate()方法进行稍微修改即可，如下图：</p>
<pre class="hljs d"><code class="d"><span class="hljs-keyword">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> onCreate() {
        <span class="hljs-keyword">super</span>.onCreate();
        System.<span class="hljs-keyword">out</span>.println(<span class="hljs-string">"执行了onCreat()"</span>);

        <span class="hljs-comment">//添加下列代码将后台Service变成前台Service</span>
        <span class="hljs-comment">//构建"点击通知后打开MainActivity"的Intent对象</span>
        Intent notificationIntent = <span class="hljs-keyword">new</span> Intent(<span class="hljs-keyword">this</span>,MainActivity.<span class="hljs-keyword">class</span>);
        PendingIntent pendingIntent = PendingIntent.getActivity(<span class="hljs-keyword">this</span>,<span class="hljs-number">0</span>,notificationIntent,<span class="hljs-number">0</span>);

        <span class="hljs-comment">//新建Builer对象</span>
        Notification.Builder builer = <span class="hljs-keyword">new</span> Notification.Builder(<span class="hljs-keyword">this</span>);
        builer.setContentTitle(<span class="hljs-string">"前台服务通知的标题"</span>);<span class="hljs-comment">//设置通知的标题</span>
        builer.setContentText(<span class="hljs-string">"前台服务通知的内容"</span>);<span class="hljs-comment">//设置通知的内容</span>
        builer.setSmallIcon(R.mipmap.ic_launcher);<span class="hljs-comment">//设置通知的图标</span>
        builer.setContentIntent(pendingIntent);<span class="hljs-comment">//设置点击通知后的操作</span>

        Notification notification = builer.getNotification();<span class="hljs-comment">//将Builder对象转变成普通的notification</span>
        startForeground(<span class="hljs-number">1</span>, notification);<span class="hljs-comment">//让Service变成前台Service,并在系统的状态栏显示出来</span>

    }</code></pre>
<h1>2.3.2 测试结果</h1>
<p>运行后，当点击Start Service或Bind Service按钮，Service就会以前台Service的模式启动（通知栏上有通知），如下图<br></p><div class="image-package">
<img src="images/944365-caa89c41afed8e78.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-caa89c41afed8e78.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">点击启动服务</div>
</div>
<h1>2.4 远程Service</h1>
<p>具体请看我写的另外一篇文章：<a href="Servicehttp://www.jianshu.com/p/34326751b2c6" target="_blank">Android：远程服务Service（含AIDL &amp; IPC讲解）</a></p>
<h1>3. 使用场景</h1>
<ul>
<li>通过上述描述，你应该对Service类型及其使用非常了解；</li>
<li>那么，我们该什么时候用哪种类型的Service呢？</li>
<li>各种Service的使用场景请看下图：<br><div class="image-package">
<img src="images/944365-8a3cef8a174ae4b8.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-8a3cef8a174ae4b8.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">使用场景</div>
</div>
</li>
</ul>
<h1>4. 总结</h1>
<ul>
<li>本文对Service的使用进行了全面解析（本地、可通信、前台和远程Service）</li>
<li>如果你还想了解关于Service的其他知识，请浏览以下文章：<blockquote><p><a href="http://www.jianshu.com/p/d963c55c3ab9" target="_blank">Android四大组件：Service史上最全面解析</a><br><a href="http://www.jianshu.com/p/8d0cde35eb10" target="_blank">Android：Service生命周期最全面解析</a><br><a href="http://www.jianshu.com/p/e04c4239b07e" target="_blank">Android：（本地、可通信的、前台、远程）Service使用全面介绍</a><br><a href="http://www.jianshu.com/p/34326751b2c6" target="_blank">Android：远程服务Service（含AIDL &amp; IPC讲解）</a><br><a href="http://www.jianshu.com/p/8a3c44a9173a" target="_blank">Android多线程全面解析：IntentService用法&amp;源码</a></p></blockquote>
</li>
</ul>
<ul>
<li>接下来会介绍继续介绍Android开发中的相关知识，有兴趣可以继续关注<a href="http://www.jianshu.com/users/383970bef0a0/latest_articles" target="_blank">Carson_Ho的安卓开发笔记</a>
</li>
</ul>
<hr>
<h4>请点赞！因为你们的赞同/鼓励是我写作的最大动力！</h4>
<blockquote><p><strong>相关文章阅读</strong><br><a href="http://www.jianshu.com/p/ec5a1a30694b" target="_blank">Android开发：最全面、最易懂的Android屏幕适配解决方案</a><br><a href="http://www.jianshu.com/p/9fe944ee02f7" target="_blank">Android开发：Handler异步通信机制全面解析（包含Looper、Message Queue）</a><br><a href="http://www.jianshu.com/p/3c94ae673e2a" target="_blank">Android开发：最全面、最易懂的Webview详解</a><br><a href="http://www.jianshu.com/p/b87fee2f7a23" target="_blank">Android开发：JSON简介及最全面解析方法!</a><br><a href="http://www.jianshu.com/p/ce1d060573ba" target="_blank">Android开发：顶部Tab导航栏实现（TabLayout+ViewPager+Fragment）</a><br><a href="http://www.jianshu.com/p/a663803b2a44" target="_blank">Android开发：底部Tab菜单栏实现（FragmentTabHost+ViewPager）</a></p></blockquote>
<hr>
<h3>欢迎关注<a href="http://www.jianshu.com/users/383970bef0a0/latest_articles" target="_blank">Carson_Ho</a>的简书！</h3>
<p>不定期分享关于<strong>安卓开发</strong>的干货，追求<strong>短、平、快</strong>，但<strong>却不缺深度</strong>。</p>
<div class="image-package">
<img src="images/944365-9b76fa3c52d478a7.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-9b76fa3c52d478a7.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption"></div>
</div>

        </div>