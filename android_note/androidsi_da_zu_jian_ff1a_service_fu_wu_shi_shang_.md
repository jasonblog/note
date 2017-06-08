# Android四大組件：Service服務史上最全面解析



<hr>
<h1>前言</h1>
<ul>
<li>Service作為Android四大組件之一，應用非常廣泛</li>
<li>本文將介紹對Service進行全面介紹（基礎認識、生命週期、使用和應用場景）</li>
</ul>
<hr>
<h1>目錄</h1>
<div class="image-package">
<img src="images/944365-087501b69a8c3451.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-087501b69a8c3451.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">目錄</div>
</div>
<hr>
<h1>1. 基礎知識</h1>
<ul>
<li>定義：服務，屬於Android中的計算型組件</li>
<li>作用：提供需要在後臺長期運行的服務（如複雜計算、下載等等）</li>
<li>特點：長生命週期的、沒有用戶界面、在後臺運行</li>
</ul>
<hr>
<h1>2. 生命週期方法詳解</h1>
<p>具體請看我寫的文章：<a href="http://www.jianshu.com/p/8d0cde35eb10" target="_blank">Android：Service生命週期最全面解析</a></p>
<hr>
<h1>3. Service分類</h1>
<h3>3.1 Service的類型</h3>
<div class="image-package">
<img src="images/944365-d42fa78828930bdb.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-d42fa78828930bdb.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">分類</div>
</div>
<h3>3.2  詳細介紹</h3>
<div class="image-package">
<img src="images/944365-8855e3a5340bece5.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-8855e3a5340bece5.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">Service類型的詳細介紹</div>
</div>
<h1>4. Service的使用解析</h1>
<p>由上述可知，服務Service總共分為：</p>
<div class="image-package">
<img src="images/944365-d42fa78828930bdb.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-d42fa78828930bdb.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">分類</div>
</div>
<p>接下來，我將介紹每種Service的具體使用，具體請看我寫的文章:<a href="http://www.jianshu.com/p/e04c4239b07e" target="_blank">Android：（本地、可通信的、前臺、遠程）Service使用全面介紹</a></p>
<h1>5. 使用場景</h1>
<ul>
<li>通過上述描述，你應該對Service類型及其使用非常瞭解；</li>
<li>那麼，我們該什麼時候用哪種類型的Service呢？</li>
<li>各種Service的使用場景請看下圖：<br><div class="image-package">
<img src="images/944365-a1e97f2646e93bbb.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-a1e97f2646e93bbb.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">使用場景</div>
</div>
</li>
</ul>
<h1>6. 其他思考</h1>
<h3>6.1 Service和Thread的區別</h3>
<ul>
<li>結論：Service和Thread之間沒有任何關係</li>
<li>
<p>之所以有不少人會把它們聯繫起來，主要因為Service的後臺概念</p>
<blockquote><p>後臺的定義：後臺任務運行完全不依賴UI，即使Activity被銷燬，或者程序被關閉，只要進程還在，後臺任務就可以繼續運行</p></blockquote>
</li>
<li>
<p>其實二者存在較大的區別，如下圖：<br></p>
<div class="image-package">
<img src="images/944365-384170261e06ecbb.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-384170261e06ecbb.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">Paste_Image.png</div>
</div>
</li>
</ul>
<p>一般來說，會將Service和Thread聯合著用，即在Service中再創建一個子線程（工作線程）去處理耗時操作邏輯，如下代碼：</p>
<pre class="hljs aspectj"><code class="aspectj"><span class="hljs-meta">@Override</span>  
<span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">int</span> <span class="hljs-title">onStartCommand</span><span class="hljs-params">(Intent intent, <span class="hljs-keyword">int</span> flags, <span class="hljs-keyword">int</span> startId)</span> </span>{  
<span class="hljs-comment">//新建工作線程</span>
    <span class="hljs-keyword">new</span> Thread(<span class="hljs-keyword">new</span> Runnable() {  
        <span class="hljs-meta">@Override</span>  
        <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">run</span><span class="hljs-params">()</span> </span>{  
            <span class="hljs-comment">// 開始執行後臺任務  </span>
        }  
    }).start();  
    <span class="hljs-function"><span class="hljs-keyword">return</span> <span class="hljs-keyword">super</span>.<span class="hljs-title">onStartCommand</span><span class="hljs-params">(intent, flags, startId)</span></span>;  
}  

<span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">MyBinder</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">Binder</span> </span>{  
    <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">service_connect_Activity</span><span class="hljs-params">()</span> </span>{  
  <span class="hljs-comment">//新建工作線程</span>
        <span class="hljs-keyword">new</span> Thread(<span class="hljs-keyword">new</span> Runnable() {  
            <span class="hljs-meta">@Override</span>  
            <span class="hljs-keyword">public</span> <span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">run</span><span class="hljs-params">()</span> </span>{  
                <span class="hljs-comment">// 執行具體的下載任務  </span>
            }  
        }).start();  
    }  

}</code></pre>
<h3>6.2 Service和IntentService的區別</h3>
<p>具體請看我寫的文章：<a href="http://www.jianshu.com/p/8a3c44a9173a" target="_blank">Android多線程全面解析：IntentService用法&amp;源碼</a></p>
<hr>
<h1>7.總結</h1>
<ul>
<li>本文對Service的所有知識進行了全面解析（基礎認識、生命週期、使用和應用場景）</li>
<li>如果你還想了解關於Service的其他知識，請瀏覽以下文章：<blockquote><p><a href="http://www.jianshu.com/p/d963c55c3ab9" target="_blank">Android：Service服務史上最全面解析</a><br><a href="http://www.jianshu.com/p/8d0cde35eb10" target="_blank">Android：Service生命週期最全面解析</a><br><a href="http://www.jianshu.com/p/e04c4239b07e" target="_blank">Android：（本地、可通信的、前臺、遠程）Service使用全面介紹</a><br><a href="http://www.jianshu.com/p/34326751b2c6" target="_blank">Android：遠程服務Service（含AIDL &amp; IPC講解）</a><br><a href="http://www.jianshu.com/p/8a3c44a9173a" target="_blank">Android多線程全面解析：IntentService用法&amp;源碼</a></p></blockquote>
</li>
</ul>
<ul>
<li>接下來會介紹繼續介紹Android開發中的相關知識，有興趣可以繼續關注<a href="http://www.jianshu.com/users/383970bef0a0/latest_articles" target="_blank">Carson_Ho的安卓開發筆記</a>
</li>
</ul>
<hr>
<h4>請點贊！因為你們的贊同/鼓勵是我寫作的最大動力！</h4>
<blockquote><p><strong>相關文章閱讀</strong><br><a href="http://www.jianshu.com/p/ec5a1a30694b" target="_blank">Android開發：最全面、最易懂的Android屏幕適配解決方案</a><br><a href="http://www.jianshu.com/p/9fe944ee02f7" target="_blank">Android開發：Handler異步通信機制全面解析（包含Looper、Message Queue）</a><br><a href="http://www.jianshu.com/p/3c94ae673e2a" target="_blank">Android開發：最全面、最易懂的Webview詳解</a><br><a href="http://www.jianshu.com/p/b87fee2f7a23" target="_blank">Android開發：JSON簡介及最全面解析方法!</a><br><a href="http://www.jianshu.com/p/ce1d060573ba" target="_blank">Android開發：頂部Tab導航欄實現（TabLayout+ViewPager+Fragment）</a><br><a href="http://www.jianshu.com/p/a663803b2a44" target="_blank">Android開發：底部Tab菜單欄實現（FragmentTabHost+ViewPager）</a></p></blockquote>
<hr>
