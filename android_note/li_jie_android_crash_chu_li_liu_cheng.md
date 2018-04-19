# 理解Android Crash处理流程


<div class="
                col-lg-8 col-lg-offset-2
                col-md-10 col-md-offset-1
                post-container">

								<blockquote>
  <p>基于Android 6.0的源码剖析， 分析Android应用Crash是如何处理的。</p>
</blockquote>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cs">/frameworks/<span class="hljs-keyword">base</span>/core/java/com/android/<span class="hljs-keyword">internal</span>/os/RuntimeInit.java
/frameworks/<span class="hljs-keyword">base</span>/core/java/android/app/ActivityManagerNative.java (含内部类AMP)
/frameworks/<span class="hljs-keyword">base</span>/core/java/android/app/ApplicationErrorReport.java

/frameworks/<span class="hljs-keyword">base</span>/services/core/java/com/android/server/
    - am/ActivityManagerService.java
    - am/ProcessRecord.java
    - am/ActivityRecord.java
    - am/ActivityStackSupervisor.java
    - am/ActivityStack.java
    - am/ActivityRecord.java
    - am/BroadcastQueue.java
    - wm/WindowManagerService.java

/libcore/libart/src/main/java/java/lang/Thread.java
</code></pre></div></div>

<h2 id="一概述">一、概述<a class="anchorjs-link " href="#一概述" aria-label="Anchor link for: 一概述" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h2>

<p>App crash(全称Application crash), 对于Crash可分为native crash和framework crash(包含app crash在内)，对于crash相信很多app开发者都会遇到，那么上层什么时候会出现crash呢，系统又是如何处理crash的呢。例如，在app大家经常使用<code class="highlighter-rouge">try...catch</code>语句，那么如果没有有效catch exception，就是导致应用crash，发生没有catch exception，系统便会来进行捕获，并进入crash流程。如果你是从事Android系统开发或者架构相关工作，或者遇到需要解系统性的疑难杂症，那么很有必要了解系统Crash处理流程，知其然还需知其所以然；如果你仅仅是App初级开发，可能本文并非很适合阅读，整个系统流程错综复杂。</p>

<p>在Android系统启动系列文章，已讲述过上层应用都是由Zygote fork孵化而来，分为system_server系统进程和各种应用进程，在这些进程创建之初会设置未捕获异常的处理器，当系统抛出未捕获的异常时，最终都交给异常处理器。</p>

<ul>
  <li>对于system_server进程：文章<a href="http://gityuan.com/2016/02/14/android-system-server/#commonInit">Android系统启动-SystemServer上篇</a>，system_server启动过程中由RuntimeInit.java的<code class="highlighter-rouge">commonInit</code>方法设置UncaughtHandler，用于处理未捕获异常；</li>
  <li>对于普通应用进程：文章<a href="http://gityuan.com/2016/03/26/app-process-create/#commoninit">理解Android进程创建流程</a> ，进程创建过程中，同样会调用RuntimeInit.java的<code class="highlighter-rouge">commonInit</code>方法设置UncaughtHandler。</li>
</ul>

<h3 id="11-crash调用链">1.1 crash调用链<a class="anchorjs-link " href="#11-crash调用链" aria-label="Anchor link for: 11 crash调用链" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>crash流程的方法调用关系来结尾：</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs css"><span class="hljs-selector-tag">AMP</span><span class="hljs-selector-class">.handleApplicationCrash</span>
    <span class="hljs-selector-tag">AMS</span><span class="hljs-selector-class">.handleApplicationCrash</span>
        <span class="hljs-selector-tag">AMS</span><span class="hljs-selector-class">.findAppProcess</span>
        <span class="hljs-selector-tag">AMS</span><span class="hljs-selector-class">.handleApplicationCrashInner</span>
            <span class="hljs-selector-tag">AMS</span><span class="hljs-selector-class">.addErrorToDropBox</span>
            <span class="hljs-selector-tag">AMS</span><span class="hljs-selector-class">.crashApplication</span>
                <span class="hljs-selector-tag">AMS</span><span class="hljs-selector-class">.makeAppCrashingLocked</span>
                    <span class="hljs-selector-tag">AMS</span><span class="hljs-selector-class">.startAppProblemLocked</span>
                    <span class="hljs-selector-tag">ProcessRecord</span><span class="hljs-selector-class">.stopFreezingAllLocked</span>
                        <span class="hljs-selector-tag">ActivityRecord</span><span class="hljs-selector-class">.stopFreezingScreenLocked</span>
                            <span class="hljs-selector-tag">WMS</span><span class="hljs-selector-class">.stopFreezingScreenLocked</span>
                                <span class="hljs-selector-tag">WMS</span><span class="hljs-selector-class">.stopFreezingDisplayLocked</span>
                    <span class="hljs-selector-tag">AMS</span><span class="hljs-selector-class">.handleAppCrashLocked</span>
                <span class="hljs-selector-tag">mUiHandler</span><span class="hljs-selector-class">.sendMessage</span>(<span class="hljs-selector-tag">SHOW_ERROR_MSG</span>)

<span class="hljs-selector-tag">Process</span><span class="hljs-selector-class">.killProcess</span>(<span class="hljs-selector-tag">Process</span><span class="hljs-selector-class">.myPid</span>());
<span class="hljs-selector-tag">System</span><span class="hljs-selector-class">.exit</span>(10);
</code></pre></div></div>

<p>接下来说说这个过程。</p>

<h2 id="二crash处理流程">二、Crash处理流程<a class="anchorjs-link " href="#二crash处理流程" aria-label="Anchor link for: 二crash处理流程" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h2>

<p>那么接下来以commonInit()方法为起点来展开说明。</p>

<h3 id="1-runtimeinitcommoninit">1. RuntimeInit.commonInit<a class="anchorjs-link " href="#1-runtimeinitcommoninit" aria-label="Anchor link for: 1 runtimeinitcommoninit" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-keyword">public</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">RuntimeInit</span> </span>{
    ...
    <span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">static</span> <span class="hljs-keyword">final</span> <span class="hljs-keyword">void</span> <span class="hljs-title">commonInit</span><span class="hljs-params">()</span> </span>{
        <span class="hljs-comment">//设置默认的未捕获异常处理器，UncaughtHandler实例化过程【见小节2】</span>
        Thread.setDefaultUncaughtExceptionHandler(<span class="hljs-keyword">new</span> UncaughtHandler());
        ...
    }
}
</code></pre></div></div>

<p>setDefaultUncaughtExceptionHandler()只是将异常处理器handler对象赋给Thread成员变量,即<code class="highlighter-rouge">Thread.defaultUncaughtHandler = new UncaughtHandler()</code>。接下来看看UncaughtHandler对象实例化过程。</p>

<h3 id="2-uncaughthandler">2. UncaughtHandler<a class="anchorjs-link " href="#2-uncaughthandler" aria-label="Anchor link for: 2 uncaughthandler" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[–&gt;RuntimeInit.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-keyword">private</span> <span class="hljs-keyword">static</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">UncaughtHandler</span> <span class="hljs-keyword">implements</span> <span class="hljs-title">Thread</span>.<span class="hljs-title">UncaughtExceptionHandler</span> </span>{
    <span class="hljs-comment">//覆写接口方法</span>
    <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">uncaughtException</span><span class="hljs-params">(Thread t, Throwable e)</span> </span>{
        <span class="hljs-keyword">try</span> {
            <span class="hljs-comment">//保证crash处理过程不会重入</span>
            <span class="hljs-keyword">if</span> (mCrashing) <span class="hljs-keyword">return</span>;
            mCrashing = <span class="hljs-keyword">true</span>;

            <span class="hljs-keyword">if</span> (mApplicationObject == <span class="hljs-keyword">null</span>) {
                <span class="hljs-comment">//system_server进程</span>
                Clog_e(TAG, <span class="hljs-string">"*** FATAL EXCEPTION IN SYSTEM PROCESS: "</span> + t.getName(), e);
            } <span class="hljs-keyword">else</span> {
                <span class="hljs-comment">//普通应用进程</span>
                StringBuilder message = <span class="hljs-keyword">new</span> StringBuilder();
                message.append(<span class="hljs-string">"FATAL EXCEPTION: "</span>).append(t.getName()).append(<span class="hljs-string">"\n"</span>);
                <span class="hljs-keyword">final</span> String processName = ActivityThread.currentProcessName();
                <span class="hljs-keyword">if</span> (processName != <span class="hljs-keyword">null</span>) {
                    message.append(<span class="hljs-string">"Process: "</span>).append(processName).append(<span class="hljs-string">", "</span>);
                }
                message.append(<span class="hljs-string">"PID: "</span>).append(Process.myPid());
                Clog_e(TAG, message.toString(), e);
            }

            <span class="hljs-comment">//启动crash对话框，等待处理完成 【见小节2.1和3】</span>
            ActivityManagerNative.getDefault().handleApplicationCrash(
                    mApplicationObject, <span class="hljs-keyword">new</span> ApplicationErrorReport.CrashInfo(e));
        } <span class="hljs-keyword">catch</span> (Throwable t2) {
            ...
        } <span class="hljs-keyword">finally</span> {
            <span class="hljs-comment">//确保当前进程彻底杀掉【见小节11】</span>
            Process.killProcess(Process.myPid());
            System.exit(<span class="hljs-number">10</span>);
        }
    }
}
</code></pre></div></div>

<ol>
  <li>当system进程crash的信息：
    <ul>
      <li>开头<code class="highlighter-rouge">*** FATAL EXCEPTION IN SYSTEM PROCESS [线程名]</code>；</li>
      <li>接着输出发生crash时的调用栈信息；</li>
    </ul>
  </li>
  <li>当app进程crash时的信息：
    <ul>
      <li>开头<code class="highlighter-rouge">FATAL EXCEPTION: [线程名]</code>；</li>
      <li>紧接着 <code class="highlighter-rouge">Process: [进程名], PID: [进程id]</code>；</li>
      <li>最后输出发生crash时的调用栈信息。</li>
    </ul>
  </li>
</ol>

<p>看到这里，你就会发现要从log中搜索crash信息，只需要搜索关键词<code class="highlighter-rouge">FATAL EXCEPTION</code>；如果需要进一步筛选只搜索系统crash信息，则可以搜索的关键词可以有多样，比如<code class="highlighter-rouge">*** FATAL EXCEPTION</code>。</p>

<p>当输出完crash信息到logcat里面，这只是crash流程的刚开始阶段，接下来弹出<code class="highlighter-rouge">crash对话框</code>，ActivityManagerNative.getDefault()返回的是ActivityManagerProxy（简称<code class="highlighter-rouge">AMP</code>），<code class="highlighter-rouge">AMP</code>经过binder调用最终交给ActivityManagerService(简称<code class="highlighter-rouge">AMS</code>)中相应的方法去处理，故接下来调用的是AMS.handleApplicationCrash()。</p>

<p><strong>注意</strong>: mApplicationObject等于null,一定不是普通的app进程. 但是除了system进程, 也有可能是shell进程, 即通过app_process + 命令参数 的方式创建的进程.</p>

<h4 id="21-crashinfo">2.1 CrashInfo<a class="anchorjs-link " href="#21-crashinfo" aria-label="Anchor link for: 21 crashinfo" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<p>[-&gt; ApplicationErrorReport.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-keyword">public</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">ApplicationErrorReport</span> <span class="hljs-keyword">implements</span> <span class="hljs-title">Parcelable</span> </span>{
    ...
    <span class="hljs-keyword">public</span> <span class="hljs-keyword">static</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">CrashInfo</span> </span>{
        <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-title">CrashInfo</span><span class="hljs-params">(Throwable tr)</span> </span>{
            StringWriter sw = <span class="hljs-keyword">new</span> StringWriter();
            PrintWriter pw = <span class="hljs-keyword">new</span> FastPrintWriter(sw, <span class="hljs-keyword">false</span>, <span class="hljs-number">256</span>);
            tr.printStackTrace(pw); <span class="hljs-comment">//输出栈trace</span>
            pw.flush();
            stackTrace = sw.toString();
            exceptionMessage = tr.getMessage();

            Throwable rootTr = tr;
            <span class="hljs-keyword">while</span> (tr.getCause() != <span class="hljs-keyword">null</span>) {
                tr = tr.getCause();
                <span class="hljs-keyword">if</span> (tr.getStackTrace() != <span class="hljs-keyword">null</span> &amp;&amp; tr.getStackTrace().length &gt; <span class="hljs-number">0</span>) {
                    rootTr = tr;
                }
                String msg = tr.getMessage();
                <span class="hljs-keyword">if</span> (msg != <span class="hljs-keyword">null</span> &amp;&amp; msg.length() &gt; <span class="hljs-number">0</span>) {
                    exceptionMessage = msg;
                }
            }

            exceptionClassName = rootTr.getClass().getName();
            <span class="hljs-keyword">if</span> (rootTr.getStackTrace().length &gt; <span class="hljs-number">0</span>) {
                StackTraceElement trace = rootTr.getStackTrace()[<span class="hljs-number">0</span>];
                throwFileName = trace.getFileName();
                throwClassName = trace.getClassName();
                throwMethodName = trace.getMethodName();
                throwLineNumber = trace.getLineNumber();
            } <span class="hljs-keyword">else</span> {
                throwFileName = <span class="hljs-string">"unknown"</span>;
                throwClassName = <span class="hljs-string">"unknown"</span>;
                throwMethodName = <span class="hljs-string">"unknown"</span>;
                throwLineNumber = <span class="hljs-number">0</span>;
            }
        }
        ...
    }
}
</code></pre></div></div>

