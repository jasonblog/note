# Passing data to functions


<div class="main">
			<h1>ARM assembler in Raspberry Pi – Chapter 17 </h1>
		<p class="post-meta"><span class="date">November 20, 2013</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/#comments">13</a></span> </p>
		<p>
In chapter 10 we saw the basics to call a function. In this chapter we will cover more topics related to functions.
</p>
<p><span id="more-1349"></span></p>
<h2>Passing data to functions</h2>
<p>
We already know how to call a function and pass them parameters. We also know how to return data from a function. Nevertheless, there are some issues which we have not fully solved yet.
</p>
<ul>
<li>Passing large amounts of data
</li><li>Returning more than one piece of data
</li></ul>
<p>
There are several ways to tackle this problem, but most of them involve pointers. Pointers are dreaded by many people, who do not fully understand them, but they are a crucial part in the way computers work. That said, most of the troubles with pointers are actually related to <em>dynamic memory</em> rather than the pointers themselves. We will not consider dynamic memory here.
</p>
<h2>So what is a pointer?</h2>
<p>
A pointer is some location in the memory the contents of which are simply an address of the memory.
</p>
<p>
This definition may be confusing, but we have already been using pointers in previous chapters. It is just that we did not name them this way. We usually talked about <em>addresses</em> and/or labels in the assembler. Consider this very simple program:
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> first_pointer<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
number_1  <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">3</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
<span style="color: #339933;">.</span>globl main
&nbsp;
main<span style="color: #339933;">:</span>
    ldr r0<span style="color: #339933;">,</span> pointer_to_number    <span style="color: #339933;">/*</span> r0 ← &amp;number <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>                 <span style="color: #339933;">/*</span> r0 ← <span style="color: #339933;">*</span>r0<span style="color: #339933;">.</span> So r0 ← number_1 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr
&nbsp;
pointer_to_number<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> number_1</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* first_pointer.s */

.data

.align 4
number_1  : .word 3

.text
.globl main

main:
    ldr r0, pointer_to_number    /* r0 ← &amp;number */
    ldr r0, [r0]                 /* r0 ← *r0. So r0 ← number_1 */

    bx lr

pointer_to_number: .word number_1</p></div>

<p>
As you can see, I deliberatedly used the name <code>pointer_to_number</code> to express the fact that this location in memory is actually a pointer. It is a pointer to <code>number_1</code> because it holds its address.
</p>
<p>
Imagine we add another number, let’s call it <code>number_2</code> and want <code>pointer_to_number</code> to be able to point to <code>number_2</code>, this is, contain the address of <code>number_2</code> as well. Let’s make a first attempt.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
number_1  <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">3</span>
number_2  <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">4</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
<span style="color: #339933;">.</span>globl main
&nbsp;
main<span style="color: #339933;">:</span>
    ldr r1<span style="color: #339933;">,</span> address_of_number_2  <span style="color: #339933;">/*</span> r1 ← &amp;number_2 <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> r1<span style="color: #339933;">,</span> pointer_to_number    <span style="color: #339933;">/*</span> pointer_to_number ← r1<span style="color: #339933;">,</span> this is pointer_to_number ← &amp;number_2 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr
&nbsp;
pointer_to_number<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> number_1
address_of_number_2<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> number_2</pre></td></tr></tbody></table><p class="theCode" style="display:none;">.data

.align 4
number_1  : .word 3
number_2  : .word 4

.text
.globl main

main:
    ldr r1, address_of_number_2  /* r1 ← &amp;number_2 */
    str r1, pointer_to_number    /* pointer_to_number ← r1, this is pointer_to_number ← &amp;number_2 */

    bx lr

pointer_to_number: .word number_1
address_of_number_2: .word number_2</p></div>

<p>
But if you run this you will get a rude <code>Segmentation fault</code>. We cannot actually modify <code>pointer_to_number</code> because, even if it is a location of memory that contains an address (and it would contain another address after the store) it is not in the <code>data</code> section, but in the <code>text</code> section. So this is a statically defined pointer, whose value (i.e. the address it contains) cannot change. So, how can we have a pointer that can change? Well, we will have to put it in the <code>data</code> section, where we usually put all the data of our program.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
number_1  <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">3</span>
number_2  <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">4</span>
pointer_to_number<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">0</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
<span style="color: #339933;">.</span>globl main
&nbsp;
&nbsp;
main<span style="color: #339933;">:</span>
    ldr r0<span style="color: #339933;">,</span> addr_of_pointer_to_number
                             <span style="color: #339933;">/*</span> r0 ← &amp;pointer_to_number <span style="color: #339933;">*/</span>
&nbsp;
    ldr r1<span style="color: #339933;">,</span> addr_of_number_2 <span style="color: #339933;">/*</span> r1 ← &amp;number_2 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">str</span> r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>             <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span>r0 ← r1<span style="color: #339933;">.</span>
                                This is actually
                                  pointer_to_number ← &amp;number_2 <span style="color: #339933;">*/</span>
&nbsp;
    ldr r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>             <span style="color: #339933;">/*</span> r1 ← <span style="color: #339933;">*</span>r0<span style="color: #339933;">.</span>
                                This is actually
                                  r1 ← pointer_to_number
                                Since pointer_to_number has the value &amp;number_2
                                then this is like
                                  r1 ← &amp;number_2
                             <span style="color: #339933;">*/</span>
&nbsp;
&nbsp;
    ldr r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #009900; font-weight: bold;">]</span>             <span style="color: #339933;">/*</span> r0 ← <span style="color: #339933;">*</span>r1
                                Since r1 had as value &amp;number_2
                                then this is like
                                   r0 ← number_2
                             <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr
&nbsp;
addr_of_number_1<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> number_1
addr_of_number_2<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> number_2
addr_of_pointer_to_number<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> pointer_to_number</pre></td></tr></tbody></table><p class="theCode" style="display:none;">.data

.align 4
number_1  : .word 3
number_2  : .word 4
pointer_to_number: .word 0

.text
.globl main


main:
    ldr r0, addr_of_pointer_to_number
                             /* r0 ← &amp;pointer_to_number */

    ldr r1, addr_of_number_2 /* r1 ← &amp;number_2 */

    str r1, [r0]             /* *r0 ← r1.
                                This is actually
                                  pointer_to_number ← &amp;number_2 */

    ldr r1, [r0]             /* r1 ← *r0.
                                This is actually
                                  r1 ← pointer_to_number
                                Since pointer_to_number has the value &amp;number_2
                                then this is like
                                  r1 ← &amp;number_2
                             */


    ldr r0, [r1]             /* r0 ← *r1
                                Since r1 had as value &amp;number_2
                                then this is like
                                   r0 ← number_2
                             */

    bx lr

addr_of_number_1: .word number_1
addr_of_number_2: .word number_2
addr_of_pointer_to_number: .word pointer_to_number</p></div>

