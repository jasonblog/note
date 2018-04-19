# 理解Native Crash處理流程


<div class="row">

    <!-- Post Container -->
            <div class="
                col-lg-8 col-lg-offset-2
                col-md-10 col-md-offset-1
                post-container">

								<blockquote>
  <p>本文是基於Android 7.0源碼，來分析Native Crash流程。</p>
</blockquote>

<h2 id="一native-crash">一、Native Crash<a class="anchorjs-link " href="#一native-crash" aria-label="Anchor link for: 一native crash" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h2>

<p>從系統全局來說，Crash分為Framework/App Crash， Native Crash，以及Kernel Crash。</p>

<ul>
  <li>對於framework層或者app層的Crash(即Java層面Crash)，那麼往往是通過拋出未捕獲異常而導致的Crash，這個內容在本文的姊妹篇<a href="http://gityuan.com/2016/06/24/app-crash/">理解Android Crash處理流程</a>已詳細介紹過。</li>
  <li>至於Kernel Crash，很多情況是發生Kernel panic，對於內核崩潰往往是驅動或者硬件出現故障。</li>
  <li>Native Crash，即C/C++層面的Crash，這是介於系統framework層與Linux層之間的一層，這是本文接下來要講解的內容。</li>
</ul>

<p>如果你是從事Android系統開發或者架構相關工作，或者遇到需要解系統性的疑難雜症，再或者需要寫JNI代碼，則就有可能遇到Native Crash，瞭解系統Native Crash處理流程就很有必要。</p>

<p>接下來介紹介紹<code class="highlighter-rouge">Android N</code>的Native Crash處理流程，你沒有看錯，本文就是針對最新Android Nouget來分析的。Native crash的工作核心是由debuggerd守護進程來完成，在文章<a href="http://gityuan.com/2016/06/15/android-debuggerd/">調試系列4：debuggerd源碼篇)</a>，已經介紹過Debuggerdd的工作原理。</p>

<p>要了解Native Crash，首先從應用程序入口位於<code class="highlighter-rouge">begin.S</code>中的<code class="highlighter-rouge">__linker_init</code>入手。</p>

<h3 id="11-begins">1.1 begin.S<a class="anchorjs-link " href="#11-begins" aria-label="Anchor link for: 11 begins" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; arch/arm/begin.S]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs objectivec">ENTRY(_start)
  mov r0, sp
  <span class="hljs-comment">//入口地址 【見小節1.2】</span>
  bl __linker_init
  <span class="hljs-comment">/* linker init returns the _entry address in the main image */</span>
  mov pc, r0
END(_start)
</code></pre></div></div>

<h3 id="12--__linker_init">1.2  __linker_init<a class="anchorjs-link " href="#12--__linker_init" aria-label="Anchor link for: 12  __linker_init" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; linker.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cs"><span class="hljs-keyword">extern</span> <span class="hljs-string">"C"</span> ElfW(Addr) __linker_init(<span class="hljs-keyword">void</span>* raw_args) {
  <span class="hljs-function">KernelArgumentBlock <span class="hljs-title">args</span>(<span class="hljs-params">raw_args</span>)</span>;
  ElfW(Addr) linker_addr = args.getauxval(AT_BASE);
  ...
  <span class="hljs-comment">//【見小節1.3】</span>
  ElfW(Addr) start_address = __linker_init_post_relocation(args, linker_addr);
  <span class="hljs-keyword">return</span> start_address;
}
</code></pre></div></div>

<h3 id="13-__linker_init_post_relocation">1.3 __linker_init_post_relocation<a class="anchorjs-link " href="#13-__linker_init_post_relocation" aria-label="Anchor link for: 13 __linker_init_post_relocation" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; linker.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-title">ElfW</span><span class="hljs-params">(Addr)</span> <span class="hljs-title">__linker_init_post_relocation</span><span class="hljs-params">(KernelArgumentBlock&amp; args, ElfW(Addr)</span> linker_base) </span>{
  ...
  <span class="hljs-comment">// Sanitize the environment.</span>
  __libc_init_AT_SECURE(args);
  <span class="hljs-comment">// Initialize system properties</span>
  __system_properties_init();
  <span class="hljs-comment">//【見小節1.4】</span>
  debuggerd_init();
  ...
}
</code></pre></div></div>

<h3 id="14-debuggerd_init">1.4 debuggerd_init<a class="anchorjs-link " href="#14-debuggerd_init" aria-label="Anchor link for: 14 debuggerd_init" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; linker/debugger.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp">__<span class="hljs-function">LIBC_HIDDEN__ <span class="hljs-keyword">void</span> <span class="hljs-title">debuggerd_init</span><span class="hljs-params">()</span> </span>{
  <span class="hljs-keyword">struct</span> sigaction action;
  <span class="hljs-built_in">memset</span>(&amp;action, <span class="hljs-number">0</span>, <span class="hljs-keyword">sizeof</span>(action));
  sigemptyset(&amp;action.sa_mask);
  <span class="hljs-comment">//【見小節1.5】</span>
  action.sa_sigaction = debuggerd_signal_handler;
  <span class="hljs-comment">//SA_RESTART代表中斷某個syscall，則會自動重新調用該syscall</span>
  <span class="hljs-comment">//SA_SIGINFO代表信號附帶參數siginfo_t結構體可傳送到signal_handler函數</span>
  action.sa_flags = SA_RESTART | SA_SIGINFO;
  <span class="hljs-comment">//使用備用signal棧(如果可用)，以便我們能捕獲棧溢出</span>
  action.sa_flags |= SA_ONSTACK;
  sigaction(SIGABRT, &amp;action, <span class="hljs-literal">nullptr</span>);
  sigaction(SIGBUS, &amp;action, <span class="hljs-literal">nullptr</span>);
  sigaction(SIGFPE, &amp;action, <span class="hljs-literal">nullptr</span>);
  sigaction(SIGILL, &amp;action, <span class="hljs-literal">nullptr</span>);
  sigaction(SIGPIPE, &amp;action, <span class="hljs-literal">nullptr</span>);
  sigaction(SIGSEGV, &amp;action, <span class="hljs-literal">nullptr</span>);
<span class="hljs-meta">#<span class="hljs-meta-keyword">if</span> defined(SIGSTKFLT)</span>
  sigaction(SIGSTKFLT, &amp;action, <span class="hljs-literal">nullptr</span>);
<span class="hljs-meta">#<span class="hljs-meta-keyword">endif</span></span>
  sigaction(SIGTRAP, &amp;action, <span class="hljs-literal">nullptr</span>);
}
</code></pre></div></div>

<h3 id="15-debuggerd_signal_handler">1.5 debuggerd_signal_handler<a class="anchorjs-link " href="#15-debuggerd_signal_handler" aria-label="Anchor link for: 15 debuggerd_signal_handler" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>連接到bionic上的native程序(C/C++)出現異常時，kernel會發送相應的signal；
當進程捕獲致命的signal，通知debuggerd調用ptrace來獲取有價值的信息(發生crash之前)。</p>

