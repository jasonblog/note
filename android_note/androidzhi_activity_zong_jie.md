# Android之Activity总结


<div class="show-content">
          <h2>1. 简介</h2>
<p>本篇不针对于新手，而是对于Activity中一些常识或者问题进行总结。Activity是Android四大组件之一，为用户提供与系统交互的界面，每一个应用都有一个或者多个Acticity，这样会有各种各样的细节问题需要查找，我将本人接触到的知识点汇总到此篇文章。</p>
<h2>2. 生命周期</h2>
<p>Activity生命周期的回调主要有onCreate()、onRestart()、onStart()、onResume()、onPause()、onStop()、onDestory()这几个方法，Activity的生命周期类别主要分为三种，如下。</p>
<ul>
<li>
<strong>整个生命周期</strong>，Activity完整生命周期发生在onCreate()和onDestroy()之间。在onCreate()中执行一些全局性的设置（例如设置布局文件，初始化View等等），在onDestroy()中释放所有资源</li>
<li>
<strong>可见生命周期</strong>，Activity可见生命周期发生在onStart()和onStop()之间，在这段时间内，用户可以在屏幕上看见Activity并与之交互。在整个生命周期，Activity对用户可见和隐藏两种状态可能交替出现，系统就会多次调用onStart()和onStop()方法。</li>
<li>
<strong>前台生命周期</strong>，Activity的前台生命周期发生在onResume()和onPause()之间，在这段时间内，Activity位于屏幕上其他Activiy之前，而且获取屏幕的焦点。Activity可能频繁的转入或转出前台，例如当设备休眠或者弹出对话框时，系统会调用onPause()方法。因为此状态可能经常发生变化，所以在这两个方法中建议做一些轻量级操作。</li>
</ul>
<p>Activity生命周期图如下：</p>
<div class="image-package">
<img src="images/354846-e175a1066ca56f85.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/640" data-original-src="images/354846-e175a1066ca56f85.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">图-1 Activity生命周期</div>
</div>
<h2>3. 启动与关闭</h2>
<h3>3.1 启动</h3>
<p>被启动的Activity必须要在AndroidManifest.xml文件中声明，否则会抛出异常。</p>
<ul>
<li>
<p>正常启动一个Activity的代码如下：</p>
<pre class="hljs java"><code class="java">  <span class="hljs-comment">// 显示启动</span>
  Intent intent = <span class="hljs-keyword">new</span> Intent(<span class="hljs-keyword">this</span>, MyActivity.class);
  <span class="hljs-comment">// 设置传递的数据</span>
  intent.put(KEY_NAME, value);
  startActivity(intent);

  <span class="hljs-comment">// 隐式启动</span>
  Intent intent = <span class="hljs-keyword">new</span> Intent(ACTION_NAME);
  <span class="hljs-comment">// 设置其他匹配规则</span>
  ...
  <span class="hljs-comment">// 设置传递的数据，bundle数据集</span>
  intent.putExtras(bundle);
  startActivity(intent);</code></pre>
</li>
<li>
<p>启动一个Activity并获取其执行结果。</p>
<pre class="hljs java"><code class="java">  Intent intent = <span class="hljs-keyword">new</span> Intent(<span class="hljs-keyword">this</span>, MyActivity.class);
  startActivityForResult(intent, REQUEST_CODE);</code></pre>
<p>  需要当前Activity重写onActivityResult()方法以获取结果。</p>
<pre class="hljs java"><code class="java">  <span class="hljs-meta">@Override</span>
  <span class="hljs-function"><span class="hljs-keyword">protected</span> <span class="hljs-keyword">void</span> <span class="hljs-title">onActivityResult</span><span class="hljs-params">(<span class="hljs-keyword">int</span> requestCode, <span class="hljs-keyword">int</span> resultCode, Intent data)</span> </span>{
      <span class="hljs-comment">// 如果结果码是OK，而且请求码和我们设置的请求码相同</span>
      <span class="hljs-keyword">if</span> (resultCode == Activity.RESULT_OK &amp;&amp; requestCode == REQUEST_CODE)
      {
          <span class="hljs-comment">// 其他操作</span>
      }        
  }</code></pre>
