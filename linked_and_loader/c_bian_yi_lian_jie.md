
<div class="post-container">
  <div class="blog-header">
    <h1>C 編譯鏈接</h1>
    <div class="post-description">
      <i class="fa fa-calendar"></i> 2014-08-26 Tuesday &nbsp; &nbsp;
      <i class="fa fa-tags"></i>  program ,  misc  
      
    </div>
  </div>
  <hr>
  <p>詳細介紹下與 C 語言相關的概念。</p>

<!-- more -->

<h2 id="目標文件">目標文件</h2>

<p>目標文件是編譯器編譯完後，還沒有進行鏈接的文件，通常採用和可執行文件相同的存儲格式，如 Windows 平臺下的 <code class="highlighter-rouge">Portable Executable, PE</code>，Linux 平臺下的 <code class="highlighter-rouge">Executable Linkale Format, ELF</code>，它們都是 <code class="highlighter-rouge">Common File Format, COFF</code> 的變種。</p>

<p>除可執行文件，動態鏈接庫 <code class="highlighter-rouge">(Dynamic Linking Library)</code> 和靜態鏈接庫 <code class="highlighter-rouge">(Statci Linking Library)</code> 也是按照可執行文件進行存儲。</p>

<p>對於文件的類型可以通過 file 命令進行查看，常見的類型包括了：</p>

<ul>
  <li>可重定位文件(Relocatable File)<br>主要包含了代碼和數據，主要用來鏈接成可執行文件或共享目標文件，如 <code class="highlighter-rouge">.o</code> 文件。</li>
  <li>可執行文件(Executable File)<br>主要是可以直接執行的程序，如 <code class="highlighter-rouge">/bin/bash</code> 。</li>
  <li>共享目標文件(Shared Object File)<br>包含了代碼和數據，常見的有動態和靜態鏈接庫，如 <code class="highlighter-rouge">/lib64/libc-2.17.so</code> 。</li>
  <li>核心轉儲文件(Core Dump File)<br>進程意外終止時，系統將該進程的地址空間的內容及終止時的一些其他信息轉儲到該文件中。</li>
</ul>

<h3 id="示例程序">示例程序</h3>

<p>目標文件通過段 (Segment) 進行存儲，在 Windows 中可以通過 <code class="highlighter-rouge">Process Explorer</code> 查看進程相關信息，Linux 可以通過 <code class="highlighter-rouge">objdump</code> 查看。主要的段包括了 <code class="highlighter-rouge">.text</code> <code class="highlighter-rouge">.data</code> <code class="highlighter-rouge">.bss(Block Started by Symbol)</code>，當然還有一些其它段，如 <code class="highlighter-rouge">.rodata</code> <code class="highlighter-rouge">.comment</code> 等。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="kt">int</span> <span class="n">printf</span><span class="p">(</span><span class="k">const</span> <span class="kt">char</span><span class="o">*</span> <span class="n">format</span><span class="p">,</span> <span class="p">...);</span>

<span class="kt">int</span> <span class="n">global_init_var</span> <span class="o">=</span> <span class="mi">84</span><span class="p">;</span>
<span class="kt">int</span> <span class="n">global_uninit_var</span><span class="p">;</span>

