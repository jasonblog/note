# Integer SIMD


<div class="main">
			<h1>ARM assembler in Raspberry Pi – Chapter 25 </h1>
		<p class="post-meta"><span class="date">July 4, 2015</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2015/07/04/arm-assembler-raspberry-pi-chapter-25/#comments">6</a></span> </p>
		<p>
In chapter 13 we saw VFPv2 and the fact that it allows vectorial operations on floating-point numbers. You may be wondering if such a similar feature exists for integers. The answer is yes although in a more limited way.
</p>
<p><span id="more-2018"></span></p>
<h2>SIMD</h2>
<p>
SIMD stands for <em>single instruction multiple data</em> and means that an instruction can be used to perform the same operation on several operands at the same time. In chapter 13 and 14 we saw that by changing the <code>len</code> field in the <code>fpscr</code> and using at least one operand in the vectorial banks, then an instruction operated on <code>len</code> registers in the vectorial bank(s), effectively doing <code>len</code> times a floating point operation. This way, a single instruction like <code>vadd.f32</code> could then be used to perform up to 8 floating point additions. This strategy of speeding up computation is also called <em>data parallelism</em>.
</p>
<h3>SIMD with integers</h3>
<p>
SIMD support for integers exists also in ARMv6 but it is more limited: the multiple data are the subwords (see chapter 21) of a general purpose register. This means that we can do 2 operations on the 2 half words of a general purpose register. Similarly, we can do and up to 4 operations on the 4 bytes of a general purpose register.
</p>
<h2>Motivating example</h2>
<p>
At this point you may be wondering what is the purpose of this feature and why it does exist. Let’s assume we have two 16-bit PCM audio signals sampled at some frequency (i.e. 44.1kHz like in a CD Audio). This means that at the time of recording the “analog sound” of each channel is sampled many times per second and the sample, which represents the amplitude of the signal, is encoded using a 16-bit number.
</p>
<p>
An operation we may want to do is mixing the two signals in one signal (e.g. prior playing that final signal through the speakers). A (slightly incorrect) way to do this is by averaging the two signals. The code belows is a schema of what we want to do.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="cpp" style="font-family:monospace;"><span style="color: #0000ff;">short</span> <span style="color: #0000ff;">int</span> channel1<span style="color: #008000;">[</span>num_samples<span style="color: #008000;">]</span><span style="color: #008080;">;</span> <span style="color: #666666;">// in our environment a 'short int' is a half-word</span>
<span style="color: #0000ff;">short</span> <span style="color: #0000ff;">int</span> channel2<span style="color: #008000;">[</span>num_samples<span style="color: #008000;">]</span><span style="color: #008080;">;</span>
&nbsp;
<span style="color: #0000ff;">short</span> <span style="color: #0000ff;">int</span> channel_out<span style="color: #008000;">[</span>num_samples<span style="color: #008000;">]</span><span style="color: #008080;">;</span>
<span style="color: #0000ff;">for</span> <span style="color: #008000;">(</span>i <span style="color: #000080;">=</span> <span style="color: #0000dd;">0</span><span style="color: #008080;">;</span> i <span style="color: #000080;">&lt;</span> num_samples<span style="color: #008080;">;</span> i<span style="color: #000040;">++</span><span style="color: #008000;">)</span>
<span style="color: #008000;">{</span>
   channel_out<span style="color: #008000;">[</span>i<span style="color: #008000;">]</span> <span style="color: #000080;">=</span> <span style="color: #008000;">(</span>channel1<span style="color: #008000;">[</span>i<span style="color: #008000;">]</span> <span style="color: #000040;">+</span> channel2<span style="color: #008000;">[</span>i<span style="color: #008000;">]</span><span style="color: #008000;">)</span> <span style="color: #000040;">/</span> <span style="color: #0000dd;">2</span><span style="color: #008080;">;</span>
<span style="color: #008000;">}</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">short int channel1[num_samples]; // in our environment a 'short int' is a half-word
short int channel2[num_samples];

short int channel_out[num_samples];
for (i = 0; i &lt; num_samples; i++)
{
   channel_out[i] = (channel1[i] + channel2[i]) / 2;
}</p></div>

