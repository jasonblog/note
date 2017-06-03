# 进程通信之Messenger&AIDL使用详解


<div class="show-content">
          <h2>1. 前言</h2>
<p>提到的进程间通信（IPC：Inter-Process Communication），在Android系统中，一个进程是不能直接访问另一个进程的内存的，需要提供一些机制在不同的进程之间进行通信，Android官方推出了AIDL(Android Interface Definition Language)，它是基于Binder机制的，至于官方为什么要采用Binder，查看<a href="https://www.zhihu.com/question/39440766" target="_blank">为什么Android要采用Binder作为IPC机制</a>，分析很全面。</p>
<p>上篇<a href="http://www.jianshu.com/p/a1d3d9693e91" target="_blank">Android之Service总结
</a>提到组件在与Service通信方法有三种。</p>
<ul>
<li>实现IBinder</li>
<li>Messenger</li>
<li>AIDL</li>
</ul>
<p>后面两种可以跨进程通信，是基于Binder机制的通信方式。</p>
<h2>2. 使用时机</h2>
<p>在确定使用什么机制之前，首先了解应用场景。Android系统中，如果组件与服务通信是在同一进程，就使用第一种方式；如果是跨进程通信，使用第二种和第三种，两者不同在于，Messenger不能处理多线程并发请求。</p>
<h2>3. AIDL使用</h2>
<p>AIDL是可以处理多线程访问的请求的，所以实现AIDL首先要保证线程安全。</p>
<ul>
<li>创建.aidl文件，定义接口</li>
<li>在代码中实现接口，Android SDK会根据aidl文件，生成接口，接口内部有一个名为Stub内部抽象类，这个类用于继承了Binder类并实现aidl文件中定义的接口，我们需要拓展Stub类并实现里面的抽象方法</li>
<li>复写Service的<code>onBind()</code>，返回Stub类的实现</li>
</ul>
<h3>3.1 创建.aidl文件</h3>
<p>在Android Studio中工程目录下，反键<code>new -&gt; AIDL -&gt; AIDL FIle</code>，可以新建aidl文件，编译器会自动在<code>app(壳工程)/src/main/</code>目录下新建aidl文件，同时也会新建文件夹，默认以工程包名作为aidl文件所在的目录。</p>
<p>目录结构如下：</p>
<div class="image-package">

<img src="images/354846-006d3a6a69c932e9.png" data-original-src="http://upload-images.jianshu.io/upload_images/354846-006d3a6a69c932e9?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">图-1 aidl文件目录结构</div>
</div>
<p>也可以手动创建，aidl接口定义的包名也可以和工程包名不同，aidl文件语法和Java语法相似，aidl定义的接口名必须和文件名一致，而且支持传递自定义的数据类型，需要实现parcelable接口。</p>
<p>IRemoteService.aidl</p>
<pre class="hljs cs"><code class="cs">package com.demo.aidl;

import com.demo.aidl.ParcelableData;

<span class="hljs-keyword">interface</span> <span class="hljs-title">IRemoteService</span> {
    <span class="hljs-comment">/**
     * 获取当前进程的pid
     */</span>
    <span class="hljs-function"><span class="hljs-keyword">int</span> <span class="hljs-title">getPid</span>(<span class="hljs-params"></span>)</span>;
    <span class="hljs-comment">/**
     * 获取当前服务名称
     */</span>
    <span class="hljs-function">String <span class="hljs-title">getServiceName</span>(<span class="hljs-params"></span>)</span>;
    <span class="hljs-comment">/**
     * 处理客户端传过来的数据
     */</span>
    <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">handleData</span>(<span class="hljs-params"><span class="hljs-keyword">in</span> ParcelableData data</span>)</span>;
}</code></pre>
<p>ParcelableData.aidl</p>
<pre class="hljs css"><code class="css"><span class="hljs-selector-tag">package</span> <span class="hljs-selector-tag">com</span><span class="hljs-selector-class">.demo</span><span class="hljs-selector-class">.aidl</span>;

<span class="hljs-comment">/**
 * 声明支持传递的类
 */</span>
<span class="hljs-selector-tag">parcelable</span> <span class="hljs-selector-tag">ParcelableData</span>;</code></pre>
<p>IRemoteServiceCallBack.aidl</p>
<pre class="hljs cs"><code class="cs">package com.demo.aidl;