<span class="kt">void</span> <span class="nf">func1</span><span class="p">(</span><span class="kt">int</span> <span class="n">i</span><span class="p">)</span>
<span class="p">{</span>
    <span class="n">printf</span><span class="p">(</span><span class="s">"%d</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span> <span class="n">i</span><span class="p">);</span>
<span class="p">}</span>

<span class="kt">int</span> <span class="nf">main</span><span class="p">(</span><span class="kt">void</span><span class="p">)</span>
<span class="p">{</span>
    <span class="k">static</span> <span class="kt">int</span> <span class="n">static_var</span> <span class="o">=</span> <span class="mi">85</span><span class="p">;</span>
    <span class="k">static</span> <span class="kt">int</span> <span class="n">static_var2</span><span class="p">;</span>

    <span class="kt">int</span> <span class="n">a</span> <span class="o">=</span> <span class="mi">1</span><span class="p">;</span>
    <span class="kt">int</span> <span class="n">b</span><span class="p">;</span>

    <span class="n">func1</span><span class="p">(</span><span class="n">static_var</span> <span class="o">+</span> <span class="n">static_var2</span> <span class="o">+</span> <span class="n">a</span> <span class="o">+</span> <span class="n">b</span><span class="p">);</span>

    <span class="k">return</span> <span class="n">a</span><span class="p">;</span>
<span class="p">}</span></code></pre></figure>

<p>然後，可以通過 <code class="highlighter-rouge">gcc -c section.c</code> 編譯，編譯完上述的文件之後，可以通過 <code class="highlighter-rouge">objdump -h</code> 查看頭部信息，也可以通過 <code class="highlighter-rouge">-x</code> 參數查看更詳細的信息。</p>

<p>比較重要的是 <code class="highlighter-rouge">File off</code> 和 <code class="highlighter-rouge">Size</code> 信息，一般頭部信息的大小為 <code class="highlighter-rouge">0x34</code> ，因此第一個段的地址就會從 <code class="highlighter-rouge">0x34</code> 開始 (地址從 0 開始計數)，另外，由於需要 4bytes 對齊，因此會從 <code class="highlighter-rouge">54(0x36)</code> 開始。也可以通過 size 查看，採用的是十進制，最後會用十進制和十六進製表示總的大小。</p>

<p>數據段 <code class="highlighter-rouge">.data</code> 用來保存已經初始化了的全局變量和局部靜態變量，如上述的 <code class="highlighter-rouge">global_init_var</code> 和 <code class="highlighter-rouge">static_var</code> 。</p>

<p>只讀數據段 <code class="highlighter-rouge">.rodata</code> ，主要用於保存常量，如 <code class="highlighter-rouge">printf()</code> 中的字符串和 <code class="highlighter-rouge">const</code> 類型的變量，該段在加載時也會將其設置為只讀。</p>

<p><code class="highlighter-rouge">BSS</code> 段保存了未初始化的全局變量和局部靜態變量，如上述 <code class="highlighter-rouge">global_uninit_var</code> 和 <code class="highlighter-rouge">static_var2</code> 。</p>

<!--
正常應該是 8 字節，但是查看時只有 4 字節，通過符號表(Symbol Table)可以看到，只有 static_var2 保存在 .bss 段，而 global_uninit_var 未存放在任何段，只是一個未定義的 COMMON 符號。這與不同的語言和編譯器實現有關，有些編譯器會將全局的為初始化變量存放在目標文件 .bss 段，有些則不存放，只是預留一個未定義的全局變量符號，等到最終鏈接成可執行文件時再在 .bss 段分配空間。
-->

<p><code class="highlighter-rouge">.text</code> 為代碼段，<code class="highlighter-rouge">.data</code> 保存含初始值的變量，<code class="highlighter-rouge">.bss</code> 只保存了變量的符號。</p>

<h3 id="添加一個段">添加一個段</h3>

<p>將以個二進制文件，如圖片、MP3 音樂等作為目標文件的一個段。如下所示，此時可以直接聲明 <code class="highlighter-rouge">_binary_example_png_start</code> 和 <code class="highlighter-rouge">_binary_example_png_end</code> 並使用。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ objcopy -I binary -O elf32-i386 -B i386 example.png image.o
$ objdump -ht image.o

image.o:     file format elf32-i386

Sections:
Idx Name          Size      VMA       LMA       File off  Algn
  0 .data         000293d6  00000000  00000000  00000034  2**0
                  CONTENTS, ALLOC, LOAD, DATA
SYMBOL TABLE:
00000000 l    d  .data	00000000 .data
00000000 g       .data	00000000 _binary_example_png_start
000293d6 g       .data	00000000 _binary_example_png_end
000293d6 g       *ABS*	00000000 _binary_example_png_size</code></pre></figure>

<p>如果在編譯時想將某個函數或者變量放置在一個段裡，可以通過如下的方式進行。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="n">__attribute__</span><span class="p">((</span><span class="n">section</span><span class="p">(</span><span class="s">"FOO"</span><span class="p">)))</span> <span class="kt">int</span> <span class="n">global</span> <span class="o">=</span> <span class="mi">42</span><span class="p">;</span>
<span class="n">__attribute__</span><span class="p">((</span><span class="n">section</span><span class="p">(</span><span class="s">"BAR"</span><span class="p">)))</span> <span class="kt">void</span> <span class="n">foo</span><span class="p">()</span> <span class="p">{</span> <span class="p">}</span></code></pre></figure>

<h3 id="運行庫">運行庫</h3>

<p>在 <code class="highlighter-rouge">main()</code> 運行之前通常會先執行一段代碼，運行這些代碼的函數稱為 <strong>入口函數</strong> 或 <strong>入口點</strong> ，大致的步驟如下：</p>

<ul>
  <li>操作系統創建進程後，把控制權交給程序入口，這個入口往往是運行庫中的某個入口函數。</li>
  <li>入口函數對運行庫和程序運行環境進行初始化，包括堆、I/O、線程、全局變量構造等。</li>
  <li>入口函數在完成初始化之後，調用 main 函數，正式開始執行程序主體部分。</li>
  <li><code class="highlighter-rouge">main()</code> 執行完後，返回到入口函數，入口函數進行清理工作，包括全局變量析構、堆銷燬、關閉 IO 等，然後進行系統調用結束進程。</li>
</ul>

<h2 id="鏈接過程">鏈接過程</h2>

<p>在程序由源碼到可執行文件的編譯過程實際有預處理 (Propressing)、編譯 (Compilation)、彙編 (Assembly) 和鏈接 (Linking) 四步，在 <code class="highlighter-rouge">gcc</code> 中分別使用 <code class="highlighter-rouge">ccp</code>，<code class="highlighter-rouge">cc1</code>，<code class="highlighter-rouge">as</code>，<code class="highlighter-rouge">ld</code> 來完成。</p>

<p>關於鏈接方面可以直接從網上搜索 《linker and loader》。</p>

<p><img src="https://jin-yang.github.io/images/linux/compile-link-gcc-details.jpg" alt="compile link gcc details" title="compile link gcc details" class="pull-center"></p>

<h3 id="預編譯">預編譯</h3>

<p>將源代碼和頭文件通過預編譯成一個 <code class="highlighter-rouge">.i</code> 文件，相當與如下命令。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ gcc -E main.c -o main.i          # C
$ cpp main.c &gt; main.i              # CPP</code></pre></figure>

<p>與編譯主要是處理源碼中以 <code class="highlighter-rouge">"#"</code> 開始的與編譯指令，主要的處理規則是：</p>

<ul>
  <li>刪除所有的 <code class="highlighter-rouge">"#define"</code> ，並且展開所有的宏定義。</li>
  <li>處理所有條件預編譯指令，比如 <code class="highlighter-rouge">"#if"</code>、<code class="highlighter-rouge">"#ifdef"</code>、<code class="highlighter-rouge">"#elif"</code>、<code class="highlighter-rouge">"#else"</code>、<code class="highlighter-rouge">"#endif"</code> 。</li>
  <li>處理 <code class="highlighter-rouge">"#include"</code> ，將被包含的文件插入到該預編譯指令的位置，該過程是遞歸的。</li>
  <li>刪除多有的註釋 <code class="highlighter-rouge">"//"</code> 和 <code class="highlighter-rouge">"/* */"</code> 。</li>
  <li>添加行號和文件名標識，如 <code class="highlighter-rouge">#2 "main.c" 2</code> ，用於編譯時產生調試用的行號以及在編譯時產生錯誤或警告時顯示行號。</li>
  <li>保留所有的 <code class="highlighter-rouge">"#pragma"</code> 編譯器指令，因為編譯器需要使用它們。</li>
</ul>

<p>經過預編譯後的 <code class="highlighter-rouge">.i</code> 文件不包含任何宏定義，因為所有的宏已經被展開，並且包含的文件也已經被插入到 <code class="highlighter-rouge">.i</code> 文件中。所以，當無法判斷宏定義是否正確或頭文件包含是否正確時，可以查看該文件。</p>

<h3 id="編譯">編譯</h3>

<p>編譯過程就是把預處理後的文件進行一系列的詞法分析、語法分析、語義分析以及優化後生成相應的彙編代碼文件，這個是核心部分，也是最複雜的部分。</p>

<p>gcc 把預編譯和編譯合併成一個步驟，對於 C 語言使用的是 <code class="highlighter-rouge">cc1</code> ，C++ 使用的是 <code class="highlighter-rouge">cc1obj</code> 。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ gcc -S hello.i -o hello.s
$ gcc -S main.c -o main.s</code></pre></figure>

<!-- $ /usr/lib/gcc/i386-linux-gnu/4.7/cc1 main.c -->

<h3 id="彙編">彙編</h3>

<p>彙編器是將彙編代碼轉化成機器碼，每條彙編語句幾乎都對應一條機器指令。彙編器不需要複雜的語法語義，也不用進行指令優化，只是根據彙編指令和機器指令的對照表一一翻譯即可。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ gcc -c hello.s -o hello.o
$ as main.s -o main.o
$ gcc -c main.s -o main.o
$ gcc -c main.c -o main.o</code></pre></figure>

<h3 id="鏈接">鏈接</h3>

<p>可以通過 <code class="highlighter-rouge">gcc hello.c -o hello -v</code> 查看。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ gcc hello.o -o hello.exe</code></pre></figure>

<h2 id="靜態庫和動態庫">靜態庫和動態庫</h2>

<p>庫有動態與靜態兩種，Linux 中動態通常用 <code class="highlighter-rouge">.so</code> 為後綴，靜態用 <code class="highlighter-rouge">.a</code> 為後綴，如：<code class="highlighter-rouge">libhello.so</code> <code class="highlighter-rouge">libhello.a</code>。為了在同一系統中使用不同版本的庫，可以在庫文件名後加上版本號為後綴，例如：<code class="highlighter-rouge">libhello.so.1.0</code>，然後，使用時通過符號鏈接指向不同版本。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text"># ln -s libhello.so.1.0 libhello.so.1
# ln -s libhello.so.1 libhello.so</code></pre></figure>

<p>在動態鏈接的情況下，可執行文件中還有很多外部符號的引用還處於無效地址的狀態，因此需要首先啟一個 <strong>動態鏈接器 (Dynamic Linker)</strong>，該連接器的位置在程序的 <code class="highlighter-rouge">".interp"</code> (interpreter) 中指定，可以通過如下的命令查詢。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ readelf -l a.out | grep interpreter</code></pre></figure>

<h3 id="共享庫的更新">共享庫的更新</h3>

<p>對於共享庫更新時通常會有兼容更新和不兼容更新，此處所說的兼容是指二進制接口，即 <code class="highlighter-rouge">ABI (Application Binary Interface)</code>。</p>

<p>為了保證共享庫的兼容性， Linux 採用一套規則來命名系統中的共享庫，簡單來說，其規則如下 <code class="highlighter-rouge">libname.so.x.y.z</code>，name 為庫的名稱，x y z 的含義如下：</p>

<ul>
  <li>x，主版本號(Major Version Number)，庫的重大升級，不同的主版本號之間不兼容。</li>
  <li>y，次版本號(Minor Version Number)，庫的增量升級，增加了一些新的接口，且保持原來的符號不變。</li>
  <li>z，發佈版本號(Release Version Number)，庫的一些錯誤的修正、性能的改進等，並不添加任何新的接口，也不對接口進行改進。</li>
</ul>

<p>由於歷史的原因最基本的 C 語言庫 glibc 動態鏈接庫不使用這種規則，如 <code class="highlighter-rouge">libc-x.y.z.so</code> 、<code class="highlighter-rouge">ld-x.y.z.so</code> 。</p>

<p>下面這篇論文， Library Interface Versioning in Solaris and Linux ，對 Salaris 和 Linux 的共享庫版本機制和符號版本機製做了非常詳細的介紹。</p>

<p>在 Linux 中採用 SO-NAME 的命名機制，每個庫會對應一個 SO-NAME ，這個 SO-NAME 只保留主版本號，也即 SO-NAME 規定了共享庫的接口。</p>

<h3 id="路徑問題">路徑問題</h3>

<p>如果動態庫不在搜索路徑中，則會報 <code class="highlighter-rouge">cannot open shared object file: No such file or directory</code> 的錯誤。可以通過 <code class="highlighter-rouge">gcc --print-search-dirs</code> 命令查看默認的搜索路徑。</p>

<p>查找順序通常為：</p>

<ol>
  <li>查找程序編譯指定的路徑，保存在 <code class="highlighter-rouge">.dynstr</code> 段，其中包含了一個以冒號分割的目錄搜索列表。</li>
  <li>查找環境變量 <code class="highlighter-rouge">LD_LIBRARY_PATH</code>，以冒號分割的目錄搜索列表。</li>
  <li>查找 <code class="highlighter-rouge">/etc/ld.so.conf</code> 。</li>
  <li>默認路徑 <code class="highlighter-rouge">/lib</code> 和 <code class="highlighter-rouge">/usr/lib</code> 。</li>
</ol>

<p>為了讓執行程序順利找到動態庫，有三種方法：</p>

<h5 id="1-複製到指定路徑">1. 複製到指定路徑</h5>

<p>把庫拷貝到查找路徑下，通常為 <code class="highlighter-rouge">/usr/lib</code> 和 <code class="highlighter-rouge">/lib</code> 目錄下，或者通過 <code class="highlighter-rouge">gcc --print-search-dirs</code> 查看動態庫的搜索路徑。</p>

<h5 id="2-添加鏈接選項">2. 添加鏈接選項</h5>

<p>編譯時添加鏈接選項，指定鏈接庫的目錄，此時會將該路徑保存在二進制文件中。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ gcc -o test test.o -L. -lhello -Wl,-rpath,/home/lib:.
$ readelf -d test | grep RPATH
$ objdump -s -j .dynstr test                     // 查看.dynstr段的內容</code></pre></figure>

<h5 id="3-設置環境變量">3. 設置環境變量</h5>

<p>執行時在 <code class="highlighter-rouge">LD_LIBRARY_PATH</code> 環境變量中加上庫所在路徑，例如動態庫 <code class="highlighter-rouge">libhello.so</code> 在 <code class="highlighter-rouge">/home/test/lib</code> 目錄下。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/test/lib</code></pre></figure>

<h5 id="4-修改配置文件">4. 修改配置文件</h5>

<p>修改 <code class="highlighter-rouge">/etc/ld.so.conf</code> 文件，把庫所在的路徑加到文件中，並執行 <code class="highlighter-rouge">ldconfig</code> 刷新配置。動態鏈接庫通常保存在 <code class="highlighter-rouge">/etc/ld.so.cache</code> 文件中，執行 <code class="highlighter-rouge">ldconfig</code> 可以對其進行刷新。</p>

<h3 id="靜態連接庫">靜態連接庫</h3>

<p>當要使用靜態的程序庫時，連接器會找出程序所需的函數，然後將它們拷貝到執行文件，由於這種拷貝是完整的，所以一旦連接成功，靜態程序庫也就不再需要了，缺點是佔用的空間比較大。通常，靜態鏈接的程序要比共享函數庫的程序運行速度上快一些，大概 1-5％ 。</p>

<!--
動態庫會在執行程序內留下一個標記指明當程序執行時，首先必須載入這個庫。
-->

<p>注意，對於 CentOS 需要安裝 <code class="highlighter-rouge">yum install glibc-static</code> 庫。</p>

<p>Linux 下進行連接的缺省操作是首先連接動態庫，也就是說，如果同時存在靜態和動態庫，不特別指定的話，將與動態庫相連接。</p>

<p>現在假設有一個 hello 程序開發包，它提供一個靜態庫 <code class="highlighter-rouge">libhello.a</code>，一個動態庫 <code class="highlighter-rouge">libhello.so</code>，一個頭文件 <code class="highlighter-rouge">hello.h</code>，頭文件中提供 <code class="highlighter-rouge">foobar()</code> 這個函數的聲明。</p>

<p>下面這段程序 <code class="highlighter-rouge">main.c</code> 使用 hello 庫中的 <code class="highlighter-rouge">foobar()</code> 函數。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cm">/* filename: foobar.c */</span>
<span class="cp">#include "hello.h"
#include &lt;stdio.h&gt;
</span><span class="kt">void</span> <span class="nf">foobar</span><span class="p">()</span>
<span class="p">{</span>
   <span class="n">printf</span><span class="p">(</span><span class="s">"FooBar!</span><span class="se">\n</span><span class="s">"</span><span class="p">);</span>
<span class="p">}</span></code></pre></figure>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cm">/* filename: hello.c */</span>
<span class="cp">#include "hello.h"
#include &lt;stdio.h&gt;
</span><span class="kt">void</span> <span class="nf">hello</span><span class="p">()</span>
<span class="p">{</span>
   <span class="n">printf</span><span class="p">(</span><span class="s">"Hello world!</span><span class="se">\n</span><span class="s">"</span><span class="p">);</span>
<span class="p">}</span></code></pre></figure>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cm">/* filename: hello.h */</span>
<span class="cp">#ifndef _HELLO_H__
#define _HELLO_H__
</span><span class="kt">void</span> <span class="n">hello</span><span class="p">();</span>
<span class="kt">void</span> <span class="n">foobar</span><span class="p">();</span>
<span class="cp">#endif</span></code></pre></figure>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cm">/* filename: main.c */</span>
<span class="cp">#include "hello.h"
</span><span class="kt">int</span> <span class="nf">main</span><span class="p">(</span><span class="kt">int</span> <span class="n">argc</span><span class="p">,</span> <span class="kt">char</span> <span class="o">**</span><span class="n">argv</span><span class="p">)</span>
<span class="p">{</span>
   <span class="n">foobar</span><span class="p">();</span>
   <span class="n">hello</span><span class="p">();</span>
   <span class="k">return</span> <span class="mi">0</span><span class="p">;</span>
<span class="p">}</span></code></pre></figure>

<p>生成靜態庫，先對源文件進行編譯；然後使用 <code class="highlighter-rouge">ar(archive)</code> 命令連接成靜態庫。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ gcc -c hello.c -o hello.o
$ gcc -c foobar.c -o foobar.o
$ ar crv libhello.a hello.o foobar.o
$ ar -t libhello.a                              // 查看打包的文件</code></pre></figure>

<p><code class="highlighter-rouge">ar</code> 實際是一個打包工具，可以用來打包常見文件，不過現在被 <code class="highlighter-rouge">tar</code> 替代，目前主要是用於生成靜態庫，詳細格式可以參考 <a href="http://en.wikipedia.org/wiki/Ar_(Unix)">ar(Unix) wiki</a> 。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ echo "hello" &gt; a.txt &amp;&amp; echo "world" &gt; b.txt
$ ar crv text.a a.txt b.txt
$ cat text.a</code></pre></figure>

<p>與靜態庫連接麻煩一些，主要是參數問題。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ gcc main.c -o test -lhello                    // 庫在默認路徑下，如/usr/lib
$ gcc main.c -lhello -L. -static -o main        // 通過-L指定庫的路徑

$ gcc main.o -o main -WI,-Bstatic -lhello       // 報錯，顯示找不到-lgcc_s</code></pre></figure>

<p>注意：這個特別的 <code class="highlighter-rouge">"-WI,-Bstatic"</code> 參數，實際上是傳給了連接器 <code class="highlighter-rouge">ld</code>，指示它與靜態庫連接，如果系統中只有靜態庫可以不需要這個參數； 如果要和多個庫相連接，而每個庫的連接方式不一樣，比如上面的程序既要和 <code class="highlighter-rouge">libhello</code> 進行靜態連接，又要和 <code class="highlighter-rouge">libbye</code> 進行動態連接，其命令應為：</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ gcc testlib.o -o test -WI,-Bstatic -lhello -WI,-Bdynamic -lbye</code></pre></figure>

<p>最好不要進行分別編譯、鏈接，因為在生成可執行文件時往往需要很多的其他文件，可以通過 <code class="highlighter-rouge">-v</code> 選項進行查看，如果通過如下方式進行編譯通常會出現錯誤。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ gcc -c main.c
$ ld main.o -L. -lhello</code></pre></figure>

<h3 id="動態庫">動態庫</h3>

<p>用 gcc 編繹該文件，在編繹時可以使用任何編繹參數，例如 <code class="highlighter-rouge">-g</code> 加入調試代碼等；<code class="highlighter-rouge">-fPIC</code> 生成與位置無關的代碼（可以在任何地址被連接和裝載）。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ gcc -c -fPIC hello.c -o hello.o

$ gcc -shared -Wl,-soname,libhello.so.1 -o libhello.so.1.0 hello.o // 生成動態庫，可能存在多個版本，通常指定版本號

$ ln  -s  libhello.so.1.0  libhello.so.1                           // 另外再建立兩個符號連接
$ ln  -s  libhello.so.1  libhello.so

$ gcc -fPIC -shared -o libhello.so hello.c                         // 當然對於上述的步驟可以通過一步完成

$ readelf -d libhello.so.1.0 | grep SONAME                         // 查看對應的soname
$ nm -D libhello.so                                                // 查看符號</code></pre></figure>

<p>最重要的是傳 <code class="highlighter-rouge">-shared</code> 參數使其生成是動態庫而不是普通執行程序； <code class="highlighter-rouge">-Wl</code> 表示後面的參數也就是 <code class="highlighter-rouge">-soname,libhello.so.1</code> 直接傳給連接器 <code class="highlighter-rouge">ld</code> 進行處理。</p>

<p>實際上，每一個庫都有一個 <code class="highlighter-rouge">soname</code> ，當連接器發現它正在查找的程序庫中有這樣一個名稱，連接器便會將 <code class="highlighter-rouge">soname</code> 嵌入連結中的二進制文件內，而不是它正在運行的實際文件名，在程序執行期間，程序會查找擁有 <code class="highlighter-rouge">soname</code> 名字的文件，而不是庫的文件名，換句話說，<code class="highlighter-rouge">soname</code> 是庫的區分標誌。</p>

<p>其目的主要是允許系統中多個版本的庫文件共存，習慣上在命名庫文件的時候通常與 <code class="highlighter-rouge">soname</code> 相同 <code class="highlighter-rouge">libxxxx.so.major.minor</code> 其中，<code class="highlighter-rouge">xxxx</code> 是庫的名字， <code class="highlighter-rouge">major</code> 是主版本號， <code class="highlighter-rouge">minor</code> 是次版本號。</p>

<h3 id="查看庫中的符號">查看庫中的符號</h3>

<p>有時候可能需要查看一個庫中到底有哪些函數，<code class="highlighter-rouge">nm</code> 命令可以打印出庫中的涉及到的所有符號，庫既可以是靜態的也可以是動態的。</p>

<p><code class="highlighter-rouge">nm</code> 列出的符號有很多，常見的有三種：</p>

<ul>
  <li>在庫中被調用，但並沒有在庫中定義(表明需要其他庫支持)，用U表示；</li>
  <li>庫中定義的函數，用T表示，這是最常見的；</li>
  <li>所謂的“弱態”符號，它們雖然在庫中被定義，但是可能被其他庫中的同名符號覆蓋，用W表示。</li>
</ul>

<p>例如，希望知道上文提到的 <code class="highlighter-rouge">hello</code> 庫中是否定義了 <code class="highlighter-rouge">printf()</code> 。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ nm libhello.so</code></pre></figure>

<p>發現其中沒有 <code class="highlighter-rouge">printf()</code> 的定義，取而代之的是 <code class="highlighter-rouge">puts()</code> 函數，而且為 <code class="highlighter-rouge">U</code> ，表示符號 <code class="highlighter-rouge">puts</code> 被引用，但是並沒有在函數內定義，由此可以推斷，要正常使用 <code class="highlighter-rouge">hello</code> 庫，必須有其它庫支持，再使用 <code class="highlighter-rouge">ldd</code> 命令查看 <code class="highlighter-rouge">hello</code> 依賴於哪些庫：</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ ldd -v hello
$ readelf -d hello     直接使用readelf</code></pre></figure>

<p>每行 <code class="highlighter-rouge">=&gt;</code> 前面的，為動態鏈接程序所需的動態鏈接庫的名字；而 <code class="highlighter-rouge">=&gt;</code> 後面的，則是運行時系統實際調用的動態鏈接庫的名字。所需的動態鏈接庫在系統中不存在時，<code class="highlighter-rouge">=&gt;</code> 後面將顯示 <code class="highlighter-rouge">"not found"</code>，括號所括的數字為虛擬的執行地址。</p>

<p>常用的系統動態鏈接庫有：</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">libc.so.x        基本C庫
ld-linux.so.2    動態裝入庫(用於動態鏈接庫的裝入及運行)</code></pre></figure>

<h2 id="動態庫加載api">動態庫加載API</h2>

<p>對於 Linux 下的可執行文件 ELF 使用如下命令查看，可以發現其中有一個 <code class="highlighter-rouge">.interp</code> 段，它指明瞭將要被使用的動態鏈接器 (<code class="highlighter-rouge">/lib/ld-linux.so</code>)。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ readelf -l EXECUTABLE</code></pre></figure>

<p>對於動態加載的函數主要包括了下面的四個函數，需要 <code class="highlighter-rouge">dlfcn.h</code> 頭文件，定義在 <code class="highlighter-rouge">libdl.so</code> 庫中。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">void *dlopen( const char *file, int mode );
  用來打開一個文件，使對象文件可被程序訪問，同時還會自動解析共享庫中的依賴項，這樣，如果打開了一個
    依賴於其他共享庫的對象，它就會自動加載它們，該函數返回一個句柄，該句柄用於後續的 API 調用。
  mode 參數通知動態鏈接器何時執行再定位，有兩個可能的值：
    A) RTLD_NOW，表明動態鏈接器將會在調用 dlopen 時完成所有必要的再定位；
    B) RTLD_LAZY，只在需要時執行再定位。

