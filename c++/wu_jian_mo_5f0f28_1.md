# 物件模式(1)


<article class="post" itemscope="" itemtype="http://schema.org/BlogPosting">

  <header class="post-header">
    <h1 class="post-title" itemprop="name headline">C++ 對象模型</h1>
    <p class="post-meta"><time datetime="2016-08-07T03:59:00+00:00" itemprop="datePublished">Aug 7, 2016</time></p>
  </header>

  <div class="post-content" itemprop="articleBody">
    <p><em>這是C++語言的一系列文章，內容取自於網易微專業《C++開發工程師（升級版）》。</em></p>

<p><em>本文是聽了侯捷老師關於“C++對象模型”的課程以後，總結而成的。</em>
<em>課程中講到了範型編程和麵向對象編程兩種模式，因此本文的主題是 template。</em>
<em>包括類模板、函數模板、成員模板、模板特化和偏特化、可變模板參數、模板嵌套等。</em></p>

<p><em>說到面向對象，本文的另一個主題是C++底層的對象模型，包括this指針、虛指針vptr、虛表vtable、虛函數、多態等。</em></p>

<h2 id="c">第三週：C++對象模型</h2>

<h2 id="part-1class-">Part 1：class 的高級用法</h2>

<p>這一部分，我們介紹一些C++ 類的高級用法：conversion function、non-explicit-one-argument constructor、pointer-like
classes、function-like classes。</p>

<h3 id="conversion-function-">1.1 conversion function (轉換函數)</h3>

<p>轉換函數 可以將對象 默認轉換為另一種類型，方便程序的調用。</p>

<p>其作用與只接收一個參數的構造函數（不使用 explicit修飾符）作用正好相反。關於構造函數的坑參考<a href="http://tipsandtricks.runicsoft.com/Cpp/Explicit.html" title="explicit">這篇文章</a>。
下一個小結講解的就是 non-explicit-one-argument constructor。</p>

<p>廢話少說，先上個例子（注意觀察它的使用方法）：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// Fraction 類
</span><span class="k">class</span> <span class="nc">Fraction</span> <span class="p">{</span>
<span class="k">public</span><span class="o">:</span>
  <span class="n">Fraction</span><span class="p">(</span><span class="kt">int</span> <span class="n">num</span><span class="p">,</span> <span class="kt">int</span> <span class="n">den</span><span class="o">=</span><span class="mi">1</span><span class="p">)</span><span class="o">:</span> <span class="n">m_numerator</span><span class="p">(</span><span class="n">num</span><span class="p">),</span> <span class="n">m_denominator</span><span class="p">(</span><span class="n">den</span><span class="p">){};</span>
  <span class="k">operator</span> <span class="kt">double</span><span class="p">()</span> <span class="k">const</span> <span class="p">{</span><span class="k">return</span> <span class="p">(</span><span class="kt">double</span><span class="p">)</span><span class="n">m_numerator</span><span class="o">/</span><span class="n">m_denominator</span><span class="p">;</span> <span class="p">};</span>
  <span class="k">private</span><span class="o">:</span>
    <span class="kt">int</span> <span class="n">m_numerator</span><span class="p">;</span>
    <span class="kt">int</span> <span class="n">m_denominator</span><span class="p">;</span>
<span class="p">};</span>

<span class="c1">// main 函數（使用 Fraction 的轉換函數 operator double() const)
</span><span class="kt">int</span> <span class="nf">main</span><span class="p">(</span><span class="kt">int</span> <span class="n">argc</span><span class="p">,</span> <span class="kt">char</span> <span class="o">*</span> <span class="n">argv</span><span class="p">[])</span> <span class="p">{</span>
  <span class="n">Fraction</span> <span class="n">f</span><span class="p">(</span><span class="mi">3</span><span class="p">,</span> <span class="mi">5</span><span class="p">);</span>
  <span class="kt">double</span> <span class="n">d</span> <span class="o">=</span> <span class="mf">4.0</span> <span class="o">+</span> <span class="n">f</span><span class="p">;</span>      <span class="c1">// f 調用 operator double() 轉換為一個 double 值 
</span>  <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">"d = "</span> <span class="o">&lt;&lt;</span> <span class="n">d</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">endl</span><span class="p">;</span>
<span class="p">}</span>
</code></pre>
</div>

<p>編譯器認為必要時，將對象轉換為某種特定的類型。轉換函數的三個特徵：</p>

<ol>
  <li>轉換函數 的聲明中不需要寫返回類型 (因為返回類型和函數名稱相同)</li>
  <li>函數名必須是 operator xxx</li>
  <li>必須使用 const 修飾符，因為轉換函數不會修改 this 指針</li>
</ol>

<h3 id="non-explicit-one-argument-constructor">1.2 non-explicit-one-argument constructor</h3>

<p>non-explicit-one-argument constructor 有下面兩個語法特徵：</p>

