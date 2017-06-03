# 概述设计架构



<div class="show-content">
          <p>本系列文章将分N篇介绍Android中的消息机制。</p>
<ol>
<li>概述和设计架构</li>
<li>Message和MessageQueue</li>
<li>Looper</li>
<li>Handler</li>
<li>Handler使用实战</li>
<li>Handler引起的内存溢出</li>
<li>ThreadLocal</li>
</ol>
<p>Android的应用程序和Windows应用程序一样，都是由消息驱动的。在Android操作系统中，谷歌也实现了消息循环处理机制。</p>
<h2>相关概念</h2>
<p>学习Android的消息机制，有几个设计概念我们必须了解：</p>
<ol>
<li>
<strong>消息：Message</strong><br>消息（Message）代表一个行为（what）或者一串动作（Runnable）,每一个消息在加入消息队列时，都有明确的目标（Handler）。</li>
<li>
<strong>消息队列：MessageQueue</strong><br>以队列的形式对外提供插入和删除的工作，其内部结构是以链表的形式存储消息的。</li>
<li>
<strong>Looper</strong><br>Looper是循环的意思，它负责从消息队列中循环的取出消息然后把消息交给目标(Handler)处理。</li>
<li>
<strong>Handler</strong><br>消息的真正处理者，具备获取消息、发送消息、处理消息、移除消息等功能。</li>
<li>线程<br>线程，CPU调度资源的基本单位。Android中的消息机制也是基于线程中的概念。</li>
<li>ThreadLocal<br>可以理解为<code>ThreadLocalData</code>,ThreadLocal的作用是提供线程内的局部变量（TLS），这种变量在线程的生命周期内起作用，每一个线程有他自己所属的值（线程隔离）。<blockquote><p>5、6为牵涉到的概念，不是本文重点。会另起文章讨论</p></blockquote>
</li>
</ol>
<p>平时我们最常使用的就是Message与Handler了，如果使用过HandlerThread或者自己实现类似HandlerThread的东西可能还会接触到Looper，而MessageQueue是Looper内部使用的，对于标准的SDK，我们是无法实例化并使用的（构造函数是包可见性）。</p>
<p>我们平时接触到的Looper、Message、Handler都是用JAVA实现的，Android是一个基于Linux的系统，底层用C、C++实现的，而且还有NDK的存在，Android消息驱动的模型为了消息的及时性、高效性，在Native层也设计了Java层对应的类如Looper、MessageQueue等。</p>
<div class="image-package">
<img src="https://raw.githubusercontent.com/wangkuiwu/android_applets/master/os/pic/messagequeue/message_queue01.jpg" data-original-src="https://raw.githubusercontent.com/wangkuiwu/android_applets/master/os/pic/messagequeue/message_queue01.jpg" style="cursor: zoom-in;"><br><div class="image-caption">handle机制.jpg</div>
</div>
<h2>他们如何协作</h2>
<div class="image-package">
<img src="http://upload-images.jianshu.io/upload_images/652037-8523323f2946a1d8.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="http://upload-images.jianshu.io/upload_images/652037-8523323f2946a1d8.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">Handler、MessageQueue、Looper如何协作</div>
</div>
<p><strong>一句话总结为：Looper不断从MessageQueue中取出一个Message，然后交给其对应的Handler处理。</strong></p>
<p>他们之间的类图如下：<br></p><div class="image-package">
<img src="http://7xkrut.com1.z1.glb.clouddn.com/Handler%E7%B1%BB%E5%9B%BE.jpg" data-original-src="http://7xkrut.com1.z1.glb.clouddn.com/Handler%E7%B1%BB%E5%9B%BE.jpg" style="cursor: zoom-in;"><br><div class="image-caption">Handler、Looper、Message、MessageQueue类图</div>
</div><p><br>从上文两张图中我们可以得到以下结论：</p>
<ol>
<li>Looper依赖于MessageQueue和Thread，每个Thread只对应一个Looper，每个Looper只对应一个MessageQueue（一对一）。</li>
<li>MessageQueue依赖于Message，每个MessageQueue中有N个待处理消息（一对N）。</li>
<li>Message依赖于Handler来进行处理，每个Message有且仅有一个对应的Handler。（一对一）</li>
<li>Handler中持有Looper和MessageQueue的引用，可直接对其进行操作。</li>
</ol>
<p>还有一点要说明的是：普通的线程是没有looper的，如果需要looper对象，那么必须要先调用Looper.prepare()方法，而且一个线程只能有一个looper。调用完以后，此线程就成为了所谓的LooperThread,若在当前LooperThread中创建Handler对象，那么此Handler会自动关联到当前线程的looper对象，也就是拥有looper的引用。<br>下面是官方给出的LooperThread最标准的用法。</p>
<pre class="hljs java"><code class="java"><span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">LooperThread</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">Thread</span> </span>{
<span class="hljs-keyword">public</span> Handler mHandler;

<span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">run</span><span class="hljs-params">()</span> </span>{
Looper.prepare();

mHandler = <span class="hljs-keyword">new</span> Handler() {
<span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">handleMessage</span><span class="hljs-params">(Message msg)</span> </span>{
<span class="hljs-comment">// process incoming messages here</span>
}
};