<p>
From this last example several things should be clear. We have static pointers to <code>number_1</code>, <code>number_2</code> and <code>pointer_to_number</code> (respectively called <code>addr_of_number_1</code>, <code>addr_of_number_2</code> and <code>addr_of_pointer_to_number</code>). Note that <code>addr_of_pointer_to_number</code> is actually a pointer to a pointer! Why these pointers are statically defined? Well, we can name locations of memory (i.e. addresses) using labels (this way we do not have to really know the exact address and at the same time we can use a descriptive name). These locations of memory, named through labels, will never change during the execution of the program so they are somehow predefined before the program starts. This why the addresses of <code>number_1</code>, <code>number_2</code> and <code>addr_of_pointer_to_number</code> are statically defined and stored in a part of the program that cannot change (the <code>.text</code> section cannot be modified when the program runs).
</p>
<p>
This means that accessing to <code>pointer_to_number</code> using <code>addr_of_pointer_to_number</code> involves using a pointer to a pointer. Nothing fancy here, a pointer to a pointer is just a location of memory that contains the address of another location of memory that we know is a pointer too.
</p>
<p>
The program simply loads the value 4, stored in <code>number_2</code> using <code>pointer_to_number</code>. We first load the address of the pointer (this is, the pointer to the pointer, but the <em>address of the pointer</em> may be clearer) into <code>r0</code> in line 13. Then we do the same with the address of <code>number_2</code>, storing it in <code>r1</code>, line 16. Then in line 18 we update the value <code>pointer_to_number</code> (remember, the value of a pointer will always be an address) with the address of <code>number_2</code>. In line 22 we actually get the value of <code>pointer_to_number</code> loading it into <code>r1</code>. I insist again: the value of <code>pointer_to_number</code> is an address, so now r1 contains an address. This is the reason why in line 31 we load into <code>r0</code> the value of the in <code>r1</code>.
</p>
<h2>Passing large amounts of data</h2>
<p>
When we pass data to functions we follow the conventions defined in the AAPCS. We try to fill the first 4 registers r0 to r3. If more data is expected we must use the stack. This means that if we were to pass a big chunk of data to a function we may end spending a lot of time just preparing the call (setting registers r0 to r3 and then pushing all the data on top of the stack, and remember, in reverse order!) than running the code of the function itself.
</p>
<p>
There are several cases when this situation arises. In a language like C, all parameters are passed by value. This means that the function receives a copy of the value. This way the function may freely modify this value and the caller will not see any changes in it. This may seem inefficient but from a productivity point of view, a function that does not cause any side effect to its inputs may be regarded as easier to understand than one that does.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #993333;">struct</span> A
<span style="color: #009900;">{</span> 
  <span style="color: #666666; font-style: italic;">// big structure</span>
<span style="color: #009900;">}</span><span style="color: #339933;">;</span>
&nbsp;
<span style="color: #666666; font-style: italic;">// This function computes a 'thing_t' using a 'struct A'</span>
thing_t compute_something<span style="color: #009900;">(</span><span style="color: #993333;">struct</span> A<span style="color: #009900;">)</span><span style="color: #339933;">;</span>
&nbsp;
<span style="color: #993333;">void</span> my_code<span style="color: #009900;">(</span><span style="color: #993333;">void</span><span style="color: #009900;">)</span>
<span style="color: #009900;">{</span>
  <span style="color: #993333;">struct</span> A a<span style="color: #339933;">;</span>
  thing_t something<span style="color: #339933;">;</span>
&nbsp;
  a <span style="color: #339933;">=</span> ...<span style="color: #339933;">;</span>
  something <span style="color: #339933;">=</span> compute_something<span style="color: #009900;">(</span>a<span style="color: #009900;">)</span>
  <span style="color: #666666; font-style: italic;">// a is unchanged here!</span>
<span style="color: #009900;">}</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">struct A
{ 
  // big structure
};

// This function computes a 'thing_t' using a 'struct A'
thing_t compute_something(struct A);

void my_code(void)
{
  struct A a;
  thing_t something;

  a = ...;
  something = compute_something(a)
  // a is unchanged here!
}</p></div>

