<div class="post-container">
  <div class="blog-header">
    <h1>C 加載過程</h1>
    <div class="post-description">
      <i class="fa fa-calendar"></i> 2015-09-12 Saturday &nbsp; &nbsp;
      <i class="fa fa-tags"></i>  linux ,  program  
      
    </div>
  </div>
  <hr>
  <p>利用動態庫，可以節省磁盤、內存空間，而且可以提高程序運行效率；不過同時也導致調試比較困難，而且可能存在潛在的安全威脅。</p>

<p>這裡主要討論符號的動態鏈接過程，即程序在執行過程中，對其中包含的一些未確定地址的符號進行重定位的過程。</p>

<!-- more -->

<h2 id="簡介">簡介</h2>

<p><code class="highlighter-rouge">ld.so</code> (<span class="contextHighlighter">Dynamic Linker</span>/Loader) 和 <code class="highlighter-rouge">ldd</code> 都會使用到 ELF 格式中的 <code class="highlighter-rouge">.dynstr</code> (dynamic linking string table) 字段，如果通過 <code class="highlighter-rouge">strip -R .dynstr hello</code> 命令將該字段刪除，那麼 <code class="highlighter-rouge">ldd</code> 就會報錯。</p>

<h3 id="shell-執行">Shell 執行</h3>

<p>大致記錄一下 bash 的執行過程，當打開終端後，可以通過 <code class="highlighter-rouge">tty</code> 命令查看當前的虛擬終端，假設為 <code class="highlighter-rouge">/dev/pts/27</code>，然後再通過 <code class="highlighter-rouge">ps -ef | grep pts/27 | grep bash | grep -v grep</code> 查看對應的 PID 。</p>

<p>打開另一個終端，通過 <code class="highlighter-rouge"><span class="contextHighlighter">pstack</span> PID</code> 即可看到對應的調用堆棧。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">main()                               ← 各種初始化工作，shell.c
 |-reader_loop()                     ← 死循環，通過read_command()讀取命令，eval.c
   |-read_command()                  ← 通過parse_command()和yyparse()解析輸入的命令，eval.c
   |-execute_command()               ← 開始執行命令，execute_cmd.c
     |-execute_command_internal()
       |-execute_simple_command()
         |-execute_disk_command()
           |-execve()                ← 通過系統調用執行</code></pre></figure>

<p>其中詞法語法解析通過 <code class="highlighter-rouge">flex-biso</code> 解析，涉及的文件為 <code class="highlighter-rouge">parse.y</code>，沒有找到詞法解析的文件。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ rpm -qf `which bash`                      // 查看所屬包
$ yumdownloader --source bash               // 下載源碼
$ rpm2cpio bash-version.src.rpm | cpio -id  // 解壓源碼</code></pre></figure>

<p>通過 <code class="highlighter-rouge">strace ./hello</code> 查看系統調用，定位到 <code class="highlighter-rouge">execve()</code> ，也就是通過該函數執行。</p>

<h3 id="常見概念">常見概念</h3>

<p>解釋器 <code class="highlighter-rouge">.interp</code> 分區用於指定程序動態裝載、鏈接器 <code class="highlighter-rouge">ld-linux.so</code> 的位置，而過程鏈接表 <code class="highlighter-rouge">plt</code>、全局偏移表 <code class="highlighter-rouge">got</code>、重定位表則用於輔助動態鏈接過程。</p>

<h4 id="符號">符號</h4>

<p>對於可執行文件除了編譯器引入的一些符號外，主要就是用戶自定義的全局變量、函數等，而對於可重定位文件僅僅包含用戶自定義的一些符號。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">----- 生成可重定位文件，並通過nm命令查看ELF文件的符號表信息
$ gcc -c main.c
$ nm main.o
0000000000000000 B global
0000000000000000 T main
                 U printf</code></pre></figure>

<p>上面包含全局變量、自定義函數以及動態鏈接庫中的函數，但不包含局部變量，而且發現這三個符號的地址都沒有確定。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">----- 生成可執行文件
$ gcc -o main main.o
$ nm main | egrep "main$| printf|global$"
0000000000601038 B global
000000000040052d T main
                 U printf@@GLIBC_2.2.5</code></pre></figure>

<p>經鏈接之後，<code class="highlighter-rouge">global</code> 和 <code class="highlighter-rouge">main</code> 的地址都已經確定了，但是 <code class="highlighter-rouge">printf</code> 卻還沒，因為它是動態鏈接庫 <code class="highlighter-rouge">glibc</code> 中定義函數，需要動態鏈接，而不是這裡的靜態鏈接。</p>

<p>也就是說 main.o 中的符號地址沒有確定，而經過鏈接後部分符號地址已經確定，也就是對符號的引用變成了對地址的引用，這樣程序運行時就可通過訪問內存地址而訪問特定的數據。對於動態鏈接庫，也就是上述的 <code class="highlighter-rouge">printf()</code> 則需要在運行時通過動態鏈接器 ld-linux.so 進行重定位，即動態鏈接。</p>

<p>另外，除了 nm 還可以用 <code class="highlighter-rouge">readelf -s</code> 查看 <code class="highlighter-rouge">.dynsym</code> 表或者用 <code class="highlighter-rouge">objdump -tT</code> 查看。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ nm -D /lib64/libc-2.17.so | grep "\ printf$"</code></pre></figure>