<p>
Now imagine we want to implement this in ARMv6. With our current knowledge the code would look like this (I will omit in these examples the AAPCS function call convention).
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">naive_channel_mixing<span style="color: #339933;">:</span>
    <span style="color: #339933;">/*</span> r0 contains the base address of channel1 <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r1 contains the base address of channel2 <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r2 contains the base address of channel_out <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r3 is the number of samples <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r4 is the number of the current sample
          so it holds that <span style="color: #ff0000;">0</span> ≤ r4 &lt; r3 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>              <span style="color: #339933;">/*</span> r4 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    b <span style="color: #339933;">.</span>Lcheck_loop          <span style="color: #339933;">/*</span> branch to check_loop <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>Lloop<span style="color: #339933;">:</span>
      <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">1</span>    <span style="color: #339933;">/*</span> r5 ← r4 &lt;&lt; <span style="color: #ff0000;">1</span> <span style="color: #009900; font-weight: bold;">(</span>this is r5 ← r4 <span style="color: #339933;">*</span> <span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
                            <span style="color: #339933;">/*</span> a halfword takes two bytes<span style="color: #339933;">,</span> so multiply
                               the index by two<span style="color: #339933;">.</span> We <span style="color: #0000ff; font-weight: bold;">do</span> this here because
                               ldrsh does <span style="color: #00007f; font-weight: bold;">not</span> allow an addressing mode
                               like <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
      ldrsh r6<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #339933;">,</span> r5<span style="color: #009900; font-weight: bold;">]</span>    <span style="color: #339933;">/*</span> r6 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">{</span>signed half<span style="color: #009900; font-weight: bold;">}</span><span style="color: #009900; font-weight: bold;">(</span>r0 <span style="color: #339933;">+</span> r5<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
      ldrsh r7<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #339933;">,</span> r5<span style="color: #009900; font-weight: bold;">]</span>    <span style="color: #339933;">/*</span> r7 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">{</span>signed half<span style="color: #009900; font-weight: bold;">}</span><span style="color: #009900; font-weight: bold;">(</span>r1 <span style="color: #339933;">+</span> r5<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">add</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> r7        <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r8</span> ← r6 <span style="color: #339933;">+</span> r7 <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">mov</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> ASR #<span style="color: #ff0000;">1</span>    <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r8</span> ← <span style="color: #46aa03; font-weight: bold;">r8</span> &gt;&gt; <span style="color: #ff0000;">1</span> <span style="color: #009900; font-weight: bold;">(</span>this is <span style="color: #46aa03; font-weight: bold;">r8</span> ← <span style="color: #46aa03; font-weight: bold;">r8</span> <span style="color: #339933;">/</span> <span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">*/</span>
      strh <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r2<span style="color: #339933;">,</span> r5<span style="color: #009900; font-weight: bold;">]</span>     <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">{</span>half<span style="color: #009900; font-weight: bold;">}</span><span style="color: #009900; font-weight: bold;">(</span>r2 <span style="color: #339933;">+</span> r5<span style="color: #009900; font-weight: bold;">)</span> ← <span style="color: #46aa03; font-weight: bold;">r8</span> <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">add</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>        <span style="color: #339933;">/*</span> r4 ← r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>Lcheck_loop<span style="color: #339933;">:</span> 
      <span style="color: #00007f; font-weight: bold;">cmp</span> r4<span style="color: #339933;">,</span> r3            <span style="color: #339933;">/*</span> compute r4 <span style="color: #339933;">-</span> r3 <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
      blt <span style="color: #339933;">.</span>Lloop            <span style="color: #339933;">/*</span> if r4 &lt; r3 jump to the
                               beginning of the <span style="color: #00007f; font-weight: bold;">loop</span> <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">naive_channel_mixing:
    /* r0 contains the base address of channel1 */
    /* r1 contains the base address of channel2 */
    /* r2 contains the base address of channel_out */
    /* r3 is the number of samples */
    /* r4 is the number of the current sample
          so it holds that 0 ≤ r4 &lt; r3 */
    
    mov r4, #0              /* r4 ← 0 */
    b .Lcheck_loop          /* branch to check_loop */
    .Lloop:
      mov r5, r4, LSL #1    /* r5 ← r4 &lt;&lt; 1 (this is r5 ← r4 * 2) */
                            /* a halfword takes two bytes, so multiply
                               the index by two. We do this here because
                               ldrsh does not allow an addressing mode
                               like [r0, r5, LSL #1] */
      ldrsh r6, [r0, r5]    /* r6 ← *{signed half}(r0 + r5) */
      ldrsh r7, [r1, r5]    /* r7 ← *{signed half}(r1 + r5) */
      add r8, r6, r7        /* r8 ← r6 + r7 */
      mov r8, r8, ASR #1    /* r8 ← r8 &gt;&gt; 1 (this is r8 ← r8 / 2)*/
      strh r8, [r2, r5]     /* *{half}(r2 + r5) ← r8 */
      add r4, r4, #1        /* r4 ← r4 + 1 */
    .Lcheck_loop: 
      cmp r4, r3            /* compute r4 - r3 and update cpsr */
      blt .Lloop            /* if r4 &lt; r3 jump to the
                               beginning of the loop */</p></div>

<p>
We could probably be happy with this code but if you were in the business of designing processors for embedded devices you would probably be sensitive to your customer codes. And chances are that your portable MP3 player (or any gadget able to play music) is “ARM inside”. So this is a code that is eligible for improvement from an architecture point of view.
</p>
<h2>Parallel additions and subtractions</h2>
<p>
ARMv6 data parallel instructions allow us to add/subtract the corresponding half words or bytes. It provides them both for unsigned integers and signed integers.
</p>
<ul>
<li>Halfwords
<ul>
<li>Signed: <code>sadd16</code>, <code>ssub16</code>
</li><li>Unsigned: <code>uadd16</code>, <code>usub16</code>
</li></ul>
</li><li>Bytes
<ul>
<li>Signed: <code>sadd8</code>, <code>ssub8</code>
</li><li>Unsigned: <code>uadd8</code>, <code>usub8</code>
</li></ul>
</li></ul>
<p>
It should not be hard to find obvious uses for these instructions. For instance, the following loop can benefit from the <code>uadd8</code> instruction.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #666666; font-style: italic;">// unsigned char is an unsigned byte in our environment</span>
<span style="color: #666666; font-style: italic;">// a, b and c are arrays of N unsigned chars</span>
<span style="color: #993333;">unsigned</span> <span style="color: #993333;">char</span> a<span style="color: #009900;">[</span>N<span style="color: #009900;">]</span><span style="color: #339933;">,</span> b<span style="color: #009900;">[</span>N<span style="color: #009900;">]</span><span style="color: #339933;">,</span> c<span style="color: #009900;">[</span>N<span style="color: #009900;">]</span><span style="color: #339933;">;</span>
&nbsp;
<span style="color: #993333;">int</span> i<span style="color: #339933;">;</span>
<span style="color: #b1b100;">for</span> <span style="color: #009900;">(</span>i <span style="color: #339933;">=</span> <span style="color: #0000dd;">0</span><span style="color: #339933;">;</span> i <span style="color: #339933;">&lt;</span> N<span style="color: #339933;">;</span> i<span style="color: #339933;">++</span><span style="color: #009900;">)</span>
<span style="color: #009900;">{</span>
  c<span style="color: #009900;">[</span>i<span style="color: #009900;">]</span> <span style="color: #339933;">=</span> a<span style="color: #009900;">[</span>i<span style="color: #009900;">]</span> <span style="color: #339933;">+</span> b<span style="color: #009900;">[</span>i<span style="color: #009900;">]</span><span style="color: #339933;">;</span>
<span style="color: #009900;">}</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">// unsigned char is an unsigned byte in our environment
// a, b and c are arrays of N unsigned chars
unsigned char a[N], b[N], c[N];

int i;
for (i = 0; i &lt; N; i++)
{
  c[i] = a[i] + b[i];
}</p></div>

