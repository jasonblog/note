# 物件模式(1)


<article class="post" itemscope="" itemtype="http://schema.org/BlogPosting">

  <header class="post-header">
    <h1 class="post-title" itemprop="name headline">C++ 对象模型</h1>
    <p class="post-meta"><time datetime="2016-08-07T03:59:00+00:00" itemprop="datePublished">Aug 7, 2016</time></p>
  </header>

  <div class="post-content" itemprop="articleBody">
    <p><em>这是C++语言的一系列文章，内容取自于网易微专业《C++开发工程师（升级版）》。</em></p>

<p><em>本文是听了侯捷老师关于“C++对象模型”的课程以后，总结而成的。</em>
<em>课程中讲到了范型编程和面向对象编程两种模式，因此本文的主题是 template。</em>
<em>包括类模板、函数模板、成员模板、模板特化和偏特化、可变模板参数、模板嵌套等。</em></p>

<p><em>说到面向对象，本文的另一个主题是C++底层的对象模型，包括this指针、虚指针vptr、虚表vtable、虚函数、多态等。</em></p>

<h2 id="c">第三周：C++对象模型</h2>

<h2 id="part-1class-">Part 1：class 的高级用法</h2>

<p>这一部分，我们介绍一些C++ 类的高级用法：conversion function、non-explicit-one-argument constructor、pointer-like
classes、function-like classes。</p>

<h3 id="conversion-function-">1.1 conversion function (转换函数)</h3>

<p>转换函数 可以将对象 默认转换为另一种类型，方便程序的调用。</p>

<p>其作用与只接收一个参数的构造函数（不使用 explicit修饰符）作用正好相反。关于构造函数的坑参考<a href="http://tipsandtricks.runicsoft.com/Cpp/Explicit.html" title="explicit">这篇文章</a>。
下一个小结讲解的就是 non-explicit-one-argument constructor。</p>

<p>废话少说，先上个例子（注意观察它的使用方法）：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// Fraction 类
</span><span class="k">class</span> <span class="nc">Fraction</span> <span class="p">{</span>
<span class="k">public</span><span class="o">:</span>
  <span class="n">Fraction</span><span class="p">(</span><span class="kt">int</span> <span class="n">num</span><span class="p">,</span> <span class="kt">int</span> <span class="n">den</span><span class="o">=</span><span class="mi">1</span><span class="p">)</span><span class="o">:</span> <span class="n">m_numerator</span><span class="p">(</span><span class="n">num</span><span class="p">),</span> <span class="n">m_denominator</span><span class="p">(</span><span class="n">den</span><span class="p">){};</span>
  <span class="k">operator</span> <span class="kt">double</span><span class="p">()</span> <span class="k">const</span> <span class="p">{</span><span class="k">return</span> <span class="p">(</span><span class="kt">double</span><span class="p">)</span><span class="n">m_numerator</span><span class="o">/</span><span class="n">m_denominator</span><span class="p">;</span> <span class="p">};</span>
  <span class="k">private</span><span class="o">:</span>
    <span class="kt">int</span> <span class="n">m_numerator</span><span class="p">;</span>
    <span class="kt">int</span> <span class="n">m_denominator</span><span class="p">;</span>
<span class="p">};</span>

<span class="c1">// main 函数（使用 Fraction 的转换函数 operator double() const)
</span><span class="kt">int</span> <span class="nf">main</span><span class="p">(</span><span class="kt">int</span> <span class="n">argc</span><span class="p">,</span> <span class="kt">char</span> <span class="o">*</span> <span class="n">argv</span><span class="p">[])</span> <span class="p">{</span>
  <span class="n">Fraction</span> <span class="n">f</span><span class="p">(</span><span class="mi">3</span><span class="p">,</span> <span class="mi">5</span><span class="p">);</span>
  <span class="kt">double</span> <span class="n">d</span> <span class="o">=</span> <span class="mf">4.0</span> <span class="o">+</span> <span class="n">f</span><span class="p">;</span>      <span class="c1">// f 调用 operator double() 转换为一个 double 值 
</span>  <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">"d = "</span> <span class="o">&lt;&lt;</span> <span class="n">d</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">endl</span><span class="p">;</span>
<span class="p">}</span>
</code></pre>
</div>

<p>编译器认为必要时，将对象转换为某种特定的类型。转换函数的三个特征：</p>

<ol>
  <li>转换函数 的声明中不需要写返回类型 (因为返回类型和函数名称相同)</li>
  <li>函数名必须是 operator xxx</li>
  <li>必须使用 const 修饰符，因为转换函数不会修改 this 指针</li>
</ol>

<h3 id="non-explicit-one-argument-constructor">1.2 non-explicit-one-argument constructor</h3>

<p>non-explicit-one-argument constructor 有下面两个语法特征：</p>

<ol>
  <li>这类构造函数只接收一个参数</li>
  <li>没有使用 explicit 修饰符 （默认 implicit）</li>
</ol>

<p>该函数在编译器认为需要的时候，将参数类型的变量转换成该类型的对象。具体看下面代码：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// Fraction 类
</span><span class="k">class</span> <span class="nc">Fraction</span> <span class="p">{</span>
  <span class="k">public</span><span class="o">:</span>
    <span class="n">Fraction</span><span class="p">(</span><span class="kt">int</span> <span class="n">num</span><span class="p">,</span> <span class="kt">int</span> <span class="n">den</span><span class="o">=</span><span class="mi">1</span><span class="p">)</span><span class="o">:</span> <span class="n">m_numerator</span><span class="p">(</span><span class="n">num</span><span class="p">),</span> <span class="n">m_denominator</span><span class="p">(</span><span class="n">den</span><span class="p">){};</span>
    <span class="n">Fraction</span> <span class="k">operator</span><span class="o">+</span><span class="p">(</span><span class="k">const</span> <span class="n">Fraction</span><span class="o">&amp;</span> <span class="n">f</span><span class="p">)</span> <span class="p">{</span>
      <span class="k">return</span> <span class="n">Fraction</span><span class="p">(</span><span class="n">m_denominator</span> <span class="o">*</span> <span class="n">f</span><span class="p">.</span><span class="n">m_numerator</span> <span class="o">+</span> <span class="n">m_numerator</span> <span class="o">*</span> <span class="n">f</span><span class="p">.</span><span class="n">m_denominator</span><span class="p">,</span> <span class="n">m_denominator</span> <span class="o">*</span> <span class="n">f</span><span class="p">.</span><span class="n">m_denominator</span><span class="p">);</span> 
    <span class="p">};</span>

    <span class="k">friend</span> <span class="n">ostream</span><span class="o">&amp;</span> <span class="k">operator</span> <span class="o">&lt;&lt;</span> <span class="p">(</span><span class="n">ostream</span><span class="o">&amp;</span> <span class="n">os</span><span class="p">,</span> <span class="k">const</span> <span class="n">Fraction</span><span class="o">&amp;</span> <span class="n">f</span><span class="p">);</span>
  <span class="k">private</span><span class="o">:</span>
    <span class="kt">int</span> <span class="n">m_numerator</span><span class="p">;</span>
    <span class="kt">int</span> <span class="n">m_denominator</span><span class="p">;</span>
<span class="p">};</span>

