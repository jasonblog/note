# Android之Service总结


<div class="show-content">
          <h3>1. 简介</h3>
<p>与前一篇<a href="http://www.jianshu.com/p/c2c2ee4eb48a" target="_blank">Android之Activity总结</a>是同一系列的文章，是自己在学习和研发过程中，对Service的一些知识点的总结，汇总得到这篇文章。</p>
<p>这篇文章会从Service的一些小知识点，延伸到Android中几种常用进程间通信方法。</p>
<h3>2. 进程</h3>
<p>Service是一种不提供用户交互页面但是可以在后台长时间运行的组件，可以通过在AndroidManifest.xml设置Service的<code>android:process=":remote"</code>属性，让Service运行另一个进程中，也就是说，虽然你是在当前应用启动的这个Service，但是这个Service和这个应用并不是同一个进程。</p>
<p><strong>四大组件都支持<code>android:process=":remote"</code>这个属性。</strong></p>
<p>因为Service可以运行在不同的进程，这里说一下Android中几种进程的优先级，当系统内存不足时候，系统会从优先级低的进程开始回收，下面根据优先级由高到低列出Android中几种进程。</p>
<ul>
<li>
<p><strong>前台进程</strong>，当前用户操作所需要的进程</p>
<ul>
<li>用户正在交互的Activity（Activity执行了onResume方法）</li>
<li>与正在交互的Activity绑定的Service</li>
<li>设置为前台权限的Service（Service调用startForeground()方法）</li>
<li>正在执行某些生命周期回调的Service，onCreate()、onStart()、onDestroy()</li>
<li>
<p>正在执行onReceive()的BroadcastReceiver</p>
<p>这种进程基本不会被回收，只有当内存不足以支持前台进程同时运行时候，系统才回回收它们，主要关注前三个。</p>
</li>
</ul>
</li>
<li>
<p><strong>可见进程</strong>，没有与用户交互所必须的组件，但是在屏幕上仍然可见其内容的进程</p>
<ul>
<li>调用了onPause()方法但仍对用户可见的Activity</li>
<li>与上面这种Activity绑定的Service</li>
</ul>
</li>
<li>
<p><strong>服务进程</strong>，使用startService()启动的Service且不属于上面两种类别进程的进程，虽然这个进程与用户交互没有直接关系，但是一般会在后台执行一些耗时操作，所以，只有当内存不足以维持所有前台进程和可见进程同时运行，系统才回回收这个类别的进程。</p>
</li>
<li>
<p><strong>后台进程</strong>，对用户不可见的Activity进程，已调用了onStop()方法的Activity</p>
</li>
<li>
<strong>空进程</strong>，不包含任何活动应用组件的进程，保留这种进程唯一目的是作为缓存，缩短引用组件下次启动时间。通常系统会最优先回收这类进程。</li>
</ul>
<p>此外，一个进程的级别可能会因为其他进程对它的依赖而有所提高，即进程A服务于进程B（B依赖A），那么A的进程级别至少是和B一样高的。</p>
<h3>3. Service配置</h3>
<p>和其他组件(Activity/ContentProvider/BroadcastReceiver)一样，Service需要在Androidmanifest.xml中声明。</p>
<pre class="hljs xml"><code class="xml"><span class="hljs-tag">&lt;<span class="hljs-name">manifest</span> <span class="hljs-attr">...</span> &gt;</span>
  ...
  <span class="hljs-tag">&lt;<span class="hljs-name">application</span> <span class="hljs-attr">...</span> &gt;</span>
      <span class="hljs-tag">&lt;<span class="hljs-name">service</span> <span class="hljs-attr">android:name</span>=<span class="hljs-string">".DemoService"</span> /&gt;</span>
      ...
  <span class="hljs-tag">&lt;/<span class="hljs-name">application</span>&gt;</span>