<p>将crash信息<code class="highlighter-rouge">文件名</code>，<code class="highlighter-rouge">类名</code>，<code class="highlighter-rouge">方法名</code>，<code class="highlighter-rouge">对应行号</code>以及<code class="highlighter-rouge">异常信息</code>都封装到CrashInfo对象。</p>

<h3 id="3-handleapplicationcrash">3. handleApplicationCrash<a class="anchorjs-link " href="#3-handleapplicationcrash" aria-label="Anchor link for: 3 handleapplicationcrash" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[–&gt;ActivityManagerService.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">handleApplicationCrash</span><span class="hljs-params">(IBinder app, ApplicationErrorReport.CrashInfo crashInfo)</span> </span>{
    <span class="hljs-comment">//获取进程record对象【见小节3.1】</span>
    ProcessRecord r = findAppProcess(app, <span class="hljs-string">"Crash"</span>);
    <span class="hljs-keyword">final</span> String processName = app == <span class="hljs-keyword">null</span> ? <span class="hljs-string">"system_server"</span>
            : (r == <span class="hljs-keyword">null</span> ? <span class="hljs-string">"unknown"</span> : r.processName);
    <span class="hljs-comment">//【见小节4】</span>
    handleApplicationCrashInner(<span class="hljs-string">"crash"</span>, r, processName, crashInfo);
}
</code></pre></div></div>

<p>关于进程名(processName)：</p>

<ol>
  <li>当远程IBinder对象为空时，则进程名为<code class="highlighter-rouge">system_server</code>；</li>
  <li>当远程IBinder对象不为空，且ProcessRecord为空时，则进程名为<code class="highlighter-rouge">unknown</code>;</li>
  <li>当远程IBinder对象不为空，且ProcessRecord不为空时，则进程名为ProcessRecord对象中相应进程名。</li>
</ol>

<h4 id="31-findappprocess">3.1 findAppProcess<a class="anchorjs-link " href="#31-findappprocess" aria-label="Anchor link for: 31 findappprocess" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<p>[–&gt;ActivityManagerService.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">private</span> ProcessRecord <span class="hljs-title">findAppProcess</span><span class="hljs-params">(IBinder app, String reason)</span> </span>{
    <span class="hljs-keyword">if</span> (app == <span class="hljs-keyword">null</span>) {
        <span class="hljs-keyword">return</span> <span class="hljs-keyword">null</span>;
    }

    <span class="hljs-keyword">synchronized</span> (<span class="hljs-keyword">this</span>) {
        <span class="hljs-keyword">final</span> <span class="hljs-keyword">int</span> NP = mProcessNames.getMap().size();
        <span class="hljs-keyword">for</span> (<span class="hljs-keyword">int</span> ip=<span class="hljs-number">0</span>; ip&lt;NP; ip++) {
            SparseArray&lt;ProcessRecord&gt; apps = mProcessNames.getMap().valueAt(ip);
            <span class="hljs-keyword">final</span> <span class="hljs-keyword">int</span> NA = apps.size();
            <span class="hljs-keyword">for</span> (<span class="hljs-keyword">int</span> ia=<span class="hljs-number">0</span>; ia&lt;NA; ia++) {
                ProcessRecord p = apps.valueAt(ia);
                <span class="hljs-comment">//当找到目标进程则返回</span>
                <span class="hljs-keyword">if</span> (p.thread != <span class="hljs-keyword">null</span> &amp;&amp; p.thread.asBinder() == app) {
                    <span class="hljs-keyword">return</span> p;
                }
            }
        }
        <span class="hljs-comment">//如果代码执行到这里，表明无法找到应用所在的进程</span>
        <span class="hljs-keyword">return</span> <span class="hljs-keyword">null</span>;
    }
}
</code></pre></div></div>

<p>其中 <code class="highlighter-rouge">mProcessNames = new ProcessMap&lt;ProcessRecord&gt;();</code>对于代码<code class="highlighter-rouge">mProcessNames.getMap()</code>返回的是<code class="highlighter-rouge">mMap</code>，而<code class="highlighter-rouge">mMap= new ArrayMap&lt;String, SparseArray&lt;ProcessRecord&gt;&gt;()</code>;</p>

<p><strong>知识延伸：</strong><code class="highlighter-rouge">SparseArray</code>和<code class="highlighter-rouge">ArrayMap</code>是Android专门针对内存优化而设计的取代Java API中的<code class="highlighter-rouge">HashMap</code>的数据结构。对于key是int类型则使用<code class="highlighter-rouge">SparseArray</code>，可避免自动装箱过程；对于key为其他类型则使用<code class="highlighter-rouge">ArrayMap</code>。<code class="highlighter-rouge">HashMap</code>的查找和插入时间复杂度为O(1)的代价是牺牲大量的内存来实现的，而<code class="highlighter-rouge">SparseArray</code>和<code class="highlighter-rouge">ArrayMap</code>性能略逊于<code class="highlighter-rouge">HashMap</code>，但更节省内存。</p>

<p>再回到<code class="highlighter-rouge">mMap</code>，这是以进程name为key，再以(uid为key，以ProcessRecord为Value的)结构体作为value。下面看看其get()和put()方法</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cs"><span class="hljs-comment">//获取mMap中(name,uid)所对应的ProcessRecord</span>
<span class="hljs-function"><span class="hljs-keyword">public</span> ProcessRecord <span class="hljs-title">get</span>(<span class="hljs-params">String name, <span class="hljs-keyword">int</span> uid</span>) </span>{}；
<span class="hljs-comment">//将(name,uid, value)添加到mMap</span>
<span class="hljs-function"><span class="hljs-keyword">public</span> ProcessRecord <span class="hljs-title">put</span>(<span class="hljs-params">String name, <span class="hljs-keyword">int</span> uid, ProcessRecord <span class="hljs-keyword">value</span></span>) </span>{}；
</code></pre></div></div>

<p>findAppProcess()根据app(IBinder类型)来查询相应的目标对象ProcessRecord。</p>

<p>有了进程记录对象ProcessRecord和进程名processName，则进入执行Crash处理方法，继续往下看。</p>

<h3 id="4-handleapplicationcrashinner">4. handleApplicationCrashInner<a class="anchorjs-link " href="#4-handleapplicationcrashinner" aria-label="Anchor link for: 4 handleapplicationcrashinner" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[–&gt;ActivityManagerService.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs javascript"><span class="hljs-keyword">void</span> handleApplicationCrashInner(<span class="hljs-built_in">String</span> eventType, ProcessRecord r, <span class="hljs-built_in">String</span> processName,
        ApplicationErrorReport.CrashInfo crashInfo) {
    <span class="hljs-comment">//将Crash信息写入到Event log</span>
    EventLog.writeEvent(EventLogTags.AM_CRASH,...);
    <span class="hljs-comment">//将错误信息添加到DropBox</span>
    addErrorToDropBox(eventType, r, processName, <span class="hljs-literal">null</span>, <span class="hljs-literal">null</span>, <span class="hljs-literal">null</span>, <span class="hljs-literal">null</span>, <span class="hljs-literal">null</span>, crashInfo);
    <span class="hljs-comment">//【见小节5】</span>
    crashApplication(r, crashInfo);
}
</code></pre></div></div>

<p>其中<a href="http://gityuan.com/2016/06/12/DropBoxManagerService/#amsadderrortodropbox">addErrorToDropBox</a>是将crash的信息输出到目录<code class="highlighter-rouge">/data/system/dropbox</code>。例如system_server的dropbox文件名为system_server_crash@xxx.txt (xxx代表的是时间戳)</p>

<h3 id="5-crashapplication">5. crashApplication<a class="anchorjs-link " href="#5-crashapplication" aria-label="Anchor link for: 5 crashapplication" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[–&gt;ActivityManagerService.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">void</span> <span class="hljs-title">crashApplication</span><span class="hljs-params">(ProcessRecord r, ApplicationErrorReport.CrashInfo crashInfo)</span> </span>{
    <span class="hljs-keyword">long</span> timeMillis = System.currentTimeMillis();
    String shortMsg = crashInfo.exceptionClassName;
    String longMsg = crashInfo.exceptionMessage;
    String stackTrace = crashInfo.stackTrace;
    <span class="hljs-keyword">if</span> (shortMsg != <span class="hljs-keyword">null</span> &amp;&amp; longMsg != <span class="hljs-keyword">null</span>) {
        longMsg = shortMsg + <span class="hljs-string">": "</span> + longMsg;
    } <span class="hljs-keyword">else</span> <span class="hljs-keyword">if</span> (shortMsg != <span class="hljs-keyword">null</span>) {
        longMsg = shortMsg;
    }

    AppErrorResult result = <span class="hljs-keyword">new</span> AppErrorResult();
    <span class="hljs-keyword">synchronized</span> (<span class="hljs-keyword">this</span>) {
        <span class="hljs-comment">// 当存在ActivityController的情况,比如monkey</span>
        <span class="hljs-keyword">if</span> (mController != <span class="hljs-keyword">null</span>) {
            <span class="hljs-keyword">try</span> {
                String name = r != <span class="hljs-keyword">null</span> ? r.processName : <span class="hljs-keyword">null</span>;
                <span class="hljs-keyword">int</span> pid = r != <span class="hljs-keyword">null</span> ? r.pid : Binder.getCallingPid();
                <span class="hljs-keyword">int</span> uid = r != <span class="hljs-keyword">null</span> ? r.info.uid : Binder.getCallingUid();
                <span class="hljs-comment">//调用monkey的appCrashed</span>
                <span class="hljs-keyword">if</span> (!mController.appCrashed(name, pid,
                        shortMsg, longMsg, timeMillis, crashInfo.stackTrace)) {
                    <span class="hljs-keyword">if</span> (<span class="hljs-string">"1"</span>.equals(SystemProperties.get(SYSTEM_DEBUGGABLE, <span class="hljs-string">"0"</span>))
                            &amp;&amp; <span class="hljs-string">"Native crash"</span>.equals(crashInfo.exceptionClassName)) {
                        Slog.w(TAG, <span class="hljs-string">"Skip killing native crashed app "</span> + name
                                + <span class="hljs-string">"("</span> + pid + <span class="hljs-string">") during testing"</span>);
                    } <span class="hljs-keyword">else</span> {
                        Slog.w(TAG, <span class="hljs-string">"Force-killing crashed app "</span> + name
                                + <span class="hljs-string">" at watcher's request"</span>);
                        <span class="hljs-keyword">if</span> (r != <span class="hljs-keyword">null</span>) {
                            r.kill(<span class="hljs-string">"crash"</span>, <span class="hljs-keyword">true</span>);
                        } <span class="hljs-keyword">else</span> {
                            Process.killProcess(pid);
                            killProcessGroup(uid, pid);
                        }
                    }
                    <span class="hljs-keyword">return</span>;
                }
            } <span class="hljs-keyword">catch</span> (RemoteException e) {
                mController = <span class="hljs-keyword">null</span>;
                Watchdog.getInstance().setActivityController(<span class="hljs-keyword">null</span>);
            }
        }
        
        <span class="hljs-comment">//清除远程调用者uid和pid信息，并保存到origId</span>
        <span class="hljs-keyword">final</span> <span class="hljs-keyword">long</span> origId = Binder.clearCallingIdentity();
        ...

        <span class="hljs-comment">//【见小节6】</span>
        <span class="hljs-keyword">if</span> (r == <span class="hljs-keyword">null</span> || !makeAppCrashingLocked(r, shortMsg, longMsg, stackTrace)) {
            Binder.restoreCallingIdentity(origId);
            <span class="hljs-keyword">return</span>;
        }

        Message msg = Message.obtain();
        msg.what = SHOW_ERROR_MSG;
        HashMap data = <span class="hljs-keyword">new</span> HashMap();
        data.put(<span class="hljs-string">"result"</span>, result);
        data.put(<span class="hljs-string">"app"</span>, r);
        msg.obj = data;
        <span class="hljs-comment">//发送消息SHOW_ERROR_MSG，弹出提示crash的对话框，等待用户选择【见小节10】</span>
        mUiHandler.sendMessage(msg);
        <span class="hljs-comment">//恢复远程调用者uid和pid</span>
        Binder.restoreCallingIdentity(origId);
    }

    <span class="hljs-comment">//进入阻塞等待，直到用户选择crash对话框"退出"或者"退出并报告"</span>
    <span class="hljs-keyword">int</span> res = result.get();

    Intent appErrorIntent = <span class="hljs-keyword">null</span>;
    <span class="hljs-keyword">synchronized</span> (<span class="hljs-keyword">this</span>) {
        <span class="hljs-keyword">if</span> (r != <span class="hljs-keyword">null</span> &amp;&amp; !r.isolated) {
            <span class="hljs-comment">// 将崩溃的进程信息保存到mProcessCrashTimes</span>
            mProcessCrashTimes.put(r.info.processName, r.uid,
                    SystemClock.uptimeMillis());
        }
        <span class="hljs-keyword">if</span> (res == AppErrorDialog.FORCE_QUIT_AND_REPORT) {
            <span class="hljs-comment">//创建action="android.intent.action.APP_ERROR"，组件为r.errorReportReceiver的Intent</span>
            appErrorIntent = createAppErrorIntentLocked(r, timeMillis, crashInfo);
        }
    }

    <span class="hljs-keyword">if</span> (appErrorIntent != <span class="hljs-keyword">null</span>) {
        <span class="hljs-keyword">try</span> {
            <span class="hljs-comment">//启动Intent为appErrorIntent的Activity</span>
            mContext.startActivityAsUser(appErrorIntent, <span class="hljs-keyword">new</span> UserHandle(r.userId));
        } <span class="hljs-keyword">catch</span> (ActivityNotFoundException e) {
            Slog.w(TAG, <span class="hljs-string">"bug report receiver dissappeared"</span>, e);
        }
    }
}
</code></pre></div></div>

