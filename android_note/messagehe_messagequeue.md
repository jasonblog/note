# Message和MessageQueue


<div class="show-content">
       
<h3>消息結構</h3>
<p>每個消息用Message表示，Message主要包含以下內容：</p>
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
<td>what</td>
<td>消息類別</td>
<td>由用戶定義，用來區分不同的消息</td>
</tr>
<tr>
<td>arg1</td>
<td>參數1</td>
<td>是一種輕量級的傳遞數據的方式</td>
</tr>
<tr>
<td>arg2</td>
<td>參數2</td>
<td>是一種輕量級的傳遞數據的方式</td>
</tr>
<tr>
<td>obj</td>
<td>消息內容</td>
<td>任意對象，但是使用Messenger跨進程傳遞Message時不能為null</td>
</tr>
<tr>
<td>data</td>
<td>Bundle數據</td>
<td>比較複雜的數據建議使用該變量（相比上面幾個，這個縣的比較重量級）</td>
</tr>
<tr>
<td>target</td>
<td>消息響應方</td>
<td>關聯的Handler對象，處理Message時會調用它分發處理Message對象</td>
</tr>
<tr>
<td>when</td>
<td>觸發響應時間</td>
<td>處理消息時間</td>
</tr>
<tr>
<td><code>next</code></td>
<td>Message隊列裡的下一個Message對象</td>
<td>用next指向下一條Message，實現一個鏈表數據結構，用戶一般使用不到該字段。</td>
</tr>
</tbody>
</table>
<blockquote><p>這裡的用戶指一般的APP開發者。</p></blockquote>
<p>一般不用手動設置target,調用Handler.obtainMessage()方法會自動的設置Message的target為當前的Handler。<br>得到Message之後可以調用sendToTarget(),發送消息給Handler，Handler再把消息放到message queue的尾部。<br>對Message除了給部分成員變量賦值外的操作都可以交由Handler來處理。</p>
<h3>消息池</h3>
<p>在通過Handler發送消息時，我們可以通過代碼<code>Message message=new Message();</code>新建一條消息，但是我們並不推薦這樣做，因為這樣每次都會新建一條消息，很容易造成資源浪費。Android中設計了消息池用於避免該現象：</p>
<ul>
<li>
<p>獲取消息 <code>obtain()</code><br>從消息池中獲取消息: <code>Message msg=Message.obtain();</code><br>obtain()方法源碼：</p>
<pre class="hljs java"><code class="java"><span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">static</span> Message <span class="hljs-title">obtain</span><span class="hljs-params">()</span> </span>{
      <span class="hljs-keyword">synchronized</span> (sPoolSync) {
          <span class="hljs-keyword">if</span> (sPool != <span class="hljs-keyword">null</span>) {
              Message m = sPool;
              sPool = m.next;
              m.next = <span class="hljs-keyword">null</span>; <span class="hljs-comment">//從sPool中取出一個Message對象，並消息鏈表斷開</span>
              m.flags = <span class="hljs-number">0</span>; <span class="hljs-comment">// clear in-use flag清除in-use flag</span>
              sPoolSize--;<span class="hljs-comment">//消息池的可用大小進行-1操作</span>
              <span class="hljs-keyword">return</span> m;
          }
      }
      <span class="hljs-keyword">return</span> <span class="hljs-keyword">new</span> Message();<span class="hljs-comment">// 當消息池為空時，直接創建Message對象</span>
  }</code></pre>
