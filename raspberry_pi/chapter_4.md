# Chapter 4


<div class="main">
			<h1>Exploring AArch64 assembler – Chapter 4 </h1>
		<p class="post-meta"><span class="date">October 23, 2016</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2016/10/23/exploring-aarch64-assembler-chapter-4/#respond">0</a></span> </p>
		<p>
In this chapter we will see some instructions that will allow us to compute things.
</p>
<p><span id="more-3380"></span></p>
<h2>Arithmetic instructions</h2>
<p>
Since a computer is nothing but a glorified calculator (or a calculator it is nothing but a tiny computer), it has to be possible to perform basic arithmetic. For now we will restrict to integer operations. In later chapters we will see how we can manipulate other kinds of numbers.
</p>
<h3>Addition and subtraction</h3>
<p>
We can perform addition and subtraction using <code>add</code> and <code>sub</code> instructions. These instructions are pretty flexible in that they allow using many forms.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">add</span> Rdest<span style="color: #339933;">,</span> Rsource1<span style="color: #339933;">,</span> #immediate          <span style="color: #339933;">//</span> Rdest ← Rsource1 <span style="color: #339933;">+</span> immediate
<span style="color: #00007f; font-weight: bold;">add</span> Rdest<span style="color: #339933;">,</span> Rsource1<span style="color: #339933;">,</span> Rsource2            <span style="color: #339933;">//</span> Rdest ← Rsource1 <span style="color: #339933;">+</span> Rsource2
<span style="color: #00007f; font-weight: bold;">add</span> Xdest<span style="color: #339933;">,</span> Xsource1<span style="color: #339933;">,</span> Xsource2<span style="color: #339933;">,</span> shiftop   <span style="color: #339933;">//</span> Xdest ← Xsource1 <span style="color: #339933;">+</span> shiftop<span style="color: #009900; font-weight: bold;">(</span>Xsource2<span style="color: #009900; font-weight: bold;">)</span>
<span style="color: #00007f; font-weight: bold;">add</span> Xdest<span style="color: #339933;">,</span> Xsource1<span style="color: #339933;">,</span> Xsource2<span style="color: #339933;">,</span> extop     <span style="color: #339933;">//</span> Xdest ← Xsource1 <span style="color: #339933;">+</span> extop<span style="color: #009900; font-weight: bold;">(</span>Xsource2<span style="color: #009900; font-weight: bold;">)</span>
<span style="color: #00007f; font-weight: bold;">add</span> Wdest<span style="color: #339933;">,</span> Wsource1<span style="color: #339933;">,</span> Wsource2<span style="color: #339933;">,</span> shiftop   <span style="color: #339933;">//</span> Wdest ← Wsource1 <span style="color: #339933;">+</span> shiftop<span style="color: #009900; font-weight: bold;">(</span>Wsource2<span style="color: #009900; font-weight: bold;">)</span>
<span style="color: #00007f; font-weight: bold;">add</span> Wdest<span style="color: #339933;">,</span> Wsource1<span style="color: #339933;">,</span> Wsource2<span style="color: #339933;">,</span> extop     <span style="color: #339933;">//</span> Wdest ← Wsource1 <span style="color: #339933;">+</span> extop<span style="color: #009900; font-weight: bold;">(</span>Wsource2<span style="color: #009900; font-weight: bold;">)</span>
<span style="color: #00007f; font-weight: bold;">add</span> Xdest<span style="color: #339933;">,</span> Xsource1<span style="color: #339933;">,</span> Wsource2<span style="color: #339933;">,</span> extop     <span style="color: #339933;">//</span> Xdest ← Xsource1 <span style="color: #339933;">+</span> extop<span style="color: #009900; font-weight: bold;">(</span>Wsource2<span style="color: #009900; font-weight: bold;">)</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">add Rdest, Rsource1, #immediate          // Rdest ← Rsource1 + immediate
add Rdest, Rsource1, Rsource2            // Rdest ← Rsource1 + Rsource2
add Xdest, Xsource1, Xsource2, shiftop   // Xdest ← Xsource1 + shiftop(Xsource2)
add Xdest, Xsource1, Xsource2, extop     // Xdest ← Xsource1 + extop(Xsource2)
add Wdest, Wsource1, Wsource2, shiftop   // Wdest ← Wsource1 + shiftop(Wsource2)
add Wdest, Wsource1, Wsource2, extop     // Wdest ← Wsource1 + extop(Wsource2)
add Xdest, Xsource1, Wsource2, extop     // Xdest ← Xsource1 + extop(Wsource2)</p></div>