<p>
Let’s first write a naive approach to the above loop, which is similar to the one in the beginning of the post.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">naive_byte_array_addition<span style="color: #339933;">:</span>
    <span style="color: #339933;">/*</span> r0 contains the base address of a <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r1 contains the base address of b <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r2 contains the base address of c <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r3 is N <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r4 is the number of the current item
          so it holds that <span style="color: #ff0000;">0</span> ≤ r4 &lt; r3 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>             <span style="color: #339933;">/*</span> r4 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    b <span style="color: #339933;">.</span>Lcheck_loop0        <span style="color: #339933;">/*</span> branch to check_loop0 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">.</span>Lloop0<span style="color: #339933;">:</span>
      ldrb r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #339933;">,</span> r4<span style="color: #009900; font-weight: bold;">]</span>    <span style="color: #339933;">/*</span> r5 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">{</span>unsigned <span style="color: #0000ff; font-weight: bold;">byte</span><span style="color: #009900; font-weight: bold;">}</span><span style="color: #009900; font-weight: bold;">(</span>r0 <span style="color: #339933;">+</span> r4<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
      ldrb r6<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #339933;">,</span> r4<span style="color: #009900; font-weight: bold;">]</span>    <span style="color: #339933;">/*</span> r6 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">{</span>unsigned <span style="color: #0000ff; font-weight: bold;">byte</span><span style="color: #009900; font-weight: bold;">}</span><span style="color: #009900; font-weight: bold;">(</span>r1 <span style="color: #339933;">+</span> r4<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">add</span> r7<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6       <span style="color: #339933;">/*</span> r7 ← r5 <span style="color: #339933;">+</span> r6 <span style="color: #339933;">*/</span>
      strb r7<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r2<span style="color: #339933;">,</span> r4<span style="color: #009900; font-weight: bold;">]</span>    <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">{</span>unsigned <span style="color: #0000ff; font-weight: bold;">byte</span><span style="color: #009900; font-weight: bold;">}</span><span style="color: #009900; font-weight: bold;">(</span>r2 <span style="color: #339933;">+</span> r4<span style="color: #009900; font-weight: bold;">)</span> ← r7 <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">add</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>       <span style="color: #339933;">/*</span> r4 ← r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>Lcheck_loop0<span style="color: #339933;">:</span>
       <span style="color: #00007f; font-weight: bold;">cmp</span> r4<span style="color: #339933;">,</span> r3          <span style="color: #339933;">/*</span> perform r4 <span style="color: #339933;">-</span> r3 <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
       blt <span style="color: #339933;">.</span>Lloop0         <span style="color: #339933;">/*</span> if cpsr means that r4 &lt; r3 jump to loop0 <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">naive_byte_array_addition:
    /* r0 contains the base address of a */
    /* r1 contains the base address of b */
    /* r2 contains the base address of c */
    /* r3 is N */
    /* r4 is the number of the current item
          so it holds that 0 ≤ r4 &lt; r3 */

    mov r4, #0             /* r4 ← 0 */
    b .Lcheck_loop0        /* branch to check_loop0 */

    .Lloop0:
      ldrb r5, [r0, r4]    /* r5 ← *{unsigned byte}(r0 + r4) */
      ldrb r6, [r1, r4]    /* r6 ← *{unsigned byte}(r1 + r4) */
      add r7, r5, r6       /* r7 ← r5 + r6 */
      strb r7, [r2, r4]    /* *{unsigned byte}(r2 + r4) ← r7 */
      add r4, r4, #1       /* r4 ← r4 + 1 */
    .Lcheck_loop0:
       cmp r4, r3          /* perform r4 - r3 and update cpsr */
       blt .Lloop0         /* if cpsr means that r4 &lt; r3 jump to loop0 */</p></div>

<p>
This loop again is fine but we can do better by using the instruction <code>uadd8</code>. Note that now we will be able to add 4 bytes at a time. This means that we will have to increment <code>r4</code> by 4.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">simd_byte_array_addition_0<span style="color: #339933;">:</span>
    <span style="color: #339933;">/*</span> r0 contains the base address of a <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r1 contains the base address of b <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r2 contains the base address of c <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r3 is N <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r4 is the number of the current item
          so it holds that <span style="color: #ff0000;">0</span> ≤ r4 &lt; r3 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>             <span style="color: #339933;">/*</span> r4 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    b <span style="color: #339933;">.</span>Lcheck_loop1        <span style="color: #339933;">/*</span> branch to check_loop1 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">.</span>Lloop1<span style="color: #339933;">:</span>
      ldr r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #339933;">,</span> r4<span style="color: #009900; font-weight: bold;">]</span>     <span style="color: #339933;">/*</span> r5 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r0 <span style="color: #339933;">+</span> r4<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
      ldr r6<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #339933;">,</span> r4<span style="color: #009900; font-weight: bold;">]</span>     <span style="color: #339933;">/*</span> r6 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r1 <span style="color: #339933;">+</span> r4<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
      sadd8 r7<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6     <span style="color: #339933;">/*</span> r7<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">7</span><span style="color: #339933;">:</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span> ← r5<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">7</span><span style="color: #339933;">:</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">+</span> r6<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">7</span><span style="color: #339933;">:</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
                           <span style="color: #339933;">/*</span> r7<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">15</span><span style="color: #339933;">:</span><span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span> ← r5<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">15</span><span style="color: #339933;">:</span><span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">+</span> r6<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">15</span><span style="color: #339933;">:</span><span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
                           <span style="color: #339933;">/*</span> r7<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">23</span><span style="color: #339933;">:</span><span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">]</span> ← r5<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">23</span><span style="color: #339933;">:</span><span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">+</span> r6<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">23</span><span style="color: #339933;">:</span><span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
                           <span style="color: #339933;">/*</span> r7<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">31</span><span style="color: #339933;">:</span><span style="color: #ff0000;">24</span><span style="color: #009900; font-weight: bold;">]</span> ← r5<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">31</span><span style="color: #339933;">:</span><span style="color: #ff0000;">24</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">+</span> r6<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">31</span><span style="color: #339933;">:</span><span style="color: #ff0000;">24</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
                           <span style="color: #339933;">/*</span> rA<span style="color: #009900; font-weight: bold;">[</span>x<span style="color: #339933;">:</span>y<span style="color: #009900; font-weight: bold;">]</span> means <span style="color: #0000ff; font-weight: bold;">bits</span> x to y of the register rA <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">str</span> r7<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r2<span style="color: #339933;">,</span> r4<span style="color: #009900; font-weight: bold;">]</span>     <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r2 <span style="color: #339933;">+</span> r4<span style="color: #009900; font-weight: bold;">)</span> ← r7 <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">add</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>       <span style="color: #339933;">/*</span> r4 ← r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span> <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>Lcheck_loop1<span style="color: #339933;">:</span>
       <span style="color: #00007f; font-weight: bold;">cmp</span> r4<span style="color: #339933;">,</span> r3          <span style="color: #339933;">/*</span> perform r4 <span style="color: #339933;">-</span> r3 <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
       blt <span style="color: #339933;">.</span>Lloop1         <span style="color: #339933;">/*</span> if cpsr means that r4 &lt; r3 jump to loop1 <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">simd_byte_array_addition_0:
    /* r0 contains the base address of a */
    /* r1 contains the base address of b */
    /* r2 contains the base address of c */
    /* r3 is N */
    /* r4 is the number of the current item
          so it holds that 0 ≤ r4 &lt; r3 */

    mov r4, #0             /* r4 ← 0 */
    b .Lcheck_loop1        /* branch to check_loop1 */

    .Lloop1:
      ldr r5, [r0, r4]     /* r5 ← *(r0 + r4) */
      ldr r6, [r1, r4]     /* r6 ← *(r1 + r4) */
      sadd8 r7, r5, r6     /* r7[7:0] ← r5[7:0] + r6[7:0] */
                           /* r7[15:8] ← r5[15:8] + r6[15:8] */
                           /* r7[23:16] ← r5[23:16] + r6[23:16] */
                           /* r7[31:24] ← r5[31:24] + r6[31:24] */
                           /* rA[x:y] means bits x to y of the register rA */
      str r7, [r2, r4]     /* *(r2 + r4) ← r7 */
      add r4, r4, #4       /* r4 ← r4 + 4 */
    .Lcheck_loop1:
       cmp r4, r3          /* perform r4 - r3 and update cpsr */
       blt .Lloop1         /* if cpsr means that r4 &lt; r3 jump to loop1 */</p></div>