void *dlsym( void *restrict handle, const char *restrict name );
  通過句柄和連接符名稱獲取函數名或者變量名。

char *dlerror();
  返回一個可讀的錯誤字符串，該函數沒有參數，它會在發生前面的錯誤時返回一個字符串，在沒有錯誤發生時返回NULL。

char *dlclose( void *handle ); 
  通知操作系統不再需要句柄和對象引用了。它完全是按引用來計數的，所以同一個共享對象的多個用戶相互間
    不會發生衝突（只要還有一個用戶在使用它，它就會待在內存中）。
    任何通過已關閉的對象的 dlsym 解析的符號都將不再可用。</code></pre></figure>

<p>有了 ELF 對象的句柄，就可以通過調用 dlsym 來識別這個對象內的符號的地址了。該函數採用一個符號名稱，如對象內的一個函數的名稱，返回值為對象符號的解析地址。</p>

<p>下面是一個動態加載的示例 <a href="https://github.com/Jin-Yang/examples/tree/master/c_cpp/c/libdl.c">github libdl.c</a>，通過如下的命令進行編譯，其中選項 <code class="highlighter-rouge">-rdynamic</code> 用來通知鏈接器將所有符號添加到動態符號表中（目的是能夠通過使用 dlopen 來實現向後跟蹤）。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ gcc -rdynamic -o dl library_libdl.c -ldl        # 編譯
$ ./dl                                            # 測試
&gt; libm.so cosf 0.0
   1.000000
