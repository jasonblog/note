# debuggerd守护进程



<div class="
                col-lg-8 col-lg-offset-2
                col-md-10 col-md-offset-1
                post-container">

								<blockquote>
  <p>基于Android 6.0源码， 分析debuggerd守护进程的工作原理</p>
</blockquote>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs perl">/<span class="hljs-keyword">system</span>/core/debuggerd/debuggerd.cpp
/<span class="hljs-keyword">system</span>/core/debuggerd/tombstone.cpp
/<span class="hljs-keyword">system</span>/core/debuggerd/backtrace.cpp
/<span class="hljs-keyword">system</span>/core/debuggerd/arm/Machine.cpp
/<span class="hljs-keyword">system</span>/core/debuggerd/arm64/Machine.cpp

/<span class="hljs-keyword">system</span>/core/libcutils/debugger.c
/<span class="hljs-keyword">system</span>/core/include/BacktraceMap.h
/<span class="hljs-keyword">system</span>/core/libbacktrace/BacktraceMap.cpp
/<span class="hljs-keyword">system</span>/core/libbacktrace/Backtrace.cpp

/bionic/linker/arch/arm/begin.S
/bionic/linker/linker.cpp
/bionic/linker/debugger.cpp
</code></pre></div></div>

<h2 id="一概述">一、概述<a class="anchorjs-link " href="#一概述" aria-label="Anchor link for: 一概述" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h2>

<p>Android系统有监控程序异常退出的机制，这便是本文要讲述得debuggerd守护进程。当发生native crash或者主动调用debuggerd时，会输出进程相关的状态信息到文件或者控制台。输出的debuggerd数据
保存在文件<code class="highlighter-rouge">/data/tombstones/tombstone_XX</code>，该类型文件个数上限位10个，当超过时则每次覆盖时间最老的文件。</p>

<p>针对进程出现的不同的状态，Linux kernel会发送相应的signal给异常进程，捕获signal并对其做相应的处理（通常动作是退出异常进程）。而Android在这机制的前提下，通过拦截这些信号来dump进程信息，方便开发人员调试分析。</p>

<p>debuggerd守护进程会打开socket服务端，当需要调用debuggerd服务时，先通过客户端进程向debuggerd服务端建立socket连接，然后发送不同的请求给debuggerd服务端，当服务端收到不同的请求，则会采取相应的dump操作。</p>

<p>接下来从源码角度来探索debuggerd客户端和服务端的工作原理。</p>

<h2 id="二debuggerd客户端">二、debuggerd客户端<a class="anchorjs-link " href="#二debuggerd客户端" aria-label="Anchor link for: 二debuggerd客户端" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h2>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs xml">debuggerd -b <span class="hljs-tag">&lt;<span class="hljs-name">tid</span>&gt;</span>
debuggerd <span class="hljs-tag">&lt;<span class="hljs-name">tid</span>&gt;</span>
</code></pre></div></div>

<p>通过adb执行上面的命令都能触发debuggerd进行相应的dump操作，其中参数<code class="highlighter-rouge">-b</code>表示在控制台中输出backtrace，参数tid表示的是需要dump的进程或者线程id。这两个命令的输出结果相差较大，下面来一步步分析看看这两个命令分别能触发哪些操作，执行上述命令都会调用debuggerd的main方法()。</p>

<h3 id="21-main">2.1 main<a class="anchorjs-link " href="#21-main" aria-label="Anchor link for: 21 main" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; /debuggerd/debuggerd.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cs"><span class="hljs-function"><span class="hljs-keyword">int</span> <span class="hljs-title">main</span>(<span class="hljs-params"><span class="hljs-keyword">int</span> argc, <span class="hljs-keyword">char</span>** argv</span>) </span>{
  ...
  <span class="hljs-keyword">bool</span> dump_backtrace = <span class="hljs-literal">false</span>;
  <span class="hljs-keyword">bool</span> have_tid = <span class="hljs-literal">false</span>;
  pid_t tid = <span class="hljs-number">0</span>;
  <span class="hljs-comment">//参数解析backtrace与tid信息</span>
  <span class="hljs-keyword">for</span> (<span class="hljs-keyword">int</span> i = <span class="hljs-number">1</span>; i &lt; argc; i++) {
    <span class="hljs-keyword">if</span> (!strcmp(argv[i], <span class="hljs-string">"-b"</span>)) {
      dump_backtrace = <span class="hljs-literal">true</span>;
    } <span class="hljs-function"><span class="hljs-keyword">else</span> <span class="hljs-title">if</span> (<span class="hljs-params">!have_tid</span>) </span>{
      tid = atoi(argv[i]);
      have_tid = <span class="hljs-literal">true</span>;
    } <span class="hljs-keyword">else</span> {
      usage();
      <span class="hljs-keyword">return</span> <span class="hljs-number">1</span>;
    }
  }
  <span class="hljs-comment">//没有指定tid则直接返回</span>
  <span class="hljs-keyword">if</span> (!have_tid) {
    usage();
    <span class="hljs-keyword">return</span> <span class="hljs-number">1</span>;
  }
  <span class="hljs-comment">//【见小节2.2】</span>
  <span class="hljs-keyword">return</span> do_explicit_dump(tid, dump_backtrace);
}
</code></pre></div></div>

<p>对于debuggerd命令，必须指定线程tid，否则不做任何操作，直接返回。</p>

<h3 id="22-do_explicit_dump">2.2 do_explicit_dump<a class="anchorjs-link " href="#22-do_explicit_dump" aria-label="Anchor link for: 22 do_explicit_dump" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; /debuggerd/debuggerd.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">int</span> <span class="hljs-title">do_explicit_dump</span><span class="hljs-params">(pid_t tid, <span class="hljs-keyword">bool</span> dump_backtrace)</span> </span>{
  <span class="hljs-built_in">fprintf</span>(<span class="hljs-built_in">stdout</span>, <span class="hljs-string">"Sending request to dump task %d.\n"</span>, tid);

  <span class="hljs-keyword">if</span> (dump_backtrace) {
    fflush(<span class="hljs-built_in">stdout</span>);
    <span class="hljs-comment">//输出到控制台【见小节2.3】</span>
    <span class="hljs-keyword">if</span> (dump_backtrace_to_file(tid, fileno(stdout)) &lt; <span class="hljs-number">0</span>) {
      <span class="hljs-built_in">fputs</span>(<span class="hljs-string">"Error dumping backtrace.\n"</span>, <span class="hljs-built_in">stderr</span>);
      return <span class="hljs-number">1</span>;
    }
  } <span class="hljs-keyword">else</span> {
    <span class="hljs-keyword">char</span> tombstone_path[PATH_MAX];
    <span class="hljs-comment">//输出到tombstone文件【见小节2.4】</span>
    <span class="hljs-keyword">if</span> (dump_tombstone(tid, tombstone_path, sizeof(tombstone_path)) &lt; <span class="hljs-number">0</span>) {
      <span class="hljs-built_in">fputs</span>(<span class="hljs-string">"Error dumping tombstone.\n"</span>, <span class="hljs-built_in">stderr</span>);
      return <span class="hljs-number">1</span>;
    }
    <span class="hljs-built_in">fprintf</span>(<span class="hljs-built_in">stderr</span>, <span class="hljs-string">"Tombstone written to: %s\n"</span>, tombstone_path);
  }
  return <span class="hljs-number">0</span>;
}
</code></pre></div></div>

<p>dump_backtrace等于true代表的是输出backtrace到控制台，否则意味着输出到tombstone文件。</p>

<h3 id="23-dump_backtrace_to_file">2.3 dump_backtrace_to_file<a class="anchorjs-link " href="#23-dump_backtrace_to_file" aria-label="Anchor link for: 23 dump_backtrace_to_file" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; libcutils/debugger.c]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs objectivec"><span class="hljs-keyword">int</span> dump_backtrace_to_file(pid_t tid, <span class="hljs-keyword">int</span> fd) {
    <span class="hljs-keyword">return</span> dump_backtrace_to_file_timeout(tid, fd, <span class="hljs-number">0</span>);
}

<span class="hljs-keyword">int</span> dump_backtrace_to_file_timeout(pid_t tid, <span class="hljs-keyword">int</span> fd, <span class="hljs-keyword">int</span> timeout_secs) {
  <span class="hljs-comment">//向socket服务端发送dump backtrace的请求【见小节2.5】</span>
  <span class="hljs-keyword">int</span> sock_fd = make_dump_request(DEBUGGER_ACTION_DU<span class="hljs-built_in">MP_BACKTRACE</span>, tid, timeout_secs);
  <span class="hljs-keyword">if</span> (sock_fd &lt; <span class="hljs-number">0</span>) {
    <span class="hljs-keyword">return</span> <span class="hljs-number">-1</span>;
  }

  <span class="hljs-keyword">int</span> result = <span class="hljs-number">0</span>;
  <span class="hljs-keyword">char</span> buffer[<span class="hljs-number">1024</span>];
  ssize_t n;
  <span class="hljs-comment">//阻塞等待，从sock_fd中读取到服务端发送过来的数据，并写入buffer</span>
  <span class="hljs-keyword">while</span> ((n = TE<span class="hljs-built_in">MP_FAILURE_RETRY</span>(read(sock_fd, buffer, <span class="hljs-keyword">sizeof</span>(buffer)))) &gt; <span class="hljs-number">0</span>) {
    <span class="hljs-comment">//再将buffer数据输出到fd，此处是stdout文件描述符(屏幕终端)。</span>
    <span class="hljs-keyword">if</span> (TE<span class="hljs-built_in">MP_FAILURE_RETRY</span>(write(fd, buffer, n)) != n) {
      result = <span class="hljs-number">-1</span>;
      <span class="hljs-keyword">break</span>;
    }
  }
  close(sock_fd);
  <span class="hljs-keyword">return</span> result;
}
</code></pre></div></div>

<p>该方法的功能：</p>

<ul>
  <li>首先，向debuggerd的socket服务端发出<code class="highlighter-rouge">DEBUGGER_ACTION_DUMP_BACKTRACE</code>请求，然后阻塞等待；</li>
  <li>循环遍历读取debuggerd服务端发送过来的数据，并写入到buffer;</li>
  <li>再将buffer数据输出到fd，此处是stdout文件描述符(屏幕终端)。</li>
</ul>

<h3 id="24-dump_tombstone">2.4 dump_tombstone<a class="anchorjs-link " href="#24-dump_tombstone" aria-label="Anchor link for: 24 dump_tombstone" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; libcutils/debugger.c]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs objectivec"><span class="hljs-keyword">int</span> dump_tombstone(pid_t tid, <span class="hljs-keyword">char</span>* pathbuf, size_t pathlen) {
  <span class="hljs-keyword">return</span> dump_tombstone_timeout(tid, pathbuf, pathlen, <span class="hljs-number">0</span>);
}

<span class="hljs-keyword">int</span> dump_tombstone_timeout(pid_t tid, <span class="hljs-keyword">char</span>* pathbuf, size_t pathlen, <span class="hljs-keyword">int</span> timeout_secs) {
  <span class="hljs-comment">//向socket服务端发送dump tombstone的请求【见小节2.5】</span>
  <span class="hljs-keyword">int</span> sock_fd = make_dump_request(DEBUGGER_ACTION_DU<span class="hljs-built_in">MP_TOMBSTONE</span>, tid, timeout_secs);
  <span class="hljs-keyword">if</span> (sock_fd &lt; <span class="hljs-number">0</span>) {
    <span class="hljs-keyword">return</span> <span class="hljs-number">-1</span>;
  }

  <span class="hljs-keyword">char</span> buffer[<span class="hljs-number">100</span>];
  <span class="hljs-keyword">int</span> result = <span class="hljs-number">0</span>; ，
  <span class="hljs-comment">//从sock_fd中读取到服务端发送过来的tombstone文件名，并写入buffer</span>
  ssize_t n = TE<span class="hljs-built_in">MP_FAILURE_RETRY</span>(read(sock_fd, buffer, <span class="hljs-keyword">sizeof</span>(buffer) - <span class="hljs-number">1</span>));
  <span class="hljs-keyword">if</span> (n &lt;= <span class="hljs-number">0</span>) {
    result = <span class="hljs-number">-1</span>;
  } <span class="hljs-keyword">else</span> {
    <span class="hljs-keyword">if</span> (pathbuf &amp;&amp; pathlen) {
      <span class="hljs-keyword">if</span> (n &gt;= (ssize_t) pathlen) {
        n = pathlen - <span class="hljs-number">1</span>;
      }
      buffer[n] = <span class="hljs-string">'\0'</span>;
      <span class="hljs-comment">//将buffer数据拷贝到pathbuf</span>
      memcpy(pathbuf, buffer, n + <span class="hljs-number">1</span>);
    }
  }
  close(sock_fd);
  <span class="hljs-keyword">return</span> result;
}
</code></pre></div></div>

<p>该方法的功能：</p>

<ul>
  <li>首先，向debuggerd的socket服务端发出<code class="highlighter-rouge">DEBUGGER_ACTION_DUMP_TOMBSTONE</code>请求，然后阻塞等待；</li>
  <li>循环遍历读取debuggerd服务端发送过来的tombstone文件名，并写入到buffer;</li>
  <li>将buffer数据拷贝到pathbuf，即拷贝tombstone文件名。</li>