oneway <span class="hljs-keyword">interface</span> <span class="hljs-title">IRemoteServiceCallBack</span> {
    <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">valueChanged</span>(<span class="hljs-params"><span class="hljs-keyword">int</span> <span class="hljs-keyword">value</span></span>)</span>;
}</code></pre>
<p>aidl文件定义的接口支持的数据类型如下：</p>
<ul>
<li>Java的八种基本数据类型（byte，int，short，long，float，double，char，boolean）</li>
<li>String</li>
<li>CharSequence</li>
<li>List，List中所有的元素必须是aidl文件支持的数据类型，例如，List&lt; String &gt;</li>
<li>Map，Map中所有的元素必须是aidl文件支持的数据类型，</li>
<li>其他aidl定义的接口，要手动添加import</li>
<li>其他aidl文件中申明的类，要手动添加import</li>
</ul>
<p>aidl文件中定义的方法接受的参数，除了Java的基本数据类型和aidl定义的接口之外，其他参数都需要标记数据的走向，in/out/inout，基本数据类型和aidl定义的接口作为参数，默认是in。</p>
<ul>
<li>in表示输入参数，客户端把参数传递给服务端使用。</li>
<li>out表示输出参数，客户端将参数传递给服务端填充，然后自己使用处理。</li>
<li>inout标书输入输出参数，传送相应的值并接收返回。</li>
</ul>
<p>关键字<code>oneway</code>表示用户请求相应功能时不需要等待响应可直接调用返回，非阻塞效果，该关键字可以用来声明接口或者声明方法，如果接口声明中用到了oneway关键字，则该接口声明的所有方法都采用oneway方式</p>
<p>新建完毕aidl文件后，rebuild工程或者使用gradle assembleDebug(或gradle assembleRelease)指令编译工程，生成具体的java代码，在<code>壳工程/build/generated/aidl/</code>目录下的<code>debug</code>或者<code>release</code>文件夹下，根据build的类型而定，如图：</p>
<div class="image-package">
<img src="images/354846-a7ed31b10149aad0.png" data-original-src="http://upload-images.jianshu.io/upload_images/354846-a7ed31b10149aad0?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">图-2 adil生成代码目录图</div>
</div>
<p><strong>AIDL接口首次公布后对其的任何修改都必须保持向后兼容性，避免中断客户端对服务的使用，因为需要将.aidl文件复制给其他应用，才能使其他应用能够访问服务，所以必须保持对原始接口的支持。</strong></p>
<h3>3.2 实现接口</h3>
<p>Android SDK会根据.aidl文件生成同名.java文件，生成的接口中有一个Stub的抽象子类，这个类实现（implements）aidl定义的接口，同时继承了<code>Binder</code>。</p>
<p>具体代码如下：</p>
<pre class="hljs aspectj"><code class="aspectj"><span class="hljs-keyword">private</span> <span class="hljs-keyword">final</span> IRemoteService.Stub mBinder = <span class="hljs-keyword">new</span> IRemoteService.Stub() {
    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">int</span> <span class="hljs-title">getPid</span><span class="hljs-params">()</span> <span class="hljs-keyword">throws</span> RemoteException </span>{
        <span class="hljs-function"><span class="hljs-keyword">return</span> Process.<span class="hljs-title">myPid</span><span class="hljs-params">()</span></span>;
    }

    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-function">String <span class="hljs-title">getServiceName</span><span class="hljs-params">()</span> <span class="hljs-keyword">throws</span> RemoteException </span>{
        <span class="hljs-keyword">return</span> RemoteService.<span class="hljs-keyword">this</span>.getClass().getSimpleName();
    }

    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">handleData</span><span class="hljs-params">(ParcelableData data)</span> <span class="hljs-keyword">throws</span> RemoteException </span>{
        Toast.makeText(RemoteService.<span class="hljs-keyword">this</span>, <span class="hljs-string">"num is "</span> + data.num, Toast.LENGTH_SHORT).show();
    }

    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">registerCallback</span><span class="hljs-params">(IRemoteServiceCallBack cb)</span> <span class="hljs-keyword">throws</span> RemoteException </span>{
        <span class="hljs-keyword">if</span>(cb != <span class="hljs-keyword">null</span>) {
            mCallBacks.register(cb);
        }
    }

    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">unregisterCallback</span><span class="hljs-params">(IRemoteServiceCallBack cb)</span> <span class="hljs-keyword">throws</span> RemoteException </span>{
        <span class="hljs-keyword">if</span>(cb != <span class="hljs-keyword">null</span>) {
            mCallBacks.unregister(cb);
        }
    }
};</code></pre>
<p>现在mBinder是Stub类的一个实例，同时也是一个Binder，用于服务定义的RPC服务，作为<code>onBind()</code>方法的返回对象实例。</p>
<p>实现AIDL接口注意事项：</p>
<ul>
<li>因为AIDL可以处理多线程并发，在实现过程中要保证线程安全</li>
<li>默认情况下，RPC调用是同步的，但是服务端可能有耗时操作，客户端最好不要在主线程调用服务</li>
<li>在服务端人工抛出的任何异常不会返回给客户端</li>
</ul>
<h3>3.3 向客户端暴露接口</h3>
<p>实现接口后，需要向客户端将接口暴露出来，以便客户端使用。将Stub的实例化对象作为Service中<code>onBind()</code>方法的返回对象。</p>
<pre class="hljs aspectj"><code class="aspectj"><span class="hljs-keyword">public</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">RemoteService</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">Service</span> </span>{
    <span class="hljs-comment">/**
     * 回调容器
     */</span>
    <span class="hljs-keyword">private</span> <span class="hljs-keyword">final</span> RemoteCallbackList&lt;IRemoteServiceCallBack&gt; mCallBacks = <span class="hljs-keyword">new</span> RemoteCallbackList&lt;&gt;();
    <span class="hljs-comment">/**
     * aidl接口具体实现
     */</span>
    <span class="hljs-keyword">private</span> <span class="hljs-keyword">final</span> IRemoteService.Stub mBinder = <span class="hljs-keyword">new</span> IRemoteService.Stub() {
        <span class="hljs-meta">@Override</span>
        <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">int</span> <span class="hljs-title">getPid</span><span class="hljs-params">()</span> <span class="hljs-keyword">throws</span> RemoteException </span>{
            <span class="hljs-function"><span class="hljs-keyword">return</span> Process.<span class="hljs-title">myPid</span><span class="hljs-params">()</span></span>;
        }

        <span class="hljs-meta">@Override</span>
        <span class="hljs-keyword">public</span> <span class="hljs-function">String <span class="hljs-title">getServiceName</span><span class="hljs-params">()</span> <span class="hljs-keyword">throws</span> RemoteException </span>{
            <span class="hljs-keyword">return</span> RemoteService.<span class="hljs-keyword">this</span>.getClass().getSimpleName();
        }

        <span class="hljs-meta">@Override</span>
        <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">handleData</span><span class="hljs-params">(ParcelableData data)</span> <span class="hljs-keyword">throws</span> RemoteException </span>{
            Toast.makeText(RemoteService.<span class="hljs-keyword">this</span>, <span class="hljs-string">"num is "</span> + data.num, Toast.LENGTH_SHORT).show();
        }

        <span class="hljs-meta">@Override</span>
        <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">registerCallback</span><span class="hljs-params">(IRemoteServiceCallBack cb)</span> <span class="hljs-keyword">throws</span> RemoteException </span>{
            <span class="hljs-keyword">if</span>(cb != <span class="hljs-keyword">null</span>) {
                mCallBacks.register(cb);
            }
        }

        <span class="hljs-meta">@Override</span>
        <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">unregisterCallback</span><span class="hljs-params">(IRemoteServiceCallBack cb)</span> <span class="hljs-keyword">throws</span> RemoteException </span>{
            <span class="hljs-keyword">if</span>(cb != <span class="hljs-keyword">null</span>) {
                mCallBacks.unregister(cb);
            }
        }
    };

    <span class="hljs-meta">@Nullable</span>
    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-function">IBinder <span class="hljs-title">onBind</span><span class="hljs-params">(Intent intent)</span> </span>{
        <span class="hljs-keyword">return</span> mBinder;
    }

    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">onDestroy</span><span class="hljs-params">()</span> </span>{
        <span class="hljs-comment">// 注销所有回调</span>
        mCallBacks.kill();
    }
}</code></pre>
<h3>3.4 客户端调用</h3>
<p>服务提供给第三方应用使用，其他应用就必须要有接口类，在客户端创建相同的aidl文件（可以直接拷贝过去）。</p>
<p>核心连接远端服务的代码：</p>
<pre class="hljs cs"><code class="cs"><span class="hljs-comment">/**
 * 远端服务
 */</span>
