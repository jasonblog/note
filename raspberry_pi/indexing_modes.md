# Indexing modes


<div class="main">
			<h1>ARM assembler in Raspberry Pi – Chapter 7 </h1>
		<p class="post-meta"><span class="date">January 26, 2013</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2013/01/26/arm-assembler-raspberry-pi-chapter-7/#comments">8</a></span> </p>
		<p>
ARM architecture has been for long targeted at embedded systems. Embedded systems usually end being used in massively manufactured products (dishwashers, mobile phones, TV sets, etc). In this context margins are very tight so a designer will always try to spare as much components as possible (a cent saved in hundreds of thousands or even millions of appliances may pay off). One relatively expensive component is memory although every day memory is less and less expensive. Anyway, in constrained memory environments being able to save memory is good and ARM instruction set was designed with this goal in mind. It will take us several chapters to learn all of these techniques, today we will start with one feature usually named <em>shifted operand</em>.
</p>
<p><span id="more-513"></span></p>
<h2>Indexing modes</h2>
<p>
We have seen that, except for load (<code>ldr</code>), store (<code>str</code>) and branches (<code>b</code> and <code>bXX</code>), ARM instructions take as operands either registers or immediate values. We have also seen that the first operand is usually the destination register (being <code>str</code> a notable exception as there it plays the role of source because the destination is now the memory). Instruction <code>mov</code> has another operand, a register or an immediate value. Arithmetic instructions like <code>add</code> and <code>and</code> (and many others) have two more source registers, the first of which is always a register and the second can be a register or an immediate value.
</p>
<p>
These sets of allowed operands in instructions are collectively called <em>indexing modes</em>. Today this concept will look a bit off since we will not index anything. The name <em>indexing</em> makes sense in memory operands but ARM instructions, except load and store, do not have memory operands. This is the nomenclature you will find in ARM documentation so it seems sensible to use theirs.
</p>
<p>
We can summarize the syntax of most of the ARM instructions in the following pattern
</p>
<pre>instruction Rdest, Rsource1, source2
</pre>
<p>
There are some exceptions, mainly move (<code>mov</code>), branches, load and stores. In fact move is not so different actually.
</p>
<pre>mov Rdest, source2
</pre>
<p>Both <code>Rdest</code> and <code>Rsource1</code> must be registers. In the next section we will talk about <code>source2</code>.</p>
<p>
We will discuss the indexing modes of load and store instructions in a future chapter. Branches, on the other hand, are surprisingly simple and their single operand is just a label of our program, so there is little to discuss on indexing modes for branches.
</p>
<h2>Shifted operand</h2>
<p>
What is this mysterious <code>source2</code> in the instruction patterns above? If you recall the previous chapters we have used registers or immediate values. So at least that <code>source2</code> is this: register or immediate value. You can use an immediate or a register where a <code>source2</code> is expected. Some examples follow, but we have already used them in the examples of previous chapters.
</p>
<pre>mov r0, #1
mov r1, r0
add r2, r1, r0
add r2, r3, #4
</pre>
<p>
But <code>source2</code> can be much more than just a simple register or an immediate. In fact, when it is a register we can combine it with a <em>shift operation</em>. We already saw one of these shift operations in chapter 6. Not it is time to unveil all of them.
</p>
<ul>
<li><code>LSL #n</code><br>
<strong>L</strong>ogical <strong>S</strong>hift <strong>L</strong>eft. Shifts bits <code>n</code> times left. The <code>n</code> leftmost bits are lost and the <code>n</code> rightmost are set to zero.<p></p>
</li><li><code>LSL Rsource3</code><br>
Like the previous one but instead of an immediate the lower byte of a register specifies the amount of shifting.<p></p>
</li><li><code>LSR #n</code><br>
<strong>L</strong>ogical <strong>S</strong>hift <strong>R</strong>ight. Shifts bits <code>n</code> times right. The <code>n</code> rightmost bits are lost and the <code>n</code> leftmost bits are set to zero,<p></p>
</li><li><code>LSR Rsource3</code><br>
Like the previous one but instead of an immediate the lower byte of a register specifies the amount of shifting.<p></p>
</li><li><code>ASR #n</code><br>
<strong>A</strong>rithmetic <strong>S</strong>hift <strong>R</strong>ight. Like LSR but the leftmost bit before shifting is used instead of zero in the <code>n</code> leftmost ones.<p></p>
</li><li><code>ASR Rsource3</code><br>
Like the previous one but using a the lower byte of a register instead of an immediate.<p></p>
</li><li><code>ROR #n</code><br>
<strong>Ro</strong>tate <strong>R</strong>ight. Like LSR but the <code>n</code> rightmost bits are not lost but pushed onto the <code>n</code> leftmost bits<p></p>
</li><li><code>ROR Rsource3</code><br>
Like the previous one but using a the lower byte of a register instead of an immediate.
</li></ul>
<p>
In the listing above, <code>n</code> is an immediate from 1 to 31. These extra operations may be applied to the value in the second source register (to the value, not to the register itself) so we can perform some more operations in a single instruction. For instance, ARM does not have any shift right or left instruction. You just use the <code>mov</code> instruction.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">1</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">mov r1, r2, LSL #1</p></div>

