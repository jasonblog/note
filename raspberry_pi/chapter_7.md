# Chapter 7


<div class="main">
			<h1>Exploring AArch64 assembler – Chapter 7 </h1>
		<p class="post-meta"><span class="date">March 19, 2017</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2017/03/19/exploring-aarch64-assembler-chapter-7/#respond">0</a></span> </p>
		<p>
In the previous installment of this series we saw how to alter the sequencing of our programs. Today we will see how we can reuse instructions by means of branches. Let’s talk about functions.
</p>
<p><span id="more-3778"></span></p>
<h2>Routines</h2>
<p>
In the process of solving a problem with a computer we will encounter that some of its steps are repeatedly performed. These steps, probably part of an algorithm, can be encoded using instructions. This means that we may end with many sequences of instructions whose purpose is the same. What if we could <em>factor out</em> these instructions in a single place, and use them when needed. This is the fundamental idea behind a routine. We rarely use the word routine nowadays and most programming languages use other names like functions, procedures, subroutines, methods, lambda expressions, etc. Sure, there are differences between them but all of them encompass the idea of <em>reusing code</em>. I will use the common term <q>function</q>.
</p>
<h2>Using a function</h2>
<p>
Functions can be handled like values (i.e. like an integer value) and there are a few operations we can do with them. At this level, though, we will only care about two of them: getting the address of the function, which we will usually achieve in a trivial way just using a label, and <em>calling</em> the function, which is the interesting bit.
</p>
<h3>Address of a function</h3>
<p>
Functions will be a sequence of instructions that we will reuse. We will identify functions using the address of their first instruction. Most of the time a label will be used to designate the function as, recall, a label is an address.
</p>
<h3>Calling a function</h3>
<p>
Calling a function is a process that in practice means passing some data to the function, branching to the address of the function. When the function ends, it will branch back to the caller.
</p>
<p>
A call to a function is at its core a branch, but it is a special branch that is so frequent that it is worth to devote an instruction only for it. In AArch64 this instruction is <code>bl</code> which means something like <em>branch and link</em>. It is an unconditional branch that performs what an unconditional branch does plus it sets to <code>x30</code> to the address of the next instruction after <code>bl</code>. Recall that <code>x30</code> is a general purpose register but in this case we’re giving it a special meaning: it contains the address where the function must branch back when it ends. For historical reasons, when <code>x30</code> is used with this purpose it is called the <em>link register</em>.
</p>
<p>
To return from a function, then, the only thing we have to do is just branch to the value of <code>x30</code>. There is an instruction to unconditionally branch to the address stored in a register called <code>br</code>. So a way to call a function and just return from it is the following.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #0000ff; font-weight: bold;">.text</span>
my_function<span style="color: #339933;">:</span>
  br x30
caller<span style="color: #339933;">:</span>
  <span style="color: #46aa03; font-weight: bold;">bl</span> my_function
  <span style="color: #339933;">//</span> more instructions <span style="color: #339933;">...</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">.text
my_function:
  br x30
caller:
  bl my_function
  // more instructions ...</p></div>

<p>
But returning from a function is also a very common operation so rather than doing <code>br x30</code> we can just do <code>ret</code>.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #0000ff; font-weight: bold;">.text</span>
my_function<span style="color: #339933;">:</span>
  <span style="color: #00007f; font-weight: bold;">ret</span>
caller<span style="color: #339933;">:</span>
  <span style="color: #46aa03; font-weight: bold;">bl</span> my_function
  <span style="color: #339933;">//</span> more instructions <span style="color: #339933;">...</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">.text
my_function:
  ret
caller:
  bl my_function
  // more instructions ...</p></div>

