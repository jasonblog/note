# Nested functions



<div class="main">
			<h1>ARM assembler in Raspberry Pi – Chapter 23 </h1>
		<p class="post-meta"><span class="date">January 2, 2015</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2015/01/02/arm-assembler-raspberry-pi-chapter-23/#comments">4</a></span> </p>
		<p>
Today we will see what happens when we nest a function inside another. It seems a harmless thing to do but it happens to come with its own dose of interesting details.
</p>
<p><span id="more-1798"></span></p>
<h2>Nested functions</h2>
<p>
At the assembler level, functions cannot be nested.
</p>
<p>
In fact functions do not even exist at the assembler level. They <em>logically</em> exist because we follow some conventions (in ARM Linux it is the AAPCS) and we call them <em>functions</em>. At the assembler level everything is either data, instructions or addresses. Anything else is built on top of those. This fact, though, has not prevented us from enjoying functions: we have called functions like <code>printf</code> and <code>scanf</code> to print and read strings and in chapter 20 we even called functions indirectly. So functions are a very useful logical convention.
</p>
<p>
So it may make sense to nest a function inside another. What does mean to nest a function inside another? Well, it means that this function will only have meaning as long as its enclosing function is dynamically active (i.e. has been called).
</p>
<p>
At the assembler level a nested function will look like much any other function but they have enough differences to be interesting.
</p>
<h2>Dynamic link</h2>
<p>
In chapter 18 we talked about the <em>dynamic link</em>. The dynamic link is set at the beginning of the function and we use the <code>fp</code> register (an alias for <code>r11</code>) to keep an address in the stack usually called the frame pointer (hence the <code>fp</code> name). It is dynamic because it related to the dynamic activation of the function. The frame pointer gives us a consistent way of accessing local data of the function (that will always be stored in the stack) and those parameters that have to be passed using the stack.
</p>
<p>
Recall that local data, due to the stack growing downwards, is found in negative offsets from the address in the <code>fp</code>. Conversely, parameters passed using the stack will be in positive offsets. Note that <code>fp</code> (aka <code>r11</code>) is a callee-saved register as specified by the AAPCS. This means that we will have to <code>push</code> it onto the stack upon the entry to the function. A non obvious fact from this last step is that the previous frame pointer is always accessible from the current one. In fact it is found among the other callee-saved registers in a positive offset from <code>fp</code> (but a lower offset than the parameters passed using the stack because callee-saved registers are pushed last). This last property may seem non interesting but allows us to chain up through the frame pointer of our callers. In general, this is only of interest for debuggers because they need to keep track of functions being called so far.
</p>
<p>
The following image shows how the stack layout, after the dynamic link has been set and the stack has been enlarged for local variables, looks like for a function <code>g</code> that has been called by <code>f</code>. The set of data that is addressed using the frame pointer is commonly called the <em>activation record</em>, since it is a bunch of information that is specific of the dynamic activation of the function (i.e. of the current call).
</p>
<p><img data-attachment-id="1962" data-permalink="http://thinkingeek.com/2015/01/02/arm-assembler-raspberry-pi-chapter-23/activation_record/" data-orig-file="images/activation_record.png" data-orig-size="500,527" data-comments-opened="1" data-image-meta="{&quot;aperture&quot;:&quot;0&quot;,&quot;credit&quot;:&quot;&quot;,&quot;camera&quot;:&quot;&quot;,&quot;caption&quot;:&quot;&quot;,&quot;created_timestamp&quot;:&quot;0&quot;,&quot;copyright&quot;:&quot;&quot;,&quot;focal_length&quot;:&quot;0&quot;,&quot;iso&quot;:&quot;0&quot;,&quot;shutter_speed&quot;:&quot;0&quot;,&quot;title&quot;:&quot;&quot;,&quot;orientation&quot;:&quot;0&quot;}" data-image-title="activation_record" data-image-description="" data-medium-file="images/activation_record-285x300.png" data-large-file="images/activation_record.png" src="images/activation_record.png" alt="This picture shows the stack layout and the activation records of a function g thas has been called by a function f" width="500" height="527" class="aligncenter size-full wp-image-1962" srcset="images/activation_record.png 500w, images/activation_record-285x300.png 285w" sizes="(max-width: 500px) 100vw, 500px"></p>
<h2>Static link</h2>
<p>
When a function calls a <em>nested function</em> (also called a <em>local function</em>), the nested function can use local variables of the enclosing function. This means that there must be a way for the nested function to access local variables from the enclosing function. One might think that the dynamic link should be enough. In fact, if the programming language only allowed nested functions call other (immediately) nested functions, this would be true. But if this were so, that programming language would be rather limited. That said, for the moment, let’s assume that this is the case: check again the image above. If <code>g</code> is a local function of <code>f</code>, then it should be possible for <code>g</code> to access local variables of <code>f</code> by getting to the previous <code>fp</code>.
</p>
<p>
Consider the following C code (note that Standard C does not allow nesting functions though <a href="https://gcc.gnu.org/onlinedocs/gcc/Nested-Functions.html" title="GCC nested functions">GCC implements them as an extension</a> that we will discuss in a later chapter).
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
</pre></td><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #993333;">void</span> f<span style="color: #009900;">(</span><span style="color: #009900;">)</span> <span style="color: #666666; font-style: italic;">// non-nested (normal) function</span>
<span style="color: #009900;">{</span>
  <span style="color: #993333;">int</span> x<span style="color: #339933;">;</span>
  <span style="color: #993333;">void</span> g<span style="color: #009900;">(</span><span style="color: #009900;">)</span> <span style="color: #666666; font-style: italic;">// nested function</span>
  <span style="color: #009900;">{</span>
    x <span style="color: #339933;">=</span> x <span style="color: #339933;">+</span> <span style="color: #0000dd;">1</span><span style="color: #339933;">;</span> <span style="color: #666666; font-style: italic;">// x ← x + 1</span>
  <span style="color: #009900;">}</span>
&nbsp;
  x <span style="color: #339933;">=</span> <span style="color: #0000dd;">1</span><span style="color: #339933;">;</span>     <span style="color: #666666; font-style: italic;">// x ← 1</span>
  g<span style="color: #009900;">(</span><span style="color: #009900;">)</span><span style="color: #339933;">;</span>       <span style="color: #666666; font-style: italic;">// call g</span>
  x <span style="color: #339933;">=</span> x <span style="color: #339933;">+</span> <span style="color: #0000dd;">1</span><span style="color: #339933;">;</span> <span style="color: #666666; font-style: italic;">// x ← x + 1</span>
  <span style="color: #666666; font-style: italic;">// here x will be 3</span>
<span style="color: #009900;">}</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">void f() // non-nested (normal) function
{
  int x;
  void g() // nested function
  {
    x = x + 1; // x ← x + 1
  }

  x = 1;     // x ← 1
  g();       // call g
  x = x + 1; // x ← x + 1
  // here x will be 3
}</p></div>

