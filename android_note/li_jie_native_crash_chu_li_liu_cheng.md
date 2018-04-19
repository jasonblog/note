# 理解Native Crash处理流程


<div class="row">

    <!-- Post Container -->
            <div class="
                col-lg-8 col-lg-offset-2
                col-md-10 col-md-offset-1
                post-container">

								<blockquote>
  <p>本文是基于Android 7.0源码，来分析Native Crash流程。</p>
</blockquote>

<h2 id="一native-crash">一、Native Crash<a class="anchorjs-link " href="#一native-crash" aria-label="Anchor link for: 一native crash" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h2>

<p>从系统全局来说，Crash分为Framework/App Crash， Native Crash，以及Kernel Crash。</p>

<ul>
  <li>对于framework层或者app层的Crash(即Java层面Crash)，那么往往是通过抛出未捕获异常而导致的Crash，这个内容在本文的姊妹篇<a href="http://gityuan.com/2016/06/24/app-crash/">理解Android Crash处理流程</a>已详细介绍过。</li>
  <li>至于Kernel Crash，很多情况是发生Kernel panic，对于内核崩溃往往是驱动或者硬件出现故障。</li>
  <li>Native Crash，即C/C++层面的Crash，这是介于系统framework层与Linux层之间的一层，这是本文接下来要讲解的内容。</li>
</ul>

<p>如果你是从事Android系统开发或者架构相关工作，或者遇到需要解系统性的疑难杂症，再或者需要写JNI代码，则就有可能遇到Native Crash，了解系统Native Crash处理流程就很有必要。</p>

<p>接下来介绍介绍<code class="highlighter-rouge">Android N</code>的Native Crash处理流程，你没有看错，本文就是针对最新Android Nouget来分析的。Native crash的工作核心是由debuggerd守护进程来完成，在文章<a href="http://gityuan.com/2016/06/15/android-debuggerd/">调试系列4：debuggerd源码篇)</a>，已经介绍过Debuggerdd的工作原理。</p>

<p>要了解Native Crash，首先从应用程序入口位于<code class="highlighter-rouge">begin.S</code>中的<code class="highlighter-rouge">__linker_init</code>入手。</p>

<h3 id="11-begins">1.1 begin.S<a class="anchorjs-link " href="#11-begins" aria-label="Anchor link for: 11 begins" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; arch/arm/begin.S]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs objectivec">ENTRY(_start)
  mov r0, sp
  <span class="hljs-comment">//入口地址 【见小节1.2】</span>
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
  <span class="hljs-comment">//【见小节1.3】</span>
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
  <span class="hljs-comment">//【见小节1.4】</span>
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
  <span class="hljs-comment">//【见小节1.5】</span>
  action.sa_sigaction = debuggerd_signal_handler;
  <span class="hljs-comment">//SA_RESTART代表中断某个syscall，则会自动重新调用该syscall</span>
  <span class="hljs-comment">//SA_SIGINFO代表信号附带参数siginfo_t结构体可传送到signal_handler函数</span>
  action.sa_flags = SA_RESTART | SA_SIGINFO;
  <span class="hljs-comment">//使用备用signal栈(如果可用)，以便我们能捕获栈溢出</span>
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

<p>连接到bionic上的native程序(C/C++)出现异常时，kernel会发送相应的signal；
当进程捕获致命的signal，通知debuggerd调用ptrace来获取有价值的信息(发生crash之前)。</p>

<p>[-&gt; linker/debugger.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">void</span> <span class="hljs-title">debuggerd_signal_handler</span><span class="hljs-params">(<span class="hljs-keyword">int</span> signal_number, siginfo_t* info, <span class="hljs-keyword">void</span>*)</span> </span>{
  <span class="hljs-keyword">if</span> (!have_siginfo(signal_number)) {
    info = <span class="hljs-literal">nullptr</span>; <span class="hljs-comment">//SA_SIGINFO标识被意外清空，则info未定义</span>
  }
  <span class="hljs-comment">//防止debuggerd无法链接时，仍可以输出一些简要signal信息</span>
  log_signal_summary(signal_number, info);
  <span class="hljs-comment">//建立于debuggerd的socket通信连接 【见小节1.6】</span>
  send_debuggerd_packet(info);
  <span class="hljs-comment">//重置信号处理函数为SIG_DFL(默认操作)</span>
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
  <span class="hljs-comment">// Mutex防止多个crashing线程同一时间来来尝试跟debuggerd进行通信</span>
  <span class="hljs-keyword">static</span> <span class="hljs-keyword">pthread_mutex_t</span> crash_mutex = PTHREAD_MUTEX_INITIALIZER;
  <span class="hljs-keyword">int</span> ret = pthread_mutex_trylock(&amp;crash_mutex);
  <span class="hljs-keyword">if</span> (ret != <span class="hljs-number">0</span>) {
    <span class="hljs-keyword">if</span> (ret == EBUSY) {
      __libc_format_log(ANDROID_LOG_INFO, <span class="hljs-string">"libc"</span>,
          <span class="hljs-string">"Another thread contacted debuggerd first; not contacting debuggerd."</span>);
      <span class="hljs-comment">//等待其他线程释放该锁，从而获取该锁</span>
      pthread_mutex_lock(&amp;crash_mutex);
    }
    return;
  }
  <span class="hljs-comment">//建立与debuggerd的socket通道</span>
  <span class="hljs-keyword">int</span> s = socket_abstract_client(DEBUGGER_SOCKET_NAME, SOCK_STREAM | SOCK_CLOEXEC);
  ...
  <span class="hljs-keyword">debugger_msg_t</span> msg;
  msg.action = DEBUGGER_ACTION_CRASH;
  msg.tid = gettid();
  msg.abort_msg_address = <span class="hljs-keyword">reinterpret_cast</span>&lt;<span class="hljs-keyword">uintptr_t</span>&gt;(g_abort_message);
  msg.original_si_code = (info != nullptr) ? info-&gt;si_code : <span class="hljs-number">0</span>;
  <span class="hljs-comment">//将DEBUGGER_ACTION_CRASH消息发送给debuggerd服务端</span>
  ret = TEMP_FAILURE_RETRY(write(s, &amp;msg, sizeof(msg)));
  <span class="hljs-keyword">if</span> (ret == <span class="hljs-keyword">sizeof</span>(msg)) {
    <span class="hljs-keyword">char</span> debuggerd_ack;
    <span class="hljs-comment">//阻塞等待debuggerd服务端的回应数据</span>
    ret = TEMP_FAILURE_RETRY(read(s, &amp;debuggerd_ack, <span class="hljs-number">1</span>));
    <span class="hljs-keyword">int</span> saved_errno = errno;
    notify_gdb_of_libraries();
    errno = saved_errno;
  }
  close(s);
}
</code></pre></div></div>

