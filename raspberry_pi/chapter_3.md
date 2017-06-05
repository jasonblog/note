# Chapter 3


<div class="main">
			<h1>Exploring AArch64 assembler – Chapter 3 </h1>
		<p class="post-meta"><span class="date">October 23, 2016</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2016/10/23/exploring-aarch64-assembler-chapter-3/#comments">7</a></span> </p>
		<p>
In the last chapter we saw that instructions may have register operands and immediate operands. We also mentioned that mixing 32-bit and 64-bit register was not allowed. Today we will talk a bit more about register operands.
</p>
<p><span id="more-3367"></span></p>
<h2>Operators for register operands</h2>
<p>
Many instructions that take a register as the second source operand of an instruction can also apply some extra operation to the value of that source register. This can be used as a way to increase density of computation by requiring less instructions and also to allow some common operations, e.g. conversions, in one of the operands.
</p>
<p>
We can distinguish two kinds of operators here: shifting operators and extending operators.
</p>
<h3>Shifting operators</h3>
<p>
There are three shifting operators in AArch64: LSL, LSR, ASR and ROR. Their syntax is as follows:
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">reg<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span><span style="color: #339933;">,</span> #amount
reg<span style="color: #339933;">,</span> LSR<span style="color: #339933;">,</span> #amount
reg<span style="color: #339933;">,</span> ASR<span style="color: #339933;">,</span> #amount
reg<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">ROR</span><span style="color: #339933;">,</span> #amount</pre></td></tr></tbody></table><p class="theCode" style="display:none;">reg, LSL, #amount
reg, LSR, #amount
reg, ASR, #amount
reg, ROR, #amount</p></div>

<p>
where <code>reg</code> can be a 64-bit register <code>Xn</code> or a 32-bit register <code>Wn</code> and <code>amount</code> is a number whose range depends on the register used and ranges from 0 to 31 for 32-bit registers and from 0 to 63 for 64-bit registers.
</p>
<p>
Operator <code>LSL</code> performs a <strong>l</strong>ogical <strong>s</strong>hift <strong>l</strong>eft to the value in <code>reg</code> (it does not change the contents of <code>reg</code> though). Shifting <em>n</em> bits to the left means introducing <code>n</code> zeros as the least significant bits and discarding <code>n</code> most significant bits from the original value. Shifting left n-bits is equivalent to multiply to 2<sup>n.
</sup></p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">add</span> r1<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r3<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">4</span> <span style="color: #339933;">/*</span> r1 ← r2 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span>r3 &lt;&lt; <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span>
                          this is the same as
                          r1 ← r2 <span style="color: #339933;">+</span> r3 <span style="color: #339933;">*</span> <span style="color: #ff0000;">16</span> <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">add</span> r0<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span>  <span style="color: #339933;">/*</span> r0  ← r0 <span style="color: #339933;">+</span> r0 &lt;&lt; <span style="color: #ff0000;">2</span>
                           this is the same as
                           r0 ← r0 <span style="color: #339933;">+</span> r0 <span style="color: #339933;">*</span> <span style="color: #ff0000;">4</span>
                           which happens to be the same as
                           r0 ← r0 <span style="color: #339933;">*</span> <span style="color: #ff0000;">5</span>
                           assuming no overflow happens
                         <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">add r1, r2, r3, LSL #4 /* r1 ← r2 + (r3 &lt;&lt; 4)
                          this is the same as
                          r1 ← r2 + r3 * 16 */
add r0, r0, r0, LSL #2  /* r0  ← r0 + r0 &lt;&lt; 2
                           this is the same as
                           r0 ← r0 + r0 * 4
                           which happens to be the same as
                           r0 ← r0 * 5
                           assuming no overflow happens
                         */</p></div>

