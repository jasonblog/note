# C 語言的奇技淫巧


<div class="col-lg-9 blog-main" style="min-width: 500px">
    
<div class="post-container">
  <div class="blog-header">
    <h1>C 語言的奇技淫巧</h1>
    <div class="post-description">
      <i class="fa fa-calendar"></i> 2015-08-16 Sunday &nbsp; &nbsp;
      <i class="fa fa-tags"></i>  linux ,  program  
      
    </div>
  </div>
  <hr>
  <p>整理下 C 語言中常用的技巧。</p>

<!-- more -->

<h2 id="變長數組">變長數組</h2>

<p>實際編程中，經常會使用變長數組，但是 C 語言並不支持變長的數組，可以使用結構體實現。</p>

<p>類似如下的結構體，其中 value 成員變量不佔用內存空間，也可以使用 <code class="highlighter-rouge">char value[]</code> ，但是不要使用 <code class="highlighter-rouge">char *value</code>，該變量會佔用指針對應的空間。</p>

<p>常見的操作示例如下。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#include &lt;stdio.h&gt;
#include &lt;stdlib.h&gt;
#include &lt;string.h&gt;
</span>
<span class="k">typedef</span> <span class="k">struct</span> <span class="n">foobar</span> <span class="p">{</span>
  <span class="kt">int</span> <span class="n">len</span><span class="p">;</span>
  <span class="kt">char</span> <span class="n">value</span><span class="p">[</span><span class="mi">0</span><span class="p">];</span>
<span class="p">}</span> <span class="n">foobar_t</span><span class="p">;</span>

<span class="kt">int</span> <span class="nf">main</span><span class="p">(</span><span class="kt">int</span> <span class="n">argc</span><span class="p">,</span> <span class="kt">char</span> <span class="o">*</span><span class="n">argv</span><span class="p">[])</span>
<span class="p">{</span>
    <span class="c1">// 兩者佔用的存儲空間相同，也就是value不佔用空間
</span>    <span class="n">printf</span><span class="p">(</span><span class="s">"%li %li</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span> <span class="k">sizeof</span><span class="p">(</span><span class="kt">int</span><span class="p">),</span> <span class="k">sizeof</span><span class="p">(</span><span class="n">foobar_t</span><span class="p">));</span>

    <span class="c1">// 初始化
</span>    <span class="kt">int</span> <span class="o">*</span><span class="n">values</span> <span class="o">=</span> <span class="p">(</span><span class="kt">int</span> <span class="o">*</span><span class="p">)</span><span class="n">malloc</span><span class="p">(</span><span class="mi">10</span><span class="o">*</span><span class="k">sizeof</span><span class="p">(</span><span class="kt">int</span><span class="p">)),</span> <span class="n">i</span><span class="p">,</span> <span class="n">j</span><span class="p">,</span> <span class="o">*</span><span class="n">ptr</span><span class="p">;</span>
    <span class="k">for</span> <span class="p">(</span><span class="n">i</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span> <span class="n">i</span> <span class="o">&lt;</span> <span class="mi">10</span><span class="p">;</span> <span class="n">i</span><span class="o">++</span><span class="p">)</span>
      <span class="n">values</span><span class="p">[</span><span class="n">i</span><span class="p">]</span> <span class="o">=</span> <span class="mi">10</span><span class="o">*</span><span class="n">i</span><span class="p">;</span>
    <span class="k">for</span> <span class="p">(</span><span class="n">i</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span> <span class="n">i</span> <span class="o">&lt;</span> <span class="mi">10</span><span class="p">;</span> <span class="n">i</span><span class="o">++</span><span class="p">)</span>
      <span class="n">printf</span><span class="p">(</span><span class="s">" %i"</span><span class="p">,</span> <span class="n">values</span><span class="p">[</span><span class="n">i</span><span class="p">]);</span>
    <span class="n">printf</span><span class="p">(</span><span class="s">"</span><span class="se">\n</span><span class="s">"</span><span class="p">);</span>

    <span class="c1">// 針對單個結構體的操作
</span>    <span class="n">foobar_t</span> <span class="o">*</span><span class="n">buff</span> <span class="o">=</span> <span class="p">(</span><span class="n">foobar_t</span> <span class="o">*</span><span class="p">)</span><span class="n">malloc</span><span class="p">(</span><span class="k">sizeof</span><span class="p">(</span><span class="n">foobar_t</span><span class="p">)</span> <span class="o">+</span> <span class="mi">10</span><span class="o">*</span><span class="k">sizeof</span><span class="p">(</span><span class="kt">int</span><span class="p">));</span>
    <span class="n">buff</span><span class="o">-&gt;</span><span class="n">len</span> <span class="o">=</span> <span class="mi">10</span><span class="p">;</span>
    <span class="n">memcpy</span><span class="p">(</span><span class="n">buff</span><span class="o">-&gt;</span><span class="n">value</span><span class="p">,</span> <span class="n">values</span><span class="p">,</span> <span class="mi">10</span><span class="o">*</span><span class="k">sizeof</span><span class="p">(</span><span class="kt">int</span><span class="p">));</span>
    <span class="n">ptr</span> <span class="o">=</span> <span class="p">(</span><span class="kt">int</span> <span class="o">*</span><span class="p">)</span><span class="n">buff</span><span class="o">-&gt;</span><span class="n">value</span><span class="p">;</span>

    <span class="n">printf</span><span class="p">(</span><span class="s">"length: %i, vlaues:"</span><span class="p">,</span> <span class="n">buff</span><span class="o">-&gt;</span><span class="n">len</span><span class="p">);</span>
    <span class="k">for</span> <span class="p">(</span><span class="n">i</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span> <span class="n">i</span> <span class="o">&lt;</span> <span class="mi">10</span><span class="p">;</span> <span class="n">i</span><span class="o">++</span><span class="p">)</span>
      <span class="n">printf</span><span class="p">(</span><span class="s">" %i"</span><span class="p">,</span> <span class="n">ptr</span><span class="p">[</span><span class="n">i</span><span class="p">]);</span>
    <span class="n">printf</span><span class="p">(</span><span class="s">"</span><span class="se">\n</span><span class="s">"</span><span class="p">);</span>
    <span class="n">free</span><span class="p">(</span><span class="n">buff</span><span class="p">);</span>

    <span class="c1">// 針對數組的操作
</span>    <span class="cp">#define FOOBAR_T_SIZE(elements) (sizeof(foobar_t) + sizeof(int) * (elements))
</span>    <span class="n">foobar_t</span> <span class="o">**</span><span class="n">buf</span> <span class="o">=</span> <span class="p">(</span><span class="n">foobar_t</span> <span class="o">**</span><span class="p">)</span><span class="n">malloc</span><span class="p">(</span><span class="mi">6</span><span class="o">*</span><span class="n">FOOBAR_T_SIZE</span><span class="p">(</span><span class="mi">10</span><span class="p">));</span>
    <span class="n">foobar_t</span> <span class="o">*</span><span class="n">ptr_buf</span><span class="p">;</span>
    <span class="k">for</span> <span class="p">(</span><span class="n">i</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span> <span class="n">i</span> <span class="o">&lt;</span> <span class="mi">6</span><span class="p">;</span> <span class="n">i</span><span class="o">++</span><span class="p">)</span> <span class="p">{</span>
      <span class="n">ptr_buf</span> <span class="o">=</span> <span class="p">(</span><span class="n">foobar_t</span><span class="o">*</span><span class="p">)((</span><span class="kt">char</span> <span class="o">*</span><span class="p">)</span><span class="n">buf</span> <span class="o">+</span> <span class="n">i</span><span class="o">*</span><span class="n">FOOBAR_T_SIZE</span><span class="p">(</span><span class="mi">10</span><span class="p">));</span>
      <span class="n">ptr_buf</span><span class="o">-&gt;</span><span class="n">len</span> <span class="o">=</span> <span class="n">i</span><span class="p">;</span>
      <span class="n">memcpy</span><span class="p">(</span><span class="n">ptr_buf</span><span class="o">-&gt;</span><span class="n">value</span><span class="p">,</span> <span class="n">values</span><span class="p">,</span> <span class="mi">10</span><span class="o">*</span><span class="k">sizeof</span><span class="p">(</span><span class="kt">int</span><span class="p">));</span>

      <span class="n">ptr</span> <span class="o">=</span> <span class="p">(</span><span class="kt">int</span> <span class="o">*</span><span class="p">)</span><span class="n">ptr_buf</span><span class="o">-&gt;</span><span class="n">value</span><span class="p">;</span>
      <span class="n">printf</span><span class="p">(</span><span class="s">"length: %i, vlaues:"</span><span class="p">,</span> <span class="n">ptr_buf</span><span class="o">-&gt;</span><span class="n">len</span><span class="p">);</span>
      <span class="k">for</span> <span class="p">(</span><span class="n">j</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span> <span class="n">j</span> <span class="o">&lt;</span> <span class="mi">10</span><span class="p">;</span> <span class="n">j</span><span class="o">++</span><span class="p">)</span>
        <span class="n">printf</span><span class="p">(</span><span class="s">" %i"</span><span class="p">,</span> <span class="n">ptr</span><span class="p">[</span><span class="n">j</span><span class="p">]);</span>
      <span class="n">printf</span><span class="p">(</span><span class="s">"</span><span class="se">\n</span><span class="s">"</span><span class="p">);</span>
    <span class="p">}</span>
    <span class="n">free</span><span class="p">(</span><span class="n">values</span><span class="p">);</span>
    <span class="n">free</span><span class="p">(</span><span class="n">buf</span><span class="p">);</span>

    <span class="k">return</span> <span class="mi">0</span><span class="p">;</span>
<span class="p">}</span></code></pre></figure>

<h2 id="qsort">qsort</h2>

<p><code class="highlighter-rouge">qsort()</code> 會根據給出的比較函數進行快排，通過指針移動實現排序，時間複雜度為 <code class="highlighter-rouge">n*log(n)</code>，排序之後的結果仍然放在原數組中，不保證排序穩定性，如下是其聲明。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));
void qsort_r(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *, void *), void *arg);
&nbsp;&nbsp;&nbsp; base: 數組起始地址；
&nbsp;&nbsp;&nbsp; nmemb: 數組元素個數；
&nbsp;&nbsp;&nbsp; size: 每個元素的大小；
&nbsp;&nbsp;&nbsp; compar: 函數指針，指向定義的比較函數，當elem1&gt;elem2返回正數，此時不交換。</code></pre></figure>

<p>通常可以對整數、字符串、結構體進行排序，如下是常用示例。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">----- 對int類型數組排序
int num[100];
int cmp(const void *a , const void *b)
{
	return *(int *)a - *(int *)b;
}
qsort(num, sizeof(num)/sizeof(num[0]), sizeof(num[0]), cmp);

----- 對結構體進行排序
struct foobar {
	int data;
	char string[10];
} s[100]
int cmp_int(const void *a, const void *b) /* 按照data遞增排序 */
{
	return (*(struct foobar *)a).data &gt; (*(struct foobar *)b).data ? 1 : -1;
}
int cmp_string(const void *a, const void *b)
{
	return strcmp((*(struct foobar *)a).string, (*(struct foobar *)b).string);
}
qsort(num, sizeof(num)/sizeof(num[0]), sizeof(num[0]), cmp);</code></pre></figure>

<p>以及示例程序。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#include &lt;stdio.h&gt;
#include &lt;string.h&gt;
#include &lt;stdlib.h&gt;
</span>
<span class="k">struct</span> <span class="n">foobar</span> <span class="p">{</span>
	<span class="kt">int</span> <span class="n">data</span><span class="p">;</span>
	<span class="kt">char</span> <span class="n">string</span><span class="p">[</span><span class="mi">100</span><span class="p">];</span>
<span class="p">}</span> <span class="n">array</span><span class="p">[</span><span class="mi">10</span><span class="p">];</span>

<span class="kt">int</span> <span class="nf">cmp_int</span><span class="p">(</span><span class="k">const</span> <span class="kt">void</span> <span class="o">*</span> <span class="n">a</span><span class="p">,</span> <span class="k">const</span> <span class="kt">void</span> <span class="o">*</span> <span class="n">b</span><span class="p">)</span>
<span class="p">{</span>
	<span class="k">return</span> <span class="p">(</span><span class="o">*</span><span class="p">(</span><span class="k">struct</span> <span class="n">foobar</span> <span class="o">*</span><span class="p">)</span><span class="n">a</span><span class="p">).</span><span class="n">data</span> <span class="o">-</span> <span class="p">(</span><span class="o">*</span><span class="p">(</span><span class="k">struct</span> <span class="n">foobar</span> <span class="o">*</span><span class="p">)</span><span class="n">b</span><span class="p">).</span><span class="n">data</span><span class="p">;</span>
<span class="p">}</span>

<span class="kt">int</span> <span class="nf">cmp_string</span><span class="p">(</span><span class="k">const</span> <span class="kt">void</span> <span class="o">*</span> <span class="n">a</span><span class="p">,</span> <span class="k">const</span> <span class="kt">void</span> <span class="o">*</span> <span class="n">b</span><span class="p">)</span>
<span class="p">{</span>
	<span class="k">return</span> <span class="n">strcmp</span><span class="p">((</span><span class="o">*</span><span class="p">(</span><span class="k">struct</span> <span class="n">foobar</span> <span class="o">*</span><span class="p">)</span><span class="n">a</span><span class="p">).</span><span class="n">string</span><span class="p">,</span> <span class="p">(</span><span class="o">*</span><span class="p">(</span><span class="k">struct</span> <span class="n">foobar</span> <span class="o">*</span><span class="p">)</span><span class="n">b</span><span class="p">).</span><span class="n">string</span><span class="p">);</span>
<span class="p">}</span>

