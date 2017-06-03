# android service 之一 (start service)


<div id="blog_content" class="blog_content">
    <p>&nbsp;我们有两种方式（start与bind）启动一个Service,每一种方式启动的Service生命周期是不一样的，这篇贴子主要写的是 start service。</p>
<p>它的生命周期中只有三个阶段：onCreate, onStartCommand(取代原来的onStart方法), onDestroy。如下图：</p>
<p><br><img src="images/015ce6a9-72a2-3250-9cd4-fe70653962d5.jpg" alt=""><br>&nbsp;</p>
<p>需要注意的有：</p>
<p>①&nbsp;如果是 调用者 直接退出而没有调用 stopService 的话，那么被启动的 Service 会一直在后台运行，直至其stopService 方法被调用，或者它自己调用stopSelf 方法。</p>
<p>② 在服务未被创建时，系统会先调用服务的onCreate()方法，接着调用onStartCommand()方法。如果调用startService()方法前服务已经被创建，那么会直接调用onStartCommand()方法。也就是说，多次调用startService()方法并不会导致多次创建服务。另外，不管被 start 多少次，只需一次 stop 便可将相应的service关闭。<br>③ 具体的操作应该放在 onStartCommand()&nbsp;里面<br>以下通过截图来看：</p>
<p><img src="images/74fc29ab-de73-31b2-95ec-bf0172477d55.jpg" alt=""><br><br>上图的中的四个按钮均是针对于同一个Service进行的操作，此时去 application ，可以查看到 "Running Service" 的列表如下：</p>
<p><br><img src="images/c5f0bac4-b0e9-3ab6-aad0-cb6ab647a356.jpg" alt=""><br>&nbsp;</p>
<p><br><img src="images/d03745d4-9c9d-3291-baab-57bee975f24a.jpg" alt=""><br>&nbsp;</p>
<p>这个，我们去点击上面的按钮（分别点击startservice 1 和 2 各两次），结果如下：</p>
<p><br><img src="images/89765e8c-89a4-3f48-8e01-306f7328302e.jpg" alt=""><br>&nbsp;从图中，我们可以看出， onCreate() 方法只在第一次创建服务的时候被调用了。</p>
<p>现在，通过“返回键”来退回至主界面，然后再去 application 的 running service中去查看，可得下面的截图：</p>
<p><br><img src="images/b8290b29-942c-33f1-9aa4-483037db2841.jpg" alt=""><br>&nbsp;从此图中，我们可以看出，虽然Activity被finish掉了，但是由它启动的service仍然在后台运行着。</p>
<p>此时，重新打开该应用，然后直接点击 stop service 1 和 2 按钮各两次（不需再新点击 start service按钮），可以如下截图：</p>
<p><br><img src="images/eb360e07-d7e9-3def-9216-361b3c7c237c.jpg" alt=""><br>&nbsp;从此图中我们可以看出，只有第一次停止服务的时候，才会调用 onDestroy() 方法。</p>
<p>此时，再去 application 的 running service中去查看，可得下面的截图，发现服务确实已经被停止了：</p>
<p><br><img src="images/9d0e2d4a-8e9f-3252-a268-d8b7362e191e.jpg" alt=""><br>&nbsp;</p>
<p>下面附上部分源码（具体地请参见附件）：</p>
<div class="dp-highlighter" id=""><div class="bar"><div class="tools">Java代码 <embed wmode="transparent" src="/javascripts/syntaxhighlighter/clipboard_new.swf" width="14" height="15" flashvars="clipboard=%2F%2F%20%E5%9B%9B%E4%B8%AA%E6%8C%89%E9%92%AE%E7%9A%84%E5%93%8D%E5%BA%94%E4%BA%8B%E4%BB%B6%0A%0Aprivate%20OnClickListener%20btnListener%20%3D%20new%20OnClickListener()%20%7B%0A%09%09%40Override%0A%09%09public%20void%20onClick(View%20v)%20%7B%0A%09%09%09switch%20(v.getId())%20%7B%0A%09%09%09%09case%20R.id.startSer1%3A%0A%09%09%09%09%09updateLog(%22Start%20Service%201%20pressed%22)%3B%0A%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%2F%2F%20%E5%90%AF%E5%8A%A8%E6%9C%8D%E5%8A%A1(%E5%A6%82%E6%9E%9C%E6%83%B3%E4%BC%A0%E9%80%92%E6%95%B0%E6%8D%AE%EF%BC%8C%E4%B9%9F%E5%8F%AF%E4%BB%A5%E5%B0%86%E5%85%B6%E5%B0%81%E8%A3%85%E8%BF%9B%E8%AF%A5intent)%0A%09%09%09%09%09startService(intent)%3B%0A%09%09%09%09%09break%3B%0A%09%09%09%09case%20R.id.startSer2%3A%0A%09%09%09%09%09updateLog(%22Start%20Service%202%20pressed%22)%3B%0A%09%09%09%09%09startService(intent)%3B%0A%09%09%09%09%09break%3B%0A%09%09%09%09case%20R.id.stopSer1%3A%0A%09%09%09%09%09updateLog(%22Stop%20Service%201%20pressed%22)%3B%0A%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%2F%2F%20%E5%81%9C%E6%AD%A2%E6%9C%8D%E5%8A%A1%0A%09%09%09%09%09stopService(intent)%3B%0A%09%09%09%09%09break%3B%0A%09%09%09%09case%20R.id.stopSer2%3A%0A%09%09%09%09%09updateLog(%22Stop%20Service%202%20pressed%22)%3B%0A%09%09%09%09%09stopService(intent)%3B%0A%09%09%09%09%09break%3B%0A%09%09%09%09default%3A%0A%09%09%09%09%09break%3B%0A%09%09%09%7D%0A%09%09%7D%0A%09%7D%3B%0A" quality="high" allowscriptaccess="always" type="application/x-shockwave-flash" pluginspage="http://www.macromedia.com/go/getflashplayer">&nbsp;<a href="javascript:void()" title="收藏这段代码" onclick="code_favorites_do_favorite(this);return false;"><img class="star" src="/images/icon_star.png" alt="收藏代码"><img class="spinner" src="/images/spinner.gif" style="display:none"></a></div></div><ol start="1" class="dp-j"><li><span><span class="comment">//&nbsp;四个按钮的响应事件</span><span>&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;</span></li><li><span><span class="keyword">private</span><span>&nbsp;OnClickListener&nbsp;btnListener&nbsp;=&nbsp;</span><span class="keyword">new</span><span>&nbsp;OnClickListener()&nbsp;{&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="annotation">@Override</span><span>&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="keyword">public</span><span>&nbsp;</span><span class="keyword">void</span><span>&nbsp;onClick(View&nbsp;v)&nbsp;{&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="keyword">switch</span><span>&nbsp;(v.getId())&nbsp;{&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="keyword">case</span><span>&nbsp;R.id.startSer1:&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;updateLog(<span class="string">"Start&nbsp;Service&nbsp;1&nbsp;pressed"</span><span>);&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="comment">//&nbsp;启动服务(如果想传递数据，也可以将其封装进该intent)</span><span>&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;startService(intent);&nbsp;&nbsp;</span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="keyword">break</span><span>;&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="keyword">case</span><span>&nbsp;R.id.startSer2:&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;updateLog(<span class="string">"Start&nbsp;Service&nbsp;2&nbsp;pressed"</span><span>);&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;startService(intent);&nbsp;&nbsp;</span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="keyword">break</span><span>;&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="keyword">case</span><span>&nbsp;R.id.stopSer1:&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;updateLog(<span class="string">"Stop&nbsp;Service&nbsp;1&nbsp;pressed"</span><span>);&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="comment">//&nbsp;停止服务</span><span>&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;stopService(intent);&nbsp;&nbsp;</span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="keyword">break</span><span>;&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="keyword">case</span><span>&nbsp;R.id.stopSer2:&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;updateLog(<span class="string">"Stop&nbsp;Service&nbsp;2&nbsp;pressed"</span><span>);&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;stopService(intent);&nbsp;&nbsp;</span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="keyword">break</span><span>;&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="keyword">default</span><span>:&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="keyword">break</span><span>;&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;}&nbsp;&nbsp;</span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;}&nbsp;&nbsp;</span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;};&nbsp;&nbsp;</span></li></ol></div><pre name="code" class="java" codeable_id="" codeable_type="BlogComment" source_url="http://rainbow702.iteye.com/blog/1142685#" pre_index="0" title="android service 之一 (start service)" style="display: none;">// 四个按钮的响应事件

