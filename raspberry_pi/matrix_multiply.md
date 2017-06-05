# Matrix multiply


<div class="main">
			<h1>ARM assembler in Raspberry Pi – Chapter 14 </h1>
		<p class="post-meta"><span class="date">May 12, 2013</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/#comments">16</a></span> </p>
		<p>
In chapter 13 we saw the basic elements of VFPv2, the floating point subarchitecture of ARMv6. In this chapter we will implement a floating point matrix multiply using VFPv2.
</p>
<p><span id="more-1003"></span></p>
<p style="background-color: #ffe1e1; padding: 1em;">
<b>Disclaimer</b>: I advise you against using the code in this chapter in commercial-grade projects unless you fully review it for both correctness and precision.
</p>
<h2>Matrix multiply</h2>
<p>
Given two vectors <strong>v</strong> and <strong>w</strong> of rank <em>r</em> where <strong>v</strong> = &lt;v<sub>0</sub>, v<sub>1</sub>, … v<sub>r-1</sub>&gt; and w = &lt;w<sub>0</sub>, w<sub>1</sub>, …, w<sub>r-1</sub>&gt;, we define the <em>dot product</em> of <strong>v</strong> by <strong>w</strong> as the scalar <strong>v</strong> · <strong>w</strong> = v<sub>0</sub>×w<sub>0</sub> + v<sub>1</sub>×w<sub>0</sub> + … + v<sub>r-1</sub>×w<sub>r-1</sub>.
</p>
<p>
We can multiply a matrix <code>A</code> of <code>n</code> rows and <code>m</code> columns (<code>n</code> x <code>m</code>) by a matrix <code>B</code> of <code>m</code> rows and <code>p</code> columns (<code>m</code> x <code>p</code>). The result is a matrix of <code>n</code> rows and p columns. Matrix multiplication may seem complicated but actually it is not. Every element in the result matrix it is just the dot product (defined in the paragraph above) of the corresponding row of the matrix <code>A</code> by the corresponding column of the matrix <code>B</code> (this is why there must be as many columns in <code>A</code> as there are rows in <code>B</code>).
</p>
<p><img data-attachment-id="1006" data-permalink="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/matmul/" data-orig-file="images/matmul.png" data-orig-size="579,504" data-comments-opened="1" data-image-meta="{&quot;aperture&quot;:&quot;0&quot;,&quot;credit&quot;:&quot;&quot;,&quot;camera&quot;:&quot;&quot;,&quot;caption&quot;:&quot;&quot;,&quot;created_timestamp&quot;:&quot;0&quot;,&quot;copyright&quot;:&quot;&quot;,&quot;focal_length&quot;:&quot;0&quot;,&quot;iso&quot;:&quot;0&quot;,&quot;shutter_speed&quot;:&quot;0&quot;,&quot;title&quot;:&quot;&quot;}" data-image-title="Matrix multiplication schema" data-image-description="" data-medium-file="images/matmul-300x261.png" data-large-file="images/matmul.png" src="images/matmul.png" alt="Matrix multiplication schema" width="579" height="504" class="aligncenter size-full wp-image-1006" srcset="images/matmul.png 579w, images/matmul-300x261.png 300w" sizes="(max-width: 579px) 100vw, 579px"></p>
<p>
A straightforward implementation of the matrix multiplication in C is as follows.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
</pre></td><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #993333;">float</span> A<span style="color: #009900;">[</span>N<span style="color: #009900;">]</span><span style="color: #009900;">[</span>M<span style="color: #009900;">]</span><span style="color: #339933;">;</span> <span style="color: #666666; font-style: italic;">// N rows of M columns each row</span>
<span style="color: #993333;">float</span> B<span style="color: #009900;">[</span>M<span style="color: #009900;">]</span><span style="color: #009900;">[</span>P<span style="color: #009900;">]</span><span style="color: #339933;">;</span> <span style="color: #666666; font-style: italic;">// M rows of P columns each row</span>
<span style="color: #666666; font-style: italic;">// Result</span>
<span style="color: #993333;">float</span> C<span style="color: #009900;">[</span>N<span style="color: #009900;">]</span><span style="color: #009900;">[</span>P<span style="color: #009900;">]</span><span style="color: #339933;">;</span>
&nbsp;
<span style="color: #b1b100;">for</span> <span style="color: #009900;">(</span><span style="color: #993333;">int</span> i <span style="color: #339933;">=</span> <span style="color: #0000dd;">0</span><span style="color: #339933;">;</span> i <span style="color: #339933;">&lt;</span> N<span style="color: #339933;">;</span> i<span style="color: #339933;">++</span><span style="color: #009900;">)</span> <span style="color: #666666; font-style: italic;">// for each row of the result</span>
<span style="color: #009900;">{</span>
  <span style="color: #b1b100;">for</span> <span style="color: #009900;">(</span><span style="color: #993333;">int</span> j <span style="color: #339933;">=</span> <span style="color: #0000dd;">0</span><span style="color: #339933;">;</span> j <span style="color: #339933;">&lt;</span> P<span style="color: #339933;">;</span> j<span style="color: #339933;">++</span><span style="color: #009900;">)</span> <span style="color: #666666; font-style: italic;">// and for each column</span>
  <span style="color: #009900;">{</span>
    C<span style="color: #009900;">[</span>i<span style="color: #009900;">]</span><span style="color: #009900;">[</span>j<span style="color: #009900;">]</span> <span style="color: #339933;">=</span> <span style="color: #0000dd;">0</span><span style="color: #339933;">;</span> <span style="color: #666666; font-style: italic;">// Initialize to zero</span>
    <span style="color: #666666; font-style: italic;">// Now make the dot matrix of the row by the column</span>
    <span style="color: #b1b100;">for</span> <span style="color: #009900;">(</span><span style="color: #993333;">int</span> k <span style="color: #339933;">=</span> <span style="color: #0000dd;">0</span><span style="color: #339933;">;</span> k <span style="color: #339933;">&lt;</span> M<span style="color: #339933;">;</span> k<span style="color: #339933;">++</span><span style="color: #009900;">)</span>
       C<span style="color: #009900;">[</span>i<span style="color: #009900;">]</span><span style="color: #009900;">[</span>j<span style="color: #009900;">]</span> <span style="color: #339933;">+=</span> A<span style="color: #009900;">[</span>i<span style="color: #009900;">]</span><span style="color: #009900;">[</span>k<span style="color: #009900;">]</span> <span style="color: #339933;">*</span> B<span style="color: #009900;">[</span>k<span style="color: #009900;">]</span><span style="color: #009900;">[</span>j<span style="color: #009900;">]</span><span style="color: #339933;">;</span>
  <span style="color: #009900;">}</span>
<span style="color: #009900;">}</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">float A[N][M]; // N rows of M columns each row
float B[M][P]; // M rows of P columns each row
// Result
float C[N][P];

for (int i = 0; i &lt; N; i++) // for each row of the result
{
  for (int j = 0; j &lt; P; j++) // and for each column
  {
    C[i][j] = 0; // Initialize to zero
    // Now make the dot matrix of the row by the column
    for (int k = 0; k &lt; M; k++)
       C[i][j] += A[i][k] * B[k][j];
  }
}</p></div>

<p>
In order to simplify the example, we will asume that both matrices A and B are square matrices of size <code>n x n</code>. This simplifies just a bit the algorithm.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>1
2
3
4
5
6
7
8
9
10
11
12
13
14
</pre></td><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #993333;">float</span> A<span style="color: #009900;">[</span>N<span style="color: #009900;">]</span><span style="color: #009900;">[</span>N<span style="color: #009900;">]</span><span style="color: #339933;">;</span>
<span style="color: #993333;">float</span> B<span style="color: #009900;">[</span>N<span style="color: #009900;">]</span><span style="color: #009900;">[</span>N<span style="color: #009900;">]</span><span style="color: #339933;">;</span>
<span style="color: #666666; font-style: italic;">// Result</span>
<span style="color: #993333;">float</span> C<span style="color: #009900;">[</span>N<span style="color: #009900;">]</span><span style="color: #009900;">[</span>N<span style="color: #009900;">]</span><span style="color: #339933;">;</span>
&nbsp;
<span style="color: #b1b100;">for</span> <span style="color: #009900;">(</span><span style="color: #993333;">int</span> i <span style="color: #339933;">=</span> <span style="color: #0000dd;">0</span><span style="color: #339933;">;</span> i <span style="color: #339933;">&lt;</span> N<span style="color: #339933;">;</span> i<span style="color: #339933;">++</span><span style="color: #009900;">)</span>
<span style="color: #009900;">{</span>
  <span style="color: #b1b100;">for</span> <span style="color: #009900;">(</span><span style="color: #993333;">int</span> j <span style="color: #339933;">=</span> <span style="color: #0000dd;">0</span><span style="color: #339933;">;</span> j <span style="color: #339933;">&lt;</span> N<span style="color: #339933;">;</span> j<span style="color: #339933;">++</span><span style="color: #009900;">)</span>
  <span style="color: #009900;">{</span>
    C<span style="color: #009900;">[</span>i<span style="color: #009900;">]</span><span style="color: #009900;">[</span>j<span style="color: #009900;">]</span> <span style="color: #339933;">=</span> <span style="color: #0000dd;">0</span><span style="color: #339933;">;</span>
    <span style="color: #b1b100;">for</span> <span style="color: #009900;">(</span><span style="color: #993333;">int</span> k <span style="color: #339933;">=</span> <span style="color: #0000dd;">0</span><span style="color: #339933;">;</span> k <span style="color: #339933;">&lt;</span> N<span style="color: #339933;">;</span> k<span style="color: #339933;">++</span><span style="color: #009900;">)</span>
       C<span style="color: #009900;">[</span>i<span style="color: #009900;">]</span><span style="color: #009900;">[</span>j<span style="color: #009900;">]</span> <span style="color: #339933;">+=</span> A<span style="color: #009900;">[</span>i<span style="color: #009900;">]</span><span style="color: #009900;">[</span>k<span style="color: #009900;">]</span> <span style="color: #339933;">*</span> B<span style="color: #009900;">[</span>k<span style="color: #009900;">]</span><span style="color: #009900;">[</span>j<span style="color: #009900;">]</span><span style="color: #339933;">;</span>
  <span style="color: #009900;">}</span>
<span style="color: #009900;">}</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">float A[N][N];
float B[N][N];
// Result
float C[N][N];

for (int i = 0; i &lt; N; i++)
{
  for (int j = 0; j &lt; N; j++)
  {
    C[i][j] = 0;
    for (int k = 0; k &lt; N; k++)
       C[i][j] += A[i][k] * B[k][j];
  }
}</p></div>

<p>
Matrix multiplication is an important operation used in many areas. For instance, in computer graphics is usually performed on 3×3 and 4×4 matrices representing 3D geometry. So we will try to make a reasonably fast version of it (we do not aim at making the best one, though).
</p>
<p>
A first improvement we want to do in this algorithm is making the loops perfectly nested. There are some technical reasons beyond the scope of this code for that. So we will get rid of the initialization of <code>C[i][j]</code> to 0, outside of the loop.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>1
2
3
4
5
6
7
8
9
10
11
12
13
</pre></td><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #993333;">float</span> A<span style="color: #009900;">[</span>N<span style="color: #009900;">]</span><span style="color: #009900;">[</span>N<span style="color: #009900;">]</span><span style="color: #339933;">;</span>
<span style="color: #993333;">float</span> B<span style="color: #009900;">[</span>M<span style="color: #009900;">]</span><span style="color: #009900;">[</span>N<span style="color: #009900;">]</span><span style="color: #339933;">;</span>
<span style="color: #666666; font-style: italic;">// Result</span>
<span style="color: #993333;">float</span> C<span style="color: #009900;">[</span>N<span style="color: #009900;">]</span><span style="color: #009900;">[</span>N<span style="color: #009900;">]</span><span style="color: #339933;">;</span>
&nbsp;
<span style="color: #b1b100;">for</span> <span style="color: #009900;">(</span><span style="color: #993333;">int</span> i <span style="color: #339933;">=</span> <span style="color: #0000dd;">0</span><span style="color: #339933;">;</span> i <span style="color: #339933;">&lt;</span> N<span style="color: #339933;">;</span> i<span style="color: #339933;">++</span><span style="color: #009900;">)</span>
  <span style="color: #b1b100;">for</span> <span style="color: #009900;">(</span><span style="color: #993333;">int</span> j <span style="color: #339933;">=</span> <span style="color: #0000dd;">0</span><span style="color: #339933;">;</span> j <span style="color: #339933;">&lt;</span> N<span style="color: #339933;">;</span> j<span style="color: #339933;">++</span><span style="color: #009900;">)</span>
    C<span style="color: #009900;">[</span>i<span style="color: #009900;">]</span><span style="color: #009900;">[</span>j<span style="color: #009900;">]</span> <span style="color: #339933;">=</span> <span style="color: #0000dd;">0</span><span style="color: #339933;">;</span>
&nbsp;
<span style="color: #b1b100;">for</span> <span style="color: #009900;">(</span><span style="color: #993333;">int</span> i <span style="color: #339933;">=</span> <span style="color: #0000dd;">0</span><span style="color: #339933;">;</span> i <span style="color: #339933;">&lt;</span> N<span style="color: #339933;">;</span> i<span style="color: #339933;">++</span><span style="color: #009900;">)</span>
  <span style="color: #b1b100;">for</span> <span style="color: #009900;">(</span><span style="color: #993333;">int</span> j <span style="color: #339933;">=</span> <span style="color: #0000dd;">0</span><span style="color: #339933;">;</span> j <span style="color: #339933;">&lt;</span> N<span style="color: #339933;">;</span> j<span style="color: #339933;">++</span><span style="color: #009900;">)</span>
    <span style="color: #b1b100;">for</span> <span style="color: #009900;">(</span><span style="color: #993333;">int</span> k <span style="color: #339933;">=</span> <span style="color: #0000dd;">0</span><span style="color: #339933;">;</span> k <span style="color: #339933;">&lt;</span> N<span style="color: #339933;">;</span> k<span style="color: #339933;">++</span><span style="color: #009900;">)</span>
       C<span style="color: #009900;">[</span>i<span style="color: #009900;">]</span><span style="color: #009900;">[</span>j<span style="color: #009900;">]</span> <span style="color: #339933;">+=</span> A<span style="color: #009900;">[</span>i<span style="color: #009900;">]</span><span style="color: #009900;">[</span>k<span style="color: #009900;">]</span> <span style="color: #339933;">*</span> B<span style="color: #009900;">[</span>k<span style="color: #009900;">]</span><span style="color: #009900;">[</span>j<span style="color: #009900;">]</span><span style="color: #339933;">;</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">float A[N][N];
float B[M][N];
// Result
float C[N][N];

for (int i = 0; i &lt; N; i++)
  for (int j = 0; j &lt; N; j++)
    C[i][j] = 0;

for (int i = 0; i &lt; N; i++)
  for (int j = 0; j &lt; N; j++)
    for (int k = 0; k &lt; N; k++)
       C[i][j] += A[i][k] * B[k][j];</p></div>

<p>
After this change, the interesting part of our algorithm, line 13, is inside a perfect nest of loops of depth 3.
</p>
<h3>Accessing a matrix</h3>
<p>
It is relatively straightforward to access an array of just one dimension, like in <code>a[i]</code>. Just get <code>i</code>, multiply it by the size in bytes of each element of the array and then add the address of <code>a</code> (the base address of the array). So, the address of <code>a[i]</code> is just <code>a + ELEMENTSIZE*i</code>.
</p>
<p>
Things get a bit more complicated when our array has more than one dimension, like a matrix or a cube. Given an access like <code>a[i][j][k]</code> we have to compute which element is denoted by <code>[i][j][k]</code>. This depends on whether the language is row-major order or column-major order. We assume row-major order here (like in C language). So <code>[i][j][k]</code> must denote <code>k + j * NK + i * NK * NJ</code>, where <code>NK</code> and <code>NJ</code> are the number of elements in every dimension. For instance, a three dimensional array of 3 x 4 x 5 elements, the element [1][2][3] is 3 + 2 * 5 + 1 * 5 * 4 = 23 (here <code>NK</code> = 5 and <code>NJ</code> = 4. Note that <code>NI</code> = 3 but we do not need it at all). We assume that our language indexes arrays starting from 0 (like C). If the language allows a lower bound other than 0, we first have to subtract the lower bound to get the position.
</p>
<p>
We can compute the position in a slightly better way if we reorder it. Instead of calculating  <code>k + j * NK + i * NK * NJ</code> we will do <code>k + NK * (j + NJ * i)</code>. This way all the calculus is just a repeated set of steps calculating <code>x + N<sub>i</sub> * y</code> like in the example below.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> Calculating the address of C<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>j<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>k<span style="color: #009900; font-weight: bold;">]</span> declared as <span style="color: #00007f; font-weight: bold;">int</span> C<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">3</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">5</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
<span style="color: #339933;">/*</span> &amp;C<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>j<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>k<span style="color: #009900; font-weight: bold;">]</span> is<span style="color: #339933;">,</span> thus<span style="color: #339933;">,</span> C <span style="color: #339933;">+</span> ELEMENTSIZE <span style="color: #339933;">*</span> <span style="color: #009900; font-weight: bold;">(</span> k <span style="color: #339933;">+</span> NK <span style="color: #339933;">*</span> <span style="color: #009900; font-weight: bold;">(</span>j <span style="color: #339933;">+</span> NJ <span style="color: #339933;">*</span> i<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
<span style="color: #339933;">//</span> Assume i is <span style="color: #00007f; font-weight: bold;">in</span> r4<span style="color: #339933;">,</span> j <span style="color: #00007f; font-weight: bold;">in</span> r5 <span style="color: #00007f; font-weight: bold;">and</span> k <span style="color: #00007f; font-weight: bold;">in</span> r6 <span style="color: #00007f; font-weight: bold;">and</span> the base address of C <span style="color: #00007f; font-weight: bold;">in</span> r3 <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">mov</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>        <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r8</span> ← NJ <span style="color: #009900; font-weight: bold;">(</span>Recall that NJ = <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">mul</span> r7<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r4    <span style="color: #339933;">/*</span> r7 ← NJ <span style="color: #339933;">*</span> i <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">add</span> r7<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r7    <span style="color: #339933;">/*</span> r7 ← j <span style="color: #339933;">+</span> NJ <span style="color: #339933;">*</span> i <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">mov</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">5</span>        <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r8</span> ← NK <span style="color: #009900; font-weight: bold;">(</span>Recall that NK = <span style="color: #ff0000;">5</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">mul</span> r7<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r7    <span style="color: #339933;">/*</span> r7 ← NK <span style="color: #339933;">*</span> <span style="color: #009900; font-weight: bold;">(</span>j <span style="color: #339933;">+</span> NJ <span style="color: #339933;">*</span> i<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">add</span> r7<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> r7    <span style="color: #339933;">/*</span> r7 ← k <span style="color: #339933;">+</span> NK <span style="color: #339933;">*</span> <span style="color: #009900; font-weight: bold;">(</span>j <span style="color: #339933;">+</span> NJ <span style="color: #339933;">+</span> i<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">mov</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>        <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r8</span> ← ELEMENTSIZE <span style="color: #009900; font-weight: bold;">(</span>Recall that size of an <span style="color: #00007f; font-weight: bold;">int</span> is <span style="color: #ff0000;">4</span> bytes<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">mul</span> r7<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r7    <span style="color: #339933;">/*</span> r7 ← ELEMENTSIZE <span style="color: #339933;">*</span> <span style="color: #009900; font-weight: bold;">(</span> k <span style="color: #339933;">+</span> NK <span style="color: #339933;">*</span> <span style="color: #009900; font-weight: bold;">(</span>j <span style="color: #339933;">+</span> NJ <span style="color: #339933;">*</span> i<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">add</span> r7<span style="color: #339933;">,</span> r3<span style="color: #339933;">,</span> r7    <span style="color: #339933;">/*</span> r7 ← C <span style="color: #339933;">+</span> ELEMENTSIZE <span style="color: #339933;">*</span> <span style="color: #009900; font-weight: bold;">(</span> k <span style="color: #339933;">+</span> NK <span style="color: #339933;">*</span> <span style="color: #009900; font-weight: bold;">(</span>j <span style="color: #339933;">+</span> NJ <span style="color: #339933;">*</span> i<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* Calculating the address of C[i][j][k] declared as int C[3][4][5] */
/* &amp;C[i][j][k] is, thus, C + ELEMENTSIZE * ( k + NK * (j + NJ * i) ) */
// Assume i is in r4, j in r5 and k in r6 and the base address of C in r3 */
mov r8, #4        /* r8 ← NJ (Recall that NJ = 4) */
mul r7, r8, r4    /* r7 ← NJ * i */
add r7, r5, r7    /* r7 ← j + NJ * i */
mov r8, #5        /* r8 ← NK (Recall that NK = 5) */
mul r7, r8, r7    /* r7 ← NK * (j + NJ * i) */
add r7, r6, r7    /* r7 ← k + NK * (j + NJ + i) */
mov r8, #4        /* r8 ← ELEMENTSIZE (Recall that size of an int is 4 bytes) */
mul r7, r8, r7    /* r7 ← ELEMENTSIZE * ( k + NK * (j + NJ * i) ) */
add r7, r3, r7    /* r7 ← C + ELEMENTSIZE * ( k + NK * (j + NJ * i) ) */</p></div>