<span class="n">ostream</span><span class="o">&amp;</span> <span class="k">operator</span><span class="o">&lt;&lt;</span><span class="p">(</span><span class="n">ostream</span><span class="o">&amp;</span> <span class="n">os</span><span class="p">,</span> <span class="k">const</span> <span class="n">Fraction</span><span class="o">&amp;</span> <span class="n">f</span><span class="p">)</span> <span class="p">{</span>
  <span class="k">return</span> <span class="n">os</span> <span class="o">&lt;&lt;</span> <span class="n">f</span><span class="p">.</span><span class="n">m_numerator</span> <span class="o">&lt;&lt;</span> <span class="s">"/"</span> <span class="o">&lt;&lt;</span> <span class="n">f</span><span class="p">.</span><span class="n">m_denominator</span><span class="p">;</span>
<span class="p">}</span>

<span class="c1">// main 函数（注意变量 d 的类型）
</span><span class="kt">int</span> <span class="n">main</span><span class="p">(</span><span class="kt">int</span> <span class="n">argc</span><span class="p">,</span> <span class="kt">char</span> <span class="o">*</span> <span class="n">argv</span><span class="p">[])</span> <span class="p">{</span>
  <span class="n">Fraction</span> <span class="n">f</span><span class="p">(</span><span class="mi">3</span><span class="p">,</span> <span class="mi">5</span><span class="p">);</span>
  <span class="n">Fraction</span> <span class="n">d</span> <span class="o">=</span>  <span class="n">f</span> <span class="o">+</span> <span class="mi">4</span><span class="p">;</span>  <span class="c1">// here 4 is converted to a Fraction(4, 1), 4+f is not allowed
</span>  <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">"d = "</span> <span class="o">&lt;&lt;</span> <span class="n">d</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">endl</span><span class="p">;</span>
<span class="p">}</span>
</code></pre>
</div>

<p>使用 explicit 修饰构造函数以后，上面的默认转换就会失败。</p>

<h3 id="pointer-like-class-">1.3  pointer-like class (智能指针)</h3>
<p>像指针的类，是指该 class 的对象表现出来像一个指针。这么做的目的是实现一个比指针更强大的结构。
标准库内值了一些指针类，如 std::shared_ptr, std::weak_ptr, std::unique_ptr，具体参考<a href="http://www.cplusplus.com/reference/memory/" title="memory">cplusplus</a></p>

<h4 id="sharedptr-">1.3.1 shared_ptr 智能指针</h4>
<p>实现一个智能指针类，则必须重载 (override) 两个成员函数： operator*() 和 operator-&gt;()。</p>

<p>shared_ptr 代码的抽离出来一部分如下：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// shared_ptr 模板类的定义
</span><span class="k">template</span> <span class="o">&lt;</span><span class="k">typename</span> <span class="n">T</span><span class="o">&gt;</span>
<span class="k">class</span> <span class="nc">shared_ptr</span><span class="p">{</span>
<span class="k">public</span><span class="o">:</span>
  <span class="n">T</span><span class="o">&amp;</span> <span class="k">operator</span><span class="o">*</span><span class="p">()</span> <span class="k">const</span> <span class="p">{</span> <span class="c1">// 解引用
</span>    <span class="k">return</span> <span class="o">*</span><span class="n">px</span><span class="p">;</span>
  <span class="p">}</span>
  <span class="n">T</span><span class="o">*</span> <span class="k">operator</span><span class="o">-&gt;</span><span class="p">()</span> <span class="k">const</span> <span class="p">{</span> <span class="c1">// 取指针，这个方法有点诡异
</span>    <span class="k">return</span> <span class="n">px</span><span class="p">;</span>
  <span class="p">}</span>
  
  <span class="n">shared_ptr</span><span class="p">(</span><span class="n">T</span> <span class="o">*</span><span class="n">p</span><span class="p">)</span><span class="o">:</span> <span class="n">px</span><span class="p">(</span><span class="n">p</span><span class="p">)</span> <span class="p">{}</span>
<span class="k">private</span><span class="o">:</span>
  <span class="n">T</span><span class="o">*</span> <span class="n">px</span><span class="p">;</span>
  <span class="kt">long</span><span class="o">*</span> <span class="n">pn</span><span class="p">;</span>
<span class="p">};</span>

