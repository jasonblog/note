# The Thumb instruction set


<div class="main">
			<h1>ARM assembler in Raspberry Pi – Chapter 22 </h1>
		<p class="post-meta"><span class="date">December 20, 2014</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2014/12/20/arm-assembler-raspberry-pi-chapter-22/#comments">4</a></span> </p>
		<p>
Several times in previous chapters we have talked about ARM as an architecture that has several features aimed at embedding systems. In embedded systems memory is scarce and expensive, so designs that help reduce the memory footprint are very welcome. Today we will see another of these features: the Thumb instruction set.
</p>
<p><span id="more-1725"></span></p>
<h2>The Thumb instruction set</h2>
<p>
In previous installments we have been working with the ARMv6 instruction set (the one implemented in the Raspberry Pi). In this instruction set, all instructions are 32-bit wide, so every instruction takes 4 bytes. This is a common design since the arrival of <a href="http://en.wikipedia.org/wiki/Reduced_instruction_set_computing">RISC processors</a>. That said, in some scenarios such codification is overkill in terms of memory consumption: many platforms are very simple and rarely need all the features provided by the instruction set. If only they could use a subset of the original instruction set that can be encoded in a smaller number of bits!
</p>
<p>
So, this is what the <strong>Thumb instruction set</strong> is all about. They are a reencoded subset of the ARM instructions that take only 16 bits per instructions. This means that we will have to waive away some instructions. As a benefit our code density is higher: most of the time we will be able to encode the code of our programs in half the space.
</p>
<h3>Support of Thumb in Raspbian</h3>
<p>
While the processor of the Raspberry Pi properly supports Thumb, there is still some software support that unfortunately is not provided by Raspbian. This means that we will be able to write<br>
some snippets in Thumb but in general this is not supported (if you try to use Thumb for a full C program you will end with a <code>sorry, unimplemented</code> message by the compiler).
</p>
<h2>Instructions</h2>
<p>
Thumb provides about 45 instructions (of about 115 in ARMv6). The narrower codification of 16 bit means that we will be more limited in what we can do in our code. Registers are split into two sets: <em>low registers</em>, <code>r0</code> to <code>r7</code>, and <em>high registers</em>, <code>r7</code> to <code>r15</code>. Most instructions can only fully work with low registers and some others have limited behaviour when working with high registers.
</p>
<p>
Also, Thumb instructions cannot be predicated. Recall that almost every ARM instruction can be made conditional depending on the flags in the <code>cpsr</code> register. This is not the case in Thumb where only the branch instruction is conditional.
</p>
<p>
Mixing ARM and Thumb is only possible at function level: a function must be wholly ARM or Thumb, it cannot be a mix of the two instruction sets. Recall that our Raspbian system does not support Thumb so at some point we will have to jump from ARM code to Thumb code. This is done using the instruction (available in both instruction sets) <code>blx</code>. This instruction behaves like the <code>bl</code> instruction we use for function calls but changes the state of the processor from ARM to Thumb (or Thumb to ARM).
</p>
<p>
We also have to tell the assembler that some portion of assembler is actually Thumb while the other is ARM. Since by default the assembler expects ARM, we will have to change to Thumb at some point.
</p>
<h2>From ARM to Thumb</h2>
<p>
Let’s start with a very simple program returning an error code of 2 set in Thumb.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> thumb<span style="color: #339933;">-</span>first<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">code</span> <span style="color: #ff0000;">16</span>     <span style="color: #339933;">/*</span> Here we say we will use Thumb <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">2</span>     <span style="color: #339933;">/*</span> Make sure instructions are aligned <span style="color: #0000ff; font-weight: bold;">at</span> <span style="color: #ff0000;">2</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> boundary <span style="color: #339933;">*/</span>
&nbsp;
thumb_function<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span>   <span style="color: #339933;">/*</span> r0 ← <span style="color: #ff0000;">2</span> <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr        <span style="color: #339933;">/*</span> return <span style="color: #339933;">*/</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">code</span> <span style="color: #ff0000;">32</span>     <span style="color: #339933;">/*</span> Here we say we will use ARM <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>     <span style="color: #339933;">/*</span> Make sure instructions are aligned <span style="color: #0000ff; font-weight: bold;">at</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> boundary <span style="color: #339933;">*/</span>
&nbsp;
<span style="color: #339933;">.</span>globl main
main<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
&nbsp;
    blx thumb_function <span style="color: #339933;">/*</span> From ARM to Thumb we use blx <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* thumb-first.s */