<span class="hljs-tag">&lt;/<span class="hljs-name">manifest</span>&gt;</span></code></pre>
<p>Service是运行在主线程中的，如果有什么耗时的操作，建议新建子线程去处理，避免阻塞主线程，降低ANR的风险。</p>
<p>在另外一篇文章中<a href="http://www.jianshu.com/p/a7535c3f33df" target="_blank">Intent以及IntentFilter详解</a>提到过，为了确保应用的安全，不要为Service设置intent-filter，因为如果使用隐式Intent去启动Service时候，手机里面那么多应用，并不能确定哪一个Service响应了这个Intent，所以在项目中尽量使用显式Intent去启动Service。在Android 5.0(API LEVEL 21)版本后的，如果传入隐式Intent去调用<code>bindService()</code>方法，系统会抛出异常。</p>
<p>可以通过设置<code>android:exported=false</code>来确保这个Service仅能在本应用中使用。</p>
<h3>4. 服务启动方式</h3>
<p>服务可以由其他组件启动，而且如果用户切换到其他应用，这个服务可能会继续在后台执行。到目前为止，Android中Service总共有三种启动方式。</p>
<ul>
<li>Scheduled，可定时执行的Service，是Android 5.0（API LEVEL 21）版本中新添加的一个Service，名为JobService，继承Service类，使用JobScheduler类调度它并且设置JobService运行的一些配置。具体文档可以参考<a href="https://developer.android.com/reference/android/app/job/JobScheduler.html" target="_blank">JobScheduler</a>，如果你的应用最低支持版本是21，官方建议使用JobService。</li>
<li>Started，通过<code>startService()</code>启动的Service。通过这种方式启动的Service会独立的运行在后台，即使启动它的组件已经销毁了。例如Activity A使用startService()启动了Service B，过了会儿，Activity A执行onDestroy()被销毁了，如果Service B任务没有执行完毕，它仍然会在后台执行。这种启动方式启动的Service需要主动调用<code>StopService()</code>停止服务。</li>
<li>Bound，通过<code>bindService()</code>启动的Service。通过这种方式启动Service时候，会返回一个客户端交互接口，用户可以通过这个接口与服务进行交互，如果这个服务是在另一个进程中，那么就实现了进程间通信，也就是Messenger和AIDL，这个会是下篇文章的重点。多个组件可以同时绑定同一个Service，如果所有的组件都调用<code>unbindService()</code>解绑后，Service会被销毁。</li>
</ul>
<p><strong>startService和bindService可以同时使用</strong></p>
<h3>5. 主要方法</h3>
<p>Service是一个抽象类，使用需要我们去实现它的抽象方法<code>onBind()</code>，Service有且仅有这一个抽象方法，还有一些其他的生命周期回调方法需要复写帮助我们实现具体的功能。</p>
<ul>
<li>onCreate()，在创建服务时候，可以在这个方法中执行一些的初始化操作，它在<code>onStartCommand()</code>和<code>onBind()</code>之前被调用。如果服务已经存在，调用<code>startService()</code>启动服务时候这个方法不会调用，只会调用<code>onStartCommand()</code>方法。</li>
<li>onStartCommand()，其他组件通过<code>startService()</code>启动服务时候会回调这个方法，这个方法执行后，服务会启动被在后台运行，需要调用<code>stopSelf()</code>或者<code>stopService()</code>停止服务。</li>
<li>onBind()，其他组件通过<code>bindService()</code>绑定服务时候会回调的方法，这是Service的一个抽象方法，如果客户端需要与服务交互，需要在这个方法中返回一个<code>IBinder</code>实现类实例化对象，如果不想其他客户端与服务绑定，直接返回null。</li>
<li>onDestroy()，当服务不在还是用且即将被销毁时，会回调这个方法，可以在这个方法中做一些释放资源操作，这是服务生命周期的最后一个回调。</li>
</ul>
<p>如果组件仅通过<code>startService()</code>启动服务，不论服务是否已经启动，都会回调<code>onStartCommand()</code>方法，而且服务会一直运行，需要调用<code>stopSelf</code>和<code>stopService</code>方法关闭服务。</p>
<p>如果组件仅通过<code>bindService()</code>绑定服务，则服务只有在与组件绑定时候运行，一旦所有的客户端全部取消绑定<code>unbindService</code>，系统才会销毁该服务。</p>
<p>多次启动同一个服务，只有在服务初次启动时候会回调<code>onCreate</code>方法，但是每次都会回调<code>onStartCommand</code>，可以利用这个向服务传递一些信息。</p>
<p><strong>onStartCommand()的回调是在UI主线程，如果有什么耗时的操作，建议新启线程去处理。</strong></p>
<h3>6. 启动和关闭服务</h3>
<p>启动服务：</p>
<ul>
<li><code>JobScheduler.schedule()</code></li>
<li><code>startService(Intent)</code></li>
<li><code>bindService(Intent service, ServiceConnection conn, int flags)</code></li>
</ul>
<p>关闭服务：</p>
<ul>
<li>
<code>JobScheduler.cancel()</code>或者<code>JobScheduler.cancelAll()</code>，对应<code>JobScheduler.schedule()</code>
</li>
<li>Service自身的<code>stopSelf()</code>方法，组件的<code>stopService(Intent)</code>方法，对应<code>startService</code>启动方法</li>
<li>
<code>unbindService(ServiceConnection conn)</code>，对应<code>bindService</code>
</li>
</ul>
<p>示例：</p>
<pre class="hljs pony"><code class="pony"><span class="hljs-comment">// 启动服务</span>
<span class="hljs-type">Intent</span> intent = <span class="hljs-function"><span class="hljs-keyword">new</span> <span class="hljs-title">Intent</span>(this, <span class="hljs-type">DemoService</span>.class);
<span class="hljs-title">startService</span>(intent);

