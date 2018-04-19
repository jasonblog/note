# 理解Android Crash處理流程


<div class="
                col-lg-8 col-lg-offset-2
                col-md-10 col-md-offset-1
                post-container">

								<blockquote>
  <p>基於Android 6.0的源碼剖析， 分析Android應用Crash是如何處理的。</p>
</blockquote>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cs">/frameworks/<span class="hljs-keyword">base</span>/core/java/com/android/<span class="hljs-keyword">internal</span>/os/RuntimeInit.java
/frameworks/<span class="hljs-keyword">base</span>/core/java/android/app/ActivityManagerNative.java (含內部類AMP)
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

<p>App crash(全稱Application crash), 對於Crash可分為native crash和framework crash(包含app crash在內)，對於crash相信很多app開發者都會遇到，那麼上層什麼時候會出現crash呢，系統又是如何處理crash的呢。例如，在app大家經常使用<code class="highlighter-rouge">try...catch</code>語句，那麼如果沒有有效catch exception，就是導致應用crash，發生沒有catch exception，系統便會來進行捕獲，並進入crash流程。如果你是從事Android系統開發或者架構相關工作，或者遇到需要解系統性的疑難雜症，那麼很有必要了解系統Crash處理流程，知其然還需知其所以然；如果你僅僅是App初級開發，可能本文並非很適合閱讀，整個系統流程錯綜複雜。</p>

<p>在Android系統啟動系列文章，已講述過上層應用都是由Zygote fork孵化而來，分為system_server系統進程和各種應用進程，在這些進程創建之初會設置未捕獲異常的處理器，當系統拋出未捕獲的異常時，最終都交給異常處理器。</p>

<ul>
  <li>對於system_server進程：文章<a href="http://gityuan.com/2016/02/14/android-system-server/#commonInit">Android系統啟動-SystemServer上篇</a>，system_server啟動過程中由RuntimeInit.java的<code class="highlighter-rouge">commonInit</code>方法設置UncaughtHandler，用於處理未捕獲異常；</li>
  <li>對於普通應用進程：文章<a href="http://gityuan.com/2016/03/26/app-process-create/#commoninit">理解Android進程創建流程</a> ，進程創建過程中，同樣會調用RuntimeInit.java的<code class="highlighter-rouge">commonInit</code>方法設置UncaughtHandler。</li>
</ul>

<h3 id="11-crash調用鏈">1.1 crash調用鏈<a class="anchorjs-link " href="#11-crash調用鏈" aria-label="Anchor link for: 11 crash調用鏈" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>crash流程的方法調用關係來結尾：</p>

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

<p>接下來說說這個過程。</p>

<h2 id="二crash處理流程">二、Crash處理流程<a class="anchorjs-link " href="#二crash處理流程" aria-label="Anchor link for: 二crash處理流程" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h2>

<p>那麼接下來以commonInit()方法為起點來展開說明。</p>

<h3 id="1-runtimeinitcommoninit">1. RuntimeInit.commonInit<a class="anchorjs-link " href="#1-runtimeinitcommoninit" aria-label="Anchor link for: 1 runtimeinitcommoninit" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-keyword">public</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">RuntimeInit</span> </span>{
    ...
    <span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">static</span> <span class="hljs-keyword">final</span> <span class="hljs-keyword">void</span> <span class="hljs-title">commonInit</span><span class="hljs-params">()</span> </span>{
        <span class="hljs-comment">//設置默認的未捕獲異常處理器，UncaughtHandler實例化過程【見小節2】</span>
        Thread.setDefaultUncaughtExceptionHandler(<span class="hljs-keyword">new</span> UncaughtHandler());
        ...
    }
}
</code></pre></div></div>

<p>setDefaultUncaughtExceptionHandler()只是將異常處理器handler對象賦給Thread成員變量,即<code class="highlighter-rouge">Thread.defaultUncaughtHandler = new UncaughtHandler()</code>。接下來看看UncaughtHandler對象實例化過程。</p>

<h3 id="2-uncaughthandler">2. UncaughtHandler<a class="anchorjs-link " href="#2-uncaughthandler" aria-label="Anchor link for: 2 uncaughthandler" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[–&gt;RuntimeInit.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-keyword">private</span> <span class="hljs-keyword">static</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">UncaughtHandler</span> <span class="hljs-keyword">implements</span> <span class="hljs-title">Thread</span>.<span class="hljs-title">UncaughtExceptionHandler</span> </span>{
    <span class="hljs-comment">//覆寫接口方法</span>
    <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">uncaughtException</span><span class="hljs-params">(Thread t, Throwable e)</span> </span>{
        <span class="hljs-keyword">try</span> {
            <span class="hljs-comment">//保證crash處理過程不會重入</span>
            <span class="hljs-keyword">if</span> (mCrashing) <span class="hljs-keyword">return</span>;
            mCrashing = <span class="hljs-keyword">true</span>;

            <span class="hljs-keyword">if</span> (mApplicationObject == <span class="hljs-keyword">null</span>) {
                <span class="hljs-comment">//system_server進程</span>
                Clog_e(TAG, <span class="hljs-string">"*** FATAL EXCEPTION IN SYSTEM PROCESS: "</span> + t.getName(), e);
            } <span class="hljs-keyword">else</span> {
                <span class="hljs-comment">//普通應用進程</span>
                StringBuilder message = <span class="hljs-keyword">new</span> StringBuilder();
                message.append(<span class="hljs-string">"FATAL EXCEPTION: "</span>).append(t.getName()).append(<span class="hljs-string">"\n"</span>);
                <span class="hljs-keyword">final</span> String processName = ActivityThread.currentProcessName();
                <span class="hljs-keyword">if</span> (processName != <span class="hljs-keyword">null</span>) {
                    message.append(<span class="hljs-string">"Process: "</span>).append(processName).append(<span class="hljs-string">", "</span>);
                }
                message.append(<span class="hljs-string">"PID: "</span>).append(Process.myPid());
                Clog_e(TAG, message.toString(), e);
            }

            <span class="hljs-comment">//啟動crash對話框，等待處理完成 【見小節2.1和3】</span>
            ActivityManagerNative.getDefault().handleApplicationCrash(
                    mApplicationObject, <span class="hljs-keyword">new</span> ApplicationErrorReport.CrashInfo(e));
        } <span class="hljs-keyword">catch</span> (Throwable t2) {
            ...
        } <span class="hljs-keyword">finally</span> {
            <span class="hljs-comment">//確保當前進程徹底殺掉【見小節11】</span>
            Process.killProcess(Process.myPid());
            System.exit(<span class="hljs-number">10</span>);
        }
    }
}
</code></pre></div></div>

<ol>
  <li>當system進程crash的信息：
    <ul>
      <li>開頭<code class="highlighter-rouge">*** FATAL EXCEPTION IN SYSTEM PROCESS [線程名]</code>；</li>
      <li>接著輸出發生crash時的調用棧信息；</li>
    </ul>
  </li>
  <li>當app進程crash時的信息：
    <ul>
      <li>開頭<code class="highlighter-rouge">FATAL EXCEPTION: [線程名]</code>；</li>
      <li>緊接著 <code class="highlighter-rouge">Process: [進程名], PID: [進程id]</code>；</li>
      <li>最後輸出發生crash時的調用棧信息。</li>
    </ul>
  </li>
</ol>

<p>看到這裡，你就會發現要從log中搜索crash信息，只需要搜索關鍵詞<code class="highlighter-rouge">FATAL EXCEPTION</code>；如果需要進一步篩選只搜索系統crash信息，則可以搜索的關鍵詞可以有多樣，比如<code class="highlighter-rouge">*** FATAL EXCEPTION</code>。</p>

<p>當輸出完crash信息到logcat裡面，這只是crash流程的剛開始階段，接下來彈出<code class="highlighter-rouge">crash對話框</code>，ActivityManagerNative.getDefault()返回的是ActivityManagerProxy（簡稱<code class="highlighter-rouge">AMP</code>），<code class="highlighter-rouge">AMP</code>經過binder調用最終交給ActivityManagerService(簡稱<code class="highlighter-rouge">AMS</code>)中相應的方法去處理，故接下來調用的是AMS.handleApplicationCrash()。</p>

<p><strong>注意</strong>: mApplicationObject等於null,一定不是普通的app進程. 但是除了system進程, 也有可能是shell進程, 即通過app_process + 命令參數 的方式創建的進程.</p>