<p>
A subtlety of the above code is that it only works if <code>N</code> (kept in <code>r3</code>) is a multiple of 4. If it is not the case (and this includes when 0 ≤ r3 &lt; 4), then the loop will do fewer iterations than expected. If we know that <code>N</code> is a multiple of 4, then nothing else must be done. But if it may be not a multiple of 4, we will need what is called an <em>epilog loop</em>, for the remaining cases. Note that in our case, the epilog loop will have to do 0 (if N was a multiple of 4), 1, 2 or 3 iterations. We can implement it as a switch with 4 cases plus fall-through (see chapter 16) or if we are concerned about code size, with a loop. We will use a loop.
</p>
<p>
We cannot, though, simply append an epilog loop to the above loop,because it is actually doing more work than we want. When N is not a multiple of four, the last iteration will add 1, 2 or 3 more bytes that do not belong to the original array. This is a recipe for a disaster so we have to avoid this. We need to make sure that when we are in the loop, <code>r4</code> is such that <code>r4</code>, <code>r4 + 1</code>, <code>r4 + 2</code> and <code>r4 + 3</code> are valid elements of the array. This means that we should check that <code>r4 &lt; N</code>, <code>r4 + 1 &lt; N</code>,<code> r4 + 2 &lt; N</code> and <code>r4 + 3 &lt; N</code>. Since the last of these four implies the first three, it is enough to check that <code>r4 + 3 &lt; N</code>.
</p>
<p>
Note that checking <code>r4 + 3 &lt; N</code> would force us to compute <code>r4 + 3</code> at every iteration in the loop, but we do not have to. Checking <code>r4 + 3 &lt; N</code> is equivalent to check <code>r4 &lt; N - 3</code>. <code>N - 3</code> does not depend on <code>r4</code> so it can be computed before the loop.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">simd_byte_array_addition_2<span style="color: #339933;">:</span>
    <span style="color: #339933;">/*</span> r0 contains the base address of a <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r1 contains the base address of b <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r2 contains the base address of c <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r3 is N <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r4 is the number of the current item
          so it holds that <span style="color: #ff0000;">0</span> ≤ r4 &lt; r3 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>             <span style="color: #339933;">/*</span> r4 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">sub</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r3<span style="color: #339933;">,</span> #<span style="color: #ff0000;">3</span>         <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r8</span> ← r3 <span style="color: #339933;">-</span> <span style="color: #ff0000;">3</span>
                              this is <span style="color: #46aa03; font-weight: bold;">r8</span> ← N <span style="color: #339933;">-</span> <span style="color: #ff0000;">3</span> <span style="color: #339933;">*/</span>
    b <span style="color: #339933;">.</span>Lcheck_loop2        <span style="color: #339933;">/*</span> branch to check_loop2 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">.</span>Lloop2<span style="color: #339933;">:</span>
      ldr r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #339933;">,</span> r4<span style="color: #009900; font-weight: bold;">]</span>     <span style="color: #339933;">/*</span> r5 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r0 <span style="color: #339933;">+</span> r4<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
      ldr r6<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #339933;">,</span> r4<span style="color: #009900; font-weight: bold;">]</span>     <span style="color: #339933;">/*</span> r6 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r1 <span style="color: #339933;">+</span> r4<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
      sadd8 r7<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6     <span style="color: #339933;">/*</span> r7<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">7</span><span style="color: #339933;">:</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span> ← r5<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">7</span><span style="color: #339933;">:</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">+</span> r6<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">7</span><span style="color: #339933;">:</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
                           <span style="color: #339933;">/*</span> r7<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">15</span><span style="color: #339933;">:</span><span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span> ← r5<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">15</span><span style="color: #339933;">:</span><span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">+</span> r6<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">15</span><span style="color: #339933;">:</span><span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
                           <span style="color: #339933;">/*</span> r7<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">23</span><span style="color: #339933;">:</span><span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">]</span> ← r5<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">23</span><span style="color: #339933;">:</span><span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">+</span> r6<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">23</span><span style="color: #339933;">:</span><span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
                           <span style="color: #339933;">/*</span> r7<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">31</span><span style="color: #339933;">:</span><span style="color: #ff0000;">24</span><span style="color: #009900; font-weight: bold;">]</span> ← r5<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">31</span><span style="color: #339933;">:</span><span style="color: #ff0000;">24</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">+</span> r6<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">31</span><span style="color: #339933;">:</span><span style="color: #ff0000;">24</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">str</span> r7<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r2<span style="color: #339933;">,</span> r4<span style="color: #009900; font-weight: bold;">]</span>     <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r2 <span style="color: #339933;">+</span> r4<span style="color: #009900; font-weight: bold;">)</span> ← r7 <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">add</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>       <span style="color: #339933;">/*</span> r4 ← r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span> <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>Lcheck_loop2<span style="color: #339933;">:</span>
       <span style="color: #00007f; font-weight: bold;">cmp</span> r4<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span>          <span style="color: #339933;">/*</span> perform r4 <span style="color: #339933;">-</span> <span style="color: #46aa03; font-weight: bold;">r8</span> <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
       blt <span style="color: #339933;">.</span>Lloop2         <span style="color: #339933;">/*</span> if cpsr means that r4 &lt; <span style="color: #46aa03; font-weight: bold;">r8</span> jump to loop2 <span style="color: #339933;">*/</span>
                           <span style="color: #339933;">/*</span> i<span style="color: #339933;">.</span>e<span style="color: #339933;">.</span> if r4 &lt; N <span style="color: #339933;">-</span> <span style="color: #ff0000;">3</span> jump to loop2 <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">simd_byte_array_addition_2:
    /* r0 contains the base address of a */
    /* r1 contains the base address of b */
    /* r2 contains the base address of c */
    /* r3 is N */
    /* r4 is the number of the current item
          so it holds that 0 ≤ r4 &lt; r3 */

    mov r4, #0             /* r4 ← 0 */
    sub r8, r3, #3         /* r8 ← r3 - 3
                              this is r8 ← N - 3 */
    b .Lcheck_loop2        /* branch to check_loop2 */

    .Lloop2:
      ldr r5, [r0, r4]     /* r5 ← *(r0 + r4) */
      ldr r6, [r1, r4]     /* r6 ← *(r1 + r4) */
      sadd8 r7, r5, r6     /* r7[7:0] ← r5[7:0] + r6[7:0] */
                           /* r7[15:8] ← r5[15:8] + r6[15:8] */
                           /* r7[23:16] ← r5[23:16] + r6[23:16] */
                           /* r7[31:24] ← r5[31:24] + r6[31:24] */
      str r7, [r2, r4]     /* *(r2 + r4) ← r7 */
      add r4, r4, #4       /* r4 ← r4 + 4 */
    .Lcheck_loop2:
       cmp r4, r8          /* perform r4 - r8 and update cpsr */
       blt .Lloop2         /* if cpsr means that r4 &lt; r8 jump to loop2 */
                           /* i.e. if r4 &lt; N - 3 jump to loop2 */</p></div>