<p>
Note that in C, array types are not passed by value but this is by design: there are no array values in C although there are array types (you may need to repeat to yourself this last sentence several times before fully understanding it <img draggable="false" class="emoji" alt="😉" src="https://s.w.org/images/core/emoji/2.2.1/svg/1f609.svg" scale="0">
</p>
<p>
If our function is going to modify the parameter and we do not want to see the changes after the call, there is little that we can do. We have to invest some time in the parameter passing.
</p>
<p>
But what if our function does not actually modify the data? Or, what if we are interested in the changes the function did? Or even better, what if the parameter being modified is actually another output of the function?
</p>
<p>
Well, all these scenarios involve pointers.
</p>
<h2>Passing a big array by value</h2>
<p>
Consider an array of 32-bit integers and we want to sum all the elements. Our array will be in memory, it is just a contiguous sequence of 32-bit integers. We want to pass, somehow, the array<br>
to the function (together with the length of the array if the length may not be constant), sum all the integers and return the sum. Note that in this case the function does not modify the array it just reads it.
</p>
<p>
Let’s make a function <code>sum_array_value</code> that must have the array of integers passed by value. The first parameter, in <code>r0</code> will be the number of items of the integer array. Registers <code>r1</code> to <code>r3</code> may (or may not) have value depending on the number of items in the array. So the first three elements must be handled differently. Then, if there are still items left, they must be loaded from the stack.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">sum_array_value <span style="color: #339933;">:</span> 
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
&nbsp;
    <span style="color: #339933;">/*</span> We have passed all the <span style="color: #0000ff; font-weight: bold;">data</span> by value <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> r4 will hold the sum so far <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>      <span style="color: #339933;">/*</span> r4 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">In</span> r0 we have the number of items of the array <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>            <span style="color: #339933;">/*</span> r0 <span style="color: #339933;">-</span> #<span style="color: #ff0000;">1</span> <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
    blt <span style="color: #339933;">.</span>Lend_of_sum_array  <span style="color: #339933;">/*</span> if r0 &lt; <span style="color: #ff0000;">1</span> branch to end_of_sum_array <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> r1        <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">add</span> the first item <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span>            <span style="color: #339933;">/*</span> r0 <span style="color: #339933;">-</span> #<span style="color: #ff0000;">2</span> <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
    blt <span style="color: #339933;">.</span>Lend_of_sum_array  <span style="color: #339933;">/*</span> if r0 &lt; <span style="color: #ff0000;">2</span> branch to end_of_sum_array <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> r2        <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">add</span> the second item <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">3</span>            <span style="color: #339933;">/*</span> r0 <span style="color: #339933;">-</span> #<span style="color: #ff0000;">3</span> <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
    blt <span style="color: #339933;">.</span>Lend_of_sum_array  <span style="color: #339933;">/*</span> if r0 &lt; <span style="color: #ff0000;">3</span> branch to end_of_sum_array <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> r3        <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">add</span> the third item <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> 
     The <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #0000ff; font-weight: bold;">at</span> this point looks like this
       |                | <span style="color: #009900; font-weight: bold;">(</span>lower addresses<span style="color: #009900; font-weight: bold;">)</span>
       |                |
       | lr             |  &lt;<span style="color: #339933;">-</span> <span style="color: #46aa03; font-weight: bold;">sp</span> points here
       | r6             |  &lt;<span style="color: #339933;">-</span> this is <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span>
       | r5             |  &lt;<span style="color: #339933;">-</span> this is <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span>
       | r4             |  &lt;<span style="color: #339933;">-</span> this is <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">+</span> <span style="color: #ff0000;">12</span>
       | big_array<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">3</span><span style="color: #009900; font-weight: bold;">]</span>   |  &lt;<span style="color: #339933;">-</span> this is <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">+</span> <span style="color: #ff0000;">16</span> <span style="color: #009900; font-weight: bold;">(</span>we want r5 to point here<span style="color: #009900; font-weight: bold;">)</span>
       | big_array<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>   |
       |     <span style="color: #339933;">...</span>        |
       | big_array<span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">255</span><span style="color: #009900; font-weight: bold;">]</span> |
       |                | 
       |                | <span style="color: #009900; font-weight: bold;">(</span>higher addresses<span style="color: #009900; font-weight: bold;">)</span>
&nbsp;
    keep <span style="color: #00007f; font-weight: bold;">in</span> r5 the address where the <span style="color: #0000ff; font-weight: bold;">stack</span><span style="color: #339933;">-</span>passed portion of the array starts <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r5<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">16</span> <span style="color: #339933;">/*</span> r5 ← <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">+</span> <span style="color: #ff0000;">16</span> <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">in</span> register r3 we will count how many items we have read
       from the <span style="color: #0000ff; font-weight: bold;">stack</span><span style="color: #339933;">.</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r3<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>
&nbsp;
    <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">in</span> the <span style="color: #0000ff; font-weight: bold;">stack</span> there will always be <span style="color: #ff0000;">3</span> less items because
       the first <span style="color: #ff0000;">3</span> were already passed <span style="color: #00007f; font-weight: bold;">in</span> registers
       <span style="color: #009900; font-weight: bold;">(</span>recall that r0 had how many items were <span style="color: #00007f; font-weight: bold;">in</span> the array<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">sub</span> r0<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">3</span>
&nbsp;
    b <span style="color: #339933;">.</span>Lcheck_loop_sum_array
    <span style="color: #339933;">.</span>Lloop_sum_array<span style="color: #339933;">:</span>
      ldr r6<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r5<span style="color: #339933;">,</span> r3<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">]</span>       <span style="color: #339933;">/*</span> r6 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r5 <span style="color: #339933;">+</span> r3 <span style="color: #339933;">*</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span> load
                                        the array item r3 from the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">add</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> r6                 <span style="color: #339933;">/*</span> r4 ← r4 <span style="color: #339933;">+</span> r6
                                        accumulate <span style="color: #00007f; font-weight: bold;">in</span> r4 <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">add</span> r3<span style="color: #339933;">,</span> r3<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>                 <span style="color: #339933;">/*</span> r3 ← r3 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> 
                                        move to the next item <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>Lcheck_loop_sum_array<span style="color: #339933;">:</span>
      <span style="color: #00007f; font-weight: bold;">cmp</span> r3<span style="color: #339933;">,</span> r0           <span style="color: #339933;">/*</span> r0 <span style="color: #339933;">-</span> r3 <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
      blt <span style="color: #339933;">.</span>Lloop_sum_array   <span style="color: #339933;">/*</span> if r3 &lt; r0  branch to loop_sum_array <span style="color: #339933;">*/</span>
&nbsp;
  <span style="color: #339933;">.</span>Lend_of_sum_array<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> r4  <span style="color: #339933;">/*</span> r0 ← r4<span style="color: #339933;">,</span> to return the value of the sum <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
&nbsp;
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">sum_array_value : 
    push {r4, r5, r6, lr}

    /* We have passed all the data by value */

    /* r4 will hold the sum so far */
    mov r4, #0      /* r4 ← 0 */
    /* In r0 we have the number of items of the array */

    cmp r0, #1            /* r0 - #1 and update cpsr */
    blt .Lend_of_sum_array  /* if r0 &lt; 1 branch to end_of_sum_array */
    add r4, r4, r1        /* add the first item */

    cmp r0, #2            /* r0 - #2 and update cpsr */
    blt .Lend_of_sum_array  /* if r0 &lt; 2 branch to end_of_sum_array */
    add r4, r4, r2        /* add the second item */

    cmp r0, #3            /* r0 - #3 and update cpsr */
    blt .Lend_of_sum_array  /* if r0 &lt; 3 branch to end_of_sum_array */
    add r4, r4, r3        /* add the third item */

    /* 
     The stack at this point looks like this
       |                | (lower addresses)
       |                |
       | lr             |  &lt;- sp points here
       | r6             |  &lt;- this is sp + 4
       | r5             |  &lt;- this is sp + 8
       | r4             |  &lt;- this is sp + 12
       | big_array[3]   |  &lt;- this is sp + 16 (we want r5 to point here)
       | big_array[4]   |
       |     ...        |
       | big_array[255] |
       |                | 
       |                | (higher addresses)
    
    keep in r5 the address where the stack-passed portion of the array starts */
    add r5, sp, #16 /* r5 ← sp + 16 */

    /* in register r3 we will count how many items we have read
       from the stack. */
    mov r3, #0

    /* in the stack there will always be 3 less items because
       the first 3 were already passed in registers
       (recall that r0 had how many items were in the array) */
    sub r0, r0, #3

    b .Lcheck_loop_sum_array
    .Lloop_sum_array:
      ldr r6, [r5, r3, LSL #2]       /* r6 ← *(r5 + r3 * 4) load
                                        the array item r3 from the stack */
      add r4, r4, r6                 /* r4 ← r4 + r6
                                        accumulate in r4 */
      add r3, r3, #1                 /* r3 ← r3 + 1 
                                        move to the next item */
    .Lcheck_loop_sum_array:
      cmp r3, r0           /* r0 - r3 and update cpsr */
      blt .Lloop_sum_array   /* if r3 &lt; r0  branch to loop_sum_array */

  .Lend_of_sum_array:
    mov r0, r4  /* r0 ← r4, to return the value of the sum */
    pop {r4, r5, r6, lr}

    bx lr</p></div>

<p>
The function is not particularly complex except for the special handling of the first 3 items (stored in <code>r1</code> to <code>r3</code>) and that we have to be careful when locating inside the stack the array. Upon entering the function the items of the array passed through the stack are laid out consecutively starting from <code>sp</code>. The <code>push</code> instruction at the beginning pushes onto the stack four registers (<code>r4</code>, <code>r5</code>, <code>r6</code> and <code>lr</code>) so our array is now in <code>sp + 16</code> (see lines 30 and 38). Besides of these details, we just loop the items of the array and accumulate the sum in the register <code>r4</code>. Finally, we move <code>r4</code> into <code>r0</code> for the return value of the function.
</p>
<p>
In order to call this function we have to put an array into the stack. Consider the following program.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
&nbsp;
big_array <span style="color: #339933;">:</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">0</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">1</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">2</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">3</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">5</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">6</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">7</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">8</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">9</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">10</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">11</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">12</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">13</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">14</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">15</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">16</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">17</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">18</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">19</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">20</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">21</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">22</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">23</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">24</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">25</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">26</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">27</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">28</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">29</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">30</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">31</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">32</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">33</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">34</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">35</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">36</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">37</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">38</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">39</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">40</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">41</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">42</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">43</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">44</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">45</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">46</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">47</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">48</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">49</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">50</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">51</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">52</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">53</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">54</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">55</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">56</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">57</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">58</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">59</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">60</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">61</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">62</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">63</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">64</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">65</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">66</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">67</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">68</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">69</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">70</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">71</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">72</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">73</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">74</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">75</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">76</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">77</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">78</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">79</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">80</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">81</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">82</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">83</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">84</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">85</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">86</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">87</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">88</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">89</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">90</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">91</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">92</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">93</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">94</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">95</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">96</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">97</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">98</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">99</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">100</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">101</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">102</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">103</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">104</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">105</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">106</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">107</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">108</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">109</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">110</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">111</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">112</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">113</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">114</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">115</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">116</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">117</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">118</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">119</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">120</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">121</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">122</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">123</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">124</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">125</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">126</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">127</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">128</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">129</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">130</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">131</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">132</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">133</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">134</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">135</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">136</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">137</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">138</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">139</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">140</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">141</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">142</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">143</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">144</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">145</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">146</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">147</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">148</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">149</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">150</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">151</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">152</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">153</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">154</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">155</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">156</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">157</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">158</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">159</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">160</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">161</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">162</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">163</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">164</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">165</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">166</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">167</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">168</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">169</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">170</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">171</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">172</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">173</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">174</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">175</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">176</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">177</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">178</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">179</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">180</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">181</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">182</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">183</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">184</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">185</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">186</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">187</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">188</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">189</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">190</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">191</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">192</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">193</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">194</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">195</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">196</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">197</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">198</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">199</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">200</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">201</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">202</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">203</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">204</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">205</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">206</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">207</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">208</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">209</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">210</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">211</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">212</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">213</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">214</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">215</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">216</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">217</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">218</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">219</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">220</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">221</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">222</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">223</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">224</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">225</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">226</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">227</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">228</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">229</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">230</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">231</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">232</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">233</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">234</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">235</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">236</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">237</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">238</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">239</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">240</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">241</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">242</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">243</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">244</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">245</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">246</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">247</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">248</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">249</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">250</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">251</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">252</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">253</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">254</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">255</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
&nbsp;
message<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"The sum of 0 to 255 is %d\n"</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
<span style="color: #339933;">.</span>globl main
&nbsp;
sum_array_value <span style="color: #339933;">:</span>
   <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">code</span> shown above <span style="color: #339933;">*/</span>
&nbsp;
main<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> r7<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #339933;">/*</span> we will <span style="color: #00007f; font-weight: bold;">not</span> use <span style="color: #46aa03; font-weight: bold;">r8</span> but we need to keep the function <span style="color: #ff0000;">8</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
&nbsp;
    ldr r4<span style="color: #339933;">,</span> address_of_big_array
&nbsp;
    <span style="color: #339933;">/*</span> Prepare <span style="color: #00007f; font-weight: bold;">call</span> <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">256</span>  <span style="color: #339933;">/*</span> Load <span style="color: #00007f; font-weight: bold;">in</span> the first parameter the number of items 
                     r0 ← <span style="color: #ff0000;">256</span>
                     <span style="color: #339933;">*/</span>
&nbsp;
    ldr r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #009900; font-weight: bold;">]</span>     <span style="color: #339933;">/*</span> load <span style="color: #00007f; font-weight: bold;">in</span> the second parameter the first item of the array <span style="color: #339933;">*/</span>
    ldr r2<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">/*</span> load <span style="color: #00007f; font-weight: bold;">in</span> the third parameter the second item of the array <span style="color: #339933;">*/</span>
    ldr r3<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">/*</span> load <span style="color: #00007f; font-weight: bold;">in</span> the fourth parameter the third item of the array <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> before pushing anything <span style="color: #00007f; font-weight: bold;">in</span> the <span style="color: #0000ff; font-weight: bold;">stack</span> keep its position <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r7<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span>
&nbsp;
    <span style="color: #339933;">/*</span> We cannot use more registers<span style="color: #339933;">,</span> now we have to <span style="color: #00007f; font-weight: bold;">push</span> them onto the <span style="color: #0000ff; font-weight: bold;">stack</span>
       <span style="color: #009900; font-weight: bold;">(</span><span style="color: #00007f; font-weight: bold;">in</span> reverse order<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">255</span>   <span style="color: #339933;">/*</span> r5 ← <span style="color: #ff0000;">255</span>
                      This is the last item position
                      <span style="color: #009900; font-weight: bold;">(</span>note that the first would be <span style="color: #00007f; font-weight: bold;">in</span> position <span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
&nbsp;
    b <span style="color: #339933;">.</span>Lcheck_pass_parameter_loop
    <span style="color: #339933;">.</span>Lpass_parameter_loop<span style="color: #339933;">:</span>
&nbsp;
      ldr r6<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">]</span>  <span style="color: #339933;">/*</span> r6 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r4 <span style="color: #339933;">+</span> r5 <span style="color: #339933;">*</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span>
                                   loads the item <span style="color: #00007f; font-weight: bold;">in</span> position r5 <span style="color: #00007f; font-weight: bold;">into</span> r6<span style="color: #339933;">.</span> Note that
                                   we have to multiply by <span style="color: #ff0000;">4</span> because this is the size
                                   of each item <span style="color: #00007f; font-weight: bold;">in</span> the array <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r6<span style="color: #009900; font-weight: bold;">}</span>                 <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">push</span> the loaded value to the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">sub</span> r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>            <span style="color: #339933;">/*</span> we are done with the current item<span style="color: #339933;">,</span>
                                   go to the previous index of the array <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>Lcheck_pass_parameter_loop<span style="color: #339933;">:</span>
      <span style="color: #00007f; font-weight: bold;">cmp</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span>                <span style="color: #339933;">/*</span> compute r5 <span style="color: #339933;">-</span> #<span style="color: #ff0000;">2</span> <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
      bne <span style="color: #339933;">.</span>Lpass_parameter_loop   <span style="color: #339933;">/*</span> if r5 != #<span style="color: #ff0000;">2</span> branch to pass_parameter_loop <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> We are done<span style="color: #339933;">,</span> we have passed all the values of the array<span style="color: #339933;">,</span>
       now <span style="color: #00007f; font-weight: bold;">call</span> the function <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> sum_array_value
&nbsp;
    <span style="color: #339933;">/*</span> restore the <span style="color: #0000ff; font-weight: bold;">stack</span> position <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> r7
&nbsp;
    <span style="color: #339933;">/*</span> prepare the <span style="color: #00007f; font-weight: bold;">call</span> to printf <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> r0                  <span style="color: #339933;">/*</span> second parameter<span style="color: #339933;">,</span> the sum itself <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> address_of_message  <span style="color: #339933;">/*</span> first parameter<span style="color: #339933;">,</span> the message <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> printf
&nbsp;
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> r7<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr
&nbsp;
address_of_big_array <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> big_array
address_of_message <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> message</pre></td></tr></tbody></table><p class="theCode" style="display:none;">.data

.align 4

big_array :
.word 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21
.word 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41
.word 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61
.word 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81
.word 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100
.word 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116
.word 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132
.word 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148
.word 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164
.word 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180
.word 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196
.word 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212
.word 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228
.word 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244
.word 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255

.align 4

message: .asciz "The sum of 0 to 255 is %d\n"

.text
.globl main

sum_array_value :
   /* code shown above */

main:
    push {r4, r5, r6, r7, r8, lr}
    /* we will not use r8 but we need to keep the function 8-byte aligned */

    ldr r4, address_of_big_array

    /* Prepare call */

    mov r0, #256  /* Load in the first parameter the number of items 
                     r0 ← 256
                     */

    ldr r1, [r4]     /* load in the second parameter the first item of the array */
    ldr r2, [r4, #4] /* load in the third parameter the second item of the array */
    ldr r3, [r4, #8] /* load in the fourth parameter the third item of the array */

    /* before pushing anything in the stack keep its position */
    mov r7, sp

    /* We cannot use more registers, now we have to push them onto the stack
       (in reverse order) */
    mov r5, #255   /* r5 ← 255
                      This is the last item position
                      (note that the first would be in position 0) */

    b .Lcheck_pass_parameter_loop
    .Lpass_parameter_loop:

      ldr r6, [r4, r5, LSL #2]  /* r6 ← *(r4 + r5 * 4).
                                   loads the item in position r5 into r6. Note that
                                   we have to multiply by 4 because this is the size
                                   of each item in the array */
      push {r6}                 /* push the loaded value to the stack */
      sub r5, r5, #1            /* we are done with the current item,
                                   go to the previous index of the array */
    .Lcheck_pass_parameter_loop:
      cmp r5, #2                /* compute r5 - #2 and update cpsr */
      bne .Lpass_parameter_loop   /* if r5 != #2 branch to pass_parameter_loop */

    /* We are done, we have passed all the values of the array,
       now call the function */
    bl sum_array_value

    /* restore the stack position */
    mov sp, r7

    /* prepare the call to printf */
    mov r1, r0                  /* second parameter, the sum itself */
    ldr r0, address_of_message  /* first parameter, the message */
    bl printf

    pop {r4, r5, r6, r7, r8, lr}
    bx lr

address_of_big_array : .word big_array
address_of_message : .word message</p></div>

<p>
In line 40 we start preparing the call to <code>sum_array_value</code>. The first parameter, passed in register <code>r0</code>, is the number of items of this array (in the example hardcoded to 256 items). Then we pass the first three items of the array in registers <code>r1</code> to <code>r3</code>. Remaining items must be passed on the stack. Remember that in a stack the last item pushed will be the first popped, so if we want our array be laid in the same order we have to push it backwards. So we start from the last item, line 53, and then we load every item and push it onto the stack. Once all the elements have been pushed onto the stack we can call <code>sum_array_value</code> (line 73).
</p>
<p>
An important caveat when manipulating the stack in this way is that it is very important to restore it and leave it in the same state as it was before preparing the call. This is the reason we keep <code>sp</code> in <code>r7</code> in line 49 and we restore it right after the call in line 76. Forgetting to do this will make further operations on the stack push data onto the wrong place or pop from the stack wrong data. Keeping the stack synched is essential when calling functions.
</p>
<h2>Passing a big array by reference</h2>
<p>
Now you are probably thinking that passing a big array through the stack (along with all the boilerplate that this requires) to a function that does not modify it, is, to say the least, wasteful.
</p>
<p>
Note that, when the amount of data is small, registers r0 to r3 are usually enough, so pass by value is affordable. Passing some data in the stack is fine too, but passing big structures on the stack may harm the performance (especially if our function is being called lots of times).
</p>
<p>
Can we do better? Yes. Instead of passing copies of the values of the array, would it be possible to pass the address to the array? The answer is, again, yes. This is the concept of <em>pass by reference</em>. When we pass by value, the value of the data passed is somehow copied (either in a register or a stack). Here we will pass a <em>reference</em> (i.e. an address) to the data. So now we are done by just passing the number of items and then the address of the array, and let the function use this address to perform its computation.
</p>
<p>
Consider the following program, which also sums an array of integers but now passing the array by reference.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
&nbsp;
big_array <span style="color: #339933;">:</span>
  <span style="color: #339933;">/*</span> Same as above <span style="color: #339933;">*/</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
&nbsp;
message<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"The sum of 0 to 255 is %d\n"</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
<span style="color: #339933;">.</span>globl main
&nbsp;
sum_array_ref <span style="color: #339933;">:</span>
    <span style="color: #339933;">/*</span> Parameters<span style="color: #339933;">:</span> 
           r0  Number of items
           r1  Address of the array
    <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
&nbsp;
    <span style="color: #339933;">/*</span> We have passed all the <span style="color: #0000ff; font-weight: bold;">data</span> by reference <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> r4 will hold the sum so far <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>      <span style="color: #339933;">/*</span> r4 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>      <span style="color: #339933;">/*</span> r5 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
&nbsp;
    b <span style="color: #339933;">.</span>Lcheck_loop_array_sum
    <span style="color: #339933;">.</span>Lloop_array_sum<span style="color: #339933;">:</span>
      ldr r6<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">]</span>   <span style="color: #339933;">/*</span> r6 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r1 <span style="color: #339933;">+</span> r5 <span style="color: #339933;">*</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">add</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> r6             <span style="color: #339933;">/*</span> r4 ← r4 <span style="color: #339933;">+</span> r6 <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">add</span> r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>             <span style="color: #339933;">/*</span> r5 ← r5 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>Lcheck_loop_array_sum<span style="color: #339933;">:</span>
      <span style="color: #00007f; font-weight: bold;">cmp</span> r5<span style="color: #339933;">,</span> r0                 <span style="color: #339933;">/*</span> r5 <span style="color: #339933;">-</span> r0 <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
      bne <span style="color: #339933;">.</span>Lloop_array_sum       <span style="color: #339933;">/*</span> if r5 != r0 go to <span style="color: #339933;">.</span>Lloop_array_sum <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> r4  <span style="color: #339933;">/*</span> r0 ← r4<span style="color: #339933;">,</span> to return the value of the sum <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
&nbsp;
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr
&nbsp;
&nbsp;
main<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #339933;">/*</span> we will <span style="color: #00007f; font-weight: bold;">not</span> use r4 but we need to keep the function <span style="color: #ff0000;">8</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">256</span>
    ldr r1<span style="color: #339933;">,</span> address_of_big_array
&nbsp;
    <span style="color: #46aa03; font-weight: bold;">bl</span> sum_array_ref
&nbsp;
    <span style="color: #339933;">/*</span> prepare the <span style="color: #00007f; font-weight: bold;">call</span> to printf <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> r0                  <span style="color: #339933;">/*</span> second parameter<span style="color: #339933;">,</span> the sum itself <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> address_of_message  <span style="color: #339933;">/*</span> first parameter<span style="color: #339933;">,</span> the message <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> printf
&nbsp;
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr
&nbsp;
address_of_big_array <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> big_array
address_of_message <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> message</pre></td></tr></tbody></table><p class="theCode" style="display:none;">.data

.align 4

big_array :
  /* Same as above */

.align 4

message: .asciz "The sum of 0 to 255 is %d\n"

.text
.globl main

sum_array_ref :
    /* Parameters: 
           r0  Number of items
           r1  Address of the array
    */
    push {r4, r5, r6, lr}

    /* We have passed all the data by reference */

    /* r4 will hold the sum so far */
    mov r4, #0      /* r4 ← 0 */
    mov r5, #0      /* r5 ← 0 */

    b .Lcheck_loop_array_sum
    .Lloop_array_sum:
      ldr r6, [r1, r5, LSL #2]   /* r6 ← *(r1 + r5 * 4) */
      add r4, r4, r6             /* r4 ← r4 + r6 */
      add r5, r5, #1             /* r5 ← r5 + 1 */
    .Lcheck_loop_array_sum:
      cmp r5, r0                 /* r5 - r0 and update cpsr */
      bne .Lloop_array_sum       /* if r5 != r0 go to .Lloop_array_sum */

    mov r0, r4  /* r0 ← r4, to return the value of the sum */
    pop {r4, r5, r6, lr}

    bx lr


main:
    push {r4, lr}
    /* we will not use r4 but we need to keep the function 8-byte aligned */

    mov r0, #256
    ldr r1, address_of_big_array

    bl sum_array_ref

    /* prepare the call to printf */
    mov r1, r0                  /* second parameter, the sum itself */
    ldr r0, address_of_message  /* first parameter, the message */
    bl printf

    pop {r4, lr}
    bx lr

address_of_big_array : .word big_array
address_of_message : .word message</p></div>

<p>
Now the code is much simpler as we avoid copying the values of the array in the stack. We simply pass the address of the array as the second parameter of the function and<br>
then we use it to access the array and compute the sum. Much simpler, isn’t it?
</p>
<h2>Modifying data through pointers</h2>
<p>
We saw at the beginning of the post that we could modify data through pointers. If we pass a pointer to a function we can let the function modify it as well. Imagine a function that takes an integer and increments its. We could do this by returning the value, for instance.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">increment<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r0<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>  <span style="color: #339933;">/*</span> r0 ← r0 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">increment:
    add r0, r0, #1  /* r0 ← r0 + 1 */</p></div>

<p>
This takes the first parameter (in <code>r0</code>) increments it and returns it (recall that we return integers in <code>r0</code>).
</p>
<p>
An alternative approach, could be receiving a pointer to some data and let the function increment the data at the position defined by the pointer.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">increment_ptr<span style="color: #339933;">:</span>
  ldr r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>      <span style="color: #339933;">/*</span> r1 ← <span style="color: #339933;">*</span>r0 <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">add</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>    <span style="color: #339933;">/*</span> r1 ← r1 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">str</span> r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>      <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span>r0 ← r1 <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">increment_ptr:
  ldr r1, [r0]      /* r1 ← *r0 */
  add r1, r1, #1    /* r1 ← r1 + 1 */
  str r1, [r0]      /* *r0 ← r1 */</p></div>

<p>
For a more elaborated example, let’s retake the array code but this time instead of computing the sum of all the values, we will multiply each item by two and keep it in the same array. To prove that we have modified it, we will also print each item.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> double_array<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
big_array <span style="color: #339933;">:</span>
 <span style="color: #339933;">/*</span> Same as above <span style="color: #339933;">*/</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
message<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"Item at position %d has value %d\n"</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
<span style="color: #339933;">.</span>globl main
&nbsp;
double_array <span style="color: #339933;">:</span> 
    <span style="color: #339933;">/*</span> Parameters<span style="color: #339933;">:</span> 
           r0  Number of items
           r1  Address of the array
    <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>      <span style="color: #339933;">/*</span> r4 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
&nbsp;
    b <span style="color: #339933;">.</span>Lcheck_loop_array_double
    <span style="color: #339933;">.</span>Lloop_array_double<span style="color: #339933;">:</span>
      ldr r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">]</span>   <span style="color: #339933;">/*</span> r5 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r1 <span style="color: #339933;">+</span> r4 <span style="color: #339933;">*</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">1</span>         <span style="color: #339933;">/*</span> r5 ← r5 <span style="color: #339933;">*</span> <span style="color: #ff0000;">2</span> <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">str</span> r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">]</span>   <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r1 <span style="color: #339933;">+</span> r4 <span style="color: #339933;">*</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span> ← r5 <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">add</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>             <span style="color: #339933;">/*</span> r4 ← r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>Lcheck_loop_array_double<span style="color: #339933;">:</span>
      <span style="color: #00007f; font-weight: bold;">cmp</span> r4<span style="color: #339933;">,</span> r0                 <span style="color: #339933;">/*</span> r4 <span style="color: #339933;">-</span> r0 <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
      bne <span style="color: #339933;">.</span>Lloop_array_double    <span style="color: #339933;">/*</span> if r4 != r0 go to <span style="color: #339933;">.</span>Lloop_array_double <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
&nbsp;
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr
&nbsp;
print_each_item<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> r7<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r8</span> is unused <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>      <span style="color: #339933;">/*</span> r4 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r6<span style="color: #339933;">,</span> r0      <span style="color: #339933;">/*</span> r6 ← r0<span style="color: #339933;">.</span> Keep r0 because we will overwrite it <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r7<span style="color: #339933;">,</span> r1      <span style="color: #339933;">/*</span> r7 ← r1<span style="color: #339933;">.</span> Keep r1 because we will overwrite it <span style="color: #339933;">*/</span>
&nbsp;
&nbsp;
    b <span style="color: #339933;">.</span>Lcheck_loop_print_items
    <span style="color: #339933;">.</span>Lloop_print_items<span style="color: #339933;">:</span>
      ldr r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r7<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">]</span>   <span style="color: #339933;">/*</span> r5 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r7 <span style="color: #339933;">+</span> r4 <span style="color: #339933;">*</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
&nbsp;
      <span style="color: #339933;">/*</span> Prepare the <span style="color: #00007f; font-weight: bold;">call</span> to printf <span style="color: #339933;">*/</span>
      ldr r0<span style="color: #339933;">,</span> address_of_message <span style="color: #339933;">/*</span> first parameter of the <span style="color: #00007f; font-weight: bold;">call</span> to printf below <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> r4      <span style="color: #339933;">/*</span> second parameter<span style="color: #339933;">:</span> item position <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">mov</span> r2<span style="color: #339933;">,</span> r5      <span style="color: #339933;">/*</span> third parameter<span style="color: #339933;">:</span> item value <span style="color: #339933;">*/</span>
      <span style="color: #46aa03; font-weight: bold;">bl</span> printf       <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> printf <span style="color: #339933;">*/</span>
&nbsp;
      <span style="color: #00007f; font-weight: bold;">add</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>             <span style="color: #339933;">/*</span> r4 ← r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>Lcheck_loop_print_items<span style="color: #339933;">:</span>
      <span style="color: #00007f; font-weight: bold;">cmp</span> r4<span style="color: #339933;">,</span> r6                 <span style="color: #339933;">/*</span> r4 <span style="color: #339933;">-</span> r6 <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
      bne <span style="color: #339933;">.</span>Lloop_print_items       <span style="color: #339933;">/*</span> if r4 != r6 goto <span style="color: #339933;">.</span>Lloop_print_items <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> r7<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r8</span><span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr
&nbsp;
main<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #339933;">/*</span> we will <span style="color: #00007f; font-weight: bold;">not</span> use r4 but we need to keep the function <span style="color: #ff0000;">8</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> first <span style="color: #00007f; font-weight: bold;">call</span> print_each_item <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">256</span>                   <span style="color: #339933;">/*</span> first_parameter<span style="color: #339933;">:</span> number of items <span style="color: #339933;">*/</span>
    ldr r1<span style="color: #339933;">,</span> address_of_big_array   <span style="color: #339933;">/*</span> second parameter<span style="color: #339933;">:</span> address of the array <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> print_each_item             <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> to print_each_item <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> to double_array <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">256</span>                   <span style="color: #339933;">/*</span> first_parameter<span style="color: #339933;">:</span> number of items <span style="color: #339933;">*/</span>
    ldr r1<span style="color: #339933;">,</span> address_of_big_array   <span style="color: #339933;">/*</span> second parameter<span style="color: #339933;">:</span> address of the array <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> double_array               <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> to double_array <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> second <span style="color: #00007f; font-weight: bold;">call</span> print_each_item <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">256</span>                   <span style="color: #339933;">/*</span> first_parameter<span style="color: #339933;">:</span> number of items <span style="color: #339933;">*/</span>
    ldr r1<span style="color: #339933;">,</span> address_of_big_array   <span style="color: #339933;">/*</span> second parameter<span style="color: #339933;">:</span> address of the array <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> print_each_item             <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> to print_each_item <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr
&nbsp;
address_of_big_array <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> big_array
address_of_message <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> message</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* double_array.s */

.data

.align 4
big_array :
 /* Same as above */

.align 4
message: .asciz "Item at position %d has value %d\n"

.text
.globl main

double_array : 
    /* Parameters: 
           r0  Number of items
           r1  Address of the array
    */
    push {r4, r5, r6, lr}

    mov r4, #0      /* r4 ← 0 */

    b .Lcheck_loop_array_double
    .Lloop_array_double:
      ldr r5, [r1, r4, LSL #2]   /* r5 ← *(r1 + r4 * 4) */
      mov r5, r5, LSL #1         /* r5 ← r5 * 2 */
      str r5, [r1, r4, LSL #2]   /* *(r1 + r4 * 4) ← r5 */
      add r4, r4, #1             /* r4 ← r4 + 1 */
    .Lcheck_loop_array_double:
      cmp r4, r0                 /* r4 - r0 and update cpsr */
      bne .Lloop_array_double    /* if r4 != r0 go to .Lloop_array_double */

    pop {r4, r5, r6, lr}

    bx lr
    
print_each_item:
    push {r4, r5, r6, r7, r8, lr} /* r8 is unused */

    mov r4, #0      /* r4 ← 0 */
    mov r6, r0      /* r6 ← r0. Keep r0 because we will overwrite it */
    mov r7, r1      /* r7 ← r1. Keep r1 because we will overwrite it */


    b .Lcheck_loop_print_items
    .Lloop_print_items:
      ldr r5, [r7, r4, LSL #2]   /* r5 ← *(r7 + r4 * 4) */

      /* Prepare the call to printf */
      ldr r0, address_of_message /* first parameter of the call to printf below */
      mov r1, r4      /* second parameter: item position */
      mov r2, r5      /* third parameter: item value */
      bl printf       /* call printf */

      add r4, r4, #1             /* r4 ← r4 + 1 */
    .Lcheck_loop_print_items:
      cmp r4, r6                 /* r4 - r6 and update cpsr */
      bne .Lloop_print_items       /* if r4 != r6 goto .Lloop_print_items */

    pop {r4, r5, r6, r7, r8, lr}
    bx lr

main:
    push {r4, lr}
    /* we will not use r4 but we need to keep the function 8-byte aligned */

    /* first call print_each_item */
    mov r0, #256                   /* first_parameter: number of items */
    ldr r1, address_of_big_array   /* second parameter: address of the array */
    bl print_each_item             /* call to print_each_item */

    /* call to double_array */
    mov r0, #256                   /* first_parameter: number of items */
    ldr r1, address_of_big_array   /* second parameter: address of the array */
    bl double_array               /* call to double_array */

    /* second call print_each_item */
    mov r0, #256                   /* first_parameter: number of items */
    ldr r1, address_of_big_array   /* second parameter: address of the array */
    bl print_each_item             /* call to print_each_item */

    pop {r4, lr}
    bx lr

address_of_big_array : .word big_array
address_of_message : .word message</p></div>

<p>
If you run this program you will see that the items of the array have been effectively doubled.
</p>
<pre>...
Item at position 248 has value 248
Item at position 249 has value 249
Item at position 250 has value 250
Item at position 251 has value 251
Item at position 252 has value 252
Item at position 253 has value 253
Item at position 254 has value 254
Item at position 255 has value 255
Item at position 0 has value 0
Item at position 1 has value 2
Item at position 2 has value 4
Item at position 3 has value 6
Item at position 4 has value 8
Item at position 5 has value 10
Item at position 6 has value 12
Item at position 7 has value 14
Item at position 8 has value 16
Item at position 9 has value 18
...
</pre>
<h2>Returning more than one piece of data</h2>
<p>
Functions, per the AAPCS convention, return their values in register <code>r0</code> (and <code>r1</code> if the returned item is 8 bytes long). We can return more than one thing if we just pass a pointer to some storage (possibly in the stack) as a parameter to the function. More on this topic  in a next chapter.
</p>
<p>
That’s all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/&amp;text=ARM+assembler+in+Raspberry+Pi+%E2%80%93+Chapter+17+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>						<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2013/11/15/create-temporary-zip-file-send-response-rails/" rel="prev">Create a temporary zip file to send as response in Rails</a></span>
			<span class="next"><a href="http://thinkingeek.com/2014/02/12/check-progress-mysql-database-import/" rel="next">Check progress of a mysql database import</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
			<h3>13 thoughts on “<span>ARM assembler in Raspberry Pi – Chapter 17</span>”</h3>
		<ul class="commentlist">
				<li class="comment even thread-even depth-1 parent" id="comment-2649">
				<div id="div-comment-2649" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-9e81362ff9654533accffc53762c9520-0" originals="32" src-orig="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn"><a href="http://devel.meruelo.com.es/blog/" rel="external nofollow" class="url">Fernando</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/#comment-2649">
			January 1, 2014 at 11:39 pm</a>		</div>

		<p>Very nice, as the others. </p>
<p>I just have a couple of questions:<br>
The first, regarding the prohibition of modifying anything inside the “.text” section, I asume that this only applies when compiling under an OS, as it will set the proper MMU rules, but not while in bare-metal, as there is nothing that limits you to accessing any section of memory. Am I right?</p>
<p>The other thing is regarding the stack: in the program in which you pass a big chunk of data via the stack, after returning you restore the previous value of the “stack pointer”. I assume that that is done because you cannot guarantee that all the data is going to be popped inside the callee function, and you need to be able to correctly pop out the other registers before exiting (r3, …, lr). Am I right? </p>
<p>P.S: One typo, on line 59, where it says ” /* if r3 &lt; r3  branch to loop_sum_array */" I think it has to be "r0 &lt; r3".</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/?replytocom=2649#respond" onclick="return addComment.moveForm( &quot;div-comment-2649&quot;, &quot;2649&quot;, &quot;respond&quot;, &quot;1349&quot; )" aria-label="Reply to Fernando">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2 parent" id="comment-2651">
				<div id="div-comment-2651" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-0">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/#comment-2651">
			January 2, 2014 at 11:12 am</a>		</div>

		<p>Hi Fernando,</p>
<p>yes, the OS enforces this restrictions (by setting the MMU) when it loads the programs.</p>
<p>Having several sections is not strictly mandatory and is more a convention to predefine some properties and attributes (read-only code, read-write data) to the different parts of the program.</p>
<p>Note that the assumption «the code does not change» is true 99% of the time. The remaining 1% happens in very <a href="http://en.wikipedia.org/wiki/Self-modifying_code#Usage" rel="nofollow">rare cases</a>.</p>
<p>Other than that (the MMU and the OS), in principle nothing prevents you from accessing any position of the memory.</p>
<p>Regarding your question on the program that passes the array by value, I assume you wonder why it is the caller (and not the callee, the function) who frees the stack.</p>
<p>In this example, the function could free the stack. But to successfully achieve this it should do that between the <code>pop {r4, r5, r6, lr}</code> and the <code>bx lr</code>. This means that we should move the value of <code>r5</code> into some available scratch register (i.e. r1 to r3, r0 is already taken for the result) and then use it to adjust <code>sp</code>.</p>
<p>While in this case it would be feasible, it could become close to impossible to do if, for instance, we are passing several arrays by value: the number of items of the arrays after the first one will be in the stack, we will have to load these values in the stack into <code>r1</code> to <code>r3</code>… etc.</p>
<p>It is much easier to adjust the stack when the callee returns. And not only this, it seems sensible that the one who pushed everything onto the stack (the caller) must be the responsible to clean it up. Luckily this is just as simple to restore the <code>sp</code> to a previously stored value prior the preparation of the call.</p>
<p>I have fixed the typo you mentioned, you are right it must be <code>r0 &lt; r3</code>. Thanks.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/?replytocom=2651#respond" onclick="return addComment.moveForm( &quot;div-comment-2651&quot;, &quot;2651&quot;, &quot;respond&quot;, &quot;1349&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment even depth-3 parent" id="comment-2661">
				<div id="div-comment-2661" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-9e81362ff9654533accffc53762c9520-1" originals="32" src-orig="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn"><a href="http://devel.meruelo.com.es/blog/" rel="external nofollow" class="url">Fernando</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/#comment-2661">
			January 2, 2014 at 11:01 pm</a>		</div>

		<p>Thanks for your time. </p>
<p>Please, can you tell us in advance which ones are going to be the topics of your next posts? Really looking forward to reading them!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/?replytocom=2661#respond" onclick="return addComment.moveForm( &quot;div-comment-2661&quot;, &quot;2661&quot;, &quot;respond&quot;, &quot;1349&quot; )" aria-label="Reply to Fernando">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-4" id="comment-2663">
				<div id="div-comment-2663" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-1">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/#comment-2663">
			January 2, 2014 at 11:42 pm</a>		</div>

		<p>Hi Fernando,</p>
<p>well, I have not a plan fully laid out. It is more of a set of topics I feel like to talk about. I can say that the next chapter (still in the works) will be about using a frame pointer in a function.</p>
<p>About topics I still want to talk about: pointers to functions, and pointers to functions with lexical scopes (known as closures). I also want to talk about some more lower level stuff, probably caches, system calls, exceptions, etc.</p>
<p>I’m quite slow writing the posts because I like to have <i>interesting</i> examples. Usually I am only able to come up with very simple and boring examples so I try hard to devise more interesting ones.</p>
<p>Thanks for your interest!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/?replytocom=2663#respond" onclick="return addComment.moveForm( &quot;div-comment-2663&quot;, &quot;2663&quot;, &quot;respond&quot;, &quot;1349&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-2995">
				<div id="div-comment-2995" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/62e7549015b3534162c581a6e0ebfcf6?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/62e7549015b3534162c581a6e0ebfcf6?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-62e7549015b3534162c581a6e0ebfcf6-0" originals="32" src-orig="http://0.gravatar.com/avatar/62e7549015b3534162c581a6e0ebfcf6?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn"><a href="http://brg.vvs.ir" rel="external nofollow" class="url">Ahrovan</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/#comment-2995">
			January 11, 2014 at 2:19 pm</a>		</div>

		<p>Hi<br>
/* Many thanks * /</p>
<p>To use the interface ex,i2c or spi with assembler,you have any idea ?<br>
about cubieboard (arm7) we can use this Tutorial,but CPU architecture is different,where I can learn that architecture؟</p>
<p>/* Thanks ,Ahrovan */</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/?replytocom=2995#respond" onclick="return addComment.moveForm( &quot;div-comment-2995&quot;, &quot;2995&quot;, &quot;respond&quot;, &quot;1349&quot; )" aria-label="Reply to Ahrovan">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2 parent" id="comment-2996">
				<div id="div-comment-2996" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-2">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/#comment-2996">
			January 11, 2014 at 2:40 pm</a>		</div>

		<p>Hi Ahrovan,</p>
<p>I’m not familiar with ex, i2c or spi. So I cannot help you much here. Sorry.</p>
<p>Regarding ARMv7, your Cubieboard comes with an ARM Cortex-A8 which implements the ARMv7-A architecture. The detailed document of that architecture is not available in the ARM website but a google search of “armv7-a architecture reference manual issue C” should show some links to a large PDF. Make sure it is the document that describes “ARMv7-A and ARMv7-R” and not the one that describes “ARMv7-M”.</p>
<p>That said, the Cubieboard processor is binary compatible with the processor of the Raspberry Pi so there are very small differences, actually.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/?replytocom=2996#respond" onclick="return addComment.moveForm( &quot;div-comment-2996&quot;, &quot;2996&quot;, &quot;respond&quot;, &quot;1349&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment even depth-3" id="comment-3017">
				<div id="div-comment-3017" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/62e7549015b3534162c581a6e0ebfcf6?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/62e7549015b3534162c581a6e0ebfcf6?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-62e7549015b3534162c581a6e0ebfcf6-1" originals="32" src-orig="http://0.gravatar.com/avatar/62e7549015b3534162c581a6e0ebfcf6?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn"><a href="http://brg.vvs.ir" rel="external nofollow" class="url">Ahrovan</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/#comment-3017">
			January 12, 2014 at 11:18 am</a>		</div>

		<p>OK</p>
<p>I’ll download the document<br>
/* <a href="http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0406c/index.html" rel="nofollow">http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0406c/index.html</a> */</p>
<p>During the test &amp; programming, I will ask you to help </p>
<p>thanks</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/?replytocom=3017#respond" onclick="return addComment.moveForm( &quot;div-comment-3017&quot;, &quot;3017&quot;, &quot;respond&quot;, &quot;1349&quot; )" aria-label="Reply to Ahrovan">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-even depth-1 parent" id="comment-962043">
				<div id="div-comment-962043" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/1371ac8d536f38aed05ec096939db2ef?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/1371ac8d536f38aed05ec096939db2ef?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-1371ac8d536f38aed05ec096939db2ef-0" originals="32" src-orig="http://1.gravatar.com/avatar/1371ac8d536f38aed05ec096939db2ef?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Paul</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/#comment-962043">
			March 8, 2016 at 11:25 pm</a>		</div>

		<p>I realize this is a 2014 tutorial, but I have found it just recently and it is truly fantastic.  I’ve always wanted to learn assembly, and though the raspberry pi would be a perfect tool to learn it on.</p>
<p>I’ve been following along, and everything is going great!  I know you’ve said you weren’t concerned with efficiency, or minimizing redundant statements, but I’m just wondering why line #56 in double_array.s?   Why do we need to store the item value back into the array in the print function?  I commented this line out and the program still works as expected.  Was this an oversight, or do you have any thoughts why you had that line?  I’m really interested in learning as much as I can.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/?replytocom=962043#respond" onclick="return addComment.moveForm( &quot;div-comment-962043&quot;, &quot;962043&quot;, &quot;respond&quot;, &quot;1349&quot; )" aria-label="Reply to Paul">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2 parent" id="comment-962098">
				<div id="div-comment-962098" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-3">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/#comment-962098">
			March 9, 2016 at 9:19 pm</a>		</div>

		<p>Hi Paul,</p>
<p>yes, you’re right, this is an oversight by my side.</p>
<p>I don’t remember but probably copied and pasted the code of function <code>double_array</code> and when adapting it to print the values I forgot to remove this useless store.</p>
<p>I have updated the code.</p>
<p>Thank you very much for spotting it!</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/?replytocom=962098#respond" onclick="return addComment.moveForm( &quot;div-comment-962098&quot;, &quot;962098&quot;, &quot;respond&quot;, &quot;1349&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment odd alt depth-3 parent" id="comment-962119">
				<div id="div-comment-962119" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/1371ac8d536f38aed05ec096939db2ef?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/1371ac8d536f38aed05ec096939db2ef?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-1371ac8d536f38aed05ec096939db2ef-1" originals="32" src-orig="http://1.gravatar.com/avatar/1371ac8d536f38aed05ec096939db2ef?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Paul</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/#comment-962119">
			March 10, 2016 at 5:12 am</a>		</div>

		<p>Wow, I really didn’t expect a response now.  I’m just so glad to hear that I’m actually getting it!!!  You really do a great job at explaining the material.  I’ve been doing programming for many years, some at the C/C++ level, but mainly in Java and Oracle SQL &amp; PL/SQL, so dipping down into Assembly is really cool.  I’ve also started to adjunct at the University I work at, so far for the Intro to Computer Science course, but have a chance to maybe teach a hardware class.  Your tutorial was just what I needed to get myself started.  Thanks again!!!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/?replytocom=962119#respond" onclick="return addComment.moveForm( &quot;div-comment-962119&quot;, &quot;962119&quot;, &quot;respond&quot;, &quot;1349&quot; )" aria-label="Reply to Paul">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-4" id="comment-962145">
				<div id="div-comment-962145" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-4">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/#comment-962145">
			March 10, 2016 at 9:57 am</a>		</div>

		<p>Hi Paul,</p>
<p>thank you very much for your kind words! I’m glad to see that this tutorial is useful.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/?replytocom=962145#respond" onclick="return addComment.moveForm( &quot;div-comment-962145&quot;, &quot;962145&quot;, &quot;respond&quot;, &quot;1349&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1 parent" id="comment-976716">
				<div id="div-comment-976716" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/098597ee293c3628de503d0a2c11da57?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/098597ee293c3628de503d0a2c11da57?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-098597ee293c3628de503d0a2c11da57-0" originals="32" src-orig="http://0.gravatar.com/avatar/098597ee293c3628de503d0a2c11da57?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">B2B</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/#comment-976716">
			October 11, 2016 at 11:36 am</a>		</div>

		<p>Pointer to pointer… I don’t get it…</p>
<p>addr_of_number_1 and addr_of_number_2 should be also pointers to pointers, or not?  :O</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/?replytocom=976716#respond" onclick="return addComment.moveForm( &quot;div-comment-976716&quot;, &quot;976716&quot;, &quot;respond&quot;, &quot;1349&quot; )" aria-label="Reply to B2B">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2" id="comment-976983">
				<div id="div-comment-976983" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-5">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/#comment-976983">
			October 15, 2016 at 12:38 pm</a>		</div>

		<p>Hi,</p>
<p>as labels “addr_of_number_1” and “addr_of_number_2” designate addresses but do not have a location. The addresses designated by the labels are encoded in the program of course, but not in a useable way as a pointer.</p>
<p>Then we use these addresses to find a location. This location contains the real address of <code>number</code>. This location in memory can be understood as a pointer. Not one that we can change as being able to change the address of a variable would be dangerous!</p>
<p>So in principle, there is only one pointer involved here. Here pointer meaning “location of memory containing an address”.</p>
<p>Hope it is clear.</p>
<p>Kind regards,<br>
Roger</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/11/20/arm-assembler-raspberry-pi-chapter-17/?replytocom=976983#respond" onclick="return addComment.moveForm( &quot;div-comment-976983&quot;, &quot;976983&quot;, &quot;respond&quot;, &quot;1349&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		</ul>
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2013/11/20/arm-assembler-raspberry-pi-chapter-17/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="1349" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="b2c3a1b571"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496666379288"></form>
			</div><!-- #respond -->
	</div>	</div>
