# GDB


<div class="main">
			<h1>ARM assembler in Raspberry Pi – Chapter 4 </h1>
		<p class="post-meta"><span class="date">January 12, 2013</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/#comments">14</a></span> </p>
		<p>
As we advance learning the foundations of ARM assembler, our examples will become longer. Since it is easy to make mistakes, I think it is worth learning how to use GNU Debugger <code>gdb</code> to debug assembler. If you develop C/C++ in Linux and never used <code>gdb</code>, shame on you. If you know <code>gdb</code> this small chapter will explain you how to debug assembler directly.
</p>
<p><span id="more-416"></span></p>
<h2>gdb</h2>
<p>
We will use the example <code>store01</code> from chapter 3. Start <code>gdb</code> specifying the program you are going to debug.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="bash" style="font-family:monospace;">$ <span style="color: #c20cb9; font-weight: bold;">gdb</span> <span style="color: #660033;">--args</span> .<span style="color: #000000; font-weight: bold;">/</span>store01
GNU <span style="color: #c20cb9; font-weight: bold;">gdb</span> <span style="color: #7a0874; font-weight: bold;">(</span>GDB<span style="color: #7a0874; font-weight: bold;">)</span> 7.4.1-debian
Copyright <span style="color: #7a0874; font-weight: bold;">(</span>C<span style="color: #7a0874; font-weight: bold;">)</span> <span style="color: #000000;">2012</span> Free Software Foundation, Inc.
License GPLv3+: GNU GPL version <span style="color: #000000;">3</span> or later 
This is <span style="color: #c20cb9; font-weight: bold;">free</span> software: you are <span style="color: #c20cb9; font-weight: bold;">free</span> to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type <span style="color: #ff0000;">"show copying"</span>
and <span style="color: #ff0000;">"show warranty"</span> <span style="color: #000000; font-weight: bold;">for</span> details.
This GDB was configured <span style="color: #c20cb9; font-weight: bold;">as</span> <span style="color: #ff0000;">"arm-linux-gnueabihf"</span>.
For bug reporting instructions, please see:
...
Reading symbols from <span style="color: #000000; font-weight: bold;">/</span>home<span style="color: #000000; font-weight: bold;">/</span>roger<span style="color: #000000; font-weight: bold;">/</span>asm<span style="color: #000000; font-weight: bold;">/</span>chapter03<span style="color: #000000; font-weight: bold;">/</span>store01...<span style="color: #7a0874; font-weight: bold;">(</span>no debugging symbols found<span style="color: #7a0874; font-weight: bold;">)</span>...done.
<span style="color: #7a0874; font-weight: bold;">(</span><span style="color: #c20cb9; font-weight: bold;">gdb</span><span style="color: #7a0874; font-weight: bold;">)</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ gdb --args ./store01
GNU gdb (GDB) 7.4.1-debian
Copyright (C) 2012 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later 
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "arm-linux-gnueabihf".
For bug reporting instructions, please see:
...
Reading symbols from /home/roger/asm/chapter03/store01...(no debugging symbols found)...done.
(gdb)</p></div>

<p>
Ok, we are in the <i>interactive</i> mode of <code>gdb</code>. In this mode you communicate with <code>gdb</code> using commands. There is a builtin help command called <code>help</code>. Or you can check the <a href="http://sourceware.org/gdb/current/onlinedocs/gdb/">GNU Debugger Documentation</a>. A first command to learn is
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="gdb" style="font-family:monospace;"><span style="font-weight:bold;">(</span>gdb<span style="font-weight:bold;">)</span> quit</pre></td></tr></tbody></table><p class="theCode" style="display:none;">(gdb) quit</p></div>

<p>
Ok, now start <code>gdb</code> again. The program is not running yet. In fact <code>gdb</code> will not be able to tell you many things about it since it does not have debugging info. But this is fine, we are debugging assembler, so we do not need much debugging info. So as a first step let’s start the program.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="gdb" style="font-family:monospace;"><span style="font-weight:bold;">(</span>gdb<span style="font-weight:bold;">)</span> start
Temporary breakpoint <span style="">1</span> at <span style="color: #555;">0x8390</span>
Starting program: /home/roger/asm/chapter03/store01 
&nbsp;
Temporary breakpoint <span style="">1</span>, <span style="color: #555;">0x00008390</span> in <span style="color: #442886; font-weight:bold;">main</span> <span style="font-weight:bold;">(</span><span style="font-weight:bold;">)</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">(gdb) start
Temporary breakpoint 1 at 0x8390
Starting program: /home/roger/asm/chapter03/store01 

