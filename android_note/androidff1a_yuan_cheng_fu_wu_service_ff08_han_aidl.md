# Android：远程服务Service（含AIDL & IPC讲解）


<hr>
<h1>前言</h1>
<ul>
<li>Service作为Android四大组件之一，应用非常广泛</li>
<li>本文将介绍Service其中一种常见用法：远程Service<blockquote><p>如果你对Service还未了解，建议先阅读我写的另外一篇文章：<br><a href="http://www.jianshu.com/p/d963c55c3ab9" target="_blank">Android四大组件：Service史上最全面解析</a></p></blockquote>
</li>
</ul>
<hr>
<h1>目录</h1>
<div class="image-package">
<img src="images/944365-ea7ba2c6e52ca163.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-ea7ba2c6e52ca163.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">目录</div>
</div>
<hr>
<h1>1. 远程服务与本地服务的区别</h1>
<ul>
<li>远程服务与本地服务最大的区别是：远程Service与调用者不在同一个进程里（即远程Service是运行在另外一个进程）；而本地服务则是与调用者运行在同一个进程里</li>
<li>二者区别的详细区别如下图：</li>
</ul>
<div class="image-package">
<img src="images/944365-843b2b4e2988ff66.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-843b2b4e2988ff66.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">按运行地点分类</div>
</div>
<hr>
<h1>2. 使用场景</h1>
<p>多个应用程序共享同一个后台服务（远程服务）</p>
<blockquote><p>即一个远程Service与多个应用程序的组件（四大组件）进行跨进程通信</p></blockquote>
<div class="image-package">
<img src="images/944365-0e2a0b99bf323de8.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-0e2a0b99bf323de8.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">使用场景</div>
</div>
<hr>
<h1>3. 具体使用</h1>
<ul>
<li>
<p>为了让远程Service与多个应用程序的组件（四大组件）进行跨进程通信（IPC），需要使用AIDL</p>
<blockquote><ol>
<li>IPC：Inter-Process Communication，即跨进程通信</li>
<li>AIDL：Android Interface Definition Language，即Android接口定义语言；用于让某个Service与多个应用程序组件之间进行跨进程通信，从而可以实现多个应用程序共享同一个Service的功能。</li>
</ol></blockquote>
</li>
<li>
<p>在多进程通信中，存在两个进程角色（以最简单的为例）：服务器端和客户端</p>
</li>
<li>
<p>以下是两个进程角色的具体使用步骤：<br><strong>服务器端（Service）</strong><br>步骤1：新建定义AIDL文件，并声明该服务需要向客户端提供的接口<br>步骤2：在Service子类中实现AIDL中定义的接口方法，并定义生命周期的方法（onCreat、onBind()、blabla）<br>步骤3：在AndroidMainfest.xml中注册服务 &amp; 声明为远程服务</p>
<p><strong>客户端（Client）</strong><br>步骤1：拷贝服务端的AIDL文件到目录下<br>步骤2：使用Stub.asInterface接口获取服务器的Binder，根据需要调用服务提供的接口方法<br>步骤3：通过Intent指定服务端的服务名称和所在包，绑定远程Service</p>
</li>
</ul>
<p>接下来，我将用一个具体实例来介绍远程Service的使用</p>
<hr>
<h1>4. 具体实例</h1>
<ul>
<li>实例描述：客户端远程调用服务器端的远程Service</li>
<li>具体使用：</li>
</ul>
<h3>4.1 服务器端（Service）</h3>
<p>新建一个服务器端的工程：Service - server</p>
<blockquote><p>先下Demo再看，效果会更好：<a href="https://github.com/Carson-Ho/Service_Server" target="_blank">Github_RemoteService_Server</a></p></blockquote>
<p>步骤1. 新建一个AIDL文件</p>
<div class="image-package">
<img src="images/944365-765bf5673bb721f4.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-765bf5673bb721f4.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">新建AIDL文件</div>
</div>
<p>步骤2. 在新建AIDL文件里定义Service需要与Activity进行通信的内容（方法），并进行编译（Make Project）</p>
<pre class="hljs java"><code class="java"><span class="hljs-comment">// 在新建的AIDL_Service1.aidl里声明需要与Activity进行通信的方法</span>
<span class="hljs-keyword">package</span> scut.carson_ho.demo_service;