<p>
In line 10 where we compute <code>r8</code> which will keep <code>N - 3</code>, we use it in line 24 to check the loop iteration.
</p>
<p>
The epilog loop follows.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>27
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">     <span style="color: #339933;">/*</span> epilog <span style="color: #00007f; font-weight: bold;">loop</span> <span style="color: #339933;">*/</span>
     b <span style="color: #339933;">.</span>Lcheck_loop3       <span style="color: #339933;">/*</span> branch to check_loop3 <span style="color: #339933;">*/</span>
&nbsp;
     <span style="color: #339933;">.</span>Lloop3<span style="color: #339933;">:</span>
        ldrb r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #339933;">,</span> r4<span style="color: #009900; font-weight: bold;">]</span>  <span style="color: #339933;">/*</span> r5 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">{</span>unsigned <span style="color: #0000ff; font-weight: bold;">byte</span><span style="color: #009900; font-weight: bold;">}</span><span style="color: #009900; font-weight: bold;">(</span>r0 <span style="color: #339933;">+</span> r4<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
        ldrb r6<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #339933;">,</span> r4<span style="color: #009900; font-weight: bold;">]</span>  <span style="color: #339933;">/*</span> r6 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">{</span>unsigned <span style="color: #0000ff; font-weight: bold;">byte</span><span style="color: #009900; font-weight: bold;">}</span><span style="color: #009900; font-weight: bold;">(</span>r1 <span style="color: #339933;">+</span> r4<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">add</span> r7<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6     <span style="color: #339933;">/*</span> r7 ← r5 <span style="color: #339933;">+</span> r6 <span style="color: #339933;">*/</span>
        strb r7<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r2<span style="color: #339933;">,</span> r4<span style="color: #009900; font-weight: bold;">]</span>  <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">{</span>unsigned <span style="color: #0000ff; font-weight: bold;">byte</span><span style="color: #009900; font-weight: bold;">}</span><span style="color: #009900; font-weight: bold;">(</span>r2 <span style="color: #339933;">+</span> r4<span style="color: #009900; font-weight: bold;">)</span> ← r7 <span style="color: #339933;">*/</span>
&nbsp;
        <span style="color: #00007f; font-weight: bold;">add</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>     <span style="color: #339933;">/*</span> r4 ← r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
     <span style="color: #339933;">.</span>Lcheck_loop3<span style="color: #339933;">:</span>
        <span style="color: #00007f; font-weight: bold;">cmp</span> r4<span style="color: #339933;">,</span> r3         <span style="color: #339933;">/*</span> perform r4 <span style="color: #339933;">-</span> r3 <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
        blt <span style="color: #339933;">.</span>Lloop3        <span style="color: #339933;">/*</span> if cpsr means that r4 &lt; r3 jump to <span style="color: #00007f; font-weight: bold;">loop</span> <span style="color: #ff0000;">3</span> <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">     /* epilog loop */
     b .Lcheck_loop3       /* branch to check_loop3 */

     .Lloop3:
        ldrb r5, [r0, r4]  /* r5 ← *{unsigned byte}(r0 + r4) */
        ldrb r6, [r1, r4]  /* r6 ← *{unsigned byte}(r1 + r4) */
        add r7, r5, r6     /* r7 ← r5 + r6 */
        strb r7, [r2, r4]  /* *{unsigned byte}(r2 + r4) ← r7 */

        add r4, r4, #1     /* r4 ← r4 + 1 */
     .Lcheck_loop3:
        cmp r4, r3         /* perform r4 - r3 and update cpsr */
        blt .Lloop3        /* if cpsr means that r4 &lt; r3 jump to loop 3 */</p></div>