<span class="c1">// Foo 类
</span><span class="k">struct</span> <span class="n">Foo</span> <span class="p">{</span>
  <span class="c1">// 省略其它部分，关注 method 
</span>  <span class="kt">void</span> <span class="n">method</span><span class="p">()</span> <span class="p">{</span> <span class="c1">//... }
</span><span class="p">};</span>

<span class="c1">// 使用 shared_ptr
</span><span class="kt">int</span> <span class="nf">main</span><span class="p">()</span> <span class="p">{</span>
  <span class="n">shared_ptr</span><span class="o">&lt;</span><span class="n">Foo</span><span class="o">&gt;</span> <span class="n">sp</span><span class="p">(</span><span class="k">new</span> <span class="n">Foo</span><span class="p">);</span>

  <span class="n">Foo</span> <span class="n">f</span><span class="p">(</span><span class="o">*</span><span class="n">sp</span><span class="p">);</span>  <span class="c1">// 使用 解引用 操作符 *
</span>  <span class="n">sp</span><span class="o">-&gt;</span><span class="n">method</span><span class="p">();</span>  <span class="c1">// 使用 操作符 -&gt;  ( 等价于 px-&gt;method() )
</span><span class="p">}</span>
</code></pre>
</div>

<p><em>关于 operator -&gt; ，从语法角度上来说，-&gt; 是可重生的，所以下面 main函数中才可以这样使用。</em></p>

<p>很容易发现，除了构造，shared_ptr 在使用上与裸指针几乎没有差别，但是不需要手动释放内存。
当然，仿指针类的能力远不止于自动释放内存，还有很多，这里我们看看标准库中 std::shared_ptr的附加功能。</p>

<p>std::shared_ptr 不仅提供了有限的垃圾回收特性，还提供了内存拥有权的管理 (ownership)，点击<a href="http://www.cplusplus.com/reference/memory/shared_ptr/?kw=shared_ptr" title="shared_ptr">这里查看详情</a></p>

<h4 id="iterator-">1.3.2 iterator 迭代器</h4>

<p>pointer-like classes 在迭代器中也有广泛的应用。
标准库中所有的容器(std::vector等) 都有迭代器。换句话说，标准库的迭代器也实现了 operator* 和 operator-&gt; 方法。</p>

<p>每个迭代器对象 指向 一个容器变量，但同时实现了下面几个方法：</p>

<ol>
  <li>operator==</li>
  <li>operator!=</li>
  <li>operator++</li>
  <li>operator–</li>
</ol>

<p>关于 迭代器中 operator* 和 operator-&gt; 的实现，也相当值得考究：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// 忽略上下文
</span>
<span class="n">reference</span> <span class="k">operator</span><span class="o">*</span><span class="p">()</span> <span class="k">const</span> <span class="p">{</span>
  <span class="k">return</span> <span class="p">(</span><span class="o">*</span><span class="n">node</span><span class="p">).</span><span class="n">data</span><span class="p">;</span>
<span class="p">}</span>

<span class="n">pointer</span> <span class="k">operator</span><span class="o">-&gt;</span><span class="p">()</span> <span class="k">const</span> <span class="p">{</span> <span class="c1">// 借助于 operator* 实现
</span>  <span class="k">return</span> <span class="o">&amp;</span><span class="p">(</span><span class="k">operator</span><span class="o">*</span><span class="p">());</span>
<span class="p">}</span>
</code></pre>
</div>

<p>你可以像下面这样使用这两个方法：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="n">list</span><span class="o">&lt;</span><span class="n">Foo</span><span class="o">&gt;::</span><span class="n">iterator</span> <span class="n">ite</span><span class="p">;</span>

<span class="c1">//... 省略一部分代码...
</span>
<span class="o">*</span><span class="n">ite</span><span class="p">;</span>   <span class="c1">// 获取 Foo 对象的引用
</span>
<span class="n">ite</span><span class="o">-&gt;</span><span class="n">method</span><span class="p">();</span>  
<span class="c1">// 意思是 调用 Foo::method()
// 相当于 (*ite).method();
// 相当于 (&amp;(*ite))-&gt;method();
</span>
</code></pre>
</div>

<h3 id="function-like-classes-">1.4 function-like classes (仿函数)</h3>

<h4 id="section">1.4.1 什么是仿函数？</h4>
<p>仿函数其实不是函数，是一个类，但是它的行为和函数类似。在实现的层面上，一个类一旦定义了 operator() 方法，就可以称之为仿函数。</p>

<p>C++标准库内置了很多<a href="http://www.cplusplus.com/reference/functional/," title="function">仿函数模板</a>。
我们先用 std::less 和 std::less_equal 为例，对仿函数的用法有一个直观的认识：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// less example (http://www.cplusplus.com/reference/functional/less/)
// compile: g++ -o main main.cpp -lm
</span><span class="cp">#include &lt;iostream&gt;     // std::cout
#include &lt;functional&gt;   // std::less
#include &lt;algorithm&gt;    // std::sort, std::includes
</span>
<span class="kt">int</span> <span class="nf">main</span> <span class="p">()</span> <span class="p">{</span>
  <span class="c1">// 自己写的简单例子, 表达式 "std::less&lt;int&gt;()" 创建了一个临时对象  
</span>  <span class="kt">int</span> <span class="n">a</span> <span class="o">=</span> <span class="mi">5</span><span class="p">,</span> <span class="n">b</span> <span class="o">=</span> <span class="mi">4</span><span class="p">;</span>
  <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">"std::less&lt;int&gt;()("</span> <span class="o">&lt;&lt;</span> <span class="n">a</span> <span class="o">&lt;&lt;</span> <span class="s">", "</span> <span class="o">&lt;&lt;</span> <span class="n">b</span> <span class="o">&lt;&lt;</span> <span class="s">"): "</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">less</span><span class="o">&lt;</span><span class="kt">int</span><span class="o">&gt;</span><span class="p">()(</span><span class="n">a</span><span class="p">,</span> <span class="n">b</span><span class="p">)</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">endl</span><span class="p">;</span>
  <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">"std::less&lt;int&gt;()("</span> <span class="o">&lt;&lt;</span> <span class="n">b</span> <span class="o">&lt;&lt;</span> <span class="s">", "</span> <span class="o">&lt;&lt;</span> <span class="n">a</span> <span class="o">&lt;&lt;</span> <span class="s">"): "</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">less</span><span class="o">&lt;</span><span class="kt">int</span><span class="o">&gt;</span><span class="p">()(</span><span class="n">b</span><span class="p">,</span> <span class="n">a</span><span class="p">)</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">endl</span><span class="p">;</span>

  <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">"std::less_equal&lt;int&gt;()("</span> <span class="o">&lt;&lt;</span> <span class="n">a</span> <span class="o">&lt;&lt;</span> <span class="s">", "</span> <span class="o">&lt;&lt;</span> <span class="n">b</span> <span class="o">&lt;&lt;</span> <span class="s">"): "</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">less_equal</span><span class="o">&lt;</span><span class="kt">int</span><span class="o">&gt;</span><span class="p">()(</span><span class="n">a</span><span class="p">,</span> <span class="n">b</span><span class="p">)</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">endl</span><span class="p">;</span>
  <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">"std::less_equal&lt;int&gt;()("</span> <span class="o">&lt;&lt;</span> <span class="n">b</span> <span class="o">&lt;&lt;</span> <span class="s">", "</span> <span class="o">&lt;&lt;</span> <span class="n">a</span> <span class="o">&lt;&lt;</span> <span class="s">"): "</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">less_equal</span><span class="o">&lt;</span><span class="kt">int</span><span class="o">&gt;</span><span class="p">()(</span><span class="n">b</span><span class="p">,</span> <span class="n">a</span><span class="p">)</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">endl</span><span class="p">;</span>
  <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">"std::less_equal&lt;int&gt;()("</span> <span class="o">&lt;&lt;</span> <span class="n">a</span> <span class="o">&lt;&lt;</span> <span class="s">", "</span> <span class="o">&lt;&lt;</span> <span class="n">a</span> <span class="o">&lt;&lt;</span> <span class="s">"): "</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">less_equal</span><span class="o">&lt;</span><span class="kt">int</span><span class="o">&gt;</span><span class="p">()(</span><span class="n">a</span><span class="p">,</span> <span class="n">a</span><span class="p">)</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">endl</span><span class="p">;</span>

  <span class="c1">// 网站上带的高级例子
</span>  <span class="kt">int</span> <span class="n">foo</span><span class="p">[]</span><span class="o">=</span><span class="p">{</span><span class="mi">10</span><span class="p">,</span><span class="mi">20</span><span class="p">,</span><span class="mi">5</span><span class="p">,</span><span class="mi">15</span><span class="p">,</span><span class="mi">25</span><span class="p">};</span>
  <span class="kt">int</span> <span class="n">bar</span><span class="p">[]</span><span class="o">=</span><span class="p">{</span><span class="mi">15</span><span class="p">,</span><span class="mi">10</span><span class="p">,</span><span class="mi">20</span><span class="p">};</span>
  <span class="n">std</span><span class="o">::</span><span class="n">sort</span> <span class="p">(</span><span class="n">foo</span><span class="p">,</span> <span class="n">foo</span><span class="o">+</span><span class="mi">5</span><span class="p">,</span> <span class="n">std</span><span class="o">::</span><span class="n">less</span><span class="o">&lt;</span><span class="kt">int</span><span class="o">&gt;</span><span class="p">());</span>  <span class="c1">// 5 10 15 20 25
</span>  <span class="n">std</span><span class="o">::</span><span class="n">sort</span> <span class="p">(</span><span class="n">bar</span><span class="p">,</span> <span class="n">bar</span><span class="o">+</span><span class="mi">3</span><span class="p">,</span> <span class="n">std</span><span class="o">::</span><span class="n">less</span><span class="o">&lt;</span><span class="kt">int</span><span class="o">&gt;</span><span class="p">());</span>  <span class="c1">//   10 15 20
</span>  <span class="k">if</span> <span class="p">(</span><span class="n">std</span><span class="o">::</span><span class="n">includes</span> <span class="p">(</span><span class="n">foo</span><span class="p">,</span> <span class="n">foo</span><span class="o">+</span><span class="mi">5</span><span class="p">,</span> <span class="n">bar</span><span class="p">,</span> <span class="n">bar</span><span class="o">+</span><span class="mi">3</span><span class="p">,</span> <span class="n">std</span><span class="o">::</span><span class="n">less</span><span class="o">&lt;</span><span class="kt">int</span><span class="o">&gt;</span><span class="p">()))</span>
    <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">"foo includes bar.</span><span class="se">\n</span><span class="s">"</span><span class="p">;</span>
  <span class="k">return</span> <span class="mi">0</span><span class="p">;</span>
<span class="p">}</span>
</code></pre>
</div>

<h4 id="section-1">1.4.2 仿函数的实现</h4>

<p>仿函数实际上是一个 类 (class)，这个类实现了 operator() 方法。
下面这个是 C++11 中 std::less 的实现：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// C++11 中的实现（侯捷老师讲的是 C++98中的实现）
</span><span class="k">template</span> <span class="o">&lt;</span><span class="k">class</span> <span class="nc">T</span><span class="o">&gt;</span> <span class="k">struct</span> <span class="n">less</span> <span class="p">{</span>
  <span class="kt">bool</span> <span class="k">operator</span><span class="p">()</span> <span class="p">(</span><span class="k">const</span> <span class="n">T</span><span class="o">&amp;</span> <span class="n">x</span><span class="p">,</span> <span class="k">const</span> <span class="n">T</span><span class="o">&amp;</span> <span class="n">y</span><span class="p">)</span> <span class="k">const</span> <span class="p">{</span><span class="k">return</span> <span class="n">x</span><span class="o">&lt;</span><span class="n">y</span><span class="p">;}</span>
  <span class="k">typedef</span> <span class="n">T</span> <span class="n">first_argument_type</span><span class="p">;</span>
  <span class="k">typedef</span> <span class="n">T</span> <span class="n">second_argument_type</span><span class="p">;</span>
  <span class="k">typedef</span> <span class="kt">bool</span> <span class="n">result_type</span><span class="p">;</span>
<span class="p">};</span>
</code></pre>
</div>

<p>注意：std::less 是类模板。在课程中，侯捷老师提到了 unary_function 和 binary_function，这两个类定义了参数类型，C++11中已经不再
使用，而是内置到 std::less 中，具体参考<a href="http://www.cplusplus.com/reference/functional/less/" title="less">这里</a></p>

<h4 id="namespace">1.5 命名空间 (namespace)</h4>
<p>命名空间用于 模块分离和解耦。为了更好地说明一些细节，这里使用从 <a href="https://msdn.microsoft.com/en-us/library/5cb46ksf.aspx" title="msdn">msdn</a> 摘取一段话：</p>

<p><code class="highlighter-rouge">A namespace is a declarative region that provides a scope to the identifiers (the names of types, functions, variables, etc) inside it.</code></p>

<p><code class="highlighter-rouge">Namespaces are used to organize code into logical groups and to prevent name collisions that can occur especially when your code base includes multiple libraries.</code></p>

<p><code class="highlighter-rouge">All identifiers at namespace scope are visible to one another without qualification.</code></p>

<p><code class="highlighter-rouge">Identifiers outside the namespace can access the members by using the fully qualified name for each identifier, for example std::vector&lt;std::string&gt; vec;, or else by a using Declaration for a single identifier (using std::string), or a using Directive (C++) for all the identifiers in the namespace (using namespace std;).</code></p>

<p><code class="highlighter-rouge">Code in header files should always use the fully qualified namespace name.</code></p>

<h2 id="part-2--template">Part 2 模板 (template)</h2>

<h3 id="class-template-">2.1 class template (类模板)</h3>

<p>前面几篇博客对类模板有所涉及，这里不再赘述。C++标准库的容器都是类模板的范例，比如：</p>

<ol>
  <li><a href="http://www.cplusplus.com/reference/vector/" title="vector">std::vector</a></li>
  <li><a href="http://www.cplusplus.com/reference/stack/" title="stack">std::stack</a></li>
  <li><a href="http://www.cplusplus.com/reference/stack/" title="array">std::array</a></li>
  <li><a href="http://www.cplusplus.com/reference/map/" title="map">std::map</a></li>
  <li><a href="http://www.cplusplus.com/reference/," title="etc">and so on</a></li>
</ol>

<h3 id="function-template-">2.2 function template (函数模板)</h3>

<p>对于 function template ，前面几篇博客也都有所涉及。C++标准库 algorithm 分类下有将近 90 个函数模板，
这里我列出几个：</p>

<ol>
  <li><a href="http://www.cplusplus.com/reference/algorithm/min/" title="min">std::min</a></li>
  <li><a href="http://www.cplusplus.com/reference/algorithm/max/" title="max">std::max</a></li>
  <li><a href="http://www.cplusplus.com/reference/algorithm/minmax/" title="minmax">std::minmax</a></li>
  <li><a href="http://www.cplusplus.com/reference/algorithm/sort/" title="sort">std::sort</a></li>
  <li><a href="http://www.cplusplus.com/reference/algorithm/copy/" title="copy">std::copy</a></li>
  <li><a href="http://www.cplusplus.com/reference/algorithm/for_each/" title="for_each">std::for_each</a></li>
  <li><a href="http://www.cplusplus.com/reference/algorithm/" title="algorithm">and so on</a></li>
</ol>

<p>下面我们以 std::for_each 为例，看下如何使用函数模板：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// for_each example (来源：http://www.cplusplus.com/reference/algorithm/for_each/)
</span><span class="cp">#include &lt;iostream&gt;     // std::cout
#include &lt;algorithm&gt;    // std::for_each
#include &lt;vector&gt;       // std::vector
</span>
<span class="kt">void</span> <span class="nf">myfunction</span> <span class="p">(</span><span class="kt">int</span> <span class="n">i</span><span class="p">)</span> <span class="p">{</span>  <span class="c1">// function:
</span>  <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="sc">' '</span> <span class="o">&lt;&lt;</span> <span class="n">i</span><span class="p">;</span>
<span class="p">}</span>

<span class="k">struct</span> <span class="n">myclass</span> <span class="p">{</span>           <span class="c1">// function object type:
</span>  <span class="kt">void</span> <span class="k">operator</span><span class="p">()</span> <span class="p">(</span><span class="kt">int</span> <span class="n">i</span><span class="p">)</span> <span class="p">{</span><span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="sc">' '</span> <span class="o">&lt;&lt;</span> <span class="n">i</span><span class="p">;}</span>
<span class="p">}</span> <span class="n">myobject</span><span class="p">;</span>

<span class="kt">int</span> <span class="nf">main</span> <span class="p">()</span> <span class="p">{</span>
  <span class="n">std</span><span class="o">::</span><span class="n">vector</span><span class="o">&lt;</span><span class="kt">int</span><span class="o">&gt;</span> <span class="n">myvector</span><span class="p">;</span>
  <span class="n">myvector</span><span class="p">.</span><span class="n">push_back</span><span class="p">(</span><span class="mi">10</span><span class="p">);</span>
  <span class="n">myvector</span><span class="p">.</span><span class="n">push_back</span><span class="p">(</span><span class="mi">20</span><span class="p">);</span>
  <span class="n">myvector</span><span class="p">.</span><span class="n">push_back</span><span class="p">(</span><span class="mi">30</span><span class="p">);</span>

  <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">"myvector contains:"</span><span class="p">;</span>
  <span class="n">for_each</span> <span class="p">(</span><span class="n">myvector</span><span class="p">.</span><span class="n">begin</span><span class="p">(),</span> <span class="n">myvector</span><span class="p">.</span><span class="n">end</span><span class="p">(),</span> <span class="n">myfunction</span><span class="p">);</span>
  <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="sc">'\n'</span><span class="p">;</span>

  <span class="c1">// or:
</span>  <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">"myvector contains:"</span><span class="p">;</span>
  <span class="n">for_each</span> <span class="p">(</span><span class="n">myvector</span><span class="p">.</span><span class="n">begin</span><span class="p">(),</span> <span class="n">myvector</span><span class="p">.</span><span class="n">end</span><span class="p">(),</span> <span class="n">myobject</span><span class="p">);</span>
  <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="sc">'\n'</span><span class="p">;</span>

  <span class="k">return</span> <span class="mi">0</span><span class="p">;</span>
<span class="p">}</span>
</code></pre>
</div>

<p>在这个例子中，注意函数 myfunction 和 仿函数 myobject 的用法，think twice about that。</p>

<p>另外，使用函数模板时，不需要指定特化类型，因为编译器会根据参数进行自动推导。</p>

<h3 id="member-method-">2.3 Member method (成员模板，默认为成员函数模板)</h3>

<p>从使用者的角度来看，成员模板 比 类模板 具有更大的自由度。由于C++强大的继承机制，成员模板也有一些使用场景。
这里以 shared_ptr 为例：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// 定义 类模板 shared_ptr
</span><span class="k">template</span> <span class="o">&lt;</span><span class="k">typename</span> <span class="n">_Tp</span><span class="o">&gt;</span>
<span class="k">class</span> <span class="nc">shared_ptr</span> <span class="o">:</span> <span class="n">pubic</span> <span class="n">__shared_ptr</span><span class="o">&lt;</span><span class="n">_Tp</span><span class="o">&gt;</span> <span class="p">{</span>
  <span class="c1">//... 省略代码 ...
</span>
  <span class="k">template</span> <span class="o">&lt;</span><span class="k">typename</span> <span class="n">_Tp1</span><span class="o">&gt;</span>
  <span class="k">explicit</span> <span class="n">shared_ptr</span><span class="p">(</span><span class="n">_Tp1</span><span class="o">*</span> <span class="n">__p</span><span class="p">)</span> <span class="o">:</span> <span class="n">__shared_ptr</span><span class="o">&lt;</span><span class="n">_TP</span><span class="o">&gt;</span><span class="p">(</span><span class="n">__p</span><span class="p">)</span> <span class="p">{}</span>

  <span class="c1">// ... 省略代码 ...
</span><span class="p">};</span>

<span class="c1">// 使用 shared_ptr 的模板构造函数
// Derived1 类是 Base1 的子类
</span><span class="kt">int</span> <span class="nf">main</span><span class="p">()</span> <span class="p">{</span>
  <span class="n">Base1</span> <span class="o">*</span><span class="n">ptr</span> <span class="o">=</span> <span class="k">new</span> <span class="n">Derived1</span><span class="p">;</span>  <span class="c1">// 向上转型
</span>
  <span class="n">shared_ptr</span><span class="o">&lt;</span><span class="n">Base1</span><span class="o">&gt;</span> <span class="n">sptr</span><span class="p">(</span><span class="k">new</span> <span class="n">Derived1</span><span class="p">);</span>  <span class="c1">// 支持向上转型
</span><span class="p">}</span>
</code></pre>
</div>

<p>这个例子中，成员模板允许 shared_ptr 支持接收子类对象的指针，构造一个父类shared_ptr。</p>

<h3 id="specialization-">2.4 specialization (模板特化)</h3>

<p>模板本身是泛化的，允许用户在使用时进行特化。所谓“特化”，其实是指 在编译器的展开。
但是模板的设计有时候不能满足所有特化类型的要求，比如 std::vector 容纳 bool 时会有问题，
所有有了 <code class="highlighter-rouge">std::vector&lt;bool&gt;</code> 的特化版本。</p>

<h4 id="section-2">2.4.1 模板偏特化</h4>

<p>模板偏特化 可以分为两类：</p>

<ol>
  <li>个数上的“偏”</li>
</ol>

<p>例如 <code class="highlighter-rouge">std::vector&lt;int, typename Alloc=.....&gt;</code> 相对于 <code class="highlighter-rouge">std::vector&lt;typename T, typename Alloc=......&gt;</code></p>

<ol>
  <li>类型上的“偏” (由对象扩展到 指针类型)</li>
</ol>

<p>这里直接看一个例子：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// 泛化版本
</span><span class="k">template</span> <span class="o">&lt;</span><span class="k">typename</span> <span class="n">T</span><span class="o">&gt;</span>
<span class="k">class</span> <span class="nc">C</span> <span class="p">{</span>
  <span class="c1">//... 
</span><span class="p">};</span>

<span class="c1">// 扩展到指针的 特化版本
</span><span class="k">template</span> <span class="o">&lt;</span><span class="k">typename</span> <span class="n">T</span><span class="o">&gt;</span>
<span class="k">class</span> <span class="nc">C</span><span class="o">&lt;</span><span class="n">T</span><span class="o">*&gt;</span> <span class="p">{</span>
  <span class="c1">//...
</span><span class="p">};</span>

<span class="c1">// 使用 特化版本
</span><span class="kt">int</span> <span class="nf">main</span><span class="p">()</span> <span class="p">{</span>
  <span class="n">C</span><span class="o">&lt;</span><span class="n">string</span><span class="o">&gt;</span> <span class="n">obj1</span><span class="p">;</span>    <span class="c1">// 正常的特化版本
</span>  <span class="n">C</span><span class="o">&lt;</span><span class="n">string</span><span class="o">*&gt;</span> <span class="n">obj2</span><span class="p">;</span>   <span class="c1">// 特化的指针版本
</span><span class="p">}</span>
</code></pre>
</div>

<h4 id="section-3">2.4.2 模板模板参数（模板嵌套)</h4>
<p>模板模板参数是指 一个模板作为另一个模板的参数存在。这样的设计在使用上更为灵活。这里直接上一个例子：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// 定义一个类模板，它使用一个模板作为模板参数
</span><span class="k">template</span> <span class="o">&lt;</span><span class="k">typename</span> <span class="n">T</span><span class="p">,</span>
  <span class="k">template</span> <span class="o">&lt;</span><span class="k">typename</span> <span class="n">T</span><span class="o">&gt;</span>
  <span class="k">class</span> <span class="nc">Container</span>