<p>
In the forms above <code>Rx</code> means either <code>Xx</code> or <code>Wx</code> (but without mixing them in the same instruction), <code>shiftop</code> and <code>extop</code> are the shift and extension operands described in chapter 3. In this case, <code>shiftop</code> does not include <code>ROR</code>. All the forms shown for <code>add</code> can be used for <code>sub</code> as well.
</p>
<h3>Multiplication and division</h3>
<p>
Compared to addition and subtraction, multiplication and division are harder operations. And there are a few different instructions for this purpose.
</p>
<p>
Due to the nature of multiplication, multiplying two values of 32/64 bits may end requiring 64/128 bits to be able to fully encode the mathematical result. If we know this will not happen (i.e. the result of the value can be encoded in 32/64 bits) or we do not care we can use the <code>mul</code> instruction. If there are excess bits, they will be dropped.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">mul</span> Rdest<span style="color: #339933;">,</span> Rsource1<span style="color: #339933;">,</span> Rsource2 <span style="color: #339933;">//</span> Rdest ← Rsource1 <span style="color: #339933;">*</span> Rsource2
                              <span style="color: #339933;">//</span> but be careful with overflow</pre></td></tr></tbody></table><p class="theCode" style="display:none;">mul Rdest, Rsource1, Rsource2 // Rdest ← Rsource1 * Rsource2
                              // but be careful with overflow</p></div>

<p>
If we do care about the excess bits, then we have a bunch of instructions we can use. For 32-bit multiplications, <code>umull</code> and <code>smull</code> can be used. The latter is required when multiplying numbers in two’s complement so the sign bit is correctly handled.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">umull Xdest<span style="color: #339933;">,</span> Wsource1<span style="color: #339933;">,</span> Wsource2  <span style="color: #339933;">//</span> Xdest ← Wsource1 <span style="color: #339933;">*</span> Wsource2
smull Xdest<span style="color: #339933;">,</span> Wsource1<span style="color: #339933;">,</span> Wsource2  <span style="color: #339933;">//</span> Xdest ← Wsource1 <span style="color: #339933;">*</span> Wsource2 
                                 <span style="color: #339933;">//</span> for two<span style="color: #7f007f;">'s complement numbers
                                 // in Wsource1 and Wsource2</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">umull Xdest, Wsource1, Wsource2  // Xdest ← Wsource1 * Wsource2
smull Xdest, Wsource1, Wsource2  // Xdest ← Wsource1 * Wsource2 
                                 // for two's complement numbers
                                 // in Wsource1 and Wsource2</p></div>

<p>
For the less common case where we multiply two 64-bit registers and still we care about the upper 64-bits of the 128-bit result, then we can use <code>umulh</code> and <code>smulh</code>. For this case we will need two 64-bit registers (named <code>Xlower</code> and <code>Xupper</code> in the example below).
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">mul</span> Xlower<span style="color: #339933;">,</span> Xsource1<span style="color: #339933;">,</span> Xsource2   <span style="color: #339933;">//</span> Xlower ← Lower64Bits<span style="color: #009900; font-weight: bold;">(</span>Xsource1 <span style="color: #339933;">*</span> Xsource2<span style="color: #009900; font-weight: bold;">)</span>
smulh Xupper<span style="color: #339933;">,</span> Xsource1<span style="color: #339933;">,</span> Xsource2 <span style="color: #339933;">//</span> Xupper ← Upper64Bits<span style="color: #009900; font-weight: bold;">(</span>Xsource1 <span style="color: #339933;">*</span> Xsource2<span style="color: #009900; font-weight: bold;">)</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">mul Xlower, Xsource1, Xsource2   // Xlower ← Lower64Bits(Xsource1 * Xsource2)
smulh Xupper, Xsource1, Xsource2 // Xupper ← Upper64Bits(Xsource1 * Xsource2)</p></div>