<h2>Parameter passing</h2>
<p>
Ok, so we know the basics of calling a function and returning from it. But, this way, the only thing we achieve is just reuse sequences of instructions that will always do the same. Many times we will want to parameterize the behaviour of the function: for instance we want a function that computes the average of two given numbers. This means that when calling the function we need a mechanism to pass those parameters to the function.
</p>
<p>
At this point we can consider several approaches. A first approach is using global variables where we will first put the values and the function will read those values and leave the result in some other global variable. This works, kind of, and some early programming languages worked this way. The problem is that this mechanism composes badly because it bars using recursion and even worse, it does not work in multithreaded environments. In a modern setting this approach is rarely used.
</p>
<p>
Another alternative, is, well, we can use a <em>private memory</em> that we use only for calling functions. And we give it some kind of <em>stack discipline</em>: we can put or remove things only on its top (but we can access the i-th element below the top of the stack). Before we call a function we put the arguments on the top of the stack. The function can access the top of the stack and the elements below it to retrieve the parameters. The result can also be put in the stack, for instance, the function can replace the parameters with the result value, so the caller only has  to check the top of the stack again. This technique works well for recursion and also for multithreading. This is a technique used in some programming languages and in architectures with a severe shortage of registers (like 32-bit x86).
</p>
<p>
A hybrid approach, the common used in RISC architectures, involves devoting a few registers to pass parameters and if we run out of them, use a stack-like approach like the one described above. This works well because in general most function calls involve a few parameters, usually less than 4 or so. And given that in AArch64 we have around 30 registers, it makes sense to devote a few of them as the parameter passing. Which ones? Well, this is a conventional thing, and as a convention it should be agreed upon first.
</p>
<p>
We could use any convention (even made our own) but in AArch64 there is one already described in <a href="https://developer.arm.com/docs/ihi0055/latest/procedure-call-standard-for-the-arm-64-bit-architecture-aarch64">Procedure Call Standard for the ARM 64-bit Architecture</a> (or for short the PCS). That is a very long document with lots of details. For the purpose of this series we will simplify the convention as follows:
</p>
<ul>
<li>Registers <code>x0</code>–<code>x7</code> are used to pass parameters and return values. The value of these registers may be freely modified by the called function (the callee) so the caller cannot assume anything about their content, even if they are not used in the parameter passing or for the returned value. This means that these registers are in practice <em>calle<strong>r</strong>-saved</em>.
</li><li>Registers <code>x8</code>–<code>x18</code> are temporary registers for every function. As such no assumption can be made on their values upon returning a function. In practice these registers are also <em>caller-saved</em>.
</li><li>Registers <code>x19</code>–<code>x28</code> are registers, that, if used by the function, must have their values preserved and later restored upon returning the function. This registers are known as <em>calle<strong>e</strong>-saved</em>.
</li><li>We already know that register x30 is the <em>link register</em> and its value must be preserved until the function uses the <code>ret</code> instruction to return to the caller.
</li></ul>
<p>
This means that we can pass up to 8 parameters in registers <code>x0</code> to <code>x7</code>. If we are passing an integer of 64-bit or an address we will use the corresponding <code>x<em>i</em></code> register. For 32-bit integers we will use the corresponding <code>w<em>i</em></code> (we won’t bother packing two 32-bit integers in a single 64-bit register).
</p>
<p>
What if we have to pass more than 8 parameters? How do we keep registers <code>x19</code> to <code>x28</code> and more importantly, how do we keep <code>x30</code>? Well, in this case we will have to use the stack, but we will leave this for another chapter. In this chapter we will use a global variable to temporarily store <code>x30</code>.
</p>
<h2>Say hello!</h2>
<p>
Ok, equipped with this knowledge, we can now start doing some interesting examples. As a starter for today we will say hello. We can use the function <code>puts</code> of the C library for this. This function only receives a parameter: an address to a null-ended string.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
<span style="color: #339933;">.</span>balign <span style="color: #ff0000;">8</span>
<span style="color: #339933;">/*</span> This is the greeting message <span style="color: #339933;">*/</span>
say_hello<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"Hello world!"</span>
&nbsp;
<span style="color: #339933;">.</span>balign <span style="color: #ff0000;">8</span>
<span style="color: #339933;">/*</span> We need to keep x30 otherwise we will <span style="color: #00007f; font-weight: bold;">not</span> be able to return from main! <span style="color: #339933;">*/</span>
keep_x30<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">dword</span> <span style="color: #ff0000;">0</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
<span style="color: #339933;">/*</span> We are going to <span style="color: #00007f; font-weight: bold;">call</span> a C<span style="color: #339933;">-</span>library puts function <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span>globl puts
&nbsp;
<span style="color: #339933;">.</span>globl main
main<span style="color: #339933;">:</span>
    ldr w0<span style="color: #339933;">,</span> addr_keep_x30     <span style="color: #339933;">//</span> w0 ← &amp;keep_30   <span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">32</span><span style="color: #009900; font-weight: bold;">]</span>
    <span style="color: #00007f; font-weight: bold;">str</span> x30<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>x0<span style="color: #009900; font-weight: bold;">]</span>             <span style="color: #339933;">//</span> <span style="color: #339933;">*</span>keep_30 ← x30  <span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">64</span><span style="color: #009900; font-weight: bold;">]</span>
&nbsp;
    ldr w0<span style="color: #339933;">,</span> addr_say_hello    <span style="color: #339933;">//</span> w0 ← &amp;say_hello <span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">32</span><span style="color: #009900; font-weight: bold;">]</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> puts                   <span style="color: #339933;">//</span> <span style="color: #00007f; font-weight: bold;">call</span> puts
&nbsp;
    ldr w0<span style="color: #339933;">,</span> addr_keep_x30     <span style="color: #339933;">//</span> w0 ← &amp;keep_30   <span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">32</span><span style="color: #009900; font-weight: bold;">]</span>
    ldr x30<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>x0<span style="color: #009900; font-weight: bold;">]</span>             <span style="color: #339933;">//</span> x30 ← <span style="color: #339933;">*</span>keep_30  <span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">64</span><span style="color: #009900; font-weight: bold;">]</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> w0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>                <span style="color: #339933;">//</span> w0 ← <span style="color: #ff0000;">0</span>
    <span style="color: #00007f; font-weight: bold;">ret</span>                       <span style="color: #339933;">//</span> return
&nbsp;
addr_keep_x30 <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> keep_x30
addr_say_hello<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> say_hello</pre></td></tr></tbody></table><p class="theCode" style="display:none;">.data

.balign 8
/* This is the greeting message */
say_hello: .asciz "Hello world!"

