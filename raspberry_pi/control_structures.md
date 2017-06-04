# Control structures


<div class="main">
			<h1>ARM assembler in Raspberry Pi – Chapter 6 </h1>
		<p class="post-meta"><span class="date">January 20, 2013</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2013/01/20/arm-assembler-raspberry-pi-chapter-6/#comments">10</a></span> </p>
		<h2>Control structures</h2>
<p>
In the previous chapter we learnt branch instructions. They are really powerful tools because they allow us to express control structures. <em>Structured programming</em> is an important milestone in better computing engineering (a foundational one, but nonetheless an important one). So being able to map usual structured programming constructs in assembler, in our processor, is a Good Thing™.
</p>
<p><span id="more-479"></span></p>
<h2>If, then, else</h2>
<p>
Well, this one is a basic one, and in fact we already used this structure in the previous chapter. Consider the following structure, where <code>E</code> is an expression and <code>S1</code> and <code>S2</code> are statements (they may be compound statements like <code>{ SA; SB; SC; }</code>)
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #b1b100;">if</span> <span style="color: #009900;">(</span>E<span style="color: #009900;">)</span> then
   S1
<span style="color: #b1b100;">else</span>
   S2</pre></td></tr></tbody></table><p class="theCode" style="display:none;">if (E) then
   S1
else
   S2</p></div>

<p>
A possible way to express this in ARM assembler could be the following
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">if_eval<span style="color: #339933;">:</span> 
    <span style="color: #339933;">/*</span> Assembler that evaluates E <span style="color: #00007f; font-weight: bold;">and</span> updates the cpsr accordingly <span style="color: #339933;">*/</span>
bXX else <span style="color: #339933;">/*</span> Here XX is the appropiate condition <span style="color: #339933;">*/</span>
then_part<span style="color: #339933;">:</span> 
   <span style="color: #339933;">/*</span> assembler for S1<span style="color: #339933;">,</span> the <span style="color: #7f007f;">"then"</span> part <span style="color: #339933;">*/</span>
   b end_of_if
else<span style="color: #339933;">:</span>
   <span style="color: #339933;">/*</span> assembler for S2<span style="color: #339933;">,</span> the <span style="color: #7f007f;">"else"</span> part <span style="color: #339933;">*/</span>
end_of_if<span style="color: #339933;">:</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">if_eval: 
    /* Assembler that evaluates E and updates the cpsr accordingly */
bXX else /* Here XX is the appropiate condition */
then_part: 
   /* assembler for S1, the "then" part */
   b end_of_if
else:
   /* assembler for S2, the "else" part */
end_of_if:</p></div>

<p>
If there is no else part, we can replace <code>bXX else</code> with <code>bXX end_of_if</code>.
</p>
<h2>Loops</h2>
<p>
This is another usual one in structured programming. While there are several types of loops, actually all reduce to the following structure.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #b1b100;">while</span> <span style="color: #009900;">(</span>E<span style="color: #009900;">)</span>
  S</pre></td></tr></tbody></table><p class="theCode" style="display:none;">while (E)
  S</p></div>

<p>
Supposedly <code>S</code> makes something so <code>E</code> eventually becomes false and the loop is left. Otherwise we would stay in the loop forever (sometimes this is what you want but not in our examples). A way to implement these loops is as follows.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">while_condition <span style="color: #339933;">:</span> <span style="color: #339933;">/*</span> assembler to evaluate E <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
  bXX end_of_loop  <span style="color: #339933;">/*</span> If E is false<span style="color: #339933;">,</span> then <span style="color: #00007f; font-weight: bold;">leave</span> the <span style="color: #00007f; font-weight: bold;">loop</span> right now <span style="color: #339933;">*/</span>
  <span style="color: #339933;">/*</span> assembler of S <span style="color: #339933;">*/</span>
  b while_condition <span style="color: #339933;">/*</span> Unconditional branch to the beginning <span style="color: #339933;">*/</span>
end_of_loop<span style="color: #339933;">:</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">while_condition : /* assembler to evaluate E and update cpsr */
  bXX end_of_loop  /* If E is false, then leave the loop right now */
  /* assembler of S */
  b while_condition /* Unconditional branch to the beginning */
end_of_loop:</p></div>

<p>
A common loop involves iterating from a single range of integers, like in
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #b1b100;">for</span> <span style="color: #009900;">(</span>i <span style="color: #339933;">=</span> L<span style="color: #339933;">;</span> i <span style="color: #339933;">&lt;</span> N<span style="color: #339933;">;</span> i <span style="color: #339933;">+=</span> K<span style="color: #009900;">)</span>
  S</pre></td></tr></tbody></table><p class="theCode" style="display:none;">for (i = L; i &lt; N; i += K)
  S</p></div>

