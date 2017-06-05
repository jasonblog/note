# Loops and the status register


<div class="main">
			<h1>ARM assembler in Raspberry Pi – Chapter 12 </h1>
		<p class="post-meta"><span class="date">March 28, 2013</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/#comments">19</a></span> </p>
		<p>
We saw in chapter 6 some simple schemes to implement usual structured programming constructs like if-then-else and loops. In this chapter we will revisit these constructs and exploit a feature of the ARM instruction set that we have not learnt yet.
</p>
<p><span id="more-823"></span></p>
<h2>Playing with loops</h2>
<p>
The most generic form of loop is this one.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #b1b100;">while</span> <span style="color: #009900;">(</span>E<span style="color: #009900;">)</span>
  S<span style="color: #339933;">;</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">while (E)
  S;</p></div>

<p>
There are also two special forms, which are actually particular incarnations of the one shown above but are interesting as well.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #b1b100;">for</span> <span style="color: #009900;">(</span>i <span style="color: #339933;">=</span> lower<span style="color: #339933;">;</span> i <span style="color: #339933;">&lt;=</span> upper<span style="color: #339933;">;</span> i <span style="color: #339933;">+=</span> step<span style="color: #009900;">)</span>
  S<span style="color: #339933;">;</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">for (i = lower; i &lt;= upper; i += step)
  S;</p></div>


<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #b1b100;">do</span> 
  S
<span style="color: #b1b100;">while</span> <span style="color: #009900;">(</span>E<span style="color: #009900;">)</span><span style="color: #339933;">;</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">do 
  S
while (E);</p></div>

<p>
Some languages, like Pascal, have constructs like this one.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="pascal" style="font-family:monospace;"><span style="color: #000000; font-weight: bold;">repeat</span>
  S
<span style="color: #000000; font-weight: bold;">until</span> E<span style="color: #000066;">;</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">repeat
  S
until E;</p></div>

<p>
but this is like a <code>do S while (!E)</code>.
</p>
<p>
We can manipulate loops to get a form that may be more convenient. For instance.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;">   <span style="color: #b1b100;">do</span> 
     S
   <span style="color: #b1b100;">while</span> <span style="color: #009900;">(</span>E<span style="color: #009900;">)</span><span style="color: #339933;">;</span>
&nbsp;
<span style="color: #808080; font-style: italic;">/* Can be rewritten as */</span>
&nbsp;
   S<span style="color: #339933;">;</span>
   <span style="color: #b1b100;">while</span> <span style="color: #009900;">(</span>E<span style="color: #009900;">)</span>
      S<span style="color: #339933;">;</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">   do 
     S
   while (E);

/* Can be rewritten as */

   S;
   while (E)
      S;</p></div>


<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;">   <span style="color: #b1b100;">while</span> <span style="color: #009900;">(</span>E<span style="color: #009900;">)</span>
     S<span style="color: #339933;">;</span>
&nbsp;
<span style="color: #808080; font-style: italic;">/* Can be rewritten as */</span>
&nbsp;
   <span style="color: #b1b100;">if</span> <span style="color: #009900;">(</span>E<span style="color: #009900;">)</span>
   <span style="color: #009900;">{</span>
      <span style="color: #b1b100;">do</span>
        S
      <span style="color: #b1b100;">while</span> <span style="color: #009900;">(</span>E<span style="color: #009900;">)</span><span style="color: #339933;">;</span>
   <span style="color: #009900;">}</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">   while (E)
     S;

/* Can be rewritten as */

   if (E)
   {
      do
        S
      while (E);
   }</p></div>

<p>
The last manipulation is interesting, because we can avoid the <code>if-then</code> if we directly go to the <code>while</code> part.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #808080; font-style: italic;">/* This is not valid C */</span>
<span style="color: #b1b100;">goto</span> check<span style="color: #339933;">;</span>
<span style="color: #b1b100;">do</span>
  S
check<span style="color: #339933;">:</span> <span style="color: #b1b100;">while</span> <span style="color: #009900;">(</span>E<span style="color: #009900;">)</span><span style="color: #339933;">;</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* This is not valid C */
goto check;
do
  S
check: while (E);</p></div>

<p>
In valid C, the above transformation would be written as follows.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #b1b100;">goto</span> check<span style="color: #339933;">;</span>
loop<span style="color: #339933;">:</span>
  S<span style="color: #339933;">;</span>
check<span style="color: #339933;">:</span>
  <span style="color: #b1b100;">if</span> <span style="color: #009900;">(</span>E<span style="color: #009900;">)</span> <span style="color: #b1b100;">goto</span> loop<span style="color: #339933;">;</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">goto check;
loop:
  S;
check:
  if (E) goto loop;</p></div>

<p>
Which looks much uglier than abusing a bit C syntax.
</p>
<h2>The -s suffix</h2>
<p>
So far, when checking the condition of an <code>if</code> or <code>while</code>, we have evaluated the condition and then used the <code>cmp</code> intruction to update <code>cpsr</code>. The update of the <code>cpsr</code> is mandatory for our conditional codes, no matter if we use branching or predication. But <code>cmp</code> is not the only way to update <code>cpsr</code>. In fact many instructions can update it.
</p>
<p>
By default an instruction does not update <code>cpsr</code> unless we append the suffix <code>-s</code>. So instead of the instruction <code>add</code> or <code>sub</code> we write <code>adds</code> or <code>subs</code>. The result of the instruction (what would be stored in the destination register) is used to update <code>cpsr</code>.
</p>
<p>
How can we use this? Well, consider this simple loop counting backwards.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> for <span style="color: #009900; font-weight: bold;">(</span><span style="color: #00007f; font-weight: bold;">int</span> i = <span style="color: #ff0000;">100</span> <span style="color: #666666; font-style: italic;">; i &gt;= 0; i--) */</span>
<span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">100</span>
<span style="color: #00007f; font-weight: bold;">loop</span><span style="color: #339933;">:</span>
  <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">do</span> something <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">sub</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>      <span style="color: #339933;">/*</span> r1 ← r1 <span style="color: #339933;">-</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">cmp</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>          <span style="color: #339933;">/*</span> update cpsr with r1 <span style="color: #339933;">-</span> <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
  bge <span style="color: #00007f; font-weight: bold;">loop</span>            <span style="color: #339933;">/*</span> branch if r1 &gt;= <span style="color: #ff0000;">100</span> <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* for (int i = 100 ; i &gt;= 0; i--) */
mov r1, #100
loop:
  /* do something */
  sub r1, r1, #1      /* r1 ← r1 - 1 */
  cmp r1, #0          /* update cpsr with r1 - 0 */
  bge loop            /* branch if r1 &gt;= 100 */</p></div>

<p>
If we replace <code>sub</code> by <code>subs</code> then <code>cpsr</code> will be updated with the result of the subtration. This means that the flags N, Z, C and V will be updated, so we can use a branch right after <code>subs</code>. In our case we want to jump back to loop only if <code>i &gt;= 0</code>, this is when the result is non-negative. We can use <code>bpl</code> to achieve this.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> for <span style="color: #009900; font-weight: bold;">(</span><span style="color: #00007f; font-weight: bold;">int</span> i = <span style="color: #ff0000;">100</span> <span style="color: #666666; font-style: italic;">; i &gt;= 0; i--) */</span>
<span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">100</span>
<span style="color: #00007f; font-weight: bold;">loop</span><span style="color: #339933;">:</span>
  <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">do</span> something <span style="color: #339933;">*/</span>
  subs r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>      <span style="color: #339933;">/*</span> r1 ← r1 <span style="color: #339933;">-</span> <span style="color: #ff0000;">1</span>  <span style="color: #00007f; font-weight: bold;">and</span> update cpsr with the final r1 <span style="color: #339933;">*/</span>
  <span style="color: #46aa03; font-weight: bold;">bpl</span> <span style="color: #00007f; font-weight: bold;">loop</span>             <span style="color: #339933;">/*</span> branch if the previous <span style="color: #00007f; font-weight: bold;">sub</span> computed a positive number <span style="color: #009900; font-weight: bold;">(</span>N flag <span style="color: #00007f; font-weight: bold;">in</span> cpsr is <span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* for (int i = 100 ; i &gt;= 0; i--) */
mov r1, #100
loop:
  /* do something */
  subs r1, r1, #1      /* r1 ← r1 - 1  and update cpsr with the final r1 */
  bpl loop             /* branch if the previous sub computed a positive number (N flag in cpsr is 0) */</p></div>