<blockquote><p>從消息池取Message，都是把消息池表頭的Message取走，再把表頭指向下一條消息<code>next</code>;</p></blockquote>
</li>
<li>
<p>回收消息 <code>recycle()</code><br>把不再使用的消息回收到消息池 <code>mgs.recycle();</code><br>recycle()方法源碼:</p>
<pre class="hljs java"><code class="java"><span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">recycle</span><span class="hljs-params">()</span> </span>{
      <span class="hljs-keyword">if</span> (isInUse()) {<span class="hljs-comment">//判斷消息是否正在使用</span>
          <span class="hljs-keyword">if</span> (gCheckRecycle) {
              <span class="hljs-keyword">throw</span> <span class="hljs-keyword">new</span> IllegalStateException(<span class="hljs-string">"This message cannot be recycled because it "</span>
                      + <span class="hljs-string">"is still in use."</span>);
          }
          <span class="hljs-keyword">return</span>;
      }
      recycleUnchecked();
  }

  <span class="hljs-comment">/**
   * 對於不再使用的消息，加入到消息池
   * Recycles a Message that may be in-use.
   * Used internally by the MessageQueue and Looper when disposing of queued Messages.
   */</span>
  <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">recycleUnchecked</span><span class="hljs-params">()</span> </span>{
      <span class="hljs-comment">// Mark the message as in use while it remains in the recycled object pool.</span>
      <span class="hljs-comment">// Clear out all other details.</span>
      <span class="hljs-comment">//將消息標示位置為IN_USE，並清空消息所有的參數。</span>
      flags = FLAG_IN_USE;
      what = <span class="hljs-number">0</span>;
      arg1 = <span class="hljs-number">0</span>;
      arg2 = <span class="hljs-number">0</span>;
      obj = <span class="hljs-keyword">null</span>;
      replyTo = <span class="hljs-keyword">null</span>;
      sendingUid = -<span class="hljs-number">1</span>;
      when = <span class="hljs-number">0</span>;
      target = <span class="hljs-keyword">null</span>;
      callback = <span class="hljs-keyword">null</span>;
      data = <span class="hljs-keyword">null</span>;

      <span class="hljs-keyword">synchronized</span> (sPoolSync) {
          <span class="hljs-keyword">if</span> (sPoolSize &lt; MAX_POOL_SIZE) {<span class="hljs-comment">//當消息池沒有滿時，將Message對象加入消息池</span>
              next = sPool;
              sPool = <span class="hljs-keyword">this</span>;
              sPoolSize++;<span class="hljs-comment">//消息池的可用大小進行加1操作</span>
          }
      }
  }</code></pre>
<blockquote><p>消息回收，就是將Message內容重置後，再把Message加到鏈表的表頭，加入到消息池的過程；</p></blockquote>
</li>
</ul>
<h2>MessageQueue</h2>
<p>負責管理消息隊列，實際上Message類有一個next字段，會將Message對象串在一起成為一個消息隊列，所以並不需要LinkedList之類的數據結構將Message對象組在一起成為隊列。</p>
<ul>
<li>
<strong>創建消息隊列</strong><pre class="hljs java"><code class="java">   MessageQueue(<span class="hljs-keyword">boolean</span> quitAllowed) {
      mQuitAllowed = quitAllowed;
      mPtr = nativeInit();<span class="hljs-comment">//通過native方法初始化消息隊列，其中mPtr是供native代碼使用</span>
  }</code></pre>