<ol>
  <li>這類構造函數只接收一個參數</li>
  <li>沒有使用 explicit 修飾符 （默認 implicit）</li>
</ol>

<p>該函數在編譯器認為需要的時候，將參數類型的變量轉換成該類型的對象。具體看下面代碼：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// Fraction 類
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

<span class="c1">// main 函數（注意變量 d 的類型）
</span><span class="kt">int</span> <span class="n">main</span><span class="p">(</span><span class="kt">int</span> <span class="n">argc</span><span class="p">,</span> <span class="kt">char</span> <span class="o">*</span> <span class="n">argv</span><span class="p">[])</span> <span class="p">{</span>
  <span class="n">Fraction</span> <span class="n">f</span><span class="p">(</span><span class="mi">3</span><span class="p">,</span> <span class="mi">5</span><span class="p">);</span>
  <span class="n">Fraction</span> <span class="n">d</span> <span class="o">=</span>  <span class="n">f</span> <span class="o">+</span> <span class="mi">4</span><span class="p">;</span>  <span class="c1">// here 4 is converted to a Fraction(4, 1), 4+f is not allowed
</span>  <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">"d = "</span> <span class="o">&lt;&lt;</span> <span class="n">d</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">endl</span><span class="p">;</span>
<span class="p">}</span>
</code></pre>
</div>

<p>使用 explicit 修飾構造函數以後，上面的默認轉換就會失敗。</p>

<h3 id="pointer-like-class-">1.3  pointer-like class (智能指針)</h3>
<p>像指針的類，是指該 class 的對象表現出來像一個指針。這麼做的目的是實現一個比指針更強大的結構。
標準庫內值了一些指針類，如 std::shared_ptr, std::weak_ptr, std::unique_ptr，具體參考<a href="http://www.cplusplus.com/reference/memory/" title="memory">cplusplus</a></p>

<h4 id="sharedptr-">1.3.1 shared_ptr 智能指針</h4>
<p>實現一個智能指針類，則必須重載 (override) 兩個成員函數： operator*() 和 operator-&gt;()。</p>

<p>shared_ptr 代碼的抽離出來一部分如下：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// shared_ptr 模板類的定義
</span><span class="k">template</span> <span class="o">&lt;</span><span class="k">typename</span> <span class="n">T</span><span class="o">&gt;</span>
<span class="k">class</span> <span class="nc">shared_ptr</span><span class="p">{</span>
<span class="k">public</span><span class="o">:</span>
  <span class="n">T</span><span class="o">&amp;</span> <span class="k">operator</span><span class="o">*</span><span class="p">()</span> <span class="k">const</span> <span class="p">{</span> <span class="c1">// 解引用
</span>    <span class="k">return</span> <span class="o">*</span><span class="n">px</span><span class="p">;</span>
  <span class="p">}</span>
  <span class="n">T</span><span class="o">*</span> <span class="k">operator</span><span class="o">-&gt;</span><span class="p">()</span> <span class="k">const</span> <span class="p">{</span> <span class="c1">// 取指針，這個方法有點詭異
</span>    <span class="k">return</span> <span class="n">px</span><span class="p">;</span>
  <span class="p">}</span>
  
  <span class="n">shared_ptr</span><span class="p">(</span><span class="n">T</span> <span class="o">*</span><span class="n">p</span><span class="p">)</span><span class="o">:</span> <span class="n">px</span><span class="p">(</span><span class="n">p</span><span class="p">)</span> <span class="p">{}</span>
<span class="k">private</span><span class="o">:</span>
  <span class="n">T</span><span class="o">*</span> <span class="n">px</span><span class="p">;</span>
  <span class="kt">long</span><span class="o">*</span> <span class="n">pn</span><span class="p">;</span>
<span class="p">};</span>