&gt; libm.so sinf 0.0
   0.000000
&gt; libm.so tanf 1.0
   1.557408
&gt; bye</code></pre></figure>

<p>另外，可以通過如下方式簡單使用。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ cat caculate.c                                     # 查看動態庫源碼
int add(int a, int b) {
    return (a + b);
}
int sub(int a, int b) {
    return (a - b);
}
$ gcc -fPIC -shared caculate.c -o libcaculate.so     # 生成動態庫
$ cat foobar.c                                       # 測試源碼
#include &lt;stdio.h&gt;
#include &lt;dlfcn.h&gt;
#include &lt;stdlib.h&gt;

typedef int (*CAC_FUNC)(int, int);                           // 定義函數指針類型
int main(int argc, char** argv) {
    void *handle;
    char *error;
    CAC_FUNC cac_func = NULL;

    if ( !(handle=dlopen("./libcaculate.so", RTLD_LAZY)) ) { // 打開動態鏈接庫
        fprintf(stderr, "!!! %s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    cac_func = dlsym(handle, "add");                         // 獲取一個函數
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "!!! %s\n", error);
        exit(EXIT_FAILURE);
    }
    printf("add: %d\n", (cac_func)(2,7));

    dlclose(handle);                                         // 關閉動態鏈接庫
    exit(EXIT_SUCCESS);
}
$ gcc -rdynamic -o foobar foobar.c -ldl              # 編譯測試</code></pre></figure>