<p>
The epilog loop is like the naive one, but it will only run 0, 1, 2 or 3 iterations. This means that for big enough values of <code>N</code>, in practice all iterations will use the data parallel instructions and only up to 3 will have to use the slower approach.
</p>
<h2>Halving instructions</h2>
<p>
The data parallel instructions also come in a form where the addition/subtraction is halved. This means that it is possible to compute averages of half words and bytes easily.
</p>
<ul>
<li>Halfwords
<ul>
<li>Signed: <code>shadd16</code>, <code>shsub16</code>
</li><li>Unsigned: <code>uhadd16</code>, <code>uhsub16</code>
</li></ul>
</li><li>Bytes
<ul>
<li>Signed: <code>shadd8</code>, <code>shsub8</code>
</li><li>Unsigned: <code>uhadd8</code>, <code>uhsub8</code>
</li></ul>
</li></ul>
<p>
Thus, the motivating example of the beginning of the post can be implemented using the <code>shsub16</code> instruction. For simplicity, let's assume that <code>num_samples</code> is a multiple of 2 (now we are dealing with halfwords) so no epilog is necessary.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">better_channel_mixing<span style="color: #339933;">:</span>
    <span style="color: #339933;">/*</span> r0 contains the base address of channel1 <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r1 contains the base address of channel2 <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r2 contains the base address of channel_out <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r3 is the number of samples <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r4 is the number of the current sample
          so it holds that <span style="color: #ff0000;">0</span> ≤ r4 &lt; r3 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>              <span style="color: #339933;">/*</span> r4 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    b <span style="color: #339933;">.</span>Lcheck_loop          <span style="color: #339933;">/*</span> branch to check_loop <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>Lloop<span style="color: #339933;">:</span>
      ldr r6<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #339933;">,</span> r4<span style="color: #009900; font-weight: bold;">]</span>      <span style="color: #339933;">/*</span> r6 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r0 <span style="color: #339933;">+</span> r4<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
      ldr r7<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #339933;">,</span> r4<span style="color: #009900; font-weight: bold;">]</span>      <span style="color: #339933;">/*</span> r7 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r1 <span style="color: #339933;">+</span> r4<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
      shadd16 <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> r7    <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">15</span><span style="color: #339933;">:</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span> ← <span style="color: #009900; font-weight: bold;">(</span>r6<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">15</span><span style="color: #339933;">:</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">+</span> r7<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">15</span><span style="color: #339933;">:</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">)</span> &gt;&gt; <span style="color: #ff0000;">1</span><span style="color: #339933;">*/</span>
                            <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">31</span><span style="color: #339933;">:</span><span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">]</span> ← <span style="color: #009900; font-weight: bold;">(</span>r6<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">31</span><span style="color: #339933;">:</span><span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">+</span> r7<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">31</span><span style="color: #339933;">:</span><span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">)</span> &gt;&gt; <span style="color: #ff0000;">1</span><span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">str</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r2<span style="color: #339933;">,</span> r4<span style="color: #009900; font-weight: bold;">]</span>      <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r2 <span style="color: #339933;">+</span> r4<span style="color: #009900; font-weight: bold;">)</span> ← <span style="color: #46aa03; font-weight: bold;">r8</span> <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">add</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span>        <span style="color: #339933;">/*</span> r4 ← r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">2</span> <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>Lcheck_loop<span style="color: #339933;">:</span>
      <span style="color: #00007f; font-weight: bold;">cmp</span> r4<span style="color: #339933;">,</span> r3            <span style="color: #339933;">/*</span> compute r4 <span style="color: #339933;">-</span> r3 <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
      blt <span style="color: #339933;">.</span>Lloop            <span style="color: #339933;">/*</span> if r4 &lt; r3 jump to the
                               beginning of the <span style="color: #00007f; font-weight: bold;">loop</span> <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">better_channel_mixing:
    /* r0 contains the base address of channel1 */
    /* r1 contains the base address of channel2 */
    /* r2 contains the base address of channel_out */
    /* r3 is the number of samples */
    /* r4 is the number of the current sample
          so it holds that 0 ≤ r4 &lt; r3 */

    mov r4, #0              /* r4 ← 0 */
    b .Lcheck_loop          /* branch to check_loop */
    .Lloop:
      ldr r6, [r0, r4]      /* r6 ← *(r0 + r4) */
      ldr r7, [r1, r4]      /* r7 ← *(r1 + r4) */
      shadd16 r8, r6, r7    /* r8[15:0] ← (r6[15:0] + r7[15:0]) &gt;&gt; 1*/
                            /* r8[31:16] ← (r6[31:16] + r7[31:16]) &gt;&gt; 1*/
      str r8, [r2, r4]      /* *(r2 + r4) ← r8 */
      add r4, r4, #2        /* r4 ← r4 + 2 */
    .Lcheck_loop:
      cmp r4, r3            /* compute r4 - r3 and update cpsr */
      blt .Lloop            /* if r4 &lt; r3 jump to the
                               beginning of the loop */</p></div>

<h2>Saturating arithmetic</h2>
<p>
Let's go back to our motivating example. We averaged the two 16-bit channels to mix them but, in reality, mixing is achieved by just adding the two channels. In general this is OK because signals are not correlated and the amplitude of a mixed sample usually can be encoded in 16-bit. Sometimes, though, the mixed sample may have an amplitude that falls outside the 16-bit range. In this case we want to clip the sample within the representable range. A sample with a too positive amplitude will be clipped to 2<sup>15</sup>-1, a sample with a too negative amplitude will be clipped to -2<sup>15</sup>.
</p>
<p>
With lack of hardware support, clipping can be implemented by checking overflow after each addition. So, every addition should check that the resulting number is in the interval [-32768, 32767]<br>
Let's write a function that adds two 32-bit integers and clips them in the 16-bit range.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #0000ff; font-weight: bold;">.data</span>
max16bit<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">32767</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
clipped_add16bit<span style="color: #339933;">:</span>
    <span style="color: #339933;">/*</span> first operand is <span style="color: #00007f; font-weight: bold;">in</span> r0 <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> second operand is <span style="color: #00007f; font-weight: bold;">in</span> r0 <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> result is left <span style="color: #00007f; font-weight: bold;">in</span> r0 <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>             <span style="color: #339933;">/*</span> keep registers <span style="color: #339933;">*/</span>
&nbsp;
    ldr r4<span style="color: #339933;">,</span> addr_of_max16bit  <span style="color: #339933;">/*</span> r4 ← &amp;max16bit <span style="color: #339933;">*/</span>
    ldr r4<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #009900; font-weight: bold;">]</span>              <span style="color: #339933;">/*</span> r4 ← <span style="color: #339933;">*</span>r4 <span style="color: #339933;">*/</span>
                              <span style="color: #339933;">/*</span> now r4 == <span style="color: #ff0000;">32767</span> <span style="color: #009900; font-weight: bold;">(</span>i<span style="color: #339933;">.</span>e<span style="color: #339933;">.</span> <span style="color: #ff0000;">2</span>^<span style="color: #ff0000;">15</span> <span style="color: #339933;">-</span> <span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">add</span> r0<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> r1            <span style="color: #339933;">/*</span> r0 ← r0 <span style="color: #339933;">+</span> r1 <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> r4                <span style="color: #339933;">/*</span> perform r0 <span style="color: #339933;">-</span> r4 <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
    movgt r0<span style="color: #339933;">,</span> r4              <span style="color: #339933;">/*</span> if r0 &gt; r4 then r0 ← r4 <span style="color: #339933;">*/</span>
    bgt end                   <span style="color: #339933;">/*</span> if r0 &gt; r4 then branch to end <span style="color: #339933;">*/</span>
&nbsp;
    mvn r4<span style="color: #339933;">,</span> r4                <span style="color: #339933;">/*</span> r4 ← ~r4
                                 now r4 == <span style="color: #339933;">-</span><span style="color: #ff0000;">32768</span> <span style="color: #009900; font-weight: bold;">(</span>i<span style="color: #339933;">.</span>e<span style="color: #339933;">.</span> <span style="color: #339933;">-</span><span style="color: #ff0000;">2</span>^<span style="color: #ff0000;">15</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> r4                <span style="color: #339933;">/*</span> perform r0 <span style="color: #339933;">-</span> r4 <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
    movlt r0<span style="color: #339933;">,</span> r4              <span style="color: #339933;">/*</span> if r0 &lt; r4 then r0 ← r4 <span style="color: #339933;">*/</span>