<p>该方法的主要功能：</p>

<ul>
  <li>调用socket_abstract_client，建立于debuggerd的socket通道；</li>
  <li>将<code class="highlighter-rouge">action = DEBUGGER_ACTION_CRASH</code>的消息发送给debuggerd服务端；</li>
  <li>阻塞等待debuggerd服务端的回应数据。</li>
</ul>

<p>接下来，看看debuggerd服务端接收到<code class="highlighter-rouge">DEBUGGER_ACTION_CRASH</code>的处理流程</p>

<h2 id="二debuggerd服务端">二、debuggerd服务端<a class="anchorjs-link " href="#二debuggerd服务端" aria-label="Anchor link for: 二debuggerd服务端" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h2>

<p>debuggerd 守护进程启动后，一直在等待socket client的连接。当native crash发送后便会向debuggerd发送<code class="highlighter-rouge">action = DEBUGGER_ACTION_CRASH</code>的消息。</p>

<h3 id="21-do_server">2.1 do_server<a class="anchorjs-link " href="#21-do_server" aria-label="Anchor link for: 21 do_server" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; /debuggerd/debuggerd.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">int</span> <span class="hljs-title">do_server</span><span class="hljs-params">()</span> </span>{
  ...
  <span class="hljs-keyword">for</span> (;;) {
    sockaddr_storage ss;
    sockaddr* addrp = <span class="hljs-keyword">reinterpret_cast</span>&lt;sockaddr*&gt;(&amp;ss);
    <span class="hljs-keyword">socklen_t</span> alen = <span class="hljs-keyword">sizeof</span>(ss);
    <span class="hljs-comment">//等待客户端连接</span>
    <span class="hljs-keyword">int</span> fd = accept4(s, addrp, &amp;alen, SOCK_CLOEXEC);
    <span class="hljs-keyword">if</span> (fd == <span class="hljs-number">-1</span>) {
      <span class="hljs-keyword">continue</span>; <span class="hljs-comment">//accept失败</span>
    }
    <span class="hljs-comment">//处理native crash发送过来的请求【见小节2.2】</span>
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
  <span class="hljs-comment">//读取client发送过来的请求【见小节2.3】</span>
  <span class="hljs-keyword">int</span> status = read_request(fd, &amp;request);
  ...

  <span class="hljs-comment">//fork子进程来处理其余请求命令</span>
  <span class="hljs-keyword">pid_t</span> fork_pid = fork();
  <span class="hljs-keyword">if</span> (fork_pid == <span class="hljs-number">-1</span>) {
    ALOGE(<span class="hljs-string">"debuggerd: failed to fork: %s\n"</span>, strerror(errno));
  } <span class="hljs-keyword">else</span> <span class="hljs-keyword">if</span> (fork_pid == <span class="hljs-number">0</span>) {
     <span class="hljs-comment">//子进程执行【见小节2.4】</span>
    worker_process(fd, request);
  } <span class="hljs-keyword">else</span> {
    <span class="hljs-comment">//父进程执行【见小节2.5】</span>
    monitor_worker_process(fork_pid, request);
  }
}
</code></pre></div></div>