<p>该方法主要做的两件事：</p>

<ul>
  <li>调用<code class="highlighter-rouge">makeAppCrashingLocked</code>，继续处理crash流程；</li>
  <li>发送消息SHOW_ERROR_MSG，弹出提示crash的对话框，等待用户选择；</li>
</ul>

<h3 id="6-makeappcrashinglocked">6. makeAppCrashingLocked<a class="anchorjs-link " href="#6-makeappcrashinglocked" aria-label="Anchor link for: 6 makeappcrashinglocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[–&gt;ActivityManagerService.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">boolean</span> <span class="hljs-title">makeAppCrashingLocked</span><span class="hljs-params">(ProcessRecord app,
        String shortMsg, String longMsg, String stackTrace)</span> </span>{
    app.crashing = <span class="hljs-keyword">true</span>;
    <span class="hljs-comment">//封装crash信息到crashingReport对象</span>
    app.crashingReport = generateProcessError(app,
            ActivityManager.ProcessErrorStateInfo.CRASHED, <span class="hljs-keyword">null</span>, shortMsg, longMsg, stackTrace);
    <span class="hljs-comment">//【见小节7】</span>
    startAppProblemLocked(app);
    <span class="hljs-comment">//停止屏幕冻结【见小节8】</span>
    app.stopFreezingAllLocked();
    <span class="hljs-comment">//【见小节9】</span>
    <span class="hljs-keyword">return</span> handleAppCrashLocked(app, <span class="hljs-string">"force-crash"</span>, shortMsg, longMsg, stackTrace);
}
</code></pre></div></div>

<h3 id="7-startappproblemlocked">7. startAppProblemLocked<a class="anchorjs-link " href="#7-startappproblemlocked" aria-label="Anchor link for: 7 startappproblemlocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[–&gt;ActivityManagerService.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">startAppProblemLocked</span><span class="hljs-params">(ProcessRecord app)</span> </span>{
    app.errorReportReceiver = <span class="hljs-keyword">null</span>;

    <span class="hljs-keyword">for</span> (<span class="hljs-keyword">int</span> userId : mCurrentProfileIds) {
        <span class="hljs-keyword">if</span> (app.userId == userId) {
            <span class="hljs-comment">//获取当前用户下的crash应用的error receiver【见小节7.1】</span>
            app.errorReportReceiver = ApplicationErrorReport.getErrorReportReceiver(
                    mContext, app.info.packageName, app.info.flags);
        }
    }
    <span class="hljs-comment">//忽略当前app的广播接收【见小节7.2】</span>
    skipCurrentReceiverLocked(app);
}
</code></pre></div></div>

<p>该方法主要功能：</p>

<ul>
  <li>获取当前用户下的crash应用的error receiver；</li>
  <li>忽略当前app的广播接收；</li>
</ul>

<h4 id="71-geterrorreportreceiver">7.1 getErrorReportReceiver<a class="anchorjs-link " href="#71-geterrorreportreceiver" aria-label="Anchor link for: 71 geterrorreportreceiver" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<p>[-&gt; ApplicationErrorReport.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cs"><span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">static</span> ComponentName <span class="hljs-title">getErrorReportReceiver</span>(<span class="hljs-params">Context context,
        String packageName, <span class="hljs-keyword">int</span> appFlags</span>) </span>{
    <span class="hljs-comment">//检查Settings中的"send_action_app_error"是否使能错误报告的功能</span>
    <span class="hljs-keyword">int</span> enabled = Settings.Global.getInt(context.getContentResolver(),
            Settings.Global.SEND_ACTION_APP_ERROR, <span class="hljs-number">0</span>);
    <span class="hljs-keyword">if</span> (enabled == <span class="hljs-number">0</span>) {
        <span class="hljs-comment">//1.当未使能时，则直接返回</span>
        <span class="hljs-keyword">return</span> <span class="hljs-literal">null</span>;
    }

    PackageManager pm = context.getPackageManager();

    String candidate = <span class="hljs-literal">null</span>;
    ComponentName result = <span class="hljs-literal">null</span>;
    <span class="hljs-keyword">try</span> {
        <span class="hljs-comment">//获取该crash应用的安装器的包名</span>
        candidate = pm.getInstallerPackageName(packageName);
    } <span class="hljs-keyword">catch</span> (IllegalArgumentException e) {
    }

    <span class="hljs-keyword">if</span> (candidate != <span class="hljs-literal">null</span>) {
        result = getErrorReportReceiver(pm, packageName, candidate);<span class="hljs-comment">//【见下文】</span>
        <span class="hljs-keyword">if</span> (result != <span class="hljs-literal">null</span>) {
            <span class="hljs-comment">//2.当找到该crash应用的安装器，则返回；</span>
            <span class="hljs-keyword">return</span> result;
        }
    }

    <span class="hljs-keyword">if</span> ((appFlags&amp;ApplicationInfo.FLAG_SYSTEM) != <span class="hljs-number">0</span>) {
        <span class="hljs-comment">//该系统属性名为"ro.error.receiver.system.apps"</span>
        candidate = SystemProperties.<span class="hljs-keyword">get</span>(SYSTEM_APPS_ERROR_RECEIVER_PROPERTY);
        result = getErrorReportReceiver(pm, packageName, candidate);<span class="hljs-comment">//【见下文】</span>
        <span class="hljs-keyword">if</span> (result != <span class="hljs-literal">null</span>) {
            <span class="hljs-comment">//3.当crash应用是系统应用时，且系统属性指定error receiver时，则返回；</span>
            <span class="hljs-keyword">return</span> result;
        }
    }

    <span class="hljs-comment">//该默认属性名为"ro.error.receiver.default"</span>
    candidate = SystemProperties.<span class="hljs-keyword">get</span>(DEFAULT_ERROR_RECEIVER_PROPERTY);
    <span class="hljs-comment">//4.当默认属性值指定error receiver时，则返回；</span>
    <span class="hljs-keyword">return</span> getErrorReportReceiver(pm, packageName, candidate); <span class="hljs-comment">//【见下文】</span>
}
</code></pre></div></div>

<p><strong>getErrorReportReceiver</strong>：这是同名不同输入参数的另一个方法：</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs javascript"><span class="hljs-keyword">static</span> ComponentName getErrorReportReceiver(PackageManager pm, <span class="hljs-built_in">String</span> errorPackage,
        <span class="hljs-built_in">String</span> receiverPackage) {
    <span class="hljs-keyword">if</span> (receiverPackage == <span class="hljs-literal">null</span> || receiverPackage.length() == <span class="hljs-number">0</span>) {
        <span class="hljs-keyword">return</span> <span class="hljs-literal">null</span>;
    }

    <span class="hljs-comment">//当安装应用程序的安装器Crash，则直接返回</span>
    <span class="hljs-keyword">if</span> (receiverPackage.equals(errorPackage)) {
        <span class="hljs-keyword">return</span> <span class="hljs-literal">null</span>;
    }

    <span class="hljs-comment">//ACTION_APP_ERROR值为"android.intent.action.APP_ERROR"</span>
    Intent intent = <span class="hljs-keyword">new</span> Intent(Intent.ACTION_APP_ERROR);
    intent.setPackage(receiverPackage);
    ResolveInfo info = pm.resolveActivity(intent, <span class="hljs-number">0</span>);
    <span class="hljs-keyword">if</span> (info == <span class="hljs-literal">null</span> || info.activityInfo == <span class="hljs-literal">null</span>) {
        <span class="hljs-keyword">return</span> <span class="hljs-literal">null</span>;
    }
    <span class="hljs-comment">//创建包名为receiverPackage的组件</span>
    <span class="hljs-keyword">return</span> <span class="hljs-keyword">new</span> ComponentName(receiverPackage, info.activityInfo.name);
}
</code></pre></div></div>

<h4 id="72-skipcurrentreceiverlocked">7.2 skipCurrentReceiverLocked<a class="anchorjs-link " href="#72-skipcurrentreceiverlocked" aria-label="Anchor link for: 72 skipcurrentreceiverlocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<p>[–&gt;ActivityManagerService.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">skipCurrentReceiverLocked</span><span class="hljs-params">(ProcessRecord app)</span> </span>{
    <span class="hljs-keyword">for</span> (BroadcastQueue <span class="hljs-built_in">queue</span> : mBroadcastQueues) {
        <span class="hljs-built_in">queue</span>.skipCurrentReceiverLocked(app);  <span class="hljs-comment">//【见小节7.2.1】</span>
    }
}
</code></pre></div></div>

<h5 id="721--skipcurrentreceiverlocked">7.2.1  skipCurrentReceiverLocked<a class="anchorjs-link " href="#721--skipcurrentreceiverlocked" aria-label="Anchor link for: 721  skipcurrentreceiverlocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h5>

<p>[-&gt; BroadcastQueue.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cs"><span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">skipCurrentReceiverLocked</span>(<span class="hljs-params">ProcessRecord app</span>) </span>{
    BroadcastRecord r = <span class="hljs-literal">null</span>;
    <span class="hljs-comment">//查看app进程中的广播</span>
    <span class="hljs-keyword">if</span> (mOrderedBroadcasts.size() &gt; <span class="hljs-number">0</span>) {
        BroadcastRecord br = mOrderedBroadcasts.<span class="hljs-keyword">get</span>(<span class="hljs-number">0</span>);
        <span class="hljs-keyword">if</span> (br.curApp == app) {
            r = br;
        }
    }
    <span class="hljs-keyword">if</span> (r == <span class="hljs-literal">null</span> &amp;&amp; mPendingBroadcast != <span class="hljs-literal">null</span> &amp;&amp; mPendingBroadcast.curApp == app) {
        r = mPendingBroadcast;
    }

    <span class="hljs-keyword">if</span> (r != <span class="hljs-literal">null</span>) {
        <span class="hljs-comment">//结束app进程的广播结束</span>
        finishReceiverLocked(r, r.resultCode, r.resultData,
                r.resultExtras, r.resultAbort, <span class="hljs-literal">false</span>);
        <span class="hljs-comment">//广播调度</span>
        scheduleBroadcastsLocked();
    }
}
</code></pre></div></div>

<h3 id="8-prstopfreezingalllocked">8. PR.stopFreezingAllLocked<a class="anchorjs-link " href="#8-prstopfreezingalllocked" aria-label="Anchor link for: 8 prstopfreezingalllocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; ProcessRecord.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cs"><span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">stopFreezingAllLocked</span>(<span class="hljs-params"></span>) </span>{
    <span class="hljs-keyword">int</span> i = activities.size();
    <span class="hljs-keyword">while</span> (i &gt; <span class="hljs-number">0</span>) {
        i--;
        activities.<span class="hljs-keyword">get</span>(i).stopFreezingScreenLocked(<span class="hljs-literal">true</span>); <span class="hljs-comment">//【见小节8.1】</span>
    }
}
</code></pre></div></div>

<p>其中activities类型为ArrayList<code class="highlighter-rouge">&lt;ActivityRecord</code>&gt;，停止进程里所有的Activity</p>

