# Functions (I)


<div class="main">
			<h1>ARM assembler in Raspberry Pi – Chapter 9 </h1>
		<p class="post-meta"><span class="date">February 2, 2013</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/#comments">18</a></span> </p>
		<p>
In previous chapters we learnt the foundations of ARM assembler: registers, some arithmetic operations, loads and stores and branches. Now it is time to put everything together and add another level of abstraction to our assembler skills: functions.
</p>
<p><span id="more-622"></span></p>
<h2>Why functions?</h2>
<p>
Functions are a way to reuse code. If we have some code that will be needed more than once, being able to reuse it is a Good Thing™. This way, we only have to ensure that the code being reused is correct. If we repeated the code whe should verify it is correct at every point. This clearly does not scale. Functions can also get parameters. This way not only we reuse code but we can use it in several ways, by passing different parameters. All this magic, though, comes at some price. A function must be a a <em>well-behaved</em> citizen.
</p>
<h2>Do’s and don’ts of a function</h2>
<p>
Assembler gives us a lot of power. But with a lot of power also comes a lot of responsibility. We can break lots of things in assembler, because we are at a very low level. An error and nasty things may happen. In order to make all functions behave in the same way, there are <em>conventions</em> in every environment that dictate how a function must behave. Since we are in a Raspberry Pi running Linux we will use the <abbr title="Procedure Call Standard for ARM Architecture®">AAPCS</abbr> (chances are that other ARM operating systems like RISCOS or Windows RT follow it). You may find this document in the ARM documentation website but I will try to summarize it in this chapter.
</p>
<h3>New special named registers</h3>
<p>
When discussing branches we learnt that <code>r15</code> was also called <code>pc</code> but we never called it <code>r15</code> anymore. Well, let’s rename from now <code>r14</code> as <code>lr</code> and <code>r13</code> as <code>sp</code>. <code>lr</code> stands for <em><strong>l</strong>ink <strong>r</strong>egister</em> and it is the address of the instruction following the instruction that <em>called us</em> (we will see later what is this). <code>sp</code> stands for <em><strong>s</strong>tack <strong>p</strong>ointer</em>. The <em>stack</em> is an area of memory owned only by the current function, the <code>sp</code> register stores the top address of that stack. For now, let’s put the stack aside. We will get it back in the next chapter.
</p>
<h3>Passing parameters</h3>
<p>
Functions can receive parameters. The first 4 parameters must be stored, sequentially, in the registers <code>r0</code>, <code>r1</code>, <code>r2</code> and <code>r3</code>. You may be wondering how to pass more than 4 parameters. We can, of course, but we need to use the stack, but we will discuss it in the next chapter. Until then, we will only pass up to 4 parameters.
</p>
<h3><q>Well behaved</q> functions</h3>
<p>
A function must adhere, at least, to the following rules if we want it to be AAPCS compliant.
</p>
<ul>
<li>A function should not make any assumption on the contents of the <code>cpsr</code>. So, at the entry of a function condition codes N, Z, C and V are unknown.
</li><li>A function can freely modify registers <code>r0</code>, <code>r1</code>, <code>r2</code> and <code>r3</code>.
</li><li>A function cannot assume anything on the contents of <code>r0</code>, <code>r1</code>, <code>r2</code> and <code>r3</code> unless they are playing the role of a parameter.
</li><li>A function can freely modify <code>lr</code> but the value upon entering the function will be needed when leaving the function (so such value must be kept somewhere).
</li><li>A function can modify all the remaining registers as long as their values are restored upon leaving the function. This includes <code>sp</code> and registers <code>r4</code> to <code>r11</code>.<br>
This means that, after calling a function, we have to assume that (only) registers <code>r0</code>, <code>r1</code>, <code>r2</code>, <code>r3</code> and <code>lr</code> have been overwritten.
</li></ul>
<h3>Calling a function</h3>
<p>
There are two ways to call a function. If the function is statically known (meaning we know exactly which function must be called) we will use <code>bl label</code>. That label must be a label defined in the <code>.text</code> section. This is called a direct (or immediate) call. We may do indirect calls by first storing the address of the function into a register and then using <code>blx Rsource1</code>.
</p>
<p>
In both cases the behaviour is as follows: the address of the function (immediately encoded in the <code>bl</code> or using the value of the register in <code>blx</code>) is stored in <code>pc</code>. The address of the instruction following the <code>bl</code> or <code>blx</code> instruction is kept in <code>lr</code>.
</p>
<h3>Leaving a function</h3>
<p>
A well behaved function, as stated above, will have to keep the initial value of <code>lr</code> somewhere. When leaving the function, we will retrieve that value and put it in some register (it can be <code>lr</code> again but this is not mandatory). Then we will <code>bx Rsource1</code> (we could use <code>blx</code> as well but the latter would update <code>lr</code> which is useless here).
</p>
<h3>Returning data from functions</h3>
<p>
Functions must use <code>r0</code> for data that fits in 32 bit (or less). This is, C types <code>char</code>, <code>short</code>, <code>int</code>, <code>long</code> (and <code>float</code> though we have not seen floating point yet) will be returned in <code>r0</code>. For basic types of 64 bit, like C types <code>long long</code> and <code>double</code>, they will be returned in <code>r1</code> and <code>r0</code>. Any other data is returned through the stack unless it is 32 bit or less, where it will be returned in <code>r0</code>.
</p>
<p>
In the examples in previous chapters we returned the error code of the program in <code>r0</code>. This now makes sense. C’s <code>main</code> returns an <code>int</code>, which is used as the value of the error code of our program.
</p>
<h2>Hello world</h2>
<p>
Usually this is the first program you write in any high level programming language. In our case we had to learn lots of things first. Anyway, here it is. A “Hello world” in ARM assembler.
</p>
<p>
(Note to experts: since we will not discuss the stack until the next chapter, this code may look very dumb to you)
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> <span style="color: #339933;">--</span> hello01<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
greeting<span style="color: #339933;">:</span>
 <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"Hello world"</span>
