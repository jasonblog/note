
<div class="post-container">
  <div class="blog-header">
    <h1>C 编译链接</h1>
    <div class="post-description">
      <i class="fa fa-calendar"></i> 2014-08-26 Tuesday &nbsp; &nbsp;
      <i class="fa fa-tags"></i>  program ,  misc  
      
    </div>
  </div>
  <hr>
  <p>详细介绍下与 C 语言相关的概念。</p>

<!-- more -->

<h2 id="目标文件">目标文件</h2>

<p>目标文件是编译器编译完后，还没有进行链接的文件，通常采用和可执行文件相同的存储格式，如 Windows 平台下的 <code class="highlighter-rouge">Portable Executable, PE</code>，Linux 平台下的 <code class="highlighter-rouge">Executable Linkale Format, ELF</code>，它们都是 <code class="highlighter-rouge">Common File Format, COFF</code> 的变种。</p>

<p>除可执行文件，动态链接库 <code class="highlighter-rouge">(Dynamic Linking Library)</code> 和静态链接库 <code class="highlighter-rouge">(Statci Linking Library)</code> 也是按照可执行文件进行存储。</p>

<p>对于文件的类型可以通过 file 命令进行查看，常见的类型包括了：</p>

<ul>
  <li>可重定位文件(Relocatable File)<br>主要包含了代码和数据，主要用来链接成可执行文件或共享目标文件，如 <code class="highlighter-rouge">.o</code> 文件。</li>
  <li>可执行文件(Executable File)<br>主要是可以直接执行的程序，如 <code class="highlighter-rouge">/bin/bash</code> 。</li>
  <li>共享目标文件(Shared Object File)<br>包含了代码和数据，常见的有动态和静态链接库，如 <code class="highlighter-rouge">/lib64/libc-2.17.so</code> 。</li>
  <li>核心转储文件(Core Dump File)<br>进程意外终止时，系统将该进程的地址空间的内容及终止时的一些其他信息转储到该文件中。</li>
</ul>

<h3 id="示例程序">示例程序</h3>

<p>目标文件通过段 (Segment) 进行存储，在 Windows 中可以通过 <code class="highlighter-rouge">Process Explorer</code> 查看进程相关信息，Linux 可以通过 <code class="highlighter-rouge">objdump</code> 查看。主要的段包括了 <code class="highlighter-rouge">.text</code> <code class="highlighter-rouge">.data</code> <code class="highlighter-rouge">.bss(Block Started by Symbol)</code>，当然还有一些其它段，如 <code class="highlighter-rouge">.rodata</code> <code class="highlighter-rouge">.comment</code> 等。</p>

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

<p>然后，可以通过 <code class="highlighter-rouge">gcc -c section.c</code> 编译，编译完上述的文件之后，可以通过 <code class="highlighter-rouge">objdump -h</code> 查看头部信息，也可以通过 <code class="highlighter-rouge">-x</code> 参数查看更详细的信息。</p>

<p>比较重要的是 <code class="highlighter-rouge">File off</code> 和 <code class="highlighter-rouge">Size</code> 信息，一般头部信息的大小为 <code class="highlighter-rouge">0x34</code> ，因此第一个段的地址就会从 <code class="highlighter-rouge">0x34</code> 开始 (地址从 0 开始计数)，另外，由于需要 4bytes 对齐，因此会从 <code class="highlighter-rouge">54(0x36)</code> 开始。也可以通过 size 查看，采用的是十进制，最后会用十进制和十六进制表示总的大小。</p>

<p>数据段 <code class="highlighter-rouge">.data</code> 用来保存已经初始化了的全局变量和局部静态变量，如上述的 <code class="highlighter-rouge">global_init_var</code> 和 <code class="highlighter-rouge">static_var</code> 。</p>

<p>只读数据段 <code class="highlighter-rouge">.rodata</code> ，主要用于保存常量，如 <code class="highlighter-rouge">printf()</code> 中的字符串和 <code class="highlighter-rouge">const</code> 类型的变量，该段在加载时也会将其设置为只读。</p>

<p><code class="highlighter-rouge">BSS</code> 段保存了未初始化的全局变量和局部静态变量，如上述 <code class="highlighter-rouge">global_uninit_var</code> 和 <code class="highlighter-rouge">static_var2</code> 。</p>