<span class="o">&gt;</span>
<span class="k">class</span> <span class="nc">XCls</span> <span class="p">{</span>
<span class="k">private</span><span class="o">:</span>
  <span class="n">Container</span><span class="o">&lt;</span><span class="n">T</span><span class="o">&gt;</span> <span class="n">c</span><span class="p">;</span>
<span class="k">public</span><span class="o">:</span>
  <span class="c1">// ...
</span><span class="p">};</span>

<span class="c1">// 定义Lst
</span><span class="k">template</span><span class="o">&lt;</span><span class="k">typename</span> <span class="n">T</span><span class="o">&gt;</span>
<span class="k">using</span> <span class="n">Lst</span> <span class="o">=</span> <span class="n">list</span><span class="o">&lt;</span><span class="n">T</span><span class="p">,</span> <span class="n">allocator</span><span class="o">&lt;</span><span class="n">T</span><span class="o">&gt;</span> <span class="o">&gt;</span><span class="p">;</span>     <span class="c1">// 注意： Lst 只有一个模板参数，而 list 有两个模板参数
// 使用该模板
</span><span class="kt">int</span> <span class="nf">main</span><span class="p">()</span> <span class="p">{</span>
  <span class="n">XCls</span><span class="o">&lt;</span><span class="n">string</span><span class="p">,</span> <span class="n">list</span><span class="o">&gt;</span> <span class="n">mylist1</span><span class="p">;</span>   <span class="c1">// 合法的定义
</span>
<span class="c1">//XCls&lt;string, Lst&gt; mylist2;    // 不合法，因为 XCls 的第二个模板参数只接受一个参数（有点绕，think about it）
</span><span class="p">}</span>
</code></pre>
</div>