</ul>

<h3 id="25-make_dump_request">2.5 make_dump_request<a class="anchorjs-link " href="#25-make_dump_request" aria-label="Anchor link for: 25 make_dump_request" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; libcutils/debugger.c]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cs"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">int</span> <span class="hljs-title">make_dump_request</span>(<span class="hljs-params">debugger_action_t action, pid_t tid, <span class="hljs-keyword">int</span> timeout_secs</span>) </span>{
  debugger_msg_t msg;
  memset(&amp;msg, <span class="hljs-number">0</span>, <span class="hljs-keyword">sizeof</span>(msg));
  msg.tid = tid;
  msg.action = action;
  <span class="hljs-comment">//与debuggerd服务端建立socket通信，获取client端描述符sock_fd</span>
  <span class="hljs-keyword">int</span> sock_fd = socket_local_client(DEBUGGER_SOCKET_NAME, ANDROID_SOCKET_NAMESPACE_ABSTRACT,
      SOCK_STREAM | SOCK_CLOEXEC);
  ...
  <span class="hljs-comment">//通过write()方法将msg信息写入文件描述符sock_fd【见小节2.6】</span>
  <span class="hljs-keyword">if</span> (send_request(sock_fd, &amp;msg, <span class="hljs-keyword">sizeof</span>(msg)) &lt; <span class="hljs-number">0</span>) {
    close(sock_fd);
    <span class="hljs-keyword">return</span> <span class="hljs-number">-1</span>;
  }
  <span class="hljs-keyword">return</span> sock_fd;
}
</code></pre></div></div>

<p>该函数的功能是与debuggerd服务端建立socket通信，并发送action请求，以执行相应操作。</p>

<h3 id="26-send_request">2.6 send_request<a class="anchorjs-link " href="#26-send_request" aria-label="Anchor link for: 26 send_request" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs objectivec"><span class="hljs-keyword">static</span> <span class="hljs-keyword">int</span> send_request(<span class="hljs-keyword">int</span> sock_fd, <span class="hljs-keyword">void</span>* msg_ptr, size_t msg_len) {
  <span class="hljs-keyword">int</span> result = <span class="hljs-number">0</span>;
  <span class="hljs-comment">//写入消息</span>
  <span class="hljs-keyword">if</span> (TE<span class="hljs-built_in">MP_FAILURE_RETRY</span>(write(sock_fd, msg_ptr, msg_len)) != (ssize_t) msg_len) {
    result = <span class="hljs-number">-1</span>;
  } <span class="hljs-keyword">else</span> {
    <span class="hljs-keyword">char</span> ack;
    <span class="hljs-comment">//等待应答消息</span>
    <span class="hljs-keyword">if</span> (TE<span class="hljs-built_in">MP_FAILURE_RETRY</span>(read(sock_fd, &amp;ack, <span class="hljs-number">1</span>)) != <span class="hljs-number">1</span>) {
      result = <span class="hljs-number">-1</span>;
    }
  }
  <span class="hljs-keyword">return</span> result;
}
</code></pre></div></div>

<h3 id="27-小节">2.7 小节<a class="anchorjs-link " href="#27-小节" aria-label="Anchor link for: 27 小节" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>通过调用<code class="highlighter-rouge">debuggerd &lt;tid&gt;</code>命令调用流程图：</p>

<p><img src="/images/debug/debuggerd_client.jpg" alt="debuggerd_client"></p>

<p>执行debuggerd命令最终都是调用send_request()方法，向debuggerd服务端发出<code class="highlighter-rouge">DEBUGGER_ACTION_DUMP_TOMBSTONE</code>或者<code class="highlighter-rouge">DEBUGGER_ACTION_DUMP_BACKTRACE</code>请求，那对于debuggerd服务端收到相应命令做了哪些操作呢，要想明白这个过程，接下来看看debuggerd服务端的工作。</p>

<h2 id="三debuggerd服务端">三、debuggerd服务端<a class="anchorjs-link " href="#三debuggerd服务端" aria-label="Anchor link for: 三debuggerd服务端" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h2>

<p>在执行debuggerd命令之前，debuggerd服务端早早就以准备就绪，时刻等待着client请求的到来。</p>

<h3 id="31-debuggerdrc">3.1 debuggerd.rc<a class="anchorjs-link " href="#31-debuggerdrc" aria-label="Anchor link for: 31 debuggerdrc" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>由init进程fork子进程来以daemon方式启动，定义在debuggerd.rc文件（旧版本位于init.rc）</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs perl">service debuggerd /<span class="hljs-keyword">system</span>/bin/debuggerd
    group root readproc
    writepid /dev/cpuset/<span class="hljs-keyword">system</span>-background/tasks
</code></pre></div></div>

<p>init进程会解析上述rc文件，调用/system/bin/debuggerd文件，进入main方法，此时不带有任何参数。
接下来进入main()方法。</p>

<h3 id="32-main">3.2 main<a class="anchorjs-link " href="#32-main" aria-label="Anchor link for: 32 main" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; /debuggerd/debuggerd.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs objectivec"><span class="hljs-keyword">int</span> main(<span class="hljs-keyword">int</span> argc, <span class="hljs-keyword">char</span>** argv) {
  <span class="hljs-keyword">union</span> selinux_callback cb;
  <span class="hljs-comment">//当参数个数为1则启动服务</span>
  <span class="hljs-keyword">if</span> (argc == <span class="hljs-number">1</span>) {
    cb.func_audit = audit_callback;
    selinux_set_callback(SELINUX_CB_AUDIT, cb);
    cb.func_log = selinux_log_callback;
    selinux_set_callback(SELINUX_CB_LOG, cb);
    <span class="hljs-comment">//【见小节3.3】</span>
    <span class="hljs-keyword">return</span> do_server();
  }
  ...
}
</code></pre></div></div>

<h3 id="33-do_server">3.3 do_server<a class="anchorjs-link " href="#33-do_server" aria-label="Anchor link for: 33 do_server" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; /debuggerd/debuggerd.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">int</span> <span class="hljs-title">do_server</span><span class="hljs-params">()</span> </span>{
  <span class="hljs-comment">//忽略debuggerd进程自身crash的处理过程。重置所有crash handlers</span>
  signal(SIGABRT, SIG_DFL);
  signal(SIGBUS, SIG_DFL);
  signal(SIGFPE, SIG_DFL);
  signal(SIGILL, SIG_DFL);
  signal(SIGSEGV, SIG_DFL);
<span class="hljs-meta">#<span class="hljs-meta-keyword">ifdef</span> SIGSTKFLT</span>
  signal(SIGSTKFLT, SIG_DFL);
<span class="hljs-meta">#<span class="hljs-meta-keyword">endif</span></span>
  signal(SIGTRAP, SIG_DFL);

  <span class="hljs-comment">//忽略向已关闭socket执行写操作失败的信号</span>
  signal(SIGPIPE, SIG_IGN);
  <span class="hljs-comment">//阻塞SIGCHLD</span>
  <span class="hljs-keyword">sigset_t</span> sigchld;
  sigemptyset(&amp;sigchld);
  sigaddset(&amp;sigchld, SIGCHLD);
  sigprocmask(SIG_SETMASK, &amp;sigchld, <span class="hljs-literal">nullptr</span>);
  <span class="hljs-comment">//建立socket通信中的服务端</span>
  <span class="hljs-keyword">int</span> s = socket_local_server(SOCKET_NAME, ANDROID_SOCKET_NAMESPACE_ABSTRACT,
                              SOCK_STREAM | SOCK_CLOEXEC);
  <span class="hljs-keyword">if</span> (s == <span class="hljs-number">-1</span>) return <span class="hljs-number">1</span>;

  <span class="hljs-comment">// Fork子进程来发送信号(同样具有root权限)，并监听pipe来暂停和恢复目标进程</span>
  <span class="hljs-keyword">if</span> (!start_signal_sender()) {
    ALOGE(<span class="hljs-string">"debuggerd: failed to fork signal sender"</span>);
    return <span class="hljs-number">1</span>;
  }
  ALOGI(<span class="hljs-string">"debuggerd: starting\n"</span>);

  <span class="hljs-keyword">for</span> (;;) {
    sockaddr_storage ss;
    sockaddr* addrp = <span class="hljs-keyword">reinterpret_cast</span>&lt;sockaddr*&gt;(&amp;ss);
    <span class="hljs-keyword">socklen_t</span> alen = <span class="hljs-keyword">sizeof</span>(ss);
    <span class="hljs-comment">//等待客户端连接</span>
    ALOGV(<span class="hljs-string">"waiting for connection\n"</span>);
    <span class="hljs-keyword">int</span> fd = accept4(s, addrp, &amp;alen, SOCK_CLOEXEC);
    <span class="hljs-keyword">if</span> (fd == <span class="hljs-number">-1</span>) {
      ALOGE(<span class="hljs-string">"accept failed: %s\n"</span>, strerror(errno));
      <span class="hljs-keyword">continue</span>;
    }
    <span class="hljs-comment">//处理新连接的客户端请求【见小节3.4】</span>
    handle_request(fd);
  }
  return <span class="hljs-number">0</span>;
}
</code></pre></div></div>

<p>主要功能：</p>

<ul>
  <li>忽略debuggerd进程自身crash的处理过程；</li>
  <li>重置所有crash handlers；</li>
  <li>建立socket通信中的服务端；</li>
  <li>循环等待客户端的连接，并调用handle_request处理客户端请求。</li>
</ul>

<h3 id="34-handle_request">3.4 handle_request<a class="anchorjs-link " href="#34-handle_request" aria-label="Anchor link for: 34 handle_request" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; /debuggerd/debuggerd.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">void</span> <span class="hljs-title">handle_request</span><span class="hljs-params">(<span class="hljs-keyword">int</span> fd)</span> </span>{
  ALOGV(<span class="hljs-string">"handle_request(%d)\n"</span>, fd);
  android::base::<span class="hljs-function">unique_fd <span class="hljs-title">closer</span><span class="hljs-params">(fd)</span></span>;
  <span class="hljs-keyword">debugger_request_t</span> request;
  <span class="hljs-built_in">memset</span>(&amp;request, <span class="hljs-number">0</span>, <span class="hljs-keyword">sizeof</span>(request));
  <span class="hljs-comment">//读取client发送过来的请求【见小节3.5】</span>
  <span class="hljs-keyword">int</span> status = read_request(fd, &amp;request);
  <span class="hljs-keyword">if</span> (status != <span class="hljs-number">0</span>) {
    return;
  }

<span class="hljs-meta">#<span class="hljs-meta-keyword">if</span> defined(__LP64__)</span>
  <span class="hljs-comment">//对于32位的进程，重定向到32位debuggerd</span>
  <span class="hljs-keyword">if</span> (is32bit(request.tid)) {
    <span class="hljs-comment">//仅仅dump backtrace和tombstone请求能重定向</span>
    <span class="hljs-keyword">if</span> (request.action == DEBUGGER_ACTION_DUMP_BACKTRACE ||
        request.action == DEBUGGER_ACTION_DUMP_TOMBSTONE) {
      redirect_to_32(fd, &amp;request);
    }
    return;
  }
<span class="hljs-meta">#<span class="hljs-meta-keyword">endif</span></span>

  <span class="hljs-comment">//fork子进程来处理其余请求命令</span>
  <span class="hljs-keyword">pid_t</span> fork_pid = fork();
  <span class="hljs-keyword">if</span> (fork_pid == <span class="hljs-number">-1</span>) {
    ALOGE(<span class="hljs-string">"debuggerd: failed to fork: %s\n"</span>, strerror(errno));
  } <span class="hljs-keyword">else</span> <span class="hljs-keyword">if</span> (fork_pid == <span class="hljs-number">0</span>) {
     <span class="hljs-comment">//子进程执行【见小节3.6】</span>
    worker_process(fd, request);
  } <span class="hljs-keyword">else</span> {
    <span class="hljs-comment">//父进程执行【见小节3.7】</span>
    monitor_worker_process(fork_pid, request);
  }
}
</code></pre></div></div>

<h3 id="35-read_request">3.5 read_request<a class="anchorjs-link " href="#35-read_request" aria-label="Anchor link for: 35 read_request" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

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
    <span class="hljs-comment">// C/C++进程crash时发送过来的请求</span>
    <span class="hljs-keyword">char</span> buf[<span class="hljs-number">64</span>];
    <span class="hljs-keyword">struct</span> stat s;
    <span class="hljs-built_in">snprintf</span>(buf, <span class="hljs-keyword">sizeof</span> buf, <span class="hljs-string">"/proc/%d/task/%d"</span>, out_request-&gt;pid, out_request-&gt;tid);
    <span class="hljs-keyword">if</span> (stat(buf, &amp;s)) {
      return <span class="hljs-number">-1</span>;  <span class="hljs-comment">//tid不存在，忽略该显式dump请求</span>
    }
  } <span class="hljs-keyword">else</span> <span class="hljs-keyword">if</span> (cr.uid == <span class="hljs-number">0</span>
            || (cr.uid == AID_SYSTEM &amp;&amp; msg.action == DEBUGGER_ACTION_DUMP_BACKTRACE)) {
    <span class="hljs-comment">//root权限既可以可收集backtraces，又可以dump tombstones；</span>
    <span class="hljs-comment">//system权限只允许收集backtraces;</span>
    status = get_process_info(out_request-&gt;tid, &amp;out_request-&gt;pid,
                              &amp;out_request-&gt;uid, &amp;out_request-&gt;gid);
    <span class="hljs-keyword">if</span> (status &lt; <span class="hljs-number">0</span>) {
      return <span class="hljs-number">-1</span>; <span class="hljs-comment">//tid不存在，忽略该显式dump请求</span>
    }

    <span class="hljs-keyword">if</span> (!selinux_action_allowed(fd, out_request))
      return <span class="hljs-number">-1</span>; <span class="hljs-comment">//selinux权限不足，忽略该请求</span>

  } <span class="hljs-keyword">else</span> {
    <span class="hljs-comment">//其他情况，则直接忽略</span>
    return <span class="hljs-number">-1</span>;
  }
  return <span class="hljs-number">0</span>;
}
</code></pre></div></div>