<span class="kt">int</span> <span class="nf">main</span> <span class="p">(</span><span class="kt">void</span><span class="p">)</span>
<span class="p">{</span>
	<span class="kt">int</span> <span class="n">i</span><span class="p">,</span> <span class="n">j</span><span class="p">;</span>
	<span class="kt">int</span> <span class="n">array_size</span> <span class="o">=</span> <span class="k">sizeof</span><span class="p">(</span><span class="n">array</span><span class="p">)</span><span class="o">/</span><span class="k">sizeof</span><span class="p">(</span><span class="n">array</span><span class="p">[</span><span class="mi">0</span><span class="p">]);</span>
	<span class="n">printf</span><span class="p">(</span><span class="s">"Array size %d</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span> <span class="n">array_size</span><span class="p">);</span>

	<span class="n">srand</span><span class="p">((</span><span class="kt">int</span><span class="p">)</span><span class="n">time</span><span class="p">(</span><span class="mi">0</span><span class="p">));</span>
	<span class="k">for</span> <span class="p">(</span><span class="n">i</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span> <span class="n">i</span> <span class="o">&lt;</span> <span class="n">array_size</span><span class="p">;</span> <span class="n">i</span><span class="o">++</span><span class="p">)</span> <span class="p">{</span>
		<span class="kt">int</span> <span class="n">r</span> <span class="o">=</span> <span class="n">rand</span><span class="p">()</span> <span class="o">%</span> <span class="mi">100</span><span class="p">;</span>
		<span class="n">array</span><span class="p">[</span><span class="n">i</span><span class="p">].</span><span class="n">data</span> <span class="o">=</span> <span class="n">r</span><span class="p">;</span>
		<span class="k">for</span> <span class="p">(</span><span class="n">j</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span> <span class="n">j</span> <span class="o">&lt;</span> <span class="n">r</span><span class="p">;</span> <span class="n">j</span><span class="o">++</span><span class="p">)</span>
			<span class="n">array</span><span class="p">[</span><span class="n">i</span><span class="p">].</span><span class="n">string</span><span class="p">[</span><span class="n">j</span><span class="p">]</span> <span class="o">=</span> <span class="sc">'A'</span> <span class="o">+</span> <span class="n">rand</span><span class="p">()</span> <span class="o">%</span> <span class="mi">26</span><span class="p">;</span>
		<span class="n">array</span><span class="p">[</span><span class="n">i</span><span class="p">].</span><span class="n">string</span><span class="p">[</span><span class="n">r</span><span class="p">]</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span>
	<span class="p">}</span>

	<span class="n">printf</span><span class="p">(</span><span class="s">"Before sorting the list is: </span><span class="se">\n</span><span class="s">"</span><span class="p">);</span>
	<span class="k">for</span> <span class="p">(</span><span class="n">i</span> <span class="o">=</span> <span class="mi">0</span> <span class="p">;</span> <span class="n">i</span> <span class="o">&lt;</span> <span class="n">array_size</span><span class="p">;</span> <span class="n">i</span><span class="o">++</span> <span class="p">)</span>
		<span class="n">printf</span><span class="p">(</span><span class="s">"%d "</span><span class="p">,</span> <span class="n">array</span><span class="p">[</span><span class="n">i</span><span class="p">].</span><span class="n">data</span><span class="p">);</span>
	<span class="n">puts</span><span class="p">(</span><span class="s">""</span><span class="p">);</span>
	<span class="k">for</span> <span class="p">(</span><span class="n">i</span> <span class="o">=</span> <span class="mi">0</span> <span class="p">;</span> <span class="n">i</span> <span class="o">&lt;</span> <span class="n">array_size</span><span class="p">;</span> <span class="n">i</span><span class="o">++</span> <span class="p">)</span>
		<span class="n">printf</span><span class="p">(</span><span class="s">"%s</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span> <span class="n">array</span><span class="p">[</span><span class="n">i</span><span class="p">].</span><span class="n">string</span><span class="p">);</span>

	<span class="n">printf</span><span class="p">(</span><span class="s">"</span><span class="se">\n</span><span class="s">After sorting the list is: </span><span class="se">\n</span><span class="s">"</span><span class="p">);</span>
	<span class="n">qsort</span><span class="p">(</span><span class="n">array</span><span class="p">,</span> <span class="n">array_size</span><span class="p">,</span> <span class="k">sizeof</span><span class="p">(</span><span class="k">struct</span> <span class="n">foobar</span><span class="p">),</span> <span class="n">cmp_int</span><span class="p">);</span>
	<span class="k">for</span> <span class="p">(</span><span class="n">i</span> <span class="o">=</span> <span class="mi">0</span> <span class="p">;</span> <span class="n">i</span> <span class="o">&lt;</span> <span class="n">array_size</span><span class="p">;</span> <span class="n">i</span><span class="o">++</span> <span class="p">)</span>
		<span class="n">printf</span><span class="p">(</span><span class="s">"%d "</span><span class="p">,</span> <span class="n">array</span><span class="p">[</span><span class="n">i</span><span class="p">].</span><span class="n">data</span><span class="p">);</span>
	<span class="n">puts</span><span class="p">(</span><span class="s">""</span><span class="p">);</span>

	<span class="n">printf</span><span class="p">(</span><span class="s">"</span><span class="se">\n</span><span class="s">After sorting the list is: </span><span class="se">\n</span><span class="s">"</span><span class="p">);</span>
	<span class="n">qsort</span><span class="p">(</span><span class="n">array</span><span class="p">,</span> <span class="n">array_size</span><span class="p">,</span> <span class="k">sizeof</span><span class="p">(</span><span class="k">struct</span> <span class="n">foobar</span><span class="p">),</span> <span class="n">cmp_string</span><span class="p">);</span>
	<span class="k">for</span> <span class="p">(</span><span class="n">i</span> <span class="o">=</span> <span class="mi">0</span> <span class="p">;</span> <span class="n">i</span> <span class="o">&lt;</span> <span class="n">array_size</span><span class="p">;</span> <span class="n">i</span><span class="o">++</span> <span class="p">)</span>
		<span class="n">printf</span><span class="p">(</span><span class="s">"%s</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span> <span class="n">array</span><span class="p">[</span><span class="n">i</span><span class="p">].</span><span class="n">string</span><span class="p">);</span>

	<span class="k">return</span> <span class="mi">0</span><span class="p">;</span>
<span class="p">}</span></code></pre></figure>

<!--
https://www.felix021.com/blog/read.php?entryid=1951
-->

<h2 id="指針">指針</h2>

<p>指針或許是 C 語言中最複雜的東西了。</p>

<h3 id="指針常量-vs-常量指針">指針常量 VS. 常量指針</h3>

<p>前面是一個修飾詞，後面的是中心詞。</p>

<h4 id="常量指針">常量指針</h4>

<p><strong>常量指針</strong> 首先是一個指針，指向的是常量，即指向常量的指針；可以通過如下的方式定義：</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="k">const</span> <span class="kt">int</span> <span class="n">a</span> <span class="o">=</span> <span class="mi">7</span><span class="p">;</span>
<span class="k">const</span> <span class="kt">int</span> <span class="o">*</span><span class="n">p</span> <span class="o">=</span> <span class="o">&amp;</span><span class="n">a</span><span class="p">;</span></code></pre></figure>

<p>對於常量，我們不能對其內容進行修改；指針的內容本身是一個地址，通過常量指針指向一個常量，為的就是防止我們寫程序過程中對指針誤操作出現了修改常量這樣的錯誤，如果我們修改常量指針的所指向的空間的時候，編譯系統就會提示我們出錯信息。</p>

<p>在 C 語言中，通常定義的字符串會返回一個常量指針，因此字符串不能賦值給字符數組，只能賦值到指針。</p>

<p>總結一下，<font color="red">常量指針就是指向常量的指針，指針所指向的地址的內容是不可修改的，指針本身的內容是可以修改的</font> 。</p>

<h4 id="指針常量">指針常量</h4>

<p><strong>指針常量</strong>  首先是一個常量，再才是一個指針；可以通過如下的方式定義：</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="kt">int</span> <span class="n">a</span> <span class="o">=</span> <span class="mi">7</span><span class="p">;</span>
<span class="kt">int</span> <span class="o">*</span> <span class="k">const</span> <span class="n">p</span> <span class="o">=</span> <span class="o">&amp;</span><span class="n">a</span><span class="p">;</span> <span class="o">//</span> <span class="n">OR</span> <span class="kt">int</span> <span class="k">const</span> <span class="o">*</span><span class="n">p</span> <span class="o">=</span> <span class="o">&amp;</span><span class="n">a</span><span class="p">;</span></code></pre></figure>

<p>常量的性質是不能修改，指針的內容實際是一個地址，那麼指針常量就是內容不能修改的常量，即內容不能修改的指針，指針的內容是什麼呀？指針的內容是地址，所以，說到底，就是不能修改這個指針所指向的地址，一開始初始化，指向哪兒，它就只能指向哪兒了，不能指向其他的地方了，就像一個數組的數組名一樣，是一個固定的指針，不能對它移動操作。</p>

<p>它只是不能修改它指向的地方，但這個指向的地方里的內容是可以替換的，這和上面說的常量指針是完全不同的概念。</p>

<p>作一下總結，<font color="red">指針常量就是是指針的常量，它是不可改變地址的指針，但是可以對它所指向的內容進行修改</font> 。</p>

<p>源碼可以參考 <a href="https://github.com/Jin-Yang/examples/tree/master/c_cpp/c/const_pointer.c">github const_pointer.c</a> 。</p>

<h3 id="與一維數組">與一維數組</h3>

<p>假設有如下數組，</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="kt">int</span> <span class="n">Array</span><span class="p">[]</span> <span class="o">=</span> <span class="p">{</span><span class="mi">1</span><span class="p">,</span> <span class="mi">2</span><span class="p">,</span> <span class="mi">3</span><span class="p">,</span> <span class="mi">4</span><span class="p">};</span>
<span class="kt">int</span> <span class="o">*</span><span class="n">ptr</span> <span class="o">=</span> <span class="n">Array</span><span class="p">;</span></code></pre></figure>

<p>其中 Array 為指針常量，而 ptr 為指針變量，且 <code class="highlighter-rouge">ptr = &amp;Array[0]</code>，那麼如下的操作相同 <code class="highlighter-rouge">ptr[i] &lt;=&gt; *(ptr+i)</code> 以及 <code class="highlighter-rouge">Array[i] &lt;=&gt; *(Array + i)</code> 。</p>

<p>如下，簡單介紹下常見操作。</p>

<h4 id="ptr">*ptr++</h4>

<p>由於 <code class="highlighter-rouge">*</code> 和 <code class="highlighter-rouge">++</code> 優先級相同，結合性為由右至左，即 <code class="highlighter-rouge">*ptr++</code> 等價於 <code class="highlighter-rouge">*(ptr++)</code> ，由於 <code class="highlighter-rouge">++</code> 為後繼加，所以當得到 <code class="highlighter-rouge">*ptr</code> 後再處理 <code class="highlighter-rouge">++</code>；所以 <code class="highlighter-rouge">*ptr++</code> 等於 1，進行此項操作後 <code class="highlighter-rouge">*ptr</code> 等於 2。</p>

<p>執行的步驟為 1) <code class="highlighter-rouge">++</code> 操作符產生 ptr 的一份拷貝；2) <code class="highlighter-rouge">++</code> 操作符增加 ptr 的值；3) 在 ptr 上執行間接訪問操作。</p>

<h4 id="ptr-1">++*ptr</h4>

<p>利用優先級和結合性可得，<code class="highlighter-rouge">++*ptr</code> 等價於 <code class="highlighter-rouge">++(*ptr)</code> ，此時 <code class="highlighter-rouge">Array[0]</code> 為 2，返回 2 。</p>

<h4 id="ptr-2">*ptr++</h4>

<p>利用優先級和結合性可得，<code class="highlighter-rouge">*ptr++</code> 等價於 <code class="highlighter-rouge">*(ptr++)</code> ，返回 1，ptr 值加 1 。</p>

<h2 id="大小端">大小端</h2>

<p>當數據類型大於一個字節時，其所佔用的字節在內存中的順序存在兩種模式：小端模式 (little endian) 和大端模式 (big endian)，其中 MSB(Most Significant Bit) 最高有效位，LSB(Least Significant Bit) 最低有效位.</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">小端模式
MSB                             LSB
+-------------------------------+
|   1   |   2   |   3   |   4   | int 0x1234
+-------------------------------+
  0x03    0x02    0x01    0x00   Address

大端模式
MSB                             LSB
+-------------------------------+
|   1   |   2   |   3   |   4   | int 0x1234
+-------------------------------+
  0x00    0x01    0x02    0x03   Address</code></pre></figure>

