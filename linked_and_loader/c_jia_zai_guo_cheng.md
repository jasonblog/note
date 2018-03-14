<div class="post-container">
  <div class="blog-header">
    <h1>C 加载过程</h1>
    <div class="post-description">
      <i class="fa fa-calendar"></i> 2015-09-12 Saturday &nbsp; &nbsp;
      <i class="fa fa-tags"></i>  linux ,  program  
      
    </div>
  </div>
  <hr>
  <p>利用动态库，可以节省磁盘、内存空间，而且可以提高程序运行效率；不过同时也导致调试比较困难，而且可能存在潜在的安全威胁。</p>

<p>这里主要讨论符号的动态链接过程，即程序在执行过程中，对其中包含的一些未确定地址的符号进行重定位的过程。</p>

<!-- more -->

<h2 id="简介">简介</h2>

<p><code class="highlighter-rouge">ld.so</code> (<span class="contextHighlighter">Dynamic Linker</span>/Loader) 和 <code class="highlighter-rouge">ldd</code> 都会使用到 ELF 格式中的 <code class="highlighter-rouge">.dynstr</code> (dynamic linking string table) 字段，如果通过 <code class="highlighter-rouge">strip -R .dynstr hello</code> 命令将该字段删除，那么 <code class="highlighter-rouge">ldd</code> 就会报错。</p>

<h3 id="shell-执行">Shell 执行</h3>

<p>大致记录一下 bash 的执行过程，当打开终端后，可以通过 <code class="highlighter-rouge">tty</code> 命令查看当前的虚拟终端，假设为 <code class="highlighter-rouge">/dev/pts/27</code>，然后再通过 <code class="highlighter-rouge">ps -ef | grep pts/27 | grep bash | grep -v grep</code> 查看对应的 PID 。</p>

<p>打开另一个终端，通过 <code class="highlighter-rouge"><span class="contextHighlighter">pstack</span> PID</code> 即可看到对应的调用堆栈。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">main()                               ← 各种初始化工作，shell.c
 |-reader_loop()                     ← 死循环，通过read_command()读取命令，eval.c
   |-read_command()                  ← 通过parse_command()和yyparse()解析输入的命令，eval.c
   |-execute_command()               ← 开始执行命令，execute_cmd.c
     |-execute_command_internal()
       |-execute_simple_command()
         |-execute_disk_command()
           |-execve()                ← 通过系统调用执行</code></pre></figure>

<p>其中词法语法解析通过 <code class="highlighter-rouge">flex-biso</code> 解析，涉及的文件为 <code class="highlighter-rouge">parse.y</code>，没有找到词法解析的文件。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ rpm -qf `which bash`                      // 查看所属包
$ yumdownloader --source bash               // 下载源码
$ rpm2cpio bash-version.src.rpm | cpio -id  // 解压源码</code></pre></figure>

<p>通过 <code class="highlighter-rouge">strace ./hello</code> 查看系统调用，定位到 <code class="highlighter-rouge">execve()</code> ，也就是通过该函数执行。</p>

<h3 id="常见概念">常见概念</h3>

<p>解释器 <code class="highlighter-rouge">.interp</code> 分区用于指定程序动态装载、链接器 <code class="highlighter-rouge">ld-linux.so</code> 的位置，而过程链接表 <code class="highlighter-rouge">plt</code>、全局偏移表 <code class="highlighter-rouge">got</code>、重定位表则用于辅助动态链接过程。</p>

<h4 id="符号">符号</h4>

<p>对于可执行文件除了编译器引入的一些符号外，主要就是用户自定义的全局变量、函数等，而对于可重定位文件仅仅包含用户自定义的一些符号。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">----- 生成可重定位文件，并通过nm命令查看ELF文件的符号表信息
$ gcc -c main.c
$ nm main.o
0000000000000000 B global
0000000000000000 T main
                 U printf</code></pre></figure>

<p>上面包含全局变量、自定义函数以及动态链接库中的函数，但不包含局部变量，而且发现这三个符号的地址都没有确定。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">----- 生成可执行文件
$ gcc -o main main.o
$ nm main | egrep "main$| printf|global$"
0000000000601038 B global
000000000040052d T main
                 U printf@@GLIBC_2.2.5</code></pre></figure>

<p>经链接之后，<code class="highlighter-rouge">global</code> 和 <code class="highlighter-rouge">main</code> 的地址都已经确定了，但是 <code class="highlighter-rouge">printf</code> 却还没，因为它是动态链接库 <code class="highlighter-rouge">glibc</code> 中定义函数，需要动态链接，而不是这里的静态链接。</p>

<p>也就是说 main.o 中的符号地址没有确定，而经过链接后部分符号地址已经确定，也就是对符号的引用变成了对地址的引用，这样程序运行时就可通过访问内存地址而访问特定的数据。对于动态链接库，也就是上述的 <code class="highlighter-rouge">printf()</code> 则需要在运行时通过动态链接器 ld-linux.so 进行重定位，即动态链接。</p>

<p>另外，除了 nm 还可以用 <code class="highlighter-rouge">readelf -s</code> 查看 <code class="highlighter-rouge">.dynsym</code> 表或者用 <code class="highlighter-rouge">objdump -tT</code> 查看。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ nm -D /lib64/libc-2.17.so | grep "\ printf$"</code></pre></figure>

<p>注意，在部分新系统上，如果不使用参数 <code class="highlighter-rouge">-D</code> ，那么可能会无法查看符号表，因为 nm 默认打印 <code class="highlighter-rouge">.symtab</code> 和 <code class="highlighter-rouge">.strtab</code>，不过一般在打包时会通过 strip 删除掉，只保留了动态符号 (在 <code class="highlighter-rouge">.dynsym</code> 和 <code class="highlighter-rouge">.dynstr</code> 中)，以便动态链接器在执行程序时寻址这些外部用到的符号。</p>

<!-- https://stackoverflow.com/questions/9961473/nm-vs-readelf-s -->

<!--
## 动态链接

动态链接就是在程序运行时对符号进行重定位，确定符号对应的内存地址的过程。为了提高效率，Linux 下符号的动态链接默认采用 Lazy Mode 方式，也就是在程序运行过程中用到该符号时才去解析它的地址。

不过这种默认是可以通过设置 LD_BIND_NOW 为非空来打破的（下面会通过实例来分析这个变量的作用），也就是说如果设置了这个变量，动态链接器将在程序加载后和符号被使用之前就对这些符号的地址进行解析。

## 动态链接库

在程序中，保存了依赖的库信息。

$ readelf -d main | grep NEEDED
 0x0000000000000001 (NEEDED)             Shared library: [libc.so.6]

在 `.dynamic` 分区中保存了和动态链接相关的信息，当然只有版本信息，而没有保存绝对路径，其搜索路径如上所述。

### FIXME: 获取动态链接器

动态链接器保存在 `.interp` 分区，可以通过 `readelf -x .interp main` 命令查看。


注意，与 libc.so 之类的动态库不同，ld-linux.so 的路径是绝对路径，而类似于 libc.so 库则只包含了文件名。

这是因为，程序执行时 ld-linux.so 最先被加载到内存，没有其他程序知道去哪里查找 ld-linux.so，所以它的路径必须是绝对的；当 ld-linux.so 被装载以后，由它来去装载可执行文件和相关的共享库，它会根据下面介绍的流程去加载。

### 动态链接器