<h4 id="21-crashinfo">2.1 CrashInfo<a class="anchorjs-link " href="#21-crashinfo" aria-label="Anchor link for: 21 crashinfo" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<p>[-&gt; ApplicationErrorReport.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-keyword">public</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">ApplicationErrorReport</span> <span class="hljs-keyword">implements</span> <span class="hljs-title">Parcelable</span> </span>{
    ...
    <span class="hljs-keyword">public</span> <span class="hljs-keyword">static</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">CrashInfo</span> </span>{
        <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-title">CrashInfo</span><span class="hljs-params">(Throwable tr)</span> </span>{
            StringWriter sw = <span class="hljs-keyword">new</span> StringWriter();
            PrintWriter pw = <span class="hljs-keyword">new</span> FastPrintWriter(sw, <span class="hljs-keyword">false</span>, <span class="hljs-number">256</span>);
            tr.printStackTrace(pw); <span class="hljs-comment">//輸出棧trace</span>
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

<p>將crash信息<code class="highlighter-rouge">文件名</code>，<code class="highlighter-rouge">類名</code>，<code class="highlighter-rouge">方法名</code>，<code class="highlighter-rouge">對應行號</code>以及<code class="highlighter-rouge">異常信息</code>都封裝到CrashInfo對象。</p>

<h3 id="3-handleapplicationcrash">3. handleApplicationCrash<a class="anchorjs-link " href="#3-handleapplicationcrash" aria-label="Anchor link for: 3 handleapplicationcrash" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[–&gt;ActivityManagerService.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">handleApplicationCrash</span><span class="hljs-params">(IBinder app, ApplicationErrorReport.CrashInfo crashInfo)</span> </span>{
    <span class="hljs-comment">//獲取進程record對象【見小節3.1】</span>
    ProcessRecord r = findAppProcess(app, <span class="hljs-string">"Crash"</span>);
    <span class="hljs-keyword">final</span> String processName = app == <span class="hljs-keyword">null</span> ? <span class="hljs-string">"system_server"</span>
            : (r == <span class="hljs-keyword">null</span> ? <span class="hljs-string">"unknown"</span> : r.processName);
    <span class="hljs-comment">//【見小節4】</span>
    handleApplicationCrashInner(<span class="hljs-string">"crash"</span>, r, processName, crashInfo);
}
</code></pre></div></div>

<p>關於進程名(processName)：</p>

<ol>
  <li>當遠程IBinder對象為空時，則進程名為<code class="highlighter-rouge">system_server</code>；</li>
  <li>當遠程IBinder對象不為空，且ProcessRecord為空時，則進程名為<code class="highlighter-rouge">unknown</code>;</li>
  <li>當遠程IBinder對象不為空，且ProcessRecord不為空時，則進程名為ProcessRecord對象中相應進程名。</li>
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
                <span class="hljs-comment">//當找到目標進程則返回</span>
                <span class="hljs-keyword">if</span> (p.thread != <span class="hljs-keyword">null</span> &amp;&amp; p.thread.asBinder() == app) {
                    <span class="hljs-keyword">return</span> p;
                }
            }
        }
        <span class="hljs-comment">//如果代碼執行到這裡，表明無法找到應用所在的進程</span>
        <span class="hljs-keyword">return</span> <span class="hljs-keyword">null</span>;
    }
}
</code></pre></div></div>

<p>其中 <code class="highlighter-rouge">mProcessNames = new ProcessMap&lt;ProcessRecord&gt;();</code>對於代碼<code class="highlighter-rouge">mProcessNames.getMap()</code>返回的是<code class="highlighter-rouge">mMap</code>，而<code class="highlighter-rouge">mMap= new ArrayMap&lt;String, SparseArray&lt;ProcessRecord&gt;&gt;()</code>;</p>

<p><strong>知識延伸：</strong><code class="highlighter-rouge">SparseArray</code>和<code class="highlighter-rouge">ArrayMap</code>是Android專門針對內存優化而設計的取代Java API中的<code class="highlighter-rouge">HashMap</code>的數據結構。對於key是int類型則使用<code class="highlighter-rouge">SparseArray</code>，可避免自動裝箱過程；對於key為其他類型則使用<code class="highlighter-rouge">ArrayMap</code>。<code class="highlighter-rouge">HashMap</code>的查找和插入時間複雜度為O(1)的代價是犧牲大量的內存來實現的，而<code class="highlighter-rouge">SparseArray</code>和<code class="highlighter-rouge">ArrayMap</code>性能略遜於<code class="highlighter-rouge">HashMap</code>，但更節省內存。</p>

<p>再回到<code class="highlighter-rouge">mMap</code>，這是以進程name為key，再以(uid為key，以ProcessRecord為Value的)結構體作為value。下面看看其get()和put()方法</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cs"><span class="hljs-comment">//獲取mMap中(name,uid)所對應的ProcessRecord</span>
<span class="hljs-function"><span class="hljs-keyword">public</span> ProcessRecord <span class="hljs-title">get</span>(<span class="hljs-params">String name, <span class="hljs-keyword">int</span> uid</span>) </span>{}；
<span class="hljs-comment">//將(name,uid, value)添加到mMap</span>
<span class="hljs-function"><span class="hljs-keyword">public</span> ProcessRecord <span class="hljs-title">put</span>(<span class="hljs-params">String name, <span class="hljs-keyword">int</span> uid, ProcessRecord <span class="hljs-keyword">value</span></span>) </span>{}；
</code></pre></div></div>

<p>findAppProcess()根據app(IBinder類型)來查詢相應的目標對象ProcessRecord。</p>

<p>有了進程記錄對象ProcessRecord和進程名processName，則進入執行Crash處理方法，繼續往下看。</p>

<h3 id="4-handleapplicationcrashinner">4. handleApplicationCrashInner<a class="anchorjs-link " href="#4-handleapplicationcrashinner" aria-label="Anchor link for: 4 handleapplicationcrashinner" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[–&gt;ActivityManagerService.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs javascript"><span class="hljs-keyword">void</span> handleApplicationCrashInner(<span class="hljs-built_in">String</span> eventType, ProcessRecord r, <span class="hljs-built_in">String</span> processName,
        ApplicationErrorReport.CrashInfo crashInfo) {
    <span class="hljs-comment">//將Crash信息寫入到Event log</span>
    EventLog.writeEvent(EventLogTags.AM_CRASH,...);
    <span class="hljs-comment">//將錯誤信息添加到DropBox</span>
    addErrorToDropBox(eventType, r, processName, <span class="hljs-literal">null</span>, <span class="hljs-literal">null</span>, <span class="hljs-literal">null</span>, <span class="hljs-literal">null</span>, <span class="hljs-literal">null</span>, crashInfo);
    <span class="hljs-comment">//【見小節5】</span>
    crashApplication(r, crashInfo);
}
</code></pre></div></div>

<p>其中<a href="http://gityuan.com/2016/06/12/DropBoxManagerService/#amsadderrortodropbox">addErrorToDropBox</a>是將crash的信息輸出到目錄<code class="highlighter-rouge">/data/system/dropbox</code>。例如system_server的dropbox文件名為system_server_crash@xxx.txt (xxx代表的是時間戳)</p>

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
        <span class="hljs-comment">// 當存在ActivityController的情況,比如monkey</span>
        <span class="hljs-keyword">if</span> (mController != <span class="hljs-keyword">null</span>) {
            <span class="hljs-keyword">try</span> {
                String name = r != <span class="hljs-keyword">null</span> ? r.processName : <span class="hljs-keyword">null</span>;
                <span class="hljs-keyword">int</span> pid = r != <span class="hljs-keyword">null</span> ? r.pid : Binder.getCallingPid();
                <span class="hljs-keyword">int</span> uid = r != <span class="hljs-keyword">null</span> ? r.info.uid : Binder.getCallingUid();
                <span class="hljs-comment">//調用monkey的appCrashed</span>
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
        
        <span class="hljs-comment">//清除遠程調用者uid和pid信息，並保存到origId</span>
        <span class="hljs-keyword">final</span> <span class="hljs-keyword">long</span> origId = Binder.clearCallingIdentity();
        ...

        <span class="hljs-comment">//【見小節6】</span>
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
        <span class="hljs-comment">//發送消息SHOW_ERROR_MSG，彈出提示crash的對話框，等待用戶選擇【見小節10】</span>
        mUiHandler.sendMessage(msg);
        <span class="hljs-comment">//恢復遠程調用者uid和pid</span>
        Binder.restoreCallingIdentity(origId);
    }

    <span class="hljs-comment">//進入阻塞等待，直到用戶選擇crash對話框"退出"或者"退出並報告"</span>
    <span class="hljs-keyword">int</span> res = result.get();

    Intent appErrorIntent = <span class="hljs-keyword">null</span>;
    <span class="hljs-keyword">synchronized</span> (<span class="hljs-keyword">this</span>) {
        <span class="hljs-keyword">if</span> (r != <span class="hljs-keyword">null</span> &amp;&amp; !r.isolated) {
            <span class="hljs-comment">// 將崩潰的進程信息保存到mProcessCrashTimes</span>
            mProcessCrashTimes.put(r.info.processName, r.uid,
                    SystemClock.uptimeMillis());
        }
        <span class="hljs-keyword">if</span> (res == AppErrorDialog.FORCE_QUIT_AND_REPORT) {
            <span class="hljs-comment">//創建action="android.intent.action.APP_ERROR"，組件為r.errorReportReceiver的Intent</span>
            appErrorIntent = createAppErrorIntentLocked(r, timeMillis, crashInfo);
        }
    }

    <span class="hljs-keyword">if</span> (appErrorIntent != <span class="hljs-keyword">null</span>) {
        <span class="hljs-keyword">try</span> {
            <span class="hljs-comment">//啟動Intent為appErrorIntent的Activity</span>
            mContext.startActivityAsUser(appErrorIntent, <span class="hljs-keyword">new</span> UserHandle(r.userId));
        } <span class="hljs-keyword">catch</span> (ActivityNotFoundException e) {
            Slog.w(TAG, <span class="hljs-string">"bug report receiver dissappeared"</span>, e);
        }
    }
}
</code></pre></div></div>