<h4 id="81-arstopfreezingscreenlocked">8.1. AR.stopFreezingScreenLocked<a class="anchorjs-link " href="#81-arstopfreezingscreenlocked" aria-label="Anchor link for: 81 arstopfreezingscreenlocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<p>[-&gt; ActivityRecord.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">stopFreezingScreenLocked</span><span class="hljs-params">(<span class="hljs-keyword">boolean</span> force)</span> </span>{
    <span class="hljs-keyword">if</span> (force || frozenBeforeDestroy) {
        frozenBeforeDestroy = <span class="hljs-keyword">false</span>;
        <span class="hljs-comment">//mWindowManager类型为WMS //【见小节8.1.1】</span>
        service.mWindowManager.stopAppFreezingScreen(appToken, force);
    }
}
</code></pre></div></div>

<p>其中appToken是IApplication.Stub类型，即WindowManager的token。</p>

<h5 id="811-wmsstopfreezingscreenlocked">8.1.1 WMS.stopFreezingScreenLocked<a class="anchorjs-link " href="#811-wmsstopfreezingscreenlocked" aria-label="Anchor link for: 811 wmsstopfreezingscreenlocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h5>

<p>[-&gt; WindowManagerService.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-meta">@Override</span>
<span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">stopFreezingScreen</span><span class="hljs-params">()</span> </span>{
    <span class="hljs-comment">//权限检查</span>
    <span class="hljs-keyword">if</span> (!checkCallingPermission(android.Manifest.permission.FREEZE_SCREEN,
            <span class="hljs-string">"stopFreezingScreen()"</span>)) {
        <span class="hljs-keyword">throw</span> <span class="hljs-keyword">new</span> SecurityException(<span class="hljs-string">"Requires FREEZE_SCREEN permission"</span>);
    }

    <span class="hljs-keyword">synchronized</span>(mWindowMap) {
        <span class="hljs-keyword">if</span> (mClientFreezingScreen) {
            mClientFreezingScreen = <span class="hljs-keyword">false</span>;
            mLastFinishedFreezeSource = <span class="hljs-string">"client"</span>;
            <span class="hljs-keyword">final</span> <span class="hljs-keyword">long</span> origId = Binder.clearCallingIdentity();
            <span class="hljs-keyword">try</span> {
                stopFreezingDisplayLocked(); <span class="hljs-comment">//【见流程8.1.1.1】</span>
            } <span class="hljs-keyword">finally</span> {
                Binder.restoreCallingIdentity(origId);
            }
        }
    }
}
</code></pre></div></div>

<h6 id="8111-wmsstopfreezingdisplaylocked">8.1.1.1 WMS.stopFreezingDisplayLocked<a class="anchorjs-link " href="#8111-wmsstopfreezingdisplaylocked" aria-label="Anchor link for: 8111 wmsstopfreezingdisplaylocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h6>

<p>[-&gt; WindowManagerService.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">void</span> <span class="hljs-title">stopFreezingDisplayLocked</span><span class="hljs-params">()</span> </span>{
    <span class="hljs-keyword">if</span> (!mDisplayFrozen) {
        <span class="hljs-keyword">return</span>; <span class="hljs-comment">//显示没有冻结，则直接返回</span>
    }

    <span class="hljs-comment">//往往跟屏幕旋转相关</span>
    ...

    mDisplayFrozen = <span class="hljs-keyword">false</span>;
    <span class="hljs-comment">//从上次冻屏到现在的总时长</span>
    mLastDisplayFreezeDuration = (<span class="hljs-keyword">int</span>)(SystemClock.elapsedRealtime() - mDisplayFreezeTime);

    <span class="hljs-comment">//移除冻屏的超时消息</span>
    mH.removeMessages(H.APP_FREEZE_TIMEOUT);
    mH.removeMessages(H.CLIENT_FREEZE_TIMEOUT);

    <span class="hljs-keyword">boolean</span> updateRotation = <span class="hljs-keyword">false</span>;
    <span class="hljs-comment">//获取默认的DisplayContent</span>
    <span class="hljs-keyword">final</span> DisplayContent displayContent = getDefaultDisplayContentLocked();
    <span class="hljs-keyword">final</span> <span class="hljs-keyword">int</span> displayId = displayContent.getDisplayId();
    ScreenRotationAnimation screenRotationAnimation =
            mAnimator.getScreenRotationAnimationLocked(displayId);

    <span class="hljs-comment">//屏幕旋转动画的相关操作</span>
    <span class="hljs-keyword">if</span> (CUSTOM_SCREEN_ROTATION &amp;&amp; screenRotationAnimation != <span class="hljs-keyword">null</span>
            &amp;&amp; screenRotationAnimation.hasScreenshot()) {
        DisplayInfo displayInfo = displayContent.getDisplayInfo();
        <span class="hljs-keyword">boolean</span> isDimming = displayContent.isDimming();
        <span class="hljs-keyword">if</span> (!mPolicy.validateRotationAnimationLw(mExitAnimId, mEnterAnimId, isDimming)) {
            mExitAnimId = mEnterAnimId = <span class="hljs-number">0</span>;
        }
        <span class="hljs-comment">//加载动画最大时长为10s</span>
        <span class="hljs-keyword">if</span> (screenRotationAnimation.dismiss(mFxSession, MAX_ANIMATION_DURATION,
                getTransitionAnimationScaleLocked(), displayInfo.logicalWidth,
                    displayInfo.logicalHeight, mExitAnimId, mEnterAnimId)) {
            scheduleAnimationLocked();
        } <span class="hljs-keyword">else</span> {
            screenRotationAnimation.kill();
            mAnimator.setScreenRotationAnimationLocked(displayId, <span class="hljs-keyword">null</span>);
            updateRotation = <span class="hljs-keyword">true</span>;
        }
    } <span class="hljs-keyword">else</span> {
        <span class="hljs-keyword">if</span> (screenRotationAnimation != <span class="hljs-keyword">null</span>) {
            screenRotationAnimation.kill();
            mAnimator.setScreenRotationAnimationLocked(displayId, <span class="hljs-keyword">null</span>);
        }
        updateRotation = <span class="hljs-keyword">true</span>;
    }
    <span class="hljs-comment">//经过层层调用到InputManagerService服务，IMS服务使能输入事件分发功能</span>
    mInputMonitor.thawInputDispatchingLw();

    <span class="hljs-keyword">boolean</span> configChanged;
    <span class="hljs-comment">//当display被冻结时不再计算屏幕方向，以避免不连续的状态。</span>
    configChanged = updateOrientationFromAppTokensLocked(<span class="hljs-keyword">false</span>);

    <span class="hljs-comment">//display冻结时，执行gc操作</span>
    mH.removeMessages(H.FORCE_GC);
    mH.sendEmptyMessageDelayed(H.FORCE_GC, <span class="hljs-number">2000</span>);

    <span class="hljs-comment">//mScreenFrozenLock的类型为PowerManager.WakeLock，即释放屏幕冻结的锁</span>
    mScreenFrozenLock.release();

    <span class="hljs-keyword">if</span> (updateRotation) {
        <span class="hljs-comment">//更新当前的屏幕方向</span>
        configChanged |= updateRotationUncheckedLocked(<span class="hljs-keyword">false</span>);
    }

    <span class="hljs-keyword">if</span> (configChanged) {
        <span class="hljs-comment">//向mH发送configuraion改变的消息</span>
        mH.sendEmptyMessage(H.SEND_NEW_CONFIGURATION);
    }
}
</code></pre></div></div>

<p>该方法主要功能：</p>

<ol>
  <li>处理屏幕旋转相关逻辑；</li>
  <li>移除冻屏的超时消息；</li>
  <li>屏幕旋转动画的相关操作;</li>
  <li>使能输入事件分发功能；</li>
  <li>display冻结时，执行gc操作；</li>
  <li>更新当前的屏幕方向；</li>
  <li>向mH发送configuraion改变的消息。</li>
</ol>

<h3 id="9amshandleappcrashlocked">9.AMS.handleAppCrashLocked<a class="anchorjs-link " href="#9amshandleappcrashlocked" aria-label="Anchor link for: 9amshandleappcrashlocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; ActivityManagerService.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">boolean</span> <span class="hljs-title">handleAppCrashLocked</span><span class="hljs-params">(ProcessRecord app, String reason,
        String shortMsg, String longMsg, String stackTrace)</span> </span>{
    <span class="hljs-keyword">long</span> now = SystemClock.uptimeMillis();

    Long crashTime;
    <span class="hljs-keyword">if</span> (!app.isolated) {
        crashTime = mProcessCrashTimes.get(app.info.processName, app.uid);
    } <span class="hljs-keyword">else</span> {
        crashTime = <span class="hljs-keyword">null</span>;
    }
    <span class="hljs-comment">//当同一个进程，连续两次crash的时间间隔小于1分钟时，则认为crash太过于频繁</span>
    <span class="hljs-keyword">if</span> (crashTime != <span class="hljs-keyword">null</span> &amp;&amp; now &lt; crashTime+ProcessList.MIN_CRASH_INTERVAL) {
        EventLog.writeEvent(EventLogTags.AM_PROCESS_CRASHED_TOO_MUCH,
                app.userId, app.info.processName, app.uid);
        <span class="hljs-comment">//【见小节9.1】</span>
        mStackSupervisor.handleAppCrashLocked(app);
        <span class="hljs-keyword">if</span> (!app.persistent) {
            <span class="hljs-comment">//不再重启非persistent进程，除非用户显式地调用</span>
            EventLog.writeEvent(EventLogTags.AM_PROC_BAD, app.userId, app.uid,
                    app.info.processName);
            <span class="hljs-keyword">if</span> (!app.isolated) {
                <span class="hljs-comment">//将当前app加入到mBadProcesses</span>
                mBadProcesses.put(app.info.processName, app.uid,
                        <span class="hljs-keyword">new</span> BadProcessInfo(now, shortMsg, longMsg, stackTrace));
                mProcessCrashTimes.remove(app.info.processName, app.uid);
            }
            app.bad = <span class="hljs-keyword">true</span>;
            app.removed = <span class="hljs-keyword">true</span>;
            <span class="hljs-comment">//移除进程的所有服务，保证不再重启【见小节9.2】</span>
            removeProcessLocked(app, <span class="hljs-keyword">false</span>, <span class="hljs-keyword">false</span>, <span class="hljs-string">"crash"</span>);
            <span class="hljs-comment">//恢复最顶部的Activity【见小节9.3】</span>
            mStackSupervisor.resumeTopActivitiesLocked();
            <span class="hljs-keyword">return</span> <span class="hljs-keyword">false</span>;
        }
        mStackSupervisor.resumeTopActivitiesLocked();
    } <span class="hljs-keyword">else</span> {
        <span class="hljs-comment">//此处reason="force-crash"【见小节9.4】</span>
        mStackSupervisor.finishTopRunningActivityLocked(app, reason);
    }

    <span class="hljs-comment">//运行在当前进程中的所有服务的crash次数执行加1操作</span>
    <span class="hljs-keyword">for</span> (<span class="hljs-keyword">int</span> i=app.services.size()-<span class="hljs-number">1</span>; i&gt;=<span class="hljs-number">0</span>; i--) {
        ServiceRecord sr = app.services.valueAt(i);
        sr.crashCount++;
    }

    <span class="hljs-comment">//当桌面应用crash，并且被三方app所取代，那么需要清空桌面应用的偏爱选项。</span>
    <span class="hljs-keyword">final</span> ArrayList&lt;ActivityRecord&gt; activities = app.activities;
    <span class="hljs-keyword">if</span> (app == mHomeProcess &amp;&amp; activities.size() &gt; <span class="hljs-number">0</span>
                &amp;&amp; (mHomeProcess.info.flags &amp; ApplicationInfo.FLAG_SYSTEM) == <span class="hljs-number">0</span>) {
        <span class="hljs-keyword">for</span> (<span class="hljs-keyword">int</span> activityNdx = activities.size() - <span class="hljs-number">1</span>; activityNdx &gt;= <span class="hljs-number">0</span>; --activityNdx) {
            <span class="hljs-keyword">final</span> ActivityRecord r = activities.get(activityNdx);
            <span class="hljs-keyword">if</span> (r.isHomeActivity()) {
                <span class="hljs-comment">//清空偏爱应用</span>
                ActivityThread.getPackageManager()
                        .clearPackagePreferredActivities(r.packageName);
            }
        }
    }

    <span class="hljs-keyword">if</span> (!app.isolated) {
        <span class="hljs-comment">//无法记录孤立进程的crash时间点，由于他们并没有一个固定身份</span>
        mProcessCrashTimes.put(app.info.processName, app.uid, now);
    }
    <span class="hljs-comment">//当app存在crash的handler，那么交给其处理</span>
    <span class="hljs-keyword">if</span> (app.crashHandler != <span class="hljs-keyword">null</span>) mHandler.post(app.crashHandler);
    <span class="hljs-keyword">return</span> <span class="hljs-keyword">true</span>;
}
</code></pre></div></div>

