# Chapter 2


<div class="main">
			<h1>Exploring AArch64 assembler – Chapter 2 </h1>
		<p class="post-meta"><span class="date">October 8, 2016</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter-2/#comments">4</a></span> </p>
		<p>
In the first installment of this series we did a very first simple program. In this chapter we will continue learning a bit more about AArch64.
</p>
<p><span id="more-3346"></span></p>
<h2>Registers</h2>
<p>
Computers only work with binary data, so programs are encoded in what it is called machine code. But writing machine code is very unwieldy, so instead assembly language is used. In assembly we can specify instructions (and their operands) and the data of the program. Instructions tell the computer what to do (so they have a meaning).
</p>
<p>
The CPU is the part of the computer that executes programs. The instructions of a CPU that implements the AArch64 architecture, can only work in data that is found inside the CPU. The place where this data is located is called the <em>registers</em>. Any data that is not in registers and must be manipulated must be loaded first in the registers. Usually the loaded data will come from the memory but it can come from peripherals. As a way to communicate with the outer world, data can also be taken out of the registers to memory or peripherals.
</p>
<p>
In AArch64 there are 31 general-purpose registers. They are called general-purpose because they can hold any kind of data. In general they hold only integer or addresses (we will talk about addresses in a later chapter) but anything that can be encoded in 64 bits can be stored in a register. These 31 registers are called <code>x0</code>, <code>x1</code>, …, <code>x30</code>. You may be wondering why 31 and not 32, which fits as a more natural power of 2 value. The reason is that what would be the <em>x31</em> is actually called <code>xzr</code> and means the <em>Zero Register</em>. It is a very special register with limited usage. Later we will see some examples on how to use this register. In general all registers can be used for any purpose, but in a later chapter we will see that there are some conventions on how to use them.
</p>
<p>
The AArch64 architecture defines more registers but they have more specific purposes and we will unveil them in later chapters.
</p>
<p>
While working with 64-bit wide registers could be enough, this would imply that all the operations happen in a 64-bit domain. Many times we do not need so many bits, in fact most programs have enough with 32-bit data (or even less). In order to provide 32-bit processing, it is possible to access the lower 32-bit of a <code>x</code><em>n</em> register using the name <code>w</code><em>n</em>. So the lower 32-bit of register <code>x6</code> is <code>w6</code>. It is not possible to name the upper 32-bits. Register <code>xzr</code> has an equivalent 32-bit name called <code>wzr</code>.
</p>
<p>
This is the reason why our program in the first chapter was just.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">mov</span> w0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span>           <span style="color: #339933;">//</span> w0 ← <span style="color: #ff0000;">2</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">mov w0, #2           // w0 ← 2</p></div>

<p>
In C the return value of <code>main</code> is an <code>int</code> value. Technically C does not specify what is the specific width in bits of a <code>int</code> value (it just states some minimal ranges of values it has to be able to represent), but for economical reasons (given that <code>int</code> is the most used type in C) almost all 64-bit environments (including AArch64 and x86-64) make <code>int</code> a 32-bit integer type.
</p>
<h2>Working with data in registers</h2>
<p>
Almost all instructions in AArch64 have three operands. A destination register and two source registers. For instance, we can store in register <code>w5</code> the result of adding registers <code>w3</code>, and <code>w4</code> doing:
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">add</span> w5<span style="color: #339933;">,</span> w3<span style="color: #339933;">,</span> w4       <span style="color: #339933;">//</span> w5 ← w3 <span style="color: #339933;">+</span> w4</pre></td></tr></tbody></table><p class="theCode" style="display:none;">add w5, w3, w4       // w5 ← w3 + w4</p></div>

<p>
similarly
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">add</span> x5<span style="color: #339933;">,</span> x3<span style="color: #339933;">,</span> x4       <span style="color: #339933;">//</span> x5 ← x3 <span style="color: #339933;">+</span> x4</pre></td></tr></tbody></table><p class="theCode" style="display:none;">add x5, x3, x4       // x5 ← x3 + x4</p></div>

<p>
but note that in general we cannot name <code>w</code><em>n</em> and <code>x</code><em>n</em> registers in the same operation.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">add</span> w5<span style="color: #339933;">,</span> w3<span style="color: #339933;">,</span> x4</pre></td></tr></tbody></table><p class="theCode" style="display:none;">add w5, w3, x4</p></div>

<p>will fail with a message suggesting valid alternatives</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="text" style="font-family:monospace;">add.s:6: Error: operand mismatch -- `add w5,w3,x4'
add.s:6: Info:    did you mean this?
add.s:6: Info:    	add w5,w3,w4
add.s:6: Info:    other valid variant(s):
add.s:6: Info:    	add x5,x3,x4</pre></td></tr></tbody></table><p class="theCode" style="display:none;">add.s:6: Error: operand mismatch -- `add w5,w3,x4'
add.s:6: Info:    did you mean this?
add.s:6: Info:    	add w5,w3,w4
add.s:6: Info:    other valid variant(s):
add.s:6: Info:    	add x5,x3,x4</p></div>