<p>该方法的功能是首先从socket获取client进程的pid,uid,gid用于权限控制，能处理以下三种情况：</p>

<ul>
  <li>C/C++进程crash时发送过来的请求；</li>
  <li>root权限既可以可收集backtraces，又可以dump tombstones；</li>
  <li>system权限只允许收集backtraces。</li>
</ul>

<p>针对这些情况若相应的tid不存在或selinux权限不满足，则都忽略该显式dump请求。read_request执行完成后，则从socket通道中读取到request信息。</p>

<h3 id="36-worker_process">3.6 worker_process<a class="anchorjs-link " href="#36-worker_process" aria-label="Anchor link for: 36 worker_process" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>处于client发送过来的请求，server端通过子进程来处理</p>

<p>[-&gt; /debuggerd/debuggerd.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">void</span> <span class="hljs-title">worker_process</span><span class="hljs-params">(<span class="hljs-keyword">int</span> fd, debugger_request_t&amp; request)</span> </span>{
  <span class="hljs-built_in">std</span>::<span class="hljs-built_in">string</span> tombstone_path;
  <span class="hljs-keyword">int</span> tombstone_fd = <span class="hljs-number">-1</span>;
  <span class="hljs-keyword">switch</span> (request.action) {
    <span class="hljs-keyword">case</span> DEBUGGER_ACTION_DUMP_TOMBSTONE: <span class="hljs-comment">//case1:输出tombstone文件</span>
    <span class="hljs-keyword">case</span> DEBUGGER_ACTION_CRASH: <span class="hljs-comment">//case2:出现native crash</span>
      <span class="hljs-comment">//打开tombstone文件【见小节3.6.1】</span>
      tombstone_fd = open_tombstone(&amp;tombstone_path);
      <span class="hljs-keyword">if</span> (tombstone_fd == <span class="hljs-number">-1</span>) {
        <span class="hljs-built_in">exit</span>(<span class="hljs-number">1</span>); <span class="hljs-comment">//无法打开tombstone文件，则退出该进程</span>
      }
      <span class="hljs-keyword">break</span>;

    <span class="hljs-keyword">case</span> DEBUGGER_ACTION_DUMP_BACKTRACE: <span class="hljs-comment">//case3:输出backtrace</span>
      <span class="hljs-keyword">break</span>;

    <span class="hljs-keyword">default</span>:
      <span class="hljs-built_in">exit</span>(<span class="hljs-number">1</span>); <span class="hljs-comment">//其他case则直接结束进程</span>
  }

  <span class="hljs-comment">// Attach到目标进程</span>
  <span class="hljs-keyword">if</span> (ptrace(PTRACE_ATTACH, request.tid, <span class="hljs-number">0</span>, <span class="hljs-number">0</span>) != <span class="hljs-number">0</span>) {
    <span class="hljs-built_in">exit</span>(<span class="hljs-number">1</span>); <span class="hljs-comment">//attach失败则退出该进程</span>
  }

  <span class="hljs-keyword">bool</span> attach_gdb = should_attach_gdb(request);
  <span class="hljs-keyword">if</span> (attach_gdb) {
    <span class="hljs-comment">// 在特权模式降级之前，打开所有需要监听的input设备</span>
    <span class="hljs-keyword">if</span> (init_getevent() != <span class="hljs-number">0</span>) {
      attach_gdb = <span class="hljs-literal">false</span>; <span class="hljs-comment">//初始化input设备失败，不再等待gdb</span>
    }
  }
  ...

  <span class="hljs-comment">//生成backtrace【见小节3.6.2】</span>
  <span class="hljs-built_in">std</span>::<span class="hljs-built_in">unique_ptr</span>&lt;BacktraceMap&gt; backtrace_map(BacktraceMap::Create(request.pid));

  <span class="hljs-keyword">int</span> amfd = <span class="hljs-number">-1</span>;
  <span class="hljs-built_in">std</span>::<span class="hljs-built_in">unique_ptr</span>&lt;<span class="hljs-built_in">std</span>::<span class="hljs-built_in">string</span>&gt; amfd_data;
  <span class="hljs-keyword">if</span> (request.action == DEBUGGER_ACTION_CRASH) {
    <span class="hljs-comment">//当发生native crash，则连接到AMS【见小节3.6.3】</span>
    amfd = activity_manager_connect();
    amfd_data.reset(new <span class="hljs-built_in">std</span>::<span class="hljs-built_in">string</span>);
  }

  <span class="hljs-keyword">bool</span> succeeded = <span class="hljs-literal">false</span>;

  <span class="hljs-comment">//取消特权模式</span>
  <span class="hljs-keyword">if</span> (!drop_privileges()) {
    _exit(<span class="hljs-number">1</span>); <span class="hljs-comment">//操作失败，则退出</span>
  }

  <span class="hljs-keyword">int</span> crash_signal = SIGKILL;
  <span class="hljs-comment">//执行dump操作，【见小节3.6.4】</span>
  succeeded = perform_dump(request, fd, tombstone_fd, backtrace_map.get(), siblings,
                           &amp;crash_signal, amfd_data.get());
  <span class="hljs-keyword">if</span> (succeeded) {
    <span class="hljs-keyword">if</span> (request.action == DEBUGGER_ACTION_DUMP_TOMBSTONE) {
      <span class="hljs-keyword">if</span> (!tombstone_path.empty()) {
        android::base::WriteFully(fd, tombstone_path.c_str(), tombstone_path.length());
      }
    }
  }

  <span class="hljs-keyword">if</span> (attach_gdb) {
    <span class="hljs-comment">//向目标进程发送SIGSTOP信号</span>
    <span class="hljs-keyword">if</span> (!send_signal(request.pid, <span class="hljs-number">0</span>, SIGSTOP)) {
      attach_gdb = <span class="hljs-literal">false</span>; <span class="hljs-comment">//无法停止通过gdb attach的进程</span>
    }
  }

  <span class="hljs-keyword">if</span> (!attach_gdb) {
    <span class="hljs-comment">//将进程crash情况告知AMS【见小节3.6.5】</span>
    activity_manager_write(request.pid, crash_signal, amfd, *amfd_data.get());
  }
  <span class="hljs-comment">//detach目标进程</span>
  ptrace(PTRACE_DETACH, request.tid, <span class="hljs-number">0</span>, <span class="hljs-number">0</span>);

  <span class="hljs-keyword">for</span> (<span class="hljs-keyword">pid_t</span> sibling : siblings) {
    ptrace(PTRACE_DETACH, sibling, <span class="hljs-number">0</span>, <span class="hljs-number">0</span>);
  }

  <span class="hljs-keyword">if</span> (!attach_gdb &amp;&amp; request.action == DEBUGGER_ACTION_CRASH) {
    <span class="hljs-comment">//发送信号SIGKILL给目标进程</span>
    <span class="hljs-keyword">if</span> (!send_signal(request.pid, request.tid, crash_signal)) {
      ALOGE(<span class="hljs-string">"debuggerd: failed to kill process %d: %s"</span>, request.pid, strerror(errno));
    }
  }

  <span class="hljs-comment">//如果需要则等待gdb</span>
  <span class="hljs-keyword">if</span> (attach_gdb) {
    wait_for_user_action(request);
    <span class="hljs-comment">//将进程crash情况告知AMS</span>
    activity_manager_write(request.pid, crash_signal, amfd, *amfd_data.get());

    <span class="hljs-comment">//发送信号SIGCONT给目标进程</span>
    <span class="hljs-keyword">if</span> (!send_signal(request.pid, <span class="hljs-number">0</span>, SIGCONT)) {
      ALOGE(<span class="hljs-string">"debuggerd: failed to resume process %d: %s"</span>, request.pid, strerror(errno));
    }

    uninit_getevent();
  }

  close(amfd);
  <span class="hljs-built_in">exit</span>(!succeeded);
}
</code></pre></div></div>

<p>这个流程比较长，这里介绍attach_gdb=false的执行流程</p>

<h4 id="361-open_tombstone">3.6.1 open_tombstone<a class="anchorjs-link " href="#361-open_tombstone" aria-label="Anchor link for: 361 open_tombstone" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<p>[-&gt; tombstone.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs objectivec"><span class="hljs-keyword">int</span> open_tombstone(std::string* out_path) {
  <span class="hljs-keyword">char</span> path[<span class="hljs-number">128</span>];
  <span class="hljs-keyword">int</span> fd = <span class="hljs-number">-1</span>;
  <span class="hljs-keyword">int</span> oldest = <span class="hljs-number">-1</span>;
  <span class="hljs-keyword">struct</span> stat oldest_sb;
  <span class="hljs-comment">//遍历查找</span>
  <span class="hljs-keyword">for</span> (<span class="hljs-keyword">int</span> i = <span class="hljs-number">0</span>; i &lt; MAX_TOMBSTONES; i++) {
    snprintf(path, <span class="hljs-keyword">sizeof</span>(path), TOMBSTONE_TE<span class="hljs-built_in">MPLATE</span>, i);
    <span class="hljs-keyword">struct</span> stat sb;
    <span class="hljs-keyword">if</span> (stat(path, &amp;sb) == <span class="hljs-number">0</span>) {
      <span class="hljs-comment">//记录修改时间最老的tombstone文件</span>
      <span class="hljs-keyword">if</span> (oldest &lt; <span class="hljs-number">0</span> || sb.st_mtime &lt; oldest_sb.st_mtime) {
        oldest = i;
        oldest_sb.st_mtime = sb.st_mtime;
      }
      <span class="hljs-keyword">continue</span>;
    }
    <span class="hljs-comment">//存在没有使用的tombstone文件，则打开并赋给out_path，然后直接返回</span>
    fd = open(path, O_CREAT | O_E<span class="hljs-built_in">XCL</span> | O_WRONLY | O_NOFOLLOW | O_CLOEXEC, <span class="hljs-number">0600</span>);

    <span class="hljs-keyword">if</span> (out_path) {
      *out_path = path;
    }
    fchown(fd, AID_SYSTEM, AID_SYSTEM);
    <span class="hljs-keyword">return</span> fd;
  }

   <span class="hljs-comment">//找不到最老的可用tombstone文件，则默认使用tombstone 0</span>
  <span class="hljs-keyword">if</span> (oldest &lt; <span class="hljs-number">0</span>) {
    oldest = <span class="hljs-number">0</span>;
  }

  snprintf(path, <span class="hljs-keyword">sizeof</span>(path), TOMBSTONE_TE<span class="hljs-built_in">MPLATE</span>, oldest);
  <span class="hljs-comment">//打开最老的tombstone文件</span>
  fd = open(path, O_CREAT | O_TRUNC | O_WRONLY | O_NOFOLLOW | O_CLOEXEC, <span class="hljs-number">0600</span>);
  ...

  <span class="hljs-keyword">if</span> (out_path) {
    *out_path = path;
  }
  fchown(fd, AID_SYSTEM, AID_SYSTEM);
  <span class="hljs-keyword">return</span> fd;
}
</code></pre></div></div>

<p>其中TOMBSTONE_TEMPLATE为<code class="highlighter-rouge">data/tombstones/tombstone_%02d</code>，文件个数上限<code class="highlighter-rouge">MAX_TOMBSTONES</code>=10</p>

<p>打开tombstone文件规则：</p>

<ol>
  <li>当已使用的tombstone文件个数小于10时，则创建新的tombstone文件；否则执行2；</li>
  <li>获取修改时间最老的tombstone文件，并覆写该文件；</li>
  <li>如果最老文件不存在，则默认使用文件<code class="highlighter-rouge">data/tombstones/tombstone_00</code></li>
</ol>

<h4 id="362-backtracemapcreate">3.6.2 BacktraceMap::Create<a class="anchorjs-link " href="#362-backtracemapcreate" aria-label="Anchor link for: 362 backtracemapcreate" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<p>[-&gt; BacktraceMap.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp">BacktraceMap* BacktraceMap::Create(<span class="hljs-keyword">pid_t</span> pid, <span class="hljs-keyword">bool</span> <span class="hljs-comment">/*uncached*/</span>) {
  BacktraceMap* <span class="hljs-built_in">map</span> = new BacktraceMap(pid);
  <span class="hljs-keyword">if</span> (!<span class="hljs-built_in">map</span>-&gt;Build()) {
    <span class="hljs-keyword">delete</span> <span class="hljs-built_in">map</span>;
    return <span class="hljs-literal">nullptr</span>;
  }
  return <span class="hljs-built_in">map</span>;
}
</code></pre></div></div>