<ol>
  <li>当同一进程在时间间隔小于1分钟时连续两次crash，则执行的情况下：
    <ul>
      <li>对于非persistent进程：
        <ul>
          <li>[9.1] mStackSupervisor.handleAppCrashLocked(app);</li>
          <li>[9.2] removeProcessLocked(app, false, false, “crash”);</li>
          <li>[9.3] mStackSupervisor.resumeTopActivitiesLocked();</li>
        </ul>
      </li>
      <li>对于persistent进程，则只执行
        <ul>
          <li>[9.3] mStackSupervisor.resumeTopActivitiesLocked();</li>
        </ul>
      </li>
    </ul>
  </li>
  <li>否则执行
    <ul>
      <li>[9.4] mStackSupervisor.finishTopRunningActivityLocked(app, reason);</li>
    </ul>
  </li>
</ol>

<h4 id="91-asshandleappcrashlocked">9.1 ASS.handleAppCrashLocked<a class="anchorjs-link " href="#91-asshandleappcrashlocked" aria-label="Anchor link for: 91 asshandleappcrashlocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<p>[-&gt; ActivityStackSupervisor.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">handleAppCrashLocked</span><span class="hljs-params">(ProcessRecord app)</span> </span>{
    <span class="hljs-keyword">for</span> (<span class="hljs-keyword">int</span> displayNdx = mActivityDisplays.size() - <span class="hljs-number">1</span>; displayNdx &gt;= <span class="hljs-number">0</span>; --displayNdx) {
        <span class="hljs-keyword">final</span> ArrayList&lt;ActivityStack&gt; stacks = mActivityDisplays.valueAt(displayNdx).mStacks;
        <span class="hljs-keyword">int</span> stackNdx = stacks.size() - <span class="hljs-number">1</span>;
        <span class="hljs-keyword">while</span> (stackNdx &gt;= <span class="hljs-number">0</span>) {
            <span class="hljs-comment">//调用ActivityStack【见小节9.1.1】</span>
            stacks.get(stackNdx).handleAppCrashLocked(app);
            stackNdx--;
        }
    }
}
</code></pre></div></div>

<h5 id="911-ashandleappcrashlocked">9.1.1 AS.handleAppCrashLocked<a class="anchorjs-link " href="#911-ashandleappcrashlocked" aria-label="Anchor link for: 911 ashandleappcrashlocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h5>

<p>[-&gt; ActivityStack.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">handleAppCrashLocked</span><span class="hljs-params">(ProcessRecord app)</span> </span>{
    <span class="hljs-keyword">for</span> (<span class="hljs-keyword">int</span> taskNdx = mTaskHistory.size() - <span class="hljs-number">1</span>; taskNdx &gt;= <span class="hljs-number">0</span>; --taskNdx) {
        <span class="hljs-keyword">final</span> ArrayList&lt;ActivityRecord&gt; activities = mTaskHistory.get(taskNdx).mActivities;
        <span class="hljs-keyword">for</span> (<span class="hljs-keyword">int</span> activityNdx = activities.size() - <span class="hljs-number">1</span>; activityNdx &gt;= <span class="hljs-number">0</span>; --activityNdx) {
            <span class="hljs-keyword">final</span> ActivityRecord r = activities.get(activityNdx);
            <span class="hljs-keyword">if</span> (r.app == app) {
                r.app = <span class="hljs-keyword">null</span>;
                <span class="hljs-comment">//结束当前activity</span>
                finishCurrentActivityLocked(r, FINISH_IMMEDIATELY, <span class="hljs-keyword">false</span>);
            }
        }
    }
}
</code></pre></div></div>

<p>这里的<code class="highlighter-rouge">mTaskHistory</code>数据类型为ArrayList<taskrecord>，记录着所有先前的后台activities。遍历所有activities，找到位于该ProcessRecord的所有ActivityRecord，并结束该Acitivity。</taskrecord></p>

<h4 id="92-amsremoveprocesslocked">9.2 AMS.removeProcessLocked<a class="anchorjs-link " href="#92-amsremoveprocesslocked" aria-label="Anchor link for: 92 amsremoveprocesslocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<p>[-&gt; ActivityManagerService.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">final</span> <span class="hljs-keyword">boolean</span> <span class="hljs-title">removeProcessLocked</span><span class="hljs-params">(ProcessRecord app,
        <span class="hljs-keyword">boolean</span> callerWillRestart, <span class="hljs-keyword">boolean</span> allowRestart, String reason)</span> </span>{
    <span class="hljs-keyword">final</span> String name = app.processName;
    <span class="hljs-keyword">final</span> <span class="hljs-keyword">int</span> uid = app.uid;
    <span class="hljs-comment">//从mProcessNames移除该进程</span>
    removeProcessNameLocked(name, uid);
    ...
    <span class="hljs-keyword">if</span> (app.pid &gt; <span class="hljs-number">0</span> &amp;&amp; app.pid != MY_PID) {
        <span class="hljs-keyword">int</span> pid = app.pid;
        <span class="hljs-keyword">synchronized</span> (mPidsSelfLocked) {
            mPidsSelfLocked.remove(pid); <span class="hljs-comment">//移除该pid</span>
            mHandler.removeMessages(PROC_START_TIMEOUT_MSG, app);
        }
        ...
        <span class="hljs-keyword">boolean</span> willRestart = <span class="hljs-keyword">false</span>;
        <span class="hljs-comment">//对于非孤立的persistent进程设置成可重启flags</span>
        <span class="hljs-keyword">if</span> (app.persistent &amp;&amp; !app.isolated) {
            <span class="hljs-keyword">if</span> (!callerWillRestart) {
                willRestart = <span class="hljs-keyword">true</span>;
            } <span class="hljs-keyword">else</span> {
                needRestart = <span class="hljs-keyword">true</span>;
            }
        }
        <span class="hljs-comment">// 杀进程【9.2.1】</span>
        app.kill(reason, <span class="hljs-keyword">true</span>);
         <span class="hljs-comment">//移除进程并清空该进程相关联的activity/service等组件 【9.2.2】</span>
        handleAppDiedLocked(app, willRestart, allowRestart);

        <span class="hljs-keyword">if</span> (willRestart) {
            <span class="hljs-comment">//此处willRestart=false，不进入该分支</span>
            removeLruProcessLocked(app);
            addAppLocked(app.info, <span class="hljs-keyword">false</span>, <span class="hljs-keyword">null</span> <span class="hljs-comment">/* ABI override */</span>);
        }
    } <span class="hljs-keyword">else</span> {
        mRemovedProcesses.add(app);
    }
    <span class="hljs-keyword">return</span> needRestart;
}
</code></pre></div></div>

<ul>
  <li><code class="highlighter-rouge">mProcessNames</code>数据类型为ProcessMap<processrecord>，这是以进程名为key，记录着所有的ProcessRecord信息</processrecord></li>
  <li><code class="highlighter-rouge">mPidsSelfLocked</code>数据类型为SparseArray<processrecord>,这是以pid为key，记录着所有的ProcessRecord信息。该对象的同步保护是通过自身锁，而非全局ActivityManager锁。</processrecord></li>
</ul>

<h5 id="921-appkill">9.2.1 app.kill<a class="anchorjs-link " href="#921-appkill" aria-label="Anchor link for: 921 appkill" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h5>

<p>[-&gt; ProcessRecord.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">kill</span><span class="hljs-params">(String reason, <span class="hljs-keyword">boolean</span> noisy)</span> </span>{
    <span class="hljs-keyword">if</span> (!killedByAm) {
        Trace.traceBegin(Trace.TRACE_TAG_ACTIVITY_MANAGER, <span class="hljs-string">"kill"</span>);
        <span class="hljs-keyword">if</span> (noisy) {
            Slog.i(TAG, <span class="hljs-string">"Killing "</span> + toShortString() + <span class="hljs-string">" (adj "</span> + setAdj + <span class="hljs-string">"): "</span> + reason);
        }
        EventLog.writeEvent(EventLogTags.AM_KILL, userId, pid, processName, setAdj, reason);
        Process.killProcessQuiet(pid); <span class="hljs-comment">//杀进程</span>
        Process.killProcessGroup(info.uid, pid); <span class="hljs-comment">//杀进程组，包括native进程</span>
        <span class="hljs-keyword">if</span> (!persistent) {
            killed = <span class="hljs-keyword">true</span>;
            killedByAm = <span class="hljs-keyword">true</span>;
        }
        Trace.traceEnd(Trace.TRACE_TAG_ACTIVITY_MANAGER);
    }
}
</code></pre></div></div>

<p>此处reason为“crash”，关于杀进程的过程见我的另一篇文章<a href="http://gityuan.com/2016/04/16/kill-signal/">理解杀进程的实现原理</a>.</p>

<h5 id="922-handleappdiedlocked">9.2.2 handleAppDiedLocked<a class="anchorjs-link " href="#922-handleappdiedlocked" aria-label="Anchor link for: 922 handleappdiedlocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h5>

<p>[-&gt; ActivityManagerService.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">final</span> <span class="hljs-keyword">void</span> <span class="hljs-title">handleAppDiedLocked</span><span class="hljs-params">(ProcessRecord app,
        <span class="hljs-keyword">boolean</span> restarting, <span class="hljs-keyword">boolean</span> allowRestart)</span> </span>{
    <span class="hljs-keyword">int</span> pid = app.pid;
    <span class="hljs-comment">//清除应用中service/receiver/ContentProvider信息</span>
    <span class="hljs-keyword">boolean</span> kept = cleanUpApplicationRecordLocked(app, restarting, allowRestart, -<span class="hljs-number">1</span>);
    <span class="hljs-keyword">if</span> (!kept &amp;&amp; !restarting) {
        removeLruProcessLocked(app);
        <span class="hljs-keyword">if</span> (pid &gt; <span class="hljs-number">0</span>) {
            ProcessList.remove(pid);
        }
    }

    <span class="hljs-keyword">if</span> (mProfileProc == app) {
        clearProfilerLocked();
    }

    <span class="hljs-comment">//清除应用中activity相关信息</span>
    <span class="hljs-keyword">boolean</span> hasVisibleActivities = mStackSupervisor.handleAppDiedLocked(app);

    app.activities.clear();
    ...
    <span class="hljs-keyword">if</span> (!restarting &amp;&amp; hasVisibleActivities &amp;&amp; !mStackSupervisor.resumeTopActivitiesLocked()) {
        mStackSupervisor.ensureActivitiesVisibleLocked(<span class="hljs-keyword">null</span>, <span class="hljs-number">0</span>);
    }
}
</code></pre></div></div>

<h4 id="93-assresumetopactivitieslocked">9.3 ASS.resumeTopActivitiesLocked<a class="anchorjs-link " href="#93-assresumetopactivitieslocked" aria-label="Anchor link for: 93 assresumetopactivitieslocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<p>[-&gt; ActivityStackSupervisor.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">boolean</span> <span class="hljs-title">resumeTopActivitiesLocked</span><span class="hljs-params">()</span> </span>{
    <span class="hljs-keyword">return</span> resumeTopActivitiesLocked(<span class="hljs-keyword">null</span>, <span class="hljs-keyword">null</span>, <span class="hljs-keyword">null</span>);
}

<span class="hljs-function"><span class="hljs-keyword">boolean</span> <span class="hljs-title">resumeTopActivitiesLocked</span><span class="hljs-params">(ActivityStack targetStack, ActivityRecord target,
        Bundle targetOptions)</span> </span>{
    <span class="hljs-keyword">if</span> (targetStack == <span class="hljs-keyword">null</span>) {
        targetStack = mFocusedStack;
    }
    <span class="hljs-keyword">boolean</span> result = <span class="hljs-keyword">false</span>;
    <span class="hljs-keyword">if</span> (isFrontStack(targetStack)) {
        <span class="hljs-comment">//【见小节9.3.1】</span>
        result = targetStack.resumeTopActivityLocked(target, targetOptions);
    }

    <span class="hljs-keyword">for</span> (<span class="hljs-keyword">int</span> displayNdx = mActivityDisplays.size() - <span class="hljs-number">1</span>; displayNdx &gt;= <span class="hljs-number">0</span>; --displayNdx) {
        <span class="hljs-keyword">final</span> ArrayList&lt;ActivityStack&gt; stacks = mActivityDisplays.valueAt(displayNdx).mStacks;
        <span class="hljs-keyword">for</span> (<span class="hljs-keyword">int</span> stackNdx = stacks.size() - <span class="hljs-number">1</span>; stackNdx &gt;= <span class="hljs-number">0</span>; --stackNdx) {
            <span class="hljs-keyword">final</span> ActivityStack stack = stacks.get(stackNdx);
            <span class="hljs-keyword">if</span> (stack == targetStack) {
                <span class="hljs-keyword">continue</span>; <span class="hljs-comment">//已经启动</span>
            }
            <span class="hljs-keyword">if</span> (isFrontStack(stack)) {
                stack.resumeTopActivityLocked(<span class="hljs-keyword">null</span>);
            }
        }
    }
    <span class="hljs-keyword">return</span> result;
}
</code></pre></div></div>

