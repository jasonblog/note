# Functions (II). The stack


<div class="main">
			<h1>ARM assembler in Raspberry Pi – Chapter 10 </h1>
		<p class="post-meta"><span class="date">February 7, 2013</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/#comments">17</a></span> </p>
		<p>
In chapter 9 we were introduced to functions and we saw that they have to follow a number of conventions in order to play nice with other functions. We also briefly mentioned the stack, as an area of memory owned solely by the function. In this chapter we will go in depth with the stack and why it is important for functions.
</p>
<p><span id="more-669"></span></p>
<h2>Dynamic activation</h2>
<p>
One of the benefits of functions is being able to call them more than once. But that <em>more than once</em> hides a small trap. We are not restricting who will be able to call the function, so it might happen that it is the same function who calls itself. This happens when we use recursion.
</p>
<p>
A typical example of recursion is the factorial of a number <em>n</em>, usually written as <em>n!</em>. A factorial in C can be written as follows.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #993333;">int</span> factorial<span style="color: #009900;">(</span><span style="color: #993333;">int</span> n<span style="color: #009900;">)</span>
<span style="color: #009900;">{</span>
   <span style="color: #b1b100;">if</span> <span style="color: #009900;">(</span>n <span style="color: #339933;">==</span> <span style="color: #0000dd;">0</span><span style="color: #009900;">)</span>
      <span style="color: #b1b100;">return</span> <span style="color: #0000dd;">1</span><span style="color: #339933;">;</span>
   <span style="color: #b1b100;">else</span>
      <span style="color: #b1b100;">return</span> n <span style="color: #339933;">*</span> factorial<span style="color: #009900;">(</span>n<span style="color: #339933;">-</span><span style="color: #0000dd;">1</span><span style="color: #009900;">)</span><span style="color: #339933;">;</span>
<span style="color: #009900;">}</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">int factorial(int n)
{
   if (n == 0)
      return 1;
   else
      return n * factorial(n-1);
}</p></div>

<p>
Note that there is only one function <code>factorial</code>, but it may be called several times. For instance: <em>factorial(3) → factorial(2) → factorial(1) → factorial(0)</em>, where → means a «it calls». A function, thus, is <em>dynamically activated</em> each time is called. The span of a dynamic activation goes from the point where the function is called until it returns. At a given time, more than one function is dynamically activated. The whole dynamic activation set of functions includes the current function and the dynamic activation set of the function that called it (the current function).
</p>
<p>
Ok. We have a function that calls itself. No big deal, right? Well, this would not be a problem if it weren’t for the rules that a function must observe. Let’s quickly recall them.
</p>
<ul>
<li>Only <code>r0</code>, <code>r1</code>, <code>r2</code> and <code>r3</code> can be freely modified.
</li><li><code>lr</code> value at the entry of the function must be kept somewhere because we will need it to leave the function (to return to the caller).
</li><li>All other registers <code>r4</code> to <code>r11</code> and <code>sp</code> can be modified but they must be restored to their original values upon leaving the function.
</li></ul>
<p>
In chapter 9 we used a global variable to keep <code>lr</code>. But if we attempted to use a global variable in our <em>factorial(3)</em> example, it would be overwritten at the next dynamic activation of factorial. We would only be able to return from <em>factorial(0)</em> to <em>factorial(1)</em>. After that we would be stuck in <em>factorial(1)</em>, as <code>lr</code> would always have the same value.
</p>
<p>
So it looks like we need some way to keep at least the value of <code>lr</code> <strong>per each dynamic activation</strong>. And not only <code>lr</code>, if we wanted to use registers from <code>r4</code> to <code>r11</code> we also need to keep somehow per each dynamic activation, a global variable would not be enough either. This is where the stack comes into play.
</p>
<h2>The stack</h2>
<p>
In computing, a stack is a data structure (a way to organize data that provides some interesting properties). A stack typically has three operations: access the top of the stack, push onto the top, pop from the top. Dependening on the context you can only access the top of the stack, in our case we will be able to access more elements than just the top.
</p>
<p>
But, what is the stack? I already said in chaper 9 that the stack is a region of memory owned solely by the function. We can now reword this a bit better: the stack is a region of memory owned solely by the current dynamic activation. And how we control the stack? Well, in chapter 9 we said that the register <code>sp</code> stands for <em><strong>s</strong>tack <strong>p</strong>ointer</em>. This register will contain the top of the stack. The region of memory owned by the dynamic activation is the extent of bytes contained between the current value of <code>sp</code> and the initial value that <code>sp</code> had at the beginning of the function. We will call that region the <strong>local memory</strong> of a function (more precisely, of a dynamic activation of it). We will put there whatever has to be saved at the beginning of a function and restored before leaving. We will also keep there the <strong>local variables</strong> of a function (dynamic activation).
</p>
<p>
Our function also has to adhere to some rules when handling the stack.
</p>
<ul>
<li>The stack pointer (<code>sp</code>) is always 4 byte aligned. This is absolutely mandatory. However, due to the Procedure Call Standard for the ARM architecture (AAPCS), the stack pointer will have to be 8 byte aligned, otherwise funny things may happen when we call what the AAPCS calls as <em>public interfaces</em> (this is, code written by other people).
</li><li>The value of <code>sp</code> when leaving the function should be the same value it had upon entering the function.
</li></ul>
<p>
The first rule is consistent with the alignment constraints of ARM, where most of times addresses must be 4 byte aligned. Due to AAPCS we will stick to the extra 8 byte alignment constraint. The second rule states that, no matter how large is our local memory, it will always disappear at the end of the function. This is important, because local variables of a dynamic activation need not have any storage after that dynamic activation ends.
</p>
<p>
It is a convention how the stack, and thus the local memory, has its size defined. The stack can grow upwards or downwards. If it grows upwards it means that we have to increase the value of the <code>sp</code> register in order to enlarge the local memory. If it grows downwards we have to do the opposite, the value of the <code>sp</code> register must be subtracted as many bytes as the size of the local storage. In Linux ARM, the stack grows downwards, towards zero (although it never should reach zero). Addresses of local variables have very large values in the 32 bit range. They are usually close to 2<sup>32</sup>.
</p>
<p>
Another convention when using the stack concerns whether the <code>sp</code> register contains the address of the top of the stack or some bytes above. In Linux ARM the <code>sp</code> register directly points to the top of the stack: in the memory addressed by <code>sp</code> there is useful information.
</p>
<p>
Ok, we know the stack grows downwards and the top of the stack must always be in <code>sp</code>. So to enlarge the local memory it should be enough by decreasing <code>sp</code>. The local memory is then defined by the range of memory from the current <code>sp</code> value to the original value that <code>sp</code> had at the beginning of the function. One register we almost always have to keep is <code>lr</code>. Let’s see how can we keep in the stack.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">sub</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span>  <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">sp</span> ← <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">-</span> <span style="color: #ff0000;">8</span><span style="color: #339933;">.</span> This enlarges the <span style="color: #0000ff; font-weight: bold;">stack</span> by <span style="color: #ff0000;">8</span> bytes <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">str</span> lr<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #009900; font-weight: bold;">]</span>    <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span><span style="color: #46aa03; font-weight: bold;">sp</span> ← lr <span style="color: #339933;">*/</span>
<span style="color: #339933;">...</span> <span style="color: #339933;">//</span> <span style="color: #0000ff; font-weight: bold;">Code</span> of the function
ldr lr<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #009900; font-weight: bold;">]</span>    <span style="color: #339933;">/*</span> lr ← <span style="color: #339933;">*</span><span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">add</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span>  <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">sp</span> ← <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span><span style="color: #339933;">.</span> <span style="color: #339933;">/*</span> This reduces the <span style="color: #0000ff; font-weight: bold;">stack</span> by <span style="color: #ff0000;">8</span> bytes
                                effectively restoring the <span style="color: #0000ff; font-weight: bold;">stack</span> 
                                pointer to its original value <span style="color: #339933;">*/</span>