<!-- https://www.ibm.com/developerworks/cn/linux/l-elf/part1/index.html -->

<h2 id="常用命令">常用命令</h2>

<h3 id="objdump">objdump</h3>

<p>詳細參考 <code class="highlighter-rouge">man objdump</code> 。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">-h, --section-headers, --headers
  查看目標文件的頭部信息。
-x, --all-headers
  顯示所有的頭部信息，包括了符號表和重定位表，等價於 -a -f -h -p -r -t 。
-s, --full-contents
  顯示所請求段的全部信息，通常用十六進製表示，默認只會顯示非空段。
-d, --disassemble
  反彙編，一般只反彙編含有指令的段。
-t, --syms
  顯示符號表，與nm類似，只是顯示的格式不同，當然顯示與文件的格式相關，對於ELF如下所示。
  00000000 l    d  .bss   00000000 .bss
  00000000 g       .text  00000000 fred</code></pre></figure>

<!--
第一列為符號的值，有時是地址；下一個是用字符表示的標誌位；接著是與符號相關的段，*ABS* 表示段是絕對的（沒和任何段相關聯）， *UND* 表示未定義；對於普通符號(Common Symbols)表示對齊，其它的表示大小；最後是符號的名字。<br><br>

對於標誌組的字符被分為如下的 7 組。
<ol type="A"><li>
    "l(local)" "g(global)" "u(unique global)" " (neither global nor local)" "!(both global and local)"<br>
    通常一個符號應該是 local 或 global ，但還有其他的一些原因，如用於調試、"!"表示一個bug、"u"是 ELF 的擴展，表示整個進程中只有一個同類型同名的變量。</li><br><li>

    "w(weak)" " (strong)"<br>
    表示強或弱符號。</li><br><li>

    "C(constructor)" " (ordinary)"<br>
    為構造函數還是普通符號。</li><br><li>

    "W(warning)" " (normal symbol)"<br>
    如果一個含有警告標誌的符號被引用時，將會輸出警告信息。</li><br><li>

    "I"
   "i" The symbol is an indirect reference to another symbol (I), a function to be evaluated
       during reloc processing (i) or a normal symbol (a space).

   "d(debugging symbol)" "D(dynamic symbol)" " (normal symbol)"<br>
    表示調試符號、動態符號還是普通的符號。</li><br><li>

   "F(function)" "f(file)" "O(object)" " (normal)"<br>
    表示函數、文件、對象或只是一個普通的符號。
