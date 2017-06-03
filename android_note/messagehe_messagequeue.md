# Message和MessageQueue


<div class="show-content">
       
<h3>消息结构</h3>
<p>每个消息用Message表示，Message主要包含以下内容：</p>
<table>
<thead>
<tr>
<th>filed</th>
<th>含义</th>
<th>说明</th>
</tr>
</thead>
<tbody>
<tr>
<td>what</td>
<td>消息类别</td>
<td>由用户定义，用来区分不同的消息</td>
</tr>
<tr>
<td>arg1</td>
<td>参数1</td>
<td>是一种轻量级的传递数据的方式</td>
</tr>
<tr>
<td>arg2</td>
<td>参数2</td>
<td>是一种轻量级的传递数据的方式</td>
</tr>
<tr>
<td>obj</td>
<td>消息内容</td>
<td>任意对象，但是使用Messenger跨进程传递Message时不能为null</td>
</tr>
<tr>
<td>data</td>
<td>Bundle数据</td>
<td>比较复杂的数据建议使用该变量（相比上面几个，这个县的比较重量级）</td>
</tr>
<tr>
<td>target</td>
<td>消息响应方</td>
<td>关联的Handler对象，处理Message时会调用它分发处理Message对象</td>
</tr>
<tr>
<td>when</td>
<td>触发响应时间</td>
<td>处理消息时间</td>
</tr>
<tr>
<td><code>next</code></td>
<td>Message队列里的下一个Message对象</td>
<td>用next指向下一条Message，实现一个链表数据结构，用户一般使用不到该字段。</td>
</tr>
</tbody>
</table>
<blockquote><p>这里的用户指一般的APP开发者。</p></blockquote>
<p>一般不用手动设置target,调用Handler.obtainMessage()方法会自动的设置Message的target为当前的Handler。<br>得到Message之后可以调用sendToTarget(),发送消息给Handler，Handler再把消息放到message queue的尾部。<br>对Message除了给部分成员变量赋值外的操作都可以交由Handler来处理。</p>
<h3>消息池</h3>
<p>在通过Handler发送消息时，我们可以通过代码<code>Message message=new Message();</code>新建一条消息，但是我们并不推荐这样做，因为这样每次都会新建一条消息，很容易造成资源浪费。Android中设计了消息池用于避免该现象：</p>
<ul>
<li>
<p>获取消息 <code>obtain()</code><br>从消息池中获取消息: <code>Message msg=Message.obtain();</code><br>obtain()方法源码：</p>
<pre class="hljs java"><code class="java"><span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">static</span> Message <span class="hljs-title">obtain</span><span class="hljs-params">()</span> </span>{
      <span class="hljs-keyword">synchronized</span> (sPoolSync) {
          <span class="hljs-keyword">if</span> (sPool != <span class="hljs-keyword">null</span>) {
              Message m = sPool;
              sPool = m.next;
              m.next = <span class="hljs-keyword">null</span>; <span class="hljs-comment">//从sPool中取出一个Message对象，并消息链表断开</span>
              m.flags = <span class="hljs-number">0</span>; <span class="hljs-comment">// clear in-use flag清除in-use flag</span>
              sPoolSize--;<span class="hljs-comment">//消息池的可用大小进行-1操作</span>
              <span class="hljs-keyword">return</span> m;
          }
      }
      <span class="hljs-keyword">return</span> <span class="hljs-keyword">new</span> Message();<span class="hljs-comment">// 当消息池为空时，直接创建Message对象</span>
  }</code></pre>