<span style="color: #46aa03; font-weight: bold;">bx</span> lr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">sub sp, sp, #8  /* sp ← sp - 8. This enlarges the stack by 8 bytes */
str lr, [sp]    /* *sp ← lr */
... // Code of the function
ldr lr, [sp]    /* lr ← *sp */
add sp, sp, #8  /* sp ← sp + 8. /* This reduces the stack by 8 bytes
                                effectively restoring the stack 
                                pointer to its original value */
bx lr</p></div>

<p>
A well behaved function may modify sp but must ensure that at the end it has the same value it had when we entered the function. This is what we do here. We first subtract 8 bytes to sp and at the end we add back 8 bytes.
</p>
<p>
This sequence of instructions would do indeed. But maybe you remember chapter 8 and the indexing modes that you could use in load and store. Note that the first two instructions behave exactly like a preindexing. We first update <code>sp</code> and then we use <code>sp</code> as the address where we store <code>lr</code>. This is exactly a preindex! Likewise for the last two instructions. We first load <code>lr</code> using the current address of <code>sp</code> and then we decrease <code>sp</code>. This is exactly a postindex!
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">str</span> lr<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span>!  <span style="color: #339933;">/*</span> preindex<span style="color: #339933;">:</span> <span style="color: #46aa03; font-weight: bold;">sp</span> ← <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">-</span> <span style="color: #ff0000;">8</span><span style="color: #666666; font-style: italic;">; *sp ← lr */</span>
<span style="color: #339933;">...</span> <span style="color: #339933;">//</span> <span style="color: #0000ff; font-weight: bold;">Code</span> of the function
ldr lr<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> #<span style="color: #339933;">+</span><span style="color: #ff0000;">8</span>   <span style="color: #339933;">/*</span> postindex<span style="color: #666666; font-style: italic;">; lr ← *sp; sp ← sp + 8 */</span>
<span style="color: #46aa03; font-weight: bold;">bx</span> lr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">str lr, [sp, #-8]!  /* preindex: sp ← sp - 8; *sp ← lr */
... // Code of the function
ldr lr, [sp], #+8   /* postindex; lr ← *sp; sp ← sp + 8 */
bx lr</p></div>

<p>
Yes, these addressing modes were invented to support this sort of things. Using a single instruction is better in terms of code size. This may not seem relevant, but it is when we realize that the stack bookkeeping is required in almost every function we write!
</p>
<h2>First approach</h2>
<p>
Let’s implement the factorial function above.
</p>
<p>
First we have to learn a new instruction to multiply two numbers: <code>mul Rdest, Rsource1, Rsource2</code>. Note that multiplying two 32 bit values may require up to 64 bits for the result. This instruction only computes the lower 32 bits. Because we are not going to use 64 bit values in this example, the maximum factorial we will be able to compute is 12! (13! is bigger than 2<sup>32</sup>). We will not check that the entered number is lower than 13 to keep the example simple (I encourage you to add this check to the example, though). In versions of the ARM architecture prior to ARMv6 this instruction could not have <code>Rdest</code> the same as <code>Rsource1</code>. GNU assembler may print a warning if you don’t pass <code>-march=armv6</code>.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> <span style="color: #339933;">--</span> factorial01<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
message1<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"Type a number: "</span>
format<span style="color: #339933;">:</span>   <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"%d"</span>
message2<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"The factorial of %d is %d\n"</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
factorial<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">str</span> lr<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span>#<span style="color: #339933;">-</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>!  <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Push</span> lr onto the top of the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span>#<span style="color: #339933;">-</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>!  <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Push</span> r0 onto the top of the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
                       <span style="color: #339933;">/*</span> Note that after that<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span> is <span style="color: #ff0000;">8</span> <span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>         <span style="color: #339933;">/*</span> compare r0 <span style="color: #00007f; font-weight: bold;">and</span> <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    bne is_nonzero     <span style="color: #339933;">/*</span> if r0 != <span style="color: #ff0000;">0</span> then branch <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>         <span style="color: #339933;">/*</span> r0 ← <span style="color: #ff0000;">1</span><span style="color: #339933;">.</span> This is the return <span style="color: #339933;">*/</span>
    b end
is_nonzero<span style="color: #339933;">:</span>
                       <span style="color: #339933;">/*</span> Prepare the <span style="color: #00007f; font-weight: bold;">call</span> to factorial<span style="color: #009900; font-weight: bold;">(</span>n<span style="color: #339933;">-</span><span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">sub</span> r0<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>     <span style="color: #339933;">/*</span> r0 ← r0 <span style="color: #339933;">-</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> factorial
                       <span style="color: #339933;">/*</span> After the <span style="color: #00007f; font-weight: bold;">call</span> r0 contains factorial<span style="color: #009900; font-weight: bold;">(</span>n<span style="color: #339933;">-</span><span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
                       <span style="color: #339933;">/*</span> Load r0 <span style="color: #009900; font-weight: bold;">(</span>that we kept <span style="color: #00007f; font-weight: bold;">in</span> th <span style="color: #0000ff; font-weight: bold;">stack</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #00007f; font-weight: bold;">into</span> r1 <span style="color: #339933;">*/</span>
    ldr r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #009900; font-weight: bold;">]</span>       <span style="color: #339933;">/*</span> r1 ← <span style="color: #339933;">*</span><span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mul</span> r0<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> r1     <span style="color: #339933;">/*</span> r0 ← r0 <span style="color: #339933;">*</span> r1 <span style="color: #339933;">*/</span>