<h2>Naive matrix multiply of 4×4 single-precision</h2>
<p>
As a first step, let’s implement a naive matrix multiply that follows the C algorithm above according to the letter.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
21
22
23
24
25
26
27
28
29
30
31
32
33
34
35
36
37
38
39
40
41
42
43
44
45
46
47
48
49
50
51
52
53
54
55
56
57
58
59
60
61
62
63
64
65
66
67
68
69
70
71
72
73
74
75
76
77
78
79
80
81
82
83
84
85
86
87
88
89
90
91
92
93
94
95
96
97
98
99
100
101
102
103
104
105
106
107
108
109
110
111
112
113
114
115
116
117
118
119
120
121
122
123
124
125
126
127
128
129
130
131
132
133
134
135
136
137
138
139
140
141
142
143
144
145
146
147
148
149
150
151
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> <span style="color: #339933;">--</span> matmul<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #0000ff; font-weight: bold;">.data</span>
mat_A<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">float</span> <span style="color: #ff0000;">0.1</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">0.2</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">0.0</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">0.1</span>
       <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">float</span> <span style="color: #ff0000;">0.2</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">0.1</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">0.3</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">0.0</span>
       <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">float</span> <span style="color: #ff0000;">0.0</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">0.3</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">0.1</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">0.5</span> 
       <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">float</span> <span style="color: #ff0000;">0.0</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">0.6</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">0.4</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">0.1</span>
mat_B<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">float</span>  <span style="color: #ff0000;">4.92</span><span style="color: #339933;">,</span>  <span style="color: #ff0000;">2.54</span><span style="color: #339933;">,</span> <span style="color: #339933;">-</span><span style="color: #ff0000;">0.63</span><span style="color: #339933;">,</span> <span style="color: #339933;">-</span><span style="color: #ff0000;">1.75</span>
       <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">float</span>  <span style="color: #ff0000;">3.02</span><span style="color: #339933;">,</span> <span style="color: #339933;">-</span><span style="color: #ff0000;">1.51</span><span style="color: #339933;">,</span> <span style="color: #339933;">-</span><span style="color: #ff0000;">0.87</span><span style="color: #339933;">,</span>  <span style="color: #ff0000;">1.35</span>
       <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">float</span> <span style="color: #339933;">-</span><span style="color: #ff0000;">4.29</span><span style="color: #339933;">,</span>  <span style="color: #ff0000;">2.14</span><span style="color: #339933;">,</span>  <span style="color: #ff0000;">0.71</span><span style="color: #339933;">,</span>  <span style="color: #ff0000;">0.71</span>
       <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">float</span> <span style="color: #339933;">-</span><span style="color: #ff0000;">0.95</span><span style="color: #339933;">,</span>  <span style="color: #ff0000;">0.48</span><span style="color: #339933;">,</span>  <span style="color: #ff0000;">2.38</span><span style="color: #339933;">,</span> <span style="color: #339933;">-</span><span style="color: #ff0000;">0.95</span>
mat_C<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">float</span> <span style="color: #ff0000;">0.0</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">0.0</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">0.0</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">0.0</span>
       <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">float</span> <span style="color: #ff0000;">0.0</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">0.0</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">0.0</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">0.0</span>
       <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">float</span> <span style="color: #ff0000;">0.0</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">0.0</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">0.0</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">0.0</span>
       <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">float</span> <span style="color: #ff0000;">0.0</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">0.0</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">0.0</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">0.0</span>
       <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">float</span> <span style="color: #ff0000;">0.0</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">0.0</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">0.0</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">0.0</span>
&nbsp;
format_result <span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"Matrix result is:\n%5.2f %5.2f %5.2f %5.2f\n%5.2f %5.2f %5.2f %5.2f\n%5.2f %5.2f %5.2f %5.2f\n%5.2f %5.2f %5.2f %5.2f\n"</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
naive_matmul_4x4<span style="color: #339933;">:</span>
    <span style="color: #339933;">/*</span> r0 address of A
       r1 address of B
       r2 address of C
    <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> r7<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">/*</span> Keep integer registers <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> First zero <span style="color: #ff0000;">16</span> single floating point <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">In</span> IEEE <span style="color: #ff0000;">754</span><span style="color: #339933;">,</span> all <span style="color: #0000ff; font-weight: bold;">bits</span> cleared means <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> r2
    <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">16</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r6<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>
    b <span style="color: #339933;">.</span>Lloop_init_test
    <span style="color: #339933;">.</span>Lloop_init <span style="color: #339933;">:</span>
      <span style="color: #00007f; font-weight: bold;">str</span> r6<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> <span style="color: #339933;">+</span>#<span style="color: #ff0000;">4</span>   <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span>r4 ← r6 then r4 ← r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span> <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>Lloop_init_test<span style="color: #339933;">:</span>
      subs r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>
      bge <span style="color: #339933;">.</span>Lloop_init
&nbsp;
    <span style="color: #339933;">/*</span> We will use 
           r4 as i
           r5 as j
           r6 as k
    <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span> <span style="color: #339933;">/*</span> r4 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>Lloop_i<span style="color: #339933;">:</span>  <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">loop</span> header of i <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">cmp</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> if r4 == <span style="color: #ff0000;">4</span> goto end of the <span style="color: #00007f; font-weight: bold;">loop</span> i <span style="color: #339933;">*/</span>
      beq <span style="color: #339933;">.</span>Lend_loop_i
      <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>  <span style="color: #339933;">/*</span> r5 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
      <span style="color: #339933;">.</span>Lloop_j<span style="color: #339933;">:</span> <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">loop</span> header of j <span style="color: #339933;">*/</span>
       <span style="color: #00007f; font-weight: bold;">cmp</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span> <span style="color: #339933;">/*</span> if r5 == <span style="color: #ff0000;">4</span> goto end of the <span style="color: #00007f; font-weight: bold;">loop</span> j <span style="color: #339933;">*/</span>
        beq <span style="color: #339933;">.</span>Lend_loop_j
        <span style="color: #339933;">/*</span> Compute the address of C<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>j<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #00007f; font-weight: bold;">and</span> load it <span style="color: #00007f; font-weight: bold;">into</span> s0 <span style="color: #339933;">*/</span>
        <span style="color: #339933;">/*</span> Address of C<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>j<span style="color: #009900; font-weight: bold;">]</span> is C <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #ff0000;">4</span> <span style="color: #339933;">*</span> i <span style="color: #339933;">+</span> j<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">mov</span> r7<span style="color: #339933;">,</span> r5               <span style="color: #339933;">/*</span> r7 ← r5<span style="color: #339933;">.</span> This is r7 ← j <span style="color: #339933;">*/</span>
        adds r7<span style="color: #339933;">,</span> r7<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span>  <span style="color: #339933;">/*</span> r7 ← r7 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span>r4 &lt;&lt; <span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> 
                                    This is r7 ← j <span style="color: #339933;">+</span> i <span style="color: #339933;">*</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">.</span>
                                    We multiply i by the row size <span style="color: #009900; font-weight: bold;">(</span><span style="color: #ff0000;">4</span> elements<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
        adds r7<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r7<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span>  <span style="color: #339933;">/*</span> r7 ← r2 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span>r7 &lt;&lt; <span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span>
                                    This is r7 ← C <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>j <span style="color: #339933;">+</span> i <span style="color: #339933;">*</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span>
                                    We multiply <span style="color: #009900; font-weight: bold;">(</span>j <span style="color: #339933;">+</span> i <span style="color: #339933;">*</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span> by the size of the element<span style="color: #339933;">.</span>
                                    A single<span style="color: #339933;">-</span>precision floating point takes <span style="color: #ff0000;">4</span> bytes<span style="color: #339933;">.</span>
                                    <span style="color: #339933;">*/</span>
        vldr s0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r7<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">/*</span> s0 ← <span style="color: #339933;">*</span>r7 <span style="color: #339933;">*/</span>
&nbsp;
        <span style="color: #00007f; font-weight: bold;">mov</span> r6<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span> <span style="color: #339933;">/*</span> r6 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
        <span style="color: #339933;">.</span>Lloop_k <span style="color: #339933;">:</span>  <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">loop</span> header of k <span style="color: #339933;">*/</span>
          <span style="color: #00007f; font-weight: bold;">cmp</span> r6<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span> <span style="color: #339933;">/*</span> if r6 == <span style="color: #ff0000;">4</span> goto end of the <span style="color: #00007f; font-weight: bold;">loop</span> k <span style="color: #339933;">*/</span>
          beq <span style="color: #339933;">.</span>Lend_loop_k
&nbsp;
          <span style="color: #339933;">/*</span> Compute the address of a<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>k<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #00007f; font-weight: bold;">and</span> load it <span style="color: #00007f; font-weight: bold;">into</span> s1 <span style="color: #339933;">*/</span>
          <span style="color: #339933;">/*</span> Address of a<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>k<span style="color: #009900; font-weight: bold;">]</span> is a <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #ff0000;">4</span> <span style="color: #339933;">*</span> i <span style="color: #339933;">+</span> k<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
          <span style="color: #00007f; font-weight: bold;">mov</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r6               <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r8</span> ← r6<span style="color: #339933;">.</span> This is <span style="color: #46aa03; font-weight: bold;">r8</span> ← k <span style="color: #339933;">*/</span>
          adds <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span>  <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r8</span> ← <span style="color: #46aa03; font-weight: bold;">r8</span> <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span>r4 &lt;&lt; <span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> This is <span style="color: #46aa03; font-weight: bold;">r8</span> ← k <span style="color: #339933;">+</span> i <span style="color: #339933;">*</span> <span style="color: #ff0000;">4</span> <span style="color: #339933;">*/</span>
          adds <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span>  <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r8</span> ← r0 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span><span style="color: #46aa03; font-weight: bold;">r8</span> &lt;&lt; <span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> This is <span style="color: #46aa03; font-weight: bold;">r8</span> ← a <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>k <span style="color: #339933;">+</span> i <span style="color: #339933;">*</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
          vldr s1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #009900; font-weight: bold;">]</span>            <span style="color: #339933;">/*</span> s1 ← <span style="color: #339933;">*</span><span style="color: #46aa03; font-weight: bold;">r8</span> <span style="color: #339933;">*/</span>
&nbsp;
          <span style="color: #339933;">/*</span> Compute the address of b<span style="color: #009900; font-weight: bold;">[</span>k<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>j<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #00007f; font-weight: bold;">and</span> load it <span style="color: #00007f; font-weight: bold;">into</span> s2 <span style="color: #339933;">*/</span>
          <span style="color: #339933;">/*</span> Address of b<span style="color: #009900; font-weight: bold;">[</span>k<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>j<span style="color: #009900; font-weight: bold;">]</span> is b <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #ff0000;">4</span> <span style="color: #339933;">*</span> k <span style="color: #339933;">+</span> j<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
          <span style="color: #00007f; font-weight: bold;">mov</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r5               <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r8</span> ← r5<span style="color: #339933;">.</span> This is <span style="color: #46aa03; font-weight: bold;">r8</span> ← j <span style="color: #339933;">*/</span>
          adds <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span>  <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r8</span> ← <span style="color: #46aa03; font-weight: bold;">r8</span> <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span>r6 &lt;&lt; <span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> This is <span style="color: #46aa03; font-weight: bold;">r8</span> ← j <span style="color: #339933;">+</span> k <span style="color: #339933;">*</span> <span style="color: #ff0000;">4</span> <span style="color: #339933;">*/</span>
          adds <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span>  <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r8</span> ← r1 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span><span style="color: #46aa03; font-weight: bold;">r8</span> &lt;&lt; <span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> This is <span style="color: #46aa03; font-weight: bold;">r8</span> ← b <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>j <span style="color: #339933;">+</span> k <span style="color: #339933;">*</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
          vldr s2<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #009900; font-weight: bold;">]</span>            <span style="color: #339933;">/*</span> s1 ← <span style="color: #339933;">*</span><span style="color: #46aa03; font-weight: bold;">r8</span> <span style="color: #339933;">*/</span>
&nbsp;
          vmul<span style="color: #339933;">.</span>f32 s3<span style="color: #339933;">,</span> s1<span style="color: #339933;">,</span> s2      <span style="color: #339933;">/*</span> s3 ← s1 <span style="color: #339933;">*</span> s2 <span style="color: #339933;">*/</span>
          vadd<span style="color: #339933;">.</span>f32 s0<span style="color: #339933;">,</span> s0<span style="color: #339933;">,</span> s3      <span style="color: #339933;">/*</span> s0 ← s0 <span style="color: #339933;">+</span> s3 <span style="color: #339933;">*/</span>
&nbsp;
          <span style="color: #00007f; font-weight: bold;">add</span> r6<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>           <span style="color: #339933;">/*</span> r6 ← r6 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
          b <span style="color: #339933;">.</span>Lloop_k               <span style="color: #339933;">/*</span> next iteration of <span style="color: #00007f; font-weight: bold;">loop</span> k <span style="color: #339933;">*/</span>
        <span style="color: #339933;">.</span>Lend_loop_k<span style="color: #339933;">:</span> <span style="color: #339933;">/*</span> Here ends <span style="color: #00007f; font-weight: bold;">loop</span> k <span style="color: #339933;">*/</span>
        vstr s0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r7<span style="color: #009900; font-weight: bold;">]</span>            <span style="color: #339933;">/*</span> Store s0 back to C<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>j<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">add</span> r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>  <span style="color: #339933;">/*</span> r5 ← r5 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
        b <span style="color: #339933;">.</span>Lloop_j <span style="color: #339933;">/*</span> next iteration of <span style="color: #00007f; font-weight: bold;">loop</span> j <span style="color: #339933;">*/</span>
       <span style="color: #339933;">.</span>Lend_loop_j<span style="color: #339933;">:</span> <span style="color: #339933;">/*</span> Here ends <span style="color: #00007f; font-weight: bold;">loop</span> j <span style="color: #339933;">*/</span>
       <span style="color: #00007f; font-weight: bold;">add</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span> <span style="color: #339933;">/*</span> r4 ← r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
       b <span style="color: #339933;">.</span>Lloop_i     <span style="color: #339933;">/*</span> next iteration of <span style="color: #00007f; font-weight: bold;">loop</span> i <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>Lend_loop_i<span style="color: #339933;">:</span> <span style="color: #339933;">/*</span> Here ends <span style="color: #00007f; font-weight: bold;">loop</span> i <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> r7<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>  <span style="color: #339933;">/*</span> Restore integer registers <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Leave</span> function <span style="color: #339933;">*/</span>
&nbsp;
&nbsp;
<span style="color: #339933;">.</span>globl main
main<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>  <span style="color: #339933;">/*</span> Keep integer registers <span style="color: #339933;">*/</span>
    vpush <span style="color: #009900; font-weight: bold;">{</span>d0<span style="color: #339933;">-</span>d1<span style="color: #009900; font-weight: bold;">}</span>          <span style="color: #339933;">/*</span> Keep floating point registers <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> Prepare <span style="color: #00007f; font-weight: bold;">call</span> to naive_matmul_4x4 <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> addr_mat_A  <span style="color: #339933;">/*</span> r0 ← a <span style="color: #339933;">*/</span>
    ldr r1<span style="color: #339933;">,</span> addr_mat_B  <span style="color: #339933;">/*</span> r1 ← b <span style="color: #339933;">*/</span>
    ldr r2<span style="color: #339933;">,</span> addr_mat_C  <span style="color: #339933;">/*</span> r2 ← c <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> naive_matmul_4x4
&nbsp;
    <span style="color: #339933;">/*</span> Now print the result matrix <span style="color: #339933;">*/</span>
    ldr r4<span style="color: #339933;">,</span> addr_mat_C  <span style="color: #339933;">/*</span> r4 ← c <span style="color: #339933;">*/</span>
&nbsp;
    vldr s0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">/*</span> s0 ← <span style="color: #339933;">*</span>r4<span style="color: #339933;">.</span> This is s0 ← c<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
    vcvt<span style="color: #339933;">.</span>f64<span style="color: #339933;">.</span>f32 d1<span style="color: #339933;">,</span> s0 <span style="color: #339933;">/*</span> Convert it <span style="color: #00007f; font-weight: bold;">into</span> a double<span style="color: #339933;">-</span>precision
                           d1 ← s0
                         <span style="color: #339933;">*/</span>
    vmov r2<span style="color: #339933;">,</span> r3<span style="color: #339933;">,</span> d1      <span style="color: #339933;">/*</span> <span style="color: #009900; font-weight: bold;">{</span>r2<span style="color: #339933;">,</span>r3<span style="color: #009900; font-weight: bold;">}</span> ← d1 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r6<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span>     <span style="color: #339933;">/*</span> Remember the <span style="color: #0000ff; font-weight: bold;">stack</span> pointer<span style="color: #339933;">,</span> we need it to restore it back later <span style="color: #339933;">*/</span>
                   <span style="color: #339933;">/*</span> r6 ← <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>  <span style="color: #339933;">/*</span> We will iterate from <span style="color: #ff0000;">1</span> to <span style="color: #ff0000;">15</span> <span style="color: #009900; font-weight: bold;">(</span>because the 0th item has already been handled <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">60</span> <span style="color: #339933;">/*</span> Go to the last item of the matrix c<span style="color: #339933;">,</span> this is c<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">3</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">3</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>Lloop<span style="color: #339933;">:</span>
        vldr s0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">/*</span> s0 ← <span style="color: #339933;">*</span>r4<span style="color: #339933;">.</span> Load the current item <span style="color: #339933;">*/</span>
        vcvt<span style="color: #339933;">.</span>f64<span style="color: #339933;">.</span>f32 d1<span style="color: #339933;">,</span> s0 <span style="color: #339933;">/*</span> Convert it <span style="color: #00007f; font-weight: bold;">into</span> a double<span style="color: #339933;">-</span>precision
                               d1 ← s0
                             <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">sub</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span>      <span style="color: #339933;">/*</span> Make room <span style="color: #00007f; font-weight: bold;">in</span> the <span style="color: #0000ff; font-weight: bold;">stack</span> for the double<span style="color: #339933;">-</span>precision <span style="color: #339933;">*/</span>
        vstr d1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #009900; font-weight: bold;">]</span>       <span style="color: #339933;">/*</span> Store the double precision <span style="color: #00007f; font-weight: bold;">in</span> the top of the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">sub</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>      <span style="color: #339933;">/*</span> Move to the previous element <span style="color: #00007f; font-weight: bold;">in</span> the matrix <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">add</span> r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>      <span style="color: #339933;">/*</span> One more item has been handled <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">cmp</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">16</span>         <span style="color: #339933;">/*</span> if r5 != <span style="color: #ff0000;">16</span> go to next iteration of the <span style="color: #00007f; font-weight: bold;">loop</span> <span style="color: #339933;">*/</span>
        bne <span style="color: #339933;">.</span>Lloop