<p>
Operator LSR performs a <strong>l</strong>ogical <strong>s</strong>hift <strong>r</strong>ight. This operation is the dual of LSL, but zeros are introduced in the <em>n</em> most significant bits and the <em>n</em> least significant bits are discarded. For unsigned arithmetic numbers, this operation is equivalent to division by 2<sup>n</sup>.
</p>
<p>
Operator ASR performs an <strong>a</strong>rithmetic <strong>s</strong>hift <strong>r</strong>ight. This is like LSL but instead of introducing zeros in the <em>n</em> most significant bits the most significant bit is replicated <em>n</em> times in the <em>n</em> most significant bits. As in LSL, the <em>n</em> least significant bits are discarded. If the most significant bit of the register is zero, ASR is equivalent to LSR. This shift operator is useful for two’s complement numbers as it propagates the sign bit (which would be the most significant bit in the register if interpreted as a binary number) and it can be used for dividing by 2<sup>n</sup> negative numbers as well. A LSR on a two’s complement negative number does not make sense for the purpose of a division.
</p>
<p>
Operator ROR performs a <strong>ro</strong>tate <strong>r</strong>ight of the register. This is commonly used for cryptography and its usage is less usual than the other shifting operands. A rotation is similar to LSR but rather than dropping bits and introducing zeros, the least signficant bits that would be dropped are introduced as the most significant bits. There is no rotate left because a rotate right can be used for this: just rotate all bits minus the number of steps we want to rotate to the left.
</p>
<p>
In AArch64 only a few instructions (mainly logical ones) can use the ROR shifting operator.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">mov</span> w2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0x1234</span>            <span style="color: #339933;">//</span> w2 ← <span style="color: #ff0000;">0x1234</span>
<span style="color: #00007f; font-weight: bold;">mov</span> w1<span style="color: #339933;">,</span> wzr                <span style="color: #339933;">//</span> w1 ← <span style="color: #ff0000;">0</span>
orr w0<span style="color: #339933;">,</span> w1<span style="color: #339933;">,</span> w2<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">ROR</span> #<span style="color: #ff0000;">4</span>     <span style="color: #339933;">//</span> w0 ← BitwiseOr<span style="color: #009900; font-weight: bold;">(</span>w1<span style="color: #339933;">,</span> RotateRight<span style="color: #009900; font-weight: bold;">(</span>w2<span style="color: #339933;">,</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #009900; font-weight: bold;">)</span>
                           <span style="color: #339933;">//</span> this <span style="color: #00007f; font-weight: bold;">sets</span> w0 to <span style="color: #ff0000;">0x40000123</span>
orr w0<span style="color: #339933;">,</span> w1<span style="color: #339933;">,</span> w2<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">ROR</span> #<span style="color: #ff0000;">28</span>    <span style="color: #339933;">//</span> w0 ← BitwiseOr<span style="color: #009900; font-weight: bold;">(</span>w1<span style="color: #339933;">,</span> RotateRight<span style="color: #009900; font-weight: bold;">(</span>w2<span style="color: #339933;">,</span> <span style="color: #ff0000;">32</span><span style="color: #339933;">-</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #009900; font-weight: bold;">)</span>
                           <span style="color: #339933;">//</span> this is <span style="color: #00007f; font-weight: bold;">in</span> practice like RotateLeft<span style="color: #009900; font-weight: bold;">(</span>w2<span style="color: #339933;">,</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span>
                           <span style="color: #339933;">//</span> so this <span style="color: #00007f; font-weight: bold;">sets</span> w0 to <span style="color: #ff0000;">0x12340</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">mov w2, #0x1234            // w2 ← 0x1234
mov w1, wzr                // w1 ← 0
orr w0, w1, w2, ROR #4     // w0 ← BitwiseOr(w1, RotateRight(w2, 4))
                           // this sets w0 to 0x40000123
orr w0, w1, w2, ROR #28    // w0 ← BitwiseOr(w1, RotateRight(w2, 32-4))
                           // this is in practice like RotateLeft(w2, 4)
                           // so this sets w0 to 0x12340</p></div>