-->

<h3 id="strip">strip</h3>

<p>我們知道二進制的程序中包含了大量的符號表格(symbol table)，有一部分是用來 gdb 調試提供必要信息的，可以通過如下命令查看這些符號信息。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ readelf -S hello</code></pre></figure>

<p>其中類似與 <code class="highlighter-rouge">.debug_xxxx</code> 的就是 gdb 調試用的。去掉它們不會影響程序的執行。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ strip hello</code></pre></figure>

<h3 id="objcopy">objcopy</h3>

<p>用於轉換目標文件。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">常用參數：
  -S / --strip-all
    不從源文件中拷貝重定位信息和符號信息到輸出文件(目的文件)中去。
  -I bfdname/--input-target=bfdname
    明確告訴程序源文件的格式是什麼，bfdname是BFD庫中描述的標準格式名。
  -O bfdname/--output-target=bfdname
    使用指定的格式來寫輸出文件(即目標文件)，bfdname是BFD庫中描述的標準格式名，
    如binary(raw binary 格式)、srec(s-record 文件)。
  -R sectionname/--remove-section=sectionname
    從輸出文件中刪掉所有名為section-name的段。</code></pre></figure>

<p>上一步的 strip 命令只能拿掉一般 symbol table，有些信息還是沒拿掉，而這些信息對於程序的最終執行沒有影響，如: <code class="highlighter-rouge">.comment</code> <code class="highlighter-rouge">.note.ABI-tag</code> <code class="highlighter-rouge">.gnu.version</code> 就是完全可以去掉的。</p>

