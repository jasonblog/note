# 物件模式(二)


<article class="post" itemscope="" itemtype="http://schema.org/BlogPosting">

  <header class="post-header">
    <h1 class="post-title" itemprop="name headline">C++ 对象模型(二)</h1>
    <p class="post-meta"><time datetime="2016-08-10T03:59:00+00:00" itemprop="datePublished">Aug 10, 2016</time></p>
  </header>

  <div class="post-content" itemprop="articleBody">
    <p><em>这是C++语言的一系列文章，内容取自于网易微专业《C++开发工程师（升级版）》。</em></p>

<p><em>本文是听了侯捷老师关于“C++对象模型”的课程以后，总结而成的。</em>
<em>课程中讲到了范型编程和面向对象编程两种模式，上篇博客中，我们讨论了类和模板的一些高级话题。</em>
<em>本文中，我们对继承体系下的类的构成进行深入讨论，相关话题包括this指针、虚指针vptr、虚表vtable、虚函数、多态等。</em>
<em>另外，new 和 delete 的重载是本文的另一个话题。</em></p>

<h2 id="part-1">Part 1：虚表、虚指针、动态绑定</h2>

<p>这一部分，我们介绍下 继承体系下，类和对象的存储形式。</p>

<h3 id="vptr---vtable-">1.1 vptr 虚指针 和 vtable 虚表</h3>

<p>对于虚指针和虚表的定义，这里引用一段 <a href="https://www.quora.com/What-are-vTable-and-VPTR-in-C++" title="c++">quora</a> 上的一个回复(这里我已经翻译成中文)：
如果一个类存在一个或多个虚函数，编译器会为这个类的实例 (对象) 创建一个隐藏的成员变量，即虚指针(virtual-pointer)，简称 vptr。
vptr 指向一个包含一组函数指针的表，我们称之为 虚表 (virtual table)，简称 vtable。虚表由编译器创建，虚表中的每一项均是
一个指向对应虚函数的指针。</p>

<p>为了实现动态绑定 (dynamic binding)，编译器为每一个拥有虚函数的类 (和它的子类) 创建一张虚表。编译器将虚函数的地址存放到对应
类的虚表中。 当通过基类指针 (或父类指针，Base * pb) 调用虚函数时，编译器插入一段在虚表中查找虚函数地址和获取 vptr 的代码。
所以才能够调用到”正确”的函数，实现动态绑定。</p>

<p>关于 vptr 和 vtable 的调用，这里用侯捷老师 PPT 上的一张图表示：</p>

<p><img src="images/vptr-vtable.jpg" alt="vptr-vtable" title="vtable-vptr"></p>

<p>关于 类 A、B、C 的结构声明参考下面的代码 (注意这里不包含具体实现)：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// 上图中 类 A、B、C 的声明
</span><span class="k">class</span> <span class="nc">A</span> <span class="p">{</span>
<span class="k">public</span><span class="o">:</span>
  <span class="k">virtual</span> <span class="kt">void</span> <span class="n">vfunc1</span><span class="p">();</span>
  <span class="k">virtual</span> <span class="kt">void</span> <span class="n">vfunc2</span><span class="p">();</span>
  <span class="kt">void</span> <span class="n">func1</span><span class="p">();</span>
  <span class="kt">void</span> <span class="n">func2</span><span class="p">();</span>
<span class="k">private</span><span class="o">:</span>
  <span class="kt">int</span> <span class="n">m_data1</span><span class="p">,</span> <span class="n">m_data2</span><span class="p">;</span>
<span class="p">}</span>

<span class="k">class</span> <span class="nc">B</span><span class="o">:</span> <span class="k">public</span> <span class="n">A</span> <span class="p">{</span>
<span class="k">public</span><span class="o">:</span>
  <span class="k">virtual</span> <span class="kt">void</span> <span class="n">vfunc1</span><span class="p">();</span>
  <span class="kt">void</span> <span class="n">func2</span><span class="p">();</span>
<span class="k">private</span><span class="o">:</span>
  <span class="kt">int</span> <span class="n">m_data3</span><span class="p">;</span>
<span class="p">}</span>

<span class="k">class</span> <span class="nc">C</span><span class="o">:</span> <span class="k">public</span> <span class="n">B</span> <span class="p">{</span>
<span class="k">public</span><span class="o">:</span>
  <span class="k">virtual</span> <span class="kt">void</span> <span class="n">vfunc1</span><span class="p">();</span>
  <span class="kt">void</span> <span class="n">func2</span><span class="p">();</span>
<span class="k">private</span><span class="o">:</span>
  <span class="kt">int</span> <span class="n">m_data1</span><span class="p">,</span> <span class="n">m_data4</span><span class="p">;</span>
<span class="p">}</span>
</code></pre>
</div>

<h3 id="this-pointer-template-method">1.2 this pointer (template method)</h3>

<p>在继承体系中，子类对象调用一个方法时，如果该类本身这个函数，那么会调用这个函数；如果本身没有，那么编译器会沿着继承树向上查找父类中是否有该方法。</p>

<p>侯捷老师PPT中的一张图很好地体现了这种调用机制：</p>

