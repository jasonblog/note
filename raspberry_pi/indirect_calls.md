# Indirect calls


<div class="main">
			<h1>ARM assembler in Raspberry Pi – Chapter 20 </h1>
		<p class="post-meta"><span class="date">August 20, 2014</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2014/08/20/arm-assembler-raspberry-pi-chapter-20/#comments">5</a></span> </p>
		<p>
Today we will see how to make indirect calls.
</p>
<p><span id="more-1571"></span></p>
<h2>Labels</h2>
<p>
One of the distinguishing features of assemblers is the shortage of symbolic information. The only symbolic support available at this (low) level are <em>labels</em>. We already know that labels are just addresses to the memory of the program (both data and code).
</p>
<p>
When we define a function in assembler, we define a label for it.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">fun<span style="color: #339933;">:</span> <span style="color: #339933;">/*</span> label <span style="color: #7f007f;">'fun'</span> <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #009900; font-weight: bold;">}</span>
  <span style="color: #339933;">...</span>
  <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #009900; font-weight: bold;">}</span>
  <span style="color: #46aa03; font-weight: bold;">bx</span> lr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">fun: /* label 'fun' */
  push {r4, r5}
  ...
  pop {r4, r5}
  bx lr</p></div>

<p>
Later (or before, assemblers usually do not care) we use the label. So a call like
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">  <span style="color: #46aa03; font-weight: bold;">bl</span> fun</pre></td></tr></tbody></table><p class="theCode" style="display:none;">  bl fun</p></div>

<p>
Is saying to the assembler, <q>I’m using <code>fun</code> here, but you have to put the appropiate address there when generating machine code, ok?</q>.
</p>
<p>
In reality, calling a function is usually much more involved but at the end there is a label that brings us to the function.
</p>
<h2>Our first indirect call</h2>
<p>
What if rather than using the label of a function, we were able to keep the addres of a function (or several of them) somewhere and call a function indirectly? Let’s try that. First, we will start with a basic <q>Hello world</q> that uses a label. We will call this a <em>direct call</em>.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #0000ff; font-weight: bold;">.data</span>     <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">data</span> <span style="color: #0000ff; font-weight: bold;">section</span> <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> ensure the next label is <span style="color: #ff0000;">4</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
message<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"Hello world\n"</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>     <span style="color: #339933;">/*</span> text <span style="color: #0000ff; font-weight: bold;">section</span> <span style="color: #009900; font-weight: bold;">(</span>= <span style="color: #0000ff; font-weight: bold;">code</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> ensure the next label is <span style="color: #ff0000;">4</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
say_hello<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>            <span style="color: #339933;">/*</span> keep lr because we <span style="color: #00007f; font-weight: bold;">call</span> printf<span style="color: #339933;">,</span> 
                                we keep r4 to keep the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #ff0000;">8</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span>
                                aligned<span style="color: #339933;">,</span> as per AAPCS requirements <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> Prepare the <span style="color: #00007f; font-weight: bold;">call</span> to printf <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> addr_of_message  <span style="color: #339933;">/*</span> r0 ← &amp;message <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> printf                <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> printf <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>             <span style="color: #339933;">/*</span> restore r4 <span style="color: #00007f; font-weight: bold;">and</span> lr <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr                    <span style="color: #339933;">/*</span> return to the caller <span style="color: #339933;">*/</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> ensure the next label is <span style="color: #ff0000;">4</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
addr_of_message<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> message
&nbsp;
<span style="color: #339933;">.</span>globl main <span style="color: #339933;">/*</span> state that <span style="color: #7f007f;">'main'</span> label is <span style="color: #0000ff; font-weight: bold;">global</span> <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> ensure the next label is <span style="color: #ff0000;">4</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
main<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>            <span style="color: #339933;">/*</span> keep lr because we <span style="color: #00007f; font-weight: bold;">call</span> say_hello<span style="color: #339933;">,</span> 
                                we keep r4 to keep the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #ff0000;">8</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span>
                                aligned<span style="color: #339933;">,</span> as per AAPCS requirements <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> say_hello             <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> say_hello<span style="color: #339933;">,</span> directly<span style="color: #339933;">,</span> using the label <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>               <span style="color: #339933;">/*</span> return from the program<span style="color: #339933;">,</span> set error <span style="color: #0000ff; font-weight: bold;">code</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>             <span style="color: #339933;">/*</span> restore r4 <span style="color: #00007f; font-weight: bold;">and</span> lr <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr                    <span style="color: #339933;">/*</span> return to the caller <span style="color: #009900; font-weight: bold;">(</span>the system<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">.data     /* data section */
.align 4  /* ensure the next label is 4-byte aligned */
message: .asciz "Hello world\n"

.text     /* text section (= code) */

.align 4  /* ensure the next label is 4-byte aligned */
say_hello:
    push {r4, lr}            /* keep lr because we call printf, 
                                we keep r4 to keep the stack 8-byte
                                aligned, as per AAPCS requirements */
    /* Prepare the call to printf */
    ldr r0, addr_of_message  /* r0 ← &amp;message */
    bl printf                /* call printf */
    pop {r4, lr}             /* restore r4 and lr */
    bx lr                    /* return to the caller */

.align 4  /* ensure the next label is 4-byte aligned */
addr_of_message: .word message

.globl main /* state that 'main' label is global */
.align 4  /* ensure the next label is 4-byte aligned */
main:
    push {r4, lr}            /* keep lr because we call say_hello, 
                                we keep r4 to keep the stack 8-byte
                                aligned, as per AAPCS requirements */
    bl say_hello             /* call say_hello, directly, using the label */

    mov r0, #0               /* return from the program, set error code */
    pop {r4, lr}             /* restore r4 and lr */
    bx lr                    /* return to the caller (the system) */</p></div>