&nbsp;
end<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">add</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #339933;">+</span><span style="color: #ff0000;">4</span>    <span style="color: #339933;">/*</span> Discard the r0 we kept <span style="color: #00007f; font-weight: bold;">in</span> the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
    ldr lr<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> #<span style="color: #339933;">+</span><span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Pop</span> the top of the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #00007f; font-weight: bold;">and</span> put it <span style="color: #00007f; font-weight: bold;">in</span> lr <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr              <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Leave</span> factorial <span style="color: #339933;">*/</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">global</span> main
main<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">str</span> lr<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span>#<span style="color: #339933;">-</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>!            <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Push</span> lr onto the top of the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">sub</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>               <span style="color: #339933;">/*</span> Make room for one <span style="color: #ff0000;">4</span> <span style="color: #0000ff; font-weight: bold;">byte</span> integer <span style="color: #00007f; font-weight: bold;">in</span> the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
                                 <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">In</span> these <span style="color: #ff0000;">4</span> bytes we will keep the number <span style="color: #339933;">*/</span>
                                 <span style="color: #339933;">/*</span> entered by the user <span style="color: #339933;">*/</span>
                                 <span style="color: #339933;">/*</span> Note that after that the <span style="color: #0000ff; font-weight: bold;">stack</span> is <span style="color: #ff0000;">8</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> address_of_message1  <span style="color: #339933;">/*</span> Set &amp;message1 as the first parameter of printf <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> printf                    <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Call</span> printf <span style="color: #339933;">*/</span>
&nbsp;
    ldr r0<span style="color: #339933;">,</span> address_of_format    <span style="color: #339933;">/*</span> Set &amp;format as the first parameter of scanf <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span>                   <span style="color: #339933;">/*</span> Set the top of the <span style="color: #0000ff; font-weight: bold;">stack</span> as the second parameter <span style="color: #339933;">*/</span>
                                 <span style="color: #339933;">/*</span> of scanf <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> scanf                     <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Call</span> scanf <span style="color: #339933;">*/</span>
&nbsp;
    ldr r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #009900; font-weight: bold;">]</span>                 <span style="color: #339933;">/*</span> Load the integer read by scanf <span style="color: #00007f; font-weight: bold;">into</span> r0 <span style="color: #339933;">*/</span>
                                 <span style="color: #339933;">/*</span> So we set it as the first parameter of factorial <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> factorial                 <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Call</span> factorial <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r2<span style="color: #339933;">,</span> r0                   <span style="color: #339933;">/*</span> Get the result of factorial <span style="color: #00007f; font-weight: bold;">and</span> move it to r2 <span style="color: #339933;">*/</span>
                                 <span style="color: #339933;">/*</span> So we set it as the third parameter of printf <span style="color: #339933;">*/</span>
    ldr r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #009900; font-weight: bold;">]</span>                 <span style="color: #339933;">/*</span> Load the integer read by scanf <span style="color: #00007f; font-weight: bold;">into</span> r1 <span style="color: #339933;">*/</span>
                                 <span style="color: #339933;">/*</span> So we set it as the second parameter of printf <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> address_of_message2  <span style="color: #339933;">/*</span> Set &amp;message2 as the first parameter of printf <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> printf                    <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Call</span> printf <span style="color: #339933;">*/</span>
&nbsp;
&nbsp;
    <span style="color: #00007f; font-weight: bold;">add</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #339933;">+</span><span style="color: #ff0000;">4</span>              <span style="color: #339933;">/*</span> Discard the integer read by scanf <span style="color: #339933;">*/</span>
    ldr lr<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> #<span style="color: #339933;">+</span><span style="color: #ff0000;">4</span>            <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Pop</span> the top of the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #00007f; font-weight: bold;">and</span> put it <span style="color: #00007f; font-weight: bold;">in</span> lr <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr                        <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Leave</span> main <span style="color: #339933;">*/</span>
&nbsp;
address_of_message1<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> message1
address_of_message2<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> message2
address_of_format<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> format</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* -- factorial01.s */
.data

message1: .asciz "Type a number: "
format:   .asciz "%d"
message2: .asciz "The factorial of %d is %d\n"

.text