<!--
正常应该是 8 字节，但是查看时只有 4 字节，通过符号表(Symbol Table)可以看到，只有 static_var2 保存在 .bss 段，而 global_uninit_var 未存放在任何段，只是一个未定义的 COMMON 符号。这与不同的语言和编译器实现有关，有些编译器会将全局的为初始化变量存放在目标文件 .bss 段，有些则不存放，只是预留一个未定义的全局变量符号，等到最终链接成可执行文件时再在 .bss 段分配空间。
-->

<p><code class="highlighter-rouge">.text</code> 为代码段，<code class="highlighter-rouge">.data</code> 保存含初始值的变量，<code class="highlighter-rouge">.bss</code> 只保存了变量的符号。</p>

<h3 id="添加一个段">添加一个段</h3>

<p>将以个二进制文件，如图片、MP3 音乐等作为目标文件的一个段。如下所示，此时可以直接声明 <code class="highlighter-rouge">_binary_example_png_start</code> 和 <code class="highlighter-rouge">_binary_example_png_end</code> 并使用。</p>

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

<p>如果在编译时想将某个函数或者变量放置在一个段里，可以通过如下的方式进行。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="n">__attribute__</span><span class="p">((</span><span class="n">section</span><span class="p">(</span><span class="s">"FOO"</span><span class="p">)))</span> <span class="kt">int</span> <span class="n">global</span> <span class="o">=</span> <span class="mi">42</span><span class="p">;</span>
<span class="n">__attribute__</span><span class="p">((</span><span class="n">section</span><span class="p">(</span><span class="s">"BAR"</span><span class="p">)))</span> <span class="kt">void</span> <span class="n">foo</span><span class="p">()</span> <span class="p">{</span> <span class="p">}</span></code></pre></figure>

<h3 id="运行库">运行库</h3>

<p>在 <code class="highlighter-rouge">main()</code> 运行之前通常会先执行一段代码，运行这些代码的函数称为 <strong>入口函数</strong> 或 <strong>入口点</strong> ，大致的步骤如下：</p>

<ul>
  <li>操作系统创建进程后，把控制权交给程序入口，这个入口往往是运行库中的某个入口函数。</li>
  <li>入口函数对运行库和程序运行环境进行初始化，包括堆、I/O、线程、全局变量构造等。</li>
  <li>入口函数在完成初始化之后，调用 main 函数，正式开始执行程序主体部分。</li>
  <li><code class="highlighter-rouge">main()</code> 执行完后，返回到入口函数，入口函数进行清理工作，包括全局变量析构、堆销毁、关闭 IO 等，然后进行系统调用结束进程。</li>
</ul>

<h2 id="链接过程">链接过程</h2>

<p>在程序由源码到可执行文件的编译过程实际有预处理 (Propressing)、编译 (Compilation)、汇编 (Assembly) 和链接 (Linking) 四步，在 <code class="highlighter-rouge">gcc</code> 中分别使用 <code class="highlighter-rouge">ccp</code>，<code class="highlighter-rouge">cc1</code>，<code class="highlighter-rouge">as</code>，<code class="highlighter-rouge">ld</code> 来完成。</p>

<p>关于链接方面可以直接从网上搜索 《linker and loader》。</p>

<p><img src="https://jin-yang.github.io/images/linux/compile-link-gcc-details.jpg" alt="compile link gcc details" title="compile link gcc details" class="pull-center"></p>

<h3 id="预编译">预编译</h3>

<p>将源代码和头文件通过预编译成一个 <code class="highlighter-rouge">.i</code> 文件，相当与如下命令。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ gcc -E main.c -o main.i          # C
$ cpp main.c &gt; main.i              # CPP</code></pre></figure>

<p>与编译主要是处理源码中以 <code class="highlighter-rouge">"#"</code> 开始的与编译指令，主要的处理规则是：</p>