可以通过 `man ld-linux` 获取与动态链接器相关的资料，包括各种相关的环境变量和文件都有详细的说明。

对于环境变量，除了上面提到过的 LD_LIBRARY_PATH 和 LD_BIND_NOW 变量外，还有其他几个重要参数，比如 LD_PRELOAD 用于指定预装载一些库，以便替换其他库中的函数，从而做一些安全方面的处理 [6]，[9]，[12]，而环境变量 LD_DEBUG 可以用来进行动态链接的相关调试。
对于文件，除了上面提到的 ld.so.conf 和 ld.so.cache 外，还有一个文件 /etc/ld.so.preload 用于指定需要预装载的库。

实际上，ELF 格式可以从两个角度去看，包括链接和执行，分别通过 `Section Header Table` 和 `Program Header Table` 表示。

SHT 保存了 ELF 所包含的段信息，可以通过 `readelf -S /bin/bash` 查看，其中比较重要的有 REL sections (relocations), SYMTAB/DYNSYM (symbol tables), VERSYM/VERDEF/VERNEED sections (symbol versioning information).

#### 1. 加载到内存

在 ELF 文件的文件头中就指定了该文件的入口地址，程序的代码和数据部分会相继映射到对应的内存中。

$ readelf -h /bin/bash | grep Entry
  Entry point address:               0x41d361
-->

<h2 id="内核加载">内核加载</h2>

<p>ELF 有静态和动态链接两种方式，加载过程由内核开始，而动态链接库的加载则可以在用户层完成。GNU 对于动态链接过程为 A) 把 ELF 映像的装入/启动加载在 Linux 内核中；B) 把动态链接的实现放在用户空间 (glibc)，并为此提供一个称为 “解释器” (ld-linux.so.2) 工具。</p>

<p>注意，解释器的装入/启动也由内核负责，详细可以查看 <a href="/post/kernel-memory-management-from-userspace-view.html">内存-用户空间</a> 中的介绍，在此只介绍 ELF 的加载过程。</p>

<h3 id="内核模块">内核模块</h3>

<p>如果要支持不同的执行格式，需要在内核中添加注册模块，每种类型通过 <code class="highlighter-rouge">struct linux_binfmt</code> 格式表示，其定义以及 ELF 的定义如下所示：</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="k">struct</span> <span class="n">linux_binfmt</span> <span class="p">{</span>
    <span class="k">struct</span> <span class="n">list_head</span> <span class="n">lh</span><span class="p">;</span>
    <span class="k">struct</span> <span class="n">module</span> <span class="o">*</span><span class="n">module</span><span class="p">;</span>
    <span class="kt">int</span> <span class="p">(</span><span class="o">*</span><span class="n">load_binary</span><span class="p">)(</span><span class="k">struct</span> <span class="n">linux_binprm</span> <span class="o">*</span><span class="p">);</span>
    <span class="kt">int</span> <span class="p">(</span><span class="o">*</span><span class="n">load_shlib</span><span class="p">)(</span><span class="k">struct</span> <span class="n">file</span> <span class="o">*</span><span class="p">);</span>
    <span class="kt">int</span> <span class="p">(</span><span class="o">*</span><span class="n">core_dump</span><span class="p">)(</span><span class="k">struct</span> <span class="n">coredump_params</span> <span class="o">*</span><span class="n">cprm</span><span class="p">);</span>
    <span class="kt">unsigned</span> <span class="kt">long</span> <span class="n">min_coredump</span><span class="p">;</span> <span class="cm">/* minimal dump size */</span>
<span class="p">};</span>

<span class="k">static</span> <span class="k">struct</span> <span class="n">linux_binfmt</span> <span class="n">elf_format</span> <span class="o">=</span> <span class="p">{</span>
    <span class="p">.</span><span class="n">module</span>        <span class="o">=</span> <span class="n">THIS_MODULE</span><span class="p">,</span>
    <span class="p">.</span><span class="n">load_binary</span>   <span class="o">=</span> <span class="n">load_elf_binary</span><span class="p">,</span>
    <span class="p">.</span><span class="n">load_shlib</span>    <span class="o">=</span> <span class="n">load_elf_library</span><span class="p">,</span>
    <span class="p">.</span><span class="n">core_dump</span>     <span class="o">=</span> <span class="n">elf_core_dump</span><span class="p">,</span>
    <span class="p">.</span><span class="n">min_coredump</span>  <span class="o">=</span> <span class="n">ELF_EXEC_PAGESIZE</span><span class="p">,</span>
<span class="p">};</span></code></pre></figure>

<p>其中的 <code class="highlighter-rouge">load_binary</code> 函数指针指向的就是一个可执行程序的处理函数，要支持 ELF 文件的运行，则必须通过 <code class="highlighter-rouge">register_binfmt()</code> 向内核登记这个数据结构，加入到内核支持的可执行程序的队列中。</p>

<p>当要运行程序时，则扫描该队列，让各对象所提供的处理程序 (ELF中即为<code class="highlighter-rouge">load_elf_binary()</code>)，逐一前来认领，如果某个格式的处理程序发现相符后，便执行该格式映像的装入和启动。</p>

<h3 id="内核加载-1">内核加载</h3>

<p>内核执行 <code class="highlighter-rouge">execv()</code> 或 <code class="highlighter-rouge">execve()</code> 系统调用时，会通过 <code class="highlighter-rouge">do_execve()</code> 调用，该函数先打开目标映像文件，并读入文件的头部信息，也就是开始 128 字节。</p>

<p>然后，调用另一个 <code class="highlighter-rouge">search_binary_handler()</code> 函数，该函数中会搜索上面提到的 Linux 支持的可执行文件类型队列，让各种可执行程序的处理程序前来认领和处理。</p>