&nbsp;
<span style="color: #339933;">.</span>balign <span style="color: #ff0000;">4</span>
return<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">0</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">global</span> main
main<span style="color: #339933;">:</span>
    ldr r1<span style="color: #339933;">,</span> address_of_return     <span style="color: #339933;">/*</span>   r1 ← &amp;address_of_return <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> lr<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #009900; font-weight: bold;">]</span>                  <span style="color: #339933;">/*</span>   <span style="color: #339933;">*</span>r1 ← lr <span style="color: #339933;">*/</span>
&nbsp;
    ldr r0<span style="color: #339933;">,</span> address_of_greeting   <span style="color: #339933;">/*</span> r0 ← &amp;address_of_greeting <span style="color: #339933;">*/</span>
                                  <span style="color: #339933;">/*</span> First parameter of puts <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #46aa03; font-weight: bold;">bl</span> puts                       <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Call</span> to puts <span style="color: #339933;">*/</span>
                                  <span style="color: #339933;">/*</span> lr ← address of next instruction <span style="color: #339933;">*/</span>
&nbsp;
    ldr r1<span style="color: #339933;">,</span> address_of_return     <span style="color: #339933;">/*</span> r1 ← &amp;address_of_return <span style="color: #339933;">*/</span>
    ldr lr<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #009900; font-weight: bold;">]</span>                  <span style="color: #339933;">/*</span> lr ← <span style="color: #339933;">*</span>r1 <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr                         <span style="color: #339933;">/*</span> return from main <span style="color: #339933;">*/</span>
address_of_greeting<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> greeting
address_of_return<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> return
&nbsp;
<span style="color: #339933;">/*</span> External <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">global</span> puts</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* -- hello01.s */
.data

greeting:
 .asciz "Hello world"

.balign 4
return: .word 0

.text

.global main
main:
    ldr r1, address_of_return     /*   r1 ← &amp;address_of_return */
    str lr, [r1]                  /*   *r1 ← lr */

    ldr r0, address_of_greeting   /* r0 ← &amp;address_of_greeting */
                                  /* First parameter of puts */

    bl puts                       /* Call to puts */
                                  /* lr ← address of next instruction */

    ldr r1, address_of_return     /* r1 ← &amp;address_of_return */
    ldr lr, [r1]                  /* lr ← *r1 */
    bx lr                         /* return from main */
address_of_greeting: .word greeting
address_of_return: .word return

/* External */
.global puts</p></div>

<p>
We are going to call <code>puts</code> function. This function is defined in the C library and has the following prototype <code>int puts(const char*)</code>. It receives, as a first parameter, the address of a C-string (this is, a sequence of bytes where no byte but the last is zero). When executed it outputs that string to <code>stdout</code> (so it should appear by default to our terminal). Finally it returns the number of bytes written.
</p>
<p>
We start by defining in the <code>.data</code> the label <code>greeting</code> in lines 4 and 5. This label will contain the address of our greeting message. GNU as provides a convenient <code>.asciz</code> directive for that purpose. This directive emits as bytes as needed to represent the string plus the final zero byte. We could have used another directive <code>.ascii</code> as long as we explicitly added the final zero byte.
</p>
<p>
After the bytes of the greeting message, we make sure the next label will be 4 bytes aligned and we define a <code>return</code> label in line 8. In that label we will keep the value of <code>lr</code> that we have in <code>main</code>. As stated above, this is a requirement for a well behaved function: be able to get the original value of <code>lr</code> upon entering. So we make some room for it.
</p>
<p>
The first two instructions, lines 14 an 15, of our main function keep the value of <code>lr</code> in that <code>return</code> variable defined above. Then in line 17 we prepare the arguments for the call to <code>puts</code>. We load the address of the greeting message into <code>r0</code> register. This register will hold the first (the only one actually) parameter of <code>puts</code>. Then in line 20 we call the function. Recall that <code>bl</code> will set in <code>lr</code> the address of the instruction following it (this is the instruction in line 23). This is the reason why we copied the value of <code>lr</code> in a variable in the beginning of the <code>main</code> function, because it was going to be overwritten by <code>bl</code>.
</p>
<p>
Ok, <code>puts</code> runs and the message is printed on the <code>stdout</code>. Time to get the initial value of <code>lr</code> so we can return successfully from main. Then we return.
</p>
<p>
Is our <code>main</code> function well behaved? Yes, it keeps and gets back <code>lr</code> to leave. It only modifies <code>r0</code> and <code>r1</code>. We can assume that <code>puts</code> is well behaved as well, so everything should work fine. Plus the bonus of seeing how many bytes have been written to the output.</p>
<p>
</p><div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="bash" style="font-family:monospace;">$ .<span style="color: #000000; font-weight: bold;">/</span>hello01 
Hello world
$ <span style="color: #7a0874; font-weight: bold;">echo</span> <span style="color: #007800;">$?</span>
<span style="color: #000000;">12</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ ./hello01 
Hello world
$ echo $?
12</p></div>

<p>
Note that “Hello world” is just 11 bytes (the final zero is not counted as it just plays the role of a finishing byte) but the program returns 12. This is because <code>puts</code> always adds a newline byte, which accounts for that extra byte.
</p>
<h2>Real interaction!</h2>
<p>
Now we have the power of calling functions we can glue them together. Let’s call printf and scanf to read a number and then print it back to the standard output.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> <span style="color: #339933;">--</span> printf01<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
<span style="color: #339933;">/*</span> First message <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span>balign <span style="color: #ff0000;">4</span>
message1<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"Hey, type a number: "</span>
&nbsp;
<span style="color: #339933;">/*</span> Second message <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span>balign <span style="color: #ff0000;">4</span>
message2<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"I read the number %d\n"</span>
&nbsp;
<span style="color: #339933;">/*</span> Format pattern for scanf <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span>balign <span style="color: #ff0000;">4</span>
scan_pattern <span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"%d"</span>
&nbsp;
<span style="color: #339933;">/*</span> Where scanf will store the number read <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span>balign <span style="color: #ff0000;">4</span>
number_read<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">0</span>
&nbsp;
<span style="color: #339933;">.</span>balign <span style="color: #ff0000;">4</span>
return<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">0</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">global</span> main
main<span style="color: #339933;">:</span>
    ldr r1<span style="color: #339933;">,</span> address_of_return        <span style="color: #339933;">/*</span> r1 ← &amp;address_of_return <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> lr<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #009900; font-weight: bold;">]</span>                     <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span>r1 ← lr <span style="color: #339933;">*/</span>