<ul>
  <li>删除所有的 <code class="highlighter-rouge">"#define"</code> ，并且展开所有的宏定义。</li>
  <li>处理所有条件预编译指令，比如 <code class="highlighter-rouge">"#if"</code>、<code class="highlighter-rouge">"#ifdef"</code>、<code class="highlighter-rouge">"#elif"</code>、<code class="highlighter-rouge">"#else"</code>、<code class="highlighter-rouge">"#endif"</code> 。</li>
  <li>处理 <code class="highlighter-rouge">"#include"</code> ，将被包含的文件插入到该预编译指令的位置，该过程是递归的。</li>
  <li>删除多有的注释 <code class="highlighter-rouge">"//"</code> 和 <code class="highlighter-rouge">"/* */"</code> 。</li>
  <li>添加行号和文件名标识，如 <code class="highlighter-rouge">#2 "main.c" 2</code> ，用于编译时产生调试用的行号以及在编译时产生错误或警告时显示行号。</li>
  <li>保留所有的 <code class="highlighter-rouge">"#pragma"</code> 编译器指令，因为编译器需要使用它们。</li>
</ul>

<p>经过预编译后的 <code class="highlighter-rouge">.i</code> 文件不包含任何宏定义，因为所有的宏已经被展开，并且包含的文件也已经被插入到 <code class="highlighter-rouge">.i</code> 文件中。所以，当无法判断宏定义是否正确或头文件包含是否正确时，可以查看该文件。</p>

<h3 id="编译">编译</h3>

<p>编译过程就是把预处理后的文件进行一系列的词法分析、语法分析、语义分析以及优化后生成相应的汇编代码文件，这个是核心部分，也是最复杂的部分。</p>

<p>gcc 把预编译和编译合并成一个步骤，对于 C 语言使用的是 <code class="highlighter-rouge">cc1</code> ，C++ 使用的是 <code class="highlighter-rouge">cc1obj</code> 。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ gcc -S hello.i -o hello.s
$ gcc -S main.c -o main.s</code></pre></figure>

<!-- $ /usr/lib/gcc/i386-linux-gnu/4.7/cc1 main.c -->

<h3 id="汇编">汇编</h3>

<p>汇编器是将汇编代码转化成机器码，每条汇编语句几乎都对应一条机器指令。汇编器不需要复杂的语法语义，也不用进行指令优化，只是根据汇编指令和机器指令的对照表一一翻译即可。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ gcc -c hello.s -o hello.o
$ as main.s -o main.o
$ gcc -c main.s -o main.o
$ gcc -c main.c -o main.o</code></pre></figure>

<h3 id="链接">链接</h3>

<p>可以通过 <code class="highlighter-rouge">gcc hello.c -o hello -v</code> 查看。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ gcc hello.o -o hello.exe</code></pre></figure>

<h2 id="静态库和动态库">静态库和动态库</h2>

<p>库有动态与静态两种，Linux 中动态通常用 <code class="highlighter-rouge">.so</code> 为后缀，静态用 <code class="highlighter-rouge">.a</code> 为后缀，如：<code class="highlighter-rouge">libhello.so</code> <code class="highlighter-rouge">libhello.a</code>。为了在同一系统中使用不同版本的库，可以在库文件名后加上版本号为后缀，例如：<code class="highlighter-rouge">libhello.so.1.0</code>，然后，使用时通过符号链接指向不同版本。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text"># ln -s libhello.so.1.0 libhello.so.1
# ln -s libhello.so.1 libhello.so</code></pre></figure>

<p>在动态链接的情况下，可执行文件中还有很多外部符号的引用还处于无效地址的状态，因此需要首先启一个 <strong>动态链接器 (Dynamic Linker)</strong>，该连接器的位置在程序的 <code class="highlighter-rouge">".interp"</code> (interpreter) 中指定，可以通过如下的命令查询。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ readelf -l a.out | grep interpreter</code></pre></figure>

<h3 id="共享库的更新">共享库的更新</h3>

<p>对于共享库更新时通常会有兼容更新和不兼容更新，此处所说的兼容是指二进制接口，即 <code class="highlighter-rouge">ABI (Application Binary Interface)</code>。</p>

<p>为了保证共享库的兼容性， Linux 采用一套规则来命名系统中的共享库，简单来说，其规则如下 <code class="highlighter-rouge">libname.so.x.y.z</code>，name 为库的名称，x y z 的含义如下：</p>

<ul>
  <li>x，主版本号(Major Version Number)，库的重大升级，不同的主版本号之间不兼容。</li>
  <li>y，次版本号(Minor Version Number)，库的增量升级，增加了一些新的接口，且保持原来的符号不变。</li>
  <li>z，发布版本号(Release Version Number)，库的一些错误的修正、性能的改进等，并不添加任何新的接口，也不对接口进行改进。</li>
