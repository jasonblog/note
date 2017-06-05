# Trampolines


<div class="main">
			<h1>ARM assembler in Raspberry Pi – Chapter 24 </h1>
		<p class="post-meta"><span class="date">January 9, 2015</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2015/01/09/arm-assembler-raspberry-pi-chapter-24/#comments">7</a></span> </p>
		<p>
Today we will continue with nested functions.
</p>
<p><span id="more-1768"></span></p>
<h2>Sorting</h2>
<p>
We will first take a detour. The C function <code>qsort</code> can be used to sort any kind of array. Its C signature is the following.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #993333;">void</span> <span style="color: #000066;">qsort</span><span style="color: #009900;">(</span><span style="color: #993333;">void</span> <span style="color: #339933;">*</span>base<span style="color: #339933;">,</span>
     <span style="color: #993333;">size_t</span> nmemb<span style="color: #339933;">,</span>
     <span style="color: #993333;">size_t</span> size<span style="color: #339933;">,</span>
     <span style="color: #993333;">int</span> <span style="color: #009900;">(</span><span style="color: #339933;">*</span>compar<span style="color: #009900;">)</span><span style="color: #009900;">(</span><span style="color: #993333;">const</span> <span style="color: #993333;">void</span> <span style="color: #339933;">*,</span> <span style="color: #993333;">const</span> <span style="color: #993333;">void</span> <span style="color: #339933;">*</span><span style="color: #009900;">)</span><span style="color: #009900;">)</span><span style="color: #339933;">;</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">void qsort(void *base,
     size_t nmemb,
     size_t size,
     int (*compar)(const void *, const void *));</p></div>

<p>
<code>qsort</code> returns <code>void</code>, this is, it does not return anything because it performs the sort <em>in place</em>. This means that we will pass a (potentially unsorted) array called <code>base</code> of length <code>nmemb</code> to <code>qsort</code>. When <code>qsort</code> returns, the elements in this array will be sorted. If <code>qsort</code> were able to just sort a specific kind of arrays it would be rather limited. In order to be able to sort any array, <code>qsort</code> requires the <code>size</code> of each element in the array. Note that the array is passed by reference (otherwise the in place sorting would not be possible): <code>void*</code> is the C way to say «I accept an address to any kind of data».
</p>
<p>We will come back later to the <code>compar</code> bit of <code>qsort</code>.</p>
<h3>Print an array</h3>
<p>
Before we sort an array, we need a way to examine it. We will use for that a function <code>print_array</code> that prints an array of integers.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> print<span style="color: #339933;">-</span>array<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
<span style="color: #339933;">/*</span> declare an array of <span style="color: #ff0000;">10</span> integers called my_array <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
my_array<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">82</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">70</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">93</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">77</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">91</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">30</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">42</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">6</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">92</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">64</span>
&nbsp;
<span style="color: #339933;">/*</span> format strings for printf <span style="color: #339933;">*/</span>
<span style="color: #339933;">/*</span> format string that prints an integer plus a space <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
integer_printf<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"%d "</span>
<span style="color: #339933;">/*</span> format string that simply prints a newline <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
newline_printf<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"\n"</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
print_array<span style="color: #339933;">:</span>
    <span style="color: #339933;">/*</span> r0 will be the address of the integer array <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r1 will be the number of items <span style="color: #00007f; font-weight: bold;">in</span> the array <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>  <span style="color: #339933;">/*</span> keep r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6 <span style="color: #00007f; font-weight: bold;">and</span> lr <span style="color: #00007f; font-weight: bold;">in</span> the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> r0             <span style="color: #339933;">/*</span> r4 ← r0<span style="color: #339933;">.</span> keep the address of the array <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> r1             <span style="color: #339933;">/*</span> r5 ← r1<span style="color: #339933;">.</span> keep the number of items <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r6<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>             <span style="color: #339933;">/*</span> r6 ← <span style="color: #ff0000;">0</span><span style="color: #339933;">.</span>  current item to print <span style="color: #339933;">*/</span>
&nbsp;
    b <span style="color: #339933;">.</span>Lprint_array_check_loop <span style="color: #339933;">/*</span> go to the condition check of the <span style="color: #00007f; font-weight: bold;">loop</span> <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">.</span>Lprint_array_loop<span style="color: #339933;">:</span>
      <span style="color: #339933;">/*</span> prepare the <span style="color: #00007f; font-weight: bold;">call</span> to printf <span style="color: #339933;">*/</span>
      ldr r0<span style="color: #339933;">,</span> addr_of_integer_printf  <span style="color: #339933;">/*</span> r0 ← &amp;integer_printf <span style="color: #339933;">*/</span>
      <span style="color: #339933;">/*</span> load the item r6 <span style="color: #00007f; font-weight: bold;">in</span> the array <span style="color: #00007f; font-weight: bold;">in</span> address r4<span style="color: #339933;">.</span>
         elements are of size <span style="color: #ff0000;">4</span> bytes so we need to multiply r6 by <span style="color: #ff0000;">4</span> <span style="color: #339933;">*/</span>
      ldr r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #339933;">,</span> <span style="color: #339933;">+</span>r6<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">]</span>       <span style="color: #339933;">/*</span> r1 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r4 <span style="color: #339933;">+</span> r6 &lt;&lt; <span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">)</span>
                                         this is the same as
                                         r1 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r4 <span style="color: #339933;">+</span> r6 <span style="color: #339933;">*</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
      <span style="color: #46aa03; font-weight: bold;">bl</span> printf                       <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> printf <span style="color: #339933;">*/</span>
&nbsp;
      <span style="color: #00007f; font-weight: bold;">add</span> r6<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>                  <span style="color: #339933;">/*</span> r6 ← r6 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>Lprint_array_check_loop<span style="color: #339933;">:</span> 
      <span style="color: #00007f; font-weight: bold;">cmp</span> r6<span style="color: #339933;">,</span> r5               <span style="color: #339933;">/*</span> perform r6 <span style="color: #339933;">-</span> r5 <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
      bne <span style="color: #339933;">.</span>Lprint_array_loop   <span style="color: #339933;">/*</span> if cpsr states that r6 is <span style="color: #00007f; font-weight: bold;">not</span> equal to r5
                                  branch to the body of the <span style="color: #00007f; font-weight: bold;">loop</span> <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> prepare <span style="color: #00007f; font-weight: bold;">call</span> to printf <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> addr_of_newline_printf <span style="color: #339933;">/*</span> r0 ← &amp;newline_printf <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> printf
&nbsp;
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>   <span style="color: #339933;">/*</span> restore r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6 <span style="color: #00007f; font-weight: bold;">and</span> lr from the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr                  <span style="color: #339933;">/*</span> return <span style="color: #339933;">*/</span>
&nbsp;
addr_of_integer_printf<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> integer_printf
addr_of_newline_printf<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> newline_printf
&nbsp;
<span style="color: #339933;">.</span>globl main
main<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>             <span style="color: #339933;">/*</span> keep r4 <span style="color: #00007f; font-weight: bold;">and</span> lr <span style="color: #00007f; font-weight: bold;">in</span> the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> prepare <span style="color: #00007f; font-weight: bold;">call</span> to print_array <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> addr_of_my_array  <span style="color: #339933;">/*</span> r0 ← &amp;my_array <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">10</span>               <span style="color: #339933;">/*</span> r1 ← <span style="color: #ff0000;">10</span>
                                 our array is of length <span style="color: #ff0000;">10</span> <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> print_array            <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> print_array <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>                <span style="color: #339933;">/*</span> r0 ← <span style="color: #ff0000;">0</span> set errorcode to <span style="color: #ff0000;">0</span> prior returning from main <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>              <span style="color: #339933;">/*</span> restore r4 <span style="color: #00007f; font-weight: bold;">and</span> lr <span style="color: #00007f; font-weight: bold;">in</span> the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr                     <span style="color: #339933;">/*</span> return <span style="color: #339933;">*/</span>
&nbsp;
addr_of_my_array<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> my_array</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* print-array.s */

.data

/* declare an array of 10 integers called my_array */
.align 4
my_array: .word 82, 70, 93, 77, 91, 30, 42, 6, 92, 64