&nbsp;
    ldr r0<span style="color: #339933;">,</span> address_of_message1      <span style="color: #339933;">/*</span> r0 ← &amp;message1 <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> printf                        <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> to printf <span style="color: #339933;">*/</span>
&nbsp;
    ldr r0<span style="color: #339933;">,</span> address_of_scan_pattern  <span style="color: #339933;">/*</span> r0 ← &amp;scan_pattern <span style="color: #339933;">*/</span>
    ldr r1<span style="color: #339933;">,</span> address_of_number_read   <span style="color: #339933;">/*</span> r1 ← &amp;number_read <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> scanf                         <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> to scanf <span style="color: #339933;">*/</span>
&nbsp;
    ldr r0<span style="color: #339933;">,</span> address_of_message2      <span style="color: #339933;">/*</span> r0 ← &amp;message2 <span style="color: #339933;">*/</span>
    ldr r1<span style="color: #339933;">,</span> address_of_number_read   <span style="color: #339933;">/*</span> r1 ← &amp;number_read <span style="color: #339933;">*/</span>
    ldr r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #009900; font-weight: bold;">]</span>                     <span style="color: #339933;">/*</span> r1 ← <span style="color: #339933;">*</span>r1 <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> printf                        <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> to printf <span style="color: #339933;">*/</span>
&nbsp;
    ldr r0<span style="color: #339933;">,</span> address_of_number_read   <span style="color: #339933;">/*</span> r0 ← &amp;number_read <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>                     <span style="color: #339933;">/*</span> r0 ← <span style="color: #339933;">*</span>r0 <span style="color: #339933;">*/</span>
&nbsp;
    ldr lr<span style="color: #339933;">,</span> address_of_return        <span style="color: #339933;">/*</span> lr ← &amp;address_of_return <span style="color: #339933;">*/</span>
    ldr lr<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>lr<span style="color: #009900; font-weight: bold;">]</span>                     <span style="color: #339933;">/*</span> lr ← <span style="color: #339933;">*</span>lr <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr                            <span style="color: #339933;">/*</span> return from main using lr <span style="color: #339933;">*/</span>
address_of_message1 <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> message1
address_of_message2 <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> message2
address_of_scan_pattern <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> scan_pattern
address_of_number_read <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> number_read
address_of_return <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> return
&nbsp;
<span style="color: #339933;">/*</span> External <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">global</span> printf
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">global</span> scanf</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* -- printf01.s */
.data

/* First message */
.balign 4
message1: .asciz "Hey, type a number: "

/* Second message */
.balign 4
message2: .asciz "I read the number %d\n"

/* Format pattern for scanf */
.balign 4
scan_pattern : .asciz "%d"

/* Where scanf will store the number read */
.balign 4
number_read: .word 0

.balign 4
return: .word 0

.text

.global main
main:
    ldr r1, address_of_return        /* r1 ← &amp;address_of_return */
    str lr, [r1]                     /* *r1 ← lr */

    ldr r0, address_of_message1      /* r0 ← &amp;message1 */
    bl printf                        /* call to printf */

    ldr r0, address_of_scan_pattern  /* r0 ← &amp;scan_pattern */
    ldr r1, address_of_number_read   /* r1 ← &amp;number_read */
    bl scanf                         /* call to scanf */

    ldr r0, address_of_message2      /* r0 ← &amp;message2 */
    ldr r1, address_of_number_read   /* r1 ← &amp;number_read */
    ldr r1, [r1]                     /* r1 ← *r1 */
    bl printf                        /* call to printf */

    ldr r0, address_of_number_read   /* r0 ← &amp;number_read */
    ldr r0, [r0]                     /* r0 ← *r0 */

    ldr lr, address_of_return        /* lr ← &amp;address_of_return */
    ldr lr, [lr]                     /* lr ← *lr */
    bx lr                            /* return from main using lr */
address_of_message1 : .word message1
address_of_message2 : .word message2
address_of_scan_pattern : .word scan_pattern
address_of_number_read : .word number_read
address_of_return : .word return

/* External */
.global printf
.global scanf</p></div>

<p>
In this example we will ask the user to type a number and then we will print it back. We also return the number in the error code, so we can check twice if everything goes as expected. For the error code check, make sure your number is lower than 255 (otherwise the error code will show only its lower 8 bits).
</p>
<pre escaped="1">$ ./printf01 
Hey, type a number: <span style="color: blue;">123↴</span>
I read the number 123
$ ./printf01 ; echo $?
Hey, type a number: <span style="color: blue;">124↴</span>
I read the number 124
124
</pre>
<h2>Our first function</h2>
<p>
Let’s define our first function. Lets extend the previous example but multiply the number by 5.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>23
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">.</span>balign <span style="color: #ff0000;">4</span>
return2<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">0</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
<span style="color: #339933;">/*</span>
mult_by_5 function
<span style="color: #339933;">*/</span>
mult_by_5<span style="color: #339933;">:</span> 
    ldr r1<span style="color: #339933;">,</span> address_of_return2       <span style="color: #339933;">/*</span> r1 ← &amp;address_of_return <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> lr<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #009900; font-weight: bold;">]</span>                     <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span>r1 ← lr <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">add</span> r0<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span>           <span style="color: #339933;">/*</span> r0 ← r0 <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span>r0 <span style="color: #339933;">*/</span>