<p>該方法主要做的兩件事：</p>

<ul>
  <li>調用<code class="highlighter-rouge">makeAppCrashingLocked</code>，繼續處理crash流程；</li>
  <li>發送消息SHOW_ERROR_MSG，彈出提示crash的對話框，等待用戶選擇；</li>
</ul>

<h3 id="6-makeappcrashinglocked">6. makeAppCrashingLocked<a class="anchorjs-link " href="#6-makeappcrashinglocked" aria-label="Anchor link for: 6 makeappcrashinglocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[–&gt;ActivityManagerService.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">boolean</span> <span class="hljs-title">makeAppCrashingLocked</span><span class="hljs-params">(ProcessRecord app,
        String shortMsg, String longMsg, String stackTrace)</span> </span>{
    app.crashing = <span class="hljs-keyword">true</span>;
    <span class="hljs-comment">//封裝crash信息到crashingReport對象</span>
    app.crashingReport = generateProcessError(app,
            ActivityManager.ProcessErrorStateInfo.CRASHED, <span class="hljs-keyword">null</span>, shortMsg, longMsg, stackTrace);
    <span class="hljs-comment">//【見小節7】</span>
    startAppProblemLocked(app);
    <span class="hljs-comment">//停止屏幕凍結【見小節8】</span>
    app.stopFreezingAllLocked();
    <span class="hljs-comment">//【見小節9】</span>
    <span class="hljs-keyword">return</span> handleAppCrashLocked(app, <span class="hljs-string">"force-crash"</span>, shortMsg, longMsg, stackTrace);
}
</code></pre></div></div>

<h3 id="7-startappproblemlocked">7. startAppProblemLocked<a class="anchorjs-link " href="#7-startappproblemlocked" aria-label="Anchor link for: 7 startappproblemlocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[–&gt;ActivityManagerService.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">startAppProblemLocked</span><span class="hljs-params">(ProcessRecord app)</span> </span>{
    app.errorReportReceiver = <span class="hljs-keyword">null</span>;

    <span class="hljs-keyword">for</span> (<span class="hljs-keyword">int</span> userId : mCurrentProfileIds) {
        <span class="hljs-keyword">if</span> (app.userId == userId) {
            <span class="hljs-comment">//獲取當前用戶下的crash應用的error receiver【見小節7.1】</span>
            app.errorReportReceiver = ApplicationErrorReport.getErrorReportReceiver(
                    mContext, app.info.packageName, app.info.flags);
        }
    }
    <span class="hljs-comment">//忽略當前app的廣播接收【見小節7.2】</span>
    skipCurrentReceiverLocked(app);
}
</code></pre></div></div>

<p>該方法主要功能：</p>

<ul>
  <li>獲取當前用戶下的crash應用的error receiver；</li>
  <li>忽略當前app的廣播接收；</li>
</ul>

<h4 id="71-geterrorreportreceiver">7.1 getErrorReportReceiver<a class="anchorjs-link " href="#71-geterrorreportreceiver" aria-label="Anchor link for: 71 geterrorreportreceiver" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<p>[-&gt; ApplicationErrorReport.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cs"><span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">static</span> ComponentName <span class="hljs-title">getErrorReportReceiver</span>(<span class="hljs-params">Context context,
        String packageName, <span class="hljs-keyword">int</span> appFlags</span>) </span>{
    <span class="hljs-comment">//檢查Settings中的"send_action_app_error"是否使能錯誤報告的功能</span>
    <span class="hljs-keyword">int</span> enabled = Settings.Global.getInt(context.getContentResolver(),
            Settings.Global.SEND_ACTION_APP_ERROR, <span class="hljs-number">0</span>);
    <span class="hljs-keyword">if</span> (enabled == <span class="hljs-number">0</span>) {
        <span class="hljs-comment">//1.當未使能時，則直接返回</span>
        <span class="hljs-keyword">return</span> <span class="hljs-literal">null</span>;
    }

    PackageManager pm = context.getPackageManager();

    String candidate = <span class="hljs-literal">null</span>;
    ComponentName result = <span class="hljs-literal">null</span>;
    <span class="hljs-keyword">try</span> {
        <span class="hljs-comment">//獲取該crash應用的安裝器的包名</span>
        candidate = pm.getInstallerPackageName(packageName);
    } <span class="hljs-keyword">catch</span> (IllegalArgumentException e) {
    }

    <span class="hljs-keyword">if</span> (candidate != <span class="hljs-literal">null</span>) {
        result = getErrorReportReceiver(pm, packageName, candidate);<span class="hljs-comment">//【見下文】</span>
        <span class="hljs-keyword">if</span> (result != <span class="hljs-literal">null</span>) {
            <span class="hljs-comment">//2.當找到該crash應用的安裝器，則返回；</span>
            <span class="hljs-keyword">return</span> result;
        }
    }

    <span class="hljs-keyword">if</span> ((appFlags&amp;ApplicationInfo.FLAG_SYSTEM) != <span class="hljs-number">0</span>) {
        <span class="hljs-comment">//該系統屬性名為"ro.error.receiver.system.apps"</span>
        candidate = SystemProperties.<span class="hljs-keyword">get</span>(SYSTEM_APPS_ERROR_RECEIVER_PROPERTY);
        result = getErrorReportReceiver(pm, packageName, candidate);<span class="hljs-comment">//【見下文】</span>
        <span class="hljs-keyword">if</span> (result != <span class="hljs-literal">null</span>) {
            <span class="hljs-comment">//3.當crash應用是系統應用時，且系統屬性指定error receiver時，則返回；</span>
            <span class="hljs-keyword">return</span> result;
        }
    }

    <span class="hljs-comment">//該默認屬性名為"ro.error.receiver.default"</span>
    candidate = SystemProperties.<span class="hljs-keyword">get</span>(DEFAULT_ERROR_RECEIVER_PROPERTY);
    <span class="hljs-comment">//4.當默認屬性值指定error receiver時，則返回；</span>
    <span class="hljs-keyword">return</span> getErrorReportReceiver(pm, packageName, candidate); <span class="hljs-comment">//【見下文】</span>
}
</code></pre></div></div>

<p><strong>getErrorReportReceiver</strong>：這是同名不同輸入參數的另一個方法：</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs javascript"><span class="hljs-keyword">static</span> ComponentName getErrorReportReceiver(PackageManager pm, <span class="hljs-built_in">String</span> errorPackage,
        <span class="hljs-built_in">String</span> receiverPackage) {
    <span class="hljs-keyword">if</span> (receiverPackage == <span class="hljs-literal">null</span> || receiverPackage.length() == <span class="hljs-number">0</span>) {
        <span class="hljs-keyword">return</span> <span class="hljs-literal">null</span>;
    }

    <span class="hljs-comment">//當安裝應用程序的安裝器Crash，則直接返回</span>
    <span class="hljs-keyword">if</span> (receiverPackage.equals(errorPackage)) {
        <span class="hljs-keyword">return</span> <span class="hljs-literal">null</span>;
    }

    <span class="hljs-comment">//ACTION_APP_ERROR值為"android.intent.action.APP_ERROR"</span>
    Intent intent = <span class="hljs-keyword">new</span> Intent(Intent.ACTION_APP_ERROR);
    intent.setPackage(receiverPackage);
    ResolveInfo info = pm.resolveActivity(intent, <span class="hljs-number">0</span>);
    <span class="hljs-keyword">if</span> (info == <span class="hljs-literal">null</span> || info.activityInfo == <span class="hljs-literal">null</span>) {
        <span class="hljs-keyword">return</span> <span class="hljs-literal">null</span>;
    }
    <span class="hljs-comment">//創建包名為receiverPackage的組件</span>
    <span class="hljs-keyword">return</span> <span class="hljs-keyword">new</span> ComponentName(receiverPackage, info.activityInfo.name);
}
</code></pre></div></div>