<p>[-&gt; linker/debugger.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">void</span> <span class="hljs-title">debuggerd_signal_handler</span><span class="hljs-params">(<span class="hljs-keyword">int</span> signal_number, siginfo_t* info, <span class="hljs-keyword">void</span>*)</span> </span>{
  <span class="hljs-keyword">if</span> (!have_siginfo(signal_number)) {
    info = <span class="hljs-literal">nullptr</span>; <span class="hljs-comment">//SA_SIGINFO標識被意外清空，則info未定義</span>
  }
  <span class="hljs-comment">//防止debuggerd無法鏈接時，仍可以輸出一些簡要signal信息</span>
  log_signal_summary(signal_number, info);
  <span class="hljs-comment">//建立於debuggerd的socket通信連接 【見小節1.6】</span>
  send_debuggerd_packet(info);
  <span class="hljs-comment">//重置信號處理函數為SIG_DFL(默認操作)</span>
  signal(signal_number, SIG_DFL);

  <span class="hljs-keyword">switch</span> (signal_number) {
    <span class="hljs-keyword">case</span> SIGABRT:
    <span class="hljs-keyword">case</span> SIGFPE:
    <span class="hljs-keyword">case</span> SIGPIPE:
<span class="hljs-meta">#<span class="hljs-meta-keyword">if</span> defined(SIGSTKFLT)</span>
    <span class="hljs-keyword">case</span> SIGSTKFLT:
<span class="hljs-meta">#<span class="hljs-meta-keyword">endif</span></span>
    <span class="hljs-keyword">case</span> SIGTRAP:
      tgkill(getpid(), gettid(), signal_number);
      <span class="hljs-keyword">break</span>;
    <span class="hljs-keyword">default</span>:    <span class="hljs-comment">// SIGILL, SIGBUS, SIGSEGV</span>
      <span class="hljs-keyword">break</span>;
  }
}
</code></pre></div></div>

<h3 id="16-send_debuggerd_packet">1.6 send_debuggerd_packet<a class="anchorjs-link " href="#16-send_debuggerd_packet" aria-label="Anchor link for: 16 send_debuggerd_packet" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; linker/debugger.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">void</span> <span class="hljs-title">send_debuggerd_packet</span><span class="hljs-params">(siginfo_t* info)</span> </span>{
  <span class="hljs-comment">// Mutex防止多個crashing線程同一時間來來嘗試跟debuggerd進行通信</span>
  <span class="hljs-keyword">static</span> <span class="hljs-keyword">pthread_mutex_t</span> crash_mutex = PTHREAD_MUTEX_INITIALIZER;
  <span class="hljs-keyword">int</span> ret = pthread_mutex_trylock(&amp;crash_mutex);
  <span class="hljs-keyword">if</span> (ret != <span class="hljs-number">0</span>) {
    <span class="hljs-keyword">if</span> (ret == EBUSY) {
      __libc_format_log(ANDROID_LOG_INFO, <span class="hljs-string">"libc"</span>,
          <span class="hljs-string">"Another thread contacted debuggerd first; not contacting debuggerd."</span>);
      <span class="hljs-comment">//等待其他線程釋放該鎖，從而獲取該鎖</span>
      pthread_mutex_lock(&amp;crash_mutex);
    }
    return;
  }
  <span class="hljs-comment">//建立與debuggerd的socket通道</span>
  <span class="hljs-keyword">int</span> s = socket_abstract_client(DEBUGGER_SOCKET_NAME, SOCK_STREAM | SOCK_CLOEXEC);
  ...
  <span class="hljs-keyword">debugger_msg_t</span> msg;
  msg.action = DEBUGGER_ACTION_CRASH;
  msg.tid = gettid();
  msg.abort_msg_address = <span class="hljs-keyword">reinterpret_cast</span>&lt;<span class="hljs-keyword">uintptr_t</span>&gt;(g_abort_message);
  msg.original_si_code = (info != nullptr) ? info-&gt;si_code : <span class="hljs-number">0</span>;
  <span class="hljs-comment">//將DEBUGGER_ACTION_CRASH消息發送給debuggerd服務端</span>
  ret = TEMP_FAILURE_RETRY(write(s, &amp;msg, sizeof(msg)));
  <span class="hljs-keyword">if</span> (ret == <span class="hljs-keyword">sizeof</span>(msg)) {
    <span class="hljs-keyword">char</span> debuggerd_ack;
    <span class="hljs-comment">//阻塞等待debuggerd服務端的迴應數據</span>
    ret = TEMP_FAILURE_RETRY(read(s, &amp;debuggerd_ack, <span class="hljs-number">1</span>));
    <span class="hljs-keyword">int</span> saved_errno = errno;
    notify_gdb_of_libraries();
    errno = saved_errno;
  }
  close(s);
}
</code></pre></div></div>

<p>該方法的主要功能：</p>

<ul>
  <li>調用socket_abstract_client，建立於debuggerd的socket通道；</li>
  <li>將<code class="highlighter-rouge">action = DEBUGGER_ACTION_CRASH</code>的消息發送給debuggerd服務端；</li>
  <li>阻塞等待debuggerd服務端的迴應數據。</li>
</ul>

<p>接下來，看看debuggerd服務端接收到<code class="highlighter-rouge">DEBUGGER_ACTION_CRASH</code>的處理流程</p>

<h2 id="二debuggerd服務端">二、debuggerd服務端<a class="anchorjs-link " href="#二debuggerd服務端" aria-label="Anchor link for: 二debuggerd服務端" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h2>

<p>debuggerd 守護進程啟動後，一直在等待socket client的連接。當native crash發送後便會向debuggerd發送<code class="highlighter-rouge">action = DEBUGGER_ACTION_CRASH</code>的消息。</p>

<h3 id="21-do_server">2.1 do_server<a class="anchorjs-link " href="#21-do_server" aria-label="Anchor link for: 21 do_server" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; /debuggerd/debuggerd.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">int</span> <span class="hljs-title">do_server</span><span class="hljs-params">()</span> </span>{
  ...
  <span class="hljs-keyword">for</span> (;;) {
    sockaddr_storage ss;
    sockaddr* addrp = <span class="hljs-keyword">reinterpret_cast</span>&lt;sockaddr*&gt;(&amp;ss);
    <span class="hljs-keyword">socklen_t</span> alen = <span class="hljs-keyword">sizeof</span>(ss);
    <span class="hljs-comment">//等待客戶端連接</span>
    <span class="hljs-keyword">int</span> fd = accept4(s, addrp, &amp;alen, SOCK_CLOEXEC);
    <span class="hljs-keyword">if</span> (fd == <span class="hljs-number">-1</span>) {
      <span class="hljs-keyword">continue</span>; <span class="hljs-comment">//accept失敗</span>
    }
    <span class="hljs-comment">//處理native crash發送過來的請求【見小節2.2】</span>
    handle_request(fd);
  }
  return <span class="hljs-number">0</span>;
}
</code></pre></div></div>

