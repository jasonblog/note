# Handler


<div class="show-content">
          <p>Handler是我们在开发中接触到最多的类了。<br>他可谓是Android消息机制中的总调度员。他几乎无所不能：创建消息可以是他，发消息是他，处理消息是他，移除消息还是他。所以，很多开发者对Handler很熟悉，对其背后底层默默工作的MessageQueue和Looper反而比较陌生。</p>
<p>我们先看一下Handler的类结构：<br></p><div class="image-package">
<img src="http://upload-images.jianshu.io/upload_images/652037-e439362539abc0ab.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="http://upload-images.jianshu.io/upload_images/652037-e439362539abc0ab.jpg?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">Handler类结构.jpg</div>
</div><p><br>从图中我们可以看出，其包含了文章开头所讲的所有功能函数。<br>Handler的主要功能有：</p>
<ol>
<li>构造函数</li>
<li>获取消息</li>
<li>发送消息（消息入队）</li>
<li>处理消息（消息的真正处理逻辑）</li>
<li>移除消息（消息出列）</li>
</ol>
<h2>构造函数</h2>
<p>Handler的构造函数最终目的就是设置Handler中的几个重要的成员变量：<code>mLooper</code>,<code>mQueue</code>,<code>mCallback</code>,<code>mAsynchronous</code>。</p>
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
<td>mLooper</td>
<td>消息循环器Looper</td>
<td>该Handler所对应的Looper</td>
</tr>
<tr>
<td>mQueue</td>
<td>消息队列MessageQueue</td>
<td>该Handler所处理消息所在的消息队列</td>
</tr>
<tr>
<td>mCallback</td>
<td>Handler级别回调</td>
<td>Handler处理所有send Message系列消息时的统一回调。(下文会细述)</td>
</tr>
<tr>
<td>mAsynchronous</td>
<td>是不是异步处理方式</td>
<td>其实只有一个作用，就是在设置Barrier时仍可以不受Barrier的影响被正常处理，如果没有设置 Barrier，异步消息就与同步消息没有区别</td>
</tr>
</tbody>
</table>
<p>代码如下：</p>
<pre class="hljs java"><code class="java">    <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-title">Handler</span><span class="hljs-params">(Callback callback, <span class="hljs-keyword">boolean</span> async)</span> </span>{
        <span class="hljs-comment">//这个会判断一个警告，意思是说Handler class应该是一个静态类，否则可能导致内存泄漏，</span>
        <span class="hljs-comment">// 至于为什么可以参考链接http://stackoverflow.com/questions/11407943/this-handler-class-should-be-static-or-leaks-might-occur-incominghandler</span>
        <span class="hljs-keyword">if</span> (FIND_POTENTIAL_LEAKS) {
            <span class="hljs-keyword">final</span> Class&lt;? extends Handler&gt; klass = getClass();
            <span class="hljs-keyword">if</span> ((klass.isAnonymousClass() || klass.isMemberClass() || klass.isLocalClass()) &amp;&amp;
                    (klass.getModifiers() &amp; Modifier.STATIC) == <span class="hljs-number">0</span>) {
                Log.w(TAG, <span class="hljs-string">"The following Handler class should be static or leaks might occur: "</span> +
                    klass.getCanonicalName());
            }
        }
        <span class="hljs-comment">//必须先执行Looper.prepare()，才能获取Looper对象，否则为null.</span>
        mLooper = Looper.myLooper();
        <span class="hljs-keyword">if</span> (mLooper == <span class="hljs-keyword">null</span>) {
            <span class="hljs-keyword">throw</span> <span class="hljs-keyword">new</span> RuntimeException(
                <span class="hljs-string">"Can't create handler inside thread that has not called Looper.prepare()"</span>);
        }
        mQueue = mLooper.mQueue;<span class="hljs-comment">//消息队列，来自Looper对象</span>
        mCallback = callback;<span class="hljs-comment">//回调方法</span>
        mAsynchronous = async; <span class="hljs-comment">//设置消息是否为异步处理方式</span>
    }</code></pre>