<p>
It is a bit tricky to get these things right (this is why we use compilers). For instance this similar, but not identical, loop would use <code>bne</code> instead of <code>bpl</code>. Here the condition is <code>ne</code> (not equal). It would be nice to have an alias like <code>nz</code> (not zero) but, unfortunately, this does not exist in ARM.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> for <span style="color: #009900; font-weight: bold;">(</span><span style="color: #00007f; font-weight: bold;">int</span> i = <span style="color: #ff0000;">100</span> <span style="color: #666666; font-style: italic;">; i &gt; 0; i--). Note here i &gt; 0, not i &gt;= 0 as in the example above */</span>
<span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">100</span>
<span style="color: #00007f; font-weight: bold;">loop</span><span style="color: #339933;">:</span>
  <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">do</span> something <span style="color: #339933;">*/</span>
  subs r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>      <span style="color: #339933;">/*</span> r1 ← r1 <span style="color: #339933;">-</span> <span style="color: #ff0000;">1</span>  <span style="color: #00007f; font-weight: bold;">and</span> update cpsr with the final r1 <span style="color: #339933;">*/</span>
  bne <span style="color: #00007f; font-weight: bold;">loop</span>             <span style="color: #339933;">/*</span> branch if the previous <span style="color: #00007f; font-weight: bold;">sub</span> computed a number that is <span style="color: #00007f; font-weight: bold;">not</span> zero <span style="color: #009900; font-weight: bold;">(</span>Z flag <span style="color: #00007f; font-weight: bold;">in</span> cpsr is <span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* for (int i = 100 ; i &gt; 0; i--). Note here i &gt; 0, not i &gt;= 0 as in the example above */
mov r1, #100
loop:
  /* do something */
  subs r1, r1, #1      /* r1 ← r1 - 1  and update cpsr with the final r1 */
  bne loop             /* branch if the previous sub computed a number that is not zero (Z flag in cpsr is 0) */</p></div>

<p>
A rule of thumb where we may want to apply the use of the -s suffix is in codes in the following form.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;">s <span style="color: #339933;">=</span> ...
<span style="color: #b1b100;">if</span> <span style="color: #009900;">(</span>s @ <span style="color: #0000dd;">0</span><span style="color: #009900;">)</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">s = ...
if (s @ 0)</p></div>

<p>
where <code>@</code> means any comparison respect 0 (equals, different, lower, etc.).
</p>
<h2>Operating 64-bit numbers</h2>
<p>
As an example of using the suffix -s we will implement three 64-bit integer operations in ARM: addition, subtraction and multiplication. Remember that ARM is a 32-bit architecture, so everything is 32-bit minded. If we only use 32-bit numbers, this is not a problem, but if for some reason we need 64-bit numbers things get a bit more complicated. We will represent a 64-bit number as two 32-bit numbers, the lower and higher part. This way a 64-bit number n represented using two 32-bit parts, n<sub>lower</sub> and n<sub>higher</sub> will have the value n = 2<sup>32</sup> × n<sub>higher</sub> + n<sub>lower</sub>
</p>
<p>
We will, obviously, need to kep the 32-bit somewhere. When keeping them in registers, we will use two consecutive registers (e.g. r1 and r2, that we will write it as <code>{r1,r2}</code>) and we will keep the higher part in the higher numbered register. When keeping a 64-bit number in memory, we will store in two consecutive addresses the two parts, being the lower one in the lower address. The address will be 8-byte aligned.
</p>
<h3>Addition</h3>
<p>
Adding two 64-bit numbers using 32-bit operands means adding first the lower part and then adding the higher parts but taking into account a possible carry from the lower part. With our current knowledge we could write something like this (assume the first number is in <code>{r2,r3}</code>, the second in <code>{r4,r5}</code> and the result will be in <code>{r0,r1}</code>).
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">add</span> r1<span style="color: #339933;">,</span> r3<span style="color: #339933;">,</span> r5      <span style="color: #339933;">/*</span> First we <span style="color: #00007f; font-weight: bold;">add</span> the higher part <span style="color: #339933;">*/</span>
                    <span style="color: #339933;">/*</span> r1 ← r3 <span style="color: #339933;">+</span> r5 <span style="color: #339933;">*/</span>
adds r0<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r4     <span style="color: #339933;">/*</span> Now we <span style="color: #00007f; font-weight: bold;">add</span> the lower part <span style="color: #00007f; font-weight: bold;">and</span> we update cpsr <span style="color: #339933;">*/</span>
                    <span style="color: #339933;">/*</span> r0 ← r2 <span style="color: #339933;">+</span> r4 <span style="color: #339933;">*/</span>
<span style="color: #adadad; font-style: italic;">addc</span>s r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>    <span style="color: #339933;">/*</span> If adding the lower part caused carry<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">add</span> <span style="color: #ff0000;">1</span> to the higher part <span style="color: #339933;">*/</span>
                    <span style="color: #339933;">/*</span> if C = <span style="color: #ff0000;">1</span> then r1 ← r1 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
                    <span style="color: #339933;">/*</span> Note that here the suffix <span style="color: #339933;">-</span>s is <span style="color: #00007f; font-weight: bold;">not</span> applied<span style="color: #339933;">,</span> <span style="color: #339933;">-</span><span style="color: #46aa03; font-weight: bold;">cs</span> means carry set <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">add r1, r3, r5      /* First we add the higher part */
                    /* r1 ← r3 + r5 */
adds r0, r2, r4     /* Now we add the lower part and we update cpsr */
                    /* r0 ← r2 + r4 */
addcs r1, r1, #1    /* If adding the lower part caused carry, add 1 to the higher part */
                    /* if C = 1 then r1 ← r1 + 1 */
                    /* Note that here the suffix -s is not applied, -cs means carry set */</p></div>

<p>
This would work. Fortunately ARM provides an instructions <code>adc</code> which adds two numbers and the carry flag. So we could rewrite the above code with just two instructions.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">adds r0<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r4     <span style="color: #339933;">/*</span> First <span style="color: #00007f; font-weight: bold;">add</span> the lower part <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
                    <span style="color: #339933;">/*</span> r0 ← r2 <span style="color: #339933;">+</span> r4 <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">adc</span> r1<span style="color: #339933;">,</span> r3<span style="color: #339933;">,</span> r5      <span style="color: #339933;">/*</span> Now <span style="color: #00007f; font-weight: bold;">add</span> the higher part plus the carry from the lower one <span style="color: #339933;">*/</span>
                    <span style="color: #339933;">/*</span> r1 ← r3 <span style="color: #339933;">+</span> r5 <span style="color: #339933;">+</span> C <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">adds r0, r2, r4     /* First add the lower part and update cpsr */
                    /* r0 ← r2 + r4 */
adc r1, r3, r5      /* Now add the higher part plus the carry from the lower one */
                    /* r1 ← r3 + r5 + C */</p></div>

<h3>Subtraction</h3>
<p>
Subtracting two numbers is similar to adding them. In ARM when subtracting two numbers using <code>subs</code>, if we need to borrow (because the second operand is larger than the first) then C will be disabled (C will be 0). If we do not need to borrow, C will be enabled (C will be 1). This is a bit surprising but consistent with the remainder of the architecture (check in chapter 5 conditions CS/HS and CC/LO). Similar to <code>adc</code> there is a <code>sbc</code> which performs a normal subtraction if C is 1. Otherwise it subtracts one more element. Again, this is consistent on how C works in the <code>subs</code> instruction.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">subs r0<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r4     <span style="color: #339933;">/*</span> First subtract the lower part <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
                    <span style="color: #339933;">/*</span> r0 ← r2 <span style="color: #339933;">-</span> r4 <span style="color: #339933;">*/</span>
sbc r1<span style="color: #339933;">,</span> r3<span style="color: #339933;">,</span> r5      <span style="color: #339933;">/*</span> Now subtract the higher part plus the <span style="color: #00007f; font-weight: bold;">NOT</span> of the carry from the lower one <span style="color: #339933;">*/</span>
                    <span style="color: #339933;">/*</span> r1 ← r3 <span style="color: #339933;">-</span> r5 <span style="color: #339933;">-</span> ~C <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">subs r0, r2, r4     /* First subtract the lower part and update cpsr */
                    /* r0 ← r2 - r4 */
sbc r1, r3, r5      /* Now subtract the higher part plus the NOT of the carry from the lower one */
                    /* r1 ← r3 - r5 - ~C */</p></div>