<p>这个模板的灵活性在于，第二个模板参数，你可以使用 std::list, std::stack, std::vector 
等迭代器的特化版本作为参数，也就是说底层可以接入不同的“内存管理方案”（这个词相对准确）。</p>

<h2 id="part-3c">Part 3：C++语言层面的相关主题</h2>

<h3 id="c-1">3.1 C++标准库概论</h3>

<p>这里用一张图表示</p>

<p><img src="http://obi1zst3q.bkt.clouddn.com/20160808_C++%20%E6%A0%87%E5%87%86%E5%BA%93%20%E5%85%A8%E5%9B%BE" alt="stl" title="stl"></p>

<h3 id="variadic-templates-c11">3.2 variadic templates：模板的可变参数列表 (C++11)</h3>

<p>模板的可变参数列表与 正常的可变参数列表是一样的，只是语法上有些特殊。
下面是一个 print 的例子：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// 定义 print 函数
</span><span class="kt">void</span> <span class="nf">print</span><span class="p">()</span> <span class="p">{}</span>

<span class="k">template</span><span class="o">&lt;</span><span class="k">typename</span> <span class="n">T</span><span class="p">,</span> <span class="k">typename</span><span class="p">...</span> <span class="n">Types</span><span class="o">&gt;</span>
<span class="kt">void</span> <span class="n">print</span><span class="p">(</span><span class="k">const</span> <span class="n">T</span><span class="o">&amp;</span> <span class="n">firstArg</span><span class="p">,</span> <span class="k">const</span> <span class="n">Types</span><span class="o">&amp;</span><span class="p">...</span> <span class="n">args</span><span class="p">)</span> <span class="p">{</span>
  <span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="n">firstArg</span> <span class="o">&lt;&lt;</span> <span class="n">endl</span><span class="p">;</span>
  <span class="n">print</span><span class="p">(</span><span class="n">args</span><span class="p">...);</span>
<span class="p">}</span>