<p>
You may be wondering why one would want to shift left or right the value of a register. If you recall chapter 6 we saw that shifting left (<code>LSL</code>) a value gives a value that the same as multiplying it by 2. Conversely, shifting it right (<code>ASR</code> if we use two’s complement, <code>LSR</code> otherwise) is the same as dividing by 2. Since a shift of <code>n</code> is the same as doing <code>n</code> shifts of 1, shifts actually multiply or divide a value by 2<sup>n</sup>.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">1</span>      <span style="color: #339933;">/*</span> r1 ← <span style="color: #009900; font-weight: bold;">(</span>r2<span style="color: #339933;">*</span><span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span>      <span style="color: #339933;">/*</span> r1 ← <span style="color: #009900; font-weight: bold;">(</span>r2<span style="color: #339933;">*</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> r3<span style="color: #339933;">,</span> ASR #<span style="color: #ff0000;">3</span>      <span style="color: #339933;">/*</span> r1 ← <span style="color: #009900; font-weight: bold;">(</span>r3<span style="color: #339933;">/</span><span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">mov</span> r3<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>
<span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> r3      <span style="color: #339933;">/*</span> r1 ← <span style="color: #009900; font-weight: bold;">(</span>r2<span style="color: #339933;">*</span><span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">mov r1, r2, LSL #1      /* r1 ← (r2*2) */
mov r1, r2, LSL #2      /* r1 ← (r2*4) */
mov r1, r3, ASR #3      /* r1 ← (r3/8) */
mov r3, #4
mov r1, r2, LSL r3      /* r1 ← (r2*16) */</p></div>

<p>
We can combine it with <code>add</code> to get some useful cases.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">add</span> r1<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">1</span>   <span style="color: #339933;">/*</span> r1 ← r2 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span>r2<span style="color: #339933;">*</span><span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">)</span> equivalent to r1 ← r2<span style="color: #339933;">*</span><span style="color: #ff0000;">3</span> <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">add</span> r1<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span>   <span style="color: #339933;">/*</span> r1 ← r2 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span>r2<span style="color: #339933;">*</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span> equivalent to r1 ← r2<span style="color: #339933;">*</span><span style="color: #ff0000;">5</span> <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">add r1, r2, r2, LSL #1   /* r1 ← r2 + (r2*2) equivalent to r1 ← r2*3 */
add r1, r2, r2, LSL #2   /* r1 ← r2 + (r2*4) equivalent to r1 ← r2*5 */</p></div>