<h3 id="22-handle_request">2.2 handle_request<a class="anchorjs-link " href="#22-handle_request" aria-label="Anchor link for: 22 handle_request" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; /debuggerd/debuggerd.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">void</span> <span class="hljs-title">handle_request</span><span class="hljs-params">(<span class="hljs-keyword">int</span> fd)</span> </span>{
  android::base::<span class="hljs-function">unique_fd <span class="hljs-title">closer</span><span class="hljs-params">(fd)</span></span>;
  <span class="hljs-keyword">debugger_request_t</span> request;
  <span class="hljs-built_in">memset</span>(&amp;request, <span class="hljs-number">0</span>, <span class="hljs-keyword">sizeof</span>(request));
  <span class="hljs-comment">//讀取client發送過來的請求【見小節2.3】</span>
  <span class="hljs-keyword">int</span> status = read_request(fd, &amp;request);
  ...

  <span class="hljs-comment">//fork子進程來處理其餘請求命令</span>
  <span class="hljs-keyword">pid_t</span> fork_pid = fork();
  <span class="hljs-keyword">if</span> (fork_pid == <span class="hljs-number">-1</span>) {
    ALOGE(<span class="hljs-string">"debuggerd: failed to fork: %s\n"</span>, strerror(errno));
  } <span class="hljs-keyword">else</span> <span class="hljs-keyword">if</span> (fork_pid == <span class="hljs-number">0</span>) {
     <span class="hljs-comment">//子進程執行【見小節2.4】</span>
    worker_process(fd, request);
  } <span class="hljs-keyword">else</span> {
    <span class="hljs-comment">//父進程執行【見小節2.5】</span>
    monitor_worker_process(fork_pid, request);
  }
}
</code></pre></div></div>

<h3 id="23-read_request">2.3 read_request<a class="anchorjs-link " href="#23-read_request" aria-label="Anchor link for: 23 read_request" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; /debuggerd/debuggerd.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">int</span> <span class="hljs-title">read_request</span><span class="hljs-params">(<span class="hljs-keyword">int</span> fd, debugger_request_t* out_request)</span> </span>{
  ucred cr;
  <span class="hljs-keyword">socklen_t</span> len = <span class="hljs-keyword">sizeof</span>(cr);
  <span class="hljs-comment">//從fd獲取client進程的pid,uid,gid</span>
  <span class="hljs-keyword">int</span> status = getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &amp;cr, &amp;len);
  ...
  fcntl(fd, F_SETFL, O_NONBLOCK);

  pollfd pollfds[<span class="hljs-number">1</span>];
  pollfds[<span class="hljs-number">0</span>].fd = fd;
  pollfds[<span class="hljs-number">0</span>].events = POLLIN;
  pollfds[<span class="hljs-number">0</span>].revents = <span class="hljs-number">0</span>;
  <span class="hljs-comment">//讀取tid</span>
  status = TEMP_FAILURE_RETRY(poll(pollfds, <span class="hljs-number">1</span>, <span class="hljs-number">3000</span>));
  <span class="hljs-keyword">debugger_msg_t</span> msg;
  <span class="hljs-built_in">memset</span>(&amp;msg, <span class="hljs-number">0</span>, <span class="hljs-keyword">sizeof</span>(msg));
  <span class="hljs-comment">//從fd讀取數據並保存到結構體msg</span>
  status = TEMP_FAILURE_RETRY(read(fd, &amp;msg, sizeof(msg)));
  ...

  out_request-&gt;action = <span class="hljs-keyword">static_cast</span>&lt;<span class="hljs-keyword">debugger_action_t</span>&gt;(msg.action);
  out_request-&gt;tid = msg.tid;
  out_request-&gt;pid = cr.pid;
  out_request-&gt;uid = cr.uid;
  out_request-&gt;gid = cr.gid;
  out_request-&gt;abort_msg_address = msg.abort_msg_address;
  out_request-&gt;original_si_code = msg.original_si_code;

  <span class="hljs-keyword">if</span> (msg.action == DEBUGGER_ACTION_CRASH) {
    <span class="hljs-comment">// native crash時發送過來的請求</span>
    <span class="hljs-keyword">char</span> buf[<span class="hljs-number">64</span>];
    <span class="hljs-keyword">struct</span> stat s;
    <span class="hljs-built_in">snprintf</span>(buf, <span class="hljs-keyword">sizeof</span> buf, <span class="hljs-string">"/proc/%d/task/%d"</span>, out_request-&gt;pid, out_request-&gt;tid);
    <span class="hljs-keyword">if</span> (stat(buf, &amp;s)) {
      return <span class="hljs-number">-1</span>;  <span class="hljs-comment">//tid不存在，忽略該顯式dump請求</span>
    }
  } <span class="hljs-keyword">else</span> <span class="hljs-keyword">if</span> (cr.uid == <span class="hljs-number">0</span>
            || (cr.uid == AID_SYSTEM &amp;&amp; msg.action == DEBUGGER_ACTION_DUMP_BACKTRACE)) {
    ...
  } <span class="hljs-keyword">else</span> {
    return <span class="hljs-number">-1</span>;
  }
  return <span class="hljs-number">0</span>;
}
</code></pre></div></div>

<p>read_request執行完成後，則從socket通道中讀取到out_request。</p>

<h3 id="24-worker_process">2.4 worker_process<a class="anchorjs-link " href="#24-worker_process" aria-label="Anchor link for: 24 worker_process" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>處於client發送過來的請求，server端通過子進程來處理</p>