private OnClickListener btnListener = new OnClickListener() {
		@Override
		public void onClick(View v) {
			switch (v.getId()) {
				case R.id.startSer1:
					updateLog("Start Service 1 pressed");
                     // 启动服务(如果想传递数据，也可以将其封装进该intent)
					startService(intent);
					break;
				case R.id.startSer2:
					updateLog("Start Service 2 pressed");
					startService(intent);
					break;
				case R.id.stopSer1:
					updateLog("Stop Service 1 pressed");
                    // 停止服务
					stopService(intent);
					break;
				case R.id.stopSer2:
					updateLog("Stop Service 2 pressed");
					stopService(intent);
					break;
				default:
					break;
			}
		}
	};
</pre>
<p>&nbsp;</p>
<div class="dp-highlighter" id=""><div class="bar"><div class="tools">Java代码 <embed wmode="transparent" src="/javascripts/syntaxhighlighter/clipboard_new.swf" width="14" height="15" flashvars="clipboard=%2F%2F%20service%20%E7%9A%84%E5%AE%9E%E7%8E%B0%0Apublic%20class%20MyService%20extends%20Service%20%7B%0A%09private%20static%20final%20String%20TAG%20%3D%20%22MyService%22%3B%0A%0A%09%40Override%0A%09public%20void%20onCreate()%20%7B%0A%09%09super.onCreate()%3B%0A%09%09MyServiceActivity.updateLog(TAG%20%2B%20%22%20----%3E%20onCreate()%22)%3B%0A%09%7D%0A%0A%09%40Override%0A%09public%20int%20onStartCommand(Intent%20intent%2C%20int%20flags%2C%20int%20startId)%20%7B%0A%09%09MyServiceActivity.updateLog(TAG%20%2B%20%22%20----%3E%20onStartCommand()%22)%3B%0A%09%09return%20START_STICKY%3B%0A%09%7D%0A%0A%09%40Override%0A%09public%20IBinder%20onBind(Intent%20intent)%20%7B%0A%09%09MyServiceActivity.updateLog(TAG%20%2B%20%22%20----%3E%20onBind()%22)%3B%0A%09%09return%20null%3B%0A%09%7D%0A%0A%09%40Override%0A%09public%20void%20onDestroy()%20%7B%0A%09%09super.onDestroy()%3B%0A%09%09MyServiceActivity.updateLog(TAG%20%2B%20%22%20----%3E%20onDestroy()%22)%3B%0A%09%7D%0A%7D" quality="high" allowscriptaccess="always" type="application/x-shockwave-flash" pluginspage="http://www.macromedia.com/go/getflashplayer">&nbsp;<a href="javascript:void()" title="收藏这段代码" onclick="code_favorites_do_favorite(this);return false;"><img class="star" src="/images/icon_star.png" alt="收藏代码"><img class="spinner" src="/images/spinner.gif" style="display:none"></a></div></div><ol start="1" class="dp-j"><li><span><span class="comment">//&nbsp;service&nbsp;的实现</span><span>&nbsp;&nbsp;</span></span></li><li><span><span class="keyword">public</span><span>&nbsp;</span><span class="keyword">class</span><span>&nbsp;MyService&nbsp;</span><span class="keyword">extends</span><span>&nbsp;Service&nbsp;{&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;<span class="keyword">private</span><span>&nbsp;</span><span class="keyword">static</span><span>&nbsp;</span><span class="keyword">final</span><span>&nbsp;String&nbsp;TAG&nbsp;=&nbsp;</span><span class="string">"MyService"</span><span>;&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;</span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;<span class="annotation">@Override</span><span>&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;<span class="keyword">public</span><span>&nbsp;</span><span class="keyword">void</span><span>&nbsp;onCreate()&nbsp;{&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="keyword">super</span><span>.onCreate();&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;MyServiceActivity.updateLog(TAG&nbsp;+&nbsp;<span class="string">"&nbsp;----&gt;&nbsp;onCreate()"</span><span>);&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;}&nbsp;&nbsp;</span></li><li><span>&nbsp;&nbsp;</span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;<span class="annotation">@Override</span><span>&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;<span class="keyword">public</span><span>&nbsp;</span><span class="keyword">int</span><span>&nbsp;onStartCommand(Intent&nbsp;intent,&nbsp;</span><span class="keyword">int</span><span>&nbsp;flags,&nbsp;</span><span class="keyword">int</span><span>&nbsp;startId)&nbsp;{&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;MyServiceActivity.updateLog(TAG&nbsp;+&nbsp;<span class="string">"&nbsp;----&gt;&nbsp;onStartCommand()"</span><span>);&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="keyword">return</span><span>&nbsp;START_STICKY;&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;}&nbsp;&nbsp;</span></li><li><span>&nbsp;&nbsp;</span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;<span class="annotation">@Override</span><span>&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;<span class="keyword">public</span><span>&nbsp;IBinder&nbsp;onBind(Intent&nbsp;intent)&nbsp;{&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;MyServiceActivity.updateLog(TAG&nbsp;+&nbsp;<span class="string">"&nbsp;----&gt;&nbsp;onBind()"</span><span>);&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="keyword">return</span><span>&nbsp;</span><span class="keyword">null</span><span>;&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;}&nbsp;&nbsp;</span></li><li><span>&nbsp;&nbsp;</span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;<span class="annotation">@Override</span><span>&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;<span class="keyword">public</span><span>&nbsp;</span><span class="keyword">void</span><span>&nbsp;onDestroy()&nbsp;{&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class="keyword">super</span><span>.onDestroy();&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;MyServiceActivity.updateLog(TAG&nbsp;+&nbsp;<span class="string">"&nbsp;----&gt;&nbsp;onDestroy()"</span><span>);&nbsp;&nbsp;</span></span></li><li><span>&nbsp;&nbsp;&nbsp;&nbsp;}&nbsp;&nbsp;</span></li><li><span>}&nbsp;&nbsp;</span></li></ol></div><pre name="code" class="java" codeable_id="" codeable_type="BlogComment" source_url="http://rainbow702.iteye.com/blog/1142685#" pre_index="1" title="android service 之一 (start service)" style="display: none;">// service 的实现
public class MyService extends Service {
	private static final String TAG = "MyService";

	@Override
	public void onCreate() {
		super.onCreate();
		MyServiceActivity.updateLog(TAG + " ----&gt; onCreate()");
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		MyServiceActivity.updateLog(TAG + " ----&gt; onStartCommand()");
		return START_STICKY;
	}

	@Override
	public IBinder onBind(Intent intent) {
		MyServiceActivity.updateLog(TAG + " ----&gt; onBind()");
		return null;
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		MyServiceActivity.updateLog(TAG + " ----&gt; onDestroy()");
	}
}</pre>
<p>&nbsp;&nbsp;&nbsp;</p>
  </div>