</ul>

<p>由于历史的原因最基本的 C 语言库 glibc 动态链接库不使用这种规则，如 <code class="highlighter-rouge">libc-x.y.z.so</code> 、<code class="highlighter-rouge">ld-x.y.z.so</code> 。</p>

<p>下面这篇论文， Library Interface Versioning in Solaris and Linux ，对 Salaris 和 Linux 的共享库版本机制和符号版本机制做了非常详细的介绍。</p>

<p>在 Linux 中采用 SO-NAME 的命名机制，每个库会对应一个 SO-NAME ，这个 SO-NAME 只保留主版本号，也即 SO-NAME 规定了共享库的接口。</p>

<h3 id="路径问题">路径问题</h3>

<p>如果动态库不在搜索路径中，则会报 <code class="highlighter-rouge">cannot open shared object file: No such file or directory</code> 的错误。可以通过 <code class="highlighter-rouge">gcc --print-search-dirs</code> 命令查看默认的搜索路径。</p>

<p>查找顺序通常为：</p>

<ol>
  <li>查找程序编译指定的路径，保存在 <code class="highlighter-rouge">.dynstr</code> 段，其中包含了一个以冒号分割的目录搜索列表。</li>
  <li>查找环境变量 <code class="highlighter-rouge">LD_LIBRARY_PATH</code>，以冒号分割的目录搜索列表。</li>
  <li>查找 <code class="highlighter-rouge">/etc/ld.so.conf</code> 。</li>
  <li>默认路径 <code class="highlighter-rouge">/lib</code> 和 <code class="highlighter-rouge">/usr/lib</code> 。</li>
</ol>

<p>为了让执行程序顺利找到动态库，有三种方法：</p>

<h5 id="1-复制到指定路径">1. 复制到指定路径</h5>

<p>把库拷贝到查找路径下，通常为 <code class="highlighter-rouge">/usr/lib</code> 和 <code class="highlighter-rouge">/lib</code> 目录下，或者通过 <code class="highlighter-rouge">gcc --print-search-dirs</code> 查看动态库的搜索路径。</p>

<h5 id="2-添加链接选项">2. 添加链接选项</h5>

<p>编译时添加链接选项，指定链接库的目录，此时会将该路径保存在二进制文件中。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ gcc -o test test.o -L. -lhello -Wl,-rpath,/home/lib:.
$ readelf -d test | grep RPATH
$ objdump -s -j .dynstr test                     // 查看.dynstr段的内容</code></pre></figure>

<h5 id="3-设置环境变量">3. 设置环境变量</h5>

<p>执行时在 <code class="highlighter-rouge">LD_LIBRARY_PATH</code> 环境变量中加上库所在路径，例如动态库 <code class="highlighter-rouge">libhello.so</code> 在 <code class="highlighter-rouge">/home/test/lib</code> 目录下。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/test/lib</code></pre></figure>

<h5 id="4-修改配置文件">4. 修改配置文件</h5>

<p>修改 <code class="highlighter-rouge">/etc/ld.so.conf</code> 文件，把库所在的路径加到文件中，并执行 <code class="highlighter-rouge">ldconfig</code> 刷新配置。动态链接库通常保存在 <code class="highlighter-rouge">/etc/ld.so.cache</code> 文件中，执行 <code class="highlighter-rouge">ldconfig</code> 可以对其进行刷新。</p>

<h3 id="静态连接库">静态连接库</h3>

<p>当要使用静态的程序库时，连接器会找出程序所需的函数，然后将它们拷贝到执行文件，由于这种拷贝是完整的，所以一旦连接成功，静态程序库也就不再需要了，缺点是占用的空间比较大。通常，静态链接的程序要比共享函数库的程序运行速度上快一些，大概 1-5％ 。</p>

<!--
动态库会在执行程序内留下一个标记指明当程序执行时，首先必须载入这个库。
-->

<p>注意，对于 CentOS 需要安装 <code class="highlighter-rouge">yum install glibc-static</code> 库。</p>

<p>Linux 下进行连接的缺省操作是首先连接动态库，也就是说，如果同时存在静态和动态库，不特别指定的话，将与动态库相连接。</p>

