# Android四大组件：Service服务史上最全面解析



<hr>
<h1>前言</h1>
<ul>
<li>Service作为Android四大组件之一，应用非常广泛</li>
<li>本文将介绍对Service进行全面介绍（基础认识、生命周期、使用和应用场景）</li>
</ul>
<hr>
<h1>目录</h1>
<div class="image-package">
<img src="images/944365-087501b69a8c3451.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-087501b69a8c3451.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">目录</div>
</div>
<hr>
<h1>1. 基础知识</h1>
<ul>
<li>定义：服务，属于Android中的计算型组件</li>
<li>作用：提供需要在后台长期运行的服务（如复杂计算、下载等等）</li>
<li>特点：长生命周期的、没有用户界面、在后台运行</li>
</ul>
<hr>
<h1>2. 生命周期方法详解</h1>
<p>具体请看我写的文章：<a href="http://www.jianshu.com/p/8d0cde35eb10" target="_blank">Android：Service生命周期最全面解析</a></p>
<hr>
<h1>3. Service分类</h1>
<h3>3.1 Service的类型</h3>
<div class="image-package">
<img src="images/944365-d42fa78828930bdb.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-d42fa78828930bdb.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">分类</div>
</div>
<h3>3.2  详细介绍</h3>
<div class="image-package">
<img src="images/944365-8855e3a5340bece5.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-8855e3a5340bece5.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">Service类型的详细介绍</div>
</div>
<h1>4. Service的使用解析</h1>
<p>由上述可知，服务Service总共分为：</p>
<div class="image-package">
<img src="images/944365-d42fa78828930bdb.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-d42fa78828930bdb.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">分类</div>
</div>
<p>接下来，我将介绍每种Service的具体使用，具体请看我写的文章:<a href="http://www.jianshu.com/p/e04c4239b07e" target="_blank">Android：（本地、可通信的、前台、远程）Service使用全面介绍</a></p>
<h1>5. 使用场景</h1>
<ul>
<li>通过上述描述，你应该对Service类型及其使用非常了解；</li>
<li>那么，我们该什么时候用哪种类型的Service呢？</li>
<li>各种Service的使用场景请看下图：<br><div class="image-package">
<img src="images/944365-a1e97f2646e93bbb.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-a1e97f2646e93bbb.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">使用场景</div>
</div>
</li>
</ul>
<h1>6. 其他思考</h1>
<h3>6.1 Service和Thread的区别</h3>
<ul>
<li>结论：Service和Thread之间没有任何关系</li>
<li>
<p>之所以有不少人会把它们联系起来，主要因为Service的后台概念</p>
<blockquote><p>后台的定义：后台任务运行完全不依赖UI，即使Activity被销毁，或者程序被关闭，只要进程还在，后台任务就可以继续运行</p></blockquote>
</li>
<li>
<p>其实二者存在较大的区别，如下图：<br></p>
<div class="image-package">
<img src="images/944365-384170261e06ecbb.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-384170261e06ecbb.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">Paste_Image.png</div>
</div>
</li>
</ul>
<p>一般来说，会将Service和Thread联合着用，即在Service中再创建一个子线程（工作线程）去处理耗时操作逻辑，如下代码：</p>
<pre class="hljs aspectj"><code class="aspectj"><span class="hljs-meta">@Override</span>  
<span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">int</span> <span class="hljs-title">onStartCommand</span><span class="hljs-params">(Intent intent, <span class="hljs-keyword">int</span> flags, <span class="hljs-keyword">int</span> startId)</span> </span>{  
<span class="hljs-comment">//新建工作线程</span>
    <span class="hljs-keyword">new</span> Thread(<span class="hljs-keyword">new</span> Runnable() {  
        <span class="hljs-meta">@Override</span>  
        <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">run</span><span class="hljs-params">()</span> </span>{  
            <span class="hljs-comment">// 开始执行后台任务  </span>
        }  
    }).start();  
    <span class="hljs-function"><span class="hljs-keyword">return</span> <span class="hljs-keyword">super</span>.<span class="hljs-title">onStartCommand</span><span class="hljs-params">(intent, flags, startId)</span></span>;  
}  

<span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">MyBinder</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">Binder</span> </span>{  
    <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">service_connect_Activity</span><span class="hljs-params">()</span> </span>{  
  <span class="hljs-comment">//新建工作线程</span>
        <span class="hljs-keyword">new</span> Thread(<span class="hljs-keyword">new</span> Runnable() {  
            <span class="hljs-meta">@Override</span>  
            <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">run</span><span class="hljs-params">()</span> </span>{  
                <span class="hljs-comment">// 执行具体的下载任务  </span>
            }  
        }).start();  
    }  

}</code></pre>
<h3>6.2 Service和IntentService的区别</h3>
<p>具体请看我写的文章：<a href="http://www.jianshu.com/p/8a3c44a9173a" target="_blank">Android多线程全面解析：IntentService用法&amp;源码</a></p>
<hr>
<h1>7.总结</h1>
<ul>
<li>本文对Service的所有知识进行了全面解析（基础认识、生命周期、使用和应用场景）</li>
<li>如果你还想了解关于Service的其他知识，请浏览以下文章：<blockquote><p><a href="http://www.jianshu.com/p/d963c55c3ab9" target="_blank">Android：Service服务史上最全面解析</a><br><a href="http://www.jianshu.com/p/8d0cde35eb10" target="_blank">Android：Service生命周期最全面解析</a><br><a href="http://www.jianshu.com/p/e04c4239b07e" target="_blank">Android：（本地、可通信的、前台、远程）Service使用全面介绍</a><br><a href="http://www.jianshu.com/p/34326751b2c6" target="_blank">Android：远程服务Service（含AIDL &amp; IPC讲解）</a><br><a href="http://www.jianshu.com/p/8a3c44a9173a" target="_blank">Android多线程全面解析：IntentService用法&amp;源码</a></p></blockquote>
</li>
</ul>
<ul>
<li>接下来会介绍继续介绍Android开发中的相关知识，有兴趣可以继续关注<a href="http://www.jianshu.com/users/383970bef0a0/latest_articles" target="_blank">Carson_Ho的安卓开发笔记</a>
</li>
</ul>
<hr>
<h4>请点赞！因为你们的赞同/鼓励是我写作的最大动力！</h4>
<blockquote><p><strong>相关文章阅读</strong><br><a href="http://www.jianshu.com/p/ec5a1a30694b" target="_blank">Android开发：最全面、最易懂的Android屏幕适配解决方案</a><br><a href="http://www.jianshu.com/p/9fe944ee02f7" target="_blank">Android开发：Handler异步通信机制全面解析（包含Looper、Message Queue）</a><br><a href="http://www.jianshu.com/p/3c94ae673e2a" target="_blank">Android开发：最全面、最易懂的Webview详解</a><br><a href="http://www.jianshu.com/p/b87fee2f7a23" target="_blank">Android开发：JSON简介及最全面解析方法!</a><br><a href="http://www.jianshu.com/p/ce1d060573ba" target="_blank">Android开发：顶部Tab导航栏实现（TabLayout+ViewPager+Fragment）</a><br><a href="http://www.jianshu.com/p/a663803b2a44" target="_blank">Android开发：底部Tab菜单栏实现（FragmentTabHost+ViewPager）</a></p></blockquote>
<hr>
