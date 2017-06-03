# Looper


<div class="show-content">
          <p>Looper在Android消息机制中的主要作用就是一直循环从MessageQueue中取Message，取出Message后交给它的target，该target是一个Handler对象，消息交给Handler后通过调用Handler的<code>dispatchMessage()</code>方法进行处理。</p>
<h2>类成员</h2>
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
<td>sThreadLocal</td>
<td>Looper 对象</td>
<td>每个线程中的Looper对象其实是一个ThreadLocal，即线程本地存储(TLS)对象</td>
</tr>
<tr>
<td>sMainLooper</td>
<td>主线程使用的Looper对象</td>
<td>由系统在ActivityThread主线程中创建。</td>
</tr>
<tr>
<td>mQueue</td>
<td>和Looper对应的消息队列</td>
<td>一个Looper依赖一个消息队列（一对一）</td>
</tr>
<tr>
<td>mThread</td>
<td>和Looper对应的线程</td>
<td>一个Looper和一个线程绑定（一对一）</td>
</tr>
</tbody>
</table>
<h2>Looper工作过程</h2>
<ol>
<li>
<p><strong>为线程创建消息循环</strong><br>使用Looper对象时，会先调用静态的prepare方法或者prepareMainLooper方法来创建线程的Looper对象。如果是主线程会调用prepareMainLooper，如果是普通线程只需调用prepare方法，两者都会调用prepare(boolean quitAllowed)方法，该方法源码如下：</p>
<pre class="hljs java"><code class="java"> <span class="hljs-comment">/**
  * 该方法会创建Looper对象，Looper对象的构造方法中会创建一个MessageQueue对象，再将Looper对象保存到当前线程 TLS
  * <span class="hljs-doctag">@param</span> quitAllowed
  */</span>
 <span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">static</span> <span class="hljs-keyword">void</span> <span class="hljs-title">prepare</span><span class="hljs-params">(<span class="hljs-keyword">boolean</span> quitAllowed)</span> </span>{
     <span class="hljs-keyword">if</span> (sThreadLocal.get() != <span class="hljs-keyword">null</span>) {
         <span class="hljs-comment">// 试图在有Looper的线程中再次创建Looper将抛出异常，一个线程只能有一个looper。</span>
         <span class="hljs-keyword">throw</span> <span class="hljs-keyword">new</span> RuntimeException(<span class="hljs-string">"Only one Looper may be created per thread"</span>);
     }
     <span class="hljs-comment">// 我们调用该方法会在调用线程的TLS中创建Looper对象</span>
     sThreadLocal.set(<span class="hljs-keyword">new</span> Looper(quitAllowed));
 }</code></pre>