<span class="hljs-keyword">private</span> IRemoteService mService;

<span class="hljs-keyword">private</span> ServiceConnection mConnection = <span class="hljs-keyword">new</span> ServiceConnection() {
    <span class="hljs-comment">/**
     * 连接服务器成功回调
     * 
     * @param className
     * @param service
     */</span>
    <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">onServiceConnected</span>(<span class="hljs-params">ComponentName className, IBinder service</span>) </span>{
        mService = IRemoteService.Stub.asInterface(service);
    }

    <span class="hljs-comment">/**
     * 服务器因为一场情况断开连接时候回调
     * 
     * @param className
     */</span>
    <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">onServiceDisconnected</span>(<span class="hljs-params">ComponentName className</span>) </span>{
        mService = <span class="hljs-literal">null</span>;
    }
};

<span class="hljs-comment">/**
 * 绑定服务
 */</span>
<span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">void</span> <span class="hljs-title">doBindService</span>(<span class="hljs-params"></span>) </span>{
    isBound = <span class="hljs-literal">true</span>;
    Intent intent = <span class="hljs-keyword">new</span> Intent(BindRemoteServiceActivity.<span class="hljs-keyword">this</span>, RemoteService.class);
    bindService(intent, mConnection, Context.BIND_AUTO_CREATE);
}