&nbsp;
    ldr lr<span style="color: #339933;">,</span> address_of_return2       <span style="color: #339933;">/*</span> lr ← &amp;address_of_return <span style="color: #339933;">*/</span>
    ldr lr<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>lr<span style="color: #009900; font-weight: bold;">]</span>                     <span style="color: #339933;">/*</span> lr ← <span style="color: #339933;">*</span>lr <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr                            <span style="color: #339933;">/*</span> return from main using lr <span style="color: #339933;">*/</span>
address_of_return2 <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> return2</pre></td></tr></tbody></table><p class="theCode" style="display:none;">.balign 4
return2: .word 0

.text

/*
mult_by_5 function
*/
mult_by_5: 
    ldr r1, address_of_return2       /* r1 ← &amp;address_of_return */
    str lr, [r1]                     /* *r1 ← lr */

    add r0, r0, r0, LSL #2           /* r0 ← r0 + 4*r0 */

    ldr lr, address_of_return2       /* lr ← &amp;address_of_return */
    ldr lr, [lr]                     /* lr ← *lr */
    bx lr                            /* return from main using lr */
address_of_return2 : .word return2</p></div>

<p>
This function will need another “<code>return</code>” variable like the one <code>main</code> uses. But this is for the sake of the example. Actually this function does not call another function. When this happens it does not need to keep <code>lr</code> as no <code>bl</code> or <code>blx</code> instruction is going to modify it. If the function wanted to use <code>lr</code> as the the <code>r14</code> general purpose register, the process of keeping the value would still be mandatory.
</p>
<p>
As you can see, once the function has computed the value, it is enough keeping it in <code>r0</code>. In this case it was pretty easy and a single instruction was enough.
</p>
<p>
The whole example follows.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> <span style="color: #339933;">--</span> printf02<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
<span style="color: #339933;">/*</span> First message <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span>balign <span style="color: #ff0000;">4</span>
message1<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"Hey, type a number: "</span>
&nbsp;
<span style="color: #339933;">/*</span> Second message <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span>balign <span style="color: #ff0000;">4</span>
message2<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"%d times 5 is %d\n"</span>
&nbsp;
<span style="color: #339933;">/*</span> Format pattern for scanf <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span>balign <span style="color: #ff0000;">4</span>
scan_pattern <span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"%d"</span>
&nbsp;
<span style="color: #339933;">/*</span> Where scanf will store the number read <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span>balign <span style="color: #ff0000;">4</span>
number_read<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">0</span>
&nbsp;
<span style="color: #339933;">.</span>balign <span style="color: #ff0000;">4</span>
return<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">0</span>
&nbsp;
<span style="color: #339933;">.</span>balign <span style="color: #ff0000;">4</span>
return2<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">0</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
<span style="color: #339933;">/*</span>
mult_by_5 function
<span style="color: #339933;">*/</span>
mult_by_5<span style="color: #339933;">:</span> 
    ldr r1<span style="color: #339933;">,</span> address_of_return2       <span style="color: #339933;">/*</span> r1 ← &amp;address_of_return <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> lr<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #009900; font-weight: bold;">]</span>                     <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span>r1 ← lr <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">add</span> r0<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span>           <span style="color: #339933;">/*</span> r0 ← r0 <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span>r0 <span style="color: #339933;">*/</span>
&nbsp;
    ldr lr<span style="color: #339933;">,</span> address_of_return2       <span style="color: #339933;">/*</span> lr ← &amp;address_of_return <span style="color: #339933;">*/</span>
    ldr lr<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>lr<span style="color: #009900; font-weight: bold;">]</span>                     <span style="color: #339933;">/*</span> lr ← <span style="color: #339933;">*</span>lr <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr                            <span style="color: #339933;">/*</span> return from main using lr <span style="color: #339933;">*/</span>
address_of_return2 <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> return2
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">global</span> main
main<span style="color: #339933;">:</span>
    ldr r1<span style="color: #339933;">,</span> address_of_return        <span style="color: #339933;">/*</span> r1 ← &amp;address_of_return <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> lr<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #009900; font-weight: bold;">]</span>                     <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span>r1 ← lr <span style="color: #339933;">*/</span>
&nbsp;
    ldr r0<span style="color: #339933;">,</span> address_of_message1      <span style="color: #339933;">/*</span> r0 ← &amp;message1 <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> printf                        <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> to printf <span style="color: #339933;">*/</span>
&nbsp;
    ldr r0<span style="color: #339933;">,</span> address_of_scan_pattern  <span style="color: #339933;">/*</span> r0 ← &amp;scan_pattern <span style="color: #339933;">*/</span>
    ldr r1<span style="color: #339933;">,</span> address_of_number_read   <span style="color: #339933;">/*</span> r1 ← &amp;number_read <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> scanf                         <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> to scanf <span style="color: #339933;">*/</span>
&nbsp;
    ldr r0<span style="color: #339933;">,</span> address_of_number_read   <span style="color: #339933;">/*</span> r0 ← &amp;number_read <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>                     <span style="color: #339933;">/*</span> r0 ← <span style="color: #339933;">*</span>r0 <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> mult_by_5
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r2<span style="color: #339933;">,</span> r0                       <span style="color: #339933;">/*</span> r2 ← r0 <span style="color: #339933;">*/</span>
    ldr r1<span style="color: #339933;">,</span> address_of_number_read   <span style="color: #339933;">/*</span> r1 ← &amp;number_read <span style="color: #339933;">*/</span>
    ldr r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #009900; font-weight: bold;">]</span>                     <span style="color: #339933;">/*</span> r1 ← <span style="color: #339933;">*</span>r1 <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> address_of_message2      <span style="color: #339933;">/*</span> r0 ← &amp;message2 <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> printf                        <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> to printf <span style="color: #339933;">*/</span>