<p>现在假设有一个 hello 程序开发包，它提供一个静态库 <code class="highlighter-rouge">libhello.a</code>，一个动态库 <code class="highlighter-rouge">libhello.so</code>，一个头文件 <code class="highlighter-rouge">hello.h</code>，头文件中提供 <code class="highlighter-rouge">foobar()</code> 这个函数的声明。</p>

<p>下面这段程序 <code class="highlighter-rouge">main.c</code> 使用 hello 库中的 <code class="highlighter-rouge">foobar()</code> 函数。</p>

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

<p>生成静态库，先对源文件进行编译；然后使用 <code class="highlighter-rouge">ar(archive)</code> 命令连接成静态库。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ gcc -c hello.c -o hello.o
$ gcc -c foobar.c -o foobar.o
$ ar crv libhello.a hello.o foobar.o
$ ar -t libhello.a                              // 查看打包的文件</code></pre></figure>

<p><code class="highlighter-rouge">ar</code> 实际是一个打包工具，可以用来打包常见文件，不过现在被 <code class="highlighter-rouge">tar</code> 替代，目前主要是用于生成静态库，详细格式可以参考 <a href="http://en.wikipedia.org/wiki/Ar_(Unix)">ar(Unix) wiki</a> 。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ echo "hello" &gt; a.txt &amp;&amp; echo "world" &gt; b.txt
$ ar crv text.a a.txt b.txt
$ cat text.a</code></pre></figure>

<p>与静态库连接麻烦一些，主要是参数问题。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ gcc main.c -o test -lhello                    // 库在默认路径下，如/usr/lib
$ gcc main.c -lhello -L. -static -o main        // 通过-L指定库的路径

$ gcc main.o -o main -WI,-Bstatic -lhello       // 报错，显示找不到-lgcc_s</code></pre></figure>

<p>注意：这个特别的 <code class="highlighter-rouge">"-WI,-Bstatic"</code> 参数，实际上是传给了连接器 <code class="highlighter-rouge">ld</code>，指示它与静态库连接，如果系统中只有静态库可以不需要这个参数； 如果要和多个库相连接，而每个库的连接方式不一样，比如上面的程序既要和 <code class="highlighter-rouge">libhello</code> 进行静态连接，又要和 <code class="highlighter-rouge">libbye</code> 进行动态连接，其命令应为：</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ gcc testlib.o -o test -WI,-Bstatic -lhello -WI,-Bdynamic -lbye</code></pre></figure>

<p>最好不要进行分别编译、链接，因为在生成可执行文件时往往需要很多的其他文件，可以通过 <code class="highlighter-rouge">-v</code> 选项进行查看，如果通过如下方式进行编译通常会出现错误。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ gcc -c main.c
$ ld main.o -L. -lhello</code></pre></figure>

<h3 id="动态库">动态库</h3>

<p>用 gcc 编绎该文件，在编绎时可以使用任何编绎参数，例如 <code class="highlighter-rouge">-g</code> 加入调试代码等；<code class="highlighter-rouge">-fPIC</code> 生成与位置无关的代码（可以在任何地址被连接和装载）。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ gcc -c -fPIC hello.c -o hello.o

$ gcc -shared -Wl,-soname,libhello.so.1 -o libhello.so.1.0 hello.o // 生成动态库，可能存在多个版本，通常指定版本号

$ ln  -s  libhello.so.1.0  libhello.so.1                           // 另外再建立两个符号连接
$ ln  -s  libhello.so.1  libhello.so

$ gcc -fPIC -shared -o libhello.so hello.c                         // 当然对于上述的步骤可以通过一步完成

$ readelf -d libhello.so.1.0 | grep SONAME                         // 查看对应的soname
$ nm -D libhello.so                                                // 查看符号</code></pre></figure>

<p>最重要的是传 <code class="highlighter-rouge">-shared</code> 参数使其生成是动态库而不是普通执行程序； <code class="highlighter-rouge">-Wl</code> 表示后面的参数也就是 <code class="highlighter-rouge">-soname,libhello.so.1</code> 直接传给连接器 <code class="highlighter-rouge">ld</code> 进行处理。</p>

<p>实际上，每一个库都有一个 <code class="highlighter-rouge">soname</code> ，当连接器发现它正在查找的程序库中有这样一个名称，连接器便会将 <code class="highlighter-rouge">soname</code> 嵌入连结中的二进制文件内，而不是它正在运行的实际文件名，在程序执行期间，程序会查找拥有 <code class="highlighter-rouge">soname</code> 名字的文件，而不是库的文件名，换句话说，<code class="highlighter-rouge">soname</code> 是库的区分标志。</p>