<p><img src="images/20160811-this-pointer-2.jpg" alt="虚函数-子类调用" title="virtual-inheritance"></p>

<h3 id="dynamic-binding-">1.3 dynamic binding 动态绑定</h3>

<h4 id="section">1.3.1 什么是动态绑定？</h4>

<p>动态绑定是编程语言的一种特性（或机制），它允许程序在运行时决定执行操作的细节，而不是在编译时就确定。在设计一个软件时，通常会出现下面两类情况：</p>

<ol>
  <li>类的接口已经确定，但是还不知道具体怎么实现</li>
  <li>开发者知道需要什么算法，但是不知道具体的操作</li>
</ol>

<p>这两种情况下，开发者都需要延迟决定，延迟到什么时候呢？延迟到已经有足够的信息去做一个正确的决策。此时如果能不修改原先的实现，我们的目标就达到了。</p>

<p>动态绑定正是为了满足这些需求而存在，结果就是更灵活和可伸缩的软件架构。比如在软件开发初期，不需要做出所有设计决策。这里我们讨论下灵活性和可伸缩性：</p>

<ol>
  <li>flexibility (灵活性): 很容易将现存组件和新的配置合并到一起</li>
  <li>extensibility (扩展性)： 很容易添加新组件</li>
</ol>

<p>C++ 通过 虚表和虚指针机制 实现对动态绑定的支持，具体的机制我们在上面已经谈到，这里不再赘述。</p>

<h4 id="c-">1.3.2 动态绑定在 C++ 中的体现</h4>

<p>在 C++ 中，动态绑定的标志是在声明类方法时，在方法名前面显式地添加 virtual 关键字。比如下面这样：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="k">class</span> <span class="nc">Base</span> <span class="p">{</span>
<span class="k">public</span><span class="o">:</span>
  <span class="k">virtual</span> <span class="kt">void</span> <span class="n">vfunc1</span><span class="p">()</span> <span class="p">{</span> <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">"Base::vfunc1()"</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">endl</span><span class="p">;</span> <span class="p">}</span>
  <span class="kt">void</span> <span class="n">func1</span><span class="p">()</span> <span class="p">{</span> <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">"Base::func1()"</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">endl</span><span class="p">;</span> <span class="p">}</span>
<span class="p">}</span>
</code></pre>
</div>

<p>只有类的成员函数才能被声明为虚函数，下面三种是不可以的：</p>

<ol>
  <li>普通的函数 (不属于任何一个类)</li>
  <li>类的成员变量</li>
  <li>静态方法 (static 修饰的成员函数)</li>
</ol>

<p>virtual 修饰的成员函数的接口是固定的，但是子类中的同名成员函数可以修改默认实现，比如像下面这样：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="k">class</span> <span class="nc">Derived_1</span> <span class="p">{</span>
<span class="k">public</span><span class="o">:</span>
  <span class="k">virtual</span> <span class="kt">void</span> <span class="n">vfunc1</span><span class="p">()</span> <span class="p">{</span> <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">"Derived_1::vfunc1() "</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">endl</span><span class="p">;</span> <span class="p">}</span>
<span class="p">}</span>
</code></pre>
</div>

<p>注意：上面的代码中， virtual 是可选的，即便不写，它仍然是虚函数！</p>

<p>在程序运行时，虚函数调度机制会根据对象的”动态类型”选择对应的成员函数。
被选择的成员函数依赖于被指针指向的对象，而不是指针的类型。看下面代码：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="kt">void</span> <span class="nf">foo</span> <span class="p">(</span><span class="n">Base</span> <span class="o">*</span><span class="n">bp</span><span class="p">)</span> <span class="p">{</span> <span class="n">bp</span><span class="o">-&gt;</span><span class="n">vf1</span> <span class="p">();</span> <span class="cm">/* virtual */</span> <span class="p">}</span>
<span class="n">Base</span> <span class="n">b</span><span class="p">;</span>
<span class="n">Base</span> <span class="o">*</span><span class="n">bp</span> <span class="o">=</span> <span class="o">&amp;</span><span class="n">b</span><span class="p">;</span>
<span class="n">bp</span><span class="o">-&gt;</span><span class="n">vf1</span> <span class="p">();</span> <span class="c1">// 打印 "Base::vfunc1()"
</span><span class="n">Derived_1</span> <span class="n">d</span><span class="p">;</span>
<span class="n">bp</span> <span class="o">=</span> <span class="o">&amp;</span><span class="n">d</span><span class="p">;</span>
<span class="n">bp</span><span class="o">-&gt;</span><span class="n">vf1</span> <span class="p">();</span> <span class="c1">// 打印 "Derived_1::vfunc1()"
</span><span class="n">foo</span> <span class="p">(</span><span class="o">&amp;</span><span class="n">b</span><span class="p">);</span> <span class="c1">// 打印 "Base::vfunc1()"
</span><span class="n">foo</span> <span class="p">(</span><span class="o">&amp;</span><span class="n">d</span><span class="p">);</span> <span class="c1">// 打印 "Derived_1::vfunc1()"，这里存在一个隐式的向上转型
</span></code></pre>
</div>