.text

.code 16     /* Here we say we will use Thumb */
.align 2     /* Make sure instructions are aligned at 2-byte boundary */

thumb_function:
    mov r0, #2   /* r0 ← 2 */
    bx lr        /* return */
    
.code 32     /* Here we say we will use ARM */
.align 4     /* Make sure instructions are aligned at 4-byte boundary */

.globl main
main:
    push {r4, lr}
    
    blx thumb_function /* From ARM to Thumb we use blx */

    pop {r4, lr}
    bx lr</p></div>

<p>
Thumb instructions in our thumb_function actually resemble ARM instructions. In fact most of the time there will not be much difference. As stated above, Thumb instructions are more limited in features than their ARM counterparts.
</p>
<p>
If we run the program, it does what we expect.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="shell" style="font-family:monospace;">$ ./thumb-first; echo $?
2</pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ ./thumb-first; echo $?
2</p></div>

<p>
How can we tell our program actually mixes ARM and Thumb? We can use <code>objdump -d</code> to dump the instructions of our <code>thumb-first.o</code> file.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="shell" style="font-family:monospace;">$ objdump  -d thumb-first.o 
&nbsp;
thumb-first.o:     file format elf32-littlearm
&nbsp;
&nbsp;
Disassembly of section .text:
&nbsp;
00000000 &lt;thumb_function&gt;:
   0:	2002      	movs	r0, #2
   2:	4770      	bx	lr
   4:	e1a00000 	nop			; (mov r0, r0)
   8:	e1a00000 	nop			; (mov r0, r0)
   c:	e1a00000 	nop			; (mov r0, r0)
&nbsp;
00000010 &lt;main&gt;:
  10:	e92d4010 	push	{r4, lr}
  14:	fafffff9 	blx	0 &lt;thumb_function&gt;
  18:	e8bd4010 	pop	{r4, lr}
  1c:	e12fff1e 	bx	lr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ objdump  -d thumb-first.o 

thumb-first.o:     file format elf32-littlearm


Disassembly of section .text:

00000000 &lt;thumb_function&gt;:
   0:	2002      	movs	r0, #2
   2:	4770      	bx	lr
   4:	e1a00000 	nop			; (mov r0, r0)
   8:	e1a00000 	nop			; (mov r0, r0)
   c:	e1a00000 	nop			; (mov r0, r0)

00000010 &lt;main&gt;:
  10:	e92d4010 	push	{r4, lr}
  14:	fafffff9 	blx	0 &lt;thumb_function&gt;
  18:	e8bd4010 	pop	{r4, lr}
  1c:	e12fff1e 	bx	lr</p></div>