<p>其目的主要是允许系统中多个版本的库文件共存，习惯上在命名库文件的时候通常与 <code class="highlighter-rouge">soname</code> 相同 <code class="highlighter-rouge">libxxxx.so.major.minor</code> 其中，<code class="highlighter-rouge">xxxx</code> 是库的名字， <code class="highlighter-rouge">major</code> 是主版本号， <code class="highlighter-rouge">minor</code> 是次版本号。</p>

<h3 id="查看库中的符号">查看库中的符号</h3>

<p>有时候可能需要查看一个库中到底有哪些函数，<code class="highlighter-rouge">nm</code> 命令可以打印出库中的涉及到的所有符号，库既可以是静态的也可以是动态的。</p>

<p><code class="highlighter-rouge">nm</code> 列出的符号有很多，常见的有三种：</p>

<ul>
  <li>在库中被调用，但并没有在库中定义(表明需要其他库支持)，用U表示；</li>
  <li>库中定义的函数，用T表示，这是最常见的；</li>
  <li>所谓的“弱态”符号，它们虽然在库中被定义，但是可能被其他库中的同名符号覆盖，用W表示。</li>
</ul>

<p>例如，希望知道上文提到的 <code class="highlighter-rouge">hello</code> 库中是否定义了 <code class="highlighter-rouge">printf()</code> 。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ nm libhello.so</code></pre></figure>

<p>发现其中没有 <code class="highlighter-rouge">printf()</code> 的定义，取而代之的是 <code class="highlighter-rouge">puts()</code> 函数，而且为 <code class="highlighter-rouge">U</code> ，表示符号 <code class="highlighter-rouge">puts</code> 被引用，但是并没有在函数内定义，由此可以推断，要正常使用 <code class="highlighter-rouge">hello</code> 库，必须有其它库支持，再使用 <code class="highlighter-rouge">ldd</code> 命令查看 <code class="highlighter-rouge">hello</code> 依赖于哪些库：</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ ldd -v hello
$ readelf -d hello     直接使用readelf</code></pre></figure>

<p>每行 <code class="highlighter-rouge">=&gt;</code> 前面的，为动态链接程序所需的动态链接库的名字；而 <code class="highlighter-rouge">=&gt;</code> 后面的，则是运行时系统实际调用的动态链接库的名字。所需的动态链接库在系统中不存在时，<code class="highlighter-rouge">=&gt;</code> 后面将显示 <code class="highlighter-rouge">"not found"</code>，括号所括的数字为虚拟的执行地址。</p>

<p>常用的系统动态链接库有：</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">libc.so.x        基本C库
ld-linux.so.2    动态装入库(用于动态链接库的装入及运行)</code></pre></figure>

<h2 id="动态库加载api">动态库加载API</h2>

<p>对于 Linux 下的可执行文件 ELF 使用如下命令查看，可以发现其中有一个 <code class="highlighter-rouge">.interp</code> 段，它指明了将要被使用的动态链接器 (<code class="highlighter-rouge">/lib/ld-linux.so</code>)。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ readelf -l EXECUTABLE</code></pre></figure>

<p>对于动态加载的函数主要包括了下面的四个函数，需要 <code class="highlighter-rouge">dlfcn.h</code> 头文件，定义在 <code class="highlighter-rouge">libdl.so</code> 库中。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">void *dlopen( const char *file, int mode );
  用来打开一个文件，使对象文件可被程序访问，同时还会自动解析共享库中的依赖项，这样，如果打开了一个
    依赖于其他共享库的对象，它就会自动加载它们，该函数返回一个句柄，该句柄用于后续的 API 调用。
  mode 参数通知动态链接器何时执行再定位，有两个可能的值：
    A) RTLD_NOW，表明动态链接器将会在调用 dlopen 时完成所有必要的再定位；
    B) RTLD_LAZY，只在需要时执行再定位。

void *dlsym( void *restrict handle, const char *restrict name );
  通过句柄和连接符名称获取函数名或者变量名。

char *dlerror();
  返回一个可读的错误字符串，该函数没有参数，它会在发生前面的错误时返回一个字符串，在没有错误发生时返回NULL。