<p>
You can do something similar with <code>sub</code>.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">sub</span> r1<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">3</span>  <span style="color: #339933;">/*</span> r1 ← r2 <span style="color: #339933;">-</span> <span style="color: #009900; font-weight: bold;">(</span>r2<span style="color: #339933;">*</span><span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span> equivalent to r1 ← r2<span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #339933;">-</span><span style="color: #ff0000;">7</span><span style="color: #009900; font-weight: bold;">)</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">sub r1, r2, r2, LSL #3  /* r1 ← r2 - (r2*8) equivalent to r1 ← r2*(-7)</p></div>

<p>
ARM comes with a handy <code>rsb</code> (<strong>R</strong>everse <strong>S</strong>u<strong>b</strong>stract) instruction which computes <code>Rdest ← source2 - Rsource1</code> (compare it to <code>sub</code> which computes <code>Rdest ← Rsource1 - source2</code>).
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">rsb r1<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">3</span>      <span style="color: #339933;">/*</span> r1 ← <span style="color: #009900; font-weight: bold;">(</span>r2<span style="color: #339933;">*</span><span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">-</span> r2 equivalent to r1 ← r2<span style="color: #339933;">*</span><span style="color: #ff0000;">7</span> <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">rsb r1, r2, r2, LSL #3      /* r1 ← (r2*8) - r2 equivalent to r1 ← r2*7 */</p></div>

<p>
Another example, a bit more contrived.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> Complicated way to multiply the initial value of r1 by <span style="color: #ff0000;">42</span> = <span style="color: #ff0000;">7</span><span style="color: #339933;">*</span><span style="color: #ff0000;">3</span><span style="color: #339933;">*</span><span style="color: #ff0000;">2</span> <span style="color: #339933;">*/</span>
rsb r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">3</span>  <span style="color: #339933;">/*</span> r1 ← <span style="color: #009900; font-weight: bold;">(</span>r1<span style="color: #339933;">*</span><span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">-</span> r1 equivalent to r1 ← <span style="color: #ff0000;">7</span><span style="color: #339933;">*</span>r1 <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">add</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">1</span>  <span style="color: #339933;">/*</span> r1 ← r1 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span><span style="color: #ff0000;">2</span><span style="color: #339933;">*</span>r1<span style="color: #009900; font-weight: bold;">)</span> equivalent to r1 ← <span style="color: #ff0000;">3</span><span style="color: #339933;">*</span>r1 <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">add</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> r1          <span style="color: #339933;">/*</span> r1 ← r1 <span style="color: #339933;">+</span> r1     equivalent to r1 ← <span style="color: #ff0000;">2</span><span style="color: #339933;">*</span>r1 <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* Complicated way to multiply the initial value of r1 by 42 = 7*3*2 */
rsb r1, r1, r1, LSL #3  /* r1 ← (r1*8) - r1 equivalent to r1 ← 7*r1 */
add r1, r1, r1, LSL #1  /* r1 ← r1 + (2*r1) equivalent to r1 ← 3*r1 */
add r1, r1, r1          /* r1 ← r1 + r1     equivalent to r1 ← 2*r1 */</p></div>

<p>
You are probably wondering why would we want to use shifts to perform multiplications. Well, the generic multiplication instruction always work but it is usually much harder to compute by our ARM processor so it may take more time. There are times where there is no other option but for many small constant values a single instruction may be more efficient.
</p>
<p>
Rotations are less useful than shifts in everyday use. They are usually used in cryptography, to reorder bits and “scramble” them. ARM does not provide a way to rotate left but we can do a <code>n</code> rotate left doing a <code>32-n</code> rotate right.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> Assume r1 is <span style="color: #ff0000;">0x12345678</span> <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">ROR</span> #<span style="color: #ff0000;">1</span>   <span style="color: #339933;">/*</span> r1 ← r1 <span style="color: #00007f; font-weight: bold;">ror</span> <span style="color: #ff0000;">1</span><span style="color: #339933;">.</span> This is r1 ← <span style="color: #ff0000;">0x91a2b3c</span> <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">ROR</span> #<span style="color: #ff0000;">31</span>  <span style="color: #339933;">/*</span> r1 ← r1 <span style="color: #00007f; font-weight: bold;">ror</span> <span style="color: #ff0000;">31</span><span style="color: #339933;">.</span> This is r1 ← <span style="color: #ff0000;">0x12345678</span> <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* Assume r1 is 0x12345678 */
mov r1, r1, ROR #1   /* r1 ← r1 ror 1. This is r1 ← 0x91a2b3c */
mov r1, r1, ROR #31  /* r1 ← r1 ror 31. This is r1 ← 0x12345678 */</p></div>

