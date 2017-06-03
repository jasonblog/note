# Looper


<div class="show-content">
          <p>Looper在Android消息機制中的主要作用就是一直循環從MessageQueue中取Message，取出Message後交給它的target，該target是一個Handler對象，消息交給Handler後通過調用Handler的<code>dispatchMessage()</code>方法進行處理。</p>
<h2>類成員</h2>
<table>
<thead>
<tr>
<th>filed</th>
<th>含義</th>
<th>說明</th>
</tr>
</thead>
<tbody>
<tr>
<td>sThreadLocal</td>
<td>Looper 對象</td>
<td>每個線程中的Looper對象其實是一個ThreadLocal，即線程本地存儲(TLS)對象</td>
</tr>
<tr>
<td>sMainLooper</td>
<td>主線程使用的Looper對象</td>
<td>由系統在ActivityThread主線程中創建。</td>
</tr>
<tr>
<td>mQueue</td>
<td>和Looper對應的消息隊列</td>
<td>一個Looper依賴一個消息隊列（一對一）</td>
</tr>
<tr>
<td>mThread</td>
<td>和Looper對應的線程</td>
<td>一個Looper和一個線程綁定（一對一）</td>
</tr>
</tbody>
</table>
<h2>Looper工作過程</h2>
<ol>
<li>
<p><strong>為線程創建消息循環</strong><br>使用Looper對象時，會先調用靜態的prepare方法或者prepareMainLooper方法來創建線程的Looper對象。如果是主線程會調用prepareMainLooper，如果是普通線程只需調用prepare方法，兩者都會調用prepare(boolean quitAllowed)方法，該方法源碼如下：</p>
<pre class="hljs java"><code class="java"> <span class="hljs-comment">/**
  * 該方法會創建Looper對象，Looper對象的構造方法中會創建一個MessageQueue對象，再將Looper對象保存到當前線程 TLS
  * <span class="hljs-doctag">@param</span> quitAllowed
  */</span>
 <span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">static</span> <span class="hljs-keyword">void</span> <span class="hljs-title">prepare</span><span class="hljs-params">(<span class="hljs-keyword">boolean</span> quitAllowed)</span> </span>{
     <span class="hljs-keyword">if</span> (sThreadLocal.get() != <span class="hljs-keyword">null</span>) {
         <span class="hljs-comment">// 試圖在有Looper的線程中再次創建Looper將拋出異常，一個線程只能有一個looper。</span>
         <span class="hljs-keyword">throw</span> <span class="hljs-keyword">new</span> RuntimeException(<span class="hljs-string">"Only one Looper may be created per thread"</span>);
     }
     <span class="hljs-comment">// 我們調用該方法會在調用線程的TLS中創建Looper對象</span>
     sThreadLocal.set(<span class="hljs-keyword">new</span> Looper(quitAllowed));
 }</code></pre>