char *dlclose( void *handle ); 
  通知操作系统不再需要句柄和对象引用了。它完全是按引用来计数的，所以同一个共享对象的多个用户相互间
    不会发生冲突（只要还有一个用户在使用它，它就会待在内存中）。
    任何通过已关闭的对象的 dlsym 解析的符号都将不再可用。</code></pre></figure>

<p>有了 ELF 对象的句柄，就可以通过调用 dlsym 来识别这个对象内的符号的地址了。该函数采用一个符号名称，如对象内的一个函数的名称，返回值为对象符号的解析地址。</p>

<p>下面是一个动态加载的示例 <a href="https://github.com/Jin-Yang/examples/tree/master/c_cpp/c/libdl.c">github libdl.c</a>，通过如下的命令进行编译，其中选项 <code class="highlighter-rouge">-rdynamic</code> 用来通知链接器将所有符号添加到动态符号表中（目的是能够通过使用 dlopen 来实现向后跟踪）。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ gcc -rdynamic -o dl library_libdl.c -ldl        # 编译
$ ./dl                                            # 测试
&gt; libm.so cosf 0.0
   1.000000
&gt; libm.so sinf 0.0
   0.000000
&gt; libm.so tanf 1.0
   1.557408
&gt; bye</code></pre></figure>

<p>另外，可以通过如下方式简单使用。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ cat caculate.c                                     # 查看动态库源码
int add(int a, int b) {
    return (a + b);
}
int sub(int a, int b) {
    return (a - b);
}
$ gcc -fPIC -shared caculate.c -o libcaculate.so     # 生成动态库
$ cat foobar.c                                       # 测试源码
#include &lt;stdio.h&gt;
#include &lt;dlfcn.h&gt;
#include &lt;stdlib.h&gt;

typedef int (*CAC_FUNC)(int, int);                           // 定义函数指针类型
int main(int argc, char** argv) {
    void *handle;
    char *error;
    CAC_FUNC cac_func = NULL;

    if ( !(handle=dlopen("./libcaculate.so", RTLD_LAZY)) ) { // 打开动态链接库
        fprintf(stderr, "!!! %s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    cac_func = dlsym(handle, "add");                         // 获取一个函数
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "!!! %s\n", error);
        exit(EXIT_FAILURE);
    }
    printf("add: %d\n", (cac_func)(2,7));

    dlclose(handle);                                         // 关闭动态链接库
    exit(EXIT_SUCCESS);
}
$ gcc -rdynamic -o foobar foobar.c -ldl              # 编译测试</code></pre></figure>

<!-- https://www.ibm.com/developerworks/cn/linux/l-elf/part1/index.html -->

<h2 id="常用命令">常用命令</h2>

<h3 id="objdump">objdump</h3>

<p>详细参考 <code class="highlighter-rouge">man objdump</code> 。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">-h, --section-headers, --headers
  查看目标文件的头部信息。
-x, --all-headers
  显示所有的头部信息，包括了符号表和重定位表，等价于 -a -f -h -p -r -t 。
-s, --full-contents
  显示所请求段的全部信息，通常用十六进制表示，默认只会显示非空段。
-d, --disassemble
  反汇编，一般只反汇编含有指令的段。
-t, --syms
  显示符号表，与nm类似，只是显示的格式不同，当然显示与文件的格式相关，对于ELF如下所示。
  00000000 l    d  .bss   00000000 .bss
  00000000 g       .text  00000000 fred</code></pre></figure>

<!--
第一列为符号的值，有时是地址；下一个是用字符表示的标志位；接着是与符号相关的段，*ABS* 表示段是绝对的（没和任何段相关联）， *UND* 表示未定义；对于普通符号(Common Symbols)表示对齐，其它的表示大小；最后是符号的名字。<br><br>

对于标志组的字符被分为如下的 7 组。
<ol type="A"><li>
    "l(local)" "g(global)" "u(unique global)" " (neither global nor local)" "!(both global and local)"<br>
    通常一个符号应该是 local 或 global ，但还有其他的一些原因，如用于调试、"!"表示一个bug、"u"是 ELF 的扩展，表示整个进程中只有一个同类型同名的变量。</li><br><li>

    "w(weak)" " (strong)"<br>
    表示强或弱符号。</li><br><li>

    "C(constructor)" " (ordinary)"<br>
    为构造函数还是普通符号。</li><br><li>

    "W(warning)" " (normal symbol)"<br>
    如果一个含有警告标志的符号被引用时，将会输出警告信息。</li><br><li>

    "I"
   "i" The symbol is an indirect reference to another symbol (I), a function to be evaluated
       during reloc processing (i) or a normal symbol (a space).

   "d(debugging symbol)" "D(dynamic symbol)" " (normal symbol)"<br>
    表示调试符号、动态符号还是普通的符号。</li><br><li>

   "F(function)" "f(file)" "O(object)" " (normal)"<br>
    表示函数、文件、对象或只是一个普通的符号。
