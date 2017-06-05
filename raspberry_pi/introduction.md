# Introduction


<div class="main">
			<h1>ARM assembler in Raspberry Pi – Chapter 1 </h1>
		<p class="post-meta"><span class="date">January 9, 2013</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comments">68</a></span> </p>
		<p>
In my opinion, it is much more beneficial learning a high level language than a specific architecture assembler. But I fancied learning some ARM assembler just for fun since I know some 386 assembler. The idea is not to become a master but understand some of the details of what happens underneath.
</p>
<p><span id="more-334"></span></p>
<h2>Introducing ARM</h2>
<p>
You will see that my explanations do not aim at being very thorough when describing the architecture. I will try to be pragmatic.
</p>
<p>
ARM is a 32-bit architecture that has a simple goal in mind: flexibility. While this is great for integrators (as they have a lot of freedom when designing their hardware) it is not so good for system developers which have to cope with the differences in the ARM hardware. So in this text I will assume that <b>everything is done on a Raspberry Pi Model B running Raspbian</b> (the one with 2 USB ports and 512 MB of RAM).
</p>
<p>
Some parts will be ARM-generic but others will be Raspberry Pi specific. I will not make a distinction. The <a href="http://infocenter.arm.com/">ARM website</a> has a lot of documentation. Use it!
</p>
<h2>Writing assembler</h2>
<p>
Assembler language is just a thin syntax layer on top of the binary code.
</p>
<p>
Binary code is what a computer can run. It is composed of instructions, that are encoded in a binary representation (such encodings are documented in the ARM manuals). You could write binary code encoding instructions but that would be painstaking (besides some other technicalities related to Linux itself that we can happily ignore now).
</p>
<p>
So we will write assembler, ARM assembler. Since the computer cannot run assembler we have to get binary code from it. We use a tool called, well, <i>assembler</i> to <i>assemble</i> the <i>assembler code</i> into a binary code that we can run.
</p>
<p>
The tool to do this is called <code>as</code>. In particular GNU Assembler, which is the assembler tool from the GNU project, sometimes it is also known as <code>gas</code> for this reason. This is the tool we will use to assemble our programs.
</p>
<p>
Just open an editor like <code>vim</code>, <code>nano</code> or <code>emacs</code>. Our assembler language files (called <i>source files</i>) will have a suffix <code>.s</code>. I have no idea why it is <code>.s</code> but this is the usual convention.
</p>
<h2>Our first program</h2>
<p>
We have to start with something, so we will start with a ridiculously simple program which does nothing but return an error code.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>1
2
3
4
5
6
7
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> <span style="color: #339933;">--</span> first<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #339933;">/*</span> This is a comment <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">global</span> main <span style="color: #339933;">/*</span> <span style="color: #7f007f;">'main'</span> is our entry point <span style="color: #00007f; font-weight: bold;">and</span> must be <span style="color: #0000ff; font-weight: bold;">global</span> <span style="color: #339933;">*/</span>
&nbsp;
main<span style="color: #339933;">:</span>          <span style="color: #339933;">/*</span> This is main <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span> <span style="color: #339933;">/*</span> Put a <span style="color: #ff0000;">2</span> inside the register r0 <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr      <span style="color: #339933;">/*</span> Return from main <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* -- first.s */
/* This is a comment */
.global main /* 'main' is our entry point and must be global */

main:          /* This is main */
    mov r0, #2 /* Put a 2 inside the register r0 */
    bx lr      /* Return from main */</p></div>

<p>
Create a file called <code>first.s</code> and write the contents shown above. Save it.
</p>
<p>
To <i>assemble</i> the file type the following command (write what comes after <code>$ </code>).
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>1
</pre></td><td class="code"><pre class="bash" style="font-family:monospace;"><span style="color: #666666;">$ </span><span style="color: #c20cb9; font-weight: bold;">as</span> <span style="color: #660033;">-o</span> first.o first.s</pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ as -o first.o first.s</p></div>

<p>
This will create a <code>first.o</code>. Now link this file to get an executable.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>1
</pre></td><td class="code"><pre class="bash" style="font-family:monospace;"><span style="color: #666666;">$ </span><span style="color: #c20cb9; font-weight: bold;">gcc</span> <span style="color: #660033;">-o</span> first first.o</pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ gcc -o first first.o</p></div>

<p>
If everything goes as expected you will get a <code>first</code> file. This is your program. Run it.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>1
</pre></td><td class="code"><pre class="bash" style="font-family:monospace;"><span style="color: #666666;">$ </span>.<span style="color: #000000; font-weight: bold;">/</span>first</pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ ./first</p></div>

<p>
It should do nothing. Yes, it is a bit disappointing, but it actually does something. Get its error code this time.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>1
2
</pre></td><td class="code"><pre class="bash" style="font-family:monospace;">$ .<span style="color: #000000; font-weight: bold;">/</span>first ; <span style="color: #7a0874; font-weight: bold;">echo</span> <span style="color: #007800;">$?</span>
<span style="color: #000000;">2</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ ./first ; echo $?
2</p></div>

<p>
Great! That error code of 2 is not by chance, it is due to that <code>#2</code> in the assembler code.
</p>
<p>
Since running the assembler and the linker soon becomes boring, I’d recommend you using the following <code>Makefile</code> file instead or a similar one.
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
</pre></td><td class="code"><pre class="make" style="font-family:monospace;"><span style="color: #339900; font-style: italic;"># Makefile</span>
all<span style="color: #004400;">:</span> first
&nbsp;
first<span style="color: #004400;">:</span> first<span style="color: #004400;">.</span>o
	gcc <span style="color: #004400;">-</span>o <span style="color: #000088; font-weight: bold;">$@</span> <span style="color: #000088; font-weight: bold;">$+</span>
&nbsp;
first<span style="color: #004400;">.</span>o <span style="color: #004400;">:</span> first<span style="color: #004400;">.</span>s
	as <span style="color: #004400;">-</span>o <span style="color: #000088; font-weight: bold;">$@</span> <span style="color: #000088; font-weight: bold;">$&lt;</span>
&nbsp;
clean<span style="color: #004400;">:</span>
	rm <span style="color: #004400;">-</span>vf first <span style="color: #004400;">*.</span>o</pre></td></tr></tbody></table><p class="theCode" style="display:none;"># Makefile
all: first

first: first.o
	gcc -o $@ $+

first.o : first.s
	as -o $@ $&lt;

clean:
	rm -vf first *.o</p></div>

<h2>Well, what happened?</h2>
<p>
We cheated a bit just to make things a bit easier. We wrote a C <code>main</code> function in assembler which only does <code>return 2;</code>. This way our program is easier since the C runtime handled initialization and termination of the program for us. I will use this approach all the time.
</p>
<p>
Let’s review every line of our minimal assembler file.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>1
2
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> <span style="color: #339933;">--</span> first<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #339933;">/*</span> This is a comment <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* -- first.s */
/* This is a comment */</p></div>

<p>
These are comments. Comments are enclosed in <code>/*</code> and <code>*/</code>. Use them to document your assembler as they are ignored. As usually, do not nest <code>/*</code> and */ inside <code>/*</code> because it does not work.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>3
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">global</span> main <span style="color: #339933;">/*</span> <span style="color: #7f007f;">'main'</span> is our entry point <span style="color: #00007f; font-weight: bold;">and</span> must be <span style="color: #0000ff; font-weight: bold;">global</span> <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">.global main /* 'main' is our entry point and must be global */</p></div>