&nbsp;
    ldr lr<span style="color: #339933;">,</span> address_of_return        <span style="color: #339933;">/*</span> lr ← &amp;address_of_return <span style="color: #339933;">*/</span>
    ldr lr<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>lr<span style="color: #009900; font-weight: bold;">]</span>                     <span style="color: #339933;">/*</span> lr ← <span style="color: #339933;">*</span>lr <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr                            <span style="color: #339933;">/*</span> return from main using lr <span style="color: #339933;">*/</span>
address_of_message1 <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> message1
address_of_message2 <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> message2
address_of_scan_pattern <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> scan_pattern
address_of_number_read <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> number_read
address_of_return <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> return
&nbsp;
<span style="color: #339933;">/*</span> External <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">global</span> printf
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">global</span> scanf</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* -- printf02.s */
.data

/* First message */
.balign 4
message1: .asciz "Hey, type a number: "

/* Second message */
.balign 4
message2: .asciz "%d times 5 is %d\n"

/* Format pattern for scanf */
.balign 4
scan_pattern : .asciz "%d"

/* Where scanf will store the number read */
.balign 4
number_read: .word 0

.balign 4
return: .word 0

.balign 4
return2: .word 0

.text

/*
mult_by_5 function
*/
mult_by_5: 
    ldr r1, address_of_return2       /* r1 ← &amp;address_of_return */
    str lr, [r1]                     /* *r1 ← lr */

    add r0, r0, r0, LSL #2           /* r0 ← r0 + 4*r0 */

    ldr lr, address_of_return2       /* lr ← &amp;address_of_return */
    ldr lr, [lr]                     /* lr ← *lr */
    bx lr                            /* return from main using lr */
address_of_return2 : .word return2

.global main
main:
    ldr r1, address_of_return        /* r1 ← &amp;address_of_return */
    str lr, [r1]                     /* *r1 ← lr */

    ldr r0, address_of_message1      /* r0 ← &amp;message1 */
    bl printf                        /* call to printf */

    ldr r0, address_of_scan_pattern  /* r0 ← &amp;scan_pattern */
    ldr r1, address_of_number_read   /* r1 ← &amp;number_read */
    bl scanf                         /* call to scanf */

    ldr r0, address_of_number_read   /* r0 ← &amp;number_read */
    ldr r0, [r0]                     /* r0 ← *r0 */
    bl mult_by_5

    mov r2, r0                       /* r2 ← r0 */
    ldr r1, address_of_number_read   /* r1 ← &amp;number_read */
    ldr r1, [r1]                     /* r1 ← *r1 */
    ldr r0, address_of_message2      /* r0 ← &amp;message2 */
    bl printf                        /* call to printf */

    ldr lr, address_of_return        /* lr ← &amp;address_of_return */
    ldr lr, [lr]                     /* lr ← *lr */
    bx lr                            /* return from main using lr */
address_of_message1 : .word message1
address_of_message2 : .word message2
address_of_scan_pattern : .word scan_pattern
address_of_number_read : .word number_read
address_of_return : .word return

/* External */
.global printf
.global scanf</p></div>

<p>
I want you to notice lines 58 to 62. There we prepare the call to <code>printf</code> which receives three parameters: the format and the two integers referenced in the format. We want the first integer be the number entered by the user. The second one will be that same number multiplied by 5. After the call to <code>mult_by_5</code>, <code>r0</code> contains the number entered by the user multiplied by 5. We want it to be the third parameter so we move it to <code>r2</code>. Then we load the value of the number entered by the user into <code>r1</code>. Finally we load in <code>r0</code> the address to the format message of <code>printf</code>. Note that here the order of preparing the arguments of a call is nonrelevant as long as the values are correct at the point of the call. We use the fact that we will have to overwrite <code>r0</code>, so for convenience we first copy <code>r0</code> to <code>r2</code>.
</p>
<pre escaped="1">$ ./printf02
Hey, type a number: <span style="color: blue;">1234↴</span>
1234 times 5 is 6170
</pre>
<p>
That’s all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/&amp;text=ARM+assembler+in+Raspberry+Pi+%E2%80%93+Chapter+9+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>				<p class="tags"><span><a href="http://thinkingeek.com/tag/arm/" rel="tag">arm</a>, <a href="http://thinkingeek.com/tag/assembler/" rel="tag">assembler</a>, <a href="http://thinkingeek.com/tag/function/" rel="tag">function</a>, <a href="http://thinkingeek.com/tag/function-call/" rel="tag">function call</a>, <a href="http://thinkingeek.com/tag/functions/" rel="tag">functions</a>, <a href="http://thinkingeek.com/tag/pi/" rel="tag">pi</a>, <a href="http://thinkingeek.com/tag/raspberry/" rel="tag">raspberry</a></span></p>		<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/" rel="prev">ARM assembler in Raspberry Pi – Chapter 8</a></span>
			<span class="next"><a href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/" rel="next">ARM assembler in Raspberry Pi – Chapter 10</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
			<h3>18 thoughts on “<span>ARM assembler in Raspberry Pi – Chapter 9</span>”</h3>
		<ul class="commentlist">
				<li class="pingback even thread-even depth-1" id="comment-1142">
				<div id="div-comment-1142" class="comment-body">
				<div class="comment-author vcard">
						<cite class="fn"><a href="http://www.raspberrypithai.com/2013/04/07/%e0%b8%ab%e0%b8%a3%e0%b8%a1-%e0%b8%94%e0%b9%89%e0%b8%a7%e0%b8%a2%e0%b8%a0%e0%b8%b2%e0%b8%a9%e0%b8%b2-assembly-%e0%b8%9a%e0%b8%99-raspberry-pi/" rel="external nofollow" class="url">ห.ร.ม. ด้วยภาษา Assembly บน Raspberry Pi | Raspberry Pi Thailand</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/#comment-1142">
			April 7, 2013 at 12:30 pm</a>		</div>

		<p>[…] และอีกบทความครับ สอนละเอียดเลย <a href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9" rel="nofollow">http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9</a> […]</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/?replytocom=1142#respond" onclick="return addComment.moveForm( &quot;div-comment-1142&quot;, &quot;1142&quot;, &quot;respond&quot;, &quot;622&quot; )" aria-label="Reply to ห.ร.ม. ด้วยภาษา Assembly บน Raspberry Pi | Raspberry Pi Thailand">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1 parent" id="comment-1277">
				<div id="div-comment-1277" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/5a0796d3f5a35d57f483d7d13d884007?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/5a0796d3f5a35d57f483d7d13d884007?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/5a0796d3f5a35d57f483d7d13d884007?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-5a0796d3f5a35d57f483d7d13d884007-0">			<cite class="fn">Pablo</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/#comment-1277">
			May 9, 2013 at 10:07 pm</a>		</div>

		<p>Great post! Is there any way to know get the length of an array that was previously created, or to add elements and extend the array? </p>