/* format strings for printf */
/* format string that prints an integer plus a space */
.align 4
integer_printf: .asciz "%d "
/* format string that simply prints a newline */
.align 4
newline_printf: .asciz "\n"

.text

print_array:
    /* r0 will be the address of the integer array */
    /* r1 will be the number of items in the array */
    push {r4, r5, r6, lr}  /* keep r4, r5, r6 and lr in the stack */

    mov r4, r0             /* r4 ← r0. keep the address of the array */
    mov r5, r1             /* r5 ← r1. keep the number of items */
    mov r6, #0             /* r6 ← 0.  current item to print */

    b .Lprint_array_check_loop /* go to the condition check of the loop */

    .Lprint_array_loop:
      /* prepare the call to printf */
      ldr r0, addr_of_integer_printf  /* r0 ← &amp;integer_printf */
      /* load the item r6 in the array in address r4.
         elements are of size 4 bytes so we need to multiply r6 by 4 */
      ldr r1, [r4, +r6, LSL #2]       /* r1 ← *(r4 + r6 &lt;&lt; 2)
                                         this is the same as
                                         r1 ← *(r4 + r6 * 4) */
      bl printf                       /* call printf */

      add r6, r6, #1                  /* r6 ← r6 + 1 */
    .Lprint_array_check_loop: 
      cmp r6, r5               /* perform r6 - r5 and update cpsr */
      bne .Lprint_array_loop   /* if cpsr states that r6 is not equal to r5
                                  branch to the body of the loop */

    /* prepare call to printf */
    ldr r0, addr_of_newline_printf /* r0 ← &amp;newline_printf */
    bl printf
    
    pop {r4, r5, r6, lr}   /* restore r4, r5, r6 and lr from the stack */
    bx lr                  /* return */

addr_of_integer_printf: .word integer_printf
addr_of_newline_printf: .word newline_printf

.globl main
main:
    push {r4, lr}             /* keep r4 and lr in the stack */

    /* prepare call to print_array */
    ldr r0, addr_of_my_array  /* r0 ← &amp;my_array */
    mov r1, #10               /* r1 ← 10
                                 our array is of length 10 */
    bl print_array            /* call print_array */

    mov r0, #0                /* r0 ← 0 set errorcode to 0 prior returning from main */
    pop {r4, lr}              /* restore r4 and lr in the stack */
    bx lr                     /* return */

addr_of_my_array: .word my_array</p></div>

<p>
The code above is pretty straightforward and it does not feature anything that has not been seen in previous installments. Running it simply prints the current contents of the array.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="shell" style="font-family:monospace;">$ ./print-array 
82 70 93 77 91 30 42 6 92 64</pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ ./print-array 
82 70 93 77 91 30 42 6 92 64</p></div>

<h3>Comparison</h3>
<p>
Above, when we talked about <code>qsort</code> we skipped the <code>compar</code> parameter. What is <code>compar</code>? It is a an address to a function. The funky syntax for C tells us that this function, if it is ever called, will be passed two addreses (again, it does not care what they are, so they are <code>void*</code>) and returns an integer. The <a href="http://man7.org/linux/man-pages/man3/qsort.3.html" title="manual of qsort">manual of qsort</a> explains that this function has to return lower than zero, zero or greater than zero. If the object in the address of the first parameter of <code>compar</code> is <em>lower</em> than the object in the address of the second parameter, then it has to return lower than zero. If they are <em>equal</em>, it should return zero. If the first object is <em>greater</em> than the second, then it should return greater than zero.
</p>
<p>
If you wonder why the parameters of <code>compar</code> are actually <code>const void*</code> rather than <code>void*</code>, it is the C way of telling us that the data of the referenced objects cannot change during the comparison. This may sound obvious given that changing things is not the job of a comparison function. Passing them by reference would let us to change them. So this is reminder that we should not.
</p>
<p>
Since our array is an array of integers we will have to compare integers: let’s write a function that, given two pointers to integers (i.e. addresses) behaves as stated above.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>19
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">integer_comparison<span style="color: #339933;">:</span>
    <span style="color: #339933;">/*</span> r0 will be the address to the first integer <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r1 will be the address to the second integer <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>    <span style="color: #339933;">/*</span> r0 ← <span style="color: #339933;">*</span>r0
                       load the integer pointed by r0 <span style="color: #00007f; font-weight: bold;">in</span> r0 <span style="color: #339933;">*/</span>
    ldr r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #009900; font-weight: bold;">]</span>    <span style="color: #339933;">/*</span> r1 ← <span style="color: #339933;">*</span>r1
                       load the integer pointed by r1 <span style="color: #00007f; font-weight: bold;">in</span> r1 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> r1      <span style="color: #339933;">/*</span> compute r0 <span style="color: #339933;">-</span> r1 <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
    moveq r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>    <span style="color: #339933;">/*</span> if cpsr means that r0 == r1 then r0 ←  <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    movlt r0<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">1</span>   <span style="color: #339933;">/*</span> if cpsr means that r0 &lt;  r1 then r0 ← <span style="color: #339933;">-</span><span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    movgt r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>    <span style="color: #339933;">/*</span> if cpsr means that r0 &gt;  r1 then r0 ←  <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr           <span style="color: #339933;">/*</span> return <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">integer_comparison:
    /* r0 will be the address to the first integer */
    /* r1 will be the address to the second integer */
    ldr r0, [r0]    /* r0 ← *r0
                       load the integer pointed by r0 in r0 */
    ldr r1, [r1]    /* r1 ← *r1
                       load the integer pointed by r1 in r1 */

    cmp r0, r1      /* compute r0 - r1 and update cpsr */
    moveq r0, #0    /* if cpsr means that r0 == r1 then r0 ←  0 */
    movlt r0, #-1   /* if cpsr means that r0 &lt;  r1 then r0 ← -1 */
    movgt r0, #1    /* if cpsr means that r0 &gt;  r1 then r0 ←  1 */
    bx lr           /* return */</p></div>

<p>
Function <code>integer_comparison</code> does not feature anything new either: it simply avoids branches by using predication as we saw in chapter 11.
</p>
<p>
Now we have the last missing bit to be able to call <code>qsort</code>. Here is a program that prints (only <code>main</code> is shown) the array twice, before sorting it and after sorting it.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>66
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">.</span>globl main
main<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>             <span style="color: #339933;">/*</span> keep r4 <span style="color: #00007f; font-weight: bold;">and</span> lr <span style="color: #00007f; font-weight: bold;">in</span> the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> prepare <span style="color: #00007f; font-weight: bold;">call</span> to print_array <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> addr_of_my_array  <span style="color: #339933;">/*</span> r0 ← &amp;my_array <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">10</span>               <span style="color: #339933;">/*</span> r1 ← <span style="color: #ff0000;">10</span>
                                 our array is of length <span style="color: #ff0000;">10</span> <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> print_array            <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> print_array <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> prepare <span style="color: #00007f; font-weight: bold;">call</span> to qsort <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span>
    void qsort<span style="color: #009900; font-weight: bold;">(</span>void <span style="color: #339933;">*</span>base<span style="color: #339933;">,</span>
         size_t nmemb<span style="color: #339933;">,</span>
         size_t size<span style="color: #339933;">,</span>
         <span style="color: #00007f; font-weight: bold;">int</span> <span style="color: #009900; font-weight: bold;">(</span><span style="color: #339933;">*</span>compar<span style="color: #009900; font-weight: bold;">)</span><span style="color: #009900; font-weight: bold;">(</span>const void <span style="color: #339933;">*,</span> const void <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #666666; font-style: italic;">;</span>
    <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> addr_of_my_array  <span style="color: #339933;">/*</span> r0 ← &amp;my_array
                                 base <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">10</span>               <span style="color: #339933;">/*</span> r1 ← <span style="color: #ff0000;">10</span>
                                 nmemb = number of members
                                 our array is <span style="color: #ff0000;">10</span> elements long <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>                <span style="color: #339933;">/*</span> r2 ← <span style="color: #ff0000;">4</span>
                                 size of each member is <span style="color: #ff0000;">4</span> bytes <span style="color: #339933;">*/</span>
    ldr r3<span style="color: #339933;">,</span> addr_of_integer_comparison
                              <span style="color: #339933;">/*</span> r3 ← &amp;integer_comparison
                                 compar <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> qsort                  <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> qsort <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> now print again to see if elements were sorted <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> prepare <span style="color: #00007f; font-weight: bold;">call</span> to print_array <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> addr_of_my_array  <span style="color: #339933;">/*</span> r0 ← &amp;my_array <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">10</span>               <span style="color: #339933;">/*</span> r1 ← <span style="color: #ff0000;">10</span>
                                 our array is of length <span style="color: #ff0000;">10</span> <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> print_array            <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> print_array <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>                <span style="color: #339933;">/*</span> r0 ← <span style="color: #ff0000;">0</span> set errorcode to <span style="color: #ff0000;">0</span> prior returning from main <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>              <span style="color: #339933;">/*</span> restore r4 <span style="color: #00007f; font-weight: bold;">and</span> lr <span style="color: #00007f; font-weight: bold;">in</span> the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr                     <span style="color: #339933;">/*</span> return <span style="color: #339933;">*/</span>
&nbsp;
addr_of_my_array<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> my_array
addr_of_integer_comparison <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> integer_comparison</pre></td></tr></tbody></table><p class="theCode" style="display:none;">.globl main
main:
    push {r4, lr}             /* keep r4 and lr in the stack */

    /* prepare call to print_array */
    ldr r0, addr_of_my_array  /* r0 ← &amp;my_array */
    mov r1, #10               /* r1 ← 10
                                 our array is of length 10 */
    bl print_array            /* call print_array */

    /* prepare call to qsort */
    /*
    void qsort(void *base,
         size_t nmemb,
         size_t size,
         int (*compar)(const void *, const void *));
    */
    ldr r0, addr_of_my_array  /* r0 ← &amp;my_array
                                 base */
    mov r1, #10               /* r1 ← 10
                                 nmemb = number of members
                                 our array is 10 elements long */
    mov r2, #4                /* r2 ← 4
                                 size of each member is 4 bytes */
    ldr r3, addr_of_integer_comparison
                              /* r3 ← &amp;integer_comparison
                                 compar */
    bl qsort                  /* call qsort */

    /* now print again to see if elements were sorted */
    /* prepare call to print_array */
    ldr r0, addr_of_my_array  /* r0 ← &amp;my_array */
    mov r1, #10               /* r1 ← 10
                                 our array is of length 10 */
    bl print_array            /* call print_array */

    mov r0, #0                /* r0 ← 0 set errorcode to 0 prior returning from main */
    pop {r4, lr}              /* restore r4 and lr in the stack */
    bx lr                     /* return */

addr_of_my_array: .word my_array
addr_of_integer_comparison : .word integer_comparison</p></div>

<p>
If we put everything together, we can verify that our array is effectively sorted after the call to the <code>qsort</code> function.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="shell" style="font-family:monospace;">$ ./sort-array 
82 70 93 77 91 30 42 6 92 64 
6 30 42 64 70 77 82 91 92 93</pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ ./sort-array 
82 70 93 77 91 30 42 6 92 64 
6 30 42 64 70 77 82 91 92 93</p></div>

<h3>What is going on?</h3>
<p>
C function <code>qsort</code> implements a sorting algorithm (the C Standard does not specify which one must be but it is usually a fine-tuned version of <a href="http://en.wikipedia.org/wiki/Quicksort" title="quicksort">quicksort</a>) which at some point will require to compare two elements. To do this, <code>qsort</code> calls the function <code>compar</code>.
</p>
<h2>Count how many comparisons happen</h2>
<p>
Now, we want to count how many comparisons (i.e., how many times <code>integer_comparison</code> is called) when sorting the array. We could change <code>integer_comparison</code> so it increments a global counter.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #0000ff; font-weight: bold;">.data</span>
global_counter<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">0</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
integer_comparison_count_global<span style="color: #339933;">:</span>
    <span style="color: #339933;">/*</span> r0 will be the address to the first integer <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r1 will be the address to the second integer <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #009900; font-weight: bold;">}</span>   <span style="color: #339933;">/*</span> keep callee<span style="color: #339933;">-</span>saved registers <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>    <span style="color: #339933;">/*</span> r0 ← <span style="color: #339933;">*</span>r0
                       load the integer pointed by r0 <span style="color: #00007f; font-weight: bold;">in</span> r0 <span style="color: #339933;">*/</span>
    ldr r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #009900; font-weight: bold;">]</span>    <span style="color: #339933;">/*</span> r1 ← <span style="color: #339933;">*</span>r1
                       load the integer pointed by r1 <span style="color: #00007f; font-weight: bold;">in</span> r1 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> r1      <span style="color: #339933;">/*</span> compute r0 <span style="color: #339933;">-</span> r1 <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
    moveq r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>    <span style="color: #339933;">/*</span> if cpsr means that r0 == r1 then r0 ←  <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    movlt r0<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">1</span>   <span style="color: #339933;">/*</span> if cpsr means that r0 &lt;  r1 then r0 ← <span style="color: #339933;">-</span><span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    movgt r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>    <span style="color: #339933;">/*</span> if cpsr means that r0 &gt;  r1 then r0 ←  <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
&nbsp;
    ldr r4<span style="color: #339933;">,</span> addr_of_global_counter <span style="color: #339933;">/*</span> r4 ← &amp;global_counter <span style="color: #339933;">*/</span>
    ldr r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #009900; font-weight: bold;">]</span>    <span style="color: #339933;">/*</span> r5 ← <span style="color: #339933;">*</span>r4 <span style="color: #339933;">*/</span> 
    <span style="color: #00007f; font-weight: bold;">add</span> r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>  <span style="color: #339933;">/*</span> r5 ← r5 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #009900; font-weight: bold;">]</span>    <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span>r4 ← r5 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #009900; font-weight: bold;">}</span>    <span style="color: #339933;">/*</span> restore callee<span style="color: #339933;">-</span>saved registers <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr           <span style="color: #339933;">/*</span> return <span style="color: #339933;">*/</span>
addr_of_global_counter<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> global_counter</pre></td></tr></tbody></table><p class="theCode" style="display:none;">.data
global_counter: .word 0

.text
integer_comparison_count_global:
    /* r0 will be the address to the first integer */
    /* r1 will be the address to the second integer */
    push {r4, r5}   /* keep callee-saved registers */
    ldr r0, [r0]    /* r0 ← *r0
                       load the integer pointed by r0 in r0 */
    ldr r1, [r1]    /* r1 ← *r1
                       load the integer pointed by r1 in r1 */

    cmp r0, r1      /* compute r0 - r1 and update cpsr */
    moveq r0, #0    /* if cpsr means that r0 == r1 then r0 ←  0 */
    movlt r0, #-1   /* if cpsr means that r0 &lt;  r1 then r0 ← -1 */
    movgt r0, #1    /* if cpsr means that r0 &gt;  r1 then r0 ←  1 */

    ldr r4, addr_of_global_counter /* r4 ← &amp;global_counter */
    ldr r5, [r4]    /* r5 ← *r4 */ 
    add r5, r5, #1  /* r5 ← r5 + 1 */
    str r5, [r4]    /* *r4 ← r5 */

    pop {r4, r5}    /* restore callee-saved registers */
    bx lr           /* return */
addr_of_global_counter: .word global_counter</p></div>

<p>
But this post is about nested functions so we will use nested functions. Recall that nested functions can access local variables of their enclosing functions. So we will use a local variable of <code>main</code> as the counter and a nested function (of <code>main</code>) that performs the comparison and updates the counter.
</p>
<p>
In the last chapter we ended with a short discussion about nested functions. A downside of nested functions it that a pointer to a nested function requires two things: the address of the function and the lexical scope. If you check again the previous example where we call <code>qsort</code>, you will see that we do not mention anywhere the lexical scope. And there is a reason for that, it is not possible to pass it to <code>qsort</code>. In C, functions cannot be nested so a pointer to a function can just be the address of the function.
</p>
<h2>Trampoline</h2>
<p>
We will continue using the convention of the last chapter: <code>r10</code> will have the lexical scope upon the entry of the function. But <code>qsort</code>, when calls <code>integer_compare_count</code> will not set it for us: we cannout count on <code>r10</code> having a meaningful value when called from <code>qsort</code>. This means that <code>qsort</code> should actually call something that first sets <code>r10</code> with the right value and then jumps to <code>integer_compare_count</code>. We will call this ancillary code (or pseudofunction) a <em>trampoline</em>. The technique used here is similar to the one used by <a href="https://gcc.gnu.org/onlinedocs/gcc/Nested-Functions.html">GCC</a> described in <a href="http://www-cs-students.stanford.edu/~blynn/files/lexic.pdf">Lexical Closures for C++ (Thomas M. Breuel, USENIX C++ Conference Proceedings, October 17-21, 1988)</a>.
</p>
<p>
The trampoline is a small, always the same, sequence of instructions that behaves like a function and its only purpose is to set <code>r10</code> and then make an indirect call to the nested function. Since the sequence of instructions is always the same, the instructions themselves look like a <em>template</em>.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>174
175
176
177
178
179
180
181
182
183
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">.</span>Laddr_trampoline_template <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #339933;">.</span>Ltrampoline_template <span style="color: #339933;">/*</span> we will use this below <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span>Ltrampoline_template<span style="color: #339933;">:</span>
    <span style="color: #339933;">.</span>Lfunction_called<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">0x0</span>
    <span style="color: #339933;">.</span>Llexical_scope<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">0x0</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r10</span><span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>           <span style="color: #339933;">/*</span> keep callee<span style="color: #339933;">-</span>saved registers <span style="color: #339933;">*/</span>
    ldr r4<span style="color: #339933;">,</span> <span style="color: #339933;">.</span>Lfunction_called        <span style="color: #339933;">/*</span> r4 ← function called <span style="color: #339933;">*/</span>
    ldr <span style="color: #46aa03; font-weight: bold;">r10</span><span style="color: #339933;">,</span> <span style="color: #339933;">.</span>Llexical_scope         <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">r10</span> ← lexical scope <span style="color: #339933;">*/</span>
    blx r4                           <span style="color: #339933;">/*</span> indirect <span style="color: #00007f; font-weight: bold;">call</span> to r4 <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r10</span><span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>            <span style="color: #339933;">/*</span> restore callee<span style="color: #339933;">-</span>saved registers <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr                            <span style="color: #339933;">/*</span> return <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">.Laddr_trampoline_template : .word .Ltrampoline_template /* we will use this below */
.Ltrampoline_template:
    .Lfunction_called: .word 0x0
    .Llexical_scope: .word 0x0
    push {r4, r5, r10, lr}           /* keep callee-saved registers */
    ldr r4, .Lfunction_called        /* r4 ← function called */
    ldr r10, .Llexical_scope         /* r10 ← lexical scope */
    blx r4                           /* indirect call to r4 */
    pop {r4, r5, r10, lr}            /* restore callee-saved registers */
    bx lr                            /* return */</p></div>

<p>
I used the word template because while the instructions are not going to change, there are two items in the trampoline, labeled <code>function_called</code> and <code>lexical_scope</code>, that will have to be appropriately set before using the trampoline.
</p>
<p>
It may be easier to understand if you consider the code above as if it were data: see it as an array of integers. The first two integers, <code>function_called</code> and <code>lexical_scope</code>, are still zero but will be set at some point. The remaining elements in the array are other integers (we do not care which ones) that happen to encode ARM instructions. The cool thing is that these instructions refer to the two first integers, so by changing them we are indirectly changing what the trampoline does. This trampoline takes 8 words, so 32 bytes.
</p>
<p>
Let’s start with this example.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> trampoline<span style="color: #339933;">-</span>sort<span style="color: #339933;">-</span>arrays<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
<span style="color: #339933;">/*</span> declare an array of <span style="color: #ff0000;">10</span> integers called my_array <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
my_array<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">82</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">70</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">93</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">77</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">91</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">30</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">42</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">6</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">92</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">64</span>
&nbsp;
<span style="color: #339933;">/*</span> format strings for printf <span style="color: #339933;">*/</span>
<span style="color: #339933;">/*</span> format string that prints an integer plus a space <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
integer_printf<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"%d "</span>
<span style="color: #339933;">/*</span> format string that simply prints a newline <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
newline_printf<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"\n"</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span> <span style="color: #339933;">/*</span> format string for number of comparisons <span style="color: #339933;">*/</span>
comparison_message<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"Num comparisons: %d\n"</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* trampoline-sort-arrays.s */

.data

/* declare an array of 10 integers called my_array */
.align 4
my_array: .word 82, 70, 93, 77, 91, 30, 42, 6, 92, 64

/* format strings for printf */
/* format string that prints an integer plus a space */
.align 4
integer_printf: .asciz "%d "
/* format string that simply prints a newline */
.align 4
newline_printf: .asciz "\n"
.align 4 /* format string for number of comparisons */
comparison_message: .asciz "Num comparisons: %d\n"

.text</p></div>

<p>
The function <code>print_array</code> will be the same as above. Next is <code>main</code>.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>54
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">.</span>globl main
main<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> fp<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">/*</span> keep callee saved registers <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> fp<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span>                <span style="color: #339933;">/*</span> setup dynamic link <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">sub</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>            <span style="color: #339933;">/*</span> counter will be <span style="color: #00007f; font-weight: bold;">in</span> fp <span style="color: #339933;">-</span> <span style="color: #ff0000;">4</span> <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> note that now the <span style="color: #0000ff; font-weight: bold;">stack</span> is <span style="color: #ff0000;">8</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> set counter to zero <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>        <span style="color: #339933;">/*</span> r4 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> r4<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">/*</span> counter ← r4 <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">.globl main
main:
    push {r4, r5, r6, fp, lr} /* keep callee saved registers */
    mov fp, sp                /* setup dynamic link */

    sub sp, sp, #4            /* counter will be in fp - 4 */
    /* note that now the stack is 8-byte aligned */

    /* set counter to zero */
    mov r4, #0        /* r4 ← 0 */
    str r4, [fp, #-4] /* counter ← r4 */</p></div>

<p>
Nothing fancy here, we set the dynamic link, allocate space in the stack for the counter and set it to zero.
</p>
<p>
Now we make room for the trampoline in the stack. Recall that our trampoline takes 32 bytes.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>66
67
68
69
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">    <span style="color: #339933;">/*</span> Make room for the trampoline <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">sub</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">32</span> <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">sp</span> ← <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">-</span> <span style="color: #ff0000;">32</span> <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> note that <span style="color: #ff0000;">32</span> is a multiple of <span style="color: #ff0000;">8</span><span style="color: #339933;">,</span> so the <span style="color: #0000ff; font-weight: bold;">stack</span>
       is still <span style="color: #ff0000;">8</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">    /* Make room for the trampoline */
    sub sp, sp, #32 /* sp ← sp - 32 */
    /* note that 32 is a multiple of 8, so the stack
       is still 8-byte aligned */</p></div>

<p>
Now we will copy the trampoline template into the stack storage we just allocated. We do this with a loop that copies a word (4 bytes) at a time.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>71
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">    <span style="color: #339933;">/*</span> copy the trampoline <span style="color: #00007f; font-weight: bold;">into</span> the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">32</span>                        <span style="color: #339933;">/*</span> r4 ← <span style="color: #ff0000;">32</span> <span style="color: #339933;">*/</span>
    ldr r5<span style="color: #339933;">,</span> <span style="color: #339933;">.</span>Laddr_trampoline_template <span style="color: #339933;">/*</span> r4 ← &amp;trampoline_template <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r6<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span>                         <span style="color: #339933;">/*</span> r6 ← <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">*/</span>
    b <span style="color: #339933;">.</span>Lcopy_trampoline_loop_check     <span style="color: #339933;">/*</span> branch to copy_trampoline_loop_check <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">.</span>Lcopy_trampoline_loop<span style="color: #339933;">:</span>
        ldr r7<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r5<span style="color: #009900; font-weight: bold;">]</span>     <span style="color: #339933;">/*</span> r7 ← <span style="color: #339933;">*</span>r5 <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">str</span> r7<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r6<span style="color: #009900; font-weight: bold;">]</span>     <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span>r6 ← r7 <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">add</span> r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>   <span style="color: #339933;">/*</span> r5 ← r5 <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span> <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">add</span> r6<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>   <span style="color: #339933;">/*</span> r6 ← r6 <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span> <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">sub</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>   <span style="color: #339933;">/*</span> r4 ← r4 <span style="color: #339933;">-</span> <span style="color: #ff0000;">4</span> <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>Lcopy_trampoline_loop_check<span style="color: #339933;">:</span>
        <span style="color: #00007f; font-weight: bold;">cmp</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>                  <span style="color: #339933;">/*</span> compute r4 <span style="color: #339933;">-</span> <span style="color: #ff0000;">0</span> <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
        bgt <span style="color: #339933;">.</span>Lcopy_trampoline_loop  <span style="color: #339933;">/*</span> if cpsr means that r4 &gt; <span style="color: #ff0000;">0</span>
                                       then branch to copy_trampoline_loop <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">    /* copy the trampoline into the stack */
    mov r4, #32                        /* r4 ← 32 */
    ldr r5, .Laddr_trampoline_template /* r4 ← &amp;trampoline_template */
    mov r6, sp                         /* r6 ← sp */
    b .Lcopy_trampoline_loop_check     /* branch to copy_trampoline_loop_check */

    .Lcopy_trampoline_loop:
        ldr r7, [r5]     /* r7 ← *r5 */
        str r7, [r6]     /* *r6 ← r7 */
        add r5, r5, #4   /* r5 ← r5 + 4 */
        add r6, r6, #4   /* r6 ← r6 + 4 */
        sub r4, r4, #4   /* r4 ← r4 - 4 */
    .Lcopy_trampoline_loop_check:
        cmp r4, #0                  /* compute r4 - 0 and update cpsr */
        bgt .Lcopy_trampoline_loop  /* if cpsr means that r4 &gt; 0
                                       then branch to copy_trampoline_loop */</p></div>

<p>
In the loop above, <code>r4</code> counts how many bytes remain to copy. <code>r5</code> and <code>r6</code> are pointers inside the (source) trampoline and the (destination) stack, respectively. Since we copy 4 bytes at a time, all three registers are updated by 4.
</p>
<p>
Now we have the trampoline copied in the stack. Recall, it is just an array of words, the two first of which must be updated. The first 4 bytes must be the address of function to be called, i.e. <code>integer_comparison_count</code> and the second 4 bytes must be the static link, i.e. <code>fp</code>.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>88
89
90
91
92
93
94
95
96
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">    <span style="color: #339933;">/*</span> setup the trampoline <span style="color: #339933;">*/</span>
    ldr r4<span style="color: #339933;">,</span> addr_of_integer_comparison_count
                       <span style="color: #339933;">/*</span> r4 ← &amp;integer_comparison_count <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> r4<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">36</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>fp <span style="color: #339933;">-</span> <span style="color: #ff0000;">36</span><span style="color: #009900; font-weight: bold;">)</span> ← r4 <span style="color: #339933;">*/</span>
                       <span style="color: #339933;">/*</span> set the function_called <span style="color: #00007f; font-weight: bold;">in</span> the trampoline
                          to be &amp;integer_comparison_count <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> fp<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">32</span><span style="color: #009900; font-weight: bold;">]</span>  <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>fp <span style="color: #339933;">-</span> <span style="color: #ff0000;">32</span><span style="color: #009900; font-weight: bold;">)</span> ← fp <span style="color: #339933;">*/</span>
                        <span style="color: #339933;">/*</span> set the lexical_scope <span style="color: #00007f; font-weight: bold;">in</span> the trampoline
                           to be fp <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">    /* setup the trampoline */
    ldr r4, addr_of_integer_comparison_count
                       /* r4 ← &amp;integer_comparison_count */
    str r4, [fp, #-36] /* *(fp - 36) ← r4 */
                       /* set the function_called in the trampoline
                          to be &amp;integer_comparison_count */
    str fp, [fp, #-32]  /* *(fp - 32) ← fp */
                        /* set the lexical_scope in the trampoline
                           to be fp */</p></div>

<p>
Recall that our trampoline takes 32 bytes but in the stack we also have the counter. This is the reason why the trampoline starts in <code>fp - 36</code> (this is also the address of the first word of the trampoline, of course). The second word is then at <code>fp - 32</code>.
</p>
<p>
Now we proceed like in the sort example above: we print the array before sorting it and after sorting it. Before printing the sorted array, we will also print the number of comparisons that were performed.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>103
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">   <span style="color: #339933;">/*</span> prepare <span style="color: #00007f; font-weight: bold;">call</span> to print_array <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> addr_of_my_array <span style="color: #339933;">/*</span> r0 ← &amp;my_array <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">10</span>              <span style="color: #339933;">/*</span> r1 ← <span style="color: #ff0000;">10</span>
                                our array is of length <span style="color: #ff0000;">10</span> <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> print_array           <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> print_array <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> prepare <span style="color: #00007f; font-weight: bold;">call</span> to qsort <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span>
    void qsort<span style="color: #009900; font-weight: bold;">(</span>void <span style="color: #339933;">*</span>base<span style="color: #339933;">,</span>
         size_t nmemb<span style="color: #339933;">,</span>
         size_t size<span style="color: #339933;">,</span>
         <span style="color: #00007f; font-weight: bold;">int</span> <span style="color: #009900; font-weight: bold;">(</span><span style="color: #339933;">*</span>compar<span style="color: #009900; font-weight: bold;">)</span><span style="color: #009900; font-weight: bold;">(</span>const void <span style="color: #339933;">*,</span> const void <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #666666; font-style: italic;">;</span>
    <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> addr_of_my_array <span style="color: #339933;">/*</span> r0 ← &amp;my_array
                                base <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">10</span>              <span style="color: #339933;">/*</span> r1 ← <span style="color: #ff0000;">10</span>
                                nmemb = number of members
                                our array is <span style="color: #ff0000;">10</span> elements long <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>               <span style="color: #339933;">/*</span> r2 ← <span style="color: #ff0000;">4</span>
                                size of each member is <span style="color: #ff0000;">4</span> bytes <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">sub</span> r3<span style="color: #339933;">,</span> fp<span style="color: #339933;">,</span> #<span style="color: #ff0000;">28</span>          <span style="color: #339933;">/*</span> r3 ← fp <span style="color: #339933;">-</span> <span style="color: #ff0000;">28</span> <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> qsort                 <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> qsort <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> prepare <span style="color: #00007f; font-weight: bold;">call</span> to printf <span style="color: #339933;">*/</span>
    ldr r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>                    <span style="color: #339933;">/*</span> r1 ← counter <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> addr_of_comparison_message   <span style="color: #339933;">/*</span> r0 ← &amp;comparison_message <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> printf                            <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> printf <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> now print again the array to see if elements were sorted <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> prepare <span style="color: #00007f; font-weight: bold;">call</span> to print_array <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> addr_of_my_array  <span style="color: #339933;">/*</span> r0 ← &amp;my_array <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">10</span>               <span style="color: #339933;">/*</span> r1 ← <span style="color: #ff0000;">10</span>
                                 our array is of length <span style="color: #ff0000;">10</span> <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> print_array            <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> print_array <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">   /* prepare call to print_array */
    ldr r0, addr_of_my_array /* r0 ← &amp;my_array */
    mov r1, #10              /* r1 ← 10
                                our array is of length 10 */
    bl print_array           /* call print_array */

    /* prepare call to qsort */
    /*
    void qsort(void *base,
         size_t nmemb,
         size_t size,
         int (*compar)(const void *, const void *));
    */
    ldr r0, addr_of_my_array /* r0 ← &amp;my_array
                                base */
    mov r1, #10              /* r1 ← 10
                                nmemb = number of members
                                our array is 10 elements long */
    mov r2, #4               /* r2 ← 4
                                size of each member is 4 bytes */
    sub r3, fp, #28          /* r3 ← fp - 28 */
    bl qsort                 /* call qsort */

    /* prepare call to printf */
    ldr r1, [fp, #-4]                    /* r1 ← counter */
    ldr r0, addr_of_comparison_message   /* r0 ← &amp;comparison_message */
    bl printf                            /* call printf */

    /* now print again the array to see if elements were sorted */
    /* prepare call to print_array */
    ldr r0, addr_of_my_array  /* r0 ← &amp;my_array */
    mov r1, #10               /* r1 ← 10
                                 our array is of length 10 */
    bl print_array            /* call print_array */</p></div>

<p>
Note that the argument <code>compar</code> passed to qsort (line 123) is not the address of the nested function but the trampoline. In fact, it is not the trampoline but its third word since, as we know, the two first words of the trampoline are the address of the nested function to call and the lexical scope (that we set earlier, lines 91 and 94).
</p>
<p>
Finally we return from main as usual.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>139
140
141
142
143
144
145
146
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>                <span style="color: #339933;">/*</span> r0 ← <span style="color: #ff0000;">0</span> set errorcode to <span style="color: #ff0000;">0</span> prior returning from main <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> fp
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> fp<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>      <span style="color: #339933;">/*</span> restore callee<span style="color: #339933;">-</span>saved registers <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr                     <span style="color: #339933;">/*</span> return <span style="color: #339933;">*/</span>
&nbsp;
addr_of_my_array<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> my_array
addr_of_comparison_message <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> comparison_message</pre></td></tr></tbody></table><p class="theCode" style="display:none;">    mov r0, #0                /* r0 ← 0 set errorcode to 0 prior returning from main */

    mov sp, fp
    pop {r4, r5, r6, fp, lr}      /* restore callee-saved registers */
    bx lr                     /* return */

addr_of_my_array: .word my_array
addr_of_comparison_message : .word comparison_message</p></div>

<p>
The nested comparison function is next.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>148
149
150
151
152
153
154
155
156
157
158
159
160
161
162
163
164
165
166
167
168
169
170
171
172
173
174
175
176
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">    <span style="color: #339933;">/*</span> nested function integer comparison <span style="color: #339933;">*/</span>
    addr_of_integer_comparison_count <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> integer_comparison_count
    integer_comparison_count<span style="color: #339933;">:</span>
        <span style="color: #339933;">/*</span> r0 will be the address to the first integer <span style="color: #339933;">*/</span>
        <span style="color: #339933;">/*</span> r1 will be the address to the second integer <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r10</span><span style="color: #339933;">,</span> fp<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">/*</span> keep callee<span style="color: #339933;">-</span>saved registers <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">mov</span> fp<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span>                 <span style="color: #339933;">/*</span> setup dynamic link <span style="color: #339933;">*/</span>
&nbsp;
        ldr r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>    <span style="color: #339933;">/*</span> r0 ← <span style="color: #339933;">*</span>r0
                           load the integer pointed by r0 <span style="color: #00007f; font-weight: bold;">in</span> r0 <span style="color: #339933;">*/</span>
        ldr r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #009900; font-weight: bold;">]</span>    <span style="color: #339933;">/*</span> r1 ← <span style="color: #339933;">*</span>r1
                           load the integer pointed by r1 <span style="color: #00007f; font-weight: bold;">in</span> r1 <span style="color: #339933;">*/</span>
&nbsp;
        <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> r1      <span style="color: #339933;">/*</span> compute r0 <span style="color: #339933;">-</span> r1 <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
        moveq r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>    <span style="color: #339933;">/*</span> if cpsr means that r0 == r1 then r0 ←  <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
        movlt r0<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">1</span>   <span style="color: #339933;">/*</span> if cpsr means that r0 &lt;  r1 then r0 ← <span style="color: #339933;">-</span><span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
        movgt r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>    <span style="color: #339933;">/*</span> if cpsr means that r0 &gt;  r1 then r0 ←  <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
&nbsp;
        ldr r4<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span>  <span style="color: #339933;">/*</span> r4 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>fp <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span>
                             get static link <span style="color: #00007f; font-weight: bold;">in</span> the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
        ldr r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">/*</span> r5 ← counter
                             get value of counter <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">add</span> r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>    <span style="color: #339933;">/*</span> r5 ← r5 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">str</span> r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">/*</span> counter ← r5
                             update counter <span style="color: #339933;">*/</span>
&nbsp;
        <span style="color: #00007f; font-weight: bold;">mov</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> fp        <span style="color: #339933;">/*</span> restore <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r10</span><span style="color: #339933;">,</span> fp<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">/*</span> restore callee<span style="color: #339933;">-</span>saved registers <span style="color: #339933;">*/</span>
        <span style="color: #46aa03; font-weight: bold;">bx</span> lr           <span style="color: #339933;">/*</span> return <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">    /* nested function integer comparison */
    addr_of_integer_comparison_count : .word integer_comparison_count
    integer_comparison_count:
        /* r0 will be the address to the first integer */
        /* r1 will be the address to the second integer */
        push {r4, r5, r10, fp, lr} /* keep callee-saved registers */
        mov fp, sp                 /* setup dynamic link */

        ldr r0, [r0]    /* r0 ← *r0
                           load the integer pointed by r0 in r0 */
        ldr r1, [r1]    /* r1 ← *r1
                           load the integer pointed by r1 in r1 */
     
        cmp r0, r1      /* compute r0 - r1 and update cpsr */
        moveq r0, #0    /* if cpsr means that r0 == r1 then r0 ←  0 */
        movlt r0, #-1   /* if cpsr means that r0 &lt;  r1 then r0 ← -1 */
        movgt r0, #1    /* if cpsr means that r0 &gt;  r1 then r0 ←  1 */

        ldr r4, [fp, #8]  /* r4 ← *(fp + 8)
                             get static link in the stack */
        ldr r5, [r4, #-4] /* r5 ← counter
                             get value of counter */
        add r5, r5, #1    /* r5 ← r5 + 1 */
        str r5, [r4, #-4] /* counter ← r5
                             update counter */

        mov sp, fp        /* restore stack */
        pop {r4, r5, r10, fp, lr} /* restore callee-saved registers */
        bx lr           /* return */</p></div>

<p>
As you can see, the nested function expects <code>r10</code> to be correctly set. This is what the trampoline does.
</p>
<h2>Harvard architecture</h2>
<p>
If you try to run the program as shown, it will probably work. But it will do it by chance. The reason is that we are featuring some simple form of self modifying code.
</p>
<p>
The Raspberry Pi processor features a <a href="http://en.wikipedia.org/wiki/Modified_Harvard_architecture">modified Harvard architecture</a>. This means that at some point there exists a distinction between instructions memory (<code>.text</code>) and the data memory (<code>.data</code>). Nowadays there are not many processors that feature a strict distinction between instruction and data memory (so at some point the program and the data are both in <em>main memory</em>, commonly called the RAM) but such differentiation is kept for <em><a href="http://en.wikipedia.org/wiki/CPU_cache">caches</a></em>.
</p>
<p>
A cache is a smaller and faster memory, that sits between the processor and the main memory. It is used to speed up memory accesses since most of the time such accesses happen close to other memory accesses (i.e. accessing elements of an array, different local variables in the stack or one instruction after the other in implicit sequencing) or close in time (i.e. accessing several times the same local variable or executing the same instruction when the code is in a loop).
</p>
<p>
Most modern processors feature distinguished caches for data (called the <em>data cache</em>) and instructions (called the <em>instruction cache</em>). The reason for such differentiation is that accessing to memory to execute instruction has a different pattern than accessing to memory to load/store data. It is beneficial to make such distinction but it comes at some price: when a program manipulates data that later will be executed as instructions (like we did with the trampoline, but also when the operating system loads a program in memory) the view of the two caches respect to the program state becomes incoherent: changes that we did in the data will have effect in the data cache but not in the instruction cache. Conversely, since the instruction cache will only get data from the main memory (and not from the data cache), we need to <em>write back</em> all the changes we did in the data cache to the main memory (this is called <em>flushing</em> the cache). We also have to make sure the instruction cache effectively gets the instructions from the memory, rather than reusing previously loaded instructions (which would be stale now), so we have to <em>invalidate</em> (or clear) the instruction cache.
</p>
<p>
In ARM the instructions that flush and invalidate caches are privileged operations (done through coprocessor instructions on the coprocessor 15 which manages the memory system of the CPU). This means that only the operating system can execute such instructions. As you see, user code may have to request a cache clear. Linux provides a <code>cacheflush</code> system call for this purpose.<br>
Recall that in chapter 19 we saw ho to make system calls.</p>
<p>
<a href="http://lxr.free-electrons.com/source/arch/arm/kernel/traps.c#L597">According to the Linux kernel</a>, register r0 must contain the address of the beginning of the region to be flushed and invalidated. <code>r1</code> must contain the address of the first byte that will not be invalidated. <code>r2</code> must be zero. The <a href="http://lxr.free-electrons.com/source/arch/arm/include/uapi/asm/unistd.h#L422">cacheflush service number</a>, that has to be set in <code>r7</code> is <code>0xf0002</code>.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r7<span style="color: #009900; font-weight: bold;">}</span>          <span style="color: #339933;">/*</span> keep r7 because we are going to modify it <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r7<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0xf0000</span>   <span style="color: #339933;">/*</span> r7 ← <span style="color: #ff0000;">0xf0000</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r7<span style="color: #339933;">,</span> r7<span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span>     <span style="color: #339933;">/*</span> r7 ← r7 <span style="color: #339933;">+</span> <span style="color: #ff0000;">2</span><span style="color: #339933;">.</span> So r7 ← <span style="color: #ff0000;">0xf0002</span>
                          We <span style="color: #0000ff; font-weight: bold;">do</span> this <span style="color: #00007f; font-weight: bold;">in</span> two steps because
                          we cannot encode <span style="color: #ff0000;">0xf0002</span> <span style="color: #00007f; font-weight: bold;">in</span>
                          the instruction <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span>         <span style="color: #339933;">/*</span> r0 ← <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r1<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">32</span>    <span style="color: #339933;">/*</span> r1 ← <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">+</span> <span style="color: #ff0000;">32</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>         <span style="color: #339933;">/*</span> r2 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    swi <span style="color: #ff0000;">0</span>              <span style="color: #339933;">/*</span> system <span style="color: #00007f; font-weight: bold;">call</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">pop</span>  <span style="color: #009900; font-weight: bold;">{</span>r7<span style="color: #009900; font-weight: bold;">}</span>          <span style="color: #339933;">/*</span> restore r7 <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">    push {r7}          /* keep r7 because we are going to modify it */
    mov r7, #0xf0000   /* r7 ← 0xf0000 */
    add r7, r7, #2     /* r7 ← r7 + 2. So r7 ← 0xf0002
                          We do this in two steps because
                          we cannot encode 0xf0002 in
                          the instruction */
    mov r0, sp         /* r0 ← sp */
    add r1, sp, #32    /* r1 ← sp + 32 */
    mov r2, #0         /* r2 ← 0 */
    swi 0              /* system call */
    pop  {r7}          /* restore r7 */</p></div>

<p>
As an alternative we can call an internal function implemented in <code>libgcc</code> (<a href="https://gcc.gnu.org/onlinedocs/gccint/Libgcc.html">the GCC low-level runtime library</a>) called <a href="https://gcc.gnu.org/onlinedocs/gccint/Miscellaneous-routines.html">__clear_cache</a>. This function will internally call the Linux service.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>98
99
100
101
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">    <span style="color: #339933;">/*</span> prepare <span style="color: #00007f; font-weight: bold;">call</span> to __clear_cache <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span>       <span style="color: #339933;">/*</span> r0 ← <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r1<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">32</span>  <span style="color: #339933;">/*</span> r1 ← <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">+</span> <span style="color: #ff0000;">32</span> <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> __clear_cache <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> __clear_cache <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">    /* prepare call to __clear_cache */
    mov r0, sp       /* r0 ← sp */
    add r1, sp, #32  /* r1 ← sp + 32 */
    bl __clear_cache /* call __clear_cache */</p></div>

<p>
We will invalidate and flush the caches right after setting up the trampoline (lines 89 to 94).
</p>
<p>
Now it only remains to run our program.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="shell" style="font-family:monospace;">$ ./trampoline-sort-array 
82 70 93 77 91 30 42 6 92 64 
Num comparisons: 22
6 30 42 64 70 77 82 91 92 93</pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ ./trampoline-sort-array 
82 70 93 77 91 30 42 6 92 64 
Num comparisons: 22
6 30 42 64 70 77 82 91 92 93</p></div>

<p>
You can see the full listing <a href="https://github.com/rofirrim/raspberry-pi-assembler/blob/master/chapter24/trampoline-sort-array.s">here</a>.
</p>
<h2>Discussion</h2>
<p>
Given that nested functions require a lexical scope, they cannot be trivially passed as plain addresses to other functions. Today we have seen that by using a trampoline it is possible to pass them to functions that do not allow passing a lexical scope. The price is having to copy a template, the trampoline, having to set it up with the proper values. We also have to flush caches in order to avoid executing wrong code. It is complicated but doable.
</p>
<p>
Having to flush the cache is undesirable (although not required in all architectures) and may cause a severe degradation of performance. Performance-critical pieces of code typically would not want to do this.
</p>
<p>
A serious concern, though, with the trampoline approach relates to the fact that we need an executable stack. A modern operating system, like Linux, can mark regions of memory to be readable, writable or executable. A region of memory that is not executable may contain instructions but if we branch to that region the processor will signal a fault, and the operating system will likely kill our process. Being able to disable execution of specific memory regions is done for security purposes. Most of the time one does not have to execute instructions that are found in stack or <code>.data</code> section. Only <code>.text</code> makes sense in these cases to be executable.
</p>
<p>
If you check what we did above, we actually copied some code (which was in <code>.text</code>) into the stack and then, <code>qsort</code> branched to the stack. This is because our programs allow an executable stack. Executable stacks are linked to common program vulnerability exploits like <a href="http://en.wikipedia.org/wiki/Buffer_overflow#Executable_space_protection">buffer overflows</a>.
</p>
<p>
As we’ve seen in this chapter and in the previous one, nested functions come with several downsides, so it is not surprising that several programming languages do not provide support for them.
</p>
<p>
That’s all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2015/01/09/arm-assembler-raspberry-pi-chapter-24/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2015/01/09/arm-assembler-raspberry-pi-chapter-24/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2015/01/09/arm-assembler-raspberry-pi-chapter-24/&amp;text=ARM+assembler+in+Raspberry+Pi+%E2%80%93+Chapter+24+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2015/01/09/arm-assembler-raspberry-pi-chapter-24/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>				<p class="tags"><span><a href="http://thinkingeek.com/tag/arm/" rel="tag">arm</a>, <a href="http://thinkingeek.com/tag/assembler/" rel="tag">assembler</a>, <a href="http://thinkingeek.com/tag/raspberry/" rel="tag">raspberry</a></span></p>		<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2015/01/03/dvd-with-bogus-permissions-ubuntu/" rel="prev">Read DVDs with bogus permissions in Ubuntu</a></span>
			<span class="next"><a href="http://thinkingeek.com/2015/02/26/array-array/" rel="next">When an array is not an array</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
			<h3>7 thoughts on “<span>ARM assembler in Raspberry Pi – Chapter 24</span>”</h3>
		<ul class="commentlist">
				<li class="comment even thread-even depth-1 parent" id="comment-768262">
				<div id="div-comment-768262" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-9e81362ff9654533accffc53762c9520-0">			<cite class="fn"><a href="http://devel.meruelo.com.es/blog/" rel="external nofollow" class="url">Fernando</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2015/01/09/arm-assembler-raspberry-pi-chapter-24/#comment-768262">
			January 18, 2015 at 8:45 pm</a>		</div>

		<p>As stated at the beginning of the post, using a global variable would be much easier in terms of coding, faster in terms of execution, will work in any context… </p>
<p>So, the question is: why would you rather use nested functions? I mean, I know this is just an example of a neat feature, but, would you use it in real-life applications? It looks like a lot of trouble for little-to-none benefit.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2015/01/09/arm-assembler-raspberry-pi-chapter-24/?replytocom=768262#respond" onclick="return addComment.moveForm( &quot;div-comment-768262&quot;, &quot;768262&quot;, &quot;respond&quot;, &quot;1768&quot; )" aria-label="Reply to Fernando">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2 parent" id="comment-776681">
				<div id="div-comment-776681" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-a779b8290b1ca104fdf84d8016fd010b-0" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2015/01/09/arm-assembler-raspberry-pi-chapter-24/#comment-776681">
			January 23, 2015 at 10:25 pm</a>		</div>

		<p>Hi Fernando,</p>
<p>I admit that the example in this chapter may not be very revealing on why a nested function may be interesting, so I understand you think that a global variable would be easier (and indeed it is!).</p>
<p>But it would not work in any context: if for some reason we have more than one <a href="http://en.wikipedia.org/wiki/Thread_%28computing%29" rel="nofollow">thread</a> and we want to sort more than one array concurrently, a global variable will not be appropiate because each thread will overwrite the value of the counter. If you want the aggregate sum of the comparisons this could suffice, but imagine you need each counter (for instance you want to compute the average of comparisons done by each thread). If rather than a global variable we use a local variable, each thread can call <code>qsort</code> without having to worry if another thread is going to overwrite it.</p>
<p>We have not considered threads in any chapter, so if you are not happy with the explanation above may be you can be convinced that in general you want the <em>state</em> of your program to be isolated as much as possible in order to unnecessarily expose it to other parts of the program. A global variable may be modified everywhere and so you have to believe that only the comparison function is going to modify it. By wrapping the state as a local variable, only that function can access that state and discard it as soon as it is not needed anymore (when leaving the function by shrinking the stack). You cannot do this with a global variable which will always take some amount of memory.</p>
<p>Of course, there is a tradeoff: accessing a global variable is easy and fast. Passing a pointer to a nested function that needs to access a local variable requires some acrobatics. Sometimes such acrobatics are not acceptable and nested functions get out of the way.</p>
<p>Usually, for languages where there are no nested functions, the usual <em>workaround</em> involves having functions with an extra parameter that plays a similar role to the lexical scope. Note that for qsort this is not possible but it would be possible for <a href="http://man7.org/linux/man-pages/man3/qsort.3.html" rel="nofollow">qsort_r</a>.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2015/01/09/arm-assembler-raspberry-pi-chapter-24/?replytocom=776681#respond" onclick="return addComment.moveForm( &quot;div-comment-776681&quot;, &quot;776681&quot;, &quot;respond&quot;, &quot;1768&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment even depth-3" id="comment-780560">
				<div id="div-comment-780560" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-9e81362ff9654533accffc53762c9520-1">			<cite class="fn"><a href="http://devel.meruelo.com.es/blog/" rel="external nofollow" class="url">Fernando</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2015/01/09/arm-assembler-raspberry-pi-chapter-24/#comment-780560">
			January 25, 2015 at 11:22 pm</a>		</div>

		<p>Reentrancy, what a tricky subject :P. </p>
<p>Your last reply was crystal clear: you are right that I was only thinking in single threaded programs, but still your point of minimizing the exposition of each function is good enough to think about nested functions.</p>
<p>Thanks!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2015/01/09/arm-assembler-raspberry-pi-chapter-24/?replytocom=780560#respond" onclick="return addComment.moveForm( &quot;div-comment-780560&quot;, &quot;780560&quot;, &quot;respond&quot;, &quot;1768&quot; )" aria-label="Reply to Fernando">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1 parent" id="comment-805695">
				<div id="div-comment-805695" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/8e02ba12a3b8f9d19306c56fbde0d455?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/8e02ba12a3b8f9d19306c56fbde0d455?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/8e02ba12a3b8f9d19306c56fbde0d455?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-8e02ba12a3b8f9d19306c56fbde0d455-0">			<cite class="fn"><a href="http://www.utdallas.edu/~pervin" rel="external nofollow" class="url">William Pervin</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2015/01/09/arm-assembler-raspberry-pi-chapter-24/#comment-805695">
			February 16, 2015 at 3:06 am</a>		</div>

		<p>I have written “A Programmer’s Guide to Assembler” based on the MIPS processor for my students. Since the Raspberry Pi looks like a wonderful way to introduce them to computer architecture for very little money, I have been thinking of writing a version for the ARM processor. Your tutorial is great. Do you have any plans to put it into the form of a textbook with exercises and additional examples? I like to work with you if you are interested.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2015/01/09/arm-assembler-raspberry-pi-chapter-24/?replytocom=805695#respond" onclick="return addComment.moveForm( &quot;div-comment-805695&quot;, &quot;805695&quot;, &quot;respond&quot;, &quot;1768&quot; )" aria-label="Reply to William Pervin">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2 parent" id="comment-805933">
				<div id="div-comment-805933" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-a779b8290b1ca104fdf84d8016fd010b-1" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2015/01/09/arm-assembler-raspberry-pi-chapter-24/#comment-805933">
			February 16, 2015 at 10:39 pm</a>		</div>

		<p>Hi William,</p>
<p>thanks for your kind words. Several friends of mine also suggested me to compile the posts in a book or something but, admittedly, I never thought to make a textbook of it. I am afraid that I would not be able to devote much time to it so a collaboration does not seem possible now. My posting frequency is rather irregular already.</p>
<p>That said, feel free to use (and even improve, I bet there is room for that) the examples in this tutorial.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2015/01/09/arm-assembler-raspberry-pi-chapter-24/?replytocom=805933#respond" onclick="return addComment.moveForm( &quot;div-comment-805933&quot;, &quot;805933&quot;, &quot;respond&quot;, &quot;1768&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment odd alt depth-3 parent" id="comment-805940">
				<div id="div-comment-805940" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/8e02ba12a3b8f9d19306c56fbde0d455?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/8e02ba12a3b8f9d19306c56fbde0d455?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/8e02ba12a3b8f9d19306c56fbde0d455?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-8e02ba12a3b8f9d19306c56fbde0d455-1">			<cite class="fn"><a href="http://www.utdallas.edu/~pervin" rel="external nofollow" class="url">William Pervin</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2015/01/09/arm-assembler-raspberry-pi-chapter-24/#comment-805940">
			February 16, 2015 at 11:40 pm</a>		</div>

		<p>Since I was planning on an ARM assembler book anyhow, I’ll start writing it. I found a very good emulator (qemu) that runs all your code just fine and I like the idea of my students being able to carry their Raspberry Pi with them in their laptop. Thanks for the reply. I will keep you informed of my progress.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2015/01/09/arm-assembler-raspberry-pi-chapter-24/?replytocom=805940#respond" onclick="return addComment.moveForm( &quot;div-comment-805940&quot;, &quot;805940&quot;, &quot;respond&quot;, &quot;1768&quot; )" aria-label="Reply to William Pervin">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment even depth-4" id="comment-818760">
				<div id="div-comment-818760" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/9a37c8deec849a39058b6cadc88e52ed?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/9a37c8deec849a39058b6cadc88e52ed?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-9a37c8deec849a39058b6cadc88e52ed-0" originals="32" src-orig="http://0.gravatar.com/avatar/9a37c8deec849a39058b6cadc88e52ed?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Clyde Wilson</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2015/01/09/arm-assembler-raspberry-pi-chapter-24/#comment-818760">
			March 12, 2015 at 5:02 pm</a>		</div>

		<p>Hi William, </p>
<p>Could you give us some insight on how you did this?  Are the laptops running Windows?</p>
<p>Thank you very much!</p>
<p>Clyde Wilson</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2015/01/09/arm-assembler-raspberry-pi-chapter-24/?replytocom=818760#respond" onclick="return addComment.moveForm( &quot;div-comment-818760&quot;, &quot;818760&quot;, &quot;respond&quot;, &quot;1768&quot; )" aria-label="Reply to Clyde Wilson">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		</ul>
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2015/01/09/arm-assembler-raspberry-pi-chapter-24/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="1768" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="ab88715200"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496667442007"></form>
			</div><!-- #respond -->
	</div>	</div>