<blockquote><p>从消息池取Message，都是把消息池表头的Message取走，再把表头指向下一条消息<code>next</code>;</p></blockquote>
</li>
<li>
<p>回收消息 <code>recycle()</code><br>把不再使用的消息回收到消息池 <code>mgs.recycle();</code><br>recycle()方法源码:</p>
<pre class="hljs java"><code class="java"><span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">recycle</span><span class="hljs-params">()</span> </span>{
      <span class="hljs-keyword">if</span> (isInUse()) {<span class="hljs-comment">//判断消息是否正在使用</span>
          <span class="hljs-keyword">if</span> (gCheckRecycle) {
              <span class="hljs-keyword">throw</span> <span class="hljs-keyword">new</span> IllegalStateException(<span class="hljs-string">"This message cannot be recycled because it "</span>
                      + <span class="hljs-string">"is still in use."</span>);
          }
          <span class="hljs-keyword">return</span>;
      }
      recycleUnchecked();
  }

  <span class="hljs-comment">/**
   * 对于不再使用的消息，加入到消息池
   * Recycles a Message that may be in-use.
   * Used internally by the MessageQueue and Looper when disposing of queued Messages.
   */</span>
  <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">recycleUnchecked</span><span class="hljs-params">()</span> </span>{
      <span class="hljs-comment">// Mark the message as in use while it remains in the recycled object pool.</span>
      <span class="hljs-comment">// Clear out all other details.</span>
      <span class="hljs-comment">//将消息标示位置为IN_USE，并清空消息所有的参数。</span>
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
          <span class="hljs-keyword">if</span> (sPoolSize &lt; MAX_POOL_SIZE) {<span class="hljs-comment">//当消息池没有满时，将Message对象加入消息池</span>
              next = sPool;
              sPool = <span class="hljs-keyword">this</span>;
              sPoolSize++;<span class="hljs-comment">//消息池的可用大小进行加1操作</span>
          }
      }
  }</code></pre>
<blockquote><p>消息回收，就是将Message内容重置后，再把Message加到链表的表头，加入到消息池的过程；</p></blockquote>
</li>
</ul>
<h2>MessageQueue</h2>
<p>负责管理消息队列，实际上Message类有一个next字段，会将Message对象串在一起成为一个消息队列，所以并不需要LinkedList之类的数据结构将Message对象组在一起成为队列。</p>
<ul>
<li>
<strong>创建消息队列</strong><pre class="hljs java"><code class="java">   MessageQueue(<span class="hljs-keyword">boolean</span> quitAllowed) {
      mQuitAllowed = quitAllowed;
      mPtr = nativeInit();<span class="hljs-comment">//通过native方法初始化消息队列，其中mPtr是供native代码使用</span>
  }</code></pre>
在MessageQueue初始化的时候调用了nativeInit，这是一个Native方法：<pre class="hljs cpp"><code class="cpp"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">void</span> <span class="hljs-title">android_os_MessageQueue_nativeInit</span><span class="hljs-params">(JNIEnv* env, jobject obj)</span> </span>{ 
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
方法名由java层类的包名+类名+方法名组成，这不是标准，是习惯写法，也可以采用其他名称组合，具体是什么名称由JNINativeMethod方法中Java对象与c++对象的映射决定，此处是JNI方面的内容，不作过多解释。<br>在nativeInit中，new了一个Native层的MessageQueue的对象，并将其地址保存在了Java层MessageQueue的成员mPtr中，Android中有好多这样的实现，一个类在Java层与Native层都有实现，通过JNI的GetFieldID与SetIntField把Native层的类的实例地址保存到Java层类的实例的mPtr成员中，比如Parcel。</li>
</ul>
<p>再看NativeMessageQueue的实现：</p>
<pre class="hljs aspectj"><code class="aspectj">NativeMessageQueue::NativeMessageQueue() : mInCallback(<span class="hljs-keyword">false</span>), mExceptionObj(NULL) { 
    mLooper = Looper::getForThread();
    <span class="hljs-keyword">if</span> (mLooper == NULL) {
      mLooper = <span class="hljs-keyword">new</span> Looper(<span class="hljs-keyword">false</span>);
      Looper::setForThread(mLooper); 
    }
}</code></pre>
<ul>
<li>
<p><strong>消息入队 <code>enqueueMessage()</code></strong><br>enqueueMessage 用于将Message对象插入消息队列。MessageQueue永远是按照Message触发的时间先后顺序排列的，队头的消息是将要最早触发的消息。当有消息需要加入消息队列时，会从队列头开始遍历，直到找到消息应该插入的合适位置，以保证所有消息的时间顺序。<br>该方法会被Handler对象调用。<br>源码如下：</p>
<pre class="hljs java"><code class="java"> <span class="hljs-comment">/**
   * 添加一条消息到消息队列
   * <span class="hljs-doctag">@param</span> msg 要添加的消息
   * <span class="hljs-doctag">@param</span> when 消息处理时间
   * <span class="hljs-doctag">@return</span> 添加成功与否
   */</span>
  <span class="hljs-function"><span class="hljs-keyword">boolean</span> <span class="hljs-title">enqueueMessage</span><span class="hljs-params">(Message msg, <span class="hljs-keyword">long</span> when)</span> </span>{
      <span class="hljs-keyword">if</span> (msg.target == <span class="hljs-keyword">null</span>) {<span class="hljs-comment">// 每一个Message必须有一个target</span>
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
              <span class="hljs-comment">//p为null(代表MessageQueue没有消息） 或者msg的触发时间是队列中最早的， 则进入该该分支</span>
              msg.next = p;
              mMessages = msg;
              needWake = mBlocked;
          } <span class="hljs-keyword">else</span> {
              <span class="hljs-comment">//将消息按时间顺序插入到MessageQueue。一般地，不需要唤醒事件队列，除非</span>
              <span class="hljs-comment">//消息队头存在barrier，并且同时Message是队列中最早的异步消息。</span>

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
          <span class="hljs-comment">//消息没有退出，我们认为此时mPtr != 0</span>
          <span class="hljs-keyword">if</span> (needWake) {
              nativeWake(mPtr);
          }
      }
      <span class="hljs-keyword">return</span> <span class="hljs-keyword">true</span>;
  }</code></pre>