&nbsp;
    end<span style="color: #339933;">:</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>              <span style="color: #339933;">/*</span> restore registers <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr                     <span style="color: #339933;">/*</span> return <span style="color: #339933;">*/</span>
addr_of_max16bit<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> max16bit</pre></td></tr></tbody></table><p class="theCode" style="display:none;">.data
max16bit: .word 32767

.text

clipped_add16bit:
    /* first operand is in r0 */
    /* second operand is in r0 */
    /* result is left in r0 */
    push {r4, lr}             /* keep registers */

    ldr r4, addr_of_max16bit  /* r4 ← &amp;max16bit */
    ldr r4, [r4]              /* r4 ← *r4 */
                              /* now r4 == 32767 (i.e. 2^15 - 1) */

    add r0, r0, r1            /* r0 ← r0 + r1 */
    cmp r0, r4                /* perform r0 - r4 and update cpsr */
    movgt r0, r4              /* if r0 &gt; r4 then r0 ← r4 */
    bgt end                   /* if r0 &gt; r4 then branch to end */

    mvn r4, r4                /* r4 ← ~r4
                                 now r4 == -32768 (i.e. -2^15) */
    cmp r0, r4                /* perform r0 - r4 and update cpsr */
    movlt r0, r4              /* if r0 &lt; r4 then r0 ← r4 */

    end:

    pop {r4, lr}              /* restore registers */
    bx lr                     /* return */
addr_of_max16bit: .word max16bit</p></div>

<p>
As you can see, a seemingly simple addition that clips the result requires a bunch of instructions. As before, the code is correct but we can do much better thanks to the <em>saturated arithmetics</em> instructions of ARMv6.
</p>
<ul>
<li>Halfwords
<ul>
<li>Signed: <code>qadd16</code>, <code>qsub16</code>
</li><li>Unsigned: <code>uqadd16</code>, <code>uqsub16</code>
</li></ul>
</li><li>Bytes
<ul>
<li>Signed: <code>qadd8</code>, <code>qsub8</code>
</li><li>Unsigned: <code>uqadd8</code>, <code>uqsub8</code>
</li></ul>
</li></ul>
<p>
Now we can write a more realistic mixing of two channels.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">more_realistic_channel_mixing<span style="color: #339933;">:</span>
    <span style="color: #339933;">/*</span> r0 contains the base address of channel1 <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r1 contains the base address of channel2 <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r2 contains the base address of channel_out <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r3 is the number of samples <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r4 is the number of the current sample
          so it holds that <span style="color: #ff0000;">0</span> ≤ r4 &lt; r3 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>              <span style="color: #339933;">/*</span> r4 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    b <span style="color: #339933;">.</span>Lcheck_loop          <span style="color: #339933;">/*</span> branch to check_loop <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>Lloop<span style="color: #339933;">:</span>
      ldr r6<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #339933;">,</span> r4<span style="color: #009900; font-weight: bold;">]</span>      <span style="color: #339933;">/*</span> r6 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r0 <span style="color: #339933;">+</span> r4<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
      ldr r7<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #339933;">,</span> r4<span style="color: #009900; font-weight: bold;">]</span>      <span style="color: #339933;">/*</span> r7 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r1 <span style="color: #339933;">+</span> r4<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
      qadd16 <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> r7     <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">15</span><span style="color: #339933;">:</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span> ← saturated_sum_16<span style="color: #009900; font-weight: bold;">(</span>r6<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">15</span><span style="color: #339933;">:</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> r7<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">15</span><span style="color: #339933;">:</span><span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
                            <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">31</span><span style="color: #339933;">:</span><span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">]</span> ← saturated_sum_16<span style="color: #009900; font-weight: bold;">(</span>r6<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">31</span><span style="color: #339933;">:</span><span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> r7<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">31</span><span style="color: #339933;">:</span><span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">str</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r2<span style="color: #339933;">,</span> r4<span style="color: #009900; font-weight: bold;">]</span>      <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r2 <span style="color: #339933;">+</span> r4<span style="color: #009900; font-weight: bold;">)</span> ← <span style="color: #46aa03; font-weight: bold;">r8</span> <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">add</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span>        <span style="color: #339933;">/*</span> r4 ← r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">2</span> <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>Lcheck_loop<span style="color: #339933;">:</span>
      <span style="color: #00007f; font-weight: bold;">cmp</span> r4<span style="color: #339933;">,</span> r3            <span style="color: #339933;">/*</span> compute r4 <span style="color: #339933;">-</span> r3 <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
      blt <span style="color: #339933;">.</span>Lloop            <span style="color: #339933;">/*</span> if r4 &lt; r3 jump to the
                               beginning of the <span style="color: #00007f; font-weight: bold;">loop</span> <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">more_realistic_channel_mixing:
    /* r0 contains the base address of channel1 */
    /* r1 contains the base address of channel2 */
    /* r2 contains the base address of channel_out */
    /* r3 is the number of samples */
    /* r4 is the number of the current sample
          so it holds that 0 ≤ r4 &lt; r3 */

    mov r4, #0              /* r4 ← 0 */
    b .Lcheck_loop          /* branch to check_loop */
    .Lloop:
      ldr r6, [r0, r4]      /* r6 ← *(r0 + r4) */
      ldr r7, [r1, r4]      /* r7 ← *(r1 + r4) */
      qadd16 r8, r6, r7     /* r8[15:0] ← saturated_sum_16(r6[15:0], r7[15:0]) */
                            /* r8[31:16] ← saturated_sum_16(r6[31:16], r7[31:16]) */
      str r8, [r2, r4]      /* *(r2 + r4) ← r8 */
      add r4, r4, #2        /* r4 ← r4 + 2 */
    .Lcheck_loop:
      cmp r4, r3            /* compute r4 - r3 and update cpsr */
      blt .Lloop            /* if r4 &lt; r3 jump to the
                               beginning of the loop */</p></div>