<p>如果类型匹配，则调用 <code class="highlighter-rouge">load_binary</code> 函数指针所指向的处理函数来处理目标映像文件，对于 ELF 文件也就是 <code class="highlighter-rouge">load_elf_binary()</code> 函数，下面主要就是分析 <code class="highlighter-rouge">load_elf_binary()</code> 的执行过程。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="k">static</span> <span class="kt">int</span> <span class="nf">load_elf_binary</span><span class="p">(</span><span class="k">struct</span> <span class="n">linux_binprm</span> <span class="o">*</span><span class="n">bprm</span><span class="p">)</span>
<span class="p">{</span>
    <span class="cm">/* 上述已经读取 128 字节的头部信息 */</span>
    <span class="n">loc</span><span class="o">-&gt;</span><span class="n">elf_ex</span> <span class="o">=</span> <span class="o">*</span><span class="p">((</span><span class="k">struct</span> <span class="n">elfhdr</span> <span class="o">*</span><span class="p">)</span><span class="n">bprm</span><span class="o">-&gt;</span><span class="n">buf</span><span class="p">);</span>

    <span class="n">retval</span> <span class="o">=</span> <span class="o">-</span><span class="n">ENOEXEC</span><span class="p">;</span>
    <span class="cm">/* 首先校验ELF的头部信息，也就是"\177ELF" */</span>
    <span class="k">if</span> <span class="p">(</span><span class="n">memcmp</span><span class="p">(</span><span class="n">loc</span><span class="o">-&gt;</span><span class="n">elf_ex</span><span class="p">.</span><span class="n">e_ident</span><span class="p">,</span> <span class="n">ELFMAG</span><span class="p">,</span> <span class="n">SELFMAG</span><span class="p">)</span> <span class="o">!=</span> <span class="mi">0</span><span class="p">)</span>
        <span class="k">goto</span> <span class="n">out</span><span class="p">;</span>
    <span class="cm">/* 只能是可执行文件(ET_EXEC)或者动态库(ET_DYN) */</span>
    <span class="k">if</span> <span class="p">(</span><span class="n">loc</span><span class="o">-&gt;</span><span class="n">elf_ex</span><span class="p">.</span><span class="n">e_type</span> <span class="o">!=</span> <span class="n">ET_EXEC</span> <span class="o">&amp;&amp;</span> <span class="n">loc</span><span class="o">-&gt;</span><span class="n">elf_ex</span><span class="p">.</span><span class="n">e_type</span> <span class="o">!=</span> <span class="n">ET_DYN</span><span class="p">)</span>
        <span class="k">goto</span> <span class="n">out</span><span class="p">;</span>

    <span class="cm">/* 读取ELF的头部信息，也就是整个Program Header Table，一个可执行程序必须至少有一个段，且不能超过64K */</span>
    <span class="n">retval</span> <span class="o">=</span> <span class="n">kernel_read</span><span class="p">(</span><span class="n">bprm</span><span class="o">-&gt;</span><span class="n">file</span><span class="p">,</span> <span class="n">loc</span><span class="o">-&gt;</span><span class="n">elf_ex</span><span class="p">.</span><span class="n">e_phoff</span><span class="p">,</span> <span class="p">(</span><span class="kt">char</span> <span class="o">*</span><span class="p">)</span><span class="n">elf_phdata</span><span class="p">,</span> <span class="n">size</span><span class="p">);</span>
    <span class="k">if</span> <span class="p">(</span><span class="n">retval</span> <span class="o">!=</span> <span class="n">size</span><span class="p">)</span> <span class="p">{</span>
        <span class="k">if</span> <span class="p">(</span><span class="n">retval</span> <span class="o">&gt;=</span> <span class="mi">0</span><span class="p">)</span>
            <span class="n">retval</span> <span class="o">=</span> <span class="o">-</span><span class="n">EIO</span><span class="p">;</span>
        <span class="k">goto</span> <span class="n">out_free_ph</span><span class="p">;</span>
    <span class="p">}</span>

    <span class="cm">/* 在for循环中，用来寻找和处理目标映像的"解释器"段，可以通过 ???????? 查看 */</span>
    <span class="k">for</span> <span class="p">(</span><span class="n">i</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span> <span class="n">i</span> <span class="o">&lt;</span> <span class="n">loc</span><span class="o">-&gt;</span><span class="n">elf_ex</span><span class="p">.</span><span class="n">e_phnum</span><span class="p">;</span> <span class="n">i</span><span class="o">++</span><span class="p">)</span> <span class="p">{</span>
        <span class="k">if</span> <span class="p">(</span><span class="n">elf_ppnt</span><span class="o">-&gt;</span><span class="n">p_type</span> <span class="o">==</span> <span class="n">PT_INTERP</span><span class="p">)</span> <span class="p">{</span>
            <span class="cm">/* 找到后就根据其位置的p_offset和大小p_filesz把整个"解释器"段的内容读入缓冲区，实际上
             * 这个"解释器"段实际上只是一个字符串，即解释器的文件名，如"/lib/ld-linux.so.2"
             */</span>
            <span class="n">retval</span> <span class="o">=</span> <span class="n">kernel_read</span><span class="p">(</span><span class="n">bprm</span><span class="o">-&gt;</span><span class="n">file</span><span class="p">,</span> <span class="n">elf_ppnt</span><span class="o">-&gt;</span><span class="n">p_offset</span><span class="p">,</span>
                         <span class="n">elf_interpreter</span><span class="p">,</span> <span class="n">elf_ppnt</span><span class="o">-&gt;</span><span class="n">p_filesz</span><span class="p">);</span>
            <span class="k">if</span> <span class="p">(</span><span class="n">retval</span> <span class="o">!=</span> <span class="n">elf_ppnt</span><span class="o">-&gt;</span><span class="n">p_filesz</span><span class="p">)</span> <span class="p">{</span>
                <span class="k">if</span> <span class="p">(</span><span class="n">retval</span> <span class="o">&gt;=</span> <span class="mi">0</span><span class="p">)</span>
                    <span class="n">retval</span> <span class="o">=</span> <span class="o">-</span><span class="n">EIO</span><span class="p">;</span>
                <span class="k">goto</span> <span class="n">out_free_interp</span><span class="p">;</span>
            <span class="p">}</span>

            <span class="cm">/* 通过open_exec()打开这个文件，再通过kernel_read()读入开始的128个字节，也就是解释器的头部 */</span>
            <span class="n">interpreter</span> <span class="o">=</span> <span class="n">open_exec</span><span class="p">(</span><span class="n">elf_interpreter</span><span class="p">);</span>
            <span class="n">retval</span> <span class="o">=</span> <span class="n">PTR_ERR</span><span class="p">(</span><span class="n">interpreter</span><span class="p">);</span>
            <span class="k">if</span> <span class="p">(</span><span class="n">IS_ERR</span><span class="p">(</span><span class="n">interpreter</span><span class="p">))</span>
                <span class="k">goto</span> <span class="n">out_free_interp</span><span class="p">;</span>
            <span class="n">retval</span> <span class="o">=</span> <span class="n">kernel_read</span><span class="p">(</span><span class="n">interpreter</span><span class="p">,</span> <span class="mi">0</span><span class="p">,</span> <span class="n">bprm</span><span class="o">-&gt;</span><span class="n">buf</span><span class="p">,</span> <span class="n">BINPRM_BUF_SIZE</span><span class="p">);</span>
            <span class="k">if</span> <span class="p">(</span><span class="n">retval</span> <span class="o">!=</span> <span class="n">BINPRM_BUF_SIZE</span><span class="p">)</span> <span class="p">{</span>
                <span class="k">if</span> <span class="p">(</span><span class="n">retval</span> <span class="o">&gt;=</span> <span class="mi">0</span><span class="p">)</span>
                    <span class="n">retval</span> <span class="o">=</span> <span class="o">-</span><span class="n">EIO</span><span class="p">;</span>
                <span class="k">goto</span> <span class="n">out_free_dentry</span><span class="p">;</span>
            <span class="p">}</span>
            <span class="k">break</span><span class="p">;</span>
        <span class="p">}</span>
        <span class="n">elf_ppnt</span><span class="o">++</span><span class="p">;</span>
    <span class="p">}</span>

    <span class="cm">/* 从目标映像的程序头中搜索类型为PT_LOAD的段；在二进制映像中，只有类型为PT_LOAD的段才是需要加载的。
     * 只是在加载前，需要确定加载的地址，包括页对齐、该段的p_vaddr域的值；在确定了装入地址后，就会通过
     * elf_map()建立用户空间虚拟地址空间与目标映像文件中某个连续区间之间的映射，其返回值就是实际映射的
     * 起始地址。
     */</span>
    <span class="k">for</span><span class="p">(</span><span class="n">i</span> <span class="o">=</span> <span class="mi">0</span><span class="p">,</span> <span class="n">elf_ppnt</span> <span class="o">=</span> <span class="n">elf_phdata</span><span class="p">;</span> <span class="n">i</span> <span class="o">&lt;</span> <span class="n">loc</span><span class="o">-&gt;</span><span class="n">elf_ex</span><span class="p">.</span><span class="n">e_phnum</span><span class="p">;</span> <span class="n">i</span><span class="o">++</span><span class="p">,</span> <span class="n">elf_ppnt</span><span class="o">++</span><span class="p">)</span> <span class="p">{</span>
        <span class="k">if</span> <span class="p">(</span><span class="n">elf_ppnt</span><span class="o">-&gt;</span><span class="n">p_type</span> <span class="o">!=</span> <span class="n">PT_LOAD</span><span class="p">)</span>
            <span class="k">continue</span><span class="p">;</span>

        <span class="n">error</span> <span class="o">=</span> <span class="n">elf_map</span><span class="p">(</span><span class="n">bprm</span><span class="o">-&gt;</span><span class="n">file</span><span class="p">,</span> <span class="n">load_bias</span> <span class="o">+</span> <span class="n">vaddr</span><span class="p">,</span> <span class="n">elf_ppnt</span><span class="p">,</span> <span class="n">elf_prot</span><span class="p">,</span> <span class="n">elf_flags</span><span class="p">,</span> <span class="n">total_size</span><span class="p">);</span>
        <span class="k">if</span> <span class="p">(</span><span class="n">BAD_ADDR</span><span class="p">(</span><span class="n">error</span><span class="p">))</span> <span class="p">{</span>
            <span class="n">retval</span> <span class="o">=</span> <span class="n">IS_ERR</span><span class="p">((</span><span class="kt">void</span> <span class="o">*</span><span class="p">)</span><span class="n">error</span><span class="p">)</span> <span class="o">?</span>
                <span class="n">PTR_ERR</span><span class="p">((</span><span class="kt">void</span><span class="o">*</span><span class="p">)</span><span class="n">error</span><span class="p">)</span> <span class="o">:</span> <span class="o">-</span><span class="n">EINVAL</span><span class="p">;</span>
            <span class="k">goto</span> <span class="n">out_free_dentry</span><span class="p">;</span>
        <span class="p">}</span>
    <span class="p">}</span>

    <span class="cm">/* 若存在PT_INTERP段，则通过load_elf_interp()加载，并把进入用户空间的入口地址设置成load_elf_interp()
     * 的返回值，即解释器映像的入口地址；如果不装入解释器，那么这个入口地址就是目标映像本身的入口地址，
     * 也就是静态编译的程序。
     */</span>
    <span class="k">if</span> <span class="p">(</span><span class="n">elf_interpreter</span><span class="p">)</span> <span class="p">{</span>
        <span class="n">elf_entry</span> <span class="o">=</span> <span class="n">load_elf_interp</span><span class="p">(</span><span class="o">&amp;</span><span class="n">loc</span><span class="o">-&gt;</span><span class="n">interp_elf_ex</span><span class="p">,</span> <span class="n">interpreter</span><span class="p">,</span> <span class="o">&amp;</span><span class="n">interp_map_addr</span><span class="p">,</span> <span class="n">load_bias</span><span class="p">);</span>
        <span class="k">if</span> <span class="p">(</span><span class="o">!</span><span class="n">IS_ERR</span><span class="p">((</span><span class="kt">void</span> <span class="o">*</span><span class="p">)</span><span class="n">elf_entry</span><span class="p">))</span> <span class="p">{</span>
            <span class="n">interp_load_addr</span> <span class="o">=</span> <span class="n">elf_entry</span><span class="p">;</span>
            <span class="n">elf_entry</span> <span class="o">+=</span> <span class="n">loc</span><span class="o">-&gt;</span><span class="n">interp_elf_ex</span><span class="p">.</span><span class="n">e_entry</span><span class="p">;</span>
        <span class="p">}</span>
        <span class="k">if</span> <span class="p">(</span><span class="n">BAD_ADDR</span><span class="p">(</span><span class="n">elf_entry</span><span class="p">))</span> <span class="p">{</span>
            <span class="n">retval</span> <span class="o">=</span> <span class="n">IS_ERR</span><span class="p">((</span><span class="kt">void</span> <span class="o">*</span><span class="p">)</span><span class="n">elf_entry</span><span class="p">)</span> <span class="o">?</span>  <span class="p">(</span><span class="kt">int</span><span class="p">)</span><span class="n">elf_entry</span> <span class="o">:</span> <span class="o">-</span><span class="n">EINVAL</span><span class="p">;</span>
            <span class="k">goto</span> <span class="n">out_free_dentry</span><span class="p">;</span>
        <span class="p">}</span>
        <span class="n">reloc_func_desc</span> <span class="o">=</span> <span class="n">interp_load_addr</span><span class="p">;</span>
    <span class="p">}</span> <span class="k">else</span> <span class="p">{</span>
        <span class="n">elf_entry</span> <span class="o">=</span> <span class="n">loc</span><span class="o">-&gt;</span><span class="n">elf_ex</span><span class="p">.</span><span class="n">e_entry</span><span class="p">;</span>
    <span class="p">}</span>

    <span class="cm">/* 完成加载且启动用户空间的映像运行之前，还需要为目标映像和解释器准备好一些有关的信息，这些信息包括常
     * 规的argc、envc等等，还有一些"辅助向量(Auxiliary Vector)"。这些信息需要复制到用户空间，使它们在CPU
     * 进入解释器或目标映像的程序入口时出现在用户空间堆栈上。这里的create_elf_tables()就起着这个作用。
     */</span>
    <span class="n">retval</span> <span class="o">=</span> <span class="n">create_elf_tables</span><span class="p">(</span><span class="n">bprm</span><span class="p">,</span> <span class="o">&amp;</span><span class="n">loc</span><span class="o">-&gt;</span><span class="n">elf_ex</span><span class="p">,</span> <span class="n">load_addr</span><span class="p">,</span> <span class="n">interp_load_addr</span><span class="p">);</span>
    <span class="k">if</span> <span class="p">(</span><span class="n">retval</span> <span class="o">&lt;</span> <span class="mi">0</span><span class="p">)</span>
        <span class="k">goto</span> <span class="n">out</span><span class="p">;</span>

    <span class="cm">/* 最后，通过start_thread()宏将eip和esp改成新的地址，就使得CPU在返回用户空间时就进入新的程序入口。如果存
     * 在解释器映像，那么这就是解释器映像的程序入口(动态链接)，否则就是目标映像的程序入口(静态链接)。
     */</span>
    <span class="n">start_thread</span><span class="p">(</span><span class="n">regs</span><span class="p">,</span> <span class="n">elf_entry</span><span class="p">,</span> <span class="n">bprm</span><span class="o">-&gt;</span><span class="n">p</span><span class="p">);</span>
<span class="p">}</span></code></pre></figure>