<p>
But this is nothing but
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;">  i <span style="color: #339933;">=</span> L<span style="color: #339933;">;</span>
  <span style="color: #b1b100;">while</span> <span style="color: #009900;">(</span>i <span style="color: #339933;">&lt;</span> N<span style="color: #009900;">)</span>
  <span style="color: #009900;">{</span>
     S<span style="color: #339933;">;</span>
     i <span style="color: #339933;">+=</span> K<span style="color: #339933;">;</span>
  <span style="color: #009900;">}</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">  i = L;
  while (i &lt; N)
  {
     S;
     i += K;
  }</p></div>

<p>
So we do not have to learn a new way to implement the loop itself.
</p>
<h2>1 + 2 + 3 + 4 + … + 22</h2>
<p>
As a first example lets sum all the numbers from 1 to 22 (I’ll tell you later why I chose 22). The result of the sum is <code>253</code> (check it with a <a href="https://www.google.es/#q=1%2B2%2B3%2B4%2B5%2B6%2B7%2B8%2B9%2B10%2B11%2B12%2B13%2B14%2B15%2B16%2B17%2B18%2B19%2B20%2B21%2B22">calculator</a>). I know it makes little sense to compute something the result of which we know already, but this is just an example.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> <span style="color: #339933;">--</span> loop01<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #0000ff; font-weight: bold;">.text</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">global</span> main
main<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>       <span style="color: #339933;">/*</span> r1 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>       <span style="color: #339933;">/*</span> r2 ← <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">loop</span><span style="color: #339933;">:</span> 
    <span style="color: #00007f; font-weight: bold;">cmp</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">22</span>      <span style="color: #339933;">/*</span> compare r2 <span style="color: #00007f; font-weight: bold;">and</span> <span style="color: #ff0000;">22</span> <span style="color: #339933;">*/</span>
    bgt end          <span style="color: #339933;">/*</span> branch if r2 &gt; <span style="color: #ff0000;">22</span> to end <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> r2   <span style="color: #339933;">/*</span> r1 ← r1 <span style="color: #339933;">+</span> r2 <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r2<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>   <span style="color: #339933;">/*</span> r2 ← r2 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    b <span style="color: #00007f; font-weight: bold;">loop</span>
end<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> r1       <span style="color: #339933;">/*</span> r0 ← r1 <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* -- loop01.s */
.text
.global main
main:
    mov r1, #0       /* r1 ← 0 */
    mov r2, #1       /* r2 ← 1 */
loop: 
    cmp r2, #22      /* compare r2 and 22 */
    bgt end          /* branch if r2 &gt; 22 to end */
    add r1, r1, r2   /* r1 ← r1 + r2 */
    add r2, r2, #1   /* r2 ← r2 + 1 */
    b loop
end:
    mov r0, r1       /* r0 ← r1 */
    bx lr</p></div>

<p>
Here we are counting from 1 to 22. We will use the register <code>r2</code> as the counter. As you can see in line 6 we initialize it to 1. The sum will be accumulated in the register <code>r1</code>, at the end of the program we move the contents of <code>r1</code> into <code>r0</code> to return the result of the sum as the error code of the program (we could have used <code>r0</code> in all the code and avoid this final <code>mov</code> but I think it is clearer this way).
</p>
<p>
In line 8 we compare <code>r2</code> (remember, the counter that will go from 1 to 22) to 22. This will update the <code>cpsr</code> thus in line 9 we can check if the comparison was such that r2 was greater than 22. If this is the case, we end the loop by branching to <code>end</code>. Otherwise we add the current value of <code>r2</code> to the current value of <code>r1</code> (remember, in <code>r1</code> we accumulate the sum from 1 to 22).
</p>
<p>
Line 11 is an important one. We increase the value of <code>r2</code>, because we are counting from 1 to 22 and we already added the current counter value in <code>r2</code> to the result of the sum in <code>r1</code>. Then at line 12 we branch back at the beginning of the loop. Note that if line 11 was not there we would hang as the comparison in line 8 would always be false and we would never leave the loop in line 9!
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="bash" style="font-family:monospace;">$ .<span style="color: #000000; font-weight: bold;">/</span>loop01; <span style="color: #7a0874; font-weight: bold;">echo</span> <span style="color: #007800;">$?</span>
<span style="color: #000000;">253</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ ./loop01; echo $?
253</p></div>

<p>
Well, now you could change the line 8 and try with let’s say, #100. The result should be 5050.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="bash" style="font-family:monospace;">$ .<span style="color: #000000; font-weight: bold;">/</span>loop01; <span style="color: #7a0874; font-weight: bold;">echo</span> <span style="color: #007800;">$?</span>
<span style="color: #000000;">186</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ ./loop01; echo $?
186</p></div>