<span class="hljs-class"><span class="hljs-keyword">interface</span> <span class="hljs-title">AIDL_Service1</span> </span>{
    <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">AIDL_Service</span><span class="hljs-params">()</span></span>;
}
<span class="hljs-comment">//AIDL中支持以下的数据类型</span>
<span class="hljs-comment">//1. 基本数据类型</span>
<span class="hljs-comment">//2. String 和CharSequence</span>
<span class="hljs-comment">//3. List 和 Map ,List和Map 对象的元素必须是AIDL支持的数据类型;</span>
<span class="hljs-comment">//4. AIDL自动生成的接口（需要导入-import）</span>
<span class="hljs-comment">//5. 实现android.os.Parcelable 接口的类（需要导入-import)</span></code></pre>
<div class="image-package">
<img src="images/944365-b3c4011539256750.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-b3c4011539256750.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">编译</div>
</div>
<p>步骤3：在Service子类中实现AIDL中定义的接口方法，并定义生命周期的方法（onCreat、onBind()、blabla）<br><em>MyService.java</em></p>
<pre class="hljs aspectj"><code class="aspectj"><span class="hljs-keyword">public</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">MyService</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">Service</span> </span>{

    <span class="hljs-comment">// 实例化AIDL的Stub类(Binder的子类)</span>
    AIDL_Service1.Stub mBinder = <span class="hljs-keyword">new</span> AIDL_Service1.Stub() {

        <span class="hljs-comment">//重写接口里定义的方法</span>
        <span class="hljs-meta">@Override</span>
        <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">AIDL_Service</span><span class="hljs-params">()</span> <span class="hljs-keyword">throws</span> RemoteException </span>{
            System.out.println(<span class="hljs-string">"客户端通过AIDL与远程后台成功通信"</span>);
        }
    };

<span class="hljs-comment">//重写与Service生命周期的相关方法</span>
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
    <span class="hljs-comment">//在onBind()返回继承自Binder的Stub类型的Binder，非常重要</span>
        <span class="hljs-keyword">return</span> mBinder;
    }

    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">boolean</span> <span class="hljs-title">onUnbind</span><span class="hljs-params">(Intent intent)</span> </span>{
        System.out.println(<span class="hljs-string">"执行了onUnbind()"</span>);
        <span class="hljs-function"><span class="hljs-keyword">return</span> <span class="hljs-keyword">super</span>.<span class="hljs-title">onUnbind</span><span class="hljs-params">(intent)</span></span>;
    }

}</code></pre>
<p>步骤4：在AndroidMainfest.xml中注册服务 &amp; 声明为远程服务</p>
<pre class="hljs javascript"><code class="javascript">&lt;service
            android:name=<span class="hljs-string">".MyService"</span>
            android:process=<span class="hljs-string">":remote"</span>  <span class="hljs-comment">//将本地服务设置成远程服务</span>
            android:exported=<span class="hljs-string">"true"</span>      <span class="hljs-comment">//设置可被其他进程调用</span>
            &gt;
            <span class="hljs-comment">//该Service可以响应带有scut.carson_ho.service_server.AIDL_Service1这个action的Intent。</span>
            <span class="hljs-comment">//此处Intent的action必须写成“服务器端包名.aidl文件名”</span>
            &lt;intent-filter&gt;
                <span class="xml"><span class="hljs-tag">&lt;<span class="hljs-name">action</span> <span class="hljs-attr">android:name</span>=<span class="hljs-string">"scut.carson_ho.service_server.AIDL_Service1"</span>/&gt;</span>
            <span class="hljs-tag">&lt;/<span class="hljs-name">intent-filter</span>&gt;</span>

        <span class="hljs-tag">&lt;/<span class="hljs-name">service</span>&gt;</span></span></code></pre>