<p>此处<code class="highlighter-rouge">mFocusedStack</code>是当前正在等待接收input事件或者正在启动下一个activity的<code class="highlighter-rouge">ActivityStack</code>。</p>

<h5 id="931-asresumetopactivitylocked">9.3.1 AS.resumeTopActivityLocked<a class="anchorjs-link " href="#931-asresumetopactivitylocked" aria-label="Anchor link for: 931 asresumetopactivitylocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h5>

<p>[-&gt; ActivityStack.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-keyword">final</span> <span class="hljs-keyword">boolean</span> .resumeTopActivityLocked(ActivityRecord prev, Bundle options) {
    ...
    result = resumeTopActivityInnerLocked(prev, options);<span class="hljs-comment">//【见小节9.3.2】</span>
    <span class="hljs-keyword">return</span> result;
}
</code></pre></div></div>

<h5 id="932-asresumetopactivityinnerlocked">9.3.2 AS.resumeTopActivityInnerLocked<a class="anchorjs-link " href="#932-asresumetopactivityinnerlocked" aria-label="Anchor link for: 932 asresumetopactivityinnerlocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h5>

<p>[-&gt; ActivityStack.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">boolean</span> <span class="hljs-title">resumeTopActivityInnerLocked</span><span class="hljs-params">(ActivityRecord prev, Bundle options)</span> </span>{
    <span class="hljs-comment">//找到mTaskHistory栈中第一个未处于finishing状态的Activity</span>
    <span class="hljs-keyword">final</span> ActivityRecord next = topRunningActivityLocked(<span class="hljs-keyword">null</span>);

    <span class="hljs-keyword">if</span> (mResumedActivity == next &amp;&amp; next.state == ActivityState.RESUMED &amp;&amp;
                        mStackSupervisor.allResumedActivitiesComplete()) {
        <span class="hljs-comment">//当top activity已经处于resume，则无需操作；</span>
        <span class="hljs-keyword">return</span> <span class="hljs-keyword">false</span>;
    }

    <span class="hljs-keyword">if</span> (mService.isSleepingOrShuttingDown()
            &amp;&amp; mLastPausedActivity == next
            &amp;&amp; mStackSupervisor.allPausedActivitiesComplete()) {
        <span class="hljs-comment">//当正处于sleeping状态，top activity处于paused，则无需操作</span>
        <span class="hljs-keyword">return</span> <span class="hljs-keyword">false</span>;
    }

    <span class="hljs-comment">//正在启动app的activity，确保app不会被设置为stopped</span>
    AppGlobals.getPackageManager().setPackageStoppedState(
                next.packageName, <span class="hljs-keyword">false</span>, next.userId);
    <span class="hljs-comment">//回调应用onResume方法</span>
    next.app.thread.scheduleResumeActivity(next.appToken, next.app.repProcState,
                            mService.isNextTransitionForward(), resumeAnimOptions);
    ...
}
</code></pre></div></div>

<p>该方法代码比较长，这里就简单列举几条比较重要的代码。执行完该方法，应用也便完成了activity的resume过程。</p>

<h4 id="94-finishtoprunningactivitylocked">9.4 finishTopRunningActivityLocked<a class="anchorjs-link " href="#94-finishtoprunningactivitylocked" aria-label="Anchor link for: 94 finishtoprunningactivitylocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<h5 id="941-assfinishtoprunningactivitylocked">9.4.1 ASS.finishTopRunningActivityLocked<a class="anchorjs-link " href="#941-assfinishtoprunningactivitylocked" aria-label="Anchor link for: 941 assfinishtoprunningactivitylocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h5>

<p>[-&gt; ActivityStackSupervisor.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">finishTopRunningActivityLocked</span><span class="hljs-params">(ProcessRecord app, String reason)</span> </span>{
    <span class="hljs-keyword">for</span> (<span class="hljs-keyword">int</span> displayNdx = mActivityDisplays.size() - <span class="hljs-number">1</span>; displayNdx &gt;= <span class="hljs-number">0</span>; --displayNdx) {
        <span class="hljs-keyword">final</span> ArrayList&lt;ActivityStack&gt; stacks = mActivityDisplays.valueAt(displayNdx).mStacks;
        <span class="hljs-keyword">final</span> <span class="hljs-keyword">int</span> numStacks = stacks.size();
        <span class="hljs-keyword">for</span> (<span class="hljs-keyword">int</span> stackNdx = <span class="hljs-number">0</span>; stackNdx &lt; numStacks; ++stackNdx) {
            <span class="hljs-keyword">final</span> ActivityStack stack = stacks.get(stackNdx);
            <span class="hljs-comment">//此处reason= "force-crash"【见小节9.4.2】</span>
            stack.finishTopRunningActivityLocked(app, reason);
        }
    }
}
</code></pre></div></div>

<h5 id="942-asfinishtoprunningactivitylocked">9.4.2 AS.finishTopRunningActivityLocked<a class="anchorjs-link " href="#942-asfinishtoprunningactivitylocked" aria-label="Anchor link for: 942 asfinishtoprunningactivitylocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h5>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cs"><span class="hljs-function">final <span class="hljs-keyword">void</span> <span class="hljs-title">finishTopRunningActivityLocked</span>(<span class="hljs-params">ProcessRecord app, String reason</span>) </span>{
    <span class="hljs-comment">//找到栈顶第一个不处于finishing状态的activity</span>
    ActivityRecord r = topRunningActivityLocked(<span class="hljs-literal">null</span>);
    <span class="hljs-keyword">if</span> (r != <span class="hljs-literal">null</span> &amp;&amp; r.app == app) {
        <span class="hljs-keyword">int</span> taskNdx = mTaskHistory.indexOf(r.task);
        <span class="hljs-keyword">int</span> activityNdx = r.task.mActivities.indexOf(r);
        <span class="hljs-comment">//【见小节9.4.3】</span>
        finishActivityLocked(r, Activity.RESULT_CANCELED, <span class="hljs-literal">null</span>, reason, <span class="hljs-literal">false</span>);
        --activityNdx;
        <span class="hljs-keyword">if</span> (activityNdx &lt; <span class="hljs-number">0</span>) {
            <span class="hljs-keyword">do</span> {
                --taskNdx;
                <span class="hljs-keyword">if</span> (taskNdx &lt; <span class="hljs-number">0</span>) {
                    <span class="hljs-keyword">break</span>;
                }
                activityNdx = mTaskHistory.<span class="hljs-keyword">get</span>(taskNdx).mActivities.size() - <span class="hljs-number">1</span>;
            } <span class="hljs-keyword">while</span> (activityNdx &lt; <span class="hljs-number">0</span>);
        }
        <span class="hljs-keyword">if</span> (activityNdx &gt;= <span class="hljs-number">0</span>) {
            r = mTaskHistory.<span class="hljs-keyword">get</span>(taskNdx).mActivities.<span class="hljs-keyword">get</span>(activityNdx);
            <span class="hljs-keyword">if</span> (r.state == ActivityState.RESUMED
                    || r.state == ActivityState.PAUSING
                    || r.state == ActivityState.PAUSED) {
                <span class="hljs-keyword">if</span> (!r.isHomeActivity() || mService.mHomeProcess != r.app) {
                    <span class="hljs-comment">//【见小节9.4.3】</span>
                    finishActivityLocked(r, Activity.RESULT_CANCELED, <span class="hljs-literal">null</span>, reason, <span class="hljs-literal">false</span>);
                }
            }
        }
    }
}
</code></pre></div></div>

<h5 id="943-asfinishactivitylocked">9.4.3 AS.finishActivityLocked<a class="anchorjs-link " href="#943-asfinishactivitylocked" aria-label="Anchor link for: 943 asfinishactivitylocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h5>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">final</span> <span class="hljs-keyword">boolean</span> <span class="hljs-title">finishActivityLocked</span><span class="hljs-params">(ActivityRecord r, <span class="hljs-keyword">int</span> resultCode, Intent resultData,
           String reason, <span class="hljs-keyword">boolean</span> oomAdj)</span> </span>{
    <span class="hljs-keyword">if</span> (r.finishing) {
        <span class="hljs-keyword">return</span> <span class="hljs-keyword">false</span>; <span class="hljs-comment">//正在finishing则返回</span>
    }
    <span class="hljs-comment">//设置finish状态的activity不可见</span>
    r.makeFinishingLocked();
    <span class="hljs-comment">//暂停key的分发事件</span>
    r.pauseKeyDispatchingLocked();

    mWindowManager.prepareAppTransition(endTask
            ? AppTransition.TRANSIT_TASK_CLOSE
            : AppTransition.TRANSIT_ACTIVITY_CLOSE, <span class="hljs-keyword">false</span>);

    mWindowManager.setAppVisibility(r.appToken, <span class="hljs-keyword">false</span>);
    <span class="hljs-comment">//回调activity的onPause方法</span>
    startPausingLocked(<span class="hljs-keyword">false</span>, <span class="hljs-keyword">false</span>, <span class="hljs-keyword">false</span>, <span class="hljs-keyword">false</span>);
    ...
}
</code></pre></div></div>

<p>该方法最终会回调到activity的pause方法。</p>

<p>执行到这，我们还回过来看小节<code class="highlighter-rouge">5.crashApplication</code>中，处理完makeAppCrashingLocked，则会再发送消息SHOW_ERROR_MSG，弹出提示crash的对话框，接下来再看看该过程。</p>

<h3 id="10-uihandler">10. UiHandler<a class="anchorjs-link " href="#10-uihandler" aria-label="Anchor link for: 10 uihandler" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>通过mUiHandler发送message，且消息的msg.waht=SHOW_ERROR_MSG，接下来进入UiHandler来看看handleMessage的处理过程。</p>

<p>[-&gt; ActivityManagerService.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-keyword">final</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">UiHandler</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">Handler</span> </span>{
    <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">handleMessage</span><span class="hljs-params">(Message msg)</span> </span>{
        <span class="hljs-keyword">switch</span> (msg.what) {
        <span class="hljs-keyword">case</span> SHOW_ERROR_MSG: {
           HashMap&lt;String, Object&gt; data = (HashMap&lt;String, Object&gt;) msg.obj;
           <span class="hljs-keyword">synchronized</span> (ActivityManagerService.<span class="hljs-keyword">this</span>) {
               ProcessRecord proc = (ProcessRecord)data.get(<span class="hljs-string">"app"</span>);
               AppErrorResult res = (AppErrorResult) data.get(<span class="hljs-string">"result"</span>);
              、
               <span class="hljs-keyword">boolean</span> isBackground = (UserHandle.getAppId(proc.uid)
                       &gt;= Process.FIRST_APPLICATION_UID
                       &amp;&amp; proc.pid != MY_PID);
                ...

               <span class="hljs-keyword">if</span> (mShowDialogs &amp;&amp; !mSleeping &amp;&amp; !mShuttingDown) {
                   <span class="hljs-comment">//创建提示crash对话框，等待用户选择，5分钟操作等待。</span>
                   Dialog d = <span class="hljs-keyword">new</span> AppErrorDialog(mContext,
                           ActivityManagerService.<span class="hljs-keyword">this</span>, res, proc);
                   d.show();
                   proc.crashDialog = d;
               } <span class="hljs-keyword">else</span> {
                   <span class="hljs-comment">//当处于sleep状态，则默认选择退出。</span>
                   <span class="hljs-keyword">if</span> (res != <span class="hljs-keyword">null</span>) {
                       res.set(<span class="hljs-number">0</span>);
                   }
               }
           }
        } <span class="hljs-keyword">break</span>;
        ...
    }
}
</code></pre></div></div>

<p>在发生crash时，默认系统会弹出提示crash的对话框，并阻塞等待用户选择是“退出”或 “退出并报告”，当用户不做任何选择时5min超时后，默认选择“退出”，当手机休眠时也默认选择“退出”。到这里也并没有真正结束，在小节<code class="highlighter-rouge">2.uncaughtException</code>中在<code class="highlighter-rouge">finnally</code>语句块还有一个杀进程的动作。</p>

<h3 id="11-killprocess">11. killProcess<a class="anchorjs-link " href="#11-killprocess" aria-label="Anchor link for: 11 killprocess" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs css"><span class="hljs-selector-tag">Process</span><span class="hljs-selector-class">.killProcess</span>(<span class="hljs-selector-tag">Process</span><span class="hljs-selector-class">.myPid</span>());
<span class="hljs-selector-tag">System</span><span class="hljs-selector-class">.exit</span>(10);
</code></pre></div></div>