<p>
What happened? Well, it happens that in Linux the error code of a program is a number from 0 to 255 (8 bits). If the result is 5050, only the lower 8 bits of the number are used. 5050 in binary is <code>1001110111010</code>, its lower 8 bits are <code>10111010</code> which is exactly 186. How can we check the computed <code>r1</code> is 5050 before ending the program? Let’s use GDB.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="gdb" style="font-family:monospace;">$ gdb <span style="color: #442886; font-weight:bold;">loop
...</span>
<span style="font-weight:bold;">(</span>gdb<span style="font-weight:bold;">)</span> start
Temporary breakpoint <span style="">1</span> at <span style="color: #555;">0x8390</span>
Starting program: /home/roger/asm/chapter06/loop01 
&nbsp;
Temporary breakpoint <span style="">1</span>, <span style="color: #555;">0x00008390</span> in <span style="color: #442886; font-weight:bold;">main</span> <span style="font-weight:bold;">(</span><span style="font-weight:bold;">)</span>
<span style="font-weight:bold;">(</span>gdb<span style="font-weight:bold;">)</span> disas main,+<span style="font-weight:bold;">(</span><span style="">9</span>*<span style="">4</span><span style="font-weight:bold;">)</span>
Dump of assembler code from <span style="color: #0057AE; text-style:italic;"><span style="color: #555;">0x8390</span> to <span style="color: #555;">0x83b4</span>:</span>
   <span style="color: #555;">0x00008390</span> &lt;main+<span style="">0</span>&gt;:	mov	r1, #<span style="">0</span>
   <span style="color: #555;">0x00008394</span> &lt;main+<span style="">4</span>&gt;:	mov	r2, #<span style="">1</span>
   <span style="color: #555;">0x00008398</span> &lt;loop+<span style="">0</span>&gt;:	cmp	r2, #<span style="">100</span>	; <span style="color: #555;">0x64</span>
   <span style="color: #555;">0x0000839c</span> &lt;loop+<span style="">4</span>&gt;:	bgt	<span style="color: #555;">0x83ac</span> &lt;end&gt;
   <span style="color: #555;">0x000083a0</span> &lt;loop+<span style="">8</span>&gt;:	add	r1, r1, r2
   <span style="color: #555;">0x000083a4</span> &lt;loop+<span style="">12</span>&gt;:	add	r2, r2, #<span style="">1</span>
   <span style="color: #555;">0x000083a8</span> &lt;loop+<span style="">16</span>&gt;:	b	<span style="color: #555;">0x8398</span> &lt;loop&gt;
   <span style="color: #555;">0x000083ac</span> &lt;end+<span style="">0</span>&gt;:	mov	r0, r1
   <span style="color: #555;">0x000083b0</span> &lt;end+<span style="">4</span>&gt;:	bx	lr
End of assembler dump.</pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ gdb loop
...
(gdb) start
Temporary breakpoint 1 at 0x8390
Starting program: /home/roger/asm/chapter06/loop01 

Temporary breakpoint 1, 0x00008390 in main ()
(gdb) disas main,+(9*4)
Dump of assembler code from 0x8390 to 0x83b4:
   0x00008390 &lt;main+0&gt;:	mov	r1, #0
   0x00008394 &lt;main+4&gt;:	mov	r2, #1
   0x00008398 &lt;loop+0&gt;:	cmp	r2, #100	; 0x64
   0x0000839c &lt;loop+4&gt;:	bgt	0x83ac &lt;end&gt;
   0x000083a0 &lt;loop+8&gt;:	add	r1, r1, r2
   0x000083a4 &lt;loop+12&gt;:	add	r2, r2, #1
   0x000083a8 &lt;loop+16&gt;:	b	0x8398 &lt;loop&gt;
   0x000083ac &lt;end+0&gt;:	mov	r0, r1
   0x000083b0 &lt;end+4&gt;:	bx	lr
End of assembler dump.</p></div>

<p>
Let’s tell gdb to stop at <code>0x000083ac</code>, right before executing <code>mov r0, r1</code>.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="gdb" style="font-family:monospace;"><span style="font-weight:bold;">(</span>gdb<span style="font-weight:bold;">)</span> break <span style="color: #442886; font-weight:bold;">*<span style="color: #555;">0x000083ac</span></span>
<span style="font-weight:bold;">(</span>gdb<span style="font-weight:bold;">)</span> cont
Continuing.
&nbsp;
Breakpoint <span style="">2</span>, <span style="color: #555;">0x000083ac</span> in <span style="color: #442886; font-weight:bold;">end</span> <span style="font-weight:bold;">(</span><span style="font-weight:bold;">)</span>
<span style="font-weight:bold;">(</span>gdb<span style="font-weight:bold;">)</span> disas
Dump of assembler code for function end:
=&gt; <span style="color: #555;">0x000083ac</span> &lt;+<span style="">0</span>&gt;:	mov	r0, r1
   <span style="color: #555;">0x000083b0</span> &lt;+<span style="">4</span>&gt;:	bx	lr