<p>[-&gt; /debuggerd/debuggerd.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">void</span> <span class="hljs-title">worker_process</span><span class="hljs-params">(<span class="hljs-keyword">int</span> fd, debugger_request_t&amp; request)</span> </span>{
  <span class="hljs-built_in">std</span>::<span class="hljs-built_in">string</span> tombstone_path;
  <span class="hljs-keyword">int</span> tombstone_fd = <span class="hljs-number">-1</span>;
  <span class="hljs-keyword">switch</span> (request.action) {
    <span class="hljs-keyword">case</span> DEBUGGER_ACTION_CRASH:
      <span class="hljs-comment">//打開tombstone文件</span>
      tombstone_fd = open_tombstone(&amp;tombstone_path);
      <span class="hljs-keyword">if</span> (tombstone_fd == <span class="hljs-number">-1</span>) {
        <span class="hljs-built_in">exit</span>(<span class="hljs-number">1</span>); <span class="hljs-comment">//無法打開tombstone文件，則退出該進程</span>
      }
      <span class="hljs-keyword">break</span>;
    ...
  }

  <span class="hljs-comment">// Attach到目標進程</span>
  <span class="hljs-keyword">if</span> (ptrace(PTRACE_ATTACH, request.tid, <span class="hljs-number">0</span>, <span class="hljs-number">0</span>) != <span class="hljs-number">0</span>) {
    <span class="hljs-built_in">exit</span>(<span class="hljs-number">1</span>); <span class="hljs-comment">//attach失敗則退出該進程</span>
  }
  ...
  <span class="hljs-comment">//生成backtrace</span>
  <span class="hljs-built_in">std</span>::<span class="hljs-built_in">unique_ptr</span>&lt;BacktraceMap&gt; backtrace_map(BacktraceMap::Create(request.pid));

  <span class="hljs-keyword">int</span> amfd = <span class="hljs-number">-1</span>;
  <span class="hljs-built_in">std</span>::<span class="hljs-built_in">unique_ptr</span>&lt;<span class="hljs-built_in">std</span>::<span class="hljs-built_in">string</span>&gt; amfd_data;
  <span class="hljs-keyword">if</span> (request.action == DEBUGGER_ACTION_CRASH) {
    <span class="hljs-comment">//當發生native crash，則連接到AMS【見小節2.4.1】</span>
    amfd = activity_manager_connect();
    amfd_data.reset(new <span class="hljs-built_in">std</span>::<span class="hljs-built_in">string</span>);
  }

  <span class="hljs-keyword">bool</span> succeeded = <span class="hljs-literal">false</span>;

  <span class="hljs-comment">//取消特權模式</span>
  <span class="hljs-keyword">if</span> (!drop_privileges()) {
    _exit(<span class="hljs-number">1</span>); <span class="hljs-comment">//操作失敗則退出</span>
  }

  <span class="hljs-keyword">int</span> crash_signal = SIGKILL;
  <span class="hljs-comment">//執行dump操作，【見小節2.4.2】</span>
  succeeded = perform_dump(request, fd, tombstone_fd, backtrace_map.get(), siblings,
                           &amp;crash_signal, amfd_data.get());

  <span class="hljs-keyword">if</span> (!attach_gdb) {
    <span class="hljs-comment">//將進程crash情況告知AMS【見小節2.4.3】</span>
    activity_manager_write(request.pid, crash_signal, amfd, *amfd_data.get());
  }
  <span class="hljs-comment">//detach目標進程</span>
  ptrace(PTRACE_DETACH, request.tid, <span class="hljs-number">0</span>, <span class="hljs-number">0</span>);

  <span class="hljs-keyword">for</span> (<span class="hljs-keyword">pid_t</span> sibling : siblings) {
    ptrace(PTRACE_DETACH, sibling, <span class="hljs-number">0</span>, <span class="hljs-number">0</span>);
  }

  <span class="hljs-keyword">if</span> (!attach_gdb &amp;&amp; request.action == DEBUGGER_ACTION_CRASH) {
    <span class="hljs-comment">//發送信號SIGKILL給目標進程[【見小節2.4.4】</span>
    <span class="hljs-keyword">if</span> (!send_signal(request.pid, request.tid, crash_signal)) {
      ALOGE(<span class="hljs-string">"debuggerd: failed to kill process %d: %s"</span>, request.pid, strerror(errno));
    }
  }
  ...
}
</code></pre></div></div>

<p>整個過程比較複雜，下面只介紹attach_gdb=false的執行流程：</p>

<ol>
  <li>當DEBUGGER_ACTION_CRASH ，則調用open_tombstone並繼續執行；</li>
  <li>調用ptrace方法attach到目標進程;</li>
  <li>調用BacktraceMap::Create來生成backtrace;</li>
  <li>當DEBUGGER_ACTION_CRASH，則執行activity_manager_connect；</li>
  <li>調用drop_privileges來取消特權模式；</li>
  <li>通過perform_dump執行dump操作；
    <ul>
      <li>SIGBUS等致命信號，則調用<code class="highlighter-rouge">engrave_tombstone</code>()，這是核心方法</li>
    </ul>
  </li>
  <li>調用activity_manager_write，將進程crash情況告知AMS；</li>
  <li>調用ptrace方法detach到目標進程;</li>
  <li>當DEBUGGER_ACTION_CRASH，發送信號SIGKILL給目標進程tid</li>
</ol>

<h4 id="241-activity_manager_connect">2.4.1 activity_manager_connect<a class="anchorjs-link " href="#241-activity_manager_connect" aria-label="Anchor link for: 241 activity_manager_connect" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<p>[-&gt; debuggerd.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs objectivec"><span class="hljs-keyword">static</span> <span class="hljs-keyword">int</span> activity_manager_connect() {
  android::base::unique_fd amfd(socket(PF_UNIX, SOCK_STREAM, <span class="hljs-number">0</span>));
  <span class="hljs-keyword">if</span> (amfd.get() &lt; <span class="hljs-number">-1</span>) {
    <span class="hljs-keyword">return</span> <span class="hljs-number">-1</span>; <span class="hljs-comment">///無法連接到ActivityManager(socket失敗)</span>
  }

  <span class="hljs-keyword">struct</span> sockaddr_un address;
  memset(&amp;address, <span class="hljs-number">0</span>, <span class="hljs-keyword">sizeof</span>(address));
  address.sun_family = AF_UNIX;
  <span class="hljs-comment">//該路徑必須匹配NativeCrashListener.java中的定義</span>
  strncpy(address.sun_path, <span class="hljs-string">"/data/system/ndebugsocket"</span>, <span class="hljs-keyword">sizeof</span>(address.sun_path));
  <span class="hljs-keyword">if</span> (TE<span class="hljs-built_in">MP_FAILURE_RETRY</span>(connect(amfd.get(), reinterpret_cast&lt;<span class="hljs-keyword">struct</span> sockaddr*&gt;(&amp;address),
                                 <span class="hljs-keyword">sizeof</span>(address))) == <span class="hljs-number">-1</span>) {
    <span class="hljs-keyword">return</span> <span class="hljs-number">-1</span>;  <span class="hljs-comment">//無法連接到ActivityManager(connect失敗)</span>
  }

  <span class="hljs-keyword">struct</span> timeval tv;
  memset(&amp;tv, <span class="hljs-number">0</span>, <span class="hljs-keyword">sizeof</span>(tv));
  tv.tv_sec = <span class="hljs-number">1</span>;
  <span class="hljs-keyword">if</span> (setsockopt(amfd.get(), SOL_SOCKET, SO_SNDTIMEO, &amp;tv, <span class="hljs-keyword">sizeof</span>(tv)) == <span class="hljs-number">-1</span>) {
    <span class="hljs-keyword">return</span> <span class="hljs-number">-1</span>; <span class="hljs-comment">//無法連接到ActivityManager(setsockopt SO_SNDTIMEO失敗)</span>
  }

  tv.tv_sec = <span class="hljs-number">3</span>;
  <span class="hljs-keyword">if</span> (setsockopt(amfd.get(), SOL_SOCKET, SO_RCVTIMEO, &amp;tv, <span class="hljs-keyword">sizeof</span>(tv)) == <span class="hljs-number">-1</span>) {
    <span class="hljs-keyword">return</span> <span class="hljs-number">-1</span>; <span class="hljs-comment">//無法連接到ActivityManager(setsockopt SO_RCVTIMEO失敗)</span>
  }

  <span class="hljs-keyword">return</span> amfd.release();
}
</code></pre></div></div>

<p>該方法的功能是建立跟上層<code class="highlighter-rouge">ActivityManager</code>的socket連接。對於”/data/system/ndebugsocket”的服務端是在，NativeCrashListener.java方法中創建並啟動的。</p>

<h4 id="242-perform_dump">2.4.2 perform_dump<a class="anchorjs-link " href="#242-perform_dump" aria-label="Anchor link for: 242 perform_dump" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>
<p>根據接收到不同的signal採取相應的操作</p>