<p>
Now let’s add some storage in the data section to keep the address of <code>say_hello</code>.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #0000ff; font-weight: bold;">.data</span>     <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">data</span> <span style="color: #0000ff; font-weight: bold;">section</span> <span style="color: #339933;">*/</span>
<span style="color: #339933;">...</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> ensure the next label is <span style="color: #ff0000;">4</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
ptr_of_fun<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">0</span>   <span style="color: #339933;">/*</span> we set its initial value zero <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">.data     /* data section */
...
.align 4  /* ensure the next label is 4-byte aligned */
ptr_of_fun: .word 0   /* we set its initial value zero */</p></div>

<p>
Now we will add a new function <code>make_indirect_call</code> that does the indirect call using the value stored in <code>ptr_of_fun</code>.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
make_indirect_call<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>            <span style="color: #339933;">/*</span> keep lr because we <span style="color: #00007f; font-weight: bold;">call</span> printf<span style="color: #339933;">,</span> 
                                we keep r4 to keep the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #ff0000;">8</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span>
                                aligned<span style="color: #339933;">,</span> as per AAPCS requirements <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> addr_ptr_of_fun  <span style="color: #339933;">/*</span> r0 ← &amp;ptr_of_fun <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>             <span style="color: #339933;">/*</span> r0 ← <span style="color: #339933;">*</span>r0 <span style="color: #339933;">*/</span>
    blx r0                   <span style="color: #339933;">/*</span> indirect <span style="color: #00007f; font-weight: bold;">call</span> to r0 <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>             <span style="color: #339933;">/*</span> restore r4 <span style="color: #00007f; font-weight: bold;">and</span> lr <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr                    <span style="color: #339933;">/*</span> return to the caller <span style="color: #339933;">*/</span>
&nbsp;
addr_ptr_of_fun<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> ptr_of_fun</pre></td></tr></tbody></table><p class="theCode" style="display:none;">.align 4
make_indirect_call:
    push {r4, lr}            /* keep lr because we call printf, 
                                we keep r4 to keep the stack 8-byte
                                aligned, as per AAPCS requirements */
    ldr r0, addr_ptr_of_fun  /* r0 ← &amp;ptr_of_fun */
    ldr r0, [r0]             /* r0 ← *r0 */
    blx r0                   /* indirect call to r0 */
    pop {r4, lr}             /* restore r4 and lr */
    bx lr                    /* return to the caller */

addr_ptr_of_fun: .word ptr_of_fun</p></div>

<p>
Doing an indirect call is done using the instruction <code>blx</code>. It behaves like <code>bl</code> but expects a register rather than a label.
</p>
<p>
Yoy may be wondering whether we could have used <code>bx</code> rather than <code>blx</code>. We cannot. The instruction <code>bx</code> does not set the <code>lr</code> register to the next instruction, like <code>bl</code> and <code>blx</code> do. Thus, we would call the function but it would not be able to return: it would jump back to the wrong place! (try to think which one).
</p>
<p>
Now in the <code>main</code> we will keep the address of <code>say_hello</code> in <code>ptr_of_fun</code> and call <code>make_indirect_call</code>.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">main<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>            <span style="color: #339933;">/*</span> keep lr because we <span style="color: #00007f; font-weight: bold;">call</span> printf<span style="color: #339933;">,</span> 
                                we keep r4 to keep the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #ff0000;">8</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span>
                                aligned<span style="color: #339933;">,</span> as per AAPCS requirements <span style="color: #339933;">*/</span>
&nbsp;
    ldr r1<span style="color: #339933;">,</span> addr_say_hello   <span style="color: #339933;">/*</span> r1 ← &amp;say_hello <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> addr_ptr_of_fun  <span style="color: #339933;">/*</span> r0 ← &amp;addr_ptr_of_fun <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>             <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span>r0 ← r1
                                this is
                                ptr_of_fun ← &amp;say_hello <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #46aa03; font-weight: bold;">bl</span> make_indirect_call    <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> make_indirect_call <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>               <span style="color: #339933;">/*</span> return from the program<span style="color: #339933;">,</span> set error <span style="color: #0000ff; font-weight: bold;">code</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>             <span style="color: #339933;">/*</span> restore r4 <span style="color: #00007f; font-weight: bold;">and</span> lr <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr                    <span style="color: #339933;">/*</span> return to the caller <span style="color: #009900; font-weight: bold;">(</span>the system<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
&nbsp;
addr_ptr_of_fun<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> ptr_of_fun
addr_say_hello <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> say_hello</pre></td></tr></tbody></table><p class="theCode" style="display:none;">main:
    push {r4, lr}            /* keep lr because we call printf, 
                                we keep r4 to keep the stack 8-byte
                                aligned, as per AAPCS requirements */

    ldr r1, addr_say_hello   /* r1 ← &amp;say_hello */
    ldr r0, addr_ptr_of_fun  /* r0 ← &amp;addr_ptr_of_fun */
    str r1, [r0]             /* *r0 ← r1
                                this is
                                ptr_of_fun ← &amp;say_hello */

    bl make_indirect_call    /* call make_indirect_call */

    mov r0, #0               /* return from the program, set error code */
    pop {r4, lr}             /* restore r4 and lr */
    bx lr                    /* return to the caller (the system) */

addr_ptr_of_fun: .word ptr_of_fun
addr_say_hello : .word say_hello</p></div>

<p>
Note that, in the function <code>make_indirect_call</code> we did
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">    ldr r0<span style="color: #339933;">,</span> addr_ptr_of_fun  <span style="color: #339933;">/*</span> r0 ← &amp;ptr_of_fun <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>             <span style="color: #339933;">/*</span> r0 ← <span style="color: #339933;">*</span>r0 <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">    ldr r0, addr_ptr_of_fun  /* r0 ← &amp;ptr_of_fun */
    ldr r0, [r0]             /* r0 ← *r0 */</p></div>