<blockquote><p>第一次调用prepare()方法后，新创建出来的当前线程对应的Looper对象就被存储到一个<code>TLS</code>对象中，如果重复调用，就会报错。</p></blockquote>
</li>
<li>
<p><strong>开启消息循环</strong><br>Looper类乃至Android消息处理机制的核心部分，在使用Looper时，调用完<code>Looper.prepare()</code>后，还需要调用Looper.loop()方法开启消息循环。该方法是一个死循环会将不断重复下面的操作，直到没有消息时退出循环。</p>
<ol>
<li>读取MessageQueue的下一条Message</li>
<li>把Message分发给相应的target（Handler）来处理</li>
<li>
<p>把分发后的Message，回收到消息池以复用</p>
<pre class="hljs java"><code class="java"><span class="hljs-comment">/**
* 在这个线程中启动队列，请确保在循环结束时候调用{<span class="hljs-doctag">@link</span> #quit()}
*
* Run the message queue in this thread. Be sure to call
* {<span class="hljs-doctag">@link</span> #quit()} to end the loop.
*/</span>
<span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">static</span> <span class="hljs-keyword">void</span> <span class="hljs-title">loop</span><span class="hljs-params">()</span> </span>{
  <span class="hljs-keyword">final</span> Looper me = myLooper();<span class="hljs-comment">//获取TLS存储的Looper对象</span>
  <span class="hljs-keyword">if</span> (me == <span class="hljs-keyword">null</span>) {<span class="hljs-comment">//如果没有调用Loop.prepare()的话，就会抛出下面这个异常</span>
      <span class="hljs-keyword">throw</span> <span class="hljs-keyword">new</span> RuntimeException(<span class="hljs-string">"No Looper; Looper.prepare() wasn't called on this thread."</span>);
  }
  <span class="hljs-keyword">final</span> MessageQueue queue = me.mQueue;<span class="hljs-comment">//从Looper中取出消息队列</span>

  <span class="hljs-comment">// Make sure the identity of this thread is that of the local process,</span>
  <span class="hljs-comment">// and keep track of what that identity token actually is.</span>
  Binder.clearCallingIdentity();
  <span class="hljs-comment">//确保在权限检查时基于本地进程，而不是基于最初调用进程。</span>
  <span class="hljs-keyword">final</span> <span class="hljs-keyword">long</span> ident = Binder.clearCallingIdentity();

  <span class="hljs-comment">//死循环，循环的取消息，没有新消息就会阻塞</span>
  <span class="hljs-keyword">for</span> (;;) {
      <span class="hljs-comment">//调用MessageQueue的next方法来获取新消息，而，next是一个阻塞方法，没有消息时，loop方法将跟随next方法会一直阻塞在这里。</span>
      Message msg = queue.next(); <span class="hljs-comment">// might block,如果没有新消息,这里会被阻塞。</span>
      <span class="hljs-comment">//因为以上获取消息是阻塞方法，所以，当消息队列中没有消息时，将阻塞在上一步。而如果上一步拿到了一个空消息，只能说明</span>
      <span class="hljs-comment">//我们退出了该消息队列。那么这里直接退出</span>
      <span class="hljs-keyword">if</span> (msg == <span class="hljs-keyword">null</span>) {
          <span class="hljs-comment">// No message indicates that the message queue is quitting.</span>
          <span class="hljs-comment">//没有消息意味着消息队列正在退出。这也就是为什么Looper的quit(）方法中只需要退出消息队列即可。</span>
          <span class="hljs-keyword">return</span>;
      }

      <span class="hljs-comment">// This must be in a local variable, in case a UI event sets the logger</span>
      Printer logging = me.mLogging;<span class="hljs-comment">//默认为null，可通过setMessageLogging()方法来指定输出，用于debug功能</span>
      <span class="hljs-keyword">if</span> (logging != <span class="hljs-keyword">null</span>) {
          logging.println(<span class="hljs-string">"&gt;&gt;&gt;&gt;&gt; Dispatching to "</span> + msg.target + <span class="hljs-string">" "</span> +
                  msg.callback + <span class="hljs-string">": "</span> + msg.what);
      }

      msg.target.dispatchMessage(msg); <span class="hljs-comment">//msg.target就是与此线程关联的Handler对象，调用它的dispatchMessage处理消息</span>

      <span class="hljs-keyword">if</span> (logging != <span class="hljs-keyword">null</span>) {
          logging.println(<span class="hljs-string">"&lt;&lt;&lt;&lt;&lt; Finished to "</span> + msg.target + <span class="hljs-string">" "</span> + msg.callback);
      }

      <span class="hljs-comment">// Make sure that during the course of dispatching the</span>
      <span class="hljs-comment">// identity of the thread wasn't corrupted.</span>
      <span class="hljs-keyword">final</span> <span class="hljs-keyword">long</span> newIdent = Binder.clearCallingIdentity();<span class="hljs-comment">//确保分发过程中identity不会损坏</span>
      <span class="hljs-keyword">if</span> (ident != newIdent) {
          Log.wtf(TAG, <span class="hljs-string">"Thread identity changed from 0x"</span>
                  + Long.toHexString(ident) + <span class="hljs-string">" to 0x"</span>
                  + Long.toHexString(newIdent) + <span class="hljs-string">" while dispatching to "</span>
                  + msg.target.getClass().getName() + <span class="hljs-string">" "</span>
                  + msg.callback + <span class="hljs-string">" what="</span> + msg.what);
      }

      msg.recycleUnchecked(); <span class="hljs-comment">//将已经处理过的消息会受到消息池</span>
  }
}</code></pre>
<blockquote><p>上面代码中可以看到有logging方法，这是用于debug的，默认情况下logging == null，通过设置setMessageLogging()用来开启debug工作。</p></blockquote>
</li>
</ol>
</li>
<li>
<p><strong>获得消息循环</strong><br><code>myLooper()</code>方法用于获取当前消息循环对象。Looper对象从成员变量 sThreadLocal(<code>线程本地存储(TLS)对象</code>) 中获取。</p>
<blockquote><p>获得的Looper对象可以作为Handler的构建函数参数，将在下篇文章中说明。</p></blockquote>
</li>
<li>
<strong>退出消息循环</strong><br>主要是退出消息队列:<pre class="hljs java"><code class="java">  <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">quit</span><span class="hljs-params">()</span> </span>{
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
<strong>Looper构造方法</strong><br>Looper在执行静态方法<code>Looper.loop()</code>时调用Looper的构造函数（代码见上文）。在Looper初始化时，新建了一个MessageQueue的对象保存了在成员mQueue中。Looper是依赖于一个线程和一个消息队列的。<pre class="hljs java"><code class="java">  <span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-title">Looper</span><span class="hljs-params">(<span class="hljs-keyword">boolean</span> quitAllowed)</span> </span>{
      <span class="hljs-comment">// 每个Looper对象中有它的消息队列，和它所属的线程</span>
      mQueue = <span class="hljs-keyword">new</span> MessageQueue(quitAllowed);
      mThread = Thread.currentThread();
  }</code></pre>
</li>
<li>
<strong>prepareMainLooper()</strong><br>该方法只在主线程中调用，系统已帮我们做好，我们一般不用也不能调用。</li>
</ul>

        </div>