<p>解析/proc/[pid]/maps, 生成BacktraceMap.</p>

<h4 id="363-activity_manager_connect">3.6.3 activity_manager_connect<a class="anchorjs-link " href="#363-activity_manager_connect" aria-label="Anchor link for: 363 activity_manager_connect" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

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
    ALOGE(<span class="hljs-string">"debuggerd: Unable to connect to activity manager (setsockopt SO_SNDTIMEO failed: %s)"</span>,
          strerror(errno));
    <span class="hljs-keyword">return</span> <span class="hljs-number">-1</span>; <span class="hljs-comment">//无法连接到ActivityManager(setsockopt SO_SNDTIMEO失败)</span>
  }

  tv.tv_sec = <span class="hljs-number">3</span>;
  <span class="hljs-keyword">if</span> (setsockopt(amfd.get(), SOL_SOCKET, SO_RCVTIMEO, &amp;tv, <span class="hljs-keyword">sizeof</span>(tv)) == <span class="hljs-number">-1</span>) {
    ALOGE(<span class="hljs-string">"debuggerd: Unable to connect to activity manager (setsockopt SO_RCVTIMEO failed: %s)"</span>,
          strerror(errno));
    <span class="hljs-keyword">return</span> <span class="hljs-number">-1</span>; <span class="hljs-comment">//无法连接到ActivityManager(setsockopt SO_RCVTIMEO失败)</span>
  }

  <span class="hljs-keyword">return</span> amfd.release();
}
</code></pre></div></div>

<p>该方法的功能是建立与ActivityManager的socket连接。</p>

<h4 id="364-perform_dump">3.6.4 perform_dump<a class="anchorjs-link " href="#364-perform_dump" aria-label="Anchor link for: 364 perform_dump" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>
<p>根据接收到不同的signal采取相应的操作</p>