<h3 id="23-read_request">2.3 read_request<a class="anchorjs-link " href="#23-read_request" aria-label="Anchor link for: 23 read_request" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; /debuggerd/debuggerd.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">int</span> <span class="hljs-title">read_request</span><span class="hljs-params">(<span class="hljs-keyword">int</span> fd, debugger_request_t* out_request)</span> </span>{
  ucred cr;
  <span class="hljs-keyword">socklen_t</span> len = <span class="hljs-keyword">sizeof</span>(cr);
  <span class="hljs-comment">//从fd获取client进程的pid,uid,gid</span>
  <span class="hljs-keyword">int</span> status = getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &amp;cr, &amp;len);
  ...
  fcntl(fd, F_SETFL, O_NONBLOCK);

  pollfd pollfds[<span class="hljs-number">1</span>];
  pollfds[<span class="hljs-number">0</span>].fd = fd;
  pollfds[<span class="hljs-number">0</span>].events = POLLIN;
  pollfds[<span class="hljs-number">0</span>].revents = <span class="hljs-number">0</span>;
  <span class="hljs-comment">//读取tid</span>
  status = TEMP_FAILURE_RETRY(poll(pollfds, <span class="hljs-number">1</span>, <span class="hljs-number">3000</span>));
  <span class="hljs-keyword">debugger_msg_t</span> msg;
  <span class="hljs-built_in">memset</span>(&amp;msg, <span class="hljs-number">0</span>, <span class="hljs-keyword">sizeof</span>(msg));
  <span class="hljs-comment">//从fd读取数据并保存到结构体msg</span>
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
    <span class="hljs-comment">// native crash时发送过来的请求</span>
    <span class="hljs-keyword">char</span> buf[<span class="hljs-number">64</span>];
    <span class="hljs-keyword">struct</span> stat s;
    <span class="hljs-built_in">snprintf</span>(buf, <span class="hljs-keyword">sizeof</span> buf, <span class="hljs-string">"/proc/%d/task/%d"</span>, out_request-&gt;pid, out_request-&gt;tid);
    <span class="hljs-keyword">if</span> (stat(buf, &amp;s)) {
      return <span class="hljs-number">-1</span>;  <span class="hljs-comment">//tid不存在，忽略该显式dump请求</span>
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

<p>read_request执行完成后，则从socket通道中读取到out_request。</p>

<h3 id="24-worker_process">2.4 worker_process<a class="anchorjs-link " href="#24-worker_process" aria-label="Anchor link for: 24 worker_process" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>处于client发送过来的请求，server端通过子进程来处理</p>

<p>[-&gt; /debuggerd/debuggerd.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">void</span> <span class="hljs-title">worker_process</span><span class="hljs-params">(<span class="hljs-keyword">int</span> fd, debugger_request_t&amp; request)</span> </span>{
  <span class="hljs-built_in">std</span>::<span class="hljs-built_in">string</span> tombstone_path;
  <span class="hljs-keyword">int</span> tombstone_fd = <span class="hljs-number">-1</span>;
  <span class="hljs-keyword">switch</span> (request.action) {
    <span class="hljs-keyword">case</span> DEBUGGER_ACTION_CRASH:
      <span class="hljs-comment">//打开tombstone文件</span>
      tombstone_fd = open_tombstone(&amp;tombstone_path);
      <span class="hljs-keyword">if</span> (tombstone_fd == <span class="hljs-number">-1</span>) {
        <span class="hljs-built_in">exit</span>(<span class="hljs-number">1</span>); <span class="hljs-comment">//无法打开tombstone文件，则退出该进程</span>
      }
      <span class="hljs-keyword">break</span>;
    ...
  }

  <span class="hljs-comment">// Attach到目标进程</span>
  <span class="hljs-keyword">if</span> (ptrace(PTRACE_ATTACH, request.tid, <span class="hljs-number">0</span>, <span class="hljs-number">0</span>) != <span class="hljs-number">0</span>) {
    <span class="hljs-built_in">exit</span>(<span class="hljs-number">1</span>); <span class="hljs-comment">//attach失败则退出该进程</span>
  }
  ...
  <span class="hljs-comment">//生成backtrace</span>
  <span class="hljs-built_in">std</span>::<span class="hljs-built_in">unique_ptr</span>&lt;BacktraceMap&gt; backtrace_map(BacktraceMap::Create(request.pid));

  <span class="hljs-keyword">int</span> amfd = <span class="hljs-number">-1</span>;
  <span class="hljs-built_in">std</span>::<span class="hljs-built_in">unique_ptr</span>&lt;<span class="hljs-built_in">std</span>::<span class="hljs-built_in">string</span>&gt; amfd_data;
  <span class="hljs-keyword">if</span> (request.action == DEBUGGER_ACTION_CRASH) {
    <span class="hljs-comment">//当发生native crash，则连接到AMS【见小节2.4.1】</span>
    amfd = activity_manager_connect();
    amfd_data.reset(new <span class="hljs-built_in">std</span>::<span class="hljs-built_in">string</span>);
  }

  <span class="hljs-keyword">bool</span> succeeded = <span class="hljs-literal">false</span>;

  <span class="hljs-comment">//取消特权模式</span>
  <span class="hljs-keyword">if</span> (!drop_privileges()) {
    _exit(<span class="hljs-number">1</span>); <span class="hljs-comment">//操作失败则退出</span>
  }

  <span class="hljs-keyword">int</span> crash_signal = SIGKILL;
  <span class="hljs-comment">//执行dump操作，【见小节2.4.2】</span>
  succeeded = perform_dump(request, fd, tombstone_fd, backtrace_map.get(), siblings,
                           &amp;crash_signal, amfd_data.get());

  <span class="hljs-keyword">if</span> (!attach_gdb) {
    <span class="hljs-comment">//将进程crash情况告知AMS【见小节2.4.3】</span>
    activity_manager_write(request.pid, crash_signal, amfd, *amfd_data.get());
  }
  <span class="hljs-comment">//detach目标进程</span>
  ptrace(PTRACE_DETACH, request.tid, <span class="hljs-number">0</span>, <span class="hljs-number">0</span>);

  <span class="hljs-keyword">for</span> (<span class="hljs-keyword">pid_t</span> sibling : siblings) {
    ptrace(PTRACE_DETACH, sibling, <span class="hljs-number">0</span>, <span class="hljs-number">0</span>);
  }

  <span class="hljs-keyword">if</span> (!attach_gdb &amp;&amp; request.action == DEBUGGER_ACTION_CRASH) {
    <span class="hljs-comment">//发送信号SIGKILL给目标进程[【见小节2.4.4】</span>
    <span class="hljs-keyword">if</span> (!send_signal(request.pid, request.tid, crash_signal)) {
      ALOGE(<span class="hljs-string">"debuggerd: failed to kill process %d: %s"</span>, request.pid, strerror(errno));
    }
  }
  ...
}
</code></pre></div></div>

<p>整个过程比较复杂，下面只介绍attach_gdb=false的执行流程：</p>

<ol>
  <li>当DEBUGGER_ACTION_CRASH ，则调用open_tombstone并继续执行；</li>
  <li>调用ptrace方法attach到目标进程;</li>
  <li>调用BacktraceMap::Create来生成backtrace;</li>
  <li>当DEBUGGER_ACTION_CRASH，则执行activity_manager_connect；</li>
  <li>调用drop_privileges来取消特权模式；</li>
  <li>通过perform_dump执行dump操作；
    <ul>
      <li>SIGBUS等致命信号，则调用<code class="highlighter-rouge">engrave_tombstone</code>()，这是核心方法</li>
    </ul>
  </li>
  <li>调用activity_manager_write，将进程crash情况告知AMS；</li>
  <li>调用ptrace方法detach到目标进程;</li>
  <li>当DEBUGGER_ACTION_CRASH，发送信号SIGKILL给目标进程tid</li>
</ol>

<h4 id="241-activity_manager_connect">2.4.1 activity_manager_connect<a class="anchorjs-link " href="#241-activity_manager_connect" aria-label="Anchor link for: 241 activity_manager_connect" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<p>[-&gt; debuggerd.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs objectivec"><span class="hljs-keyword">static</span> <span class="hljs-keyword">int</span> activity_manager_connect() {
  android::base::unique_fd amfd(socket(PF_UNIX, SOCK_STREAM, <span class="hljs-number">0</span>));
  <span class="hljs-keyword">if</span> (amfd.get() &lt; <span class="hljs-number">-1</span>) {
    <span class="hljs-keyword">return</span> <span class="hljs-number">-1</span>; <span class="hljs-comment">///无法连接到ActivityManager(socket失败)</span>
  }

  <span class="hljs-keyword">struct</span> sockaddr_un address;
  memset(&amp;address, <span class="hljs-number">0</span>, <span class="hljs-keyword">sizeof</span>(address));
  address.sun_family = AF_UNIX;
  <span class="hljs-comment">//该路径必须匹配NativeCrashListener.java中的定义</span>
  strncpy(address.sun_path, <span class="hljs-string">"/data/system/ndebugsocket"</span>, <span class="hljs-keyword">sizeof</span>(address.sun_path));
  <span class="hljs-keyword">if</span> (TE<span class="hljs-built_in">MP_FAILURE_RETRY</span>(connect(amfd.get(), reinterpret_cast&lt;<span class="hljs-keyword">struct</span> sockaddr*&gt;(&amp;address),
                                 <span class="hljs-keyword">sizeof</span>(address))) == <span class="hljs-number">-1</span>) {
    <span class="hljs-keyword">return</span> <span class="hljs-number">-1</span>;  <span class="hljs-comment">//无法连接到ActivityManager(connect失败)</span>
  }

  <span class="hljs-keyword">struct</span> timeval tv;
  memset(&amp;tv, <span class="hljs-number">0</span>, <span class="hljs-keyword">sizeof</span>(tv));
  tv.tv_sec = <span class="hljs-number">1</span>;
  <span class="hljs-keyword">if</span> (setsockopt(amfd.get(), SOL_SOCKET, SO_SNDTIMEO, &amp;tv, <span class="hljs-keyword">sizeof</span>(tv)) == <span class="hljs-number">-1</span>) {
    <span class="hljs-keyword">return</span> <span class="hljs-number">-1</span>; <span class="hljs-comment">//无法连接到ActivityManager(setsockopt SO_SNDTIMEO失败)</span>
  }

  tv.tv_sec = <span class="hljs-number">3</span>;
  <span class="hljs-keyword">if</span> (setsockopt(amfd.get(), SOL_SOCKET, SO_RCVTIMEO, &amp;tv, <span class="hljs-keyword">sizeof</span>(tv)) == <span class="hljs-number">-1</span>) {
    <span class="hljs-keyword">return</span> <span class="hljs-number">-1</span>; <span class="hljs-comment">//无法连接到ActivityManager(setsockopt SO_RCVTIMEO失败)</span>
  }

  <span class="hljs-keyword">return</span> amfd.release();
}
</code></pre></div></div>

<p>该方法的功能是建立跟上层<code class="highlighter-rouge">ActivityManager</code>的socket连接。对于”/data/system/ndebugsocket”的服务端是在，NativeCrashListener.java方法中创建并启动的。</p>

<h4 id="242-perform_dump">2.4.2 perform_dump<a class="anchorjs-link " href="#242-perform_dump" aria-label="Anchor link for: 242 perform_dump" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>
<p>根据接收到不同的signal采取相应的操作</p>

<p>[-&gt; debuggerd.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">bool</span> <span class="hljs-title">perform_dump</span><span class="hljs-params">(<span class="hljs-keyword">const</span> debugger_request_t&amp; request, <span class="hljs-keyword">int</span> fd, <span class="hljs-keyword">int</span> tombstone_fd,
                         BacktraceMap* backtrace_map, <span class="hljs-keyword">const</span> <span class="hljs-built_in">std</span>::<span class="hljs-built_in">set</span>&lt;pid_t&gt;&amp; siblings,
                         <span class="hljs-keyword">int</span>* crash_signal, <span class="hljs-built_in">std</span>::<span class="hljs-built_in">string</span>* amfd_data)</span> </span>{
  <span class="hljs-keyword">if</span> (TEMP_FAILURE_RETRY(write(fd, <span class="hljs-string">"\0"</span>, <span class="hljs-number">1</span>)) != <span class="hljs-number">1</span>) {
    return <span class="hljs-literal">false</span>; <span class="hljs-comment">//无法响应client端请求</span>
  }

  <span class="hljs-keyword">int</span> total_sleep_time_usec = <span class="hljs-number">0</span>;
  <span class="hljs-keyword">while</span> (<span class="hljs-literal">true</span>) {
    <span class="hljs-comment">//等待信号到来</span>
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
        <span class="hljs-comment">//这是输出tombstone信息最为核心的方法</span>
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

<p>对于以下信号都是致命的信号:</p>

<ul>
  <li>SIGABRT：abort退出异常</li>
  <li>SIGBUS：硬件访问异常</li>
  <li>SIGFPE：浮点运算异常</li>
  <li>SIGILL：非法指令异常</li>
  <li>SIGSEGV：内存访问异常</li>
  <li>SIGSTKFLT：协处理器栈异常</li>
  <li>SIGTRAP：陷阱异常</li>
</ul>

<p>另外，上篇文章已介绍过<a href="http://gityuan.com/2016/06/15/android-debuggerd/#tombstone">engrave_tombstone</a>的功能内容，这里就不再累赘了。</p>

<h4 id="243-activity_manager_write">2.4.3 activity_manager_write<a class="anchorjs-link " href="#243-activity_manager_write" aria-label="Anchor link for: 243 activity_manager_write" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<p>[-&gt; debuggerd.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs php"><span class="hljs-keyword">static</span> void activity_manager_write(int pid, int signal, int amfd, <span class="hljs-keyword">const</span> std::string&amp; amfd_data) {
  <span class="hljs-keyword">if</span> (amfd == <span class="hljs-number">-1</span>) {
    <span class="hljs-keyword">return</span>;
  }

  <span class="hljs-comment">//写入pid和signal，以及原始dump信息，最后添加0以标记结束</span>
  uint32_t datum = htonl(pid);
  <span class="hljs-keyword">if</span> (!android::base::WriteFully(amfd, &amp;datum, <span class="hljs-number">4</span>)) {
    <span class="hljs-keyword">return</span>; <span class="hljs-comment">//AM pid写入失败</span>
  }
  datum = htonl(signal);
  <span class="hljs-keyword">if</span> (!android::base::WriteFully(amfd, &amp;datum, <span class="hljs-number">4</span>)) {
    <span class="hljs-keyword">return</span>;<span class="hljs-comment">//AM signal写入失败</span>
  }

  <span class="hljs-keyword">if</span> (!android::base::WriteFully(amfd, amfd_data.c_str(), amfd_data.size())) {
    <span class="hljs-keyword">return</span>;<span class="hljs-comment">//AM data写入失败</span>
  }

  uint8_t eodMarker = <span class="hljs-number">0</span>;
  <span class="hljs-keyword">if</span> (!android::base::WriteFully(amfd, &amp;eodMarker, <span class="hljs-number">1</span>)) {
    <span class="hljs-keyword">return</span>; <span class="hljs-comment">//AM eod 写入失败</span>
  }
  <span class="hljs-comment">//读取应答消息，如果3s超时未收到则读取失败</span>
  android::base::ReadFully(amfd, &amp;eodMarker, <span class="hljs-number">1</span>);
}
</code></pre></div></div>

<p>debuggerd与AMS的NativeCrashListener建立socket连接后，再通过该方法发送数据，数据项包括pid、signal、dump信息。</p>

<h4 id="244-send_signal">2.4.4 send_signal<a class="anchorjs-link " href="#244-send_signal" aria-label="Anchor link for: 244 send_signal" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<p>此处只是向目标进程发送SIGKILL信号，用于杀掉目标进程，文章<a href="http://gityuan.com/2016/04/16/kill-signal/#sendsignal">理解杀进程的实现原理</a>已详细讲述过发送SIGKILL信号的处理流程。</p>

<h3 id="25-monitor_worker_process">2.5 monitor_worker_process<a class="anchorjs-link " href="#25-monitor_worker_process" aria-label="Anchor link for: 25 monitor_worker_process" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs bash">static void monitor_worker_process(int child_pid, const debugger_request_t&amp; request) {
  struct timespec timeout = {.tv_sec = 10, .tv_nsec = 0 };
  <span class="hljs-keyword">if</span> (should_attach_gdb(request)) {
    //如果使能<span class="hljs-built_in">wait</span>_<span class="hljs-keyword">for</span>_gdb，则将timeout设置为非常大
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

<p>该方法是运行在debuggerd父进程，用于监控子进程的执行情况。</p>

<h3 id="26-小结">2.6 小结<a class="anchorjs-link " href="#26-小结" aria-label="Anchor link for: 26 小结" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>debuggerd服务端调用链：</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs css"><span class="hljs-selector-tag">do_server</span>
    <span class="hljs-selector-tag">handle_request</span>
        <span class="hljs-selector-tag">read_request</span>
        <span class="hljs-selector-tag">worker_process</span>(子进程执行)
            <span class="hljs-selector-tag">open_tombstone</span>
            <span class="hljs-selector-tag">ptrace</span>(<span class="hljs-selector-tag">PTRACE_ATTACH</span>, <span class="hljs-selector-tag">request</span><span class="hljs-selector-class">.tid</span>, 0, 0)
            <span class="hljs-selector-tag">backtrace_map</span>
            <span class="hljs-selector-tag">activity_manager_connect</span>
            <span class="hljs-selector-tag">perform_dump</span>
            <span class="hljs-selector-tag">activity_manager_write</span>
            <span class="hljs-selector-tag">ptrace</span>(<span class="hljs-selector-tag">PTRACE_DETACH</span>, <span class="hljs-selector-tag">request</span><span class="hljs-selector-class">.tid</span>, 0, 0);
            <span class="hljs-selector-tag">send_signal</span>
        <span class="hljs-selector-tag">monitor_worker_process</span>(父进程执行)
</code></pre></div></div>

<p>handle_request方法中通过fork机制，创建子进程来执行worker_process，由于fork返回两次，返回到父进程则执行monitor_worker_process。</p>

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
               <span class="hljs-comment">//【见小节3.2】</span>
               mActivityManagerService.startObservingNativeCrashes();
           } <span class="hljs-keyword">catch</span> (Throwable e) {
               reportWtf(<span class="hljs-string">"observing native crashes"</span>, e);
           }
        }
    }
}
</code></pre></div></div>