<h3>Multiplication</h3>
<p>
Multiplying two 64-bit numbers is a tricky thing. When we multiply two N-bit numbers the result may need up to 2*N-bits. So when multiplying two 64-bit numbers we may need a 128-bit number. For the sake of simplicity we will assume that this does not happen and 64-bit will be enough. Our 64-bit numbers are two 32-bit integers, so a 64-bit x is actually x = 2<sup>32</sup> × x<sub>1</sub> + x<sub>0</sub>, where x<sub>1</sub> and x<sub>0</sub> are two 32-bit numbers. Similarly another 64-bit number y would be y = 2<sup>32</sup> × y<sub>1</sub> + y<sub>0</sub>. Multiplying x and y yields z where z = 2<sup>64</sup> × x<sub>1</sub> × y<sub>1</sub> + 2<sup>32</sup> × (x<sub>0</sub> × y<sub>1</sub> + x<sub>1</sub> × y<sub>0</sub>) + x<sub>0</sub> × y<sub>0</sub>. Well, now our problem is multiplying each x<sub>i</sub> by y<sub>i</sub>, but again we may need 64-bit to represent the value.
</p>
<p>
ARM provides a bunch of different instructions for multiplication. Today we will see just three of them. If we are multiplying 32-bits and we do not care about the result not fitting in a 32-bit number we can use <code>mul Rd, Rsource1, Rsource2</code>. Unfortunately it does not set any flag in the <code>cpsr</code> useful for detecting an overflow of the multiplication (i.e. when the result does not fit in the 32-bit range). This instruction is the fastest one of the three. If we do want the 64-bit resulting from the multiplication, we have two other instructions <code>smull</code> and <code>umull</code>. The former is used when we multiply to numbers in two’s complement, the latter when we represent unsigned values. Their syntax is <code>{s,u}mull RdestLower, RdestHigher, Rsource1, Rsource2</code>. The lower part of the 64-bit result is kept in the register <code>RdestLower</code> and the higher part in he register <code>RdestHigher</code>.
</p>
<p>
In this example we have to use <code>umull</code> otherwise the 32-bit lower parts might end being interpreted as negative numbers, giving negative intermediate values. That said, we can now multiply x<sub>0</sub> and y<sub>0</sub>. Recall that we have the two 64-bit numbers in <code>r2,r3</code> and <code>r4,r5</code> pairs of registers. So first multiply <code>r2</code> and <code>r4</code>. Note the usage of <code>r0</code> since this will be its final value. In contrast, register <code>r6</code> will be used later.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">umull r0<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r4</pre></td></tr></tbody></table><p class="theCode" style="display:none;">umull r0, r6, r2, r4</p></div>

<p>
Now let’s multiply x<sub>0</sub> by y<sub>1</sub> and x<sub>1</sub> by y<sub>0</sub>. This is <code>r3</code> by <code>r4</code> and <code>r2</code> by <code>r5</code>. Note how we overwrite <code>r4</code> and <code>r5</code> in the second multiplication. This is fine since we will not need them anymore.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">umull r7<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r3<span style="color: #339933;">,</span> r4
umull r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r5</pre></td></tr></tbody></table><p class="theCode" style="display:none;">umull r7, r8, r3, r4
umull r4, r5, r2, r5</p></div>

<p>
There is no need to make the multiplication of x<sub>1</sub> by y<sub>1</sub> because if it gives a nonzero value, it will always overflow a 64-bit number. This means that if both <code>r3</code> and <code>r5</code> were nonzero, the multiplication will never fit a 64-bit. This is a suficient condition, but not a necessary one. The number might overflow when adding the intermediate values that will result in <code>r1</code>.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">adds r2<span style="color: #339933;">,</span> r7<span style="color: #339933;">,</span> r4
<span style="color: #00007f; font-weight: bold;">adc</span> r1<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r6</pre></td></tr></tbody></table><p class="theCode" style="display:none;">adds r2, r7, r4
adc r1, r2, r6</p></div>

<p>
Let’s package this code in a nice function in a program to see if it works. We will multiply numbers 12345678901 (this is 2×2<sup>32</sup> + 3755744309) and 12345678 and print the result.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> <span style="color: #339933;">--</span> mult64<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
message <span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"Multiplication of %lld by %lld is %lld\n"</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">8</span>
number_a_low<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">3755744309</span>
number_a_high<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">2</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">8</span>
number_b_low<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">12345678</span>
number_b_high<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">0</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
<span style="color: #339933;">/*</span> Note<span style="color: #339933;">:</span> This is <span style="color: #00007f; font-weight: bold;">not</span> the most efficient way to doa <span style="color: #ff0000;">64</span><span style="color: #339933;">-</span>bit multiplication<span style="color: #339933;">.</span>
   This is for illustration purposes <span style="color: #339933;">*/</span>
mult64<span style="color: #339933;">:</span>
   <span style="color: #339933;">/*</span> The argument will be passed <span style="color: #00007f; font-weight: bold;">in</span> r0<span style="color: #339933;">,</span> r1 <span style="color: #00007f; font-weight: bold;">and</span> r2<span style="color: #339933;">,</span> r3 <span style="color: #00007f; font-weight: bold;">and</span> returned <span style="color: #00007f; font-weight: bold;">in</span> r0<span style="color: #339933;">,</span> r1 <span style="color: #339933;">*/</span>
   <span style="color: #339933;">/*</span> Keep the registers that we are going to <span style="color: #0000ff; font-weight: bold;">write</span> <span style="color: #339933;">*/</span>
   <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> r7<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
   <span style="color: #339933;">/*</span> For covenience<span style="color: #339933;">,</span> move <span style="color: #009900; font-weight: bold;">{</span>r0<span style="color: #339933;">,</span>r1<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #00007f; font-weight: bold;">into</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span>r5<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">*/</span>
   <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> r0   <span style="color: #339933;">/*</span> r0 ← r4 <span style="color: #339933;">*/</span>
   <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> r1   <span style="color: #339933;">/*</span> r5 ← r1 <span style="color: #339933;">*/</span>
&nbsp;
   umull r0<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r4    <span style="color: #339933;">/*</span> <span style="color: #009900; font-weight: bold;">{</span>r0<span style="color: #339933;">,</span>r6<span style="color: #009900; font-weight: bold;">}</span> ← r2 <span style="color: #339933;">*</span> r4 <span style="color: #339933;">*/</span>
   umull r7<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> r3<span style="color: #339933;">,</span> r4    <span style="color: #339933;">/*</span> <span style="color: #009900; font-weight: bold;">{</span>r7<span style="color: #339933;">,</span><span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #009900; font-weight: bold;">}</span> ← r3 <span style="color: #339933;">*</span> r4 <span style="color: #339933;">*/</span>
   umull r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r5    <span style="color: #339933;">/*</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span>r5<span style="color: #009900; font-weight: bold;">}</span> ← r2 <span style="color: #339933;">*</span> r5 <span style="color: #339933;">*/</span>
   adds r2<span style="color: #339933;">,</span> r7<span style="color: #339933;">,</span> r4         <span style="color: #339933;">/*</span> r2 ← r7 <span style="color: #339933;">+</span> r4 <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
   <span style="color: #00007f; font-weight: bold;">adc</span> r1<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r6          <span style="color: #339933;">/*</span> r1 ← r2 <span style="color: #339933;">+</span> r6 <span style="color: #339933;">+</span> C <span style="color: #339933;">*/</span>
&nbsp;
   <span style="color: #339933;">/*</span> Restore registers <span style="color: #339933;">*/</span>
   <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> r7<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
   <span style="color: #46aa03; font-weight: bold;">bx</span> lr                   <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Leave</span> mult64 <span style="color: #339933;">*/</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">global</span> main