在MessageQueue初始化的時候調用了nativeInit，這是一個Native方法：<pre class="hljs cpp"><code class="cpp"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">void</span> <span class="hljs-title">android_os_MessageQueue_nativeInit</span><span class="hljs-params">(JNIEnv* env, jobject obj)</span> </span>{ 
  NativeMessageQueue* nativeMessageQueue = <span class="hljs-keyword">new</span> NativeMessageQueue(); 
  <span class="hljs-keyword">if</span> (!nativeMessageQueue) { 
      jniThrowRuntimeException(env, <span class="hljs-string">"Unable to allocate native queue"</span>); 
      <span class="hljs-keyword">return</span>;
      }
  nativeMessageQueue-&gt;incStrong(env); 
  android_os_MessageQueue_setNativeMessageQueue(env, obj, nativeMessageQueue);
} 
<span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">void</span> <span class="hljs-title">android_os_MessageQueue_setNativeMessageQueue</span><span class="hljs-params">(JNIEnv* env, jobject messageQueueObj,
      NativeMessageQueue* nativeMessageQueue)</span> </span>{ 
  env-&gt;SetIntField(messageQueueObj, gMessageQueueClassInfo.mPtr, 
  <span class="hljs-keyword">reinterpret_cast</span>&lt;jint&gt;(nativeMessageQueue)); 
}</code></pre>
方法名由java層類的包名+類名+方法名組成，這不是標準，是習慣寫法，也可以採用其他名稱組合，具體是什麼名稱由JNINativeMethod方法中Java對象與c++對象的映射決定，此處是JNI方面的內容，不作過多解釋。<br>在nativeInit中，new了一個Native層的MessageQueue的對象，並將其地址保存在了Java層MessageQueue的成員mPtr中，Android中有好多這樣的實現，一個類在Java層與Native層都有實現，通過JNI的GetFieldID與SetIntField把Native層的類的實例地址保存到Java層類的實例的mPtr成員中，比如Parcel。</li>
</ul>
<p>再看NativeMessageQueue的實現：</p>
<pre class="hljs aspectj"><code class="aspectj">NativeMessageQueue::NativeMessageQueue() : mInCallback(<span class="hljs-keyword">false</span>), mExceptionObj(NULL) { 
    mLooper = Looper::getForThread();
    <span class="hljs-keyword">if</span> (mLooper == NULL) {
      mLooper = <span class="hljs-keyword">new</span> Looper(<span class="hljs-keyword">false</span>);
      Looper::setForThread(mLooper); 
    }
}</code></pre>
<ul>
<li>
<p><strong>消息入隊 <code>enqueueMessage()</code></strong><br>enqueueMessage 用於將Message對象插入消息隊列。MessageQueue永遠是按照Message觸發的時間先後順序排列的，隊頭的消息是將要最早觸發的消息。當有消息需要加入消息隊列時，會從隊列頭開始遍歷，直到找到消息應該插入的合適位置，以保證所有消息的時間順序。<br>該方法會被Handler對象調用。<br>源碼如下：</p>
<pre class="hljs java"><code class="java"> <span class="hljs-comment">/**
   * 添加一條消息到消息隊列
   * <span class="hljs-doctag">@param</span> msg 要添加的消息
   * <span class="hljs-doctag">@param</span> when 消息處理時間
   * <span class="hljs-doctag">@return</span> 添加成功與否
   */</span>
  <span class="hljs-function"><span class="hljs-keyword">boolean</span> <span class="hljs-title">enqueueMessage</span><span class="hljs-params">(Message msg, <span class="hljs-keyword">long</span> when)</span> </span>{
      <span class="hljs-keyword">if</span> (msg.target == <span class="hljs-keyword">null</span>) {<span class="hljs-comment">// 每一個Message必須有一個target</span>
          <span class="hljs-keyword">throw</span> <span class="hljs-keyword">new</span> IllegalArgumentException(<span class="hljs-string">"Message must have a target."</span>);
      }
      <span class="hljs-keyword">if</span> (msg.isInUse()) {
          <span class="hljs-keyword">throw</span> <span class="hljs-keyword">new</span> IllegalStateException(msg + <span class="hljs-string">" This message is already in use."</span>);
      }

      <span class="hljs-keyword">synchronized</span> (<span class="hljs-keyword">this</span>) {
          <span class="hljs-keyword">if</span> (mQuitting) {
              IllegalStateException e = <span class="hljs-keyword">new</span> IllegalStateException(
                      msg.target + <span class="hljs-string">" sending message to a Handler on a dead thread"</span>);
              Log.w(TAG, e.getMessage(), e);
              msg.recycle();
              <span class="hljs-keyword">return</span> <span class="hljs-keyword">false</span>;
          }

          msg.markInUse();
          msg.when = when;
          Message p = mMessages;
          <span class="hljs-keyword">boolean</span> needWake;
          <span class="hljs-keyword">if</span> (p == <span class="hljs-keyword">null</span> || when == <span class="hljs-number">0</span> || when &lt; p.when) {
              <span class="hljs-comment">// New head, wake up the event queue if blocked.</span>
              <span class="hljs-comment">//p為null(代表MessageQueue沒有消息） 或者msg的觸發時間是隊列中最早的， 則進入該該分支</span>
              msg.next = p;
              mMessages = msg;
              needWake = mBlocked;
          } <span class="hljs-keyword">else</span> {
              <span class="hljs-comment">//將消息按時間順序插入到MessageQueue。一般地，不需要喚醒事件隊列，除非</span>
              <span class="hljs-comment">//消息隊頭存在barrier，並且同時Message是隊列中最早的異步消息。</span>

              <span class="hljs-comment">// Inserted within the middle of the queue.  Usually we don't have to wake</span>
              <span class="hljs-comment">// up the event queue unless there is a barrier at the head of the queue</span>
              <span class="hljs-comment">// and the message is the earliest asynchronous message in the queue.</span>
              needWake = mBlocked &amp;&amp; p.target == <span class="hljs-keyword">null</span> &amp;&amp; msg.isAsynchronous();
              Message prev;
              <span class="hljs-keyword">for</span> (;;) {
                  prev = p;
                  p = p.next;
                  <span class="hljs-keyword">if</span> (p == <span class="hljs-keyword">null</span> || when &lt; p.when) {
                      <span class="hljs-keyword">break</span>;
                  }
                  <span class="hljs-keyword">if</span> (needWake &amp;&amp; p.isAsynchronous()) {
                      needWake = <span class="hljs-keyword">false</span>;
                  }
              }
              msg.next = p; <span class="hljs-comment">// invariant: p == prev.next</span>
              prev.next = msg;
          }

          <span class="hljs-comment">// We can assume mPtr != 0 because mQuitting is false.</span>
          <span class="hljs-comment">//消息沒有退出，我們認為此時mPtr != 0</span>
          <span class="hljs-keyword">if</span> (needWake) {
              nativeWake(mPtr);
          }
      }
      <span class="hljs-keyword">return</span> <span class="hljs-keyword">true</span>;
  }</code></pre>