<p>
Check <code>thumb_function</code>: its two instructions are encoded in just two bytes (instruction <code>bx lr</code> is at offset 2 of <code>mov r0, #2</code>. Compare this to the instructions in <code>main</code>: each one is at offset 4 of its predecessor instruction. Note that some padding was added by the assembler at the end of the <code>thumb_function</code> in form of <code>nop</code>s (that should not be executed, anyway).
</p>
<h2>Calling functions in Thumb</h2>
<p>
In in Thumb we want to follow the AAPCS convention like we do when in ARM mode, but then some oddities happen. Consider the following snippet where <code>thumb_function_1</code> calls <code>thumb_function_2</code>.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">code</span> <span style="color: #ff0000;">16</span>     <span style="color: #339933;">/*</span> Here we say we will use Thumb <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">2</span>     <span style="color: #339933;">/*</span> Make sure instructions are aligned <span style="color: #0000ff; font-weight: bold;">at</span> <span style="color: #ff0000;">2</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> boundary <span style="color: #339933;">*/</span>
thumb_function_2<span style="color: #339933;">:</span>
    <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">Do</span> something here <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr
&nbsp;
thumb_function_1<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> thumb_function_2
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>    <span style="color: #339933;">/*</span> ERROR<span style="color: #339933;">:</span> cannot use lr <span style="color: #00007f; font-weight: bold;">in</span> <span style="color: #00007f; font-weight: bold;">pop</span>  <span style="color: #00007f; font-weight: bold;">in</span> Thumb mode <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">.code 16     /* Here we say we will use Thumb */
.align 2     /* Make sure instructions are aligned at 2-byte boundary */
thumb_function_2:
    /* Do something here */
    bx lr

thumb_function_1:
    push {r4, lr}
    bl thumb_function_2
    pop {r4, lr}    /* ERROR: cannot use lr in pop  in Thumb mode */
    bx lr</p></div>

<p>
Unfortunately, this will be rejected by the assembler. If you recall from chapter 10, in ARM push and pop are mnemonics for <code>stmdb sp!</code> and <code>ldmia sp!</code>, respectively. But in Thumb mode <code>push</code> and <code>pop</code> are instructions on their own and so they are more limited: <code>push</code> can only use low registers and <code>lr</code>, <code>pop</code> can only use low registers and <code>pc</code>. The behaviour of these two instructions almost the same as the ARM mnemomics. So, you are now probably wondering why these two special cases for <code>lr</code> and <code>pc</code>. This is the trick: in Thumb mode <code>pop {pc}</code> is equivalent to pop the value <code>val</code> from the stack and then do <code>bx val</code>. So the two instruction sequence: <code>pop {r4, lr}</code> followed by <code>bx lr</code> becomes simply <code>pop {r4, pc}</code>.
</p>
<p>
So, our code will look like this.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> thumb<span style="color: #339933;">-</span><span style="color: #00007f; font-weight: bold;">call</span><span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">code</span> <span style="color: #ff0000;">16</span>     <span style="color: #339933;">/*</span> Here we say we will use Thumb <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">2</span>     <span style="color: #339933;">/*</span> Make sure instructions are aligned <span style="color: #0000ff; font-weight: bold;">at</span> <span style="color: #ff0000;">2</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> boundary <span style="color: #339933;">*/</span>
&nbsp;
thumb_function_2<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr   <span style="color: #339933;">/*</span> A leaf Thumb function <span style="color: #009900; font-weight: bold;">(</span>i<span style="color: #339933;">.</span>e<span style="color: #339933;">.</span> a function that does <span style="color: #00007f; font-weight: bold;">not</span> <span style="color: #00007f; font-weight: bold;">call</span>
               any other function so it did <span style="color: #00007f; font-weight: bold;">not</span> have to keep lr <span style="color: #00007f; font-weight: bold;">in</span> the <span style="color: #0000ff; font-weight: bold;">stack</span><span style="color: #009900; font-weight: bold;">)</span>
               returns using <span style="color: #7f007f;">"bx lr"</span> <span style="color: #339933;">*/</span>
&nbsp;
thumb_function_1<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> thumb_function_2 <span style="color: #339933;">/*</span> From Thumb to Thumb we use <span style="color: #46aa03; font-weight: bold;">bl</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> pc<span style="color: #009900; font-weight: bold;">}</span>  <span style="color: #339933;">/*</span> This is how we return from a non<span style="color: #339933;">-</span>leaf Thumb function <span style="color: #339933;">*/</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">code</span> <span style="color: #ff0000;">32</span>     <span style="color: #339933;">/*</span> Here we say we will use ARM <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>     <span style="color: #339933;">/*</span> Make sure instructions are aligned <span style="color: #0000ff; font-weight: bold;">at</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> boundary <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span>globl main
main<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
&nbsp;
    blx thumb_function_1 <span style="color: #339933;">/*</span> From ARM to Thumb we use blx <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* thumb-call.s */
.text

.code 16     /* Here we say we will use Thumb */
.align 2     /* Make sure instructions are aligned at 2-byte boundary */

thumb_function_2:
    mov r0, #2
    bx lr   /* A leaf Thumb function (i.e. a function that does not call
               any other function so it did not have to keep lr in the stack)
               returns using "bx lr" */

thumb_function_1:
    push {r4, lr}
    bl thumb_function_2 /* From Thumb to Thumb we use bl */
    pop {r4, pc}  /* This is how we return from a non-leaf Thumb function */

.code 32     /* Here we say we will use ARM */
.align 4     /* Make sure instructions are aligned at 4-byte boundary */
.globl main
main:
    push {r4, lr}

    blx thumb_function_1 /* From ARM to Thumb we use blx */

    pop {r4, lr}
    bx lr</p></div>

<h2>From Thumb to ARM</h2>
<p>
Finally we may want to call an ARM function from Thumb. As long as we stick to AAPCS everything should work correctly. The Thumb instruction to call an ARM function is again <code>blx</code>. Following is an example of a small program that says “Hello world” four times calling <code>printf</code>, a function in the C library that in Raspbian is of course implemented using ARM instructions.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> thumb<span style="color: #339933;">-</span>first<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.data</span>
message<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"Hello world %d\n"</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">code</span> <span style="color: #ff0000;">16</span>     <span style="color: #339933;">/*</span> Here we say we will use Thumb <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">2</span>     <span style="color: #339933;">/*</span> Make sure instructions are aligned <span style="color: #0000ff; font-weight: bold;">at</span> <span style="color: #ff0000;">2</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> boundary <span style="color: #339933;">*/</span>
thumb_function<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>         <span style="color: #339933;">/*</span> keep r4 <span style="color: #00007f; font-weight: bold;">and</span> lr <span style="color: #00007f; font-weight: bold;">in</span> the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>            <span style="color: #339933;">/*</span> r4 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    b check_loop          <span style="color: #339933;">/*</span> unconditional branch to check_loop <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">loop</span><span style="color: #339933;">:</span>        
       <span style="color: #339933;">/*</span> prepare the <span style="color: #00007f; font-weight: bold;">call</span> to printf <span style="color: #339933;">*/</span>
       ldr r0<span style="color: #339933;">,</span> addr_of_message  <span style="color: #339933;">/*</span> r0 ← &amp;message <span style="color: #339933;">*/</span>
       <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> r4               <span style="color: #339933;">/*</span> r1 ← r4 <span style="color: #339933;">*/</span>
       blx printf               <span style="color: #339933;">/*</span> From Thumb to ARM we use blx<span style="color: #339933;">.</span>
                                   printf is a function
                                   <span style="color: #00007f; font-weight: bold;">in</span> the C library that is implemented
                                   using ARM instructions <span style="color: #339933;">*/</span>
       <span style="color: #00007f; font-weight: bold;">add</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>           <span style="color: #339933;">/*</span> r4 ← r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    check_loop<span style="color: #339933;">:</span>
       <span style="color: #00007f; font-weight: bold;">cmp</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>               <span style="color: #339933;">/*</span> compute r4 <span style="color: #339933;">-</span> <span style="color: #ff0000;">4</span> <span style="color: #00007f; font-weight: bold;">and</span> update the cpsr <span style="color: #339933;">*/</span>
       blt <span style="color: #00007f; font-weight: bold;">loop</span>                 <span style="color: #339933;">/*</span> if the cpsr means that r4 is lower than <span style="color: #ff0000;">4</span> 
                                   then branch to <span style="color: #00007f; font-weight: bold;">loop</span> <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> pc<span style="color: #009900; font-weight: bold;">}</span>          <span style="color: #339933;">/*</span> restore registers <span style="color: #00007f; font-weight: bold;">and</span> return from Thumb function <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
addr_of_message<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> message
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">code</span> <span style="color: #ff0000;">32</span>     <span style="color: #339933;">/*</span> Here we say we will use ARM <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>     <span style="color: #339933;">/*</span> Make sure instructions are aligned <span style="color: #0000ff; font-weight: bold;">at</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> boundary <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span>globl main
main<span style="color: #339933;">:</span>  
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>      <span style="color: #339933;">/*</span> keep r4 <span style="color: #00007f; font-weight: bold;">and</span> lr <span style="color: #00007f; font-weight: bold;">in</span> the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
    blx thumb_function <span style="color: #339933;">/*</span> from ARM to Thumb we use blx  <span style="color: #339933;">*/</span>       
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>       <span style="color: #339933;">/*</span> restore registers <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr              <span style="color: #339933;">/*</span> return <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* thumb-first.s */

.text

.data
message: .asciz "Hello world %d\n"
    
.code 16     /* Here we say we will use Thumb */
.align 2     /* Make sure instructions are aligned at 2-byte boundary */
thumb_function:
    push {r4, lr}         /* keep r4 and lr in the stack */
    mov r4, #0            /* r4 ← 0 */
    b check_loop          /* unconditional branch to check_loop */
    loop:        
       /* prepare the call to printf */
       ldr r0, addr_of_message  /* r0 ← &amp;message */
       mov r1, r4               /* r1 ← r4 */
       blx printf               /* From Thumb to ARM we use blx.
                                   printf is a function
                                   in the C library that is implemented
                                   using ARM instructions */
       add r4, r4, #1           /* r4 ← r4 + 1 */
    check_loop:
       cmp r4, #4               /* compute r4 - 4 and update the cpsr */
       blt loop                 /* if the cpsr means that r4 is lower than 4 
                                   then branch to loop */

    pop {r4, pc}          /* restore registers and return from Thumb function */
.align 4
addr_of_message: .word message
    
.code 32     /* Here we say we will use ARM */
.align 4     /* Make sure instructions are aligned at 4-byte boundary */
.globl main
main:  
    push {r4, lr}      /* keep r4 and lr in the stack */
    blx thumb_function /* from ARM to Thumb we use blx  */       
    pop {r4, lr}       /* restore registers */
    bx lr              /* return */</p></div>

<h2>To know more</h2>
<p>
In next installments we will go back to ARM, so if you are interested in Thumb, you may want to check this <a href="http://infocenter.arm.com/help/topic/com.arm.doc.qrc0006e/QRC0006_UAL16.pdf">Thumb 16-bit Instruction Set Quick Reference Card</a> provided by ARM. When checking that card, be aware that the processor of the Raspberry Pi only implements ARMv6T, not ARMv6T2.
</p>
<p>
That’s all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2014/12/20/arm-assembler-raspberry-pi-chapter-22/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2014/12/20/arm-assembler-raspberry-pi-chapter-22/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2014/12/20/arm-assembler-raspberry-pi-chapter-22/&amp;text=ARM+assembler+in+Raspberry+Pi+%E2%80%93+Chapter+22+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2014/12/20/arm-assembler-raspberry-pi-chapter-22/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>				<p class="tags"><span><a href="http://thinkingeek.com/tag/arm/" rel="tag">arm</a>, <a href="http://thinkingeek.com/tag/assembler/" rel="tag">assembler</a></span></p>		<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2014/08/23/arm-assembler-raspberry-pi-chapter-21/" rel="prev">ARM assembler in Raspberry Pi – Chapter 21</a></span>
			<span class="next"><a href="http://thinkingeek.com/2015/01/02/arm-assembler-raspberry-pi-chapter-23/" rel="next">ARM assembler in Raspberry Pi – Chapter 23</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
			<h3>4 thoughts on “<span>ARM assembler in Raspberry Pi – Chapter 22</span>”</h3>
		<ul class="commentlist">
				<li class="comment even thread-even depth-1 parent" id="comment-766875">
				<div id="div-comment-766875" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-9e81362ff9654533accffc53762c9520-0">			<cite class="fn"><a href="http://devel.meruelo.com.es/blog/" rel="external nofollow" class="url">Fernando</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/12/20/arm-assembler-raspberry-pi-chapter-22/#comment-766875">
			January 17, 2015 at 11:06 pm</a>		</div>

		<p>Glad to read you back! Nice article!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/12/20/arm-assembler-raspberry-pi-chapter-22/?replytocom=766875#respond" onclick="return addComment.moveForm( &quot;div-comment-766875&quot;, &quot;766875&quot;, &quot;respond&quot;, &quot;1725&quot; )" aria-label="Reply to Fernando">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-776616">
				<div id="div-comment-776616" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-0">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/12/20/arm-assembler-raspberry-pi-chapter-22/#comment-776616">
			January 23, 2015 at 9:37 pm</a>		</div>

		<p>Thanks Fernando!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/12/20/arm-assembler-raspberry-pi-chapter-22/?replytocom=776616#respond" onclick="return addComment.moveForm( &quot;div-comment-776616&quot;, &quot;776616&quot;, &quot;respond&quot;, &quot;1725&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-816835">
				<div id="div-comment-816835" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/8e02ba12a3b8f9d19306c56fbde0d455?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/8e02ba12a3b8f9d19306c56fbde0d455?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/8e02ba12a3b8f9d19306c56fbde0d455?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-8e02ba12a3b8f9d19306c56fbde0d455-0">			<cite class="fn"><a href="http://pervin@utdallas.edu" rel="external nofollow" class="url">William Pervin</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/12/20/arm-assembler-raspberry-pi-chapter-22/#comment-816835">
			March 10, 2015 at 5:08 pm</a>		</div>

		<p>It seems to me that you don’t have to be in Thumb mode to have push {r4, lr} at the beginning and pop {r4, pc} at the end to get the usual result of returning normally. </p>
<p>I note that the mul command sometimes requires the operands to be in different registers (with slight changes for Thumb mode). Where can I find detailed descriptions of such simple operators?</p>
<p>Still really enjoying your notes.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/12/20/arm-assembler-raspberry-pi-chapter-22/?replytocom=816835#respond" onclick="return addComment.moveForm( &quot;div-comment-816835&quot;, &quot;816835&quot;, &quot;respond&quot;, &quot;1725&quot; )" aria-label="Reply to William Pervin">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-818022">
				<div id="div-comment-818022" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-1">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/12/20/arm-assembler-raspberry-pi-chapter-22/#comment-818022">
			March 11, 2015 at 8:59 pm</a>		</div>

		<p>Hi William,</p>
<p>well you can get the documentation of the ARMv6 architecture in the <a href="http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0419c/index.html" title="ARM Information Center" rel="nofollow">ARM Information Center</a>. The document is a PDF only available upon  (free, AFAIK) registration.</p>
<p>That said, if you don’t feel like registering for only one document there are <a href="https://www.scss.tcd.ie/~waldroj/3d1/arm_arm.pdf" rel="nofollow">some copies online</a>.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/12/20/arm-assembler-raspberry-pi-chapter-22/?replytocom=818022#respond" onclick="return addComment.moveForm( &quot;div-comment-818022&quot;, &quot;818022&quot;, &quot;respond&quot;, &quot;1725&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		</ul>
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2014/12/20/arm-assembler-raspberry-pi-chapter-22/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="1725" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="33a397cf36"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496666817517"></form>
			</div><!-- #respond -->
	</div>	</div>