<p>至此，服务器端（远程Service）已经完成了。</p>
<h2>4.2 客户端（Client）</h2>
<p>新建一个客户端的工程：Service - Client</p>
<blockquote><p>先下Demo再看，效果会更好：<a href="https://github.com/Carson-Ho/Service_Client" target="_blank">Github_RemoteService_Client</a></p></blockquote>
<p>步骤1：将服务端的AIDL文件所在的包复制到客户端目录下（Project/app/src/main），并进行编译</p>
<blockquote><p>注：记得要原封不动地复制！！什么都不要改！</p></blockquote>
<div class="image-package">
<img src="images/944365-83eec5d94c1549dc.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-83eec5d94c1549dc.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">复制后的目录</div>
</div>
<p>步骤2：在主布局文件定义“绑定服务”的按钮<br><em>MainActivity.xml</em></p>
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
        <span class="hljs-attr">android:text</span>=<span class="hljs-string">"绑定服务"</span>
        /&gt;</span>
<span class="hljs-tag">&lt;/<span class="hljs-name">RelativeLayout</span>&gt;</span></code></pre>
<p>步骤3：在MainActivity.java里</p>
<ul>
<li>使用Stub.asInterface接口获取服务器的Binder；</li>
<li>通过Intent指定服务端的服务名称和所在包，进行Service绑定；</li>
<li>根据需要调用服务提供的接口方法。</li>
</ul>
<p><em>MainActivity.java</em></p>
<pre class="hljs java"><code class="java"><span class="hljs-keyword">public</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">MainActivity</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">AppCompatActivity</span> </span>{

        <span class="hljs-keyword">private</span> Button bindService;

        <span class="hljs-comment">//定义aidl接口变量</span>
        <span class="hljs-keyword">private</span> AIDL_Service1 mAIDL_Service;

        <span class="hljs-comment">//创建ServiceConnection的匿名类</span>
        <span class="hljs-keyword">private</span> ServiceConnection connection = <span class="hljs-keyword">new</span> ServiceConnection() {

            <span class="hljs-comment">//重写onServiceConnected()方法和onServiceDisconnected()方法</span>
            <span class="hljs-comment">//在Activity与Service建立关联和解除关联的时候调用</span>
            <span class="hljs-meta">@Override</span>
            <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">onServiceDisconnected</span><span class="hljs-params">(ComponentName name)</span> </span>{
            }

            <span class="hljs-comment">//在Activity与Service建立关联时调用</span>
            <span class="hljs-meta">@Override</span>
            <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">onServiceConnected</span><span class="hljs-params">(ComponentName name, IBinder service)</span> </span>{

                <span class="hljs-comment">//使用AIDLService1.Stub.asInterface()方法获取服务器端返回的IBinder对象</span>
                <span class="hljs-comment">//将IBinder对象传换成了mAIDL_Service接口对象</span>
                mAIDL_Service = AIDL_Service1.Stub.asInterface(service);

                <span class="hljs-keyword">try</span> {

                    <span class="hljs-comment">//通过该对象调用在MyAIDLService.aidl文件中定义的接口方法,从而实现跨进程通信</span>
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

            <span class="hljs-comment">//设置绑定服务的按钮</span>
            bindService.setOnClickListener(<span class="hljs-keyword">new</span> View.OnClickListener() {
                <span class="hljs-meta">@Override</span>
                <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">onClick</span><span class="hljs-params">(View v)</span> </span>{

                    <span class="hljs-comment">//通过Intent指定服务端的服务名称和所在包，与远程Service进行绑定</span>
                    <span class="hljs-comment">//参数与服务器端的action要一致,即"服务器包名.aidl接口文件名"</span>
                    Intent intent = <span class="hljs-keyword">new</span> Intent(<span class="hljs-string">"scut.carson_ho.service_server.AIDL_Service1"</span>);

                    <span class="hljs-comment">//Android5.0后无法只通过隐式Intent绑定远程Service</span>
                    <span class="hljs-comment">//需要通过setPackage()方法指定包名</span>
                    intent.setPackage(<span class="hljs-string">"scut.carson_ho.service_server"</span>);

                    <span class="hljs-comment">//绑定服务,传入intent和ServiceConnection对象</span>
                    bindService(intent, connection, Context.BIND_AUTO_CREATE);

                }
            });
        }

    }</code></pre>
