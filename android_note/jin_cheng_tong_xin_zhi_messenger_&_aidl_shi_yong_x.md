# 進程通信之Messenger&AIDL使用詳解


<div class="show-content">
          <h2>1. 前言</h2>
<p>提到的進程間通信（IPC：Inter-Process Communication），在Android系統中，一個進程是不能直接訪問另一個進程的內存的，需要提供一些機制在不同的進程之間進行通信，Android官方推出了AIDL(Android Interface Definition Language)，它是基於Binder機制的，至於官方為什麼要採用Binder，查看<a href="https://www.zhihu.com/question/39440766" target="_blank">為什麼Android要採用Binder作為IPC機制</a>，分析很全面。</p>
<p>上篇<a href="http://www.jianshu.com/p/a1d3d9693e91" target="_blank">Android之Service總結
</a>提到組件在與Service通信方法有三種。</p>
<ul>
<li>實現IBinder</li>
<li>Messenger</li>
<li>AIDL</li>
</ul>
<p>後面兩種可以跨進程通信，是基於Binder機制的通信方式。</p>
<h2>2. 使用時機</h2>
<p>在確定使用什麼機制之前，首先了解應用場景。Android系統中，如果組件與服務通信是在同一進程，就使用第一種方式；如果是跨進程通信，使用第二種和第三種，兩者不同在於，Messenger不能處理多線程併發請求。</p>
<h2>3. AIDL使用</h2>
<p>AIDL是可以處理多線程訪問的請求的，所以實現AIDL首先要保證線程安全。</p>
<ul>
<li>創建.aidl文件，定義接口</li>
<li>在代碼中實現接口，Android SDK會根據aidl文件，生成接口，接口內部有一個名為Stub內部抽象類，這個類用於繼承了Binder類並實現aidl文件中定義的接口，我們需要拓展Stub類並實現裡面的抽象方法</li>
<li>複寫Service的<code>onBind()</code>，返回Stub類的實現</li>
</ul>
<h3>3.1 創建.aidl文件</h3>
<p>在Android Studio中工程目錄下，反鍵<code>new -&gt; AIDL -&gt; AIDL FIle</code>，可以新建aidl文件，編譯器會自動在<code>app(殼工程)/src/main/</code>目錄下新建aidl文件，同時也會新建文件夾，默認以工程包名作為aidl文件所在的目錄。</p>
<p>目錄結構如下：</p>
<div class="image-package">

<img src="images/354846-006d3a6a69c932e9.png" data-original-src="http://upload-images.jianshu.io/upload_images/354846-006d3a6a69c932e9?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">圖-1 aidl文件目錄結構</div>
</div>
<p>也可以手動創建，aidl接口定義的包名也可以和工程包名不同，aidl文件語法和Java語法相似，aidl定義的接口名必須和文件名一致，而且支持傳遞自定義的數據類型，需要實現parcelable接口。</p>
<p>IRemoteService.aidl</p>
<pre class="hljs cs"><code class="cs">package com.demo.aidl;

import com.demo.aidl.ParcelableData;

<span class="hljs-keyword">interface</span> <span class="hljs-title">IRemoteService</span> {
    <span class="hljs-comment">/**
     * 獲取當前進程的pid
     */</span>
    <span class="hljs-function"><span class="hljs-keyword">int</span> <span class="hljs-title">getPid</span>(<span class="hljs-params"></span>)</span>;
    <span class="hljs-comment">/**
     * 獲取當前服務名稱
     */</span>
    <span class="hljs-function">String <span class="hljs-title">getServiceName</span>(<span class="hljs-params"></span>)</span>;
    <span class="hljs-comment">/**
     * 處理客戶端傳過來的數據
     */</span>
    <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">handleData</span>(<span class="hljs-params"><span class="hljs-keyword">in</span> ParcelableData data</span>)</span>;
}</code></pre>
<p>ParcelableData.aidl</p>
<pre class="hljs css"><code class="css"><span class="hljs-selector-tag">package</span> <span class="hljs-selector-tag">com</span><span class="hljs-selector-class">.demo</span><span class="hljs-selector-class">.aidl</span>;

<span class="hljs-comment">/**
 * 聲明支持傳遞的類
 */</span>
<span class="hljs-selector-tag">parcelable</span> <span class="hljs-selector-tag">ParcelableData</span>;</code></pre>
<p>IRemoteServiceCallBack.aidl</p>
<pre class="hljs cs"><code class="cs">package com.demo.aidl;