End of assembler <span style="color: #442886; font-weight:bold;">dump.</span>
<span style="font-weight:bold;">(</span>gdb<span style="font-weight:bold;">)</span> info register r1
r1             <span style="color: #555;">0x13ba</span>	<span style="">5050</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">(gdb) break *0x000083ac
(gdb) cont
Continuing.

Breakpoint 2, 0x000083ac in end ()
(gdb) disas
Dump of assembler code for function end:
=&gt; 0x000083ac &lt;+0&gt;:	mov	r0, r1
   0x000083b0 &lt;+4&gt;:	bx	lr
End of assembler dump.
(gdb) info register r1
r1             0x13ba	5050</p></div>

<p>
Great, this is what we expected but we could not see due to limits in the error code.
</p>
<p>
Maybe you have noticed that something odd happens with our labels being identified as functions. We will address this issue in a future chapter, this is mostly harmless though.
</p>
<h2>3n + 1</h2>
<p>
Let’s make another example a bit more complicated. This is the famous <em>3n + 1</em> problem also known as the <a href="http://en.wikipedia.org/wiki/Collatz_conjecture">Collatz conjecture</a>. Given a number <code>n</code> we will divide it by 2 if it is even and multiply it by 3 and add one if it is odd.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #b1b100;">if</span> <span style="color: #009900;">(</span>n <span style="color: #339933;">%</span> <span style="color: #0000dd;">2</span> <span style="color: #339933;">==</span> <span style="color: #0000dd;">0</span><span style="color: #009900;">)</span>
  n <span style="color: #339933;">=</span> n <span style="color: #339933;">/</span> <span style="color: #0000dd;">2</span><span style="color: #339933;">;</span>
<span style="color: #b1b100;">else</span>
  n <span style="color: #339933;">=</span> <span style="color: #0000dd;">3</span><span style="color: #339933;">*</span>n <span style="color: #339933;">+</span> <span style="color: #0000dd;">1</span><span style="color: #339933;">;</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">if (n % 2 == 0)
  n = n / 2;
else
  n = 3*n + 1;</p></div>

<p>
Before continuing, our ARM processor is able to multiply two numbers but we should learn a new instruction <code>mul</code> which would detour us a bit. Instead we will use the following identity <code>3 * n = 2*n + n</code>. We do not really know how to multiply or divide by two yet, we will study this in a future chapter, so for now just assume it works as shown in the assembler below.
</p>
<p>
Collatz conjecture states that, for any number <code>n</code>, repeatedly applying this procedure will eventually give us the number 1. Theoretically it could happen that this is not the case. So far, no such number has been found, but it has not been proved otherwise. If we want to repeatedly apply the previous procedure, our program is doing something like this.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;">n <span style="color: #339933;">=</span> ...<span style="color: #339933;">;</span>
<span style="color: #b1b100;">while</span> <span style="color: #009900;">(</span>n <span style="color: #339933;">!=</span> <span style="color: #0000dd;">1</span><span style="color: #009900;">)</span>
<span style="color: #009900;">{</span>
  <span style="color: #b1b100;">if</span> <span style="color: #009900;">(</span>n <span style="color: #339933;">%</span> <span style="color: #0000dd;">2</span> <span style="color: #339933;">==</span> <span style="color: #0000dd;">0</span><span style="color: #009900;">)</span>
     n <span style="color: #339933;">=</span> n <span style="color: #339933;">/</span> <span style="color: #0000dd;">2</span><span style="color: #339933;">;</span>
  <span style="color: #b1b100;">else</span>
     n <span style="color: #339933;">=</span> <span style="color: #0000dd;">3</span><span style="color: #339933;">*</span>n <span style="color: #339933;">+</span> <span style="color: #0000dd;">1</span><span style="color: #339933;">;</span>
<span style="color: #009900;">}</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">n = ...;
while (n != 1)
{
  if (n % 2 == 0)
     n = n / 2;
  else
     n = 3*n + 1;
}</p></div>