<h4 id="72-skipcurrentreceiverlocked">7.2 skipCurrentReceiverLocked<a class="anchorjs-link " href="#72-skipcurrentreceiverlocked" aria-label="Anchor link for: 72 skipcurrentreceiverlocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<p>[–&gt;ActivityManagerService.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">skipCurrentReceiverLocked</span><span class="hljs-params">(ProcessRecord app)</span> </span>{
    <span class="hljs-keyword">for</span> (BroadcastQueue <span class="hljs-built_in">queue</span> : mBroadcastQueues) {
        <span class="hljs-built_in">queue</span>.skipCurrentReceiverLocked(app);  <span class="hljs-comment">//【見小節7.2.1】</span>
    }
}
</code></pre></div></div>

<h5 id="721--skipcurrentreceiverlocked">7.2.1  skipCurrentReceiverLocked<a class="anchorjs-link " href="#721--skipcurrentreceiverlocked" aria-label="Anchor link for: 721  skipcurrentreceiverlocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h5>

<p>[-&gt; BroadcastQueue.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cs"><span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">skipCurrentReceiverLocked</span>(<span class="hljs-params">ProcessRecord app</span>) </span>{
    BroadcastRecord r = <span class="hljs-literal">null</span>;
    <span class="hljs-comment">//查看app進程中的廣播</span>
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
        <span class="hljs-comment">//結束app進程的廣播結束</span>
        finishReceiverLocked(r, r.resultCode, r.resultData,
                r.resultExtras, r.resultAbort, <span class="hljs-literal">false</span>);
        <span class="hljs-comment">//廣播調度</span>
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
        activities.<span class="hljs-keyword">get</span>(i).stopFreezingScreenLocked(<span class="hljs-literal">true</span>); <span class="hljs-comment">//【見小節8.1】</span>
    }
}
</code></pre></div></div>

<p>其中activities類型為ArrayList<code class="highlighter-rouge">&lt;ActivityRecord</code>&gt;，停止進程裡所有的Activity</p>

<h4 id="81-arstopfreezingscreenlocked">8.1. AR.stopFreezingScreenLocked<a class="anchorjs-link " href="#81-arstopfreezingscreenlocked" aria-label="Anchor link for: 81 arstopfreezingscreenlocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<p>[-&gt; ActivityRecord.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">stopFreezingScreenLocked</span><span class="hljs-params">(<span class="hljs-keyword">boolean</span> force)</span> </span>{
    <span class="hljs-keyword">if</span> (force || frozenBeforeDestroy) {
        frozenBeforeDestroy = <span class="hljs-keyword">false</span>;
        <span class="hljs-comment">//mWindowManager類型為WMS //【見小節8.1.1】</span>
        service.mWindowManager.stopAppFreezingScreen(appToken, force);
    }
}
</code></pre></div></div>

<p>其中appToken是IApplication.Stub類型，即WindowManager的token。</p>

<h5 id="811-wmsstopfreezingscreenlocked">8.1.1 WMS.stopFreezingScreenLocked<a class="anchorjs-link " href="#811-wmsstopfreezingscreenlocked" aria-label="Anchor link for: 811 wmsstopfreezingscreenlocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h5>

<p>[-&gt; WindowManagerService.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-meta">@Override</span>
<span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">stopFreezingScreen</span><span class="hljs-params">()</span> </span>{
    <span class="hljs-comment">//權限檢查</span>
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
                stopFreezingDisplayLocked(); <span class="hljs-comment">//【見流程8.1.1.1】</span>
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
        <span class="hljs-keyword">return</span>; <span class="hljs-comment">//顯示沒有凍結，則直接返回</span>
    }

    <span class="hljs-comment">//往往跟屏幕旋轉相關</span>
    ...

    mDisplayFrozen = <span class="hljs-keyword">false</span>;
    <span class="hljs-comment">//從上次凍屏到現在的總時長</span>
    mLastDisplayFreezeDuration = (<span class="hljs-keyword">int</span>)(SystemClock.elapsedRealtime() - mDisplayFreezeTime);

    <span class="hljs-comment">//移除凍屏的超時消息</span>
    mH.removeMessages(H.APP_FREEZE_TIMEOUT);
    mH.removeMessages(H.CLIENT_FREEZE_TIMEOUT);

    <span class="hljs-keyword">boolean</span> updateRotation = <span class="hljs-keyword">false</span>;
    <span class="hljs-comment">//獲取默認的DisplayContent</span>
    <span class="hljs-keyword">final</span> DisplayContent displayContent = getDefaultDisplayContentLocked();
    <span class="hljs-keyword">final</span> <span class="hljs-keyword">int</span> displayId = displayContent.getDisplayId();
    ScreenRotationAnimation screenRotationAnimation =
            mAnimator.getScreenRotationAnimationLocked(displayId);

    <span class="hljs-comment">//屏幕旋轉動畫的相關操作</span>
    <span class="hljs-keyword">if</span> (CUSTOM_SCREEN_ROTATION &amp;&amp; screenRotationAnimation != <span class="hljs-keyword">null</span>
            &amp;&amp; screenRotationAnimation.hasScreenshot()) {
        DisplayInfo displayInfo = displayContent.getDisplayInfo();
        <span class="hljs-keyword">boolean</span> isDimming = displayContent.isDimming();
        <span class="hljs-keyword">if</span> (!mPolicy.validateRotationAnimationLw(mExitAnimId, mEnterAnimId, isDimming)) {
            mExitAnimId = mEnterAnimId = <span class="hljs-number">0</span>;
        }
        <span class="hljs-comment">//加載動畫最大時長為10s</span>
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
    <span class="hljs-comment">//經過層層調用到InputManagerService服務，IMS服務使能輸入事件分發功能</span>
    mInputMonitor.thawInputDispatchingLw();

    <span class="hljs-keyword">boolean</span> configChanged;
    <span class="hljs-comment">//當display被凍結時不再計算屏幕方向，以避免不連續的狀態。</span>
    configChanged = updateOrientationFromAppTokensLocked(<span class="hljs-keyword">false</span>);

    <span class="hljs-comment">//display凍結時，執行gc操作</span>
    mH.removeMessages(H.FORCE_GC);
    mH.sendEmptyMessageDelayed(H.FORCE_GC, <span class="hljs-number">2000</span>);

    <span class="hljs-comment">//mScreenFrozenLock的類型為PowerManager.WakeLock，即釋放屏幕凍結的鎖</span>
    mScreenFrozenLock.release();

    <span class="hljs-keyword">if</span> (updateRotation) {
        <span class="hljs-comment">//更新當前的屏幕方向</span>
        configChanged |= updateRotationUncheckedLocked(<span class="hljs-keyword">false</span>);
    }

    <span class="hljs-keyword">if</span> (configChanged) {
        <span class="hljs-comment">//向mH發送configuraion改變的消息</span>
        mH.sendEmptyMessage(H.SEND_NEW_CONFIGURATION);
    }
}
</code></pre></div></div>

<p>該方法主要功能：</p>