<p>通过finnally语句块保证能执行并彻底杀掉Crash进程，关于杀进程的过程见我的另一篇文章<a href="http://gityuan.com/2016/04/16/kill-signal/">理解杀进程的实现原理</a>.。当Crash进程被杀后，并没有完全结束，还有Binder死亡通知的流程还没有处理完成。</p>

<h3 id="12-小结">12. 小结<a class="anchorjs-link " href="#12-小结" aria-label="Anchor link for: 12 小结" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>当进程抛出未捕获异常时，则系统会处理该异常并进入crash处理流程。</p>

<p><img src="./images/app_crash.jpg" alt="app_crash"></p>

<p>其中最为核心的工作图中红色部分<code class="highlighter-rouge">AMS.handleAppCrashLocked</code>的主要功能：</p>

<ol>
  <li>当同一进程1分钟之内连续两次crash，则执行的情况下：
    <ul>
      <li>对于非persistent进程：
        <ul>
          <li>ASS.handleAppCrashLocked, 直接结束该应用所有activity</li>
          <li>AMS.removeProcessLocked，杀死该进程以及同一个进程组下的所有进</li>
          <li>ASS.resumeTopActivitiesLocked，恢复栈顶第一个非finishing状态的activity</li>
        </ul>
      </li>
      <li>对于persistent进程，则只执行
        <ul>
          <li>ASS.resumeTopActivitiesLocked，恢复栈顶第一个非finishing状态的activity</li>
        </ul>
      </li>
    </ul>
  </li>
  <li>否则，当进程没连续频繁crash
    <ul>
      <li>ASS.finishTopRunningActivityLocked，执行结束栈顶正在运行activity</li>
    </ul>
  </li>
</ol>

<p>另外，<code class="highlighter-rouge">AMS.handleAppCrashLocked</code>，该方法内部主要调用链，如下：</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs css"><span class="hljs-selector-tag">AMS</span><span class="hljs-selector-class">.handleAppCrashLocked</span>
   <span class="hljs-selector-tag">ASS</span><span class="hljs-selector-class">.handleAppCrashLocked</span>
       <span class="hljs-selector-tag">AS</span><span class="hljs-selector-class">.handleAppCrashLocked</span>
           <span class="hljs-selector-tag">AS</span><span class="hljs-selector-class">.finishCurrentActivityLocked</span>
   <span class="hljs-selector-tag">AMS</span><span class="hljs-selector-class">.removeProcessLocked</span>
       <span class="hljs-selector-tag">ProcessRecord</span><span class="hljs-selector-class">.kill</span>
       <span class="hljs-selector-tag">AMS</span><span class="hljs-selector-class">.handleAppDiedLocked</span>
           <span class="hljs-selector-tag">ASS</span><span class="hljs-selector-class">.handleAppDiedLocked</span>
               <span class="hljs-selector-tag">AMS</span><span class="hljs-selector-class">.cleanUpApplicationRecordLocked</span>
               <span class="hljs-selector-tag">AS</span><span class="hljs-selector-class">.handleAppDiedLocked</span>
                   <span class="hljs-selector-tag">AS</span><span class="hljs-selector-class">.removeHistoryRecordsForAppLocked</span>

   <span class="hljs-selector-tag">ASS</span><span class="hljs-selector-class">.resumeTopActivitiesLocked</span>
       <span class="hljs-selector-tag">AS</span><span class="hljs-selector-class">.resumeTopActivityLocked</span>
           <span class="hljs-selector-tag">AS</span><span class="hljs-selector-class">.resumeTopActivityInnerLocked</span>
   <span class="hljs-selector-tag">ASS</span><span class="hljs-selector-class">.finishTopRunningActivityLocked</span>
       <span class="hljs-selector-tag">AS</span><span class="hljs-selector-class">.finishTopRunningActivityLocked</span>
           <span class="hljs-selector-tag">AS</span><span class="hljs-selector-class">.finishActivityLocked</span>
</code></pre></div></div>

<h2 id="三binder死亡通知">三、Binder死亡通知<a class="anchorjs-link " href="#三binder死亡通知" aria-label="Anchor link for: 三binder死亡通知" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h2>

<p>进程被杀，如果还记得Binder的死亡回调机制，在应用进程创建的过程中有一个<code class="highlighter-rouge">attachApplicationLocked</code>方法的过程中便会创建死亡通知。</p>

<p>[-&gt; ActivityManagerService.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">final</span> <span class="hljs-keyword">boolean</span> <span class="hljs-title">attachApplicationLocked</span><span class="hljs-params">(IApplicationThread thread,
            <span class="hljs-keyword">int</span> pid)</span> </span>{
    <span class="hljs-keyword">try</span> {
        <span class="hljs-comment">//创建binder死亡通知</span>
        AppDeathRecipient adr = <span class="hljs-keyword">new</span> AppDeathRecipient(
                app, pid, thread);
        thread.asBinder().linkToDeath(adr, <span class="hljs-number">0</span>);
        app.deathRecipient = adr;
    } <span class="hljs-keyword">catch</span> (RemoteException e) {
        app.resetPackageList(mProcessStats);
        startProcessLocked(app, <span class="hljs-string">"link fail"</span>, processName);
        <span class="hljs-keyword">return</span> <span class="hljs-keyword">false</span>;
    }
    ...
}
</code></pre></div></div>

<p>当binder服务端挂了之后，便会通过binder的DeathRecipient来通知AMS进行相应的清理收尾工作。前面已经降到crash的进程会被kill掉，那么当该进程会杀，则会回调到binderDied()方法。</p>

<h3 id="1-binderdied">1. binderDied<a class="anchorjs-link " href="#1-binderdied" aria-label="Anchor link for: 1 binderdied" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; ActivityManagerService.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-keyword">private</span> <span class="hljs-keyword">final</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">AppDeathRecipient</span> <span class="hljs-keyword">implements</span> <span class="hljs-title">IBinder</span>.<span class="hljs-title">DeathRecipient</span> </span>{
    <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">binderDied</span><span class="hljs-params">()</span> </span>{
        <span class="hljs-keyword">synchronized</span>(ActivityManagerService.<span class="hljs-keyword">this</span>) {
            appDiedLocked(mApp, mPid, mAppThread, <span class="hljs-keyword">true</span>);<span class="hljs-comment">//【见小节2】</span>
        }
    }
}
</code></pre></div></div>

<h3 id="2-appdiedlocked">2. appDiedLocked<a class="anchorjs-link " href="#2-appdiedlocked" aria-label="Anchor link for: 2 appdiedlocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">final</span> <span class="hljs-keyword">void</span> <span class="hljs-title">appDiedLocked</span><span class="hljs-params">(ProcessRecord app, <span class="hljs-keyword">int</span> pid, IApplicationThread thread,
        <span class="hljs-keyword">boolean</span> fromBinderDied)</span> </span>{
    ...
    <span class="hljs-keyword">if</span> (!app.killed) {
        <span class="hljs-keyword">if</span> (!fromBinderDied) {
            Process.killProcessQuiet(pid);
        }
        killProcessGroup(app.info.uid, pid);
        app.killed = <span class="hljs-keyword">true</span>;
    }

    <span class="hljs-comment">// Clean up already done if the process has been re-started.</span>
    <span class="hljs-keyword">if</span> (app.pid == pid &amp;&amp; app.thread != <span class="hljs-keyword">null</span> &amp;&amp;
            app.thread.asBinder() == thread.asBinder()) {
        <span class="hljs-keyword">boolean</span> doLowMem = app.instrumentationClass == <span class="hljs-keyword">null</span>;
        <span class="hljs-keyword">boolean</span> doOomAdj = doLowMem;
        <span class="hljs-keyword">if</span> (!app.killedByAm) {
            mAllowLowerMemLevel = <span class="hljs-keyword">true</span>;
        } <span class="hljs-keyword">else</span> {
            mAllowLowerMemLevel = <span class="hljs-keyword">false</span>;
            doLowMem = <span class="hljs-keyword">false</span>;
        }
        <span class="hljs-comment">//【见小节3】</span>
        handleAppDiedLocked(app, <span class="hljs-keyword">false</span>, <span class="hljs-keyword">true</span>);

        <span class="hljs-keyword">if</span> (doOomAdj) {
            updateOomAdjLocked();
        }
        <span class="hljs-keyword">if</span> (doLowMem) {
            doLowMemReportIfNeededLocked(app);
        }
    }
    ...
}
</code></pre></div></div>

<h3 id="3-handleappdiedlocked">3 handleAppDiedLocked<a class="anchorjs-link " href="#3-handleappdiedlocked" aria-label="Anchor link for: 3 handleappdiedlocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; ActivityManagerService.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">final</span> <span class="hljs-keyword">void</span> <span class="hljs-title">handleAppDiedLocked</span><span class="hljs-params">(ProcessRecord app,
        <span class="hljs-keyword">boolean</span> restarting, <span class="hljs-keyword">boolean</span> allowRestart)</span> </span>{
    <span class="hljs-keyword">int</span> pid = app.pid;
    <span class="hljs-comment">//清理应用程序service, BroadcastReceiver, ContentProvider相关信息【见小节4】</span>
    <span class="hljs-keyword">boolean</span> kept = cleanUpApplicationRecordLocked(app, restarting, allowRestart, -<span class="hljs-number">1</span>);

    <span class="hljs-keyword">if</span> (!kept &amp;&amp; !restarting) {
        removeLruProcessLocked(app);
        <span class="hljs-keyword">if</span> (pid &gt; <span class="hljs-number">0</span>) {
            ProcessList.remove(pid);
        }
    }

    <span class="hljs-comment">//清理activity相关信息</span>
    <span class="hljs-keyword">boolean</span> hasVisibleActivities = mStackSupervisor.handleAppDiedLocked(app);
    app.activities.clear();
    ...
    <span class="hljs-comment">//恢复栈顶第一个非finish的activity</span>
    <span class="hljs-keyword">if</span> (!restarting &amp;&amp; hasVisibleActivities &amp;&amp; !mStackSupervisor.resumeTopActivitiesLocked()) {
       mStackSupervisor.ensureActivitiesVisibleLocked(<span class="hljs-keyword">null</span>, <span class="hljs-number">0</span>);
   }
}
</code></pre></div></div>

<h3 id="4-cleanupapplicationrecordlocked">4 cleanUpApplicationRecordLocked<a class="anchorjs-link " href="#4-cleanupapplicationrecordlocked" aria-label="Anchor link for: 4 cleanupapplicationrecordlocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>该方法清理应用程序service, BroadcastReceiver, ContentProvider，process相关信息，为了便于说明将该方法划分为4个部分讲解</p>

<h4 id="41-清理service">4.1 清理service<a class="anchorjs-link " href="#41-清理service" aria-label="Anchor link for: 41 清理service" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<p>参数restarting = false,  allowRestart =true,  index =-1</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">final</span> <span class="hljs-keyword">boolean</span> <span class="hljs-title">cleanUpApplicationRecordLocked</span><span class="hljs-params">(ProcessRecord app,
        <span class="hljs-keyword">boolean</span> restarting, <span class="hljs-keyword">boolean</span> allowRestart, <span class="hljs-keyword">int</span> index)</span> </span>{
    ...
    mProcessesToGc.remove(app);
    mPendingPssProcesses.remove(app);

    <span class="hljs-comment">//如果存在，则清除crash/anr/wait对话框</span>
    <span class="hljs-keyword">if</span> (app.crashDialog != <span class="hljs-keyword">null</span> &amp;&amp; !app.forceCrashReport) {
        app.crashDialog.dismiss();
        app.crashDialog = <span class="hljs-keyword">null</span>;
    }
    <span class="hljs-keyword">if</span> (app.anrDialog != <span class="hljs-keyword">null</span>) {
        app.anrDialog.dismiss();
        app.anrDialog = <span class="hljs-keyword">null</span>;
    }
    <span class="hljs-keyword">if</span> (app.waitDialog != <span class="hljs-keyword">null</span>) {
        app.waitDialog.dismiss();
        app.waitDialog = <span class="hljs-keyword">null</span>;
    }

    app.crashing = <span class="hljs-keyword">false</span>;
    app.notResponding = <span class="hljs-keyword">false</span>;

    app.resetPackageList(mProcessStats);
    app.unlinkDeathRecipient(); <span class="hljs-comment">//解除app的死亡通告</span>
    app.makeInactive(mProcessStats);
    app.waitingToKill = <span class="hljs-keyword">null</span>;
    app.forcingToForeground = <span class="hljs-keyword">null</span>;
    <span class="hljs-comment">//将app移除前台进程</span>
    updateProcessForegroundLocked(app, <span class="hljs-keyword">false</span>, <span class="hljs-keyword">false</span>);
    app.foregroundActivities = <span class="hljs-keyword">false</span>;
    app.hasShownUi = <span class="hljs-keyword">false</span>;
    app.treatLikeActivity = <span class="hljs-keyword">false</span>;
    app.hasAboveClient = <span class="hljs-keyword">false</span>;
    app.hasClientActivities = <span class="hljs-keyword">false</span>;
    <span class="hljs-comment">//清理service信息，这个过程也比较复杂，后续再展开</span>
    mServices.killServicesLocked(app, allowRestart);
    <span class="hljs-keyword">boolean</span> restart = <span class="hljs-keyword">false</span>;
}
</code></pre></div></div>