<span class="c1">// 使用 print 函数
</span><span class="kt">int</span> <span class="n">main</span><span class="p">()</span> <span class="p">{</span>
  <span class="n">print</span><span class="p">(</span><span class="mf">7.5</span><span class="p">,</span> <span class="s">"hello"</span><span class="p">,</span> <span class="n">bitset</span><span class="o">&lt;</span><span class="mi">16</span><span class="o">&gt;</span><span class="p">(</span><span class="mi">377</span><span class="p">),</span><span class="mi">42</span><span class="p">);</span>
<span class="p">}</span>
</code></pre>
</div>

<p>另外，对于模板参数，C++ 提供了辅助函数，用来获取可变参数列表的长度，函数签名为 <code class="highlighter-rouge">size_type sizeof...(args)</code>。</p>

<h3 id="auto-c11">3.3 auto (C++11)</h3>

<p>auto 允许用户不声明变量的类型，而是留给编译器去推导。它是C++11加入的语法糖，可以减少有效代码量。</p>

<p>关于 auto，更多细节参考 <a href="https://msdn.microsoft.com/en-us/library/dd293667.aspx" title="msdn">msdn</a></p>

<h3 id="range-based-for-c11">3.4 range-based for (c++11)</h3>

<p>这是C++11 新增加的语法，可以有效减少代码量，与 auto 配合使用更佳。考虑到是否需要修改数组的值，决定是否采用引用，看代码：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="n">vector</span><span class="o">&lt;</span><span class="kt">double</span><span class="o">&gt;</span> <span class="n">vec</span><span class="p">;</span>
<span class="k">for</span> <span class="p">(</span><span class="k">auto</span> <span class="n">elem</span><span class="o">:</span> <span class="n">vec</span><span class="p">)</span> <span class="p">{</span>  <span class="c1">// 按值传递，不会修改数组的值
</span>  <span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="n">elem</span> <span class="o">&lt;&lt;</span> <span class="n">endl</span><span class="p">;</span>  
  <span class="n">elem</span> <span class="o">*=</span> <span class="mi">3</span><span class="p">;</span>   <span class="c1">// 即便这样写， 也只是修改了一个副本，不会修改 vec 的值。
</span><span class="p">}</span>