</li>
</ul>
<h3>3.2 关闭</h3>
<ul>
<li>
<code>finish()</code>结束当前的Activity</li>
<li>
<code>finishActivity(int requestCode)</code>结束当前Activity使用<code>startActivityForResult()</code>方法启动的子Activity</li>
</ul>
<h2>4. 启动其他Activity时候生命周期协调</h2>
<p>当一个Activity A去启动一个新的Activity B时候，A和B的生命周期并不是依次进行，也就是说它们的生命周期会有所重叠。在创建B的时候，A不会完全停止，更确切的说，启动B的过程与A停止的过程会有重叠。所以A和B生命周期回调的顺序就很重要了，回调顺序如下。</p>
<ul>
<li>Activity A的onPause()方法执行</li>
<li>Activity B的onCreate()、onStart()、onResume()方法依次执行。onResume()方法执行后，Activity B获取屏幕焦点</li>
<li>Activity A的onStop()方法执行</li>
</ul>
<p>在知道了从一个Activity到另一个Activity转变时候生命周期的顺序，平时研发时候就需要注意了。例如，当必须在第一个Activity停止之前存储数据，以便下一个Activity能够使用，应该在onPause()方法中储存而不是onStop()方法中。</p>
<h2>5. 状态保存</h2>
<p>用户与页面交互过程中，会出现应用前后台切换或者进入其他页面等等的情况，也就是Activity调用暂停(onPause)或者停止(onStop)但是未调用(onDestroy)，此时Activity仍然在内存中，其有关状态和成员信息处于活跃状态，用户在Activity中所作的任何更改都会得到保留，这样一来，当Activity返回前台继续执行时候，这些更改信息依然存在，页面能够继续显示。</p>
<p>但是一旦系统需要内存而将某个Activity销毁时，当再次回到这个Activity，系统需要重建这个Activity，但是用户并不知道系统销毁了这个Activity需要重建，他们希望返回页面时候页面还是保存之前的状态。这种情况下，需要我们手动将一些信息给保存起来，可以实现Activity中的另一个回调方法<code>onSaveInstanceState()</code>，保存Activity状态的一些重要信息。系统会向该方法传递一个Bundle，然后我们可以向这个Bundle里面储存一些重要信息。当系统重建Activity时候，系统会将这个Bundle同时传递给<code>onCreate()</code>和<code>onRestoreInstanceState()</code>方法，我们可以在这两个方法中恢复之前场景。</p>
<p>看一下状态保存的介绍图。</p>
<div class="image-package">
<img src="images/354846-d9d333eded62323d.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/354846-d9d333eded62323d.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">图-2 Activity状态保存</div>
</div>
<p>面试时候可能会问到<code>onSaveInstanceState()</code>调用时机，看一下官方源代码的注释。</p>
<blockquote>
<p>Do not confuse this method with activity lifecycle callbacks such as {@link #onPause}, which is always called when an activity is being placed in the background or on its way to destruction, or {@link #onStop} which is called before destruction.  One example of when {@link #onPause} and {@link #onStop} is called and not this method is when a user navigates back from activity B to activity A。</p>
<p>不要把<code>onSaveInstanceState()</code>这个方法和Activity生命周期的几个方法混淆了，这个方法只有在Activity切换到后台或者即将被销毁时候被调用。有一个例子是如果从Activity B返回到Activity A，这个方法是不会被调用的。</p>
</blockquote>
<p>也许很难理解这段注释的意思，我个人理解是，如果一个Activity失去了屏幕焦点后，失去屏幕焦点一般是指<code>onPause()</code>和<code>onStop()</code>方法被调用，<code>onSaveInstanceState()</code>方法就会被调用，有一种特殊情况是从一个Activity B返回到上一个Activity A，这个方法并不会被调用。</p>
<p>个人总结了一下，大体有以下几种情况系统可能会调用<code>onSaveInstanceState()</code></p>
<ul>
<li>按下Home将程序切换到后台</li>
<li>关闭屏幕</li>
<li>Activity A启动一个新的Activity B，会回调A中<code>onSaveInstanceState()</code>方法</li>
<li>屏幕横竖屏方向切换</li>
<li>长按Home或者菜单键进入程序列表页面</li>
</ul>
<blockquote><p>为什么平时并没有实现<code>onSaveInstanceState()</code>和<code>onRestoreInstanceState()</code>方法，但是有些时候，Activity中的UI状态依然得到了保存，是为什么？</p></blockquote>
<p>在Android中，Activity类的<code>onSaveInstanceState()</code>方法默认实现会调用布局中每个View的<code>onSaveInstanceState()</code>方法去保存其本身的状态信息，Android框架中几乎每个控件都会实现这个方法。我们只需要为想要保存其状态的每个控件提供一个唯一的ID（在xml中设置 <code>android:id</code>属性），如果控件没有 ID，则系统无法保存其状态。</p>
<p>我们可以通过将View的<code>android:saveEnabled</code>属性设置为<code>false</code>或通过调用View的<code>setSaveEnabled()</code>方法显式阻止布局内的视图保存其状态，通常不需要设置这些属性，但如果想以不同方式恢复Activity UI的状态，可以这样做。</p>
<p><strong>注：由于无法保证系统调用onSaveInstanceState()的时机，我们只用它来保存Activity的瞬间状态，不要用它来储存持久性数据，上面提到过，建议在onPause()中储存持久性数据。</strong></p>
<h2>6. 启动模式</h2>
<p>敲黑板敲黑板，划重点来了，同学们快拿出笔和纸快做笔记。</p>
<p>一个应用一般包含很多Activity，它们按照各自打开的顺序排列在返回栈（Back Stack）中，这些Activity统称为Task。大多数Task的起点是用户在屏幕中点击应用图标启动应用，该应用的Task出现在前台，如果该应用没有Task，也就是最近未被打开，则会新建一个Task，并且会将该应用的MainActivity加入返回栈中，作为返回栈中的根Activity。</p>
<p>通常情况下，当前一个Activity启动一个新的Activity时候，新的Activity会被加入返回栈中，并处于栈顶，获取屏幕焦点，而前一个Activity仍保留在返回栈中，处于停止(onStop)状态。 Activity停止时，如上所说，系统会保存其页面状态。当用户返回时候，当前处于栈顶的Activity会从返回栈中弹出，并被销毁(onDestroy)，恢复前一个Activity的状态。返回栈中的Activity永远不会重新排列，遵循<strong>先进后出</strong>的原则。</p>
<div class="image-package">
<img src="images/354846-1cb59bea18e38607.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/640" data-original-src="images/354846-1cb59bea18e38607.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">图-3 Activity出入返回栈</div>
</div>
<p>上述讲的只是标准的Activity与返回栈的关系，在Android中Activity有四种启动模式，分别是<code>standard</code>、<code>singleTop</code>、<code>singleTask</code>、<code>singleInstance</code>。</p>
<h3>6.1 设置启动模式</h3>
<p>我们可以通过在AndroidManifest.xml配置Activity的启动模式。</p>
<pre class="hljs xml"><code class="xml"><span class="hljs-tag">&lt;<span class="hljs-name">activity</span>
    <span class="hljs-attr">android:name</span>=<span class="hljs-string">".aidldemo.BindingActivity"</span>
    <span class="hljs-attr">android:launchMode</span>=<span class="hljs-string">"standard"</span>
    <span class="hljs-attr">...</span> /&gt;</span></code></pre>
<p>或者在代码中向Intent添加相应标志。</p>
<pre class="hljs java"><code class="java">Intent intent = <span class="hljs-keyword">new</span> Intent(<span class="hljs-keyword">this</span>, MyActivity.class);  
intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);  
startActivity(intent);</code></pre>
<p><strong>第二种方法设置启动模式的优先级高于第一种，如果两者都存在，以第二种为准。</strong></p>
<h3>6.2 standard（默认模式）</h3>
<p>默认的启动模式，新启动的Activity放入返回栈栈顶，遵循<strong>先进后出</strong>原则，同一个Activity可以被实例化多次。</p>
<h3>6.3 singleTop</h3>
<ul>
<li>如果当前返回栈的顶部不存在该Activity，则新建该Activity并放入栈顶；</li>
<li>如果当前返回栈的顶部已存在Activity的一个实例，则系统会通过调用该实例的onNewIntent()方法向其传送Intent，不创建该Activity的新实例。</li>
</ul>
<h3>6.4 singleTask</h3>
<ul>
<li>如果该Activity需要的返回栈是A，但是当前系统中不存在A返回栈，系统会新建返回栈A，然后再创建该Activity实例将其压入返回栈中。</li>
<li>如果该Activity需要的返回栈存在，而且返回栈中没有该Activity，则新建Activity并放入Task栈顶</li>
<li>如果该Activity需要的返回栈存在，而且返回栈中有该Activity<ul>
<li>如果该Activity在栈顶，调用其<code>onNewIntent()</code>方法传入Intent</li>
<li>如果该Activity不在栈顶，弹出其上面的所有Activity，让该Activity置于栈顶，并调用其<code>onNewIntent()</code>方法传入Intent</li>
</ul>
</li>
</ul>
<p>默认情况下，所有Activity所需要的返回栈名称为应用的包名，我们可以在AndroidManifest.xml中通过设置Activity的android:taskAffinity属性来指定该Activity需要的返回栈名称，这个名称不能和应用包名相同，否则相当于没有指定。<code>taskAffinity</code>翻译过来是返回栈亲和性，我个人理解这个属性是指定与返回栈亲和度或者优先级，并不是每次都会新建返回栈。注意一般android:taskAffinity属性和<code>singleTask</code>一起使用才有意义，会新建返回栈，如果只是指定了android:taskAffinity属性但是依然是<code>singleTop</code>和<code>standard</code>模式，新启动的Activity依然会在原来的返回栈中。</p>
<h3>6.5 singleInstance</h3>
<p>系统创建一个新的Task并创建Activity的新实例置于新Task返回栈中，但是系统不会将任何其他Activity的实例放入这个新建的Task中。该Activity始终是其Task唯一仅有的成员，由此Activity启动的任何Activity，如果没有指定返回栈名称，则新启动的Activity放入默认的返回栈；如果指定了返回栈名称，则将新启动的Activity放入指定的返回栈中。</p>
<h3>6.6 返回栈顺序调用图(个人理解，大家可跳过)</h3>
<p>Android中返回栈分为前台返回栈和后台返回栈，前台返回栈是指返回栈栈顶的Activity正在和用户进行交互。<br>上面说了几种启动模式，下面看一下几种启动模式混合时候返回栈调度情况，我个人的理解和官方有些不同，这个大家可以跳过，去看官方的介绍。</p>
<p>个人理解一个应用创建的默认返回栈为基准，按返回键时候，根据返回栈创建顺序依次清空返回栈，当默认返回栈清空时候，应用也就关闭了，但是有些后台返回栈中的Activity并不会立即销毁。</p>
<p>Activity A和Activity B为默认启动模式，未设置taskAffinity属性。<br>Activity C启动模式是singleTask，设置了taskAffinity属性。<br>启动顺序是<code>Activity A -&gt; Activity B -&gt; Activity C</code><br>看一下返回栈调用情况</p>
<div class="image-package">
<img src="images/354846-301ce03b66d8d7b0.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/640" data-original-src="images/354846-301ce03b66d8d7b0.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">图-4 返回栈图1</div>
</div>
<p>Activity A和Activity B为默认启动模式，A未设置taskAffinity属性，B设置taskAffinity属性为默认返回栈。<br>Activity C和Activity D启动模式是singleTask，设置了相同的taskAffinity属性。<br>启动顺序是<code>Activity A -&gt; Activity C -&gt; Activity D -&gt; Activity B</code><br>返回栈调用情况如下图</p>
<div class="image-package">
<img src="images/354846-58bcf9c955865978.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/640" data-original-src="images/354846-58bcf9c955865978.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">图-5 返回栈图2</div>
</div>
<p>官方图，这里注意一下，官方图中在<code>StartActivity Y</code>后，Y与X所在返回栈和1与2所在的返回栈是不同的，他们并不在同一个返回栈：<br></p><div class="image-package">
<img src="images/354846-2d34fdcecd4a31bc.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240" data-original-src="images/354846-2d34fdcecd4a31bc.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">图-6 返回栈图3</div>
</div>
<p>Activity A和Activity C为默认启动模式，未设置taskAffinity属性<br>Activity B启动模式是singleInstance<br>启动顺序是<code>Activity A -&gt; Activity B -&gt; Activity C</code><br>返回栈调用情况如下图</p>
<div class="image-package">
<img src="images/354846-cafce21d989c846d.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/640" data-original-src="images/354846-cafce21d989c846d.png?imageMogr2/auto-orient/strip%7CimageView2/2" style="cursor: zoom-in;"><br><div class="image-caption">图-7 返回栈图4</div>
</div>
<p><strong>按返回键和启动Activity从返回栈A到返回栈B结果是不同的，按返回键时候，会首先弹出返回栈A中的Activity，等到返回栈没有Activity时候，才会进入另一个返回栈，这个时候返回栈A已经没有Activity了。</strong></p>
<h3>6.7 XML添加属性和Intent添加标签设置启动模式对比</h3>
<p>上面讲到的四种启动模式都是在<code>Androidmanifest.xml</code>中设置启动模式，也提及了用Intent添加flags来设置启动模式。下面针对两种方法做一下对比。</p>
<ul>
<li>
<code>FLAG_ACTIVITY_SINGLE_TOP</code>，这个标记的作用是为Activity指定<code>singleTop</code>启动模式，与在XML设置启动模式相同</li>
<li>
<code>FLAG_ACTIVITY_NEW_TASK</code>，这个标记只是在设置了taskAffinity属性下有意义，如果被标记的Activity需要de 返回栈不存在，则新建返回栈，然后新建Activity；如果被标记Activity需要的返回栈存在，则将返回栈带回前台，并不会创建新的Activity或者调用onNewIntent。</li>
<li>
<code>FLAG_ACTIVITY_CLEAR_TOP</code>，用这个标记启动的Activity，当它启动是，在同一个任务战中所有位于它上面的Activity都要出栈。这个标记一般会和<code>singleTask</code>启动模式一起出现，在这种情况下，被启动的Activity的实力如果已经存在，那么系统就会调用它的onNewIntent。如果被启动的Activity采用<code>standard</code>启动模式，那么连同<strong>它和它之上</strong>的Activity都要出栈，系统会创建新的Activity实力并放入栈顶。<code>singleTask</code>启动模式默认就具有此标记的效果。</li>
<li>
<code>FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS</code>，用这个标记启动的Activity不会出现在近期任务中，也就是在Android任务列表中隐藏了该Activity，和在XML设置<code>android:excludeFromRecents="true"</code>相同，这个属性需要配合<code>singleInstance</code>启动模式才有用</li>
</ul>
<p>可以看到XML设置没有类似<code>FLAG_ACTIVITY_CLEAR_TOP</code>标记这种效果的启动模式，而标记中没有<code>singleInstance</code>这种启动模式的标记。</p>
<p><strong>平时如果我们使用ApplicationContext.startActivity去启动一个<code>standard</code>启动模式的Activity时候，会报错如下，这是因为<code>standard</code>模式的Activity会默认进入启动它的Activity的返回栈中，但是由于非Activity类型的Context(如ApplicationContext)并没有所谓的返回栈，所以抛出这个异常。解决这个问题的方法就是在Intent中添加<code>FLAG_ACTIVITY_NEW_TASK</code>的标记，这个时候启动其实是以<code>singleTask</code>模式启动的</strong></p>
<pre class="hljs livecodeserver"><code class="livecodeserver">ERROR/AndroidRuntime(<span class="hljs-number">5066</span>): 
Caused <span class="hljs-keyword">by</span>: 
android.util.AndroidRuntimeException: 
Calling startActivity() <span class="hljs-built_in">from</span> outside <span class="hljs-keyword">of</span> <span class="hljs-keyword">an</span> Activity context requires <span class="hljs-keyword">the</span> FLAG_ACTIVITY_NEW_TASK flag. Is this really what you want?</code></pre>
<h3>6.8 taskAffinity和allowTaskReparenting结合</h3>
<p>在AndroidManifest.xml中可以为Activity同时设置这两个属性，taskAffinity这个属性上面将结果，allowTaskReparenting这个属性是标记Activity是否可以更换返回栈，也就是从一个返回栈转移到另一个返回栈。当应用需要给其他应用提供页面支持的时候，这两者结合起来就很有意义。</p>
<p>B应用提供了一个对外的Activity C，taskAffinity属性是应用B包名，allowTaskReparenting设置为true。</p>
<p>应用A调用了应用B的Activity C，然后按Home键退回到主屏幕，单击应用B的桌面图标。</p>
<ul>
<li>如果应用B已经启动，Activity C会出现在应用B返回栈的栈顶，应用B显示Activity C，应用A中Activity C消失，也就将是Activity C从应用A的返回栈转移到应用B的返回栈</li>
<li>如果应用B未启动，这个时候并不是启动应用B的MainActivity，而是重新显示了已经被应用A启动的Activity C，或者说Activity C从应用A的返回栈转移到了应用B的返回栈中。可以理解成，由于应用A启动Activity C，当应用B启动新建返回栈时候，系统发现Activity C原本所想要的返回栈创建完毕，就把Activity C从应用A的返回栈转移到应用B的返回栈。</li>
</ul>
<h2>7. 清空返回栈</h2>
<p>如果用户将应用长时间的切换到后台，系统会清除返回栈中除了根Activity的所有Activity。当用户再次回到应用时候，仅恢复根Activity。有几个标签能够协助我们控制返回栈的清空。</p>
<ul>
<li>
<code>alwaysRetainTaskState</code>，如果根Activity的该属性设置为True，系统会长时间的保存所有的Activity在返回栈中，并不会清空。（杀死应用除外）</li>
<li>
<code>clearTaskOnLaunch</code>，如果根Activity的该属性设置为True，每当用户离开再返回时候，系统都会将返回栈清空只留下根Activity。这个属性和<code>alwaysRetainTaskState</code>刚好相反，即时用户只离开片刻，系统也会清空返回栈。</li>
</ul>
<h2>8.结束语</h2>
<p>从开始学Android开始，就想对四大组件进行一些梳理，将自己的知识点细化并记录下来，可能网上已经有很多关于Activity的文章，没用的很多，还是自己来写靠谱，温故而知新，共勉。</p>
<p>注：关于Activity启动匹配比较复杂，可以去查看我的另一篇文章<a href="http://www.jianshu.com/p/a7535c3f33df" target="_blank">浅谈Intent以及IntentFilter</a>。</p>

        </div>