<p>如下是一個測試程序。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#include &lt;stdio.h&gt;
</span>
<span class="kt">void</span> <span class="nf">main</span><span class="p">(</span><span class="kt">void</span><span class="p">)</span>
<span class="p">{</span>
   <span class="kt">int</span> <span class="n">test</span> <span class="o">=</span> <span class="mh">0x41424344</span><span class="p">;</span>
   <span class="kt">char</span><span class="o">*</span> <span class="n">pAddress</span> <span class="o">=</span> <span class="p">(</span><span class="kt">char</span><span class="o">*</span><span class="p">)</span><span class="o">&amp;</span><span class="n">test</span><span class="p">;</span>

<span class="cp">#ifdef DEBUG
</span>   <span class="n">printf</span><span class="p">(</span><span class="s">"int  Address:%x Value:%x</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span> <span class="p">(</span><span class="kt">unsigned</span> <span class="kt">int</span><span class="p">)</span><span class="o">&amp;</span><span class="n">test</span><span class="p">,</span> <span class="n">test</span><span class="p">);</span>
   <span class="n">printf</span><span class="p">(</span><span class="s">"</span><span class="se">\n</span><span class="s">------------------------------------</span><span class="se">\n</span><span class="s">"</span><span class="p">);</span>

   <span class="kt">int</span> <span class="n">j</span><span class="p">;</span>
   <span class="k">for</span><span class="p">(</span><span class="n">j</span><span class="o">=</span><span class="mi">0</span><span class="p">;</span> <span class="n">j</span><span class="o">&lt;=</span><span class="mi">3</span><span class="p">;</span> <span class="n">j</span><span class="o">++</span><span class="p">){</span>
      <span class="n">printf</span><span class="p">(</span><span class="s">"char Address:%x Value:%c</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span> <span class="p">(</span><span class="kt">unsigned</span> <span class="kt">int</span><span class="p">)</span><span class="n">pAddress</span><span class="p">,</span> <span class="o">*</span><span class="n">pAddress</span><span class="p">);</span>
      <span class="n">pAddress</span><span class="o">++</span><span class="p">;</span>
   <span class="p">}</span>
   <span class="n">printf</span><span class="p">(</span><span class="s">"------------------------------------</span><span class="se">\n\n</span><span class="s">"</span><span class="p">);</span>
   <span class="n">pAddress</span> <span class="o">=</span> <span class="p">(</span><span class="kt">char</span><span class="o">*</span><span class="p">)</span><span class="o">&amp;</span><span class="n">test</span><span class="p">;</span>
<span class="cp">#endif
</span>   <span class="k">if</span><span class="p">(</span><span class="o">*</span><span class="n">pAddress</span> <span class="o">==</span> <span class="mh">0x44</span><span class="p">)</span>
      <span class="n">printf</span><span class="p">(</span><span class="s">"Little-Endian</span><span class="se">\n</span><span class="s">"</span><span class="p">);</span>
   <span class="k">else</span> <span class="k">if</span><span class="p">(</span><span class="o">*</span><span class="n">pAddress</span> <span class="o">==</span> <span class="mh">0x41</span><span class="p">)</span>
      <span class="n">printf</span><span class="p">(</span><span class="s">"Big-Endian</span><span class="se">\n</span><span class="s">"</span><span class="p">);</span>
   <span class="k">else</span>
      <span class="n">printf</span><span class="p">(</span><span class="s">"Something Error!</span><span class="se">\n</span><span class="s">"</span><span class="p">);</span>
<span class="p">}</span></code></pre></figure>

<p>如果採用大端模式，則在向某一個函數通過向下類型裝換來傳遞參數時可能會出錯。如一個變量為 <code class="highlighter-rouge">int i=1;</code> 經過函數 <code class="highlighter-rouge">void foo(short *j);</code> 的調用，即 <code class="highlighter-rouge">foo((short*)&amp;i);</code>，在 foo() 中將 i 修改為 3 則最後得到的 i 為 0x301 。</p>

<p>大端模式規定 MSB 在存儲時放在低地址，在傳輸時 MSB 放在流的開始；小段模式反之。</p>

<h2 id="變參傳遞">變參傳遞</h2>

<p>頭文件 <code class="highlighter-rouge">stdarg.h</code> 中對相關的宏進行了定義，其基本內容如下所示：</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="k">typedef</span> <span class="kt">char</span> <span class="o">*</span> <span class="kt">va_list</span><span class="p">;</span>

<span class="cp">#define _INTSIZEOF(n)       ((sizeof(n) + sizeof(int) - 1) &amp; ~(sizeof(int) - 1))
#define va_start(arg_ptr,v) (arg_ptr = (va_list)&amp;v + _INTSIZEOF(v))
#define va_arg(arg_ptr,t)   (*(t *)((arg_ptr += _INTSIZEOF(t)) - _INTSIZEOF(t)))
#define va_end(arg_ptr)     (arg_ptr = (va_list)0)</span></code></pre></figure>

<!--
_INTSIZEOF(n)：我們可以經過測試得到，如果在上面的測試程序中用一個double類型（長度為8byte）和一個 long double類型（長度為10byte）做可變參數。我們發現double類型佔了8byte,而long double佔了12byte，即都是4的整數倍。這裡是因為對齊，即對Intel80x86 機器來說就是要求每個變量的地址都是sizeof(int)的倍數。char類型的參數只佔了1byte，但是它後面的參數因為對齊的關係只能跳過3byte存儲，而那3byte也就浪費掉了。<br><br>

<font color=blue><strong>va_start(arg_ptr,v)</strong></font>：使參數列表指針arg_ptr指向函數參數列表中的第一個可選參數。v是位於第一個可選參數之前的固定參數，如果有一函數的聲明是void va_test(char a, char b, char c, …)，則它的固定參數依次是a,b,c，最後一個固定參數v為c，因此就是va_start(arg_ptr, c)。<br><br>

<font color=blue><strong>va_arg(arg_ptr, t)</strong></font>：返回參數列表中指針arg_ptr所指的參數，返回類型為t，並使指針arg_ptr指向參數列表中下一個參數。首先計算(arg_ptr += _INTSIZEOF(t))，此時arg_ptr將指向下一個參數；((arg_ptr += _INTSIZEOF(t)) - _INTSIZEOF(t))等價於arg_ptr，通過(t *)將arg_ptr轉換為t類型的指針，並通過*取其值。<br><br>

<font color=blue><strong>va_end(arg_ptr)</strong></font>：使ap不再指向堆棧,而是跟NULL一樣。<br><br>
-->

<p>示例如下圖所示：</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#include &lt;stdarg.h&gt;
#include &lt;stdio.h&gt;
</span>
<span class="kt">int</span> <span class="nf">max</span><span class="p">(</span> <span class="kt">int</span> <span class="n">num</span><span class="p">,...)</span>
<span class="p">{</span>
    <span class="kt">int</span> <span class="n">m</span> <span class="o">=</span> <span class="o">-</span><span class="mh">0x7FFFFFFF</span><span class="p">;</span> <span class="cm">/* 32系統中最小的整數 */</span>
    <span class="kt">int</span> <span class="n">i</span> <span class="o">=</span> <span class="mi">0</span><span class="p">,</span> <span class="n">t</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span>
    <span class="kt">va_list</span> <span class="n">ap</span><span class="p">;</span>
    <span class="n">va_start</span><span class="p">(</span> <span class="n">ap</span><span class="p">,</span> <span class="n">num</span><span class="p">);</span>
    <span class="k">for</span><span class="p">(</span> <span class="n">i</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span> <span class="n">i</span> <span class="o">&lt;</span> <span class="n">num</span><span class="p">;</span> <span class="n">i</span><span class="o">++</span><span class="p">)</span> <span class="p">{</span>
         <span class="n">t</span> <span class="o">=</span> <span class="n">va_arg</span><span class="p">(</span> <span class="n">ap</span><span class="p">,</span> <span class="kt">int</span><span class="p">);</span>
         <span class="k">if</span><span class="p">(</span> <span class="n">t</span> <span class="o">&gt;</span> <span class="n">m</span><span class="p">)</span>
            <span class="n">m</span> <span class="o">=</span> <span class="n">t</span><span class="p">;</span>
    <span class="p">}</span>
    <span class="n">va_end</span><span class="p">(</span><span class="n">ap</span><span class="p">);</span>
    <span class="k">return</span> <span class="n">m</span><span class="p">;</span>
<span class="p">}</span>

<span class="kt">int</span> <span class="nf">main</span><span class="p">(</span><span class="kt">int</span> <span class="n">argc</span><span class="p">,</span><span class="kt">char</span> <span class="o">*</span><span class="n">argv</span><span class="p">[])</span>
<span class="p">{</span>
  <span class="kt">int</span> <span class="n">n</span><span class="p">,</span> <span class="n">m</span><span class="p">;</span>

  <span class="n">n</span> <span class="o">=</span> <span class="n">max</span><span class="p">(</span> <span class="mi">5</span><span class="p">,</span> <span class="mi">5</span><span class="p">,</span> <span class="mi">6</span><span class="p">,</span> <span class="mi">3</span><span class="p">,</span> <span class="mi">8</span><span class="p">,</span> <span class="mi">5</span><span class="p">);</span>
  <span class="n">m</span> <span class="o">=</span> <span class="n">max</span><span class="p">(</span> <span class="mi">7</span><span class="p">,</span> <span class="mi">5</span><span class="p">,</span> <span class="mi">1</span><span class="p">,</span> <span class="mi">9</span><span class="p">,</span> <span class="mi">8</span><span class="p">,</span> <span class="mi">5</span><span class="p">,</span> <span class="mi">7</span><span class="p">,</span> <span class="mi">0</span><span class="p">);</span>

  <span class="n">printf</span><span class="p">(</span><span class="s">"%d</span><span class="se">\t</span><span class="s">%d</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span><span class="n">n</span><span class="p">,</span><span class="n">m</span><span class="p">);</span>

  <span class="k">return</span> <span class="mi">0</span><span class="p">;</span>
<span class="p">}</span></code></pre></figure>

<p>其中函數傳參是通過棧傳遞，保存時從右至左依次入棧，以函數 <code class="highlighter-rouge">void func(int x, float y, char z)</code> 為例，調用該函數時 z、y、x 依次入棧，理論上來說，只要知道任意一個變量地址，以及所有變量的類型，那麼就可以通過指針移位獲取到所有的輸入變量。</p>

<p><code class="highlighter-rouge">va_list</code> 是一個字符指針，可以理解為指向當前參數的一個指針，取參必須通過這個指針進行。</p>

<p>在使用時，其步驟如下：</p>
<ol>
  <li>調用之前定義一個 va_list 類型的變量，一般變量名為 ap 。</li>
  <li>通過 va_start(ap, first) 初始化 ap ，指向可變參數列表中的第一個參數，其中 first 就是 … 之前的那個參數。</li>
  <li>接著調用 va_arg(ap, type) 依次獲取參數，其中第二個參數為獲取參數的類型，該宏會返回指定類型的值，並指向下一個變量。</li>
  <li>最後關閉定義的變量，實際上就是將指針賦值為 NULL 。</li>
</ol>

<p>其中的使用關鍵是如何獲取變量的類型，通常有兩種方法：A) 提前約定好，如上面的示例；B) 通過入參判斷，如 printf() 。</p>

<p>另外，常見的用法還有獲取省略號指定的參數，例如：</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="kt">void</span> <span class="nf">foobar</span><span class="p">(</span><span class="kt">char</span> <span class="o">*</span><span class="n">str</span><span class="p">,</span> <span class="kt">size_t</span> <span class="n">size</span><span class="p">,</span> <span class="k">const</span> <span class="kt">char</span> <span class="o">*</span><span class="n">fmt</span><span class="p">,</span> <span class="p">...)</span>
<span class="p">{</span>
	<span class="kt">va_list</span> <span class="n">ap</span><span class="p">;</span>
	<span class="n">va_start</span><span class="p">(</span><span class="n">ap</span><span class="p">,</span> <span class="n">fmt</span><span class="p">);</span>
	<span class="n">_vsnprintf</span><span class="p">(</span><span class="n">str</span><span class="p">,</span> <span class="n">size</span><span class="p">,</span> <span class="n">fmt</span><span class="p">,</span> <span class="n">ap</span><span class="p">);</span>
	<span class="n">va_end</span><span class="p">(</span><span class="n">ap</span><span class="p">);</span>
<span class="p">}</span></code></pre></figure>

<p>假設，在調用上述的函數時，如果在 <code class="highlighter-rouge">_vsnprintf()</code> 中會再調用類似的函數，那麼可以通過 <code class="highlighter-rouge">va_list args; va_copy(args, ap);</code> 複製一份。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">va_list args;
va_copy(args, ap);
some_other_foobar(str, size, fmt, args);</code></pre></figure>

<h2 id="調試">調試</h2>

<p>當調試時定義 DEBUG 輸出信息，通常有如下的幾種方式。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="c1">// 常用格式
</span><span class="cp">#ifdef DEBUG
</span>	<span class="cp">#define debug(fmt, args...) printf("debug: " fmt "\n", ##args) // OR
</span>	<span class="cp">#define debug(fmt, ...) printf("debug: " fmt "\n", ## __VA_ARGS__);
#else
</span>	<span class="cp">#define debug(fmt,args...)
#endif
</span>
<span class="c1">// 輸出文件名、函數名、行數
</span><span class="cp">#ifdef DEBUG
</span>	<span class="cp">#define debug(fmt, args...) printf("%s, %s, %d: " fmt , __FILE__, __FUNCTION__, __LINE__, ##args)
#else
</span>	<span class="cp">#define debug(fmt, args...)
#endif
</span>
<span class="c1">// 輸出信息含有彩色
</span><span class="cp">#ifdef DEBUG
</span>   <span class="cp">#define debug(fmt,args...)    \
      do{                        \
         printf("\033[32;40m");  \
         printf(fmt, ##args);    \
         printf("\033[0m");      \
      } while(0);
#else
</span>   <span class="cp">#define debug(fmt,args...)
#endif</span></code></pre></figure>

<p>另外，也可以通過如下方式判斷支持可變參數的格式。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#if defined __STDC_VERSION__ &amp;&amp; __STDC_VERSION__ &gt;= 199901L
</span>	<span class="cp">#define _debug(...) do { printf("debug: " __VA_ARGS__); putchar('\n'); } while(0);
</span>	<span class="cp">#define _warn(...)&nbsp; do { printf("warn : " __VA_ARGS__); putchar('\n'); } while(0);
</span>	<span class="cp">#define _error(...) do { printf("error: " __VA_ARGS__); putchar('\n'); } while(0);
#elif defined __GNUC__
</span>	<span class="cp">#define _debug(fmt, args...)&nbsp; do { printf("debug: " fmt, ## args); putchar('\n'); } while(0);
</span>	<span class="cp">#define _warn(fmt, args...)&nbsp;&nbsp; do { printf("warn: "&nbsp; fmt, ## args); putchar('\n'); } while(0);
</span>	<span class="cp">#define _error(fmt, args...)&nbsp; do { printf("error: " fmt, ## args); putchar('\n'); } while(0);
#endif</span></code></pre></figure>

<h2 id="對齊操作">對齊操作</h2>

<p>為了性能上的考慮，很多的平臺都會從某一個特定的地址開始讀取數據，比如偶地址。</p>

<p>數據結構中的數據變量都是按照定義的順序來定義，第一個變量的地址等同於數據結構的地址，結構體中的成員也要對齊，最後結構體也同樣需要對齊。對齊是指 <strong>起始地址對齊</strong>，其中對齊規則如下:</p>

<ol>
  <li>
    <p>數據成員對齊規則<br>結構體(struct)或聯合(union)的數據成員，第一個數據成員放在offset為0的地方，以後每個數據成員的對齊按照<code class="highlighter-rouge">#pragma pack</code>指定的數值n和這個數據成員自身長度中，比較小的那個進行。</p>
  </li>
  <li>
    <p>結構體(或聯合)的整體對齊規則<br>在數據成員完成各自對齊之後，結構體(或聯合)本身也要進行對齊，對齊將按照<code class="highlighter-rouge">#pragma pack</code>指定的數值n和結構體(或聯合)最大數據成員長度中，比較小的那個進行。</p>
  </li>
  <li>
    <p>當<code class="highlighter-rouge">#pragma pack</code>的n值等於或超過所有數據成員長度的時候，這個n值的大小將不生任何效果。</p>
  </li>
</ol>

<p>現舉例如下：</p>

<h4 id="1字節對齊">1字節對齊</h4>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#pragma pack(1)
</span><span class="k">struct</span> <span class="n">test_t</span> <span class="p">{</span>
    <span class="kt">int</span>    <span class="n">a</span><span class="p">;</span>   <span class="c1">// 長度4 &gt; 1 按1對齊；起始offset=0 0%1=0；存放位置區間[0,3]
</span>    <span class="kt">char</span>   <span class="n">b</span><span class="p">;</span>   <span class="c1">// 長度1 = 1 按1對齊；起始offset=4 4%1=0；存放位置區間[4]
</span>    <span class="kt">short</span>  <span class="n">c</span><span class="p">;</span>   <span class="c1">// 長度2 &gt; 1 按1對齊；起始offset=5 5%1=0；存放位置區間[5,6]
</span>    <span class="kt">char</span>   <span class="n">d</span><span class="p">;</span>   <span class="c1">// 長度1 = 1 按1對齊；起始offset=7 7%1=0；存放位置區間[7]
</span><span class="p">};</span>
<span class="cp">#pragma pack()  // 取消對齊</span></code></pre></figure>

<figure class="highlight"><pre><code class="language-text" data-lang="text">輸出結果 sizeof(struct test_t) = 8
整體對齊係數 min((max(int,short,char), 1) = 1
整體大小(size)=$(成員總大小8) 按 $(整體對齊係數) 圓整 = 8</code></pre></figure>

<h4 id="2字節對齊">2字節對齊</h4>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#pragma pack(2)
</span><span class="k">struct</span> <span class="n">test_t</span> <span class="p">{</span>
    <span class="kt">int</span>    <span class="n">a</span><span class="p">;</span>   <span class="c1">// 長度4 &gt; 2 按2對齊；起始offset=0 0%2=0；存放位置區間[0,3]
</span>    <span class="kt">char</span>   <span class="n">b</span><span class="p">;</span>   <span class="c1">// 長度1 &lt; 2 按1對齊；起始offset=4 4%1=0；存放位置區間[4]
</span>    <span class="kt">short</span>  <span class="n">c</span><span class="p">;</span>   <span class="c1">// 長度2 = 2 按2對齊；起始offset=6 6%2=0；存放位置區間[6,7]
</span>    <span class="kt">char</span>   <span class="n">d</span><span class="p">;</span>   <span class="c1">// 長度1 &lt; 2 按1對齊；起始offset=8 8%1=0；存放位置區間[8]
</span><span class="p">};</span>
<span class="cp">#pragma pack()  // 取消對齊</span></code></pre></figure>

<figure class="highlight"><pre><code class="language-text" data-lang="text">輸出結果 sizeof(struct test_t) = 10
整體對齊係數 = min((max(int,short,char), 2) = 2
整體大小(size)=$(成員總大小9) 按 $(整體對齊係數) 圓整 = 10</code></pre></figure>

<h4 id="4字節對齊">4字節對齊</h4>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#pragma pack(4)
</span><span class="k">struct</span> <span class="n">test_t</span> <span class="p">{</span>
    <span class="kt">int</span>    <span class="n">a</span><span class="p">;</span>   <span class="c1">// 長度4 = 4 按4對齊；起始offset=0 0%4=0；存放位置區間[0,3]
</span>    <span class="kt">char</span>   <span class="n">b</span><span class="p">;</span>   <span class="c1">// 長度1 &lt; 4 按1對齊；起始offset=4 4%1=0；存放位置區間[4]
</span>    <span class="kt">short</span>  <span class="n">c</span><span class="p">;</span>   <span class="c1">// 長度2 &lt; 4 按2對齊；起始offset=6 6%2=0；存放位置區間[6,7]
</span>    <span class="kt">char</span>   <span class="n">d</span><span class="p">;</span>   <span class="c1">// 長度1 &lt; 4 按1對齊；起始offset=8 8%1=0；存放位置區間[8]
</span><span class="p">};</span>
<span class="cp">#pragma pack() // 取消對齊</span></code></pre></figure>

<figure class="highlight"><pre><code class="language-text" data-lang="text">輸出結果 sizeof(struct test_t) = 12
整體對齊係數 = min((max(int,short,char), 4) = 4
整體大小(size)=$(成員總大小9) 按 $(整體對齊係數) 圓整 = 12</code></pre></figure>

<h4 id="8字節對齊">8字節對齊</h4>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#pragma pack(8)
</span><span class="k">struct</span> <span class="n">test_t</span> <span class="p">{</span>
    <span class="kt">int</span>    <span class="n">a</span><span class="p">;</span>   <span class="c1">// 長度4 &lt; 8 按4對齊；起始offset=0 0%4=0；存放位置區間[0,3]
</span>    <span class="kt">char</span>   <span class="n">b</span><span class="p">;</span>   <span class="c1">// 長度1 &lt; 8 按1對齊；起始offset=4 4%1=0；存放位置區間[4]
</span>    <span class="kt">short</span>  <span class="n">c</span><span class="p">;</span>   <span class="c1">// 長度2 &lt; 8 按2對齊；起始offset=6 6%2=0；存放位置區間[6,7]
</span>    <span class="kt">char</span>   <span class="n">d</span><span class="p">;</span>   <span class="c1">// 長度1 &lt; 8 按1對齊；起始offset=8 8%1=0；存放位置區間[8]
</span><span class="p">};</span>
<span class="cp">#pragma pack()  // 取消對齊</span></code></pre></figure>

<figure class="highlight"><pre><code class="language-text" data-lang="text">輸出結果 sizeof(struct test_t) = 12
整體對齊係數 = min((max(int,short,char), 8) = 4
整體大小(size)=$(成員總大小9) 按 $(整體對齊係數) 圓整 = 12</code></pre></figure>

<h3 id="__attributealignedn">__attribute((aligned(n)))</h3>

<p>另一種方式是 <code class="highlighter-rouge">__attribute((aligned(n)))</code> 讓所作用的結構成員對齊在 <code class="highlighter-rouge">n</code> 字節自然邊界上，如果結構中有成員長度大於 <code class="highlighter-rouge">n </code>，則按照最大的成員的長度對齊。</p>

<p>示例如下：</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">struct test_t {
    int    a;
    char   b;
    short  c;
    char   d;
} __attribute((aligned(n)));</code></pre></figure>

<p><code class="highlighter-rouge">__attribute__((packed))</code> 取消編譯過程中的優化對齊，按照實際佔用字節數進行對齊。</p>

<!--
採用宏進行對齊操作，計算a以size為倍數的上下界數，
#define alignment_down(a, size)  (a & (~(size - 1)))
#define alignment_up(a, size)    ((a + size - 1) & (~(size - 1)))
-->

<p>詳見參考程序 <a href="https://github.com/Jin-Yang/examples/tree/master/c_cpp/c/align.c">github align.c</a> 。</p>

<h2 id="參數解析">參數解析</h2>

<p><code class="highlighter-rouge">getopt()</code> 是採用緩衝機制，因此對於多線程編程是 <strong>不安全</strong> 的。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">#include &lt;unistd.h&gt;
// 選項的參數指針
extern char *optarg;

// 初值為1, 下次調用時，從optind位置開始檢測，通過agrv[optind]可以得到下一個參數，從而可以自行檢測
extern int optind;

// 對於不能識別的參數將輸出錯誤信息，可以將opterr設置為0，從而阻止向stderr輸出錯誤信息
extern int opterr;

// 如果選項字符不再optstring中則返回':'或'?'，並將字符保存在optopt中
extern int optopt;

int getopt(int argc, char * const argv[],const char *optstring);
描述：
  該函數處理"-"起始的參數，有些平臺可能支持"--"
參數：
  argc、argv分別為main()傳入的參數；其中optstring可以有如下的選項:
    單個字符，表示選項。
    單個字符後接一個冒號，表示該選項後必須跟一個參數，參數緊跟在選項後或者以空格隔開，該參數的指針賦給optarg。
    單個字符後跟兩個冒號，表示該選項後必須跟一個參數，參數必須緊跟在選項後不能以空格隔開，否則optarg指向為NULL，
        該參數的指針賦給optarg，這個特性是GNU的擴展。</code></pre></figure>

<p>如 <code class="highlighter-rouge">optstring="ab:c::d::"</code> ，命令行為 <code class="highlighter-rouge">getopt.exe -a -b host -ckeke -d haha</code>，在這個命令行參數中，<code class="highlighter-rouge">-a</code> <code class="highlighter-rouge">-b</code> 和 <code class="highlighter-rouge">-c</code> 是選項元素，去掉 <code class="highlighter-rouge">'-'</code>，a b c 就是選項。</p>

<p>host 是 b 的參數，keke 是 c 的參數，但 haha 並不是 d 的參數，因為它們中間有空格隔開。</p>

<p><strong>注意</strong>：如果 optstring 中的字符串以 <code class="highlighter-rouge">'+'</code> 加號開頭或者環境變量 <code class="highlighter-rouge">POSIXLY_CORRE</code> 被設置，那麼一遇到不包含選項的命令行參數，getopt 就會停止，返回 -1；命令參數中的 <code class="highlighter-rouge">"--"</code> 用來強制終止掃描。</p>

<p>默認情況下 getopt 會重新排列命令行參數的順序，所以到最後所有不包含選項的命令行參數都排到最後，如 <code class="highlighter-rouge">getopt -a ima -b host -ckeke -d haha</code>，都最後命令行參數的順序是 <code class="highlighter-rouge">-a -b host -ckeke -d ima haha</code> 。</p>

<p>如果檢測到設置的參數項，則返回參數項；如果檢測完成則返回 -1；如果有不能識別的參數則將該參數保存在 optopt 中，輸出錯誤信息到 stderr，如果 optstring 以 <code class="highlighter-rouge">':'</code> 開頭則返回 <code class="highlighter-rouge">':'</code> 否則返回 <code class="highlighter-rouge">'?'</code>。</p>

<p>源碼可以參考 <a href="https://github.com/Jin-Yang/examples/tree/master/c_cpp/c/getopt.c">github getopt.c</a> 。</p>

<h3 id="長選項">長選項</h3>

<figure class="highlight"><pre><code class="language-text" data-lang="text">#include &lt;getopt.h&gt;
int getopt_long(int argc, char * const argv[],
    const char *optstring, const struct option *longopts, int *longindex);
int getopt_long_only(int argc, char * const argv[],
    const char *optstring, const struct option *longopts, int *longindex);

描述：
  該函數與getopt函數類似，不過可以接收長選項(通常以"--"開頭)，如果只接收長選項則optstring應該設置為""，而非NULL。</code></pre></figure>

<!--
<font color=blue size=3><strong>參數：</strong></font><br>
    前3個參數與getopt類似，struct option定義在&lt;getopt.h&gt;中，<br>
<pre>
struct option {
    const char *name;   //name表示的是長參數名
    int has_arg; // 有3個值，
                 // no_argument(或者是0)，表示該參數後面不跟參數值
                 // required_argument(或者是1),表示該參數後面一定要跟個參數值
                 // optional_argument(或者是2),表示該參數後面可以跟，也可以不跟參數值
    int *flag; // 用來決定，getopt_long()的返回值到底是什麼。如果flag是null，
               // 則函數會返回與該項option匹配的val值；否則如果找到參數則返回0，
               // flag指向該option的val，如果沒有找到則保持不變。
    int val; // 和flag聯合決定返回值
}</pre>
    如果longindex沒有設置為空則，longindex指向longopts<br><br>
-->

<p>源碼可以參考 <a href="https://github.com/Jin-Yang/examples/tree/master/c_cpp/c/getopt_long.c">github getopt_long.c</a> 。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ getopt-long a.out -a -b -c -x foo --add --back  --check --extra=foo</code></pre></figure>

<p><code class="highlighter-rouge">getopt_long_only()</code> 和 <code class="highlighter-rouge">getopt_long()</code> 類似，但是 <code class="highlighter-rouge">'-'</code> 和 <code class="highlighter-rouge">'--'</code> 均被認為是長選項，只有當 <code class="highlighter-rouge">'-'</code> 沒有對應的選項時才會與相應的短選項匹配。</p>

<h2 id="整型溢出">整型溢出</h2>

<p>以 8-bits 的數據為例，unsigned 取值範圍為 0~255，signed 的取值範圍為 -128~127。在計算機中數據以補碼（正數原碼與補碼相同，原碼=除符號位的補碼求反+1）的形式存在，且規定 0x80 為-128 。</p>

<h3 id="無符號整數">無符號整數</h3>

<p>對於無符號整數，當超過 255 後將會溢出，常見的是 Linux 內核中的 jiffies 變量，jiffies 以及相關的宏保存在 linux/jiffies.h 中，如果 a 發生在 b 之後則返回真，即 a&gt;b 返回真，無論是否有溢出。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#define time_after(a,b)     \
    (typecheck(unsigned long, a) &amp;&amp; \
     typecheck(unsigned long, b) &amp;&amp; \
     ((long)((b) - (a)) &lt; 0))</span></code></pre></figure>

<!--
    下面以8-bit數據進行講解，在 0x00~0x7F 範圍內，表示 0~127；在 0x80~0xFF 範圍內表示 -128~-1，對於可能出現的數據有四種情況：<ol><li>
        都位於0x00~0x7F<br>
        如果a發生在b之後，則a > b。(char)b - (char)a < 0，100 - 125 = -25 < 0。</li><br><li>

        都位於0x80~0xFF<br>
        如果a發生在b之後，則a > b。(char)b - (char)a < 0，150 - 180 = -106 - (-76) = -30 < 0。</li><br><li>

        b位於0x00~0x7F，a位於0x80~0xFF<br>
        如果a發生在b之後，則a > b。(char)b - (char)a < 0，100 - 150 = 100 - (-106) = 206 = -50 < 0。<br><strong>注意，此時在a-b<=128時有效。</strong></li><br><li>

        a位於0x00~0x7F，b位於0x80~0xFF<br>
    如果a發生在b之後，此時有溢出。(char)b - (char)a < 0，150 - 10 = -106 - 10 = -116 < 0。<br><strong>注意，此時在a-b<=128時有效。</strong></li></ol>

    <div style="padding: 10pt 0pt 10pt 0pt ;" align="right">
    <table frame="void" width="90%">
        <tbody><tr><td><b>Tips:</b><br><span style="color : #009000"><font size="-1">typecheck位於相同文件夾下的typecheck.h文件中，當兩個參數不是同一個類型是將會產生警告，提醒用戶注意，只是提醒。</font></span></td>
    <td><img src="src/info.png" width="80" heigh="80"></td></tr></tbody></table></div>
    </p>
-->

<!--
<br id="Error_handling"><br><br>
<h1>錯誤處理_OK</h1>
<p>
    需要包含的頭文件及函數原型，<pre>
#include &lt;stdio.h&gt;
void perror(const char *s);

#include &lt;errno.h&gt;
const char *sys_errlist[];
int sys_nerr;  // 前兩個變量是參考了BSD，glibc中保存在&lt;stdio.h&gt;
int errno;</pre>

    如果s不為NULL且*s != '\0'則輸出s並加上": "+錯誤信息+換行，否則只輸出錯誤信息+換行。通常s應該為出錯的函數名稱，此函數需要調用errno。如果函數調用錯誤後沒有直接調用<tt>perror()</tt>，則為防止其他錯誤將其覆蓋，需要保存errno。<br><br>


sys_errlist保存了所有的錯誤信息，errno(注意出現錯誤時進行了設置，但是正確調用時可能沒有清除)為索引，最大的索引為<tt>(sys_nerr - 1)</tt>。當直接調用sys_errlist時可能錯誤信息還沒有添加。
</p>

## errno

http://www.bo56.com/linux%E4%B8%ADc%E8%AF%AD%E8%A8%80errno%E7%9A%84%E4%BD%BF%E7%94%A8/
https://www.ibm.com/developerworks/cn/aix/library/au-errnovariable/

-->

<h2 id="clang">Clang</h2>

<p><img src="https://jin-yang.github.io/images/programs/clang_logo.png" alt="clang logo" title="clang logo" class="pull-center"></p>

<p>Clang 是一個 C++ 編寫，基於 LLVM 的 C/C++、Objective-C 語言的輕量級編譯器，在 2013.04 開始，已經全面支持 C++11 標準。</p>

<h3 id="pragma">pragma</h3>

<p><code class="highlighter-rouge">#pragma</code> 宏定義在本質上是聲明，常用的功能就是註釋，尤其是給 Code 分段註釋；另外，還支持處理編譯器警告。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#pragma clang diagnostic push
</span>
<span class="c1">//----- 方法棄用告警
</span><span class="cp">#pragma clang diagnostic ignored "-Wdeprecated-declarations"
</span><span class="c1">//----- 不兼容指針類型
</span><span class="cp">#pragma clang diagnostic ignored "-Wincompatible-pointer-types"
</span><span class="c1">//----- 未使用變量
</span><span class="cp">#pragma clang diagnostic ignored "-Wunused-variable"
</span><span class="c1">//----- 無返回值
</span><span class="cp">#pragma clang diagnostic ignored "-Wmissing-noreturn"
</span><span class="c1">//... ...
</span>
<span class="cp">#pragma clang diagnostic pop</span></code></pre></figure>

<h2 id="__attribute__">__attribute__</h2>

<h3 id="__attribute__format">__attribute__((format))</h3>

<p>該屬性用於自實現的字符串格式化參數添加類似 printf() 的格式化參數的校驗，判斷需要格式化的參數與入參是否相同。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">format (archetype, string-index, first-to-check)

__attribute__((format(printf,m,n)))
__attribute__((format(scanf,m,n)))
  m : 第m個參數為格式化字符串(從1開始)；
  n : 變長參數(也即"...")的第一個參數排在總參數的第幾個；</code></pre></figure>

<p>如下是使用示例。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">void myprint(const char *format,...) __attribute__((format(printf,1,2)));
void myprint(int l，const char *format,...) __attribute__((format(printf,2,3)));</code></pre></figure>

<p>如下是一個簡單的使用示例。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#include &lt;stdio.h&gt;
</span>
<span class="k">extern</span> <span class="kt">void</span> <span class="n">myprint</span><span class="p">(</span><span class="k">const</span> <span class="kt">char</span> <span class="o">*</span><span class="n">format</span><span class="p">,...)</span> <span class="n">__attribute__</span><span class="p">((</span><span class="n">format</span><span class="p">(</span><span class="n">printf</span><span class="p">,</span><span class="mi">1</span><span class="p">,</span><span class="mi">2</span><span class="p">)));</span>

<span class="kt">int</span> <span class="nf">myprint</span><span class="p">(</span><span class="kt">char</span> <span class="o">*</span><span class="n">fmt</span><span class="p">,</span> <span class="p">...)</span>
<span class="p">{</span>
    <span class="kt">int</span> <span class="n">result</span><span class="p">;</span>
    <span class="kt">va_list</span> <span class="n">args</span><span class="p">;</span>
    <span class="n">va_start</span><span class="p">(</span><span class="n">args</span><span class="p">,</span> <span class="n">fmt</span><span class="p">);</span>
    <span class="n">fputs</span><span class="p">(</span><span class="s">"foobar: "</span><span class="p">,</span> <span class="n">stderr</span><span class="p">);</span>
    <span class="n">result</span> <span class="o">=</span> <span class="n">vfprintf</span><span class="p">(</span><span class="n">stderr</span><span class="p">,</span> <span class="n">fmt</span><span class="p">,</span> <span class="n">args</span><span class="p">);</span>
    <span class="n">va_end</span><span class="p">(</span><span class="n">args</span><span class="p">);</span>
    <span class="k">return</span> <span class="n">result</span><span class="p">;</span>
<span class="p">}</span>
<span class="kt">int</span> <span class="nf">main</span><span class="p">(</span><span class="kt">int</span> <span class="n">argc</span><span class="p">,</span> <span class="kt">char</span> <span class="o">**</span><span class="n">argv</span><span class="p">)</span>
<span class="p">{</span>
    <span class="n">myprint</span><span class="p">(</span><span class="s">"i=%d</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span><span class="mi">6</span><span class="p">);</span>
    <span class="n">myprint</span><span class="p">(</span><span class="s">"i=%s</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span><span class="mi">6</span><span class="p">);</span>
    <span class="n">myprint</span><span class="p">(</span><span class="s">"i=%s</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span><span class="s">"abc"</span><span class="p">);</span>
    <span class="n">myprint</span><span class="p">(</span><span class="s">"%s,%d,%d</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span><span class="mi">1</span><span class="p">,</span><span class="mi">2</span><span class="p">);</span>
 <span class="k">return</span> <span class="mi">0</span><span class="p">;</span>
<span class="p">}</span></code></pre></figure>

<p>編譯時添加 <code class="highlighter-rouge">-Wall</code> 就會打印 Warning 信息，如果去除，實際上不會顯示任何信息，通常可以提前發現常見的問題。</p>

<h3 id="__attribute__constructor">__attribute__((constructor))</h3>

<p>這是 GCC 的擴展機制，通過上述的屬性，可以使程序在開始執行或停止時調用指定的函數。</p>

<p><code class="highlighter-rouge">__attribute__((constructor))</code> 在 main() 之前執行，<code class="highlighter-rouge">__attribute__((destructor))</code> 在 main() 執行結束之後執行。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#include &lt;stdio.h&gt;
#include &lt;stdlib.h&gt;
</span>
<span class="k">static</span>  <span class="nf">__attribute__</span><span class="p">((</span><span class="n">constructor</span><span class="p">))</span> <span class="kt">void</span> <span class="n">before</span><span class="p">()</span>
<span class="p">{</span>
    <span class="n">printf</span><span class="p">(</span><span class="s">"Hello World</span><span class="se">\n</span><span class="s">"</span><span class="p">);</span>
<span class="p">}</span>

<span class="k">static</span>  <span class="nf">__attribute__</span><span class="p">((</span><span class="n">destructor</span><span class="p">))</span> <span class="kt">void</span> <span class="n">after</span><span class="p">()</span>
<span class="p">{</span>
    <span class="n">printf</span><span class="p">(</span><span class="s">"Bye World!</span><span class="se">\n</span><span class="s">"</span><span class="p">);</span>
<span class="p">}</span>

<span class="kt">int</span> <span class="nf">main</span><span class="p">(</span><span class="kt">int</span> <span class="n">args</span><span class="p">,</span><span class="kt">char</span> <span class="o">**</span> <span class="n">argv</span><span class="p">)</span>
<span class="p">{</span>
    <span class="n">printf</span><span class="p">(</span><span class="s">"Live...</span><span class="se">\n</span><span class="s">"</span><span class="p">);</span>
    <span class="k">return</span> <span class="n">EXIT_SUCCESS</span><span class="p">;</span>
<span class="p">}</span></code></pre></figure>

<p>如果有多個函數，可以指定優先級，其中 0~100 (含100)系統保留。在 main 之前順序為有小到大，退出時順序為由大到小。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#include &lt;stdio.h&gt;
#include &lt;stdlib.h&gt;
</span>
<span class="k">static</span>  <span class="nf">__attribute__</span><span class="p">((</span><span class="n">constructor</span><span class="p">(</span><span class="mi">102</span><span class="p">)))</span> <span class="kt">void</span> <span class="n">before102</span><span class="p">()</span>
<span class="p">{</span>
    <span class="n">printf</span><span class="p">(</span><span class="s">"Hello World 102</span><span class="se">\n</span><span class="s">"</span><span class="p">);</span>
<span class="p">}</span>

<span class="k">static</span>  <span class="nf">__attribute__</span><span class="p">((</span><span class="n">destructor</span><span class="p">(</span><span class="mi">102</span><span class="p">)))</span> <span class="kt">void</span> <span class="n">after102</span><span class="p">()</span>
<span class="p">{</span>
    <span class="n">printf</span><span class="p">(</span><span class="s">"Bye World! 102</span><span class="se">\n</span><span class="s">"</span><span class="p">);</span>
<span class="p">}</span>

<span class="k">static</span>  <span class="nf">__attribute__</span><span class="p">((</span><span class="n">constructor</span><span class="p">(</span><span class="mi">101</span><span class="p">)))</span> <span class="kt">void</span> <span class="n">before101</span><span class="p">()</span>
<span class="p">{</span>
    <span class="n">printf</span><span class="p">(</span><span class="s">"Hello World 101</span><span class="se">\n</span><span class="s">"</span><span class="p">);</span>
<span class="p">}</span>

<span class="k">static</span>  <span class="nf">__attribute__</span><span class="p">((</span><span class="n">destructor</span><span class="p">(</span><span class="mi">101</span><span class="p">)))</span> <span class="kt">void</span> <span class="n">after101</span><span class="p">()</span>
<span class="p">{</span>
    <span class="n">printf</span><span class="p">(</span><span class="s">"Bye World! 101</span><span class="se">\n</span><span class="s">"</span><span class="p">);</span>
<span class="p">}</span>

<span class="kt">int</span> <span class="nf">main</span><span class="p">(</span><span class="kt">int</span> <span class="n">args</span><span class="p">,</span><span class="kt">char</span> <span class="o">**</span> <span class="n">argv</span><span class="p">)</span>
<span class="p">{</span>
    <span class="n">printf</span><span class="p">(</span><span class="s">"Live...</span><span class="se">\n</span><span class="s">"</span><span class="p">);</span>
    <span class="k">return</span> <span class="n">EXIT_SUCCESS</span><span class="p">;</span>
<span class="p">}</span></code></pre></figure>

<p>在使用時也可以先聲明然再定義</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#include &lt;stdio.h&gt;
#include &lt;stdlib.h&gt;
</span>
<span class="kt">void</span> <span class="n">before</span><span class="p">()</span> <span class="n">__attribute__</span><span class="p">((</span><span class="n">constructor</span><span class="p">));</span>
<span class="kt">void</span> <span class="n">after</span><span class="p">()</span> <span class="n">__attribute__</span><span class="p">((</span><span class="n">destructor</span><span class="p">));</span>

<span class="kt">void</span> <span class="nf">before</span><span class="p">()</span>
<span class="p">{</span>
    <span class="n">printf</span><span class="p">(</span><span class="s">"Hello World</span><span class="se">\n</span><span class="s">"</span><span class="p">);</span>
<span class="p">}</span>

<span class="kt">void</span> <span class="nf">after</span><span class="p">()</span>
<span class="p">{</span>
    <span class="n">printf</span><span class="p">(</span><span class="s">"Bye World!</span><span class="se">\n</span><span class="s">"</span><span class="p">);</span>
<span class="p">}</span>

<span class="kt">int</span> <span class="nf">main</span><span class="p">(</span><span class="kt">int</span> <span class="n">args</span><span class="p">,</span><span class="kt">char</span> <span class="o">**</span> <span class="n">argv</span><span class="p">)</span>
<span class="p">{</span>
    <span class="n">printf</span><span class="p">(</span><span class="s">"Live...</span><span class="se">\n</span><span class="s">"</span><span class="p">);</span>
    <span class="k">return</span> <span class="n">EXIT_SUCCESS</span><span class="p">;</span>
<span class="p">}</span></code></pre></figure>

<h3 id="__attribute__visibility">__attribute__((visibility))</h3>

<p>程序調用某個函數 A，而 A 函數存在於兩個動態鏈接庫 liba.so 和 libb.so 中，並且程序執行需要鏈接這兩個庫，此時程序調用的 A 函數到底是來自於 a 還是 b 呢？</p>

<p>這取決於鏈接時的順序，首先鏈接的庫會更新符號表，比如先鏈接 liba.so，這時候通過 liba.so 的導出符號表就可以找到函數 A 的定義，並加入到符號表中，而不會再查找 libb.so 。</p>

<p>也就是說，這裡的調用嚴重的依賴於鏈接庫加載的順序，可能會導致混亂。</p>

<p>gcc 的擴展中有如下屬性 <code class="highlighter-rouge">__attribute__ ((visibility("hidden")))</code> 可以用於抑制將一個函數的名稱被導出，對連接該庫的程序文件來說，該函數是不可見的，使用的方法如下：</p>

<!--
-fvisibility=default|internal|hidden|protected
gcc的visibility是說，如果編譯的時候用了這個屬性，那麼動態庫的符號都是hidden的，除非強制聲明。
-->

<h4 id="1-創建一個c源文件">1. 創建一個c源文件</h4>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#include&lt;stdio.h&gt;
#include&lt;stdlib.h&gt;
</span>
<span class="n">__attribute</span> <span class="p">((</span><span class="n">visibility</span><span class="p">(</span><span class="s">"default"</span><span class="p">)))</span> <span class="kt">void</span> <span class="n">not_hidden</span><span class="p">()</span>
<span class="p">{</span>
    <span class="n">printf</span><span class="p">(</span><span class="s">"exported symbol</span><span class="se">\n</span><span class="s">"</span><span class="p">);</span>
<span class="p">}</span>

<span class="kt">void</span> <span class="n">is_hidden</span><span class="p">()</span>
<span class="p">{</span>
    <span class="n">printf</span><span class="p">(</span><span class="s">"hidden one</span><span class="se">\n</span><span class="s">"</span><span class="p">);</span>
<span class="p">}</span></code></pre></figure>

<p>想要做的是，第一個函數符號可以被導出，第二個被隱藏。</p>

<h4 id="2-生成動態庫">2. 生成動態庫</h4>

<p>先編譯成一個動態庫，使用到屬性 <code class="highlighter-rouge">-fvisibility</code> 。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">----- 編譯
$ gcc -shared -o libvis.so -fvisibility=hidden foobar.c

----- 查看符號鏈接
# readelf -s libvis.so |grep hidden
 7: 0000040c 20 FUNC GLOBAL DEFAULT 11 not_hidden
48: 00000420 20 FUNC LOCAL  HIDDEN  11 is_hidden
51: 0000040c 20 FUNC GLOBAL DEFAULT 11 not_hidden</code></pre></figure>

<p>可以看到，屬性確實有作用了。</p>

<h4 id="3-編譯鏈接">3. 編譯鏈接</h4>

<p>現在試圖鏈接程序。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="kt">int</span> <span class="nf">main</span><span class="p">()</span>
<span class="p">{</span>
    <span class="n">not_hidden</span><span class="p">();</span>
    <span class="n">is_hidden</span><span class="p">();</span>
    <span class="k">return</span> <span class="mi">0</span><span class="p">;</span>
<span class="p">}</span></code></pre></figure>

<p>試圖編譯成一個可執行文件，鏈接到剛才生成的動態庫。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ gcc -o exe main.c -L ./ -lvis
/tmp/cckYTHcl.o: In function `main':
main.c:(.text+0x17): undefined reference to `is_hidden'</code></pre></figure>

<p>說明瞭 hidden 確實起到作用了。</p>

<h3 id="__attribute__sentinel">__attribute__((sentinel))</h3>

<p>該屬性表示，此可變參數函數需要一個 <code class="highlighter-rouge">NULL</code> 作為最後一個參數，這個 <code class="highlighter-rouge">NULL</code> 參數一般被叫做 “哨兵參數”。例如，有如下程序：</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#include &lt;stdio.h&gt;
#include &lt;stdarg.h&gt;
#include &lt;string.h&gt;
#include &lt;malloc.h&gt;
</span>
<span class="kt">void</span> <span class="nf">foo</span><span class="p">(</span><span class="kt">char</span> <span class="o">*</span><span class="n">first</span><span class="p">,</span> <span class="p">...)</span>
<span class="p">{</span>
    <span class="kt">char</span> <span class="o">*</span><span class="n">p</span> <span class="o">=</span> <span class="p">(</span><span class="kt">char</span> <span class="o">*</span><span class="p">)</span><span class="n">malloc</span><span class="p">(</span><span class="mi">100</span><span class="p">),</span> <span class="o">*</span><span class="n">q</span> <span class="o">=</span> <span class="n">first</span><span class="p">;</span>

    <span class="kt">va_list</span> <span class="n">args</span><span class="p">;</span>
    <span class="n">va_start</span><span class="p">(</span><span class="n">args</span><span class="p">,</span> <span class="n">first</span><span class="p">);</span>
    <span class="k">while</span> <span class="p">(</span><span class="n">q</span><span class="p">)</span> <span class="p">{</span>
        <span class="n">strcat</span><span class="p">(</span><span class="n">p</span><span class="p">,</span> <span class="n">q</span><span class="p">);</span>
        <span class="n">q</span> <span class="o">=</span> <span class="n">va_arg</span><span class="p">(</span><span class="n">args</span><span class="p">,</span> <span class="kt">char</span> <span class="o">*</span><span class="p">);</span>
    <span class="p">}</span>
    <span class="n">va_end</span><span class="p">(</span><span class="n">args</span><span class="p">);</span>

    <span class="n">printf</span><span class="p">(</span><span class="s">"%s</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span> <span class="n">p</span><span class="p">);</span>

    <span class="n">free</span><span class="p">(</span><span class="n">p</span><span class="p">);</span>
<span class="p">}</span>

<span class="kt">int</span> <span class="nf">main</span><span class="p">(</span><span class="kt">void</span><span class="p">)</span>
<span class="p">{</span>
    <span class="n">foo</span><span class="p">(</span><span class="s">"Hello"</span><span class="p">,</span> <span class="s">"World"</span><span class="p">);</span>

    <span class="k">return</span> <span class="mi">0</span><span class="p">;</span>
<span class="p">}</span></code></pre></figure>

<p>當通過 <code class="highlighter-rouge">gcc main.c -Wall</code> 進行編譯時，會發現沒有任何警告，不過很顯然，調用 <code class="highlighter-rouge">foo()</code> 時最後一個參數應該是個 <code class="highlighter-rouge">NULL</code> 以表明 “可變參數就這麼多”。</p>

<p>編譯完成後，如果嘗試運行則會打印一些亂碼，顯然是有問題的。</p>

<p>正常來說，應該通過如下方式調用 <code class="highlighter-rouge">foo("Hello", "World", NULL);</code>，為此，就需要用到了上述的屬性，用於表示最後一個參數需要為 <code class="highlighter-rouge">NULL</code> 。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="kt">void</span> <span class="n">foo</span><span class="p">(</span><span class="kt">char</span> <span class="o">*</span><span class="n">first</span><span class="p">,</span> <span class="p">...)</span> <span class="n">__attribute__</span><span class="p">((</span><span class="n">sentinel</span><span class="p">));</span></code></pre></figure>

<p>這樣再不寫哨兵參數，在編譯時編譯器就會發出警告了。</p>

<p>但是，對於同樣使用可變參數的 <code class="highlighter-rouge">printf()</code> 來說，為什麼就不需要哨兵屬性，實際上，通過第一個參數就可以確定需要多少個參數，如下。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cm">/*
 * 第一個參數中規定了有兩個待打印項，所以打印時會取 "string" 和 1，多寫的 "another_string" 會被忽略。
 * printf()在被調用時明確知道此次調用需要多少個參數，所以也就無需哨兵參數的幫忙。
 */</span>
<span class="n">printf</span><span class="p">(</span><span class="s">"%s %d</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span> <span class="s">"string"</span><span class="p">,</span> <span class="mi">1</span><span class="p">,</span> <span class="s">"another_string"</span><span class="p">);</span></code></pre></figure>

<h2 id="atexit">atexit()</h2>

<p>很多時候我們需要在程序退出的時候做一些諸如釋放資源的操作，但程序退出的方式有很多種，比如 main() 函數運行結束、在程序的某個地方用 exit() 結束程序、用戶通過 Ctrl+C 或 Ctrl+break 操作來終止程序等等，因此需要有一種與程序退出方式無關的方法來進行程序退出時的必要處理。</p>

<p>方法就是用 atexit() 函數來註冊程序正常終止時要被調用的函數。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#include &lt;stdlib.h&gt;
</span><span class="kt">int</span> <span class="n">atexit</span><span class="p">(</span><span class="kt">void</span><span class="p">(</span><span class="o">*</span><span class="n">func</span><span class="p">)(</span><span class="kt">void</span><span class="p">));</span></code></pre></figure>

<p>成功時返回零，失敗時返回非零。</p>

<p>在一個程序中至少可以用 atexit() 註冊 32 個處理函數，依賴於編譯器。這些處理函數的調用順序與其註冊的順序相反，也即最先註冊的最後調用，最後註冊的最先調用。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="kt">void</span> <span class="nf">fnExit1</span> <span class="p">(</span><span class="kt">void</span><span class="p">)</span> <span class="p">{</span> <span class="n">puts</span> <span class="p">(</span><span class="s">"Exit function 1."</span><span class="p">);</span> <span class="p">}</span>
<span class="kt">void</span> <span class="nf">fnExit2</span> <span class="p">(</span><span class="kt">void</span><span class="p">)</span> <span class="p">{</span> <span class="n">puts</span> <span class="p">(</span><span class="s">"Exit function 2."</span><span class="p">);</span> <span class="p">}</span>

<span class="kt">int</span> <span class="nf">main</span> <span class="p">()</span>
<span class="p">{</span>
    <span class="n">atexit</span> <span class="p">(</span><span class="n">fnExit1</span><span class="p">);</span>
    <span class="n">atexit</span> <span class="p">(</span><span class="n">fnExit2</span><span class="p">);</span>
    <span class="n">puts</span> <span class="p">(</span><span class="s">"Main function."</span><span class="p">);</span>
    <span class="k">return</span> <span class="mi">0</span><span class="p">;</span>
<span class="p">}</span></code></pre></figure>

<h2 id="do-while">do-while</h2>

<p>如果通過 <code class="highlighter-rouge">define</code> 定義一個含有多個語句的宏，通常我們使用 <code class="highlighter-rouge">do{...} while(0);</code> 進行定義，具體原因，如下詳細介紹。</p>

<p>如果想在宏中包含多個語句，可能會如下這樣寫。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#define do_something() \
   do_a(); \
   do_b();</span></code></pre></figure>

<p>通常，這樣就可以用 <code class="highlighter-rouge">do_somethin()</code> 來執行一系列操作，但這樣會有個問題：如果通過如下的方式用這個宏，將會出錯。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="k">if</span> <span class="p">(...)</span>
   <span class="n">do_something</span><span class="p">();</span>

<span class="c1">// 宏被展開後
</span><span class="k">if</span> <span class="p">(...)</span>
   <span class="n">do_a</span><span class="p">();</span>
   <span class="n">do_b</span><span class="p">();</span></code></pre></figure>

<p>原代碼的目的是想在 if 為真的時候執行 <code class="highlighter-rouge">do_a()</code> 和 <code class="highlighter-rouge">do_b()</code>, 但現在，實際上只有 <code class="highlighter-rouge">do_a()</code> 在條件語句中，而 <code class="highlighter-rouge">do_b()</code> 任何時候都會執行。</p>

<p>當然這時可以通過如下的方式將那個宏改進一下。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#define do_something() { \
   do_a(); \
   do_b(); \
}</span></code></pre></figure>

<p>然而，即使是這樣，仍然有錯。假設有一個宏是這個樣子的，</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#define do_something() { \
   if (a)     \
      do_a(); \
   else       \
      do_b(); </span><span class="err">\</span></code></pre></figure>

<p>在使用如下情況時，仍會出錯。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="k">if</span> <span class="p">(...)</span>
   <span class="n">do_something</span><span class="p">();</span>
<span class="k">else</span> <span class="p">{</span>
   <span class="p">...</span>
<span class="p">}</span>

<span class="c1">// 宏展開後
</span><span class="k">if</span> <span class="p">(...)</span>
<span class="p">{</span>
   <span class="k">if</span> <span class="p">(</span><span class="n">a</span><span class="p">)</span>
      <span class="n">do_a</span><span class="p">();</span>
   <span class="k">else</span>
      <span class="n">do_b</span><span class="p">();</span>
<span class="p">};</span> <span class="k">else</span> <span class="p">{</span>
   <span class="p">...</span>
<span class="p">}</span></code></pre></figure>

<p>此時第二個 else 前邊會有一個分號，那麼編譯時就會出錯。</p>

<p>因此對於含有多條語句的宏我們使用 <code class="highlighter-rouge">do{...} while(0);</code> ，do-while 語句是需要分號來結束的，另外，現代編譯器的優化模塊能夠足夠聰明地注意到這個循環只會執行一次而將其優化掉。</p>

<p>綜上所述，<code class="highlighter-rouge">do{...} while(0);</code> 這個技術就是為了類似的宏可以在任何時候使用。</p>

<h2 id="assert">assert()</h2>

<p>其作用是如果它的條件返回錯誤，則輸出錯誤信息 (包括文件名，函數名等信息)，並終止程序執行，原型定義：</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#include &lt;assert.h&gt;
</span><span class="kt">void</span> <span class="n">assert</span><span class="p">(</span><span class="kt">int</span> <span class="n">expression</span><span class="p">);</span></code></pre></figure>

<p>如下是一個簡單的示例。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#include &lt;stdio.h&gt;
#include &lt;assert.h&gt;
#include &lt;stdlib.h&gt;
</span>
<span class="kt">int</span> <span class="nf">main</span><span class="p">(</span><span class="kt">int</span> <span class="n">argc</span><span class="p">,</span> <span class="kt">char</span> <span class="o">**</span><span class="n">argv</span><span class="p">)</span>
<span class="p">{</span>
    <span class="kt">FILE</span> <span class="o">*</span><span class="n">fp</span><span class="p">;</span>

   <span class="n">fp</span> <span class="o">=</span> <span class="n">fopen</span><span class="p">(</span> <span class="s">"test.txt"</span><span class="p">,</span> <span class="s">"w"</span> <span class="p">);</span> <span class="c1">// 不存在就創建一個同名文件
</span>   <span class="n">assert</span><span class="p">(</span> <span class="n">fp</span> <span class="p">);</span>                  <span class="c1">// 所以這裡不會出錯
</span>   <span class="n">fclose</span><span class="p">(</span> <span class="n">fp</span> <span class="p">);</span>

    <span class="n">fp</span> <span class="o">=</span> <span class="n">fopen</span><span class="p">(</span> <span class="s">"noexitfile.txt"</span><span class="p">,</span> <span class="s">"r"</span> <span class="p">);</span>  <span class="c1">// 不存在就打開文件失敗
</span>    <span class="n">assert</span><span class="p">(</span> <span class="n">fp</span> <span class="p">);</span>                         <span class="c1">// 這裡出錯
</span>    <span class="n">fclose</span><span class="p">(</span> <span class="n">fp</span> <span class="p">);</span>                         <span class="c1">// 程序不會執行到此處
</span>
    <span class="k">return</span> <span class="mi">0</span><span class="p">;</span>
<span class="p">}</span></code></pre></figure>

<p>當在 <code class="highlighter-rouge">&lt;assert.h&gt;</code> 之前定義 NDEBUG 時，assert 不會產生任何代碼，否則會顯示錯誤。</p>

<h3 id="判斷程序是否有-assert">判斷程序是否有 assert</h3>

<p>在 glibc 中，會定義如下的內容：</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#define assert(e) ((e \
    ? ((void)0) \
    :__assert_fail(#e,__FILE__,__LINE__))</span></code></pre></figure>

<p>可以通過 nm 查看程序，判斷是否存在 <code class="highlighter-rouge">__assert_fail@@GLIBC_2.2.5</code> ，如果存在該函數則說明未關閉 <code class="highlighter-rouge">assert()</code> 。</p>

<p>對於 autotool 可以通過如下的一種方式關閉：</p>

<ol>
  <li>在 <code class="highlighter-rouge">configure.ac</code> 文件中添加 <code class="highlighter-rouge">AC_HEADER_ASSERT</code> ，然後如果關閉是添加 <code class="highlighter-rouge">--disable-assert</code> 參數，注意，一定要保證源碼包含了 <code class="highlighter-rouge">config.h</code> 頭文件；</li>
  <li>執行 configure 命令前設置環境變量，如 <code class="highlighter-rouge">CPPFLAGS="CPPFLAGS=-DNDEBUG" ./configure</code>；</li>
  <li>也可以在 <code class="highlighter-rouge">Makefile.am</code> 中設置 <code class="highlighter-rouge">AM_CPPFLAGS += -DNDEBUG</code> 參數。</li>
</ol>

<h2 id="backtrace">backtrace</h2>

<p>一般可以通過 gdb 的 bt 命令查看函數運行時堆棧，但是，有時為了分析程序的 BUG，可以在程序出錯時打印出函數的調用堆棧。</p>

<p>在 glibc 頭文件 <code class="highlighter-rouge">execinfo.h</code> 中聲明瞭三個函數用於獲取當前線程的函數調用堆棧。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">int backtrace(void **buffer,int size);
    用於獲取當前線程的調用堆棧，獲取的信息將會被存放在buffer中，它是一個指針列表。參數size用來指
    定buffer中可以保存多少個void*元素，該函數返回值是實際獲取的指針個數，最大不超過size大小；

char **backtrace_symbols(void *const *buffer, int size);
    將從上述函數獲取的信息轉化為字符串數組，參數buffer應該是從backtrace()獲取的指針數組，size是該
    數組中的元素個數，也就是backtrace()的返回值。
    函數返回值是一個指向字符串數組的指針，它的大小同buffer相同，每個字符串包含了一個相對於buffer中
    對應元素的可打印信息，包括函數名、函數的偏移地址和實際的返回地址。

void backtrace_symbols_fd(void *const *buffer, int size, int fd);
    與上述函數相同，只是將結果寫入文件描述符為fd的文件中，每個函數對應一行。</code></pre></figure>

<p>注意，需要傳遞相應的符號給鏈接器以能支持函數名功能，比如，在使用 GNU ld 鏈接器的時需要傳遞 <code class="highlighter-rouge">-rdynamic</code> 參數，該參數用來通知鏈接器將所有符號添加到動態符號表中。</p>

<p>下面是 glibc 中的實例。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#include &lt;execinfo.h&gt;
#include &lt;stdio.h&gt;
#include &lt;stdlib.h&gt;
#include &lt;unistd.h&gt;
</span>
<span class="cp">#define SIZE 100
</span>
<span class="kt">void</span> <span class="nf">myfunc3</span><span class="p">(</span><span class="kt">void</span><span class="p">)</span>
<span class="p">{</span>
    <span class="kt">int</span> <span class="n">j</span><span class="p">,</span> <span class="n">nptrs</span><span class="p">;</span>
    <span class="kt">void</span> <span class="o">*</span><span class="n">buffer</span><span class="p">[</span><span class="mi">100</span><span class="p">];</span>
    <span class="kt">char</span> <span class="o">**</span><span class="n">strings</span><span class="p">;</span>

    <span class="n">nptrs</span> <span class="o">=</span> <span class="n">backtrace</span><span class="p">(</span><span class="n">buffer</span><span class="p">,</span> <span class="n">SIZE</span><span class="p">);</span>
    <span class="n">printf</span><span class="p">(</span><span class="s">"backtrace() returned %d addresses</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span> <span class="n">nptrs</span><span class="p">);</span>

    <span class="cm">/*
     * The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
     * would produce similar output to the following:
     */</span>
    <span class="n">strings</span> <span class="o">=</span> <span class="n">backtrace_symbols</span><span class="p">(</span><span class="n">buffer</span><span class="p">,</span> <span class="n">nptrs</span><span class="p">);</span>
    <span class="k">if</span> <span class="p">(</span><span class="n">strings</span> <span class="o">==</span> <span class="nb">NULL</span><span class="p">)</span> <span class="p">{</span>
        <span class="n">perror</span><span class="p">(</span><span class="s">"backtrace_symbols"</span><span class="p">);</span>
        <span class="n">exit</span><span class="p">(</span><span class="n">EXIT_FAILURE</span><span class="p">);</span>
    <span class="p">}</span>

    <span class="k">for</span> <span class="p">(</span><span class="n">j</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span> <span class="n">j</span> <span class="o">&lt;</span> <span class="n">nptrs</span><span class="p">;</span> <span class="n">j</span><span class="o">++</span><span class="p">)</span>
        <span class="n">printf</span><span class="p">(</span><span class="s">"%s</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span> <span class="n">strings</span><span class="p">[</span><span class="n">j</span><span class="p">]);</span>

    <span class="n">free</span><span class="p">(</span><span class="n">strings</span><span class="p">);</span>
<span class="p">}</span>

<span class="k">static</span> <span class="kt">void</span> <span class="nf">myfunc2</span><span class="p">(</span><span class="kt">void</span><span class="p">)</span> <span class="cm">/* "static" means don't export the symbol... */</span>
<span class="p">{</span>
    <span class="n">myfunc3</span><span class="p">();</span>
<span class="p">}</span>

<span class="kt">void</span> <span class="nf">myfunc</span><span class="p">(</span><span class="kt">int</span> <span class="n">ncalls</span><span class="p">)</span>
<span class="p">{</span>
    <span class="k">if</span> <span class="p">(</span><span class="n">ncalls</span> <span class="o">&gt;</span> <span class="mi">1</span><span class="p">)</span>
        <span class="n">myfunc</span><span class="p">(</span><span class="n">ncalls</span> <span class="o">-</span> <span class="mi">1</span><span class="p">);</span>
    <span class="k">else</span>
        <span class="n">myfunc2</span><span class="p">();</span>
<span class="p">}</span>

<span class="kt">int</span> <span class="nf">main</span><span class="p">(</span><span class="kt">int</span> <span class="n">argc</span><span class="p">,</span> <span class="kt">char</span> <span class="o">*</span><span class="n">argv</span><span class="p">[])</span>
<span class="p">{</span>
    <span class="k">if</span> <span class="p">(</span><span class="n">argc</span> <span class="o">!=</span> <span class="mi">2</span><span class="p">)</span> <span class="p">{</span>
        <span class="n">fprintf</span><span class="p">(</span><span class="n">stderr</span><span class="p">,</span> <span class="s">"%s num-calls</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span> <span class="n">argv</span><span class="p">[</span><span class="mi">0</span><span class="p">]);</span>
        <span class="n">exit</span><span class="p">(</span><span class="n">EXIT_FAILURE</span><span class="p">);</span>
    <span class="p">}</span>

    <span class="n">myfunc</span><span class="p">(</span><span class="n">atoi</span><span class="p">(</span><span class="n">argv</span><span class="p">[</span><span class="mi">1</span><span class="p">]));</span>
    <span class="n">exit</span><span class="p">(</span><span class="n">EXIT_SUCCESS</span><span class="p">);</span>
<span class="p">}</span></code></pre></figure>

<p>然後通過如下方式編譯，執行。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">$ cc -rdynamic prog.c -o prog
$ ./prog 2
backtrace() returned 7 addresses
./prog(myfunc3+0x1f) [0x400a7c]
./prog() [0x400b11]
./prog(myfunc+0x25) [0x400b38]
./prog(myfunc+0x1e) [0x400b31]
./prog(main+0x59) [0x400b93]
/lib64/libc.so.6(__libc_start_main+0xf5) [0x7f727d449b35]
./prog() [0x400999]</code></pre></figure>

<p>還可以利用 backtrace 來定位段錯誤位置。</p>

<!--
通常情況系，程序發生段錯誤時系統會發送SIGSEGV信號給程序，缺省處理是退出函數。我們可以使用 signal(SIGSEGV, &your_function);函數來接管SIGSEGV信號的處理，程序在發生段錯誤後，自動調用我們準備好的函數，從而在那個函數裡來獲取當前函數調用棧。

舉例如下：

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <execinfo.h>
#include <signal.h>

void dump(int signo)
{
 void *buffer[30] = {0};
 size_t size;
 char **strings = NULL;
 size_t i = 0;

 size = backtrace(buffer, 30);
 fprintf(stdout, "Obtained %zd stack frames.nm\n", size);
 strings = backtrace_symbols(buffer, size);
 if (strings == NULL)
 {
  perror("backtrace_symbols.");
  exit(EXIT_FAILURE);
 }

 for (i = 0; i < size; i++)
 {
  fprintf(stdout, "%s\n", strings[i]);
 }
 free(strings);
 strings = NULL;
 exit(0);
}

void func_c()
{
 *((volatile char *)0x0) = 0x9999;
}

void func_b()
{
 func_c();
}

void func_a()
{
 func_b();
}

int main(int argc, const char *argv[])
{
 if (signal(SIGSEGV, dump) == SIG_ERR)
  perror("can't catch SIGSEGV");
 func_a();
 return 0;
}

編譯程序：
gcc -g -rdynamic test.c -o test; ./test
輸出如下：

Obtained6stackframes.nm
./backstrace_debug(dump+0x45)[0x80487c9]
[0x468400]
./backstrace_debug(func_b+0x8)[0x804888c]
./backstrace_debug(func_a+0x8)[0x8048896]
./backstrace_debug(main+0x33)[0x80488cb]
/lib/i386-linux-gnu/libc.so.6(__libc_start_main+0xf3)[0x129113]

接著：
objdump -d test > test.s
在test.s中搜索804888c如下：

8048884 <func_b>:
8048884: 55          push %ebp
8048885: 89 e5      mov %esp, %ebp
8048887: e8 eb ff ff ff      call 8048877 <func_c>
804888c: 5d            pop %ebp
804888d: c3            ret

其中80488c時調用（call 8048877）C函數後的地址，雖然並沒有直接定位到C函數，通過彙編代碼， 基本可以推出是C函數出問題了（pop指令不會導致段錯誤的）。
我們也可以通過addr2line來查看

addr2line 0x804888c -e backstrace_debug -f

輸出：

func_b
/home/astrol/c/backstrace_debug.c:57
-->

<!--
## 宏定義
http://hbprotoss.github.io/posts/cyu-yan-hong-de-te-shu-yong-fa-he-ji-ge-keng.html
http://gcc.gnu.org/onlinedocs/cpp/Macros.html
-->

<h2 id="其它">其它</h2>

<h3 id="is-not-a-symbolic-link">is not a symbolic link</h3>

<p>正常情況下，類似庫 <code class="highlighter-rouge">libxerces-c-3.0.so</code> 應該是個符號鏈接，而不是實體文件，對於這種情況只需要修改其為符號鏈接即可。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text"># mv libxerces-c-3.0.so libxerces-c.so.3.0
# ln -s libxerces-c.so.3.0 libxerces-c-3.0.so</code></pre></figure>

<h3 id="結構體初始化">結構體初始化</h3>

<p>對於 C 中結構體初始化可以通過如下設置。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#include &lt;stdio.h&gt;
</span>
<span class="k">struct</span> <span class="n">foobar</span> <span class="p">{</span>
        <span class="kt">int</span> <span class="n">foo</span><span class="p">;</span>
        <span class="k">struct</span> <span class="n">a</span> <span class="p">{</span>
                <span class="kt">int</span> <span class="n">type</span><span class="p">;</span>
                <span class="kt">int</span> <span class="n">value</span><span class="p">;</span>
        <span class="p">}</span> <span class="o">*</span><span class="n">array</span><span class="p">;</span>
        <span class="kt">int</span> <span class="n">length</span><span class="p">;</span>
<span class="p">};</span>

<span class="kt">int</span> <span class="nf">main</span><span class="p">(</span><span class="kt">int</span> <span class="n">argc</span><span class="p">,</span> <span class="kt">char</span> <span class="o">**</span><span class="n">argv</span><span class="p">)</span>
<span class="p">{</span>
        <span class="kt">int</span> <span class="n">i</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span>

        <span class="k">struct</span> <span class="n">foobar</span> <span class="n">f</span> <span class="o">=</span> <span class="p">{</span>
                <span class="p">.</span><span class="n">foo</span> <span class="o">=</span> <span class="mi">1</span><span class="p">,</span>
                <span class="p">.</span><span class="n">length</span> <span class="o">=</span> <span class="mi">3</span><span class="p">,</span>
                <span class="p">.</span><span class="n">array</span> <span class="o">=</span> <span class="p">(</span><span class="k">struct</span> <span class="n">a</span><span class="p">[]){</span>
                        <span class="p">{.</span><span class="n">type</span> <span class="o">=</span> <span class="mi">1</span><span class="p">,</span> <span class="p">.</span><span class="n">value</span> <span class="o">=</span> <span class="mi">2</span><span class="p">},</span>
                        <span class="p">{.</span><span class="n">type</span> <span class="o">=</span> <span class="mi">1</span><span class="p">,</span> <span class="p">.</span><span class="n">value</span> <span class="o">=</span> <span class="mi">3</span><span class="p">},</span>
                        <span class="p">{.</span><span class="n">type</span> <span class="o">=</span> <span class="mi">1</span><span class="p">,</span> <span class="p">.</span><span class="n">value</span> <span class="o">=</span> <span class="mi">3</span><span class="p">}</span>
                <span class="p">}</span>
        <span class="p">};</span>

        <span class="k">for</span> <span class="p">(</span><span class="n">i</span> <span class="o">=</span> <span class="mi">0</span><span class="p">;</span> <span class="n">i</span> <span class="o">&lt;</span> <span class="n">f</span><span class="p">.</span><span class="n">length</span><span class="p">;</span> <span class="n">i</span><span class="o">++</span><span class="p">)</span>
                <span class="n">printf</span><span class="p">(</span><span class="s">"&gt;&gt;&gt;&gt; %d %d</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span> <span class="n">i</span><span class="p">,</span> <span class="n">f</span><span class="p">.</span><span class="n">array</span><span class="p">[</span><span class="n">i</span><span class="p">].</span><span class="n">type</span><span class="p">);</span>

        <span class="k">return</span> <span class="mi">0</span><span class="p">;</span>
<span class="p">}</span></code></pre></figure>

<h3 id="結構體地址定位">結構體地址定位</h3>

<p>通過結構體可以將多種不同類型的對象聚合到一個對象中，編譯器會按照成員列表順序分配內存，不過由於內存對齊機制不同，導致不同架構有所區別，所以各個成員之間可能會有間隙，所以不能簡單的通過成員類型所佔的字長來推斷其它成員或結構體對象的地址。</p>

<p>假設有如下的一個鏈表。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">typedef struct list_node {
	int ivar;
	char cvar;
	double dvar;
	struct list_node *next;
} list_node;</code></pre></figure>

<p>當已知一個變量的地址時，如何獲取到某個成員的偏移量，Linux 內核中的實現如下。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#define offsetof(TYPE, MEMBER) ((size_t) &amp;((TYPE *)0)-&gt;MEMBER)</span></code></pre></figure>

<p>當知道了成員偏移量，那麼就可以通過結構體成員的地址，反向求結構體的地址，如下。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#define container_of(ptr, type, member) ({
</span>	<span class="k">const</span> <span class="n">typeof</span><span class="p">(((</span><span class="n">type</span> <span class="o">*</span><span class="p">)</span><span class="mi">0</span><span class="p">)</span><span class="o">-&gt;</span><span class="n">member</span> <span class="p">)</span> <span class="o">*</span><span class="n">__mptr</span> <span class="o">=</span> <span class="p">(</span><span class="n">ptr</span><span class="p">);</span>
	<span class="p">(</span><span class="n">type</span> <span class="o">*</span><span class="p">)((</span><span class="kt">char</span> <span class="o">*</span><span class="p">)</span><span class="n">__mptr</span> <span class="o">-</span> <span class="n">offsetof</span><span class="p">(</span><span class="n">type</span><span class="p">,</span><span class="n">member</span><span class="p">));</span>
	<span class="p">})</span></code></pre></figure>

<p>現在很多的動態語言是可以支持動態獲取變量類型的，其中 GCC 提供了 <code class="highlighter-rouge">typeof</code> 關鍵字，所不同的是這個只在預編譯時，最後實際轉化為數據類型被編譯器處理。基本用法是這樣的：</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="kt">int</span> <span class="n">a</span><span class="p">;</span>
<span class="n">typeof</span><span class="p">(</span><span class="n">a</span><span class="p">)</span>  <span class="n">b</span><span class="p">;</span> <span class="c1">// int b;
</span><span class="n">typeof</span><span class="p">(</span><span class="o">&amp;</span><span class="n">a</span><span class="p">)</span> <span class="n">c</span><span class="p">;</span> <span class="o">//</span> <span class="kt">int</span><span class="o">*</span> <span class="n">c</span><span class="p">;</span></code></pre></figure>

<p>如上的宏定義中， ptr 代表已知成員的地址，type 代表結構體的類型，member 代表已知的成員。</p>

<h3 id="指針參數修改">指針參數修改</h3>

<p>一個比較容易犯錯的地方，願意是在 <code class="highlighter-rouge">foobar()</code> 函數內修改 <code class="highlighter-rouge">main()</code> 中的 v 指向的變量，其中後者實際上是修改的本地棧中保存的臨時版本。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#include &lt;stdio.h&gt;
#include &lt;string.h&gt;
#include &lt;stdlib.h&gt;
</span>
<span class="k">typedef</span> <span class="k">struct</span> <span class="n">value_s</span> <span class="p">{</span>
  <span class="k">union</span> <span class="p">{</span>
    <span class="kt">char</span> <span class="o">*</span><span class="n">string</span><span class="p">;</span>
  <span class="p">}</span> <span class="n">value</span><span class="p">;</span>
<span class="p">}</span> <span class="n">value_t</span><span class="p">;</span>

<span class="cp">#if 1
</span><span class="kt">void</span> <span class="nf">foobar</span> <span class="p">(</span> <span class="n">value_t</span> <span class="o">*</span><span class="n">t</span> <span class="p">)</span>
<span class="p">{</span>
  <span class="kt">char</span> <span class="o">**</span><span class="n">v</span> <span class="o">=</span> <span class="o">&amp;</span><span class="n">t</span><span class="o">-&gt;</span><span class="n">value</span><span class="p">.</span><span class="n">string</span><span class="p">;</span>
  <span class="n">printf</span><span class="p">(</span><span class="s">"foobar %p %s</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span> <span class="o">*</span><span class="n">v</span><span class="p">,</span> <span class="o">*</span><span class="n">v</span><span class="p">);</span>
  <span class="o">*</span><span class="n">v</span> <span class="o">=</span> <span class="s">"yang"</span><span class="p">;</span>
  <span class="n">printf</span><span class="p">(</span><span class="s">"foobar %p %s</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span> <span class="o">*</span><span class="n">v</span><span class="p">,</span> <span class="o">*</span><span class="n">v</span><span class="p">);</span>
<span class="p">}</span>
<span class="cp">#else
</span><span class="kt">void</span> <span class="nf">foobar</span> <span class="p">(</span> <span class="n">value_t</span> <span class="o">*</span><span class="n">t</span> <span class="p">)</span>
<span class="p">{</span>
  <span class="n">value_t</span> <span class="n">v</span> <span class="o">=</span> <span class="o">*</span><span class="n">t</span><span class="p">;</span>
  <span class="n">printf</span><span class="p">(</span><span class="s">"foobar %p %s</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span> <span class="n">v</span><span class="p">.</span><span class="n">value</span><span class="p">.</span><span class="n">string</span><span class="p">,</span> <span class="n">v</span><span class="p">.</span><span class="n">value</span><span class="p">.</span><span class="n">string</span><span class="p">);</span>
  <span class="n">v</span><span class="p">.</span><span class="n">value</span><span class="p">.</span><span class="n">string</span> <span class="o">=</span> <span class="s">"yang"</span><span class="p">;</span>
  <span class="n">printf</span><span class="p">(</span><span class="s">"foobar %p %s</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span> <span class="n">v</span><span class="p">.</span><span class="n">value</span><span class="p">.</span><span class="n">string</span><span class="p">,</span> <span class="n">v</span><span class="p">.</span><span class="n">value</span><span class="p">.</span><span class="n">string</span><span class="p">);</span>
<span class="p">}</span>
<span class="cp">#endif
</span>
<span class="kt">int</span> <span class="nf">main</span><span class="p">()</span>
<span class="p">{</span>
  <span class="n">value_t</span> <span class="n">v</span><span class="p">;</span>
  <span class="n">v</span><span class="p">.</span><span class="n">value</span><span class="p">.</span><span class="n">string</span> <span class="o">=</span> <span class="s">"jin"</span><span class="p">;</span>
  <span class="n">printf</span><span class="p">(</span><span class="s">"       %p %s</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span> <span class="n">v</span><span class="p">.</span><span class="n">value</span><span class="p">.</span><span class="n">string</span><span class="p">,</span> <span class="n">v</span><span class="p">.</span><span class="n">value</span><span class="p">.</span><span class="n">string</span><span class="p">);</span>
  <span class="n">foobar</span><span class="p">(</span> <span class="o">&amp;</span><span class="n">v</span> <span class="p">);</span>
  <span class="n">printf</span><span class="p">(</span><span class="s">"       %p %s</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span> <span class="n">v</span><span class="p">.</span><span class="n">value</span><span class="p">.</span><span class="n">string</span><span class="p">,</span> <span class="n">v</span><span class="p">.</span><span class="n">value</span><span class="p">.</span><span class="n">string</span><span class="p">);</span>

  <span class="k">return</span><span class="p">(</span><span class="mi">0</span><span class="p">);</span>
<span class="p">}</span></code></pre></figure>

<h3 id="取固定大小的變量">取固定大小的變量</h3>

<p>Linux 每個數據類型的大小可以在 <code class="highlighter-rouge">sys/types.h</code> 中查看</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#include &lt;sys/types.h&gt;
</span><span class="k">typedef</span>    <span class="kt">int8_t</span>            <span class="n">S8</span><span class="p">;</span>
<span class="k">typedef</span>    <span class="kt">int16_t</span>           <span class="n">S16</span><span class="p">;</span>
<span class="k">typedef</span>    <span class="kt">int32_t</span>           <span class="n">S32</span><span class="p">;</span>
<span class="k">typedef</span>    <span class="kt">int64_t</span>           <span class="n">S64</span><span class="p">;</span>

<span class="k">typedef</span>    <span class="n">u_int8_t</span>          <span class="n">U8</span><span class="p">;</span>
<span class="k">typedef</span>    <span class="n">u_int16_t</span>         <span class="n">U16</span><span class="p">;</span>
<span class="k">typedef</span>    <span class="n">u_int32_t</span>         <span class="n">U32</span><span class="p">;</span>
<span class="k">typedef</span>    <span class="n">u_int64_t</span>         <span class="n">U64</span><span class="p">;</span></code></pre></figure>

<h3 id="環境變量">環境變量</h3>

<p>簡單介紹下 C 中，如何獲取以及設置環境變量。</p>

<p>其中設置環境變量方法包括了 <code class="highlighter-rouge">putenv()</code> 以及 <code class="highlighter-rouge">setenv()</code> 兩種，前者必須是 <code class="highlighter-rouge">Key=Value</code> 這種格式，後者則以參數形式傳遞。</p>

<p>對於 <code class="highlighter-rouge">putenv()</code> 如果環境變量已經存在則替換，而 <code class="highlighter-rouge">setenv()</code> 則可以設置是否覆蓋 。</p>

<figure class="highlight"><pre><code class="language-c" data-lang="c"><span class="cp">#include &lt;stdio.h&gt;
#include &lt;stdlib.h&gt;
</span>
<span class="kt">int</span> <span class="nf">main</span><span class="p">(</span><span class="kt">void</span><span class="p">)</span>
<span class="p">{</span>
	<span class="kt">char</span> <span class="o">*</span><span class="n">p</span><span class="p">;</span>

	<span class="k">if</span><span class="p">((</span><span class="n">p</span> <span class="o">=</span> <span class="n">getenv</span><span class="p">(</span><span class="s">"USER"</span><span class="p">)))</span>
		<span class="n">printf</span><span class="p">(</span><span class="s">"USER = %s</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span> <span class="n">p</span><span class="p">);</span>

	<span class="n">setenv</span><span class="p">(</span><span class="s">"USER"</span><span class="p">,</span> <span class="s">"test"</span><span class="p">,</span> <span class="mi">1</span><span class="p">);</span>
	<span class="n">printf</span><span class="p">(</span><span class="s">"USER = %s</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span> <span class="n">getenv</span><span class="p">(</span><span class="s">"USER"</span><span class="p">));</span>

	<span class="n">putenv</span><span class="p">(</span><span class="s">"NAME=foobar"</span><span class="p">);</span>
	<span class="n">printf</span><span class="p">(</span><span class="s">"NAME = %s</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span> <span class="n">getenv</span><span class="p">(</span><span class="s">"NAME"</span><span class="p">));</span>

	<span class="n">unsetenv</span><span class="p">(</span><span class="s">"USER"</span><span class="p">);</span>
	<span class="n">printf</span><span class="p">(</span><span class="s">"USER = %s</span><span class="se">\n</span><span class="s">"</span><span class="p">,</span> <span class="n">getenv</span><span class="p">(</span><span class="s">"USER"</span><span class="p">));</span>
<span class="p">}</span></code></pre></figure>

<h3 id="其它-1">其它</h3>

<h4 id="flt_radix">FLT_RADIX</h4>

<p>C 語言標準庫 <code class="highlighter-rouge">float.h</code> 中的 <code class="highlighter-rouge">FLT_RADIX</code> 常數用於定義指數的基數，也就是以這個數為底的多少次方。</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">FLT_RADIX 10     10 的多少次方
FLT_RADIX 2       2 的多少次方</code></pre></figure>

<p>例如：</p>

<figure class="highlight"><pre><code class="language-text" data-lang="text">#define FLT_MAX_EXP 128
#define FLT_RADIX   2</code></pre></figure>

<p>意思是 float 型，最大指數是 128，它的底是 2，也就說最大指數是 2 的 128 方。</p>

<!--
FLT 是 float 的 縮寫。( DBL 是 double 的 縮寫。)
-->

<h4 id="implicit-declaration">implicit declaration</h4>

<p>按常規來講，出現 <code class="highlighter-rouge">implicit declaration of function 'xxxx'</code> 是因為頭文件未包含導致的！</p>

<p>這裡是由於 <code class="highlighter-rouge">nanosleep()</code> 函數的報錯，而實際上 <code class="highlighter-rouge">time.h</code> 頭文件已經包含了，後來才發現原來是在 <code class="highlighter-rouge">Makefile</code> 中添加了 <code class="highlighter-rouge">-std=c99</code> 導致，可以通過 <code class="highlighter-rouge">-std=gnu99</code> 替換即可。</p>

<p>另外，不能定義 <code class="highlighter-rouge">-D_POSIX_SOURCE</code> 宏。</p>

<h2 id="參考">參考</h2>

<p><a href="https://github.com/ludocode/schemaless-benchmarks">Schemaless Benchmarking Suite</a></p>



    </div>