-->

<h3 id="strip">strip</h3>

<p>我们知道二进制的程序中包含了大量的符号表格(symbol table)，有一部分是用来 gdb 调试提供必要信息的，可以通过如下命令查看这些符号信息。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ readelf -S hello</code></pre></figure>

<p>其中类似与 <code class="highlighter-rouge">.debug_xxxx</code> 的就是 gdb 调试用的。去掉它们不会影响程序的执行。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ strip hello</code></pre></figure>

<h3 id="objcopy">objcopy</h3>

<p>用于转换目标文件。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">常用参数：
  -S / --strip-all
    不从源文件中拷贝重定位信息和符号信息到输出文件(目的文件)中去。
  -I bfdname/--input-target=bfdname
    明确告诉程序源文件的格式是什么，bfdname是BFD库中描述的标准格式名。
  -O bfdname/--output-target=bfdname
    使用指定的格式来写输出文件(即目标文件)，bfdname是BFD库中描述的标准格式名，
    如binary(raw binary 格式)、srec(s-record 文件)。
  -R sectionname/--remove-section=sectionname
    从输出文件中删掉所有名为section-name的段。</code></pre></figure>

<p>上一步的 strip 命令只能拿掉一般 symbol table，有些信息还是沒拿掉，而这些信息对于程序的最终执行没有影响，如: <code class="highlighter-rouge">.comment</code> <code class="highlighter-rouge">.note.ABI-tag</code> <code class="highlighter-rouge">.gnu.version</code> 就是完全可以去掉的。</p>

<p>所以说程序还有简化的余地，我们可以使用 objcopy 命令把它们抽取掉。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ objcopy -R .comment -R .note.ABI-tag -R .gnu.version hello hello1</code></pre></figure>

<h3 id="readelf">readelf</h3>

<p>用于读取 ELF 格式文件，包括可执行程序和动态库。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">常用参数：
  -a --all
    等价于-h -l -S -s -r -d -V -A -I
  -h --file-header
    文件头信息；
  -l --program-headers
    程序的头部信息；
  -S --section-headers
    各个段的头部信息；
  -e --headers
    全部头信息，等价于-h -l -S；

示例用法：
----- 读取dynstr段，包含了很多需要加载的符号，每个动态库后跟着需要加载函数
$ readelf -p .dynstr hello
----- 查看是否含有调试信息
$ readelf -S hello | grep debug</code></pre></figure>

<!--
readelf  -S hello
readelf -d hello

  --sections
An alias for –section-headers
-s –syms 符号表 Display the symbol table
--symbols
An alias for –syms
-n –notes 内核注释 Display the core notes (if present)
-r –relocs 重定位 Display the relocations (if present)
-u –unwind Display the unwind info (if present)
-d --dynamic
  显示动态段的内容；
-V –version-info 版本 Display the version sections (if present)
-A –arch-specific CPU构架 Display architecture specific information (if any).
-D –use-dynamic 动态段 Use the dynamic section info when displaying symbols
-x –hex-dump=<number> 显示 段内内容Dump the contents of section <number>
-w[liaprmfFso] or
-I –histogram Display histogram of bucket list lengths
-W –wide 宽行输出 Allow output width to exceed 80 characters
-H –help Display this information
-v –version Display the version number of readelf
-->

<h2 id="其它">其它</h2>

<h4 id="1-静态库生成动态库">1. 静态库生成动态库</h4>

<p>可以通过多个静态库生成动态库，而实际上静态库是一堆 <code class="highlighter-rouge">.o</code> 库的压缩集合，而生成动态库需要保证 <code class="highlighter-rouge">.o</code> 编译后是与地址无关的，也就是添加 <code class="highlighter-rouge">-fPIC</code> 参数。</p>



</div>