<h3>The zero register</h3>
<p>
The zero register <code>zr</code> (or <code>wzr</code>) is only useful as a source register. It does not represent a real register, it simply is a way to say «assume a zero here as the value of the operand».
</p>
<h3>Move</h3>
<p>
There are several exceptions to the one destination register and two source registers schema mentioned above. A notable one is the <code>mov</code> instruction. It takes a single source register.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">mov</span> w0<span style="color: #339933;">,</span> w1    <span style="color: #339933;">//</span> w0 ← w1</pre></td></tr></tbody></table><p class="theCode" style="display:none;">mov w0, w1    // w0 ← w1</p></div>

<p>
Note that this is a convenience instruction and it can be implemented using other instructions. A way could be adding the source register to zero. An instruction that would achieve the same as the mov above could be:
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">add</span> w0<span style="color: #339933;">,</span> w1<span style="color: #339933;">,</span> wzr   <span style="color: #339933;">//</span> w0 ← w1 <span style="color: #339933;">+</span> <span style="color: #ff0000;">0</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">add w0, w1, wzr   // w0 ← w1 + 0</p></div>

<p>
Actually in AArch64 <code>mov</code> is implemented using <code>orr</code> that is an instruction that performs a <a href="https://en.wikipedia.org/wiki/Bitwise_operation#OR">bitwise or operation</a> using as the first source operand <code>wzr</code>.
</p>
<h3>Immediates</h3>
<p>
If source operands of instructions were restricted to registers, it would be impossible to load initial values to registers. This is the reason why some instructions allow what is called <em>immediates</em>. An immediate is an integer that is encoded in the instruction itself. This means that not any value will be possible to encode using an immediate, but fortunately many will. The ranges of allowed values of immediates depends on the instruction but many of them allow numbers in the range [-4096, 4095] (i.e. 12-bit). Due to the encoding used, any number in that range multiplied by 2<sup>12</sup> (4096) is also allowed as an immediate. For instance 12288 and 16384 can be used as immediates as well (but not any other number inbetween). Immediates are represented in the assembler syntax preceding them with a <code>#</code>.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">mov</span> w0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span>      <span style="color: #339933;">//</span> w0 ← <span style="color: #ff0000;">2</span>
<span style="color: #00007f; font-weight: bold;">mov</span> w1<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">2</span>     <span style="color: #339933;">//</span> w1 ← <span style="color: #339933;">-</span><span style="color: #ff0000;">2</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">mov w0, #2      // w0 ← 2
mov w1, #-2     // w1 ← -2</p></div>

<p>
Because immediates are encoded in the instruction itself and the space constraints mentioned above, only one immediate is usually allowed. It may vary depend instructions but in general the second source operand is allowed to be an immediate.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">add</span> w0<span style="color: #339933;">,</span> w1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span>   <span style="color: #339933;">//</span> w0 ← w1 <span style="color: #339933;">+</span> <span style="color: #ff0000;">2</span>
<span style="color: #00007f; font-weight: bold;">add</span> w0<span style="color: #339933;">,</span> w1<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">2</span>   <span style="color: #339933;">//</span> w0 ← w1 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span><span style="color: #339933;">-</span><span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">)</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">add w0, w1, #2   // w0 ← w1 + 2
add w0, w1, #-2   // w0 ← w1 + (-2)</p></div>

<p>
These are not allowed:
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">add</span> w0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span><span style="color: #339933;">,</span> w1   <span style="color: #339933;">//</span> ERROR<span style="color: #339933;">:</span> second operand should be an integer register
<span style="color: #00007f; font-weight: bold;">add</span> w0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span>   <span style="color: #339933;">//</span> ERROR<span style="color: #339933;">:</span> second operand should be an integer register<span style="color: #339933;">.</span>
                 <span style="color: #339933;">//</span> This case is actually better expressed as
                 <span style="color: #339933;">//</span>    <span style="color: #00007f; font-weight: bold;">mov</span> w0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">3</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">add w0, #1, w1   // ERROR: second operand should be an integer register
add w0, #1, #2   // ERROR: second operand should be an integer register.
                 // This case is actually better expressed as
                 //    mov w0, #3</p></div>

<h3>32-bit registers as destination</h3>
<p>
When the destination register of an instruction is a 32-bit register, the upper 32-bits are set to zero. They are not preserved.
</p>
<h2>A a silly example</h2>
<p>
At this point we cannot do much things yet but we can play a bit with our program. The number of arguments to our program is found in w0 when the program starts. Let’s just return this same number plus 1.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #666666; font-style: italic;">// test.s</span>
.<span style="color: #202020;">text</span>
.<span style="color: #202020;">globl</span> main
&nbsp;
main<span style="color: #339933;">:</span>
  add w0<span style="color: #339933;">,</span> w0<span style="color: #339933;">,</span> <span style="color: #339933;">#1   // w0 ← w0 + 1</span>
  ret              <span style="color: #666666; font-style: italic;">// return from main</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">// test.s
.text
.globl main

main:
  add w0, w0, #1   // w0 ← w0 + 1
  ret              // return from main</p></div>