<p>[-&gt; debuggerd.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">bool</span> <span class="hljs-title">perform_dump</span><span class="hljs-params">(<span class="hljs-keyword">const</span> debugger_request_t&amp; request, <span class="hljs-keyword">int</span> fd, <span class="hljs-keyword">int</span> tombstone_fd,
                         BacktraceMap* backtrace_map, <span class="hljs-keyword">const</span> <span class="hljs-built_in">std</span>::<span class="hljs-built_in">set</span>&lt;pid_t&gt;&amp; siblings,
                         <span class="hljs-keyword">int</span>* crash_signal, <span class="hljs-built_in">std</span>::<span class="hljs-built_in">string</span>* amfd_data)</span> </span>{
  <span class="hljs-keyword">if</span> (TEMP_FAILURE_RETRY(write(fd, <span class="hljs-string">"\0"</span>, <span class="hljs-number">1</span>)) != <span class="hljs-number">1</span>) {
    ALOGE(<span class="hljs-string">"debuggerd: failed to respond to client: %s\n"</span>, strerror(errno));
    return <span class="hljs-literal">false</span>; <span class="hljs-comment">//无法响应client端请求</span>
  }

  <span class="hljs-keyword">int</span> total_sleep_time_usec = <span class="hljs-number">0</span>;
  <span class="hljs-keyword">while</span> (<span class="hljs-literal">true</span>) {
    <span class="hljs-comment">//等待信号到来</span>
    <span class="hljs-keyword">int</span> signal = wait_for_signal(request.tid, &amp;total_sleep_time_usec);
    <span class="hljs-keyword">switch</span> (signal) {
      <span class="hljs-keyword">case</span> <span class="hljs-number">-1</span>:
        ALOGE(<span class="hljs-string">"debuggerd: timed out waiting for signal"</span>);
        return <span class="hljs-literal">false</span>; <span class="hljs-comment">//等待超时</span>

      <span class="hljs-keyword">case</span> SIGSTOP:
        <span class="hljs-keyword">if</span> (request.action == DEBUGGER_ACTION_DUMP_TOMBSTONE) {
          ALOGV(<span class="hljs-string">"debuggerd: stopped -- dumping to tombstone"</span>);
          <span class="hljs-comment">//【见小节4.1】</span>
          engrave_tombstone(tombstone_fd, backtrace_map, request.pid, request.tid, siblings, signal,
                            request.original_si_code, request.abort_msg_address, amfd_data);
        } <span class="hljs-keyword">else</span> <span class="hljs-keyword">if</span> (request.action == DEBUGGER_ACTION_DUMP_BACKTRACE) {
          ALOGV(<span class="hljs-string">"debuggerd: stopped -- dumping to fd"</span>);
          <span class="hljs-comment">//【见小节4.4】</span>
          dump_backtrace(fd, backtrace_map, request.pid, request.tid, siblings, <span class="hljs-literal">nullptr</span>);
        } <span class="hljs-keyword">else</span> {
          ALOGV(<span class="hljs-string">"debuggerd: stopped -- continuing"</span>);
          <span class="hljs-keyword">if</span> (ptrace(PTRACE_CONT, request.tid, <span class="hljs-number">0</span>, <span class="hljs-number">0</span>) != <span class="hljs-number">0</span>) {
            ALOGE(<span class="hljs-string">"debuggerd: ptrace continue failed: %s"</span>, strerror(errno));
            return <span class="hljs-literal">false</span>;
          }
          <span class="hljs-keyword">continue</span>;  <span class="hljs-comment">//再次循环</span>
        }
        <span class="hljs-keyword">break</span>;

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
        <span class="hljs-comment">//【见小节4.1】</span>
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

<p>致命信号有SIGABRT，SIGBUS，SIGFPE，SIGILL，SIGSEGV，SIGSTKFLT，SIGTRAP共7个信息，能造成native crash。</p>

<h4 id="365-activity_manager_write">3.6.5 activity_manager_write<a class="anchorjs-link " href="#365-activity_manager_write" aria-label="Anchor link for: 365 activity_manager_write" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<p>[-&gt; debuggerd.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs php"><span class="hljs-keyword">static</span> void activity_manager_write(int pid, int signal, int amfd, <span class="hljs-keyword">const</span> std::string&amp; amfd_data) {
  <span class="hljs-keyword">if</span> (amfd == <span class="hljs-number">-1</span>) {
    <span class="hljs-keyword">return</span>;
  }

  <span class="hljs-comment">//写入32-bit pid和signal，以及原始dump信息，最后添加0以标记结束</span>
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

<h3 id="37-monitor_worker_process">3.7 monitor_worker_process<a class="anchorjs-link " href="#37-monitor_worker_process" aria-label="Anchor link for: 37 monitor_worker_process" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>父进程处理</p>

<p>[-&gt; debuggerd.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs bash">static void monitor_worker_process(int child_pid, const debugger_request_t&amp; request) {
  struct timespec timeout = {.tv_sec = 10, .tv_nsec = 0 };
  <span class="hljs-keyword">if</span> (should_attach_gdb(request)) {
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

  <span class="hljs-keyword">if</span> (<span class="hljs-built_in">kill</span>_worker) {
    // Something bad happened, <span class="hljs-built_in">kill</span> the worker.
    <span class="hljs-keyword">if</span> (<span class="hljs-built_in">kill</span>(child_pid, SIGKILL) != 0) {
      ALOGE(<span class="hljs-string">"debuggerd: failed to kill worker process %d: %s"</span>, child_pid, strerror(errno));
    } <span class="hljs-keyword">else</span> {
      waitpid(child_pid, &amp;status, 0);
    }
  }

  int <span class="hljs-built_in">exit</span>_signal = SIGCONT;
  <span class="hljs-keyword">if</span> (<span class="hljs-built_in">kill</span>_target &amp;&amp; request.action == DEBUGGER_ACTION_CRASH) {
    ALOGE(<span class="hljs-string">"debuggerd: killing target %d"</span>, request.pid);
    <span class="hljs-built_in">exit</span>_signal = SIGKILL;
  } <span class="hljs-keyword">else</span> {
    ALOGW(<span class="hljs-string">"debuggerd: resuming target %d"</span>, request.pid);
  }

  <span class="hljs-keyword">if</span> (<span class="hljs-built_in">kill</span>(request.pid, <span class="hljs-built_in">exit</span>_signal) != 0) {
    ALOGE(<span class="hljs-string">"debuggerd: failed to send signal %d to target: %s"</span>, <span class="hljs-built_in">exit</span>_signal, strerror(errno));
  }

  <span class="hljs-keyword">if</span> (<span class="hljs-built_in">kill</span>_self) {
    stop_signal_sender();
    _<span class="hljs-built_in">exit</span>(1);
  }
}
</code></pre></div></div>

<h3 id="38-小节">3.8 小节<a class="anchorjs-link " href="#38-小节" aria-label="Anchor link for: 38 小节" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>调用流程：</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs bash">debuggerd.main
    <span class="hljs-keyword">do</span>_server
        handle_request
            <span class="hljs-built_in">read</span>_request
                worker_process(子进程)
                monitor_worker_process(父进程)
</code></pre></div></div>

<p>整个过程的核心方法为<code class="highlighter-rouge">worker_process()</code>，其流程如下：</p>

<ol>
  <li>根据请worker_process()求中的不同action采取相应操作，除此之外则立即结束进程
    <ul>
      <li>DEBUGGER_ACTION_DUMP_TOMBSTONE，则调用open_tombstone并继续执行；</li>
      <li>DEBUGGER_ACTION_CRASH ，则调用open_tombstone并继续执行；</li>
      <li>DEBUGGER_ACTION_DUMP_BACKTRACE，则直接继续执行；</li>
    </ul>
  </li>
  <li>调用ptrace方法attach到目标进程;</li>
  <li>调用BacktraceMap::Create来生成backtrace;</li>
  <li>当Action=<code class="highlighter-rouge">DEBUGGER_ACTION_CRASH</code>，则执行activity_manager_connect；</li>
  <li>调用drop_privileges来取消特权模式；</li>
  <li>通过perform_dump执行dump操作：【见小节3.6.4】
    <ul>
      <li>SIGSTOP &amp;&amp; <code class="highlighter-rouge">DEBUGGER_ACTION_DUMP_BACKTRACE</code>，则<code class="highlighter-rouge">dump_backtrace</code>()</li>
      <li>SIGSTOP &amp;&amp; <code class="highlighter-rouge">DEBUGGER_ACTION_DUMP_TOMBSTONE</code>，则<code class="highlighter-rouge">engrave_tombstone</code>()</li>
      <li>SIGBUS等致命信号，则<code class="highlighter-rouge">engrave_tombstone</code>()</li>
    </ul>
  </li>
  <li>当Action=<code class="highlighter-rouge">DEBUGGER_ACTION_DUMP_TOMBSTONE</code>，则将向client端写入tombstone数据；</li>
  <li>调用activity_manager_write，将进程crash情况告知AMS；</li>
  <li>调用ptrace方法detach到目标进程;</li>
  <li>当Action=<code class="highlighter-rouge">DEBUGGER_ACTION_CRASH</code>，发送信号SIGKILL给目标进程tid</li>
  <li>调用exit来结束进程。</li>
</ol>

<p>整个过程中，【见小节3.6.4】perform_dump是核心过程：对于DEBUGGER_ACTION_DUMP_BACKTRACE命令，则执行<code class="highlighter-rouge">dump_backtrace</code>；否则执行<code class="highlighter-rouge">engrave_tombstone</code>。接下来分别说说这两个过程</p>

<h2 id="四tombstone">四、tombstone<a class="anchorjs-link " href="#四tombstone" aria-label="Anchor link for: 四tombstone" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h2>

<h3 id="41-engrave_tombstone">4.1 engrave_tombstone<a class="anchorjs-link " href="#41-engrave_tombstone" aria-label="Anchor link for: 41 engrave_tombstone" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; debuggerd/tombstone.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">engrave_tombstone</span><span class="hljs-params">(<span class="hljs-keyword">int</span> tombstone_fd, BacktraceMap* <span class="hljs-built_in">map</span>, pid_t pid, pid_t tid,
                       <span class="hljs-keyword">const</span> <span class="hljs-built_in">std</span>::<span class="hljs-built_in">set</span>&lt;pid_t&gt;&amp; siblings, <span class="hljs-keyword">int</span> signal, <span class="hljs-keyword">int</span> original_si_code,
                       uintptr_t abort_msg_address, <span class="hljs-built_in">std</span>::<span class="hljs-built_in">string</span>* amfd_data)</span> </span>{
  <span class="hljs-keyword">log_t</span> <span class="hljs-built_in">log</span>;
  <span class="hljs-built_in">log</span>.current_tid = tid;
  <span class="hljs-built_in">log</span>.crashed_tid = tid;

  <span class="hljs-keyword">if</span> (tombstone_fd &lt; <span class="hljs-number">0</span>) {
    ALOGE(<span class="hljs-string">"debuggerd: skipping tombstone write, nothing to do.\n"</span>);
    return;
  }

  <span class="hljs-built_in">log</span>.tfd = tombstone_fd;
  <span class="hljs-built_in">log</span>.amfd_data = amfd_data;
  <span class="hljs-comment">//【见小节4.2】</span>
  dump_crash(&amp;<span class="hljs-built_in">log</span>, <span class="hljs-built_in">map</span>, pid, tid, siblings, signal, original_si_code, abort_msg_address);
}
</code></pre></div></div>

<h3 id="42-dump_crash">4.2 dump_crash<a class="anchorjs-link " href="#42-dump_crash" aria-label="Anchor link for: 42 dump_crash" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; debuggerd/tombstone.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-comment">// Dump该pid所对应进程的所有tombstone信息</span>
<span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">void</span> <span class="hljs-title">dump_crash</span><span class="hljs-params">(log_t* <span class="hljs-built_in">log</span>, BacktraceMap* <span class="hljs-built_in">map</span>, pid_t pid, pid_t tid,
                       <span class="hljs-keyword">const</span> <span class="hljs-built_in">std</span>::<span class="hljs-built_in">set</span>&lt;pid_t&gt;&amp; siblings, <span class="hljs-keyword">int</span> signal, <span class="hljs-keyword">int</span> si_code,
                       uintptr_t abort_msg_address)</span> </span>{
  <span class="hljs-keyword">char</span> value[PROPERTY_VALUE_MAX];
  <span class="hljs-comment">//当ro.debuggable =1，则输出log信息</span>
  property_get(<span class="hljs-string">"ro.debuggable"</span>, value, <span class="hljs-string">"0"</span>);
  <span class="hljs-keyword">bool</span> want_logs = (value[<span class="hljs-number">0</span>] == <span class="hljs-string">'1'</span>);

  _LOG(<span class="hljs-built_in">log</span>, logtype::HEADER,
       <span class="hljs-string">"*** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***\n"</span>);
  <span class="hljs-comment">//tombstone头部信息【见小节4.3】</span>
  dump_header_info(<span class="hljs-built_in">log</span>);
  <span class="hljs-comment">//tombstone线程信息【见小节4.4】</span>
  dump_thread(<span class="hljs-built_in">log</span>, pid, tid, <span class="hljs-built_in">map</span>, signal, si_code, abort_msg_address, <span class="hljs-literal">true</span>);
  <span class="hljs-keyword">if</span> (want_logs) {
    <span class="hljs-comment">//输出log信息【见小节4.5】</span>
    dump_logs(<span class="hljs-built_in">log</span>, pid, <span class="hljs-number">5</span>);
  }

  <span class="hljs-keyword">if</span> (!siblings.empty()) {
    <span class="hljs-keyword">for</span> (<span class="hljs-keyword">pid_t</span> sibling : siblings) {
      <span class="hljs-comment">//tombstone兄弟线程信息【见小节4.4】</span>
      dump_thread(<span class="hljs-built_in">log</span>, pid, sibling, <span class="hljs-built_in">map</span>, <span class="hljs-number">0</span>, <span class="hljs-number">0</span>, <span class="hljs-number">0</span>, <span class="hljs-literal">false</span>);
    }
  }

  <span class="hljs-keyword">if</span> (want_logs) {
    dump_logs(<span class="hljs-built_in">log</span>, pid, <span class="hljs-number">0</span>);
  }
}
</code></pre></div></div>

<p>主要输出信息：</p>

<ul>
  <li>dump_header_info</li>
  <li>主线程dump_thread</li>
  <li>dump_logs (ro.debuggable=1 才输出此项)</li>
  <li>兄弟线程dump_thread</li>
  <li>dump_logs (ro.debuggable=1 才输出此项)</li>
</ul>

<h3 id="43-dump_header_info">4.3 dump_header_info<a class="anchorjs-link " href="#43-dump_header_info" aria-label="Anchor link for: 43 dump_header_info" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>[-&gt; debuggerd/tombstone.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">void</span> <span class="hljs-title">dump_header_info</span><span class="hljs-params">(log_t* <span class="hljs-built_in">log</span>)</span> </span>{
  <span class="hljs-keyword">char</span> fingerprint[PROPERTY_VALUE_MAX];
  <span class="hljs-keyword">char</span> revision[PROPERTY_VALUE_MAX];

  property_get(<span class="hljs-string">"ro.build.fingerprint"</span>, fingerprint, <span class="hljs-string">"unknown"</span>);
  property_get(<span class="hljs-string">"ro.revision"</span>, revision, <span class="hljs-string">"unknown"</span>);

  _LOG(<span class="hljs-built_in">log</span>, logtype::HEADER, <span class="hljs-string">"Build fingerprint: '%s'\n"</span>, fingerprint);
  _LOG(<span class="hljs-built_in">log</span>, logtype::HEADER, <span class="hljs-string">"Revision: '%s'\n"</span>, revision);
  _LOG(<span class="hljs-built_in">log</span>, logtype::HEADER, <span class="hljs-string">"ABI: '%s'\n"</span>, ABI_STRING);
}
</code></pre></div></div>

<p>例如：</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs nginx"><span class="hljs-attribute">Build</span> fingerprint: <span class="hljs-string">'xxx/xxx/MMB29M/gityuan06080845:userdebug/test-keys'</span>
Revision: <span class="hljs-string">'0'</span>
ABI: <span class="hljs-string">'arm'</span>
</code></pre></div></div>

<h3 id="44-dump_thread主">4.4 dump_thread(主)<a class="anchorjs-link " href="#44-dump_thread主" aria-label="Anchor link for: 44 dump_thread主" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>调用方法dump_thread(log, pid, tid, map, signal, si_code, abort_msg_address, true);</p>

<p>[-&gt; debuggerd/tombstone.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">void</span> <span class="hljs-title">dump_thread</span><span class="hljs-params">(log_t* <span class="hljs-built_in">log</span>, pid_t pid, pid_t tid, BacktraceMap* <span class="hljs-built_in">map</span>, <span class="hljs-keyword">int</span> signal,
                        <span class="hljs-keyword">int</span> si_code, uintptr_t abort_msg_address, <span class="hljs-keyword">bool</span> primary_thread)</span> </span>{
  <span class="hljs-built_in">log</span>-&gt;current_tid = tid;
  <span class="hljs-comment">//【见小节4.4.1】</span>
  dump_thread_info(<span class="hljs-built_in">log</span>, pid, tid);

  <span class="hljs-keyword">if</span> (signal) {
  <span class="hljs-comment">//【见小节4.4.2】</span>
    dump_signal_info(<span class="hljs-built_in">log</span>, tid, signal, si_code);
  }
  <span class="hljs-comment">//【见小节3.6.2】</span>
  <span class="hljs-built_in">std</span>::<span class="hljs-built_in">unique_ptr</span>&lt;Backtrace&gt; backtrace(Backtrace::Create(pid, tid, map));
  <span class="hljs-keyword">if</span> (primary_thread) {
    <span class="hljs-comment">//【见小节4.4.3】</span>
    dump_abort_message(backtrace.get(), <span class="hljs-built_in">log</span>, abort_msg_address);
  }
  <span class="hljs-comment">//【见小节4.4.4】</span>
  dump_registers(<span class="hljs-built_in">log</span>, tid);
  <span class="hljs-keyword">if</span> (backtrace-&gt;Unwind(<span class="hljs-number">0</span>)) {
    <span class="hljs-comment">//【见小节4.4.5】</span>
    dump_backtrace_and_stack(backtrace.get(), <span class="hljs-built_in">log</span>);
  } <span class="hljs-keyword">else</span> {
    ALOGE(<span class="hljs-string">"Unwind failed: pid = %d, tid = %d"</span>, pid, tid);
  }

  <span class="hljs-keyword">if</span> (primary_thread) {
    <span class="hljs-comment">//【见小节4.4.6】</span>
    dump_memory_and_code(<span class="hljs-built_in">log</span>, backtrace.get());
    <span class="hljs-keyword">if</span> (<span class="hljs-built_in">map</span>) {
        <span class="hljs-comment">//【见小节4.4.7】</span>
      dump_all_maps(backtrace.get(), <span class="hljs-built_in">map</span>, <span class="hljs-built_in">log</span>, tid);
    }
  }
  <span class="hljs-built_in">log</span>-&gt;current_tid = <span class="hljs-built_in">log</span>-&gt;crashed_tid;
}
</code></pre></div></div>

<h4 id="441-dump_thread_info">4.4.1 dump_thread_info<a class="anchorjs-link " href="#441-dump_thread_info" aria-label="Anchor link for: 441 dump_thread_info" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">void</span> <span class="hljs-title">dump_thread_info</span><span class="hljs-params">(log_t* <span class="hljs-built_in">log</span>, pid_t pid, pid_t tid)</span> </span>{
  <span class="hljs-keyword">char</span> path[<span class="hljs-number">64</span>];
  <span class="hljs-keyword">char</span> threadnamebuf[<span class="hljs-number">1024</span>];
  <span class="hljs-keyword">char</span>* threadname = <span class="hljs-literal">nullptr</span>;
  FILE *fp;
  <span class="hljs-comment">//获取/proc/&lt;tid&gt;/comm节点的线程名</span>
  <span class="hljs-built_in">snprintf</span>(path, <span class="hljs-keyword">sizeof</span>(path), <span class="hljs-string">"/proc/%d/comm"</span>, tid);
  <span class="hljs-keyword">if</span> ((fp = fopen(path, <span class="hljs-string">"r"</span>))) {
    threadname = fgets(threadnamebuf, sizeof(threadnamebuf), fp);
    fclose(fp);
    <span class="hljs-keyword">if</span> (threadname) {
      <span class="hljs-keyword">size_t</span> len = <span class="hljs-built_in">strlen</span>(threadname);
      <span class="hljs-keyword">if</span> (len &amp;&amp; threadname[len - <span class="hljs-number">1</span>] == <span class="hljs-string">'\n'</span>) {
        threadname[len - <span class="hljs-number">1</span>] = <span class="hljs-string">'\0'</span>;
      }
    }
  }
  <span class="hljs-comment">// Blacklist logd, logd.reader, logd.writer, logd.auditd, logd.control ...</span>
  <span class="hljs-keyword">static</span> <span class="hljs-keyword">const</span> <span class="hljs-keyword">char</span> logd[] = <span class="hljs-string">"logd"</span>;
  <span class="hljs-keyword">if</span> (threadname != <span class="hljs-literal">nullptr</span> &amp;&amp; !<span class="hljs-built_in">strncmp</span>(threadname, logd, sizeof(logd) - <span class="hljs-number">1</span>)
      &amp;&amp; (!threadname[sizeof(logd) - <span class="hljs-number">1</span>] || (threadname[sizeof(logd) - <span class="hljs-number">1</span>] == <span class="hljs-string">'.'</span>))) {
    <span class="hljs-built_in">log</span>-&gt;should_retrieve_logcat = <span class="hljs-literal">false</span>;
  }

  <span class="hljs-keyword">char</span> procnamebuf[<span class="hljs-number">1024</span>];
  <span class="hljs-keyword">char</span>* procname = <span class="hljs-literal">nullptr</span>;

  <span class="hljs-comment">//获取/proc/&lt;pid&gt;/cmdline节点的进程名</span>
  <span class="hljs-built_in">snprintf</span>(path, <span class="hljs-keyword">sizeof</span>(path), <span class="hljs-string">"/proc/%d/cmdline"</span>, pid);
  <span class="hljs-keyword">if</span> ((fp = fopen(path, <span class="hljs-string">"r"</span>))) {
    procname = fgets(procnamebuf, sizeof(procnamebuf), fp);
    fclose(fp);
  }

  _LOG(<span class="hljs-built_in">log</span>, logtype::HEADER, <span class="hljs-string">"pid: %d, tid: %d, name: %s  &gt;&gt;&gt; %s &lt;&lt;&lt;\n"</span>, pid, tid,
       threadname ? threadname : <span class="hljs-string">"UNKNOWN"</span>, procname ? procname : <span class="hljs-string">"UNKNOWN"</span>);
}
</code></pre></div></div>

<ul>
  <li>获取进程名：<code class="highlighter-rouge">/proc/&lt;pid&gt;/cmdline</code></li>
  <li>获取线程名：<code class="highlighter-rouge">/proc/&lt;tid&gt;/comm</code></li>
</ul>

<p>例如：</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs objectivec"><span class="hljs-comment">//代表system_server进程的主线程system_server</span>
pid: <span class="hljs-number">1789</span>, tid: <span class="hljs-number">1789</span>, name: system_server  &gt;&gt;&gt; system_server &lt;&lt;&lt;
<span class="hljs-comment">//代表system_server进程的子线程ActivityManager</span>
pid: <span class="hljs-number">1789</span>, tid: <span class="hljs-number">1827</span>, name: ActivityManager  &gt;&gt;&gt; system_server &lt;&lt;&lt;
</code></pre></div></div>

<h4 id="442-dump_signal_info">4.4.2 dump_signal_info<a class="anchorjs-link " href="#442-dump_signal_info" aria-label="Anchor link for: 442 dump_signal_info" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">void</span> <span class="hljs-title">dump_signal_info</span><span class="hljs-params">(log_t* <span class="hljs-built_in">log</span>, pid_t tid, <span class="hljs-keyword">int</span> signal, <span class="hljs-keyword">int</span> si_code)</span> </span>{
  <span class="hljs-keyword">siginfo_t</span> si;
  <span class="hljs-built_in">memset</span>(&amp;si, <span class="hljs-number">0</span>, <span class="hljs-keyword">sizeof</span>(si));
  <span class="hljs-keyword">if</span> (ptrace(PTRACE_GETSIGINFO, tid, <span class="hljs-number">0</span>, &amp;si) == <span class="hljs-number">-1</span>) {
    ALOGE(<span class="hljs-string">"cannot get siginfo: %s\n"</span>, strerror(errno));
    return;
  }

  si.si_code = si_code;

  <span class="hljs-keyword">char</span> addr_desc[<span class="hljs-number">32</span>];
  <span class="hljs-keyword">if</span> (signal_has_si_addr(signal)) {
    <span class="hljs-built_in">snprintf</span>(addr_desc, <span class="hljs-keyword">sizeof</span>(addr_desc), <span class="hljs-string">"%p"</span>, si.si_addr);
  } <span class="hljs-keyword">else</span> {
    <span class="hljs-built_in">snprintf</span>(addr_desc, sizeof(addr_desc), <span class="hljs-string">"--------"</span>);
  }

  _LOG(<span class="hljs-built_in">log</span>, logtype::HEADER, <span class="hljs-string">"signal %d (%s), code %d (%s), fault addr %s\n"</span>,
       signal, get_signame(signal), si.si_code, get_sigcode(signal, si.si_code), addr_desc);
}
</code></pre></div></div>

<ul>
  <li>对于<code class="highlighter-rouge">SIGBUS</code>，<code class="highlighter-rouge">SIGFPE</code>，<code class="highlighter-rouge">SIGILL</code>，<code class="highlighter-rouge">SIGSEGV</code>，<code class="highlighter-rouge">SIGTRAP</code>时触发的dump,则会输出fault addr的具体地址，</li>
  <li>对于<code class="highlighter-rouge">SIGSTOP</code>时,则输出fault addr为”——–”</li>
</ul>

<p>例如：</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs sql">signal 11 (SIGSEGV), code 1 (SEGV_MAPERR), fault addr 0xd140109c
signal 19 (SIGSTOP), code 0 (SI_USER), fault addr <span class="hljs-comment">--------</span>
</code></pre></div></div>

<p>此处<code class="highlighter-rouge">get_sigcode</code>函数功能负责根据signal以及si_code来获取相应信息，下面来列举每种signal所包含的信息种类：</p>

<div class="table-responsive"><table class="table">
  <thead>
    <tr>
      <th>signal</th>
      <th>get_sigcode</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>SIGILL</td>
      <td>ILL_ILLOPC</td>
    </tr>
    <tr>
      <td>SIGILL</td>
      <td>ILL_ILLOPN</td>
    </tr>
    <tr>
      <td>SIGILL</td>
      <td>ILL_ILLADR</td>
    </tr>
    <tr>
      <td>SIGILL</td>
      <td>ILL_ILLTRP</td>
    </tr>
    <tr>
      <td>SIGILL</td>
      <td>ILL_PRVOPC</td>
    </tr>
    <tr>
      <td>SIGILL</td>
      <td>ILL_PRVREG</td>
    </tr>
    <tr>
      <td>SIGILL</td>
      <td>ILL_COPROC</td>
    </tr>
    <tr>
      <td>SIGILL</td>
      <td>ILL_BADSTK</td>
    </tr>
  </tbody>
</table></div>

<div class="table-responsive"><table class="table">
  <thead>
    <tr>
      <th>signal</th>
      <th>get_sigcode</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>SIGBUS</td>
      <td>BUS_ADRALN</td>
    </tr>
    <tr>
      <td>SIGBUS</td>
      <td>BUS_ADRERR</td>
    </tr>
    <tr>
      <td>SIGBUS</td>
      <td>BUS_OBJERR</td>
    </tr>
    <tr>
      <td>SIGBUS</td>
      <td>BUS_MCEERR_AR</td>
    </tr>
    <tr>
      <td>SIGBUS</td>
      <td>BUS_MCEERR_AO</td>
    </tr>
  </tbody>
</table></div>

<div class="table-responsive"><table class="table">
  <thead>
    <tr>
      <th>signal</th>
      <th>get_sigcode</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>SIGFPE</td>
      <td>FPE_INTDIV</td>
    </tr>
    <tr>
      <td>SIGFPE</td>
      <td>FPE_INTOVF</td>
    </tr>
    <tr>
      <td>SIGFPE</td>
      <td>FPE_FLTDIV</td>
    </tr>
    <tr>
      <td>SIGFPE</td>
      <td>FPE_FLTOVF</td>
    </tr>
    <tr>
      <td>SIGFPE</td>
      <td>FPE_FLTUND</td>
    </tr>
    <tr>
      <td>SIGFPE</td>
      <td>FPE_FLTRES</td>
    </tr>
    <tr>
      <td>SIGFPE</td>
      <td>FPE_FLTINV</td>
    </tr>
    <tr>
      <td>SIGFPE</td>
      <td>FPE_FLTSUB</td>
    </tr>
  </tbody>
</table></div>

<div class="table-responsive"><table class="table">
  <thead>
    <tr>
      <th>signal</th>
      <th>get_sigcode</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>SIGSEGV</td>
      <td>SEGV_MAPERR</td>
    </tr>
    <tr>
      <td>SIGSEGV</td>
      <td>SEGV_ACCERR</td>
    </tr>
    <tr>
      <td>SIGSEGV</td>
      <td>SEGV_BNDERR</td>
    </tr>
    <tr>
      <td>SIGSEGV</td>
      <td>SEGV_MAPERR</td>
    </tr>
  </tbody>
</table></div>

<div class="table-responsive"><table class="table">
  <thead>
    <tr>
      <th>signal</th>
      <th>get_sigcode</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>SIGTRAP</td>
      <td>TRAP_BRKPT</td>
    </tr>
    <tr>
      <td>SIGTRAP</td>
      <td>TRAP_TRACE</td>
    </tr>
    <tr>
      <td>SIGTRAP</td>
      <td>TRAP_BRANCH</td>
    </tr>
    <tr>
      <td>SIGTRAP</td>
      <td>TRAP_HWBKPT</td>
    </tr>
  </tbody>
</table></div>

<h4 id="443-dump_abort_message">4.4.3 dump_abort_message<a class="anchorjs-link " href="#443-dump_abort_message" aria-label="Anchor link for: 443 dump_abort_message" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">void</span> <span class="hljs-title">dump_abort_message</span><span class="hljs-params">(Backtrace* backtrace, log_t* <span class="hljs-built_in">log</span>, uintptr_t address)</span> </span>{
  <span class="hljs-keyword">if</span> (address == <span class="hljs-number">0</span>) {
    return;
  }

  address += <span class="hljs-keyword">sizeof</span>(<span class="hljs-keyword">size_t</span>);  <span class="hljs-comment">// Skip the buffer length.</span>

  <span class="hljs-keyword">char</span> msg[<span class="hljs-number">512</span>];
  <span class="hljs-built_in">memset</span>(msg, <span class="hljs-number">0</span>, <span class="hljs-keyword">sizeof</span>(msg));
  <span class="hljs-keyword">char</span>* p = &amp;msg[<span class="hljs-number">0</span>];
  <span class="hljs-keyword">while</span> (p &lt; &amp;msg[<span class="hljs-keyword">sizeof</span>(msg)]) {
    <span class="hljs-keyword">word_t</span> data;
    <span class="hljs-keyword">size_t</span> len = <span class="hljs-keyword">sizeof</span>(<span class="hljs-keyword">word_t</span>);
    <span class="hljs-keyword">if</span> (!backtrace-&gt;ReadWord(address, &amp;data)) {
      <span class="hljs-keyword">break</span>;
    }
    address += <span class="hljs-keyword">sizeof</span>(<span class="hljs-keyword">word_t</span>);

    <span class="hljs-keyword">while</span> (len &gt; <span class="hljs-number">0</span> &amp;&amp; (*p++ = (data &gt;&gt; (sizeof(<span class="hljs-keyword">word_t</span>) - len) * <span class="hljs-number">8</span>) &amp; <span class="hljs-number">0xff</span>) != <span class="hljs-number">0</span>) {
      len--;
    }
  }
  msg[<span class="hljs-keyword">sizeof</span>(msg) - <span class="hljs-number">1</span>] = <span class="hljs-string">'\0'</span>;

  _LOG(<span class="hljs-built_in">log</span>, logtype::HEADER, <span class="hljs-string">"Abort message: '%s'\n"</span>, msg);
}
</code></pre></div></div>