<p>while in the <code>main</code> we do</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">    ldr r1<span style="color: #339933;">,</span> addr_say_hello   <span style="color: #339933;">/*</span> r1 ← &amp;say_hello <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">    ldr r1, addr_say_hello   /* r1 ← &amp;say_hello */</p></div>

<p>
This is a similar case like arrays: when we load an array address, we do not need to load again (as it happens when we load simple scalars). This is because if we did that, we would be loading the first element of the array. With functions a similar thing happens: the function itself, its label, is already an address. If we did another load we would be loading an instruction into the register!! Not quite what we want <img draggable="false" class="emoji" alt="🙂" src="https://s.w.org/images/core/emoji/2.2.1/svg/1f642.svg" scale="0">
</p>
<p>
In the function <code>make_indirect_call</code> we are not loading a function but a pointer to a function (<code>addr_ptr_of_fun</code>), so we have to do the typical double load we do for scalars (because at the end, a pointer is just an integer that happens to be an address of the memory of our program).
</p>
<h2>Feel the power</h2>
<p>
The last example does not look very interesting, but being able to call a function indirectly is a very powerful thing. It allows us to keep the address of a function somewhere and call it. It allows us to pass the address of a function to another function. Why would we want to do that? Well, it is a rudimentary, yet effective, way of passing code to another function.
</p>
<p>
As an example, let’s make a generic <em>greeter</em> function which receives a greeting function as a parameter. This way the exact greeting is actually deferred to another function.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #0000ff; font-weight: bold;">.data</span>     <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">data</span> <span style="color: #0000ff; font-weight: bold;">section</span> <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> ensure the next label is <span style="color: #ff0000;">4</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
message_1<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"Hello\n"</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> ensure the next label is <span style="color: #ff0000;">4</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
message_2<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"Bonjour\n"</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>     <span style="color: #339933;">/*</span> text <span style="color: #0000ff; font-weight: bold;">section</span> <span style="color: #009900; font-weight: bold;">(</span>= <span style="color: #0000ff; font-weight: bold;">code</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> ensure the next label is <span style="color: #ff0000;">4</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
say_hello<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>            <span style="color: #339933;">/*</span> keep lr because we <span style="color: #00007f; font-weight: bold;">call</span> printf<span style="color: #339933;">,</span> 
                                we keep r4 to keep the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #ff0000;">8</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span>
                                aligned<span style="color: #339933;">,</span> as per AAPCS requirements <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> Prepare the <span style="color: #00007f; font-weight: bold;">call</span> to printf <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> addr_of_message_1 <span style="color: #339933;">/*</span> r0 ← &amp;message <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> printf                 <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> printf <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>              <span style="color: #339933;">/*</span> restore r4 <span style="color: #00007f; font-weight: bold;">and</span> lr <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr                     <span style="color: #339933;">/*</span> return to the caller <span style="color: #339933;">*/</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> ensure the next label is <span style="color: #ff0000;">4</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
addr_of_message_1<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> message_1
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> ensure the next label is <span style="color: #ff0000;">4</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
say_bonjour<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>            <span style="color: #339933;">/*</span> keep lr because we <span style="color: #00007f; font-weight: bold;">call</span> printf<span style="color: #339933;">,</span> 
                                we keep r4 to keep the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #ff0000;">8</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span>
                                aligned<span style="color: #339933;">,</span> as per AAPCS requirements <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> Prepare the <span style="color: #00007f; font-weight: bold;">call</span> to printf <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> addr_of_message_2 <span style="color: #339933;">/*</span> r0 ← &amp;message <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> printf                 <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> printf <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>              <span style="color: #339933;">/*</span> restore r4 <span style="color: #00007f; font-weight: bold;">and</span> lr <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr                     <span style="color: #339933;">/*</span> return to the caller <span style="color: #339933;">*/</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> ensure the next label is <span style="color: #ff0000;">4</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
addr_of_message_2<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> message_2
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
greeter<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>            <span style="color: #339933;">/*</span> keep lr because we <span style="color: #00007f; font-weight: bold;">call</span> printf<span style="color: #339933;">,</span> 
                                we keep r4 to keep the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #ff0000;">8</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span>
                                aligned<span style="color: #339933;">,</span> as per AAPCS requirements <span style="color: #339933;">*/</span>
    blx r0                   <span style="color: #339933;">/*</span> indirect <span style="color: #00007f; font-weight: bold;">call</span> to r0 <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>             <span style="color: #339933;">/*</span> restore r4 <span style="color: #00007f; font-weight: bold;">and</span> lr <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr                    <span style="color: #339933;">/*</span> return to the caller <span style="color: #339933;">*/</span>
&nbsp;
<span style="color: #339933;">.</span>globl main <span style="color: #339933;">/*</span> state that <span style="color: #7f007f;">'main'</span> label is <span style="color: #0000ff; font-weight: bold;">global</span> <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> ensure the next label is <span style="color: #ff0000;">4</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
main<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>            <span style="color: #339933;">/*</span> keep lr because we <span style="color: #00007f; font-weight: bold;">call</span> printf<span style="color: #339933;">,</span> 
                                we keep r4 to keep the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #ff0000;">8</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span>
                                aligned<span style="color: #339933;">,</span> as per AAPCS requirements <span style="color: #339933;">*/</span>
&nbsp;
    ldr r0<span style="color: #339933;">,</span> addr_say_hello   <span style="color: #339933;">/*</span> r0 ← &amp;say_hello <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> greeter               <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> greeter <span style="color: #339933;">*/</span>