&nbsp;
    ldr r0<span style="color: #339933;">,</span> addr_format_result <span style="color: #339933;">/*</span> r0 ← &amp;format_result <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> printf <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> printf <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> r6  <span style="color: #339933;">/*</span> Restore the <span style="color: #0000ff; font-weight: bold;">stack</span> after the <span style="color: #00007f; font-weight: bold;">call</span>  <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>
    vpop <span style="color: #009900; font-weight: bold;">{</span>d0<span style="color: #339933;">-</span>d1<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr
&nbsp;
addr_mat_A <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> mat_A
addr_mat_B <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> mat_B
addr_mat_C <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> mat_C
addr_format_result <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> format_result</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* -- matmul.s */
.data
mat_A: .float 0.1, 0.2, 0.0, 0.1
       .float 0.2, 0.1, 0.3, 0.0
       .float 0.0, 0.3, 0.1, 0.5 
       .float 0.0, 0.6, 0.4, 0.1
mat_B: .float  4.92,  2.54, -0.63, -1.75
       .float  3.02, -1.51, -0.87,  1.35
       .float -4.29,  2.14,  0.71,  0.71
       .float -0.95,  0.48,  2.38, -0.95
mat_C: .float 0.0, 0.0, 0.0, 0.0
       .float 0.0, 0.0, 0.0, 0.0
       .float 0.0, 0.0, 0.0, 0.0
       .float 0.0, 0.0, 0.0, 0.0
       .float 0.0, 0.0, 0.0, 0.0

format_result : .asciz "Matrix result is:\n%5.2f %5.2f %5.2f %5.2f\n%5.2f %5.2f %5.2f %5.2f\n%5.2f %5.2f %5.2f %5.2f\n%5.2f %5.2f %5.2f %5.2f\n"

.text

naive_matmul_4x4:
    /* r0 address of A
       r1 address of B
       r2 address of C
    */
    push {r4, r5, r6, r7, r8, lr} /* Keep integer registers */
    /* First zero 16 single floating point */
    /* In IEEE 754, all bits cleared means 0 */
    mov r4, r2
    mov r5, #16
    mov r6, #0
    b .Lloop_init_test
    .Lloop_init :
      str r6, [r4], +#4   /* *r4 ← r6 then r4 ← r4 + 4 */
    .Lloop_init_test:
      subs r5, r5, #1
      bge .Lloop_init

    /* We will use 
           r4 as i
           r5 as j
           r6 as k
    */
    mov r4, #0 /* r4 ← 0 */
    .Lloop_i:  /* loop header of i */
      cmp r4, #4  /* if r4 == 4 goto end of the loop i */
      beq .Lend_loop_i
      mov r5, #0  /* r5 ← 0 */
      .Lloop_j: /* loop header of j */
       cmp r5, #4 /* if r5 == 4 goto end of the loop j */
        beq .Lend_loop_j
        /* Compute the address of C[i][j] and load it into s0 */
        /* Address of C[i][j] is C + 4*(4 * i + j) */
        mov r7, r5               /* r7 ← r5. This is r7 ← j */
        adds r7, r7, r4, LSL #2  /* r7 ← r7 + (r4 &lt;&lt; 2). 
                                    This is r7 ← j + i * 4.
                                    We multiply i by the row size (4 elements) */
        adds r7, r2, r7, LSL #2  /* r7 ← r2 + (r7 &lt;&lt; 2).
                                    This is r7 ← C + 4*(j + i * 4)
                                    We multiply (j + i * 4) by the size of the element.
                                    A single-precision floating point takes 4 bytes.
                                    */
        vldr s0, [r7] /* s0 ← *r7 */

        mov r6, #0 /* r6 ← 0 */
        .Lloop_k :  /* loop header of k */
          cmp r6, #4 /* if r6 == 4 goto end of the loop k */
          beq .Lend_loop_k

          /* Compute the address of a[i][k] and load it into s1 */
          /* Address of a[i][k] is a + 4*(4 * i + k) */
          mov r8, r6               /* r8 ← r6. This is r8 ← k */
          adds r8, r8, r4, LSL #2  /* r8 ← r8 + (r4 &lt;&lt; 2). This is r8 ← k + i * 4 */
          adds r8, r0, r8, LSL #2  /* r8 ← r0 + (r8 &lt;&lt; 2). This is r8 ← a + 4*(k + i * 4) */
          vldr s1, [r8]            /* s1 ← *r8 */

          /* Compute the address of b[k][j] and load it into s2 */
          /* Address of b[k][j] is b + 4*(4 * k + j) */
          mov r8, r5               /* r8 ← r5. This is r8 ← j */
          adds r8, r8, r6, LSL #2  /* r8 ← r8 + (r6 &lt;&lt; 2). This is r8 ← j + k * 4 */
          adds r8, r1, r8, LSL #2  /* r8 ← r1 + (r8 &lt;&lt; 2). This is r8 ← b + 4*(j + k * 4) */
          vldr s2, [r8]            /* s1 ← *r8 */

          vmul.f32 s3, s1, s2      /* s3 ← s1 * s2 */
          vadd.f32 s0, s0, s3      /* s0 ← s0 + s3 */

          add r6, r6, #1           /* r6 ← r6 + 1 */
          b .Lloop_k               /* next iteration of loop k */
        .Lend_loop_k: /* Here ends loop k */
        vstr s0, [r7]            /* Store s0 back to C[i][j] */
        add r5, r5, #1  /* r5 ← r5 + 1 */
        b .Lloop_j /* next iteration of loop j */
       .Lend_loop_j: /* Here ends loop j */
       add r4, r4, #1 /* r4 ← r4 + 1 */
       b .Lloop_i     /* next iteration of loop i */
    .Lend_loop_i: /* Here ends loop i */

    pop {r4, r5, r6, r7, r8, lr}  /* Restore integer registers */
    bx lr /* Leave function */
    

.globl main
main:
    push {r4, r5, r6, lr}  /* Keep integer registers */
    vpush {d0-d1}          /* Keep floating point registers */

    /* Prepare call to naive_matmul_4x4 */
    ldr r0, addr_mat_A  /* r0 ← a */
    ldr r1, addr_mat_B  /* r1 ← b */
    ldr r2, addr_mat_C  /* r2 ← c */
    bl naive_matmul_4x4

    /* Now print the result matrix */
    ldr r4, addr_mat_C  /* r4 ← c */

    vldr s0, [r4] /* s0 ← *r4. This is s0 ← c[0][0] */
    vcvt.f64.f32 d1, s0 /* Convert it into a double-precision
                           d1 ← s0
                         */
    vmov r2, r3, d1      /* {r2,r3} ← d1 */

    mov r6, sp     /* Remember the stack pointer, we need it to restore it back later */
                   /* r6 ← sp */

    mov r5, #1  /* We will iterate from 1 to 15 (because the 0th item has already been handled */
    add r4, r4, #60 /* Go to the last item of the matrix c, this is c[3][3] */
    .Lloop:
        vldr s0, [r4] /* s0 ← *r4. Load the current item */
        vcvt.f64.f32 d1, s0 /* Convert it into a double-precision
                               d1 ← s0
                             */
        sub sp, sp, #8      /* Make room in the stack for the double-precision */
        vstr d1, [sp]       /* Store the double precision in the top of the stack */
        sub r4, r4, #4      /* Move to the previous element in the matrix */
        add r5, r5, #1      /* One more item has been handled */
        cmp r5, #16         /* if r5 != 16 go to next iteration of the loop */
        bne .Lloop

    ldr r0, addr_format_result /* r0 ← &amp;format_result */
    bl printf /* call printf */
    mov sp, r6  /* Restore the stack after the call  */

    mov r0, #0
    vpop {d0-d1}
    pop {r4, r5, r6, lr}
    bx lr

addr_mat_A : .word mat_A
addr_mat_B : .word mat_B
addr_mat_C : .word mat_C
addr_format_result : .word format_result</p></div>

<p>
That’s a lot of code but it is not complicated. Unfortunately computing the address of the array takes an important number of instructions. In our <code>naive_matmul_4x4</code> we have the three loops <code>i</code>, <code>j</code> and <code>k</code> of the C algorithm. We compute the address of <code>C[i][j]</code> in the loop <code>j</code> (there is no need to compute it every time in the loop <code>k</code>) in lines 52 to 63. The value contained in <code>C[i][j]</code> is then loaded into <code>s0</code>. In each iteration of loop <code>k</code> we load <code>A[i][k]</code> and <code>B[k][j]</code> in <code>s1</code> and <code>s2</code> respectively (lines 70 to 82). After the loop <code>k</code> ends, we can store <code>s0</code> back to the array position (kept in <code>r7</code>, line 90)
</p>
<p>
In order to print the result matrix we have to pass 16 floating points to <code>printf</code>. Unfortunately, as stated in chapter 13, we have to first convert them into double-precision before passing them. Note also that the first single-precision can be passed using registers <code>r2</code> and <code>r3</code>. All the remaining must be passed on the stack and do not forget that the stack parameters must be passed in opposite order. This is why once the first element of the C matrix has been loaded in <code>{r2,r3}</code> (lines 117 to 120) we advance 60 bytes r4. This is <code>C[3][3]</code>, the last element of the matrix C. We load the single-precision, convert it into double-precision, push it in the stack and then move backwards register <code>r4</code>, to the previous element in the matrix (lines 128 to 137). Observe that we use <code>r6</code> as a marker of the stack, since we need to restore the stack once <code>printf</code> returns (line 122 and line 141). Of course we could avoid using <code>r6</code> and instead do <code>add sp, sp, #120</code> since this is exactly the amount of bytes we push to the stack (15 values of double-precision, each taking 8 bytes).
</p>
<p>
I have not chosen the values of the two matrices randomly. The second one is (approximately) the inverse of the first. This way we will get the identity matrix (a matrix with all zeros but a diagonal of ones). Due to rounding issues the result matrix will not be the identity, but it will be pretty close. Let’s run the program.
</p>
<pre>$ ./matmul 
Matrix result is:
 1.00 -0.00  0.00  0.00
-0.00  1.00  0.00 -0.00
 0.00  0.00  1.00  0.00
 0.00 -0.00  0.00  1.00
</pre>
<h2>Vectorial approach</h2>
<p>
The algorithm we are trying to implement is fine but it is not the most optimizable. The problem lies in the way the loop <code>k</code> accesses the elements. Access <code>A[i][k]</code> is eligible for a multiple load as <code>A[i][k]</code> and <code>A[i][k+1]</code> are contiguous elements in memory. This way we could entirely avoid all the loop <code>k</code> and perform a 4 element load from <code>A[i][0]</code> to <code>A[i][3]</code>. The access <code>B[k][j]</code> does not allow that since elements <code>B[k][j]</code> and <code>B[k+1][j]</code> have a full row inbetween. This is a <em>strided access</em> (the stride here is a full row of 4 elements, this is 16 bytes), VFPv2 does not allow a strided multiple load, so we will have to load one by one.. Once we have all the elements of the loop <code>k</code> loaded, we can do a vector multiplication and a sum.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
21
22
23
24
25
26
27
28
29
30
31
32
33
34
35
36
37
38
39
40
41
42
43
44
45
46
47
48
49
50
51
52
53
54
55
56
57
58
59
60
61
62
63
64
65
66
67
68
69
70
71
72
73
74
75
76
77
78
79
80
81
82
83
84
85
86
87
88
89
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">naive_vectorial_matmul_4x4<span style="color: #339933;">:</span>
    <span style="color: #339933;">/*</span> r0 address of A
       r1 address of B
       r2 address of C
    <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> r7<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">/*</span> Keep integer registers <span style="color: #339933;">*/</span>
    vpush <span style="color: #009900; font-weight: bold;">{</span>s16<span style="color: #339933;">-</span>s19<span style="color: #009900; font-weight: bold;">}</span>               <span style="color: #339933;">/*</span> Floating point registers starting from s16 must be preserved <span style="color: #339933;">*/</span>
    vpush <span style="color: #009900; font-weight: bold;">{</span>s24<span style="color: #339933;">-</span>s27<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #339933;">/*</span> First zero <span style="color: #ff0000;">16</span> single floating point <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">In</span> IEEE <span style="color: #ff0000;">754</span><span style="color: #339933;">,</span> all <span style="color: #0000ff; font-weight: bold;">bits</span> cleared means <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> r2
    <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">16</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r6<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>
    b <span style="color: #339933;">.</span>L1_loop_init_test
    <span style="color: #339933;">.</span>L1_loop_init <span style="color: #339933;">:</span>
      <span style="color: #00007f; font-weight: bold;">str</span> r6<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> <span style="color: #339933;">+</span>#<span style="color: #ff0000;">4</span>   <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span>r4 ← r6 then r4 ← r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span> <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>L1_loop_init_test<span style="color: #339933;">:</span>
      subs r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>
      bge <span style="color: #339933;">.</span>L1_loop_init
&nbsp;
    <span style="color: #339933;">/*</span> Set the LEN field of FPSCR to be <span style="color: #ff0000;">4</span> <span style="color: #009900; font-weight: bold;">(</span>value <span style="color: #ff0000;">3</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> #0b011                        <span style="color: #339933;">/*</span> r5 ← <span style="color: #ff0000;">3</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">16</span>                   <span style="color: #339933;">/*</span> r5 ← r5 &lt;&lt; <span style="color: #ff0000;">16</span> <span style="color: #339933;">*/</span>
    fmrx r4<span style="color: #339933;">,</span> fpscr                        <span style="color: #339933;">/*</span> r4 ← fpscr <span style="color: #339933;">*/</span>
    orr r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> r5                        <span style="color: #339933;">/*</span> r4 ← r4 | r5 <span style="color: #339933;">*/</span>
    fmxr fpscr<span style="color: #339933;">,</span> r4                        <span style="color: #339933;">/*</span> fpscr ← r4 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> We will use 
           r4 as i
           r5 as j
           r6 as k
    <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span> <span style="color: #339933;">/*</span> r4 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>L1_loop_i<span style="color: #339933;">:</span>  <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">loop</span> header of i <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">cmp</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> if r4 == <span style="color: #ff0000;">4</span> goto end of the <span style="color: #00007f; font-weight: bold;">loop</span> i <span style="color: #339933;">*/</span>
      beq <span style="color: #339933;">.</span>L1_end_loop_i
      <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>  <span style="color: #339933;">/*</span> r5 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
      <span style="color: #339933;">.</span>L1_loop_j<span style="color: #339933;">:</span> <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">loop</span> header of j <span style="color: #339933;">*/</span>
       <span style="color: #00007f; font-weight: bold;">cmp</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span> <span style="color: #339933;">/*</span> if r5 == <span style="color: #ff0000;">4</span> goto end of the <span style="color: #00007f; font-weight: bold;">loop</span> j <span style="color: #339933;">*/</span>
        beq <span style="color: #339933;">.</span>L1_end_loop_j
        <span style="color: #339933;">/*</span> Compute the address of C<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>j<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #00007f; font-weight: bold;">and</span> load it <span style="color: #00007f; font-weight: bold;">into</span> s0 <span style="color: #339933;">*/</span>
        <span style="color: #339933;">/*</span> Address of C<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>j<span style="color: #009900; font-weight: bold;">]</span> is C <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #ff0000;">4</span> <span style="color: #339933;">*</span> i <span style="color: #339933;">+</span> j<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">mov</span> r7<span style="color: #339933;">,</span> r5               <span style="color: #339933;">/*</span> r7 ← r5<span style="color: #339933;">.</span> This is r7 ← j <span style="color: #339933;">*/</span>
        adds r7<span style="color: #339933;">,</span> r7<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span>  <span style="color: #339933;">/*</span> r7 ← r7 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span>r4 &lt;&lt; <span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> 
                                    This is r7 ← j <span style="color: #339933;">+</span> i <span style="color: #339933;">*</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">.</span>
                                    We multiply i by the row size <span style="color: #009900; font-weight: bold;">(</span><span style="color: #ff0000;">4</span> elements<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
        adds r7<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r7<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span>  <span style="color: #339933;">/*</span> r7 ← r2 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span>r7 &lt;&lt; <span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span>
                                    This is r7 ← C <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>j <span style="color: #339933;">+</span> i <span style="color: #339933;">*</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span>
                                    We multiply <span style="color: #009900; font-weight: bold;">(</span>j <span style="color: #339933;">+</span> i <span style="color: #339933;">*</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span> by the size of the element<span style="color: #339933;">.</span>
                                    A single<span style="color: #339933;">-</span>precision floating point takes <span style="color: #ff0000;">4</span> bytes<span style="color: #339933;">.</span>
                                    <span style="color: #339933;">*/</span>
        <span style="color: #339933;">/*</span> Compute the address of a<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">mov</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span>
        adds <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span>
        vldmia <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">{</span>s8<span style="color: #339933;">-</span>s11<span style="color: #009900; font-weight: bold;">}</span>  <span style="color: #339933;">/*</span> Load <span style="color: #009900; font-weight: bold;">{</span>s8<span style="color: #339933;">,</span>s9<span style="color: #339933;">,</span>s10<span style="color: #339933;">,</span>s11<span style="color: #009900; font-weight: bold;">}</span> ← <span style="color: #009900; font-weight: bold;">{</span>a<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> a<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> a<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> a<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">3</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">*/</span>
&nbsp;
        <span style="color: #339933;">/*</span> Compute the address of b<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>j<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">mov</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r5               <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r8</span> ← r5<span style="color: #339933;">.</span> This is <span style="color: #46aa03; font-weight: bold;">r8</span> ← j <span style="color: #339933;">*/</span>
        adds <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span>  <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r8</span> ← r1 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span><span style="color: #46aa03; font-weight: bold;">r8</span> &lt;&lt; <span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> This is <span style="color: #46aa03; font-weight: bold;">r8</span> ← b <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>j<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
        vldr s16<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #009900; font-weight: bold;">]</span>             <span style="color: #339933;">/*</span> s16 ← <span style="color: #339933;">*</span><span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">.</span> This is s16 ← b<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>j<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
        vldr s17<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">]</span>        <span style="color: #339933;">/*</span> s17 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #46aa03; font-weight: bold;">r8</span> <span style="color: #339933;">+</span> <span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> This is s17 ← b<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>j<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
        vldr s18<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">32</span><span style="color: #009900; font-weight: bold;">]</span>        <span style="color: #339933;">/*</span> s18 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #46aa03; font-weight: bold;">r8</span> <span style="color: #339933;">+</span> <span style="color: #ff0000;">32</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> This is s17 ← b<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>j<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
        vldr s19<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">48</span><span style="color: #009900; font-weight: bold;">]</span>        <span style="color: #339933;">/*</span> s19 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #46aa03; font-weight: bold;">r8</span> <span style="color: #339933;">+</span> <span style="color: #ff0000;">48</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> This is s17 ← b<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">3</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>j<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
&nbsp;
        vmul<span style="color: #339933;">.</span>f32 s24<span style="color: #339933;">,</span> s8<span style="color: #339933;">,</span> s16      <span style="color: #339933;">/*</span> <span style="color: #009900; font-weight: bold;">{</span>s24<span style="color: #339933;">,</span>s25<span style="color: #339933;">,</span>s26<span style="color: #339933;">,</span>s27<span style="color: #009900; font-weight: bold;">}</span> ← <span style="color: #009900; font-weight: bold;">{</span>s8<span style="color: #339933;">,</span>s9<span style="color: #339933;">,</span>s10<span style="color: #339933;">,</span>s11<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">*</span> <span style="color: #009900; font-weight: bold;">{</span>s16<span style="color: #339933;">,</span>s17<span style="color: #339933;">,</span>s18<span style="color: #339933;">,</span>s19<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">*/</span>
        vmov<span style="color: #339933;">.</span>f32 s0<span style="color: #339933;">,</span> s24           <span style="color: #339933;">/*</span> s0 ← s24 <span style="color: #339933;">*/</span>
        vadd<span style="color: #339933;">.</span>f32 s0<span style="color: #339933;">,</span> s0<span style="color: #339933;">,</span> s25       <span style="color: #339933;">/*</span> s0 ← s0 <span style="color: #339933;">+</span> s25 <span style="color: #339933;">*/</span>
        vadd<span style="color: #339933;">.</span>f32 s0<span style="color: #339933;">,</span> s0<span style="color: #339933;">,</span> s26       <span style="color: #339933;">/*</span> s0 ← s0 <span style="color: #339933;">+</span> s26 <span style="color: #339933;">*/</span>
        vadd<span style="color: #339933;">.</span>f32 s0<span style="color: #339933;">,</span> s0<span style="color: #339933;">,</span> s27       <span style="color: #339933;">/*</span> s0 ← s0 <span style="color: #339933;">+</span> s27 <span style="color: #339933;">*/</span>
&nbsp;
        vstr s0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r7<span style="color: #009900; font-weight: bold;">]</span>            <span style="color: #339933;">/*</span> Store s0 back to C<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>j<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">add</span> r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>  <span style="color: #339933;">/*</span> r5 ← r5 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
        b <span style="color: #339933;">.</span>L1_loop_j <span style="color: #339933;">/*</span> next iteration of <span style="color: #00007f; font-weight: bold;">loop</span> j <span style="color: #339933;">*/</span>
       <span style="color: #339933;">.</span>L1_end_loop_j<span style="color: #339933;">:</span> <span style="color: #339933;">/*</span> Here ends <span style="color: #00007f; font-weight: bold;">loop</span> j <span style="color: #339933;">*/</span>
       <span style="color: #00007f; font-weight: bold;">add</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span> <span style="color: #339933;">/*</span> r4 ← r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
       b <span style="color: #339933;">.</span>L1_loop_i     <span style="color: #339933;">/*</span> next iteration of <span style="color: #00007f; font-weight: bold;">loop</span> i <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>L1_end_loop_i<span style="color: #339933;">:</span> <span style="color: #339933;">/*</span> Here ends <span style="color: #00007f; font-weight: bold;">loop</span> i <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> Set the LEN field of FPSCR back to <span style="color: #ff0000;">1</span> <span style="color: #009900; font-weight: bold;">(</span>value <span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> #0b011                        <span style="color: #339933;">/*</span> r5 ← <span style="color: #ff0000;">3</span> <span style="color: #339933;">*/</span>
    mvn r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">16</span>                   <span style="color: #339933;">/*</span> r5 ← ~<span style="color: #009900; font-weight: bold;">(</span>r5 &lt;&lt; <span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    fmrx r4<span style="color: #339933;">,</span> fpscr                        <span style="color: #339933;">/*</span> r4 ← fpscr <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">and</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> r5                        <span style="color: #339933;">/*</span> r4 ← r4 &amp; r5 <span style="color: #339933;">*/</span>
    fmxr fpscr<span style="color: #339933;">,</span> r4                        <span style="color: #339933;">/*</span> fpscr ← r4 <span style="color: #339933;">*/</span>