<span class="hljs-comment">/**
 * 解除绑定
 */</span>
<span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">void</span> <span class="hljs-title">doUnbindService</span>(<span class="hljs-params"></span>) </span>{
    <span class="hljs-keyword">if</span>(isBound &amp;&amp; mService != <span class="hljs-literal">null</span>) {
        isBound = <span class="hljs-literal">false</span>;
        <span class="hljs-keyword">try</span> {
            mService.unregisterCallback(mCallback);
        } <span class="hljs-keyword">catch</span> (RemoteException e) {
            e.printStackTrace();
        }

        unbindService(mConnection);
    }
}

<span class="hljs-comment">/**
 * 向服务端发送信息
 */</span>
<span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">void</span> <span class="hljs-title">doSendMsg</span>(<span class="hljs-params"></span>) </span>{
    <span class="hljs-keyword">if</span>(!isBound || mService == <span class="hljs-literal">null</span>) {
        <span class="hljs-keyword">return</span>;
    }
    ParcelableData data = <span class="hljs-keyword">new</span> ParcelableData(<span class="hljs-number">1</span>);
    <span class="hljs-keyword">try</span> {
        mService.handleData(data);
    } <span class="hljs-keyword">catch</span> (RemoteException e) {
        e.printStackTrace();
    }
}

<span class="hljs-comment">/**
 * 调用服务端方法获取信息
 */</span>