&nbsp;
    ldr r0<span style="color: #339933;">,</span> addr_say_bonjour <span style="color: #339933;">/*</span> r0 ← &amp;say_bonjour <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> greeter               <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> greeter <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>               <span style="color: #339933;">/*</span> return from the program<span style="color: #339933;">,</span> set error <span style="color: #0000ff; font-weight: bold;">code</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>             <span style="color: #339933;">/*</span> restore r4 <span style="color: #00007f; font-weight: bold;">and</span> lr <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr                    <span style="color: #339933;">/*</span> return to the caller <span style="color: #009900; font-weight: bold;">(</span>the system<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
&nbsp;
addr_say_hello <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> say_hello
addr_say_bonjour <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> say_bonjour</pre></td></tr></tbody></table><p class="theCode" style="display:none;">.data     /* data section */
.align 4  /* ensure the next label is 4-byte aligned */
message_1: .asciz "Hello\n"
.align 4  /* ensure the next label is 4-byte aligned */
message_2: .asciz "Bonjour\n"

.text     /* text section (= code) */

.align 4  /* ensure the next label is 4-byte aligned */
say_hello:
    push {r4, lr}            /* keep lr because we call printf, 
                                we keep r4 to keep the stack 8-byte
                                aligned, as per AAPCS requirements */
    /* Prepare the call to printf */
    ldr r0, addr_of_message_1 /* r0 ← &amp;message */
    bl printf                 /* call printf */
    pop {r4, lr}              /* restore r4 and lr */
    bx lr                     /* return to the caller */

.align 4  /* ensure the next label is 4-byte aligned */
addr_of_message_1: .word message_1

.align 4  /* ensure the next label is 4-byte aligned */
say_bonjour:
    push {r4, lr}            /* keep lr because we call printf, 
                                we keep r4 to keep the stack 8-byte
                                aligned, as per AAPCS requirements */
    /* Prepare the call to printf */
    ldr r0, addr_of_message_2 /* r0 ← &amp;message */
    bl printf                 /* call printf */
    pop {r4, lr}              /* restore r4 and lr */
    bx lr                     /* return to the caller */

.align 4  /* ensure the next label is 4-byte aligned */
addr_of_message_2: .word message_2

.align 4
greeter:
    push {r4, lr}            /* keep lr because we call printf, 
                                we keep r4 to keep the stack 8-byte
                                aligned, as per AAPCS requirements */
    blx r0                   /* indirect call to r0 */
    pop {r4, lr}             /* restore r4 and lr */
    bx lr                    /* return to the caller */

.globl main /* state that 'main' label is global */
.align 4  /* ensure the next label is 4-byte aligned */
main:
    push {r4, lr}            /* keep lr because we call printf, 
                                we keep r4 to keep the stack 8-byte
                                aligned, as per AAPCS requirements */

    ldr r0, addr_say_hello   /* r0 ← &amp;say_hello */
    bl greeter               /* call greeter */

    ldr r0, addr_say_bonjour /* r0 ← &amp;say_bonjour */
    bl greeter               /* call greeter */

    mov r0, #0               /* return from the program, set error code */
    pop {r4, lr}             /* restore r4 and lr */
    bx lr                    /* return to the caller (the system) */

addr_say_hello : .word say_hello
addr_say_bonjour : .word say_bonjour</p></div>

<p>
If we run it
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="shell" style="font-family:monospace;">$ ./greeter_01 
Hello
Bonjour</pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ ./greeter_01 
Hello
Bonjour</p></div>

<p>
You are probably not impressed by the output of this previous program. So let’s try to make it more interesting: we will greet people generically, some people will be greeted in English and some other will be greeted in French.
</p>
<p>
Let’s start defining a bunch of data that we will require for this example. First greeting messages in English and French. Note that we will greet the person by name, so we will use a printf format string.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>1
2
3
4
5
6
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #0000ff; font-weight: bold;">.data</span>     <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">data</span> <span style="color: #0000ff; font-weight: bold;">section</span> <span style="color: #339933;">*/</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> ensure the next label is <span style="color: #ff0000;">4</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
message_hello<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"Hello %s\n"</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> ensure the next label is <span style="color: #ff0000;">4</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
message_bonjour<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"Bonjour %s\n"</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">.data     /* data section */

.align 4  /* ensure the next label is 4-byte aligned */
message_hello: .asciz "Hello %s\n"
.align 4  /* ensure the next label is 4-byte aligned */
message_bonjour: .asciz "Bonjour %s\n"</p></div>

<p>
Next we will define some <em>tags</em> that we will use to tag people as English or French. This tag will contain the address to the specific greeting function. The English tag will have the address of <code>say_hello</code> and the French tag will have the address of <code>say_bonjour</code>.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>7
8
9
10
11
12
13
14
15
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> tags of kind of people <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> ensure the next label is <span style="color: #ff0000;">4</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
person_english <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> say_hello <span style="color: #339933;">/*</span> tag for people
                                     that will be greeted 
                                     <span style="color: #00007f; font-weight: bold;">in</span> English <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> ensure the next label is <span style="color: #ff0000;">4</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
person_french <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> say_bonjour <span style="color: #339933;">/*</span> tag for people
                                     that will be greeted 
                                     <span style="color: #00007f; font-weight: bold;">in</span> French <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* tags of kind of people */
.align 4  /* ensure the next label is 4-byte aligned */
person_english : .word say_hello /* tag for people
                                     that will be greeted 
                                     in English */
.align 4  /* ensure the next label is 4-byte aligned */
person_french : .word say_bonjour /* tag for people
                                     that will be greeted 
                                     in French */</p></div>

<p>
Let’s define some names that we will use later, when defining people.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>18
19
20
21
22
23
24
25
26
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> several names to be used <span style="color: #00007f; font-weight: bold;">in</span> the people definition <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
name_pierre<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"Pierre"</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
name_john<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"John"</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
name_sally<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"Sally"</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
name_bernadette<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"Bernadette"</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* several names to be used in the people definition */
.align 4
name_pierre: .asciz "Pierre"
.align 4
name_john: .asciz "John"
.align 4
name_sally: .asciz "Sally"
.align 4
name_bernadette: .asciz "Bernadette"</p></div>