<p>[-&gt; debuggerd.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">bool</span> <span class="hljs-title">perform_dump</span><span class="hljs-params">(<span class="hljs-keyword">const</span> debugger_request_t&amp; request, <span class="hljs-keyword">int</span> fd, <span class="hljs-keyword">int</span> tombstone_fd,
                         BacktraceMap* backtrace_map, <span class="hljs-keyword">const</span> <span class="hljs-built_in">std</span>::<span class="hljs-built_in">set</span>&lt;pid_t&gt;&amp; siblings,
                         <span class="hljs-keyword">int</span>* crash_signal, <span class="hljs-built_in">std</span>::<span class="hljs-built_in">string</span>* amfd_data)</span> </span>{
  <span class="hljs-keyword">if</span> (TEMP_FAILURE_RETRY(write(fd, <span class="hljs-string">"\0"</span>, <span class="hljs-number">1</span>)) != <span class="hljs-number">1</span>) {
    return <span class="hljs-literal">false</span>; <span class="hljs-comment">//無法響應client端請求</span>
  }

  <span class="hljs-keyword">int</span> total_sleep_time_usec = <span class="hljs-number">0</span>;
  <span class="hljs-keyword">while</span> (<span class="hljs-literal">true</span>) {
    <span class="hljs-comment">//等待信號到來</span>
    <span class="hljs-keyword">int</span> signal = wait_for_signal(request.tid, &amp;total_sleep_time_usec);
    <span class="hljs-keyword">switch</span> (signal) {
      ...

      <span class="hljs-keyword">case</span> SIGABRT:
      <span class="hljs-keyword">case</span> SIGBUS:
      <span class="hljs-keyword">case</span> SIGFPE:
      <span class="hljs-keyword">case</span> SIGILL:
      <span class="hljs-keyword">case</span> SIGSEGV:
<span class="hljs-meta">#<span class="hljs-meta-keyword">ifdef</span> SIGSTKFLT</span>
      <span class="hljs-keyword">case</span> SIGSTKFLT:
<span class="hljs-meta">#<span class="hljs-meta-keyword">endif</span></span>
      <span class="hljs-keyword">case</span> SIGTRAP:
        ALOGV(<span class="hljs-string">"stopped -- fatal signal\n"</span>);

        *crash_signal = signal;
        <span class="hljs-comment">//這是輸出tombstone信息最為核心的方法</span>
        engrave_tombstone(tombstone_fd, backtrace_map, request.pid, request.tid, siblings, signal,
                          request.original_si_code, request.abort_msg_address, amfd_data);
        <span class="hljs-keyword">break</span>;

      <span class="hljs-keyword">default</span>:
        ALOGE(<span class="hljs-string">"debuggerd: process stopped due to unexpected signal %d\n"</span>, signal);
        <span class="hljs-keyword">break</span>;
    }
    <span class="hljs-keyword">break</span>;
  }

  return <span class="hljs-literal">true</span>;
}
</code></pre></div></div>

<p>對於以下信號都是致命的信號:</p>

<ul>
  <li>SIGABRT：abort退出異常</li>
  <li>SIGBUS：硬件訪問異常</li>
  <li>SIGFPE：浮點運算異常</li>
  <li>SIGILL：非法指令異常</li>
  <li>SIGSEGV：內存訪問異常</li>
  <li>SIGSTKFLT：協處理器棧異常</li>
  <li>SIGTRAP：陷阱異常</li>
</ul>

<p>另外，上篇文章已介紹過<a href="http://gityuan.com/2016/06/15/android-debuggerd/#tombstone">engrave_tombstone</a>的功能內容，這裡就不再累贅了。</p>

<h4 id="243-activity_manager_write">2.4.3 activity_manager_write<a class="anchorjs-link " href="#243-activity_manager_write" aria-label="Anchor link for: 243 activity_manager_write" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<p>[-&gt; debuggerd.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs php"><span class="hljs-keyword">static</span> void activity_manager_write(int pid, int signal, int amfd, <span class="hljs-keyword">const</span> std::string&amp; amfd_data) {
  <span class="hljs-keyword">if</span> (amfd == <span class="hljs-number">-1</span>) {
    <span class="hljs-keyword">return</span>;
  }

  <span class="hljs-comment">//寫入pid和signal，以及原始dump信息，最後添加0以標記結束</span>
  uint32_t datum = htonl(pid);
  <span class="hljs-keyword">if</span> (!android::base::WriteFully(amfd, &amp;datum, <span class="hljs-number">4</span>)) {
    <span class="hljs-keyword">return</span>; <span class="hljs-comment">//AM pid寫入失敗</span>
  }
  datum = htonl(signal);
  <span class="hljs-keyword">if</span> (!android::base::WriteFully(amfd, &amp;datum, <span class="hljs-number">4</span>)) {
    <span class="hljs-keyword">return</span>;<span class="hljs-comment">//AM signal寫入失敗</span>
  }

  <span class="hljs-keyword">if</span> (!android::base::WriteFully(amfd, amfd_data.c_str(), amfd_data.size())) {
    <span class="hljs-keyword">return</span>;<span class="hljs-comment">//AM data寫入失敗</span>
  }

  uint8_t eodMarker = <span class="hljs-number">0</span>;
  <span class="hljs-keyword">if</span> (!android::base::WriteFully(amfd, &amp;eodMarker, <span class="hljs-number">1</span>)) {
    <span class="hljs-keyword">return</span>; <span class="hljs-comment">//AM eod 寫入失敗</span>
  }
  <span class="hljs-comment">//讀取應答消息，如果3s超時未收到則讀取失敗</span>
  android::base::ReadFully(amfd, &amp;eodMarker, <span class="hljs-number">1</span>);
}
</code></pre></div></div>

<p>debuggerd與AMS的NativeCrashListener建立socket連接後，再通過該方法發送數據，數據項包括pid、signal、dump信息。</p>

<h4 id="244-send_signal">2.4.4 send_signal<a class="anchorjs-link " href="#244-send_signal" aria-label="Anchor link for: 244 send_signal" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<p>此處只是向目標進程發送SIGKILL信號，用於殺掉目標進程，文章<a href="http://gityuan.com/2016/04/16/kill-signal/#sendsignal">理解殺進程的實現原理</a>已詳細講述過發送SIGKILL信號的處理流程。</p>