<p>代码比较简单，过程为：</p>
<ol>
<li>首先判断下是不是有可能内存泄漏（该问题我会有单独的文章阐述），</li>
<li>然后得到当前线程的Looper赋值给<code>mLooper</code>，如果mLooper为空，说明当前线程并不是一个可接受消息的线程，需要在线程开启时用Looper.prepare()和Looper.loop()来初始化才可以继续。</li>
<li>为mQueue和mCallback、mAsynchronous等成员函数赋值。</li>
</ol>
<blockquote><p>Handler也是在这里和Looper、MessageQueue联系起来的。</p></blockquote>
<h2>获取消息</h2>
<p>Handler中通过一系列的obtainMessage()方法，封装了Message从消息池中取到符合要求的消息的方法。</p>
<pre class="hljs java"><code class="java"><span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">final</span> Message <span class="hljs-title">obtainMessage</span><span class="hljs-params">()</span>
<span class="hljs-keyword">public</span> <span class="hljs-keyword">final</span> Message <span class="hljs-title">obtainMessage</span><span class="hljs-params">(<span class="hljs-keyword">int</span> what)</span>
<span class="hljs-keyword">public</span> <span class="hljs-keyword">final</span> Message <span class="hljs-title">obtainMessage</span><span class="hljs-params">(<span class="hljs-keyword">int</span> what, Object obj)</span>
<span class="hljs-keyword">public</span> <span class="hljs-keyword">final</span> Message <span class="hljs-title">obtainMessage</span><span class="hljs-params">(<span class="hljs-keyword">int</span> what, <span class="hljs-keyword">int</span> arg1, <span class="hljs-keyword">int</span> arg2)</span>
<span class="hljs-keyword">public</span> <span class="hljs-keyword">final</span> Message <span class="hljs-title">obtainMessage</span><span class="hljs-params">(<span class="hljs-keyword">int</span> what, <span class="hljs-keyword">int</span> arg1, <span class="hljs-keyword">int</span> arg2, Object obj)</span></span></code></pre>
<blockquote><p>这些方法都会将该Handler设置为该消息的<code>target</code>。</p></blockquote>
<h2>发送消息</h2>
<p>Handler发送消息主要有两种方法：post系列方法和send系列方法。</p>
<ol>
<li>
<p>post runnable 系列</p>
<pre class="hljs java"><code class="java"><span class="hljs-function"><span class="hljs-keyword">final</span> <span class="hljs-keyword">boolean</span> <span class="hljs-title">post</span><span class="hljs-params">(Runnable r)</span>
<span class="hljs-keyword">final</span> <span class="hljs-keyword">boolean</span> <span class="hljs-title">postAtTime</span><span class="hljs-params">(Runnable r, <span class="hljs-keyword">long</span> uptimeMillis)</span>
<span class="hljs-keyword">final</span> <span class="hljs-keyword">boolean</span> <span class="hljs-title">postAtTime</span><span class="hljs-params">(Runnable r, Object token, <span class="hljs-keyword">long</span> uptimeMillis)</span>
<span class="hljs-keyword">final</span> <span class="hljs-keyword">boolean</span> <span class="hljs-title">postDelayed</span><span class="hljs-params">(Runnable r, <span class="hljs-keyword">long</span> delayMillis)</span>
<span class="hljs-keyword">final</span> <span class="hljs-keyword">boolean</span> <span class="hljs-title">postAtFrontOfQueue</span><span class="hljs-params">(Runnable r)</span></span></code></pre>
</li>
<li>
<p>send Message 系列</p>
<pre class="hljs java"><code class="java"><span class="hljs-function"><span class="hljs-keyword">final</span> <span class="hljs-keyword">boolean</span> <span class="hljs-title">sendEmptyMessage</span><span class="hljs-params">(<span class="hljs-keyword">int</span> what)</span>
<span class="hljs-keyword">final</span> <span class="hljs-keyword">boolean</span> <span class="hljs-title">sendEmptyMessageDelayed</span><span class="hljs-params">(<span class="hljs-keyword">int</span> what, <span class="hljs-keyword">long</span> delayMillis)</span>
<span class="hljs-keyword">final</span> <span class="hljs-keyword">boolean</span> <span class="hljs-title">sendEmptyMessageAtTime</span><span class="hljs-params">(<span class="hljs-keyword">int</span> what, <span class="hljs-keyword">long</span> uptimeMillis)</span>
<span class="hljs-keyword">final</span> <span class="hljs-keyword">boolean</span> <span class="hljs-title">sendMessageDelayed</span><span class="hljs-params">(Message msg, <span class="hljs-keyword">long</span> delayMillis)</span>
<span class="hljs-keyword">boolean</span> <span class="hljs-title">sendMessageAtTime</span><span class="hljs-params">(Message msg, <span class="hljs-keyword">long</span> uptimeMillis)</span>
<span class="hljs-keyword">final</span> <span class="hljs-keyword">boolean</span> <span class="hljs-title">sendMessageAtFrontOfQueue</span><span class="hljs-params">(Message msg)</span></span></code></pre>
</li>
</ol>
<p>他们之间的调用关系如图：<br></p><div class="image-package">
<img src="http://upload-images.jianshu.io/upload_images/652037-5a2782c8b43acc1d.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="http://upload-images.jianshu.io/upload_images/652037-5a2782c8b43acc1d.jpg?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">Handler发送消息调用过程</div>
</div><p><br>他们最终都是将某个消息压入到MessageQueue中，等待处理。区别在于：</p>
<ol>
<li>发送机制不同。<br>post runnable系列处理的参数是封装了需执行动作的runnable，首先将runnable封装成一个Message，然后再调用对应的send系列函数把最终它压入到MessageQueue中。<br>send message系列处理的参数直接是Message，经过一些赋值后，直接压入到MessageQueue中。</li>
<li>最终处理机制不同。<br>post runnable系列方法在被封装成Message时，设置了其Callback为该runnable，最终在Handler的dispatchMessage里面会交由handlerCallback方法处理，执行其runnable的run()方法。<br>send message系列方法最终会受到Handler的Callback影响，或交由Handler的handleMessage()方法处理。</li>
<li>两者本质没有区别，区别在于是否post runnable系列方法可以在不继承Handler并重写handleMessage()方法的前提下对一些不可预知的消息类型做相应处理。比较常见的例子如SDK中的这些方法：<pre class="hljs java"><code class="java">Activity.runOnUiThread(Runnable)
View.post(Runnable)
View.postDelayed(Runnable, <span class="hljs-keyword">long</span>)</code></pre>
这也是post系列方法存在的意义。</li>
</ol>
<h2>处理消息</h2>
<p>从上一篇文章中我们可以知道，Looper循环过程中，取出一条消息后，是通过调用该消息对应的Handler的dispatchMessage(Message msg)对消息进行处理。</p>
<p>code in Looper.loop():</p>
<pre class="hljs java"><code class="java">msg.target.dispatchMessage(msg); <span class="hljs-comment">//msg.target就是与此线程关联的Handler对象，调用它的dispatchMessage处理消息</span></code></pre>
<p>Handler处理消息的源码如下：</p>
<pre class="hljs java"><code class="java">    <span class="hljs-comment">/**
     * 在这里处理系统消息
     * Handle system messages here.
     */</span>
    <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">dispatchMessage</span><span class="hljs-params">(Message msg)</span> </span>{
        <span class="hljs-keyword">if</span> (msg.callback != <span class="hljs-keyword">null</span>) {<span class="hljs-comment">//post系列方法走这里</span>
            handleCallback(msg);
        } <span class="hljs-keyword">else</span> {<span class="hljs-comment">//sendMessage系列方法走这里</span>
            <span class="hljs-keyword">if</span> (mCallback != <span class="hljs-keyword">null</span>) {<span class="hljs-comment">//Handler构造函数中定义了Callback的这里处理</span>
                <span class="hljs-keyword">if</span> (mCallback.handleMessage(msg)) {
                    <span class="hljs-keyword">return</span>;
                }
            }
            handleMessage(msg);
        }
    }</code></pre>