main<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> r7<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>       <span style="color: #339933;">/*</span> Keep the registers we are going to modify <span style="color: #339933;">*/</span>
                                        <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r8</span> is <span style="color: #00007f; font-weight: bold;">not</span> actually used here<span style="color: #339933;">,</span> but this way 
                                           the <span style="color: #0000ff; font-weight: bold;">stack</span> is already <span style="color: #ff0000;">8</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> Load the numbers from memory <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span>r5<span style="color: #009900; font-weight: bold;">}</span> ← a <span style="color: #339933;">*/</span>
    ldr r4<span style="color: #339933;">,</span> addr_number_a_low       <span style="color: #339933;">/*</span> r4 ← &amp;a_low <span style="color: #339933;">*/</span>
    ldr r4<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #009900; font-weight: bold;">]</span>                    <span style="color: #339933;">/*</span> r4 ← <span style="color: #339933;">*</span>r4 <span style="color: #339933;">*/</span>
    ldr r5<span style="color: #339933;">,</span> addr_number_a_high      <span style="color: #339933;">/*</span> r5 ← &amp;a_high  <span style="color: #339933;">*/</span>
    ldr r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r5<span style="color: #009900; font-weight: bold;">]</span>                    <span style="color: #339933;">/*</span> r5 ← <span style="color: #339933;">*</span>r5 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> <span style="color: #009900; font-weight: bold;">{</span>r6<span style="color: #339933;">,</span>r7<span style="color: #009900; font-weight: bold;">}</span> ← b <span style="color: #339933;">*/</span>
    ldr r6<span style="color: #339933;">,</span> addr_number_b_low       <span style="color: #339933;">/*</span> r6 ← &amp;b_low  <span style="color: #339933;">*/</span>
    ldr r6<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r6<span style="color: #009900; font-weight: bold;">]</span>                    <span style="color: #339933;">/*</span> r6 ← <span style="color: #339933;">*</span>r6 <span style="color: #339933;">*/</span>
    ldr r7<span style="color: #339933;">,</span> addr_number_b_high      <span style="color: #339933;">/*</span> r7 ← &amp;b_high  <span style="color: #339933;">*/</span>
    ldr r7<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r7<span style="color: #009900; font-weight: bold;">]</span>                    <span style="color: #339933;">/*</span> r7 ← <span style="color: #339933;">*</span>r7 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> Now prepare the <span style="color: #00007f; font-weight: bold;">call</span> to mult64
    <span style="color: #339933;">/*</span> 
       The first number is passed <span style="color: #00007f; font-weight: bold;">in</span> 
       registers <span style="color: #009900; font-weight: bold;">{</span>r0<span style="color: #339933;">,</span>r1<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #00007f; font-weight: bold;">and</span> the second one <span style="color: #00007f; font-weight: bold;">in</span> <span style="color: #009900; font-weight: bold;">{</span>r2<span style="color: #339933;">,</span>r3<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> r4                  <span style="color: #339933;">/*</span> r0 ← r4 <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> r5                  <span style="color: #339933;">/*</span> r1 ← r5 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r2<span style="color: #339933;">,</span> r6                  <span style="color: #339933;">/*</span> r2 ← r6 <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r3<span style="color: #339933;">,</span> r7                  <span style="color: #339933;">/*</span> r3 ← r7 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #46aa03; font-weight: bold;">bl</span> mult64                  <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> mult64 function <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> The result of the multiplication is <span style="color: #00007f; font-weight: bold;">in</span> r0<span style="color: #339933;">,</span>r1 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> Now prepare the <span style="color: #00007f; font-weight: bold;">call</span> to printf <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> We have to pass &amp;message<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span>r5<span style="color: #009900; font-weight: bold;">}</span><span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">{</span>r6<span style="color: #339933;">,</span>r7<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #00007f; font-weight: bold;">and</span> <span style="color: #009900; font-weight: bold;">{</span>r0<span style="color: #339933;">,</span>r1<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r1<span style="color: #009900; font-weight: bold;">}</span>                   <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Push</span> r1 onto the <span style="color: #0000ff; font-weight: bold;">stack</span><span style="color: #339933;">.</span> 4th <span style="color: #009900; font-weight: bold;">(</span>higher<span style="color: #009900; font-weight: bold;">)</span> parameter <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r0<span style="color: #009900; font-weight: bold;">}</span>                   <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Push</span> r0 onto the <span style="color: #0000ff; font-weight: bold;">stack</span><span style="color: #339933;">.</span> 4th <span style="color: #009900; font-weight: bold;">(</span>lower<span style="color: #009900; font-weight: bold;">)</span> parameter <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r7<span style="color: #009900; font-weight: bold;">}</span>                   <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Push</span> r7 onto the <span style="color: #0000ff; font-weight: bold;">stack</span><span style="color: #339933;">.</span> 3rd <span style="color: #009900; font-weight: bold;">(</span>higher<span style="color: #009900; font-weight: bold;">)</span> parameter <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r6<span style="color: #009900; font-weight: bold;">}</span>                   <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Push</span> r6 onto the <span style="color: #0000ff; font-weight: bold;">stack</span><span style="color: #339933;">.</span> 3rd <span style="color: #009900; font-weight: bold;">(</span>lower<span style="color: #009900; font-weight: bold;">)</span> parameter <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r3<span style="color: #339933;">,</span> r5                  <span style="color: #339933;">/*</span> r3 ← r5<span style="color: #339933;">.</span>                2rd <span style="color: #009900; font-weight: bold;">(</span>higher<span style="color: #009900; font-weight: bold;">)</span> parameter <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r2<span style="color: #339933;">,</span> r4                  <span style="color: #339933;">/*</span> r2 ← r4<span style="color: #339933;">.</span>                2nd <span style="color: #009900; font-weight: bold;">(</span>lower<span style="color: #009900; font-weight: bold;">)</span> parameter <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> addr_of_message     <span style="color: #339933;">/*</span> r0 ← &amp;message           1st parameter <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> printf                   <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Call</span> printf <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">add</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">16</span>             <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">sp</span> ← <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">+</span> <span style="color: #ff0000;">16</span> <span style="color: #339933;">*/</span>
                                <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Pop</span> the two registers we pushed above <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>                  <span style="color: #339933;">/*</span> r0 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> r7<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>        <span style="color: #339933;">/*</span> Restore the registers we kept <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr                       <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Leave</span> main <span style="color: #339933;">*/</span>
&nbsp;
addr_of_message <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> message
addr_number_a_low<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> number_a_low
addr_number_a_high<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> number_a_high
addr_number_b_low<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> number_b_low
addr_number_b_high<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> number_b_high</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* -- mult64.s */
.data

.align 4
message : .asciz "Multiplication of %lld by %lld is %lld\n"

.align 8
number_a_low: .word 3755744309
number_a_high: .word 2

.align 8
number_b_low: .word 12345678
number_b_high: .word 0

.text

/* Note: This is not the most efficient way to doa 64-bit multiplication.
   This is for illustration purposes */
mult64:
   /* The argument will be passed in r0, r1 and r2, r3 and returned in r0, r1 */
   /* Keep the registers that we are going to write */
   push {r4, r5, r6, r7, r8, lr}
   /* For covenience, move {r0,r1} into {r4,r5} */
   mov r4, r0   /* r0 ← r4 */
   mov r5, r1   /* r5 ← r1 */

   umull r0, r6, r2, r4    /* {r0,r6} ← r2 * r4 */
   umull r7, r8, r3, r4    /* {r7,r8} ← r3 * r4 */
   umull r4, r5, r2, r5    /* {r4,r5} ← r2 * r5 */
   adds r2, r7, r4         /* r2 ← r7 + r4 and update cpsr */
   adc r1, r2, r6          /* r1 ← r2 + r6 + C */

   /* Restore registers */
   pop {r4, r5, r6, r7, r8, lr}
   bx lr                   /* Leave mult64 */

.global main
main:
    push {r4, r5, r6, r7, r8, lr}       /* Keep the registers we are going to modify */
                                        /* r8 is not actually used here, but this way 
                                           the stack is already 8-byte aligned */
    /* Load the numbers from memory */
    /* {r4,r5} ← a */
    ldr r4, addr_number_a_low       /* r4 ← &amp;a_low */
    ldr r4, [r4]                    /* r4 ← *r4 */
    ldr r5, addr_number_a_high      /* r5 ← &amp;a_high  */
    ldr r5, [r5]                    /* r5 ← *r5 */

    /* {r6,r7} ← b */
    ldr r6, addr_number_b_low       /* r6 ← &amp;b_low  */
    ldr r6, [r6]                    /* r6 ← *r6 */
    ldr r7, addr_number_b_high      /* r7 ← &amp;b_high  */
    ldr r7, [r7]                    /* r7 ← *r7 */

    /* Now prepare the call to mult64
    /* 
       The first number is passed in 
       registers {r0,r1} and the second one in {r2,r3}
    */
    mov r0, r4                  /* r0 ← r4 */
    mov r1, r5                  /* r1 ← r5 */

    mov r2, r6                  /* r2 ← r6 */
    mov r3, r7                  /* r3 ← r7 */

    bl mult64                  /* call mult64 function */
    /* The result of the multiplication is in r0,r1 */

    /* Now prepare the call to printf */
    /* We have to pass &amp;message, {r4,r5}, {r6,r7} and {r0,r1} */
    push {r1}                   /* Push r1 onto the stack. 4th (higher) parameter */
    push {r0}                   /* Push r0 onto the stack. 4th (lower) parameter */
    push {r7}                   /* Push r7 onto the stack. 3rd (higher) parameter */
    push {r6}                   /* Push r6 onto the stack. 3rd (lower) parameter */
    mov r3, r5                  /* r3 ← r5.                2rd (higher) parameter */
    mov r2, r4                  /* r2 ← r4.                2nd (lower) parameter */
    ldr r0, addr_of_message     /* r0 ← &amp;message           1st parameter */
    bl printf                   /* Call printf */
    add sp, sp, #16             /* sp ← sp + 16 */
                                /* Pop the two registers we pushed above */

    mov r0, #0                  /* r0 ← 0 */
    pop {r4, r5, r6, r7, r8, lr}        /* Restore the registers we kept */
    bx lr                       /* Leave main */

addr_of_message : .word message
addr_number_a_low: .word number_a_low
addr_number_a_high: .word number_a_high
addr_number_b_low: .word number_b_low
addr_number_b_high: .word number_b_high</p></div>