<p>当开机过程中启动服务启动到阶段<code class="highlighter-rouge">PHASE_ACTIVITY_MANAGER_READY</code>(550)，即服务可以广播自己的Intents，然后启动native crash的监听进程。</p>

<h3 id="32-startobservingnativecrashes">3.2 startObservingNativeCrashes<a class="anchorjs-link " href="#32-startobservingnativecrashes" aria-label="Anchor link for: 32 startobservingnativecrashes" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; ActivityManagerService.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">startObservingNativeCrashes</span><span class="hljs-params">()</span> </span>{
    <span class="hljs-comment">//【见小节3.3】</span>
    <span class="hljs-keyword">final</span> NativeCrashListener ncl = <span class="hljs-keyword">new</span> NativeCrashListener(<span class="hljs-keyword">this</span>);
    ncl.start();
}
</code></pre></div></div>

<p>NativeCrashListener继承于<code class="highlighter-rouge">Thread</code>，可见这是线程，通过调用start方法来启动线程开始工作。</p>

<h3 id="33-nativecrashlistener">3.3 NativeCrashListener<a class="anchorjs-link " href="#33-nativecrashlistener" aria-label="Anchor link for: 33 nativecrashlistener" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; NativeCrashListener.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">public</span> <span class="hljs-keyword">void</span> <span class="hljs-title">run</span><span class="hljs-params">()</span> </span>{
    <span class="hljs-keyword">final</span> <span class="hljs-keyword">byte</span>[] ackSignal = <span class="hljs-keyword">new</span> <span class="hljs-keyword">byte</span>[<span class="hljs-number">1</span>];
    {
        <span class="hljs-comment">//此处DEBUGGERD_SOCKET_PATH= "/data/system/ndebugsocket"</span>
        File socketFile = <span class="hljs-keyword">new</span> File(DEBUGGERD_SOCKET_PATH);
        <span class="hljs-keyword">if</span> (socketFile.exists()) {
            socketFile.delete();
        }
    }

    <span class="hljs-keyword">try</span> {
        FileDescriptor serverFd = Os.socket(AF_UNIX, SOCK_STREAM, <span class="hljs-number">0</span>);
        <span class="hljs-comment">//创建socket服务端</span>
        <span class="hljs-keyword">final</span> UnixSocketAddress sockAddr = UnixSocketAddress.createFileSystem(
                DEBUGGERD_SOCKET_PATH);
        Os.bind(serverFd, sockAddr);
        Os.listen(serverFd, <span class="hljs-number">1</span>);

        <span class="hljs-keyword">while</span> (<span class="hljs-keyword">true</span>) {
            FileDescriptor peerFd = <span class="hljs-keyword">null</span>;
            <span class="hljs-keyword">try</span> {
                <span class="hljs-comment">//等待debuggerd建立连接</span>
                peerFd = Os.accept(serverFd, <span class="hljs-keyword">null</span> <span class="hljs-comment">/* peerAddress */</span>);
                <span class="hljs-comment">//获取debuggerd的socket文件描述符</span>
                <span class="hljs-keyword">if</span> (peerFd != <span class="hljs-keyword">null</span>) {
                    <span class="hljs-comment">//只有超级用户才被允许通过该socket进行通信</span>
                    StructUcred credentials =
                            Os.getsockoptUcred(peerFd, SOL_SOCKET, SO_PEERCRED);
                    <span class="hljs-keyword">if</span> (credentials.uid == <span class="hljs-number">0</span>) {
                        <span class="hljs-comment">//【见小节3.4】处理native crash信息</span>
                        consumeNativeCrashData(peerFd);
                    }
                }
            } <span class="hljs-keyword">catch</span> (Exception e) {
                Slog.w(TAG, <span class="hljs-string">"Error handling connection"</span>, e);
            } <span class="hljs-keyword">finally</span> {
                <span class="hljs-comment">//应答debuggerd已经建立连接</span>
                <span class="hljs-keyword">if</span> (peerFd != <span class="hljs-keyword">null</span>) {
                    Os.write(peerFd, ackSignal, <span class="hljs-number">0</span>, <span class="hljs-number">1</span>);<span class="hljs-comment">//写入应答消息</span>
                    Os.close(peerFd);<span class="hljs-comment">//关闭socket</span>
                    ...
                }
            }
        }
    } <span class="hljs-keyword">catch</span> (Exception e) {
        Slog.e(TAG, <span class="hljs-string">"Unable to init native debug socket!"</span>, e);
    }
}
</code></pre></div></div>