<p>
If the Collatz conjecture were false, there would exist some <code>n</code> for which the code above would hang, never reaching 1. But as I said, no such number has been found.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> <span style="color: #339933;">--</span> collatz<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #0000ff; font-weight: bold;">.text</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">global</span> main
main<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">123</span>           <span style="color: #339933;">/*</span> r1 ← <span style="color: #ff0000;">123</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>             <span style="color: #339933;">/*</span> r2 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">loop</span><span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">cmp</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>             <span style="color: #339933;">/*</span> compare r1 <span style="color: #00007f; font-weight: bold;">and</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    beq end                <span style="color: #339933;">/*</span> branch to end if r1 == <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">and</span> r3<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>         <span style="color: #339933;">/*</span> r3 ← r1 &amp; <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">cmp</span> r3<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>             <span style="color: #339933;">/*</span> compare r3 <span style="color: #00007f; font-weight: bold;">and</span> <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    bne odd                <span style="color: #339933;">/*</span> branch to odd if r3 != <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
even<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> ASR #<span style="color: #ff0000;">1</span>     <span style="color: #339933;">/*</span> r1 ← <span style="color: #009900; font-weight: bold;">(</span>r1 &gt;&gt; <span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    b end_loop
odd<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">1</span> <span style="color: #339933;">/*</span> r1 ← r1 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span>r1 &lt;&lt; <span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>         <span style="color: #339933;">/*</span> r1 ← r1 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
&nbsp;
end_loop<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r2<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>         <span style="color: #339933;">/*</span> r2 ← r2 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    b <span style="color: #00007f; font-weight: bold;">loop</span>                 <span style="color: #339933;">/*</span> branch to <span style="color: #00007f; font-weight: bold;">loop</span> <span style="color: #339933;">*/</span>
&nbsp;
end<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> r2
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* -- collatz.s */
.text
.global main
main:
    mov r1, #123           /* r1 ← 123 */
    mov r2, #0             /* r2 ← 0 */
loop:
    cmp r1, #1             /* compare r1 and 1 */
    beq end                /* branch to end if r1 == 1 */

    and r3, r1, #1         /* r3 ← r1 &amp; 1 */
    cmp r3, #0             /* compare r3 and 0 */
    bne odd                /* branch to odd if r3 != 0 */
even:
    mov r1, r1, ASR #1     /* r1 ← (r1 &gt;&gt; 1) */
    b end_loop
odd:
    add r1, r1, r1, LSL #1 /* r1 ← r1 + (r1 &lt;&lt; 1) */
    add r1, r1, #1         /* r1 ← r1 + 1 */

end_loop:
    add r2, r2, #1         /* r2 ← r2 + 1 */
    b loop                 /* branch to loop */

end:
    mov r0, r2
    bx lr</p></div>

<p>
In <code>r1</code> we will keep the number <code>n</code>. In this case we will use the number 123. 123 reaches 1 in 46 steps: [123, 370, 185, 556, 278, 139, 418, 209, 628, 314, 157, 472, 236, 118, 59, 178, 89, 268, 134, 67, 202, 101, 304, 152, 76, 38, 19, 58, 29, 88, 44, 22, 11, 34, 17, 52, 26, 13, 40, 20, 10, 5, 16, 8, 4, 2, 1]. We will count the number of steps in register <code>r2</code>. So we initialize <code>r1</code> with 123 and <code>r2</code> with 0 (no step has been performed yet).
</p>
<p>
At the beginning of the loop, in lines 8 and 9, we check if <code>r1</code> is 1. So we compare it with 1 and if it is equal we leave the loop branching to <code>end</code>.
</p>
<p>
Now we know that <code>r1</code> is not 1, so we proceed to check if it is even or odd. To do this we use a new instruction <code>and</code> which performs a <em>bitwise and operation</em>. An even number will have the least significant bit (LSB) to 0, while an odd number will have the LSB to 1. So a bitwise and using 1 will return 0 or 1 on even or odd numbers, respectively. In line 11 we keep the result of the bitwise and in <code>r3</code> register and then, in line 12, we compare it against 0. If it is not zero then we branch to <code>odd</code>, otherwise we continue on the <code>even</code> case.</p>
<p></p><p>
Now some magic happens in line 15. This is a combined operation that ARM allows us to do. This is a <code>mov</code> but we do not move the value of <code>r1</code> directly to <code>r1</code> (which would be doing nothing) but first we do an <em>arithmetic shift right</em> (ASR) to the value of <code>r1</code> (to the value, no the register itself). Then this shifted value is moved to the register <code>r1</code>. An <em>arithmetic shift right</em> shifts all the bits of a register to the right: the rightmost bit is effectively discarded and the leftmost is set to the same value as the leftmost bit prior the shift. Shifting right one bit to a number is the same as dividing that number by 2. So this <code>mov r1, r1, ASR #1</code> is actually doing <code>r1 ← r1 / 2</code>.</p>
<p></p><p>
Some similar magic happens for the even case in line 18. In this case we are doing an <code>add</code>. The first and second operands must be registers (destination operand and the first source operand). The third is combined with a <em>logical shift left</em> (LSL). The value of the operand is shifted left 1 bit: the leftmost bit is discarded and the rightmost bit is set to 0. This is effectively multiplying the value by 2. So we are adding <code>r1</code> (which keeps the value of <code>n</code>) to <code>2*r1</code>. This is <code>3*r1</code>, so <code>3*n</code>. We keep this value in <code>r1</code> again. In line 19 we add 1 to that value, so <code>r1</code> ends having the value <code>3*n+1</code> that we wanted.
</p>
<p>
Do not worry very much now about these LSL and ASR. Just take them for granted now. In a future chapter we will see them in more detail.
</p>
<p>
Finally, at the end of the loop, in line 22 we update <code>r2</code> (remember it keeps the counter of our steps) and then we branch back to the beginning of the loop. Before ending the program we move the counter to <code>r0</code> so we return the number of steps we did to reach 1.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="bash" style="font-family:monospace;">$ .<span style="color: #000000; font-weight: bold;">/</span>collatz; <span style="color: #7a0874; font-weight: bold;">echo</span> <span style="color: #007800;">$?</span>
<span style="color: #000000;">46</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ ./collatz; echo $?
46</p></div>