<h4 id="444-dump_registers">4.4.4 dump_registers<a class="anchorjs-link " href="#444-dump_registers" aria-label="Anchor link for: 444 dump_registers" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>
<p>输出系统寄存器信息，这里以arm为例来说明</p>

<p>[-&gt; debuggerd/arm/Machine.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">dump_registers</span><span class="hljs-params">(log_t* <span class="hljs-built_in">log</span>, pid_t tid)</span> </span>{
  pt_regs r;
  <span class="hljs-keyword">if</span> (ptrace(PTRACE_GETREGS, tid, <span class="hljs-number">0</span>, &amp;r)) {
    ALOGE(<span class="hljs-string">"cannot get registers: %s\n"</span>, strerror(errno));
    return;
  }

  _LOG(<span class="hljs-built_in">log</span>, logtype::REGISTERS, <span class="hljs-string">"    r0 %08x  r1 %08x  r2 %08x  r3 %08x\n"</span>,
       <span class="hljs-keyword">static_cast</span>&lt;<span class="hljs-keyword">uint32_t</span>&gt;(r.ARM_r0), <span class="hljs-keyword">static_cast</span>&lt;<span class="hljs-keyword">uint32_t</span>&gt;(r.ARM_r1),
       <span class="hljs-keyword">static_cast</span>&lt;<span class="hljs-keyword">uint32_t</span>&gt;(r.ARM_r2), <span class="hljs-keyword">static_cast</span>&lt;<span class="hljs-keyword">uint32_t</span>&gt;(r.ARM_r3));
  _LOG(<span class="hljs-built_in">log</span>, logtype::REGISTERS, <span class="hljs-string">"    r4 %08x  r5 %08x  r6 %08x  r7 %08x\n"</span>,
       <span class="hljs-keyword">static_cast</span>&lt;<span class="hljs-keyword">uint32_t</span>&gt;(r.ARM_r4), <span class="hljs-keyword">static_cast</span>&lt;<span class="hljs-keyword">uint32_t</span>&gt;(r.ARM_r5),
       <span class="hljs-keyword">static_cast</span>&lt;<span class="hljs-keyword">uint32_t</span>&gt;(r.ARM_r6), <span class="hljs-keyword">static_cast</span>&lt;<span class="hljs-keyword">uint32_t</span>&gt;(r.ARM_r7));
  _LOG(<span class="hljs-built_in">log</span>, logtype::REGISTERS, <span class="hljs-string">"    r8 %08x  r9 %08x  sl %08x  fp %08x\n"</span>,
       <span class="hljs-keyword">static_cast</span>&lt;<span class="hljs-keyword">uint32_t</span>&gt;(r.ARM_r8), <span class="hljs-keyword">static_cast</span>&lt;<span class="hljs-keyword">uint32_t</span>&gt;(r.ARM_r9),
       <span class="hljs-keyword">static_cast</span>&lt;<span class="hljs-keyword">uint32_t</span>&gt;(r.ARM_r10), <span class="hljs-keyword">static_cast</span>&lt;<span class="hljs-keyword">uint32_t</span>&gt;(r.ARM_fp));
  _LOG(<span class="hljs-built_in">log</span>, logtype::REGISTERS, <span class="hljs-string">"    ip %08x  sp %08x  lr %08x  pc %08x  cpsr %08x\n"</span>,
       <span class="hljs-keyword">static_cast</span>&lt;<span class="hljs-keyword">uint32_t</span>&gt;(r.ARM_ip), <span class="hljs-keyword">static_cast</span>&lt;<span class="hljs-keyword">uint32_t</span>&gt;(r.ARM_sp),
       <span class="hljs-keyword">static_cast</span>&lt;<span class="hljs-keyword">uint32_t</span>&gt;(r.ARM_lr), <span class="hljs-keyword">static_cast</span>&lt;<span class="hljs-keyword">uint32_t</span>&gt;(r.ARM_pc),
       <span class="hljs-keyword">static_cast</span>&lt;<span class="hljs-keyword">uint32_t</span>&gt;(r.ARM_cpsr));

  user_vfp vfp_regs;
  <span class="hljs-keyword">if</span> (ptrace(PTRACE_GETVFPREGS, tid, <span class="hljs-number">0</span>, &amp;vfp_regs)) {
    ALOGE(<span class="hljs-string">"cannot get FP registers: %s\n"</span>, strerror(errno));
    return;
  }

  <span class="hljs-keyword">for</span> (<span class="hljs-keyword">size_t</span> i = <span class="hljs-number">0</span>; i &lt; <span class="hljs-number">32</span>; i += <span class="hljs-number">2</span>) {
    _LOG(<span class="hljs-built_in">log</span>, logtype::FP_REGISTERS, <span class="hljs-string">"    d%-2d %016llx  d%-2d %016llx\n"</span>,
         i, vfp_regs.fpregs[i], i+<span class="hljs-number">1</span>, vfp_regs.fpregs[i+<span class="hljs-number">1</span>]);
  }
  _LOG(<span class="hljs-built_in">log</span>, logtype::FP_REGISTERS, <span class="hljs-string">"    scr %08lx\n"</span>, vfp_regs.fpscr);
}
</code></pre></div></div>