<p>Thanks for all the info.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/?replytocom=1277#respond" onclick="return addComment.moveForm( &quot;div-comment-1277&quot;, &quot;1277&quot;, &quot;respond&quot;, &quot;622&quot; )" aria-label="Reply to Pablo">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2 parent" id="comment-1278">
				<div id="div-comment-1278" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-0">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/#comment-1278">
			May 9, 2013 at 10:28 pm</a>		</div>

		<p>Well, it depends on what you understand by an “array” in this context and what you mean by “previously created”.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/?replytocom=1278#respond" onclick="return addComment.moveForm( &quot;div-comment-1278&quot;, &quot;1278&quot;, &quot;respond&quot;, &quot;622&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment odd alt depth-3 parent" id="comment-1279">
				<div id="div-comment-1279" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/5a0796d3f5a35d57f483d7d13d884007?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/5a0796d3f5a35d57f483d7d13d884007?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/5a0796d3f5a35d57f483d7d13d884007?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-5a0796d3f5a35d57f483d7d13d884007-1">			<cite class="fn">Pablo</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/#comment-1279">
			May 9, 2013 at 11:35 pm</a>		</div>

		<p>Let´s suppose I have [1,2,3]. On any high level language, there´s a command called “length” or anything similar that returns the number of elements on the array, which will be 3 on the structure I described previously. </p>
<p>Is it possible to make the same thing on assembly?</p>
<p>Thanks for the response <img draggable="false" class="emoji" alt="🙂" src="https://s.w.org/images/core/emoji/2.2.1/svg/1f642.svg" scale="0"></p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/?replytocom=1279#respond" onclick="return addComment.moveForm( &quot;div-comment-1279&quot;, &quot;1279&quot;, &quot;respond&quot;, &quot;622&quot; )" aria-label="Reply to Pablo">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-4" id="comment-1283">
				<div id="div-comment-1283" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-1">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/#comment-1283">
			May 12, 2013 at 9:06 am</a>		</div>

		<p>Yes, it must be possible, otherwise it would not be doable at all <img draggable="false" class="emoji" alt="😉" src="https://s.w.org/images/core/emoji/2.2.1/svg/1f609.svg" scale="0"></p>
<p>If the size of your array is statically defined (this is, it is known when you assemble) the code, you can use GNU assembler features which may be useful. For instance, the following is a simple case to compute the size of elements of an array of 4-byte integers.<br>
<code><br>
.data</code></p><code>
<p>array: .word 0x1, 0x2, 0x3, 0x4<br>
end_of_array :</p>
<p>.globl main<br>
.text</p>
</code><p><code>main:<br>
   /* This is r1 ← 4 */<br>
   mov r1, #(end_of_array - array) / 4<br>
   ...<br>