<h3 id="25-monitor_worker_process">2.5 monitor_worker_process<a class="anchorjs-link " href="#25-monitor_worker_process" aria-label="Anchor link for: 25 monitor_worker_process" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs bash">static void monitor_worker_process(int child_pid, const debugger_request_t&amp; request) {
  struct timespec timeout = {.tv_sec = 10, .tv_nsec = 0 };
  <span class="hljs-keyword">if</span> (should_attach_gdb(request)) {
    //如果使能<span class="hljs-built_in">wait</span>_<span class="hljs-keyword">for</span>_gdb，則將timeout設置為非常大
    timeout.tv_sec = INT_MAX;
  }
  sigset_t signal_<span class="hljs-built_in">set</span>;
  sigemptyset(&amp;signal_<span class="hljs-built_in">set</span>);
  sigaddset(&amp;signal_<span class="hljs-built_in">set</span>, SIGCHLD);
  bool <span class="hljs-built_in">kill</span>_worker = <span class="hljs-literal">false</span>;
  bool <span class="hljs-built_in">kill</span>_target = <span class="hljs-literal">false</span>;
  bool <span class="hljs-built_in">kill</span>_self = <span class="hljs-literal">false</span>;
  int status;
  siginfo_t siginfo;
  int signal = TEMP_FAILURE_RETRY(sigtimedwait(&amp;signal_<span class="hljs-built_in">set</span>, &amp;siginfo, &amp;timeout));
  <span class="hljs-keyword">if</span> (signal == SIGCHLD) {
    pid_t rc = waitpid(-1, &amp;status, WNOHANG | WUNTRACED);
    <span class="hljs-keyword">if</span> (rc != child_pid) {
      ALOGE(<span class="hljs-string">"debuggerd: waitpid returned unexpected pid (%d), committing murder-suicide"</span>, rc);
      <span class="hljs-keyword">if</span> (WIFEXITED(status)) {
        ALOGW(<span class="hljs-string">"debuggerd: pid %d exited with status %d"</span>, rc, WEXITSTATUS(status));
      } <span class="hljs-keyword">else</span> <span class="hljs-keyword">if</span> (WIFSIGNALED(status)) {
        ALOGW(<span class="hljs-string">"debuggerd: pid %d received signal %d"</span>, rc, WTERMSIG(status));
      } <span class="hljs-keyword">else</span> <span class="hljs-keyword">if</span> (WIFSTOPPED(status)) {
        ALOGW(<span class="hljs-string">"debuggerd: pid %d stopped by signal %d"</span>, rc, WSTOPSIG(status));
      } <span class="hljs-keyword">else</span> <span class="hljs-keyword">if</span> (WIFCONTINUED(status)) {
        ALOGW(<span class="hljs-string">"debuggerd: pid %d continued"</span>, rc);
      }
      <span class="hljs-built_in">kill</span>_worker = <span class="hljs-literal">true</span>;
      <span class="hljs-built_in">kill</span>_target = <span class="hljs-literal">true</span>;
      <span class="hljs-built_in">kill</span>_self = <span class="hljs-literal">true</span>;
    } <span class="hljs-keyword">else</span> <span class="hljs-keyword">if</span> (WIFSIGNALED(status)) {
      ALOGE(<span class="hljs-string">"debuggerd: worker process %d terminated due to signal %d"</span>, child_pid, WTERMSIG(status));
      <span class="hljs-built_in">kill</span>_worker = <span class="hljs-literal">false</span>;
      <span class="hljs-built_in">kill</span>_target = <span class="hljs-literal">true</span>;
    } <span class="hljs-keyword">else</span> <span class="hljs-keyword">if</span> (WIFSTOPPED(status)) {
      ALOGE(<span class="hljs-string">"debuggerd: worker process %d stopped due to signal %d"</span>, child_pid, WSTOPSIG(status));
      <span class="hljs-built_in">kill</span>_worker = <span class="hljs-literal">true</span>;
      <span class="hljs-built_in">kill</span>_target = <span class="hljs-literal">true</span>;
    }
  } <span class="hljs-keyword">else</span> {
    ALOGE(<span class="hljs-string">"debuggerd: worker process %d timed out"</span>, child_pid);
    <span class="hljs-built_in">kill</span>_worker = <span class="hljs-literal">true</span>;
    <span class="hljs-built_in">kill</span>_target = <span class="hljs-literal">true</span>;
  }
</code></pre></div></div>

<p>該方法是運行在debuggerd父進程，用於監控子進程的執行情況。</p>

<h3 id="26-小結">2.6 小結<a class="anchorjs-link " href="#26-小結" aria-label="Anchor link for: 26 小結" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>debuggerd服務端調用鏈：</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs css"><span class="hljs-selector-tag">do_server</span>
    <span class="hljs-selector-tag">handle_request</span>
        <span class="hljs-selector-tag">read_request</span>
        <span class="hljs-selector-tag">worker_process</span>(子進程執行)
            <span class="hljs-selector-tag">open_tombstone</span>
            <span class="hljs-selector-tag">ptrace</span>(<span class="hljs-selector-tag">PTRACE_ATTACH</span>, <span class="hljs-selector-tag">request</span><span class="hljs-selector-class">.tid</span>, 0, 0)
            <span class="hljs-selector-tag">backtrace_map</span>
            <span class="hljs-selector-tag">activity_manager_connect</span>
            <span class="hljs-selector-tag">perform_dump</span>
            <span class="hljs-selector-tag">activity_manager_write</span>
            <span class="hljs-selector-tag">ptrace</span>(<span class="hljs-selector-tag">PTRACE_DETACH</span>, <span class="hljs-selector-tag">request</span><span class="hljs-selector-class">.tid</span>, 0, 0);
            <span class="hljs-selector-tag">send_signal</span>
        <span class="hljs-selector-tag">monitor_worker_process</span>(父進程執行)
</code></pre></div></div>

<p>handle_request方法中通過fork機制，創建子進程來執行worker_process，由於fork返回兩次，返回到父進程則執行monitor_worker_process。</p>

<h2 id="三nativecrashlistener">三、NativeCrashListener<a class="anchorjs-link " href="#三nativecrashlistener" aria-label="Anchor link for: 三nativecrashlistener" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h2>

<h3 id="31-startotherservices">3.1 startOtherServices<a class="anchorjs-link " href="#31-startotherservices" aria-label="Anchor link for: 31 startotherservices" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; SystemServer.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">private</span> <span class="hljs-keyword">void</span> <span class="hljs-title">startOtherServices</span><span class="hljs-params">()</span> </span>{
    ...
    mActivityManagerService.systemReady(<span class="hljs-keyword">new</span> Runnable() {
       <span class="hljs-meta">@Override</span>
       <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">run</span><span class="hljs-params">()</span> </span>{
           mSystemServiceManager.startBootPhase(
                   SystemService.PHASE_ACTIVITY_MANAGER_READY);
           <span class="hljs-keyword">try</span> {
               <span class="hljs-comment">//【見小節3.2】</span>
               mActivityManagerService.startObservingNativeCrashes();
           } <span class="hljs-keyword">catch</span> (Throwable e) {
               reportWtf(<span class="hljs-string">"observing native crashes"</span>, e);
           }
        }
    }
}
</code></pre></div></div>

<p>當開機過程中啟動服務啟動到階段<code class="highlighter-rouge">PHASE_ACTIVITY_MANAGER_READY</code>(550)，即服務可以廣播自己的Intents，然後啟動native crash的監聽進程。</p>