<p>通过ptrace获取寄存器状态信息，这里输出r0-r9,sl,fp,ip,sp,lr,pc,cpsr 以及32个fpregs和一个fpscr.</p>

<h4 id="445-dump_backtrace_and_stack">4.4.5 dump_backtrace_and_stack<a class="anchorjs-link " href="#445-dump_backtrace_and_stack" aria-label="Anchor link for: 445 dump_backtrace_and_stack" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>
<p>[-&gt; debuggerd/tombstone.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">void</span> <span class="hljs-title">dump_backtrace_and_stack</span><span class="hljs-params">(Backtrace* backtrace, log_t* <span class="hljs-built_in">log</span>)</span> </span>{
  <span class="hljs-keyword">if</span> (backtrace-&gt;NumFrames()) {
    _LOG(<span class="hljs-built_in">log</span>, logtype::BACKTRACE, <span class="hljs-string">"\nbacktrace:\n"</span>);
    <span class="hljs-comment">//【见小节4.4.5.1】</span>
    dump_backtrace_to_log(backtrace, <span class="hljs-built_in">log</span>, <span class="hljs-string">"    "</span>);

    _LOG(<span class="hljs-built_in">log</span>, logtype::STACK, <span class="hljs-string">"\nstack:\n"</span>);
    <span class="hljs-comment">//【见小节4.4.5.2】</span>
    dump_stack(backtrace, <span class="hljs-built_in">log</span>);
  }
}
</code></pre></div></div>

<p><strong>4.4.5.1 输出backtrace信息</strong></p>

<p>[-&gt; debuggerd/Backtrace.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">dump_backtrace_to_log</span><span class="hljs-params">(Backtrace* backtrace, log_t* <span class="hljs-built_in">log</span>, <span class="hljs-keyword">const</span> <span class="hljs-keyword">char</span>* prefix)</span> </span>{
  <span class="hljs-keyword">for</span> (<span class="hljs-keyword">size_t</span> i = <span class="hljs-number">0</span>; i &lt; backtrace-&gt;NumFrames(); i++) {
    _LOG(<span class="hljs-built_in">log</span>, logtype::BACKTRACE, <span class="hljs-string">"%s%s\n"</span>, prefix, backtrace-&gt;FormatFrameData(i).c_str());
  }
}
</code></pre></div></div>

<p><strong>4.4.5.2 输出stack信息</strong></p>

<p>[-&gt; debuggerd/tombstone.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">void</span> <span class="hljs-title">dump_stack</span><span class="hljs-params">(Backtrace* backtrace, log_t* <span class="hljs-built_in">log</span>)</span> </span>{
  <span class="hljs-keyword">size_t</span> first = <span class="hljs-number">0</span>, last;
  <span class="hljs-keyword">for</span> (<span class="hljs-keyword">size_t</span> i = <span class="hljs-number">0</span>; i &lt; backtrace-&gt;NumFrames(); i++) {
    <span class="hljs-keyword">const</span> <span class="hljs-keyword">backtrace_frame_data_t</span>* frame = backtrace-&gt;GetFrame(i);
    <span class="hljs-keyword">if</span> (frame-&gt;sp) {
      <span class="hljs-keyword">if</span> (!first) {
        first = i+<span class="hljs-number">1</span>;
      }
      last = i;
    }
  }
  <span class="hljs-keyword">if</span> (!first) {
    return;
  }
  first--;

  <span class="hljs-comment">// Dump a few words before the first frame.</span>
  <span class="hljs-keyword">word_t</span> sp = backtrace-&gt;GetFrame(first)-&gt;sp - STACK_WORDS * <span class="hljs-keyword">sizeof</span>(<span class="hljs-keyword">word_t</span>);
  dump_stack_segment(backtrace, <span class="hljs-built_in">log</span>, &amp;sp, STACK_WORDS, <span class="hljs-number">-1</span>);

  <span class="hljs-comment">// Dump a few words from all successive frames.</span>
  <span class="hljs-comment">// Only log the first 3 frames, put the rest in the tombstone.</span>
  <span class="hljs-keyword">for</span> (<span class="hljs-keyword">size_t</span> i = first; i &lt;= last; i++) {
    <span class="hljs-keyword">const</span> <span class="hljs-keyword">backtrace_frame_data_t</span>* frame = backtrace-&gt;GetFrame(i);
    <span class="hljs-keyword">if</span> (sp != frame-&gt;sp) {
      _LOG(<span class="hljs-built_in">log</span>, logtype::STACK, <span class="hljs-string">"         ........  ........\n"</span>);
      sp = frame-&gt;sp;
    }
    <span class="hljs-keyword">if</span> (i == last) {
      dump_stack_segment(backtrace, <span class="hljs-built_in">log</span>, &amp;sp, STACK_WORDS, i);
      <span class="hljs-keyword">if</span> (sp &lt; frame-&gt;sp + frame-&gt;stack_size) {
        _LOG(<span class="hljs-built_in">log</span>, logtype::STACK, <span class="hljs-string">"         ........  ........\n"</span>);
      }
    } <span class="hljs-keyword">else</span> {
      <span class="hljs-keyword">size_t</span> words = frame-&gt;stack_size / <span class="hljs-keyword">sizeof</span>(<span class="hljs-keyword">word_t</span>);
      <span class="hljs-keyword">if</span> (words == <span class="hljs-number">0</span>) {
        words = <span class="hljs-number">1</span>;
      } <span class="hljs-keyword">else</span> <span class="hljs-keyword">if</span> (words &gt; STACK_WORDS) {
        words = STACK_WORDS;
      }
      dump_stack_segment(backtrace, <span class="hljs-built_in">log</span>, &amp;sp, words, i);
    }
  }
}
</code></pre></div></div>

<h4 id="446-dump_memory_and_code">4.4.6 dump_memory_and_code<a class="anchorjs-link " href="#446-dump_memory_and_code" aria-label="Anchor link for: 446 dump_memory_and_code" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>
<p>[-&gt; debuggerd/arm/Machine.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">void</span> <span class="hljs-title">dump_memory_and_code</span><span class="hljs-params">(log_t* <span class="hljs-built_in">log</span>, Backtrace* backtrace)</span> </span>{
  pt_regs regs;
  <span class="hljs-keyword">if</span> (ptrace(PTRACE_GETREGS, backtrace-&gt;Tid(), <span class="hljs-number">0</span>, &amp;regs)) {
    ALOGE(<span class="hljs-string">"cannot get registers: %s\n"</span>, strerror(errno));
    return;
  }

  <span class="hljs-keyword">static</span> <span class="hljs-keyword">const</span> <span class="hljs-keyword">char</span> reg_names[] = <span class="hljs-string">"r0r1r2r3r4r5r6r7r8r9slfpipsp"</span>;

  <span class="hljs-keyword">for</span> (<span class="hljs-keyword">int</span> reg = <span class="hljs-number">0</span>; reg &lt; <span class="hljs-number">14</span>; reg++) {
    dump_memory(<span class="hljs-built_in">log</span>, backtrace, regs.uregs[reg], <span class="hljs-string">"memory near %.2s:"</span>, &amp;reg_names[reg * <span class="hljs-number">2</span>]);
  }

  dump_memory(<span class="hljs-built_in">log</span>, backtrace, <span class="hljs-keyword">static_cast</span>&lt;<span class="hljs-keyword">uintptr_t</span>&gt;(regs.ARM_pc), <span class="hljs-string">"code around pc:"</span>);

  <span class="hljs-keyword">if</span> (regs.ARM_pc != regs.ARM_lr) {
    dump_memory(<span class="hljs-built_in">log</span>, backtrace, <span class="hljs-keyword">static_cast</span>&lt;<span class="hljs-keyword">uintptr_t</span>&gt;(regs.ARM_lr), <span class="hljs-string">"code around lr:"</span>);
  }
}
</code></pre></div></div>

<h4 id="447-dump_all_maps">4.4.7 dump_all_maps<a class="anchorjs-link " href="#447-dump_all_maps" aria-label="Anchor link for: 447 dump_all_maps" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<p>[-&gt; debuggerd/tombstone.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">void</span> <span class="hljs-title">dump_all_maps</span><span class="hljs-params">(Backtrace* backtrace, BacktraceMap* <span class="hljs-built_in">map</span>, log_t* <span class="hljs-built_in">log</span>, pid_t tid)</span> </span>{
  <span class="hljs-keyword">bool</span> print_fault_address_marker = <span class="hljs-literal">false</span>;
  <span class="hljs-keyword">uintptr_t</span> addr = <span class="hljs-number">0</span>;
  <span class="hljs-keyword">siginfo_t</span> si;
  <span class="hljs-built_in">memset</span>(&amp;si, <span class="hljs-number">0</span>, <span class="hljs-keyword">sizeof</span>(si));
  <span class="hljs-keyword">if</span> (ptrace(PTRACE_GETSIGINFO, tid, <span class="hljs-number">0</span>, &amp;si) != <span class="hljs-number">-1</span>) {
    print_fault_address_marker = signal_has_si_addr(si.si_signo);
    addr = <span class="hljs-keyword">reinterpret_cast</span>&lt;<span class="hljs-keyword">uintptr_t</span>&gt;(si.si_addr);
  } <span class="hljs-keyword">else</span> {
    ALOGE(<span class="hljs-string">"Cannot get siginfo for %d: %s\n"</span>, tid, strerror(errno));
  }

  _LOG(<span class="hljs-built_in">log</span>, logtype::MAPS, <span class="hljs-string">"\n"</span>);
  <span class="hljs-keyword">if</span> (!print_fault_address_marker) {
    _LOG(<span class="hljs-built_in">log</span>, logtype::MAPS, <span class="hljs-string">"memory map:\n"</span>);
  } <span class="hljs-keyword">else</span> {
    _LOG(log, logtype::MAPS, <span class="hljs-string">"memory map: (fault address prefixed with ---&gt;)\n"</span>);
    <span class="hljs-keyword">if</span> (<span class="hljs-built_in">map</span>-&gt;begin() != <span class="hljs-built_in">map</span>-&gt;end() &amp;&amp; addr &lt; <span class="hljs-built_in">map</span>-&gt;begin()-&gt;start) {
      _LOG(<span class="hljs-built_in">log</span>, logtype::MAPS, <span class="hljs-string">"---&gt;Fault address falls at %s before any mapped regions\n"</span>,
           get_addr_string(addr).c_str());
      print_fault_address_marker = <span class="hljs-literal">false</span>;
    }
  }

  <span class="hljs-built_in">std</span>::<span class="hljs-built_in">string</span> line;
  <span class="hljs-keyword">for</span> (BacktraceMap::const_iterator it = <span class="hljs-built_in">map</span>-&gt;begin(); it != <span class="hljs-built_in">map</span>-&gt;end(); ++it) {
    line = <span class="hljs-string">"    "</span>;
    <span class="hljs-keyword">if</span> (print_fault_address_marker) {
      <span class="hljs-keyword">if</span> (addr &lt; it-&gt;start) {
        _LOG(<span class="hljs-built_in">log</span>, logtype::MAPS, <span class="hljs-string">"---&gt;Fault address falls at %s between mapped regions\n"</span>,
             get_addr_string(addr).c_str());
        print_fault_address_marker = <span class="hljs-literal">false</span>;
      } <span class="hljs-keyword">else</span> <span class="hljs-keyword">if</span> (addr &gt;= it-&gt;start &amp;&amp; addr &lt; it-&gt;end) {
        line = <span class="hljs-string">"---&gt;"</span>;
        print_fault_address_marker = <span class="hljs-literal">false</span>;
      }
    }
    line += get_addr_string(it-&gt;start) + <span class="hljs-string">'-'</span> + get_addr_string(it-&gt;end - <span class="hljs-number">1</span>) + <span class="hljs-string">' '</span>;
    <span class="hljs-keyword">if</span> (it-&gt;flags &amp; PROT_READ) {
      line += <span class="hljs-string">'r'</span>;
    } <span class="hljs-keyword">else</span> {
      line += <span class="hljs-string">'-'</span>;
    }
    <span class="hljs-keyword">if</span> (it-&gt;flags &amp; PROT_WRITE) {
      line += <span class="hljs-string">'w'</span>;
    } <span class="hljs-keyword">else</span> {
      line += <span class="hljs-string">'-'</span>;
    }
    <span class="hljs-keyword">if</span> (it-&gt;flags &amp; PROT_EXEC) {
      line += <span class="hljs-string">'x'</span>;
    } <span class="hljs-keyword">else</span> {
      line += <span class="hljs-string">'-'</span>;
    }
    line += android::base::StringPrintf(<span class="hljs-string">"  %8"</span> PRIxPTR <span class="hljs-string">"  %8"</span> PRIxPTR,
                                        it-&gt;offset, it-&gt;end - it-&gt;start);
    <span class="hljs-keyword">bool</span> space_needed = <span class="hljs-literal">true</span>;
    <span class="hljs-keyword">if</span> (it-&gt;name.length() &gt; <span class="hljs-number">0</span>) {
      space_needed = <span class="hljs-literal">false</span>;
      line += <span class="hljs-string">"  "</span> + it-&gt;name;
      <span class="hljs-built_in">std</span>::<span class="hljs-built_in">string</span> build_id;
      <span class="hljs-keyword">if</span> ((it-&gt;flags &amp; PROT_READ) &amp;&amp; elf_get_build_id(backtrace, it-&gt;start, &amp;build_id)) {
        line += <span class="hljs-string">" (BuildId: "</span> + build_id + <span class="hljs-string">")"</span>;
      }
    }
    <span class="hljs-keyword">if</span> (it-&gt;load_base != <span class="hljs-number">0</span>) {
      <span class="hljs-keyword">if</span> (space_needed) {
        line += <span class="hljs-string">' '</span>;
      }
      line += android::base::StringPrintf(<span class="hljs-string">" (load base 0x%"</span> PRIxPTR <span class="hljs-string">")"</span>, it-&gt;load_base);
    }
    _LOG(<span class="hljs-built_in">log</span>, logtype::MAPS, <span class="hljs-string">"%s\n"</span>, line.c_str());
  }
  <span class="hljs-keyword">if</span> (print_fault_address_marker) {
    _LOG(<span class="hljs-built_in">log</span>, logtype::MAPS, <span class="hljs-string">"---&gt;Fault address falls at %s after any mapped regions\n"</span>,
         get_addr_string(addr).c_str());
  }
}
</code></pre></div></div>