<p>
That's all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2015/07/04/arm-assembler-raspberry-pi-chapter-25/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2015/07/04/arm-assembler-raspberry-pi-chapter-25/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2015/07/04/arm-assembler-raspberry-pi-chapter-25/&amp;text=ARM+assembler+in+Raspberry+Pi+%E2%80%93+Chapter+25+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2015/07/04/arm-assembler-raspberry-pi-chapter-25/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>				<p class="tags"><span><a href="http://thinkingeek.com/tag/arm/" rel="tag">arm</a>, <a href="http://thinkingeek.com/tag/assembler/" rel="tag">assembler</a>, <a href="http://thinkingeek.com/tag/pi/" rel="tag">pi</a>, <a href="http://thinkingeek.com/tag/raspberry/" rel="tag">raspberry</a></span></p>		<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2015/02/26/array-array/" rel="prev">When an array is not an array</a></span>
			<span class="next"><a href="http://thinkingeek.com/2015/08/16/a-simple-plugin-for-gcc-part-1/" rel="next">A simple plugin for GCC – Part 1</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
			<h3>6 thoughts on “<span>ARM assembler in Raspberry Pi – Chapter 25</span>”</h3>
		<ul class="commentlist">
				<li class="comment even thread-even depth-1 parent" id="comment-897601">
				<div id="div-comment-897601" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/8e02ba12a3b8f9d19306c56fbde0d455?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/8e02ba12a3b8f9d19306c56fbde0d455?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-8e02ba12a3b8f9d19306c56fbde0d455-0" originals="32" src-orig="http://2.gravatar.com/avatar/8e02ba12a3b8f9d19306c56fbde0d455?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn"><a href="http://www.utdallas.edu/~perin" rel="external nofollow" class="url">William Pervin</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2015/07/04/arm-assembler-raspberry-pi-chapter-25/#comment-897601">
			July 4, 2015 at 12:50 pm</a>		</div>

		<p>Very interesting topic! I do have trouble seeing in your clipping example why if r4 == 32767 the instruction mvn r4,r4 /* r4 &lt;- ~r4 */ gives r4 == -32768 (the largest negative number) and not just -32767.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2015/07/04/arm-assembler-raspberry-pi-chapter-25/?replytocom=897601#respond" onclick="return addComment.moveForm( &quot;div-comment-897601&quot;, &quot;897601&quot;, &quot;respond&quot;, &quot;2018&quot; )" aria-label="Reply to William Pervin">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-897605">
				<div id="div-comment-897605" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-0">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2015/07/04/arm-assembler-raspberry-pi-chapter-25/#comment-897605">
			July 4, 2015 at 1:10 pm</a>		</div>

		<p>Hi William,</p>
<p>this happens because the <code>mvn</code> instruction performs a bitwise <code>not</code> on its operand prior the movement (the ~ symbol used in the comment is the C unary operator that does a bitwise <code>not</code> to its operand).</p>
<p>In two’s complement, the bitwise <code>not</code> of a number is the negative number minus one. This follows from the usual algorithm to change the sign of a number in two’s complement: you first do a bitwise <code>not</code> and then <code>add</code> 1.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2015/07/04/arm-assembler-raspberry-pi-chapter-25/?replytocom=897605#respond" onclick="return addComment.moveForm( &quot;div-comment-897605&quot;, &quot;897605&quot;, &quot;respond&quot;, &quot;2018&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-955439">
				<div id="div-comment-955439" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/9898ed94b080519dce8a07b857d0c72b?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/9898ed94b080519dce8a07b857d0c72b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-9898ed94b080519dce8a07b857d0c72b-0" originals="32" src-orig="http://0.gravatar.com/avatar/9898ed94b080519dce8a07b857d0c72b?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Ryan Salvador</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2015/07/04/arm-assembler-raspberry-pi-chapter-25/#comment-955439">
			December 19, 2015 at 1:16 pm</a>		</div>

		<p>Hi Roger,</p>
<p>I’ve been following along the tutorials. I’m now on chapter 12 and I notice that there has really been a big improvement in my programming. I was just wondering if there’s such a thing as system calls in ARM like if I would like to read and write to files using assembly and if so, then is there any chance that we are going to see a tutorial on that in the future?</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2015/07/04/arm-assembler-raspberry-pi-chapter-25/?replytocom=955439#respond" onclick="return addComment.moveForm( &quot;div-comment-955439&quot;, &quot;955439&quot;, &quot;respond&quot;, &quot;2018&quot; )" aria-label="Reply to Ryan Salvador">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-955461">
				<div id="div-comment-955461" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-1">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2015/07/04/arm-assembler-raspberry-pi-chapter-25/#comment-955461">
			December 19, 2015 at 7:10 pm</a>		</div>

		<p>Hi Ryan,</p>
<p>yes, there is such a mechanism. Chapter 19 is about system calls of the operating system.</p>
<p>That said it is rather cumbersome to call them directly, so most of the time it is easier to use the functions offered by the C library (they act as wrappers to the system calls).</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2015/07/04/arm-assembler-raspberry-pi-chapter-25/?replytocom=955461#respond" onclick="return addComment.moveForm( &quot;div-comment-955461&quot;, &quot;955461&quot;, &quot;respond&quot;, &quot;2018&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1 parent" id="comment-982799">
				<div id="div-comment-982799" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/499384e407a4d8c40b5274bb4310549d?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/499384e407a4d8c40b5274bb4310549d?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-499384e407a4d8c40b5274bb4310549d-0" originals="32" src-orig="http://1.gravatar.com/avatar/499384e407a4d8c40b5274bb4310549d?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Filippo</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2015/07/04/arm-assembler-raspberry-pi-chapter-25/#comment-982799">
			December 29, 2016 at 1:31 am</a>		</div>

		<p>Hi Roger, thanks for the tutorials.<br>
In the first example (naive_channel_mixing) should it be:<br>
mov r8, r8, ASR #1<br>
insted of:<br>
mov r8, r8, LSR #1?</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2015/07/04/arm-assembler-raspberry-pi-chapter-25/?replytocom=982799#respond" onclick="return addComment.moveForm( &quot;div-comment-982799&quot;, &quot;982799&quot;, &quot;respond&quot;, &quot;2018&quot; )" aria-label="Reply to Filippo">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-983616">
				<div id="div-comment-983616" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-2">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2015/07/04/arm-assembler-raspberry-pi-chapter-25/#comment-983616">
			January 8, 2017 at 4:27 pm</a>		</div>

		<p>Hi Filippo,</p>
<p>yes thanks for spotting this. We are operating with signed integers.</p>
<p>I have updated the post.</p>
<p>Kind regards,<br>
Roger</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2015/07/04/arm-assembler-raspberry-pi-chapter-25/?replytocom=983616#respond" onclick="return addComment.moveForm( &quot;div-comment-983616&quot;, &quot;983616&quot;, &quot;respond&quot;, &quot;2018&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		</ul>
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2015/07/04/arm-assembler-raspberry-pi-chapter-25/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="2018" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="cb929c979e"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496667482091"></form>
			</div><!-- #respond -->
	</div>	</div>