<span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">void</span> <span class="hljs-title">doGetServiceInfo</span>(<span class="hljs-params"></span>) </span>{
    <span class="hljs-keyword">if</span>(!isBound || mService == <span class="hljs-literal">null</span>) {
        <span class="hljs-keyword">return</span>;
    }
    <span class="hljs-keyword">try</span> {
        String info = mService.getServiceName();

        mInfoTv.setText(<span class="hljs-string">"Service info :"</span> + info);
    } <span class="hljs-keyword">catch</span> (RemoteException e) {
        e.printStackTrace();
    }
}</code></pre>
<p>详情代码贴上来比较长，贴上工程地址，<a href="https://github.com/Kyogirante/AIDLDemo" target="_blank">点我呀！！！</a></p>
<h2>4. Messenger的使用</h2>
<p>Messenger的使用相对于AIDL方便好多，因为Messenger是Android系统中自带的类，服务端和客户端都不用创建AIDL文件。</p>
<p>Messenger会持有一个Handler，这个Handler用于处理接受到的信息，在服务端和乘客通过Messenger实现双方通信。</p>
<h3>4.1 服务端</h3>
<p>代码实例：</p>
<pre class="hljs aspectj"><code class="aspectj"><span class="hljs-keyword">public</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">MessengerService</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">Service</span> </span>{

    <span class="hljs-keyword">public</span> <span class="hljs-keyword">static</span> <span class="hljs-keyword">final</span> <span class="hljs-keyword">int</span> MSG_REGISTER_CLIENT = <span class="hljs-number">0X001</span>;
    <span class="hljs-keyword">public</span> <span class="hljs-keyword">static</span> <span class="hljs-keyword">final</span> <span class="hljs-keyword">int</span> MSG_UNREGISTER_CLIENT = <span class="hljs-number">0X010</span>;
    <span class="hljs-keyword">public</span> <span class="hljs-keyword">static</span> <span class="hljs-keyword">final</span> <span class="hljs-keyword">int</span> MSG_HANDLE = <span class="hljs-number">0X100</span>;

    <span class="hljs-keyword">private</span> ArrayList&lt;Messenger&gt; mClients = <span class="hljs-keyword">new</span> ArrayList&lt;&gt;();

    <span class="hljs-keyword">private</span> <span class="hljs-keyword">final</span> Messenger mMessenger = <span class="hljs-keyword">new</span> Messenger(<span class="hljs-keyword">new</span> IncomingHandler());

    <span class="hljs-meta">@Nullable</span>
    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-function">IBinder <span class="hljs-title">onBind</span><span class="hljs-params">(Intent intent)</span> </span>{
        <span class="hljs-function"><span class="hljs-keyword">return</span> mMessenger.<span class="hljs-title">getBinder</span><span class="hljs-params">()</span></span>;
    }

    <span class="hljs-meta">@Override</span>
    <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">onDestroy</span><span class="hljs-params">()</span> </span>{
        Toast.makeText(<span class="hljs-keyword">this</span>, <span class="hljs-string">"Remote Service Destroy"</span>, Toast.LENGTH_SHORT).show();
    }

    <span class="hljs-keyword">private</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">IncomingHandler</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">Handler</span> </span>{
        <span class="hljs-meta">@Override</span>
        <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">handleMessage</span><span class="hljs-params">(Message msg)</span> </span>{
            <span class="hljs-keyword">switch</span> (msg.what) {
                <span class="hljs-keyword">case</span> MSG_REGISTER_CLIENT:
                    mClients.add(msg.replyTo);
                    <span class="hljs-keyword">break</span>;
                <span class="hljs-keyword">case</span> MSG_UNREGISTER_CLIENT:
                    mClients.remove(msg.replyTo);
                    <span class="hljs-keyword">break</span>;
                <span class="hljs-keyword">case</span> MSG_HANDLE:
                    <span class="hljs-keyword">for</span> (Messenger mClient : mClients) {
                        <span class="hljs-keyword">try</span> {
                            mClient.send(Message.obtain(<span class="hljs-keyword">null</span>, MSG_HANDLE, msg.arg1, <span class="hljs-number">0</span>));
                        } <span class="hljs-keyword">catch</span> (RemoteException e) {
                            e.printStackTrace();
                            mClients.remove(mClient);
                        }
                    }
                    <span class="hljs-keyword">break</span>;
                <span class="hljs-keyword">default</span>:
                    <span class="hljs-keyword">super</span>.handleMessage(msg);
            }
        }
    };
}</code></pre>
<h3>4.2 客户端</h3>
<p>核心代码：</p>
<pre class="hljs cs"><code class="cs"><span class="hljs-comment">/**
 * 关联远端服务的messenger
 */</span>
<span class="hljs-keyword">private</span> Messenger mServiceWrapper;
<span class="hljs-comment">/**
 * 用于处理服务端发送的信息
 */</span>
final Messenger mMessenger = <span class="hljs-keyword">new</span> Messenger(<span class="hljs-keyword">new</span> IncomingHandler());

<span class="hljs-keyword">private</span> ServiceConnection mConnection = <span class="hljs-keyword">new</span> ServiceConnection() {
    @<span class="hljs-function">Override
    <span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">onServiceConnected</span>(<span class="hljs-params">ComponentName name, IBinder service</span>) </span>{
        mServiceWrapper = <span class="hljs-keyword">new</span> Messenger(service);

        mInfoTv.setText(<span class="hljs-string">"Connected Service"</span>);


        <span class="hljs-keyword">try</span> {
            <span class="hljs-comment">// 添加监听注册</span>
            Message msg = Message.obtain(<span class="hljs-literal">null</span>, MessengerService.MSG_REGISTER_CLIENT);
            msg.replyTo = mMessenger;
            mServiceWrapper.send(msg);
        } <span class="hljs-keyword">catch</span> (RemoteException e) {
            e.printStackTrace();
        }
    }

    @<span class="hljs-function">Override
    <span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">onServiceDisconnected</span>(<span class="hljs-params">ComponentName name</span>) </span>{
        mServiceWrapper = <span class="hljs-literal">null</span>;
        mInfoTv.setText(<span class="hljs-string">"Disconnected"</span>);
    }
};