<p>
This is a directive for GNU Assembler. A directive tells GNU Assembler to do something special. They start with a dot (<code>.</code>) followed by the name of the directive and some arguments. In this case we are saying that <code>main</code> is a global name. This is needed because the C runtime will call <code>main</code>. If it is not global, it will not be callable by the C runtime and the linking phase will fail.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>5
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">main<span style="color: #339933;">:</span>          <span style="color: #339933;">/*</span> This is main <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">main:          /* This is main */</p></div>

<p>
Every line in GNU Assembler that is not a directive will always be like <code>label: instruction</code>. We can omit <code>label:</code> and <code>instruction</code> (empty and blank lines are ignored). A line with only <code>label:</code>, applies that label to the next line (you can have more than one label referring to the same thing this way). The <code>instruction</code> part is the ARM assembler language itself. In this case we are just defining <code>main</code> as there is no instruction.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>6
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span> <span style="color: #339933;">/*</span> Put a <span style="color: #ff0000;">2</span> inside the register r0 <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">    mov r0, #2 /* Put a 2 inside the register r0 */</p></div>

<p>
Whitespace is ignored at the beginning of the line, but the indentation suggests visually that this instruction belongs to the <code>main</code> function.
</p>
<p>
This is the <code>mov</code> instruction which means <i>move</i>. We move a value <code>2</code> to the register <code>r0</code>. In the next chapter we will see more about registers, do not worry now. Yes, the syntax is awkward because the destination is actually at left. In ARM syntax it is always at left so we are saying something like <i>move to register r0 the immediate value 2</i>. We will see what <i>immediate value</i> means in ARM in the next chapter, do not worry again.
</p>
<p>
In summary, this instruction puts a <code>2</code> inside the register <code>r0</code> (this effectively overwrites whatever register <code>r0</code> may have at that point).
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>7
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">    <span style="color: #46aa03; font-weight: bold;">bx</span> lr      <span style="color: #339933;">/*</span> Return from main <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">    bx lr      /* Return from main */</p></div>

<p>
This instruction <code>bx</code> means <i>branch and exchange</i>. We do not really care at this point about the <i>exchange</i> part. Branching means that we will change the flow of the instruction execution. An ARM processor runs instructions sequentially, one after the other, thus after the <code>mov</code> above, this <code>bx</code> will be run (this sequential execution is not specific to ARM, but what happens in almost all architectures). A branch instruction is used to change this implicit sequential execution. In this case we branch to whatever <code>lr</code> register says. We do not care now what <code>lr</code> contains. It is enough to understand that this instruction just leaves the <code>main</code> function, thus effectively ending our program.
</p>
<p>
And the error code? Well, the result of main is the error code of the program and when leaving the function such result must be stored in the register <code>r0</code>, so the <code>mov</code> instruction performed by our main is actually setting the error code to 2.
</p>
<p>
That’s all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/&amp;text=ARM+assembler+in+Raspberry+Pi+%E2%80%93+Chapter+1+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>				<p class="tags"><span><a href="http://thinkingeek.com/tag/arm/" rel="tag">arm</a>, <a href="http://thinkingeek.com/tag/assembler/" rel="tag">assembler</a>, <a href="http://thinkingeek.com/tag/pi/" rel="tag">pi</a>, <a href="http://thinkingeek.com/tag/raspberry/" rel="tag">raspberry</a></span></p>		<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2012/12/26/fast-easy-block-bots-website-apache/" rel="prev">Fast and easy way to block bots from your website using Apache</a></span>
			<span class="next"><a href="http://thinkingeek.com/2013/01/10/arm-assembler-raspberry-pi-chapter-2/" rel="next">ARM assembler in Raspberry Pi – Chapter 2</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
			<h3>68 thoughts on “<span>ARM assembler in Raspberry Pi – Chapter 1</span>”</h3>
		<ul class="commentlist">
				<li class="comment even thread-even depth-1" id="comment-1047">
				<div id="div-comment-1047" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/fd70b462d1c07adc15e2187aecd12cac?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/fd70b462d1c07adc15e2187aecd12cac?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/fd70b462d1c07adc15e2187aecd12cac?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-fd70b462d1c07adc15e2187aecd12cac-0">			<cite class="fn">Jonathan Hinchliffe</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-1047">
			March 6, 2013 at 10:22 pm</a>		</div>

		<p>Really excellent tutorial.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=1047#respond" onclick="return addComment.moveForm( &quot;div-comment-1047&quot;, &quot;1047&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Jonathan Hinchliffe">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1" id="comment-1082">
				<div id="div-comment-1082" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-9e81362ff9654533accffc53762c9520-0">			<cite class="fn">Fernando</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-1082">
			March 21, 2013 at 12:15 am</a>		</div>

		<p>Nice! I’ve been doing a bit of reading before about registers and general CPU functioning (basically, reading some chapters about ‘Computer design’), and I think that everything makes much more sense with this bit of background.</p>
<p>I can’t stop thinking about how I’d interpreted this same Introduction with any idea – I think it’d sound like magic!</p>
<p>Again, still nice. I’d carry on with the next part!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=1082#respond" onclick="return addComment.moveForm( &quot;div-comment-1082&quot;, &quot;1082&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Fernando">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="pingback even thread-even depth-1" id="comment-1170">
				<div id="div-comment-1170" class="comment-body">
				<div class="comment-author vcard">
						<cite class="fn"><a href="http://ultramachine.wordpress.com/2013/04/11/einstieg-in-pi-assembler/" rel="external nofollow" class="url">Einstieg in Pi-Assembler | ultramachine</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-1170">
			April 11, 2013 at 8:32 pm</a>		</div>

		<p>[…] Artikel zur Assembler-Programmierung im MagPi! Für den weiteren Einstieg in das Thema gibt es bei ThinkGeek noch einen 12-Teiligen kostenfreien Kurs. Yeah, das Wochenende ist damit wohl […]</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=1170#respond" onclick="return addComment.moveForm( &quot;div-comment-1170&quot;, &quot;1170&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Einstieg in Pi-Assembler | ultramachine">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1 parent" id="comment-1269">
				<div id="div-comment-1269" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/64265dc6c0abc810209af9168aabccfa?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/64265dc6c0abc810209af9168aabccfa?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/64265dc6c0abc810209af9168aabccfa?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-64265dc6c0abc810209af9168aabccfa-0">			<cite class="fn">Barret</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-1269">
			May 3, 2013 at 11:06 pm</a>		</div>

		<p>I’m getting this error when I try to assemble the file with<br>