<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="bash" style="font-family:monospace;">$ aarch64-linux-gnu-gcc <span style="color: #660033;">-c</span> test.s
$ aarch64-linux-gnu-gcc <span style="color: #660033;">-o</span> <span style="color: #7a0874; font-weight: bold;">test</span> test.o
$ .<span style="color: #000000; font-weight: bold;">/</span><span style="color: #7a0874; font-weight: bold;">test</span> ; <span style="color: #7a0874; font-weight: bold;">echo</span> <span style="color: #007800;">$?</span>
<span style="color: #000000;">2</span>
$ .<span style="color: #000000; font-weight: bold;">/</span><span style="color: #7a0874; font-weight: bold;">test</span> foo ; <span style="color: #7a0874; font-weight: bold;">echo</span> <span style="color: #007800;">$?</span>
<span style="color: #000000;">3</span>
$ .<span style="color: #000000; font-weight: bold;">/</span><span style="color: #7a0874; font-weight: bold;">test</span> foo bar ; <span style="color: #7a0874; font-weight: bold;">echo</span> <span style="color: #007800;">$?</span>
<span style="color: #000000;">4</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ aarch64-linux-gnu-gcc -c test.s
$ aarch64-linux-gnu-gcc -o test test.o
$ ./test ; echo $?
2
$ ./test foo ; echo $?
3
$ ./test foo bar ; echo $?
4</p></div>

<p>
Yay! If you wonder why the first case returns 2 instead of 1, it is because in UNIX the <code>main</code> function of a C program always receives a first parameter with the name of the program executed.
</p>
<p>
That’s all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter-2/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter-2/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter-2/&amp;text=Exploring+AArch64+assembler+%E2%80%93+Chapter+2+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter-2/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>						<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2016/10/08/not-write-c-parser-part-2/" rel="prev">How (not) to write a C++ front end – Part 2</a></span>
			<span class="next"><a href="http://thinkingeek.com/2016/10/23/exploring-aarch64-assembler-chapter-3/" rel="next">Exploring AArch64 assembler – Chapter 3</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
			<h3>4 thoughts on “<span>Exploring AArch64 assembler – Chapter 2</span>”</h3>
		<ul class="commentlist">
				<li class="comment even thread-even depth-1 parent" id="comment-977563">
				<div id="div-comment-977563" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/07ed85448b41be2f49921c4de3d98fb0?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/07ed85448b41be2f49921c4de3d98fb0?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-07ed85448b41be2f49921c4de3d98fb0-0" originals="32" src-orig="http://0.gravatar.com/avatar/07ed85448b41be2f49921c4de3d98fb0?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Claes</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter-2/#comment-977563">
			October 21, 2016 at 11:01 pm</a>		</div>

		<p>A great guide, thank you!</p>
<p>A few typos, I think the name of the 64-bit zero register should be “xzr”, not “zr”, and in the first chapter should it be “$ qemu-aarch64 ./first” not “$ ./first”.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter-2/?replytocom=977563#respond" onclick="return addComment.moveForm( &quot;div-comment-977563&quot;, &quot;977563&quot;, &quot;respond&quot;, &quot;3346&quot; )" aria-label="Reply to Claes">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-977617">
				<div id="div-comment-977617" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-0">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter-2/#comment-977617">
			October 22, 2016 at 7:33 pm</a>		</div>

		<p>Hi Claes,</p>
<p>thanks for spotting those! I have updated the text.</p>
<p>Kind regards.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter-2/?replytocom=977617#respond" onclick="return addComment.moveForm( &quot;div-comment-977617&quot;, &quot;977617&quot;, &quot;respond&quot;, &quot;3346&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-982701">
				<div id="div-comment-982701" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a296bcb1fbd896dbcbf5d24a23968357?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a296bcb1fbd896dbcbf5d24a23968357?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-a296bcb1fbd896dbcbf5d24a23968357-0" originals="32" src-orig="http://1.gravatar.com/avatar/a296bcb1fbd896dbcbf5d24a23968357?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Jeremy</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter-2/#comment-982701">
			December 27, 2016 at 7:07 pm</a>		</div>

		<p>for A64, you omit the # in immediates, the assemblers must accept a #.  For example:</p>
<p>add w0, w0, 1</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter-2/?replytocom=982701#respond" onclick="return addComment.moveForm( &quot;div-comment-982701&quot;, &quot;982701&quot;, &quot;respond&quot;, &quot;3346&quot; )" aria-label="Reply to Jeremy">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-983619">
				<div id="div-comment-983619" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-1">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter-2/#comment-983619">
			January 8, 2017 at 4:41 pm</a>		</div>

		<p>Hi Jeremy, good to know. Thanks!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter-2/?replytocom=983619#respond" onclick="return addComment.moveForm( &quot;div-comment-983619&quot;, &quot;983619&quot;, &quot;respond&quot;, &quot;3346&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		</ul>
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2016/10/08/exploring-aarch64-assembler-chapter-2/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="3346" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="c629bdb8bb"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496668753002"></form>
			</div><!-- #respond -->
	</div>	</div>