<p>其处理逻辑流程如图：<br></p><div class="image-package">
<img src="http://upload-images.jianshu.io/upload_images/652037-f399cf90221ad3fc.jpg?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="http://upload-images.jianshu.io/upload_images/652037-f399cf90221ad3fc.jpg?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">Handler处理消息.jpg</div>
</div><p><br>其实逻辑很简单。</p>
<ol>
<li>首先看是否这条消息是否有已被预设了callback，如果有，直接调用handlerCallback(msg)处理预设好的callback。至此，满足该条件的的消息处理流程结束。</li>
<li>其次，如果不符合<code>1.</code>中的情况，看该Handler在被创建时，是否有设定Handler级别的Callback，如果有，处理之。这里和<code>1.</code>中所描述情况的区别是，Handler级别的Callback是有返回值的，处理完后会根据返回值看是否需要进一步处理。</li>
<li>如果Message没有被预设Callback，也没有Handler级别的Callback（或者有但需要进一步处理），必须在 由子类重写的<code>handleMessage(Message msg)</code>中做最后处理。大多数情况下的消息都在这里处理。</li>
</ol>
<blockquote><p>根据Handler中的发送消息的方法源码可知，Post系列方法的都会调用getPostMessage(Runnable r)函数将一个Runnable对象封装成一条Message，封装时，会将该runnable参数作为消息的callback。所以，我们可以得出结论：<strong><code>post runnable系列方法消息的最终都在handleCallback(msg)中处理。</code></strong></p></blockquote>
<h2>移除消息</h2>
<p>根据消息入列的区别，移除消息也分为<code>removeMessages</code>和<code>removeCallbacks</code>两系列。和发送消息类似，<br>Handler的移除消息最终还是对其对应的MessageQueue的操作，从中移除符合条件的消息。</p>
<blockquote><p>使用Handler的过程中，为避免Activity调用onDestroy后，Handler的MessageQueue中仍存在Message，一般会在onDestroy中调用removeCallbacksAndMessages()方法。</p></blockquote>
<h2>几个问题</h2>
<ol>
<li>为什么Handler的构造函数需要一个Callback<br>从Handler的消息处理逻辑可以看出,一旦一个Handler在构造时，Handler级别的Callback 被初始化。是所有没有单独预设Callback的Message(post系列方法发送的消息除外的消息)都会被该Callback处理。<br><strong>我们可以在Handler级别的Callback中加入由该Handler处理的所有类型消息的共同逻辑。</strong>
</li>
<li>Handler是如何与Looper和MessageQueue联系起来的。<br>是通过构造函数中联系起来的，Handler的一堆构造函数，其实最终目的就是设置Handler中的几个重要的成员变量：<code>mLooper</code>,<code>mQueue</code>,<code>mCallback</code>,<code>mAsynchronous</code>。首先设置好mLooper，mLooper中有一个和其一一对应的变量：mQueue。<br>所以，<strong>Hander是通过其对应的mLooper，进而和线程中的消息队列mQueue联系起来的</strong>。</li>
<li>post系列方法传入的Runnable中若持有Context的引用，会造成内存泄漏吗？<br>显然是会的。Runnable会被封装成Message加入到消息队列中，只要该消息不被处理或者移除，消息队列就会间接持有Context的强引用，造成内存溢出，所以，如果该Handler是针对一个Activity的操作，在Activity的 onDestory()回调函数中中一定要调用removeCallbacksAndMessages()来防止内存泄漏。</li>
</ol>

        </div>