<code>$ as -o first.o first.s</code></p>
<p><code>first.s:7: Error: expecting operand after ','; got nothing<br>
first.s:8: Error: no such instruction: `bx lr'<br>
</code></p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=1269#respond" onclick="return addComment.moveForm( &quot;div-comment-1269&quot;, &quot;1269&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Barret">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2 parent" id="comment-1270">
				<div id="div-comment-1270" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-0">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-1270">
			May 4, 2013 at 9:36 pm</a>		</div>

		<p>Are you running that command on the Raspberry Pi? This is ARM assembler, so you must use an ARM assembler like the one in the Raspberry Pi.</p>
<p>I get the very same error when trying to assemble first.s on my x86_64 machine. It will not work there, obviously.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=1270#respond" onclick="return addComment.moveForm( &quot;div-comment-1270&quot;, &quot;1270&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment odd alt depth-3 parent" id="comment-1271">
				<div id="div-comment-1271" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/64265dc6c0abc810209af9168aabccfa?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/64265dc6c0abc810209af9168aabccfa?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/64265dc6c0abc810209af9168aabccfa?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-64265dc6c0abc810209af9168aabccfa-1">			<cite class="fn">Barret</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-1271">
			May 4, 2013 at 9:38 pm</a>		</div>

		<p>Oh yeah, duh. Is there a way to cross compile code on a x86_64 machine for ARM? I’ve tried Google, but with little concrete instructions.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=1271#respond" onclick="return addComment.moveForm( &quot;div-comment-1271&quot;, &quot;1271&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Barret">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-4 parent" id="comment-1272">
				<div id="div-comment-1272" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-1">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-1272">
			May 4, 2013 at 9:48 pm</a>		</div>

		<p>You can use <a href="http://crosstool-ng.org/" rel="nofollow">crosstol-ng</a>. Last time I checked it was pretty easy to make a cross toolchain for Raspberry Pi. Anyways, you won’t be able to run the code unless you copy it to the Raspberry Pi. I think it is easier to assemble directly there.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=1272#respond" onclick="return addComment.moveForm( &quot;div-comment-1272&quot;, &quot;1272&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment odd alt depth-5" id="comment-2648">
				<div id="div-comment-2648" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/59757ad304a6c095c25c9740d1a8ead3?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/59757ad304a6c095c25c9740d1a8ead3?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/59757ad304a6c095c25c9740d1a8ead3?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-59757ad304a6c095c25c9740d1a8ead3-0">			<cite class="fn">OSCAR PAZ</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-2648">
			December 26, 2013 at 5:03 am</a>		</div>

		<p>Also you can use QEMU to run the assembly language example described above. Regards</p>

		
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
		<li class="comment even thread-even depth-1 parent" id="comment-1452">
				<div id="div-comment-1452" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-9e81362ff9654533accffc53762c9520-1">			<cite class="fn">Fernando</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-1452">
			July 8, 2013 at 5:53 pm</a>		</div>

		<p>Hi!</p>
<p>I’ve found elsewhere that there are some directives such as “.req” and “.unreq” that let us work with registers as if they were variables.</p>
<p>I haven’t seen that in this course, so I just add this quick comment hoping it is useful for someone else!</p>
<p>P.S: Complete list of directives for ARM can be found here, <a href="http://sourceware.org/binutils/docs-2.19/as/ARM-Directives.html" rel="nofollow">http://sourceware.org/binutils/docs-2.19/as/ARM-Directives.html</a></p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=1452#respond" onclick="return addComment.moveForm( &quot;div-comment-1452&quot;, &quot;1452&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Fernando">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-1453">
				<div id="div-comment-1453" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-2">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-1453">
			July 8, 2013 at 7:14 pm</a>		</div>

		<p>Thanks Fernando!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=1453#respond" onclick="return addComment.moveForm( &quot;div-comment-1453&quot;, &quot;1453&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-3118">
				<div id="div-comment-3118" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/f0157116b26705ae215fa8cb5af0267c?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/f0157116b26705ae215fa8cb5af0267c?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/f0157116b26705ae215fa8cb5af0267c?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-f0157116b26705ae215fa8cb5af0267c-0">			<cite class="fn">carnivion</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-3118">
			January 15, 2014 at 10:17 am</a>		</div>

		<p>Hi, rferrer. Thanks for this tutorial. I’m very beginning at arm assembly. I’m wondering how to build the developing environment? Do you use any IDE? Since I read above paragraphs, it’s seems using an IDE with debugging feature, and output the error code into “output” panel or some other kind of that.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=3118#respond" onclick="return addComment.moveForm( &quot;div-comment-3118&quot;, &quot;3118&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to carnivion">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2 parent" id="comment-3143">
				<div id="div-comment-3143" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-3">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-3143">
			January 15, 2014 at 7:56 pm</a>		</div>

		<p>Hi,</p>
<p>I’m not using any IDE.</p>
<p>As stated above, a text editor like <code>vim</code>, <code>nano</code> or <code>emacs</code> will do.</p>
<p>To build the programs you may want to use the <a href="http://en.wikipedia.org/wiki/Make_%28software%29" rel="nofollow">makefile</a> shown in the post.</p>
<p>Kind regards.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=3143#respond" onclick="return addComment.moveForm( &quot;div-comment-3143&quot;, &quot;3143&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment even depth-3 parent" id="comment-3460">
				<div id="div-comment-3460" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/f0157116b26705ae215fa8cb5af0267c?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/f0157116b26705ae215fa8cb5af0267c?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/f0157116b26705ae215fa8cb5af0267c?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-f0157116b26705ae215fa8cb5af0267c-1">			<cite class="fn">carnivion</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-3460">
			January 22, 2014 at 11:53 am</a>		</div>

		<p>Great! I run it ok! Thanks rferrer, and another question, I’m also getting along with baking Pi OS by Cambridge. Can I use in-built <code>as</code> command instead of <code>...none-eabi...</code> thing?</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=3460#respond" onclick="return addComment.moveForm( &quot;div-comment-3460&quot;, &quot;3460&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to carnivion">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-4" id="comment-3520">
				<div id="div-comment-3520" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-4">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-3520">
			January 23, 2014 at 10:54 am</a>		</div>

		<p>Hi,</p>
<p>yes as long as the tool <code>as --version</code> says somehting like <code>This assembler was configured for a target of `arm-linux-gnueabihf'.</code>.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=3520#respond" onclick="return addComment.moveForm( &quot;div-comment-3520&quot;, &quot;3520&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1 parent" id="comment-147832">
				<div id="div-comment-147832" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/538906abaf7d4bf2becdc9ee796f0293?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/538906abaf7d4bf2becdc9ee796f0293?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/538906abaf7d4bf2becdc9ee796f0293?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-538906abaf7d4bf2becdc9ee796f0293-0">			<cite class="fn"><a href="http://www.megasquirtuk.co.uk" rel="external nofollow" class="url">Eddie Walsh</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-147832">
			June 5, 2014 at 2:56 pm</a>		</div>

		<p>Hi, Thanks for doing this Tutorial, as I am a complete beginner, so my first question is how do I Launch GNU Assembler, but then you say to open a Text editor like vim, nano, or emacs, so where do I get one of them, again Thanks, Regards ED</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=147832#respond" onclick="return addComment.moveForm( &quot;div-comment-147832&quot;, &quot;147832&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Eddie Walsh">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2 parent" id="comment-148045">
				<div id="div-comment-148045" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-5">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-148045">
			June 5, 2014 at 7:02 pm</a>		</div>

		<p>Hi Eddie,</p>