<p>
Division is a bit simpler as only two instructions are necessary: <code>udiv</code> and <code>sdiv</code>. Again, the latter is for integer numbers encoded in two’s complement.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">udiv Rdest<span style="color: #339933;">,</span> Rsource1<span style="color: #339933;">,</span> Rsource2  <span style="color: #339933;">//</span> Rdest ← Rsource1 <span style="color: #339933;">/</span> Rsource2
sdiv Rdest<span style="color: #339933;">,</span> Rsource1<span style="color: #339933;">,</span> Rsource2  <span style="color: #339933;">//</span> Rdest ← Rsource1 <span style="color: #339933;">/</span> Rsource2
                                <span style="color: #339933;">//</span> when Rsource1<span style="color: #339933;">,</span> Rsource2 are
                                <span style="color: #339933;">//</span> <span style="color: #00007f; font-weight: bold;">in</span> two<span style="color: #7f007f;">'s complement</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">udiv Rdest, Rsource1, Rsource2  // Rdest ← Rsource1 / Rsource2
sdiv Rdest, Rsource1, Rsource2  // Rdest ← Rsource1 / Rsource2
                                // when Rsource1, Rsource2 are
                                // in two's complement</p></div>

<p>
These two instructions will compute the quotient of the division rounded towards zero.
</p>
<h2>Bitwise instructions</h2>
<p>
Bitwise instructions directly operate in the bits of the registers, without assuming any encoding in them.
</p>
<p>
Instruction <code>mvn</code> performs a <a href="https://en.wikipedia.org/wiki/Bitwise_operation#NOT">bitwise not</a> on its operand.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">mvn Rdest<span style="color: #339933;">,</span> Rsource    <span style="color: #339933;">//</span> Rdest ← ~Rsource</pre></td></tr></tbody></table><p class="theCode" style="display:none;">mvn Rdest, Rsource    // Rdest ← ~Rsource</p></div>

<p>
Most of the bitwise instructions use two source registers. The basic ones are <code>and</code>, <code>orr</code> and <code>eorr</code> (exclusive orr). They perform a <a href="https://en.wikipedia.org/wiki/Bitwise_operation#AND">bitwise and</a>, a <a href="https://en.wikipedia.org/wiki/Bitwise_operation#OR">bitwise or</a> and a <a href="https://en.wikipedia.org/wiki/Bitwise_operation#XOR">bitwise xor</a> respectively.</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">and</span> Rdest<span style="color: #339933;">,</span> Rsource1<span style="color: #339933;">,</span> #immediate          <span style="color: #339933;">//</span> Rdest ← Rsource1 &amp; immediate
<span style="color: #00007f; font-weight: bold;">and</span> Rdest<span style="color: #339933;">,</span> Rsource1<span style="color: #339933;">,</span> Rsource2            <span style="color: #339933;">//</span> Rdest ← Rsource1 &amp; Rsource2
<span style="color: #00007f; font-weight: bold;">and</span> Xdest<span style="color: #339933;">,</span> Xsource1<span style="color: #339933;">,</span> Xsource2<span style="color: #339933;">,</span> shiftop   <span style="color: #339933;">//</span> Xdest ← Xsource1 &amp; shiftop<span style="color: #009900; font-weight: bold;">(</span>Xsource2<span style="color: #009900; font-weight: bold;">)</span>
<span style="color: #00007f; font-weight: bold;">and</span> Wdest<span style="color: #339933;">,</span> Wsource1<span style="color: #339933;">,</span> Wsource2<span style="color: #339933;">,</span> shiftop   <span style="color: #339933;">//</span> Wdest ← Wsource1 &amp; shiftop<span style="color: #009900; font-weight: bold;">(</span>Wsource2<span style="color: #009900; font-weight: bold;">)</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">and Rdest, Rsource1, #immediate          // Rdest ← Rsource1 &amp; immediate
and Rdest, Rsource1, Rsource2            // Rdest ← Rsource1 &amp; Rsource2
and Xdest, Xsource1, Xsource2, shiftop   // Xdest ← Xsource1 &amp; shiftop(Xsource2)
and Wdest, Wsource1, Wsource2, shiftop   // Wdest ← Wsource1 &amp; shiftop(Wsource2)</p></div>