.balign 8
/* We need to keep x30 otherwise we will not be able to return from main! */
keep_x30: .dword 0

.text

/* We are going to call a C-library puts function */
.globl puts

.globl main
main:
    ldr w0, addr_keep_x30     // w0 ← &amp;keep_30   [32]
    str x30, [x0]             // *keep_30 ← x30  [64]

    ldr w0, addr_say_hello    // w0 ← &amp;say_hello [32]
    bl puts                   // call puts

    ldr w0, addr_keep_x30     // w0 ← &amp;keep_30   [32]
    ldr x30, [x0]             // x30 ← *keep_30  [64]

    mov w0, #0                // w0 ← 0
    ret                       // return

addr_keep_x30 : .word keep_x30
addr_say_hello: .word say_hello</p></div>

<p>
In line 3 we make sure the next data emitted by the assembler is aligned to an address multiple of 8 bytes (64-bit). In this case we want the assembler to emit a null-ended string of the characters <code>Hello world!</code>. We can use the directive <code>.asciz</code> for this (line 5). In order to be able to use this string later we set the label <code>say_hello</code> which will be the address of such string.
</p>
<p>
Since we will not see how the stack is used in this chapter, we still need to save the value of x30 somewhere. So we allocate some storage for it. Again we want this storage to be aligned to 8 bytes, so we use a <code>.balign</code> directive again (line 7). Then we define the storage itself and we label it as <code>keep_x30</code>, so we can refer to it later. As we know, <code>.dword</code> directive will emit the specified integer value as a 64-bit integer. That’s all for the data section of our small program.
</p>
<p>
In line 14 we say that we are going to use the symbol <code>puts</code>. This is the name of a function defined in the C library, so we use <code>.globl</code> (line 14) to state that this is a global symbol (in contrast of a private one). As we already know, we need to do the same for main (line 17).
</p>
<p>
Now check lines 30 and 31. Here we define storage that will contain the addresses of <code>say_hello</code> and <code>keep_x30</code>. As you recall from chapter 5, this is because we need to keep the addresses close to the load instruction. We will use 32-bit addresses. 64-bit addresses (using <code>.dword</code>) are possible but for these examples they are just wasteful.
</p>
<p>
Now back to line 18, here we load in <code>w0</code> the address of <code>keep_x30</code>. Note that, as stated above, this address is a 32-bit address so we use the 32-bit lower part of <code>x0</code>, this is, <code>w0</code>. Now we can use this address to store the register <code>x30</code>, line 19. Recall that loading a 32-bit register clears its 32-bit upper bits, so it is safe to use the address contained in <code>x0</code> to store <code>x30</code>.
</p>
<p>
Now that we have kept x30, we can call <code>puts</code>. First we need to prepare the function call following the convention described above. Function <code>puts</code> is a function in the C-library that only receives an address to a null-ended buffer of bytes. Precisely what we have in <code>say_hello</code>. As puts receives the address, not the contents themselves, we will use <code>addr_say_hello</code> instead. As described above the first parameter is passed in <code>x0</code>, so we just load the address of <code>say_hello</code> (that as said we have in <code>addr_say_hello</code>) in <code>x0</code>, line 21.</p>
<p></p><p>
Now everything is in place, then we make the call to puts in line 22. If all is correct our program will continue in the next instruction of the call, line 24. Here we simply restore the value of <code>x30</code>, as the instruction <code>bl</code> in line 22 overwrote it. Basically we load again the address of <code>keep_x30</code> and we do a load with that address to the <code>x30</code> register lines 24-25. Now everything is in place to return, so we set <code>w0</code> to 0, line 27, and we return using ret, line 28.
</p>
<p>
If we try to run this program we will be greeted.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="bash" style="font-family:monospace;">$ .<span style="color: #000000; font-weight: bold;">/</span>hello
Hello world<span style="color: #000000; font-weight: bold;">!</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ ./hello
Hello world!</p></div>

<p>
Yay! <img draggable="false" class="emoji" alt="🙂" src="https://s.w.org/images/core/emoji/2.2.1/svg/1f642.svg" scale="0"></p>
<p></p><p>
This is all for today!</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2017/03/19/exploring-aarch64-assembler-chapter-7/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2017/03/19/exploring-aarch64-assembler-chapter-7/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2017/03/19/exploring-aarch64-assembler-chapter-7/&amp;text=Exploring+AArch64+assembler+%E2%80%93+Chapter+7+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2017/03/19/exploring-aarch64-assembler-chapter-7/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>						<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2017/02/25/compilation-array-expressions-fortran/" rel="prev">Compilation of array expressions in Fortran</a></span>
			<span class="next"><a href="http://thinkingeek.com/2017/04/02/optimization/" rel="next">Whose is this optimization?</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2017/03/19/exploring-aarch64-assembler-chapter-7/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="3778" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="bc5fdcaa11"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496669015581"></form>
			</div><!-- #respond -->
	</div>	</div>