<p>if you are using Raspbian and using the graphical environment, you will see an icon on your desktop that reads “LXTerminal”. Double-click on it and it will open a terminal window where you can type commands (in that window you may use the right-button to open a popup-menu with the usual options for copy/paste, etc).</p>
<p>Now open an editor, given that you are a beginner you’ll probably want to use ‘nano’ which is the easiest of the three. In the terminal type<br>
<code><br>
nano first.s<br>
</code><br>
and copy-paste the text of <em>Our first program</em> shown above in the post. Then press Ctrl-X to exit (in nano this is depicted as ^X). Before exiting nano will ask you about keeping the file, say Yes and press Return to use the proposed filename (which is the one we specified in the command line, i.e. first.s).</p>
<p>Now copy paste the following commands shown above (note that the in the post $ at the beginning of the line is just an representation of the <em>prompt</em> and you do not have to type it). Press Return after each line. (Note that I am repeating here the commands in the post for clarity)<br>
<code><br>
as -o first.o first.s<br>
gcc -o first first.o<br>
</code><br>
The first line invokes the GNU Assembler (to assemble the code into machine code) and the second one invokes the C compiler just to link the program and generate an executable file called ‘first’. Then you can run it.<br>
<code><br>
./first; echo $?<br>
</code><br>
You should see a 2, which is the exit code of our very first program.</p>
<p>If you feel intimidatd by the command line, you may want to invest some time reading a tutorial like <a href="http://linuxcommand.org/learning_the_shell.php" title="Linux tutorial" rel="nofollow">this one</a>. It is not specific to Raspberry but nevertheless will give you enough information so you can feel comfortable in the terminal. Regarding the Makefile, you may want to read this <a href="http://www.linuxdevcenter.com/pub/a/linux/2002/01/31/make_intro.html" rel="nofollow">short introduction to make</a> which will allow you to understand how to use the Makefile shown at the end of the post.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=148045#respond" onclick="return addComment.moveForm( &quot;div-comment-148045&quot;, &quot;148045&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment even depth-3 parent" id="comment-149238">
				<div id="div-comment-149238" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/538906abaf7d4bf2becdc9ee796f0293?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/538906abaf7d4bf2becdc9ee796f0293?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/538906abaf7d4bf2becdc9ee796f0293?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-538906abaf7d4bf2becdc9ee796f0293-1">			<cite class="fn"><a href="http://www.megasquirtuk.co.uk" rel="external nofollow" class="url">Eddie Walsh</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-149238">
			June 6, 2014 at 10:51 pm</a>		</div>

		<p>Hi rferrer, Thanks again, I tried again to follow your instructions from your reply, but when you say “Copy” and “Paste” you did not say from where ??, so I went back to the orig Tutorial and at last got to the end, so I will check out the 2 more links you suggested and hopefully I will get it ??, again can not Thank you enough, Regards ED</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=149238#respond" onclick="return addComment.moveForm( &quot;div-comment-149238&quot;, &quot;149238&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Eddie Walsh">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-4 parent" id="comment-149258">
				<div id="div-comment-149258" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-6">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-149258">
			June 6, 2014 at 11:25 pm</a>		</div>

		<p>Hi Eddie,</p>
<p>maybe I wasn’t clear enough. I meant to copy and paste from the code of the first program (you’ll find in the body of this post, above the comments block of this page) into the ‘nano’ editor.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=149258#respond" onclick="return addComment.moveForm( &quot;div-comment-149258&quot;, &quot;149258&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment even depth-5 parent" id="comment-149835">
				<div id="div-comment-149835" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/538906abaf7d4bf2becdc9ee796f0293?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/538906abaf7d4bf2becdc9ee796f0293?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/538906abaf7d4bf2becdc9ee796f0293?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-538906abaf7d4bf2becdc9ee796f0293-2">			<cite class="fn"><a href="http://www.megasquirtuk.co.uk" rel="external nofollow" class="url">Eddie Walsh</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-149835">
			June 7, 2014 at 2:27 pm</a>		</div>

		<p>Hi rferrer, I thaught that was what you meant, but if I try your Tutorial at same time as Nano on Pi it virtually slows to a crawl, so I run the Tutorial on my Desk Top pc, or again am I doing something wrong, any help appreciated, Regards Ed</p>

		
				</div>
		</li><!-- #comment-## -->
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-5" id="comment-149960">
				<div id="div-comment-149960" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-7">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-149960">
			June 7, 2014 at 5:25 pm</a>		</div>

		<p>Hi Eddie,</p>
<p>you can connect to the Raspberry Pi from your laptop using the SSH protocol. Make sure that both the laptop and the Raspberry are connected to the same router.</p>
<p>This way you can have your Raspberry turned on but you do not need to connect a keyboard nor a display.</p>
<p>This <a href="https://www.modmypi.com/blog/remotely-accessing-the-raspberry-pi-via-ssh-console-mode" rel="nofollow">tutorial</a> explains the process very thoroughly.</p>

		
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
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-178348">
				<div id="div-comment-178348" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/88c69605ed8fc2f44abca2eafe53b575?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/88c69605ed8fc2f44abca2eafe53b575?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/88c69605ed8fc2f44abca2eafe53b575?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-88c69605ed8fc2f44abca2eafe53b575-0">			<cite class="fn">Chria</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-178348">
			July 11, 2014 at 12:04 pm</a>		</div>

		<p>Thank you for this great tutorial. This mini hello world assembler program puts me on the right track. </p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=178348#respond" onclick="return addComment.moveForm( &quot;div-comment-178348&quot;, &quot;178348&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Chria">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2 parent" id="comment-178633">
				<div id="div-comment-178633" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-8">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-178633">
			July 11, 2014 at 7:47 pm</a>		</div>

		<p>Thanks Chria!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=178633#respond" onclick="return addComment.moveForm( &quot;div-comment-178633&quot;, &quot;178633&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment even depth-3 parent" id="comment-183051">
				<div id="div-comment-183051" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/65228c7f0e87e7890fe82e68296b805d?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/65228c7f0e87e7890fe82e68296b805d?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/65228c7f0e87e7890fe82e68296b805d?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-65228c7f0e87e7890fe82e68296b805d-0">			<cite class="fn"><a href="http://retrocomputaria.com.br/plus" rel="external nofollow" class="url">Juan Castro</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-183051">
			July 17, 2014 at 8:15 pm</a>		</div>

		<p>I would like to make a one-day workshop here in Rio de Janeiro using your blog posts as a source — not commercial, free attendance, full credits and attribution — do you give permission?</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=183051#respond" onclick="return addComment.moveForm( &quot;div-comment-183051&quot;, &quot;183051&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Juan Castro">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-4" id="comment-183327">
				<div id="div-comment-183327" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-9">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-183327">
			July 18, 2014 at 7:56 am</a>		</div>

		<p>Hi Juan,</p>
<p>no problem.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=183327#respond" onclick="return addComment.moveForm( &quot;div-comment-183327&quot;, &quot;183327&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="pingback even thread-even depth-1" id="comment-639010">
				<div id="div-comment-639010" class="comment-body">
				<div class="comment-author vcard">
						<cite class="fn"><a href="http://blulin.wordpress.com/2014/11/18/shiny-new-raspberry-b/" rel="external nofollow" class="url">Shiny new Raspberry B+ | Blulin's Blog</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-639010">
			November 17, 2014 at 8:21 pm</a>		</div>

		<p>[…] the ARM assembly tutorial series from <a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/" rel="nofollow">http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/</a> is an interesting stuff to learn about ARM assembly and has about 21 chapters about various […]</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=639010#respond" onclick="return addComment.moveForm( &quot;div-comment-639010&quot;, &quot;639010&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Shiny new Raspberry B+ | Blulin's Blog">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1" id="comment-657242">
				<div id="div-comment-657242" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/b1673470080cb742ea9239ad8ea6d4cc?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/b1673470080cb742ea9239ad8ea6d4cc?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/b1673470080cb742ea9239ad8ea6d4cc?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-b1673470080cb742ea9239ad8ea6d4cc-0">			<cite class="fn"><a href="http://newtonsber.com/blog" rel="external nofollow" class="url">SaberWriter</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-657242">
			November 24, 2014 at 1:21 am</a>		</div>

		<p>Great article.  Thanks for writing it up.  This was the kind of start I was looking for with my RPi.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=657242#respond" onclick="return addComment.moveForm( &quot;div-comment-657242&quot;, &quot;657242&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to SaberWriter">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1 parent" id="comment-809268">
				<div id="div-comment-809268" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/f5f7d4238bdd57aa519da111f934ed12?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/f5f7d4238bdd57aa519da111f934ed12?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/f5f7d4238bdd57aa519da111f934ed12?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-f5f7d4238bdd57aa519da111f934ed12-0">			<cite class="fn">Jimmy Yang</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-809268">
			February 28, 2015 at 8:02 am</a>		</div>

		<p>Hi, I have a question.<br>
In real word, do the engineer write assembly language or just write c code to develop arm?</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=809268#respond" onclick="return addComment.moveForm( &quot;div-comment-809268&quot;, &quot;809268&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Jimmy Yang">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-810592">
				<div id="div-comment-810592" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-10">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-810592">
			March 2, 2015 at 11:08 am</a>		</div>

		<p>Hi Jimmy Yang,</p>