<span class="k">for</span> <span class="p">(</span><span class="k">auto</span><span class="o">&amp;</span> <span class="n">elem</span><span class="o">:</span> <span class="n">vec</span><span class="p">){</span>  <span class="c1">// 按引用传递
</span>  <span class="n">elem</span> <span class="o">*=</span> <span class="mi">3</span><span class="p">;</span>  <span class="c1">// 使用引用会修改数组的值
</span><span class="p">}</span>
</code></pre>
</div>

<p>更多参考 <a href="https://msdn.microsoft.com/en-us/library/jj203382.aspx">msdn上的描述</a>。</p>

<h3 id="reference-">3.5 关于 reference (一些引起误解的地方)</h3>

<h4 id="reference--1">3.5.1 reference 的特征</h4>

<p>reference的两个特征：</p>

<ol>
  <li>reference类型的变量一旦 代表某个变量，就永远不能代表另一个变量</li>
  <li>reference类型的变量  大小和地址 与 原对象相同 (即 sizeof 和 operator&amp; 的返回值)</li>
</ol>

<p>下面用侯捷老师PPT上的一段代码来说明：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="kt">int</span> <span class="nf">main</span><span class="p">()</span> <span class="p">{</span>
  <span class="kt">int</span> <span class="n">x</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span>
  <span class="kt">int</span><span class="o">*</span> <span class="n">p</span> <span class="o">=</span> <span class="o">&amp;</span><span class="n">x</span><span class="p">;</span>
  <span class="kt">int</span><span class="o">&amp;</span> <span class="n">r</span> <span class="o">=</span> <span class="n">x</span><span class="p">;</span>  <span class="c1">// r 代表 x，两者的值都是 0
</span>  <span class="kt">int</span> <span class="n">x2</span> <span class="o">=</span> <span class="mi">5</span><span class="p">;</span>

  <span class="n">r</span> <span class="o">=</span> <span class="n">x2</span><span class="p">;</span>      <span class="c1">// 这一行赋值的结果是：x 的值也变成了 5
</span>  <span class="kt">int</span><span class="o">&amp;</span> <span class="n">r2</span> <span class="o">=</span> <span class="n">r</span><span class="p">;</span> <span class="c1">// r2、r 都代表 x，即值都是 5
</span><span class="p">}</span>
</code></pre>
</div>