</code></p>
<p>This works because we substract end_of_array (the address past the last element of the array) to the array (the first element of the array). This gives us a value in bytes, so we divide it by 4 (each integer is 4 bytes). Note that this happens at assemble-time (or compilation time). So there is no real code emitted here: the assembler just computes a constant value and uses in-place of the whole expression. If it is not able to compute a constant value, this is an error.</p>
<p>I suggest you to read the <a href="http://sourceware.org/binutils/docs-2.23.1/as/index.html" rel="nofollow">GNU as</a> manual, in special the section 5 about expressions.</p>
<p>If your array size is dynamic, then everyting is more complicated. Your array will be, in a straightforward approach, a pair of numbers: the address of the array itself and the number of elements. The “length” operation is just reading the latter. How the first would be used is beyond the scope of this tutorial as it may involve either upper bounded memory (your array may be up to N items) or dynamic memory (malloc, free, etc).</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/?replytocom=1283#respond" onclick="return addComment.moveForm( &quot;div-comment-1283&quot;, &quot;1283&quot;, &quot;respond&quot;, &quot;622&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-even depth-1" id="comment-1436">
				<div id="div-comment-1436" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a16ae1e69be669d6f2c5fa0972e39c8a?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a16ae1e69be669d6f2c5fa0972e39c8a?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a16ae1e69be669d6f2c5fa0972e39c8a?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a16ae1e69be669d6f2c5fa0972e39c8a-0">			<cite class="fn"><a href="http://digilander.libero.it/zantaz/" rel="external nofollow" class="url">Mariani Antonio Mario</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/#comment-1436">
			June 29, 2013 at 5:34 pm</a>		</div>

		<p>very nice code …</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/?replytocom=1436#respond" onclick="return addComment.moveForm( &quot;div-comment-1436&quot;, &quot;1436&quot;, &quot;respond&quot;, &quot;622&quot; )" aria-label="Reply to Mariani Antonio Mario">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-159443">
				<div id="div-comment-159443" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/c5128194ae0fa12dc2c34b146c3d11a5?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/c5128194ae0fa12dc2c34b146c3d11a5?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/c5128194ae0fa12dc2c34b146c3d11a5?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-c5128194ae0fa12dc2c34b146c3d11a5-0">			<cite class="fn"><a href="http://antoniovillena.es" rel="external nofollow" class="url">Antonio Villena</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/#comment-159443">
			June 18, 2014 at 12:30 am</a>		</div>

		<p>Another errata, “cpsr” instead “cspr”</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/?replytocom=159443#respond" onclick="return addComment.moveForm( &quot;div-comment-159443&quot;, &quot;159443&quot;, &quot;respond&quot;, &quot;622&quot; )" aria-label="Reply to Antonio Villena">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2 parent" id="comment-160752">
				<div id="div-comment-160752" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-2">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/#comment-160752">
			June 19, 2014 at 9:11 pm</a>		</div>

		<p>Fixed. Thanks!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/?replytocom=160752#respond" onclick="return addComment.moveForm( &quot;div-comment-160752&quot;, &quot;160752&quot;, &quot;respond&quot;, &quot;622&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment even depth-3 parent" id="comment-162821">
				<div id="div-comment-162821" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/c5128194ae0fa12dc2c34b146c3d11a5?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/c5128194ae0fa12dc2c34b146c3d11a5?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/c5128194ae0fa12dc2c34b146c3d11a5?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-c5128194ae0fa12dc2c34b146c3d11a5-1">			<cite class="fn"><a href="http://antoniovillena.es" rel="external nofollow" class="url">Antonio Villena</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/#comment-162821">
			June 22, 2014 at 2:41 pm</a>		</div>

		<p>bad fixed, cprs != cpsr</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/?replytocom=162821#respond" onclick="return addComment.moveForm( &quot;div-comment-162821&quot;, &quot;162821&quot;, &quot;respond&quot;, &quot;622&quot; )" aria-label="Reply to Antonio Villena">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-4" id="comment-162839">
				<div id="div-comment-162839" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-3">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/#comment-162839">
			June 22, 2014 at 3:02 pm</a>		</div>

		<p>Oops! I think now it is OK.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/?replytocom=162839#respond" onclick="return addComment.moveForm( &quot;div-comment-162839&quot;, &quot;162839&quot;, &quot;respond&quot;, &quot;622&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="pingback even thread-even depth-1" id="comment-168554">
				<div id="div-comment-168554" class="comment-body">
				<div class="comment-author vcard">
						<cite class="fn"><a href="http://www.raspberrypthai.com/2013/04/07/%e0%b8%ab%e0%b8%a3%e0%b8%a1-%e0%b8%94%e0%b9%89%e0%b8%a7%e0%b8%a2%e0%b8%a0%e0%b8%b2%e0%b8%a9%e0%b8%b2-assembly-%e0%b8%9a%e0%b8%99-raspberry-pi/" rel="external nofollow" class="url">ห.ร.ม. ด้วยภาษา Assembly บน Raspberry Pi | Unofficial of Raspberry Pi Fan in Thailand</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/#comment-168554">
			June 29, 2014 at 4:13 pm</a>		</div>

		<p>[…] และอีกบทความครับ สอนละเอียดเลย <a href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9" rel="nofollow">http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9</a> […]</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/?replytocom=168554#respond" onclick="return addComment.moveForm( &quot;div-comment-168554&quot;, &quot;168554&quot;, &quot;respond&quot;, &quot;622&quot; )" aria-label="Reply to ห.ร.ม. ด้วยภาษา Assembly บน Raspberry Pi | Unofficial of Raspberry Pi Fan in Thailand">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1 parent" id="comment-960338">
				<div id="div-comment-960338" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/5e10b0e6a53fba8753a9199eade62662?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/5e10b0e6a53fba8753a9199eade62662?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/5e10b0e6a53fba8753a9199eade62662?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-5e10b0e6a53fba8753a9199eade62662-0">			<cite class="fn">Stellan</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/#comment-960338">
			February 9, 2016 at 2:51 pm</a>		</div>

		<p>Hi is there a way to create a function that is not included in the assembled code if it’s not called somewhere?</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/?replytocom=960338#respond" onclick="return addComment.moveForm( &quot;div-comment-960338&quot;, &quot;960338&quot;, &quot;respond&quot;, &quot;622&quot; )" aria-label="Reply to Stellan">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2" id="comment-960423">
				<div id="div-comment-960423" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-4">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/#comment-960423">
			February 10, 2016 at 9:25 pm</a>		</div>

		<p>
Hi Stellan,
</p>
<p>
yes, there is a technique but requires two things: a) putting a function inside its own “text.nnn” section, and b) telling the linker to remove unused sections. A very small example follows.
</p>
<pre>/* test.s */
.text

.section .text.foo
foo:
    bx lr

.section .text.main
.globl main
main:
    mov r0, #0
    bx lr
</pre>
<p>Compile like shown below.</p>
<pre>$ gcc -c test.s
$ gcc -o test test.o -Wl,--gc-sections,--print-gc-sections
/usr/bin/ld: Removing unused section '.rodata.cst4' in file '/usr/lib/gcc/arm-linux-gnueabihf/4.6/../../../arm-linux-gnueabihf/crt1.o'
/usr/bin/ld: Removing unused section '.data' in file '/usr/lib/gcc/arm-linux-gnueabihf/4.6/../../../arm-linux-gnueabihf/crt1.o'
/usr/bin/ld: Removing unused section '.data' in file '/usr/lib/gcc/arm-linux-gnueabihf/4.6/crtbegin.o'
/usr/bin/ld: Removing unused section '.text.foo' in file 'test.o'
</pre>
<p>
<code>-Wl</code> is used to tell gcc to pass-through a comma-separated list of flags (without further processing) to the linker. The linker flag <code>--gc-sections</code> tells the linker to garbage collect unused sections. The linker flag <code>--print-gc-sections</code> just reports the list of sections removed.  Note that the C library has some sections that the linker considers unused, but note the last line <code>.text.foo</code> which corresponds to our unused <code>foo</code> function.
</p>
<p>
Kind regards</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/?replytocom=960423#respond" onclick="return addComment.moveForm( &quot;div-comment-960423&quot;, &quot;960423&quot;, &quot;respond&quot;, &quot;622&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-even depth-1 parent" id="comment-960728">
				<div id="div-comment-960728" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/28e0ab4edf9bc16556790520dc3d5354?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/28e0ab4edf9bc16556790520dc3d5354?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/28e0ab4edf9bc16556790520dc3d5354?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-28e0ab4edf9bc16556790520dc3d5354-0">			<cite class="fn">Mayank</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/#comment-960728">
			February 15, 2016 at 8:51 pm</a>		</div>

		<p>If I use Loop like below between “bl printf”, I am getting some invalid output; I know it’s something because of bad handling of lr but I could not resolve it. Please do let me know the proper handling of lr while calling the functions. Also do lr gets updated by using of “beq ..” type instruction?</p>