<h2>4.3 测试结果</h2>
<div class="image-package">
<img src="images/944365-90d3da78fdab4acd.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-90d3da78fdab4acd.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">点击绑定服务按钮</div>
</div>
<p>从上面测试结果可以看出：</p>
<ul>
<li>打印的语句分别运行在不同进程（看语句前面的包名）；</li>
<li>客户端调用了服务端Service的方法</li>
</ul>
<p>即<strong>客户端和服务端进行了跨进程通信</strong></p>
<h1>4.4 Demo地址</h1>
<ul>
<li>客户端：<a href="https://github.com/Carson-Ho/Service_Client" target="_blank">Github_RemoteService_Client</a>
</li>
<li>服务端：<a href="https://github.com/Carson-Ho/Service_Server" target="_blank">Github_RemoteService_Server</a>
</li>
</ul>
<hr>
<h1>5. 总结</h1>
<ul>
<li>本文对Android组件Service中的远程Service进行了全面介绍</li>
<li>如果你还想了解关于Service的其他知识，请浏览以下文章：<blockquote><p><a href="http://www.jianshu.com/p/d963c55c3ab9" target="_blank">Android四大组件：Service史上最全面解析</a><br><a href="http://www.jianshu.com/p/8d0cde35eb10" target="_blank">Android：Service生命周期最全面解析</a><br><a href="http://www.jianshu.com/p/e04c4239b07e" target="_blank">Android：（本地、可通信的、前台、远程）Service使用全面介绍</a><br><a href="http://www.jianshu.com/p/34326751b2c6" target="_blank">Android：远程服务Service（含AIDL &amp; IPC讲解）</a><br><a href="http://www.jianshu.com/p/8a3c44a9173a" target="_blank">Android多线程全面解析：IntentService用法&amp;源码</a></p></blockquote>
</li>
<li>接下来会介绍继续介绍Android开发中的相关知识，有兴趣可以继续关注<a href="http://www.jianshu.com/users/383970bef0a0/latest_articles" target="_blank">Carson_Ho的安卓开发笔记</a>
</li>
</ul>
<hr>
<h4>请点赞！因为你们的赞同/鼓励是我写作的最大动力！</h4>
<blockquote><p><strong>相关文章阅读</strong><br><a href="http://www.jianshu.com/p/ec5a1a30694b" target="_blank">Android开发：最全面、最易懂的Android屏幕适配解决方案</a><br><a href="http://www.jianshu.com/p/9fe944ee02f7" target="_blank">Android开发：Handler异步通信机制全面解析（包含Looper、Message Queue）</a><br><a href="http://www.jianshu.com/p/ce1d060573ba" target="_blank">Android开发：顶部Tab导航栏实现（TabLayout+ViewPager+Fragment）</a><br><a href="http://www.jianshu.com/p/a663803b2a44" target="_blank">Android开发：底部Tab菜单栏实现（FragmentTabHost+ViewPager）</a><br><a href="http://www.jianshu.com/p/b87fee2f7a23" target="_blank">Android开发：JSON简介及最全面解析方法!</a><br><a href="http://www.jianshu.com/p/e636f4f8487b" target="_blank">Android开发：XML简介及DOM、SAX、PULL解析对比</a></p></blockquote>
<hr>