<p>上面这个例子中，需要注意：</p>

<ol>
  <li>sizeof(x) == sizeof(r)</li>
  <li>&amp;x == &amp;r</li>
</ol>

<h4 id="section-4">3.5.2 应用场景</h4>

<p>reference 通常用在两个地方：</p>

<ol>
  <li>参数传递 (比较快)</li>
  <li>返回类型</li>
</ol>

<h3 id="section-5">3.6 构造和析构 (时间先后)</h3>

<p>本小节主要讲解构造和析构 在继承和组合体系下的运作机制。</p>

<h4 id="section-6">3.6.1 继承体系中的构造和析构</h4>

<p>构造：由内而外。内是指Base，外指Derived
析构：由外而内。先析构Derived Class，再析构Base Class 的部分</p>

<p>注意：Base Class 的析构函数必须是 virtual 的，否则会报出 undefined behaviors 的错误。
下面这段代码重现了这个错误：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// 这段代码 来源于 stackoverflow ，但是经过了大量修改
// http://stackoverflow.com/questions/461203/when-to-use-virtual-destructors
</span>
<span class="cp">#include &lt;iostream&gt;
</span>
<span class="k">class</span> <span class="nc">Node</span> <span class="p">{</span>
<span class="k">public</span><span class="o">:</span>
    <span class="n">Node</span><span class="p">()</span> <span class="p">{</span> <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">"Node()"</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">endl</span><span class="p">;</span> <span class="p">}</span>
    <span class="o">~</span><span class="n">Node</span><span class="p">()</span> <span class="p">{</span> <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">"~Node()"</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">endl</span><span class="p">;</span> <span class="p">}</span>
<span class="p">};</span>

<span class="k">class</span> <span class="nc">Base</span> 
<span class="p">{</span>
<span class="k">public</span><span class="o">:</span>
    <span class="n">Base</span><span class="p">()</span> <span class="p">{</span> <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">"Base()"</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">endl</span><span class="p">;</span> <span class="p">}</span>
    <span class="o">~</span><span class="n">Base</span><span class="p">()</span> <span class="p">{</span> <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">"~Base()"</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">endl</span><span class="p">;</span> <span class="p">}</span>
<span class="p">};</span>

<span class="k">class</span> <span class="nc">Derived</span> <span class="o">:</span> <span class="k">public</span> <span class="n">Base</span>
<span class="p">{</span>
<span class="k">public</span><span class="o">:</span>
    <span class="n">Derived</span><span class="p">()</span>  <span class="p">{</span> <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">"Derived()"</span>  <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">endl</span><span class="p">;</span>  <span class="n">m_pNode</span> <span class="o">=</span> <span class="k">new</span> <span class="n">Node</span><span class="p">();</span> <span class="p">}</span>
    <span class="o">~</span><span class="n">Derived</span><span class="p">()</span> <span class="p">{</span> <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">"~Derived()"</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">endl</span><span class="p">;</span>  <span class="k">delete</span> <span class="n">m_pNode</span><span class="p">;</span> <span class="p">}</span>

<span class="k">private</span><span class="o">:</span>
    <span class="n">Node</span><span class="o">*</span> <span class="n">m_pNode</span><span class="p">;</span>
<span class="p">};</span>

<span class="kt">int</span> <span class="nf">main</span><span class="p">()</span> <span class="p">{</span>
    <span class="c1">// 注意：Base的析构函数设置为 virtual
</span>    <span class="n">Base</span> <span class="o">*</span><span class="n">b</span> <span class="o">=</span> <span class="k">new</span> <span class="n">Derived</span><span class="p">();</span>
    <span class="c1">// 使用 b
</span>    <span class="k">delete</span> <span class="n">b</span><span class="p">;</span> <span class="c1">// 结果是：调用了 Base的构造函数
</span>
    <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">"execute complete"</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">endl</span><span class="p">;</span>
<span class="p">}</span>


</code></pre>
</div>
<p>上面这段代码打印结果是：</p>

<div class="highlighter-rouge"><pre class="highlight"><code>Base()
Derived()
Node()
~Base() // 为什么只打印了 这个？？？
execute complete
</code></pre>
</div>

<p>注意： 在实际的测试中，代码没有报出 undefined behavior 错误。但是出现了内存泄漏 m_pNode 的内存没有被释放。
关于这段代码的解释，我联想到侯捷老师讲到的静态绑定和动态绑定，网上一张相关的ppt，
点击<a href="http://www.cs.wustl.edu/~schmidt/PDF/C++-dynamic-binding4.pdf" title="vtable">C++-dynamic-binding</a>查看。</p>

<p>然后，我给 ~Base() 和 ~Derived() 都加上了 virtual (这里就不再列出代码)，结果仍然令人疑惑，结果如下：</p>

<div class="highlighter-rouge"><pre class="highlight"><code>Base()
Derived()
Node()
~Derived()
~Node()
~Base()    // 为什么还会打印这个？？？
execute complete
</code></pre>
</div>

<p>又查了下文档，在 msdn的文档和 C++ dynamic binding.pdf 文档中，都提到 destructor 是不可继承的（看下图）：</p>

<p><img src="http://obi1zst3q.bkt.clouddn.com/blog/cpp/20160810-oop-pure-virtual-destructor.jpg" alt="destructor" title="pic"></p>

<p><img src="http://obi1zst3q.bkt.clouddn.com/blog/cpp/20160810_msdn_destructors_override.jpg" alt="destructor" title="msdn"></p>

<p>～Base() 虽然为 virtual 函数，但其不可继承（所以总是被override），因此析构的时候，会先调用 ~Derived(), 然后调用 ~Base()。</p>

<p>关于继承体系下，析构的顺序，可以参考 <a href="https://msdn.microsoft.com/en-us/library/6t4fe76c.aspx" title="msdn">msdn</a>。</p>

<p>本文到此为止，谢谢耐心阅读。</p>

  </div>

</article>