<p>关于动态绑定，更多细节参考 <a href="http://www.cs.wustl.edu/~schmidt/PDF/C++-dynamic-binding4.pdf" title="dynamic-binding">C++ dynamic binding</a>。</p>

<h2 id="part-2-const-">Part 2: const 补充</h2>

<p>这个小结中，关于 const 的所有例子均来自于 <a href="https://msdn.microsoft.com/en-us/library/07x6b05d.aspx" title="const">msdn</a>。为了便于理解，
对代码进行了稍微的调整。</p>

<h3 id="const-">2.1 const 修饰指针</h3>
<p>下面这个例子中， const 修饰的是指针，因此不能修改指针 aptr 的值，即 aptr 不能指向另一个位置。</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// constant_values3.cpp
</span><span class="kt">int</span> <span class="nf">main</span><span class="p">()</span> <span class="p">{</span>
   <span class="kt">char</span> <span class="o">*</span><span class="n">mybuf</span> <span class="o">=</span> <span class="mi">0</span><span class="p">,</span> <span class="o">*</span><span class="n">yourbuf</span><span class="p">;</span>
   <span class="kt">char</span><span class="o">*</span> <span class="k">const</span> <span class="n">aptr</span> <span class="o">=</span> <span class="n">mybuf</span><span class="p">;</span>
   <span class="o">*</span><span class="n">aptr</span> <span class="o">=</span> <span class="sc">'a'</span><span class="p">;</span>   <span class="c1">// OK
</span>   <span class="n">aptr</span> <span class="o">=</span> <span class="n">yourbuf</span><span class="p">;</span>   <span class="c1">// C3892
</span><span class="p">}</span> 
</code></pre>
</div>