<ul>
  <li>mProcessesToGc：记录着需要尽快执行gc的进程列表</li>
  <li>mPendingPssProcesses：记录着需要收集内存信息的进程列表</li>
</ul>

<h4 id="42-清理contentprovider">4.2 清理ContentProvider<a class="anchorjs-link " href="#42-清理contentprovider" aria-label="Anchor link for: 42 清理contentprovider" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">final</span> <span class="hljs-keyword">boolean</span> <span class="hljs-title">cleanUpApplicationRecordLocked</span><span class="hljs-params">(...)</span> </span>{
    ...
    <span class="hljs-keyword">for</span> (<span class="hljs-keyword">int</span> i = app.pubProviders.size() - <span class="hljs-number">1</span>; i &gt;= <span class="hljs-number">0</span>; i--) {
        <span class="hljs-comment">//获取该进程已发表的ContentProvider</span>
        ContentProviderRecord cpr = app.pubProviders.valueAt(i);
        <span class="hljs-keyword">final</span> <span class="hljs-keyword">boolean</span> always = app.bad || !allowRestart;
        <span class="hljs-comment">//ContentProvider服务端被杀，则client端进程也会被杀</span>
        <span class="hljs-keyword">boolean</span> inLaunching = removeDyingProviderLocked(app, cpr, always);
        <span class="hljs-keyword">if</span> ((inLaunching || always) &amp;&amp; cpr.hasConnectionOrHandle()) {
            restart = <span class="hljs-keyword">true</span>; <span class="hljs-comment">//需要重启</span>
        }

        cpr.provider = <span class="hljs-keyword">null</span>;
        cpr.proc = <span class="hljs-keyword">null</span>;
    }
    app.pubProviders.clear();

    <span class="hljs-comment">//处理正在启动并且是有client端正在等待的ContentProvider</span>
    <span class="hljs-keyword">if</span> (cleanupAppInLaunchingProvidersLocked(app, <span class="hljs-keyword">false</span>)) {
        restart = <span class="hljs-keyword">true</span>;
    }

    <span class="hljs-comment">//取消已连接的ContentProvider的注册</span>
    <span class="hljs-keyword">if</span> (!app.conProviders.isEmpty()) {
        <span class="hljs-keyword">for</span> (<span class="hljs-keyword">int</span> i = app.conProviders.size() - <span class="hljs-number">1</span>; i &gt;= <span class="hljs-number">0</span>; i--) {
            ContentProviderConnection conn = app.conProviders.get(i);
            conn.provider.connections.remove(conn);

            stopAssociationLocked(app.uid, app.processName, conn.provider.uid,
                    conn.provider.name);
        }
        app.conProviders.clear();
    }
</code></pre></div></div>

<h4 id="43-清理broadcastreceiver">4.3 清理BroadcastReceiver<a class="anchorjs-link " href="#43-清理broadcastreceiver" aria-label="Anchor link for: 43 清理broadcastreceiver" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">final</span> <span class="hljs-keyword">boolean</span> <span class="hljs-title">cleanUpApplicationRecordLocked</span><span class="hljs-params">(...)</span> </span>{
    ...
    skipCurrentReceiverLocked(app);

    <span class="hljs-comment">// 取消注册的广播接收者</span>
    <span class="hljs-keyword">for</span> (<span class="hljs-keyword">int</span> i = app.receivers.size() - <span class="hljs-number">1</span>; i &gt;= <span class="hljs-number">0</span>; i--) {
        removeReceiverLocked(app.receivers.valueAt(i));
    }
    app.receivers.clear();
}
</code></pre></div></div>

<h4 id="44-清理process">4.4 清理Process<a class="anchorjs-link " href="#44-清理process" aria-label="Anchor link for: 44 清理process" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">final</span> <span class="hljs-keyword">boolean</span> <span class="hljs-title">cleanUpApplicationRecordLocked</span><span class="hljs-params">(...)</span> </span>{
    ...
    <span class="hljs-comment">//当app正在备份时的处理方式</span>
    <span class="hljs-keyword">if</span> (mBackupTarget != <span class="hljs-keyword">null</span> &amp;&amp; app.pid == mBackupTarget.app.pid) {
        ...
        IBackupManager bm = IBackupManager.Stub.asInterface(
                ServiceManager.getService(Context.BACKUP_SERVICE));
        bm.agentDisconnected(app.info.packageName);
    }

    <span class="hljs-keyword">for</span> (<span class="hljs-keyword">int</span> i = mPendingProcessChanges.size() - <span class="hljs-number">1</span>; i &gt;= <span class="hljs-number">0</span>; i--) {
        ProcessChangeItem item = mPendingProcessChanges.get(i);
        <span class="hljs-keyword">if</span> (item.pid == app.pid) {
            mPendingProcessChanges.remove(i);
            mAvailProcessChanges.add(item);
        }
    }
    mUiHandler.obtainMessage(DISPATCH_PROCESS_DIED, app.pid, app.info.uid, <span class="hljs-keyword">null</span>).sendToTarget();

    <span class="hljs-keyword">if</span> (!app.persistent || app.isolated) {
        removeProcessNameLocked(app.processName, app.uid);
        <span class="hljs-keyword">if</span> (mHeavyWeightProcess == app) {
            mHandler.sendMessage(mHandler.obtainMessage(CANCEL_HEAVY_NOTIFICATION_MSG,
                    mHeavyWeightProcess.userId, <span class="hljs-number">0</span>));
            mHeavyWeightProcess = <span class="hljs-keyword">null</span>;
        }
    } <span class="hljs-keyword">else</span> <span class="hljs-keyword">if</span> (!app.removed) {
        <span class="hljs-comment">//对于persistent应用，则需要重启</span>
        <span class="hljs-keyword">if</span> (mPersistentStartingProcesses.indexOf(app) &lt; <span class="hljs-number">0</span>) {
            mPersistentStartingProcesses.add(app);
            restart = <span class="hljs-keyword">true</span>;
        }
    }

    <span class="hljs-comment">//mProcessesOnHold：记录着试图在系统ready之前就启动的进程。</span>
    <span class="hljs-comment">//在那时并不启动这些进程，先记录下来,等系统启动完成则启动这些进程。</span>
    mProcessesOnHold.remove(app);

    <span class="hljs-keyword">if</span> (app == mHomeProcess) {
        mHomeProcess = <span class="hljs-keyword">null</span>;
    }
    <span class="hljs-keyword">if</span> (app == mPreviousProcess) {
        mPreviousProcess = <span class="hljs-keyword">null</span>;
    }

    <span class="hljs-keyword">if</span> (restart &amp;&amp; !app.isolated) {
        <span class="hljs-comment">//仍有组件需要运行在该进程中，因此重启该进程</span>
        <span class="hljs-keyword">if</span> (index &lt; <span class="hljs-number">0</span>) {
            ProcessList.remove(app.pid);
        }
        addProcessNameLocked(app);
        startProcessLocked(app, <span class="hljs-string">"restart"</span>, app.processName);
        <span class="hljs-keyword">return</span> <span class="hljs-keyword">true</span>;
    } <span class="hljs-keyword">else</span> <span class="hljs-keyword">if</span> (app.pid &gt; <span class="hljs-number">0</span> &amp;&amp; app.pid != MY_PID) {
        <span class="hljs-comment">//移除该进程相关信息</span>
        <span class="hljs-keyword">boolean</span> removed;
        <span class="hljs-keyword">synchronized</span> (mPidsSelfLocked) {
            mPidsSelfLocked.remove(app.pid);
            mHandler.removeMessages(PROC_START_TIMEOUT_MSG, app);
        }
        app.setPid(<span class="hljs-number">0</span>);
    }
    <span class="hljs-keyword">return</span> <span class="hljs-keyword">false</span>;
}
</code></pre></div></div>

<p>对于需要重启进程的情形有：</p>

<ul>
  <li><code class="highlighter-rouge">mLaunchingProviders</code>：记录着存在client端等待的ContentProvider。应用当前正在启动中，当ContentProvider一旦发布则将该ContentProvider将从该list去除。当进程包含这样的ContentProvider，则需要重启进程。</li>
  <li><code class="highlighter-rouge">mPersistentStartingProcesses</code>：记录着试图在系统ready之前就启动的进程。在那时并不启动这些进程，先记录下来,等系统启动完成则启动这些进程。当进程属于这种类型也需要重启。</li>
</ul>

<h3 id="5-小结">5. 小结<a class="anchorjs-link " href="#5-小结" aria-label="Anchor link for: 5 小结" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>当crash进程执行kill操作后，进程被杀。此时需要掌握binder 死亡通知原理，由于Crash进程中拥有一个Binder服务端<code class="highlighter-rouge">ApplicationThread</code>，而应用进程在创建过程调用attachApplicationLocked()，从而attach到system_server进程，在system_server进程内有一个<code class="highlighter-rouge">ApplicationThreadProxy</code>，这是相对应的Binder客户端。当Binder服务端<code class="highlighter-rouge">ApplicationThread</code>所在进程(即Crash进程)挂掉后，则Binder客户端能收到相应的死亡通知，从而进入binderDied流程。更多关于bInder原理，这里就不细说，博客中有关于binder系列的专题。</p>

<p><img src="./images/binder_died.jpg" alt="binder_died"></p>

<h2 id="四-总结">四、 总结<a class="anchorjs-link " href="#四-总结" aria-label="Anchor link for: 四 总结" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h2>

<p>本文主要以源码的视角，详细介绍了到应用crash后系统的处理流程：</p>

<ol>
  <li>首先发生crash所在进程，在创建之初便准备好了defaultUncaughtHandler，用来来处理Uncaught Exception，并输出当前crash基本信息；</li>
  <li>调用当前进程中的AMP.handleApplicationCrash；经过binder ipc机制，传递到system_server进程；</li>
  <li>接下来，进入system_server进程，调用binder服务端执行AMS.handleApplicationCrash；</li>
  <li>从<code class="highlighter-rouge">mProcessNames</code>查找到目标进程的ProcessRecord对象；并将进程crash信息输出到目录<code class="highlighter-rouge">/data/system/dropbox</code>；</li>
  <li>执行makeAppCrashingLocked
    <ul>
      <li>创建当前用户下的crash应用的error receiver，并忽略当前应用的广播；</li>
      <li>停止当前进程中所有activity中的WMS的冻结屏幕消息，并执行相关一些屏幕相关操作；</li>
    </ul>
  </li>
  <li>再执行handleAppCrashLocked方法，
    <ul>
      <li>当1分钟内同一进程<code class="highlighter-rouge">连续crash两次</code>时，且<code class="highlighter-rouge">非persistent</code>进程，则直接结束该应用所有activity，并杀死该进程以及同一个进程组下的所有进程。然后再恢复栈顶第一个非finishing状态的activity;</li>
      <li>当1分钟内同一进程<code class="highlighter-rouge">连续crash两次</code>时，且<code class="highlighter-rouge">persistent</code>进程，，则只执行恢复栈顶第一个非finishing状态的activity;</li>
      <li>当1分钟内同一进程<code class="highlighter-rouge">未发生连续crash两次</code>时，则执行结束栈顶正在运行activity的流程。</li>
    </ul>
  </li>
  <li>通过mUiHandler发送消息<code class="highlighter-rouge">SHOW_ERROR_MSG</code>，弹出crash对话框；</li>
  <li>到此，system_server进程执行完成。回到crash进程开始执行杀掉当前进程的操作；</li>
  <li>当crash进程被杀，通过binder死亡通知，告知system_server进程来执行appDiedLocked()；</li>
  <li>最后，执行清理应用相关的activity/service/ContentProvider/receiver组件信息。</li>
</ol>

<p>这基本就是整个应用Crash后系统的执行过程。 最后，再说说对于同一个app连续crash的情况：</p>

<ul>
  <li>当60s内连续crash两次的非persistent进程时，被认定为bad进程：那么如果第3次从后台启动该进程(Intent.getFlags来判断)，则会拒绝创建进程；</li>
  <li>当crash次数达到两次的非persistent进程时，则再次杀该进程，即便允许自启的service也会在被杀后拒绝再次启动。</li>
</ul>