<p>注意，在部分新系統上，如果不使用參數 <code class="highlighter-rouge">-D</code> ，那麼可能會無法查看符號表，因為 nm 默認打印 <code class="highlighter-rouge">.symtab</code> 和 <code class="highlighter-rouge">.strtab</code>，不過一般在打包時會通過 strip 刪除掉，只保留了動態符號 (在 <code class="highlighter-rouge">.dynsym</code> 和 <code class="highlighter-rouge">.dynstr</code> 中)，以便動態鏈接器在執行程序時尋址這些外部用到的符號。</p>

<!-- https://stackoverflow.com/questions/9961473/nm-vs-readelf-s -->

<!--
## 動態鏈接

動態鏈接就是在程序運行時對符號進行重定位，確定符號對應的內存地址的過程。為了提高效率，Linux 下符號的動態鏈接默認採用 Lazy Mode 方式，也就是在程序運行過程中用到該符號時才去解析它的地址。

不過這種默認是可以通過設置 LD_BIND_NOW 為非空來打破的（下面會通過實例來分析這個變量的作用），也就是說如果設置了這個變量，動態鏈接器將在程序加載後和符號被使用之前就對這些符號的地址進行解析。

## 動態鏈接庫

在程序中，保存了依賴的庫信息。

$ readelf -d main | grep NEEDED
 0x0000000000000001 (NEEDED)             Shared library: [libc.so.6]

在 `.dynamic` 分區中保存了和動態鏈接相關的信息，當然只有版本信息，而沒有保存絕對路徑，其搜索路徑如上所述。

### FIXME: 獲取動態鏈接器

動態鏈接器保存在 `.interp` 分區，可以通過 `readelf -x .interp main` 命令查看。


注意，與 libc.so 之類的動態庫不同，ld-linux.so 的路徑是絕對路徑，而類似於 libc.so 庫則只包含了文件名。

這是因為，程序執行時 ld-linux.so 最先被加載到內存，沒有其他程序知道去哪裡查找 ld-linux.so，所以它的路徑必須是絕對的；當 ld-linux.so 被裝載以後，由它來去裝載可執行文件和相關的共享庫，它會根據下面介紹的流程去加載。

### 動態鏈接器

可以通過 `man ld-linux` 獲取與動態鏈接器相關的資料，包括各種相關的環境變量和文件都有詳細的說明。

對於環境變量，除了上面提到過的 LD_LIBRARY_PATH 和 LD_BIND_NOW 變量外，還有其他幾個重要參數，比如 LD_PRELOAD 用於指定預裝載一些庫，以便替換其他庫中的函數，從而做一些安全方面的處理 [6]，[9]，[12]，而環境變量 LD_DEBUG 可以用來進行動態鏈接的相關調試。
對於文件，除了上面提到的 ld.so.conf 和 ld.so.cache 外，還有一個文件 /etc/ld.so.preload 用於指定需要預裝載的庫。

實際上，ELF 格式可以從兩個角度去看，包括鏈接和執行，分別通過 `Section Header Table` 和 `Program Header Table` 表示。

SHT 保存了 ELF 所包含的段信息，可以通過 `readelf -S /bin/bash` 查看，其中比較重要的有 REL sections (relocations), SYMTAB/DYNSYM (symbol tables), VERSYM/VERDEF/VERNEED sections (symbol versioning information).

#### 1. 加載到內存

在 ELF 文件的文件頭中就指定了該文件的入口地址，程序的代碼和數據部分會相繼映射到對應的內存中。

$ readelf -h /bin/bash | grep Entry
  Entry point address:               0x41d361
-->

<h2 id="內核加載">內核加載</h2>

<p>ELF 有靜態和動態鏈接兩種方式，加載過程由內核開始，而動態鏈接庫的加載則可以在用戶層完成。GNU 對於動態鏈接過程為 A) 把 ELF 映像的裝入/啟動加載在 Linux 內核中；B) 把動態鏈接的實現放在用戶空間 (glibc)，併為此提供一個稱為 “解釋器” (ld-linux.so.2) 工具。</p>

<p>注意，解釋器的裝入/啟動也由內核負責，詳細可以查看 <a href="/post/kernel-memory-management-from-userspace-view.html">內存-用戶空間</a> 中的介紹，在此只介紹 ELF 的加載過程。</p>

<h3 id="內核模塊">內核模塊</h3>

<p>如果要支持不同的執行格式，需要在內核中添加註冊模塊，每種類型通過 <code class="highlighter-rouge">struct linux_binfmt</code> 格式表示，其定義以及 ELF 的定義如下所示：</p>

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

<p>其中的 <code class="highlighter-rouge">load_binary</code> 函數指針指向的就是一個可執行程序的處理函數，要支持 ELF 文件的運行，則必須通過 <code class="highlighter-rouge">register_binfmt()</code> 向內核登記這個數據結構，加入到內核支持的可執行程序的隊列中。</p>

<p>當要運行程序時，則掃描該隊列，讓各對象所提供的處理程序 (ELF中即為<code class="highlighter-rouge">load_elf_binary()</code>)，逐一前來認領，如果某個格式的處理程序發現相符後，便執行該格式映像的裝入和啟動。</p>

<h3 id="內核加載-1">內核加載</h3>