Temporary breakpoint 1, 0x00008390 in main ()</p></div>

<p>
Ok, <code>gdb</code> ran our program up to <code>main</code>. This is great, we have skipped all the initialization steps of the C library and we are about to run the first instruction of our <code>main</code> function. Let’s see whats there.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="gdb" style="font-family:monospace;"><span style="font-weight:bold;">(</span>gdb<span style="font-weight:bold;">)</span> disassemble
Dump of assembler code for function main:
=&gt; <span style="color: #555;">0x00008390</span> :	ldr	r1, <span style="font-weight:bold;">[</span>pc, #<span style="">40</span><span style="font-weight:bold;">]</span>	; <span style="color: #555;">0x83c0</span> 
   <span style="color: #555;">0x00008394</span> :	mov	r3, #<span style="">3</span>
   <span style="color: #555;">0x00008398</span> :	str	r3, <span style="font-weight:bold;">[</span>r1<span style="font-weight:bold;">]</span>
   <span style="color: #555;">0x0000839c</span> :	ldr	r2, <span style="font-weight:bold;">[</span>pc, #<span style="">32</span><span style="font-weight:bold;">]</span>	; <span style="color: #555;">0x83c4</span> 
   <span style="color: #555;">0x000083a0</span> :	mov	r3, #<span style="">4</span>
   <span style="color: #555;">0x000083a4</span> :	str	r3, <span style="font-weight:bold;">[</span>r2<span style="font-weight:bold;">]</span>
   <span style="color: #555;">0x000083a8</span> :	ldr	r1, <span style="font-weight:bold;">[</span>pc, #<span style="">16</span><span style="font-weight:bold;">]</span>	; <span style="color: #555;">0x83c0</span> 
   <span style="color: #555;">0x000083ac</span> :	ldr	r1, <span style="font-weight:bold;">[</span>r1<span style="font-weight:bold;">]</span>
   <span style="color: #555;">0x000083b0</span> :	ldr	r2, <span style="font-weight:bold;">[</span>pc, #<span style="">12</span><span style="font-weight:bold;">]</span>	; <span style="color: #555;">0x83c4</span> 
   <span style="color: #555;">0x000083b4</span> :	ldr	r2, <span style="font-weight:bold;">[</span>r2<span style="font-weight:bold;">]</span>
   <span style="color: #555;">0x000083b8</span> :	add	r0, r1, r2
   <span style="color: #555;">0x000083bc</span> :	bx	lr
End of assembler dump.</pre></td></tr></tbody></table><p class="theCode" style="display:none;">(gdb) disassemble
Dump of assembler code for function main:
=&gt; 0x00008390 :	ldr	r1, [pc, #40]	; 0x83c0 
   0x00008394 :	mov	r3, #3
   0x00008398 :	str	r3, [r1]
   0x0000839c :	ldr	r2, [pc, #32]	; 0x83c4 
   0x000083a0 :	mov	r3, #4
   0x000083a4 :	str	r3, [r2]
   0x000083a8 :	ldr	r1, [pc, #16]	; 0x83c0 
   0x000083ac :	ldr	r1, [r1]
   0x000083b0 :	ldr	r2, [pc, #12]	; 0x83c4 
   0x000083b4 :	ldr	r2, [r2]
   0x000083b8 :	add	r0, r1, r2
   0x000083bc :	bx	lr
End of assembler dump.</p></div>

<p>
Uh-oh! The instructions referring the label <code>addr_of_myvarX</code> are different. Ok. Ignore that for now, we will learn in a future chapter what has happened. There is an arrow <code>=&gt;</code> pointing the instruction we are going to run (it has not been run yet). Before running it, let’s inspect some registers.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="gdb" style="font-family:monospace;"><span style="font-weight:bold;">(</span>gdb<span style="font-weight:bold;">)</span> info registers r0 r1 r2 r3
r0             0x1	<span style="">1</span>
r1             <span style="color: #555;">0xbefff744</span>	<span style="">3204446020</span>
r2             <span style="color: #555;">0xbefff74c</span>	<span style="">3204446028</span>
r3             <span style="color: #555;">0x8390</span>	<span style="">33680</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">(gdb) info registers r0 r1 r2 r3
r0             0x1	1
r1             0xbefff744	3204446020
r2             0xbefff74c	3204446028
r3             0x8390	33680</p></div>

<p>
We can modify registers using <code>p</code> which means <code>print</code> but also evaluates side effects. For instance,
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="gdb" style="font-family:monospace;"><span style="font-weight:bold;">(</span>gdb<span style="font-weight:bold;">)</span> p $r0 = <span style="">2</span>
$1 = <span style="">2</span>
<span style="font-weight:bold;">(</span>gdb<span style="font-weight:bold;">)</span> info registers r0 r1 r2 r3
r0             0x2	<span style="">2</span>
r1             <span style="color: #555;">0xbefff744</span>	<span style="">3204446020</span>
r2             <span style="color: #555;">0xbefff74c</span>	<span style="">3204446028</span>
r3             <span style="color: #555;">0x8390</span>	<span style="">33680</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">(gdb) p $r0 = 2
$1 = 2
(gdb) info registers r0 r1 r2 r3
r0             0x2	2
r1             0xbefff744	3204446020
r2             0xbefff74c	3204446028
r3             0x8390	33680</p></div>

<p>
<code>gdb</code> has printed <code>$1</code>, this is the identifier of the result and we can use it when needed, so we can skip some typing. Not very useful now but it will be when we print a complicated expression.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="gdb" style="font-family:monospace;"><span style="font-weight:bold;">(</span>gdb<span style="font-weight:bold;">)</span> p $1
$2 = <span style="">2</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">(gdb) p $1
$2 = 2</p></div>

<p>
Now we could use <code>$2</code>, and so on. Ok, time to run the first instruction.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="gdb" style="font-family:monospace;"><span style="font-weight:bold;">(</span>gdb<span style="font-weight:bold;">)</span> stepi
<span style="color: #555;">0x00008394</span> in <span style="color: #442886; font-weight:bold;">main</span> <span style="font-weight:bold;">(</span><span style="font-weight:bold;">)</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">(gdb) stepi
0x00008394 in main ()</p></div>

<p>
Well, not much happened, let’s use <code>disassemble</code>, again.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="gdb" style="font-family:monospace;"><span style="font-weight:bold;">(</span>gdb<span style="font-weight:bold;">)</span> disassemble
Dump of assembler code for function main:
   <span style="color: #555;">0x00008390</span> :	ldr	r1, <span style="font-weight:bold;">[</span>pc, #<span style="">40</span><span style="font-weight:bold;">]</span>	; <span style="color: #555;">0x83c0</span> 
=&gt; <span style="color: #555;">0x00008394</span> :	mov	r3, #<span style="">3</span>
   <span style="color: #555;">0x00008398</span> :	str	r3, <span style="font-weight:bold;">[</span>r1<span style="font-weight:bold;">]</span>
   <span style="color: #555;">0x0000839c</span> :	ldr	r2, <span style="font-weight:bold;">[</span>pc, #<span style="">32</span><span style="font-weight:bold;">]</span>	; <span style="color: #555;">0x83c4</span> 
   <span style="color: #555;">0x000083a0</span> :	mov	r3, #<span style="">4</span>
   <span style="color: #555;">0x000083a4</span> :	str	r3, <span style="font-weight:bold;">[</span>r2<span style="font-weight:bold;">]</span>
   <span style="color: #555;">0x000083a8</span> :	ldr	r1, <span style="font-weight:bold;">[</span>pc, #<span style="">16</span><span style="font-weight:bold;">]</span>	; <span style="color: #555;">0x83c0</span> 
   <span style="color: #555;">0x000083ac</span> :	ldr	r1, <span style="font-weight:bold;">[</span>r1<span style="font-weight:bold;">]</span>
   <span style="color: #555;">0x000083b0</span> :	ldr	r2, <span style="font-weight:bold;">[</span>pc, #<span style="">12</span><span style="font-weight:bold;">]</span>	; <span style="color: #555;">0x83c4</span> 
   <span style="color: #555;">0x000083b4</span> :	ldr	r2, <span style="font-weight:bold;">[</span>r2<span style="font-weight:bold;">]</span>
   <span style="color: #555;">0x000083b8</span> :	add	r0, r1, r2
   <span style="color: #555;">0x000083bc</span> :	bx	lr
End of assembler dump.</pre></td></tr></tbody></table><p class="theCode" style="display:none;">(gdb) disassemble
Dump of assembler code for function main:
   0x00008390 :	ldr	r1, [pc, #40]	; 0x83c0 
=&gt; 0x00008394 :	mov	r3, #3
   0x00008398 :	str	r3, [r1]
   0x0000839c :	ldr	r2, [pc, #32]	; 0x83c4 
   0x000083a0 :	mov	r3, #4
   0x000083a4 :	str	r3, [r2]
   0x000083a8 :	ldr	r1, [pc, #16]	; 0x83c0 
   0x000083ac :	ldr	r1, [r1]
   0x000083b0 :	ldr	r2, [pc, #12]	; 0x83c4 
   0x000083b4 :	ldr	r2, [r2]
   0x000083b8 :	add	r0, r1, r2
   0x000083bc :	bx	lr
End of assembler dump.</p></div>

<p>
Ok, let’s see what happened in <code>r1</code>.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="gdb" style="font-family:monospace;"><span style="font-weight:bold;">(</span>gdb<span style="font-weight:bold;">)</span> info register r1
r1             <span style="color: #555;">0x10564</span>	<span style="">66916</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">(gdb) info register r1
r1             0x10564	66916</p></div>

<p>
Great, it has changed. In fact this is the address of <code>myvar1</code>. Let’s check this using its symbolic name and C syntax.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="gdb" style="font-family:monospace;"><span style="font-weight:bold;">(</span>gdb<span style="font-weight:bold;">)</span> p &amp;myvar1
$3 <span style="color: #442886; font-weight:bold;">=</span> <span style="font-weight:bold;">(</span> *<span style="font-weight:bold;">)</span> <span style="color: #555;">0x10564</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">(gdb) p &amp;myvar1
$3 = ( *) 0x10564</p></div>

<p>
Great! Can we see what is in this variable?
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="gdb" style="font-family:monospace;"><span style="font-weight:bold;">(</span>gdb<span style="font-weight:bold;">)</span> p myvar1
$4 = <span style="">0</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">(gdb) p myvar1
$4 = 0</p></div>

<p>
Perfect. This was as expected since in this example we set zero as the initial value of <code>myvar1</code> and <code>myvar2</code>. Ok, next step.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="gdb" style="font-family:monospace;"><span style="font-weight:bold;">(</span>gdb<span style="font-weight:bold;">)</span> stepi
<span style="color: #555;">0x00008398</span> in <span style="color: #442886; font-weight:bold;">main</span> <span style="font-weight:bold;">(</span><span style="font-weight:bold;">)</span>
<span style="font-weight:bold;">(</span>gdb<span style="font-weight:bold;">)</span> disas
Dump of assembler code for function main:
   <span style="color: #555;">0x00008390</span> :	ldr	r1, <span style="font-weight:bold;">[</span>pc, #<span style="">40</span><span style="font-weight:bold;">]</span>	; <span style="color: #555;">0x83c0</span> 
   <span style="color: #555;">0x00008394</span> :	mov	r3, #<span style="">3</span>
=&gt; <span style="color: #555;">0x00008398</span> :	str	r3, <span style="font-weight:bold;">[</span>r1<span style="font-weight:bold;">]</span>
   <span style="color: #555;">0x0000839c</span> :	ldr	r2, <span style="font-weight:bold;">[</span>pc, #<span style="">32</span><span style="font-weight:bold;">]</span>	; <span style="color: #555;">0x83c4</span> 
   <span style="color: #555;">0x000083a0</span> :	mov	r3, #<span style="">4</span>
   <span style="color: #555;">0x000083a4</span> :	str	r3, <span style="font-weight:bold;">[</span>r2<span style="font-weight:bold;">]</span>
   <span style="color: #555;">0x000083a8</span> :	ldr	r1, <span style="font-weight:bold;">[</span>pc, #<span style="">16</span><span style="font-weight:bold;">]</span>	; <span style="color: #555;">0x83c0</span> 
   <span style="color: #555;">0x000083ac</span> :	ldr	r1, <span style="font-weight:bold;">[</span>r1<span style="font-weight:bold;">]</span>
   <span style="color: #555;">0x000083b0</span> :	ldr	r2, <span style="font-weight:bold;">[</span>pc, #<span style="">12</span><span style="font-weight:bold;">]</span>	; <span style="color: #555;">0x83c4</span> 
   <span style="color: #555;">0x000083b4</span> :	ldr	r2, <span style="font-weight:bold;">[</span>r2<span style="font-weight:bold;">]</span>
   <span style="color: #555;">0x000083b8</span> :	add	r0, r1, r2
   <span style="color: #555;">0x000083bc</span> :	bx	lr
End of assembler dump.</pre></td></tr></tbody></table><p class="theCode" style="display:none;">(gdb) stepi
0x00008398 in main ()
(gdb) disas
Dump of assembler code for function main:
   0x00008390 :	ldr	r1, [pc, #40]	; 0x83c0 
   0x00008394 :	mov	r3, #3
=&gt; 0x00008398 :	str	r3, [r1]
   0x0000839c :	ldr	r2, [pc, #32]	; 0x83c4 
   0x000083a0 :	mov	r3, #4
   0x000083a4 :	str	r3, [r2]
   0x000083a8 :	ldr	r1, [pc, #16]	; 0x83c0 
   0x000083ac :	ldr	r1, [r1]
   0x000083b0 :	ldr	r2, [pc, #12]	; 0x83c4 
   0x000083b4 :	ldr	r2, [r2]
   0x000083b8 :	add	r0, r1, r2
   0x000083bc :	bx	lr
End of assembler dump.</p></div>

<p>
You can use <code>disas</code> (but not <code>disa</code>!) as a short for <code>disassemble</code>. Let’s check what happened to <code>r3</code>
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="gdb" style="font-family:monospace;"><span style="font-weight:bold;">(</span>gdb<span style="font-weight:bold;">)</span> info registers r3
r3             0x3	<span style="">3</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">(gdb) info registers r3
r3             0x3	3</p></div>

<p>
So far so good. Another more step.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="gdb" style="font-family:monospace;"><span style="font-weight:bold;">(</span>gdb<span style="font-weight:bold;">)</span> stepi
<span style="color: #555;">0x0000839c</span> in <span style="color: #442886; font-weight:bold;">main</span> <span style="font-weight:bold;">(</span><span style="font-weight:bold;">)</span>
<span style="font-weight:bold;">(</span>gdb<span style="font-weight:bold;">)</span> disas
Dump of assembler code for function main:
   <span style="color: #555;">0x00008390</span> :	ldr	r1, <span style="font-weight:bold;">[</span>pc, #<span style="">40</span><span style="font-weight:bold;">]</span>	; <span style="color: #555;">0x83c0</span> 
   <span style="color: #555;">0x00008394</span> :	mov	r3, #<span style="">3</span>
   <span style="color: #555;">0x00008398</span> :	str	r3, <span style="font-weight:bold;">[</span>r1<span style="font-weight:bold;">]</span>
=&gt; <span style="color: #555;">0x0000839c</span> :	ldr	r2, <span style="font-weight:bold;">[</span>pc, #<span style="">32</span><span style="font-weight:bold;">]</span>	; <span style="color: #555;">0x83c4</span> 
   <span style="color: #555;">0x000083a0</span> :	mov	r3, #<span style="">4</span>
   <span style="color: #555;">0x000083a4</span> :	str	r3, <span style="font-weight:bold;">[</span>r2<span style="font-weight:bold;">]</span>
   <span style="color: #555;">0x000083a8</span> :	ldr	r1, <span style="font-weight:bold;">[</span>pc, #<span style="">16</span><span style="font-weight:bold;">]</span>	; <span style="color: #555;">0x83c0</span> 
   <span style="color: #555;">0x000083ac</span> :	ldr	r1, <span style="font-weight:bold;">[</span>r1<span style="font-weight:bold;">]</span>
   <span style="color: #555;">0x000083b0</span> :	ldr	r2, <span style="font-weight:bold;">[</span>pc, #<span style="">12</span><span style="font-weight:bold;">]</span>	; <span style="color: #555;">0x83c4</span> 
   <span style="color: #555;">0x000083b4</span> :	ldr	r2, <span style="font-weight:bold;">[</span>r2<span style="font-weight:bold;">]</span>
   <span style="color: #555;">0x000083b8</span> :	add	r0, r1, r2
   <span style="color: #555;">0x000083bc</span> :	bx	lr
End of assembler dump.</pre></td></tr></tbody></table><p class="theCode" style="display:none;">(gdb) stepi
0x0000839c in main ()
(gdb) disas
Dump of assembler code for function main:
   0x00008390 :	ldr	r1, [pc, #40]	; 0x83c0 
   0x00008394 :	mov	r3, #3
   0x00008398 :	str	r3, [r1]
=&gt; 0x0000839c :	ldr	r2, [pc, #32]	; 0x83c4 
   0x000083a0 :	mov	r3, #4
   0x000083a4 :	str	r3, [r2]
   0x000083a8 :	ldr	r1, [pc, #16]	; 0x83c0 
   0x000083ac :	ldr	r1, [r1]
   0x000083b0 :	ldr	r2, [pc, #12]	; 0x83c4 
   0x000083b4 :	ldr	r2, [r2]
   0x000083b8 :	add	r0, r1, r2
   0x000083bc :	bx	lr
End of assembler dump.</p></div>

<p>
Ok, lets see what happened, we stored <code>r3</code>, which contained a 3 into <code>myvar1</code>, right? Let’s check this.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="gdb" style="font-family:monospace;"><span style="font-weight:bold;">(</span>gdb<span style="font-weight:bold;">)</span> p myvar1
$5 = <span style="">3</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">(gdb) p myvar1
$5 = 3</p></div>

<p>
Amazing, isn’t it? Ok. Now run until the end.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="gdb" style="font-family:monospace;"><span style="font-weight:bold;">(</span>gdb<span style="font-weight:bold;">)</span> continue
Continuing.
<span style="color: #000066; font-weight:bold;"><span style="font-weight:bold;">[</span>Inferior 1 <span style="font-weight:bold;">(</span>process 3080<span style="font-weight:bold;">)</span> exited with code 07<span style="font-weight:bold;">]</span></span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">(gdb) continue
Continuing.
[Inferior 1 (process 3080) exited with code 07]</p></div>

<p>
That’s all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/&amp;text=ARM+assembler+in+Raspberry+Pi+%E2%80%93+Chapter+4+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>				<p class="tags"><span><a href="http://thinkingeek.com/tag/arm/" rel="tag">arm</a>, <a href="http://thinkingeek.com/tag/assembler/" rel="tag">assembler</a>, <a href="http://thinkingeek.com/tag/debugger/" rel="tag">debugger</a>, <a href="http://thinkingeek.com/tag/gdb/" rel="tag">gdb</a>, <a href="http://thinkingeek.com/tag/pi/" rel="tag">pi</a>, <a href="http://thinkingeek.com/tag/raspberry/" rel="tag">raspberry</a></span></p>		<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2013/01/11/arm-assembler-raspberry-pi-chapter-3/" rel="prev">ARM assembler in Raspberry Pi – Chapter 3</a></span>
			<span class="next"><a href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/" rel="next">ARM assembler in Raspberry Pi – Chapter 5</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
			<h3>14 thoughts on “<span>ARM assembler in Raspberry Pi – Chapter 4</span>”</h3>
		<ul class="commentlist">
				<li class="comment even thread-even depth-1" id="comment-1421">
				<div id="div-comment-1421" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a16ae1e69be669d6f2c5fa0972e39c8a?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a16ae1e69be669d6f2c5fa0972e39c8a?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a16ae1e69be669d6f2c5fa0972e39c8a?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a16ae1e69be669d6f2c5fa0972e39c8a-0">			<cite class="fn"><a href="http://digilander.libero.it/zantaz/" rel="external nofollow" class="url">Mariani Antonio Mario</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/#comment-1421">
			June 12, 2013 at 5:58 am</a>		</div>

		<p>smart doc …</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/?replytocom=1421#respond" onclick="return addComment.moveForm( &quot;div-comment-1421&quot;, &quot;1421&quot;, &quot;respond&quot;, &quot;416&quot; )" aria-label="Reply to Mariani Antonio Mario">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1" id="comment-1841">
				<div id="div-comment-1841" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/5abb7f23ffdd6c5d7e9d7f04f0903309?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/5abb7f23ffdd6c5d7e9d7f04f0903309?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/5abb7f23ffdd6c5d7e9d7f04f0903309?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-5abb7f23ffdd6c5d7e9d7f04f0903309-0">			<cite class="fn"><a href="http://www.caravan.coop" rel="external nofollow" class="url">Ben</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/#comment-1841">
			November 21, 2013 at 5:12 am</a>		</div>

		<p>This is amazing.. thanks. First time I’ve looked at assembly code and understood anything.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/?replytocom=1841#respond" onclick="return addComment.moveForm( &quot;div-comment-1841&quot;, &quot;1841&quot;, &quot;respond&quot;, &quot;416&quot; )" aria-label="Reply to Ben">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1 parent" id="comment-804064">
				<div id="div-comment-804064" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/e9cb1204c99c20f18a33943e4c436709?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/e9cb1204c99c20f18a33943e4c436709?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/e9cb1204c99c20f18a33943e4c436709?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-e9cb1204c99c20f18a33943e4c436709-0">			<cite class="fn">Rob</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/#comment-804064">
			February 13, 2015 at 6:22 am</a>		</div>

		<p>This is such an awesome tutorial! Thanks so much for putting it together.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/?replytocom=804064#respond" onclick="return addComment.moveForm( &quot;div-comment-804064&quot;, &quot;804064&quot;, &quot;respond&quot;, &quot;416&quot; )" aria-label="Reply to Rob">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-804127">
				<div id="div-comment-804127" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-0">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/#comment-804127">
			February 13, 2015 at 8:15 am</a>		</div>

		<p>Thanks Rob! Much appreciated.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/?replytocom=804127#respond" onclick="return addComment.moveForm( &quot;div-comment-804127&quot;, &quot;804127&quot;, &quot;respond&quot;, &quot;416&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-818162">
				<div id="div-comment-818162" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/95b218edfcf0b8ee2eb2839c08409a78?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/95b218edfcf0b8ee2eb2839c08409a78?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/95b218edfcf0b8ee2eb2839c08409a78?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-95b218edfcf0b8ee2eb2839c08409a78-0">			<cite class="fn">John Cole</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/#comment-818162">
			March 12, 2015 at 12:00 am</a>		</div>

		<p>This is excellent.  I know several assembly languages, so this has enabled me to add one more.  However, I do have a problem.  I get a Segmentation Fault when I try to store back the value from a register into memory.  Is code memory protected?</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/?replytocom=818162#respond" onclick="return addComment.moveForm( &quot;div-comment-818162&quot;, &quot;818162&quot;, &quot;respond&quot;, &quot;416&quot; )" aria-label="Reply to John Cole">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-818953">
				<div id="div-comment-818953" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-1">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/#comment-818953">
			March 12, 2015 at 8:42 pm</a>		</div>

		<p>Hi John,</p>
<p>if you are trying to store data to an address that contains code, yes, it will crash with a segmentation fault because of the memory protections set up by the operating system (and that are enforced by the processor itself). Code segments are in general not writeable.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/?replytocom=818953#respond" onclick="return addComment.moveForm( &quot;div-comment-818953&quot;, &quot;818953&quot;, &quot;respond&quot;, &quot;416&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1 parent" id="comment-866686">
				<div id="div-comment-866686" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/eee8a4c54ad1811a4f82c621e94fbee3?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/eee8a4c54ad1811a4f82c621e94fbee3?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/eee8a4c54ad1811a4f82c621e94fbee3?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-eee8a4c54ad1811a4f82c621e94fbee3-0">			<cite class="fn">Jesus</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/#comment-866686">
			May 14, 2015 at 5:14 pm</a>		</div>

		<p>I studied assembler in university but forgot everything, anyway thank you very much for the tutorial, is great! keep the hard work!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/?replytocom=866686#respond" onclick="return addComment.moveForm( &quot;div-comment-866686&quot;, &quot;866686&quot;, &quot;respond&quot;, &quot;416&quot; )" aria-label="Reply to Jesus">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-866729">
				<div id="div-comment-866729" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-2">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/#comment-866729">
			May 14, 2015 at 7:03 pm</a>		</div>

		<p>Thanks Jesus!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/?replytocom=866729#respond" onclick="return addComment.moveForm( &quot;div-comment-866729&quot;, &quot;866729&quot;, &quot;respond&quot;, &quot;416&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-875934">
				<div id="div-comment-875934" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/715a9ce06d3660e16f17be8f5219f7b9?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/715a9ce06d3660e16f17be8f5219f7b9?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/715a9ce06d3660e16f17be8f5219f7b9?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-715a9ce06d3660e16f17be8f5219f7b9-0">			<cite class="fn">Darwin Gray</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/#comment-875934">
			May 30, 2015 at 12:55 pm</a>		</div>

		<p>Great tutorial. Clear, concise, very easy to understand. Thanks a lot !</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/?replytocom=875934#respond" onclick="return addComment.moveForm( &quot;div-comment-875934&quot;, &quot;875934&quot;, &quot;respond&quot;, &quot;416&quot; )" aria-label="Reply to Darwin Gray">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-875949">
				<div id="div-comment-875949" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-3">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/#comment-875949">
			May 30, 2015 at 1:26 pm</a>		</div>

		<p>Thanks Darwin!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/?replytocom=875949#respond" onclick="return addComment.moveForm( &quot;div-comment-875949&quot;, &quot;875949&quot;, &quot;respond&quot;, &quot;416&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1 parent" id="comment-978796">
				<div id="div-comment-978796" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/ceb4a8ea4ead3431b925cb5d288c9eb0?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/ceb4a8ea4ead3431b925cb5d288c9eb0?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/ceb4a8ea4ead3431b925cb5d288c9eb0?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-ceb4a8ea4ead3431b925cb5d288c9eb0-0">			<cite class="fn">joser</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/#comment-978796">
			November 5, 2016 at 3:54 pm</a>		</div>

		<p>Thank you for these tutorials. I am learning so much going through them.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/?replytocom=978796#respond" onclick="return addComment.moveForm( &quot;div-comment-978796&quot;, &quot;978796&quot;, &quot;respond&quot;, &quot;416&quot; )" aria-label="Reply to joser">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-979186">
				<div id="div-comment-979186" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-4">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/#comment-979186">
			November 10, 2016 at 10:19 pm</a>		</div>

		<p>Thanks!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/?replytocom=979186#respond" onclick="return addComment.moveForm( &quot;div-comment-979186&quot;, &quot;979186&quot;, &quot;respond&quot;, &quot;416&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-979369">
				<div id="div-comment-979369" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/5637b90c2f9ca53b92074df28d9b95cc?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/5637b90c2f9ca53b92074df28d9b95cc?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/5637b90c2f9ca53b92074df28d9b95cc?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-5637b90c2f9ca53b92074df28d9b95cc-0">			<cite class="fn">Jason</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/#comment-979369">
			November 15, 2016 at 4:40 am</a>		</div>

		<p>For those who are experimenting with different values for the variables, it might be helpful to note that gdb gives the exit / error code in octal.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/?replytocom=979369#respond" onclick="return addComment.moveForm( &quot;div-comment-979369&quot;, &quot;979369&quot;, &quot;respond&quot;, &quot;416&quot; )" aria-label="Reply to Jason">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-979801">
				<div id="div-comment-979801" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-5">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/#comment-979801">
			November 20, 2016 at 12:29 pm</a>		</div>

		<p>Oh yes. That leading zero is very subtle.</p>
<p>Thanks Jason!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/?replytocom=979801#respond" onclick="return addComment.moveForm( &quot;div-comment-979801&quot;, &quot;979801&quot;, &quot;respond&quot;, &quot;416&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		</ul>
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2013/01/12/arm-assembler-raspberry-pi-chapter-4/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="416" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="a4b7217c64"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496593338449"></form>
			</div><!-- #respond -->
	</div>	</div>