# Android四大組件：Service生命週期最全面解析


<div class="show-content">
          <div class="image-package">
<img src="images/944365-207a738cb165a2da.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-207a738cb165a2da.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption"></div>
</div>
<hr>
<h1>前言</h1>
<ul>
<li>Service作為Android四大組件之一，應用非常廣泛</li>
<li>本文將介紹Service最基礎的知識：Service的生命週期<blockquote><p>如果你對Service還未了解，建議先閱讀我寫的文章：<br><a href="http://www.jianshu.com/p/d963c55c3ab9" target="_blank">Android四大組件：Service史上最全面解析</a></p></blockquote>
</li>
</ul>
<hr>
<h1>目錄</h1>
<div class="image-package">
<img src="images/944365-ba6f19f002b9b8f6.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-ba6f19f002b9b8f6.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">目錄</div>
</div>
<hr>
<h1>1. 生命週期常用方法</h1>
<p>在Service的生命週期裡，常用的有：</p>
<ul>
<li>4個手動調用的方法</li>
</ul>
<table>
<thead>
<tr>
<th>手動調用方法</th>
<th style="text-align:center">作用</th>
</tr>
</thead>
<tbody>
<tr>
<td>startService()</td>
<td style="text-align:center">啟動服務</td>
</tr>
<tr>
<td>stopService()</td>
<td style="text-align:center">關閉服務</td>
</tr>
<tr>
<td>bindService()</td>
<td style="text-align:center">綁定服務</td>
</tr>
<tr>
<td>unbindService()</td>
<td style="text-align:center">解綁服務</td>
</tr>
</tbody>
</table>
<ul>
<li>5個內部自動調用的方法</li>
</ul>
<table>
<thead>
<tr>
<th>內部自動調用的方法</th>
<th style="text-align:center">作用</th>
</tr>
</thead>
<tbody>
<tr>
<td>onCreat()</td>
<td style="text-align:center">創建服務</td>
</tr>
<tr>
<td>onStartCommand()</td>
<td style="text-align:center">開始服務</td>
</tr>
<tr>
<td>onDestroy()</td>
<td style="text-align:center">銷燬服務</td>
</tr>
<tr>
<td>onBind()</td>
<td style="text-align:center">綁定服務</td>
</tr>
<tr>
<td>onUnbind()</td>
<td style="text-align:center">解綁服務</td>
</tr>
</tbody>
</table>
<hr>
<h1>2. 生命週期方法具體介紹</h1>
<p>主要介紹內部調用方法和外部調用方法的關係。</p>
<h2>2.1 startService()</h2>
<ul>
<li>作用：啟動Service服務</li>
<li>手動調用startService()後，自動調用內部方法：onCreate()、onStartCommand()</li>
<li>調用邏輯如下：<br><div class="image-package">
<img src="images/944365-8b770ac70b0d0402.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-8b770ac70b0d0402.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">調用邏輯</div>
</div>
</li>
</ul>
<h2>2.2 stopService()</h2>
<ul>
<li>作用：關閉Service服務</li>
<li>手動調用stopService()後，自動調用內部方法：onDestory()</li>
<li>調用的邏輯：</li>
</ul>
<div class="image-package">
<img src="images/944365-8b1ee9cf54c8834a.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-8b1ee9cf54c8834a.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">調用邏輯</div>
</div>
<h2>2.3 bindService()</h2>
<ul>
<li>作用：綁定Service服務</li>
<li>手動調用bindService()後，自動調用內部方法：onCreate()、onBind()</li>
<li>調用的邏輯：</li>
</ul>
<div class="image-package">
<img src="images/944365-66932d1330e96541.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-66932d1330e96541.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">調用的邏輯</div>
</div>
<h2>2.4 unbindService()</h2>
<ul>
<li>作用：解綁Service服務</li>
<li>手動調用unbindService()後，自動調用內部方法：onCreate()、onBind()、onDestory()</li>
<li>調用的邏輯：<br><div class="image-package">
<img src="images/944365-ac363af70e26cd2f.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-ac363af70e26cd2f.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">調用的邏輯</div>
</div>
</li>
</ul>
<hr>
<h1>3. 常見的生命週期使用</h1>
<h3>3.1 只使用startService啟動服務的生命週期</h3>
<div class="image-package">
<img src="images/944365-c9d086267869945c.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-c9d086267869945c.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">startService啟動服務的生命週期</div>
</div>
<h3>3.2 只使用BindService綁定服務的生命週期</h3>
<div class="image-package">
<img src="images/944365-ca62abafd7815297.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-ca62abafd7815297.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">BindService綁定服務的生命週期</div>
</div>
<h3>3.3 同時使用startService()啟動服務、BindService()綁定服務的生命週期</h3>
<div class="image-package">
<img src="images/944365-b42335ad20daed14.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-b42335ad20daed14.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">Paste_Image.png</div>
</div>
<h3>3.4 特別注意</h3>
<ul>
<li>startService()和stopService()只能開啟和關閉Service，無法操作Service；<blockquote><p>bindService()和unbindService()可以操作Service</p></blockquote>
</li>
<li>startService開啟的Service，調用者退出後Service仍然存在；<blockquote><p>BindService開啟的Service，調用者退出後，Service隨著調用者銷燬。</p></blockquote>
</li>
</ul>
<hr>
<h1>4. 總結</h1>
<ul>
<li>本文對Service的生命週期進行了全面解析</li>
<li>
<p>如果你還想了解關於Service的其他知識，請瀏覽以下文章：</p>
<blockquote><p><a href="http://www.jianshu.com/p/d963c55c3ab9" target="_blank">Android四大組件：Service史上最全面解析</a><br><a href="http://www.jianshu.com/p/8d0cde35eb10" target="_blank">Android：Service生命週期最全面解析</a><br><a href="http://www.jianshu.com/p/e04c4239b07e" target="_blank">Android：（本地、可通信的、前臺、遠程）Service使用全面介紹</a><br><a href="http://www.jianshu.com/p/34326751b2c6" target="_blank">Android：遠程服務Service（含AIDL &amp; IPC講解）</a><br><a href="http://www.jianshu.com/p/8a3c44a9173a" target="_blank">Android多線程全面解析：IntentService用法&amp;源碼</a></p></blockquote>
</li>
<li>
<p>接下來會介紹繼續介紹Android開發中的相關知識，有興趣可以繼續關注<a href="http://www.jianshu.com/users/383970bef0a0/latest_articles" target="_blank">Carson_Ho的安卓開發筆記</a></p>
</li>
</ul>
<hr>
<h4>請點贊！因為你們的贊同/鼓勵是我寫作的最大動力！</h4>
<blockquote><p><strong>相關文章閱讀</strong><br><a href="http://www.jianshu.com/p/ec5a1a30694b" target="_blank">Android開發：最全面、最易懂的Android屏幕適配解決方案</a><br><a href="http://www.jianshu.com/p/9fe944ee02f7" target="_blank">Android開發：Handler異步通信機制全面解析（包含Looper、Message Queue）</a><br><a href="http://www.jianshu.com/p/3c94ae673e2a" target="_blank">Android開發：最全面、最易懂的Webview詳解</a><br><a href="http://www.jianshu.com/p/b87fee2f7a23" target="_blank">Android開發：JSON簡介及最全面解析方法!</a><br><a href="http://www.jianshu.com/p/ce1d060573ba" target="_blank">Android開發：頂部Tab導航欄實現（TabLayout+ViewPager+Fragment）</a><br><a href="http://www.jianshu.com/p/a663803b2a44" target="_blank">Android開發：底部Tab菜單欄實現（FragmentTabHost+ViewPager）</a></p></blockquote>
<hr>
<h3>歡迎關注<a href="http://www.jianshu.com/users/383970bef0a0/latest_articles" target="_blank">Carson_Ho</a>的簡書！</h3>
<p>不定期分享關於<strong>安卓開發</strong>的乾貨，追求<strong>短、平、快</strong>，但<strong>卻不缺深度</strong>。</p>
<div class="image-package">
<img src="images/944365-9b76fa3c52d478a7.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/944365-9b76fa3c52d478a7.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption"></div>
</div>

        </div>