<h3>Extending operators</h3>
<p>
Extending operators main purpose is to widen a narrower value found in a register to match the number of bits for the operation. An extending operator is of the form <em>k</em><code>xt</code><em>w</em>, where <em>k</em> is the kind of integer we want to widen and <em>w</em> is the width of the narrow value. For the former, the kind of integer can be <code>U</code> (unsigned) or <code>S</code> (signed, i.e. two’s complement). For the latter the width can be <code>B</code>, <code>H</code> or <code>W</code> which means respectively <strong>b</strong>yte (least 8 significant bits of the register), <strong>h</strong>alf-word (least 16 significant bits of the register) or <strong>w</strong>ord (least significant 32 bits of the register).
</p>
<p>
This means that the extending operators are <code>uxtb</code>, <code>sxtb</code>, <code>uxth</code>, <code>sxth</code>, <code>uxtw</code>, <code>sxtw</code>.
</p>
<p>
These operators exist because sometimes we have to <em>lift</em> the range of the source value from a smaller bit width to a bigger one. In later chapters we will see many cases where this happens. For instance, it may happen that we need to add a 32-bit register to a 64-bit register. If both registers represent two’s complement integers then
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">add</span> x0<span style="color: #339933;">,</span> x1<span style="color: #339933;">,</span> w2<span style="color: #339933;">,</span> sxtw  <span style="color: #339933;">//</span> x0 ← x1 <span style="color: #339933;">+</span> ExtendSigned32To64<span style="color: #009900; font-weight: bold;">(</span>w2<span style="color: #009900; font-weight: bold;">)</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">add x0, x1, w2, sxtw  // x0 ← x1 + ExtendSigned32To64(w2)</p></div>

<p>
There is some kind of context that has to be taken into account when using these extension operators. For instance, the two instructions below have slight different meanings:
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">add</span> x0<span style="color: #339933;">,</span> x1<span style="color: #339933;">,</span> w2<span style="color: #339933;">,</span> sxtb <span style="color: #339933;">//</span> x0 ← x1 <span style="color: #339933;">+</span> ExtendSigned8To64<span style="color: #009900; font-weight: bold;">(</span>w2<span style="color: #009900; font-weight: bold;">)</span>
<span style="color: #00007f; font-weight: bold;">add</span> w0<span style="color: #339933;">,</span> w1<span style="color: #339933;">,</span> w2<span style="color: #339933;">,</span> sxtb <span style="color: #339933;">//</span> w0 ← w1 <span style="color: #339933;">+</span> ExtendSigned8To32<span style="color: #009900; font-weight: bold;">(</span>w2<span style="color: #009900; font-weight: bold;">)</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">add x0, x1, w2, sxtb // x0 ← x1 + ExtendSigned8To64(w2)
add w0, w1, w2, sxtb // w0 ← w1 + ExtendSigned8To32(w2)</p></div>

<p>
In both cases the least significant 8 bits of <code>w2</code> are extended but in the first case they are extended to 64 bit and in the second case to 32-bit.
</p>
<h4>Extension and shift</h4>
<p>
It is possible to extend a value and then shift it left 1, 2, 3 or 4 bits by specifying an amount after the extension operator. For instance</p>
<p>
</p><div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">mov</span> x0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>                <span style="color: #339933;">//</span> x0 ← <span style="color: #ff0000;">0</span>
<span style="color: #00007f; font-weight: bold;">mov</span> x1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0x1234</span>           <span style="color: #339933;">//</span> x0 ← <span style="color: #ff0000;">0x1234</span>
<span style="color: #00007f; font-weight: bold;">add</span> x2<span style="color: #339933;">,</span> x0<span style="color: #339933;">,</span> x1<span style="color: #339933;">,</span> sxtw #<span style="color: #ff0000;">1</span>   <span style="color: #339933;">//</span> x2 ← x0 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span>ExtendSigned16To64<span style="color: #009900; font-weight: bold;">(</span>x1<span style="color: #009900; font-weight: bold;">)</span> &lt;&lt; <span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">)</span>
                          <span style="color: #339933;">//</span> this <span style="color: #00007f; font-weight: bold;">sets</span> x2 to <span style="color: #ff0000;">0x2468</span>