<p>
Observe first that we have the addresses of the lower and upper part of each number. Instead of this we could load them by just using an offset, as we saw in chapter 8. So, in lines 41 to 44 we could have done the following.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>40
41
42
43
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">    <span style="color: #339933;">/*</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span>r5<span style="color: #009900; font-weight: bold;">}</span> ← a <span style="color: #339933;">*/</span>
    ldr r4<span style="color: #339933;">,</span> addr_number_a_low       <span style="color: #339933;">/*</span> r4 ← &amp;a_low <span style="color: #339933;">*/</span>
    ldr r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #339933;">,</span> <span style="color: #339933;">+</span>#<span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>               <span style="color: #339933;">/*</span> r5 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    ldr r4<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #009900; font-weight: bold;">]</span>                    <span style="color: #339933;">/*</span> r4 ← <span style="color: #339933;">*</span>r4  <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">    /* {r4,r5} ← a */
    ldr r4, addr_number_a_low       /* r4 ← &amp;a_low */
    ldr r5, [r4, +#4]               /* r5 ← *(r4 + 4) */
    ldr r4, [r4]                    /* r4 ← *r4  */</p></div>

<p>
In the function <code>mult64</code> we pass the first value (x) as <code>r0,r1</code> and the second value (y) as <code>r2,r3</code>. The result is stored in <code>r0,r1</code>. We move the values to the appropiate registers for parameter passing in lines 57 to 61.
</p>
<p>
Printing the result is a bit complicated. 64-bits must be passed as pairs of consecutive registers where the lower part is in an even numbered register. Since we pass the address of the message<br>
in <code>r0</code> we cannot pass the first 64-bit integer in <code>r1</code>. So we skip <code>r1</code> and we use <code>r2</code> and <code>r3</code> for the first argument. But now we have run out of registers for parameter passing. When this happens, we have to use the stack for parameter passing.
</p>
<p>
Two rules have to be taken into account when passing data in the stack.
</p>
<ol>
<li>You must ensure that the stack is aligned for the data you are going to pass (by adjusting the stack first). So, for 64-bit numbers, the stack must be 8-byte aligned. If you pass an 32-bit number and then a 64-bit number, you will have to skip 4 bytes before passing the 64-bit number. Do not forget to keep the stack always 8-byte aligned per the Procedure Call Standard for ARM Architecture (AAPCS) requirement.
</li><li>An argument with a lower position number in the call must have a lower address in the stack. So we have to pass the arguments in opposite order.
</li></ol>
<p>
The second rule is what explains why we push first <code>r1</code> and then <code>r0</code>, when they are the registers containing the last 64-bit number (the result of the multiplication) we want to pass to <code>printf</code>.
</p>
<p>
Note that in the example above, we cannot pass the parameters in the stack using <code>push {r0,r1,r6,r7}</code>, which is equivalent to <code>push {r0}</code>, <code>push {r1}</code>, <code>push {r6}</code> and <code>push {r7}</code>, but not equivalent to the required order when passing the arguments on the stack.
</p>
<p>
If we run the program we should see something like.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="bash" style="font-family:monospace;">$ .<span style="color: #000000; font-weight: bold;">/</span>mult64_2
Multiplication of <span style="color: #000000;">12345678901</span> by <span style="color: #000000;">12345678</span> is <span style="color: #000000;">152415776403139878</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ ./mult64_2
Multiplication of 12345678901 by 12345678 is 152415776403139878</p></div>

<p>
That’s all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/&amp;text=ARM+assembler+in+Raspberry+Pi+%E2%80%93+Chapter+12+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>				<p class="tags"><span><a href="http://thinkingeek.com/tag/arm/" rel="tag">arm</a>, <a href="http://thinkingeek.com/tag/assembler/" rel="tag">assembler</a>, <a href="http://thinkingeek.com/tag/pi/" rel="tag">pi</a>, <a href="http://thinkingeek.com/tag/raspberry/" rel="tag">raspberry</a></span></p>		<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2013/03/16/arm-assembler-raspberry-pi-chapter-11/" rel="prev">ARM assembler in Raspberry Pi – Chapter 11</a></span>
			<span class="next"><a href="http://thinkingeek.com/2013/04/27/capybara-pop-windows-paypal-sandbox/" rel="next">Capybara, pop up windows and the new PayPal sandbox</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
			<h3>19 thoughts on “<span>ARM assembler in Raspberry Pi – Chapter 12</span>”</h3>
		<ul class="commentlist">
				<li class="comment even thread-even depth-1 parent" id="comment-1280">
				<div id="div-comment-1280" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/27f1466f93382187b22a3c996d48ce3c?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/27f1466f93382187b22a3c996d48ce3c?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/27f1466f93382187b22a3c996d48ce3c?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-27f1466f93382187b22a3c996d48ce3c-0">			<cite class="fn">Vibe</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/#comment-1280">
			May 10, 2013 at 12:43 pm</a>		</div>

		<p>That’s an excellent tutorial! I read through all 12 chapters and learned a great deal. You have a talent for explaining things in an understandable way. Thank you for sharing this, and keep up the good work!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/?replytocom=1280#respond" onclick="return addComment.moveForm( &quot;div-comment-1280&quot;, &quot;1280&quot;, &quot;respond&quot;, &quot;823&quot; )" aria-label="Reply to Vibe">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment odd alt depth-2" id="comment-110789">
				<div id="div-comment-110789" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/73da913e674bb7397927d61b35ddac56?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/73da913e674bb7397927d61b35ddac56?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/73da913e674bb7397927d61b35ddac56?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-73da913e674bb7397927d61b35ddac56-0">			<cite class="fn"><a href="http://www.maverickexperiments.com" rel="external nofollow" class="url">Allan Rydberg</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/#comment-110789">
			March 23, 2014 at 9:09 pm</a>		</div>

		<p>Me  too.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/?replytocom=110789#respond" onclick="return addComment.moveForm( &quot;div-comment-110789&quot;, &quot;110789&quot;, &quot;respond&quot;, &quot;823&quot; )" aria-label="Reply to Allan Rydberg">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-1392">
				<div id="div-comment-1392" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/d343022f7066553235d373c4014f0c7c?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/d343022f7066553235d373c4014f0c7c?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/d343022f7066553235d373c4014f0c7c?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-d343022f7066553235d373c4014f0c7c-0">			<cite class="fn"><a href="http://www.fourtheye.org" rel="external nofollow" class="url">Bob Wilkinson</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/#comment-1392">
			June 5, 2013 at 6:02 pm</a>		</div>

		<p>I agree that you explain very well. </p>
<p>I have written some of the problems from projecteuler.net in ARM assembly language (<a href="http://www.fourtheye.org/cgi-bin/language.pl?language=asm" rel="nofollow">http://www.fourtheye.org/cgi-bin/language.pl?language=asm</a>). </p>
<p>Please use any of the code in your discussion if it helps.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/?replytocom=1392#respond" onclick="return addComment.moveForm( &quot;div-comment-1392&quot;, &quot;1392&quot;, &quot;respond&quot;, &quot;823&quot; )" aria-label="Reply to Bob Wilkinson">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-1393">
				<div id="div-comment-1393" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-0">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/#comment-1393">
			June 5, 2013 at 7:03 pm</a>		</div>

		<p>Thanks a lot Bob!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/?replytocom=1393#respond" onclick="return addComment.moveForm( &quot;div-comment-1393&quot;, &quot;1393&quot;, &quot;respond&quot;, &quot;823&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1" id="comment-1441">
				<div id="div-comment-1441" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a16ae1e69be669d6f2c5fa0972e39c8a?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a16ae1e69be669d6f2c5fa0972e39c8a?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a16ae1e69be669d6f2c5fa0972e39c8a?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a16ae1e69be669d6f2c5fa0972e39c8a-0">			<cite class="fn"><a href="http://digilander.libero.it/zantaz/" rel="external nofollow" class="url">Mariani Antonio Mario</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/#comment-1441">
			July 1, 2013 at 7:44 pm</a>		</div>

		<p>ultimate nice piece of coding … i stun of this …</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/?replytocom=1441#respond" onclick="return addComment.moveForm( &quot;div-comment-1441&quot;, &quot;1441&quot;, &quot;respond&quot;, &quot;823&quot; )" aria-label="Reply to Mariani Antonio Mario">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1 parent" id="comment-1463">
				<div id="div-comment-1463" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/ac297d867914baddafa4b5976b2ec50c?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/ac297d867914baddafa4b5976b2ec50c?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/ac297d867914baddafa4b5976b2ec50c?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-ac297d867914baddafa4b5976b2ec50c-0">			<cite class="fn">Ricky Roberson</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/#comment-1463">
			July 16, 2013 at 2:52 pm</a>		</div>

		<p>The 12 chapters of this tutorial were just what I was looking for to break into Raspberry Pi assembly language.   I am working on a project where I want to read the GPIO pins repeatedly as fast as possible, accumulate a LOT of data (like say 512KB) on the stack, then write that data to a file on the Raspberry Pi’s SD card.  Any thoughts or suggestions?  Any problem or memory boundary issues for a stack that large?  How would you initiate a file save of that block?   Thanks for any comments you might have…</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/?replytocom=1463#respond" onclick="return addComment.moveForm( &quot;div-comment-1463&quot;, &quot;1463&quot;, &quot;respond&quot;, &quot;823&quot; )" aria-label="Reply to Ricky Roberson">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2" id="comment-1490">
				<div id="div-comment-1490" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-1">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/#comment-1490">
			August 11, 2013 at 7:30 pm</a>		</div>

		<p>Hi Ricky,</p>
<p>thank you for the kind comments. </p>
<p>Regarding to your question and without more information, I think that big stacks should not be a problem as long as they are not unrealistically big (e.g. more than 1 GB).</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/?replytocom=1490#respond" onclick="return addComment.moveForm( &quot;div-comment-1490&quot;, &quot;1490&quot;, &quot;respond&quot;, &quot;823&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-even depth-1 parent" id="comment-2666">
				<div id="div-comment-2666" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/12f4e20df8cbd5492be08e34dad82663?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/12f4e20df8cbd5492be08e34dad82663?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/12f4e20df8cbd5492be08e34dad82663?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-12f4e20df8cbd5492be08e34dad82663-0">			<cite class="fn">Henryk Sarat</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/#comment-2666">
			January 3, 2014 at 2:30 am</a>		</div>

		<p>Great tutorials by the way. I found a type here:</p>
<p>subs r0, r2, r4     /* First add the lower part and update cpsr */<br>
                    /* r0 ← r2 – r4 */<br>
sbc r1, r3, r5      /* Now add the higher part plus the NOT of the carry from the lower one */<br>
                    /* r1 ← r3 – r5 – ~C */</p>
<p>In the comments you say ADD instead of subtract. </p>
<p>Thanks again for this tutorial.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/?replytocom=2666#respond" onclick="return addComment.moveForm( &quot;div-comment-2666&quot;, &quot;2666&quot;, &quot;respond&quot;, &quot;823&quot; )" aria-label="Reply to Henryk Sarat">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2" id="comment-2680">
				<div id="div-comment-2680" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-2">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/#comment-2680">
			January 3, 2014 at 1:03 pm</a>		</div>

		<p>Fixed. Thanks!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/?replytocom=2680#respond" onclick="return addComment.moveForm( &quot;div-comment-2680&quot;, &quot;2680&quot;, &quot;respond&quot;, &quot;823&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1 parent" id="comment-130640">
				<div id="div-comment-130640" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/e782b46ec0d3cdad5bf142f6c8c926da?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/e782b46ec0d3cdad5bf142f6c8c926da?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/e782b46ec0d3cdad5bf142f6c8c926da?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-e782b46ec0d3cdad5bf142f6c8c926da-0">			<cite class="fn">Marek Mularczyk</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/#comment-130640">
			May 16, 2014 at 10:26 pm</a>		</div>

		<p>Hi, </p>
<p>i think i found a better approach to multiplication:</p>
<p><code><br>
umul64:<br>
                stmdb sp!, {r4, r5}</code></p><code>
<p>                mov r5, r1<br>
                mov r4, r0</p>
<p>                umull r0, r1, r2, r4<br>
                umlal r1, r5, r2, r5 // r5 overwritten but not needed anymore<br>
                umlal r1, r5, r3, r4</p>
<p>                ldmia sp!, {r4, r5}<br>
                bx      lr</p>
</code><p><code></code></p>
<p>great tutorial btw.</p>
<p>Cheers!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/?replytocom=130640#respond" onclick="return addComment.moveForm( &quot;div-comment-130640&quot;, &quot;130640&quot;, &quot;respond&quot;, &quot;823&quot; )" aria-label="Reply to Marek Mularczyk">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2" id="comment-131116">
				<div id="div-comment-131116" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-3">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/#comment-131116">
			May 17, 2014 at 3:07 pm</a>		</div>

		<p>Hi Marek,</p>
<p>yes this, I think, the shortest sequence.</p>
<p>It is slightly more complex to understand than the naive code I wrote in the post.</p>
<p>The following explanation is aimed at other readers that may be interested in your approach.</p>
<p>First recall hat the multiplication of two 64 bit numbers A*B is equivalent to (2^32*A1 + A0) * (2^32*B1 + B0), where X1 and X0 are the 32 top bits and 32 lower bits of X. So the multiplication is actually 2^64*A1*B1 + 2^32*(A1*B0 + A0*B1) + A0*B0. Since we do not take into account overflow, we only have to compute A0*B0 and 2^32(A1*B0 + A0*B1). The trick here is realising that A0*B0 yields a 64-bit result that we will have to propagate to the upper 32 bits of the result. The result, R, will then be R0 = Lower32(A0*B0) and R1 = Lower32(A1*B0) + Lower32(A0*B1) + Upper32(A0*B0). Here Lower32 and Upper32 mean the lower and upper 32 bits of the 64-bit value.</p>
<p><code>umul r0, r1, r2, r4  /*  {r0, r1} ← r2 * r4 */</code></p>
<p>Recall that r0 will contain the lower 32-bits of the result and r1 will contain the upper 32-bit of the result at the end of the function. At this point r4 is A0 and r2 is B0. After the instruction, if r1 is nonzero means that r2*r4 did not fit in 32-bit. So r0 is Lower32(A0*B0) and r1 is Upper32(A0*B0).</p>
<p><code>umlal r1, r5, r2, r5 /* This instruction performs the following operation<br>
      {lower, upper} ← r2 * r5<br>
      r1 ← lower + r1<br>
      r5 ← upper + r5 + C  (C=1 if lower + r1 does not fit in 32 bit, C=0 otherwise)</code></p>
<p>At this point r5 is A1 and r2 B0.  We compute A1*B0 giving a 64 bit result {lower, upper}. Note that ‘upper’ (equivalent to Upper32(A1*B0)) is uninteresting because if it is nonzero it would mean that our multiplication does not fit in 64 bit, so the update of r5 is nonrelevant for our computation. r5 is just used here because the instruction umlal requires it, and we will discard r5 at the end of the function anyway.</p>
<p>The lower 32 bits of the multiplication are stored in ‘lower’ (equivalent to Lower32(A1*B0)). But remember that we had to add Upper32(A0*B0). And it happens that as said above r1 is now Upper32(A0*B0). This way at the end of this instruction r1 will be the Lower32(A1*B0) <strong>plus</strong> Upper32(A0*B0).</p>
<p><code>umlal r1, r5, r3, r4</code></p>
<p>Likewise the previous, but this time we multiply A0*B1. Again r5 is uninteresting here. At the end of the instruction r1 will have the value of Lower32(A0*B1) <strong>plus</strong> the value it had: Lower32(A1*B0) <strong>plus</strong> Upper32(A0*B0). So after these three instructions r0 happens to be Lower32(A0*B0) and r1 happens to be Lower32(A0*B1) + Lower32(A1*B0) + Upper32(A0*B0), and this is what we wanted.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/?replytocom=131116#respond" onclick="return addComment.moveForm( &quot;div-comment-131116&quot;, &quot;131116&quot;, &quot;respond&quot;, &quot;823&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-even depth-1" id="comment-157425">
				<div id="div-comment-157425" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/c5128194ae0fa12dc2c34b146c3d11a5?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/c5128194ae0fa12dc2c34b146c3d11a5?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/c5128194ae0fa12dc2c34b146c3d11a5?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-c5128194ae0fa12dc2c34b146c3d11a5-0">			<cite class="fn"><a href="http://antoniovillena.es" rel="external nofollow" class="url">Antonio Villena</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/#comment-157425">
			June 15, 2014 at 3:09 pm</a>		</div>

		<p>Errata: addcs instruction doesn’t exist. Use instead adcs</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/?replytocom=157425#respond" onclick="return addComment.moveForm( &quot;div-comment-157425&quot;, &quot;157425&quot;, &quot;respond&quot;, &quot;823&quot; )" aria-label="Reply to Antonio Villena">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1" id="comment-157451">
				<div id="div-comment-157451" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/c5128194ae0fa12dc2c34b146c3d11a5?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/c5128194ae0fa12dc2c34b146c3d11a5?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/c5128194ae0fa12dc2c34b146c3d11a5?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-c5128194ae0fa12dc2c34b146c3d11a5-1">			<cite class="fn"><a href="http://antoniovillena.es" rel="external nofollow" class="url">Antonio Villena</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/#comment-157451">
			June 15, 2014 at 4:00 pm</a>		</div>

		<p>I’m sorry. It was my mistake add+cs exists, I have not read the comments</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/?replytocom=157451#respond" onclick="return addComment.moveForm( &quot;div-comment-157451&quot;, &quot;157451&quot;, &quot;respond&quot;, &quot;823&quot; )" aria-label="Reply to Antonio Villena">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment odd alt thread-even depth-1 parent" id="comment-802897">
				<div id="div-comment-802897" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/44ec0c26849a13b335039945e01fbdaf?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/44ec0c26849a13b335039945e01fbdaf?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/44ec0c26849a13b335039945e01fbdaf?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-44ec0c26849a13b335039945e01fbdaf-0">			<cite class="fn"><a href="https://www.machinalis.com/" rel="external nofollow" class="url">Daniel Moisset</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/#comment-802897">
			February 11, 2015 at 7:36 pm</a>		</div>

		<p>I loved this set of tutorials. Thanks for the great work!</p>
<p>Just a minor correction in the code transformation examples: the explanation on how to transforme a “while (E) S;” has an initial “S” inside the if that shouldn’t be there, as it is now the loop body will never run exactly once (it will run twice if the condition is initially true, without checking the condition between the first two body executions).</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/?replytocom=802897#respond" onclick="return addComment.moveForm( &quot;div-comment-802897&quot;, &quot;802897&quot;, &quot;respond&quot;, &quot;823&quot; )" aria-label="Reply to Daniel Moisset">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2" id="comment-804128">
				<div id="div-comment-804128" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-4">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/#comment-804128">
			February 13, 2015 at 8:18 am</a>		</div>

		<p>Hi Daniel,</p>
<p>you’re right. I’ve fixed that in the post. Thank you very much!</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/?replytocom=804128#respond" onclick="return addComment.moveForm( &quot;div-comment-804128&quot;, &quot;804128&quot;, &quot;respond&quot;, &quot;823&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1 parent" id="comment-954186">
				<div id="div-comment-954186" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/7bbd6f0b8cb450826e9db7f878328799?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/7bbd6f0b8cb450826e9db7f878328799?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/7bbd6f0b8cb450826e9db7f878328799?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-7bbd6f0b8cb450826e9db7f878328799-0">			<cite class="fn"><a href="http://na" rel="external nofollow" class="url">Kristian sander Hemdrup</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/#comment-954186">
			December 8, 2015 at 12:31 pm</a>		</div>

		<p>Very very interesting articles.  I learned a lot. </p>
<p>Is it possible to persuade you to write an article about I/O (eg.  asynch ports) and timers and an article about interrupts (eg timer interrupts) ? </p>
<p>Kristian sander</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/?replytocom=954186#respond" onclick="return addComment.moveForm( &quot;div-comment-954186&quot;, &quot;954186&quot;, &quot;respond&quot;, &quot;823&quot; )" aria-label="Reply to Kristian sander Hemdrup">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2" id="comment-954208">
				<div id="div-comment-954208" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-5">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/#comment-954208">
			December 8, 2015 at 6:32 pm</a>		</div>

		<p>Hi Kristian,</p>
<p>I’ll check that but given that the examples in the chapters are run on top of an operating system, probably these operations are privileged and only the OS can do them.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/?replytocom=954208#respond" onclick="return addComment.moveForm( &quot;div-comment-954208&quot;, &quot;954208&quot;, &quot;respond&quot;, &quot;823&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-even depth-1 parent" id="comment-993461">
				<div id="div-comment-993461" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/8a532d81ab8f696f0b2c2766dca42849?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/8a532d81ab8f696f0b2c2766dca42849?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/8a532d81ab8f696f0b2c2766dca42849?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-8a532d81ab8f696f0b2c2766dca42849-0">			<cite class="fn">cammi</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/#comment-993461">
			May 27, 2017 at 11:24 pm</a>		</div>

		<p>hello how can i add and sub two numbers of 128 bits?</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/?replytocom=993461#respond" onclick="return addComment.moveForm( &quot;div-comment-993461&quot;, &quot;993461&quot;, &quot;respond&quot;, &quot;823&quot; )" aria-label="Reply to cammi">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2" id="comment-993497">
				<div id="div-comment-993497" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-6">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/#comment-993497">
			May 28, 2017 at 12:08 pm</a>		</div>

		<p>Hi,</p>
<p>similar to 64-bit but with more instructions. The complexitly lies in that you will probably not want to use registers to represent the 128-bit numbers: that would cost you 4 registers per operand. So the easiest way is having an array of 4 words per operand and load the words. Then you operate them with add/adc and sub/sbc. Make sure you update the flags for the first three operations.</p>
<p>Below is an example of this.</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">global</span> add128
<span style="color: #adadad; font-style: italic;">add1</span><span style="color: #ff0000;">28</span><span style="color: #339933;">:</span>
    <span style="color: #339933;">/*</span> r0<span style="color: #339933;">:</span> address to an array of <span style="color: #ff0000;">4</span> words representing first operand <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r1<span style="color: #339933;">:</span> address to an array of <span style="color: #ff0000;">4</span> words representing second operand <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r2<span style="color: #339933;">:</span> address to an array of <span style="color: #ff0000;">4</span> words representing result <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> Assumes little endian <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">/*</span> Keep registers that we are going to use plus one more to keep the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #ff0000;">8</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">Bits</span> <span style="color: #ff0000;">0</span> to <span style="color: #ff0000;">31</span> of the <span style="color: #ff0000;">128</span><span style="color: #339933;">-</span>bit operands <span style="color: #339933;">*/</span>
    ldr r4<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>          <span style="color: #339933;">/*</span> r4 ← <span style="color: #339933;">*</span>r0 <span style="color: #339933;">*/</span>
    ldr r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #009900; font-weight: bold;">]</span>          <span style="color: #339933;">/*</span> r5 ← <span style="color: #339933;">*</span>r1 <span style="color: #339933;">*/</span>
    adds r6<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> r5       <span style="color: #339933;">/*</span> r6 ← r4 <span style="color: #339933;">+</span> r5<span style="color: #339933;">,</span> updates cpsr <span style="color: #009900; font-weight: bold;">(</span>C0 ← carry bit of cpsr<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> r6<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r2<span style="color: #009900; font-weight: bold;">]</span>          <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span>r2 ← r6 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">Bits</span> <span style="color: #ff0000;">32</span> to <span style="color: #ff0000;">63</span> of the <span style="color: #ff0000;">128</span><span style="color: #339933;">-</span>bit operands <span style="color: #339933;">*/</span>
    ldr r4<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>      <span style="color: #339933;">/*</span> r4 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r0 <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    ldr r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>      <span style="color: #339933;">/*</span> r5 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r1 <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    adcs r6<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> r5       <span style="color: #339933;">/*</span> r6 ← r4 <span style="color: #339933;">+</span> r5 <span style="color: #339933;">+</span> C0<span style="color: #339933;">,</span> updates cpsr <span style="color: #009900; font-weight: bold;">(</span>C1 ← carry bit of cpsr<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> r6<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>      <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r2 <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span> ← r6 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">Bits</span> <span style="color: #ff0000;">64</span> to <span style="color: #ff0000;">95</span> of the <span style="color: #ff0000;">128</span><span style="color: #339933;">-</span>bit operands <span style="color: #339933;">*/</span>
    ldr r4<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span>      <span style="color: #339933;">/*</span> r4 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r0 <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    ldr r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span>      <span style="color: #339933;">/*</span> r5 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r1 <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    adcs r6<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> r5       <span style="color: #339933;">/*</span> r6 ← r4 <span style="color: #339933;">+</span> r5 <span style="color: #339933;">+</span> C1<span style="color: #339933;">,</span> updates cpsr <span style="color: #009900; font-weight: bold;">(</span>C2 ← carry bit of cpsr<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> r6<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span>      <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r2 <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span> ← r6 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">Bits</span> <span style="color: #ff0000;">96</span> to <span style="color: #ff0000;">127</span> of the <span style="color: #ff0000;">128</span><span style="color: #339933;">-</span>bit operands <span style="color: #339933;">*/</span>
    ldr r4<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">12</span><span style="color: #009900; font-weight: bold;">]</span>     <span style="color: #339933;">/*</span> r4 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r0 <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    ldr r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">12</span><span style="color: #009900; font-weight: bold;">]</span>     <span style="color: #339933;">/*</span> r5 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r1 <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">adc</span> r6<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> r5        <span style="color: #339933;">/*</span> r6 ← r4 <span style="color: #339933;">+</span> r5 <span style="color: #339933;">+</span> C2 <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> r6<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">12</span><span style="color: #009900; font-weight: bold;">]</span>     <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r2 <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span> ← r6 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>  <span style="color: #339933;">/*</span> Restore kept registers <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">global</span> sub128
sub128<span style="color: #339933;">:</span>
    <span style="color: #339933;">/*</span> r0<span style="color: #339933;">:</span> address to an array of <span style="color: #ff0000;">4</span> words representing first operand <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r1<span style="color: #339933;">:</span> address to an array of <span style="color: #ff0000;">4</span> words representing second operand <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r2<span style="color: #339933;">:</span> address to an array of <span style="color: #ff0000;">4</span> words representing result <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> Assumes little endian <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
&nbsp;
    <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">Bits</span> <span style="color: #ff0000;">0</span> to <span style="color: #ff0000;">31</span> of the <span style="color: #ff0000;">128</span><span style="color: #339933;">-</span>bit operands <span style="color: #339933;">*/</span>
    ldr r4<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>         <span style="color: #339933;">/*</span> r4 ← <span style="color: #339933;">*</span>r0 <span style="color: #339933;">*/</span>
    ldr r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #009900; font-weight: bold;">]</span>         <span style="color: #339933;">/*</span> r5 ← <span style="color: #339933;">*</span>r1 <span style="color: #339933;">*/</span>
    subs r6<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> r5      <span style="color: #339933;">/*</span> r6 ← r4 <span style="color: #339933;">-</span> r5<span style="color: #339933;">,</span> updates cpsr <span style="color: #009900; font-weight: bold;">(</span>C0 ← carry bit of cpsr<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
                         <span style="color: #339933;">/*</span> Actually what this does is
                              r6 ← r4 <span style="color: #339933;">+</span> ~r5 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span>
                            where ~r5 is the bitwise <span style="color: #00007f; font-weight: bold;">not</span> of r5
                            the <span style="color: #7f007f;">"+ 1"</span> is because this is a <span style="color: #00007f; font-weight: bold;">sub</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> r6<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r2<span style="color: #009900; font-weight: bold;">]</span>         <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span>r2 ← r6 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">Bits</span> <span style="color: #ff0000;">32</span> to <span style="color: #ff0000;">63</span> of the <span style="color: #ff0000;">128</span><span style="color: #339933;">-</span>bit operands <span style="color: #339933;">*/</span>
    ldr r4<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>     <span style="color: #339933;">/*</span> r4 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r0 <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    ldr r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>     <span style="color: #339933;">/*</span> r5 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r1 <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    sbcs r6<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> r5      <span style="color: #339933;">/*</span> r6 ← r4 <span style="color: #339933;">-</span> r5 <span style="color: #339933;">+</span> borrow from C0<span style="color: #339933;">,</span> updates cpsr <span style="color: #009900; font-weight: bold;">(</span>C1 ← carry bit of cpsr<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
                         <span style="color: #339933;">/*</span> Similar to above<span style="color: #339933;">,</span> what this does is
                              r6 ← r4 <span style="color: #339933;">+</span> ~r5 <span style="color: #339933;">+</span> C0
                         <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> r6<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>     <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r2 <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span> ← r6 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">Bits</span> <span style="color: #ff0000;">64</span> to <span style="color: #ff0000;">95</span> of the <span style="color: #ff0000;">128</span><span style="color: #339933;">-</span>bit operands <span style="color: #339933;">*/</span>
    ldr r4<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span>     <span style="color: #339933;">/*</span> r4 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r0 <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    ldr r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span>     <span style="color: #339933;">/*</span> r5 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r1 <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    sbcs r6<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> r5      <span style="color: #339933;">/*</span> r6 ← r4 <span style="color: #339933;">-</span> r5 <span style="color: #339933;">+</span> borrow from C1<span style="color: #339933;">,</span> updates cpsr <span style="color: #009900; font-weight: bold;">(</span>C2 ← carry bit of cpsr<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
                         <span style="color: #339933;">/*</span> Similar to above<span style="color: #339933;">,</span> what this does is
                              r6 ← r4 <span style="color: #339933;">+</span> ~r5 <span style="color: #339933;">+</span> C1
                         <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> r6<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span>     <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r2 <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span> ← r6 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">Bits</span> <span style="color: #ff0000;">96</span> to <span style="color: #ff0000;">127</span> of the <span style="color: #ff0000;">128</span><span style="color: #339933;">-</span>bit operands <span style="color: #339933;">*/</span>
    ldr r4<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">12</span><span style="color: #009900; font-weight: bold;">]</span>    <span style="color: #339933;">/*</span> r4 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r0 <span style="color: #339933;">+</span> <span style="color: #ff0000;">12</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    ldr r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">12</span><span style="color: #009900; font-weight: bold;">]</span>    <span style="color: #339933;">/*</span> r5 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r1 <span style="color: #339933;">+</span> <span style="color: #ff0000;">12</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    sbc r6<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> r5       <span style="color: #339933;">/*</span> r6 ← r4 <span style="color: #339933;">-</span> r5 <span style="color: #339933;">+</span> borrow from C2 <span style="color: #339933;">*/</span>
                         <span style="color: #339933;">/*</span> Similar to above<span style="color: #339933;">,</span> what this does is
                              r6 ← r4 <span style="color: #339933;">+</span> ~r5 <span style="color: #339933;">+</span> C2
                         <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> r6<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">12</span><span style="color: #009900; font-weight: bold;">]</span>    <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r2 <span style="color: #339933;">+</span> <span style="color: #ff0000;">12</span><span style="color: #009900; font-weight: bold;">)</span> ← r6 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">.text

.global add128
add128:
    /* r0: address to an array of 4 words representing first operand */
    /* r1: address to an array of 4 words representing second operand */
    /* r2: address to an array of 4 words representing result */
    /* Assumes little endian */
    push {r4, r5, r6, lr} /* Keep registers that we are going to use plus one more to keep the stack 8-byte aligned */

    /* Bits 0 to 31 of the 128-bit operands */
    ldr r4, [r0]          /* r4 ← *r0 */
    ldr r5, [r1]          /* r5 ← *r1 */
    adds r6, r4, r5       /* r6 ← r4 + r5, updates cpsr (C0 ← carry bit of cpsr) */
    str r6, [r2]          /* *r2 ← r6 */

    /* Bits 32 to 63 of the 128-bit operands */
    ldr r4, [r0, #4]      /* r4 ← *(r0 + 4) */
    ldr r5, [r1, #4]      /* r5 ← *(r1 + 4) */
    adcs r6, r4, r5       /* r6 ← r4 + r5 + C0, updates cpsr (C1 ← carry bit of cpsr) */
    str r6, [r2, #4]      /* *(r2 + 4) ← r6 */

    /* Bits 64 to 95 of the 128-bit operands */
    ldr r4, [r0, #8]      /* r4 ← *(r0 + 8) */
    ldr r5, [r1, #8]      /* r5 ← *(r1 + 8) */
    adcs r6, r4, r5       /* r6 ← r4 + r5 + C1, updates cpsr (C2 ← carry bit of cpsr) */
    str r6, [r2, #8]      /* *(r2 + 8) ← r6 */

    /* Bits 96 to 127 of the 128-bit operands */
    ldr r4, [r0, #12]     /* r4 ← *(r0 + 8) */
    ldr r5, [r1, #12]     /* r5 ← *(r1 + 8) */
    adc r6, r4, r5        /* r6 ← r4 + r5 + C2 */
    str r6, [r2, #12]     /* *(r2 + 8) ← r6 */

    pop {r4, r5, r6, lr}  /* Restore kept registers */
    bx lr

.global sub128
sub128:
    /* r0: address to an array of 4 words representing first operand */
    /* r1: address to an array of 4 words representing second operand */
    /* r2: address to an array of 4 words representing result */
    /* Assumes little endian */
    push {r4, r5, r6, lr}

    /* Bits 0 to 31 of the 128-bit operands */
    ldr r4, [r0]         /* r4 ← *r0 */
    ldr r5, [r1]         /* r5 ← *r1 */
    subs r6, r4, r5      /* r6 ← r4 - r5, updates cpsr (C0 ← carry bit of cpsr) */
                         /* Actually what this does is
                              r6 ← r4 + ~r5 + 1
                            where ~r5 is the bitwise not of r5
                            the "+ 1" is because this is a sub */
    str r6, [r2]         /* *r2 ← r6 */

    /* Bits 32 to 63 of the 128-bit operands */
    ldr r4, [r0, #4]     /* r4 ← *(r0 + 4) */
    ldr r5, [r1, #4]     /* r5 ← *(r1 + 4) */
    sbcs r6, r4, r5      /* r6 ← r4 - r5 + borrow from C0, updates cpsr (C1 ← carry bit of cpsr) */
                         /* Similar to above, what this does is
                              r6 ← r4 + ~r5 + C0
                         */
    str r6, [r2, #4]     /* *(r2 + 4) ← r6 */

    /* Bits 64 to 95 of the 128-bit operands */
    ldr r4, [r0, #8]     /* r4 ← *(r0 + 8) */
    ldr r5, [r1, #8]     /* r5 ← *(r1 + 8) */
    sbcs r6, r4, r5      /* r6 ← r4 - r5 + borrow from C1, updates cpsr (C2 ← carry bit of cpsr) */
                         /* Similar to above, what this does is
                              r6 ← r4 + ~r5 + C1
                         */
    str r6, [r2, #8]     /* *(r2 + 8) ← r6 */

    /* Bits 96 to 127 of the 128-bit operands */
    ldr r4, [r0, #12]    /* r4 ← *(r0 + 12) */
    ldr r5, [r1, #12]    /* r5 ← *(r1 + 12) */
    sbc r6, r4, r5       /* r6 ← r4 - r5 + borrow from C2 */
                         /* Similar to above, what this does is
                              r6 ← r4 + ~r5 + C2
                         */
    str r6, [r2, #12]    /* *(r2 + 12) ← r6 */

    pop {r4, r5, r6, lr}
    bx lr</p></div>


		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/?replytocom=993497#respond" onclick="return addComment.moveForm( &quot;div-comment-993497&quot;, &quot;993497&quot;, &quot;respond&quot;, &quot;823&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		</ul>
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2013/03/28/arm-assembler-raspberry-pi-chapter-12/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="823" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="494eecedc3"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496665204117"></form>
			</div><!-- #respond -->
	</div>	</div>