<p>
That’s all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2013/01/26/arm-assembler-raspberry-pi-chapter-7/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2013/01/26/arm-assembler-raspberry-pi-chapter-7/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2013/01/26/arm-assembler-raspberry-pi-chapter-7/&amp;text=ARM+assembler+in+Raspberry+Pi+%E2%80%93+Chapter+7+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2013/01/26/arm-assembler-raspberry-pi-chapter-7/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>				<p class="tags"><span><a href="http://thinkingeek.com/tag/arm/" rel="tag">arm</a>, <a href="http://thinkingeek.com/tag/assembler/" rel="tag">assembler</a>, <a href="http://thinkingeek.com/tag/indexing-modes/" rel="tag">indexing modes</a>, <a href="http://thinkingeek.com/tag/pi/" rel="tag">pi</a>, <a href="http://thinkingeek.com/tag/raspberry/" rel="tag">raspberry</a></span></p>		<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2013/01/20/arm-assembler-raspberry-pi-chapter-6/" rel="prev">ARM assembler in Raspberry Pi – Chapter 6</a></span>
			<span class="next"><a href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/" rel="next">ARM assembler in Raspberry Pi – Chapter 8</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
			<h3>8 thoughts on “<span>ARM assembler in Raspberry Pi – Chapter 7</span>”</h3>
		<ul class="commentlist">
				<li class="comment even thread-even depth-1 parent" id="comment-1086">
				<div id="div-comment-1086" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-9e81362ff9654533accffc53762c9520-0">			<cite class="fn">Fernando</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/26/arm-assembler-raspberry-pi-chapter-7/#comment-1086">
			March 21, 2013 at 11:41 pm</a>		</div>

		<p>Any insight of why is multiplication slower than addition?</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/26/arm-assembler-raspberry-pi-chapter-7/?replytocom=1086#respond" onclick="return addComment.moveForm( &quot;div-comment-1086&quot;, &quot;1086&quot;, &quot;respond&quot;, &quot;513&quot; )" aria-label="Reply to Fernando">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2 parent" id="comment-1104">
				<div id="div-comment-1104" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-0">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/26/arm-assembler-raspberry-pi-chapter-7/#comment-1104">
			March 27, 2013 at 9:17 pm</a>		</div>

		<p>The general multiplication algorithm is more expensive than addition because it involves more steps.</p>
<p>When you add two numbers by hand, you start by vertically aligning their digits. Then you add vertically each column starting from the right and moving to the left, taking care of the carry values you may need during the addition. Well, the circuitry of a processor does something similar. But a processor is able to advance the different carries found along the computation in a way that adding two numbers of N bits does not take a “k*N” amount of time but “k*log(N)”. Compare how you calculate a sum: when you add two numbers of 10 digits you need twice more time than when you add two numbers of 5 digits. The processor would just need one extra step to add 10 digits compared to adding 5 digits. Cool, isn’t it? <img draggable="false" class="emoji" alt="🙂" src="https://s.w.org/images/core/emoji/2.2.1/svg/1f642.svg" scale="0"></p>
<p>Now consider multiplication. When you multiply two numbers, you start as well by vertically aligning them. But now, you pick each digit of one of the rows (I use the lower one) and then you multiply it with every number of the other row (in my case it would be the upper one). This gives you an intermediate row. Then you repeat this process with the digit at the left of the previous one, giving you a new intermediate row that you put below the previous intermediate one but shifted left a position. Once you have done this with all the digits of the first row you use (the lower one in my case), then you add all the intermediate rows. As you can see, there are lots of steps involved here. When you multiply binary numbers a similar thing happens, though multiplying binary numbers is easier because it is just multiplying by 0 or 1, but you still have to add the intermediate rows. So the whole process is much more complicated. So the processor needs more time to calculate it.</p>
<p>When multiplying a number by a power of 2 (2, 4, 8, 16, …) shifting left is as simple as moving 1 bit to the left. All this is done by just routing the bits inside the processor: no complex computation is needed at all. So avoiding multiplications is usually a Good Thing™</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/26/arm-assembler-raspberry-pi-chapter-7/?replytocom=1104#respond" onclick="return addComment.moveForm( &quot;div-comment-1104&quot;, &quot;1104&quot;, &quot;respond&quot;, &quot;513&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment even depth-3" id="comment-1146">
				<div id="div-comment-1146" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-9e81362ff9654533accffc53762c9520-1">			<cite class="fn">Fernando</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/26/arm-assembler-raspberry-pi-chapter-7/#comment-1146">
			April 7, 2013 at 9:21 pm</a>		</div>

		<p>After asking, I thought a bit about it, and came to the same conclusion you did (I know, I should have thought before asking).</p>