factorial:
    str lr, [sp,#-4]!  /* Push lr onto the top of the stack */
    str r0, [sp,#-4]!  /* Push r0 onto the top of the stack */
                       /* Note that after that, sp is 8 byte aligned */
    cmp r0, #0         /* compare r0 and 0 */
    bne is_nonzero     /* if r0 != 0 then branch */
    mov r0, #1         /* r0 ← 1. This is the return */
    b end
is_nonzero:
                       /* Prepare the call to factorial(n-1) */
    sub r0, r0, #1     /* r0 ← r0 - 1 */
    bl factorial
                       /* After the call r0 contains factorial(n-1) */
                       /* Load r0 (that we kept in th stack) into r1 */
    ldr r1, [sp]       /* r1 ← *sp */
    mul r0, r0, r1     /* r0 ← r0 * r1 */
    
end:
    add sp, sp, #+4    /* Discard the r0 we kept in the stack */
    ldr lr, [sp], #+4  /* Pop the top of the stack and put it in lr */
    bx lr              /* Leave factorial */

.global main
main:
    str lr, [sp,#-4]!            /* Push lr onto the top of the stack */
    sub sp, sp, #4               /* Make room for one 4 byte integer in the stack */
                                 /* In these 4 bytes we will keep the number */
                                 /* entered by the user */
                                 /* Note that after that the stack is 8-byte aligned */
    ldr r0, address_of_message1  /* Set &amp;message1 as the first parameter of printf */
    bl printf                    /* Call printf */

    ldr r0, address_of_format    /* Set &amp;format as the first parameter of scanf */
    mov r1, sp                   /* Set the top of the stack as the second parameter */
                                 /* of scanf */
    bl scanf                     /* Call scanf */

    ldr r0, [sp]                 /* Load the integer read by scanf into r0 */
                                 /* So we set it as the first parameter of factorial */
    bl factorial                 /* Call factorial */

    mov r2, r0                   /* Get the result of factorial and move it to r2 */
                                 /* So we set it as the third parameter of printf */
    ldr r1, [sp]                 /* Load the integer read by scanf into r1 */
                                 /* So we set it as the second parameter of printf */
    ldr r0, address_of_message2  /* Set &amp;message2 as the first parameter of printf */
    bl printf                    /* Call printf */


    add sp, sp, #+4              /* Discard the integer read by scanf */
    ldr lr, [sp], #+4            /* Pop the top of the stack and put it in lr */
    bx lr                        /* Leave main */

address_of_message1: .word message1
address_of_message2: .word message2
address_of_format: .word format</p></div>

<p>
Most of the code is pretty straightforward. In both functions, <code>main</code> and <code>factorial</code>, we allocate 4 extra bytes on the top of the stack. In <code>factorial</code>, to keep the value of <code>r0</code>, because it will be overwritten during the recursive call (twice, as a first parameter and as the result of the recursive function call). In <code>main</code>, to keep the value entered by the user (if you recall chapter 9 we used a global variable here).</p>
<p>It is important to bear in mind that the stack, like a real stack, the last element stacked (pushed onto the top) will be the first one to be taken out the stack (popped from the top). We store <code>lr</code> and make room for a 4 bytes integer. Since this is a stack, the opposite order must be used to return the stack to its original state. We first discard the integer and then we restore the <code>lr</code>. Note that this happens as well when we reserve the stack storage for the integer using a <code>sub</code> and then we discard such storage doing the opposite operation <code>add</code>.
</p>
<h2>Can we do it better?</h2>
<p>
Note that the number of instructions that we need to push and pop data to and from the stack grows linearly with respect to the number of data items. Since ARM was designed for embedded systems, ARM designers devised a way to reduce the number of instructions we need for the «bookkeeping» of the stack. These instructions are load multiple, <code>ldm</code>, and store multiple, <code>stm</code>.
</p>
<p>
These two instructions are rather powerful and allow in a single instruction perform a lot of things. Their syntax is shown as follows. Elements enclosed in curly braces <code>{</code> and <code>}</code> may be omitted from the syntax (the effect of the instruction will vary, though).
</p>
<pre>ldm addressing-mode Rbase{!}, register-set
stm addressing-mode Rbase{!}, register-set
</pre>
<p>
We will consider <code>addressing-mode</code> later. <code>Rbase</code> is the base address used to load to or store from the <code>register-set</code>. All 16 ARM registers may be specified in <code>register-set</code> (except <code>pc</code> in <code>stm</code>). A set of addresses is generated when executing these instructions. One address per register in the register-set. Then, each register, in ascending order, is paired with each of these addresses, also in ascending order. This way the lowest-numbered register gets the lowest memory address, and the highest-numbered register gets the highest memory address. Each pair register-address is then used to perform the memory operation: load or store. Specifying <code>!</code> means that <code>Rbase</code> will be updated. The updated value depends on <code>addressing-mode</code>.
</p>
<p>
Note that, if the registers are paired with addresses depending on their register number, it seems that they will always be loaded and stored in the same way. For instance a <code>register-set</code> containing <code>r4</code>, <code>r5</code> and <code>r6</code> will always store <code>r4</code> in the lowest address generated by the instruction and <code>r6</code> in the highest one. We can, though, specify what is considered the lowest address or the highest address. So, is <code>Rbase</code> actually the highest address or the lowest address of the multiple load/store? This is one of the two aspects that is controlled by <code>addressing-mode</code>. The second aspect relates to when the address of the memory operation changes between each memory operation.
</p>
<p>
If the value in <code>Rbase</code> is to be considered the the highest address it means that we should first decrease <code>Rbase</code> as many bytes as required by the number of registers in the <code>register-set</code> (this is 4 times the number of registers) to form the lowest address. Then we can load or store each register consecutively starting from that lowest address, always in ascending order of the register number. This addressing mode is called <em>decreasing</em> and is specified using a <q><code>d</code></q>. Conversely, if <code>Rbase</code> is to be considered the lowest address, then this is a bit easier as we can use its value as the lowest address already. We proceed as usual, loading or storing each register in ascending order of their register number. This addressing mode is called <em>increasing</em> and is specified using an <q><code>i</code></q>.
</p>
<p>
At each load or store, the address generated for the memory operation may be updated <em>after</em> or <em>before</em> the memory operation itself. We can specify this using <q><code>a</code></q> or <q><code>b</code></q>, respectively.
</p>
<p>
If we specify <code>!</code>, after the instruction, <code>Rbase</code> will have the highest address generated in the increasing mode and the lowest address generated in the decreasing mode. The final value of <code>Rbase</code> will include the final addition or subtraction if we use a mode that updates after (an <q><code>a</code></q> mode).
</p>
<p>
So we have four addressing modes, namely: <code>ia</code>, <code>ib</code>, <code>da</code> and <code>db</code>. These addressing modes are specified as <strong>suffixes</strong> of the <code>stm</code> and <code>ldm</code> instructions. So the full set of names is <code>stmia</code>, <code>stmib</code>, <code>stmda</code>, <code>stmdb</code>, <code>ldmia</code>, <code>ldmib</code>, <code>ldmda</code>, <code>ldmdb</code>. Now you may think that this is overly complicated, but we need not use all the eight modes. Only two of them are of interest to us now.
</p>
<p>
When we push something onto the stack we actually decrease the stack pointer (because in Linux the stack grows downwards). More precisely, we first decrease the stack pointer as many bytes as needed before doing the actual store on that just computed stack pointer. So the appropiate <code>addressing-mode</code> when pushing onto the stack is <code>stmdb</code>. Conversely when popping from the stack we will use <code>ldmia</code>: we increment the stack pointer after we have performed the load.
</p>
<h2>Factorial again</h2>
<p>
Before illustrating these two instructions, we will first slightly rewrite our factorial.
</p>
<p>
If you go back to the code of our factorial, there is a moment, when computing <code>n * factorial(n-1)</code>, where the initial value of <code>r0</code> is required. The value of <code>n</code> was in <code>r0</code> at the beginning of the function, but <code>r0</code> can be freely modified by called functions. We chose, in the example above, to keep a copy of <code>r0</code> in the stack in line 12. Later, in line 24, we loaded it from the stack in <code>r1</code>, just before computing the multiplication.
</p>
<p>
In our second version of factorial, we will keep a copy of the initial value of <code>r0</code> into <code>r4</code>. But <code>r4</code> is a register the value of which must be restored upon leaving a function. So we will keep the value of <code>r4</code> at the entry of the function in the stack. At the end we will restore it back from the stack. This way we can use <code>r4</code> without breaking the rules of <em>well-behaved functions</em>.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>10
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">factorial<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">str</span> lr<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span>#<span style="color: #339933;">-</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>!  <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Push</span> lr onto the top of the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> r4<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span>#<span style="color: #339933;">-</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>!  <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Push</span> r4 onto the top of the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
                       <span style="color: #339933;">/*</span> The <span style="color: #0000ff; font-weight: bold;">stack</span> is now <span style="color: #ff0000;">8</span> <span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> r0         <span style="color: #339933;">/*</span> Keep a copy of the initial value of r0 <span style="color: #00007f; font-weight: bold;">in</span> r4 <span style="color: #339933;">*/</span>
&nbsp;
&nbsp;
    <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>         <span style="color: #339933;">/*</span> compare r0 <span style="color: #00007f; font-weight: bold;">and</span> <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    bne is_nonzero     <span style="color: #339933;">/*</span> if r0 != <span style="color: #ff0000;">0</span> then branch <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>         <span style="color: #339933;">/*</span> r0 ← <span style="color: #ff0000;">1</span><span style="color: #339933;">.</span> This is the return <span style="color: #339933;">*/</span>
    b end
is_nonzero<span style="color: #339933;">:</span>
                       <span style="color: #339933;">/*</span> Prepare the <span style="color: #00007f; font-weight: bold;">call</span> to factorial<span style="color: #009900; font-weight: bold;">(</span>n<span style="color: #339933;">-</span><span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">sub</span> r0<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>     <span style="color: #339933;">/*</span> r0 ← r0 <span style="color: #339933;">-</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> factorial
                       <span style="color: #339933;">/*</span> After the <span style="color: #00007f; font-weight: bold;">call</span> r0 contains factorial<span style="color: #009900; font-weight: bold;">(</span>n<span style="color: #339933;">-</span><span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
                       <span style="color: #339933;">/*</span> Load initial value of r0 <span style="color: #009900; font-weight: bold;">(</span>that we kept <span style="color: #00007f; font-weight: bold;">in</span> r4<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #00007f; font-weight: bold;">into</span> r1 <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> r4         <span style="color: #339933;">/*</span> r1 ← r4 <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mul</span> r0<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> r1     <span style="color: #339933;">/*</span> r0 ← r0 <span style="color: #339933;">*</span> r1 <span style="color: #339933;">*/</span>
&nbsp;
end<span style="color: #339933;">:</span>
    ldr r4<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> #<span style="color: #339933;">+</span><span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Pop</span> the top of the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #00007f; font-weight: bold;">and</span> put it <span style="color: #00007f; font-weight: bold;">in</span> r4 <span style="color: #339933;">*/</span>
    ldr lr<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> #<span style="color: #339933;">+</span><span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Pop</span> the top of the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #00007f; font-weight: bold;">and</span> put it <span style="color: #00007f; font-weight: bold;">in</span> lr <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr              <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Leave</span> factorial <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">factorial:
    str lr, [sp,#-4]!  /* Push lr onto the top of the stack */
    str r4, [sp,#-4]!  /* Push r4 onto the top of the stack */
                       /* The stack is now 8 byte aligned */
    mov r4, r0         /* Keep a copy of the initial value of r0 in r4 */


    cmp r0, #0         /* compare r0 and 0 */
    bne is_nonzero     /* if r0 != 0 then branch */
    mov r0, #1         /* r0 ← 1. This is the return */
    b end
is_nonzero:
                       /* Prepare the call to factorial(n-1) */
    sub r0, r0, #1     /* r0 ← r0 - 1 */
    bl factorial
                       /* After the call r0 contains factorial(n-1) */
                       /* Load initial value of r0 (that we kept in r4) into r1 */
    mov r1, r4         /* r1 ← r4 */
    mul r0, r0, r1     /* r0 ← r0 * r1 */

end:
    ldr r4, [sp], #+4  /* Pop the top of the stack and put it in r4 */
    ldr lr, [sp], #+4  /* Pop the top of the stack and put it in lr */
    bx lr              /* Leave factorial */</p></div>

<p>
Note that the remainder of the program does not have to change. This is the cool thing of functions 
</p>
<p>
Ok, now pay attention to these two sequences in our new factorial version above.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>11
12
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">    <span style="color: #00007f; font-weight: bold;">str</span> lr<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span>#<span style="color: #339933;">-</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>!  <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Push</span> lr onto the top of the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> r4<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span>#<span style="color: #339933;">-</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>!  <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Push</span> r4 onto the top of the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">    str lr, [sp,#-4]!  /* Push lr onto the top of the stack */
    str r4, [sp,#-4]!  /* Push r4 onto the top of the stack */</p></div>


<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>30
31
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">    ldr r4<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> #<span style="color: #339933;">+</span><span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Pop</span> the top of the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #00007f; font-weight: bold;">and</span> put it <span style="color: #00007f; font-weight: bold;">in</span> r4 <span style="color: #339933;">*/</span>
    ldr lr<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> #<span style="color: #339933;">+</span><span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Pop</span> the top of the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #00007f; font-weight: bold;">and</span> put it <span style="color: #00007f; font-weight: bold;">in</span> lr <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">    ldr r4, [sp], #+4  /* Pop the top of the stack and put it in r4 */
    ldr lr, [sp], #+4  /* Pop the top of the stack and put it in lr */</p></div>

<p>
Now, let’s replace them with <code>stmdb</code> and <code>ldmia</code> as explained a few paragraphs ago.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>11
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">    stmdb <span style="color: #46aa03; font-weight: bold;">sp</span>!<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>    <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Push</span> r4 <span style="color: #00007f; font-weight: bold;">and</span> lr onto the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">    stmdb sp!, {r4, lr}    /* Push r4 and lr onto the stack */</p></div>


<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>30
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">    ldmia <span style="color: #46aa03; font-weight: bold;">sp</span>!<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>    <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Pop</span> lr <span style="color: #00007f; font-weight: bold;">and</span> r4 from the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">    ldmia sp!, {r4, lr}    /* Pop lr and r4 from the stack */</p></div>

<p>
Note that the order of the registers in the set of registers is not relevant, but the processor will handle them in ascending order, so we should write them in ascending order. GNU assembler will emit a warning otherwise. Since <code>lr</code> is actually <code>r14</code> it must go after <code>r4</code>. This means that our code is 100% equivalent to the previous one since <code>r4</code> will end in a lower address than <code>lr</code>: remember our stack grows toward lower addresses, thus <code>r4</code> which is in the top of the stack in <code>factorial</code> has the lowest address.
</p>
<p>
Remembering <code>stmdb sp!</code> and <code>ldmia sp!</code> may be a bit hard. Also, given that these two instructions will be relatively common when entering and leaving functions, GNU assembler provides two <em>mnemonics</em> <code>push</code> and <code>pop</code> for <code>stmdb sp!</code> and <code>ldmia sp!</code>, respectively. Note that these are not ARM instructions actually, just convenience names that are easier to remember.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>11
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">    push {r4, lr}</p></div>


<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>30
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">    pop {r4, lr}</p></div>

<p>
That’s all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/&amp;text=ARM+assembler+in+Raspberry+Pi+%E2%80%93+Chapter+10+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>				<p class="tags"><span><a href="http://thinkingeek.com/tag/arm/" rel="tag">arm</a>, <a href="http://thinkingeek.com/tag/assembler/" rel="tag">assembler</a>, <a href="http://thinkingeek.com/tag/function/" rel="tag">function</a>, <a href="http://thinkingeek.com/tag/function-call/" rel="tag">function call</a>, <a href="http://thinkingeek.com/tag/functions/" rel="tag">functions</a>, <a href="http://thinkingeek.com/tag/pi/" rel="tag">pi</a>, <a href="http://thinkingeek.com/tag/raspberry/" rel="tag">raspberry</a>, <a href="http://thinkingeek.com/tag/stack/" rel="tag">stack</a></span></p>		<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/" rel="prev">ARM assembler in Raspberry Pi – Chapter 9</a></span>
			<span class="next"><a href="http://thinkingeek.com/2013/03/16/arm-assembler-raspberry-pi-chapter-11/" rel="next">ARM assembler in Raspberry Pi – Chapter 11</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
			<h3>17 thoughts on “<span>ARM assembler in Raspberry Pi – Chapter 10</span>”</h3>
		<ul class="commentlist">
				<li class="comment even thread-even depth-1" id="comment-1439">
				<div id="div-comment-1439" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a16ae1e69be669d6f2c5fa0972e39c8a?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a16ae1e69be669d6f2c5fa0972e39c8a?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a16ae1e69be669d6f2c5fa0972e39c8a?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a16ae1e69be669d6f2c5fa0972e39c8a-0">			<cite class="fn"><a href="http://digilander.libero.it/zantaz/" rel="external nofollow" class="url">Mariani Antonio Mario</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/#comment-1439">
			July 1, 2013 at 3:43 am</a>		</div>

		<p>very nice code … I follow it …</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/?replytocom=1439#respond" onclick="return addComment.moveForm( &quot;div-comment-1439&quot;, &quot;1439&quot;, &quot;respond&quot;, &quot;669&quot; )" aria-label="Reply to Mariani Antonio Mario">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1 parent" id="comment-1455">
				<div id="div-comment-1455" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-9e81362ff9654533accffc53762c9520-0" originals="32" src-orig="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Fernando</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/#comment-1455">
			July 9, 2013 at 9:42 pm</a>		</div>

		<p>I was wondering if you can provide more info/write a post about stack frame.</p>
<p>While disassembling C code, I found some “fp” register. Reading a bit, seems to be a pointer to the stack frame, and although it seems usefull, I can’t find any info on the AAPCS, so it looks like semi-standard?</p>
<p>Any insight on this subject will be appreciated.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/?replytocom=1455#respond" onclick="return addComment.moveForm( &quot;div-comment-1455&quot;, &quot;1455&quot;, &quot;respond&quot;, &quot;669&quot; )" aria-label="Reply to Fernando">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2 parent" id="comment-1456">
				<div id="div-comment-1456" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-0">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/#comment-1456">
			July 9, 2013 at 10:41 pm</a>		</div>

		<p>The GCC compiler has used traditionally different names for registers r10, r11 and r12. They do not seem to be in any ARM document so I guess it is just some GCC own convention.</p>
<p>Register <code>r10</code> is also called <code>sl</code>. It is used for PIC code (Position Independent Code), a specific code shape required when generating shared libraries (usually libraries like <i>libname.so</i>). This is a rather advanced thing which would require several chapters to explain.</p>
<p>r11 is called <code>fp</code>. This register is used to keep the stack frame (also called the stack base or base pointer). What is the stack frame? The stack frame is just a register that keeps the value that <code>sp</code> got at the beginning. Since r11 must be restored, the previous value of r11 must be first kept in the stack, so r11 will not contain the address of the stack at the beginning of the function but actually contains <code>sp0 - 4</code> where <code>sp0</code> means the value of <code>sp</code> at the beginning of the function.</p>
<p>This register is only required to index local variables in the stack itself when the amount of stack is not a constant value (imagine the amount local data depends on a parameter of the function). Otherwise we should be able to index all the data through <code>sp</code> because we always should know how many bytes we have decreased sp.</p>
<p>This is an example where a stack-frame (like <code>fp</code>) is required.<br>
<code><br>
int f(int n)<br>
{<br>
&nbsp;&nbsp;&nbsp;/* The length of a[n] is only known at runtime */<br>
&nbsp;&nbsp;&nbsp;int a[n];<br>
&nbsp;&nbsp;&nbsp;int i, s;<br>
&nbsp;&nbsp;&nbsp;...<br>
}<br>
</code></p>
<p>If the array <code>a</code> was declared with a fixed amount of elements (like <code>int a[10]</code>) then from the top of the stack we could know where <code>a</code>, <code>i</code> and <code>s</code>. If a has a nonconstant size we could also compute the addresses of these variables but it would be much more complicated (in fact, it is complicated: imagine if there were more than one variable-length array). So we keep the address of sp (actually, sp – 4) and we use it to index these variable-length stuff (we would probably keep also the actual size of the arrays in another local variable).</p>
<p>Finally, r12 is also called <code>ip</code> and seems to be used by the linker when the called routine is so far (in bytes distance) that it requires an intermediate function.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/?replytocom=1456#respond" onclick="return addComment.moveForm( &quot;div-comment-1456&quot;, &quot;1456&quot;, &quot;respond&quot;, &quot;669&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment odd alt depth-3 parent" id="comment-1457">
				<div id="div-comment-1457" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-9e81362ff9654533accffc53762c9520-1" originals="32" src-orig="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Fernando</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/#comment-1457">
			July 10, 2013 at 9:29 am</a>		</div>

		<p>First of all, thanks for your quick and detailed explanation. I’m really enjoying this post of yours, really really interesting and useful.</p>
<p>Next: I saw in <a href="http://www.intel.com/education/highered/embedded/lectures/04_swis.ppt" rel="nofollow">this</a> PPT from Intel (page 6) what it seems to be a “standard frame”, which contains all the registers. It does not fit with what you say (which, by the way, makes total sense for me), so:<br>
– Shall I expect a frame to be like that, or it does not exist such concept as “standard frame”, which holds every register.</p>
<p>In the other hand, I was wondering: why GCC creates frames even if I do not have a variable-length stack? The following code generates a frame when compiling with GCC (both for ‘main’ and ‘f’):<br>
void main(void){<br>
int i, j;<br>
i = 1;<br>
j = f(i);<br>
}</p>
<p>int f(int i){<br>
return 2*i;<br>
}</p>
<p>Also, that code generates some STR instructions, which I see unnecessary as there is plenty register-space for those simple operations (Also, GCC code is at least 2-3 times bigger than hand-crafted code ¿?).</p>
<p>Thanks in advance for your reply.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/?replytocom=1457#respond" onclick="return addComment.moveForm( &quot;div-comment-1457&quot;, &quot;1457&quot;, &quot;respond&quot;, &quot;669&quot; )" aria-label="Reply to Fernando">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-4 parent" id="comment-1459">
				<div id="div-comment-1459" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-1">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/#comment-1459">
			July 12, 2013 at 8:06 pm</a>		</div>

		<p>I’ll first answer the GCC thing. GCC by default does not produce optimized code, so it emits very simple-minded code which is known to be correct but can be largely improved. This is why you are seing dumb code being emitted. You can request GCC to emit better code by enabling optimization. You do that by using the flag <code>-O</code>. GCC allows up to 3 optimization levels (higher levels are downgraded to <code>-O3</code> always) but uually <code>-O2</code> is a good default. You should see that the code is much better. GCC is known for not being great in the ARM architecture, but the code should definitely look much better to you.</p>
<p>Regarding to the slides you link, they cover a topic on exceptional situations. In these cases a <em>recovery</em> function is invoked by the processor and then it changes its mode to a special one. ARM processors have 7 modes for exceptions, depending on the nature of the exceptional situation being handled. In these modes some registers are <em>banked</em>, meaning that you have a different register than the one you usually have in normal mode. For instance, there is a mode called Fast Interrupt Request (FIQ) where registers <code>r8</code> to <code>r14</code> are actually <code>r8_fiq</code> to <code>r14_fiq</code>, modifying <code>r8_fiq</code> would not affect the <code>r8</code> of your program (modifying <code>r2</code> would, because it is not banked). In these modes, the processor sets the <code>lr</code> register to the instruction (or close to, depending on the exception) that was going to be run (or actually being run) when the exception happened. This way, once we have handled the exceptional situation, we can return back to the user code.</p>
<p>As far as I know, there is nothing in the architecture that forces you to use a frame pointer, even in these exceptional cases. So, maybe the slides just relate to a convention followed in that particular environment. That said, we know that in some situations the <code>fp</code> is really handy. Given its non-mandatory nature, the user (usually the compiler) may choose to use <code>fp</code> at its own discretion as long as its original value is preserved at the exit of the function.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/?replytocom=1459#respond" onclick="return addComment.moveForm( &quot;div-comment-1459&quot;, &quot;1459&quot;, &quot;respond&quot;, &quot;669&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment odd alt depth-5 parent" id="comment-1460">
				<div id="div-comment-1460" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-9e81362ff9654533accffc53762c9520-2" originals="32" src-orig="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Fernando</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/#comment-1460">
			July 12, 2013 at 11:09 pm</a>		</div>

		<p>Well, thanks again for your detailed explanation.</p>
<p>I knew that GCC usually generates non-optimized code, but well, now I know exactly how it looks like.</p>
<p>By the way, are you planning on continuing with this ARM RPI ASM series? I’d love to see something more about exception handling, MMU, timer, and in general, some more interaction with the existing hardware.</p>

		
				</div>
		</li><!-- #comment-## -->
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-5" id="comment-1461">
				<div id="div-comment-1461" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-2">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/#comment-1461">
			July 13, 2013 at 8:30 am</a>		</div>

		<p>I’m committed to continue these series but not on the topics you mention since those topics mean entering the realm of operating systems.</p>
<p>Kind regards.</p>

		
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
		<li class="comment odd alt thread-even depth-1 parent" id="comment-2490">
				<div id="div-comment-2490" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/dabf5f0454fc588772f8fc0e93b2004e?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/dabf5f0454fc588772f8fc0e93b2004e?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-dabf5f0454fc588772f8fc0e93b2004e-0" originals="32" src-orig="http://1.gravatar.com/avatar/dabf5f0454fc588772f8fc0e93b2004e?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Denz</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/#comment-2490">
			December 8, 2013 at 9:40 pm</a>		</div>

		<p>Thanks for your the best articles!<br>
I have question: how to return structure in assembler ( apcs-linux ) from routine? Please, show example.<br>
—<br>
Den</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/?replytocom=2490#respond" onclick="return addComment.moveForm( &quot;div-comment-2490&quot;, &quot;2490&quot;, &quot;respond&quot;, &quot;669&quot; )" aria-label="Reply to Denz">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2" id="comment-2652">
				<div id="div-comment-2652" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-3">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/#comment-2652">
			January 2, 2014 at 11:15 am</a>		</div>

		<p>Hi,</p>
<p>there will be an example of returning a struct in Chapter 18 (still under preparation at the time of answering your comment).</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/?replytocom=2652#respond" onclick="return addComment.moveForm( &quot;div-comment-2652&quot;, &quot;2652&quot;, &quot;respond&quot;, &quot;669&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1 parent" id="comment-954481">
				<div id="div-comment-954481" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/7a33dfef5460bfa20067b7622dfd9371?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/7a33dfef5460bfa20067b7622dfd9371?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-7a33dfef5460bfa20067b7622dfd9371-0" originals="32" src-orig="http://1.gravatar.com/avatar/7a33dfef5460bfa20067b7622dfd9371?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Yeneros</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/#comment-954481">
			December 10, 2015 at 7:13 am</a>		</div>

		<p>Hi rferrer, </p>
<p>Excellent Tutorials! I am thoroughly enjoying them, Thank you so much for your great work</p>
<p>Just a note for anyone else doing this on the Raspberry Pi and encountering difficulty with the multiplication instruction as follows:</p>
<p>pi@raspberrypi ~/Assembly Tutorials/Lesson 10 $ as -o factoral01.o factoral01.s<br>
factoral01.s: Assembler messages:<br>
factoral01.s:25: Rd and Rm should be different in mul</p>
<p>mul r0, r0, r1     /* r0 ← r0 * r1 */</p>
<p>Apparently since ARMv6 this now works but on earlier systems you get that compiler error which you can fix by simply reversing the order of the last two operands:</p>
<p>    mul r0, r1, r0     /* r0 ← r1 * r0 */</p>
<p>Cheers.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/?replytocom=954481#respond" onclick="return addComment.moveForm( &quot;div-comment-954481&quot;, &quot;954481&quot;, &quot;respond&quot;, &quot;669&quot; )" aria-label="Reply to Yeneros">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2" id="comment-954490">
				<div id="div-comment-954490" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-4">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/#comment-954490">
			December 10, 2015 at 8:02 am</a>		</div>

		<p>Hi Yeneros,</p>
<p>yes, you are right that in earlier versions of the ARM architecture this was not allowed. You can also address this problem by passing the flag <code>-march=armv6</code> to the assembler invocation.</p>
<p>That said, your technique is easier and just requires swapping the operands. Thanks for the tip!</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/?replytocom=954490#respond" onclick="return addComment.moveForm( &quot;div-comment-954490&quot;, &quot;954490&quot;, &quot;respond&quot;, &quot;669&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-even depth-1 parent" id="comment-969996">
				<div id="div-comment-969996" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/2061235772f8d879ffaf931b260171b6?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/2061235772f8d879ffaf931b260171b6?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-2061235772f8d879ffaf931b260171b6-0" originals="32" src-orig="http://2.gravatar.com/avatar/2061235772f8d879ffaf931b260171b6?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Linus</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/#comment-969996">
			June 19, 2016 at 2:36 pm</a>		</div>

		<p>Hi Roger,</p>
<p>thank you for the easy to understand tutorials. I was wondering about one thing, is it required to move the value of r4 to r1 and then multiplying r0 and r1 in the factorial example program?<br>
    mov r1, r4         /* r1 ← r4 */<br>
    mul r0, r0, r1     /* r0 ← r0 * r1 */<br>
Can’t you do the following or am I missing something?<br>
    mul r0, r0, r4     /* r0 ← r0 * r4 */</p>
<p>Thanks again, </p>
<p>Linus</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/?replytocom=969996#respond" onclick="return addComment.moveForm( &quot;div-comment-969996&quot;, &quot;969996&quot;, &quot;respond&quot;, &quot;669&quot; )" aria-label="Reply to Linus">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2" id="comment-970002">
				<div id="div-comment-970002" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-5">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/#comment-970002">
			June 19, 2016 at 4:47 pm</a>		</div>

		<p>Hi Linus,</p>
<p>yes, you can do this. Likely I left this unnecesary <code>mov</code> when I took the first version and modified to write the second one.</p>
<p>Thank you!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/?replytocom=970002#respond" onclick="return addComment.moveForm( &quot;div-comment-970002&quot;, &quot;970002&quot;, &quot;respond&quot;, &quot;669&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1 parent" id="comment-972044">
				<div id="div-comment-972044" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/7fc7d03b21a2bcadd74ca0295a1a780f?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/7fc7d03b21a2bcadd74ca0295a1a780f?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-7fc7d03b21a2bcadd74ca0295a1a780f-0" originals="32" src-orig="http://1.gravatar.com/avatar/7fc7d03b21a2bcadd74ca0295a1a780f?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">tieugiaqb</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/#comment-972044">
			July 29, 2016 at 9:27 am</a>		</div>

		<p>thanks for your tutorials. it’s exellent series</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/?replytocom=972044#respond" onclick="return addComment.moveForm( &quot;div-comment-972044&quot;, &quot;972044&quot;, &quot;respond&quot;, &quot;669&quot; )" aria-label="Reply to tieugiaqb">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2" id="comment-972056">
				<div id="div-comment-972056" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-6">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/#comment-972056">
			July 29, 2016 at 7:00 pm</a>		</div>

		<p>Thanks!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/?replytocom=972056#respond" onclick="return addComment.moveForm( &quot;div-comment-972056&quot;, &quot;972056&quot;, &quot;respond&quot;, &quot;669&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-even depth-1 parent" id="comment-993555">
				<div id="div-comment-993555" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/398c32a6a2c667d6cf67b7e1952d01cc?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/398c32a6a2c667d6cf67b7e1952d01cc?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-398c32a6a2c667d6cf67b7e1952d01cc-0" originals="32" src-orig="http://0.gravatar.com/avatar/398c32a6a2c667d6cf67b7e1952d01cc?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Andrew McConnell</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/#comment-993555">
			May 29, 2017 at 3:03 am</a>		</div>

		<p>Great course!<br>
Just note:<br>
.globl main should be .global main</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/?replytocom=993555#respond" onclick="return addComment.moveForm( &quot;div-comment-993555&quot;, &quot;993555&quot;, &quot;respond&quot;, &quot;669&quot; )" aria-label="Reply to Andrew McConnell">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2" id="comment-993569">
				<div id="div-comment-993569" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-7">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/#comment-993569">
			May 29, 2017 at 7:33 am</a>		</div>

		<p>Hi Andrew,</p>
<p>thanks for the heads up. While <a href="https://sourceware.org/binutils/docs/as/Global.html" rel="nofollow">accepts both forms</a> better I use <code>.global</code> as it is more readable. I have updated this post but I’m afraid I may have used <code>.globl</code> in other chapters.</p>
<p>Kind regards</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/?replytocom=993569#respond" onclick="return addComment.moveForm( &quot;div-comment-993569&quot;, &quot;993569&quot;, &quot;respond&quot;, &quot;669&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		</ul>
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2013/02/07/arm-assembler-raspberry-pi-chapter-10/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="669" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="0d69925d82"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496598068673"></form>
			</div><!-- #respond -->
	</div>	</div>