<p>该方法主要功能：</p>

<ol>
  <li>创建socket服务端：”/data/system/ndebugsocket”文件权限700，owned为system:system，debuggerd是以root权限运行，因此可以与该socket建立连接，但对于第三方App则没有权限；</li>
  <li>等待socket客户端(即debuggerd)来建立连接；</li>
  <li>调用consumeNativeCrashData来处理native crash信息；</li>
  <li>应答debuggerd已经建立连接，并写入应答消息告知debuggerd进程。</li>
</ol>

<h3 id="34-consumenativecrashdata">3.4 consumeNativeCrashData<a class="anchorjs-link " href="#34-consumenativecrashdata" aria-label="Anchor link for: 34 consumenativecrashdata" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>
<p>[-&gt; NativeCrashListener.java]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs java"><span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">consumeNativeCrashData</span><span class="hljs-params">(FileDescriptor fd)</span> </span>{
    <span class="hljs-comment">//进入该方法，标识着debuggerd已经与AMS建立连接</span>
    <span class="hljs-keyword">final</span> <span class="hljs-keyword">byte</span>[] buf = <span class="hljs-keyword">new</span> <span class="hljs-keyword">byte</span>[<span class="hljs-number">4096</span>];
    <span class="hljs-keyword">final</span> ByteArrayOutputStream os = <span class="hljs-keyword">new</span> ByteArrayOutputStream(<span class="hljs-number">4096</span>);

    <span class="hljs-keyword">try</span> {
        <span class="hljs-comment">//此处SOCKET_TIMEOUT_MILLIS=2s</span>
        StructTimeval timeout = StructTimeval.fromMillis(SOCKET_TIMEOUT_MILLIS);
        Os.setsockoptTimeval(fd, SOL_SOCKET, SO_RCVTIMEO, timeout);
        Os.setsockoptTimeval(fd, SOL_SOCKET, SO_SNDTIMEO, timeout);

        <span class="hljs-comment">//1.读取pid和signal number</span>
        <span class="hljs-keyword">int</span> headerBytes = readExactly(fd, buf, <span class="hljs-number">0</span>, <span class="hljs-number">8</span>);
        <span class="hljs-keyword">if</span> (headerBytes != <span class="hljs-number">8</span>) {
            <span class="hljs-keyword">return</span>; <span class="hljs-comment">//读取失败</span>
        }

        <span class="hljs-keyword">int</span> pid = unpackInt(buf, <span class="hljs-number">0</span>);
        <span class="hljs-keyword">int</span> signal = unpackInt(buf, <span class="hljs-number">4</span>);

        <span class="hljs-comment">//2.读取dump内容</span>
        <span class="hljs-keyword">if</span> (pid &gt; <span class="hljs-number">0</span>) {
            <span class="hljs-keyword">final</span> ProcessRecord pr;
            <span class="hljs-keyword">synchronized</span> (mAm.mPidsSelfLocked) {
                pr = mAm.mPidsSelfLocked.get(pid);
            }
            <span class="hljs-keyword">if</span> (pr != <span class="hljs-keyword">null</span>) {
                <span class="hljs-comment">//persistent应用，直接忽略</span>
                <span class="hljs-keyword">if</span> (pr.persistent) {
                    <span class="hljs-keyword">return</span>;
                }

                <span class="hljs-keyword">int</span> bytes;
                do {
                    <span class="hljs-comment">//获取数据</span>
                    bytes = Os.read(fd, buf, <span class="hljs-number">0</span>, buf.length);
                    <span class="hljs-keyword">if</span> (bytes &gt; <span class="hljs-number">0</span>) {
                        <span class="hljs-keyword">if</span> (buf[bytes-<span class="hljs-number">1</span>] == <span class="hljs-number">0</span>) {
                            <span class="hljs-comment">//到达文件EOD, 忽略该字节</span>
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
                <span class="hljs-comment">//异常处理native crash报告【见小节3.5】</span>
                (<span class="hljs-keyword">new</span> NativeCrashReporter(pr, signal, reportString)).start();
            }
        }
    } <span class="hljs-keyword">catch</span> (Exception e) {
        Slog.e(TAG, <span class="hljs-string">"Exception dealing with report"</span>, e);
    }
}
</code></pre></div></div>

<p>读取debuggerd那端发送过来的数据，再通过NativeCrashReporter来把native crash事件报告给framework层。</p>

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
            <span class="hljs-comment">//AMS真正处理crash的过程</span>
            mAm.handleApplicationCrashInner(<span class="hljs-string">"native_crash"</span>, mApp, mApp.processName, ci);
        } <span class="hljs-keyword">catch</span> (Exception e) {
            Slog.e(TAG, <span class="hljs-string">"Unable to report native crash"</span>, e);
        }
    }
}
</code></pre></div></div>

