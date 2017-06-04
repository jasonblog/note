# Registers


<div class="main">
			<h1>ARM assembler in Raspberry Pi – Chapter 2 </h1>
		<p class="post-meta"><span class="date">January 10, 2013</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2013/01/10/arm-assembler-raspberry-pi-chapter-2/#comments">4</a></span> </p>
		<h2>Registers</h2>
<p>
At its core, a processor in a computer is nothing but a powerful calculator. Calculations can only be carried using values stored in very tiny memories called <i>registers</i>. The ARM processor in a Raspberry Pi has 16 integer registers and 32 floating point registers. A processor uses these registers to perform integer computations and floating point computations, respectively. We will put floating registers aside for now and eventually we will get back to them in a future installment. Let’s focus on the integer registers.
</p>
<p><span id="more-341"></span></p>
<p>
Those 16 integer registers in ARM have names from <code>r0</code> to <code>r15</code>. They can hold 32 bits. Of course these 32 bits can encode whatever you want. That said, it is convenient to represent integers in two’s complement as there are instructions which perform computations assuming this encoding. So from now, except noted, we will assume our registers contain integer values encoded in two’s complement.
</p>
<p>
Not all registers from <code>r0</code> to <code>r15</code> are used equally but we will not care about this for now. Just assume what we do is “OK”.
</p>
<h2>Basic arithmetic</h2>
<p>
Almost every processor can do some basic arithmetic computations using the integer registers. So do ARM processors. You can <code>ADD</code> two registers. Let’s retake our example from the first chapter.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>1
2
3
4
5
6
7
8
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> <span style="color: #339933;">--</span> sum01<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">global</span> main
&nbsp;
main<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">3</span>      <span style="color: #339933;">/*</span> r1 ← <span style="color: #ff0000;">3</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>      <span style="color: #339933;">/*</span> r2 ← <span style="color: #ff0000;">4</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r0<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> r2  <span style="color: #339933;">/*</span> r0 ← r1 <span style="color: #339933;">+</span> r2 <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* -- sum01.s */
.global main

main:
    mov r1, #3      /* r1 ← 3 */
    mov r2, #4      /* r2 ← 4 */
    add r0, r1, r2  /* r0 ← r1 + r2 */
    bx lr</p></div>

<p>
If we compile and run this program, the error code is, as expected, 7.
</p>
<pre>$ ./sum01 ; echo $?
7
</pre>
<p>
Nothing prevents us to reuse <code>r0</code> in a more clever way.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>1
2
3
4
5
6
7
8
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> <span style="color: #339933;">--</span> sum02<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">global</span> main
&nbsp;
main<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">3</span>      <span style="color: #339933;">/*</span> r0 ← <span style="color: #ff0000;">3</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>      <span style="color: #339933;">/*</span> r1 ← <span style="color: #ff0000;">4</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r0<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> r1  <span style="color: #339933;">/*</span> r0 ← r0 <span style="color: #339933;">+</span> r1 <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* -- sum02.s */
.global main

main:
    mov r0, #3      /* r0 ← 3 */
    mov r1, #4      /* r1 ← 4 */
    add r0, r0, r1  /* r0 ← r0 + r1 */
    bx lr</p></div>