<p>內核執行 <code class="highlighter-rouge">execv()</code> 或 <code class="highlighter-rouge">execve()</code> 系統調用時，會通過 <code class="highlighter-rouge">do_execve()</code> 調用，該函數先打開目標映像文件，並讀入文件的頭部信息，也就是開始 128 字節。</p>

<p>然後，調用另一個 <code class="highlighter-rouge">search_binary_handler()</code> 函數，該函數中會搜索上面提到的 Linux 支持的可執行文件類型隊列，讓各種可執行程序的處理程序前來認領和處理。</p>

<p>如果類型匹配，則調用 <code class="highlighter-rouge">load_binary</code> 函數指針所指向的處理函數來處理目標映像文件，對於 ELF 文件也就是 <code class="highlighter-rouge">load_elf_binary()</code> 函數，下面主要就是分析 <code class="highlighter-rouge">load_elf_binary()</code> 的執行過程。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="k">static</span> <span class="kt">int</span> <span class="nf">load_elf_binary</span><span class="p">(</span><span class="k">struct</span> <span class="n">linux_binprm</span> <span class="o">*</span><span class="n">bprm</span><span class="p">)</span>
<span class="p">{</span>
    <span class="cm">/* 上述已經讀取 128 字節的頭部信息 */</span>
    <span class="n">loc</span><span class="o">-&gt;</span><span class="n">elf_ex</span> <span class="o">=</span> <span class="o">*</span><span class="p">((</span><span class="k">struct</span> <span class="n">elfhdr</span> <span class="o">*</span><span class="p">)</span><span class="n">bprm</span><span class="o">-&gt;</span><span class="n">buf</span><span class="p">);</span>

    <span class="n">retval</span> <span class="o">=</span> <span class="o">-</span><span class="n">ENOEXEC</span><span class="p">;</span>
    <span class="cm">/* 首先校驗ELF的頭部信息，也就是"\177ELF" */</span>
    <span class="k">if</span> <span class="p">(</span><span class="n">memcmp</span><span class="p">(</span><span class="n">loc</span><span class="o">-&gt;</span><span class="n">elf_ex</span><span class="p">.</span><span class="n">e_ident</span><span class="p">,</span> <span class="n">ELFMAG</span><span class="p">,</span> <span class="n">SELFMAG</span><span class="p">)</span> <span class="o">!=</span> <span class="mi">0</span><span class="p">)</span>
        <span class="k">goto</span> <span class="n">out</span><span class="p">;</span>
    <span class="cm">/* 只能是可執行文件(ET_EXEC)或者動態庫(ET_DYN) */</span>
    <span class="k">if</span> <span class="p">(</span><span class="n">loc</span><span class="o">-&gt;</span><span class="n">elf_ex</span><span class="p">.</span><span class="n">e_type</span> <span class="o">!=</span> <span class="n">ET_EXEC</span> <span class="o">&amp;&amp;</span> <span class="n">loc</span><span class="o">-&gt;</span><span class="n">elf_ex</span><span class="p">.</span><span class="n">e_type</span> <span class="o">!=</span> <span class="n">ET_DYN</span><span class="p">)</span>
        <span class="k">goto</span> <span class="n">out</span><span class="p">;</span>

    <span class="cm">/* 讀取ELF的頭部信息，也就是整個Program Header Table，一個可執行程序必須至少有一個段，且不能超過64K */</span>
    <span class="n">retval</span> <span class="o">=</span> <span class="n">kernel_read</span><span class="p">(</span><span class="n">bprm</span><span class="o">-&gt;</span><span class="n">file</span><span class="p">,</span> <span class="n">loc</span><span class="o">-&gt;</span><span class="n">elf_ex</span><span class="p">.</span><span class="n">e_phoff</span><span class="p">,</span> <span class="p">(</span><span class="kt">char</span> <span class="o">*</span><span class="p">)</span><span class="n">elf_phdata</span><span class="p">,</span> <span class="n">size</span><span class="p">);</span>
    <span class="k">if</span> <span class="p">(</span><span class="n">retval</span> <span class="o">!=</span> <span class="n">size</span><span class="p">)</span> <span class="p">{</span>
        <span class="k">if</span> <span class="p">(</span><span class="n">retval</span> <span class="o">&gt;=</span> <span class="mi">0</span><span class="p">)</span>
            <span class="n">retval</span> <span class="o">=</span> <span class="o">-</span><span class="n">EIO</span><span class="p">;</span>
        <span class="k">goto</span> <span class="n">out_free_ph</span><span class="p">;</span>
    <span class="p">}</span>

    <span class="cm">/* 在for循環中，用來尋找和處理目標映像的"解釋器"段，可以通過 ???????? 查看 */</span>
    <span class="k">for</span> <span class="p">(</span><span class="n">i</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span> <span class="n">i</span> <span class="o">&lt;</span> <span class="n">loc</span><span class="o">-&gt;</span><span class="n">elf_ex</span><span class="p">.</span><span class="n">e_phnum</span><span class="p">;</span> <span class="n">i</span><span class="o">++</span><span class="p">)</span> <span class="p">{</span>
        <span class="k">if</span> <span class="p">(</span><span class="n">elf_ppnt</span><span class="o">-&gt;</span><span class="n">p_type</span> <span class="o">==</span> <span class="n">PT_INTERP</span><span class="p">)</span> <span class="p">{</span>
            <span class="cm">/* 找到後就根據其位置的p_offset和大小p_filesz把整個"解釋器"段的內容讀入緩衝區，實際上
             * 這個"解釋器"段實際上只是一個字符串，即解釋器的文件名，如"/lib/ld-linux.so.2"
             */</span>
            <span class="n">retval</span> <span class="o">=</span> <span class="n">kernel_read</span><span class="p">(</span><span class="n">bprm</span><span class="o">-&gt;</span><span class="n">file</span><span class="p">,</span> <span class="n">elf_ppnt</span><span class="o">-&gt;</span><span class="n">p_offset</span><span class="p">,</span>
                         <span class="n">elf_interpreter</span><span class="p">,</span> <span class="n">elf_ppnt</span><span class="o">-&gt;</span><span class="n">p_filesz</span><span class="p">);</span>
            <span class="k">if</span> <span class="p">(</span><span class="n">retval</span> <span class="o">!=</span> <span class="n">elf_ppnt</span><span class="o">-&gt;</span><span class="n">p_filesz</span><span class="p">)</span> <span class="p">{</span>
                <span class="k">if</span> <span class="p">(</span><span class="n">retval</span> <span class="o">&gt;=</span> <span class="mi">0</span><span class="p">)</span>
                    <span class="n">retval</span> <span class="o">=</span> <span class="o">-</span><span class="n">EIO</span><span class="p">;</span>
                <span class="k">goto</span> <span class="n">out_free_interp</span><span class="p">;</span>
            <span class="p">}</span>

            <span class="cm">/* 通過open_exec()打開這個文件，再通過kernel_read()讀入開始的128個字節，也就是解釋器的頭部 */</span>
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

    <span class="cm">/* 從目標映像的程序頭中搜索類型為PT_LOAD的段；在二進制映像中，只有類型為PT_LOAD的段才是需要加載的。
     * 只是在加載前，需要確定加載的地址，包括頁對齊、該段的p_vaddr域的值；在確定了裝入地址後，就會通過
     * elf_map()建立用戶空間虛擬地址空間與目標映像文件中某個連續區間之間的映射，其返回值就是實際映射的
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

    <span class="cm">/* 若存在PT_INTERP段，則通過load_elf_interp()加載，並把進入用戶空間的入口地址設置成load_elf_interp()
     * 的返回值，即解釋器映像的入口地址；如果不裝入解釋器，那麼這個入口地址就是目標映像本身的入口地址，
     * 也就是靜態編譯的程序。
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

    <span class="cm">/* 完成加載且啟動用戶空間的映像運行之前，還需要為目標映像和解釋器準備好一些有關的信息，這些信息包括常
     * 規的argc、envc等等，還有一些"輔助向量(Auxiliary Vector)"。這些信息需要複製到用戶空間，使它們在CPU
     * 進入解釋器或目標映像的程序入口時出現在用戶空間堆棧上。這裡的create_elf_tables()就起著這個作用。
     */</span>
    <span class="n">retval</span> <span class="o">=</span> <span class="n">create_elf_tables</span><span class="p">(</span><span class="n">bprm</span><span class="p">,</span> <span class="o">&amp;</span><span class="n">loc</span><span class="o">-&gt;</span><span class="n">elf_ex</span><span class="p">,</span> <span class="n">load_addr</span><span class="p">,</span> <span class="n">interp_load_addr</span><span class="p">);</span>
    <span class="k">if</span> <span class="p">(</span><span class="n">retval</span> <span class="o">&lt;</span> <span class="mi">0</span><span class="p">)</span>
        <span class="k">goto</span> <span class="n">out</span><span class="p">;</span>

    <span class="cm">/* 最後，通過start_thread()宏將eip和esp改成新的地址，就使得CPU在返回用戶空間時就進入新的程序入口。如果存
     * 在解釋器映像，那麼這就是解釋器映像的程序入口(動態鏈接)，否則就是目標映像的程序入口(靜態鏈接)。
     */</span>
    <span class="n">start_thread</span><span class="p">(</span><span class="n">regs</span><span class="p">,</span> <span class="n">elf_entry</span><span class="p">,</span> <span class="n">bprm</span><span class="o">-&gt;</span><span class="n">p</span><span class="p">);</span>
<span class="p">}</span></code></pre></figure>

<h2 id="加載過程">加載過程</h2>

<p>依賴動態庫時，會在加載時根據可執行文件的地址和動態庫的對應符號的地址推算出被調用函數的地址，這個過程被稱為動態鏈接。</p>

<p>假設，現在使用的是 Position Independent Code, PIC 模型。</p>

<h4 id="1-獲取動態鏈接器">1. 獲取動態鏈接器</h4>

<p>首先，讀取 ELF 頭部信息，解析出 <code class="highlighter-rouge">PT_INTERP</code> 信息，確定動態鏈接器的路徑，可以通過 <code class="highlighter-rouge">readelf -l foobar</code> 查看，一般是 <code class="highlighter-rouge">/lib/ld-linux.so.2</code> 或者 <code class="highlighter-rouge">/lib64/ld-linux-x86-64.so.2</code> 。</p>

<h4 id="2-加載動態庫">2. 加載動態庫</h4>

<p>關於加載的詳細順序可以查看 <code class="highlighter-rouge">man ld</code> 中 rpath-link 的介紹，一般順序為：</p>

<ol>
  <li>鏈接時 <code class="highlighter-rouge">-rpath-link</code> 參數指定路徑，只用於鏈接時使用，編譯時通過 <code class="highlighter-rouge">-Wl,rpath-link=</code> 指定；</li>
  <li>鏈接時通過 <code class="highlighter-rouge">-rpath</code> 參數指定路徑，除了用於鏈接時使用，還會在運行時使用，編譯時可利用 <code class="highlighter-rouge">-Wl,rpath=</code> 指定，會生成 <code class="highlighter-rouge">DT_RPATH</code> 或者 <code class="highlighter-rouge">DT_RUNPATH</code> 定義，可以通過 <code class="highlighter-rouge">readelf -d main | grep -E (RPATH|RUNPATH)</code> 查看；</li>
  <li>查找 <code class="highlighter-rouge">DT_RUNPATH</code> 或者 <code class="highlighter-rouge">DT_RPATH</code> 指定的路徑，如果前者存在則忽略後者；</li>
  <li>依次查看 <code class="highlighter-rouge">LD_RUN_PATH</code> 和 <code class="highlighter-rouge">LD_LIBRARY_PATH</code> 環境變量指定路徑；</li>
  <li>查找默認路徑，一般是 <code class="highlighter-rouge">/lib</code> 和 <code class="highlighter-rouge">/usr/lib</code> ，然後是 <code class="highlighter-rouge">/etc/ld.so.conf</code> 文件中的配置。</li>
</ol>

<p>另外，需要加載哪些庫通過 <code class="highlighter-rouge">DT_NEEDED</code> 字段來獲取，每條對應了一個動態庫，可以通過 <code class="highlighter-rouge">readelf -d main | grep NEEDED</code> 查看。</p>

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

<p>然後可以通過依次設置如上的加載路徑進行測試。<strong>注意</strong>，在對 <code class="highlighter-rouge">/etc/ld.so.conf</code> 文件設置後需要通過 <code class="highlighter-rouge">ldconfig</code> 更新 cache 才會生效。</p>

<p>另外，推薦使用 <code class="highlighter-rouge">DT_RUNPATH</code> 而非 <code class="highlighter-rouge">DT_RPATH</code> ，此時，在編譯時需要用到 <code class="highlighter-rouge">--enable-new-dtags</code> 參數。</p>

<!--
通過C語言直接讀取rpath參數
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

<p>不同版本的動態庫可能會不兼容，那麼如果程序在編譯時指定動態庫是某個低版本，運行是用的一個高版本，可能會導致無法運行。</p>

<p>假設有如下的示例：</p>

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

<p>需要注意是，參數 <code class="highlighter-rouge">-Wl,soname</code> 中間沒有空格，<code class="highlighter-rouge">-Wl</code> 選項用來告訴編譯器將後面的參數傳遞給鏈接器，而 <code class="highlighter-rouge">-soname</code> 則指定了動態庫的 <code class="highlighter-rouge">soname</code>。運行後在當前目錄下會生成一個 <code class="highlighter-rouge">libhello.so.0.0.1</code> 文件，當運行 <code class="highlighter-rouge">ldconfig -n .</code> 命令時，當前目錄會多一個符號連接。</p>

<p>這個軟鏈接是根據編譯生成 <code class="highlighter-rouge">libhello.so.0.0.1</code> 時指定的 <code class="highlighter-rouge">-soname</code> 生成的，會保存到編譯生成的文件中，可以通過 <code class="highlighter-rouge">readelf -d foobar</code> 查看依賴的庫。</p>

<p>所以關鍵就是這個 soname，它相當於一箇中間者，當我們的動態庫只是升級一個小版本時，可以讓它的 soname 相同，而可執行程序只認 soname 指定的動態庫，這樣依賴這個動態庫的可執行程序不需重新編譯就能使用新版動態庫的特性。</p>

<h4 id="測試程序">測試程序</h4>

<p>示例程序如下。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cm">/* filename:main.c */</span>
<span class="cp">#include "hello.h"
</span><span class="kt">int</span> <span class="nf">main</span><span class="p">()</span>
<span class="p">{</span>
    <span class="n">hello</span><span class="p">(</span><span class="s">"foobar"</span><span class="p">);</span>
    <span class="k">return</span> <span class="mi">0</span><span class="p">;</span>
<span class="p">}</span></code></pre></figure>

<p>然後可以通過 <code class="highlighter-rouge">gcc main.c -L. -lhello -o main</code> 編譯，不過此時會報 <code class="highlighter-rouge">cannot find -lhello.so.0</code> 錯誤，也就是找不到對應的庫。</p>

<p>在 Linux 中，編譯時指定 <code class="highlighter-rouge">-lhello</code> 時，鏈接器會去查找 <code class="highlighter-rouge">libhello.so</code> 這樣的文件，如果當前目錄下沒有這個文件，那麼就會導致報錯；此時，可以通過 <code class="highlighter-rouge">ln -s libhello.so.0.0.1 libhello.so</code> 建立這樣一個軟鏈接。</p>

<p>通過 <code class="highlighter-rouge">ldd</code> 查看時，發現實際依賴的是 <code class="highlighter-rouge">libhello.so.0</code> 而非 <code class="highlighter-rouge">libhello</code> 也不是 <code class="highlighter-rouge">libhello.so.0.0.1</code> ，其實在生成 main 程序的過程有如下幾步：</p>

<ol>
  <li>鏈接器通過編譯命令 <code class="highlighter-rouge">-L. -lhello</code> 在當前目錄查找 <code class="highlighter-rouge">libhello.so</code> 文件；</li>
  <li>讀取 <code class="highlighter-rouge">libhello.so</code> 鏈接指向的實際文件，這裡是 <code class="highlighter-rouge">libhello.so.0.0.1</code>；</li>
  <li>讀取 <code class="highlighter-rouge">libhello.so.0.0.1</code> 中的 <code class="highlighter-rouge">SONAME</code>，這裡是 <code class="highlighter-rouge">libhello.so.0</code>；</li>
  <li>將 <code class="highlighter-rouge">libhello.so.0</code> 記錄到 <code class="highlighter-rouge">main</code> 程序的二進制數據裡。</li>
</ol>

<p>也就是說 <code class="highlighter-rouge">libhello.so.0</code> 是已經存儲到 main 程序的二進制數據裡的，不管這個程序在哪裡，通過 <code class="highlighter-rouge">ldd</code> 查看它依賴的動態庫都是 <code class="highlighter-rouge">libhello.so.0</code> 。</p>

<p>那麼，在部署時，只需要安裝 <code class="highlighter-rouge">libhello.so.0</code> 即可。</p>

<h4 id="版本更新">版本更新</h4>

<p>假設動態庫需要做一個小小的改動。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cm">/* filename:hello.c */</span>
<span class="cp">#include &lt;stdio.h&gt;
</span><span class="kt">void</span> <span class="nf">hello</span><span class="p">(</span><span class="k">const</span> <span class="kt">char</span><span class="o">*</span> <span class="n">name</span><span class="p">)</span>
<span class="p">{</span>
    <span class="n">printf</span><span class="p">(</span><span class="s">"hello %s, welcom to our world!</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span> <span class="n">name</span><span class="p">);</span>
<span class="p">}</span></code></pre></figure>

<p>由於改動較小，編譯動態庫時仍然指定相同的 soname 。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ gcc hello.c -fPIC -shared -Wl,-soname,libhello.so.0 -o libhello.so.0.0.2</code></pre></figure>

<p>然後重新運行 <code class="highlighter-rouge">ldconfig -n .</code> 即可，會發現鏈接指向了新版本，然後直接運行即可。</p>

<p>同樣，假如我們的動態庫有大的改動，編譯動態庫時指定了新的 soname，如下：</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ gcc hello.c -fPIC -shared -Wl,-soname,libhello.so.1 -o libhello.so.1.0.0</code></pre></figure>

<p>將動態庫文件拷貝到運行目錄，並執行 <code class="highlighter-rouge">ldconfig -n .</code>，不過此時需要重新編譯才可以。</p>

<h2 id="動態解析">動態解析</h2>

<p>如上所述，控制權先是提交到解釋器，由解釋器加載動態庫，然後控制權才會到用戶程序。動態庫加載的大致過程就是將每一個依賴的動態庫都加載到內存，並形成一個鏈表，後面的符號解析過程主要就是在這個鏈表中搜索符號的定義。</p>

<!--
對於靜態文件通常只有一個文件要被映射，而動態則還有所依賴的共享目標文件，通過 /proc/PID/maps 可以查看在內存中的分佈。
地址隨機實際上包括了動態鏈接庫、堆、棧，而對於程序本身的函數，其地址是固定的。
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

----- 編譯連接
$ gcc test.c -o test -g
----- 打印程序的反彙編
$ objdump -S test

----- 使用gdb調式
$ gdb test -q
(gdb) break main
(gdb) run
(gdb) disassemble
Dump of assembler code for function main:
   0x000000000040053d &lt;+0&gt;:     push   %rbp
   0x000000000040053e &lt;+1&gt;:     mov    %rsp,%rbp
=&gt; 0x0000000000400541 &lt;+4&gt;:     callq  0x40052d &lt;foobar&gt;    此處調用的地址是固定的
   0x0000000000400546 &lt;+9&gt;:     mov    $0x0,%eax
   0x000000000040054b &lt;+14&gt;:    pop    %rbp
   0x000000000040054c &lt;+15&gt;:    retq   
End of assembler dump.
(gdb) disassemble foobar
Dump of assembler code for function foobar:
   0x000000000040052d &lt;+0&gt;:     push   %rbp
   0x000000000040052e &lt;+1&gt;:     mov    %rsp,%rbp
   0x0000000000400531 &lt;+4&gt;:     mov    $0x4005e0,%edi
   0x0000000000400536 &lt;+9&gt;:     callq  0x400410 &lt;puts@plt&gt;  反彙編
   0x000000000040053b &lt;+14&gt;:    pop    %rbp
   0x000000000040053c &lt;+15&gt;:    retq   
End of assembler dump.</code></pre></figure>

<p>從上面反彙編代碼可以看出，在調用 <code class="highlighter-rouge">foobar()</code> 時，使用的是絕對地址，<code class="highlighter-rouge">printf()</code> 的調用已經換成了 <code class="highlighter-rouge">puts()</code> ，調用的是 <code class="highlighter-rouge">puts@plt</code> 這個標號，位於 <code class="highlighter-rouge">0x400410</code>，實際上這是一個 PLT 條目，可以通過反彙編查看相應的代碼，不過它代表什麼意思呢？</p>

<p>在進一步說明符號的動態解析過程以前，需要先了解兩個概念，一個是 <code class="highlighter-rouge">Global Offset Table</code>，一個是 <code class="highlighter-rouge">Procedure Linkage Table</code> 。</p>

<h4 id="global-offset-table-got">Global Offset Table, GOT</h4>

<p>在位置無關代碼中，如共享庫，一般不會包含絕對虛擬地址，而是在程序中引用某個共享庫中的符號時，編譯鏈接階段並不知道這個符號的具體位置，只有等到動態鏈接器將所需要的共享庫加載時進內存後，也就是在運行階段，符號的地址才會最終確定。</p>

<p>因此，需要有一個數據結構來保存符號的絕對地址，這就是 GOT 表的作用，GOT 表中每項保存程序中引用其它符號的絕對地址，這樣，程序就可以通過引用 GOT 表來獲得某個符號的地址。</p>

<!--
在x86結構中，GOT表的前三項保留，用於保存特殊的數據結構地址，其它的各項保存符號的絕對地址。對於符號的動態解析過程，我們只需要瞭解的就是第二項和第三項，即GOT[1]和GOT[2]：GOT[1]保存的是一個地址，指向已經加載的共享庫的鏈表地址（前面提到加載的共享庫會形成一個鏈表）；GOT[2]保存的是一個函數的地址，定義如下：GOT[2] = &_dl_runtime_resolve，這個函數的主要作用就是找到某個符號的地址，並把它寫到與此符號相關的GOT項中，然後將控制轉移到目標函數，後面我們會詳細分析。
-->

<h4 id="procedure-linkage-table-plt">Procedure Linkage Table, PLT</h4>

<p>過程鏈接表的作用就是將位置無關的函數調用轉移到絕對地址。在編譯鏈接時，鏈接器並不能控制執行從一個可執行文件或者共享文件中轉移到另一箇中（如前所說，這時候函數的地址還不能確定），因此，鏈接器將控制轉移到PLT中的某一項。而PLT通過引用GOT表中的函數的絕對地址，來把控制轉移到實際的函數。</p>

<p>在實際的可執行程序或者共享目標文件中，GOT表在名稱為.got.plt的section中，PLT表在名稱為.plt的section中。</p>

<h3 id="plt">PLT</h3>

<p>在通過 <code class="highlighter-rouge">objdump -S test</code> 命令返彙編之後，其中的 <code class="highlighter-rouge">.plt</code> 內容如下。</p>

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

<p>當然，也可以通過 <code class="highlighter-rouge">gdb</code> 命令進行反彙編。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">(gdb) disassemble 0x400410
Dump of assembler code for function puts@plt:
   0x0000000000400410 &lt;+0&gt;:     jmpq   *0x200c02(%rip)        # 0x601018 &lt;puts@got.plt&gt;   查看對應內存
   0x0000000000400416 &lt;+6&gt;:     pushq  $0x0
   0x000000000040041b &lt;+11&gt;:    jmpq   0x400400
End of assembler dump.</code></pre></figure>

<p>可以看到 <code class="highlighter-rouge">puts@plt</code> 中包含三條指令，而且可以看出，除 <code class="highlighter-rouge">PLT0(__gmon_start__@plt-0x10)</code> 所標記的內容，其它的所有 <code class="highlighter-rouge">PLT</code> 項的形式都是一樣的，而且最後的 <code class="highlighter-rouge">jmp</code> 指令都是 <code class="highlighter-rouge">0x400400</code>，即 <code class="highlighter-rouge">PLT0</code> 為目標的；所不同的只是第一條 <code class="highlighter-rouge">jmp</code> 指令的目標和 <code class="highlighter-rouge">push</code> 指令中的數據。</p>

<p><code class="highlighter-rouge">PLT0</code> 則與之不同，但是包括 <code class="highlighter-rouge">PLT0</code> 在內的每個表項都佔 16 個字節，所以整個 PLT 就像個數組。</p>

<p>另外，需要注意，每個 PLT 表項中的第一條 <code class="highlighter-rouge">jmp</code> 指令是間接尋址的，比如的 <code class="highlighter-rouge">puts()</code> 函數是以地址 <code class="highlighter-rouge">0x601018</code> 處的內容為目標地址進行中跳轉的。</p>

<h3 id="got">GOT</h3>

<figure class="highlight"><pre><code class="language-text" data-lang="text">----- 實際等價於jmpq *0x601018 ，而*0x601018就是0x00400416，就是會調轉到0x400416所在的地址執行，
----- 實際是順序執行，最終會調轉到0x400400
(gdb) x/w 0x601018
0x601018 &lt;puts@got.plt&gt;:        0x00400416

(gdb) x /3i 0x400400            查看反彙編
   0x400400:    pushq  0x200c02(%rip)         # 0x601008
   0x400406:    jmpq   *0x200c04(%rip)        # 0x601010   跟蹤進入
   0x40040c:    nopl   0x0(%rax)

(gdb) b *0x400406               設置斷點
(gdb) c
Breakpoint 2, 0x0000000000400406 in ?? ()
(gdb) ni
_dl_runtime_resolve () at ../sysdeps/x86_64/dl-trampoline.S:58
58              subq $REGISTER_SAVE_AREA,%rsp
(gdb) i r rip
rip            0x7ffff7df0290   0x7ffff7df0290 &lt;_dl_runtime_resolve&gt;</code></pre></figure>

<p>從上面可以看出，這個地址實際上就是順序執行，也就是 <code class="highlighter-rouge">puts@plt</code> 中的第二條指令，不過正常來說這裡應該保存的是 <code class="highlighter-rouge">puts()</code> 函數的地址才對，那為什麼會這樣呢？</p>

<!--這裡的功能就是 Lazy Load，也就是延遲加載，只有在需要的時候才會加載。-->
<p>原來鏈接器在把所需要的共享庫加載進內存後，並沒有把共享庫中的函數的地址寫到 GOT 表項中，而是延遲到函數的第一次調用時，才會對函數的地址進行定位。</p>

<p>如上，在 <code class="highlighter-rouge">jmpq</code> 中設置一個斷點，觀察到，實際調轉到了 <code class="highlighter-rouge">_dl_runtime_resolve()</code> 這個函數。</p>

<h3 id="地址解析">地址解析</h3>

<p>在 gdb 中，可以通過 <code class="highlighter-rouge">disassemble _dl_runtime_resolve</code> 查看該函數的反彙編，感興趣的話可以看看其調用流程，這裡簡單介紹其功能。</p>

<p>從調用 <code class="highlighter-rouge">puts@plt</code> 到 <code class="highlighter-rouge">_dl_runtime_resolve</code> ，總共有兩次壓棧操作，一次是 <code class="highlighter-rouge">pushq  $0x0</code>，另外一次是 <code class="highlighter-rouge">pushq  0x200c02(%rip) # 601008</code>，分別表示了 <code class="highlighter-rouge">puts</code> 函數在 <code class="highlighter-rouge">GOT</code> 中的偏移以及 <code class="highlighter-rouge">GOT</code> 的起始地址。</p>

<p>在 <code class="highlighter-rouge">_dl_runtime_resolve()</code> 函數中，會解析到 <code class="highlighter-rouge">puts()</code> 函數的絕對地址，並保存到 <code class="highlighter-rouge">GOT</code> 相應的地址處，這樣後續調用時則會直接調用 <code class="highlighter-rouge">puts()</code> 函數，而不用再次解析。</p>

<p><img src="https://jin-yang.github.io/images/linux/elf-load-process.png" alt="elf load" title="elf load" class="pull-center"></p>

<p>上圖中的紅線是解析過程，藍線則是後面的調用流程。</p>

<h2 id="參考">參考</h2>

<p>關於動態庫的加載過程，可以參考 <a href="https://github.com/tinyclub/open-c-book/blob/master/zh/chapters/02-chapter4.markdown">動態符號鏈接的細節</a>。</p>

<!--
reference/linux/linux-c-dynamic-loading.markdown

ELF文件的加載和動態鏈接過程
http://jzhihui.iteye.com/blog/1447570

linux是如何加載ELF格式的文件的，包括patchelf
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
         <li class="previous"><a href="/post/linux-program-io-multiplexing.html" title="Linux IO 多路複用">← Older</a></li> 
         <li class="next"><a href="/post/program-c-string-stuff.html" title="C 語言的字符串">Newer →</a></li> 
    </ul>
  </nav><br>
<!--
  <hr><div id="section-donate"><span>賞</span></div><br>
  <p style="text-indent:0px;text-align:center;">如果喜歡這裡的文章，而且又不差錢的話，歡迎打賞個早餐 ^_^</p><br>
  <div class="row" style="text-align:center;" >
    <div class="col-md-6"><img src="/images/system/barcode-pay-alipay.jpg" style="width:150px;height:150px;" title="支付寶捐贈" /><br>支付寶打賞</div>
    <div class="col-md-6"><img src="/images/system/barcode-pay-wechat.jpg" style="width:150px;height:150px;" title="微信捐贈" /><br>微信打賞</div>
  </div><br><hr>
-->
  <p style="text-indent:0px;text-align:center;">如果喜歡這裡的文章，而且又不差錢的話，歡迎打賞個早餐 ^_^</p>
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
      <a id="btn_donate" class="btn_donate" target="_self" href="javascript:;" title="Donate 打賞"></a>
    </div>
    <div id="donate_guide" class="donate_bar center hidden">
      <div class="row" style="text-align:center;">
        <div class="col-md-6"><img src="/images/system/barcode-pay-alipay.jpg" style="width:150px;height:150px;" title="支付寶捐贈"><br>支付寶打賞</div>
        <div class="col-md-6"><img src="/images/system/barcode-pay-wechat.jpg" style="width:150px;height:150px;" title="微信捐贈"><br>微信打賞</div>
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

  <!-- 多說評論框 start -->
  <!--<div class="ds-thread" data-thread-key="請將此處替換成文章在你的站點中的ID" data-title="請替換成文章的標題" data-url="請替換成文章的網址"></div>-->
  <!--
  <div class="ds-thread" data-thread-key="https://jin-yang.github.io/post/program-c-load-process.html" data-title="C 加載過程" data-url="https://jin-yang.github.io/post/program-c-load-process.html"></div>
  -->
  <!-- 多說評論框 end -->
  <!-- 多說公共JS代碼 start (一個網頁只需插入一次) -->
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
  <!-- 多說公共JS代碼 end -->
</div>
