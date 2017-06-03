# Android之Activity總結


<div class="show-content">
          <h2>1. 簡介</h2>
<p>本篇不針對於新手，而是對於Activity中一些常識或者問題進行總結。Activity是Android四大組件之一，為用戶提供與系統交互的界面，每一個應用都有一個或者多個Acticity，這樣會有各種各樣的細節問題需要查找，我將本人接觸到的知識點彙總到此篇文章。</p>
<h2>2. 生命週期</h2>
<p>Activity生命週期的回調主要有onCreate()、onRestart()、onStart()、onResume()、onPause()、onStop()、onDestory()這幾個方法，Activity的生命週期類別主要分為三種，如下。</p>
<ul>
<li>
<strong>整個生命週期</strong>，Activity完整生命週期發生在onCreate()和onDestroy()之間。在onCreate()中執行一些全局性的設置（例如設置佈局文件，初始化View等等），在onDestroy()中釋放所有資源</li>
<li>
<strong>可見生命週期</strong>，Activity可見生命週期發生在onStart()和onStop()之間，在這段時間內，用戶可以在屏幕上看見Activity並與之交互。在整個生命週期，Activity對用戶可見和隱藏兩種狀態可能交替出現，系統就會多次調用onStart()和onStop()方法。</li>
<li>
<strong>前臺生命週期</strong>，Activity的前臺生命週期發生在onResume()和onPause()之間，在這段時間內，Activity位於屏幕上其他Activiy之前，而且獲取屏幕的焦點。Activity可能頻繁的轉入或轉出前臺，例如當設備休眠或者彈出對話框時，系統會調用onPause()方法。因為此狀態可能經常發生變化，所以在這兩個方法中建議做一些輕量級操作。</li>
</ul>
<p>Activity生命週期圖如下：</p>
<div class="image-package">
<img src="images/354846-e175a1066ca56f85.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/640" data-original-src="images/354846-e175a1066ca56f85.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">圖-1 Activity生命週期</div>
</div>
<h2>3. 啟動與關閉</h2>
<h3>3.1 啟動</h3>
<p>被啟動的Activity必須要在AndroidManifest.xml文件中聲明，否則會拋出異常。</p>
<ul>
<li>
<p>正常啟動一個Activity的代碼如下：</p>
<pre class="hljs java"><code class="java">  <span class="hljs-comment">// 顯示啟動</span>
  Intent intent = <span class="hljs-keyword">new</span> Intent(<span class="hljs-keyword">this</span>, MyActivity.class);
  <span class="hljs-comment">// 設置傳遞的數據</span>
  intent.put(KEY_NAME, value);
  startActivity(intent);

  <span class="hljs-comment">// 隱式啟動</span>
  Intent intent = <span class="hljs-keyword">new</span> Intent(ACTION_NAME);
  <span class="hljs-comment">// 設置其他匹配規則</span>
  ...
  <span class="hljs-comment">// 設置傳遞的數據，bundle數據集</span>
  intent.putExtras(bundle);
  startActivity(intent);</code></pre>
</li>
<li>
<p>啟動一個Activity並獲取其執行結果。</p>
<pre class="hljs java"><code class="java">  Intent intent = <span class="hljs-keyword">new</span> Intent(<span class="hljs-keyword">this</span>, MyActivity.class);
  startActivityForResult(intent, REQUEST_CODE);</code></pre>
<p>  需要當前Activity重寫onActivityResult()方法以獲取結果。</p>
<pre class="hljs java"><code class="java">  <span class="hljs-meta">@Override</span>
  <span class="hljs-function"><span class="hljs-keyword">protected</span> <span class="hljs-keyword">void</span> <span class="hljs-title">onActivityResult</span><span class="hljs-params">(<span class="hljs-keyword">int</span> requestCode, <span class="hljs-keyword">int</span> resultCode, Intent data)</span> </span>{
      <span class="hljs-comment">// 如果結果碼是OK，而且請求碼和我們設置的請求碼相同</span>
      <span class="hljs-keyword">if</span> (resultCode == Activity.RESULT_OK &amp;&amp; requestCode == REQUEST_CODE)
      {
          <span class="hljs-comment">// 其他操作</span>
      }        
  }</code></pre>