<p>
The code above features this simple case where a function can call a nested one. At the end of the function <code>f</code>, <code>x</code> will have the value <code>2</code> because the nested function <code>g</code> modifies the variable <code>x</code>, also modified by <code>f</code> itself.
</p>
<p>
To access to <code>x</code> from <code>g</code> we need to get the previous <code>fp</code>. Since only <code>f</code> can call us, once we get this previous <code>fp</code>, it will be like the <code>fp</code> we had inside <code>f</code>. So it is now a matter of using the same offset as <code>f</code> uses.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> nested01<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
f<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> fp<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">/*</span> keep registers <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> fp<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">/*</span> keep dynamic link <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">sub</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span>      <span style="color: #339933;">/*</span> make room for x <span style="color: #009900; font-weight: bold;">(</span><span style="color: #ff0000;">4</span> bytes<span style="color: #009900; font-weight: bold;">)</span>
                           plus <span style="color: #ff0000;">4</span> bytes to keep <span style="color: #0000ff; font-weight: bold;">stack</span>
                           aligned <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> x is <span style="color: #00007f; font-weight: bold;">in</span> address <span style="color: #7f007f;">"fp - 4"</span> <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>          <span style="color: #339933;">/*</span> r4 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> r4<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>   <span style="color: #339933;">/*</span> x ← r4 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #46aa03; font-weight: bold;">bl</span> g                <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> <span style="color: #009900; font-weight: bold;">(</span>nested function<span style="color: #009900; font-weight: bold;">)</span> g
                           <span style="color: #009900; font-weight: bold;">(</span>the <span style="color: #0000ff; font-weight: bold;">code</span> of <span style="color: #7f007f;">'g'</span> is given below<span style="color: #339933;">,</span> after <span style="color: #7f007f;">'f'</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
&nbsp;
    ldr r4<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>   <span style="color: #339933;">/*</span> r4 ← x <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>      <span style="color: #339933;">/*</span> r4 ← r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> r4<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>   <span style="color: #339933;">/*</span> x ← r4 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> fp <span style="color: #339933;">/*</span> restore dynamic link <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> fp<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">/*</span> restore registers <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr <span style="color: #339933;">/*</span> return <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> nested function g <span style="color: #339933;">*/</span>
    g<span style="color: #339933;">:</span>
        <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> fp<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">/*</span> keep registers <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">mov</span> fp<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">/*</span> keep dynamic link <span style="color: #339933;">*/</span>
&nbsp;
        <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">At</span> this point our <span style="color: #0000ff; font-weight: bold;">stack</span> looks like this
&nbsp;
          <span style="color: #0000ff; font-weight: bold;">Data</span> | Address | Notes
         <span style="color: #339933;">------+---------+--------------------------</span>
           r4  | fp      |  
           r5  | fp <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span>  |
           fp  | fp <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span>  | This is the previous fp
           lr  | fp <span style="color: #339933;">+</span> <span style="color: #ff0000;">16</span> |
        <span style="color: #339933;">*/</span>
&nbsp;
        ldr r4<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #<span style="color: #339933;">+</span><span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">/*</span> get the frame pointer
                             of my caller
                             <span style="color: #009900; font-weight: bold;">(</span>since only f can <span style="color: #00007f; font-weight: bold;">call</span> me<span style="color: #009900; font-weight: bold;">)</span>
                           <span style="color: #339933;">*/</span>
&nbsp;
        <span style="color: #339933;">/*</span> now r4 acts like the fp we had inside <span style="color: #7f007f;">'f'</span> <span style="color: #339933;">*/</span>
        ldr r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">/*</span> r5 ← x <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">add</span> r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>    <span style="color: #339933;">/*</span> r5 ← r5 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">str</span> r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">/*</span> x ← r5 <span style="color: #339933;">*/</span>
&nbsp;
        <span style="color: #00007f; font-weight: bold;">mov</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> fp <span style="color: #339933;">/*</span> restore dynamic link <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> fp<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">/*</span> restore registers <span style="color: #339933;">*/</span>
        <span style="color: #46aa03; font-weight: bold;">bx</span> lr <span style="color: #339933;">/*</span> return <span style="color: #339933;">*/</span>
&nbsp;
<span style="color: #339933;">.</span>globl main
&nbsp;
main <span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">/*</span> keep registers <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #46aa03; font-weight: bold;">bl</span> f          <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> f <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* nested01.s */

.text