<ol>
  <li>處理屏幕旋轉相關邏輯；</li>
  <li>移除凍屏的超時消息；</li>
  <li>屏幕旋轉動畫的相關操作;</li>
  <li>使能輸入事件分發功能；</li>
  <li>display凍結時，執行gc操作；</li>
  <li>更新當前的屏幕方向；</li>
  <li>向mH發送configuraion改變的消息。</li>
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
    <span class="hljs-comment">//當同一個進程，連續兩次crash的時間間隔小於1分鐘時，則認為crash太過於頻繁</span>
    <span class="hljs-keyword">if</span> (crashTime != <span class="hljs-keyword">null</span> &amp;&amp; now &lt; crashTime+ProcessList.MIN_CRASH_INTERVAL) {
        EventLog.writeEvent(EventLogTags.AM_PROCESS_CRASHED_TOO_MUCH,
                app.userId, app.info.processName, app.uid);
        <span class="hljs-comment">//【見小節9.1】</span>
        mStackSupervisor.handleAppCrashLocked(app);
        <span class="hljs-keyword">if</span> (!app.persistent) {
            <span class="hljs-comment">//不再重啟非persistent進程，除非用戶顯式地調用</span>
            EventLog.writeEvent(EventLogTags.AM_PROC_BAD, app.userId, app.uid,
                    app.info.processName);
            <span class="hljs-keyword">if</span> (!app.isolated) {
                <span class="hljs-comment">//將當前app加入到mBadProcesses</span>
                mBadProcesses.put(app.info.processName, app.uid,
                        <span class="hljs-keyword">new</span> BadProcessInfo(now, shortMsg, longMsg, stackTrace));
                mProcessCrashTimes.remove(app.info.processName, app.uid);
            }
            app.bad = <span class="hljs-keyword">true</span>;
            app.removed = <span class="hljs-keyword">true</span>;
            <span class="hljs-comment">//移除進程的所有服務，保證不再重啟【見小節9.2】</span>
            removeProcessLocked(app, <span class="hljs-keyword">false</span>, <span class="hljs-keyword">false</span>, <span class="hljs-string">"crash"</span>);
            <span class="hljs-comment">//恢復最頂部的Activity【見小節9.3】</span>
            mStackSupervisor.resumeTopActivitiesLocked();
            <span class="hljs-keyword">return</span> <span class="hljs-keyword">false</span>;
        }
        mStackSupervisor.resumeTopActivitiesLocked();
    } <span class="hljs-keyword">else</span> {
        <span class="hljs-comment">//此處reason="force-crash"【見小節9.4】</span>
        mStackSupervisor.finishTopRunningActivityLocked(app, reason);
    }

    <span class="hljs-comment">//運行在當前進程中的所有服務的crash次數執行加1操作</span>
    <span class="hljs-keyword">for</span> (<span class="hljs-keyword">int</span> i=app.services.size()-<span class="hljs-number">1</span>; i&gt;=<span class="hljs-number">0</span>; i--) {
        ServiceRecord sr = app.services.valueAt(i);
        sr.crashCount++;
    }

    <span class="hljs-comment">//當桌面應用crash，並且被三方app所取代，那麼需要清空桌面應用的偏愛選項。</span>
    <span class="hljs-keyword">final</span> ArrayList&lt;ActivityRecord&gt; activities = app.activities;
    <span class="hljs-keyword">if</span> (app == mHomeProcess &amp;&amp; activities.size() &gt; <span class="hljs-number">0</span>
                &amp;&amp; (mHomeProcess.info.flags &amp; ApplicationInfo.FLAG_SYSTEM) == <span class="hljs-number">0</span>) {
        <span class="hljs-keyword">for</span> (<span class="hljs-keyword">int</span> activityNdx = activities.size() - <span class="hljs-number">1</span>; activityNdx &gt;= <span class="hljs-number">0</span>; --activityNdx) {
            <span class="hljs-keyword">final</span> ActivityRecord r = activities.get(activityNdx);
            <span class="hljs-keyword">if</span> (r.isHomeActivity()) {
                <span class="hljs-comment">//清空偏愛應用</span>
                ActivityThread.getPackageManager()
                        .clearPackagePreferredActivities(r.packageName);
            }
        }
    }

    <span class="hljs-keyword">if</span> (!app.isolated) {
        <span class="hljs-comment">//無法記錄孤立進程的crash時間點，由於他們並沒有一個固定身份</span>
        mProcessCrashTimes.put(app.info.processName, app.uid, now);
    }
    <span class="hljs-comment">//當app存在crash的handler，那麼交給其處理</span>
    <span class="hljs-keyword">if</span> (app.crashHandler != <span class="hljs-keyword">null</span>) mHandler.post(app.crashHandler);
    <span class="hljs-keyword">return</span> <span class="hljs-keyword">true</span>;
}
</code></pre></div></div>

<ol>
  <li>當同一進程在時間間隔小於1分鐘時連續兩次crash，則執行的情況下：
    <ul>
      <li>對於非persistent進程：
        <ul>
          <li>[9.1] mStackSupervisor.handleAppCrashLocked(app);</li>
          <li>[9.2] removeProcessLocked(app, false, false, “crash”);</li>
          <li>[9.3] mStackSupervisor.resumeTopActivitiesLocked();</li>
        </ul>
      </li>
      <li>對於persistent進程，則只執行
        <ul>
          <li>[9.3] mStackSupervisor.resumeTopActivitiesLocked();</li>
        </ul>
      </li>
    </ul>
  </li>
  <li>否則執行
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
            <span class="hljs-comment">//調用ActivityStack【見小節9.1.1】</span>
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
                <span class="hljs-comment">//結束當前activity</span>
                finishCurrentActivityLocked(r, FINISH_IMMEDIATELY, <span class="hljs-keyword">false</span>);
            }
        }
    }
}
</code></pre></div></div>

<p>這裡的<code class="highlighter-rouge">mTaskHistory</code>數據類型為ArrayList<taskrecord>，記錄著所有先前的後臺activities。遍歷所有activities，找到位於該ProcessRecord的所有ActivityRecord，並結束該Acitivity。</taskrecord></p>

<h4 id="92-amsremoveprocesslocked">9.2 AMS.removeProcessLocked<a class="anchorjs-link " href="#92-amsremoveprocesslocked" aria-label="Anchor link for: 92 amsremoveprocesslocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<p>[-&gt; ActivityManagerService.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">final</span> <span class="hljs-keyword">boolean</span> <span class="hljs-title">removeProcessLocked</span><span class="hljs-params">(ProcessRecord app,
        <span class="hljs-keyword">boolean</span> callerWillRestart, <span class="hljs-keyword">boolean</span> allowRestart, String reason)</span> </span>{
    <span class="hljs-keyword">final</span> String name = app.processName;
    <span class="hljs-keyword">final</span> <span class="hljs-keyword">int</span> uid = app.uid;
    <span class="hljs-comment">//從mProcessNames移除該進程</span>
    removeProcessNameLocked(name, uid);
    ...
    <span class="hljs-keyword">if</span> (app.pid &gt; <span class="hljs-number">0</span> &amp;&amp; app.pid != MY_PID) {
        <span class="hljs-keyword">int</span> pid = app.pid;
        <span class="hljs-keyword">synchronized</span> (mPidsSelfLocked) {
            mPidsSelfLocked.remove(pid); <span class="hljs-comment">//移除該pid</span>
            mHandler.removeMessages(PROC_START_TIMEOUT_MSG, app);
        }
        ...
        <span class="hljs-keyword">boolean</span> willRestart = <span class="hljs-keyword">false</span>;
        <span class="hljs-comment">//對於非孤立的persistent進程設置成可重啟flags</span>
        <span class="hljs-keyword">if</span> (app.persistent &amp;&amp; !app.isolated) {
            <span class="hljs-keyword">if</span> (!callerWillRestart) {
                willRestart = <span class="hljs-keyword">true</span>;
            } <span class="hljs-keyword">else</span> {
                needRestart = <span class="hljs-keyword">true</span>;
            }
        }
        <span class="hljs-comment">// 殺進程【9.2.1】</span>
        app.kill(reason, <span class="hljs-keyword">true</span>);
         <span class="hljs-comment">//移除進程並清空該進程相關聯的activity/service等組件 【9.2.2】</span>
        handleAppDiedLocked(app, willRestart, allowRestart);

        <span class="hljs-keyword">if</span> (willRestart) {
            <span class="hljs-comment">//此處willRestart=false，不進入該分支</span>
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
  <li><code class="highlighter-rouge">mProcessNames</code>數據類型為ProcessMap<processrecord>，這是以進程名為key，記錄著所有的ProcessRecord信息</processrecord></li>
  <li><code class="highlighter-rouge">mPidsSelfLocked</code>數據類型為SparseArray<processrecord>,這是以pid為key，記錄著所有的ProcessRecord信息。該對象的同步保護是通過自身鎖，而非全局ActivityManager鎖。</processrecord></li>
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
        Process.killProcessQuiet(pid); <span class="hljs-comment">//殺進程</span>
        Process.killProcessGroup(info.uid, pid); <span class="hljs-comment">//殺進程組，包括native進程</span>
        <span class="hljs-keyword">if</span> (!persistent) {
            killed = <span class="hljs-keyword">true</span>;
            killedByAm = <span class="hljs-keyword">true</span>;
        }
        Trace.traceEnd(Trace.TRACE_TAG_ACTIVITY_MANAGER);
    }
}
</code></pre></div></div>

<p>此處reason為“crash”，關於殺進程的過程見我的另一篇文章<a href="http://gityuan.com/2016/04/16/kill-signal/">理解殺進程的實現原理</a>.</p>

<h5 id="922-handleappdiedlocked">9.2.2 handleAppDiedLocked<a class="anchorjs-link " href="#922-handleappdiedlocked" aria-label="Anchor link for: 922 handleappdiedlocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h5>

<p>[-&gt; ActivityManagerService.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">final</span> <span class="hljs-keyword">void</span> <span class="hljs-title">handleAppDiedLocked</span><span class="hljs-params">(ProcessRecord app,
        <span class="hljs-keyword">boolean</span> restarting, <span class="hljs-keyword">boolean</span> allowRestart)</span> </span>{
    <span class="hljs-keyword">int</span> pid = app.pid;
    <span class="hljs-comment">//清除應用中service/receiver/ContentProvider信息</span>
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

    <span class="hljs-comment">//清除應用中activity相關信息</span>
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
        <span class="hljs-comment">//【見小節9.3.1】</span>
        result = targetStack.resumeTopActivityLocked(target, targetOptions);
    }

    <span class="hljs-keyword">for</span> (<span class="hljs-keyword">int</span> displayNdx = mActivityDisplays.size() - <span class="hljs-number">1</span>; displayNdx &gt;= <span class="hljs-number">0</span>; --displayNdx) {
        <span class="hljs-keyword">final</span> ArrayList&lt;ActivityStack&gt; stacks = mActivityDisplays.valueAt(displayNdx).mStacks;
        <span class="hljs-keyword">for</span> (<span class="hljs-keyword">int</span> stackNdx = stacks.size() - <span class="hljs-number">1</span>; stackNdx &gt;= <span class="hljs-number">0</span>; --stackNdx) {
            <span class="hljs-keyword">final</span> ActivityStack stack = stacks.get(stackNdx);
            <span class="hljs-keyword">if</span> (stack == targetStack) {
                <span class="hljs-keyword">continue</span>; <span class="hljs-comment">//已經啟動</span>
            }
            <span class="hljs-keyword">if</span> (isFrontStack(stack)) {
                stack.resumeTopActivityLocked(<span class="hljs-keyword">null</span>);
            }
        }
    }
    <span class="hljs-keyword">return</span> result;
}
</code></pre></div></div>