<span class="hljs-comment">/**
 * 绑定服务
 */</span>
<span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">void</span> <span class="hljs-title">doBindService</span>(<span class="hljs-params"></span>) </span>{
    <span class="hljs-keyword">if</span>(!isBound) {
        bindService(<span class="hljs-keyword">new</span> Intent(<span class="hljs-keyword">this</span>, MessengerService.class), mConnection, Context.BIND_AUTO_CREATE);

        isBound = <span class="hljs-literal">true</span>;

        mInfoTv.setText(<span class="hljs-string">"Binding..."</span>);
    }
}

<span class="hljs-comment">/**
 * 移除监听并解绑服务
 */</span>
<span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">void</span> <span class="hljs-title">doUnbindService</span>(<span class="hljs-params"></span>) </span>{
    <span class="hljs-keyword">if</span>(isBound) {
        <span class="hljs-keyword">if</span>(mServiceWrapper != <span class="hljs-literal">null</span>) {
            <span class="hljs-keyword">try</span> {
                Message msg = Message.obtain(<span class="hljs-literal">null</span>, MessengerService.MSG_UNREGISTER_CLIENT);
                msg.replyTo = mMessenger;
                mServiceWrapper.send(msg);
            } <span class="hljs-keyword">catch</span> (RemoteException e) {
                e.printStackTrace();
            }
        }

        unbindService(mConnection);
        isBound = <span class="hljs-literal">false</span>;
        mInfoTv.setText(<span class="hljs-string">"Unbinding..."</span>);
    }
}

<span class="hljs-comment">/**
 * 向服务端发送信息
 */</span>
<span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">void</span> <span class="hljs-title">doSendMsg</span>(<span class="hljs-params"></span>) </span>{
    <span class="hljs-keyword">if</span>(mServiceWrapper != <span class="hljs-literal">null</span>) {
        <span class="hljs-keyword">try</span> {
            Message msg = Message.obtain(<span class="hljs-literal">null</span>,
                    MessengerService.MSG_HANDLE, <span class="hljs-keyword">this</span>.hashCode(), <span class="hljs-number">0</span>);
            mServiceWrapper.send(msg);
        } <span class="hljs-keyword">catch</span> (RemoteException e) {
            e.printStackTrace();
        }
    }
}</code></pre>
<h3>4.3 客户端发送信息</h3>
<p>使用Messenger向服务端发送信息，使用的是<code>Messenger.send(Message)</code>方法，这个方法具体实现如下：</p>
<pre class="hljs aspectj"><code class="aspectj"><span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">send</span><span class="hljs-params">(Message message)</span> <span class="hljs-keyword">throws</span> RemoteException </span>{
    mTarget.send(message);
}</code></pre>
<p>方法内部调用<code>mTarget.send(Message)</code>方法，在Messenger中，<code>mTarget</code>是在构造方法里面被赋值的，有两个构造函数。</p>
<pre class="hljs aspectj"><code class="aspectj"><span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-title">Messenger</span><span class="hljs-params">(Handler <span class="hljs-keyword">target</span>)</span> </span>{
    mTarget = <span class="hljs-keyword">target</span>.getIMessenger();
}

<span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-title">Messenger</span><span class="hljs-params">(IBinder <span class="hljs-keyword">target</span>)</span> </span>{
    mTarget = IMessenger.Stub.asInterface(<span class="hljs-keyword">target</span>);
}</code></pre>
<p>第一个构造函数好理解，<code>mTarget.send(Message)</code>实际上是将<code>Message</code>加入了构造函数传入的Handler的消息队列，Demo工程中服务端向乘客端发送信息就是使用的这种方法</p>
<p>第二个构造函数是不是很眼熟，这不就是获取AIDL定义的接口嘛！！！转了一圈回到了上面的AIDL，客户端向服务端发送信息实际上还是通过AIDL，只不过Android系统帮我们做了一层封装。</p>
<h2>5. 总结</h2>
<p>到此，从<a href="http://www.jianshu.com/p/a1d3d9693e91" target="_blank">Android之Service总结</a>到Android中常用的进程通信都已经总结完毕，算是2016的一个完结，撒花！！</p>
<p>最后附上Demo工程地址：<a href="https://github.com/Kyogirante/AIDLDemo" target="_blank">https://github.com/Kyogirante/AIDLDemo</a></p>

        </div>