</li>
</ul>
<h3>3.2 關閉</h3>
<ul>
<li>
<code>finish()</code>結束當前的Activity</li>
<li>
<code>finishActivity(int requestCode)</code>結束當前Activity使用<code>startActivityForResult()</code>方法啟動的子Activity</li>
</ul>
<h2>4. 啟動其他Activity時候生命週期協調</h2>
<p>當一個Activity A去啟動一個新的Activity B時候，A和B的生命週期並不是依次進行，也就是說它們的生命週期會有所重疊。在創建B的時候，A不會完全停止，更確切的說，啟動B的過程與A停止的過程會有重疊。所以A和B生命週期回調的順序就很重要了，回調順序如下。</p>
<ul>
<li>Activity A的onPause()方法執行</li>
<li>Activity B的onCreate()、onStart()、onResume()方法依次執行。onResume()方法執行後，Activity B獲取屏幕焦點</li>
<li>Activity A的onStop()方法執行</li>
</ul>
<p>在知道了從一個Activity到另一個Activity轉變時候生命週期的順序，平時研發時候就需要注意了。例如，當必須在第一個Activity停止之前存儲數據，以便下一個Activity能夠使用，應該在onPause()方法中儲存而不是onStop()方法中。</p>
<h2>5. 狀態保存</h2>
<p>用戶與頁面交互過程中，會出現應用前後臺切換或者進入其他頁面等等的情況，也就是Activity調用暫停(onPause)或者停止(onStop)但是未調用(onDestroy)，此時Activity仍然在內存中，其有關狀態和成員信息處於活躍狀態，用戶在Activity中所作的任何更改都會得到保留，這樣一來，當Activity返回前臺繼續執行時候，這些更改信息依然存在，頁面能夠繼續顯示。</p>
<p>但是一旦系統需要內存而將某個Activity銷燬時，當再次回到這個Activity，系統需要重建這個Activity，但是用戶並不知道系統銷燬了這個Activity需要重建，他們希望返回頁面時候頁面還是保存之前的狀態。這種情況下，需要我們手動將一些信息給保存起來，可以實現Activity中的另一個回調方法<code>onSaveInstanceState()</code>，保存Activity狀態的一些重要信息。系統會向該方法傳遞一個Bundle，然後我們可以向這個Bundle裡面儲存一些重要信息。當系統重建Activity時候，系統會將這個Bundle同時傳遞給<code>onCreate()</code>和<code>onRestoreInstanceState()</code>方法，我們可以在這兩個方法中恢復之前場景。</p>
<p>看一下狀態保存的介紹圖。</p>
<div class="image-package">
<img src="images/354846-d9d333eded62323d.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/354846-d9d333eded62323d.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">圖-2 Activity狀態保存</div>
</div>
<p>面試時候可能會問到<code>onSaveInstanceState()</code>調用時機，看一下官方源代碼的註釋。</p>
<blockquote>
<p>Do not confuse this method with activity lifecycle callbacks such as {@link #onPause}, which is always called when an activity is being placed in the background or on its way to destruction, or {@link #onStop} which is called before destruction.  One example of when {@link #onPause} and {@link #onStop} is called and not this method is when a user navigates back from activity B to activity A。</p>
<p>不要把<code>onSaveInstanceState()</code>這個方法和Activity生命週期的幾個方法混淆了，這個方法只有在Activity切換到後臺或者即將被銷燬時候被調用。有一個例子是如果從Activity B返回到Activity A，這個方法是不會被調用的。</p>
</blockquote>
<p>也許很難理解這段註釋的意思，我個人理解是，如果一個Activity失去了屏幕焦點後，失去屏幕焦點一般是指<code>onPause()</code>和<code>onStop()</code>方法被調用，<code>onSaveInstanceState()</code>方法就會被調用，有一種特殊情況是從一個Activity B返回到上一個Activity A，這個方法並不會被調用。</p>
<p>個人總結了一下，大體有以下幾種情況系統可能會調用<code>onSaveInstanceState()</code></p>
<ul>
<li>按下Home將程序切換到後臺</li>
<li>關閉屏幕</li>
<li>Activity A啟動一個新的Activity B，會回調A中<code>onSaveInstanceState()</code>方法</li>
<li>屏幕橫豎屏方向切換</li>
<li>長按Home或者菜單鍵進入程序列表頁面</li>
</ul>
<blockquote><p>為什麼平時並沒有實現<code>onSaveInstanceState()</code>和<code>onRestoreInstanceState()</code>方法，但是有些時候，Activity中的UI狀態依然得到了保存，是為什麼？</p></blockquote>
<p>在Android中，Activity類的<code>onSaveInstanceState()</code>方法默認實現會調用佈局中每個View的<code>onSaveInstanceState()</code>方法去保存其本身的狀態信息，Android框架中幾乎每個控件都會實現這個方法。我們只需要為想要保存其狀態的每個控件提供一個唯一的ID（在xml中設置 <code>android:id</code>屬性），如果控件沒有 ID，則系統無法保存其狀態。</p>
<p>我們可以通過將View的<code>android:saveEnabled</code>屬性設置為<code>false</code>或通過調用View的<code>setSaveEnabled()</code>方法顯式阻止佈局內的視圖保存其狀態，通常不需要設置這些屬性，但如果想以不同方式恢復Activity UI的狀態，可以這樣做。</p>
<p><strong>注：由於無法保證系統調用onSaveInstanceState()的時機，我們只用它來保存Activity的瞬間狀態，不要用它來儲存持久性數據，上面提到過，建議在onPause()中儲存持久性數據。</strong></p>
<h2>6. 啟動模式</h2>
<p>敲黑板敲黑板，劃重點來了，同學們快拿出筆和紙快做筆記。</p>
<p>一個應用一般包含很多Activity，它們按照各自打開的順序排列在返回棧（Back Stack）中，這些Activity統稱為Task。大多數Task的起點是用戶在屏幕中點擊應用圖標啟動應用，該應用的Task出現在前臺，如果該應用沒有Task，也就是最近未被打開，則會新建一個Task，並且會將該應用的MainActivity加入返回棧中，作為返回棧中的根Activity。</p>
<p>通常情況下，當前一個Activity啟動一個新的Activity時候，新的Activity會被加入返回棧中，並處於棧頂，獲取屏幕焦點，而前一個Activity仍保留在返回棧中，處於停止(onStop)狀態。 Activity停止時，如上所說，系統會保存其頁面狀態。當用戶返回時候，當前處於棧頂的Activity會從返回棧中彈出，並被銷燬(onDestroy)，恢復前一個Activity的狀態。返回棧中的Activity永遠不會重新排列，遵循<strong>先進後出</strong>的原則。</p>
<div class="image-package">
<img src="images/354846-1cb59bea18e38607.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/640" data-original-src="images/354846-1cb59bea18e38607.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">圖-3 Activity出入返回棧</div>
</div>
<p>上述講的只是標準的Activity與返回棧的關係，在Android中Activity有四種啟動模式，分別是<code>standard</code>、<code>singleTop</code>、<code>singleTask</code>、<code>singleInstance</code>。</p>
<h3>6.1 設置啟動模式</h3>
<p>我們可以通過在AndroidManifest.xml配置Activity的啟動模式。</p>
<pre class="hljs xml"><code class="xml"><span class="hljs-tag">&lt;<span class="hljs-name">activity</span>
    <span class="hljs-attr">android:name</span>=<span class="hljs-string">".aidldemo.BindingActivity"</span>
    <span class="hljs-attr">android:launchMode</span>=<span class="hljs-string">"standard"</span>
    <span class="hljs-attr">...</span> /&gt;</span></code></pre>
<p>或者在代碼中向Intent添加相應標誌。</p>
<pre class="hljs java"><code class="java">Intent intent = <span class="hljs-keyword">new</span> Intent(<span class="hljs-keyword">this</span>, MyActivity.class);  
intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);  
startActivity(intent);</code></pre>
<p><strong>第二種方法設置啟動模式的優先級高於第一種，如果兩者都存在，以第二種為準。</strong></p>
<h3>6.2 standard（默認模式）</h3>
<p>默認的啟動模式，新啟動的Activity放入返回棧棧頂，遵循<strong>先進後出</strong>原則，同一個Activity可以被實例化多次。</p>
<h3>6.3 singleTop</h3>
<ul>
<li>如果當前返回棧的頂部不存在該Activity，則新建該Activity並放入棧頂；</li>
<li>如果當前返回棧的頂部已存在Activity的一個實例，則系統會通過調用該實例的onNewIntent()方法向其傳送Intent，不創建該Activity的新實例。</li>
</ul>
<h3>6.4 singleTask</h3>
<ul>
<li>如果該Activity需要的返回棧是A，但是當前系統中不存在A返回棧，系統會新建返回棧A，然後再創建該Activity實例將其壓入返回棧中。</li>
<li>如果該Activity需要的返回棧存在，而且返回棧中沒有該Activity，則新建Activity並放入Task棧頂</li>
<li>如果該Activity需要的返回棧存在，而且返回棧中有該Activity<ul>
<li>如果該Activity在棧頂，調用其<code>onNewIntent()</code>方法傳入Intent</li>
<li>如果該Activity不在棧頂，彈出其上面的所有Activity，讓該Activity置於棧頂，並調用其<code>onNewIntent()</code>方法傳入Intent</li>
</ul>
</li>
</ul>
<p>默認情況下，所有Activity所需要的返回棧名稱為應用的包名，我們可以在AndroidManifest.xml中通過設置Activity的android:taskAffinity屬性來指定該Activity需要的返回棧名稱，這個名稱不能和應用包名相同，否則相當於沒有指定。<code>taskAffinity</code>翻譯過來是返回棧親和性，我個人理解這個屬性是指定與返回棧親和度或者優先級，並不是每次都會新建返回棧。注意一般android:taskAffinity屬性和<code>singleTask</code>一起使用才有意義，會新建返回棧，如果只是指定了android:taskAffinity屬性但是依然是<code>singleTop</code>和<code>standard</code>模式，新啟動的Activity依然會在原來的返回棧中。</p>
<h3>6.5 singleInstance</h3>
<p>系統創建一個新的Task並創建Activity的新實例置於新Task返回棧中，但是系統不會將任何其他Activity的實例放入這個新建的Task中。該Activity始終是其Task唯一僅有的成員，由此Activity啟動的任何Activity，如果沒有指定返回棧名稱，則新啟動的Activity放入默認的返回棧；如果指定了返回棧名稱，則將新啟動的Activity放入指定的返回棧中。</p>
<h3>6.6 返回棧順序調用圖(個人理解，大家可跳過)</h3>
<p>Android中返回棧分為前臺返回棧和後臺返回棧，前臺返回棧是指返回棧棧頂的Activity正在和用戶進行交互。<br>上面說了幾種啟動模式，下面看一下幾種啟動模式混合時候返回棧調度情況，我個人的理解和官方有些不同，這個大家可以跳過，去看官方的介紹。</p>
<p>個人理解一個應用創建的默認返回棧為基準，按返回鍵時候，根據返回棧創建順序依次清空返回棧，當默認返回棧清空時候，應用也就關閉了，但是有些後臺返回棧中的Activity並不會立即銷燬。</p>
<p>Activity A和Activity B為默認啟動模式，未設置taskAffinity屬性。<br>Activity C啟動模式是singleTask，設置了taskAffinity屬性。<br>啟動順序是<code>Activity A -&gt; Activity B -&gt; Activity C</code><br>看一下返回棧調用情況</p>
<div class="image-package">
<img src="images/354846-301ce03b66d8d7b0.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/640" data-original-src="images/354846-301ce03b66d8d7b0.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">圖-4 返回棧圖1</div>
</div>
<p>Activity A和Activity B為默認啟動模式，A未設置taskAffinity屬性，B設置taskAffinity屬性為默認返回棧。<br>Activity C和Activity D啟動模式是singleTask，設置了相同的taskAffinity屬性。<br>啟動順序是<code>Activity A -&gt; Activity C -&gt; Activity D -&gt; Activity B</code><br>返回棧調用情況如下圖</p>
<div class="image-package">
<img src="images/354846-58bcf9c955865978.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/640" data-original-src="images/354846-58bcf9c955865978.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">圖-5 返回棧圖2</div>
</div>
<p>官方圖，這裡注意一下，官方圖中在<code>StartActivity Y</code>後，Y與X所在返回棧和1與2所在的返回棧是不同的，他們並不在同一個返回棧：<br></p><div class="image-package">
<img src="images/354846-2d34fdcecd4a31bc.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/354846-2d34fdcecd4a31bc.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">圖-6 返回棧圖3</div>
</div>
<p>Activity A和Activity C為默認啟動模式，未設置taskAffinity屬性<br>Activity B啟動模式是singleInstance<br>啟動順序是<code>Activity A -&gt; Activity B -&gt; Activity C</code><br>返回棧調用情況如下圖</p>
<div class="image-package">
<img src="images/354846-cafce21d989c846d.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/640" data-original-src="images/354846-cafce21d989c846d.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">圖-7 返回棧圖4</div>
</div>
<p><strong>按返回鍵和啟動Activity從返回棧A到返回棧B結果是不同的，按返回鍵時候，會首先彈出返回棧A中的Activity，等到返回棧沒有Activity時候，才會進入另一個返回棧，這個時候返回棧A已經沒有Activity了。</strong></p>
<h3>6.7 XML添加屬性和Intent添加標籤設置啟動模式對比</h3>
<p>上面講到的四種啟動模式都是在<code>Androidmanifest.xml</code>中設置啟動模式，也提及了用Intent添加flags來設置啟動模式。下面針對兩種方法做一下對比。</p>
<ul>
<li>
<code>FLAG_ACTIVITY_SINGLE_TOP</code>，這個標記的作用是為Activity指定<code>singleTop</code>啟動模式，與在XML設置啟動模式相同</li>
<li>
<code>FLAG_ACTIVITY_NEW_TASK</code>，這個標記只是在設置了taskAffinity屬性下有意義，如果被標記的Activity需要de 返回棧不存在，則新建返回棧，然後新建Activity；如果被標記Activity需要的返回棧存在，則將返回棧帶回前臺，並不會創建新的Activity或者調用onNewIntent。</li>
<li>
<code>FLAG_ACTIVITY_CLEAR_TOP</code>，用這個標記啟動的Activity，當它啟動是，在同一個任務戰中所有位於它上面的Activity都要出棧。這個標記一般會和<code>singleTask</code>啟動模式一起出現，在這種情況下，被啟動的Activity的實力如果已經存在，那麼系統就會調用它的onNewIntent。如果被啟動的Activity採用<code>standard</code>啟動模式，那麼連同<strong>它和它之上</strong>的Activity都要出棧，系統會創建新的Activity實力並放入棧頂。<code>singleTask</code>啟動模式默認就具有此標記的效果。</li>
<li>
<code>FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS</code>，用這個標記啟動的Activity不會出現在近期任務中，也就是在Android任務列表中隱藏了該Activity，和在XML設置<code>android:excludeFromRecents="true"</code>相同，這個屬性需要配合<code>singleInstance</code>啟動模式才有用</li>
</ul>
<p>可以看到XML設置沒有類似<code>FLAG_ACTIVITY_CLEAR_TOP</code>標記這種效果的啟動模式，而標記中沒有<code>singleInstance</code>這種啟動模式的標記。</p>
<p><strong>平時如果我們使用ApplicationContext.startActivity去啟動一個<code>standard</code>啟動模式的Activity時候，會報錯如下，這是因為<code>standard</code>模式的Activity會默認進入啟動它的Activity的返回棧中，但是由於非Activity類型的Context(如ApplicationContext)並沒有所謂的返回棧，所以拋出這個異常。解決這個問題的方法就是在Intent中添加<code>FLAG_ACTIVITY_NEW_TASK</code>的標記，這個時候啟動其實是以<code>singleTask</code>模式啟動的</strong></p>
<pre class="hljs livecodeserver"><code class="livecodeserver">ERROR/AndroidRuntime(<span class="hljs-number">5066</span>): 
Caused <span class="hljs-keyword">by</span>: 
android.util.AndroidRuntimeException: 
Calling startActivity() <span class="hljs-built_in">from</span> outside <span class="hljs-keyword">of</span> <span class="hljs-keyword">an</span> Activity context requires <span class="hljs-keyword">the</span> FLAG_ACTIVITY_NEW_TASK flag. Is this really what you want?</code></pre>
<h3>6.8 taskAffinity和allowTaskReparenting結合</h3>
<p>在AndroidManifest.xml中可以為Activity同時設置這兩個屬性，taskAffinity這個屬性上面將結果，allowTaskReparenting這個屬性是標記Activity是否可以更換返回棧，也就是從一個返回棧轉移到另一個返回棧。當應用需要給其他應用提供頁面支持的時候，這兩者結合起來就很有意義。</p>
<p>B應用提供了一個對外的Activity C，taskAffinity屬性是應用B包名，allowTaskReparenting設置為true。</p>
<p>應用A調用了應用B的Activity C，然後按Home鍵退回到主屏幕，單擊應用B的桌面圖標。</p>
<ul>
<li>如果應用B已經啟動，Activity C會出現在應用B返回棧的棧頂，應用B顯示Activity C，應用A中Activity C消失，也就將是Activity C從應用A的返回棧轉移到應用B的返回棧</li>
<li>如果應用B未啟動，這個時候並不是啟動應用B的MainActivity，而是重新顯示了已經被應用A啟動的Activity C，或者說Activity C從應用A的返回棧轉移到了應用B的返回棧中。可以理解成，由於應用A啟動Activity C，當應用B啟動新建返回棧時候，系統發現Activity C原本所想要的返回棧創建完畢，就把Activity C從應用A的返回棧轉移到應用B的返回棧。</li>
</ul>
<h2>7. 清空返回棧</h2>
<p>如果用戶將應用長時間的切換到後臺，系統會清除返回棧中除了根Activity的所有Activity。當用戶再次回到應用時候，僅恢復根Activity。有幾個標籤能夠協助我們控制返回棧的清空。</p>
<ul>
<li>
<code>alwaysRetainTaskState</code>，如果根Activity的該屬性設置為True，系統會長時間的保存所有的Activity在返回棧中，並不會清空。（殺死應用除外）</li>
<li>
<code>clearTaskOnLaunch</code>，如果根Activity的該屬性設置為True，每當用戶離開再返回時候，系統都會將返回棧清空只留下根Activity。這個屬性和<code>alwaysRetainTaskState</code>剛好相反，即時用戶只離開片刻，系統也會清空返回棧。</li>
</ul>
<h2>8.結束語</h2>
<p>從開始學Android開始，就想對四大組件進行一些梳理，將自己的知識點細化並記錄下來，可能網上已經有很多關於Activity的文章，沒用的很多，還是自己來寫靠譜，溫故而知新，共勉。</p>
<p>注：關於Activity啟動匹配比較複雜，可以去查看我的另一篇文章<a href="http://www.jianshu.com/p/a7535c3f33df" target="_blank">淺談Intent以及IntentFilter</a>。</p>

        </div>