<span class="c1">// Foo 類
</span><span class="k">struct</span> <span class="n">Foo</span> <span class="p">{</span>
  <span class="c1">// 省略其它部分，關注 method 
</span>  <span class="kt">void</span> <span class="n">method</span><span class="p">()</span> <span class="p">{</span> <span class="c1">//... }
</span><span class="p">};</span>

<span class="c1">// 使用 shared_ptr
</span><span class="kt">int</span> <span class="nf">main</span><span class="p">()</span> <span class="p">{</span>
  <span class="n">shared_ptr</span><span class="o">&lt;</span><span class="n">Foo</span><span class="o">&gt;</span> <span class="n">sp</span><span class="p">(</span><span class="k">new</span> <span class="n">Foo</span><span class="p">);</span>

  <span class="n">Foo</span> <span class="n">f</span><span class="p">(</span><span class="o">*</span><span class="n">sp</span><span class="p">);</span>  <span class="c1">// 使用 解引用 操作符 *
</span>  <span class="n">sp</span><span class="o">-&gt;</span><span class="n">method</span><span class="p">();</span>  <span class="c1">// 使用 操作符 -&gt;  ( 等價於 px-&gt;method() )
</span><span class="p">}</span>
</code></pre>
</div>

<p><em>關於 operator -&gt; ，從語法角度上來說，-&gt; 是可重生的，所以下面 main函數中才可以這樣使用。</em></p>

<p>很容易發現，除了構造，shared_ptr 在使用上與裸指針幾乎沒有差別，但是不需要手動釋放內存。
當然，仿指針類的能力遠不止於自動釋放內存，還有很多，這裡我們看看標準庫中 std::shared_ptr的附加功能。</p>

<p>std::shared_ptr 不僅提供了有限的垃圾回收特性，還提供了內存擁有權的管理 (ownership)，點擊<a href="http://www.cplusplus.com/reference/memory/shared_ptr/?kw=shared_ptr" title="shared_ptr">這裡查看詳情</a></p>

<h4 id="iterator-">1.3.2 iterator 迭代器</h4>

<p>pointer-like classes 在迭代器中也有廣泛的應用。
標準庫中所有的容器(std::vector等) 都有迭代器。換句話說，標準庫的迭代器也實現了 operator* 和 operator-&gt; 方法。</p>

<p>每個迭代器對象 指向 一個容器變量，但同時實現了下面幾個方法：</p>

<ol>
  <li>operator==</li>
  <li>operator!=</li>
  <li>operator++</li>
  <li>operator–</li>
</ol>

<p>關於 迭代器中 operator* 和 operator-&gt; 的實現，也相當值得考究：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// 忽略上下文
</span>
<span class="n">reference</span> <span class="k">operator</span><span class="o">*</span><span class="p">()</span> <span class="k">const</span> <span class="p">{</span>
  <span class="k">return</span> <span class="p">(</span><span class="o">*</span><span class="n">node</span><span class="p">).</span><span class="n">data</span><span class="p">;</span>
<span class="p">}</span>

<span class="n">pointer</span> <span class="k">operator</span><span class="o">-&gt;</span><span class="p">()</span> <span class="k">const</span> <span class="p">{</span> <span class="c1">// 藉助於 operator* 實現
</span>  <span class="k">return</span> <span class="o">&amp;</span><span class="p">(</span><span class="k">operator</span><span class="o">*</span><span class="p">());</span>
<span class="p">}</span>
</code></pre>
</div>

<p>你可以像下面這樣使用這兩個方法：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="n">list</span><span class="o">&lt;</span><span class="n">Foo</span><span class="o">&gt;::</span><span class="n">iterator</span> <span class="n">ite</span><span class="p">;</span>

<span class="c1">//... 省略一部分代碼...
</span>
<span class="o">*</span><span class="n">ite</span><span class="p">;</span>   <span class="c1">// 獲取 Foo 對象的引用
</span>
<span class="n">ite</span><span class="o">-&gt;</span><span class="n">method</span><span class="p">();</span>  
<span class="c1">// 意思是 調用 Foo::method()
// 相當於 (*ite).method();
// 相當於 (&amp;(*ite))-&gt;method();
</span>
</code></pre>
</div>

<h3 id="function-like-classes-">1.4 function-like classes (仿函數)</h3>

<h4 id="section">1.4.1 什麼是仿函數？</h4>
<p>仿函數其實不是函數，是一個類，但是它的行為和函數類似。在實現的層面上，一個類一旦定義了 operator() 方法，就可以稱之為仿函數。</p>

<p>C++標準庫內置了很多<a href="http://www.cplusplus.com/reference/functional/," title="function">仿函數模板</a>。
我們先用 std::less 和 std::less_equal 為例，對仿函數的用法有一個直觀的認識：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// less example (http://www.cplusplus.com/reference/functional/less/)
// compile: g++ -o main main.cpp -lm
</span><span class="cp">#include &lt;iostream&gt;     // std::cout
#include &lt;functional&gt;   // std::less
#include &lt;algorithm&gt;    // std::sort, std::includes
</span>
<span class="kt">int</span> <span class="nf">main</span> <span class="p">()</span> <span class="p">{</span>
  <span class="c1">// 自己寫的簡單例子, 表達式 "std::less&lt;int&gt;()" 創建了一個臨時對象  
</span>  <span class="kt">int</span> <span class="n">a</span> <span class="o">=</span> <span class="mi">5</span><span class="p">,</span> <span class="n">b</span> <span class="o">=</span> <span class="mi">4</span><span class="p">;</span>
  <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">"std::less&lt;int&gt;()("</span> <span class="o">&lt;&lt;</span> <span class="n">a</span> <span class="o">&lt;&lt;</span> <span class="s">", "</span> <span class="o">&lt;&lt;</span> <span class="n">b</span> <span class="o">&lt;&lt;</span> <span class="s">"): "</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">less</span><span class="o">&lt;</span><span class="kt">int</span><span class="o">&gt;</span><span class="p">()(</span><span class="n">a</span><span class="p">,</span> <span class="n">b</span><span class="p">)</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">endl</span><span class="p">;</span>
  <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">"std::less&lt;int&gt;()("</span> <span class="o">&lt;&lt;</span> <span class="n">b</span> <span class="o">&lt;&lt;</span> <span class="s">", "</span> <span class="o">&lt;&lt;</span> <span class="n">a</span> <span class="o">&lt;&lt;</span> <span class="s">"): "</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">less</span><span class="o">&lt;</span><span class="kt">int</span><span class="o">&gt;</span><span class="p">()(</span><span class="n">b</span><span class="p">,</span> <span class="n">a</span><span class="p">)</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">endl</span><span class="p">;</span>

  <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">"std::less_equal&lt;int&gt;()("</span> <span class="o">&lt;&lt;</span> <span class="n">a</span> <span class="o">&lt;&lt;</span> <span class="s">", "</span> <span class="o">&lt;&lt;</span> <span class="n">b</span> <span class="o">&lt;&lt;</span> <span class="s">"): "</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">less_equal</span><span class="o">&lt;</span><span class="kt">int</span><span class="o">&gt;</span><span class="p">()(</span><span class="n">a</span><span class="p">,</span> <span class="n">b</span><span class="p">)</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">endl</span><span class="p">;</span>
  <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">"std::less_equal&lt;int&gt;()("</span> <span class="o">&lt;&lt;</span> <span class="n">b</span> <span class="o">&lt;&lt;</span> <span class="s">", "</span> <span class="o">&lt;&lt;</span> <span class="n">a</span> <span class="o">&lt;&lt;</span> <span class="s">"): "</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">less_equal</span><span class="o">&lt;</span><span class="kt">int</span><span class="o">&gt;</span><span class="p">()(</span><span class="n">b</span><span class="p">,</span> <span class="n">a</span><span class="p">)</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">endl</span><span class="p">;</span>
  <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">"std::less_equal&lt;int&gt;()("</span> <span class="o">&lt;&lt;</span> <span class="n">a</span> <span class="o">&lt;&lt;</span> <span class="s">", "</span> <span class="o">&lt;&lt;</span> <span class="n">a</span> <span class="o">&lt;&lt;</span> <span class="s">"): "</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">less_equal</span><span class="o">&lt;</span><span class="kt">int</span><span class="o">&gt;</span><span class="p">()(</span><span class="n">a</span><span class="p">,</span> <span class="n">a</span><span class="p">)</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">endl</span><span class="p">;</span>

  <span class="c1">// 網站上帶的高級例子
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

<h4 id="section-1">1.4.2 仿函數的實現</h4>

<p>仿函數實際上是一個 類 (class)，這個類實現了 operator() 方法。
下面這個是 C++11 中 std::less 的實現：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// C++11 中的實現（侯捷老師講的是 C++98中的實現）
</span><span class="k">template</span> <span class="o">&lt;</span><span class="k">class</span> <span class="nc">T</span><span class="o">&gt;</span> <span class="k">struct</span> <span class="n">less</span> <span class="p">{</span>
  <span class="kt">bool</span> <span class="k">operator</span><span class="p">()</span> <span class="p">(</span><span class="k">const</span> <span class="n">T</span><span class="o">&amp;</span> <span class="n">x</span><span class="p">,</span> <span class="k">const</span> <span class="n">T</span><span class="o">&amp;</span> <span class="n">y</span><span class="p">)</span> <span class="k">const</span> <span class="p">{</span><span class="k">return</span> <span class="n">x</span><span class="o">&lt;</span><span class="n">y</span><span class="p">;}</span>
  <span class="k">typedef</span> <span class="n">T</span> <span class="n">first_argument_type</span><span class="p">;</span>
  <span class="k">typedef</span> <span class="n">T</span> <span class="n">second_argument_type</span><span class="p">;</span>
  <span class="k">typedef</span> <span class="kt">bool</span> <span class="n">result_type</span><span class="p">;</span>
<span class="p">};</span>
</code></pre>
</div>

<p>注意：std::less 是類模板。在課程中，侯捷老師提到了 unary_function 和 binary_function，這兩個類定義了參數類型，C++11中已經不再
使用，而是內置到 std::less 中，具體參考<a href="http://www.cplusplus.com/reference/functional/less/" title="less">這裡</a></p>

<h4 id="namespace">1.5 命名空間 (namespace)</h4>
<p>命名空間用於 模塊分離和解耦。為了更好地說明一些細節，這裡使用從 <a href="https://msdn.microsoft.com/en-us/library/5cb46ksf.aspx" title="msdn">msdn</a> 摘取一段話：</p>

<p><code class="highlighter-rouge">A namespace is a declarative region that provides a scope to the identifiers (the names of types, functions, variables, etc) inside it.</code></p>

<p><code class="highlighter-rouge">Namespaces are used to organize code into logical groups and to prevent name collisions that can occur especially when your code base includes multiple libraries.</code></p>

<p><code class="highlighter-rouge">All identifiers at namespace scope are visible to one another without qualification.</code></p>

<p><code class="highlighter-rouge">Identifiers outside the namespace can access the members by using the fully qualified name for each identifier, for example std::vector&lt;std::string&gt; vec;, or else by a using Declaration for a single identifier (using std::string), or a using Directive (C++) for all the identifiers in the namespace (using namespace std;).</code></p>

<p><code class="highlighter-rouge">Code in header files should always use the fully qualified namespace name.</code></p>

<h2 id="part-2--template">Part 2 模板 (template)</h2>

<h3 id="class-template-">2.1 class template (類模板)</h3>

<p>前面幾篇博客對類模板有所涉及，這裡不再贅述。C++標準庫的容器都是類模板的範例，比如：</p>

<ol>
  <li><a href="http://www.cplusplus.com/reference/vector/" title="vector">std::vector</a></li>
  <li><a href="http://www.cplusplus.com/reference/stack/" title="stack">std::stack</a></li>
  <li><a href="http://www.cplusplus.com/reference/stack/" title="array">std::array</a></li>
  <li><a href="http://www.cplusplus.com/reference/map/" title="map">std::map</a></li>
  <li><a href="http://www.cplusplus.com/reference/," title="etc">and so on</a></li>
</ol>

<h3 id="function-template-">2.2 function template (函數模板)</h3>

<p>對於 function template ，前面幾篇博客也都有所涉及。C++標準庫 algorithm 分類下有將近 90 個函數模板，
這裡我列出幾個：</p>

<ol>
  <li><a href="http://www.cplusplus.com/reference/algorithm/min/" title="min">std::min</a></li>
  <li><a href="http://www.cplusplus.com/reference/algorithm/max/" title="max">std::max</a></li>
  <li><a href="http://www.cplusplus.com/reference/algorithm/minmax/" title="minmax">std::minmax</a></li>
  <li><a href="http://www.cplusplus.com/reference/algorithm/sort/" title="sort">std::sort</a></li>
  <li><a href="http://www.cplusplus.com/reference/algorithm/copy/" title="copy">std::copy</a></li>
  <li><a href="http://www.cplusplus.com/reference/algorithm/for_each/" title="for_each">std::for_each</a></li>
  <li><a href="http://www.cplusplus.com/reference/algorithm/" title="algorithm">and so on</a></li>
</ol>

<p>下面我們以 std::for_each 為例，看下如何使用函數模板：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// for_each example (來源：http://www.cplusplus.com/reference/algorithm/for_each/)
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

<p>在這個例子中，注意函數 myfunction 和 仿函數 myobject 的用法，think twice about that。</p>

<p>另外，使用函數模板時，不需要指定特化類型，因為編譯器會根據參數進行自動推導。</p>

<h3 id="member-method-">2.3 Member method (成員模板，默認為成員函數模板)</h3>

<p>從使用者的角度來看，成員模板 比 類模板 具有更大的自由度。由於C++強大的繼承機制，成員模板也有一些使用場景。
這裡以 shared_ptr 為例：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// 定義 類模板 shared_ptr
</span><span class="k">template</span> <span class="o">&lt;</span><span class="k">typename</span> <span class="n">_Tp</span><span class="o">&gt;</span>
<span class="k">class</span> <span class="nc">shared_ptr</span> <span class="o">:</span> <span class="n">pubic</span> <span class="n">__shared_ptr</span><span class="o">&lt;</span><span class="n">_Tp</span><span class="o">&gt;</span> <span class="p">{</span>
  <span class="c1">//... 省略代碼 ...
</span>
  <span class="k">template</span> <span class="o">&lt;</span><span class="k">typename</span> <span class="n">_Tp1</span><span class="o">&gt;</span>
  <span class="k">explicit</span> <span class="n">shared_ptr</span><span class="p">(</span><span class="n">_Tp1</span><span class="o">*</span> <span class="n">__p</span><span class="p">)</span> <span class="o">:</span> <span class="n">__shared_ptr</span><span class="o">&lt;</span><span class="n">_TP</span><span class="o">&gt;</span><span class="p">(</span><span class="n">__p</span><span class="p">)</span> <span class="p">{}</span>

  <span class="c1">// ... 省略代碼 ...
</span><span class="p">};</span>

<span class="c1">// 使用 shared_ptr 的模板構造函數
// Derived1 類是 Base1 的子類
</span><span class="kt">int</span> <span class="nf">main</span><span class="p">()</span> <span class="p">{</span>
  <span class="n">Base1</span> <span class="o">*</span><span class="n">ptr</span> <span class="o">=</span> <span class="k">new</span> <span class="n">Derived1</span><span class="p">;</span>  <span class="c1">// 向上轉型
</span>
  <span class="n">shared_ptr</span><span class="o">&lt;</span><span class="n">Base1</span><span class="o">&gt;</span> <span class="n">sptr</span><span class="p">(</span><span class="k">new</span> <span class="n">Derived1</span><span class="p">);</span>  <span class="c1">// 支持向上轉型
</span><span class="p">}</span>
</code></pre>
</div>

<p>這個例子中，成員模板允許 shared_ptr 支持接收子類對象的指針，構造一個父類shared_ptr。</p>

<h3 id="specialization-">2.4 specialization (模板特化)</h3>

<p>模板本身是泛化的，允許用戶在使用時進行特化。所謂“特化”，其實是指 在編譯器的展開。
但是模板的設計有時候不能滿足所有特化類型的要求，比如 std::vector 容納 bool 時會有問題，
所有有了 <code class="highlighter-rouge">std::vector&lt;bool&gt;</code> 的特化版本。</p>

<h4 id="section-2">2.4.1 模板偏特化</h4>

<p>模板偏特化 可以分為兩類：</p>

<ol>
  <li>個數上的“偏”</li>
</ol>

<p>例如 <code class="highlighter-rouge">std::vector&lt;int, typename Alloc=.....&gt;</code> 相對於 <code class="highlighter-rouge">std::vector&lt;typename T, typename Alloc=......&gt;</code></p>

<ol>
  <li>類型上的“偏” (由對象擴展到 指針類型)</li>
</ol>

<p>這裡直接看一個例子：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// 泛化版本
</span><span class="k">template</span> <span class="o">&lt;</span><span class="k">typename</span> <span class="n">T</span><span class="o">&gt;</span>
<span class="k">class</span> <span class="nc">C</span> <span class="p">{</span>
  <span class="c1">//... 
</span><span class="p">};</span>

<span class="c1">// 擴展到指針的 特化版本
</span><span class="k">template</span> <span class="o">&lt;</span><span class="k">typename</span> <span class="n">T</span><span class="o">&gt;</span>
<span class="k">class</span> <span class="nc">C</span><span class="o">&lt;</span><span class="n">T</span><span class="o">*&gt;</span> <span class="p">{</span>
  <span class="c1">//...
</span><span class="p">};</span>

<span class="c1">// 使用 特化版本
</span><span class="kt">int</span> <span class="nf">main</span><span class="p">()</span> <span class="p">{</span>
  <span class="n">C</span><span class="o">&lt;</span><span class="n">string</span><span class="o">&gt;</span> <span class="n">obj1</span><span class="p">;</span>    <span class="c1">// 正常的特化版本
</span>  <span class="n">C</span><span class="o">&lt;</span><span class="n">string</span><span class="o">*&gt;</span> <span class="n">obj2</span><span class="p">;</span>   <span class="c1">// 特化的指針版本
</span><span class="p">}</span>
</code></pre>
</div>

<h4 id="section-3">2.4.2 模板模板參數（模板嵌套)</h4>
<p>模板模板參數是指 一個模板作為另一個模板的參數存在。這樣的設計在使用上更為靈活。這裡直接上一個例子：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// 定義一個類模板，它使用一個模板作為模板參數
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

<span class="c1">// 定義Lst
</span><span class="k">template</span><span class="o">&lt;</span><span class="k">typename</span> <span class="n">T</span><span class="o">&gt;</span>
<span class="k">using</span> <span class="n">Lst</span> <span class="o">=</span> <span class="n">list</span><span class="o">&lt;</span><span class="n">T</span><span class="p">,</span> <span class="n">allocator</span><span class="o">&lt;</span><span class="n">T</span><span class="o">&gt;</span> <span class="o">&gt;</span><span class="p">;</span>     <span class="c1">// 注意： Lst 只有一個模板參數，而 list 有兩個模板參數
// 使用該模板
</span><span class="kt">int</span> <span class="nf">main</span><span class="p">()</span> <span class="p">{</span>
  <span class="n">XCls</span><span class="o">&lt;</span><span class="n">string</span><span class="p">,</span> <span class="n">list</span><span class="o">&gt;</span> <span class="n">mylist1</span><span class="p">;</span>   <span class="c1">// 合法的定義
</span>
<span class="c1">//XCls&lt;string, Lst&gt; mylist2;    // 不合法，因為 XCls 的第二個模板參數只接受一個參數（有點繞，think about it）
</span><span class="p">}</span>
</code></pre>
</div>

<p>這個模板的靈活性在於，第二個模板參數，你可以使用 std::list, std::stack, std::vector 
等迭代器的特化版本作為參數，也就是說底層可以接入不同的“內存管理方案”（這個詞相對準確）。</p>

<h2 id="part-3c">Part 3：C++語言層面的相關主題</h2>

<h3 id="c-1">3.1 C++標準庫概論</h3>

<p>這裡用一張圖表示</p>

<p><img src="./images/20160808_C++%20%E6%A0%87%E5%87%86%E5%BA%93%20%E5%85%A8%E5%9B%BE" alt="stl" title="stl"></p>

<h3 id="variadic-templates-c11">3.2 variadic templates：模板的可變參數列表 (C++11)</h3>

<p>模板的可變參數列表與 正常的可變參數列表是一樣的，只是語法上有些特殊。
下面是一個 print 的例子：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// 定義 print 函數
</span><span class="kt">void</span> <span class="nf">print</span><span class="p">()</span> <span class="p">{}</span>

<span class="k">template</span><span class="o">&lt;</span><span class="k">typename</span> <span class="n">T</span><span class="p">,</span> <span class="k">typename</span><span class="p">...</span> <span class="n">Types</span><span class="o">&gt;</span>
<span class="kt">void</span> <span class="n">print</span><span class="p">(</span><span class="k">const</span> <span class="n">T</span><span class="o">&amp;</span> <span class="n">firstArg</span><span class="p">,</span> <span class="k">const</span> <span class="n">Types</span><span class="o">&amp;</span><span class="p">...</span> <span class="n">args</span><span class="p">)</span> <span class="p">{</span>
  <span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="n">firstArg</span> <span class="o">&lt;&lt;</span> <span class="n">endl</span><span class="p">;</span>
  <span class="n">print</span><span class="p">(</span><span class="n">args</span><span class="p">...);</span>
<span class="p">}</span>

<span class="c1">// 使用 print 函數
</span><span class="kt">int</span> <span class="n">main</span><span class="p">()</span> <span class="p">{</span>
  <span class="n">print</span><span class="p">(</span><span class="mf">7.5</span><span class="p">,</span> <span class="s">"hello"</span><span class="p">,</span> <span class="n">bitset</span><span class="o">&lt;</span><span class="mi">16</span><span class="o">&gt;</span><span class="p">(</span><span class="mi">377</span><span class="p">),</span><span class="mi">42</span><span class="p">);</span>
<span class="p">}</span>
</code></pre>
</div>

<p>另外，對於模板參數，C++ 提供了輔助函數，用來獲取可變參數列表的長度，函數簽名為 <code class="highlighter-rouge">size_type sizeof...(args)</code>。</p>

<h3 id="auto-c11">3.3 auto (C++11)</h3>

<p>auto 允許用戶不聲明變量的類型，而是留給編譯器去推導。它是C++11加入的語法糖，可以減少有效代碼量。</p>

<p>關於 auto，更多細節參考 <a href="https://msdn.microsoft.com/en-us/library/dd293667.aspx" title="msdn">msdn</a></p>

<h3 id="range-based-for-c11">3.4 range-based for (c++11)</h3>

<p>這是C++11 新增加的語法，可以有效減少代碼量，與 auto 配合使用更佳。考慮到是否需要修改數組的值，決定是否採用引用，看代碼：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="n">vector</span><span class="o">&lt;</span><span class="kt">double</span><span class="o">&gt;</span> <span class="n">vec</span><span class="p">;</span>
<span class="k">for</span> <span class="p">(</span><span class="k">auto</span> <span class="n">elem</span><span class="o">:</span> <span class="n">vec</span><span class="p">)</span> <span class="p">{</span>  <span class="c1">// 按值傳遞，不會修改數組的值
</span>  <span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="n">elem</span> <span class="o">&lt;&lt;</span> <span class="n">endl</span><span class="p">;</span>  
  <span class="n">elem</span> <span class="o">*=</span> <span class="mi">3</span><span class="p">;</span>   <span class="c1">// 即便這樣寫， 也只是修改了一個副本，不會修改 vec 的值。
</span><span class="p">}</span>

<span class="k">for</span> <span class="p">(</span><span class="k">auto</span><span class="o">&amp;</span> <span class="n">elem</span><span class="o">:</span> <span class="n">vec</span><span class="p">){</span>  <span class="c1">// 按引用傳遞
</span>  <span class="n">elem</span> <span class="o">*=</span> <span class="mi">3</span><span class="p">;</span>  <span class="c1">// 使用引用會修改數組的值
</span><span class="p">}</span>
</code></pre>
</div>

<p>更多參考 <a href="https://msdn.microsoft.com/en-us/library/jj203382.aspx">msdn上的描述</a>。</p>

<h3 id="reference-">3.5 關於 reference (一些引起誤解的地方)</h3>

<h4 id="reference--1">3.5.1 reference 的特徵</h4>

<p>reference的兩個特徵：</p>

<ol>
  <li>reference類型的變量一旦 代表某個變量，就永遠不能代表另一個變量</li>
  <li>reference類型的變量  大小和地址 與 原對象相同 (即 sizeof 和 operator&amp; 的返回值)</li>
</ol>

<p>下面用侯捷老師PPT上的一段代碼來說明：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="kt">int</span> <span class="nf">main</span><span class="p">()</span> <span class="p">{</span>
  <span class="kt">int</span> <span class="n">x</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span>
  <span class="kt">int</span><span class="o">*</span> <span class="n">p</span> <span class="o">=</span> <span class="o">&amp;</span><span class="n">x</span><span class="p">;</span>
  <span class="kt">int</span><span class="o">&amp;</span> <span class="n">r</span> <span class="o">=</span> <span class="n">x</span><span class="p">;</span>  <span class="c1">// r 代表 x，兩者的值都是 0
</span>  <span class="kt">int</span> <span class="n">x2</span> <span class="o">=</span> <span class="mi">5</span><span class="p">;</span>

  <span class="n">r</span> <span class="o">=</span> <span class="n">x2</span><span class="p">;</span>      <span class="c1">// 這一行賦值的結果是：x 的值也變成了 5
</span>  <span class="kt">int</span><span class="o">&amp;</span> <span class="n">r2</span> <span class="o">=</span> <span class="n">r</span><span class="p">;</span> <span class="c1">// r2、r 都代表 x，即值都是 5
</span><span class="p">}</span>
</code></pre>
</div>

<p>上面這個例子中，需要注意：</p>

<ol>
  <li>sizeof(x) == sizeof(r)</li>
  <li>&amp;x == &amp;r</li>
</ol>

<h4 id="section-4">3.5.2 應用場景</h4>

<p>reference 通常用在兩個地方：</p>

<ol>
  <li>參數傳遞 (比較快)</li>
  <li>返回類型</li>
</ol>

<h3 id="section-5">3.6 構造和析構 (時間先後)</h3>

<p>本小節主要講解構造和析構 在繼承和組合體系下的運作機制。</p>

<h4 id="section-6">3.6.1 繼承體系中的構造和析構</h4>

<p>構造：由內而外。內是指Base，外指Derived
析構：由外而內。先析構Derived Class，再析構Base Class 的部分</p>

<p>注意：Base Class 的析構函數必須是 virtual 的，否則會報出 undefined behaviors 的錯誤。
下面這段代碼重現了這個錯誤：</p>

<div class="language-c++ highlighter-rouge"><pre class="highlight"><code><span class="c1">// 這段代碼 來源於 stackoverflow ，但是經過了大量修改
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
    <span class="c1">// 注意：Base的析構函數設置為 virtual
</span>    <span class="n">Base</span> <span class="o">*</span><span class="n">b</span> <span class="o">=</span> <span class="k">new</span> <span class="n">Derived</span><span class="p">();</span>
    <span class="c1">// 使用 b
</span>    <span class="k">delete</span> <span class="n">b</span><span class="p">;</span> <span class="c1">// 結果是：調用了 Base的構造函數
</span>
    <span class="n">std</span><span class="o">::</span><span class="n">cout</span> <span class="o">&lt;&lt;</span> <span class="s">"execute complete"</span> <span class="o">&lt;&lt;</span> <span class="n">std</span><span class="o">::</span><span class="n">endl</span><span class="p">;</span>
<span class="p">}</span>