f:
    push {r4, r5, fp, lr} /* keep registers */
    mov fp, sp /* keep dynamic link */

    sub sp, sp, #8      /* make room for x (4 bytes)
                           plus 4 bytes to keep stack
                           aligned */
    /* x is in address "fp - 4" */

    mov r4, #1          /* r4 ← 0 */
    str r4, [fp, #-4]   /* x ← r4 */

    bl g                /* call (nested function) g
                           (the code of 'g' is given below, after 'f') */

    ldr r4, [fp, #-4]   /* r4 ← x */
    add r4, r4, #1      /* r4 ← r4 + 1 */
    str r4, [fp, #-4]   /* x ← r4 */

    mov sp, fp /* restore dynamic link */
    pop {r4, r5, fp, lr} /* restore registers */
    bx lr /* return */

    /* nested function g */
    g:
        push {r4, r5, fp, lr} /* keep registers */
        mov fp, sp /* keep dynamic link */

        /* At this point our stack looks like this

          Data | Address | Notes
         ------+---------+--------------------------
           r4  | fp      |  
           r5  | fp + 4  |
           fp  | fp + 8  | This is the previous fp
           lr  | fp + 16 |
        */

        ldr r4, [fp, #+8] /* get the frame pointer
                             of my caller
                             (since only f can call me)
                           */

        /* now r4 acts like the fp we had inside 'f' */
        ldr r5, [r4, #-4] /* r5 ← x */
        add r5, r5, #1    /* r5 ← r5 + 1 */
        str r5, [r4, #-4] /* x ← r5 */

        mov sp, fp /* restore dynamic link */
        pop {r4, r5, fp, lr} /* restore registers */
        bx lr /* return */

.globl main

main :
    push {r4, lr} /* keep registers */

    bl f          /* call f */

    mov r0, #0
    pop {r4, lr}
    bx lr</p></div>

<p>
Ok, the essential idea is set. When accessing a local variable, we always need to get the frame pointer of the function where the local variable belongs. In line 43 we get the frame pointer of our caller and then we use it to access the variable <code>x</code>, lines 49 to 51. Of course, if the local variable belongs to the current function, nothing special has to be done since fp suffices, see lines 20 to 22.
</p>
<p>
That said, though the idea is fundamentally correct, using the dynamic link limits us a lot: only a single call from an enclosing function is possible. What if we allow nested functions to call other nested functions (sibling functions) or worse, what would have happened if <code>g</code> above called itself recursively? The dynamic link we will find in the stack will always refer to the previous dynamically activated function, and in the example above it was <code>f</code>, but if <code>g</code> recursively calls itself, <code>g</code> will be the previous dynamically activated function!
</p>
<p>
It is clear that something is amiss. Using the dynamic link is not right because, when accessing a local variable of an enclosing function, we need to get the last activation of that enclosing function at the point where the nested function was called. The way to keep the last activation of the enclosing function is called <em>static link</em> in contrast to the dynamic link.
</p>
<p>
The static link is conceptually simple, it is also a chain of frame pointers like the dynamic link. In contrast to the dynamic link, which is always set the same way by the callee), the static link may be set differently depending on which function is being called and it will be set by the caller. Below we will see the exact rules.
</p>
<p>
Consider the following more contrived example;
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #993333;">void</span> f<span style="color: #009900;">(</span><span style="color: #993333;">void</span><span style="color: #009900;">)</span> <span style="color: #666666; font-style: italic;">// non nested (nesting depth = 0)</span>
<span style="color: #009900;">{</span>
   <span style="color: #993333;">int</span> x<span style="color: #339933;">;</span>
&nbsp;
   <span style="color: #993333;">void</span> g<span style="color: #009900;">(</span><span style="color: #009900;">)</span> <span style="color: #666666; font-style: italic;">// nested (nesting depth = 1)</span>
   <span style="color: #009900;">{</span>
      x <span style="color: #339933;">=</span> x <span style="color: #339933;">+</span> <span style="color: #0000dd;">1</span><span style="color: #339933;">;</span> <span style="color: #666666; font-style: italic;">// x ← x + 1</span>
   <span style="color: #009900;">}</span>
   <span style="color: #993333;">void</span> h<span style="color: #009900;">(</span><span style="color: #009900;">)</span> <span style="color: #666666; font-style: italic;">// nested (nesting depth = 1)</span>
   <span style="color: #009900;">{</span>
      <span style="color: #993333;">void</span> m<span style="color: #009900;">(</span><span style="color: #009900;">)</span> <span style="color: #666666; font-style: italic;">// nested (nesting depth = 2)</span>
      <span style="color: #009900;">{</span>
         x <span style="color: #339933;">=</span> x <span style="color: #339933;">+</span> <span style="color: #0000dd;">2</span><span style="color: #339933;">;</span> <span style="color: #666666; font-style: italic;">// x ← x + 2</span>
         g<span style="color: #009900;">(</span><span style="color: #009900;">)</span><span style="color: #339933;">;</span> <span style="color: #666666; font-style: italic;">// call g</span>
      <span style="color: #009900;">}</span>
&nbsp;
      g<span style="color: #009900;">(</span><span style="color: #009900;">)</span><span style="color: #339933;">;</span> <span style="color: #666666; font-style: italic;">// call g</span>
      m<span style="color: #009900;">(</span><span style="color: #009900;">)</span><span style="color: #339933;">;</span> <span style="color: #666666; font-style: italic;">// call m</span>
      x <span style="color: #339933;">=</span> x <span style="color: #339933;">+</span> <span style="color: #0000dd;">3</span><span style="color: #339933;">;</span> <span style="color: #666666; font-style: italic;">// x ← x + 3</span>
   <span style="color: #009900;">}</span>
&nbsp;
   x <span style="color: #339933;">=</span> <span style="color: #0000dd;">1</span><span style="color: #339933;">;</span> <span style="color: #666666; font-style: italic;">// x ← 1</span>
   h<span style="color: #009900;">(</span><span style="color: #009900;">)</span><span style="color: #339933;">;</span>  <span style="color: #666666; font-style: italic;">// call h</span>
   <span style="color: #666666; font-style: italic;">// here x will be 8</span>
<span style="color: #009900;">}</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">void f(void) // non nested (nesting depth = 0)
{
   int x;

   void g() // nested (nesting depth = 1)
   {
      x = x + 1; // x ← x + 1
   }
   void h() // nested (nesting depth = 1)
   {
      void m() // nested (nesting depth = 2)
      {
         x = x + 2; // x ← x + 2
         g(); // call g
      }

      g(); // call g
      m(); // call m
      x = x + 3; // x ← x + 3
   }

   x = 1; // x ← 1
   h();  // call h
   // here x will be 8
}</p></div>

<p>
A function can, obviously, call an immediately nested function. So from the body of function <code>f</code> we can call <code>g</code> or <code>h</code>. Similarly from the body of function <code>h</code> we can call <code>m</code>. A function can be called by other (non-immediately nested) functions as long as the nesting depth of the caller is greater or equal than the callee. So from <code>m</code> we can call <code>m</code> (recursively), <code>h</code>, <code>g</code> and <code>f</code>. It would not be allowed that <code>f</code> or <code>g</code> called <code>m</code>.
</p>
<p>
Note that <code>h</code> and <code>g</code> are both enclosed by <code>f</code>. So when they are called, their dynamic link will be of course the caller but their static link must always point to the frame of <code>f</code>. On the other hand, <code>m</code> is enclosed by <code>h</code>, so its static link will point to the frame of <code>h</code> (and in the example, its dynamic link too because it is the only nested function inside <code>h</code> and it does not call itself recursively either). When <code>m</code> calls <code>g</code>, the static link must be again the frame of its enclosing function <code>f</code>.
</p>
<h3>Setting up a static link</h3>
<p>
Like it happens with the dynamic link, the AAPCS does not mandate any register to be used as the static link. In fact, any callee-saved register that does not have any specific purpose will do. We will use <code>r10</code>.
</p>
<p>
Setting up the static link is a bit more involved because it requires paying attention which function we are calling. There are two cases:
</p>
<ul style="list-style-type: upper-roman;">
<li>The function is immediately nested (like when from <code>f</code> we call <code>g</code> or <code>h</code>, or when from <code>h</code> we call <code>m</code>). The static link is simply the frame pointer of the caller.<bl><br>
For theses cases, thus, the following is all we have to do prior the call.<p></p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">mov</span> <span style="color: #46aa03; font-weight: bold;">r10</span><span style="color: #339933;">,</span> fp
<span style="color: #46aa03; font-weight: bold;">bl</span> immediately<span style="color: #339933;">-</span>nested<span style="color: #339933;">-</span>function</pre></td></tr></tbody></table><p class="theCode" style="display:none;">mov r10, fp
bl immediately-nested-function</p></div>

</bl></li><li>The function is not immediately nested (like when from <code>m</code> we call <code>g</code>) then the static frame must be that of the enclosing function of the callee. Since the static link forms a chain it is just a matter of advancing in the chain as many times as the difference of nesting depths.<bl><br>
For instance, when <code>m</code> calls <code>g</code>, the static link of <code>m</code> is the frame of <code>h</code>. At the same time the static link of <code>h</code> is the frame of <code>f</code>. Since <code>g</code> and <code>h</code> are siblings, their static link must be the same. So when <code>m</code> calls <code>g</code>, the static link should be the same of <code>h</code>.<bl><br>
For theses cases, we will have to do the following<p></p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">ldr <span style="color: #46aa03; font-weight: bold;">r10</span><span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #X0<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">/*</span> Xi will be the appropiate offset 
                     where the previous value of <span style="color: #46aa03; font-weight: bold;">r10</span> is found
                     Note that Xi depends on the layout of
                     our <span style="color: #0000ff; font-weight: bold;">stack</span> after we have <span style="color: #00007f; font-weight: bold;">push</span><span style="color: #339933;">-</span>ed the
                     caller<span style="color: #339933;">-</span>saved registers
                   <span style="color: #339933;">*/</span>
ldr <span style="color: #46aa03; font-weight: bold;">r10</span><span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">r10</span><span style="color: #339933;">,</span> #X1<span style="color: #009900; font-weight: bold;">]</span>  \
ldr <span style="color: #46aa03; font-weight: bold;">r10</span><span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">r10</span><span style="color: #339933;">,</span> #X2<span style="color: #009900; font-weight: bold;">]</span>   |
<span style="color: #339933;">...</span>                   | advance the static link as many <span style="color: #0000ff; font-weight: bold;">times</span>
<span style="color: #339933;">...</span>                   | the difference of the nesting depth
<span style="color: #339933;">...</span>                   | <span style="color: #009900; font-weight: bold;">(</span>it may be zero <span style="color: #0000ff; font-weight: bold;">times</span> when calling a sibling<span style="color: #009900; font-weight: bold;">)</span>
ldr <span style="color: #46aa03; font-weight: bold;">r10</span><span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">r10</span><span style="color: #339933;">,</span> #Xn<span style="color: #009900; font-weight: bold;">]</span>  <span style="color: #339933;">/</span>
<span style="color: #46aa03; font-weight: bold;">bl</span> non<span style="color: #339933;">-</span>immediately<span style="color: #339933;">-</span>nested<span style="color: #339933;">-</span>function</pre></td></tr></tbody></table><p class="theCode" style="display:none;">ldr r10, [fp, #X0] /* Xi will be the appropiate offset 
                     where the previous value of r10 is found
                     Note that Xi depends on the layout of
                     our stack after we have push-ed the
                     caller-saved registers
                   */
ldr r10, [r10, #X1]  \
ldr r10, [r10, #X2]   |
...                   | advance the static link as many times
...                   | the difference of the nesting depth
...                   | (it may be zero times when calling a sibling)
ldr r10, [r10, #Xn]  /
bl non-immediately-nested-function</p></div>

</bl></bl></li></ul>
<p>
This may seem very complicated but it is not. Since in the example above there are a few functions, we will do one function at a time. Let’s start with <code>f</code>.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>31
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">f<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r10</span><span style="color: #339933;">,</span> fp<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">/*</span> keep registers <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> fp<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span>             <span style="color: #339933;">/*</span> setup dynamic link <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">sub</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span>      <span style="color: #339933;">/*</span> make room for x <span style="color: #009900; font-weight: bold;">(</span><span style="color: #ff0000;">4</span> <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span> bytes<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> x will be <span style="color: #00007f; font-weight: bold;">in</span> address <span style="color: #7f007f;">"fp - 4"</span> <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">At</span> this point our <span style="color: #0000ff; font-weight: bold;">stack</span> looks like this
&nbsp;
     <span style="color: #0000ff; font-weight: bold;">Data</span> | Address | Notes
    <span style="color: #339933;">------+---------+---------------------------</span>
          | fp <span style="color: #339933;">-</span> <span style="color: #ff0000;">8</span>  | alignment <span style="color: #009900; font-weight: bold;">(</span>per AAPCS<span style="color: #009900; font-weight: bold;">)</span>
      x   | fp <span style="color: #339933;">-</span> <span style="color: #ff0000;">4</span>  | 
      r4  | fp      |  
      <span style="color: #46aa03; font-weight: bold;">r10</span> | fp <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span>  | previous value of <span style="color: #46aa03; font-weight: bold;">r10</span>
      fp  | fp <span style="color: #339933;">+</span> <span style="color: #ff0000;">12</span> | previous value of fp
      lr  | fp <span style="color: #339933;">+</span> <span style="color: #ff0000;">16</span> |
   <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>          <span style="color: #339933;">/*</span> r4 ← <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> r4<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>   <span style="color: #339933;">/*</span> x ← r4 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> prepare the <span style="color: #00007f; font-weight: bold;">call</span> to h <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> <span style="color: #46aa03; font-weight: bold;">r10</span><span style="color: #339933;">,</span> fp <span style="color: #339933;">/*</span> setup the static link<span style="color: #339933;">,</span>
                   since we are calling an immediately nested function
                   it is just the current frame <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> h        <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> h <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> fp             <span style="color: #339933;">/*</span> restore <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r10</span><span style="color: #339933;">,</span> fp<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>  <span style="color: #339933;">/*</span> restore registers <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr <span style="color: #339933;">/*</span> return <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">f:
    push {r4, r10, fp, lr} /* keep registers */
    mov fp, sp             /* setup dynamic link */

    sub sp, sp, #8      /* make room for x (4 + 4 bytes) */
    /* x will be in address "fp - 4" */

    /* At this point our stack looks like this

     Data | Address | Notes
    ------+---------+---------------------------
          | fp - 8  | alignment (per AAPCS)
      x   | fp - 4  | 
      r4  | fp      |  
      r10 | fp + 8  | previous value of r10
      fp  | fp + 12 | previous value of fp
      lr  | fp + 16 |
   */

    mov r4, #1          /* r4 ← 1 */
    str r4, [fp, #-4]   /* x ← r4 */

    /* prepare the call to h */
    mov r10, fp /* setup the static link,
                   since we are calling an immediately nested function
                   it is just the current frame */
    bl h        /* call h */

    mov sp, fp             /* restore stack */
    pop {r4, r10, fp, lr}  /* restore registers */
    bx lr /* return */</p></div>

<p>
Since <code>f</code> is not nested in any other function, the previous value of <code>r10</code> does not have any special meaning for us. We just keep it because <code>r10</code>, despite the special meaning we will give it, is still a callee-saved register as mandated by the AAPCS. At the beginning, we allocate space for the variable <code>x</code> by enlarging the stack (line 35). Variable <code>x</code> will be always in <code>fp - 4</code>. Then we set <code>x</code> to 1 (line 51). Nothing fancy here since this is a non-nested function.
</p>
<p>
Now f calls h (line 57). Since it is an immediately nested function, the static link is as in the case I: the current frame pointer. So we just set <code>r10</code> to be <code>fp</code> (line 56).
</p>
<p>
Let’s see the code of <code>h</code> now.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>63
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> <span style="color: #339933;">------</span> nested function <span style="color: #339933;">------------------</span> <span style="color: #339933;">*/</span>
h <span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r10</span><span style="color: #339933;">,</span> fp<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">/*</span> keep registers <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> fp<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">/*</span> setup dynamic link <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">sub</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span> <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">At</span> this point our <span style="color: #0000ff; font-weight: bold;">stack</span> looks like this
&nbsp;
      <span style="color: #0000ff; font-weight: bold;">Data</span> | Address | Notes
     <span style="color: #339933;">------+---------+---------------------------</span>
           | fp <span style="color: #339933;">-</span> <span style="color: #ff0000;">4</span>  | alignment <span style="color: #009900; font-weight: bold;">(</span>per AAPCS<span style="color: #009900; font-weight: bold;">)</span>
       r4  | fp      |  
       r5  | fp <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span>  | 
       <span style="color: #46aa03; font-weight: bold;">r10</span> | fp <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span>  | frame pointer of <span style="color: #7f007f;">'f'</span>
       fp  | fp <span style="color: #339933;">+</span> <span style="color: #ff0000;">12</span> | frame pointer of caller
       lr  | fp <span style="color: #339933;">+</span> <span style="color: #ff0000;">16</span> |
    <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> prepare <span style="color: #00007f; font-weight: bold;">call</span> to g <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> g is a sibling so the static link will be the same
       as the current one <span style="color: #339933;">*/</span>
    ldr <span style="color: #46aa03; font-weight: bold;">r10</span><span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> g
&nbsp;
    <span style="color: #339933;">/*</span> prepare <span style="color: #00007f; font-weight: bold;">call</span> to m <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> m is an immediately nested function so the static
       link is the current frame <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> <span style="color: #46aa03; font-weight: bold;">r10</span><span style="color: #339933;">,</span> fp
    <span style="color: #46aa03; font-weight: bold;">bl</span> m
&nbsp;
    ldr r4<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span>  <span style="color: #339933;">/*</span> load frame pointer of <span style="color: #7f007f;">'f'</span> <span style="color: #339933;">*/</span>
    ldr r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>  <span style="color: #339933;">/*</span> r5 ← x <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">3</span>     <span style="color: #339933;">/*</span> r5 ← r5 <span style="color: #339933;">+</span> <span style="color: #ff0000;">3</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>  <span style="color: #339933;">/*</span> x ← r5 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> fp            <span style="color: #339933;">/*</span> restore <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r10</span><span style="color: #339933;">,</span> fp<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">/*</span> restore registers <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* ------ nested function ------------------ */
h :
    push {r4, r5, r10, fp, lr} /* keep registers */
    mov fp, sp /* setup dynamic link */

    sub sp, sp, #4 /* align stack */

    /* At this point our stack looks like this

      Data | Address | Notes
     ------+---------+---------------------------
           | fp - 4  | alignment (per AAPCS)
       r4  | fp      |  
       r5  | fp + 4  | 
       r10 | fp + 8  | frame pointer of 'f'
       fp  | fp + 12 | frame pointer of caller
       lr  | fp + 16 |
    */

    /* prepare call to g */
    /* g is a sibling so the static link will be the same
       as the current one */
    ldr r10, [fp, #8]
    bl g

    /* prepare call to m */
    /* m is an immediately nested function so the static
       link is the current frame */
    mov r10, fp
    bl m

    ldr r4, [fp, #8]  /* load frame pointer of 'f' */
    ldr r5, [r4, #-4]  /* r5 ← x */
    add r5, r5, #3     /* r5 ← r5 + 3 */
    str r5, [r4, #-4]  /* x ← r5 */

    mov sp, fp            /* restore stack */
    pop {r4, r5, r10, fp, lr} /* restore registers */
    bx lr</p></div>

<p>
We start the function as usual, pushing registers onto the stack and setting up the dynamic link (lines 64 to 65). We adjust the stack so the stack pointer is 8-byte aligned because we have pushed an even number of registers (line 68). If you check the layout of the stack after this last adjustment (depicted in lines 72 to 79), you will see that in <code>fp + 8</code> we have the value of <code>r10</code> which the caller of <code>h</code> (in this example only <code>f</code>, but it could be another function) must ensure that is the frame pointer of <code>f</code>. This extra pointer in the stack is the static link.
</p>
<p>
Now the function calls <code>g</code> (line 86) but it must properly set the static link prior to the call. In this case the static link is the same as <code>h</code> because we call <code>g</code> which is a sibling of <code>h</code>, so they share the same static link. We get it from <code>fp + 8</code> (line 85). This is in fact the case II described above: <code>g</code> is not an immediately nested function of <code>h</code>. So we have to get the static link of the caller (the static link of <code>h</code>, found in <code>fp + 8</code>) and then advance it as many times as the difference of their nesting depths. Being siblings means that their nesting depths are the same, so no advancement is actually required.
</p>
<p>
After the call to <code>g</code>, the function calls <code>m</code> (line 92) which happens to be an immediately nested function, so its static link is the current frame pointer (line 91) because this is again the case I.
</p>
<p>
Let’s see now the code of <code>m</code>.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>104
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> <span style="color: #339933;">------</span> nested function <span style="color: #339933;">------------------</span> <span style="color: #339933;">*/</span>
m<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r10</span><span style="color: #339933;">,</span> fp<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">/*</span> keep registers <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> fp<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">/*</span> setup dynamic link <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">sub</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span> <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">At</span> this point our <span style="color: #0000ff; font-weight: bold;">stack</span> looks like this
&nbsp;
      <span style="color: #0000ff; font-weight: bold;">Data</span> | Address | Notes
     <span style="color: #339933;">------+---------+---------------------------</span>
           | fp <span style="color: #339933;">-</span> <span style="color: #ff0000;">4</span>  | alignment <span style="color: #009900; font-weight: bold;">(</span>per AAPCS<span style="color: #009900; font-weight: bold;">)</span>
       r4  | fp      |  
       r5  | fp <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span>  |
       <span style="color: #46aa03; font-weight: bold;">r10</span> | fp <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span>  | frame pointer of <span style="color: #7f007f;">'h'</span>
       fp  | fp <span style="color: #339933;">+</span> <span style="color: #ff0000;">12</span> | frame pointer of caller
       lr  | fp <span style="color: #339933;">+</span> <span style="color: #ff0000;">16</span> |
    <span style="color: #339933;">*/</span>
&nbsp;
    ldr r4<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span>  <span style="color: #339933;">/*</span> r4 ← frame pointer of <span style="color: #7f007f;">'h'</span> <span style="color: #339933;">*/</span>
    ldr r4<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span>  <span style="color: #339933;">/*</span> r4 ← frame pointer of <span style="color: #7f007f;">'f'</span> <span style="color: #339933;">*/</span>
    ldr r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">/*</span> r5 ← x <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span>    <span style="color: #339933;">/*</span> r5 ← r5 <span style="color: #339933;">+</span> <span style="color: #ff0000;">2</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">/*</span> x ← r5 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> setup <span style="color: #00007f; font-weight: bold;">call</span> to g <span style="color: #339933;">*/</span>
    ldr <span style="color: #46aa03; font-weight: bold;">r10</span><span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span>   <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r10</span> ← frame pointer of <span style="color: #7f007f;">'h'</span> <span style="color: #339933;">*/</span>
    ldr <span style="color: #46aa03; font-weight: bold;">r10</span><span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">r10</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span>  <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r10</span> ← frame pointer of <span style="color: #7f007f;">'f'</span> <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> g
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> fp                <span style="color: #339933;">/*</span> restore <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r10</span><span style="color: #339933;">,</span> fp<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">/*</span> restore registers <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* ------ nested function ------------------ */
m:
    push {r4, r5, r10, fp, lr} /* keep registers */
    mov fp, sp /* setup dynamic link */

    sub sp, sp, #4 /* align stack */
    /* At this point our stack looks like this

      Data | Address | Notes
     ------+---------+---------------------------
           | fp - 4  | alignment (per AAPCS)
       r4  | fp      |  
       r5  | fp + 4  |
       r10 | fp + 8  | frame pointer of 'h'
       fp  | fp + 12 | frame pointer of caller
       lr  | fp + 16 |
    */

    ldr r4, [fp, #8]  /* r4 ← frame pointer of 'h' */
    ldr r4, [r4, #8]  /* r4 ← frame pointer of 'f' */
    ldr r5, [r4, #-4] /* r5 ← x */
    add r5, r5, #2    /* r5 ← r5 + 2 */
    str r5, [r4, #-4] /* x ← r5 */

    /* setup call to g */
    ldr r10, [fp, #8]   /* r10 ← frame pointer of 'h' */
    ldr r10, [r10, #8]  /* r10 ← frame pointer of 'f' */
    bl g

    mov sp, fp                /* restore stack */
    pop {r4, r5, r10, fp, lr} /* restore registers */
    bx lr</p></div>

<p>
Function <code>m</code> starts pretty similar to h: we push the registers, setup the dynamic link and adjust the stack so it is 8-byte aligned (lines 106 to 109). After this, we again have the static link at <code>fp + 8</code>. If you are wondering if the static link will always be in <code>fp + 8</code>, the answer is no, it depends on how many registers are pushed before <code>r10</code>, it just happens that we always push <code>r4</code> and <code>r5</code>, but if we, for instance, also pushed <code>r6</code> it would be at a larger offset. Each function may have the static link at different offsets (this is why we are drawing the stack layout for every function, bear this in mind!).
</p>
<p>
The first thing m does is <code>x ← x + 2</code>. So we have to get the address of <code>x</code>. The address of <code>x</code> is relative to the frame pointer of <code>f</code> because <code>x</code> is a local variable of <code>f</code>. We do not have the frame pointer of <code>f</code> but the one of <code>h</code> (this is the static link of <code>m</code>). Since the frame pointers form a chain, we can load the frame pointer of <code>h</code> and then use it to get the static link of <code>h</code> which will be the frame pointer of <code>f</code>. You may have to reread this last statement twice  So we first get the frame pointer of <code>h</code> (line 122), recall that this is the static link of <code>m</code> that was set up when <code>h</code> called <code>m</code> (line 91). Now we have the frame pointer of h, so we can get its static link (line 123) which again is at offset <code>+8</code> but this is by chance, it could be in a different offset! The static link of <code>h</code> is the frame pointer of <code>f</code>, so we now have the frame pointer <code>f</code> as we wanted and then we can proceed to get the address of <code>x</code>, which is at offset <code>-4</code> of the frame pointer of <code>f</code>. With this address now we can perform <code>x ← x + 2</code> (lines 124 to 126).
</p>
<p>
Then <code>m</code> calls <code>g</code> (line 131). This is again a case II. But this time <code>g</code> is not a sibling of <code>m</code>: their nesting depths differ by 1. So we first load the current static link (line 129), the frame pointer of h. And then we advance 1 link through the chain of static links (line 130). Let me insist again: it is by chance that the static link of <code>h</code> and <code>f</code> is found at <code>fp+8</code>, each function could have it at different offsets.
</p>
<p>
Let’s see now the code of g, which is pretty similar to that of h except that it does not call anyone.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>127
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
152
153
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> <span style="color: #339933;">------</span> nested function <span style="color: #339933;">------------------</span> <span style="color: #339933;">*/</span>
g<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r10</span><span style="color: #339933;">,</span> fp<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">/*</span> keep registers <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> fp<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">/*</span> setup dynamic link <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">sub</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span> <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">At</span> this point our <span style="color: #0000ff; font-weight: bold;">stack</span> looks like this
&nbsp;
      <span style="color: #0000ff; font-weight: bold;">Data</span> | Address | Notes
     <span style="color: #339933;">------+---------+---------------------------</span>
           | fp <span style="color: #339933;">-</span> <span style="color: #ff0000;">4</span>  | alignment <span style="color: #009900; font-weight: bold;">(</span>per AAPCS<span style="color: #009900; font-weight: bold;">)</span>
       r4  | fp      |  
       r5  | fp <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span>  |  
       <span style="color: #46aa03; font-weight: bold;">r10</span> | fp <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span>  | frame pointer of <span style="color: #7f007f;">'f'</span>
       fp  | fp <span style="color: #339933;">+</span> <span style="color: #ff0000;">12</span> | frame pointer of caller
       lr  | fp <span style="color: #339933;">+</span> <span style="color: #ff0000;">16</span> |
    <span style="color: #339933;">*/</span>
&nbsp;
    ldr r4<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span>  <span style="color: #339933;">/*</span> r4 ← frame pointer of <span style="color: #7f007f;">'f'</span> <span style="color: #339933;">*/</span>
    ldr r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">/*</span> r5 ← x <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>    <span style="color: #339933;">/*</span> r5 ← r5 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">/*</span> x ← r5 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> fp <span style="color: #339933;">/*</span> restore dynamic link <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r10</span><span style="color: #339933;">,</span> fp<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">/*</span> restore registers <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* ------ nested function ------------------ */
g:
    push {r4, r5, r10, fp, lr} /* keep registers */
    mov fp, sp /* setup dynamic link */

    sub sp, sp, #4 /* align stack */

    /* At this point our stack looks like this

      Data | Address | Notes
     ------+---------+---------------------------
           | fp - 4  | alignment (per AAPCS)
       r4  | fp      |  
       r5  | fp + 4  |  
       r10 | fp + 8  | frame pointer of 'f'
       fp  | fp + 12 | frame pointer of caller
       lr  | fp + 16 |
    */

    ldr r4, [fp, #8]  /* r4 ← frame pointer of 'f' */
    ldr r5, [r4, #-4] /* r5 ← x */
    add r5, r5, #1    /* r5 ← r5 + 1 */
    str r5, [r4, #-4] /* x ← r5 */

    mov sp, fp /* restore dynamic link */
    pop {r4, r5, r10, fp, lr} /* restore registers */
    bx lr</p></div>

<p>
Note that <code>h</code> and <code>g</code> compute the address of <code>x</code> exactly the same way since they are at the same nesting depth.
</p>
<p>
Below is a picture of how the layout looks once <code>m</code> has called <code>g</code>. Note that the static link of <code>g</code> and <code>h</code> is the same, the frame pointer of <code>f</code>, because they are siblings.
</p>
<p><img src="images/static_link.png" alt="Depiction of the stack layout when g has been called by m" class="aligncenter size-full wp-image-1863" scale="0"></p>
<p>
Below is the same image but this time using coloured lines to show how each function can compute the address of <code>x</code>.
</p>
<p><img src="images/static_link_2.png" alt="Depiction of the stack layout when g has been called by m that shows how each function can compute the address of x" class="aligncenter size-full wp-image-1870" scale="0"></p>
<p>
Finally here is the <code>main</code>. Note that when a non-nested function calls another non-nested function, there is no need to do anything to <code>r10</code>. This is the reason why <code>r10</code> does not have any meaningful value upon the entry to <code>f</code>.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>152
153
154
155
156
157
158
159
160
161
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">.</span>globl main
&nbsp;
main <span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">/*</span> keep registers <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #46aa03; font-weight: bold;">bl</span> f          <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> f <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">.globl main

main :
    push {r4, lr} /* keep registers */

    bl f          /* call f */

    mov r0, #0
    pop {r4, lr}
    bx lr</p></div>

<h2>Discussion</h2>
<p>
If you stop and think about all this stuff of the static link you may soon realize that there is something murky with all this nested functions business: we are passing some sort of hidden parameter (through <code>r10</code>) to the nested functions. In fact, we are somehow cheating, because we set <code>r10</code> right before the call and then we push it at the entry of the nested functions even if they do not modify it in the called function. Why are we doing this seemingly useless step?
</p>
<p>
Well, by always pushing <code>r10</code> in the stack, we are just covering up the naked truth: nested functions require a, somewhat hidden, extra parameter. This extra parameter is this static link thing. Sometimes it is also called the <em>lexical scope</em>. It is called the lexical scope because it gives us the <em>context</em> of the lexically (i.e. in the code) enclosing function (in contrast the dynamic scope would be that of our caller, which we do not care about unless we are a debugger). With that lexical context we can get the local variables of that enclosing function. Due to the chaining nature of the static link, we can move up the lexical scopes. This is the reason <code>m</code> can access a variable of <code>f</code>, it just <em>climbs</em> through the static links as shown in the last picture above.
</p>
<p>
Can we pass the lexical scope to a function using the stack, rather than a callee-saved register? Sure. For convenience it may have to be the first stack-passed parameter (so its offset from <code>fp</code> is easy to compute). Instead of setting <code>r10</code> prior the call, we will enlarge <code>sp</code> as needed (at least 8 bytes, to keep the stack 8-byte aligned) and then store there the static link. In the stack layout, the static link now will be found after (i.e. larger offsets than) the pushed registers.
</p>
<p>
Can we pass the lexical scope using a caller-saved register (like <code>r0</code>, <code>r1</code>, <code>r2</code> or <code>r3</code>)? Yes, but the first thing we should do is to keep it in the stack, as a local variable (i.e. negative offsets from <code>fp</code>). Why? Because if we do not keep it in the stack we will not be able to move upwards the static links.
</p>
<p>
As you can see, any approach requires us to keep the static link in the stack. While our approach of using <code>r10</code> may not be completely orthodox ends doing the right thing.
</p>
<p>
But the discussion would not be complete if we did not talk about pointers. What about a pointer to a nested function? Is that even possible? When (directly) calling a nested function we can set the lexical scope appropiately because we know everything: we know where we are and we know which function we are going to call. But what about an indirect call using a pointer to a function? We do not know which (possibly nested) function we are going to call, how can we appropriately set its lexical scope. Well, the answer is, we cannot unless we keep the lexical scope somewhere. This means that just the address of the function will not do. We will need to keep, along with the address to the function, the lexical scope. So a pointer to a nested function happens to be different to a pointer to a non-nested function, given that the latter does not need the lexical scope to be set.
</p>
<p>
Having incompatible pointers for nested an non nested functions is not desirable. This may be a reason why C (and C++) do not directly support nested functions (albeit this limitation can be worked around using other approaches). In the next chapter, we will see a clever approach to avoid, to some extent, having different pointers to nested functions that are different from pointers to non nested functions.
</p>
<p>
That’s all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2015/01/02/arm-assembler-raspberry-pi-chapter-23/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2015/01/02/arm-assembler-raspberry-pi-chapter-23/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2015/01/02/arm-assembler-raspberry-pi-chapter-23/&amp;text=ARM+assembler+in+Raspberry+Pi+%E2%80%93+Chapter+23+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2015/01/02/arm-assembler-raspberry-pi-chapter-23/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>						<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2014/12/20/arm-assembler-raspberry-pi-chapter-22/" rel="prev">ARM assembler in Raspberry Pi – Chapter 22</a></span>
			<span class="next"><a href="http://thinkingeek.com/2015/01/03/dvd-with-bogus-permissions-ubuntu/" rel="next">Read DVDs with bogus permissions in Ubuntu</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
			<h3>4 thoughts on “<span>ARM assembler in Raspberry Pi – Chapter 23</span>”</h3>
		<ul class="commentlist">
				<li class="comment even thread-even depth-1 parent" id="comment-767021">
				<div id="div-comment-767021" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-9e81362ff9654533accffc53762c9520-0">			<cite class="fn"><a href="http://devel.meruelo.com.es/blog/" rel="external nofollow" class="url">Fernando</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2015/01/02/arm-assembler-raspberry-pi-chapter-23/#comment-767021">
			January 18, 2015 at 12:42 am</a>		</div>

		<p>It looks like I am your most devoted reader… lol. It took you some time to write something new but now that I check you’ve got three new entries. What a pity that we don’t have even more Christmas holidays <img draggable="false" class="emoji" alt="😉" src="https://s.w.org/images/core/emoji/2.2.1/svg/1f609.svg" scale="0"></p>
<p>On the other hand, something that helped me get things straight: contents between frame pointers pertain to a single function (that is, in the stack, after (lower memory) fp we’ve got local data, data pushed to be passed as parameters and then registers saved by the callee in compliance of AAPCS). On the other hand, the activation record of any function defines the bunch of data used by such a function (thus, arguments passed by caller, arguments pushed to comply AAPCS and local data). Through reading this I’ve been wondering why the fp does not point just to the top of the stack after passing parameters, before pushing callee-saved registers, as it seems easier to access what you really need (even though to exit you will need to ldmib or advance your sp prior to popping).</p>
<p>Also, why would you ever need nested functions? It looks a bit of a mess, not the assembler beneath it but the C-code -hard to follow as hell on big codes, I guess-.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2015/01/02/arm-assembler-raspberry-pi-chapter-23/?replytocom=767021#respond" onclick="return addComment.moveForm( &quot;div-comment-767021&quot;, &quot;767021&quot;, &quot;respond&quot;, &quot;1798&quot; )" aria-label="Reply to Fernando">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-776615">
				<div id="div-comment-776615" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-0">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2015/01/02/arm-assembler-raspberry-pi-chapter-23/#comment-776615">
			January 23, 2015 at 9:36 pm</a>		</div>

		<p>Hi Fernando,</p>
<p>since the AAPCS does not dictate anything about the fp register, your choice is sensible as well. Currently we do something like</p>
<p><code>push callee-saved-registers<br>
mov fp, sp<br>
...<br>
mov sp, fp<br>
pop callee-saved-registers</code></p>
<p>If I understood you correctly, you wonder why not doing instead</p>
<p><code>mov fp, sp<br>
push callee-saved-registers<br>
...<br>
pop callee-saved-registers<br>
mov sp, fp</code></p>
<p>(Note that fp will be in callee-saved-registers in both cases)</p>
<p>I think, that both approaches are fine and probably there is little difference except for the offsets where you will find your data: parameters passed on to the stack will be closer (at positive offsets, higher addresses) while local data it will be slightly farther (at negative offsets, lower addresses) because you have to “jump” the callee-saved registers. But rather than that, there is little difference in one approach and the other. I just felt simpler to add something after (and before) the usual push (and pop) sequences at the beginning (and end) of the functions.</p>
<p>Finally, thanks for following the posts and bringing up good questions like that.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2015/01/02/arm-assembler-raspberry-pi-chapter-23/?replytocom=776615#respond" onclick="return addComment.moveForm( &quot;div-comment-776615&quot;, &quot;776615&quot;, &quot;respond&quot;, &quot;1798&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-816845">
				<div id="div-comment-816845" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/8e02ba12a3b8f9d19306c56fbde0d455?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/8e02ba12a3b8f9d19306c56fbde0d455?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/8e02ba12a3b8f9d19306c56fbde0d455?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-8e02ba12a3b8f9d19306c56fbde0d455-0">			<cite class="fn"><a href="http://pervin@utdallas.edu" rel="external nofollow" class="url">William Pervin</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2015/01/02/arm-assembler-raspberry-pi-chapter-23/#comment-816845">
			March 10, 2015 at 5:26 pm</a>		</div>

		<p>In the first C code I think line 12 should be “// here x will be  3”. I agree with the contrived example that “// here x will be 8”. Got more reading to do since this is, indeed, complicated.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2015/01/02/arm-assembler-raspberry-pi-chapter-23/?replytocom=816845#respond" onclick="return addComment.moveForm( &quot;div-comment-816845&quot;, &quot;816845&quot;, &quot;respond&quot;, &quot;1798&quot; )" aria-label="Reply to William Pervin">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-818011">
				<div id="div-comment-818011" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-1">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2015/01/02/arm-assembler-raspberry-pi-chapter-23/#comment-818011">
			March 11, 2015 at 8:50 pm</a>		</div>

		<p>Hi Williams,</p>
<p>you are right. I already fixed the post.</p>
<p>Thanks a lot.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2015/01/02/arm-assembler-raspberry-pi-chapter-23/?replytocom=818011#respond" onclick="return addComment.moveForm( &quot;div-comment-818011&quot;, &quot;818011&quot;, &quot;respond&quot;, &quot;1798&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		</ul>
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2015/01/02/arm-assembler-raspberry-pi-chapter-23/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="1798" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="960166088d"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496666870476"></form>
			</div><!-- #respond -->
	</div>	</div>