</li>
<li>
<p><strong>消息輪詢 <code>next()</code></strong><br>最重要的方法,用於獲取下一個Message對象，如果沒有需要處理的Message對象，該方法將阻塞。MessageQueue用本地方法做同步互斥，因為這樣時間更精準。每個Message對象都有一個什麼時刻處理該Message對象的屬性when，沒到時間都不會處理該Message對象，如果時間不精準的話，會導致系統消息不能及時處理。 </p>
<pre class="hljs java"><code class="java"> <span class="hljs-comment">/**
   * 依次從MessageQueue中取出Message
   * <span class="hljs-doctag">@return</span> 消息
   */</span>
  <span class="hljs-function">Message <span class="hljs-title">next</span><span class="hljs-params">()</span> </span>{
      <span class="hljs-comment">// Return here if the message loop has already quit and been disposed.</span>
      <span class="hljs-comment">// This can happen if the application tries to restart a looper after quit</span>
      <span class="hljs-comment">// which is not supported.</span>
      <span class="hljs-keyword">final</span> <span class="hljs-keyword">long</span> ptr = mPtr;
      <span class="hljs-keyword">if</span> (ptr == <span class="hljs-number">0</span>) {<span class="hljs-comment">//當消息循環已經退出，則直接返回</span>
          <span class="hljs-keyword">return</span> <span class="hljs-keyword">null</span>;
      }

      <span class="hljs-keyword">int</span> pendingIdleHandlerCount = -<span class="hljs-number">1</span>; <span class="hljs-comment">// -1 only during first iteration  循環迭代的首次為-1</span>
      <span class="hljs-keyword">int</span> nextPollTimeoutMillis = <span class="hljs-number">0</span>;
      <span class="hljs-keyword">for</span> (;;) {
          <span class="hljs-keyword">if</span> (nextPollTimeoutMillis != <span class="hljs-number">0</span>) {
              Binder.flushPendingCommands();
          }
          <span class="hljs-comment">//阻塞操作，當等待nextPollTimeoutMillis時長，或者消息隊列被喚醒，都會返回。</span>
          nativePollOnce(ptr, nextPollTimeoutMillis);

          <span class="hljs-keyword">synchronized</span> (<span class="hljs-keyword">this</span>) {
              <span class="hljs-comment">// Try to retrieve the next message.  Return if found.</span>
              <span class="hljs-keyword">final</span> <span class="hljs-keyword">long</span> now = SystemClock.uptimeMillis();
              Message prevMsg = <span class="hljs-keyword">null</span>;
              Message msg = mMessages;
              <span class="hljs-keyword">if</span> (msg != <span class="hljs-keyword">null</span> &amp;&amp; msg.target == <span class="hljs-keyword">null</span>) {
                  <span class="hljs-comment">//查詢MessageQueue中的下一條異步消息</span>
                  <span class="hljs-comment">// Stalled by a barrier.  Find the next asynchronous message in the queue.</span>
                  <span class="hljs-keyword">do</span> {
                      prevMsg = msg;
                      msg = msg.next;
                  } <span class="hljs-keyword">while</span> (msg != <span class="hljs-keyword">null</span> &amp;&amp; !msg.isAsynchronous());
              }
              <span class="hljs-keyword">if</span> (msg != <span class="hljs-keyword">null</span>) {
                  <span class="hljs-keyword">if</span> (now &lt; msg.when) {
                      <span class="hljs-comment">//設置下一次輪詢消息的超時時間</span>
                      <span class="hljs-comment">// Next message is not ready.  Set a timeout to wake up when it is ready.</span>
                      nextPollTimeoutMillis = (<span class="hljs-keyword">int</span>) Math.min(msg.when - now, Integer.MAX_VALUE);
                  } <span class="hljs-keyword">else</span> {
                      <span class="hljs-comment">// 獲取一條消息，並返回</span>
                      <span class="hljs-comment">// Got a message.</span>
                      mBlocked = <span class="hljs-keyword">false</span>;
                      <span class="hljs-keyword">if</span> (prevMsg != <span class="hljs-keyword">null</span>) {
                          prevMsg.next = msg.next;
                      } <span class="hljs-keyword">else</span> {
                          mMessages = msg.next;
                      }
                      msg.next = <span class="hljs-keyword">null</span>;
                      <span class="hljs-keyword">if</span> (DEBUG) Log.v(TAG, <span class="hljs-string">"Returning message: "</span> + msg);
                      msg.markInUse();<span class="hljs-comment">//設置消息flag成使用狀態</span>
                      <span class="hljs-keyword">return</span> msg;<span class="hljs-comment">//成功地獲取MessageQueue中的下一條即將要執行的消息</span>
                  }
              } <span class="hljs-keyword">else</span> {
                  <span class="hljs-comment">// No more messages.//沒有消息了</span>
                  nextPollTimeoutMillis = -<span class="hljs-number">1</span>;
              }

              <span class="hljs-comment">// Process the quit message now that all pending messages have been handled.</span>
              <span class="hljs-keyword">if</span> (mQuitting) {<span class="hljs-comment">//消息正在退出，返回null</span>
                  dispose();
                  <span class="hljs-keyword">return</span> <span class="hljs-keyword">null</span>;
              }

              <span class="hljs-comment">// If first time idle, then get the number of idlers to run.</span>
              <span class="hljs-comment">// Idle handles only run if the queue is empty or if the first message</span>
              <span class="hljs-comment">// in the queue (possibly a barrier) is due to be handled in the future.</span>
              <span class="hljs-comment">//當消息隊列為空，或者消息隊列的第一個消息時</span>
              <span class="hljs-keyword">if</span> (pendingIdleHandlerCount &lt; <span class="hljs-number">0</span>
                      &amp;&amp; (mMessages == <span class="hljs-keyword">null</span> || now &lt; mMessages.when)) {
                  pendingIdleHandlerCount = mIdleHandlers.size();
              }
              <span class="hljs-keyword">if</span> (pendingIdleHandlerCount &lt;= <span class="hljs-number">0</span>) {
                  <span class="hljs-comment">//沒有idle handlers 需要運行，則循環並等待。</span>
                  <span class="hljs-comment">// No idle handlers to run.  Loop and wait some more.</span>
                  mBlocked = <span class="hljs-keyword">true</span>;
                  <span class="hljs-keyword">continue</span>;
              }

              <span class="hljs-keyword">if</span> (mPendingIdleHandlers == <span class="hljs-keyword">null</span>) {
                  mPendingIdleHandlers = <span class="hljs-keyword">new</span> IdleHandler[Math.max(pendingIdleHandlerCount, <span class="hljs-number">4</span>)];
              }
              mPendingIdleHandlers = mIdleHandlers.toArray(mPendingIdleHandlers);
          }

          <span class="hljs-comment">// Run the idle handlers.</span>
          <span class="hljs-comment">// We only ever reach this code block during the first iteration.</span>
          <span class="hljs-comment">//只有第一次循環時，會運行idle handlers，執行完成後，重置pendingIdleHandlerCount為0.</span>
          <span class="hljs-keyword">for</span> (<span class="hljs-keyword">int</span> i = <span class="hljs-number">0</span>; i &lt; pendingIdleHandlerCount; i++) {
              <span class="hljs-keyword">final</span> IdleHandler idler = mPendingIdleHandlers[i];
              mPendingIdleHandlers[i] = <span class="hljs-keyword">null</span>; <span class="hljs-comment">// release the reference to the handler//去掉handler的引用</span>

              <span class="hljs-keyword">boolean</span> keep = <span class="hljs-keyword">false</span>;
              <span class="hljs-keyword">try</span> {
                  keep = idler.queueIdle();<span class="hljs-comment">//idle時執行的方法</span>
              } <span class="hljs-keyword">catch</span> (Throwable t) {
                  Log.wtf(TAG, <span class="hljs-string">"IdleHandler threw exception"</span>, t);
              }

              <span class="hljs-keyword">if</span> (!keep) {
                  <span class="hljs-keyword">synchronized</span> (<span class="hljs-keyword">this</span>) {
                      mIdleHandlers.remove(idler);
                  }
              }
          }

          <span class="hljs-comment">// Reset the idle handler count to 0 so we do not run them again.</span>
          <span class="hljs-comment">//重置idle handler個數為0，以保證不會再次重複運行</span>
          pendingIdleHandlerCount = <span class="hljs-number">0</span>;

          <span class="hljs-comment">// While calling an idle handler, a new message could have been delivered</span>
          <span class="hljs-comment">// so go back and look again for a pending message without waiting.</span>
          <span class="hljs-comment">//當調用一個空閒handler時，一個新message能夠被分發，因此無需等待可以直接查詢pending message.</span>
          nextPollTimeoutMillis = <span class="hljs-number">0</span>;
      }
  }</code></pre>