<p>
And now define some people. Every person is actually a pair formed by an address to their name and an address to their tag.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>28
29
30
31
32
33
34
35
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
person_john<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> name_john<span style="color: #339933;">,</span> person_english
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
person_pierre<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> name_pierre<span style="color: #339933;">,</span> person_french
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
person_sally<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> name_sally<span style="color: #339933;">,</span> person_english
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
person_bernadette<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> name_bernadette<span style="color: #339933;">,</span> person_french</pre></td></tr></tbody></table><p class="theCode" style="display:none;">.align 4
person_john: .word name_john, person_english
.align 4
person_pierre: .word name_pierre, person_french
.align 4
person_sally: .word name_sally, person_english
.align 4
person_bernadette: .word name_bernadette, person_french</p></div>

<p>
Finally let’s group every person in an array. The array contains addresses to each people (not the people themselves).
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>38
39
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> array of people <span style="color: #339933;">*/</span>
people <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> person_john<span style="color: #339933;">,</span> person_pierre<span style="color: #339933;">,</span> person_sally<span style="color: #339933;">,</span> person_bernadette</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* array of people */
people : .word person_john, person_pierre, person_sally, person_bernadette</p></div>

<p>
Now let’s define the code. These are the two specific functions for each language (English and French). Note that we already named their labels in the tags above.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>41
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #0000ff; font-weight: bold;">.text</span>     <span style="color: #339933;">/*</span> text <span style="color: #0000ff; font-weight: bold;">section</span> <span style="color: #009900; font-weight: bold;">(</span>= <span style="color: #0000ff; font-weight: bold;">code</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> ensure the next label is <span style="color: #ff0000;">4</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
say_hello<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>            <span style="color: #339933;">/*</span> keep lr because we <span style="color: #00007f; font-weight: bold;">call</span> printf<span style="color: #339933;">,</span> 
                                we keep r4 to keep the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #ff0000;">8</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span>
                                aligned<span style="color: #339933;">,</span> as per AAPCS requirements <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> Prepare the <span style="color: #00007f; font-weight: bold;">call</span> to printf <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> r0               <span style="color: #339933;">/*</span> r1 ← r0 <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> addr_of_message_hello
                             <span style="color: #339933;">/*</span> r0 ← &amp;message_hello <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> printf                <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> printf <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>             <span style="color: #339933;">/*</span> restore r4 <span style="color: #00007f; font-weight: bold;">and</span> lr <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr                    <span style="color: #339933;">/*</span> return to the caller <span style="color: #339933;">*/</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> ensure the next label is <span style="color: #ff0000;">4</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
addr_of_message_hello<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> message_hello
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> ensure the next label is <span style="color: #ff0000;">4</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
say_bonjour<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>            <span style="color: #339933;">/*</span> keep lr because we <span style="color: #00007f; font-weight: bold;">call</span> printf<span style="color: #339933;">,</span> 
                                we keep r4 to keep the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #ff0000;">8</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span>
                                aligned<span style="color: #339933;">,</span> as per AAPCS requirements <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> Prepare the <span style="color: #00007f; font-weight: bold;">call</span> to printf <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> r0               <span style="color: #339933;">/*</span> r1 ← r0 <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> addr_of_message_bonjour
                             <span style="color: #339933;">/*</span> r0 ← &amp;message_bonjour <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> printf                <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> printf <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>             <span style="color: #339933;">/*</span> restore r4 <span style="color: #00007f; font-weight: bold;">and</span> lr <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr                    <span style="color: #339933;">/*</span> return to the caller <span style="color: #339933;">*/</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> ensure the next label is <span style="color: #ff0000;">4</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
addr_of_message_bonjour<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> message_bonjour</pre></td></tr></tbody></table><p class="theCode" style="display:none;">.text     /* text section (= code) */

.align 4  /* ensure the next label is 4-byte aligned */
say_hello:
    push {r4, lr}            /* keep lr because we call printf, 
                                we keep r4 to keep the stack 8-byte
                                aligned, as per AAPCS requirements */
    /* Prepare the call to printf */
    mov r1, r0               /* r1 ← r0 */
    ldr r0, addr_of_message_hello
                             /* r0 ← &amp;message_hello */
    bl printf                /* call printf */
    pop {r4, lr}             /* restore r4 and lr */
    bx lr                    /* return to the caller */

.align 4  /* ensure the next label is 4-byte aligned */
addr_of_message_hello: .word message_hello

.align 4  /* ensure the next label is 4-byte aligned */
say_bonjour:
    push {r4, lr}            /* keep lr because we call printf, 
                                we keep r4 to keep the stack 8-byte
                                aligned, as per AAPCS requirements */
    /* Prepare the call to printf */
    mov r1, r0               /* r1 ← r0 */
    ldr r0, addr_of_message_bonjour
                             /* r0 ← &amp;message_bonjour */
    bl printf                /* call printf */
    pop {r4, lr}             /* restore r4 and lr */
    bx lr                    /* return to the caller */

.align 4  /* ensure the next label is 4-byte aligned */
addr_of_message_bonjour: .word message_bonjour</p></div>