<h2 id="加载过程">加载过程</h2>

<p>依赖动态库时，会在加载时根据可执行文件的地址和动态库的对应符号的地址推算出被调用函数的地址，这个过程被称为动态链接。</p>

<p>假设，现在使用的是 Position Independent Code, PIC 模型。</p>

<h4 id="1-获取动态链接器">1. 获取动态链接器</h4>

<p>首先，读取 ELF 头部信息，解析出 <code class="highlighter-rouge">PT_INTERP</code> 信息，确定动态链接器的路径，可以通过 <code class="highlighter-rouge">readelf -l foobar</code> 查看，一般是 <code class="highlighter-rouge">/lib/ld-linux.so.2</code> 或者 <code class="highlighter-rouge">/lib64/ld-linux-x86-64.so.2</code> 。</p>

<h4 id="2-加载动态库">2. 加载动态库</h4>

<p>关于加载的详细顺序可以查看 <code class="highlighter-rouge">man ld</code> 中 rpath-link 的介绍，一般顺序为：</p>

<ol>
  <li>链接时 <code class="highlighter-rouge">-rpath-link</code> 参数指定路径，只用于链接时使用，编译时通过 <code class="highlighter-rouge">-Wl,rpath-link=</code> 指定；</li>
  <li>链接时通过 <code class="highlighter-rouge">-rpath</code> 参数指定路径，除了用于链接时使用，还会在运行时使用，编译时可利用 <code class="highlighter-rouge">-Wl,rpath=</code> 指定，会生成 <code class="highlighter-rouge">DT_RPATH</code> 或者 <code class="highlighter-rouge">DT_RUNPATH</code> 定义，可以通过 <code class="highlighter-rouge">readelf -d main | grep -E (RPATH|RUNPATH)</code> 查看；</li>
  <li>查找 <code class="highlighter-rouge">DT_RUNPATH</code> 或者 <code class="highlighter-rouge">DT_RPATH</code> 指定的路径，如果前者存在则忽略后者；</li>
  <li>依次查看 <code class="highlighter-rouge">LD_RUN_PATH</code> 和 <code class="highlighter-rouge">LD_LIBRARY_PATH</code> 环境变量指定路径；</li>
  <li>查找默认路径，一般是 <code class="highlighter-rouge">/lib</code> 和 <code class="highlighter-rouge">/usr/lib</code> ，然后是 <code class="highlighter-rouge">/etc/ld.so.conf</code> 文件中的配置。</li>