<p>in real world most of the time a high level language is used (like C or others). Sometimes, though, there are performance-critical scenarios where one has to get down to the assembler level because even a C compiler may not deliver all the possible performance. An example of such scenario may be video decoding: in order to be able to deliver the appropiate number of frames per second, the decoding process must be as fast as possible.</p>
<p>Also, while I would not recommend anyone to write in assembler in a day-to-day basis, it is very interesting to know what underpins higher level languages.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=810592#respond" onclick="return addComment.moveForm( &quot;div-comment-810592&quot;, &quot;810592&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="pingback even thread-odd thread-alt depth-1" id="comment-897485">
				<div id="div-comment-897485" class="comment-body">
				<div class="comment-author vcard">
						<cite class="fn"><a href="https://www.progclub.org/blog/2015/07/04/arm-assembler-in-raspberry-pi/" rel="external nofollow" class="url">ARM assembler in Raspberry Pi | ProgClub</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-897485">
			July 4, 2015 at 2:31 am</a>		</div>

		<p>[…] Reading about ARM assembler in Raspberry Pi… […]</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=897485#respond" onclick="return addComment.moveForm( &quot;div-comment-897485&quot;, &quot;897485&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to ARM assembler in Raspberry Pi | ProgClub">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment odd alt thread-even depth-1 parent" id="comment-922099">
				<div id="div-comment-922099" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/25a3be18a59cbe19c3372911d2cfab7a?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/25a3be18a59cbe19c3372911d2cfab7a?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/25a3be18a59cbe19c3372911d2cfab7a?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-25a3be18a59cbe19c3372911d2cfab7a-0">			<cite class="fn">Denis</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-922099">
			September 1, 2015 at 7:50 pm</a>		</div>

		<p>Dear Roger,<br>
big Thanks for your articles, will be waiting new posts.</p>
<p>Can I translate your articles to Russian language for the habrahabr.ru with references to you site?</p>
<p>—<br>
Best Regards<br>
Denis</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=922099#respond" onclick="return addComment.moveForm( &quot;div-comment-922099&quot;, &quot;922099&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Denis">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2" id="comment-922604">
				<div id="div-comment-922604" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-11">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-922604">
			September 2, 2015 at 6:46 pm</a>		</div>

		<p>Hi Denis,</p>