<p>此處<code class="highlighter-rouge">mFocusedStack</code>是當前正在等待接收input事件或者正在啟動下一個activity的<code class="highlighter-rouge">ActivityStack</code>。</p>

<h5 id="931-asresumetopactivitylocked">9.3.1 AS.resumeTopActivityLocked<a class="anchorjs-link " href="#931-asresumetopactivitylocked" aria-label="Anchor link for: 931 asresumetopactivitylocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h5>

<p>[-&gt; ActivityStack.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-keyword">final</span> <span class="hljs-keyword">boolean</span> .resumeTopActivityLocked(ActivityRecord prev, Bundle options) {
    ...
    result = resumeTopActivityInnerLocked(prev, options);<span class="hljs-comment">//【見小節9.3.2】</span>
    <span class="hljs-keyword">return</span> result;
}
</code></pre></div></div>

<h5 id="932-asresumetopactivityinnerlocked">9.3.2 AS.resumeTopActivityInnerLocked<a class="anchorjs-link " href="#932-asresumetopactivityinnerlocked" aria-label="Anchor link for: 932 asresumetopactivityinnerlocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h5>

<p>[-&gt; ActivityStack.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">boolean</span> <span class="hljs-title">resumeTopActivityInnerLocked</span><span class="hljs-params">(ActivityRecord prev, Bundle options)</span> </span>{
    <span class="hljs-comment">//找到mTaskHistory棧中第一個未處於finishing狀態的Activity</span>
    <span class="hljs-keyword">final</span> ActivityRecord next = topRunningActivityLocked(<span class="hljs-keyword">null</span>);

    <span class="hljs-keyword">if</span> (mResumedActivity == next &amp;&amp; next.state == ActivityState.RESUMED &amp;&amp;
                        mStackSupervisor.allResumedActivitiesComplete()) {
        <span class="hljs-comment">//當top activity已經處於resume，則無需操作；</span>
        <span class="hljs-keyword">return</span> <span class="hljs-keyword">false</span>;
    }

    <span class="hljs-keyword">if</span> (mService.isSleepingOrShuttingDown()
            &amp;&amp; mLastPausedActivity == next
            &amp;&amp; mStackSupervisor.allPausedActivitiesComplete()) {
        <span class="hljs-comment">//當正處於sleeping狀態，top activity處於paused，則無需操作</span>
        <span class="hljs-keyword">return</span> <span class="hljs-keyword">false</span>;
    }

    <span class="hljs-comment">//正在啟動app的activity，確保app不會被設置為stopped</span>
    AppGlobals.getPackageManager().setPackageStoppedState(
                next.packageName, <span class="hljs-keyword">false</span>, next.userId);
    <span class="hljs-comment">//回調應用onResume方法</span>
    next.app.thread.scheduleResumeActivity(next.appToken, next.app.repProcState,
                            mService.isNextTransitionForward(), resumeAnimOptions);
    ...
}
</code></pre></div></div>

<p>該方法代碼比較長，這裡就簡單列舉幾條比較重要的代碼。執行完該方法，應用也便完成了activity的resume過程。</p>

<h4 id="94-finishtoprunningactivitylocked">9.4 finishTopRunningActivityLocked<a class="anchorjs-link " href="#94-finishtoprunningactivitylocked" aria-label="Anchor link for: 94 finishtoprunningactivitylocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<h5 id="941-assfinishtoprunningactivitylocked">9.4.1 ASS.finishTopRunningActivityLocked<a class="anchorjs-link " href="#941-assfinishtoprunningactivitylocked" aria-label="Anchor link for: 941 assfinishtoprunningactivitylocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h5>

<p>[-&gt; ActivityStackSupervisor.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">finishTopRunningActivityLocked</span><span class="hljs-params">(ProcessRecord app, String reason)</span> </span>{
    <span class="hljs-keyword">for</span> (<span class="hljs-keyword">int</span> displayNdx = mActivityDisplays.size() - <span class="hljs-number">1</span>; displayNdx &gt;= <span class="hljs-number">0</span>; --displayNdx) {
        <span class="hljs-keyword">final</span> ArrayList&lt;ActivityStack&gt; stacks = mActivityDisplays.valueAt(displayNdx).mStacks;
        <span class="hljs-keyword">final</span> <span class="hljs-keyword">int</span> numStacks = stacks.size();
        <span class="hljs-keyword">for</span> (<span class="hljs-keyword">int</span> stackNdx = <span class="hljs-number">0</span>; stackNdx &lt; numStacks; ++stackNdx) {
            <span class="hljs-keyword">final</span> ActivityStack stack = stacks.get(stackNdx);
            <span class="hljs-comment">//此處reason= "force-crash"【見小節9.4.2】</span>
            stack.finishTopRunningActivityLocked(app, reason);
        }
    }
}
</code></pre></div></div>

<h5 id="942-asfinishtoprunningactivitylocked">9.4.2 AS.finishTopRunningActivityLocked<a class="anchorjs-link " href="#942-asfinishtoprunningactivitylocked" aria-label="Anchor link for: 942 asfinishtoprunningactivitylocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h5>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cs"><span class="hljs-function">final <span class="hljs-keyword">void</span> <span class="hljs-title">finishTopRunningActivityLocked</span>(<span class="hljs-params">ProcessRecord app, String reason</span>) </span>{
    <span class="hljs-comment">//找到棧頂第一個不處於finishing狀態的activity</span>
    ActivityRecord r = topRunningActivityLocked(<span class="hljs-literal">null</span>);
    <span class="hljs-keyword">if</span> (r != <span class="hljs-literal">null</span> &amp;&amp; r.app == app) {
        <span class="hljs-keyword">int</span> taskNdx = mTaskHistory.indexOf(r.task);
        <span class="hljs-keyword">int</span> activityNdx = r.task.mActivities.indexOf(r);
        <span class="hljs-comment">//【見小節9.4.3】</span>
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
                    <span class="hljs-comment">//【見小節9.4.3】</span>
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
        <span class="hljs-keyword">return</span> <span class="hljs-keyword">false</span>; <span class="hljs-comment">//正在finishing則返回</span>
    }
    <span class="hljs-comment">//設置finish狀態的activity不可見</span>
    r.makeFinishingLocked();
    <span class="hljs-comment">//暫停key的分發事件</span>
    r.pauseKeyDispatchingLocked();

    mWindowManager.prepareAppTransition(endTask
            ? AppTransition.TRANSIT_TASK_CLOSE
            : AppTransition.TRANSIT_ACTIVITY_CLOSE, <span class="hljs-keyword">false</span>);

    mWindowManager.setAppVisibility(r.appToken, <span class="hljs-keyword">false</span>);
    <span class="hljs-comment">//回調activity的onPause方法</span>
    startPausingLocked(<span class="hljs-keyword">false</span>, <span class="hljs-keyword">false</span>, <span class="hljs-keyword">false</span>, <span class="hljs-keyword">false</span>);
    ...
}
</code></pre></div></div>

<p>該方法最終會回調到activity的pause方法。</p>

<p>執行到這，我們還回過來看小節<code class="highlighter-rouge">5.crashApplication</code>中，處理完makeAppCrashingLocked，則會再發送消息SHOW_ERROR_MSG，彈出提示crash的對話框，接下來再看看該過程。</p>