</code></pre>
</div>
<p>上面這段代碼打印結果是：</p>

<div class="highlighter-rouge"><pre class="highlight"><code>Base()
Derived()
Node()
~Base() // 為什麼只打印了 這個？？？
execute complete
</code></pre>
</div>

<p>注意： 在實際的測試中，代碼沒有報出 undefined behavior 錯誤。但是出現了內存洩漏 m_pNode 的內存沒有被釋放。
關於這段代碼的解釋，我聯想到侯捷老師講到的靜態綁定和動態綁定，網上一張相關的ppt，
點擊<a href="http://www.cs.wustl.edu/~schmidt/PDF/C++-dynamic-binding4.pdf" title="vtable">C++-dynamic-binding</a>查看。</p>

<p>然後，我給 ~Base() 和 ~Derived() 都加上了 virtual (這裡就不再列出代碼)，結果仍然令人疑惑，結果如下：</p>

<div class="highlighter-rouge"><pre class="highlight"><code>Base()
Derived()
Node()
~Derived()
~Node()
~Base()    // 為什麼還會打印這個？？？
execute complete
</code></pre>
</div>

<p>又查了下文檔，在 msdn的文檔和 C++ dynamic binding.pdf 文檔中，都提到 destructor 是不可繼承的（看下圖）：</p>

<p><img src="./images/20160810-oop-pure-virtual-destructor.jpg" alt="destructor" title="pic"></p>

<p><img src="./images/20160810_msdn_destructors_override.jpg" alt="destructor" title="msdn"></p>

<p>～Base() 雖然為 virtual 函數，但其不可繼承（所以總是被override），因此析構的時候，會先調用 ~Derived(), 然後調用 ~Base()。</p>

<p>關於繼承體系下，析構的順序，可以參考 <a href="https://msdn.microsoft.com/en-us/library/6t4fe76c.aspx" title="msdn">msdn</a>。</p>

<p>本文到此為止，謝謝耐心閱讀。</p>

  </div>

</article>