<p>there is no problem on translating them as long as you fulfill the Creative Commons of the posts (<a href="http://creativecommons.org/licenses/by-nc-sa/4.0/" rel="nofollow">http://creativecommons.org/licenses/by-nc-sa/4.0/</a>).</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=922604#respond" onclick="return addComment.moveForm( &quot;div-comment-922604&quot;, &quot;922604&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1 parent" id="comment-945292">
				<div id="div-comment-945292" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/77acf25ce2a2341db7613df74248e0a9?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/77acf25ce2a2341db7613df74248e0a9?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/77acf25ce2a2341db7613df74248e0a9?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-77acf25ce2a2341db7613df74248e0a9-0">			<cite class="fn">Christopher</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-945292">
			October 24, 2015 at 6:02 pm</a>		</div>

		<p>Very friendly introduction! Thanks </p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=945292#respond" onclick="return addComment.moveForm( &quot;div-comment-945292&quot;, &quot;945292&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Christopher">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2" id="comment-945533">
				<div id="div-comment-945533" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-12">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-945533">
			October 25, 2015 at 9:09 am</a>		</div>

		<p>Thanks Christopher!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=945533#respond" onclick="return addComment.moveForm( &quot;div-comment-945533&quot;, &quot;945533&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-even depth-1 parent" id="comment-945294">
				<div id="div-comment-945294" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/77acf25ce2a2341db7613df74248e0a9?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/77acf25ce2a2341db7613df74248e0a9?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/77acf25ce2a2341db7613df74248e0a9?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-77acf25ce2a2341db7613df74248e0a9-1">			<cite class="fn">Christopher</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-945294">
			October 24, 2015 at 6:06 pm</a>		</div>

		<p>Your wp-comments-post.php is publicly displaying your PHPMailer communications.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=945294#respond" onclick="return addComment.moveForm( &quot;div-comment-945294&quot;, &quot;945294&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Christopher">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2" id="comment-945532">
				<div id="div-comment-945532" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-13">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-945532">
			October 25, 2015 at 9:09 am</a>		</div>

		<p>Hi,</p>
<p>it should now be fixed. Thank you very much for the notice.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=945532#respond" onclick="return addComment.moveForm( &quot;div-comment-945532&quot;, &quot;945532&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1 parent" id="comment-952912">
				<div id="div-comment-952912" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/8e244a278535d94a99bb9864eff6abf9?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/8e244a278535d94a99bb9864eff6abf9?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/8e244a278535d94a99bb9864eff6abf9?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-8e244a278535d94a99bb9864eff6abf9-0">			<cite class="fn"><a href="http://helderc.github.io" rel="external nofollow" class="url">Helder</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-952912">
			November 26, 2015 at 10:44 am</a>		</div>

		<p>Hi. I just found this post and I have to say: Thank you! And congratulations for the great job!</p>
<p>I didn’t see the whole series yet but, will You cover how to use some native Raspbian’s API (probably X.org’s api) to create a program with GUI in Assembly?</p>
<p>Would be interesting see something like that!</p>
<p>Again thank you and keep going the great job!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=952912#respond" onclick="return addComment.moveForm( &quot;div-comment-952912&quot;, &quot;952912&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Helder">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2 parent" id="comment-952947">
				<div id="div-comment-952947" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-14">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-952947">
			November 26, 2015 at 7:23 pm</a>		</div>

		<p>Hi Helder,</p>
<p>regarding your question about GUI in assembly, I do not cover GUI in any chapter and I’m not planning to do it. But who knows, maybe I change my mind.</p>
<p>It is doable, of course, but it would end being doing lots of calls to the specific GUI toolkit API. At first it looks rather boring and tedious. This does not mean it is impossible but I think it is not a task particularly revealing when done using assembler </p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=952947#respond" onclick="return addComment.moveForm( &quot;div-comment-952947&quot;, &quot;952947&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment odd alt depth-3" id="comment-953018">
				<div id="div-comment-953018" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/8e244a278535d94a99bb9864eff6abf9?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/8e244a278535d94a99bb9864eff6abf9?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/8e244a278535d94a99bb9864eff6abf9?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-8e244a278535d94a99bb9864eff6abf9-1">			<cite class="fn">Helder</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-953018">
			November 27, 2015 at 11:05 am</a>		</div>

		<p>I see…<br>
I dont know assembly and I pretend to use your tutorials to learn it for fun… Do you thing is possible to use any GUI library in assembly code?</p>
<p>Thank you.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=953018#respond" onclick="return addComment.moveForm( &quot;div-comment-953018&quot;, &quot;953018&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Helder">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1 parent" id="comment-953097">
				<div id="div-comment-953097" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/04d04f6918602e8c42d9945502f8bf32?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/04d04f6918602e8c42d9945502f8bf32?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/04d04f6918602e8c42d9945502f8bf32?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-04d04f6918602e8c42d9945502f8bf32-0">			<cite class="fn"><a href="http://yahoo.com" rel="external nofollow" class="url">Robert Service</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-953097">
			November 28, 2015 at 4:38 am</a>		</div>

		<p>One of the most lucid introductions to ARM assembly language I’ve seen.<br>
The clear,  and easy,  expository style is a breath of fresh air.<br>
One can only hope that you’ve got quite a bit more to say on this subject…</p>
<p>for example–<br>
the actual assembly process from beginning to end, i.e., text editor &gt; assembler &gt; loading object code into machine and running the program (use of the Raspberry Pi as the target machine might be desirable due to the ubiquitous nature of this device).</p>
<p>Please keep up the good work.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=953097#respond" onclick="return addComment.moveForm( &quot;div-comment-953097&quot;, &quot;953097&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Robert Service">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-953122">
				<div id="div-comment-953122" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-15">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-953122">
			November 28, 2015 at 9:37 am</a>		</div>

		<p>Hi Robert,</p>
<p>thank you for the kind words.</p>
<p>Regarding explaining the rest of the assembly process: yes I have planned to make some posts in this line in the future.</p>
<p>Warm regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=953122#respond" onclick="return addComment.moveForm( &quot;div-comment-953122&quot;, &quot;953122&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-953143">
				<div id="div-comment-953143" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/04d04f6918602e8c42d9945502f8bf32?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/04d04f6918602e8c42d9945502f8bf32?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/04d04f6918602e8c42d9945502f8bf32?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-04d04f6918602e8c42d9945502f8bf32-1">			<cite class="fn"><a href="http://yahoo.com" rel="external nofollow" class="url">Robert Service</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-953143">
			November 28, 2015 at 3:28 pm</a>		</div>

		<p>Please bear with me…<br>
My only use for assembly language has been to get hardware designs to run (minicomputers, Z80, 68XXX, 80XXX, PIC, Arduino, etc.), when there’s not a “proper” operating system to lean on. My software IS the operating system.<br>
I am not at all conversant (except for Fortran and BASIC) with any higher-level language, or environment, except to be able to READ C and C++ (which explains my trepidation about embracing the Raspberry Pi, I suppose).<br>
Hopefully, you’re in a good position to understand my question, and can answer it admirably:<br>
What is the reason for writing Assembly Language encased in a “C/C++” “wrapper”? Is this necessitated by the RPi environment? Can I write Assembly Language for the RPi as I do for a PIC machine, or an Atmel processor?<br>
For me, at least, clarification of this point will be deeply appreciated.<br>
Personal note: I  have been extremely disappointed in the number of “authors” who purport to teach RPi Assembly Language by requiring one to use Raspbian. I may not be an expert, but I know this is not true Assembly programming. It at least appears as if you’re leading us to “the real thing”, and in the proper direction.</p>
<p>Thanks in advance very much, and please keep up the outstanding work.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=953143#respond" onclick="return addComment.moveForm( &quot;div-comment-953143&quot;, &quot;953143&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Robert Service">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-953234">
				<div id="div-comment-953234" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-16">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-953234">
			November 29, 2015 at 10:47 am</a>		</div>

		<p>Hi Robert,</p>
<p>as you excellently pointed, there is actually no need to write assembly in the way we do in these chapters: this is not a requirement of the Raspberry Pi itself.</p>
<p>There are several resources where people have done <em>bare metal</em> development on the Raspberry Pi. For instance, there is the course <a href="http://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/" rel="nofollow">Baking Pi – Operating Systems Development</a> from the University of Cambridge where a basic operating system is written on top of the Raspberry Pi. Another resource of <i>bare metal</i> information for the Raspberry Pi can be found <a href="https://github.com/dwelch67/raspberrypi" rel="nofollow">here</a>.</p>
<p>In this blog, though, I focus on the point of view of applications rather than the systems. I think that most Raspberry Pi users are using an operating system, likely Raspbian (or another Linux or BSD derivative), so it makes sense to me to start writing small programs on top of the operating system using assembler.</p>
<p>I struggle with the concept of «true assembly programming». Personally, assembler is just another tool in the box (a special one that, unfortunately, may not be portable among systems). There are a few situations where assembler knowledge is a requirement: systems programming (i.e. firmwares and some critical routines of operating systems), writing compilers, debugging and writing performance-critical code.</p>
<p>I hope I answered your questions well enough.</p>
<p>Kind regards.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=953234#respond" onclick="return addComment.moveForm( &quot;div-comment-953234&quot;, &quot;953234&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1" id="comment-953249">
				<div id="div-comment-953249" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/04d04f6918602e8c42d9945502f8bf32?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/04d04f6918602e8c42d9945502f8bf32?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/04d04f6918602e8c42d9945502f8bf32?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-04d04f6918602e8c42d9945502f8bf32-2">			<cite class="fn"><a href="http://yahoo.com" rel="external nofollow" class="url">Robert Service</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-953249">
			November 29, 2015 at 5:58 pm</a>		</div>

		<p>Thank you for your rapid response; I’m beginning to ‘get it’.<br>
No need to reply; I have very confidence that, given your outstanding efforts, the only additional element needed is patience on my part.<br>
Warmest regards…</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=953249#respond" onclick="return addComment.moveForm( &quot;div-comment-953249&quot;, &quot;953249&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Robert Service">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1 parent" id="comment-958420">
				<div id="div-comment-958420" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/72bb4d00141e8e0499345e00f8a5ec7a?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/72bb4d00141e8e0499345e00f8a5ec7a?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/72bb4d00141e8e0499345e00f8a5ec7a?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-72bb4d00141e8e0499345e00f8a5ec7a-0">			<cite class="fn">Marius</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-958420">
			January 15, 2016 at 10:35 am</a>		</div>

		<p>Hi Roger,</p>
<p>First of all , thanks for the awesome tutorial you set up here. I have a few questions: </p>
<p>1) Is there any chance this will work in the raspberry pi 2 ?<br>
2) Will you ever make a tutorial for more OS-development oriented folks, nothing fancy, just something that boots up and takes you to a simple command line. I guess then everyone could get their simple OS started and modify it as they wish. </p>
<p>Again ,thanks for teaching me quite a bit about ARM assembly.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=958420#respond" onclick="return addComment.moveForm( &quot;div-comment-958420&quot;, &quot;958420&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Marius">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2" id="comment-958497">
				<div id="div-comment-958497" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-17">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-958497">
			January 16, 2016 at 10:17 am</a>		</div>

		<p>Hi Marius,</p>
<p>thank you for your kind comments.</p>
<p>1) in principle yes given that the ARMv6 architecture of the Raspberry 1 is contained in the ARMv7 architecture of the Raspberry 2.<br>
That said, most of the VFPv2 instructions used in the chapter 14 will run very slow as they are emulated in software (NEON instructions should be used instead)</p>
<p>2) Not at the moment. You may want to check <a href="http://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/" rel="nofollow">Baking Pi – Operating Systems Development</a>. They have a big warning that the tutorial has not been updated for the Raspberry Pi 2 but this should not mean it cannot work there.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=958497#respond" onclick="return addComment.moveForm( &quot;div-comment-958497&quot;, &quot;958497&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="pingback odd alt thread-even depth-1" id="comment-962230">
				<div id="div-comment-962230" class="comment-body">
				<div class="comment-author vcard">
						<cite class="fn"><a href="http://www.retrocomputaria.com.br/2015/07/29/reporter-retro-no-006/" rel="external nofollow" class="url">Repórter Retro Nº 006 | Retrocomputaria</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-962230">
			March 11, 2016 at 2:18 pm</a>		</div>

		<p>[…] Sophie Wilson fala tudo sobre o ARM, e se você quer aprender o Assembler dele, divirta-se […]</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=962230#respond" onclick="return addComment.moveForm( &quot;div-comment-962230&quot;, &quot;962230&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Repórter Retro Nº 006 | Retrocomputaria">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-966759">
				<div id="div-comment-966759" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a48b3d3c9a90482b6ec67a9aa1ceab38?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a48b3d3c9a90482b6ec67a9aa1ceab38?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a48b3d3c9a90482b6ec67a9aa1ceab38?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a48b3d3c9a90482b6ec67a9aa1ceab38-0">			<cite class="fn">Gabriel Tobar</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-966759">
			May 11, 2016 at 12:26 am</a>		</div>

		<p>Hi! Nice tutorial and very helpful! I was wondering if I could use some of your help… currently I’m making an assembly project and right now I need to generate a random number. I have tried with an algorithm do generate pseudo random numbers but I dont know if it is working because whenever I print the result it just gives me error code 1.</p>