<h3 id="32-startobservingnativecrashes">3.2 startObservingNativeCrashes<a class="anchorjs-link " href="#32-startobservingnativecrashes" aria-label="Anchor link for: 32 startobservingnativecrashes" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; ActivityManagerService.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">startObservingNativeCrashes</span><span class="hljs-params">()</span> </span>{
    <span class="hljs-comment">//【見小節3.3】</span>
    <span class="hljs-keyword">final</span> NativeCrashListener ncl = <span class="hljs-keyword">new</span> NativeCrashListener(<span class="hljs-keyword">this</span>);
    ncl.start();
}
</code></pre></div></div>

<p>NativeCrashListener繼承於<code class="highlighter-rouge">Thread</code>，可見這是線程，通過調用start方法來啟動線程開始工作。</p>

<h3 id="33-nativecrashlistener">3.3 NativeCrashListener<a class="anchorjs-link " href="#33-nativecrashlistener" aria-label="Anchor link for: 33 nativecrashlistener" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; NativeCrashListener.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">run</span><span class="hljs-params">()</span> </span>{
    <span class="hljs-keyword">final</span> <span class="hljs-keyword">byte</span>[] ackSignal = <span class="hljs-keyword">new</span> <span class="hljs-keyword">byte</span>[<span class="hljs-number">1</span>];
    {
        <span class="hljs-comment">//此處DEBUGGERD_SOCKET_PATH= "/data/system/ndebugsocket"</span>
        File socketFile = <span class="hljs-keyword">new</span> File(DEBUGGERD_SOCKET_PATH);
        <span class="hljs-keyword">if</span> (socketFile.exists()) {
            socketFile.delete();
        }
    }

    <span class="hljs-keyword">try</span> {
        FileDescriptor serverFd = Os.socket(AF_UNIX, SOCK_STREAM, <span class="hljs-number">0</span>);
        <span class="hljs-comment">//創建socket服務端</span>
        <span class="hljs-keyword">final</span> UnixSocketAddress sockAddr = UnixSocketAddress.createFileSystem(
                DEBUGGERD_SOCKET_PATH);
        Os.bind(serverFd, sockAddr);
        Os.listen(serverFd, <span class="hljs-number">1</span>);

        <span class="hljs-keyword">while</span> (<span class="hljs-keyword">true</span>) {
            FileDescriptor peerFd = <span class="hljs-keyword">null</span>;
            <span class="hljs-keyword">try</span> {
                <span class="hljs-comment">//等待debuggerd建立連接</span>
                peerFd = Os.accept(serverFd, <span class="hljs-keyword">null</span> <span class="hljs-comment">/* peerAddress */</span>);
                <span class="hljs-comment">//獲取debuggerd的socket文件描述符</span>
                <span class="hljs-keyword">if</span> (peerFd != <span class="hljs-keyword">null</span>) {
                    <span class="hljs-comment">//只有超級用戶才被允許通過該socket進行通信</span>
                    StructUcred credentials =
                            Os.getsockoptUcred(peerFd, SOL_SOCKET, SO_PEERCRED);
                    <span class="hljs-keyword">if</span> (credentials.uid == <span class="hljs-number">0</span>) {
                        <span class="hljs-comment">//【見小節3.4】處理native crash信息</span>
                        consumeNativeCrashData(peerFd);
                    }
                }
            } <span class="hljs-keyword">catch</span> (Exception e) {
                Slog.w(TAG, <span class="hljs-string">"Error handling connection"</span>, e);
            } <span class="hljs-keyword">finally</span> {
                <span class="hljs-comment">//應答debuggerd已經建立連接</span>
                <span class="hljs-keyword">if</span> (peerFd != <span class="hljs-keyword">null</span>) {
                    Os.write(peerFd, ackSignal, <span class="hljs-number">0</span>, <span class="hljs-number">1</span>);<span class="hljs-comment">//寫入應答消息</span>
                    Os.close(peerFd);<span class="hljs-comment">//關閉socket</span>
                    ...
                }
            }
        }
    } <span class="hljs-keyword">catch</span> (Exception e) {
        Slog.e(TAG, <span class="hljs-string">"Unable to init native debug socket!"</span>, e);
    }
}
</code></pre></div></div>

<p>該方法主要功能：</p>

<ol>
  <li>創建socket服務端：”/data/system/ndebugsocket”文件權限700，owned為system:system，debuggerd是以root權限運行，因此可以與該socket建立連接，但對於第三方App則沒有權限；</li>
  <li>等待socket客戶端(即debuggerd)來建立連接；</li>
  <li>調用consumeNativeCrashData來處理native crash信息；</li>
  <li>應答debuggerd已經建立連接，並寫入應答消息告知debuggerd進程。</li>
</ol>

<h3 id="34-consumenativecrashdata">3.4 consumeNativeCrashData<a class="anchorjs-link " href="#34-consumenativecrashdata" aria-label="Anchor link for: 34 consumenativecrashdata" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>
<p>[-&gt; NativeCrashListener.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">consumeNativeCrashData</span><span class="hljs-params">(FileDescriptor fd)</span> </span>{
    <span class="hljs-comment">//進入該方法，標識著debuggerd已經與AMS建立連接</span>
    <span class="hljs-keyword">final</span> <span class="hljs-keyword">byte</span>[] buf = <span class="hljs-keyword">new</span> <span class="hljs-keyword">byte</span>[<span class="hljs-number">4096</span>];
    <span class="hljs-keyword">final</span> ByteArrayOutputStream os = <span class="hljs-keyword">new</span> ByteArrayOutputStream(<span class="hljs-number">4096</span>);

    <span class="hljs-keyword">try</span> {
        <span class="hljs-comment">//此處SOCKET_TIMEOUT_MILLIS=2s</span>
        StructTimeval timeout = StructTimeval.fromMillis(SOCKET_TIMEOUT_MILLIS);
        Os.setsockoptTimeval(fd, SOL_SOCKET, SO_RCVTIMEO, timeout);
        Os.setsockoptTimeval(fd, SOL_SOCKET, SO_SNDTIMEO, timeout);

        <span class="hljs-comment">//1.讀取pid和signal number</span>
        <span class="hljs-keyword">int</span> headerBytes = readExactly(fd, buf, <span class="hljs-number">0</span>, <span class="hljs-number">8</span>);
        <span class="hljs-keyword">if</span> (headerBytes != <span class="hljs-number">8</span>) {
            <span class="hljs-keyword">return</span>; <span class="hljs-comment">//讀取失敗</span>
        }

        <span class="hljs-keyword">int</span> pid = unpackInt(buf, <span class="hljs-number">0</span>);
        <span class="hljs-keyword">int</span> signal = unpackInt(buf, <span class="hljs-number">4</span>);

        <span class="hljs-comment">//2.讀取dump內容</span>
        <span class="hljs-keyword">if</span> (pid &gt; <span class="hljs-number">0</span>) {
            <span class="hljs-keyword">final</span> ProcessRecord pr;
            <span class="hljs-keyword">synchronized</span> (mAm.mPidsSelfLocked) {
                pr = mAm.mPidsSelfLocked.get(pid);
            }
            <span class="hljs-keyword">if</span> (pr != <span class="hljs-keyword">null</span>) {
                <span class="hljs-comment">//persistent應用，直接忽略</span>
                <span class="hljs-keyword">if</span> (pr.persistent) {
                    <span class="hljs-keyword">return</span>;
                }

                <span class="hljs-keyword">int</span> bytes;
                do {
                    <span class="hljs-comment">//獲取數據</span>
                    bytes = Os.read(fd, buf, <span class="hljs-number">0</span>, buf.length);
                    <span class="hljs-keyword">if</span> (bytes &gt; <span class="hljs-number">0</span>) {
                        <span class="hljs-keyword">if</span> (buf[bytes-<span class="hljs-number">1</span>] == <span class="hljs-number">0</span>) {
                            <span class="hljs-comment">//到達文件EOD, 忽略該字節</span>
                            os.write(buf, <span class="hljs-number">0</span>, bytes-<span class="hljs-number">1</span>);
                            <span class="hljs-keyword">break</span>;
                        }
                        os.write(buf, <span class="hljs-number">0</span>, bytes);
                    }
                } <span class="hljs-keyword">while</span> (bytes &gt; <span class="hljs-number">0</span>);

                <span class="hljs-keyword">synchronized</span> (mAm) {
                    pr.crashing = <span class="hljs-keyword">true</span>;
                    pr.forceCrashReport = <span class="hljs-keyword">true</span>;
                }

                <span class="hljs-keyword">final</span> String reportString = <span class="hljs-keyword">new</span> String(os.toByteArray(), <span class="hljs-string">"UTF-8"</span>);
                <span class="hljs-comment">//異常處理native crash報告【見小節3.5】</span>
                (<span class="hljs-keyword">new</span> NativeCrashReporter(pr, signal, reportString)).start();
            }
        }
    } <span class="hljs-keyword">catch</span> (Exception e) {
        Slog.e(TAG, <span class="hljs-string">"Exception dealing with report"</span>, e);
    }
}
</code></pre></div></div>