<blockquote><p>nativePollOnce(ptr, nextPollTimeoutMillis)是一個native方法，是一個阻塞操作。其中nextPollTimeoutMillis代表下一個消息到來前，還需要等待的時長；當nextPollTimeoutMillis = -1時，表示消息隊列中無消息，會一直等待下去。空閒後，往往會執行IdleHandler中的方法。當nativePollOnce()返回後，next()從mMessages中提取一個消息。nativePollOnce()在native做了大量的工作，想深入研究可查看資料： <a href="http://gityuan.com/2015/12/27/handler-message-native/" target="_blank">Android消息機制2-Handler(Native層)</a>。</p></blockquote>
</li>
<li>
<p><strong>移除消息 <code>removeMessages()</code></strong><br>就是將消息從鏈表移除，同時將移除的消息添加到消息池，提供循環複用。<br>採用了兩個while循環，第一個循環是從隊頭開始，移除符合條件的消息，第二個循環是從頭部移除完連續的滿足條件的消息之後，再從隊列後面繼續查詢是否有滿足條件的消息需要被移除。</p>
<pre class="hljs java"><code class="java">  <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">removeMessages</span><span class="hljs-params">(Handler h, <span class="hljs-keyword">int</span> what, Object object)</span> </span>{
      <span class="hljs-keyword">if</span> (h == <span class="hljs-keyword">null</span>) {
          <span class="hljs-keyword">return</span>;
      }

      <span class="hljs-keyword">synchronized</span> (<span class="hljs-keyword">this</span>) {
          Message p = mMessages;
          <span class="hljs-comment">//從消息隊列的頭部開始，移除所有符合條件的消息</span>
          <span class="hljs-comment">// Remove all messages at front.</span>
          <span class="hljs-keyword">while</span> (p != <span class="hljs-keyword">null</span> &amp;&amp; p.target == h &amp;&amp; p.what == what
                 &amp;&amp; (object == <span class="hljs-keyword">null</span> || p.obj == object)) {
              Message n = p.next;
              mMessages = n;
              p.recycleUnchecked();
              p = n;
          }

          <span class="hljs-comment">// Remove all messages after front.</span>
          <span class="hljs-comment">//移除剩餘的符合要求的消息</span>
          <span class="hljs-keyword">while</span> (p != <span class="hljs-keyword">null</span>) {
              Message n = p.next;
              <span class="hljs-keyword">if</span> (n != <span class="hljs-keyword">null</span>) {
                  <span class="hljs-keyword">if</span> (n.target == h &amp;&amp; n.what == what
                      &amp;&amp; (object == <span class="hljs-keyword">null</span> || n.obj == object)) {
                      Message nn = n.next;
                      n.recycleUnchecked();
                      p.next = nn;
                      <span class="hljs-keyword">continue</span>;
                  }
              }
              p = n;
          }
      }
  }</code></pre>