</li>
<li>
<p><strong>消息轮询 <code>next()</code></strong><br>最重要的方法,用于获取下一个Message对象，如果没有需要处理的Message对象，该方法将阻塞。MessageQueue用本地方法做同步互斥，因为这样时间更精准。每个Message对象都有一个什么时刻处理该Message对象的属性when，没到时间都不会处理该Message对象，如果时间不精准的话，会导致系统消息不能及时处理。 </p>
<pre class="hljs java"><code class="java"> <span class="hljs-comment">/**
   * 依次从MessageQueue中取出Message
   * <span class="hljs-doctag">@return</span> 消息
   */</span>
  <span class="hljs-function">Message <span class="hljs-title">next</span><span class="hljs-params">()</span> </span>{
      <span class="hljs-comment">// Return here if the message loop has already quit and been disposed.</span>
      <span class="hljs-comment">// This can happen if the application tries to restart a looper after quit</span>
      <span class="hljs-comment">// which is not supported.</span>
      <span class="hljs-keyword">final</span> <span class="hljs-keyword">long</span> ptr = mPtr;
      <span class="hljs-keyword">if</span> (ptr == <span class="hljs-number">0</span>) {<span class="hljs-comment">//当消息循环已经退出，则直接返回</span>
          <span class="hljs-keyword">return</span> <span class="hljs-keyword">null</span>;
      }

      <span class="hljs-keyword">int</span> pendingIdleHandlerCount = -<span class="hljs-number">1</span>; <span class="hljs-comment">// -1 only during first iteration  循环迭代的首次为-1</span>
      <span class="hljs-keyword">int</span> nextPollTimeoutMillis = <span class="hljs-number">0</span>;
      <span class="hljs-keyword">for</span> (;;) {
          <span class="hljs-keyword">if</span> (nextPollTimeoutMillis != <span class="hljs-number">0</span>) {
              Binder.flushPendingCommands();
          }
          <span class="hljs-comment">//阻塞操作，当等待nextPollTimeoutMillis时长，或者消息队列被唤醒，都会返回。</span>
          nativePollOnce(ptr, nextPollTimeoutMillis);

          <span class="hljs-keyword">synchronized</span> (<span class="hljs-keyword">this</span>) {
              <span class="hljs-comment">// Try to retrieve the next message.  Return if found.</span>
              <span class="hljs-keyword">final</span> <span class="hljs-keyword">long</span> now = SystemClock.uptimeMillis();
              Message prevMsg = <span class="hljs-keyword">null</span>;
              Message msg = mMessages;
              <span class="hljs-keyword">if</span> (msg != <span class="hljs-keyword">null</span> &amp;&amp; msg.target == <span class="hljs-keyword">null</span>) {
                  <span class="hljs-comment">//查询MessageQueue中的下一条异步消息</span>
                  <span class="hljs-comment">// Stalled by a barrier.  Find the next asynchronous message in the queue.</span>
                  <span class="hljs-keyword">do</span> {
                      prevMsg = msg;
                      msg = msg.next;
                  } <span class="hljs-keyword">while</span> (msg != <span class="hljs-keyword">null</span> &amp;&amp; !msg.isAsynchronous());
              }
              <span class="hljs-keyword">if</span> (msg != <span class="hljs-keyword">null</span>) {
                  <span class="hljs-keyword">if</span> (now &lt; msg.when) {
                      <span class="hljs-comment">//设置下一次轮询消息的超时时间</span>
                      <span class="hljs-comment">// Next message is not ready.  Set a timeout to wake up when it is ready.</span>
                      nextPollTimeoutMillis = (<span class="hljs-keyword">int</span>) Math.min(msg.when - now, Integer.MAX_VALUE);
                  } <span class="hljs-keyword">else</span> {
                      <span class="hljs-comment">// 获取一条消息，并返回</span>
                      <span class="hljs-comment">// Got a message.</span>
                      mBlocked = <span class="hljs-keyword">false</span>;
                      <span class="hljs-keyword">if</span> (prevMsg != <span class="hljs-keyword">null</span>) {
                          prevMsg.next = msg.next;
                      } <span class="hljs-keyword">else</span> {
                          mMessages = msg.next;
                      }
                      msg.next = <span class="hljs-keyword">null</span>;
                      <span class="hljs-keyword">if</span> (DEBUG) Log.v(TAG, <span class="hljs-string">"Returning message: "</span> + msg);
                      msg.markInUse();<span class="hljs-comment">//设置消息flag成使用状态</span>
                      <span class="hljs-keyword">return</span> msg;<span class="hljs-comment">//成功地获取MessageQueue中的下一条即将要执行的消息</span>
                  }
              } <span class="hljs-keyword">else</span> {
                  <span class="hljs-comment">// No more messages.//没有消息了</span>
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
              <span class="hljs-comment">//当消息队列为空，或者消息队列的第一个消息时</span>
              <span class="hljs-keyword">if</span> (pendingIdleHandlerCount &lt; <span class="hljs-number">0</span>
                      &amp;&amp; (mMessages == <span class="hljs-keyword">null</span> || now &lt; mMessages.when)) {
                  pendingIdleHandlerCount = mIdleHandlers.size();
              }
              <span class="hljs-keyword">if</span> (pendingIdleHandlerCount &lt;= <span class="hljs-number">0</span>) {
                  <span class="hljs-comment">//没有idle handlers 需要运行，则循环并等待。</span>
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
          <span class="hljs-comment">//只有第一次循环时，会运行idle handlers，执行完成后，重置pendingIdleHandlerCount为0.</span>
          <span class="hljs-keyword">for</span> (<span class="hljs-keyword">int</span> i = <span class="hljs-number">0</span>; i &lt; pendingIdleHandlerCount; i++) {
              <span class="hljs-keyword">final</span> IdleHandler idler = mPendingIdleHandlers[i];
              mPendingIdleHandlers[i] = <span class="hljs-keyword">null</span>; <span class="hljs-comment">// release the reference to the handler//去掉handler的引用</span>

              <span class="hljs-keyword">boolean</span> keep = <span class="hljs-keyword">false</span>;
              <span class="hljs-keyword">try</span> {
                  keep = idler.queueIdle();<span class="hljs-comment">//idle时执行的方法</span>
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
          <span class="hljs-comment">//重置idle handler个数为0，以保证不会再次重复运行</span>
          pendingIdleHandlerCount = <span class="hljs-number">0</span>;

          <span class="hljs-comment">// While calling an idle handler, a new message could have been delivered</span>
          <span class="hljs-comment">// so go back and look again for a pending message without waiting.</span>
          <span class="hljs-comment">//当调用一个空闲handler时，一个新message能够被分发，因此无需等待可以直接查询pending message.</span>
          nextPollTimeoutMillis = <span class="hljs-number">0</span>;
      }
  }</code></pre>
<blockquote><p>nativePollOnce(ptr, nextPollTimeoutMillis)是一个native方法，是一个阻塞操作。其中nextPollTimeoutMillis代表下一个消息到来前，还需要等待的时长；当nextPollTimeoutMillis = -1时，表示消息队列中无消息，会一直等待下去。空闲后，往往会执行IdleHandler中的方法。当nativePollOnce()返回后，next()从mMessages中提取一个消息。nativePollOnce()在native做了大量的工作，想深入研究可查看资料： <a href="http://gityuan.com/2015/12/27/handler-message-native/" target="_blank">Android消息机制2-Handler(Native层)</a>。</p></blockquote>
</li>
<li>
<p><strong>移除消息 <code>removeMessages()</code></strong><br>就是将消息从链表移除，同时将移除的消息添加到消息池，提供循环复用。<br>采用了两个while循环，第一个循环是从队头开始，移除符合条件的消息，第二个循环是从头部移除完连续的满足条件的消息之后，再从队列后面继续查询是否有满足条件的消息需要被移除。</p>
<pre class="hljs java"><code class="java">  <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">removeMessages</span><span class="hljs-params">(Handler h, <span class="hljs-keyword">int</span> what, Object object)</span> </span>{
      <span class="hljs-keyword">if</span> (h == <span class="hljs-keyword">null</span>) {
          <span class="hljs-keyword">return</span>;
      }

      <span class="hljs-keyword">synchronized</span> (<span class="hljs-keyword">this</span>) {
          Message p = mMessages;
          <span class="hljs-comment">//从消息队列的头部开始，移除所有符合条件的消息</span>
          <span class="hljs-comment">// Remove all messages at front.</span>
          <span class="hljs-keyword">while</span> (p != <span class="hljs-keyword">null</span> &amp;&amp; p.target == h &amp;&amp; p.what == what
                 &amp;&amp; (object == <span class="hljs-keyword">null</span> || p.obj == object)) {
              Message n = p.next;
              mMessages = n;
              p.recycleUnchecked();
              p = n;
          }

          <span class="hljs-comment">// Remove all messages after front.</span>
          <span class="hljs-comment">//移除剩余的符合要求的消息</span>
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
<p>退出消息队列<br>消息退出的方式：</p>
<ul>
<li>当safe =true时，只移除尚未触发的所有消息，对于正在触发的消息并不移除；</li>
<li>当safe =flase时，移除所有的消息</li>
</ul>
<pre class="hljs java"><code class="java"><span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">quit</span><span class="hljs-params">(<span class="hljs-keyword">boolean</span> safe)</span> </span>{
      <span class="hljs-keyword">if</span> (!mQuitAllowed) {<span class="hljs-comment">// 当mQuitAllowed为false，表示不运行退出，强行调用quit()会抛出异常</span>
          <span class="hljs-keyword">throw</span> <span class="hljs-keyword">new</span> IllegalStateException(<span class="hljs-string">"Main thread not allowed to quit."</span>);
      }

      <span class="hljs-keyword">synchronized</span> (<span class="hljs-keyword">this</span>) {
          <span class="hljs-keyword">if</span> (mQuitting) { <span class="hljs-comment">//防止多次执行退出操作</span>
              <span class="hljs-keyword">return</span>;
          }
          mQuitting = <span class="hljs-keyword">true</span>;

          <span class="hljs-keyword">if</span> (safe) {
              removeAllFutureMessagesLocked();<span class="hljs-comment">//移除尚未触发的所有消息</span>
          } <span class="hljs-keyword">else</span> {
              removeAllMessagesLocked();<span class="hljs-comment">//移除所有的消息</span>
          }

          <span class="hljs-comment">// We can assume mPtr != 0 because mQuitting was previously false.</span>
          <span class="hljs-comment">//mQuitting=false，那么认定为 mPtr != 0</span>
          nativeWake(mPtr);
      }
  }</code></pre>
</li>
</ul>

        </div>