<p>
Great.
</p>
<p>
That’s all for today.
</p>
<h2>Postscript</h2>
<p>
Kevin Millikin rightly pointed (in a comment below) that usually a loop is not implemented in the way shown above. In fact Kevin says that a better way to do the loop of <code>loop01.s</code> is as follows.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> <span style="color: #339933;">--</span> loop02<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #0000ff; font-weight: bold;">.text</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">global</span> main
main<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>       <span style="color: #339933;">/*</span> r1 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>       <span style="color: #339933;">/*</span> r2 ← <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    b check_loop     <span style="color: #339933;">/*</span> unconditionally jump <span style="color: #0000ff; font-weight: bold;">at</span> the end of the <span style="color: #00007f; font-weight: bold;">loop</span> <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">loop</span><span style="color: #339933;">:</span> 
    <span style="color: #00007f; font-weight: bold;">add</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> r2   <span style="color: #339933;">/*</span> r1 ← r1 <span style="color: #339933;">+</span> r2 <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r2<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>   <span style="color: #339933;">/*</span> r2 ← r2 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
check_loop<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">cmp</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">22</span>      <span style="color: #339933;">/*</span> compare r2 <span style="color: #00007f; font-weight: bold;">and</span> <span style="color: #ff0000;">22</span> <span style="color: #339933;">*/</span>
    ble <span style="color: #00007f; font-weight: bold;">loop</span>         <span style="color: #339933;">/*</span> branch if r2 &lt;= <span style="color: #ff0000;">22</span> to the beginning of the <span style="color: #00007f; font-weight: bold;">loop</span> <span style="color: #339933;">*/</span>
end<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> r1       <span style="color: #339933;">/*</span> r0 ← r1 <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* -- loop02.s */
.text
.global main
main:
    mov r1, #0       /* r1 ← 0 */
    mov r2, #1       /* r2 ← 1 */
    b check_loop     /* unconditionally jump at the end of the loop */
loop: 
    add r1, r1, r2   /* r1 ← r1 + r2 */
    add r2, r2, #1   /* r2 ← r2 + 1 */
check_loop:
    cmp r2, #22      /* compare r2 and 22 */
    ble loop         /* branch if r2 &lt;= 22 to the beginning of the loop */
end:
    mov r0, r1       /* r0 ← r1 */
    bx lr</p></div>

<p>
If you count the number of instruction in the two codes, there are 9 instructions in both. But if you look carefully in Kevin’s proposal you will see that by unconditionally branching to the end of the loop, and reversing the condition check, we can skip one branch thus reducing the number of instructions of the loop itself from 5 to 4.
</p>
<p>
There is another advantage in this second version, though: there is only one branch in the loop itself as we resort to <em>implicit sequencing</em> to reach again the two instructions performing the check. For reasons beyond the scope of this post, the execution of a branch instruction may negatively affect the performance of our programs. Processors have mechanisms to mitigate the performance loss due to branches (and in fact the processor in the Raspberry Pi does have them). But avoiding a branch instruction entirely avoids the potential performance penalization of executing a branch instruction.
</p>
<p>
While we do not care very much now about the performance of our assembler. However, I thought it was worth developing a bit more Kevin’s comment.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2013/01/20/arm-assembler-raspberry-pi-chapter-6/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2013/01/20/arm-assembler-raspberry-pi-chapter-6/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2013/01/20/arm-assembler-raspberry-pi-chapter-6/&amp;text=ARM+assembler+in+Raspberry+Pi+%E2%80%93+Chapter+6+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2013/01/20/arm-assembler-raspberry-pi-chapter-6/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>				<p class="tags"><span><a href="http://thinkingeek.com/tag/arm/" rel="tag">arm</a>, <a href="http://thinkingeek.com/tag/assembler/" rel="tag">assembler</a>, <a href="http://thinkingeek.com/tag/control-structures/" rel="tag">control structures</a>, <a href="http://thinkingeek.com/tag/pi/" rel="tag">pi</a>, <a href="http://thinkingeek.com/tag/raspberry/" rel="tag">raspberry</a></span></p>		<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/" rel="prev">ARM assembler in Raspberry Pi – Chapter 5</a></span>
			<span class="next"><a href="http://thinkingeek.com/2013/01/26/arm-assembler-raspberry-pi-chapter-7/" rel="next">ARM assembler in Raspberry Pi – Chapter 7</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
			<h3>10 thoughts on “<span>ARM assembler in Raspberry Pi – Chapter 6</span>”</h3>
		<ul class="commentlist">
				<li class="comment even thread-even depth-1 parent" id="comment-756">
				<div id="div-comment-756" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/21e06f14b53b5c0239c2df5f857f0cb0?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/21e06f14b53b5c0239c2df5f857f0cb0?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/21e06f14b53b5c0239c2df5f857f0cb0?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-21e06f14b53b5c0239c2df5f857f0cb0-0">			<cite class="fn">Kevin Millikin</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/20/arm-assembler-raspberry-pi-chapter-6/#comment-756">
			January 21, 2013 at 3:38 am</a>		</div>

		<p>Thanks for writing this.  It’s been fun reading so far.</p>