<p>.data<br>
.balign 4<br>
print_Statement:<br>
 .asciz “Variable Print: %d\n”</p>
<p>.balign 4<br>
myvar:<br>
 .word 2<br>
.balign 4<br>
myarr:<br>
 .skip 4<br>
.balign 4<br>
addr_return:<br>
 .word 0</p>
<p>.text<br>
.balign 4<br>
.global main<br>
main:<br>
 ldr r1, add<br>
 ldr r2,arr<br>
 ldr r1,[r1]<br>
 mov r3,#10<br>
 ldr r0,add_ret<br>
 str lr,[r0]<br>
 ldr r0,print_patt<br>
 mov r1,r3<br>
 bl printf<br>
 loop:<br>
 cmp r3,#8<br>
 beq endLoop<br>
 ldr r0,print_patt<br>
 mov r1,r3<br>
 bl printf<br>
 sub r3,r3,#1<br>
 b loop<br>
 endLoop:<br>
 ldr r0,add_ret<br>
 ldr lr,[r0]<br>
 bx lr<br>
print_patt: .word print_Statement<br>
add: .word myvar<br>
print_patt: .word print_Statement<br>
add: .word myvar<br>
arr: .word myarr<br>
add_ret: .word addr_return</p>
<p>.global printf</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/?replytocom=960728#respond" onclick="return addComment.moveForm( &quot;div-comment-960728&quot;, &quot;960728&quot;, &quot;respond&quot;, &quot;622&quot; )" aria-label="Reply to Mayank">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2 parent" id="comment-960731">
				<div id="div-comment-960731" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-5">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/#comment-960731">
			February 15, 2016 at 9:34 pm</a>		</div>

		<p>Hi,</p>
<p>I haven’t checked in much detail but I think your problem is that <code>printf</code> is modifying <code>r3</code>. Recall that registers <code>r0</code> to <code>r3</code> can be freely modified by the <strong>callee</strong>, so if you’re keeping something important there, either back it up elsewhere or use another register. Recall that all other registers from <code>r4</code> to <code>r13</code> must be preserved by the callee so you know that their contents after the <code>printf</code> call are the same they had before the call.</p>
<p>Concerning whether <code>b</code> (and all its conditional versions <code>beq</code>, <code>bne</code>, …) modify <code>lr</code>. No, they don’t. Only <code>bl</code> and <code>blx</code> modify <code>lr</code>.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/?replytocom=960731#respond" onclick="return addComment.moveForm( &quot;div-comment-960731&quot;, &quot;960731&quot;, &quot;respond&quot;, &quot;622&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment odd alt depth-3 parent" id="comment-960733">
				<div id="div-comment-960733" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/28e0ab4edf9bc16556790520dc3d5354?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/28e0ab4edf9bc16556790520dc3d5354?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/28e0ab4edf9bc16556790520dc3d5354?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-28e0ab4edf9bc16556790520dc3d5354-1">			<cite class="fn">Mayank</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/#comment-960733">
			February 15, 2016 at 9:42 pm</a>		</div>

		<p>but printf is an external function and is just used for printing the value stored in r3 coming from main function. So I am not modifying anything in r3 as printf is an external function. r3 is modifying only in main.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/?replytocom=960733#respond" onclick="return addComment.moveForm( &quot;div-comment-960733&quot;, &quot;960733&quot;, &quot;respond&quot;, &quot;622&quot; )" aria-label="Reply to Mayank">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-4 parent" id="comment-960734">
				<div id="div-comment-960734" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-6">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/#comment-960734">
			February 15, 2016 at 9:48 pm</a>		</div>

		<p>I see your point but the AAPCS clearly states that a function may freely modify registers <code>r0</code> to <code>r3</code>, so you must assume that after the call to <code>printf</code>, registers <code>r0</code> to <code>r3</code> have got their values <em>clobbered</em>.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/?replytocom=960734#respond" onclick="return addComment.moveForm( &quot;div-comment-960734&quot;, &quot;960734&quot;, &quot;respond&quot;, &quot;622&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment odd alt depth-5" id="comment-960735">
				<div id="div-comment-960735" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/28e0ab4edf9bc16556790520dc3d5354?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/28e0ab4edf9bc16556790520dc3d5354?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/28e0ab4edf9bc16556790520dc3d5354?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-28e0ab4edf9bc16556790520dc3d5354-2">			<cite class="fn">Mayank</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/#comment-960735">
			February 15, 2016 at 10:18 pm</a>		</div>

		<p>But my problem is that I am getting an infinite loop here and printing: “Variable print: -1”.<br>
So how do you explain this infinite loop here? If I am just removing the loop statement then I am getting the correct output of r3 so printf has nothing to do with this.</p>

		
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
		</ul>
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2013/02/02/arm-assembler-raspberry-pi-chapter-9/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="622" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="78c4994272"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496598042368"></form>
			</div><!-- #respond -->
	</div>	</div>