<p>不论是Native crash还是framework crash最终都会调用到<code class="highlighter-rouge">handleApplicationCrashInner()</code>，该方法见文章<a href="http://gityuan.com/2016/06/24/app-crash/#handleApplicationCrashInner">理解Android Crash处理流程</a>。</p>

<h3 id="36-小结">3.6 小结<a class="anchorjs-link " href="#36-小结" aria-label="Anchor link for: 36 小结" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>system_server进程启动过程中，调用<code class="highlighter-rouge">startOtherServices</code>来启动各种其他系统Service时，也正是这个时机会创建一个用于监听native crash事件的NativeCrashListener对象(继承于线程)，通过socket机制来监听，等待即debuggerd与该线程创建连接，并处理相应事件。紧接着调用<code class="highlighter-rouge">handleApplicationCrashInner</code>来处理crash流程。</p>

<p>NativeCrashListener的主要工作：</p>

<ol>
  <li>创建socket服务端”/data/system/ndebugsocket”</li>
  <li>等待socket客户端(即debuggerd)来建立连接；</li>
  <li>调用consumeNativeCrashData来处理native crash信息；</li>
  <li>应答debuggerd已经建立连接，并写入应答消息告知debuggerd进程。</li>
</ol>

<h2 id="四总结">四、总结<a class="anchorjs-link " href="#四总结" aria-label="Anchor link for: 四总结" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h2>