<p>In practice one would normally test at the bottom of a while loop.  In your loop01.s there are two tests in the loop on each iteration — a forward conditional branch (not taken) and a backward unconditional branch.</p>
<p>Instead compile the test at the bottom, invert the branch condition, and enter the loop with an unconditional branch to the test.  Each iteration of the loop has a backward conditional branch (taken).</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/20/arm-assembler-raspberry-pi-chapter-6/?replytocom=756#respond" onclick="return addComment.moveForm( &quot;div-comment-756&quot;, &quot;756&quot;, &quot;respond&quot;, &quot;479&quot; )" aria-label="Reply to Kevin Millikin">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-767">
				<div id="div-comment-767" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-0">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/20/arm-assembler-raspberry-pi-chapter-6/#comment-767">
			January 21, 2013 at 10:13 pm</a>		</div>

		<p>Hi Kevin.</p>
<p>Thank you very much for your very proper comment about loops. I found it very interesting so I developed it a bit more at the end of the post.</p>
<p>Kind regards.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/20/arm-assembler-raspberry-pi-chapter-6/?replytocom=767#respond" onclick="return addComment.moveForm( &quot;div-comment-767&quot;, &quot;767&quot;, &quot;respond&quot;, &quot;479&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-803">
				<div id="div-comment-803" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/4d5c69538b67ef0f073c3c484d802a5c?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/4d5c69538b67ef0f073c3c484d802a5c?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-4d5c69538b67ef0f073c3c484d802a5c-0" originals="32" src-orig="http://1.gravatar.com/avatar/4d5c69538b67ef0f073c3c484d802a5c?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Mark</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/20/arm-assembler-raspberry-pi-chapter-6/#comment-803">
			January 26, 2013 at 5:50 pm</a>		</div>

		<p>Hi,</p>
<p>Nice set of articles. I first did ARM assembler back in 1987 !<br>
It’s usually better to count backwards in loops and use the free zero condition flag setting when you get to the end:<br>
.text<br>
.global main<br>
main:<br>
        mov r1,#0<br>
        mov r2,#22<br>
loop:<br>
        add r1,r1,r2<br>
        subs r2,r2,#1<br>
        bne loop<br>
end:<br>
        mov r0,r1<br>
        bx lr</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/20/arm-assembler-raspberry-pi-chapter-6/?replytocom=803#respond" onclick="return addComment.moveForm( &quot;div-comment-803&quot;, &quot;803&quot;, &quot;respond&quot;, &quot;479&quot; )" aria-label="Reply to Mark">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2 parent" id="comment-804">
				<div id="div-comment-804" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-1">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/20/arm-assembler-raspberry-pi-chapter-6/#comment-804">
			January 26, 2013 at 6:03 pm</a>		</div>

		<p>Hi Mark, thanks for your comment!</p>