<span style="color: #00007f; font-weight: bold;">add</span> x2<span style="color: #339933;">,</span> x0<span style="color: #339933;">,</span> x1<span style="color: #339933;">,</span> sxtw #<span style="color: #ff0000;">2</span>   <span style="color: #339933;">//</span> x2 ← x0 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span>ExtendSigned16To64<span style="color: #009900; font-weight: bold;">(</span>x1<span style="color: #009900; font-weight: bold;">)</span> &lt;&lt; <span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">)</span>
                          <span style="color: #339933;">//</span> this <span style="color: #00007f; font-weight: bold;">sets</span> x2 to <span style="color: #ff0000;">0x48d0</span>
<span style="color: #00007f; font-weight: bold;">add</span> x2<span style="color: #339933;">,</span> x0<span style="color: #339933;">,</span> x1<span style="color: #339933;">,</span> sxtw #<span style="color: #ff0000;">3</span>   <span style="color: #339933;">//</span> x2 ← x0 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span>ExtendSigned16To64<span style="color: #009900; font-weight: bold;">(</span>x1<span style="color: #009900; font-weight: bold;">)</span> &lt;&lt; <span style="color: #ff0000;">3</span><span style="color: #009900; font-weight: bold;">)</span>
                          <span style="color: #339933;">//</span> this <span style="color: #00007f; font-weight: bold;">sets</span> x2 to <span style="color: #ff0000;">0x91a0</span>
<span style="color: #00007f; font-weight: bold;">add</span> x2<span style="color: #339933;">,</span> x0<span style="color: #339933;">,</span> x1<span style="color: #339933;">,</span> sxtw #<span style="color: #ff0000;">4</span>   <span style="color: #339933;">//</span> x2 ← x0 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span>ExtendSigned16To64<span style="color: #009900; font-weight: bold;">(</span>x1<span style="color: #009900; font-weight: bold;">)</span> &lt;&lt; <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span>
                          <span style="color: #339933;">//</span> this <span style="color: #00007f; font-weight: bold;">sets</span> x2 to <span style="color: #ff0000;">0x12340</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">mov x0, #0                // x0 ← 0
mov x1, #0x1234           // x0 ← 0x1234
add x2, x0, x1, sxtw #1   // x2 ← x0 + (ExtendSigned16To64(x1) &lt;&lt; 1)
                          // this sets x2 to 0x2468
add x2, x0, x1, sxtw #2   // x2 ← x0 + (ExtendSigned16To64(x1) &lt;&lt; 2)
                          // this sets x2 to 0x48d0
add x2, x0, x1, sxtw #3   // x2 ← x0 + (ExtendSigned16To64(x1) &lt;&lt; 3)
                          // this sets x2 to 0x91a0
add x2, x0, x1, sxtw #4   // x2 ← x0 + (ExtendSigned16To64(x1) &lt;&lt; 4)
                          // this sets x2 to 0x12340</p></div>