<p>this is the code:</p>
<p>/*–random01.s*/<br>
.data</p>
<p>.balign 4<br>
mensaje1: .asciz “Random \n”</p>
<p>.balign 4<br>
return: .word 0</p>
<p>.text</p>
<p>.global main<br>
main:</p>
<p>	ldr r1, addr_of_return<br>
	str lr, [r1]</p>
<p>	ldr r0, addr_of_msg1<br>
	bl printf</p>
<p>	mov r0,#3<br>
	mov r1,#4</p>
<p>	tst r1,r1, lsr #1<br>
	movs r2,r0, rrx<br>
	adc r1,r1, r1<br>
	eor r2,r2, r0, lsl #12<br>
	eor r0,r2, r2, lsr #20</p>
<p>	ldr r0, [r0]<br>
	bl printf</p>
<p>	ldr lr, addr_of_return<br>
	ldr lr, [lr]<br>
	bx lr</p>
<p>addr_of_msg1: .word mensaje1<br>
addr_of_return: .word return</p>
<p>.global printf</p>
<p>Could you help me? I sincerely dont know what did I do wrong. Is there another way to generate the random numbers? I was trying as well with the clock, but had the same results…</p>
<p>Thank you!</p>
<p>Regards!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=966759#respond" onclick="return addComment.moveForm( &quot;div-comment-966759&quot;, &quot;966759&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Gabriel Tobar">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2 parent" id="comment-969199">
				<div id="div-comment-969199" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-18">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-969199">
			June 5, 2016 at 10:15 am</a>		</div>

		<p>Hi Gabriel,</p>
<p>I have not checked your code but an easy way to generate a random number is calling C library function <code>random</code>. It receives no arguments (but recall that r0 to r3 may be modified by the callee!) and it returns the result as a 32-bit number in r0. Note that these random numbers should <strong>not</strong> be used for serious purposes (like cryptography) </p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=969199#respond" onclick="return addComment.moveForm( &quot;div-comment-969199&quot;, &quot;969199&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment even depth-3 parent" id="comment-979226">
				<div id="div-comment-979226" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/fefe74da277fbb006cd54bdc9fde8dba?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/fefe74da277fbb006cd54bdc9fde8dba?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/fefe74da277fbb006cd54bdc9fde8dba?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-fefe74da277fbb006cd54bdc9fde8dba-0">			<cite class="fn">Max Q.</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-979226">
			November 11, 2016 at 6:40 pm</a>		</div>

		<p>Hi Roger, i was wondering if you could give an example using that random function, or give another page where i can find such example, thank you in advance.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=979226#respond" onclick="return addComment.moveForm( &quot;div-comment-979226&quot;, &quot;979226&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Max Q.">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-4" id="comment-979267">
				<div id="div-comment-979267" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-19">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-979267">
			November 12, 2016 at 8:25 pm</a>		</div>

		<p>Hi Max,</p>
<p>in chapter 9 and 10 it is explained how to call functions. Function <code>random</code> receives 0 arguments, so it should be easy. The random number will be left in <code>r0</code>.</p>
<p>More information on the random function can be found in its <a href="https://linux.die.net/man/3/random" rel="nofollow">man page</a>.</p>
<p>Regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=979267#respond" onclick="return addComment.moveForm( &quot;div-comment-979267&quot;, &quot;979267&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1 parent" id="comment-971034">
				<div id="div-comment-971034" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/3fbca7814c83651f586a33c0318d64b2?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/3fbca7814c83651f586a33c0318d64b2?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/3fbca7814c83651f586a33c0318d64b2?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-3fbca7814c83651f586a33c0318d64b2-0">			<cite class="fn">Michael Koch</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-971034">
			July 9, 2016 at 9:53 pm</a>		</div>

		<p>Where can I find a manual for the “as” assembler? I need some more info about pseudo-commands like .equ, .rept, .endr, .macro, .endm, push, pop…</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=971034#respond" onclick="return addComment.moveForm( &quot;div-comment-971034&quot;, &quot;971034&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Michael Koch">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-971035">
				<div id="div-comment-971035" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-20">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-971035">
			July 9, 2016 at 9:58 pm</a>		</div>

		<p>Hi Michael,</p>
<p>the GNU assembler is part of the binutils collection. Find the documentation for as here</p>
<p>  <a href="https://sourceware.org/binutils/docs-2.22/as/index.html" rel="nofollow">https://sourceware.org/binutils/docs-2.22/as/index.html</a></p>
<p>Kind regards,<br>
Roger</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=971035#respond" onclick="return addComment.moveForm( &quot;div-comment-971035&quot;, &quot;971035&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1" id="comment-971073">
				<div id="div-comment-971073" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/3fbca7814c83651f586a33c0318d64b2?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/3fbca7814c83651f586a33c0318d64b2?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/3fbca7814c83651f586a33c0318d64b2?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-3fbca7814c83651f586a33c0318d64b2-1">			<cite class="fn">Michael Koch</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-971073">
			July 10, 2016 at 1:41 pm</a>		</div>

		<p>Thanks!<br>
Michael</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=971073#respond" onclick="return addComment.moveForm( &quot;div-comment-971073&quot;, &quot;971073&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Michael Koch">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment odd alt thread-even depth-1 parent" id="comment-973386">
				<div id="div-comment-973386" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/1b6c12eb1ecda175c6fa29f0e880ecb5?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/1b6c12eb1ecda175c6fa29f0e880ecb5?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/1b6c12eb1ecda175c6fa29f0e880ecb5?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-1b6c12eb1ecda175c6fa29f0e880ecb5-0">			<cite class="fn">Fred</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-973386">
			August 18, 2016 at 1:21 pm</a>		</div>

		<p>Looks like an awesome and well explained tutorial. Since I already know some x86 assembler in Linux I always wondered how this looks like for ARM. And the Raspberry Pi is the perfect platform for that. I am really looking forward to learn something about ARM assembler.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=973386#respond" onclick="return addComment.moveForm( &quot;div-comment-973386&quot;, &quot;973386&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Fred">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2" id="comment-973400">
				<div id="div-comment-973400" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-21">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-973400">
			August 18, 2016 at 7:02 pm</a>		</div>

		<p>Thanks Fred!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=973400#respond" onclick="return addComment.moveForm( &quot;div-comment-973400&quot;, &quot;973400&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="pingback odd alt thread-odd thread-alt depth-1" id="comment-977801">
				<div id="div-comment-977801" class="comment-body">
				<div class="comment-author vcard">
						<cite class="fn"><a href="https://cprogramex.wordpress.com/2016/10/25/raspberry-arduino/" rel="external nofollow" class="url">Raspberry &amp; Arduino – C Programming Exercises</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-977801">
			October 25, 2016 at 6:41 am</a>		</div>

		<p>[…] Think in Geek […]</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=977801#respond" onclick="return addComment.moveForm( &quot;div-comment-977801&quot;, &quot;977801&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Raspberry &amp; Arduino – C Programming Exercises">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1 parent" id="comment-980161">
				<div id="div-comment-980161" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/1567d9eec2b29932cd4e1be2c5f64dac?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/1567d9eec2b29932cd4e1be2c5f64dac?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/1567d9eec2b29932cd4e1be2c5f64dac?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-1567d9eec2b29932cd4e1be2c5f64dac-0">			<cite class="fn">Luis Guilherme</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-980161">
			November 23, 2016 at 7:34 pm</a>		</div>

		<p>.s for Source</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=980161#respond" onclick="return addComment.moveForm( &quot;div-comment-980161&quot;, &quot;980161&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Luis Guilherme">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-980592">
				<div id="div-comment-980592" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-22">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-980592">
			November 27, 2016 at 1:01 pm</a>		</div>

		<p>Sounds plausible. Thanks!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=980592#respond" onclick="return addComment.moveForm( &quot;div-comment-980592&quot;, &quot;980592&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-986749">
				<div id="div-comment-986749" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/8f7388a1e08af0775e113faf61cae962?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/8f7388a1e08af0775e113faf61cae962?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/8f7388a1e08af0775e113faf61cae962?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-8f7388a1e08af0775e113faf61cae962-0">			<cite class="fn">Martin</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-986749">
			February 15, 2017 at 8:20 pm</a>		</div>

		<p>Hi,</p>