<p>Of course, you added some extra details I find really interesting. I hope this is useful for more people as well.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/26/arm-assembler-raspberry-pi-chapter-7/?replytocom=1146#respond" onclick="return addComment.moveForm( &quot;div-comment-1146&quot;, &quot;1146&quot;, &quot;respond&quot;, &quot;513&quot; )" aria-label="Reply to Fernando">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1 parent" id="comment-652130">
				<div id="div-comment-652130" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/2d68fd28a4c511ec72036936bb7220c2?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/2d68fd28a4c511ec72036936bb7220c2?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/2d68fd28a4c511ec72036936bb7220c2?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-2d68fd28a4c511ec72036936bb7220c2-0">			<cite class="fn"><a href="https://blulin.wordpress.com" rel="external nofollow" class="url">blulin</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/26/arm-assembler-raspberry-pi-chapter-7/#comment-652130">
			November 22, 2014 at 7:59 pm</a>		</div>

		<blockquote><p>For instance, ARM does not have any shift right or left instruction. You just use the mov instruction. </p></blockquote>
<p>Looks like  shift instructions are available.<br>
Reference : <a href="http://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/armv6.html" rel="nofollow">http://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/armv6.html</a></p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/26/arm-assembler-raspberry-pi-chapter-7/?replytocom=652130#respond" onclick="return addComment.moveForm( &quot;div-comment-652130&quot;, &quot;652130&quot;, &quot;respond&quot;, &quot;513&quot; )" aria-label="Reply to blulin">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2 parent" id="comment-652604">
				<div id="div-comment-652604" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-1">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/26/arm-assembler-raspberry-pi-chapter-7/#comment-652604">
			November 22, 2014 at 10:43 pm</a>		</div>

		<p>Hi blulin,</p>
<p>no, <code>lsl</code> is not an ARMv6 instruction but a <a href="http://en.wikipedia.org/wiki/Assembly_language#Opcode_mnemonics_and_extended_mnemonics" rel="nofollow">mnemonic</a> implemented by the assembler. It looks like an instruction but it is just another instruction in disguise.</p>
<p>You can see that by doing the following:</p>
<ol>
<li>create a <code>test.s</code> file that contains<br>
<code><br>
.text<br>
main:<br>
&nbsp;&nbsp;lsl r0, #3<br>
&nbsp;&nbsp;mov r0, r0, LSL #3<p></p>
</code><p><code></code></p>
</li>
<li>compile it with <code>gcc -c test.s</code>, this will create a file <code>test.o</code>
        </li>