<p>
Similar forms as the shown above exist for <code>orr</code> and <code>eorr</code>. For bitwise instructions <code>shiftop</code> <strong>does</strong> include <code>ROR</code>.
</p>
<p>
There are combined versions of <code>mvn</code> plus <code>and</code>, <code>orr</code> and <code>eor</code> called <code>bic</code> (bit clear), <code>orn</code> (or not) and <code>eon</code> (exclusive or not) respectively. In this case, the second operand is first applied a NOT operation. They have slightly more limited forms.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">orn Rdest<span style="color: #339933;">,</span> Rsource1<span style="color: #339933;">,</span> Rsource2            <span style="color: #339933;">//</span> Rdest ← Rsource1 | ~Rsource2
orn Xdest<span style="color: #339933;">,</span> Xsource1<span style="color: #339933;">,</span> Xsource2<span style="color: #339933;">,</span> shiftop   <span style="color: #339933;">//</span> Xdest ← Xsource1 | ~shiftop<span style="color: #009900; font-weight: bold;">(</span>Xsource2<span style="color: #009900; font-weight: bold;">)</span>
orn Wdest<span style="color: #339933;">,</span> Wsource1<span style="color: #339933;">,</span> Wsource2<span style="color: #339933;">,</span> shiftop   <span style="color: #339933;">//</span> Wdest ← Wsource1 | ~shiftop<span style="color: #009900; font-weight: bold;">(</span>Wsource2<span style="color: #009900; font-weight: bold;">)</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">orn Rdest, Rsource1, Rsource2            // Rdest ← Rsource1 | ~Rsource2
orn Xdest, Xsource1, Xsource2, shiftop   // Xdest ← Xsource1 | ~shiftop(Xsource2)
orn Wdest, Wsource1, Wsource2, shiftop   // Wdest ← Wsource1 | ~shiftop(Wsource2)</p></div>

<p>
Likewise for <code>bic</code> and <code>eon</code>.
</p>
<p>
There are more instructions with narrower use cases, so we will omit them for now.
</p>
<p>
This is all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2016/10/23/exploring-aarch64-assembler-chapter-4/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2016/10/23/exploring-aarch64-assembler-chapter-4/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2016/10/23/exploring-aarch64-assembler-chapter-4/&amp;text=Exploring+AArch64+assembler+%E2%80%93+Chapter+4+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2016/10/23/exploring-aarch64-assembler-chapter-4/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>						<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2016/10/23/exploring-aarch64-assembler-chapter-3/" rel="prev">Exploring AArch64 assembler – Chapter 3</a></span>
			<span class="next"><a href="http://thinkingeek.com/2016/10/30/arm-assembler-raspberry-pi-chapter-26/" rel="next">ARM assembler in Raspberry Pi – Chapter 26</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2016/10/23/exploring-aarch64-assembler-chapter-4/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="3380" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="3fb8c76975"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496668848295"></form>
			</div><!-- #respond -->
	</div>	</div>