<p>
Before we go to the interesting function, let’s define the <code>main</code> function.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>99
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">.</span>globl main <span style="color: #339933;">/*</span> state that <span style="color: #7f007f;">'main'</span> label is <span style="color: #0000ff; font-weight: bold;">global</span> <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> ensure the next label is <span style="color: #ff0000;">4</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
main<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>    <span style="color: #339933;">/*</span> keep callee saved registers that we will modify <span style="color: #339933;">*/</span>
&nbsp;
    ldr r4<span style="color: #339933;">,</span> addr_of_people   <span style="color: #339933;">/*</span> r4 ← &amp;people <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> recall that people is an array of addresses <span style="color: #009900; font-weight: bold;">(</span>pointers<span style="color: #009900; font-weight: bold;">)</span> to people <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> now we <span style="color: #00007f; font-weight: bold;">loop</span> from <span style="color: #ff0000;">0</span> to <span style="color: #ff0000;">4</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>               <span style="color: #339933;">/*</span> r5 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    b check_loop             <span style="color: #339933;">/*</span> branch to the <span style="color: #00007f; font-weight: bold;">loop</span> check <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">loop</span><span style="color: #339933;">:</span>
      <span style="color: #339933;">/*</span> prepare the <span style="color: #00007f; font-weight: bold;">call</span> to greet_person <span style="color: #339933;">*/</span>
      ldr r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">]</span>  <span style="color: #339933;">/*</span> r0 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r4 <span style="color: #339933;">+</span> r5 &lt;&lt; <span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">)</span>   this is
                                   r0 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r4 <span style="color: #339933;">+</span> r5 <span style="color: #339933;">*</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span>
                                   recall<span style="color: #339933;">,</span> people is an array of addresses<span style="color: #339933;">,</span>
                                   so this is
                                   r0 ← people<span style="color: #009900; font-weight: bold;">[</span>r5<span style="color: #009900; font-weight: bold;">]</span>
                                <span style="color: #339933;">*/</span>
      <span style="color: #46aa03; font-weight: bold;">bl</span> greet_person           <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> greet_person <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">add</span> r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>            <span style="color: #339933;">/*</span> r5 ← r5 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    check_loop<span style="color: #339933;">:</span>
      <span style="color: #00007f; font-weight: bold;">cmp</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>                <span style="color: #339933;">/*</span> compute r5 <span style="color: #339933;">-</span> <span style="color: #ff0000;">4</span> <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
      bne <span style="color: #00007f; font-weight: bold;">loop</span>                  <span style="color: #339933;">/*</span> if r5 != <span style="color: #ff0000;">4</span> branch to <span style="color: #00007f; font-weight: bold;">loop</span> <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>               <span style="color: #339933;">/*</span> return from the program<span style="color: #339933;">,</span> set error <span style="color: #0000ff; font-weight: bold;">code</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>     <span style="color: #339933;">/*</span> callee saved registers <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr                    <span style="color: #339933;">/*</span> return to the caller <span style="color: #009900; font-weight: bold;">(</span>the system<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
&nbsp;
addr_of_people <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> people</pre></td></tr></tbody></table><p class="theCode" style="display:none;">.globl main /* state that 'main' label is global */
.align 4  /* ensure the next label is 4-byte aligned */
main:
    push {r4, r5, r6, lr}    /* keep callee saved registers that we will modify */

    ldr r4, addr_of_people   /* r4 ← &amp;people */
    /* recall that people is an array of addresses (pointers) to people */

    /* now we loop from 0 to 4 */
    mov r5, #0               /* r5 ← 0 */
    b check_loop             /* branch to the loop check */

    loop:
      /* prepare the call to greet_person */
      ldr r0, [r4, r5, LSL #2]  /* r0 ← *(r4 + r5 &lt;&lt; 2)   this is
                                   r0 ← *(r4 + r5 * 4)
                                   recall, people is an array of addresses,
                                   so this is
                                   r0 ← people[r5]
                                */
      bl greet_person           /* call greet_person */
      add r5, r5, #1            /* r5 ← r5 + 1 */
    check_loop:
      cmp r5, #4                /* compute r5 - 4 and update cpsr */
      bne loop                  /* if r5 != 4 branch to loop */

    mov r0, #0               /* return from the program, set error code */
    pop {r4, r5, r6, lr}     /* callee saved registers */
    bx lr                    /* return to the caller (the system) */

addr_of_people : .word people</p></div>

<p>
As you can see, what we do here is to load elements 0 to 3 of the <code>people</code> array and call the function <code>greet_person</code>. Every element in <code>people</code> array is a pointer, so we can put them in a register, in this case <code>r0</code> because it will be the first parameter of <code>greet_person</code>.
</p>
<p>
Let’s see now the code for the function <code>greet_person</code>.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>75
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> This function receives an address to a person <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
greet_person<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>            <span style="color: #339933;">/*</span> keep lr because we <span style="color: #00007f; font-weight: bold;">call</span> printf<span style="color: #339933;">,</span> 
                                we keep r4 to keep the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #ff0000;">8</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span>
                                aligned<span style="color: #339933;">,</span> as per AAPCS requirements <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> prepare indirect function <span style="color: #00007f; font-weight: bold;">call</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> r0               <span style="color: #339933;">/*</span> r0 ← r4<span style="color: #339933;">,</span> keep the first parameter <span style="color: #00007f; font-weight: bold;">in</span> r4 <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #009900; font-weight: bold;">]</span>             <span style="color: #339933;">/*</span> r0 ← <span style="color: #339933;">*</span>r4<span style="color: #339933;">,</span> this is the address to the name
                                of the person <span style="color: #00007f; font-weight: bold;">and</span> the first parameter
                                of the indirect called function<span style="color: #339933;">*/</span>
&nbsp;
    ldr r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>         <span style="color: #339933;">/*</span> r1 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span> this is the address
                                to the person tag <span style="color: #339933;">*/</span>
    ldr r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #009900; font-weight: bold;">]</span>             <span style="color: #339933;">/*</span> r1 ← <span style="color: #339933;">*</span>r1<span style="color: #339933;">,</span> the address of the
                                specific greeting function <span style="color: #339933;">*/</span>
&nbsp;
    blx r1                   <span style="color: #339933;">/*</span> indirect <span style="color: #00007f; font-weight: bold;">call</span> to r1<span style="color: #339933;">,</span> this is
                                the specific greeting function <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>             <span style="color: #339933;">/*</span> restore r4 <span style="color: #00007f; font-weight: bold;">and</span> lr <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr                    <span style="color: #339933;">/*</span> return to the caller <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* This function receives an address to a person */
.align 4
greet_person:
    push {r4, lr}            /* keep lr because we call printf, 
                                we keep r4 to keep the stack 8-byte
                                aligned, as per AAPCS requirements */

    /* prepare indirect function call */
    mov r4, r0               /* r0 ← r4, keep the first parameter in r4 */
    ldr r0, [r4]             /* r0 ← *r4, this is the address to the name
                                of the person and the first parameter
                                of the indirect called function*/

    ldr r1, [r4, #4]         /* r1 ← *(r4 + 4) this is the address
                                to the person tag */
    ldr r1, [r1]             /* r1 ← *r1, the address of the
                                specific greeting function */

    blx r1                   /* indirect call to r1, this is
                                the specific greeting function */

    pop {r4, lr}             /* restore r4 and lr */
    bx lr                    /* return to the caller */</p></div>

<p>
In register <code>r0</code> we have the address of a person. We move it to <code>r4</code> for convenience as <code>r0</code> will be used for the indirectly called function. Then we load the name of the person, found in <code>[r4]</code>, this is <code>[r4, #0]</code> (this is <code>*(r4 + 0)</code>, so <code>*r4</code>) into <code>r0</code>. Then we load the person tag, found 4 bytes after the name (remember that the name of the person is an address, so it takes 4 bytes in ARM). The tag itself is not very useful except because it allows us to get the specific greeting function (either <code>say_hello</code> or <code>say_bonjour</code>). So we load <code>[r4, #4]</code>, the address of the tag, in <code>r1</code>. Ok, now <code>r1</code> contains the address of the tag and we know that the first 4 bytes of a tag contain the specific greeting function.
</p>
<p>
If we run this program the output is:
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="shell" style="font-family:monospace;">$ ./greeter_02 
Hello John
Bonjour Pierre
Hello Sally
Bonjour Bernadette</pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ ./greeter_02 
Hello John
Bonjour Pierre
Hello Sally
Bonjour Bernadette</p></div>

<h3>Late binding and object orientation</h3>
<p>
In the last example we have implemented, in a very simple way, a feature of the object-oriented programming (OOP) called <em>late binding</em>, which means that one does not know which function is called for a given object.
</p>
<p>
In our example the objects are of <em>kind</em> <code>Person</code>. Every <code>Person</code> can be greeted, this is what <code>greet_person</code> does. We do not have objects of kind <code>Person</code> really, but <code>EnglishPerson</code> and <code>FrenchPerson</code>. When you greet an <code>EnglishPerson</code> you expect to greet him/her with <q>Hello</q>, when you greet a <code>FrenchPerson</code> you expect to greet him/her with <q>Bonjour</q>.
</p>
<p>
If you know C++ (or Java), you’ll quickly realize that our last example actually implements something like this.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="cpp" style="font-family:monospace;"><span style="color: #0000ff;">struct</span> Person
<span style="color: #008000;">{</span>
  <span style="color: #0000ff;">const</span> <span style="color: #0000ff;">char</span><span style="color: #000040;">*</span> name<span style="color: #008080;">;</span>
  <span style="color: #0000ff;">virtual</span> <span style="color: #0000ff;">void</span> greet<span style="color: #008000;">(</span><span style="color: #008000;">)</span> <span style="color: #000080;">=</span> <span style="color: #0000dd;">0</span><span style="color: #008080;">;</span>
<span style="color: #008000;">}</span><span style="color: #008080;">;</span>
&nbsp;
<span style="color: #0000ff;">struct</span> EnglishPerson <span style="color: #008080;">:</span> Person
<span style="color: #008000;">{</span>
  <span style="color: #0000ff;">virtual</span> <span style="color: #0000ff;">void</span> greet<span style="color: #008000;">(</span><span style="color: #008000;">)</span>
  <span style="color: #008000;">{</span>
    <span style="color: #0000dd;">printf</span><span style="color: #008000;">(</span><span style="color: #FF0000;">"Hello %s<span style="color: #000099; font-weight: bold;">\n</span>"</span>, this<span style="color: #000040;">-</span><span style="color: #000080;">&gt;</span>name<span style="color: #008000;">)</span><span style="color: #008080;">;</span>
  <span style="color: #008000;">}</span>
<span style="color: #008000;">}</span><span style="color: #008080;">;</span>
&nbsp;
<span style="color: #0000ff;">struct</span> FrenchPerson <span style="color: #008080;">:</span> Person
<span style="color: #008000;">{</span>
  <span style="color: #0000ff;">virtual</span> <span style="color: #0000ff;">void</span> greet<span style="color: #008000;">(</span><span style="color: #008000;">)</span>
  <span style="color: #008000;">{</span>
    <span style="color: #0000dd;">printf</span><span style="color: #008000;">(</span><span style="color: #FF0000;">"Bonjour %s<span style="color: #000099; font-weight: bold;">\n</span>"</span>, this<span style="color: #000040;">-</span><span style="color: #000080;">&gt;</span>name<span style="color: #008000;">)</span><span style="color: #008080;">;</span>
  <span style="color: #008000;">}</span>
<span style="color: #008000;">}</span><span style="color: #008080;">;</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">struct Person
{
  const char* name;
  virtual void greet() = 0;
};

struct EnglishPerson : Person
{
  virtual void greet()
  {
    printf("Hello %s\n", this-&gt;name);
  }
};

struct FrenchPerson : Person
{
  virtual void greet()
  {
    printf("Bonjour %s\n", this-&gt;name);
  }
};</p></div>

<p>
In the snippet above, <code>this</code> is the <code>Person</code> we passed to our function <code>greet_person</code>. That parameter allowed us to retrieve the name of the person (<code>this-&gt;name</code>) and the specific version of <code>greet</code> we wanted.
</p>
<p>
I hope that this last example, albeit a bit long, actually shows you the power of indirect calls.
</p>
<p>
This is all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2014/08/20/arm-assembler-raspberry-pi-chapter-20/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2014/08/20/arm-assembler-raspberry-pi-chapter-20/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2014/08/20/arm-assembler-raspberry-pi-chapter-20/&amp;text=ARM+assembler+in+Raspberry+Pi+%E2%80%93+Chapter+20+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2014/08/20/arm-assembler-raspberry-pi-chapter-20/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>						<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2014/06/09/tinymce-checkbox-toggler-jquery/" rel="prev">TinyMCE checkbox toggler for jQuery</a></span>
			<span class="next"><a href="http://thinkingeek.com/2014/08/23/arm-assembler-raspberry-pi-chapter-21/" rel="next">ARM assembler in Raspberry Pi – Chapter 21</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
			<h3>5 thoughts on “<span>ARM assembler in Raspberry Pi – Chapter 20</span>”</h3>
		<ul class="commentlist">
				<li class="comment even thread-even depth-1" id="comment-218045">
				<div id="div-comment-218045" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-9e81362ff9654533accffc53762c9520-0">			<cite class="fn">Fernando</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/08/20/arm-assembler-raspberry-pi-chapter-20/#comment-218045">
			August 20, 2014 at 6:23 pm</a>		</div>

		<p>Nice, as always!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/08/20/arm-assembler-raspberry-pi-chapter-20/?replytocom=218045#respond" onclick="return addComment.moveForm( &quot;div-comment-218045&quot;, &quot;218045&quot;, &quot;respond&quot;, &quot;1571&quot; )" aria-label="Reply to Fernando">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1 parent" id="comment-972174">
				<div id="div-comment-972174" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/5fbdce467f67547bd29c50a8bc544d43?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/5fbdce467f67547bd29c50a8bc544d43?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-5fbdce467f67547bd29c50a8bc544d43-0" originals="32" src-orig="http://2.gravatar.com/avatar/5fbdce467f67547bd29c50a8bc544d43?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Bill</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/08/20/arm-assembler-raspberry-pi-chapter-20/#comment-972174">
			August 1, 2016 at 1:05 am</a>		</div>

		<p>Hi,<br>
where can I find addr_of_people? Suppose it should be people : .word person_john, person_pierre, person_sally, person_bernadette</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/08/20/arm-assembler-raspberry-pi-chapter-20/?replytocom=972174#respond" onclick="return addComment.moveForm( &quot;div-comment-972174&quot;, &quot;972174&quot;, &quot;respond&quot;, &quot;1571&quot; )" aria-label="Reply to Bill">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2" id="comment-972411">
				<div id="div-comment-972411" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-0">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/08/20/arm-assembler-raspberry-pi-chapter-20/#comment-972411">
			August 4, 2016 at 7:01 pm</a>		</div>

		<p>Hi Bill,</p>
<p>you are right it is missing. I fixed the post.</p>
<p>It should be <code>addr_of_people: .word people</code>.</p>
<p>Thank you!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/08/20/arm-assembler-raspberry-pi-chapter-20/?replytocom=972411#respond" onclick="return addComment.moveForm( &quot;div-comment-972411&quot;, &quot;972411&quot;, &quot;respond&quot;, &quot;1571&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-even depth-1 parent" id="comment-980190">
				<div id="div-comment-980190" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/190c9ec3a3c6ccec4bcee318a29e4ef1?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/190c9ec3a3c6ccec4bcee318a29e4ef1?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-190c9ec3a3c6ccec4bcee318a29e4ef1-0" originals="32" src-orig="http://1.gravatar.com/avatar/190c9ec3a3c6ccec4bcee318a29e4ef1?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Grant</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/08/20/arm-assembler-raspberry-pi-chapter-20/#comment-980190">
			November 24, 2016 at 1:34 am</a>		</div>

		<p>Creating a struct in the data area is great but if you wanted to have a struct not accessible like that are you forced to use the stack (so essentially a local variable)? So you would have to recreate the struct format every time you wanted to create one in a new function right?</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/08/20/arm-assembler-raspberry-pi-chapter-20/?replytocom=980190#respond" onclick="return addComment.moveForm( &quot;div-comment-980190&quot;, &quot;980190&quot;, &quot;respond&quot;, &quot;1571&quot; )" aria-label="Reply to Grant">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2" id="comment-980594">
				<div id="div-comment-980594" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-1">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/08/20/arm-assembler-raspberry-pi-chapter-20/#comment-980594">
			November 27, 2016 at 1:11 pm</a>		</div>

		<p>Hi Grant,</p>
<p>basically yes. This is how C++ and any other OOP programming language that allows objects be stored in the stack work. A special function is usually invoked as part of the language semantics to set everything up. It is commonly called a <em>constructor</em>.</p>
<p>Note though, that the example in this post is extremely naive. OOP programming languages do not keep every pointer of every virtual function they have in the object directly. Instead, they keep those pointers in another structure, usually called the <em>virtual table</em>, in a global variable. There is one virtual table per class (i.e. <code>Person</code>, <code>EnglishPerson</code> and <code>FrenchPerson</code> in the example). The objects (i.e. <code>john</code>, <code>pierre</code>, <code>sally</code> and <code>bernadette</code>) only need a pointer to the virtual table. So the initialization process for this part is as easy as setting up the proper pointer.</p>
<p>Hope this answers your question.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/08/20/arm-assembler-raspberry-pi-chapter-20/?replytocom=980594#respond" onclick="return addComment.moveForm( &quot;div-comment-980594&quot;, &quot;980594&quot;, &quot;respond&quot;, &quot;1571&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		</ul>
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2014/08/20/arm-assembler-raspberry-pi-chapter-20/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="1571" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="a1578d11c2"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496666656253"></form>
			</div><!-- #respond -->
	</div>	</div>