oneway <span class="hljs-keyword">interface</span> <span class="hljs-title">IRemoteServiceCallBack</span> {
    <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">valueChanged</span>(<span class="hljs-params"><span class="hljs-keyword">int</span> <span class="hljs-keyword">value</span></span>)</span>;
}</code></pre>
<p>aidl文件定義的接口支持的數據類型如下：</p>
<ul>
<li>Java的八種基本數據類型（byte，int，short，long，float，double，char，boolean）</li>
<li>String</li>
<li>CharSequence</li>
<li>List，List中所有的元素必須是aidl文件支持的數據類型，例如，List&lt; String &gt;</li>
<li>Map，Map中所有的元素必須是aidl文件支持的數據類型，</li>
<li>其他aidl定義的接口，要手動添加import</li>
<li>其他aidl文件中申明的類，要手動添加import</li>
</ul>
<p>aidl文件中定義的方法接受的參數，除了Java的基本數據類型和aidl定義的接口之外，其他參數都需要標記數據的走向，in/out/inout，基本數據類型和aidl定義的接口作為參數，默認是in。</p>
<ul>
<li>in表示輸入參數，客戶端把參數傳遞給服務端使用。</li>
<li>out表示輸出參數，客戶端將參數傳遞給服務端填充，然後自己使用處理。</li>
<li>inout標書輸入輸出參數，傳送相應的值並接收返回。</li>
</ul>
<p>關鍵字<code>oneway</code>表示用戶請求相應功能時不需要等待響應可直接調用返回，非阻塞效果，該關鍵字可以用來聲明接口或者聲明方法，如果接口聲明中用到了oneway關鍵字，則該接口聲明的所有方法都採用oneway方式</p>
<p>新建完畢aidl文件後，rebuild工程或者使用gradle assembleDebug(或gradle assembleRelease)指令編譯工程，生成具體的java代碼，在<code>殼工程/build/generated/aidl/</code>目錄下的<code>debug</code>或者<code>release</code>文件夾下，根據build的類型而定，如圖：</p>
<div class="image-package">
<img src="images/354846-a7ed31b10149aad0.png" data-original-src="http://upload-images.jianshu.io/upload_images/354846-a7ed31b10149aad0?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">圖-2 adil生成代碼目錄圖</div>
</div>
<p><strong>AIDL接口首次公佈後對其的任何修改都必須保持向後兼容性，避免中斷客戶端對服務的使用，因為需要將.aidl文件複製給其他應用，才能使其他應用能夠訪問服務，所以必須保持對原始接口的支持。</strong></p>
<h3>3.2 實現接口</h3>
<p>Android SDK會根據.aidl文件生成同名.java文件，生成的接口中有一個Stub的抽象子類，這個類實現（implements）aidl定義的接口，同時繼承了<code>Binder</code>。</p>
<p>具體代碼如下：</p>
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
<p>現在mBinder是Stub類的一個實例，同時也是一個Binder，用於服務定義的RPC服務，作為<code>onBind()</code>方法的返回對象實例。</p>
<p>實現AIDL接口注意事項：</p>
<ul>
<li>因為AIDL可以處理多線程併發，在實現過程中要保證線程安全</li>
<li>默認情況下，RPC調用是同步的，但是服務端可能有耗時操作，客戶端最好不要在主線程調用服務</li>
<li>在服務端人工拋出的任何異常不會返回給客戶端</li>
</ul>
<h3>3.3 向客戶端暴露接口</h3>
<p>實現接口後，需要向客戶端將接口暴露出來，以便客戶端使用。將Stub的實例化對象作為Service中<code>onBind()</code>方法的返回對象。</p>
<pre class="hljs aspectj"><code class="aspectj"><span class="hljs-keyword">public</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">RemoteService</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">Service</span> </span>{
    <span class="hljs-comment">/**
     * 回調容器
     */</span>
    <span class="hljs-keyword">private</span> <span class="hljs-keyword">final</span> RemoteCallbackList&lt;IRemoteServiceCallBack&gt; mCallBacks = <span class="hljs-keyword">new</span> RemoteCallbackList&lt;&gt;();
    <span class="hljs-comment">/**
     * aidl接口具體實現
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
        <span class="hljs-comment">// 註銷所有回調</span>
        mCallBacks.kill();
    }
}</code></pre>
<h3>3.4 客戶端調用</h3>
<p>服務提供給第三方應用使用，其他應用就必須要有接口類，在客戶端創建相同的aidl文件（可以直接拷貝過去）。</p>
<p>核心連接遠端服務的代碼：</p>
<pre class="hljs cs"><code class="cs"><span class="hljs-comment">/**
 * 遠端服務
 */</span>
<span class="hljs-keyword">private</span> IRemoteService mService;

<span class="hljs-keyword">private</span> ServiceConnection mConnection = <span class="hljs-keyword">new</span> ServiceConnection() {
    <span class="hljs-comment">/**
     * 連接服務器成功回調
     * 
     * @param className
     * @param service
     */</span>
    <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">onServiceConnected</span>(<span class="hljs-params">ComponentName className, IBinder service</span>) </span>{
        mService = IRemoteService.Stub.asInterface(service);
    }

    <span class="hljs-comment">/**
     * 服務器因為一場情況斷開連接時候回調
     * 
     * @param className
     */</span>
    <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">onServiceDisconnected</span>(<span class="hljs-params">ComponentName className</span>) </span>{
        mService = <span class="hljs-literal">null</span>;
    }
};

<span class="hljs-comment">/**
 * 綁定服務
 */</span>