<p>You are entirely right but since I have not explained the suffix S of instructions I could not use your approach.</p>
<p>I’ll explain this in a later chapter. I am going rather slow, introducing just a little bit in each chapter.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/20/arm-assembler-raspberry-pi-chapter-6/?replytocom=804#respond" onclick="return addComment.moveForm( &quot;div-comment-804&quot;, &quot;804&quot;, &quot;respond&quot;, &quot;479&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment even depth-3" id="comment-823">
				<div id="div-comment-823" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/583277e7960a8516fec6fdf00545bc2b?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/583277e7960a8516fec6fdf00545bc2b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/583277e7960a8516fec6fdf00545bc2b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-583277e7960a8516fec6fdf00545bc2b-0">			<cite class="fn">Mark</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/20/arm-assembler-raspberry-pi-chapter-6/#comment-823">
			January 28, 2013 at 8:22 pm</a>		</div>

		<p>Yes, good point.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/20/arm-assembler-raspberry-pi-chapter-6/?replytocom=823#respond" onclick="return addComment.moveForm( &quot;div-comment-823&quot;, &quot;823&quot;, &quot;respond&quot;, &quot;479&quot; )" aria-label="Reply to Mark">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-even depth-1" id="comment-1432">
				<div id="div-comment-1432" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a16ae1e69be669d6f2c5fa0972e39c8a?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a16ae1e69be669d6f2c5fa0972e39c8a?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-a16ae1e69be669d6f2c5fa0972e39c8a-0" originals="32" src-orig="http://1.gravatar.com/avatar/a16ae1e69be669d6f2c5fa0972e39c8a?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn"><a href="http://digilander.libero.it/zantaz/" rel="external nofollow" class="url">Mariani Antonio Mario</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/20/arm-assembler-raspberry-pi-chapter-6/#comment-1432">
			June 29, 2013 at 3:16 am</a>		</div>

		<p>nice but very nice code … I learn much …</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/20/arm-assembler-raspberry-pi-chapter-6/?replytocom=1432#respond" onclick="return addComment.moveForm( &quot;div-comment-1432&quot;, &quot;1432&quot;, &quot;respond&quot;, &quot;479&quot; )" aria-label="Reply to Mariani Antonio Mario">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-1754">
				<div id="div-comment-1754" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/445c0c582daf37aab5ae8716f751374e?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/445c0c582daf37aab5ae8716f751374e?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-445c0c582daf37aab5ae8716f751374e-0" originals="32" src-orig="http://1.gravatar.com/avatar/445c0c582daf37aab5ae8716f751374e?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Bug Killer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/20/arm-assembler-raspberry-pi-chapter-6/#comment-1754">
			October 13, 2013 at 1:05 am</a>		</div>

		<p>There’s a typo:</p>
<p>add r1, r1, r2   /* r1 ← r1 + r1 */</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/20/arm-assembler-raspberry-pi-chapter-6/?replytocom=1754#respond" onclick="return addComment.moveForm( &quot;div-comment-1754&quot;, &quot;1754&quot;, &quot;respond&quot;, &quot;479&quot; )" aria-label="Reply to Bug Killer">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-1757">
				<div id="div-comment-1757" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-2">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/20/arm-assembler-raspberry-pi-chapter-6/#comment-1757">
			October 13, 2013 at 2:07 pm</a>		</div>

		<p>Fixed, thanks!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/20/arm-assembler-raspberry-pi-chapter-6/?replytocom=1757#respond" onclick="return addComment.moveForm( &quot;div-comment-1757&quot;, &quot;1757&quot;, &quot;respond&quot;, &quot;479&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1 parent" id="comment-968714">
				<div id="div-comment-968714" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/f3b65c949b08ed017457f8c712eef678?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/f3b65c949b08ed017457f8c712eef678?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-f3b65c949b08ed017457f8c712eef678-0" originals="32" src-orig="http://0.gravatar.com/avatar/f3b65c949b08ed017457f8c712eef678?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Cecil Ward</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/20/arm-assembler-raspberry-pi-chapter-6/#comment-968714">
			May 31, 2016 at 1:44 am</a>		</div>

		<p>in your original loop01, isn’t there a _signed_ comparison branch, bgt? I would suggest it’s worth warning against the use of signed instructions because of the risk of integer overflow with sufficiently high values unless you really need signed arithmetic, so the advice is to stick to unsigned versions always unless you need to do otherwise and the range of values is safe.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/20/arm-assembler-raspberry-pi-chapter-6/?replytocom=968714#respond" onclick="return addComment.moveForm( &quot;div-comment-968714&quot;, &quot;968714&quot;, &quot;respond&quot;, &quot;479&quot; )" aria-label="Reply to Cecil Ward">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-969151">
				<div id="div-comment-969151" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-3">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/20/arm-assembler-raspberry-pi-chapter-6/#comment-969151">
			June 4, 2016 at 7:46 pm</a>		</div>

		<p>Hi Cecil,</p>
<p>yes unsigned integers have larger ranges, but overflow may happen also when working with them. Since most of the examples in the chapters use signed integers it makes sense to me to use <code>bgt</code> (rather than <code>bhi</code>)</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/20/arm-assembler-raspberry-pi-chapter-6/?replytocom=969151#respond" onclick="return addComment.moveForm( &quot;div-comment-969151&quot;, &quot;969151&quot;, &quot;respond&quot;, &quot;479&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		</ul>
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2013/01/20/arm-assembler-raspberry-pi-chapter-6/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="479" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="49e9fa7a7b"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496597799028"></form>
			</div><!-- #respond -->
	</div>	</div>