</ol>

<p>另外，需要加载哪些库通过 <code class="highlighter-rouge">DT_NEEDED</code> 字段来获取，每条对应了一个动态库，可以通过 <code class="highlighter-rouge">readelf -d main | grep NEEDED</code> 查看。</p>

<h3 id="示例程序">示例程序</h3>

<p>利用如下的示例程序。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cm">/* filename: foobar.c */</span>
<span class="cp">#define _GNU_SOURCE
#include &lt;stdio.h&gt;
#include &lt;dlfcn.h&gt;
</span>
<span class="kt">int</span> <span class="nf">foobar</span><span class="p">(</span><span class="kt">void</span><span class="p">)</span>
<span class="p">{</span>
    <span class="n">Dl_info</span> <span class="n">dl_info</span><span class="p">;</span>
    <span class="n">dladdr</span><span class="p">((</span><span class="kt">void</span><span class="o">*</span><span class="p">)</span><span class="n">foobar</span><span class="p">,</span> <span class="o">&amp;</span><span class="n">dl_info</span><span class="p">);</span>
    <span class="n">fprintf</span><span class="p">(</span><span class="n">stdout</span><span class="p">,</span> <span class="s">"load .so at: %s</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span> <span class="n">dl_info</span><span class="p">.</span><span class="n">dli_fname</span><span class="p">);</span>
    <span class="k">return</span> <span class="mi">0</span><span class="p">;</span>
<span class="p">}</span></code></pre></figure>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cm">/* filename: main.c */</span>
<span class="kt">int</span> <span class="n">foobar</span><span class="p">(</span><span class="kt">void</span><span class="p">);</span>
<span class="kt">int</span> <span class="nf">main</span><span class="p">(</span><span class="kt">int</span> <span class="n">argc</span><span class="p">,</span> <span class="kt">char</span> <span class="o">**</span><span class="n">argv</span><span class="p">)</span>
<span class="p">{</span>
  <span class="n">foobar</span><span class="p">();</span>
  <span class="k">return</span> <span class="mi">0</span><span class="p">;</span>
<span class="p">}</span></code></pre></figure>

<figure class="highlight"><pre><code class="language-makefile" data-lang="makefile"><span class="c"># filename: Makefile
</span><span class="nl">all</span><span class="o">:</span>
    <span class="err">gcc</span> <span class="err">--shared</span> <span class="err">-fPIC</span> <span class="err">foobar.c</span> <span class="err">-o</span> <span class="err">libfoobar.so</span> <span class="err">-ldl</span>
    <span class="err">gcc</span> <span class="err">main.c</span> <span class="err">-o</span> <span class="err">main</span> <span class="err">-Wl,</span><span class="nv">-rpath-link</span><span class="o">=</span>/foobar <span class="nt">-ldl</span> <span class="nt">-lfoobar</span> <span class="nt">-L</span>./
    <span class="err">readelf</span> <span class="err">-d</span> <span class="err">main</span> <span class="err">|</span> <span class="err">grep</span> <span class="err">-E</span> <span class="err">(RPATH|RUNPATH)</span></code></pre></figure>

<p>然后可以通过依次设置如上的加载路径进行测试。<strong>注意</strong>，在对 <code class="highlighter-rouge">/etc/ld.so.conf</code> 文件设置后需要通过 <code class="highlighter-rouge">ldconfig</code> 更新 cache 才会生效。</p>

<p>另外，推荐使用 <code class="highlighter-rouge">DT_RUNPATH</code> 而非 <code class="highlighter-rouge">DT_RPATH</code> ，此时，在编译时需要用到 <code class="highlighter-rouge">--enable-new-dtags</code> 参数。</p>

<!--
通过C语言直接读取rpath参数
#include <stdio.h>
#include <elf.h>
#include <link.h>

int main()
{
  const ElfW(Dyn) *dyn = _DYNAMIC;
  const ElfW(Dyn) *rpath = NULL;
  const char *strtab = NULL;
  for (; dyn->d_tag != DT_NULL; ++dyn) {
    if (dyn->d_tag == DT_RPATH) {
      rpath = dyn;
    } else if (dyn->d_tag == DT_STRTAB) {
      strtab = (const char *)dyn->d_un.d_val;
    }
  }

  if (strtab != NULL && rpath != NULL) {
    printf("RPATH: %s\n", strtab + rpath->d_un.d_val);
  }
  return 0;
}
-->

<h3 id="版本管理">版本管理</h3>

<p>不同版本的动态库可能会不兼容，那么如果程序在编译时指定动态库是某个低版本，运行是用的一个高版本，可能会导致无法运行。</p>

<p>假设有如下的示例：</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cm">/* filename:hello.c */</span>
<span class="cp">#include &lt;stdio.h&gt;
</span><span class="kt">void</span> <span class="nf">hello</span><span class="p">(</span><span class="k">const</span> <span class="kt">char</span><span class="o">*</span> <span class="n">name</span><span class="p">)</span>
<span class="p">{</span>
    <span class="n">printf</span><span class="p">(</span><span class="s">"hello %s!</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span> <span class="n">name</span><span class="p">);</span>
<span class="p">}</span></code></pre></figure>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cm">/* filename:hello.h */</span>
<span class="kt">void</span> <span class="n">hello</span><span class="p">(</span><span class="k">const</span> <span class="kt">char</span><span class="o">*</span> <span class="n">name</span><span class="p">);</span></code></pre></figure>

<figure class="highlight"><pre><code class="language-makefile" data-lang="makefile"><span class="c"># filename: Makefile
</span><span class="nl">all</span><span class="o">:</span>
    <span class="err">gcc</span> <span class="err">hello.c</span> <span class="err">-fPIC</span> <span class="err">-shared</span> <span class="err">-Wl,-soname,libhello.so.0</span> <span class="err">-o</span> <span class="err">libhello.so.0.0.1</span></code></pre></figure>

<p>需要注意是，参数 <code class="highlighter-rouge">-Wl,soname</code> 中间没有空格，<code class="highlighter-rouge">-Wl</code> 选项用来告诉编译器将后面的参数传递给链接器，而 <code class="highlighter-rouge">-soname</code> 则指定了动态库的 <code class="highlighter-rouge">soname</code>。运行后在当前目录下会生成一个 <code class="highlighter-rouge">libhello.so.0.0.1</code> 文件，当运行 <code class="highlighter-rouge">ldconfig -n .</code> 命令时，当前目录会多一个符号连接。</p>

<p>这个软链接是根据编译生成 <code class="highlighter-rouge">libhello.so.0.0.1</code> 时指定的 <code class="highlighter-rouge">-soname</code> 生成的，会保存到编译生成的文件中，可以通过 <code class="highlighter-rouge">readelf -d foobar</code> 查看依赖的库。</p>

<p>所以关键就是这个 soname，它相当于一个中间者，当我们的动态库只是升级一个小版本时，可以让它的 soname 相同，而可执行程序只认 soname 指定的动态库，这样依赖这个动态库的可执行程序不需重新编译就能使用新版动态库的特性。</p>

<h4 id="测试程序">测试程序</h4>

<p>示例程序如下。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cm">/* filename:main.c */</span>
<span class="cp">#include "hello.h"
</span><span class="kt">int</span> <span class="nf">main</span><span class="p">()</span>
<span class="p">{</span>
    <span class="n">hello</span><span class="p">(</span><span class="s">"foobar"</span><span class="p">);</span>
    <span class="k">return</span> <span class="mi">0</span><span class="p">;</span>
<span class="p">}</span></code></pre></figure>

<p>然后可以通过 <code class="highlighter-rouge">gcc main.c -L. -lhello -o main</code> 编译，不过此时会报 <code class="highlighter-rouge">cannot find -lhello.so.0</code> 错误，也就是找不到对应的库。</p>

<p>在 Linux 中，编译时指定 <code class="highlighter-rouge">-lhello</code> 时，链接器会去查找 <code class="highlighter-rouge">libhello.so</code> 这样的文件，如果当前目录下没有这个文件，那么就会导致报错；此时，可以通过 <code class="highlighter-rouge">ln -s libhello.so.0.0.1 libhello.so</code> 建立这样一个软链接。</p>

<p>通过 <code class="highlighter-rouge">ldd</code> 查看时，发现实际依赖的是 <code class="highlighter-rouge">libhello.so.0</code> 而非 <code class="highlighter-rouge">libhello</code> 也不是 <code class="highlighter-rouge">libhello.so.0.0.1</code> ，其实在生成 main 程序的过程有如下几步：</p>

<ol>
  <li>链接器通过编译命令 <code class="highlighter-rouge">-L. -lhello</code> 在当前目录查找 <code class="highlighter-rouge">libhello.so</code> 文件；</li>
  <li>读取 <code class="highlighter-rouge">libhello.so</code> 链接指向的实际文件，这里是 <code class="highlighter-rouge">libhello.so.0.0.1</code>；</li>
  <li>读取 <code class="highlighter-rouge">libhello.so.0.0.1</code> 中的 <code class="highlighter-rouge">SONAME</code>，这里是 <code class="highlighter-rouge">libhello.so.0</code>；</li>
  <li>将 <code class="highlighter-rouge">libhello.so.0</code> 记录到 <code class="highlighter-rouge">main</code> 程序的二进制数据里。</li>
</ol>

<p>也就是说 <code class="highlighter-rouge">libhello.so.0</code> 是已经存储到 main 程序的二进制数据里的，不管这个程序在哪里，通过 <code class="highlighter-rouge">ldd</code> 查看它依赖的动态库都是 <code class="highlighter-rouge">libhello.so.0</code> 。</p>

<p>那么，在部署时，只需要安装 <code class="highlighter-rouge">libhello.so.0</code> 即可。</p>

<h4 id="版本更新">版本更新</h4>

<p>假设动态库需要做一个小小的改动。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cm">/* filename:hello.c */</span>
<span class="cp">#include &lt;stdio.h&gt;
</span><span class="kt">void</span> <span class="nf">hello</span><span class="p">(</span><span class="k">const</span> <span class="kt">char</span><span class="o">*</span> <span class="n">name</span><span class="p">)</span>
<span class="p">{</span>
    <span class="n">printf</span><span class="p">(</span><span class="s">"hello %s, welcom to our world!</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span> <span class="n">name</span><span class="p">);</span>
<span class="p">}</span></code></pre></figure>

<p>由于改动较小，编译动态库时仍然指定相同的 soname 。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ gcc hello.c -fPIC -shared -Wl,-soname,libhello.so.0 -o libhello.so.0.0.2</code></pre></figure>

<p>然后重新运行 <code class="highlighter-rouge">ldconfig -n .</code> 即可，会发现链接指向了新版本，然后直接运行即可。</p>

<p>同样，假如我们的动态库有大的改动，编译动态库时指定了新的 soname，如下：</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ gcc hello.c -fPIC -shared -Wl,-soname,libhello.so.1 -o libhello.so.1.0.0</code></pre></figure>

<p>将动态库文件拷贝到运行目录，并执行 <code class="highlighter-rouge">ldconfig -n .</code>，不过此时需要重新编译才可以。</p>

<h2 id="动态解析">动态解析</h2>

<p>如上所述，控制权先是提交到解释器，由解释器加载动态库，然后控制权才会到用户程序。动态库加载的大致过程就是将每一个依赖的动态库都加载到内存，并形成一个链表，后面的符号解析过程主要就是在这个链表中搜索符号的定义。</p>

<!--
对于静态文件通常只有一个文件要被映射，而动态则还有所依赖的共享目标文件，通过 /proc/PID/maps 可以查看在内存中的分布。
地址随机实际上包括了动态链接库、堆、栈，而对于程序本身的函数，其地址是固定的。
-->

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ cat test.c
#include &lt;stdio.h&gt;
void foobar (void)
{
    puts("Hello World");
}

int main(void)
{
    foobar();
    return 0;
}

----- 编译连接
$ gcc test.c -o test -g
----- 打印程序的反汇编
$ objdump -S test

----- 使用gdb调式
$ gdb test -q
(gdb) break main
(gdb) run
(gdb) disassemble
Dump of assembler code for function main:
   0x000000000040053d &lt;+0&gt;:     push   %rbp
   0x000000000040053e &lt;+1&gt;:     mov    %rsp,%rbp
=&gt; 0x0000000000400541 &lt;+4&gt;:     callq  0x40052d &lt;foobar&gt;    此处调用的地址是固定的
   0x0000000000400546 &lt;+9&gt;:     mov    $0x0,%eax
   0x000000000040054b &lt;+14&gt;:    pop    %rbp
   0x000000000040054c &lt;+15&gt;:    retq   
End of assembler dump.
(gdb) disassemble foobar
Dump of assembler code for function foobar:
   0x000000000040052d &lt;+0&gt;:     push   %rbp
   0x000000000040052e &lt;+1&gt;:     mov    %rsp,%rbp
   0x0000000000400531 &lt;+4&gt;:     mov    $0x4005e0,%edi
   0x0000000000400536 &lt;+9&gt;:     callq  0x400410 &lt;puts@plt&gt;  反汇编
   0x000000000040053b &lt;+14&gt;:    pop    %rbp
   0x000000000040053c &lt;+15&gt;:    retq   
End of assembler dump.</code></pre></figure>

<p>从上面反汇编代码可以看出，在调用 <code class="highlighter-rouge">foobar()</code> 时，使用的是绝对地址，<code class="highlighter-rouge">printf()</code> 的调用已经换成了 <code class="highlighter-rouge">puts()</code> ，调用的是 <code class="highlighter-rouge">puts@plt</code> 这个标号，位于 <code class="highlighter-rouge">0x400410</code>，实际上这是一个 PLT 条目，可以通过反汇编查看相应的代码，不过它代表什么意思呢？</p>

<p>在进一步说明符号的动态解析过程以前，需要先了解两个概念，一个是 <code class="highlighter-rouge">Global Offset Table</code>，一个是 <code class="highlighter-rouge">Procedure Linkage Table</code> 。</p>

<h4 id="global-offset-table-got">Global Offset Table, GOT</h4>

<p>在位置无关代码中，如共享库，一般不会包含绝对虚拟地址，而是在程序中引用某个共享库中的符号时，编译链接阶段并不知道这个符号的具体位置，只有等到动态链接器将所需要的共享库加载时进内存后，也就是在运行阶段，符号的地址才会最终确定。</p>

<p>因此，需要有一个数据结构来保存符号的绝对地址，这就是 GOT 表的作用，GOT 表中每项保存程序中引用其它符号的绝对地址，这样，程序就可以通过引用 GOT 表来获得某个符号的地址。</p>

<!--
在x86结构中，GOT表的前三项保留，用于保存特殊的数据结构地址，其它的各项保存符号的绝对地址。对于符号的动态解析过程，我们只需要了解的就是第二项和第三项，即GOT[1]和GOT[2]：GOT[1]保存的是一个地址，指向已经加载的共享库的链表地址（前面提到加载的共享库会形成一个链表）；GOT[2]保存的是一个函数的地址，定义如下：GOT[2] = &_dl_runtime_resolve，这个函数的主要作用就是找到某个符号的地址，并把它写到与此符号相关的GOT项中，然后将控制转移到目标函数，后面我们会详细分析。
-->

<h4 id="procedure-linkage-table-plt">Procedure Linkage Table, PLT</h4>

<p>过程链接表的作用就是将位置无关的函数调用转移到绝对地址。在编译链接时，链接器并不能控制执行从一个可执行文件或者共享文件中转移到另一个中（如前所说，这时候函数的地址还不能确定），因此，链接器将控制转移到PLT中的某一项。而PLT通过引用GOT表中的函数的绝对地址，来把控制转移到实际的函数。</p>

<p>在实际的可执行程序或者共享目标文件中，GOT表在名称为.got.plt的section中，PLT表在名称为.plt的section中。</p>

<h3 id="plt">PLT</h3>

<p>在通过 <code class="highlighter-rouge">objdump -S test</code> 命令返汇编之后，其中的 <code class="highlighter-rouge">.plt</code> 内容如下。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">Disassembly of section .plt:

0000000000400400 &lt;puts@plt-0x10&gt;:
  400400:       ff 35 02 0c 20 00       pushq  0x200c02(%rip)        # 601008 &lt;_GLOBAL_OFFSET_TABLE_+0x8&gt;
  400406:       ff 25 04 0c 20 00       jmpq   *0x200c04(%rip)        # 601010 &lt;_GLOBAL_OFFSET_TABLE_+0x10&gt;
  40040c:       0f 1f 40 00             nopl   0x0(%rax)

0000000000400410 &lt;puts@plt&gt;:
  400410:       ff 25 02 0c 20 00       jmpq   *0x200c02(%rip)        # 601018 &lt;_GLOBAL_OFFSET_TABLE_+0x18&gt;
  400416:       68 00 00 00 00          pushq  $0x0
  40041b:       e9 e0 ff ff ff          jmpq   400400 &lt;_init+0x20&gt;

0000000000400420 &lt;__libc_start_main@plt&gt;:
  400420:       ff 25 fa 0b 20 00       jmpq   *0x200bfa(%rip)        # 601020 &lt;_GLOBAL_OFFSET_TABLE_+0x20&gt;
  400426:       68 01 00 00 00          pushq  $0x1
  40042b:       e9 d0 ff ff ff          jmpq   400400 &lt;_init+0x20&gt;

0000000000400430 &lt;__gmon_start__@plt&gt;:
  400430:       ff 25 f2 0b 20 00       jmpq   *0x200bf2(%rip)        # 601028 &lt;_GLOBAL_OFFSET_TABLE_+0x28&gt;
  400436:       68 02 00 00 00          pushq  $0x2
  40043b:       e9 c0 ff ff ff          jmpq   400400 &lt;_init+0x20&gt;

Disassembly of section .text:</code></pre></figure>

<p>当然，也可以通过 <code class="highlighter-rouge">gdb</code> 命令进行反汇编。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">(gdb) disassemble 0x400410
Dump of assembler code for function puts@plt:
   0x0000000000400410 &lt;+0&gt;:     jmpq   *0x200c02(%rip)        # 0x601018 &lt;puts@got.plt&gt;   查看对应内存
   0x0000000000400416 &lt;+6&gt;:     pushq  $0x0
   0x000000000040041b &lt;+11&gt;:    jmpq   0x400400
End of assembler dump.</code></pre></figure>

<p>可以看到 <code class="highlighter-rouge">puts@plt</code> 中包含三条指令，而且可以看出，除 <code class="highlighter-rouge">PLT0(__gmon_start__@plt-0x10)</code> 所标记的内容，其它的所有 <code class="highlighter-rouge">PLT</code> 项的形式都是一样的，而且最后的 <code class="highlighter-rouge">jmp</code> 指令都是 <code class="highlighter-rouge">0x400400</code>，即 <code class="highlighter-rouge">PLT0</code> 为目标的；所不同的只是第一条 <code class="highlighter-rouge">jmp</code> 指令的目标和 <code class="highlighter-rouge">push</code> 指令中的数据。</p>

<p><code class="highlighter-rouge">PLT0</code> 则与之不同，但是包括 <code class="highlighter-rouge">PLT0</code> 在内的每个表项都占 16 个字节，所以整个 PLT 就像个数组。</p>

<p>另外，需要注意，每个 PLT 表项中的第一条 <code class="highlighter-rouge">jmp</code> 指令是间接寻址的，比如的 <code class="highlighter-rouge">puts()</code> 函数是以地址 <code class="highlighter-rouge">0x601018</code> 处的内容为目标地址进行中跳转的。</p>

<h3 id="got">GOT</h3>

<figure class="highlight"><pre><code class="language-text" data-lang="text">----- 实际等价于jmpq *0x601018 ，而*0x601018就是0x00400416，就是会调转到0x400416所在的地址执行，
----- 实际是顺序执行，最终会调转到0x400400
(gdb) x/w 0x601018
0x601018 &lt;puts@got.plt&gt;:        0x00400416

(gdb) x /3i 0x400400            查看反汇编
   0x400400:    pushq  0x200c02(%rip)         # 0x601008
   0x400406:    jmpq   *0x200c04(%rip)        # 0x601010   跟踪进入
   0x40040c:    nopl   0x0(%rax)

(gdb) b *0x400406               设置断点
(gdb) c
Breakpoint 2, 0x0000000000400406 in ?? ()
(gdb) ni
_dl_runtime_resolve () at ../sysdeps/x86_64/dl-trampoline.S:58
58              subq $REGISTER_SAVE_AREA,%rsp
(gdb) i r rip
rip            0x7ffff7df0290   0x7ffff7df0290 &lt;_dl_runtime_resolve&gt;</code></pre></figure>

<p>从上面可以看出，这个地址实际上就是顺序执行，也就是 <code class="highlighter-rouge">puts@plt</code> 中的第二条指令，不过正常来说这里应该保存的是 <code class="highlighter-rouge">puts()</code> 函数的地址才对，那为什么会这样呢？</p>

<!--这里的功能就是 Lazy Load，也就是延迟加载，只有在需要的时候才会加载。-->
<p>原来链接器在把所需要的共享库加载进内存后，并没有把共享库中的函数的地址写到 GOT 表项中，而是延迟到函数的第一次调用时，才会对函数的地址进行定位。</p>

<p>如上，在 <code class="highlighter-rouge">jmpq</code> 中设置一个断点，观察到，实际调转到了 <code class="highlighter-rouge">_dl_runtime_resolve()</code> 这个函数。</p>

<h3 id="地址解析">地址解析</h3>

<p>在 gdb 中，可以通过 <code class="highlighter-rouge">disassemble _dl_runtime_resolve</code> 查看该函数的反汇编，感兴趣的话可以看看其调用流程，这里简单介绍其功能。</p>

<p>从调用 <code class="highlighter-rouge">puts@plt</code> 到 <code class="highlighter-rouge">_dl_runtime_resolve</code> ，总共有两次压栈操作，一次是 <code class="highlighter-rouge">pushq  $0x0</code>，另外一次是 <code class="highlighter-rouge">pushq  0x200c02(%rip) # 601008</code>，分别表示了 <code class="highlighter-rouge">puts</code> 函数在 <code class="highlighter-rouge">GOT</code> 中的偏移以及 <code class="highlighter-rouge">GOT</code> 的起始地址。</p>

<p>在 <code class="highlighter-rouge">_dl_runtime_resolve()</code> 函数中，会解析到 <code class="highlighter-rouge">puts()</code> 函数的绝对地址，并保存到 <code class="highlighter-rouge">GOT</code> 相应的地址处，这样后续调用时则会直接调用 <code class="highlighter-rouge">puts()</code> 函数，而不用再次解析。</p>

<p><img src="https://jin-yang.github.io/images/linux/elf-load-process.png" alt="elf load" title="elf load" class="pull-center"></p>

<p>上图中的红线是解析过程，蓝线则是后面的调用流程。</p>

<h2 id="参考">参考</h2>

<p>关于动态库的加载过程，可以参考 <a href="https://github.com/tinyclub/open-c-book/blob/master/zh/chapters/02-chapter4.markdown">动态符号链接的细节</a>。</p>

<!--
reference/linux/linux-c-dynamic-loading.markdown

ELF文件的加载和动态链接过程
http://jzhihui.iteye.com/blog/1447570

linux是如何加载ELF格式的文件的，包括patchelf
http://cn.windyland.me/2014/10/24/how-to-load-a-elf-file/


readelf -h /usr/bin/uptime
可以找到 Entry point address ，也即程序的入口地址。

http://michalmalik.github.io/elf-dynamic-segment-struggles
https://greek0.net/elf.html
https://lwn.net/Articles/631631/
https://www.haiku-os.org/blog/lucian/2010-07-08_anatomy_elf/
 -->


  <hr>
  <nav>
    <ul class="pager">
         <li class="previous"><a href="/post/linux-program-io-multiplexing.html" title="Linux IO 多路复用">← Older</a></li> 
         <li class="next"><a href="/post/program-c-string-stuff.html" title="C 语言的字符串">Newer →</a></li> 
    </ul>
  </nav><br>
<!--
  <hr><div id="section-donate"><span>赏</span></div><br>
  <p style="text-indent:0px;text-align:center;">如果喜欢这里的文章，而且又不差钱的话，欢迎打赏个早餐 ^_^</p><br>
  <div class="row" style="text-align:center;" >
    <div class="col-md-6"><img src="/images/system/barcode-pay-alipay.jpg" style="width:150px;height:150px;" title="支付宝捐赠" /><br>支付宝打赏</div>
    <div class="col-md-6"><img src="/images/system/barcode-pay-wechat.jpg" style="width:150px;height:150px;" title="微信捐赠" /><br>微信打赏</div>
  </div><br><hr>
-->
  <p style="text-indent:0px;text-align:center;">如果喜欢这里的文章，而且又不差钱的话，欢迎打赏个早餐 ^_^</p>
  <div id="donate_module">
    <style type="text/css">
      .donate_bar a.btn_donate{
        display: inline-block;
        position:      relative;
        text-align:    center;
        width: 82px;
        height: 82px;
        background: url("/images/misc/btn_reward.gif") no-repeat;
        _background: url("/images/misc/btn_reward.gif") no-repeat;
        -webkit-transition: background 0s;
        -moz-transition: background 0s;
        -o-transition: background 0s;
        -ms-transition: background 0s;
        transition: background 0s;
      }
      .donate_bar a.btn_donate:hover{ background-position: 0px -82px;}
      .donate_bar .donate_txt {
        display: block;
        color: #9d9d9d;
        font: 14px/2 "Microsoft Yahei";
      }
    </style>
    <div id="donate_board" class="donate_bar row" style="text-align:center;">
      <a id="btn_donate" class="btn_donate" target="_self" href="javascript:;" title="Donate 打赏"></a>
    </div>
    <div id="donate_guide" class="donate_bar center hidden">
      <div class="row" style="text-align:center;">
        <div class="col-md-6"><img src="/images/system/barcode-pay-alipay.jpg" style="width:150px;height:150px;" title="支付宝捐赠"><br>支付宝打赏</div>
        <div class="col-md-6"><img src="/images/system/barcode-pay-wechat.jpg" style="width:150px;height:150px;" title="微信捐赠"><br>微信打赏</div>
      </div>
    </div>
    <script type="text/javascript">
      document.getElementById('btn_donate').onclick = function(){
        $('#donate_board').addClass('hidden');
        $('#donate_guide').removeClass('hidden');
      }
      function donate_on_web(){
        $('#donate').submit();
      }
    </script>
  </div>

  <!-- 多说评论框 start -->
  <!--<div class="ds-thread" data-thread-key="请将此处替换成文章在你的站点中的ID" data-title="请替换成文章的标题" data-url="请替换成文章的网址"></div>-->
  <!--
  <div class="ds-thread" data-thread-key="https://jin-yang.github.io/post/program-c-load-process.html" data-title="C 加载过程" data-url="https://jin-yang.github.io/post/program-c-load-process.html"></div>
  -->
  <!-- 多说评论框 end -->
  <!-- 多说公共JS代码 start (一个网页只需插入一次) -->
  <!--
  <script type="text/javascript">
      var duoshuoQuery = {short_name:"jinyangposts"};
      (function() {
          var ds = document.createElement('script');
          ds.type = 'text/javascript';ds.async = true;
          ds.src = (document.location.protocol == 'https:' ? 'https:' : 'http:') + '//static.duoshuo.com/embed.js';
          ds.charset = 'UTF-8';
          (document.getElementsByTagName('head')[0]
           || document.getElementsByTagName('body')[0]).appendChild(ds);
      })();
  </script>
  -->
  <!-- 多说公共JS代码 end -->
</div>