<span class="hljs-comment">// 停止服务</span>
<span class="hljs-title">stopService</span>(intent)

<span class="hljs-comment">// 绑定服务</span>
<span class="hljs-title">ServiceConnection</span> <span class="hljs-title">mConnection</span> = <span class="hljs-title">ServiceConnection</span>() { ... };
<span class="hljs-title">Intent</span> <span class="hljs-title">intent</span> = <span class="hljs-title">new</span> <span class="hljs-title">Intent</span>(this, <span class="hljs-type">DemoService</span>.class);
<span class="hljs-title">bindService</span>(intent, mConnection, <span class="hljs-type">Context</span>.<span class="hljs-type">BIND_AUTO_CREATE</span>);

<span class="hljs-comment">// 解除绑定</span>
<span class="hljs-title">unbindService</span>(mConnection);</span></code></pre>
<p>绑定服务<code>bindService()</code>第三个参数数值：</p>
<ul>
<li>0，如果不想设置任何值，就设置成0</li>
<li>
<code>Context.BIND_AUTO_CREATE</code>，绑定服务时候，如果服务尚未创建，服务会自动创建，在API LEVEL 14以前的版本不支持这个标志，使用<code>Context.BIND_WAIVE_PRIORITY</code>可以达到同样效果</li>
<li>
<code>Context.BIND_DEBUG_UNBIND</code>，通常用于Debug，在<code>unbindService</code>时候，会将服务信息保存并打印出来，这个标记很容易造成内存泄漏。</li>
<li>
<code>Context.BIND_NOT_FOREGROUND</code>，不会将被绑定的服务提升到前台优先级，但是这个服务也至少会和客户端在内存中优先级是相同的。</li>
<li>
<code>Context.BIND_ABOVE_CLIENT</code>，设置服务的进程优先级高于客户端的优先级，只有当需要服务晚于客户端被销毁这种情况才这样设置。</li>
<li>
<code>Context.BIND_ALLOW_OOM_MANAGEMENT</code>，保持服务受默认的服务管理器管理，当内存不足时候，会销毁服务</li>
<li>
<code>Context.BIND_WAIVE_PRIORITY</code>，不会影响服务的进程优先级，像通用的应用进程一样将服务放在一个LRU表中</li>
<li>
<code>Context.BIND_IMPORTANT</code>，标识服务对客户端是非常重要的，会将服务提升至前台进程优先级，通常情况下，即时客户端是前台优先级，服务最多也只能被提升至可见进程优先级，</li>
<li>
<code>BIND_ADJUST_WITH_ACTIVITY</code>，如果客户端是Activity，服务优先级的提高取决于Activity的进程优先级，使用这个标识后，会无视其他标识。</li>
</ul>
<h3>7. onStartCommand()返回值</h3>
<p><code>onStartCommand()</code>方法有一个int的返回值，这个返回值标识服务关闭后系统的后续操作。</p>
<p>返回值有以下几种：</p>
<ul>
<li>
<code>Service.START_STICKY</code>，启动后的服务被杀死，系统会自动重建服务并调用<code>on onStartCommand()</code>，但是不会传入最后一个Intent(Service可能多次执行onStartCommand)，会传入一个空的Intent，使用这个标记要注意对Intent的判空处理。这个标记适用于太依靠外界数据Intent，在特定的时间，有明确的启动和关闭的服务，例如后台运行的音乐播放。</li>
<li>
<code>Service.START_NOT_STICKY</code>，启动后的服务被杀死，系统不会自动重新创建服务。这个标记是最安全的，适用于依赖外界数据Intent的服务，需要完全执行的服务。</li>
<li>
<code>Service.START_REDELIVER_INTENT</code>，启动后的服务被杀死，系统会重新创建服务并调用<code>onStartCommand()</code>，同时会传入最后一个Intent。这个标记适用于可恢复继续执行的任务，比如说下载文件。</li>
<li>
<code>Service.START_STICKY_COMPATIBILITY</code>，启动后的服务被杀死，不能保证系统一定会重新创建Service。</li>
</ul>
<h3>8. Service生命周期</h3>
<p>Service生命周期（从创建到销毁）跟它被启动的方式有关系，这里只介绍<code>startService</code>和<code>bindService</code>两种启动方法时候Service的生命周期。</p>
<ul>
<li>
<code>startService</code>启动方式，其他组件用这种方式启动服务，服务会在后台一直运行，只有服务调用本身的<code>stopSelf()</code>方法或者其他组件调用<code>stopService()</code>才能停止服务。</li>
<li>
<code>bindService</code>启动方式，其他组件用这种方法绑定服务，服务通过IBinder与客户端通信，客户端通过<code>unbindService</code>接触对服务的绑定，当没有客户端绑定到服务，服务会被系统销毁。</li>
</ul>
<p>这两种生命周期不是独立的，组件可以同时用<code>startService</code>启动服务同时用<code>bindService</code>绑定服务，例如跨页面的音乐播放器，就可以在多个页面同时绑定同一个服务，这种情况下需要调用<code>stopService()</code>或者服务本身的<code>stopSelf()</code>并且没有客户端绑定到服务，服务才会被销毁。</p>
<div class="image-package">
<img src="images/354846-1d3012753d0e93c5.png" data-original-src="images/354846-1d3012753d0e93c5.png" style="cursor: zoom-in;"><br><div class="image-caption">Service生命周期图</div>
</div>
<p>左图是使用<code>startService()</code>所创建的服务的生命周期，右图是使用<code>bindService()</code>所创建的服务的生命周期。</p>
<h3>9. 在前台运行服务</h3>
<p>服务可以通过startForeground来使服务变成前台优先级。</p>
<pre class="hljs aspectj"><code class="aspectj"><span class="hljs-keyword">public</span> <span class="hljs-keyword">final</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">startForeground</span><span class="hljs-params">(<span class="hljs-keyword">int</span> id, Notification notification)</span> </span>{
    <span class="hljs-keyword">try</span> {
        mActivityManager.setServiceForeground(
                <span class="hljs-keyword">new</span> ComponentName(<span class="hljs-keyword">this</span>, mClassName), mToken, id,
                notification, <span class="hljs-keyword">true</span>);
    } <span class="hljs-keyword">catch</span> (RemoteException ex) {
    }
}</code></pre>
<p>第一个参数用于标识你<strong>应用中唯一的通知标识id</strong>，不能设为0，最终会传入<code>NotificationManager.notify(int id, Notification notification)</code>取消通知需要用到，第二个参数是通知具体内容。</p>
<p>前台服务需要在状态栏中添加通知，例如，将音乐播放器的服务设置为前台服务，状态栏通知显示正在播放的歌曲，并允许其他组件与其交互。</p>
<pre class="hljs cpp"><code class="cpp"><span class="hljs-comment">// 设置Notification属性</span>
Notification notification = <span class="hljs-keyword">new</span> Notification(R.drawable.icon, getText(R.<span class="hljs-built_in">string</span>.ticker_text),System.currentTimeMillis());
Intent notificationIntent = <span class="hljs-keyword">new</span> Intent(<span class="hljs-keyword">this</span>, ExampleActivity.<span class="hljs-keyword">class</span>);
PendingIntent pendingIntent = PendingIntent.getActivity(<span class="hljs-keyword">this</span>, <span class="hljs-number">0</span>, notificationIntent, <span class="hljs-number">0</span>);
notification.setLatestEventInfo(<span class="hljs-keyword">this</span>, getText(R.<span class="hljs-built_in">string</span>.notification_title), getText(R.<span class="hljs-built_in">string</span>.notification_message), pendingIntent);
startForeground(ONGOING_NOTIFICATION_ID, notification);</code></pre>
<p>要将服务从前台移除，需要调用stopForeground(boolean removeNotification)，参数是一个布尔值，用来标识服务从前台服务移除时候，是否需要移除状态栏的通知。如果服务在前台运行时候被停止，也会移除状态栏的通知。</p>
<h3>10. 与服务通信</h3>
<h4>10.1 广播</h4>
<p>不多说，万能的通信。</p>
<h4>10.2 内存共享</h4>
<p>不多说，万能的通信</p>
<h4>10.3 本地数据共享</h4>
<p>不多说，万能的通信，例如ContentProvider/SharePreference等等。</p>
<h4>10.4 startService()</h4>
<p>使用这个方法启动的服务，再次调用<code>startService()</code>传入Intent即可与服务通信，因为这种方式启动的服务在完整的生命周期内<code>onCreate()</code>只会执行一次，而<code>onStartCommand()</code>会执行多次，我们再次调用<code>startService()</code>时候，可以在<code>oonStartCommand()</code>去处理。</p>
<h4>10.5 bindService()</h4>
<p>使用这种方法启动的服务，组件有三种与服务通信的方式。 </p>
<ul>
<li>Service中实现IBinder</li>
<li>Messenger(AIDL的简化版)</li>
<li>AIDL</li>
</ul>
<p>下一篇文章具体介绍Messenger、AIDL，因为它们是属于Android进程间通信。</p>
<p>如果一个服务Service只需要在本应用的进程中使用，不提供给其他进程，推荐使用第一种方法。</p>
<p>使用示例：</p>
<p>Service：</p>
<pre class="hljs scala"><code class="scala"><span class="hljs-comment">/**
 * 本地服务
 * &lt;br/&gt;
 * 和启动应用属于同一进程
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
     * 提供给客户端的方法
     *
     * @return
     */</span>
    public <span class="hljs-type">String</span> getServiceInfo() {
        <span class="hljs-keyword">return</span> <span class="hljs-keyword">this</span>.getPackageName() + <span class="hljs-string">" "</span> + <span class="hljs-keyword">this</span>.getClass().getSimpleName();
    }

    <span class="hljs-comment">/**
     * 自定义的IBinder
     */</span>
    public <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">LocalBinder</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">Binder</span> </span>{
        public <span class="hljs-type">LocalService</span> getService() {
            <span class="hljs-keyword">return</span> <span class="hljs-type">LocalService</span>.<span class="hljs-keyword">this</span>;
        }
    }
}</code></pre>
<p>Activity：</p>
<pre class="hljs java"><code class="java"><span class="hljs-comment">/**
 * 绑定本地服务的组件
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
<h3>11. 服务长存后台</h3>
<p>关于<strong>网上通用</strong>的提升服务优先级以保证服务长存后台，即保证服务不轻易被系统杀死的方法有以下几种。</p>
<ul>
<li>
<p>设置<code>android:persistent="true"</code>，这是application的一个属性，官方都不建议使用。</p>
<pre class="hljs applescript"><code class="applescript">  Whether <span class="hljs-keyword">or</span> <span class="hljs-keyword">not</span> <span class="hljs-keyword">the</span> <span class="hljs-built_in">application</span> should remain <span class="hljs-built_in">running</span> <span class="hljs-keyword">at</span> all <span class="hljs-keyword">times</span>，
  <span class="hljs-string">"true"</span> <span class="hljs-keyword">if</span> <span class="hljs-keyword">it</span> should, <span class="hljs-keyword">and</span> <span class="hljs-string">"false"</span> <span class="hljs-keyword">if</span> <span class="hljs-keyword">not</span>. 
  The default value <span class="hljs-keyword">is</span> <span class="hljs-string">"false"</span>. 
  Applications should <span class="hljs-keyword">not</span> normally <span class="hljs-keyword">set</span> this flag; 
  persistence mode <span class="hljs-keyword">is</span> intended only <span class="hljs-keyword">for</span> certain system applications.</code></pre>
</li>
<li>
<p>设置<code>android:priority</code>优先级，这个并不是Service的属性。这个属性是在<code>intent-filter</code>中设置的。<a href="https://developer.android.com/guide/topics/manifest/intent-filter-element.html" target="_blank">官方解释</a>，这个属性只对活动和广播有用，而且这个是接受Intent的优先级，并不是在内存中的优先级，呵呵。</p>
<pre class="hljs livecodeserver"><code class="livecodeserver">  android:priority
  The priority that should be given <span class="hljs-built_in">to</span> <span class="hljs-keyword">the</span> parent component <span class="hljs-keyword">with</span> regard <span class="hljs-built_in">to</span> 
  handling intents <span class="hljs-keyword">of</span> <span class="hljs-keyword">the</span> type described <span class="hljs-keyword">by</span> <span class="hljs-keyword">the</span> <span class="hljs-built_in">filter</span>. 
  This attribute has meaning <span class="hljs-keyword">for</span> both activities <span class="hljs-keyword">and</span> broadcast receivers。</code></pre>
</li>
<li>
<p>在Service的<code>onDestroy</code>中发送广播，然后重启服务，就目前我知道的，会出现Service的<code>onDestroy</code>不调用的情况。</p>
</li>
<li>
<p><code>startForeground</code>，这个上面提到过，是通过<code>Notification</code>提升优先级。</p>
</li>
<li>设置<code>onStartCommand()</code>返回值，让服务被杀死后，系统重新创建服务，上面提到过。</li>
</ul>
<p>五个里面就两个能稍微有点用，所以啊，网络谣传害死人。</p>
<h3>12. IntentService</h3>
<p>敲黑板时间，重点来了，官方强力推荐。</p>
<p>前面提到两点。</p>
<ul>
<li>因为Service中几个方法的回调都是在主线程中，如果使用Service执行特别耗时的操作，建议单独新建线程去操作，避免阻塞主线程（UI线程）</li>
<li>启动服务和停止服务是成对出现的，需要手动停止服务</li>
</ul>
<p>IntentService完美的帮我们解决了这个问题，在内部帮我们新建的线程，不需要我们手动新建，执行完毕任务后会自动关闭。IntentService也是一个抽象类，里面有一个<code>onHandleIntent(Intent intent)</code>抽象方法，这个方法是在非UI线程调用的，在这里执行耗时的操作。</p>
<p>IntentService使用非UI线程逐一处理所有的启动需求，它在内部使用Handler，将所有的请求放入队列中，依次处理，关于Handler可以看<a href="http://www.jianshu.com/p/e72aba99012a" target="_blank">这篇文章</a>，也就是说IntentService不能同时处理多个请求，如果不要求服务同时处理多个请求，可以考虑使用IntentService。</p>
<p>IntentService在内部使用<code>HandlerThread</code>配合<code>Handler</code>来处理耗时操作。</p>
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
<p>注意<code>msg.arg1</code>它是请求的唯一标识，每发送一个请求，会生成一个唯一标识，然后将请求放入<code>Handler</code>处理队列中，从源代码里面可以看见，在执行完毕<code>onHandleIntent</code>方法后，会执行<code>stopSelf</code>来关闭本身，同时IntentService中<code>onBind()</code>方法默认返回<code>null</code>，这说明<strong>启动IntetService的方式最好是用<code>startService</code>方法</strong>，这样在服务执行完毕后才会自动停止；如果使用<code>bindService</code>来启动服务，还是需要调用<code>unbindService</code>来解绑服务的，也需要复写<code>onBind()</code>方法。</p>
<blockquote><p>小盆宇：在<code>onHandleIntent</code>中，执行<code>onHandleIntent</code>后紧接着执行stopSelf(int startId)，把服务就给停止了，那第一个请求执行完毕服务就停止了，后续的请求怎么会执行？</p></blockquote>
<p>注意stopSelf(int startID)方法作用是在其参数startId跟最后启动该service时生成的id相等时才会执行停止服务，当有多个请求时候，如果发现当前请求的startId不是最后一个请求的id，那么不会停止服务，所以只有当最后一个请求执行完毕后，才回停止服务。</p>
<h3>13. 总结</h3>
<p>在年前写了出来，比Activity中的坑少了太多，希望对大家有帮助。下一篇是关于Android中进程通信Messenger/AIDL的，是本篇的补充，但是也属于单独的知识点。</p>

        </div>