<p>讀取debuggerd那端發送過來的數據，再通過NativeCrashReporter來把native crash事件報告給framework層。</p>

<h3 id="35-nativecrashreporter">3.5 NativeCrashReporter<a class="anchorjs-link " href="#35-nativecrashreporter" aria-label="Anchor link for: 35 nativecrashreporter" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; NativeCrashListener.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-class"><span class="hljs-keyword">class</span> <span class="hljs-title">NativeCrashReporter</span> <span class="hljs-keyword">extends</span> <span class="hljs-title">Thread</span> </span>{
    <span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">run</span><span class="hljs-params">()</span> </span>{
        <span class="hljs-keyword">try</span> {
            CrashInfo ci = <span class="hljs-keyword">new</span> CrashInfo();
            ci.exceptionClassName = <span class="hljs-string">"Native crash"</span>;
            ci.exceptionMessage = Os.strsignal(mSignal);
            ci.throwFileName = <span class="hljs-string">"unknown"</span>;
            ci.throwClassName = <span class="hljs-string">"unknown"</span>;
            ci.throwMethodName = <span class="hljs-string">"unknown"</span>;
            ci.stackTrace = mCrashReport;
            <span class="hljs-comment">//AMS真正處理crash的過程</span>
            mAm.handleApplicationCrashInner(<span class="hljs-string">"native_crash"</span>, mApp, mApp.processName, ci);
        } <span class="hljs-keyword">catch</span> (Exception e) {
            Slog.e(TAG, <span class="hljs-string">"Unable to report native crash"</span>, e);
        }
    }
}
</code></pre></div></div>

<p>不論是Native crash還是framework crash最終都會調用到<code class="highlighter-rouge">handleApplicationCrashInner()</code>，該方法見文章<a href="http://gityuan.com/2016/06/24/app-crash/#handleApplicationCrashInner">理解Android Crash處理流程</a>。</p>

<h3 id="36-小結">3.6 小結<a class="anchorjs-link " href="#36-小結" aria-label="Anchor link for: 36 小結" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>system_server進程啟動過程中，調用<code class="highlighter-rouge">startOtherServices</code>來啟動各種其他系統Service時，也正是這個時機會創建一個用於監聽native crash事件的NativeCrashListener對象(繼承於線程)，通過socket機制來監聽，等待即debuggerd與該線程創建連接，並處理相應事件。緊接著調用<code class="highlighter-rouge">handleApplicationCrashInner</code>來處理crash流程。</p>

<p>NativeCrashListener的主要工作：</p>

<ol>
  <li>創建socket服務端”/data/system/ndebugsocket”</li>
  <li>等待socket客戶端(即debuggerd)來建立連接；</li>
  <li>調用consumeNativeCrashData來處理native crash信息；</li>
  <li>應答debuggerd已經建立連接，並寫入應答消息告知debuggerd進程。</li>
</ol>

<h2 id="四總結">四、總結<a class="anchorjs-link " href="#四總結" aria-label="Anchor link for: 四總結" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h2>

<p>點擊查看<a href="http://gityuan.com/images/stability/native_crash.jpg">大圖</a></p>

<p><img src="./images/native_crash.jpg" alt="native_crash"></p>

<p>Native程序通過link連接後，當發生Native Crash時，則kernel會發送相應的<code class="highlighter-rouge">signal</code>，當進程捕獲致命的<code class="highlighter-rouge">signal</code>，通知<code class="highlighter-rouge">debuggerd</code>調用<code class="highlighter-rouge">ptrace</code>來獲取有價值的信息(這是發生在crash前)。</p>

<ol>
  <li>kernel 發送signal給target進程(包含native代碼)；</li>
  <li>target進程通過debuggerd_signal_handler，捕獲signal；
    <ul>
      <li>建立於debuggerd進程的socket通道；</li>
      <li>將action = DEBUGGER_ACTION_CRASH的消息發送給debuggerd服務端；</li>
      <li>阻塞等待debuggerd服務端的迴應數據。</li>
    </ul>
  </li>
  <li>debuggerd作為守護進程，一直在等待socket client的連接，此時收到action = DEBUGGER_ACTION_CRASH的消息；</li>
  <li>執行到handle_request時，通過fork創建子進程來執行各種dump相關操作；</li>
  <li>新創建的進程，通過socket與system_server進程中的NativeCrashListener線程建立socket通道，並向其發送native crash信息；</li>
  <li>NativeCrashListener線程通過創建新的名為“NativeCrashReport”的子線程來執行AMS的handleApplicationCrashInner方法。</li>
</ol>

<p>這個流程圖只是從整體來概要介紹native crash流程，其中有兩個部分是核心方法：</p>

<ul>
  <li>其一是圖中紅色塊<code class="highlighter-rouge">perform_dump</code>是整個debuggerd的核心工作，該方法內部調用<code class="highlighter-rouge">engrave_tombstone</code>，該方法的具體工作見文章<a href="http://gityuan.com/2016/06/15/android-debuggerd/#tombstone">ebuggerd守護進程</a>的功能內容，這個過程還需要與target進程通信來獲取target進程更多信息。</li>
  <li>其二是AMS的handleApplicationCrashInner，該方法的工作見姊妹篇<a href="http://gityuan.com/2016/06/24/app-crash/#handleApplicationCrashInner">理解Android Crash處理流程</a>。</li>
</ul>


   