<p>
This may seem a bit odd and arbitrary at this point but in later chapters we will see that this is actually useful in many cases.
</p>
<p>
This is all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2016/10/23/exploring-aarch64-assembler-chapter-3/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2016/10/23/exploring-aarch64-assembler-chapter-3/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2016/10/23/exploring-aarch64-assembler-chapter-3/&amp;text=Exploring+AArch64+assembler+%E2%80%93+Chapter+3+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2016/10/23/exploring-aarch64-assembler-chapter-3/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>						<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter-2/" rel="prev">Exploring AArch64 assembler – Chapter 2</a></span>
			<span class="next"><a href="http://thinkingeek.com/2016/10/23/exploring-aarch64-assembler-chapter-4/" rel="next">Exploring AArch64 assembler – Chapter 4</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
			<h3>7 thoughts on “<span>Exploring AArch64 assembler – Chapter 3</span>”</h3>
		<ul class="commentlist">
				<li class="comment even thread-even depth-1 parent" id="comment-988231">
				<div id="div-comment-988231" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/3e8f190363cf6f1f7d4813c8e8b8c9e9?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/3e8f190363cf6f1f7d4813c8e8b8c9e9?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/3e8f190363cf6f1f7d4813c8e8b8c9e9?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-3e8f190363cf6f1f7d4813c8e8b8c9e9-0">			<cite class="fn">David Todd</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/23/exploring-aarch64-assembler-chapter-3/#comment-988231">
			March 9, 2017 at 5:47 pm</a>		</div>

		<p>I believe that for your second and third “add” instructions, you meant to have “sxtw #2” and “sxtw #3” as the extension operations (in order to get the results your describe in your comments).</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/23/exploring-aarch64-assembler-chapter-3/?replytocom=988231#respond" onclick="return addComment.moveForm( &quot;div-comment-988231&quot;, &quot;988231&quot;, &quot;respond&quot;, &quot;3367&quot; )" aria-label="Reply to David Todd">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-988441">
				<div id="div-comment-988441" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-0">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/23/exploring-aarch64-assembler-chapter-3/#comment-988441">
			March 12, 2017 at 1:23 pm</a>		</div>

		<p>Hi David, thanks! I already fixed the post.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/23/exploring-aarch64-assembler-chapter-3/?replytocom=988441#respond" onclick="return addComment.moveForm( &quot;div-comment-988441&quot;, &quot;988441&quot;, &quot;respond&quot;, &quot;3367&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-988242">
				<div id="div-comment-988242" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/3e8f190363cf6f1f7d4813c8e8b8c9e9?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/3e8f190363cf6f1f7d4813c8e8b8c9e9?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/3e8f190363cf6f1f7d4813c8e8b8c9e9?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-3e8f190363cf6f1f7d4813c8e8b8c9e9-1">			<cite class="fn">David Todd</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/23/exploring-aarch64-assembler-chapter-3/#comment-988242">
			March 9, 2017 at 8:23 pm</a>		</div>

		<p>Roger, I thought it odd that you moved 0 into x0 for your extension and shift examples.  Why not just<br>
l2:	mov x1, #0x1234            // x1 ← 0x1234<br>
l3:	add x2, xzr, x1, sxtw #1   // x2 ← 0 + (ExtendSigned16To64(x1) &lt;&lt; 1)</p>
<p>That is, use the xzr register (always reads 0) as the first source operand.  But the gcc compiler [(SUSE Linux) 6.2.1 20161209 ] gives an error message "extst2.s:6: Error: extending shift is not permitted at operand 3 — `add x2,xzr,x1,sxtw#1'"  for each of those four lines.</p>
<p>I can't think of any reason why that shouldn't be an allowed operation.  Is that a compiler (assembler) error, or is there a reason why using "xzr" as the first source prevents use of extending shift of the second source operand?</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/23/exploring-aarch64-assembler-chapter-3/?replytocom=988242#respond" onclick="return addComment.moveForm( &quot;div-comment-988242&quot;, &quot;988242&quot;, &quot;respond&quot;, &quot;3367&quot; )" aria-label="Reply to David Todd">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-988443">
				<div id="div-comment-988443" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-1">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/23/exploring-aarch64-assembler-chapter-3/#comment-988443">
			March 12, 2017 at 1:39 pm</a>		</div>

		<p>Hi David, </p>
<p>if you check the <a href="https://developer.arm.com/docs/dui0801/latest/a64-general-instructions/add-extended-register" rel="nofollow">documentation of ADD</a> you will see that it reads as <code>Xn|SP</code> or <code>Wn|WSP</code>. This syntax means that XZR or WZR cannot be used in these contexts.</p>
<p>In fact, attempting to do so, like you do in the other comment actually means using the WSP, XSP which are other registers (whose meaning I have not explained but I can advance you that they mean the stack pointer <img draggable="false" class="emoji" alt="🙂" src="https://s.w.org/images/core/emoji/2.2.1/svg/1f642.svg" scale="0"> ).</p>
<p>Kind regards,<br>
Roger</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/23/exploring-aarch64-assembler-chapter-3/?replytocom=988443#respond" onclick="return addComment.moveForm( &quot;div-comment-988443&quot;, &quot;988443&quot;, &quot;respond&quot;, &quot;3367&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1 parent" id="comment-988257">
				<div id="div-comment-988257" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/3e8f190363cf6f1f7d4813c8e8b8c9e9?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/3e8f190363cf6f1f7d4813c8e8b8c9e9?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/3e8f190363cf6f1f7d4813c8e8b8c9e9?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-3e8f190363cf6f1f7d4813c8e8b8c9e9-2">			<cite class="fn">David Todd</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/23/exploring-aarch64-assembler-chapter-3/#comment-988257">
			March 9, 2017 at 11:46 pm</a>		</div>

		<p>Roger (and anyone following this): it is an assembler error.<br>
