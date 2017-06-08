# Android四大组件：Service生命周期最全面解析


<div class="show-content">
          <div class="image-package">
<img src="images/944365-207a738cb165a2da.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-207a738cb165a2da.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption"></div>
</div>
<hr>
<h1>前言</h1>
<ul>
<li>Service作为Android四大组件之一，应用非常广泛</li>
<li>本文将介绍Service最基础的知识：Service的生命周期<blockquote><p>如果你对Service还未了解，建议先阅读我写的文章：<br><a href="http://www.jianshu.com/p/d963c55c3ab9" target="_blank">Android四大组件：Service史上最全面解析</a></p></blockquote>
</li>
</ul>
<hr>
<h1>目录</h1>
<div class="image-package">
<img src="images/944365-ba6f19f002b9b8f6.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-ba6f19f002b9b8f6.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">目录</div>
</div>
<hr>
<h1>1. 生命周期常用方法</h1>
<p>在Service的生命周期里，常用的有：</p>
<ul>
<li>4个手动调用的方法</li>
</ul>
<table>
<thead>
<tr>
<th>手动调用方法</th>
<th style="text-align:center">作用</th>
</tr>
</thead>
<tbody>
<tr>
<td>startService()</td>
<td style="text-align:center">启动服务</td>
</tr>
<tr>
<td>stopService()</td>
<td style="text-align:center">关闭服务</td>
</tr>
<tr>
<td>bindService()</td>
<td style="text-align:center">绑定服务</td>
</tr>
<tr>
<td>unbindService()</td>
<td style="text-align:center">解绑服务</td>
</tr>
</tbody>
</table>
<ul>
<li>5个内部自动调用的方法</li>
</ul>
<table>
<thead>
<tr>
<th>内部自动调用的方法</th>
<th style="text-align:center">作用</th>
</tr>
</thead>
<tbody>
<tr>
<td>onCreat()</td>
<td style="text-align:center">创建服务</td>
</tr>
<tr>
<td>onStartCommand()</td>
<td style="text-align:center">开始服务</td>
</tr>
<tr>
<td>onDestroy()</td>
<td style="text-align:center">销毁服务</td>
</tr>
<tr>
<td>onBind()</td>
<td style="text-align:center">绑定服务</td>
</tr>
<tr>
<td>onUnbind()</td>
<td style="text-align:center">解绑服务</td>
</tr>
</tbody>
</table>
<hr>
<h1>2. 生命周期方法具体介绍</h1>
<p>主要介绍内部调用方法和外部调用方法的关系。</p>
<h2>2.1 startService()</h2>
<ul>
<li>作用：启动Service服务</li>
<li>手动调用startService()后，自动调用内部方法：onCreate()、onStartCommand()</li>
<li>调用逻辑如下：<br><div class="image-package">
<img src="images/944365-8b770ac70b0d0402.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-8b770ac70b0d0402.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">调用逻辑</div>
</div>
</li>
</ul>
<h2>2.2 stopService()</h2>
<ul>
<li>作用：关闭Service服务</li>
<li>手动调用stopService()后，自动调用内部方法：onDestory()</li>
<li>调用的逻辑：</li>
</ul>
<div class="image-package">
<img src="images/944365-8b1ee9cf54c8834a.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-8b1ee9cf54c8834a.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">调用逻辑</div>
</div>
<h2>2.3 bindService()</h2>
<ul>
<li>作用：绑定Service服务</li>
<li>手动调用bindService()后，自动调用内部方法：onCreate()、onBind()</li>
<li>调用的逻辑：</li>
</ul>
<div class="image-package">
<img src="images/944365-66932d1330e96541.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-66932d1330e96541.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">调用的逻辑</div>
</div>
<h2>2.4 unbindService()</h2>
<ul>
<li>作用：解绑Service服务</li>
<li>手动调用unbindService()后，自动调用内部方法：onCreate()、onBind()、onDestory()</li>
<li>调用的逻辑：<br><div class="image-package">
<img src="images/944365-ac363af70e26cd2f.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-ac363af70e26cd2f.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">调用的逻辑</div>
</div>
</li>
</ul>
<hr>
<h1>3. 常见的生命周期使用</h1>
<h3>3.1 只使用startService启动服务的生命周期</h3>
<div class="image-package">
<img src="images/944365-c9d086267869945c.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-c9d086267869945c.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">startService启动服务的生命周期</div>
</div>
<h3>3.2 只使用BindService绑定服务的生命周期</h3>
<div class="image-package">
<img src="images/944365-ca62abafd7815297.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-ca62abafd7815297.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">BindService绑定服务的生命周期</div>
</div>
<h3>3.3 同时使用startService()启动服务、BindService()绑定服务的生命周期</h3>
<div class="image-package">
<img src="images/944365-b42335ad20daed14.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-b42335ad20daed14.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">Paste_Image.png</div>
</div>
<h3>3.4 特别注意</h3>
<ul>
<li>startService()和stopService()只能开启和关闭Service，无法操作Service；<blockquote><p>bindService()和unbindService()可以操作Service</p></blockquote>
</li>
<li>startService开启的Service，调用者退出后Service仍然存在；<blockquote><p>BindService开启的Service，调用者退出后，Service随着调用者销毁。</p></blockquote>
</li>
</ul>
<hr>
<h1>4. 总结</h1>
<ul>
<li>本文对Service的生命周期进行了全面解析</li>
<li>
<p>如果你还想了解关于Service的其他知识，请浏览以下文章：</p>
<blockquote><p><a href="http://www.jianshu.com/p/d963c55c3ab9" target="_blank">Android四大组件：Service史上最全面解析</a><br><a href="http://www.jianshu.com/p/8d0cde35eb10" target="_blank">Android：Service生命周期最全面解析</a><br><a href="http://www.jianshu.com/p/e04c4239b07e" target="_blank">Android：（本地、可通信的、前台、远程）Service使用全面介绍</a><br><a href="http://www.jianshu.com/p/34326751b2c6" target="_blank">Android：远程服务Service（含AIDL &amp; IPC讲解）</a><br><a href="http://www.jianshu.com/p/8a3c44a9173a" target="_blank">Android多线程全面解析：IntentService用法&amp;源码</a></p></blockquote>
</li>
<li>
<p>接下来会介绍继续介绍Android开发中的相关知识，有兴趣可以继续关注<a href="http://www.jianshu.com/users/383970bef0a0/latest_articles" target="_blank">Carson_Ho的安卓开发笔记</a></p>
</li>
</ul>
<hr>
<h4>请点赞！因为你们的赞同/鼓励是我写作的最大动力！</h4>
<blockquote><p><strong>相关文章阅读</strong><br><a href="http://www.jianshu.com/p/ec5a1a30694b" target="_blank">Android开发：最全面、最易懂的Android屏幕适配解决方案</a><br><a href="http://www.jianshu.com/p/9fe944ee02f7" target="_blank">Android开发：Handler异步通信机制全面解析（包含Looper、Message Queue）</a><br><a href="http://www.jianshu.com/p/3c94ae673e2a" target="_blank">Android开发：最全面、最易懂的Webview详解</a><br><a href="http://www.jianshu.com/p/b87fee2f7a23" target="_blank">Android开发：JSON简介及最全面解析方法!</a><br><a href="http://www.jianshu.com/p/ce1d060573ba" target="_blank">Android开发：顶部Tab导航栏实现（TabLayout+ViewPager+Fragment）</a><br><a href="http://www.jianshu.com/p/a663803b2a44" target="_blank">Android开发：底部Tab菜单栏实现（FragmentTabHost+ViewPager）</a></p></blockquote>
<hr>
<h3>欢迎关注<a href="http://www.jianshu.com/users/383970bef0a0/latest_articles" target="_blank">Carson_Ho</a>的简书！</h3>
<p>不定期分享关于<strong>安卓开发</strong>的干货，追求<strong>短、平、快</strong>，但<strong>却不缺深度</strong>。</p>
<div class="image-package">
<img src="images/944365-9b76fa3c52d478a7.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-9b76fa3c52d478a7.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption"></div>
</div>

        </div>