&nbsp;
    vpop <span style="color: #009900; font-weight: bold;">{</span>s24<span style="color: #339933;">-</span>s27<span style="color: #009900; font-weight: bold;">}</span>                <span style="color: #339933;">/*</span> Restore preserved floating registers <span style="color: #339933;">*/</span>
    vpop <span style="color: #009900; font-weight: bold;">{</span>s16<span style="color: #339933;">-</span>s19<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> r7<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>  <span style="color: #339933;">/*</span> Restore integer registers <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Leave</span> function <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">naive_vectorial_matmul_4x4:
    /* r0 address of A
       r1 address of B
       r2 address of C
    */
    push {r4, r5, r6, r7, r8, lr} /* Keep integer registers */
    vpush {s16-s19}               /* Floating point registers starting from s16 must be preserved */
    vpush {s24-s27}
    /* First zero 16 single floating point */
    /* In IEEE 754, all bits cleared means 0 */
    mov r4, r2
    mov r5, #16
    mov r6, #0
    b .L1_loop_init_test
    .L1_loop_init :
      str r6, [r4], +#4   /* *r4 ← r6 then r4 ← r4 + 4 */
    .L1_loop_init_test:
      subs r5, r5, #1
      bge .L1_loop_init

    /* Set the LEN field of FPSCR to be 4 (value 3) */
    mov r5, #0b011                        /* r5 ← 3 */
    mov r5, r5, LSL #16                   /* r5 ← r5 &lt;&lt; 16 */
    fmrx r4, fpscr                        /* r4 ← fpscr */
    orr r4, r4, r5                        /* r4 ← r4 | r5 */
    fmxr fpscr, r4                        /* fpscr ← r4 */

    /* We will use 
           r4 as i
           r5 as j
           r6 as k
    */
    mov r4, #0 /* r4 ← 0 */
    .L1_loop_i:  /* loop header of i */
      cmp r4, #4  /* if r4 == 4 goto end of the loop i */
      beq .L1_end_loop_i
      mov r5, #0  /* r5 ← 0 */
      .L1_loop_j: /* loop header of j */
       cmp r5, #4 /* if r5 == 4 goto end of the loop j */
        beq .L1_end_loop_j
        /* Compute the address of C[i][j] and load it into s0 */
        /* Address of C[i][j] is C + 4*(4 * i + j) */
        mov r7, r5               /* r7 ← r5. This is r7 ← j */
        adds r7, r7, r4, LSL #2  /* r7 ← r7 + (r4 &lt;&lt; 2). 
                                    This is r7 ← j + i * 4.
                                    We multiply i by the row size (4 elements) */
        adds r7, r2, r7, LSL #2  /* r7 ← r2 + (r7 &lt;&lt; 2).
                                    This is r7 ← C + 4*(j + i * 4)
                                    We multiply (j + i * 4) by the size of the element.
                                    A single-precision floating point takes 4 bytes.
                                    */
        /* Compute the address of a[i][0] */
        mov r8, r4, LSL #2
        adds r8, r0, r8, LSL #2
        vldmia r8, {s8-s11}  /* Load {s8,s9,s10,s11} ← {a[i][0], a[i][1], a[i][2], a[i][3]} */

        /* Compute the address of b[0][j] */
        mov r8, r5               /* r8 ← r5. This is r8 ← j */
        adds r8, r1, r8, LSL #2  /* r8 ← r1 + (r8 &lt;&lt; 2). This is r8 ← b + 4*(j) */
        vldr s16, [r8]             /* s16 ← *r8. This is s16 ← b[0][j] */
        vldr s17, [r8, #16]        /* s17 ← *(r8 + 16). This is s17 ← b[1][j] */
        vldr s18, [r8, #32]        /* s18 ← *(r8 + 32). This is s17 ← b[2][j] */
        vldr s19, [r8, #48]        /* s19 ← *(r8 + 48). This is s17 ← b[3][j] */

        vmul.f32 s24, s8, s16      /* {s24,s25,s26,s27} ← {s8,s9,s10,s11} * {s16,s17,s18,s19} */
        vmov.f32 s0, s24           /* s0 ← s24 */
        vadd.f32 s0, s0, s25       /* s0 ← s0 + s25 */
        vadd.f32 s0, s0, s26       /* s0 ← s0 + s26 */
        vadd.f32 s0, s0, s27       /* s0 ← s0 + s27 */

        vstr s0, [r7]            /* Store s0 back to C[i][j] */
        add r5, r5, #1  /* r5 ← r5 + 1 */
        b .L1_loop_j /* next iteration of loop j */
       .L1_end_loop_j: /* Here ends loop j */
       add r4, r4, #1 /* r4 ← r4 + 1 */
       b .L1_loop_i     /* next iteration of loop i */
    .L1_end_loop_i: /* Here ends loop i */

    /* Set the LEN field of FPSCR back to 1 (value 0) */
    mov r5, #0b011                        /* r5 ← 3 */
    mvn r5, r5, LSL #16                   /* r5 ← ~(r5 &lt;&lt; 16) */
    fmrx r4, fpscr                        /* r4 ← fpscr */
    and r4, r4, r5                        /* r4 ← r4 &amp; r5 */
    fmxr fpscr, r4                        /* fpscr ← r4 */

    vpop {s24-s27}                /* Restore preserved floating registers */
    vpop {s16-s19}
    pop {r4, r5, r6, r7, r8, lr}  /* Restore integer registers */
    bx lr /* Leave function */</p></div>

<p>
With this approach we can entirely remove the loop <code>k</code>, as we do 4 operations at once. Note that we have to modify <code>fpscr</code> so the field <code>len</code> is set to 4 (and restore it back to 1 when leaving the function).
</p>
<h3>Fill the registers</h3>
<p>
In the previous version we are not exploiting all the registers of VFPv2. Each rows takes 4 registers and so does each column, so we end using only 8 registers plus 4 for the result and one in the bank 0 for the summation. We got rid the loop <code>k</code> to process <code>C[i][j]</code> at once. What if we processed <code>C[i][j]</code> and <code>C[i][j+1]</code> at the same time? This way we can fill all the 8 registers in each bank.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
21
22
23
24
25
26
27
28
29
30
31
32
33
34
35
36
37
38
39
40
41
42
43
44
45
46
47
48
49
50
51
52
53
54
55
56
57
58
59
60
61
62
63
64
65
66
67
68
69
70
71
72
73
74
75
76
77
78
79
80
81
82
83
84
85
86
87
88
89
90
91
92
93
94
95
96
97
98
99
100
101
102
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">naive_vectorial_matmul_2_4x4<span style="color: #339933;">:</span>
    <span style="color: #339933;">/*</span> r0 address of A
       r1 address of B
       r2 address of C
    <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> r7<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">/*</span> Keep integer registers <span style="color: #339933;">*/</span>
    vpush <span style="color: #009900; font-weight: bold;">{</span>s16<span style="color: #339933;">-</span>s31<span style="color: #009900; font-weight: bold;">}</span>               <span style="color: #339933;">/*</span> Floating point registers starting from s16 must be preserved <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> First zero <span style="color: #ff0000;">16</span> single floating point <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">In</span> IEEE <span style="color: #ff0000;">754</span><span style="color: #339933;">,</span> all <span style="color: #0000ff; font-weight: bold;">bits</span> cleared means <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> r2
    <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">16</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r6<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>
    b <span style="color: #339933;">.</span>L2_loop_init_test
    <span style="color: #339933;">.</span>L2_loop_init <span style="color: #339933;">:</span>
      <span style="color: #00007f; font-weight: bold;">str</span> r6<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> <span style="color: #339933;">+</span>#<span style="color: #ff0000;">4</span>   <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span>r4 ← r6 then r4 ← r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span> <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>L2_loop_init_test<span style="color: #339933;">:</span>
      subs r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>
      bge <span style="color: #339933;">.</span>L2_loop_init
&nbsp;
    <span style="color: #339933;">/*</span> Set the LEN field of FPSCR to be <span style="color: #ff0000;">4</span> <span style="color: #009900; font-weight: bold;">(</span>value <span style="color: #ff0000;">3</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> #0b011                        <span style="color: #339933;">/*</span> r5 ← <span style="color: #ff0000;">3</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">16</span>                   <span style="color: #339933;">/*</span> r5 ← r5 &lt;&lt; <span style="color: #ff0000;">16</span> <span style="color: #339933;">*/</span>
    fmrx r4<span style="color: #339933;">,</span> fpscr                        <span style="color: #339933;">/*</span> r4 ← fpscr <span style="color: #339933;">*/</span>
    orr r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> r5                        <span style="color: #339933;">/*</span> r4 ← r4 | r5 <span style="color: #339933;">*/</span>
    fmxr fpscr<span style="color: #339933;">,</span> r4                        <span style="color: #339933;">/*</span> fpscr ← r4 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> We will use 
           r4 as i
           r5 as j
    <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span> <span style="color: #339933;">/*</span> r4 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>L2_loop_i<span style="color: #339933;">:</span>  <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">loop</span> header of i <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">cmp</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> if r4 == <span style="color: #ff0000;">4</span> goto end of the <span style="color: #00007f; font-weight: bold;">loop</span> i <span style="color: #339933;">*/</span>
      beq <span style="color: #339933;">.</span>L2_end_loop_i
      <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>  <span style="color: #339933;">/*</span> r5 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
      <span style="color: #339933;">.</span>L2_loop_j<span style="color: #339933;">:</span> <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">loop</span> header of j <span style="color: #339933;">*/</span>
       <span style="color: #00007f; font-weight: bold;">cmp</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span> <span style="color: #339933;">/*</span> if r5 == <span style="color: #ff0000;">4</span> goto end of the <span style="color: #00007f; font-weight: bold;">loop</span> j <span style="color: #339933;">*/</span>
        beq <span style="color: #339933;">.</span>L2_end_loop_j
        <span style="color: #339933;">/*</span> Compute the address of C<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>j<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #00007f; font-weight: bold;">and</span> load it <span style="color: #00007f; font-weight: bold;">into</span> s0 <span style="color: #339933;">*/</span>
        <span style="color: #339933;">/*</span> Address of C<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>j<span style="color: #009900; font-weight: bold;">]</span> is C <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #ff0000;">4</span> <span style="color: #339933;">*</span> i <span style="color: #339933;">+</span> j<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">mov</span> r7<span style="color: #339933;">,</span> r5               <span style="color: #339933;">/*</span> r7 ← r5<span style="color: #339933;">.</span> This is r7 ← j <span style="color: #339933;">*/</span>
        adds r7<span style="color: #339933;">,</span> r7<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span>  <span style="color: #339933;">/*</span> r7 ← r7 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span>r4 &lt;&lt; <span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> 
                                    This is r7 ← j <span style="color: #339933;">+</span> i <span style="color: #339933;">*</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">.</span>
                                    We multiply i by the row size <span style="color: #009900; font-weight: bold;">(</span><span style="color: #ff0000;">4</span> elements<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
        adds r7<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r7<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span>  <span style="color: #339933;">/*</span> r7 ← r2 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span>r7 &lt;&lt; <span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span>
                                    This is r7 ← C <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>j <span style="color: #339933;">+</span> i <span style="color: #339933;">*</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span>
                                    We multiply <span style="color: #009900; font-weight: bold;">(</span>j <span style="color: #339933;">+</span> i <span style="color: #339933;">*</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span> by the size of the element<span style="color: #339933;">.</span>
                                    A single<span style="color: #339933;">-</span>precision floating point takes <span style="color: #ff0000;">4</span> bytes<span style="color: #339933;">.</span>
                                    <span style="color: #339933;">*/</span>
        <span style="color: #339933;">/*</span> Compute the address of a<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">mov</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span>
        adds <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span>
        vldmia <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">{</span>s8<span style="color: #339933;">-</span>s11<span style="color: #009900; font-weight: bold;">}</span>  <span style="color: #339933;">/*</span> Load <span style="color: #009900; font-weight: bold;">{</span>s8<span style="color: #339933;">,</span>s9<span style="color: #339933;">,</span>s10<span style="color: #339933;">,</span>s11<span style="color: #009900; font-weight: bold;">}</span> ← <span style="color: #009900; font-weight: bold;">{</span>a<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> a<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> a<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> a<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">3</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">*/</span>
&nbsp;
        <span style="color: #339933;">/*</span> Compute the address of b<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>j<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">mov</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r5               <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r8</span> ← r5<span style="color: #339933;">.</span> This is <span style="color: #46aa03; font-weight: bold;">r8</span> ← j <span style="color: #339933;">*/</span>
        adds <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span>  <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r8</span> ← r1 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span><span style="color: #46aa03; font-weight: bold;">r8</span> &lt;&lt; <span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> This is <span style="color: #46aa03; font-weight: bold;">r8</span> ← b <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>j<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
        vldr s16<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #009900; font-weight: bold;">]</span>             <span style="color: #339933;">/*</span> s16 ← <span style="color: #339933;">*</span><span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">.</span> This is s16 ← b<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>j<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
        vldr s17<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">]</span>        <span style="color: #339933;">/*</span> s17 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #46aa03; font-weight: bold;">r8</span> <span style="color: #339933;">+</span> <span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> This is s17 ← b<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>j<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
        vldr s18<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">32</span><span style="color: #009900; font-weight: bold;">]</span>        <span style="color: #339933;">/*</span> s18 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #46aa03; font-weight: bold;">r8</span> <span style="color: #339933;">+</span> <span style="color: #ff0000;">32</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> This is s17 ← b<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>j<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
        vldr s19<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">48</span><span style="color: #009900; font-weight: bold;">]</span>        <span style="color: #339933;">/*</span> s19 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #46aa03; font-weight: bold;">r8</span> <span style="color: #339933;">+</span> <span style="color: #ff0000;">48</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> This is s17 ← b<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">3</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>j<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
&nbsp;
        <span style="color: #339933;">/*</span> Compute the address of b<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>j<span style="color: #339933;">+</span><span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">add</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>             <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r8</span> ← r5 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span><span style="color: #339933;">.</span> This is <span style="color: #46aa03; font-weight: bold;">r8</span> ← j <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span><span style="color: #339933;">*/</span>
        adds <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span>    <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r8</span> ← r1 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span><span style="color: #46aa03; font-weight: bold;">r8</span> &lt;&lt; <span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> This is <span style="color: #46aa03; font-weight: bold;">r8</span> ← b <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>j <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
        vldr s20<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #009900; font-weight: bold;">]</span>             <span style="color: #339933;">/*</span> s20 ← <span style="color: #339933;">*</span><span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">.</span> This is s20 ← b<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>j <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
        vldr s21<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">]</span>        <span style="color: #339933;">/*</span> s21 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #46aa03; font-weight: bold;">r8</span> <span style="color: #339933;">+</span> <span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> This is s21 ← b<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>j <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
        vldr s22<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">32</span><span style="color: #009900; font-weight: bold;">]</span>        <span style="color: #339933;">/*</span> s22 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #46aa03; font-weight: bold;">r8</span> <span style="color: #339933;">+</span> <span style="color: #ff0000;">32</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> This is s22 ← b<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>j <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
        vldr s23<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">48</span><span style="color: #009900; font-weight: bold;">]</span>        <span style="color: #339933;">/*</span> s23 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #46aa03; font-weight: bold;">r8</span> <span style="color: #339933;">+</span> <span style="color: #ff0000;">48</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> This is s23 ← b<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">3</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>j <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
&nbsp;
        vmul<span style="color: #339933;">.</span>f32 s24<span style="color: #339933;">,</span> s8<span style="color: #339933;">,</span> s16      <span style="color: #339933;">/*</span> <span style="color: #009900; font-weight: bold;">{</span>s24<span style="color: #339933;">,</span>s25<span style="color: #339933;">,</span>s26<span style="color: #339933;">,</span>s27<span style="color: #009900; font-weight: bold;">}</span> ← <span style="color: #009900; font-weight: bold;">{</span>s8<span style="color: #339933;">,</span>s9<span style="color: #339933;">,</span>s10<span style="color: #339933;">,</span>s11<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">*</span> <span style="color: #009900; font-weight: bold;">{</span>s16<span style="color: #339933;">,</span>s17<span style="color: #339933;">,</span>s18<span style="color: #339933;">,</span>s19<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">*/</span>
        vmov<span style="color: #339933;">.</span>f32 s0<span style="color: #339933;">,</span> s24           <span style="color: #339933;">/*</span> s0 ← s24 <span style="color: #339933;">*/</span>
        vadd<span style="color: #339933;">.</span>f32 s0<span style="color: #339933;">,</span> s0<span style="color: #339933;">,</span> s25       <span style="color: #339933;">/*</span> s0 ← s0 <span style="color: #339933;">+</span> s25 <span style="color: #339933;">*/</span>
        vadd<span style="color: #339933;">.</span>f32 s0<span style="color: #339933;">,</span> s0<span style="color: #339933;">,</span> s26       <span style="color: #339933;">/*</span> s0 ← s0 <span style="color: #339933;">+</span> s26 <span style="color: #339933;">*/</span>
        vadd<span style="color: #339933;">.</span>f32 s0<span style="color: #339933;">,</span> s0<span style="color: #339933;">,</span> s27       <span style="color: #339933;">/*</span> s0 ← s0 <span style="color: #339933;">+</span> s27 <span style="color: #339933;">*/</span>
&nbsp;
        vmul<span style="color: #339933;">.</span>f32 s28<span style="color: #339933;">,</span> s8<span style="color: #339933;">,</span> s20      <span style="color: #339933;">/*</span> <span style="color: #009900; font-weight: bold;">{</span>s28<span style="color: #339933;">,</span>s29<span style="color: #339933;">,</span>s30<span style="color: #339933;">,</span>s31<span style="color: #009900; font-weight: bold;">}</span> ← <span style="color: #009900; font-weight: bold;">{</span>s8<span style="color: #339933;">,</span>s9<span style="color: #339933;">,</span>s10<span style="color: #339933;">,</span>s11<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">*</span> <span style="color: #009900; font-weight: bold;">{</span>s20<span style="color: #339933;">,</span>s21<span style="color: #339933;">,</span>s22<span style="color: #339933;">,</span>s23<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">*/</span>
&nbsp;
        vmov<span style="color: #339933;">.</span>f32 s1<span style="color: #339933;">,</span> s28           <span style="color: #339933;">/*</span> s1 ← s28 <span style="color: #339933;">*/</span>
        vadd<span style="color: #339933;">.</span>f32 s1<span style="color: #339933;">,</span> s1<span style="color: #339933;">,</span> s29       <span style="color: #339933;">/*</span> s1 ← s1 <span style="color: #339933;">+</span> s29 <span style="color: #339933;">*/</span>
        vadd<span style="color: #339933;">.</span>f32 s1<span style="color: #339933;">,</span> s1<span style="color: #339933;">,</span> s30       <span style="color: #339933;">/*</span> s1 ← s1 <span style="color: #339933;">+</span> s30 <span style="color: #339933;">*/</span>
        vadd<span style="color: #339933;">.</span>f32 s1<span style="color: #339933;">,</span> s1<span style="color: #339933;">,</span> s31       <span style="color: #339933;">/*</span> s1 ← s1 <span style="color: #339933;">+</span> s31 <span style="color: #339933;">*/</span>
&nbsp;
        vstmia r7<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">{</span>s0<span style="color: #339933;">-</span>s1<span style="color: #009900; font-weight: bold;">}</span>         <span style="color: #339933;">/*</span> <span style="color: #009900; font-weight: bold;">{</span>C<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>j<span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> C<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>j<span style="color: #339933;">+</span><span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">}</span> ← <span style="color: #009900; font-weight: bold;">{</span>s0<span style="color: #339933;">,</span> s1<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">*/</span>
&nbsp;
        <span style="color: #00007f; font-weight: bold;">add</span> r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span>  <span style="color: #339933;">/*</span> r5 ← r5 <span style="color: #339933;">+</span> <span style="color: #ff0000;">2</span> <span style="color: #339933;">*/</span>
        b <span style="color: #339933;">.</span>L2_loop_j <span style="color: #339933;">/*</span> next iteration of <span style="color: #00007f; font-weight: bold;">loop</span> j <span style="color: #339933;">*/</span>
       <span style="color: #339933;">.</span>L2_end_loop_j<span style="color: #339933;">:</span> <span style="color: #339933;">/*</span> Here ends <span style="color: #00007f; font-weight: bold;">loop</span> j <span style="color: #339933;">*/</span>
       <span style="color: #00007f; font-weight: bold;">add</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span> <span style="color: #339933;">/*</span> r4 ← r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
       b <span style="color: #339933;">.</span>L2_loop_i     <span style="color: #339933;">/*</span> next iteration of <span style="color: #00007f; font-weight: bold;">loop</span> i <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>L2_end_loop_i<span style="color: #339933;">:</span> <span style="color: #339933;">/*</span> Here ends <span style="color: #00007f; font-weight: bold;">loop</span> i <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> Set the LEN field of FPSCR back to <span style="color: #ff0000;">1</span> <span style="color: #009900; font-weight: bold;">(</span>value <span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> #0b011                        <span style="color: #339933;">/*</span> r5 ← <span style="color: #ff0000;">3</span> <span style="color: #339933;">*/</span>
    mvn r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">16</span>                   <span style="color: #339933;">/*</span> r5 ← ~<span style="color: #009900; font-weight: bold;">(</span>r5 &lt;&lt; <span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    fmrx r4<span style="color: #339933;">,</span> fpscr                        <span style="color: #339933;">/*</span> r4 ← fpscr <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">and</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> r5                        <span style="color: #339933;">/*</span> r4 ← r4 &amp; r5 <span style="color: #339933;">*/</span>
    fmxr fpscr<span style="color: #339933;">,</span> r4                        <span style="color: #339933;">/*</span> fpscr ← r4 <span style="color: #339933;">*/</span>
&nbsp;
    vpop <span style="color: #009900; font-weight: bold;">{</span>s16<span style="color: #339933;">-</span>s31<span style="color: #009900; font-weight: bold;">}</span>                <span style="color: #339933;">/*</span> Restore preserved floating registers <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> r7<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>  <span style="color: #339933;">/*</span> Restore integer registers <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Leave</span> function <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">naive_vectorial_matmul_2_4x4:
    /* r0 address of A
       r1 address of B
       r2 address of C
    */
    push {r4, r5, r6, r7, r8, lr} /* Keep integer registers */
    vpush {s16-s31}               /* Floating point registers starting from s16 must be preserved */
    /* First zero 16 single floating point */
    /* In IEEE 754, all bits cleared means 0 */
    mov r4, r2
    mov r5, #16
    mov r6, #0
    b .L2_loop_init_test
    .L2_loop_init :
      str r6, [r4], +#4   /* *r4 ← r6 then r4 ← r4 + 4 */
    .L2_loop_init_test:
      subs r5, r5, #1
      bge .L2_loop_init

    /* Set the LEN field of FPSCR to be 4 (value 3) */
    mov r5, #0b011                        /* r5 ← 3 */
    mov r5, r5, LSL #16                   /* r5 ← r5 &lt;&lt; 16 */
    fmrx r4, fpscr                        /* r4 ← fpscr */
    orr r4, r4, r5                        /* r4 ← r4 | r5 */
    fmxr fpscr, r4                        /* fpscr ← r4 */

    /* We will use 
           r4 as i
           r5 as j
    */
    mov r4, #0 /* r4 ← 0 */
    .L2_loop_i:  /* loop header of i */
      cmp r4, #4  /* if r4 == 4 goto end of the loop i */
      beq .L2_end_loop_i
      mov r5, #0  /* r5 ← 0 */
      .L2_loop_j: /* loop header of j */
       cmp r5, #4 /* if r5 == 4 goto end of the loop j */
        beq .L2_end_loop_j
        /* Compute the address of C[i][j] and load it into s0 */
        /* Address of C[i][j] is C + 4*(4 * i + j) */
        mov r7, r5               /* r7 ← r5. This is r7 ← j */
        adds r7, r7, r4, LSL #2  /* r7 ← r7 + (r4 &lt;&lt; 2). 
                                    This is r7 ← j + i * 4.
                                    We multiply i by the row size (4 elements) */
        adds r7, r2, r7, LSL #2  /* r7 ← r2 + (r7 &lt;&lt; 2).
                                    This is r7 ← C + 4*(j + i * 4)
                                    We multiply (j + i * 4) by the size of the element.
                                    A single-precision floating point takes 4 bytes.
                                    */
        /* Compute the address of a[i][0] */
        mov r8, r4, LSL #2
        adds r8, r0, r8, LSL #2
        vldmia r8, {s8-s11}  /* Load {s8,s9,s10,s11} ← {a[i][0], a[i][1], a[i][2], a[i][3]} */

        /* Compute the address of b[0][j] */
        mov r8, r5               /* r8 ← r5. This is r8 ← j */
        adds r8, r1, r8, LSL #2  /* r8 ← r1 + (r8 &lt;&lt; 2). This is r8 ← b + 4*(j) */
        vldr s16, [r8]             /* s16 ← *r8. This is s16 ← b[0][j] */
        vldr s17, [r8, #16]        /* s17 ← *(r8 + 16). This is s17 ← b[1][j] */
        vldr s18, [r8, #32]        /* s18 ← *(r8 + 32). This is s17 ← b[2][j] */
        vldr s19, [r8, #48]        /* s19 ← *(r8 + 48). This is s17 ← b[3][j] */

        /* Compute the address of b[0][j+1] */
        add r8, r5, #1             /* r8 ← r5 + 1. This is r8 ← j + 1*/
        adds r8, r1, r8, LSL #2    /* r8 ← r1 + (r8 &lt;&lt; 2). This is r8 ← b + 4*(j + 1) */
        vldr s20, [r8]             /* s20 ← *r8. This is s20 ← b[0][j + 1] */
        vldr s21, [r8, #16]        /* s21 ← *(r8 + 16). This is s21 ← b[1][j + 1] */
        vldr s22, [r8, #32]        /* s22 ← *(r8 + 32). This is s22 ← b[2][j + 1] */
        vldr s23, [r8, #48]        /* s23 ← *(r8 + 48). This is s23 ← b[3][j + 1] */

        vmul.f32 s24, s8, s16      /* {s24,s25,s26,s27} ← {s8,s9,s10,s11} * {s16,s17,s18,s19} */
        vmov.f32 s0, s24           /* s0 ← s24 */
        vadd.f32 s0, s0, s25       /* s0 ← s0 + s25 */
        vadd.f32 s0, s0, s26       /* s0 ← s0 + s26 */
        vadd.f32 s0, s0, s27       /* s0 ← s0 + s27 */

        vmul.f32 s28, s8, s20      /* {s28,s29,s30,s31} ← {s8,s9,s10,s11} * {s20,s21,s22,s23} */

        vmov.f32 s1, s28           /* s1 ← s28 */
        vadd.f32 s1, s1, s29       /* s1 ← s1 + s29 */
        vadd.f32 s1, s1, s30       /* s1 ← s1 + s30 */
        vadd.f32 s1, s1, s31       /* s1 ← s1 + s31 */

        vstmia r7, {s0-s1}         /* {C[i][j], C[i][j+1]} ← {s0, s1} */

        add r5, r5, #2  /* r5 ← r5 + 2 */
        b .L2_loop_j /* next iteration of loop j */
       .L2_end_loop_j: /* Here ends loop j */
       add r4, r4, #1 /* r4 ← r4 + 1 */
       b .L2_loop_i     /* next iteration of loop i */
    .L2_end_loop_i: /* Here ends loop i */

    /* Set the LEN field of FPSCR back to 1 (value 0) */
    mov r5, #0b011                        /* r5 ← 3 */
    mvn r5, r5, LSL #16                   /* r5 ← ~(r5 &lt;&lt; 16) */
    fmrx r4, fpscr                        /* r4 ← fpscr */
    and r4, r4, r5                        /* r4 ← r4 &amp; r5 */
    fmxr fpscr, r4                        /* fpscr ← r4 */

    vpop {s16-s31}                /* Restore preserved floating registers */
    pop {r4, r5, r6, r7, r8, lr}  /* Restore integer registers */
    bx lr /* Leave function */</p></div>

<p>
Note that because we now process <code>j</code> and <code>j + 1</code>, <code>r5</code> (<code>j</code>) is now increased by 2 at the end of the loop. This is usually known as <em>loop unrolling</em> and it is always legal to do. We do more than one iteration of the original loop in the unrolled loop. The amount of iterations of the original loop we do in the unrolled loop is the <em>unroll factor</em>. In this case since the number of iterations (4) perfectly divides the unrolling factor (2) we do not need an extra loop for the remainder iterations (the remainder loop has one less iteration than the value of the unrolling factor).
</p>
<p>
As you can see, the accesses to <code>b[k][j]</code> and <code>b[k][j+1]</code> are starting to become tedious. Maybe we should change a bit more the matrix multiply algorithm.
</p>
<h2>Reorder the accesses</h2>
<p>
Is there a way we can mitigate the strided accesses to the matrix B? Yes, there is one, we only have to permute the loop nest i, j, k into the loop nest k, i, j. Now you may be wondering if this is legal. Well, checking for the legality of these things is beyond the scope of this post so you will have to trust me here. Such permutation is fine. What does this mean? Well, it means that our algorithm will now look like this.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>1
2
3
4
5
6
7
8
9
10
11
12
13
</pre></td><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #993333;">float</span> A<span style="color: #009900;">[</span>N<span style="color: #009900;">]</span><span style="color: #009900;">[</span>N<span style="color: #009900;">]</span><span style="color: #339933;">;</span>
<span style="color: #993333;">float</span> B<span style="color: #009900;">[</span>M<span style="color: #009900;">]</span><span style="color: #009900;">[</span>N<span style="color: #009900;">]</span><span style="color: #339933;">;</span>
<span style="color: #666666; font-style: italic;">// Result</span>
<span style="color: #993333;">float</span> C<span style="color: #009900;">[</span>N<span style="color: #009900;">]</span><span style="color: #009900;">[</span>N<span style="color: #009900;">]</span><span style="color: #339933;">;</span>
&nbsp;
<span style="color: #b1b100;">for</span> <span style="color: #009900;">(</span><span style="color: #993333;">int</span> i <span style="color: #339933;">=</span> <span style="color: #0000dd;">0</span><span style="color: #339933;">;</span> i <span style="color: #339933;">&lt;</span> N<span style="color: #339933;">;</span> i<span style="color: #339933;">++</span><span style="color: #009900;">)</span>
  <span style="color: #b1b100;">for</span> <span style="color: #009900;">(</span><span style="color: #993333;">int</span> j <span style="color: #339933;">=</span> <span style="color: #0000dd;">0</span><span style="color: #339933;">;</span> j <span style="color: #339933;">&lt;</span> N<span style="color: #339933;">;</span> j<span style="color: #339933;">++</span><span style="color: #009900;">)</span>
    C<span style="color: #009900;">[</span>i<span style="color: #009900;">]</span><span style="color: #009900;">[</span>j<span style="color: #009900;">]</span> <span style="color: #339933;">=</span> <span style="color: #0000dd;">0</span><span style="color: #339933;">;</span>
&nbsp;
<span style="color: #b1b100;">for</span> <span style="color: #009900;">(</span><span style="color: #993333;">int</span> k <span style="color: #339933;">=</span> <span style="color: #0000dd;">0</span><span style="color: #339933;">;</span> k <span style="color: #339933;">&lt;</span> N<span style="color: #339933;">;</span> k<span style="color: #339933;">++</span><span style="color: #009900;">)</span>
  <span style="color: #b1b100;">for</span> <span style="color: #009900;">(</span><span style="color: #993333;">int</span> i <span style="color: #339933;">=</span> <span style="color: #0000dd;">0</span><span style="color: #339933;">;</span> i <span style="color: #339933;">&lt;</span> N<span style="color: #339933;">;</span> i<span style="color: #339933;">++</span><span style="color: #009900;">)</span>
    <span style="color: #b1b100;">for</span> <span style="color: #009900;">(</span><span style="color: #993333;">int</span> j <span style="color: #339933;">=</span> <span style="color: #0000dd;">0</span><span style="color: #339933;">;</span> j <span style="color: #339933;">&lt;</span> N<span style="color: #339933;">;</span> j<span style="color: #339933;">++</span><span style="color: #009900;">)</span>
       C<span style="color: #009900;">[</span>i<span style="color: #009900;">]</span><span style="color: #009900;">[</span>j<span style="color: #009900;">]</span> <span style="color: #339933;">+=</span> A<span style="color: #009900;">[</span>i<span style="color: #009900;">]</span><span style="color: #009900;">[</span>k<span style="color: #009900;">]</span> <span style="color: #339933;">*</span> B<span style="color: #009900;">[</span>k<span style="color: #009900;">]</span><span style="color: #009900;">[</span>j<span style="color: #009900;">]</span><span style="color: #339933;">;</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">float A[N][N];
float B[M][N];
// Result
float C[N][N];

for (int i = 0; i &lt; N; i++)
  for (int j = 0; j &lt; N; j++)
    C[i][j] = 0;

for (int k = 0; k &lt; N; k++)
  for (int i = 0; i &lt; N; i++)
    for (int j = 0; j &lt; N; j++)
       C[i][j] += A[i][k] * B[k][j];</p></div>

<p>
This may seem not very useful, but note that, since now k is in the outermost loop, now it is easier to use vectorial instructions.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #b1b100;">for</span> <span style="color: #009900;">(</span><span style="color: #993333;">int</span> k <span style="color: #339933;">=</span> <span style="color: #0000dd;">0</span><span style="color: #339933;">;</span> k <span style="color: #339933;">&lt;</span> N<span style="color: #339933;">;</span> k<span style="color: #339933;">++</span><span style="color: #009900;">)</span>
  <span style="color: #b1b100;">for</span> <span style="color: #009900;">(</span><span style="color: #993333;">int</span> i <span style="color: #339933;">=</span> <span style="color: #0000dd;">0</span><span style="color: #339933;">;</span> i <span style="color: #339933;">&lt;</span> N<span style="color: #339933;">;</span> i<span style="color: #339933;">++</span><span style="color: #009900;">)</span>
  <span style="color: #009900;">{</span>
     C<span style="color: #009900;">[</span>i<span style="color: #009900;">]</span><span style="color: #009900;">[</span><span style="color: #0000dd;">0</span><span style="color: #009900;">]</span> <span style="color: #339933;">+=</span> A<span style="color: #009900;">[</span>i<span style="color: #009900;">]</span><span style="color: #009900;">[</span>k<span style="color: #009900;">]</span> <span style="color: #339933;">*</span> B<span style="color: #009900;">[</span>k<span style="color: #009900;">]</span><span style="color: #009900;">[</span><span style="color: #0000dd;">0</span><span style="color: #009900;">]</span><span style="color: #339933;">;</span>
     C<span style="color: #009900;">[</span>i<span style="color: #009900;">]</span><span style="color: #009900;">[</span><span style="color: #0000dd;">1</span><span style="color: #009900;">]</span> <span style="color: #339933;">+=</span> A<span style="color: #009900;">[</span>i<span style="color: #009900;">]</span><span style="color: #009900;">[</span>k<span style="color: #009900;">]</span> <span style="color: #339933;">*</span> B<span style="color: #009900;">[</span>k<span style="color: #009900;">]</span><span style="color: #009900;">[</span><span style="color: #0000dd;">1</span><span style="color: #009900;">]</span><span style="color: #339933;">;</span>
     C<span style="color: #009900;">[</span>i<span style="color: #009900;">]</span><span style="color: #009900;">[</span><span style="color: #0000dd;">2</span><span style="color: #009900;">]</span> <span style="color: #339933;">+=</span> A<span style="color: #009900;">[</span>i<span style="color: #009900;">]</span><span style="color: #009900;">[</span>k<span style="color: #009900;">]</span> <span style="color: #339933;">*</span> B<span style="color: #009900;">[</span>k<span style="color: #009900;">]</span><span style="color: #009900;">[</span><span style="color: #0000dd;">2</span><span style="color: #009900;">]</span><span style="color: #339933;">;</span>
     C<span style="color: #009900;">[</span>i<span style="color: #009900;">]</span><span style="color: #009900;">[</span><span style="color: #0000dd;">3</span><span style="color: #009900;">]</span> <span style="color: #339933;">+=</span> A<span style="color: #009900;">[</span>i<span style="color: #009900;">]</span><span style="color: #009900;">[</span>k<span style="color: #009900;">]</span> <span style="color: #339933;">*</span> B<span style="color: #009900;">[</span>k<span style="color: #009900;">]</span><span style="color: #009900;">[</span><span style="color: #0000dd;">3</span><span style="color: #009900;">]</span><span style="color: #339933;">;</span>
  <span style="color: #009900;">}</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">for (int k = 0; k &lt; N; k++)
  for (int i = 0; i &lt; N; i++)
  {
     C[i][0] += A[i][k] * B[k][0];
     C[i][1] += A[i][k] * B[k][1];
     C[i][2] += A[i][k] * B[k][2];
     C[i][3] += A[i][k] * B[k][3];
  }</p></div>

<p>
If you remember the chapter 13, VFPv2 instructions have a mixed mode when the <code>Rsource2</code> register is in bank 0. This case makes a perfect match: we can load <code>C[i][0..3]</code> and <code>B[k][0..3]</code> with a load multiple and then load <code>A[i][k]</code> in a register of the bank 0. Then we can make multiply<code> A[i][k]*B[k][0..3]</code> and add the result to <code>C[i][0..3]</code>. As a bonus, the number of instructions is much lower.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
21
22
23
24
25
26
27
28
29
30
31
32
33
34
35
36
37
38
39
40
41
42
43
44
45
46
47
48
49
50
51
52
53
54
55
56
57
58
59
60
61
62
63
64
65
66
67
68
69
70
71
72
73
74
75
76
77
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">better_vectorial_matmul_4x4<span style="color: #339933;">:</span>
    <span style="color: #339933;">/*</span> r0 address of A
       r1 address of B
       r2 address of C
    <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> r7<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">/*</span> Keep integer registers <span style="color: #339933;">*/</span>
    vpush <span style="color: #009900; font-weight: bold;">{</span>s16<span style="color: #339933;">-</span>s19<span style="color: #009900; font-weight: bold;">}</span>               <span style="color: #339933;">/*</span> Floating point registers starting from s16 must be preserved <span style="color: #339933;">*/</span>
    vpush <span style="color: #009900; font-weight: bold;">{</span>s24<span style="color: #339933;">-</span>s27<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #339933;">/*</span> First zero <span style="color: #ff0000;">16</span> single floating point <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">In</span> IEEE <span style="color: #ff0000;">754</span><span style="color: #339933;">,</span> all <span style="color: #0000ff; font-weight: bold;">bits</span> cleared means <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> r2
    <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">16</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r6<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>
    b <span style="color: #339933;">.</span>L3_loop_init_test
    <span style="color: #339933;">.</span>L3_loop_init <span style="color: #339933;">:</span>
      <span style="color: #00007f; font-weight: bold;">str</span> r6<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> <span style="color: #339933;">+</span>#<span style="color: #ff0000;">4</span>   <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span>r4 ← r6 then r4 ← r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span> <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>L3_loop_init_test<span style="color: #339933;">:</span>
      subs r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>
      bge <span style="color: #339933;">.</span>L3_loop_init
&nbsp;
    <span style="color: #339933;">/*</span> Set the LEN field of FPSCR to be <span style="color: #ff0000;">4</span> <span style="color: #009900; font-weight: bold;">(</span>value <span style="color: #ff0000;">3</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> #0b011                        <span style="color: #339933;">/*</span> r5 ← <span style="color: #ff0000;">3</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">16</span>                   <span style="color: #339933;">/*</span> r5 ← r5 &lt;&lt; <span style="color: #ff0000;">16</span> <span style="color: #339933;">*/</span>
    fmrx r4<span style="color: #339933;">,</span> fpscr                        <span style="color: #339933;">/*</span> r4 ← fpscr <span style="color: #339933;">*/</span>
    orr r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> r5                        <span style="color: #339933;">/*</span> r4 ← r4 | r5 <span style="color: #339933;">*/</span>
    fmxr fpscr<span style="color: #339933;">,</span> r4                        <span style="color: #339933;">/*</span> fpscr ← r4 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> We will use 
           r4 as k
           r5 as i
    <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span> <span style="color: #339933;">/*</span> r4 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>L3_loop_k<span style="color: #339933;">:</span>  <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">loop</span> header of k <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">cmp</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> if r4 == <span style="color: #ff0000;">4</span> goto end of the <span style="color: #00007f; font-weight: bold;">loop</span> k <span style="color: #339933;">*/</span>
      beq <span style="color: #339933;">.</span>L3_end_loop_k
      <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>  <span style="color: #339933;">/*</span> r5 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
      <span style="color: #339933;">.</span>L3_loop_i<span style="color: #339933;">:</span> <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">loop</span> header of i <span style="color: #339933;">*/</span>
       <span style="color: #00007f; font-weight: bold;">cmp</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span> <span style="color: #339933;">/*</span> if r5 == <span style="color: #ff0000;">4</span> goto end of the <span style="color: #00007f; font-weight: bold;">loop</span> i <span style="color: #339933;">*/</span>
        beq <span style="color: #339933;">.</span>L3_end_loop_i
        <span style="color: #339933;">/*</span> Compute the address of C<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
        <span style="color: #339933;">/*</span> Address of C<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span> is C <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #ff0000;">4</span> <span style="color: #339933;">*</span> i<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">add</span> r7<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">4</span>         <span style="color: #339933;">/*</span> r7 ← r2 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span>r5 &lt;&lt; <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> This is r7 ← c <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span><span style="color: #ff0000;">4</span><span style="color: #339933;">*</span>i <span style="color: #339933;">*/</span>
        vldmia r7<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">{</span>s8<span style="color: #339933;">-</span>s11<span style="color: #009900; font-weight: bold;">}</span>            <span style="color: #339933;">/*</span> Load <span style="color: #009900; font-weight: bold;">{</span>s8<span style="color: #339933;">,</span>s9<span style="color: #339933;">,</span>s10<span style="color: #339933;">,</span>s11<span style="color: #009900; font-weight: bold;">}</span> ← <span style="color: #009900; font-weight: bold;">{</span>c<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> c<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> c<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> c<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">3</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">*/</span>
        <span style="color: #339933;">/*</span> Compute the address of A<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>k<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
        <span style="color: #339933;">/*</span> Address of A<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>k<span style="color: #009900; font-weight: bold;">]</span> is A <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #ff0000;">4</span><span style="color: #339933;">*</span>i <span style="color: #339933;">+</span> k<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">add</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span>         <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r8</span> ← r4 <span style="color: #339933;">+</span> r5 &lt;&lt; <span style="color: #ff0000;">2</span><span style="color: #339933;">.</span> This is <span style="color: #46aa03; font-weight: bold;">r8</span> ← k <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span>i <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">add</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span>         <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r8</span> ← r0 <span style="color: #339933;">+</span> <span style="color: #46aa03; font-weight: bold;">r8</span> &lt;&lt; <span style="color: #ff0000;">2</span><span style="color: #339933;">.</span> This is <span style="color: #46aa03; font-weight: bold;">r8</span> ← a <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>k <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span>i<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
        vldr s0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #009900; font-weight: bold;">]</span>                  <span style="color: #339933;">/*</span> Load s0 ← a<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>k<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
&nbsp;
        <span style="color: #339933;">/*</span> Compute the address of B<span style="color: #009900; font-weight: bold;">[</span>k<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
        <span style="color: #339933;">/*</span> Address of B<span style="color: #009900; font-weight: bold;">[</span>k<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span> is B <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #ff0000;">4</span><span style="color: #339933;">*</span>k<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">add</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">4</span>         <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r8</span> ← r1 <span style="color: #339933;">+</span> r4 &lt;&lt; <span style="color: #ff0000;">4</span><span style="color: #339933;">.</span> This is <span style="color: #46aa03; font-weight: bold;">r8</span> ← b <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #ff0000;">4</span><span style="color: #339933;">*</span>k<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
        vldmia <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">{</span>s16<span style="color: #339933;">-</span>s19<span style="color: #009900; font-weight: bold;">}</span>           <span style="color: #339933;">/*</span> Load <span style="color: #009900; font-weight: bold;">{</span>s16<span style="color: #339933;">,</span>s17<span style="color: #339933;">,</span>s18<span style="color: #339933;">,</span>s19<span style="color: #009900; font-weight: bold;">}</span> ← <span style="color: #009900; font-weight: bold;">{</span>b<span style="color: #009900; font-weight: bold;">[</span>k<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> b<span style="color: #009900; font-weight: bold;">[</span>k<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> b<span style="color: #009900; font-weight: bold;">[</span>k<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> b<span style="color: #009900; font-weight: bold;">[</span>k<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">3</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">*/</span>
&nbsp;
        vmul<span style="color: #339933;">.</span>f32 s24<span style="color: #339933;">,</span> s16<span style="color: #339933;">,</span> s0          <span style="color: #339933;">/*</span> <span style="color: #009900; font-weight: bold;">{</span>s24<span style="color: #339933;">,</span>s25<span style="color: #339933;">,</span>s26<span style="color: #339933;">,</span>s27<span style="color: #009900; font-weight: bold;">}</span> ← <span style="color: #009900; font-weight: bold;">{</span>s16<span style="color: #339933;">,</span>s17<span style="color: #339933;">,</span>s18<span style="color: #339933;">,</span>s19<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">*</span> <span style="color: #009900; font-weight: bold;">{</span>s0<span style="color: #339933;">,</span>s0<span style="color: #339933;">,</span>s0<span style="color: #339933;">,</span>s0<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">*/</span>
        vadd<span style="color: #339933;">.</span>f32 s8<span style="color: #339933;">,</span> s8<span style="color: #339933;">,</span> s24           <span style="color: #339933;">/*</span> <span style="color: #009900; font-weight: bold;">{</span>s8<span style="color: #339933;">,</span>s9<span style="color: #339933;">,</span>s10<span style="color: #339933;">,</span>s11<span style="color: #009900; font-weight: bold;">}</span> ← <span style="color: #009900; font-weight: bold;">{</span>s8<span style="color: #339933;">,</span>s9<span style="color: #339933;">,</span>s10<span style="color: #339933;">,</span>s11<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">{</span>s24<span style="color: #339933;">,</span>s25<span style="color: #339933;">,</span>s26<span style="color: #339933;">,</span>s7<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">*/</span>
&nbsp;
        vstmia r7<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">{</span>s8<span style="color: #339933;">-</span>s11<span style="color: #009900; font-weight: bold;">}</span>            <span style="color: #339933;">/*</span> Store <span style="color: #009900; font-weight: bold;">{</span>c<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> c<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> c<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> c<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">3</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">}</span> ← <span style="color: #009900; font-weight: bold;">{</span>s8<span style="color: #339933;">,</span>s9<span style="color: #339933;">,</span>s10<span style="color: #339933;">,</span>s11<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">*/</span>
&nbsp;
        <span style="color: #00007f; font-weight: bold;">add</span> r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>  <span style="color: #339933;">/*</span> r5 ← r5 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span><span style="color: #339933;">.</span> This is i = i <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
        b <span style="color: #339933;">.</span>L3_loop_i <span style="color: #339933;">/*</span> next iteration of <span style="color: #00007f; font-weight: bold;">loop</span> i <span style="color: #339933;">*/</span>
       <span style="color: #339933;">.</span>L3_end_loop_i<span style="color: #339933;">:</span> <span style="color: #339933;">/*</span> Here ends <span style="color: #00007f; font-weight: bold;">loop</span> i <span style="color: #339933;">*/</span>
       <span style="color: #00007f; font-weight: bold;">add</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span> <span style="color: #339933;">/*</span> r4 ← r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span><span style="color: #339933;">.</span> This is k = k <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
       b <span style="color: #339933;">.</span>L3_loop_k     <span style="color: #339933;">/*</span> next iteration of <span style="color: #00007f; font-weight: bold;">loop</span> k <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>L3_end_loop_k<span style="color: #339933;">:</span> <span style="color: #339933;">/*</span> Here ends <span style="color: #00007f; font-weight: bold;">loop</span> k <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> Set the LEN field of FPSCR back to <span style="color: #ff0000;">1</span> <span style="color: #009900; font-weight: bold;">(</span>value <span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> #0b011                        <span style="color: #339933;">/*</span> r5 ← <span style="color: #ff0000;">3</span> <span style="color: #339933;">*/</span>
    mvn r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">16</span>                   <span style="color: #339933;">/*</span> r5 ← ~<span style="color: #009900; font-weight: bold;">(</span>r5 &lt;&lt; <span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    fmrx r4<span style="color: #339933;">,</span> fpscr                        <span style="color: #339933;">/*</span> r4 ← fpscr <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">and</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> r5                        <span style="color: #339933;">/*</span> r4 ← r4 &amp; r5 <span style="color: #339933;">*/</span>
    fmxr fpscr<span style="color: #339933;">,</span> r4                        <span style="color: #339933;">/*</span> fpscr ← r4 <span style="color: #339933;">*/</span>
&nbsp;
    vpop <span style="color: #009900; font-weight: bold;">{</span>s24<span style="color: #339933;">-</span>s27<span style="color: #009900; font-weight: bold;">}</span>                <span style="color: #339933;">/*</span> Restore preserved floating registers <span style="color: #339933;">*/</span>
    vpop <span style="color: #009900; font-weight: bold;">{</span>s16<span style="color: #339933;">-</span>s19<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> r7<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>  <span style="color: #339933;">/*</span> Restore integer registers <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Leave</span> function <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">better_vectorial_matmul_4x4:
    /* r0 address of A
       r1 address of B
       r2 address of C
    */
    push {r4, r5, r6, r7, r8, lr} /* Keep integer registers */
    vpush {s16-s19}               /* Floating point registers starting from s16 must be preserved */
    vpush {s24-s27}
    /* First zero 16 single floating point */
    /* In IEEE 754, all bits cleared means 0 */
    mov r4, r2
    mov r5, #16
    mov r6, #0
    b .L3_loop_init_test
    .L3_loop_init :
      str r6, [r4], +#4   /* *r4 ← r6 then r4 ← r4 + 4 */
    .L3_loop_init_test:
      subs r5, r5, #1
      bge .L3_loop_init

    /* Set the LEN field of FPSCR to be 4 (value 3) */
    mov r5, #0b011                        /* r5 ← 3 */
    mov r5, r5, LSL #16                   /* r5 ← r5 &lt;&lt; 16 */
    fmrx r4, fpscr                        /* r4 ← fpscr */
    orr r4, r4, r5                        /* r4 ← r4 | r5 */
    fmxr fpscr, r4                        /* fpscr ← r4 */

    /* We will use 
           r4 as k
           r5 as i
    */
    mov r4, #0 /* r4 ← 0 */
    .L3_loop_k:  /* loop header of k */
      cmp r4, #4  /* if r4 == 4 goto end of the loop k */
      beq .L3_end_loop_k
      mov r5, #0  /* r5 ← 0 */
      .L3_loop_i: /* loop header of i */
       cmp r5, #4 /* if r5 == 4 goto end of the loop i */
        beq .L3_end_loop_i
        /* Compute the address of C[i][0] */
        /* Address of C[i][0] is C + 4*(4 * i) */
        add r7, r2, r5, LSL #4         /* r7 ← r2 + (r5 &lt;&lt; 4). This is r7 ← c + 4*4*i */
        vldmia r7, {s8-s11}            /* Load {s8,s9,s10,s11} ← {c[i][0], c[i][1], c[i][2], c[i][3]} */
        /* Compute the address of A[i][k] */
        /* Address of A[i][k] is A + 4*(4*i + k) */
        add r8, r4, r5, LSL #2         /* r8 ← r4 + r5 &lt;&lt; 2. This is r8 ← k + 4*i */
        add r8, r0, r8, LSL #2         /* r8 ← r0 + r8 &lt;&lt; 2. This is r8 ← a + 4*(k + 4*i) */
        vldr s0, [r8]                  /* Load s0 ← a[i][k] */

        /* Compute the address of B[k][0] */
        /* Address of B[k][0] is B + 4*(4*k) */
        add r8, r1, r4, LSL #4         /* r8 ← r1 + r4 &lt;&lt; 4. This is r8 ← b + 4*(4*k) */
        vldmia r8, {s16-s19}           /* Load {s16,s17,s18,s19} ← {b[k][0], b[k][1], b[k][2], b[k][3]} */

        vmul.f32 s24, s16, s0          /* {s24,s25,s26,s27} ← {s16,s17,s18,s19} * {s0,s0,s0,s0} */
        vadd.f32 s8, s8, s24           /* {s8,s9,s10,s11} ← {s8,s9,s10,s11} + {s24,s25,s26,s7} */

        vstmia r7, {s8-s11}            /* Store {c[i][0], c[i][1], c[i][2], c[i][3]} ← {s8,s9,s10,s11} */

        add r5, r5, #1  /* r5 ← r5 + 1. This is i = i + 1 */
        b .L3_loop_i /* next iteration of loop i */
       .L3_end_loop_i: /* Here ends loop i */
       add r4, r4, #1 /* r4 ← r4 + 1. This is k = k + 1 */
       b .L3_loop_k     /* next iteration of loop k */
    .L3_end_loop_k: /* Here ends loop k */

    /* Set the LEN field of FPSCR back to 1 (value 0) */
    mov r5, #0b011                        /* r5 ← 3 */
    mvn r5, r5, LSL #16                   /* r5 ← ~(r5 &lt;&lt; 16) */
    fmrx r4, fpscr                        /* r4 ← fpscr */
    and r4, r4, r5                        /* r4 ← r4 &amp; r5 */
    fmxr fpscr, r4                        /* fpscr ← r4 */

    vpop {s24-s27}                /* Restore preserved floating registers */
    vpop {s16-s19}
    pop {r4, r5, r6, r7, r8, lr}  /* Restore integer registers */
    bx lr /* Leave function */</p></div>

<p>
As adding after a multiplication is a relatively usual sequence, we can replace the sequence
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>55
56
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">vmul<span style="color: #339933;">.</span>f32 s24<span style="color: #339933;">,</span> s16<span style="color: #339933;">,</span> s0          <span style="color: #339933;">/*</span> <span style="color: #009900; font-weight: bold;">{</span>s24<span style="color: #339933;">,</span>s25<span style="color: #339933;">,</span>s26<span style="color: #339933;">,</span>s27<span style="color: #009900; font-weight: bold;">}</span> ← <span style="color: #009900; font-weight: bold;">{</span>s16<span style="color: #339933;">,</span>s17<span style="color: #339933;">,</span>s18<span style="color: #339933;">,</span>s19<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">*</span> <span style="color: #009900; font-weight: bold;">{</span>s0<span style="color: #339933;">,</span>s0<span style="color: #339933;">,</span>s0<span style="color: #339933;">,</span>s0<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">*/</span>
vadd<span style="color: #339933;">.</span>f32 s8<span style="color: #339933;">,</span> s8<span style="color: #339933;">,</span> s24           <span style="color: #339933;">/*</span> <span style="color: #009900; font-weight: bold;">{</span>s8<span style="color: #339933;">,</span>s9<span style="color: #339933;">,</span>s10<span style="color: #339933;">,</span>s11<span style="color: #009900; font-weight: bold;">}</span> ← <span style="color: #009900; font-weight: bold;">{</span>s8<span style="color: #339933;">,</span>s9<span style="color: #339933;">,</span>s10<span style="color: #339933;">,</span>s11<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">{</span>s24<span style="color: #339933;">,</span>s25<span style="color: #339933;">,</span>s26<span style="color: #339933;">,</span>s7<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">vmul.f32 s24, s16, s0          /* {s24,s25,s26,s27} ← {s16,s17,s18,s19} * {s0,s0,s0,s0} */
vadd.f32 s8, s8, s24           /* {s8,s9,s10,s11} ← {s8,s9,s10,s11} + {s24,s25,s26,s7} */</p></div>

<p>
with a single instruction <code>vmla</code> (multiply and add).
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>55
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">vmla<span style="color: #339933;">.</span>f32 s8<span style="color: #339933;">,</span> s16<span style="color: #339933;">,</span> s0          <span style="color: #339933;">/*</span> <span style="color: #009900; font-weight: bold;">{</span>s8<span style="color: #339933;">,</span>s9<span style="color: #339933;">,</span>s10<span style="color: #339933;">,</span>s11<span style="color: #009900; font-weight: bold;">}</span> ← <span style="color: #009900; font-weight: bold;">{</span>s8<span style="color: #339933;">,</span>s9<span style="color: #339933;">,</span>s10<span style="color: #339933;">,</span>s11<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span><span style="color: #009900; font-weight: bold;">{</span>s16<span style="color: #339933;">,</span>s17<span style="color: #339933;">,</span>s18<span style="color: #339933;">,</span>s19<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">*</span> <span style="color: #009900; font-weight: bold;">{</span>s0<span style="color: #339933;">,</span>s0<span style="color: #339933;">,</span>s0<span style="color: #339933;">,</span>s0<span style="color: #009900; font-weight: bold;">}</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">vmla.f32 s8, s16, s0          /* {s8,s9,s10,s11} ← {s8,s9,s10,s11} + ({s16,s17,s18,s19} * {s0,s0,s0,s0}) */</p></div>

<p>
Now we can also unroll the loop <code>i</code>, again with an unrolling factor of 2. This would give us the <em>best</em> version.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
21
22
23
24
25
26
27
28
29
30
31
32
33
34
35
36
37
38
39
40
41
42
43
44
45
46
47
48
49
50
51
52
53
54
55
56
57
58
59
60
61
62
63
64
65
66
67
68
69
70
71
72
73
74
75
76
77
78
79
80
81
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">best_vectorial_matmul_4x4<span style="color: #339933;">:</span>
    <span style="color: #339933;">/*</span> r0 address of A
       r1 address of B
       r2 address of C
    <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> r7<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">/*</span> Keep integer registers <span style="color: #339933;">*/</span>
    vpush <span style="color: #009900; font-weight: bold;">{</span>s16<span style="color: #339933;">-</span>s19<span style="color: #009900; font-weight: bold;">}</span>               <span style="color: #339933;">/*</span> Floating point registers starting from s16 must be preserved <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> First zero <span style="color: #ff0000;">16</span> single floating point <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">In</span> IEEE <span style="color: #ff0000;">754</span><span style="color: #339933;">,</span> all <span style="color: #0000ff; font-weight: bold;">bits</span> cleared means <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> r2
    <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">16</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r6<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>
    b <span style="color: #339933;">.</span>L4_loop_init_test
    <span style="color: #339933;">.</span>L4_loop_init <span style="color: #339933;">:</span>
      <span style="color: #00007f; font-weight: bold;">str</span> r6<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> <span style="color: #339933;">+</span>#<span style="color: #ff0000;">4</span>   <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span>r4 ← r6 then r4 ← r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span> <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>L4_loop_init_test<span style="color: #339933;">:</span>
      subs r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>
      bge <span style="color: #339933;">.</span>L4_loop_init
&nbsp;
    <span style="color: #339933;">/*</span> Set the LEN field of FPSCR to be <span style="color: #ff0000;">4</span> <span style="color: #009900; font-weight: bold;">(</span>value <span style="color: #ff0000;">3</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> #0b011                        <span style="color: #339933;">/*</span> r5 ← <span style="color: #ff0000;">3</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">16</span>                   <span style="color: #339933;">/*</span> r5 ← r5 &lt;&lt; <span style="color: #ff0000;">16</span> <span style="color: #339933;">*/</span>
    fmrx r4<span style="color: #339933;">,</span> fpscr                        <span style="color: #339933;">/*</span> r4 ← fpscr <span style="color: #339933;">*/</span>
    orr r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> r5                        <span style="color: #339933;">/*</span> r4 ← r4 | r5 <span style="color: #339933;">*/</span>
    fmxr fpscr<span style="color: #339933;">,</span> r4                        <span style="color: #339933;">/*</span> fpscr ← r4 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> We will use 
           r4 as k
           r5 as i
    <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span> <span style="color: #339933;">/*</span> r4 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>L4_loop_k<span style="color: #339933;">:</span>  <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">loop</span> header of k <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">cmp</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> if r4 == <span style="color: #ff0000;">4</span> goto end of the <span style="color: #00007f; font-weight: bold;">loop</span> k <span style="color: #339933;">*/</span>
      beq <span style="color: #339933;">.</span>L4_end_loop_k
      <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>  <span style="color: #339933;">/*</span> r5 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
      <span style="color: #339933;">.</span>L4_loop_i<span style="color: #339933;">:</span> <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">loop</span> header of i <span style="color: #339933;">*/</span>
       <span style="color: #00007f; font-weight: bold;">cmp</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span> <span style="color: #339933;">/*</span> if r5 == <span style="color: #ff0000;">4</span> goto end of the <span style="color: #00007f; font-weight: bold;">loop</span> i <span style="color: #339933;">*/</span>
        beq <span style="color: #339933;">.</span>L4_end_loop_i
        <span style="color: #339933;">/*</span> Compute the address of C<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
        <span style="color: #339933;">/*</span> Address of C<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span> is C <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #ff0000;">4</span> <span style="color: #339933;">*</span> i<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">add</span> r7<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">4</span>         <span style="color: #339933;">/*</span> r7 ← r2 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span>r5 &lt;&lt; <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> This is r7 ← c <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span><span style="color: #ff0000;">4</span><span style="color: #339933;">*</span>i <span style="color: #339933;">*/</span>
        vldmia r7<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">{</span>s8<span style="color: #339933;">-</span>s15<span style="color: #009900; font-weight: bold;">}</span>            <span style="color: #339933;">/*</span> Load <span style="color: #009900; font-weight: bold;">{</span>s8<span style="color: #339933;">,</span>s9<span style="color: #339933;">,</span>s10<span style="color: #339933;">,</span>s11<span style="color: #339933;">,</span>s12<span style="color: #339933;">,</span>s13<span style="color: #339933;">,</span>s14<span style="color: #339933;">,</span>s15<span style="color: #009900; font-weight: bold;">}</span> 
                                            ← <span style="color: #009900; font-weight: bold;">{</span>c<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span>   c<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span>   c<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span>   c<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">3</span><span style="color: #009900; font-weight: bold;">]</span>
                                               c<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #339933;">+</span><span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> c<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #339933;">+</span><span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> c<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #339933;">+</span><span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> c<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #339933;">+</span><span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">3</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">*/</span>
        <span style="color: #339933;">/*</span> Compute the address of A<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>k<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
        <span style="color: #339933;">/*</span> Address of A<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>k<span style="color: #009900; font-weight: bold;">]</span> is A <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #ff0000;">4</span><span style="color: #339933;">*</span>i <span style="color: #339933;">+</span> k<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">add</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span>         <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r8</span> ← r4 <span style="color: #339933;">+</span> r5 &lt;&lt; <span style="color: #ff0000;">2</span><span style="color: #339933;">.</span> This is <span style="color: #46aa03; font-weight: bold;">r8</span> ← k <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span>i <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">add</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span>         <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r8</span> ← r0 <span style="color: #339933;">+</span> <span style="color: #46aa03; font-weight: bold;">r8</span> &lt;&lt; <span style="color: #ff0000;">2</span><span style="color: #339933;">.</span> This is <span style="color: #46aa03; font-weight: bold;">r8</span> ← a <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>k <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span>i<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
        vldr s0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #009900; font-weight: bold;">]</span>                  <span style="color: #339933;">/*</span> Load s0 ← a<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>k<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
        vldr s1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">]</span>             <span style="color: #339933;">/*</span> Load s1 ← a<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #339933;">+</span><span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span>k<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
&nbsp;
        <span style="color: #339933;">/*</span> Compute the address of B<span style="color: #009900; font-weight: bold;">[</span>k<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
        <span style="color: #339933;">/*</span> Address of B<span style="color: #009900; font-weight: bold;">[</span>k<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span> is B <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #ff0000;">4</span><span style="color: #339933;">*</span>k<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">add</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">4</span>         <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r8</span> ← r1 <span style="color: #339933;">+</span> r4 &lt;&lt; <span style="color: #ff0000;">4</span><span style="color: #339933;">.</span> This is <span style="color: #46aa03; font-weight: bold;">r8</span> ← b <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #ff0000;">4</span><span style="color: #339933;">*</span>k<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
        vldmia <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">{</span>s16<span style="color: #339933;">-</span>s19<span style="color: #009900; font-weight: bold;">}</span>           <span style="color: #339933;">/*</span> Load <span style="color: #009900; font-weight: bold;">{</span>s16<span style="color: #339933;">,</span>s17<span style="color: #339933;">,</span>s18<span style="color: #339933;">,</span>s19<span style="color: #009900; font-weight: bold;">}</span> ← <span style="color: #009900; font-weight: bold;">{</span>b<span style="color: #009900; font-weight: bold;">[</span>k<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> b<span style="color: #009900; font-weight: bold;">[</span>k<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> b<span style="color: #009900; font-weight: bold;">[</span>k<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> b<span style="color: #009900; font-weight: bold;">[</span>k<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">3</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">*/</span>
&nbsp;
        vmla<span style="color: #339933;">.</span>f32 s8<span style="color: #339933;">,</span> s16<span style="color: #339933;">,</span> s0           <span style="color: #339933;">/*</span> <span style="color: #009900; font-weight: bold;">{</span>s8<span style="color: #339933;">,</span>s9<span style="color: #339933;">,</span>s10<span style="color: #339933;">,</span>s11<span style="color: #009900; font-weight: bold;">}</span> ← <span style="color: #009900; font-weight: bold;">{</span>s8<span style="color: #339933;">,</span>s9<span style="color: #339933;">,</span>s10<span style="color: #339933;">,</span>s11<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span><span style="color: #009900; font-weight: bold;">{</span>s16<span style="color: #339933;">,</span>s17<span style="color: #339933;">,</span>s18<span style="color: #339933;">,</span>s19<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">*</span> <span style="color: #009900; font-weight: bold;">{</span>s0<span style="color: #339933;">,</span>s0<span style="color: #339933;">,</span>s0<span style="color: #339933;">,</span>s0<span style="color: #009900; font-weight: bold;">}</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
        vmla<span style="color: #339933;">.</span>f32 s12<span style="color: #339933;">,</span> s16<span style="color: #339933;">,</span> s1          <span style="color: #339933;">/*</span> <span style="color: #009900; font-weight: bold;">{</span>s12<span style="color: #339933;">,</span>s13<span style="color: #339933;">,</span>s14<span style="color: #339933;">,</span>s15<span style="color: #009900; font-weight: bold;">}</span> ← <span style="color: #009900; font-weight: bold;">{</span>s12<span style="color: #339933;">,</span>s13<span style="color: #339933;">,</span>s14<span style="color: #339933;">,</span>s15<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span><span style="color: #009900; font-weight: bold;">{</span>s16<span style="color: #339933;">,</span>s17<span style="color: #339933;">,</span>s18<span style="color: #339933;">,</span>s19<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">*</span> <span style="color: #009900; font-weight: bold;">{</span>s1<span style="color: #339933;">,</span>s1<span style="color: #339933;">,</span>s1<span style="color: #339933;">,</span>s1<span style="color: #009900; font-weight: bold;">}</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
&nbsp;
        vstmia r7<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">{</span>s8<span style="color: #339933;">-</span>s15<span style="color: #009900; font-weight: bold;">}</span>            <span style="color: #339933;">/*</span> Store <span style="color: #009900; font-weight: bold;">{</span>c<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span>   c<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span>   c<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span>    c<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">3</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span>
                                                 c<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #339933;">+</span><span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> c<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #339933;">+</span><span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> c<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #339933;">+</span><span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">}</span><span style="color: #339933;">,</span> c<span style="color: #009900; font-weight: bold;">[</span>i<span style="color: #339933;">+</span><span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">3</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #009900; font-weight: bold;">}</span>
                                                ← <span style="color: #009900; font-weight: bold;">{</span>s8<span style="color: #339933;">,</span>s9<span style="color: #339933;">,</span>s10<span style="color: #339933;">,</span>s11<span style="color: #339933;">,</span>s12<span style="color: #339933;">,</span>s13<span style="color: #339933;">,</span>s14<span style="color: #339933;">,</span>s15<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">*/</span>
&nbsp;
        <span style="color: #00007f; font-weight: bold;">add</span> r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span>  <span style="color: #339933;">/*</span> r5 ← r5 <span style="color: #339933;">+</span> <span style="color: #ff0000;">2</span><span style="color: #339933;">.</span> This is i = i <span style="color: #339933;">+</span> <span style="color: #ff0000;">2</span> <span style="color: #339933;">*/</span>
        b <span style="color: #339933;">.</span>L4_loop_i <span style="color: #339933;">/*</span> next iteration of <span style="color: #00007f; font-weight: bold;">loop</span> i <span style="color: #339933;">*/</span>
       <span style="color: #339933;">.</span>L4_end_loop_i<span style="color: #339933;">:</span> <span style="color: #339933;">/*</span> Here ends <span style="color: #00007f; font-weight: bold;">loop</span> i <span style="color: #339933;">*/</span>
       <span style="color: #00007f; font-weight: bold;">add</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span> <span style="color: #339933;">/*</span> r4 ← r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span><span style="color: #339933;">.</span> This is k = k <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
       b <span style="color: #339933;">.</span>L4_loop_k     <span style="color: #339933;">/*</span> next iteration of <span style="color: #00007f; font-weight: bold;">loop</span> k <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>L4_end_loop_k<span style="color: #339933;">:</span> <span style="color: #339933;">/*</span> Here ends <span style="color: #00007f; font-weight: bold;">loop</span> k <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> Set the LEN field of FPSCR back to <span style="color: #ff0000;">1</span> <span style="color: #009900; font-weight: bold;">(</span>value <span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> #0b011                        <span style="color: #339933;">/*</span> r5 ← <span style="color: #ff0000;">3</span> <span style="color: #339933;">*/</span>
    mvn r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">16</span>                   <span style="color: #339933;">/*</span> r5 ← ~<span style="color: #009900; font-weight: bold;">(</span>r5 &lt;&lt; <span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    fmrx r4<span style="color: #339933;">,</span> fpscr                        <span style="color: #339933;">/*</span> r4 ← fpscr <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">and</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> r5                        <span style="color: #339933;">/*</span> r4 ← r4 &amp; r5 <span style="color: #339933;">*/</span>
    fmxr fpscr<span style="color: #339933;">,</span> r4                        <span style="color: #339933;">/*</span> fpscr ← r4 <span style="color: #339933;">*/</span>
&nbsp;
    vpop <span style="color: #009900; font-weight: bold;">{</span>s16<span style="color: #339933;">-</span>s19<span style="color: #009900; font-weight: bold;">}</span>                <span style="color: #339933;">/*</span> Restore preserved floating registers <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> r7<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>  <span style="color: #339933;">/*</span> Restore integer registers <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Leave</span> function <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">best_vectorial_matmul_4x4:
    /* r0 address of A
       r1 address of B
       r2 address of C
    */
    push {r4, r5, r6, r7, r8, lr} /* Keep integer registers */
    vpush {s16-s19}               /* Floating point registers starting from s16 must be preserved */

    /* First zero 16 single floating point */
    /* In IEEE 754, all bits cleared means 0 */
    mov r4, r2
    mov r5, #16
    mov r6, #0
    b .L4_loop_init_test
    .L4_loop_init :
      str r6, [r4], +#4   /* *r4 ← r6 then r4 ← r4 + 4 */
    .L4_loop_init_test:
      subs r5, r5, #1
      bge .L4_loop_init

    /* Set the LEN field of FPSCR to be 4 (value 3) */
    mov r5, #0b011                        /* r5 ← 3 */
    mov r5, r5, LSL #16                   /* r5 ← r5 &lt;&lt; 16 */
    fmrx r4, fpscr                        /* r4 ← fpscr */
    orr r4, r4, r5                        /* r4 ← r4 | r5 */
    fmxr fpscr, r4                        /* fpscr ← r4 */

    /* We will use 
           r4 as k
           r5 as i
    */
    mov r4, #0 /* r4 ← 0 */
    .L4_loop_k:  /* loop header of k */
      cmp r4, #4  /* if r4 == 4 goto end of the loop k */
      beq .L4_end_loop_k
      mov r5, #0  /* r5 ← 0 */
      .L4_loop_i: /* loop header of i */
       cmp r5, #4 /* if r5 == 4 goto end of the loop i */
        beq .L4_end_loop_i
        /* Compute the address of C[i][0] */
        /* Address of C[i][0] is C + 4*(4 * i) */
        add r7, r2, r5, LSL #4         /* r7 ← r2 + (r5 &lt;&lt; 4). This is r7 ← c + 4*4*i */
        vldmia r7, {s8-s15}            /* Load {s8,s9,s10,s11,s12,s13,s14,s15} 
                                            ← {c[i][0],   c[i][1],   c[i][2],   c[i][3]
                                               c[i+1][0], c[i+1][1], c[i+1][2], c[i+1][3]} */
        /* Compute the address of A[i][k] */
        /* Address of A[i][k] is A + 4*(4*i + k) */
        add r8, r4, r5, LSL #2         /* r8 ← r4 + r5 &lt;&lt; 2. This is r8 ← k + 4*i */
        add r8, r0, r8, LSL #2         /* r8 ← r0 + r8 &lt;&lt; 2. This is r8 ← a + 4*(k + 4*i) */
        vldr s0, [r8]                  /* Load s0 ← a[i][k] */
        vldr s1, [r8, #16]             /* Load s1 ← a[i+1][k] */

        /* Compute the address of B[k][0] */
        /* Address of B[k][0] is B + 4*(4*k) */
        add r8, r1, r4, LSL #4         /* r8 ← r1 + r4 &lt;&lt; 4. This is r8 ← b + 4*(4*k) */
        vldmia r8, {s16-s19}           /* Load {s16,s17,s18,s19} ← {b[k][0], b[k][1], b[k][2], b[k][3]} */

        vmla.f32 s8, s16, s0           /* {s8,s9,s10,s11} ← {s8,s9,s10,s11} + ({s16,s17,s18,s19} * {s0,s0,s0,s0}) */
        vmla.f32 s12, s16, s1          /* {s12,s13,s14,s15} ← {s12,s13,s14,s15} + ({s16,s17,s18,s19} * {s1,s1,s1,s1}) */

        vstmia r7, {s8-s15}            /* Store {c[i][0],   c[i][1],   c[i][2],    c[i][3],
                                                 c[i+1][0], c[i+1][1], c[i+1][2]}, c[i+1][3] }
                                                ← {s8,s9,s10,s11,s12,s13,s14,s15} */

        add r5, r5, #2  /* r5 ← r5 + 2. This is i = i + 2 */
        b .L4_loop_i /* next iteration of loop i */
       .L4_end_loop_i: /* Here ends loop i */
       add r4, r4, #1 /* r4 ← r4 + 1. This is k = k + 1 */
       b .L4_loop_k     /* next iteration of loop k */
    .L4_end_loop_k: /* Here ends loop k */

    /* Set the LEN field of FPSCR back to 1 (value 0) */
    mov r5, #0b011                        /* r5 ← 3 */
    mvn r5, r5, LSL #16                   /* r5 ← ~(r5 &lt;&lt; 16) */
    fmrx r4, fpscr                        /* r4 ← fpscr */
    and r4, r4, r5                        /* r4 ← r4 &amp; r5 */
    fmxr fpscr, r4                        /* fpscr ← r4 */

    vpop {s16-s19}                /* Restore preserved floating registers */
    pop {r4, r5, r6, r7, r8, lr}  /* Restore integer registers */
    bx lr /* Leave function */</p></div>

<h2>Comparing versions</h2>
<p>
Out of curiosity I tested the versions, to see which one was faster.
</p>
<p>
The benchmark consists on repeatedly calling the multiplication matrix function 2<sup>21</sup> times (actually 2<sup>21</sup>-1 because of a typo, see the code) in order to magnify differences. While the input should be randomized as well for a better benchmark, the benchmark more or less models contexts where a matrix multiplication is performed many times (for instance in graphics).
</p>
<p>
This is the skeleton of the benchmark.</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">main<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
&nbsp;
    ldr r0<span style="color: #339933;">,</span> addr_mat_A  <span style="color: #339933;">/*</span> r0 ← a <span style="color: #339933;">*/</span>
    ldr r1<span style="color: #339933;">,</span> addr_mat_B  <span style="color: #339933;">/*</span> r1 ← b <span style="color: #339933;">*/</span>
    ldr r2<span style="color: #339933;">,</span> addr_mat_C  <span style="color: #339933;">/*</span> r2 ← c <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">21</span>
    <span style="color: #339933;">.</span>Lmain_loop_test<span style="color: #339933;">:</span> 
      <span style="color: #46aa03; font-weight: bold;">bl</span> &lt;&lt;tested<span style="color: #339933;">-</span>matmul<span style="color: #339933;">-</span>routine&gt;&gt; <span style="color: #339933;">/*</span> Change here with the matmul you want to <span style="color: #00007f; font-weight: bold;">test</span> <span style="color: #339933;">*/</span>
      subs r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>
      bne <span style="color: #339933;">.</span>Lmain_loop_test         <span style="color: #339933;">/*</span> I should have written <span style="color: #7f007f;">'bge'</span> here<span style="color: #339933;">,</span>
                                      but I cannot change it without 
                                      having to run the benchmarks again <span style="color: #339933;">:</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">main:
    push {r4, lr}

    ldr r0, addr_mat_A  /* r0 ← a */
    ldr r1, addr_mat_B  /* r1 ← b */
    ldr r2, addr_mat_C  /* r2 ← c */
    mov r4, #1
    mov r4, r4, LSL #21
    .Lmain_loop_test: 
      bl &lt;&lt;tested-matmul-routine&gt;&gt; /* Change here with the matmul you want to test */
      subs r4, r4, #1
      bne .Lmain_loop_test         /* I should have written 'bge' here,
                                      but I cannot change it without 
                                      having to run the benchmarks again :) */

    mov r0, #0
    pop {r4, lr}
    bx lr</p></div>

<p>
Here are the results. The one we named the best turned to actually deserve that name.
</p>
<table>
<tbody><tr>
<th>Version</th>
<th>Time (seconds)</th>
</tr>
<tr>
<td>naive_matmul_4x4</td>
<td>6.41</td>
</tr>
<tr>
<td>naive_vectorial_matmul_4x4</td>
<td>3.51</td>
</tr>
<tr>
<td>naive_vectorial_matmul_2_4x4</td>
<td>2.87</td>
</tr>
<tr>
<td>better_vectorial_matmul_4x4</td>
<td>2.59</td>
</tr>
<tr>
<td>best_vectorial_matmul_4x4</td>
<td>1.51</td>
</tr>
</tbody></table>
<p>
That’s all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/&amp;text=ARM+assembler+in+Raspberry+Pi+%E2%80%93+Chapter+14+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>						<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/" rel="prev">ARM assembler in Raspberry Pi – Chapter 13</a></span>
			<span class="next"><a href="http://thinkingeek.com/2013/06/08/check_mk-software-updates-mount-options-alarms/" rel="next">Check_MK, software updates and mount options alarms</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
			<h3>16 thoughts on “<span>ARM assembler in Raspberry Pi – Chapter 14</span>”</h3>
		<ul class="commentlist">
				<li class="comment even thread-even depth-1 parent" id="comment-2945">
				<div id="div-comment-2945" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/12f4e20df8cbd5492be08e34dad82663?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/12f4e20df8cbd5492be08e34dad82663?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/12f4e20df8cbd5492be08e34dad82663?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-12f4e20df8cbd5492be08e34dad82663-0">			<cite class="fn">Henryk Sarat</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/#comment-2945">
			January 10, 2014 at 9:02 am</a>		</div>

		<p>In the Vectorial approach, why do you use AND to set LEN back to 1?</p>
<p>Why did you use OR in the first place?<br>
And why did you use the value “#0b011”?</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/?replytocom=2945#respond" onclick="return addComment.moveForm( &quot;div-comment-2945&quot;, &quot;2945&quot;, &quot;respond&quot;, &quot;1003&quot; )" aria-label="Reply to Henryk Sarat">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2 parent" id="comment-2946">
				<div id="div-comment-2946" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-0">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/#comment-2946">
			January 10, 2014 at 11:15 am</a>		</div>

		<p>Hi Henryk,</p>
<p>(I won’t make any assumption about your «bit-twiddling» skills so you may skip the following stuff if is too basic for you)</p>
<p>The bitwise OR operation has the following table of truth</p>
<p>0 OR 0 = 0<br>
0 OR 1 = 1<br>
1 OR 0 = 1<br>
1 OR 1 = 1</p>
<p>So given a 0 or 1, we can make sure it is 1 by just OR-ing it with 1. OR-ing with a 0 makes no efect (it leaves the 0 or 1 as it was)</p>
<p>Conversely, the bitwise AND operation has the following table of truth</p>
<p>0 AND 0 = 0<br>
0 AND 1 = 0<br>
1 AND 0 = 0<br>
1 AND 1 = 1</p>
<p>From this table you can see that by AND-ing with a 0 you will always get a zero. AND-ing with 1 has no effect.</p>
<p>From chapter 13 we know that the 32-bit register <code>fpscr</code> register allows us to modify the behaviour of the floating point operations. Unfortunately we cannot modify this register directly, so we have to load its value into a normal register, update that normal register and write back that register into <code>fpscr</code>.</p>
<p>Bits 18, 17 and 16 of <code>fpscr</code> are used to set the LEN of the vectorial operations. The LEN value these three bits represent is just <em>the 3-bit number plus 1</em>. This is, when the three bits (18, 17, and 16, respectively) are zero, this is 000, then LEN=1 (it would not make sense to represent LEN=0, so the minimum value that LEN has is always 1).</p>
<p>In this chapter we set LEN to 4. So the three bits, 18 to 16, must represent the number 3 (so 3 + 1 will be 4). The number 3 in binary is 011 (bit 18 must be zero, 17 and 16 must be 1). We may assume, thanks to the AAPCS, that <code>fpscr</code> has a LEN=1 upon the entry of the function, so as stated above, all three bits will be zero, i.e. 000.</p>
<p>Now comes the tricky part, we want to modify just 3 bits of fpscr but we want to leave the remaining ones alone. We only have to set two bits (17 and 16) to be 1 without modifying any other bit. As said above, we have first to load <code>fpscr</code> into some register, <code>r4</code> in the example. Now we take the value 3 (I used <code>#0b011</code> in the example, but I could have used #3) and put it into some other register, <code>r5</code> in the example. We want the second of the two 1’s of that 3 to be the bit number 16 (and then, automatically the first of the two 1’s will be the number 17). So we shift left 16 bits using a <code>mov r5, r5, LSL #16</code>. Now we have an <em>OR-mask</em> with bits 0000 0000 0000 0011 0000 0000 0000 0000. As you can see, this OR-mask is all 0 except bits 17 and 16 which are 1 (if you count them recall that the rightmost bit is the <em>zeroth</em>). Now we perform an OR between the original value of <code>fpscr</code>, in <code>r4</code>, and the OR-mask, in <code>r5</code>. Only bits 17 and 16 in <code>r4</code> will be updated to be 1, all the other will be left untouched (because, recall, OR-ing with a 0 makes no effect). Now that <code>r4</code> has the value we want, we use it to set the <code>fpscr</code>.</p>
<p>We use a similar strategy to set LEN back to 0. In this case, though, we compute an AND-mask. This is, all bits will be 1 except bits 17 and 16 that will be 0. These two bits will clear the bits in the <code>fpscr</code> but leave the remaining untouched. Note that it is easier to compute an OR-mask first and then negate all the bits by using the <code>mvn</code> instruction. This instruction works exactly like <code>mov</code> but it applies a NOT to the second operand. In this case the mask will be 1111 1111 1111 1100 1111 1111 1111 1111.</p>
<p>I hope this detailed explanation makes clear why we use <code>or</code> and <code>and</code> to modify LEN.</p>
<p>Kind regards.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/?replytocom=2946#respond" onclick="return addComment.moveForm( &quot;div-comment-2946&quot;, &quot;2946&quot;, &quot;respond&quot;, &quot;1003&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment even depth-3" id="comment-3012">
				<div id="div-comment-3012" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/aa649450777ff2f5dbf16b0a2734321c?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/aa649450777ff2f5dbf16b0a2734321c?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/aa649450777ff2f5dbf16b0a2734321c?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-aa649450777ff2f5dbf16b0a2734321c-0">			<cite class="fn">Henryk Sarat</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/#comment-3012">
			January 12, 2014 at 2:18 am</a>		</div>

		<p>Thanks for the detailed explanation. The key there for me was that you used mvn to negate the values. Maybe in the comment for that line you can make that clear for any future readers because i completely missed it (I just noticed that you used ~ as well).</p>
<p>Thanks again, these tutorials have been a huge help!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/?replytocom=3012#respond" onclick="return addComment.moveForm( &quot;div-comment-3012&quot;, &quot;3012&quot;, &quot;respond&quot;, &quot;1003&quot; )" aria-label="Reply to Henryk Sarat">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1 parent" id="comment-110866">
				<div id="div-comment-110866" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/bd940bd4d0a4e42c4d5aa3889f69c680?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/bd940bd4d0a4e42c4d5aa3889f69c680?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/bd940bd4d0a4e42c4d5aa3889f69c680?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-bd940bd4d0a4e42c4d5aa3889f69c680-0">			<cite class="fn">David Todd</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/#comment-110866">
			April 20, 2014 at 3:26 am</a>		</div>

		<p>This is an exceptionally good intro to ARM6 assembly language programming … it’s very well written, with great examples.  Thanks *very* much for your effort: I’ve really enjoyed your writing.</p>
<p>I did find a loop control error in three places in the matrix multiply examples, generally around the loop that initializes c[i][j]=0.  Though you init the counter to 16, you jump into the loop and decrement by 1 and test, so the loop is running 15…1 — only 15 iterations.  The branches should be bge rather than bne in the sequence of instructions following .Lloop_init_test, .L3_loop_init_test, and .L4_loop_init_test.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/?replytocom=110866#respond" onclick="return addComment.moveForm( &quot;div-comment-110866&quot;, &quot;110866&quot;, &quot;respond&quot;, &quot;1003&quot; )" aria-label="Reply to David Todd">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2" id="comment-110984">
				<div id="div-comment-110984" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-1">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/#comment-110984">
			April 20, 2014 at 9:24 am</a>		</div>

		<p>Hi David,</p>
<p>oops! I did not realize the mistake earlier because the C matrix is already initialized to zero. I fixed the post, thank you very much.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/?replytocom=110984#respond" onclick="return addComment.moveForm( &quot;div-comment-110984&quot;, &quot;110984&quot;, &quot;respond&quot;, &quot;1003&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-even depth-1 parent" id="comment-953075">
				<div id="div-comment-953075" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/938463c4393c7262e73d6b0d099caba5?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/938463c4393c7262e73d6b0d099caba5?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/938463c4393c7262e73d6b0d099caba5?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-938463c4393c7262e73d6b0d099caba5-0">			<cite class="fn">Martin</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/#comment-953075">
			November 27, 2015 at 11:50 pm</a>		</div>

		<p>Hi, I am having a lot fun working through your tutorial.<br>
But now I’m running into difficulties and I’m hoping for some insight.<br>
I try to compile the matrix multiplications on my cubietruck/raspberry Pi 2, both having the newer (armv7) Cortex-A7 processor with the vpfv4 coprocessor.<br>
The OS is build with hard-float support and I tried to choose all kinds of fpu settings for compiling, but I always end up with the same result:<br>
your benchmark with the naive implementation takes ~3seconds and all the improved versions have a far worse performance with up to 1m12s.<br>
Is there anything I overlooked? Is the handling of the vector registers differently (and not backwards compatible to your code) on the newer processors?</p>
<p>Thanks for your great work!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/?replytocom=953075#respond" onclick="return addComment.moveForm( &quot;div-comment-953075&quot;, &quot;953075&quot;, &quot;respond&quot;, &quot;1003&quot; )" aria-label="Reply to Martin">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2 parent" id="comment-953124">
				<div id="div-comment-953124" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-2">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/#comment-953124">
			November 28, 2015 at 10:53 am</a>		</div>

		<p>Hi Martin,</p>
<p>well, I do not have a Raspi 2 (I’ll try to borrow one from a friend of mine) and try there.</p>
<p>Your results are a bit surprising but maybe performance in this part is not portable between these two processors. That said, this would be highly undesirable. Make sure you are using the same code as I used to benchmark. I posted it in my github page.</p>
<p>  <a href="https://github.com/rofirrim/raspberry-pi-assembler/tree/master/chapter14" rel="nofollow">https://github.com/rofirrim/raspberry-pi-assembler/tree/master/chapter14</a></p>
<p>The file you are interested is <code>benchmark.s</code>.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/?replytocom=953124#respond" onclick="return addComment.moveForm( &quot;div-comment-953124&quot;, &quot;953124&quot;, &quot;respond&quot;, &quot;1003&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment odd alt depth-3 parent" id="comment-953125">
				<div id="div-comment-953125" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/938463c4393c7262e73d6b0d099caba5?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/938463c4393c7262e73d6b0d099caba5?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/938463c4393c7262e73d6b0d099caba5?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-938463c4393c7262e73d6b0d099caba5-1">			<cite class="fn">Martin</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/#comment-953125">
			November 28, 2015 at 11:18 am</a>		</div>

		<p>I tried it with your code again, with the same result:<br>
real	1m13.286s<br>
user	0m1.748s<br>
sys	1m11.319s</p>
<p>While the naive implementation is like expected:<br>
real	0m4.137s<br>
user	0m3.662s<br>
sys	0m0.020s</p>
<p>It looks like that the program is not really using the coprocessor, but the software implementation of that feature.</p>
<p>It is possible that I overlooked some basics that are necessary in order to use the coprocessor. I was looking for some differences in the usage of newer vfp versions, compared to vfpv2, but I have the impression that it is backwards compatible.<br>
The output of gcc -v ( <a href="http://dpaste.com/0G2G8JH#wrap" rel="nofollow">http://dpaste.com/0G2G8JH#wrap</a> ) suggest that the OS is capable of utilizing the coprocessor.</p>
<p>Thank you for your quick reply. </p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/?replytocom=953125#respond" onclick="return addComment.moveForm( &quot;div-comment-953125&quot;, &quot;953125&quot;, &quot;respond&quot;, &quot;1003&quot; )" aria-label="Reply to Martin">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-4 parent" id="comment-953128">
				<div id="div-comment-953128" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-3">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/#comment-953128">
			November 28, 2015 at 11:24 am</a>		</div>

		<p>Hi Martin,</p>
<p>yes, it looks like your software stack (or at least the compiler) supports “hardware floating” support (usually dubbed as “armhf” or “gnueabihf”). Maybe the operating system doesn’t support it?</p>
<p>Still surprising, though. Looking forward to test it with a Raspi 2.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/?replytocom=953128#respond" onclick="return addComment.moveForm( &quot;div-comment-953128&quot;, &quot;953128&quot;, &quot;respond&quot;, &quot;1003&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment odd alt depth-5 parent" id="comment-955718">
				<div id="div-comment-955718" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/938463c4393c7262e73d6b0d099caba5?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/938463c4393c7262e73d6b0d099caba5?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/938463c4393c7262e73d6b0d099caba5?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-938463c4393c7262e73d6b0d099caba5-2">			<cite class="fn"><a href="http://pastie.org/10646266" rel="external nofollow" class="url">Martin</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/#comment-955718">
			December 22, 2015 at 1:40 am</a>		</div>

		<p>Just for reference: <a href="http://pastie.org/10646266" rel="nofollow">http://pastie.org/10646266</a><br>
This is my neon implementation of your 4×4 matrix multiplication. The main difference is that the neon coprocessor has these 128bit registers which offer some SIMD operations for the parallel processing.<br>
The load and store operations might be a bit messy in my implementation, but since we have only a 4×4 matrix we can easily compute the product of whole rows and columns and unroll another loop.<br>
`time ./benchmark` gives me ~0.6s, compared to ~3.5s for the naive implementation.</p>

		
				</div>
		</li><!-- #comment-## -->
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-5" id="comment-955746">
				<div id="div-comment-955746" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-4">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/#comment-955746">
			December 22, 2015 at 9:31 am</a>		</div>

		<p>Hi Martin,</p>
<p>thank you for your implementation using NEON. No doubt it is useful and illustrative, in particular the pairwise additions usage is neat.</p>
<p>Regarding the sequence of loads to load a column, I was wondering if <code>vld4.f32</code> would be useable here. It will require four <code>qX</code> registers (maybe you already run out of them?) but if I understand correctly this instruction, it will already have “transposed” the columns in the registers.</p>
<p>Kind regards,</p>

		
				</div>
		</li><!-- #comment-## -->
		<li class="comment odd alt depth-5 parent" id="comment-955768">
				<div id="div-comment-955768" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/938463c4393c7262e73d6b0d099caba5?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/938463c4393c7262e73d6b0d099caba5?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/938463c4393c7262e73d6b0d099caba5?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-938463c4393c7262e73d6b0d099caba5-3">			<cite class="fn">Martin</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/#comment-955768">
			December 22, 2015 at 11:18 am</a>		</div>

		<p>As far as I understand the vld instruction can only take 4 d- or 2 q- registers. There is some interleaving/deinterleaving functionality, but I have no clue how to realize the 16bit offset between the elements of a column.<br>
Concerning the load of the A-rows: If I do vld2.f32 {q0, q1}, [r4]!, the order is somehow different and I don’t see exactly why. (maybe the interleaving)</p>

		
				</div>
		</li><!-- #comment-## -->
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-5" id="comment-955868">
				<div id="div-comment-955868" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-5">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/#comment-955868">
			December 23, 2015 at 7:52 pm</a>		</div>

		<p>Hi Martin,</p>
<p>you’re right. Is not useful in this case.</p>
<p>I could not help writing my own version using NEON. You can find it here.</p>
<p>   <a href="https://gist.github.com/rofirrim/46147612cd00abc4d033" rel="nofollow">https://gist.github.com/rofirrim/46147612cd00abc4d033</a></p>
<p>First I tried doing 4 loads of the vector rows of B and then, using vector operations, transposing them as vector columns. But it happens to be faster to just do 16 scalar loads. Then it is simply a matter of doing dot products (i.e, element-wise multiplication and then the sum). I use a full vector row store, but may be 4 scalar stores is faster.</p>
<p>It is slightly faster than you version, but not much more: ~0.50 in the benchmark.</p>
<p>Kind regards,</p>
<p>P.S.: Note that the function should keep all the floating registers it is using. I didn’t bother to change the vpop/vpush but they should be updated.</p>

		
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-4 parent" id="comment-953380">
				<div id="div-comment-953380" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-6">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/#comment-953380">
			November 30, 2015 at 9:34 pm</a>		</div>

		<p>Hi Martin,</p>
<p>a friend of mine let me use his Raspberry Pi 2 and I was able to reproduce your results.</p>
<p>What happens is that ARMv7 deprecates VFP, so the processor of the Raspi 2 (Cortex A7) does not implement these instructions. Instead, the the Linux kernel implements them in software. Results are correct but they are terribly slow and defeat any possible performance gain.</p>
<p>According to ARM documentation, in ARMv7 you should use NEON instructions (also called Advanced SIMD).</p>
<p>Hope this clears it up.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/?replytocom=953380#respond" onclick="return addComment.moveForm( &quot;div-comment-953380&quot;, &quot;953380&quot;, &quot;respond&quot;, &quot;1003&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment even depth-5" id="comment-953472">
				<div id="div-comment-953472" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/938463c4393c7262e73d6b0d099caba5?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/938463c4393c7262e73d6b0d099caba5?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/938463c4393c7262e73d6b0d099caba5?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-938463c4393c7262e73d6b0d099caba5-4">			<cite class="fn">Martin</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/#comment-953472">
			December 1, 2015 at 9:15 pm</a>		</div>

		<p>Thank you for trying. So no backwards compatibility!<br>
I’ll search for the neon/advanced simd instructions and try to implement this.</p>
<p>Keep up the good work! </p>

		
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1" id="comment-953710">
				<div id="div-comment-953710" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/04d04f6918602e8c42d9945502f8bf32?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/04d04f6918602e8c42d9945502f8bf32?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-04d04f6918602e8c42d9945502f8bf32-0" originals="32" src-orig="http://0.gravatar.com/avatar/04d04f6918602e8c42d9945502f8bf32?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn"><a href="http://yahoo.com" rel="external nofollow" class="url">Robert Service</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/#comment-953710">
			December 3, 2015 at 5:27 pm</a>		</div>

		<p>“What happens is that ARMv7 deprecates VFP, so the processor of the Raspi 2 (Cortex A7) does not implement these instructions. Instead, the the Linux kernel implements them in software. Results are correct but they are terribly slow and defeat any possible performance gain.”</p>
<p>As one who writes code to perform beginning-to-end DSP tasks, this is a serious concern of mine.</p>
<p>Let me hasten to add that whatever negative<br>
consequences of using current tools (e.g., the OS) exist, this in no way minimizes the outstanding work you’re doing in clearly explaining this assembly language.<br>
Warmest regards…</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/?replytocom=953710#respond" onclick="return addComment.moveForm( &quot;div-comment-953710&quot;, &quot;953710&quot;, &quot;respond&quot;, &quot;1003&quot; )" aria-label="Reply to Robert Service">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		</ul>
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2013/05/12/arm-assembler-raspberry-pi-chapter-14/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="1003" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="623da734cc"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496665813676"></form>
			</div><!-- #respond -->
	</div>	</div>