If I code the instruction manually into a 32-bit .word,<br>
“add x2,xzr,x1,sxtw #1” executes as expected, storing into x2 the value in x1 shifted by the requested number of bits.</p>
<p>Anyone wanting to try it, the instruction is “.word  0x8b21c7e2         // add x2,xzr,x1,sxtw #1”</p>
<p>I would hope this would be fixed in a future release of the assembler.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/23/exploring-aarch64-assembler-chapter-3/?replytocom=988257#respond" onclick="return addComment.moveForm( &quot;div-comment-988257&quot;, &quot;988257&quot;, &quot;respond&quot;, &quot;3367&quot; )" aria-label="Reply to David Todd">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2 parent" id="comment-988444">
				<div id="div-comment-988444" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-2">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/23/exploring-aarch64-assembler-chapter-3/#comment-988444">
			March 12, 2017 at 1:40 pm</a>		</div>

		<p>Hi David,</p>
<p>thanks for your comment. As I mentioned above, not all instructions allow the usage of XZR or WZR. If the syntax says Xn|XSP or Wn|WSP it means that XZR, WZR cannot be used in that operand. The instruction you have encoded is effectively the following one:</p>
<pre>8b21c7e2 	add	x2, sp, w1, sxtw #1
</pre>
<p>This is clearly not what you wanted!</p>
<p>Kind regards,<br>
Roger</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/23/exploring-aarch64-assembler-chapter-3/?replytocom=988444#respond" onclick="return addComment.moveForm( &quot;div-comment-988444&quot;, &quot;988444&quot;, &quot;respond&quot;, &quot;3367&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment even depth-3" id="comment-988479">
				<div id="div-comment-988479" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/3e8f190363cf6f1f7d4813c8e8b8c9e9?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/3e8f190363cf6f1f7d4813c8e8b8c9e9?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/3e8f190363cf6f1f7d4813c8e8b8c9e9?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-3e8f190363cf6f1f7d4813c8e8b8c9e9-3">			<cite class="fn">David Todd</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/23/exploring-aarch64-assembler-chapter-3/#comment-988479">
			March 13, 2017 at 5:01 am</a>		</div>

		<p>Thanks, Roger!  I hadn’t grasped the point that the “Xn|XSP” nomenclature was intended to exclude the XZR register, which I had been thinking of as just another general-purpose regiseter.  That’s a helpful point to keep in mind (learning the hard way, I won’t forget!).  You can use XZR as the second source operand but not as the first.</p>
<p>And again, for those following this, my hand-assembled instruction “add x2,xzr,x1,sxtw #1” is, as Roger analyzed, actually the encoding for “add	x2, sp, w1, sxtw #1”.  As the reference manual points out (ARM DDI 0487A.k<br>
Copyright © 2013-2016 ARM Limited or its affiliates. All rights reserved.  pg C6-437), the “0b1111” encoding of the register field, which I had assumed would reference X31 (the XZR register), references XSP in this context.  What I thought was a correctly executing instruction when I ran it though gdb was actually an instruction that left the correct shifted value from an earlier instruction in X2, the destination register — and I thought it had executed correctly.</p>
<p>Thanks again, Roger, for the tutorials and for the clarifications!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/23/exploring-aarch64-assembler-chapter-3/?replytocom=988479#respond" onclick="return addComment.moveForm( &quot;div-comment-988479&quot;, &quot;988479&quot;, &quot;respond&quot;, &quot;3367&quot; )" aria-label="Reply to David Todd">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		</ul>
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2016/10/23/exploring-aarch64-assembler-chapter-3/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="3367" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="d992b3ce15"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496668819228"></form>
			</div><!-- #respond -->
	</div>	</div>