Looper.loop();
}</code></pre>
<h2>为什么我们需要这样的消息处理机制</h2>
<ol>
<li>不阻塞主线程<br>Android应用程序启动时，系统会创建一个主线程，负责与UI组件（widget、view）进行交互，比如控制UI界面界面显示、更新等；分发事件给UI界面处理，比如按键事件、触摸事件、屏幕绘图事件等，因此，Android主线程也称为UI线程。<br>由此可知，UI线程只能处理一些简单的、短暂的操作，如果要执行繁重的任务或者耗时很长的操作，比如访问网络、数据库、下载等，这种单线程模型会导致线程运行性能大大降低，甚至阻塞UI线程，如果被阻塞超过5秒，系统会提示应用程序无相应对话框，缩写为ANR，导致退出整个应用程序或者短暂杀死应用程序。
<em><strong>Android系统将大部分耗时、繁重任务交给子线程完成，不会在主线程中完成。</strong></em>
</li>
<li>并发程序设计的<strong>有序性</strong><br>单线程模型的UI主线程也是不安全的，会造成不可确定的结果。<br>线程不安全简单理解为：多线程访问资源时，有可能出现多个线程先后更改数据造成数据不一致。比如，A工作线程（也称为子线程）访问某个公共UI资源，B工作线程在某个时候也访问了该公共资源，当B线程正访问时，公共资源的属性已经被A改变了，这样B得到的结果不是所需要的的，造成了数据不一致的混乱情况。<br>线程安全简单理解为：当一个线程访问功能资源时，对该资源进程了保护，比如加了锁机制，当前线程在没有访问结束释放锁之前，其他线程只能等待直到释放锁才能访问，这样的线程就是安全的。
<em><strong>Android只允许主线程更新UI界面，子线程处理后的结果无法和主线程交互，即无法直接访问主线程，这就要用到Handler机制来解决此问题。基于Handler机制，在子线程先获得Handler对象，该对象将数据发送到主线程消息队列，主线程通过Loop循环获取消息交给Handler处理。</strong></em>
</li>
</ol>
<h1>是如何完成跨线程通信的</h1>
<p>Handler发送消息后添加消息到消息队列，然后消息在恰当时候出列，都是由Handler来执行，那么是如何完成跨线程通信的？<br>这里就牵涉到了Linux系统的跨线程通信的知识，Android中采用的是Linux中的管道通信。<br> Looper是通过管道(pipe)实现的。<br>关于管道，简单来说，管道就是一个文件。<br>在管道的两端，分别是两个打开文件文件描述符，这两个打开文件描述符都是对应同一个文件，其中一个是用来读的，别一个是用来写的。<br>一般的使用方式就是，一个线程通过读文件描述符中来读管道的内容，当管道没有内容时，这个线程就会进入等待状态，而另外一个线程通过写文件描述符来向管道中写入内容，写入内容的时候，如果另一端正有线程正在等待管道中的内容，那么这个线程就会被唤醒。这个等待和唤醒的操作是如何进行的呢，这就要借助Linux系统中的epoll机制了。 Linux系统中的epoll机制为处理大批量句柄而作了改进的poll，是Linux下多路复用IO接口select/poll的增强版本，它能显著减少程序在大量并发连接中只有少量活跃的情况下的系统CPU利用率。</p>
<p>(01) pipe(wakeFds)，该函数创建了两个管道句柄。<br>(02) mWakeReadPipeFd=wakeFds[0]，是读管道的句柄。<br>(03) mWakeWritePipeFd=wakeFds<a href="http://7xkrut.com1.z1.glb.clouddn.com/Handler.png" target="_blank">1</a>，是写管道的句柄。<br>(04) epoll_create(EPOLL_SIZE_HINT)是创建epoll句柄。<br>(05) epoll_ctl(mEpollFd, EPOLL_CTL_ADD, mWakeReadPipeFd, &amp; eventItem)，它的作用是告诉mEpollFd，它要监控mWakeReadPipeFd文件描述符的EPOLLIN事件，即当管道中有内容可读时，就唤醒当前正在等待管道中的内容的线程。</p>
<p>这样一个线程（比如UI线程）消息队列和Looper就准备就绪了。</p>
<blockquote><p>消息队列创建时，会调用JNI函数，初始化NativeMessageQueue对象。NativeMessageQueue则会初始化Looper对象。Looper的作用就是，当Java层的消息队列中没有消息时，就使Android应用程序主线程进入等待状态，而当Java层的消息队列中来了新的消息后，就唤醒Android应用程序的主线程来处理这个消息。</p></blockquote>
<p>由于鄙人C++荒废，在此不做过多探讨。<br>关于C++层逻辑可参考文章：<br><a href="http://blog.csdn.net/luoshengyang/article/details/6817933" target="_blank">Android应用程序消息处理机制（Looper、Handler）分析</a><br><a href="http://gityuan.com/2015/12/27/handler-message-native/" target="_blank">Android消息机制-Handler(native篇)</a><br><a href="http://www.cnblogs.com/angeldevil/p/3340644.html" target="_blank">Android消息处理机制(Handler、Looper、MessageQueue与Message)</a><br><a href="http://wangkuiwu.github.io/2014/08/26/MessageQueue/" target="_blank">Android消息机制架构和源码解析</a></p>
<p>本系列文章参考的资料还有：<br><a href="http://item.jd.com/11452539.html" target="_blank">书籍：深入理解Android内核设计思想</a><br><a href="http://blog.csdn.net/singwhatiwanna/article/details/17361775" target="_blank">Android源码分析-消息队列和Looper</a><br><a href="http://mouxuejie.com/blog/2016-03-31/message-looper-mechanism/" target="_blank">Android消息循环机制源码分析</a><br><a href="http://www.cloudchou.com/android/post-388.html" target="_blank">Handler Looper MessageQueue 详解</a></p>

        </div>