<p>所以說程序還有簡化的餘地，我們可以使用 objcopy 命令把它們抽取掉。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ objcopy -R .comment -R .note.ABI-tag -R .gnu.version hello hello1</code></pre></figure>

<h3 id="readelf">readelf</h3>

<p>用於讀取 ELF 格式文件，包括可執行程序和動態庫。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">常用參數：
  -a --all
    等價於-h -l -S -s -r -d -V -A -I
  -h --file-header
    文件頭信息；
  -l --program-headers
    程序的頭部信息；
  -S --section-headers
    各個段的頭部信息；
  -e --headers
    全部頭信息，等價於-h -l -S；

示例用法：
----- 讀取dynstr段，包含了很多需要加載的符號，每個動態庫後跟著需要加載函數
$ readelf -p .dynstr hello
----- 查看是否含有調試信息
$ readelf -S hello | grep debug</code></pre></figure>

<!--
readelf  -S hello
readelf -d hello

  --sections
An alias for –section-headers
-s –syms 符號表 Display the symbol table
--symbols
An alias for –syms
-n –notes 內核註釋 Display the core notes (if present)
-r –relocs 重定位 Display the relocations (if present)
-u –unwind Display the unwind info (if present)
-d --dynamic
  顯示動態段的內容；
-V –version-info 版本 Display the version sections (if present)
-A –arch-specific CPU構架 Display architecture specific information (if any).
-D –use-dynamic 動態段 Use the dynamic section info when displaying symbols
-x –hex-dump=<number> 顯示 段內內容Dump the contents of section <number>
-w[liaprmfFso] or
-I –histogram Display histogram of bucket list lengths
-W –wide 寬行輸出 Allow output width to exceed 80 characters
-H –help Display this information
-v –version Display the version number of readelf
-->

<h2 id="其它">其它</h2>

<h4 id="1-靜態庫生成動態庫">1. 靜態庫生成動態庫</h4>

<p>可以通過多個靜態庫生成動態庫，而實際上靜態庫是一堆 <code class="highlighter-rouge">.o</code> 庫的壓縮集合，而生成動態庫需要保證 <code class="highlighter-rouge">.o</code> 編譯後是與地址無關的，也就是添加 <code class="highlighter-rouge">-fPIC</code> 參數。</p>



</div>