<li>disassemble it with <code>objdump -d test.o</code>, you will see the following two instructions<br>
<code><br>
00000000&nbsp;&lt;main&gt;:<br>
&nbsp;&nbsp;&nbsp;0:&nbsp;e1a00180&nbsp;&nbsp;lsl&nbsp;r0,&nbsp;r0,&nbsp;#3<br>
&nbsp;&nbsp;&nbsp;4:&nbsp;e1a00180&nbsp;&nbsp;lsl&nbsp;r0,&nbsp;r0,&nbsp;#3<br>
</code><br>
note that they are the same instruction. <code>objdump</code> recognizes the pattern and shows the mnemonic instead.
</li>
</ol>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/26/arm-assembler-raspberry-pi-chapter-7/?replytocom=652604#respond" onclick="return addComment.moveForm( &quot;div-comment-652604&quot;, &quot;652604&quot;, &quot;respond&quot;, &quot;513&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment odd alt depth-3" id="comment-656258">
				<div id="div-comment-656258" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/2d68fd28a4c511ec72036936bb7220c2?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/2d68fd28a4c511ec72036936bb7220c2?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/2d68fd28a4c511ec72036936bb7220c2?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-2d68fd28a4c511ec72036936bb7220c2-1">			<cite class="fn"><a href="https://blulin.wordpress.com" rel="external nofollow" class="url">blulin</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/26/arm-assembler-raspberry-pi-chapter-7/#comment-656258">
			November 23, 2014 at 8:09 pm</a>		</div>

		<p>Thanks for the explanation. I was going through the armv6 architecture manual and wondering why it’s not listed there as an instruction, if it.Now this explains why. </p>
<p>And I almost forgot, thanks for writing up this great series. Thoroughly enjoying this series and began programming on my old samsumg galaxy 3 armv6 android phone running debian gnu/linux <img draggable="false" class="emoji" alt="🙂" src="https://s.w.org/images/core/emoji/2.2.1/svg/1f642.svg" scale="0"></p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/26/arm-assembler-raspberry-pi-chapter-7/?replytocom=656258#respond" onclick="return addComment.moveForm( &quot;div-comment-656258&quot;, &quot;656258&quot;, &quot;respond&quot;, &quot;513&quot; )" aria-label="Reply to blulin">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1 parent" id="comment-987829">
				<div id="div-comment-987829" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/6b156eb2c12642becc9eeb850393ea1f?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/6b156eb2c12642becc9eeb850393ea1f?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/6b156eb2c12642becc9eeb850393ea1f?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-6b156eb2c12642becc9eeb850393ea1f-0">			<cite class="fn">Gerald</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/26/arm-assembler-raspberry-pi-chapter-7/#comment-987829">
			March 4, 2017 at 1:03 am</a>		</div>

		<p>Hi, also great thanks for your tutorial.</p>
<p>Some Typos in chapter 7:</p>
<p>Both RDest and Rsource must be registers.<br>
Both RDest and Rsource1 must be registers.</p>
<p>What is this misterious<br>
What is this mysterious</p>
<p>not lost bot pushed<br>
not lost but pushed</p>
<p>mov r3, 4<br>
mov r3, #4</p>
<p>equivalent to r1 &lt;- r1*3<br>
equivalent to r1 &lt;- r2*3</p>
<p>equivalent to r1 &lt;- r1*5<br>
equivalent to r1 &lt;- r2*5</p>
<p>Greetings,<br>
Gerry</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/26/arm-assembler-raspberry-pi-chapter-7/?replytocom=987829#respond" onclick="return addComment.moveForm( &quot;div-comment-987829&quot;, &quot;987829&quot;, &quot;respond&quot;, &quot;513&quot; )" aria-label="Reply to Gerald">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-987881">
				<div id="div-comment-987881" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-2">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/26/arm-assembler-raspberry-pi-chapter-7/#comment-987881">
			March 4, 2017 at 9:26 pm</a>		</div>

		<p>Hi Gerald,</p>
<p>thanks a lot! I updated the text with the fixes.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/26/arm-assembler-raspberry-pi-chapter-7/?replytocom=987881#respond" onclick="return addComment.moveForm( &quot;div-comment-987881&quot;, &quot;987881&quot;, &quot;respond&quot;, &quot;513&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		</ul>
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2013/01/26/arm-assembler-raspberry-pi-chapter-7/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="513" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="88712aed2c"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496597865457"></form>
			</div><!-- #respond -->
	</div>	</div>