<h3 id="10-uihandler">10. UiHandler<a class="anchorjs-link " href="#10-uihandler" aria-label="Anchor link for: 10 uihandler" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>通過mUiHandler發送message，且消息的msg.waht=SHOW_ERROR_MSG，接下來進入UiHandler來看看handleMessage的處理過程。</p>

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
                   <span class="hljs-comment">//創建提示crash對話框，等待用戶選擇，5分鐘操作等待。</span>
                   Dialog d = <span class="hljs-keyword">new</span> AppErrorDialog(mContext,
                           ActivityManagerService.<span class="hljs-keyword">this</span>, res, proc);
                   d.show();
                   proc.crashDialog = d;
               } <span class="hljs-keyword">else</span> {
                   <span class="hljs-comment">//當處於sleep狀態，則默認選擇退出。</span>
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

<p>在發生crash時，默認系統會彈出提示crash的對話框，並阻塞等待用戶選擇是“退出”或 “退出並報告”，當用戶不做任何選擇時5min超時後，默認選擇“退出”，當手機休眠時也默認選擇“退出”。到這裡也並沒有真正結束，在小節<code class="highlighter-rouge">2.uncaughtException</code>中在<code class="highlighter-rouge">finnally</code>語句塊還有一個殺進程的動作。</p>

<h3 id="11-killprocess">11. killProcess<a class="anchorjs-link " href="#11-killprocess" aria-label="Anchor link for: 11 killprocess" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs css"><span class="hljs-selector-tag">Process</span><span class="hljs-selector-class">.killProcess</span>(<span class="hljs-selector-tag">Process</span><span class="hljs-selector-class">.myPid</span>());
<span class="hljs-selector-tag">System</span><span class="hljs-selector-class">.exit</span>(10);
</code></pre></div></div>

<p>通過finnally語句塊保證能執行並徹底殺掉Crash進程，關於殺進程的過程見我的另一篇文章<a href="http://gityuan.com/2016/04/16/kill-signal/">理解殺進程的實現原理</a>.。當Crash進程被殺後，並沒有完全結束，還有Binder死亡通知的流程還沒有處理完成。</p>

<h3 id="12-小結">12. 小結<a class="anchorjs-link " href="#12-小結" aria-label="Anchor link for: 12 小結" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>當進程拋出未捕獲異常時，則系統會處理該異常並進入crash處理流程。</p>

<p><img src="./images/app_crash.jpg" alt="app_crash"></p>

<p>其中最為核心的工作圖中紅色部分<code class="highlighter-rouge">AMS.handleAppCrashLocked</code>的主要功能：</p>

<ol>
  <li>當同一進程1分鐘之內連續兩次crash，則執行的情況下：
    <ul>
      <li>對於非persistent進程：
        <ul>
          <li>ASS.handleAppCrashLocked, 直接結束該應用所有activity</li>
          <li>AMS.removeProcessLocked，殺死該進程以及同一個進程組下的所有進</li>
          <li>ASS.resumeTopActivitiesLocked，恢復棧頂第一個非finishing狀態的activity</li>
        </ul>
      </li>
      <li>對於persistent進程，則只執行
        <ul>
          <li>ASS.resumeTopActivitiesLocked，恢復棧頂第一個非finishing狀態的activity</li>
        </ul>
      </li>
    </ul>
  </li>
  <li>否則，當進程沒連續頻繁crash
    <ul>
      <li>ASS.finishTopRunningActivityLocked，執行結束棧頂正在運行activity</li>
    </ul>
  </li>
</ol>

<p>另外，<code class="highlighter-rouge">AMS.handleAppCrashLocked</code>，該方法內部主要調用鏈，如下：</p>

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

<p>進程被殺，如果還記得Binder的死亡回調機制，在應用進程創建的過程中有一個<code class="highlighter-rouge">attachApplicationLocked</code>方法的過程中便會創建死亡通知。</p>

<p>[-&gt; ActivityManagerService.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">final</span> <span class="hljs-keyword">boolean</span> <span class="hljs-title">attachApplicationLocked</span><span class="hljs-params">(IApplicationThread thread,
            <span class="hljs-keyword">int</span> pid)</span> </span>{
    <span class="hljs-keyword">try</span> {
        <span class="hljs-comment">//創建binder死亡通知</span>
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

<p>當binder服務端掛了之後，便會通過binder的DeathRecipient來通知AMS進行相應的清理收尾工作。前面已經降到crash的進程會被kill掉，那麼當該進程會殺，則會回調到binderDied()方法。</p>

<h3 id="1-binderdied">1. binderDied<a class="anchorjs-link " href="#1-binderdied" aria-label="Anchor link for: 1 binderdied" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; ActivityManagerService.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-keyword">private</span> <span class="hljs-keyword">final</span> <span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">AppDeathRecipient</span> <span class="hljs-keyword">implements</span> <span class="hljs-title">IBinder</span>.<span class="hljs-title">DeathRecipient</span> </span>{
    <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">binderDied</span><span class="hljs-params">()</span> </span>{
        <span class="hljs-keyword">synchronized</span>(ActivityManagerService.<span class="hljs-keyword">this</span>) {
            appDiedLocked(mApp, mPid, mAppThread, <span class="hljs-keyword">true</span>);<span class="hljs-comment">//【見小節2】</span>
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
        <span class="hljs-comment">//【見小節3】</span>
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
    <span class="hljs-comment">//清理應用程序service, BroadcastReceiver, ContentProvider相關信息【見小節4】</span>
    <span class="hljs-keyword">boolean</span> kept = cleanUpApplicationRecordLocked(app, restarting, allowRestart, -<span class="hljs-number">1</span>);

    <span class="hljs-keyword">if</span> (!kept &amp;&amp; !restarting) {
        removeLruProcessLocked(app);
        <span class="hljs-keyword">if</span> (pid &gt; <span class="hljs-number">0</span>) {
            ProcessList.remove(pid);
        }
    }

    <span class="hljs-comment">//清理activity相關信息</span>
    <span class="hljs-keyword">boolean</span> hasVisibleActivities = mStackSupervisor.handleAppDiedLocked(app);
    app.activities.clear();
    ...
    <span class="hljs-comment">//恢復棧頂第一個非finish的activity</span>
    <span class="hljs-keyword">if</span> (!restarting &amp;&amp; hasVisibleActivities &amp;&amp; !mStackSupervisor.resumeTopActivitiesLocked()) {
       mStackSupervisor.ensureActivitiesVisibleLocked(<span class="hljs-keyword">null</span>, <span class="hljs-number">0</span>);
   }
}
</code></pre></div></div>

<h3 id="4-cleanupapplicationrecordlocked">4 cleanUpApplicationRecordLocked<a class="anchorjs-link " href="#4-cleanupapplicationrecordlocked" aria-label="Anchor link for: 4 cleanupapplicationrecordlocked" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>該方法清理應用程序service, BroadcastReceiver, ContentProvider，process相關信息，為了便於說明將該方法劃分為4個部分講解</p>

<h4 id="41-清理service">4.1 清理service<a class="anchorjs-link " href="#41-清理service" aria-label="Anchor link for: 41 清理service" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<p>參數restarting = false,  allowRestart =true,  index =-1</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">final</span> <span class="hljs-keyword">boolean</span> <span class="hljs-title">cleanUpApplicationRecordLocked</span><span class="hljs-params">(ProcessRecord app,
        <span class="hljs-keyword">boolean</span> restarting, <span class="hljs-keyword">boolean</span> allowRestart, <span class="hljs-keyword">int</span> index)</span> </span>{
    ...
    mProcessesToGc.remove(app);
    mPendingPssProcesses.remove(app);

    <span class="hljs-comment">//如果存在，則清除crash/anr/wait對話框</span>
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
    <span class="hljs-comment">//將app移除前臺進程</span>
    updateProcessForegroundLocked(app, <span class="hljs-keyword">false</span>, <span class="hljs-keyword">false</span>);
    app.foregroundActivities = <span class="hljs-keyword">false</span>;
    app.hasShownUi = <span class="hljs-keyword">false</span>;
    app.treatLikeActivity = <span class="hljs-keyword">false</span>;
    app.hasAboveClient = <span class="hljs-keyword">false</span>;
    app.hasClientActivities = <span class="hljs-keyword">false</span>;
    <span class="hljs-comment">//清理service信息，這個過程也比較複雜，後續再展開</span>
    mServices.killServicesLocked(app, allowRestart);
    <span class="hljs-keyword">boolean</span> restart = <span class="hljs-keyword">false</span>;
}
</code></pre></div></div>

<ul>
  <li>mProcessesToGc：記錄著需要儘快執行gc的進程列表</li>
  <li>mPendingPssProcesses：記錄著需要收集內存信息的進程列表</li>
</ul>

<h4 id="42-清理contentprovider">4.2 清理ContentProvider<a class="anchorjs-link " href="#42-清理contentprovider" aria-label="Anchor link for: 42 清理contentprovider" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">final</span> <span class="hljs-keyword">boolean</span> <span class="hljs-title">cleanUpApplicationRecordLocked</span><span class="hljs-params">(...)</span> </span>{
    ...
    <span class="hljs-keyword">for</span> (<span class="hljs-keyword">int</span> i = app.pubProviders.size() - <span class="hljs-number">1</span>; i &gt;= <span class="hljs-number">0</span>; i--) {
        <span class="hljs-comment">//獲取該進程已發表的ContentProvider</span>
        ContentProviderRecord cpr = app.pubProviders.valueAt(i);
        <span class="hljs-keyword">final</span> <span class="hljs-keyword">boolean</span> always = app.bad || !allowRestart;
        <span class="hljs-comment">//ContentProvider服務端被殺，則client端進程也會被殺</span>
        <span class="hljs-keyword">boolean</span> inLaunching = removeDyingProviderLocked(app, cpr, always);
        <span class="hljs-keyword">if</span> ((inLaunching || always) &amp;&amp; cpr.hasConnectionOrHandle()) {
            restart = <span class="hljs-keyword">true</span>; <span class="hljs-comment">//需要重啟</span>
        }

        cpr.provider = <span class="hljs-keyword">null</span>;
        cpr.proc = <span class="hljs-keyword">null</span>;
    }
    app.pubProviders.clear();

    <span class="hljs-comment">//處理正在啟動並且是有client端正在等待的ContentProvider</span>
    <span class="hljs-keyword">if</span> (cleanupAppInLaunchingProvidersLocked(app, <span class="hljs-keyword">false</span>)) {
        restart = <span class="hljs-keyword">true</span>;
    }

    <span class="hljs-comment">//取消已連接的ContentProvider的註冊</span>
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

    <span class="hljs-comment">// 取消註冊的廣播接收者</span>
    <span class="hljs-keyword">for</span> (<span class="hljs-keyword">int</span> i = app.receivers.size() - <span class="hljs-number">1</span>; i &gt;= <span class="hljs-number">0</span>; i--) {
        removeReceiverLocked(app.receivers.valueAt(i));
    }
    app.receivers.clear();
}
</code></pre></div></div>

<h4 id="44-清理process">4.4 清理Process<a class="anchorjs-link " href="#44-清理process" aria-label="Anchor link for: 44 清理process" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">final</span> <span class="hljs-keyword">boolean</span> <span class="hljs-title">cleanUpApplicationRecordLocked</span><span class="hljs-params">(...)</span> </span>{
    ...
    <span class="hljs-comment">//當app正在備份時的處理方式</span>
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
        <span class="hljs-comment">//對於persistent應用，則需要重啟</span>
        <span class="hljs-keyword">if</span> (mPersistentStartingProcesses.indexOf(app) &lt; <span class="hljs-number">0</span>) {
            mPersistentStartingProcesses.add(app);
            restart = <span class="hljs-keyword">true</span>;
        }
    }

    <span class="hljs-comment">//mProcessesOnHold：記錄著試圖在系統ready之前就啟動的進程。</span>
    <span class="hljs-comment">//在那時並不啟動這些進程，先記錄下來,等系統啟動完成則啟動這些進程。</span>
    mProcessesOnHold.remove(app);

    <span class="hljs-keyword">if</span> (app == mHomeProcess) {
        mHomeProcess = <span class="hljs-keyword">null</span>;
    }
    <span class="hljs-keyword">if</span> (app == mPreviousProcess) {
        mPreviousProcess = <span class="hljs-keyword">null</span>;
    }

    <span class="hljs-keyword">if</span> (restart &amp;&amp; !app.isolated) {
        <span class="hljs-comment">//仍有組件需要運行在該進程中，因此重啟該進程</span>
        <span class="hljs-keyword">if</span> (index &lt; <span class="hljs-number">0</span>) {
            ProcessList.remove(app.pid);
        }
        addProcessNameLocked(app);
        startProcessLocked(app, <span class="hljs-string">"restart"</span>, app.processName);
        <span class="hljs-keyword">return</span> <span class="hljs-keyword">true</span>;
    } <span class="hljs-keyword">else</span> <span class="hljs-keyword">if</span> (app.pid &gt; <span class="hljs-number">0</span> &amp;&amp; app.pid != MY_PID) {
        <span class="hljs-comment">//移除該進程相關信息</span>
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

<p>對於需要重啟進程的情形有：</p>

<ul>
  <li><code class="highlighter-rouge">mLaunchingProviders</code>：記錄著存在client端等待的ContentProvider。應用當前正在啟動中，當ContentProvider一旦發佈則將該ContentProvider將從該list去除。當進程包含這樣的ContentProvider，則需要重啟進程。</li>
  <li><code class="highlighter-rouge">mPersistentStartingProcesses</code>：記錄著試圖在系統ready之前就啟動的進程。在那時並不啟動這些進程，先記錄下來,等系統啟動完成則啟動這些進程。當進程屬於這種類型也需要重啟。</li>
</ul>

<h3 id="5-小結">5. 小結<a class="anchorjs-link " href="#5-小結" aria-label="Anchor link for: 5 小結" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>當crash進程執行kill操作後，進程被殺。此時需要掌握binder 死亡通知原理，由於Crash進程中擁有一個Binder服務端<code class="highlighter-rouge">ApplicationThread</code>，而應用進程在創建過程調用attachApplicationLocked()，從而attach到system_server進程，在system_server進程內有一個<code class="highlighter-rouge">ApplicationThreadProxy</code>，這是相對應的Binder客戶端。當Binder服務端<code class="highlighter-rouge">ApplicationThread</code>所在進程(即Crash進程)掛掉後，則Binder客戶端能收到相應的死亡通知，從而進入binderDied流程。更多關於bInder原理，這裡就不細說，博客中有關於binder系列的專題。</p>

<p><img src="./images/binder_died.jpg" alt="binder_died"></p>

<h2 id="四-總結">四、 總結<a class="anchorjs-link " href="#四-總結" aria-label="Anchor link for: 四 總結" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h2>

<p>本文主要以源碼的視角，詳細介紹了到應用crash後系統的處理流程：</p>

<ol>
  <li>首先發生crash所在進程，在創建之初便準備好了defaultUncaughtHandler，用來來處理Uncaught Exception，並輸出當前crash基本信息；</li>
  <li>調用當前進程中的AMP.handleApplicationCrash；經過binder ipc機制，傳遞到system_server進程；</li>
  <li>接下來，進入system_server進程，調用binder服務端執行AMS.handleApplicationCrash；</li>
  <li>從<code class="highlighter-rouge">mProcessNames</code>查找到目標進程的ProcessRecord對象；並將進程crash信息輸出到目錄<code class="highlighter-rouge">/data/system/dropbox</code>；</li>
  <li>執行makeAppCrashingLocked
    <ul>
      <li>創建當前用戶下的crash應用的error receiver，並忽略當前應用的廣播；</li>
      <li>停止當前進程中所有activity中的WMS的凍結屏幕消息，並執行相關一些屏幕相關操作；</li>
    </ul>
  </li>
  <li>再執行handleAppCrashLocked方法，
    <ul>
      <li>當1分鐘內同一進程<code class="highlighter-rouge">連續crash兩次</code>時，且<code class="highlighter-rouge">非persistent</code>進程，則直接結束該應用所有activity，並殺死該進程以及同一個進程組下的所有進程。然後再恢復棧頂第一個非finishing狀態的activity;</li>
      <li>當1分鐘內同一進程<code class="highlighter-rouge">連續crash兩次</code>時，且<code class="highlighter-rouge">persistent</code>進程，，則只執行恢復棧頂第一個非finishing狀態的activity;</li>
      <li>當1分鐘內同一進程<code class="highlighter-rouge">未發生連續crash兩次</code>時，則執行結束棧頂正在運行activity的流程。</li>
    </ul>
  </li>
  <li>通過mUiHandler發送消息<code class="highlighter-rouge">SHOW_ERROR_MSG</code>，彈出crash對話框；</li>
  <li>到此，system_server進程執行完成。回到crash進程開始執行殺掉當前進程的操作；</li>
  <li>當crash進程被殺，通過binder死亡通知，告知system_server進程來執行appDiedLocked()；</li>
  <li>最後，執行清理應用相關的activity/service/ContentProvider/receiver組件信息。</li>
</ol>

<p>這基本就是整個應用Crash後系統的執行過程。 最後，再說說對於同一個app連續crash的情況：</p>

<ul>
  <li>當60s內連續crash兩次的非persistent進程時，被認定為bad進程：那麼如果第3次從後臺啟動該進程(Intent.getFlags來判斷)，則會拒絕創建進程；</li>
  <li>當crash次數達到兩次的非persistent進程時，則再次殺該進程，即便允許自啟的service也會在被殺後拒絕再次啟動。</li>
</ul>