<p>
Which behaves as expected.
</p>
<pre>$ ./sum02 ; echo $?
7
</pre>
<p>
That’s all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2013/01/10/arm-assembler-raspberry-pi-chapter-2/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2013/01/10/arm-assembler-raspberry-pi-chapter-2/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2013/01/10/arm-assembler-raspberry-pi-chapter-2/&amp;text=ARM+assembler+in+Raspberry+Pi+%E2%80%93+Chapter+2+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2013/01/10/arm-assembler-raspberry-pi-chapter-2/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>				<p class="tags"><span><a href="http://thinkingeek.com/tag/arm/" rel="tag">arm</a>, <a href="http://thinkingeek.com/tag/assembler/" rel="tag">assembler</a>, <a href="http://thinkingeek.com/tag/pi/" rel="tag">pi</a>, <a href="http://thinkingeek.com/tag/raspberry/" rel="tag">raspberry</a></span></p>		<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/" rel="prev">ARM assembler in Raspberry Pi – Chapter 1</a></span>
			<span class="next"><a href="http://thinkingeek.com/2013/01/11/arm-assembler-raspberry-pi-chapter-3/" rel="next">ARM assembler in Raspberry Pi – Chapter 3</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
			<h3>4 thoughts on “<span>ARM assembler in Raspberry Pi – Chapter 2</span>”</h3>
		<ul class="commentlist">
				<li class="comment even thread-even depth-1" id="comment-1418">
				<div id="div-comment-1418" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a16ae1e69be669d6f2c5fa0972e39c8a?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a16ae1e69be669d6f2c5fa0972e39c8a?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-a16ae1e69be669d6f2c5fa0972e39c8a-0" originals="32" src-orig="http://1.gravatar.com/avatar/a16ae1e69be669d6f2c5fa0972e39c8a?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn"><a href="http://digilander.libero.it/zantaz/" rel="external nofollow" class="url">Mariani Antonio Mario</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/10/arm-assembler-raspberry-pi-chapter-2/#comment-1418">
			June 11, 2013 at 7:28 pm</a>		</div>

		<p>smart guide … go ahead …</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/10/arm-assembler-raspberry-pi-chapter-2/?replytocom=1418#respond" onclick="return addComment.moveForm( &quot;div-comment-1418&quot;, &quot;1418&quot;, &quot;respond&quot;, &quot;341&quot; )" aria-label="Reply to Mariani Antonio Mario">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1 parent" id="comment-183511">
				<div id="div-comment-183511" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/0654eb81a0f139a5e30f2d400c10a796?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/0654eb81a0f139a5e30f2d400c10a796?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-0654eb81a0f139a5e30f2d400c10a796-0" originals="32" src-orig="http://0.gravatar.com/avatar/0654eb81a0f139a5e30f2d400c10a796?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Andreas Gwilt</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/10/arm-assembler-raspberry-pi-chapter-2/#comment-183511">
			July 18, 2014 at 2:25 pm</a>		</div>

		<p>Can’t you just say <code>add r0, r1</code> ?</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/10/arm-assembler-raspberry-pi-chapter-2/?replytocom=183511#respond" onclick="return addComment.moveForm( &quot;div-comment-183511&quot;, &quot;183511&quot;, &quot;respond&quot;, &quot;341&quot; )" aria-label="Reply to Andreas Gwilt">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2" id="comment-183530">
				<div id="div-comment-183530" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-a779b8290b1ca104fdf84d8016fd010b-0" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/10/arm-assembler-raspberry-pi-chapter-2/#comment-183530">
			July 18, 2014 at 2:57 pm</a>		</div>

		<p>Yes, although I think would be less clear for novices.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/10/arm-assembler-raspberry-pi-chapter-2/?replytocom=183530#respond" onclick="return addComment.moveForm( &quot;div-comment-183530&quot;, &quot;183530&quot;, &quot;respond&quot;, &quot;341&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-even depth-1" id="comment-657258">
				<div id="div-comment-657258" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/b1673470080cb742ea9239ad8ea6d4cc?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/b1673470080cb742ea9239ad8ea6d4cc?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-b1673470080cb742ea9239ad8ea6d4cc-0" originals="32" src-orig="http://2.gravatar.com/avatar/b1673470080cb742ea9239ad8ea6d4cc?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn"><a href="http://newtonsaber.com/blog" rel="external nofollow" class="url">SaberWriter</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/10/arm-assembler-raspberry-pi-chapter-2/#comment-657258">
			November 24, 2014 at 1:28 am</a>		</div>

		<p>This continues to be a great series of articles.  Thanks again for taking the time to write them. Great stuff.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/10/arm-assembler-raspberry-pi-chapter-2/?replytocom=657258#respond" onclick="return addComment.moveForm( &quot;div-comment-657258&quot;, &quot;657258&quot;, &quot;respond&quot;, &quot;341&quot; )" aria-label="Reply to SaberWriter">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		</ul>
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2013/01/10/arm-assembler-raspberry-pi-chapter-2/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="341" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="078f624c0c"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496592285433"></form>
			</div><!-- #respond -->
	</div>	</div>