<p>当内存出现故障时，可搜索关键词：</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs perl">memory <span class="hljs-keyword">map</span>: (fault address prefixed with ---&gt;)
</code></pre></div></div>

<h3 id="45-dump_logs">4.5 dump_logs<a class="anchorjs-link " href="#45-dump_logs" aria-label="Anchor link for: 45 dump_logs" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>
<p>[-&gt; debuggerd/tombstone.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">void</span> <span class="hljs-title">dump_logs</span><span class="hljs-params">(log_t* <span class="hljs-built_in">log</span>, pid_t pid, <span class="hljs-keyword">unsigned</span> <span class="hljs-keyword">int</span> tail)</span> </span>{
  dump_log_file(<span class="hljs-built_in">log</span>, pid, <span class="hljs-string">"system"</span>, tail); <span class="hljs-comment">//输出system log</span>
  dump_log_file(<span class="hljs-built_in">log</span>, pid, <span class="hljs-string">"main"</span>, tail); <span class="hljs-comment">//输出main log</span>
}
</code></pre></div></div>

<h3 id="46-dump_thread兄弟">4.6 dump_thread(兄弟)<a class="anchorjs-link " href="#46-dump_thread兄弟" aria-label="Anchor link for: 46 dump_thread兄弟" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p>dump_thread(log, pid, sibling, map, 0, 0, 0, false);</p>

<p>[-&gt; debuggerd/tombstone.cpp]</p>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs cpp"><span class="hljs-function"><span class="hljs-keyword">static</span> <span class="hljs-keyword">void</span> <span class="hljs-title">dump_thread</span><span class="hljs-params">(log_t* <span class="hljs-built_in">log</span>, pid_t pid, pid_t tid, BacktraceMap* <span class="hljs-built_in">map</span>, <span class="hljs-keyword">int</span> signal,
                        <span class="hljs-keyword">int</span> si_code, uintptr_t abort_msg_address, <span class="hljs-keyword">bool</span> primary_thread)</span> </span>{
  <span class="hljs-built_in">log</span>-&gt;current_tid = tid;

  <span class="hljs-keyword">if</span> (!primary_thread) {
    _LOG(<span class="hljs-built_in">log</span>, logtype::THREAD, <span class="hljs-string">"--- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---\n"</span>);
  }
  <span class="hljs-comment">//【见小节4.4.1】</span>
  dump_thread_info(<span class="hljs-built_in">log</span>, pid, tid);

  <span class="hljs-built_in">std</span>::<span class="hljs-built_in">unique_ptr</span>&lt;Backtrace&gt; backtrace(Backtrace::Create(pid, tid, map));

  <span class="hljs-comment">//【见小节4.4.4】</span>
  dump_registers(<span class="hljs-built_in">log</span>, tid);
  <span class="hljs-keyword">if</span> (backtrace-&gt;Unwind(<span class="hljs-number">0</span>)) {
    <span class="hljs-comment">//【见小节4.4.5】</span>
    dump_backtrace_and_stack(backtrace.get(), <span class="hljs-built_in">log</span>);
  }

  <span class="hljs-built_in">log</span>-&gt;current_tid = <span class="hljs-built_in">log</span>-&gt;crashed_tid;
}
</code></pre></div></div>

<p>兄弟线程dump_thread的输出内容：</p>

<ol>
  <li>线程相关信息，包含pid/tid以及相应name</li>
  <li>寄存器状态</li>
  <li>backtrace以及stack</li>
</ol>

<h3 id="47-小节">4.7 小节<a class="anchorjs-link " href="#47-小节" aria-label="Anchor link for: 47 小节" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h3>

<p><strong>engrave_tombstone</strong>主要输出信息：</p>

<ul>
  <li>dump_header_info</li>
  <li>主线程dump_thread 【见小节4.7.1】</li>
  <li>dump_logs (ro.debuggable=1 才输出此项)</li>
  <li>兄弟线程dump_thread 见小节4.7.2】</li>
  <li>dump_logs (ro.debuggable=1 才输出此项)</li>
</ul>

<h4 id="471-主线程dump_thread">4.7.1 主线程dump_thread<a class="anchorjs-link " href="#471-主线程dump_thread" aria-label="Anchor link for: 471 主线程dump_thread" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<ol>
  <li>build相关头信息；</li>
  <li>线程相关信息，包含pid/tid以及相应name</li>
  <li>signal相关信息，包含fault address</li>
  <li>寄存器状态</li>
  <li>backtrace</li>
  <li>stack</li>
  <li>memory near</li>
  <li>code around</li>
  <li>memory map</li>
</ol>

<h4 id="472-兄弟线程dump_thread">4.7.2 兄弟线程dump_thread<a class="anchorjs-link " href="#472-兄弟线程dump_thread" aria-label="Anchor link for: 472 兄弟线程dump_thread" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<ol>
  <li>线程相关信息，包含pid/tid以及相应name</li>
  <li>寄存器状态</li>
  <li>backtrace</li>
  <li>stack</li>
</ol>

<p>兄弟线程调用dump_thread也会输出的内容其实是主线程dump的第2，4，5，6项目。</p>

<h2 id="五实例">五、实例<a class="anchorjs-link " href="#五实例" aria-label="Anchor link for: 五实例" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h2>

<p>这里是dump_tombstone文件内容的组成：</p>

<h4 id="51-文件头信息">5.1 文件头信息<a class="anchorjs-link " href="#51-文件头信息" aria-label="Anchor link for: 51 文件头信息" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs markdown"><span class="hljs-strong">*** **</span><span class="hljs-bullet">* *</span><span class="hljs-strong">** **</span><span class="hljs-bullet">* *</span><span class="hljs-strong">** **</span><span class="hljs-bullet">* *</span><span class="hljs-strong">** **</span><span class="hljs-bullet">* *</span><span class="hljs-strong">** **</span><span class="hljs-bullet">* *</span><span class="hljs-strong">** **</span><span class="hljs-bullet">* *</span><span class="hljs-strong">** **</span><span class="hljs-bullet">* *</span><span class="hljs-strong">** **</span>*
//【见小节4.3】dump<span class="hljs-emphasis">_header_</span>info
Build fingerprint: 'xxx/xxx/MMB29M/gityuan06080845:userdebug/test-keys'
Revision: '0'
ABI: 'arm'
</code></pre></div></div>

<h4 id="52-主线程dump_thread">5.2 主线程dump_thread<a class="anchorjs-link " href="#52-主线程dump_thread" aria-label="Anchor link for: 52 主线程dump_thread" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs objectivec"><span class="hljs-comment">//【见小节4.4.1】dump_thread_info</span>
pid: <span class="hljs-number">1789</span>, tid: <span class="hljs-number">1789</span>, name: system_server  &gt;&gt;&gt; system_server &lt;&lt;&lt;
<span class="hljs-comment">//【见小节4.4.2】dump_signal_info</span>
signal <span class="hljs-number">19</span> (SIGSTOP), code <span class="hljs-number">0</span> (SI_USER), fault addr --------
<span class="hljs-comment">//【见小节4.4.4】dump_registers</span>
r0 fffffffc  r1 bed67e68  r2 <span class="hljs-number">00000010</span>  r3 <span class="hljs-number">0000</span>ea60
r4 <span class="hljs-number">00000000</span>  r5 <span class="hljs-number">00000008</span>  r6 <span class="hljs-number">00000000</span>  r7 <span class="hljs-number">0000015</span>a
...

<span class="hljs-comment">//【见小节4.4.5】dump_backtrace_and_stack</span>
backtrace:
    <span class="hljs-meta">#00 pc 00000000004489bc  /data/dalvik-cache/arm64/system@framework@boot.oat (offset 0x3e2e000)</span>
    <span class="hljs-meta">#01 pc 00000000003e8a74  /data/dalvik-cache/arm64/system@framework@boot.oat (offset 0x3e2e000)</span>

stack:
         <span class="hljs-number">0000007</span>ff47b26b0  <span class="hljs-number">0000000012</span>cf05e0  /dev/ashmem/dalvik-main space (deleted)
         <span class="hljs-number">0000007</span>ff47b26b8  <span class="hljs-number">0000000000000000</span>
         <span class="hljs-number">0000007</span>ff47b26c0  <span class="hljs-number">0000000012</span>cf05e0  /dev/ashmem/dalvik-main space (deleted)
<span class="hljs-comment">//【见小节4.4.6】dump_memory_and_code</span>
memory near r1:
...
code around pc:
code around lr:

<span class="hljs-comment">//【见小节4.4.7】dump_all_maps</span>
memory map:
<span class="hljs-comment">//【见小节4.5】dump_logs</span>
</code></pre></div></div>

<h4 id="53-兄弟线程dump_thread">5.3 兄弟线程dump_thread<a class="anchorjs-link " href="#53-兄弟线程dump_thread" aria-label="Anchor link for: 53 兄弟线程dump_thread" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h4>

<div class="highlighter-rouge"><div class="highlight"><pre class="highlight"><code class="hljs php"><span class="gd">--- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
</span><span class="hljs-comment">//【见小节4.4.1】dump_thread_info</span>
pid: <span class="hljs-number">1789</span>, tid: <span class="hljs-number">1803</span>, name: Binder_1  &gt;&gt;&gt; system_server &lt;&lt;&lt;
<span class="hljs-comment">//【见小节4.4.4】dump_registers</span>
    r0 <span class="hljs-number">0000000</span>b  r1 c0186201  r2 b3589868  r3 b3589860
<span class="hljs-comment">//【见小节4.4.5】dump_backtrace_and_stack</span>
backtrace:
    <span class="hljs-comment">#00 pc 00040aac  /system/lib/libc.so (__ioctl+8)</span>
    <span class="hljs-comment">#01 pc 00047529  /system/lib/libc.so (ioctl+14)</span>
    <span class="hljs-comment">#02 pc 0001e909  /system/lib/libbinder.so (_ZN7android14IPCThreadState14talkWithDriverEb+132)</span>

stack:
         b3589810  <span class="hljs-number">00000000</span>
         b3589814  <span class="hljs-number">00000000</span>
         b3589818  b6ebf07c  /system/lib/libcutils.so
         b358981c  b6eb4405  /system/lib/libcutils.so
</code></pre></div></div>

<p>所有兄弟线程是以一系列<code class="highlighter-rouge">---</code>作为开头的分割符。</p>

<h2 id="六-总结">六. 总结<a class="anchorjs-link " href="#六-总结" aria-label="Anchor link for: 六 总结" data-anchorjs-icon="#" style="opacity: 1; padding-left: 0.375em;"></a></h2>

<p>这里主要以源码角度来分析debuggerd的原理，整个过程中最重要的产物便是tombstone文件，先留坑，后续再进一步讲述如何分析tombstone文件。</p>

<ul>
  <li><code class="highlighter-rouge">debuggerd -b &lt;tid&gt;</code>:<br>
发送请求的action为<code class="highlighter-rouge">DEBUGGER_ACTION_DUMP_BACKTRACE</code>，则调用<code class="highlighter-rouge">dump_backtrace()</code>;<a href="http://gityuan.com/2016/11/27/native-traces/">Native进程之Trace原理</a></li>
  <li><code class="highlighter-rouge">debuggerd &lt;tid&gt;</code>:
发送请求的action为<code class="highlighter-rouge">DEBUGGER_ACTION_DUMP_TOMBSTONE</code>，则调用<code class="highlighter-rouge">engrave_tombstone()</code>;</li>
  <li><code class="highlighter-rouge">native crash</code>:
发送请求的action为<code class="highlighter-rouge">DEBUGGER_ACTION_CRASH</code>，且发送信号为SIGBUS等致命信号，则调用<code class="highlighter-rouge">engrave_tombstone()</code>。</li>
</ul>