<h3 id="const--1">2.2 const 修饰指针指向的数据</h3>
<p>下面这个例子中， const 修饰的是指针指向的数据，因此可以修改指针的值，但是不能修改指针指向的数据。</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// constant_values4.cpp
</span><span class="cp">#include &lt;stdio.h&gt;
</span><span class="kt">int</span> <span class="nf">main</span><span class="p">()</span> <span class="p">{</span>
   <span class="k">const</span> <span class="kt">char</span> <span class="o">*</span><span class="n">mybuf</span> <span class="o">=</span> <span class="s">"test"</span><span class="p">;</span>
   <span class="kt">char</span><span class="o">*</span> <span class="n">yourbuf</span> <span class="o">=</span> <span class="s">"test2"</span><span class="p">;</span>
   <span class="n">printf_s</span><span class="p">(</span><span class="s">"%s</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span> <span class="n">mybuf</span><span class="p">);</span>

   <span class="k">const</span> <span class="kt">char</span><span class="o">*</span> <span class="n">bptr</span> <span class="o">=</span> <span class="n">mybuf</span><span class="p">;</span>   <span class="c1">// Pointer to constant data
</span>   <span class="n">printf_s</span><span class="p">(</span><span class="s">"%s</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span> <span class="n">bptr</span><span class="p">);</span>

   <span class="c1">// *bptr = 'a';   // Error
</span><span class="p">}</span>
</code></pre>
</div>

<h3 id="const--2">2.3 const 修饰成员函数</h3>
<p>在声明成员函数时，如果在函数末尾使用 const 关键字，那么可以称这个函数是”只读”函数。 const成员函数不能修改任何 非static的成员变量，
也不能调用任何 非const 成员函数。</p>

<p>const成员函数在<code class="highlighter-rouge">声明</code>和<code class="highlighter-rouge">定义</code>时，都必须带有 const 关键字。看下面这个例子：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// constant_member_function.cpp
</span><span class="k">class</span> <span class="nc">Date</span>
<span class="p">{</span>
<span class="k">public</span><span class="o">:</span>
   <span class="n">Date</span><span class="p">(</span> <span class="kt">int</span> <span class="n">mn</span><span class="p">,</span> <span class="kt">int</span> <span class="n">dy</span><span class="p">,</span> <span class="kt">int</span> <span class="n">yr</span> <span class="p">);</span>
   <span class="kt">int</span> <span class="n">getMonth</span><span class="p">()</span> <span class="k">const</span><span class="p">;</span>     <span class="c1">// A read-only function
</span>   <span class="kt">void</span> <span class="n">setMonth</span><span class="p">(</span> <span class="kt">int</span> <span class="n">mn</span> <span class="p">);</span>   <span class="c1">// A write function; can't be const
</span><span class="nl">private:</span>
   <span class="kt">int</span> <span class="n">month</span><span class="p">;</span>
<span class="p">};</span>

<span class="kt">int</span> <span class="n">Date</span><span class="o">::</span><span class="n">getMonth</span><span class="p">()</span> <span class="k">const</span>
<span class="p">{</span>
   <span class="k">return</span> <span class="n">month</span><span class="p">;</span>        <span class="c1">// Doesn't modify anything
</span><span class="p">}</span>
<span class="kt">void</span> <span class="n">Date</span><span class="o">::</span><span class="n">setMonth</span><span class="p">(</span> <span class="kt">int</span> <span class="n">mn</span> <span class="p">)</span>
<span class="p">{</span>
   <span class="n">month</span> <span class="o">=</span> <span class="n">mn</span><span class="p">;</span>          <span class="c1">// Modifies data member
</span><span class="p">}</span>
<span class="kt">int</span> <span class="n">main</span><span class="p">()</span>
<span class="p">{</span>
   <span class="n">Date</span> <span class="n">MyDate</span><span class="p">(</span> <span class="mi">7</span><span class="p">,</span> <span class="mi">4</span><span class="p">,</span> <span class="mi">1998</span> <span class="p">);</span>
   <span class="k">const</span> <span class="n">Date</span> <span class="n">BirthDate</span><span class="p">(</span> <span class="mi">1</span><span class="p">,</span> <span class="mi">18</span><span class="p">,</span> <span class="mi">1953</span> <span class="p">);</span>
   <span class="n">MyDate</span><span class="p">.</span><span class="n">setMonth</span><span class="p">(</span> <span class="mi">4</span> <span class="p">);</span>    <span class="c1">// Okay
</span>   <span class="n">BirthDate</span><span class="p">.</span><span class="n">getMonth</span><span class="p">();</span>    <span class="c1">// Okay
</span>   <span class="n">BirthDate</span><span class="p">.</span><span class="n">setMonth</span><span class="p">(</span> <span class="mi">4</span> <span class="p">);</span> <span class="c1">// C2662 Error
</span><span class="p">}</span>
</code></pre>
</div>

<h2 id="part-3new--delete">Part 3：new 和 delete</h2>

<h3 id="new--delete">3.1 分解 new 和 delete</h3>

<p>new 和 delete 都是表达式，因此不能被重载。它们均有不同步骤组成：</p>

<p>new 的执行步骤：</p>

<ol>
  <li>调用operator new 分配内存 (malloc)</li>
  <li>对指针进行类型转换</li>
  <li>调用构造函数</li>
</ol>

<p>delete 的执行步骤：</p>

<ol>
  <li>调用析构函数</li>
  <li>调用operator delete释放内存 (free)</li>
</ol>

<p>虽然，new 和 delete 不能被重载，但是 operator new 和 operator delete 可以被重载。
更多细节查看 <a href="https://msdn.microsoft.com/en-us/library/h6227113.aspx" title="delete">msdn 上的相关页面</a>。
关于重写 operator new/delete的一些原因，参考<a href="http://www.cprogramming.com/tutorial/operator_new.html" title="delete">Customized Allocators with Operator New and Operator Delete</a>。</p>

<h3 id="operator-new--operator-delete">3.2 重载 operator new 和 operator delete</h3>

<h4 id="operator-new--operator-delete-1">3.2.1 重载全局 operator new 和 operator delete</h4>

<p>用户可以通过重新定义 全局 new 和 delete 操作符，以便通过日志或其它方式记录内存的分配和释放。
其中一个应用场景是用于检查内存泄漏。代码如下：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// 这段代码来自于 msdn：https://msdn.microsoft.com/en-us/library/kftdy56f.aspx
</span>
<span class="c1">// spec1_the_operator_delete_function1.cpp
// compile with: /EHsc
// arguments: 3
</span><span class="cp">#include &lt;iostream&gt;
#include &lt;malloc.h&gt;
#include &lt;memory.h&gt;
#include &lt;stdlib.h&gt;
</span><span class="k">using</span> <span class="k">namespace</span> <span class="n">std</span><span class="p">;</span>

<span class="kt">int</span> <span class="n">fLogMemory</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span>      <span class="c1">// Perform logging (0=no; nonzero=yes)?
</span><span class="kt">int</span> <span class="n">cBlocksAllocated</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span>  <span class="c1">// Count of blocks allocated.
</span>
<span class="c1">// User-defined operator new.
</span><span class="kt">void</span> <span class="o">*</span><span class="k">operator</span> <span class="nf">new</span><span class="p">(</span> <span class="kt">size_t</span> <span class="n">stAllocateBlock</span> <span class="p">)</span> <span class="p">{</span>
   <span class="k">static</span> <span class="kt">int</span> <span class="n">fInOpNew</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span>   <span class="c1">// Guard flag.
</span>
   <span class="k">if</span> <span class="p">(</span> <span class="n">fLogMemory</span> <span class="o">&amp;&amp;</span> <span class="o">!</span><span class="n">fInOpNew</span> <span class="p">)</span> <span class="p">{</span>
      <span class="n">fInOpNew</span> <span class="o">=</span> <span class="mi">1</span><span class="p">;</span>
      <span class="n">clog</span> <span class="o">&lt;&lt;</span> <span class="s">"Memory block "</span> <span class="o">&lt;&lt;</span> <span class="o">++</span><span class="n">cBlocksAllocated</span>
          <span class="o">&lt;&lt;</span> <span class="s">" allocated for "</span> <span class="o">&lt;&lt;</span> <span class="n">stAllocateBlock</span>
          <span class="o">&lt;&lt;</span> <span class="s">" bytes</span><span class="se">\n</span><span class="s">"</span><span class="p">;</span>
      <span class="n">fInOpNew</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span>
   <span class="p">}</span>
   <span class="k">return</span> <span class="n">malloc</span><span class="p">(</span> <span class="n">stAllocateBlock</span> <span class="p">);</span>
<span class="p">}</span>

<span class="c1">// User-defined operator delete.
</span><span class="kt">void</span> <span class="k">operator</span> <span class="nf">delete</span><span class="p">(</span> <span class="kt">void</span> <span class="o">*</span><span class="n">pvMem</span> <span class="p">)</span> <span class="p">{</span>
   <span class="k">static</span> <span class="kt">int</span> <span class="n">fInOpDelete</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span>   <span class="c1">// Guard flag.
</span>   <span class="k">if</span> <span class="p">(</span> <span class="n">fLogMemory</span> <span class="o">&amp;&amp;</span> <span class="o">!</span><span class="n">fInOpDelete</span> <span class="p">)</span> <span class="p">{</span>
      <span class="n">fInOpDelete</span> <span class="o">=</span> <span class="mi">1</span><span class="p">;</span>
      <span class="n">clog</span> <span class="o">&lt;&lt;</span> <span class="s">"Memory block "</span> <span class="o">&lt;&lt;</span> <span class="n">cBlocksAllocated</span><span class="o">--</span>
          <span class="o">&lt;&lt;</span> <span class="s">" deallocated</span><span class="se">\n</span><span class="s">"</span><span class="p">;</span>
      <span class="n">fInOpDelete</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span>
   <span class="p">}</span>

   <span class="n">free</span><span class="p">(</span> <span class="n">pvMem</span> <span class="p">);</span>
<span class="p">}</span>

<span class="c1">// User-defined operator delete.
</span><span class="kt">void</span> <span class="k">operator</span> <span class="k">delete</span><span class="p">[](</span> <span class="kt">void</span> <span class="o">*</span><span class="n">pvMem</span> <span class="p">)</span> <span class="p">{</span>
   <span class="k">static</span> <span class="kt">int</span> <span class="n">fInOpDelete</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span>   <span class="c1">// Guard flag.
</span>   <span class="k">if</span> <span class="p">(</span> <span class="n">fLogMemory</span> <span class="o">&amp;&amp;</span> <span class="o">!</span><span class="n">fInOpDelete</span> <span class="p">)</span> <span class="p">{</span>
      <span class="n">fInOpDelete</span> <span class="o">=</span> <span class="mi">1</span><span class="p">;</span>
      <span class="n">clog</span> <span class="o">&lt;&lt;</span> <span class="s">"Memory block "</span> <span class="o">&lt;&lt;</span> <span class="n">cBlocksAllocated</span><span class="o">--</span>
          <span class="o">&lt;&lt;</span> <span class="s">" deallocated</span><span class="se">\n</span><span class="s">"</span><span class="p">;</span>
      <span class="n">fInOpDelete</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span>
   <span class="p">}</span>

   <span class="n">free</span><span class="p">(</span> <span class="n">pvMem</span> <span class="p">);</span>
<span class="p">}</span>

<span class="kt">int</span> <span class="n">main</span><span class="p">(</span> <span class="kt">int</span> <span class="n">argc</span><span class="p">,</span> <span class="kt">char</span> <span class="o">*</span><span class="n">argv</span><span class="p">[]</span> <span class="p">)</span> <span class="p">{</span>
   <span class="n">fLogMemory</span> <span class="o">=</span> <span class="mi">1</span><span class="p">;</span>   <span class="c1">// Turn logging on
</span>   <span class="k">if</span><span class="p">(</span> <span class="n">argc</span> <span class="o">&gt;</span> <span class="mi">1</span> <span class="p">)</span>
      <span class="k">for</span><span class="p">(</span> <span class="kt">int</span> <span class="n">i</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span> <span class="n">i</span> <span class="o">&lt;</span> <span class="n">atoi</span><span class="p">(</span> <span class="n">argv</span><span class="p">[</span><span class="mi">1</span><span class="p">]</span> <span class="p">);</span> <span class="o">++</span><span class="n">i</span> <span class="p">)</span> <span class="p">{</span>
         <span class="kt">char</span> <span class="o">*</span><span class="n">pMem</span> <span class="o">=</span> <span class="k">new</span> <span class="kt">char</span><span class="p">[</span><span class="mi">10</span><span class="p">];</span>
         <span class="k">delete</span><span class="p">[]</span> <span class="n">pMem</span><span class="p">;</span>
      <span class="p">}</span>
   <span class="n">fLogMemory</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span>  <span class="c1">// Turn logging off.
</span>   <span class="k">return</span> <span class="n">cBlocksAllocated</span><span class="p">;</span>
<span class="p">}</span>
</code></pre>
</div>

<p>编译并运行这段代码，可以看到如下输出：</p>

<div class="highlighter-rouge"><pre class="highlight"><code>oscar@ubuntu:~/$ g++ -o main spec1_the_operator_delete_function1.cpp -lm
oscar@ubuntu:~/$ ./main 3
Memory block 1 allocated for 10 bytes
Memory block 1 deallocated
Memory block 1 allocated for 10 bytes
Memory block 1 deallocated
Memory block 1 allocated for 10 bytes
Memory block 1 deallocated
</code></pre>
</div>

<p>故事到这里还没有结束，细心的童鞋可能会发现：创建和释放 char* pMem 时，使用的分别是 operator new[] (size_t) 和 operator delete[] (void*),
并没有调用 operator new 和 operator delete。打印的结果却告诉我：operator new 和 operator delete 确实被调用了(作惊恐状)！！！</p>

<p>这里，我找到了 cpluscplus.com 上关于 operator new[] 的表述。不解释，直接上图：</p>

<p><img src="images/20160813-cpp-operator-new%5B%5D.jpg" alt="operator array new" title="operator array new"></p>

<p>关于重新定义 operator new[] 和 operator delete[]，参考 msdn上<a href="https://msdn.microsoft.com/en-us/library/kftdy56f.aspx" title="new and delete">new and delete Operators</a> 
页面最下方类成员函数 operator new[] 和 operator delete[] 的实现，它们是类似的。</p>

<h4 id="operator-new--operator-delete-2">3.2.2 重载类的成员函数 operator new 和 operator delete</h4>

<p>上面我们介绍了重写全局 operator new、operator new[]、operator delete、operator delete[] 的覆盖 (override)。
下面我们看看 类作用域下这四个函数如何实现，应用场景以及注意事项。</p>

<p>在类中重写 operator new/delete([]) 成员函数时，必须声明它们为 static，因此不能声明为虚函数。</p>

<p>下面给出一个重写类 operator new/delete 方法的例子：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// https://msdn.microsoft.com/en-us/library/kftdy56f.aspx
// spec1_the_operator_new_function1.cpp
</span><span class="cp">#include &lt;cstddef&gt;
#include &lt;iostream&gt;
#include &lt;malloc.h&gt;
#include &lt;memory.h&gt; 
</span>
<span class="k">using</span> <span class="k">namespace</span> <span class="n">std</span><span class="p">;</span>

<span class="k">class</span> <span class="nc">Blanks</span>
<span class="p">{</span>
<span class="k">public</span><span class="o">:</span>
    <span class="n">Blanks</span><span class="p">(){</span> <span class="p">}</span>
    <span class="n">Blanks</span><span class="p">(</span><span class="kt">int</span> <span class="n">dummy</span><span class="p">){</span> <span class="k">throw</span> <span class="mi">1</span><span class="p">;</span> <span class="p">}</span>

    <span class="k">static</span> <span class="kt">void</span><span class="o">*</span> <span class="k">operator</span> <span class="k">new</span><span class="p">(</span> <span class="kt">size_t</span> <span class="n">stAllocateBlock</span> <span class="p">);</span>
    <span class="k">static</span> <span class="kt">void</span><span class="o">*</span> <span class="k">operator</span> <span class="k">new</span><span class="p">(</span> <span class="kt">size_t</span> <span class="n">stAllocateBlock</span><span class="p">,</span> <span class="kt">char</span> <span class="n">chInit</span> <span class="p">);</span>
    <span class="k">static</span> <span class="kt">void</span><span class="o">*</span> <span class="k">operator</span> <span class="k">new</span><span class="p">(</span> <span class="kt">size_t</span> <span class="n">stAllocateBlock</span><span class="p">,</span> <span class="kt">double</span> <span class="n">dInit</span> <span class="p">);</span>
    <span class="k">static</span> <span class="kt">void</span> <span class="k">operator</span> <span class="k">delete</span><span class="p">(</span> <span class="kt">void</span><span class="o">*</span> <span class="n">pvMem</span> <span class="p">);</span>
    <span class="k">static</span> <span class="kt">void</span> <span class="k">operator</span> <span class="k">delete</span><span class="p">(</span> <span class="kt">void</span><span class="o">*</span> <span class="n">pvMem</span><span class="p">,</span> <span class="kt">char</span> <span class="n">chInit</span><span class="p">);</span>
    <span class="k">static</span> <span class="kt">void</span> <span class="k">operator</span> <span class="k">delete</span><span class="p">(</span> <span class="kt">void</span><span class="o">*</span> <span class="n">pvMem</span><span class="p">,</span> <span class="kt">double</span> <span class="n">dInit</span><span class="p">);</span>
<span class="p">};</span>

<span class="kt">void</span><span class="o">*</span> <span class="n">Blanks</span><span class="o">::</span><span class="k">operator</span> <span class="k">new</span><span class="p">(</span> <span class="kt">size_t</span> <span class="n">stAllocateBlock</span> <span class="p">)</span> <span class="p">{</span>
    <span class="n">clog</span> <span class="o">&lt;&lt;</span> <span class="s">"Blanks::operator new( size_t )</span><span class="se">\n</span><span class="s">"</span><span class="p">;</span>
    <span class="kt">void</span><span class="o">*</span> <span class="n">pvTemp</span> <span class="o">=</span> <span class="n">malloc</span><span class="p">(</span> <span class="n">stAllocateBlock</span> <span class="p">);</span>
    <span class="k">return</span> <span class="n">pvTemp</span><span class="p">;</span>
<span class="p">}</span>

<span class="kt">void</span><span class="o">*</span> <span class="n">Blanks</span><span class="o">::</span><span class="k">operator</span> <span class="k">new</span><span class="p">(</span> <span class="kt">size_t</span> <span class="n">stAllocateBlock</span><span class="p">,</span> <span class="kt">char</span> <span class="n">chInit</span> <span class="p">)</span>
<span class="p">{</span>
    <span class="n">clog</span> <span class="o">&lt;&lt;</span> <span class="s">"Blanks::operator new( size_t, char )</span><span class="se">\n</span><span class="s">"</span><span class="p">;</span>
    <span class="c1">// throw 20;
</span>    <span class="kt">void</span> <span class="o">*</span><span class="n">pvTemp</span> <span class="o">=</span> <span class="n">malloc</span><span class="p">(</span> <span class="n">stAllocateBlock</span> <span class="p">);</span>
    <span class="k">if</span><span class="p">(</span> <span class="n">pvTemp</span> <span class="o">!=</span> <span class="mi">0</span> <span class="p">)</span>
        <span class="n">memset</span><span class="p">(</span> <span class="n">pvTemp</span><span class="p">,</span> <span class="n">chInit</span><span class="p">,</span> <span class="n">stAllocateBlock</span> <span class="p">);</span>
    <span class="k">return</span> <span class="n">pvTemp</span><span class="p">;</span>
<span class="p">}</span>

<span class="kt">void</span><span class="o">*</span> <span class="n">Blanks</span><span class="o">::</span><span class="k">operator</span> <span class="k">new</span><span class="p">(</span> <span class="kt">size_t</span> <span class="n">stAllocateBlock</span><span class="p">,</span> <span class="kt">double</span> <span class="n">dInit</span> <span class="p">)</span> <span class="p">{</span>
    <span class="n">clog</span> <span class="o">&lt;&lt;</span> <span class="s">"Blanks::operator new( size_t, double)</span><span class="se">\n</span><span class="s">"</span><span class="p">;</span>
    <span class="k">return</span> <span class="n">malloc</span><span class="p">(</span><span class="n">stAllocateBlock</span><span class="p">);</span>
<span class="p">}</span>


<span class="kt">void</span> <span class="n">Blanks</span><span class="o">::</span><span class="k">operator</span> <span class="k">delete</span><span class="p">(</span> <span class="kt">void</span><span class="o">*</span> <span class="n">pvMem</span> <span class="p">)</span> <span class="p">{</span>
    <span class="n">clog</span> <span class="o">&lt;&lt;</span> <span class="s">"Blanks::opeator delete (void*)</span><span class="se">\n</span><span class="s">"</span><span class="p">;</span>
    <span class="n">free</span><span class="p">(</span><span class="n">pvMem</span><span class="p">);</span>
<span class="p">}</span>

<span class="kt">void</span> <span class="n">Blanks</span><span class="o">::</span><span class="k">operator</span> <span class="k">delete</span><span class="p">(</span> <span class="kt">void</span><span class="o">*</span> <span class="n">pvMem</span><span class="p">,</span> <span class="kt">char</span> <span class="n">chInit</span> <span class="p">)</span> <span class="p">{</span>
    <span class="n">clog</span> <span class="o">&lt;&lt;</span> <span class="s">"Blanks::opeator delete (void*, char)</span><span class="se">\n</span><span class="s">"</span><span class="p">;</span>
    <span class="n">free</span><span class="p">(</span><span class="n">pvMem</span><span class="p">);</span>
<span class="p">}</span> 

<span class="kt">void</span> <span class="n">Blanks</span><span class="o">::</span><span class="k">operator</span> <span class="k">delete</span><span class="p">(</span> <span class="kt">void</span><span class="o">*</span> <span class="n">pvMem</span><span class="p">,</span> <span class="kt">double</span> <span class="n">dInit</span> <span class="p">)</span> <span class="p">{</span>
    <span class="n">clog</span> <span class="o">&lt;&lt;</span> <span class="s">"Blanks::opeator delete (void*, double)</span><span class="se">\n</span><span class="s">"</span><span class="p">;</span>
    <span class="n">free</span><span class="p">(</span><span class="n">pvMem</span><span class="p">);</span>
<span class="p">}</span>
<span class="c1">// For discrete objects of type Blanks, the global operator new function
// is hidden. Therefore, the following code allocates an object of type
// Blanks and initializes it to 0xa5
</span><span class="kt">int</span> <span class="n">main</span><span class="p">()</span>
<span class="p">{</span>
   <span class="n">Blanks</span> <span class="o">*</span><span class="n">a5</span> <span class="o">=</span> <span class="k">new</span><span class="p">(</span><span class="sc">'c'</span><span class="p">)</span> <span class="n">Blanks</span><span class="p">;</span>
   <span class="k">delete</span> <span class="n">a5</span><span class="p">;</span>
   <span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="n">endl</span><span class="p">;</span>
   <span class="n">Blanks</span> <span class="o">*</span><span class="n">a6</span> <span class="o">=</span> <span class="k">new</span> <span class="n">Blanks</span><span class="p">;</span>
   <span class="k">delete</span> <span class="n">a6</span><span class="p">;</span>
   <span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="n">endl</span><span class="p">;</span>
   <span class="n">Blanks</span> <span class="o">*</span><span class="n">a7</span> <span class="o">=</span> <span class="k">new</span><span class="p">(</span><span class="mf">10.0</span><span class="p">)</span> <span class="n">Blanks</span><span class="p">(</span><span class="mi">1</span><span class="p">);</span>
   <span class="k">delete</span> <span class="n">a7</span><span class="p">;</span>
   <span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="n">endl</span><span class="p">;</span>
<span class="p">}</span>
</code></pre>
</div>

<p>linux运行上的代码，结果如下：</p>

<div class="highlighter-rouge"><pre class="highlight"><code>Blanks::operator new( size_t, char )
Blanks::opeator delete (void*)

Blanks::operator new( size_t )
Blanks::opeator delete (void*)

Blanks::operator new( size_t, double)
terminate called after throwing an instance of 'int'
Aborted (core dumped)
</code></pre>
</div>

<p>很容易发现，不管我们使用哪个版本的 operator new，最后调用的都是 不含额外的参数的 operator delete。 
构造函数抛出异常时，也没有调用对应的 operator delete 成员函数。
那么包含额外参数的 delete什么时候会被调用到，应用场景由有哪些呢？</p>

<p>我们继续找相关的文档，<a href="">msdn</a>上有这样一段文字：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="kt">void</span> <span class="k">operator</span> <span class="k">delete</span><span class="p">(</span> <span class="kt">void</span> <span class="o">*</span> <span class="p">);</span>
<span class="kt">void</span> <span class="k">operator</span> <span class="k">delete</span><span class="p">(</span> <span class="kt">void</span> <span class="o">*</span><span class="p">,</span> <span class="kt">size_t</span> <span class="p">);</span>
</code></pre>
</div>

<p><em>Only one of the preceding two forms can be present for a given class. The first form takes a single argument of type void *, which contains a pointer to the object to deallocate. The second form—sized deallocation—takes two arguments, the first of which is a pointer to the memory block to deallocate and the second of which is the number of bytes to deallocate. The return type of both forms is void (operator delete cannot return a value).</em></p>

<p><em>The intent of the second form is to speed up searching for the correct size category of the object to be deleted, which is often not stored near the allocation itself and likely uncached; the second form is particularly useful when an operator delete function from a base class is used to delete an object of a derived class.</em></p>

<p>这里的解释也有些问题，通过上面的例子，可以推断 operator new/delete 均可以被重载。
创建对象时，可以使用不同版本的operator new，但是销毁时，只调用不包含额外参数的operator delete。
delete 的应用场景之一是：在继承体系中，Base* 指向一个子类对象，调用 delete 销毁该对象时，必须保证销毁父类对象，而不是根据子类对象的大小进行截断销毁。</p>

<p>事实上，上面所说的应用场景也没有得到验证。我对上面的代码进行了修改，销毁时调用的仍然是不含额外参数的 delete:</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// https://msdn.microsoft.com/en-us/library/kftdy56f.aspx
// spec1_the_operator_new_function1.cpp
</span><span class="cp">#include &lt;cstddef&gt;
#include &lt;iostream&gt;
#include &lt;malloc.h&gt;
#include &lt;memory.h&gt; 
</span>
<span class="k">using</span> <span class="k">namespace</span> <span class="n">std</span><span class="p">;</span>

<span class="k">class</span> <span class="nc">Base</span> <span class="p">{</span>
<span class="k">public</span><span class="o">:</span>
    <span class="k">virtual</span> <span class="o">~</span><span class="n">Base</span><span class="p">()</span> <span class="p">{}</span>
<span class="p">};</span>

<span class="k">class</span> <span class="nc">Blanks</span><span class="o">:</span> <span class="k">public</span> <span class="n">Base</span>
<span class="p">{</span>
  <span class="c1">// ...   没有改变 ...
</span><span class="p">};</span>

<span class="kt">int</span> <span class="nf">main</span><span class="p">()</span> <span class="p">{</span>
   <span class="n">Base</span> <span class="o">*</span><span class="n">a5</span> <span class="o">=</span> <span class="k">new</span><span class="p">(</span><span class="sc">'c'</span><span class="p">)</span> <span class="n">Blanks</span><span class="p">;</span>   <span class="c1">// 打印 Blanks::operator new( size_t, char )
</span>   <span class="k">delete</span> <span class="n">a5</span><span class="p">;</span>                    <span class="c1">// 打印 Blanks::opeator delete (void*)
</span><span class="p">}</span>

</code></pre>
</div>

<p>根据侯捷老师关于 basic_string 的分析，operator delete 并没有传入额外的参数，而是通过 Allocator::deallocate 去删除。
因此 重载 operator delete 没有任何意义，需要时 重新定义 operator delete(void* p)即可。
需要查看 stl 文章和源码的话，可以去 <a href="http://www.codeproject.com/KB/stl/" title="stl">Code Project</a> 和 <a href="https://www.sgi.com/tech/stl/download.html" title="sgi">sgi</a> 网站上查看。</p>

<p>注意：为类定义 operator new/delete 成员函数会覆盖 全局默认的 operator new/delete。
如果要使用默认的 operator new/delete，那么在创建对象和销毁对象时，需要使用 ::new 和 ::delete。</p>

<p>关于 C++ Object Model的分析就到这里，多谢阅读。</p>

  </div>

</article>