<p>点击查看<a href="http://gityuan.com/images/stability/native_crash.jpg">大图</a></p>

<p><img src="./images/native_crash.jpg" alt="native_crash"></p>

<p>Native程序通过link连接后，当发生Native Crash时，则kernel会发送相应的<code class="highlighter-rouge">signal</code>，当进程捕获致命的<code class="highlighter-rouge">signal</code>，通知<code class="highlighter-rouge">debuggerd</code>调用<code class="highlighter-rouge">ptrace</code>来获取有价值的信息(这是发生在crash前)。</p>

<ol>
  <li>kernel 发送signal给target进程(包含native代码)；</li>
  <li>target进程通过debuggerd_signal_handler，捕获signal；
    <ul>
      <li>建立于debuggerd进程的socket通道；</li>
      <li>将action = DEBUGGER_ACTION_CRASH的消息发送给debuggerd服务端；</li>
      <li>阻塞等待debuggerd服务端的回应数据。</li>
    </ul>
  </li>
  <li>debuggerd作为守护进程，一直在等待socket client的连接，此时收到action = DEBUGGER_ACTION_CRASH的消息；</li>
  <li>执行到handle_request时，通过fork创建子进程来执行各种dump相关操作；</li>
  <li>新创建的进程，通过socket与system_server进程中的NativeCrashListener线程建立socket通道，并向其发送native crash信息；</li>
  <li>NativeCrashListener线程通过创建新的名为“NativeCrashReport”的子线程来执行AMS的handleApplicationCrashInner方法。</li>
</ol>

<p>这个流程图只是从整体来概要介绍native crash流程，其中有两个部分是核心方法：</p>

<ul>
  <li>其一是图中红色块<code class="highlighter-rouge">perform_dump</code>是整个debuggerd的核心工作，该方法内部调用<code class="highlighter-rouge">engrave_tombstone</code>，该方法的具体工作见文章<a href="http://gityuan.com/2016/06/15/android-debuggerd/#tombstone">ebuggerd守护进程</a>的功能内容，这个过程还需要与target进程通信来获取target进程更多信息。</li>
  <li>其二是AMS的handleApplicationCrashInner，该方法的工作见姊妹篇<a href="http://gityuan.com/2016/06/24/app-crash/#handleApplicationCrashInner">理解Android Crash处理流程</a>。</li>
</ul>


   