<blockquote><p>第一次調用prepare()方法後，新創建出來的當前線程對應的Looper對象就被存儲到一個<code>TLS</code>對象中，如果重複調用，就會報錯。</p></blockquote>
</li>
<li>
<p><strong>開啟消息循環</strong><br>Looper類乃至Android消息處理機制的核心部分，在使用Looper時，調用完<code>Looper.prepare()</code>後，還需要調用Looper.loop()方法開啟消息循環。該方法是一個死循環會將不斷重複下面的操作，直到沒有消息時退出循環。</p>
<ol>
<li>讀取MessageQueue的下一條Message</li>
<li>把Message分發給相應的target（Handler）來處理</li>
<li>
<p>把分發後的Message，回收到消息池以複用</p>
<pre class="hljs java"><code class="java"><span class="hljs-comment">/**
* 在這個線程中啟動隊列，請確保在循環結束時候調用{<span class="hljs-doctag">@link</span> #quit()}
*
* Run the message queue in this thread. Be sure to call
* {<span class="hljs-doctag">@link</span> #quit()} to end the loop.
*/</span>
<span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">static</span> <span class="hljs-keyword">void</span> <span class="hljs-title">loop</span><span class="hljs-params">()</span> </span>{
  <span class="hljs-keyword">final</span> Looper me = myLooper();<span class="hljs-comment">//獲取TLS存儲的Looper對象</span>
  <span class="hljs-keyword">if</span> (me == <span class="hljs-keyword">null</span>) {<span class="hljs-comment">//如果沒有調用Loop.prepare()的話，就會拋出下面這個異常</span>
      <span class="hljs-keyword">throw</span> <span class="hljs-keyword">new</span> RuntimeException(<span class="hljs-string">"No Looper; Looper.prepare() wasn't called on this thread."</span>);
  }
  <span class="hljs-keyword">final</span> MessageQueue queue = me.mQueue;<span class="hljs-comment">//從Looper中取出消息隊列</span>

  <span class="hljs-comment">// Make sure the identity of this thread is that of the local process,</span>
  <span class="hljs-comment">// and keep track of what that identity token actually is.</span>
  Binder.clearCallingIdentity();
  <span class="hljs-comment">//確保在權限檢查時基於本地進程，而不是基於最初調用進程。</span>
  <span class="hljs-keyword">final</span> <span class="hljs-keyword">long</span> ident = Binder.clearCallingIdentity();

  <span class="hljs-comment">//死循環，循環的取消息，沒有新消息就會阻塞</span>
  <span class="hljs-keyword">for</span> (;;) {
      <span class="hljs-comment">//調用MessageQueue的next方法來獲取新消息，而，next是一個阻塞方法，沒有消息時，loop方法將跟隨next方法會一直阻塞在這裡。</span>
      Message msg = queue.next(); <span class="hljs-comment">// might block,如果沒有新消息,這裡會被阻塞。</span>
      <span class="hljs-comment">//因為以上獲取消息是阻塞方法，所以，當消息隊列中沒有消息時，將阻塞在上一步。而如果上一步拿到了一個空消息，只能說明</span>
      <span class="hljs-comment">//我們退出了該消息隊列。那麼這裡直接退出</span>
      <span class="hljs-keyword">if</span> (msg == <span class="hljs-keyword">null</span>) {
          <span class="hljs-comment">// No message indicates that the message queue is quitting.</span>
          <span class="hljs-comment">//沒有消息意味著消息隊列正在退出。這也就是為什麼Looper的quit(）方法中只需要退出消息隊列即可。</span>
          <span class="hljs-keyword">return</span>;
      }

      <span class="hljs-comment">// This must be in a local variable, in case a UI event sets the logger</span>
      Printer logging = me.mLogging;<span class="hljs-comment">//默認為null，可通過setMessageLogging()方法來指定輸出，用於debug功能</span>
      <span class="hljs-keyword">if</span> (logging != <span class="hljs-keyword">null</span>) {
          logging.println(<span class="hljs-string">"&gt;&gt;&gt;&gt;&gt; Dispatching to "</span> + msg.target + <span class="hljs-string">" "</span> +
                  msg.callback + <span class="hljs-string">": "</span> + msg.what);
      }

      msg.target.dispatchMessage(msg); <span class="hljs-comment">//msg.target就是與此線程關聯的Handler對象，調用它的dispatchMessage處理消息</span>

      <span class="hljs-keyword">if</span> (logging != <span class="hljs-keyword">null</span>) {
          logging.println(<span class="hljs-string">"&lt;&lt;&lt;&lt;&lt; Finished to "</span> + msg.target + <span class="hljs-string">" "</span> + msg.callback);
      }

      <span class="hljs-comment">// Make sure that during the course of dispatching the</span>
      <span class="hljs-comment">// identity of the thread wasn't corrupted.</span>
      <span class="hljs-keyword">final</span> <span class="hljs-keyword">long</span> newIdent = Binder.clearCallingIdentity();<span class="hljs-comment">//確保分發過程中identity不會損壞</span>
      <span class="hljs-keyword">if</span> (ident != newIdent) {
          Log.wtf(TAG, <span class="hljs-string">"Thread identity changed from 0x"</span>
                  + Long.toHexString(ident) + <span class="hljs-string">" to 0x"</span>
                  + Long.toHexString(newIdent) + <span class="hljs-string">" while dispatching to "</span>
                  + msg.target.getClass().getName() + <span class="hljs-string">" "</span>
                  + msg.callback + <span class="hljs-string">" what="</span> + msg.what);
      }

      msg.recycleUnchecked(); <span class="hljs-comment">//將已經處理過的消息會受到消息池</span>
  }
}</code></pre>
<blockquote><p>上面代碼中可以看到有logging方法，這是用於debug的，默認情況下logging == null，通過設置setMessageLogging()用來開啟debug工作。</p></blockquote>
</li>
</ol>
</li>
<li>
<p><strong>獲得消息循環</strong><br><code>myLooper()</code>方法用於獲取當前消息循環對象。Looper對象從成員變量 sThreadLocal(<code>線程本地存儲(TLS)對象</code>) 中獲取。</p>
<blockquote><p>獲得的Looper對象可以作為Handler的構建函數參數，將在下篇文章中說明。</p></blockquote>
</li>
<li>
<strong>退出消息循環</strong><br>主要是退出消息隊列:<pre class="hljs java"><code class="java">  <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">quit</span><span class="hljs-params">()</span> </span>{
      mQueue.quit(<span class="hljs-keyword">false</span>);<span class="hljs-comment">//消息移除</span>
  }
  <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">quitSafely</span><span class="hljs-params">()</span> </span>{
      mQueue.quit(<span class="hljs-keyword">true</span>);
  }</code></pre>
</li>
</ol>
<h2>一些其他方法</h2>
<ul>
<li>
<strong>Looper構造方法</strong><br>Looper在執行靜態方法<code>Looper.loop()</code>時調用Looper的構造函數（代碼見上文）。在Looper初始化時，新建了一個MessageQueue的對象保存了在成員mQueue中。Looper是依賴於一個線程和一個消息隊列的。<pre class="hljs java"><code class="java">  <span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-title">Looper</span><span class="hljs-params">(<span class="hljs-keyword">boolean</span> quitAllowed)</span> </span>{
      <span class="hljs-comment">// 每個Looper對象中有它的消息隊列，和它所屬的線程</span>
      mQueue = <span class="hljs-keyword">new</span> MessageQueue(quitAllowed);
      mThread = Thread.currentThread();
  }</code></pre>
</li>
<li>
<strong>prepareMainLooper()</strong><br>該方法只在主線程中調用，系統已幫我們做好，我們一般不用也不能調用。</li>
</ul>

        </div>