<span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">void</span> <span class="hljs-title">doBindService</span>(<span class="hljs-params"></span>) </span>{
    isBound = <span class="hljs-literal">true</span>;
    Intent intent = <span class="hljs-keyword">new</span> Intent(BindRemoteServiceActivity.<span class="hljs-keyword">this</span>, RemoteService.class);
    bindService(intent, mConnection, Context.BIND_AUTO_CREATE);
}

<span class="hljs-comment">/**
 * 解除綁定
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
 * 向服務端發送信息
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
 * 調用服務端方法獲取信息
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
<p>詳情代碼貼上來比較長，貼上工程地址，<a href="https://github.com/Kyogirante/AIDLDemo" target="_blank">點我呀！！！</a></p>
<h2>4. Messenger的使用</h2>
<p>Messenger的使用相對於AIDL方便好多，因為Messenger是Android系統中自帶的類，服務端和客戶端都不用創建AIDL文件。</p>
<p>Messenger會持有一個Handler，這個Handler用於處理接受到的信息，在服務端和乘客通過Messenger實現雙方通信。</p>
<h3>4.1 服務端</h3>
<p>代碼實例：</p>
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
<h3>4.2 客戶端</h3>
<p>核心代碼：</p>
<pre class="hljs cs"><code class="cs"><span class="hljs-comment">/**
 * 關聯遠端服務的messenger
 */</span>
<span class="hljs-keyword">private</span> Messenger mServiceWrapper;
<span class="hljs-comment">/**
 * 用於處理服務端發送的信息
 */</span>
final Messenger mMessenger = <span class="hljs-keyword">new</span> Messenger(<span class="hljs-keyword">new</span> IncomingHandler());

<span class="hljs-keyword">private</span> ServiceConnection mConnection = <span class="hljs-keyword">new</span> ServiceConnection() {
    @<span class="hljs-function">Override
    <span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">onServiceConnected</span>(<span class="hljs-params">ComponentName name, IBinder service</span>) </span>{
        mServiceWrapper = <span class="hljs-keyword">new</span> Messenger(service);

        mInfoTv.setText(<span class="hljs-string">"Connected Service"</span>);


        <span class="hljs-keyword">try</span> {
            <span class="hljs-comment">// 添加監聽註冊</span>
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
 * 綁定服務
 */</span>
<span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">void</span> <span class="hljs-title">doBindService</span>(<span class="hljs-params"></span>) </span>{
    <span class="hljs-keyword">if</span>(!isBound) {
        bindService(<span class="hljs-keyword">new</span> Intent(<span class="hljs-keyword">this</span>, MessengerService.class), mConnection, Context.BIND_AUTO_CREATE);

        isBound = <span class="hljs-literal">true</span>;

        mInfoTv.setText(<span class="hljs-string">"Binding..."</span>);
    }
}

<span class="hljs-comment">/**
 * 移除監聽並解綁服務
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
 * 向服務端發送信息
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
<h3>4.3 客戶端發送信息</h3>
<p>使用Messenger向服務端發送信息，使用的是<code>Messenger.send(Message)</code>方法，這個方法具體實現如下：</p>
<pre class="hljs aspectj"><code class="aspectj"><span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">send</span><span class="hljs-params">(Message message)</span> <span class="hljs-keyword">throws</span> RemoteException </span>{
    mTarget.send(message);
}</code></pre>
<p>方法內部調用<code>mTarget.send(Message)</code>方法，在Messenger中，<code>mTarget</code>是在構造方法裡面被賦值的，有兩個構造函數。</p>
<pre class="hljs aspectj"><code class="aspectj"><span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-title">Messenger</span><span class="hljs-params">(Handler <span class="hljs-keyword">target</span>)</span> </span>{
    mTarget = <span class="hljs-keyword">target</span>.getIMessenger();
}

<span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-title">Messenger</span><span class="hljs-params">(IBinder <span class="hljs-keyword">target</span>)</span> </span>{
    mTarget = IMessenger.Stub.asInterface(<span class="hljs-keyword">target</span>);
}</code></pre>
<p>第一個構造函數好理解，<code>mTarget.send(Message)</code>實際上是將<code>Message</code>加入了構造函數傳入的Handler的消息隊列，Demo工程中服務端向乘客端發送信息就是使用的這種方法</p>
<p>第二個構造函數是不是很眼熟，這不就是獲取AIDL定義的接口嘛！！！轉了一圈回到了上面的AIDL，客戶端向服務端發送信息實際上還是通過AIDL，只不過Android系統幫我們做了一層封裝。</p>
<h2>5. 總結</h2>
<p>到此，從<a href="http://www.jianshu.com/p/a1d3d9693e91" target="_blank">Android之Service總結</a>到Android中常用的進程通信都已經總結完畢，算是2016的一個完結，撒花！！</p>
<p>最後附上Demo工程地址：<a href="https://github.com/Kyogirante/AIDLDemo" target="_blank">https://github.com/Kyogirante/AIDLDemo</a></p>

        </div>