<p>Just discovered this site, a bit late to the game it appears…</p>
<p>thanks so much for writing these, I’ve always wanted to get ‘my hands dirty’ playing with assembler and understanding its processes…this looks like the ideal support and you are clearly committed to passing on your knowledge. I am already looking forward to the next chapters</p>
<p>Thanks again</p>
<p>martin</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=986749#respond" onclick="return addComment.moveForm( &quot;div-comment-986749&quot;, &quot;986749&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Martin">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-986905">
				<div id="div-comment-986905" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-23">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-986905">
			February 17, 2017 at 9:06 pm</a>		</div>

		<p>Hi Martin, thank you very much!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=986905#respond" onclick="return addComment.moveForm( &quot;div-comment-986905&quot;, &quot;986905&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1 parent" id="comment-987648">
				<div id="div-comment-987648" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/6b6ed72e94a4679f1206cea8daf2ce2b?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/6b6ed72e94a4679f1206cea8daf2ce2b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/6b6ed72e94a4679f1206cea8daf2ce2b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-6b6ed72e94a4679f1206cea8daf2ce2b-0">			<cite class="fn">Ron Yorke</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-987648">
			February 28, 2017 at 9:26 pm</a>		</div>

		<p>Having finished my blether, does anybody have full hardware details of the various versions of the RPi?  The last time I tried to get a copy, I was informed that that level of information was only available to bona fide companies designing products for third parties.  I hope that this attitude has changed, and the information is now more freely available.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=987648#respond" onclick="return addComment.moveForm( &quot;div-comment-987648&quot;, &quot;987648&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Ron Yorke">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-987882">
				<div id="div-comment-987882" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-24">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-987882">
			March 4, 2017 at 9:32 pm</a>		</div>

		<p>Hi Ron,</p>
<p>there is some documentation here <a href="https://www.raspberrypi.org/documentation/hardware/raspberrypi/README.md" rel="nofollow">https://www.raspberrypi.org/documentation/hardware/raspberrypi/README.md</a></p>
<p>Further than that I have no idea whether it is available or not.</p>
<p>Regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=987882#respond" onclick="return addComment.moveForm( &quot;div-comment-987882&quot;, &quot;987882&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-988776">
				<div id="div-comment-988776" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/6b5a57e5813366ad5dda07e0bd8fe84b?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/6b5a57e5813366ad5dda07e0bd8fe84b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/6b5a57e5813366ad5dda07e0bd8fe84b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-6b5a57e5813366ad5dda07e0bd8fe84b-0">			<cite class="fn">ehrt74</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-988776">
			March 18, 2017 at 11:50 am</a>		</div>

		<p>btw. If anybody wants to use the gnu linker rather than gcc you have to change your code like this;</p>
<p>.global _start<br>
_start:<br>
mov r0, #2<br>
mov r7, #1 // request to exit program<br>
swi 0</p>
<p>then you can assemble and link it as follows:</p>
<p>as myProgram.s -o myProgram.o<br>
ld myProgram.o -o myProgram</p>
<p>and run it with</p>
<p>./myProgram</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=988776#respond" onclick="return addComment.moveForm( &quot;div-comment-988776&quot;, &quot;988776&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to ehrt74">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-988848">
				<div id="div-comment-988848" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-25">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-988848">
			March 19, 2017 at 4:35 pm</a>		</div>

		<p>Thanks, I avoided this way because I thought it was more complicated to explain all those magical <code>mov</code> and <code>swi</code> instructions. <a href="http://Chapter 19" rel="nofollow">http://thinkingeek.com/2014/05/24/arm-assembler-raspberry-pi-chapter-19/</a> deals about system calls. In your case I assume you are calling <code>exit</code>.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=988848#respond" onclick="return addComment.moveForm( &quot;div-comment-988848&quot;, &quot;988848&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1 parent" id="comment-990515">
				<div id="div-comment-990515" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/511715e44d1e24214bf7f6df585f1f74?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/511715e44d1e24214bf7f6df585f1f74?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/511715e44d1e24214bf7f6df585f1f74?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-511715e44d1e24214bf7f6df585f1f74-0">			<cite class="fn">aldenq</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-990515">
			April 12, 2017 at 6:47 am</a>		</div>

		<p>Hello, thanks for creating this article.<br>
I have been having some problems with it though, for example upon executing the code from the first example it returns 132 rather then 2.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=990515#respond" onclick="return addComment.moveForm( &quot;div-comment-990515&quot;, &quot;990515&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to aldenq">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-990826">
				<div id="div-comment-990826" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-26">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-990826">
			April 17, 2017 at 1:07 pm</a>		</div>

		<p>That’s weird.</p>
<p>Without more info I cannot help much. Are you following the correct steps?</p>
<p>Kind regards</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=990826#respond" onclick="return addComment.moveForm( &quot;div-comment-990826&quot;, &quot;990826&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="pingback even thread-odd thread-alt depth-1" id="comment-990978">
				<div id="div-comment-990978" class="comment-body">
				<div class="comment-author vcard">
						<cite class="fn"><a href="http://www.quarterstaff.co.uk/2017/04/raspberry-pi-assembly-tutorial/" rel="external nofollow" class="url">Raspberry Pi Assembly tutorial | Developing Skills</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-990978">
			April 20, 2017 at 8:42 pm</a>		</div>

		<p>[…] stumbled across this Raspberry Pi assembler tutorial&nbsp;which seemed like a fun thing to try out. Who would have thought you could spend your evening […]</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=990978#respond" onclick="return addComment.moveForm( &quot;div-comment-990978&quot;, &quot;990978&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Raspberry Pi Assembly tutorial | Developing Skills">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="pingback odd alt thread-even depth-1" id="comment-993809">
				<div id="div-comment-993809" class="comment-body">
				<div class="comment-author vcard">
						<cite class="fn"><a href="https://whatdoimake.com/do-you-want-to-learn-arm-assembly/" rel="external nofollow" class="url">Do You Want to Learn ARM Assembly? - What Do I Make</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#comment-993809">
			June 1, 2017 at 4:07 am</a>		</div>

		<p>[…] The “Hello World” of assembly language programming. (Code credit: Roger Ferrer&nbsp;Ibáñez) […]</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/09/arm-assembler-raspberry-pi-chapter-1/?replytocom=993809#respond" onclick="return addComment.moveForm( &quot;div-comment-993809&quot;, &quot;993809&quot;, &quot;respond&quot;, &quot;334&quot; )" aria-label="Reply to Do You Want to Learn ARM Assembly? - What Do I Make">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		</ul>
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2013/01/09/arm-assembler-raspberry-pi-chapter-1/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="334" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="9dadb9f0b7"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496588351538"></form>
			</div><!-- #respond -->
	</div>	</div>