</li>
<li>
<p>退出消息隊列<br>消息退出的方式：</p>
<ul>
<li>當safe =true時，只移除尚未觸發的所有消息，對於正在觸發的消息並不移除；</li>
<li>當safe =flase時，移除所有的消息</li>
</ul>
<pre class="hljs java"><code class="java"><span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">quit</span><span class="hljs-params">(<span class="hljs-keyword">boolean</span> safe)</span> </span>{
      <span class="hljs-keyword">if</span> (!mQuitAllowed) {<span class="hljs-comment">// 當mQuitAllowed為false，表示不運行退出，強行調用quit()會拋出異常</span>
          <span class="hljs-keyword">throw</span> <span class="hljs-keyword">new</span> IllegalStateException(<span class="hljs-string">"Main thread not allowed to quit."</span>);
      }

      <span class="hljs-keyword">synchronized</span> (<span class="hljs-keyword">this</span>) {
          <span class="hljs-keyword">if</span> (mQuitting) { <span class="hljs-comment">//防止多次執行退出操作</span>
              <span class="hljs-keyword">return</span>;
          }
          mQuitting = <span class="hljs-keyword">true</span>;

          <span class="hljs-keyword">if</span> (safe) {
              removeAllFutureMessagesLocked();<span class="hljs-comment">//移除尚未觸發的所有消息</span>
          } <span class="hljs-keyword">else</span> {
              removeAllMessagesLocked();<span class="hljs-comment">//移除所有的消息</span>
          }

          <span class="hljs-comment">// We can assume mPtr != 0 because mQuitting was previously false.</span>
          <span class="hljs-comment">//mQuitting=false，那麼認定為 mPtr != 0</span>
          nativeWake(mPtr);
      }
  }</code></pre>
</li>
</ul>

        </div>