# Branching


<div class="main">
			<h1>ARM assembler in Raspberry Pi – Chapter 5 </h1>
		<p class="post-meta"><span class="date">January 19, 2013</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/#comments">22</a></span> </p>
		<h2>Branching</h2>
<p>
Until now our small assembler programs execute one instruction after the other. If our ARM processor were only able to run this way it would be of limited use. It could not react to existing conditions which may require different sequences of instructions. This is the purpose of the <em>branch</em> instructions.
</p>
<p><span id="more-447"></span></p>
<h2>A special register</h2>
<p>
In chapter 2 we learnt that our Raspberry Pi ARM processor has 16 integer general purpose registers and we also said that some of them play special roles in our program. I deliberately ignored which registers were special as it was not relevant at that time.
</p>
<p>
But now it is relevant, at least for register <code>r15</code>. This register is very special, so special it has also another name: <code>pc</code>. It is unlikely that you see it used as <code>r15</code> since it is confusing (although correct from the point of view of the ARM architecture). From now we will only use <code>pc</code> to name it.
</p>
<p>
What does <code>pc</code> stand for? <code>pc</code> means <em>program counter</em>. This name, the origins of which are in the dawn of computing, means little to nothing nowadays. In general the <code>pc</code> register (also called <code>ip</code>, <em>instruction pointer</em>, in other architectures like 386 or x86_64) contains the address of the next instruction going to be executed.
</p>
<p>
When the ARM processor executes an instruction, two things may happen at the end of its execution. If the instruction does not modify <code>pc</code> (and most instructions do not), <code>pc</code> is just incremented by 4 (like if we did <code>add pc, pc, #4</code>). Why 4? Because in ARM, instructions are 32 bit wide, so there are 4 bytes between every instruction. If the instruction modifies <code>pc</code> then the new value for <code>pc</code> is used.
</p>
<p>
Once the processor has fully executed an instruction then it uses the value in the <code>pc</code> as the address for the next instruction to execute. This way, an instruction that does not modify the <code>pc</code> will be followed by the next contiguous instruction in memory (since it has been automatically increased by 4). This is called <em>implicit sequencing</em> of instructions: after one has run, usually the next one in memory runs. But if an instruction does modify the <code>pc</code>, for instance to a value other than <code>pc + 4</code>, then we can be running another instruction of the program. This process of changing the value of <code>pc</code> is called <em>branching</em>. In ARM this done using <em>branch instructions</em>.
</p>
<h2>Unconditional branches</h2>
<p>
You can tell the processor to branch unconditionally by using the instruction <code>b</code> (for <em>branch</em>) and a label. Consider the following program.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> <span style="color: #339933;">--</span> branch01<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #0000ff; font-weight: bold;">.text</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">global</span> main
main<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span> <span style="color: #339933;">/*</span> r0 ← <span style="color: #ff0000;">2</span> <span style="color: #339933;">*/</span>
    b end      <span style="color: #339933;">/*</span> branch to <span style="color: #7f007f;">'end'</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">3</span> <span style="color: #339933;">/*</span> r0 ← <span style="color: #ff0000;">3</span> <span style="color: #339933;">*/</span>
end<span style="color: #339933;">:</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* -- branch01.s */
.text
.global main
main:
    mov r0, #2 /* r0 ← 2 */
    b end      /* branch to 'end' */
    mov r0, #3 /* r0 ← 3 */
end:
    bx lr</p></div>

<p>
If you execute this program you will see that it returns an error code of 2.</p>
<p></p><pre>$ ./branch01 ; echo $?
2
</pre>
<p>
What happened is that instruction <code>b end</code> <em>branched</em> (modifying the <code>pc</code>) to the instruction at the label <code>end</code>, which is <code>bx lr</code>, the instruction we run at the end of our program. This way the instruction <code>mov r0, #3</code> has not actually been run at all (the processor never reached that instruction).
</p>
<p>
At this point the unconditional branch instruction <code>b</code> may look a bit useless. It is not the case. In fact this instruction is essential in some contexts, in particular when linked with conditional branching. But before we can talk about conditional branching we need to talk about conditions.
</p>
<h2>Conditional branches</h2>
<p>
If our processor were only able to branch just because, it would not be very useful. It is much more useful to branch <em>when some condition is met</em>. So a processor should be able to evaluate some sort of conditions.
</p>
<p>
Before continuing, we need to unveil another register called <code>cpsr</code> (for Current Program Status Register). This register is a bit special and directly modifying it is out of the scope of this chapter. That said, it keeps some values that can be read and updated when executing an instruction. The values of that register include four <em>condition code flags</em> called <code>N</code> (<b>n</b>egative), <code>Z</code> (<b>z</b>ero), <code>C</code> (<b>c</b>arry) and <code>V</code> (o<b>v</b>erflow). These four condition code flags are usually read by branch instructions. Arithmetic instructions and special testing and comparison instruction can update these condition codes too if requested.
</p>
<p>
The semantics of these four condition codes in instructions updating the <code>cpsr</code> are roughly the following
</p>
<ul>
<li><code>N</code> will be enabled if the result of the instruction yields a negative number. Disabled otherwise.
</li><li><code>Z</code> will be enabled if the result of the instruction yields a zero value. Disabled if nonzero.
</li><li><code>C</code> will be enabled if the result of the instruction yields a value that requires a 33rd bit to be fully represented. For instance an addition that overflows the 32 bit range of integers. There is a special case for C and subtractions where a <em>non-borrowing</em> subtraction enables it, disabled otherwise: subtracting a larger number to a smaller one enables C, but it will be disabled if the subtraction is done the other way round.
</li><li><code>V</code> will be enabled if the result of the instruction yields a value that cannot be represented in 32 bits two’s complement.
</li></ul>
<p>
So we have all the needed pieces to perform branches conditionally. But first, let’s start comparing two values. We use the instruction <code>cmp</code> for this purpose.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">cmp</span> r1<span style="color: #339933;">,</span> r2 <span style="color: #339933;">/*</span> updates cpsr doing <span style="color: #7f007f;">"r1 - r2"</span><span style="color: #339933;">,</span> but r1 <span style="color: #00007f; font-weight: bold;">and</span> r2 are <span style="color: #00007f; font-weight: bold;">not</span> modified <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">cmp r1, r2 /* updates cpsr doing "r1 - r2", but r1 and r2 are not modified */</p></div>

<p>
This instruction subtracts to the value in the first register the value in the second register. Examples of what could happen in the snippet above?
</p>
<ul>
<li>If <code>r2</code> had a value (strictly) greater than <code>r1</code> then <code>N</code> would be enabled because <code>r1-r2</code> would yield a negative result.
</li><li>If <code>r1</code> and <code>r2</code> had the same value, then <code>Z</code> would be enabled because <code>r1-r2</code> would be zero.
</li><li>If <code>r1</code> was 1 and <code>r2</code> was 0 then <code>r1-r2</code> would not borrow, so in this case <code>C</code> would be enabled. If the values were swapped (<code>r1</code> was 0 and <code>r2</code> was 1) then C would be disabled because the subtraction does borrow.
</li><li>If <code>r1</code> was 2147483648 (the largest positive integer in 32 bit two’s complement)  and <code>r1</code> was -1 then <code>r1-r2</code> would be 2147483649 but such number cannot be represented in 32 bit two’s complement, so <code>V</code> would be enabled to signal this.
</li></ul>
<p>
How can we use these flags to represent useful conditions for our programs?
</p>
<ul>
<li><code>EQ</code> (<strong>eq</strong>ual) When Z is enabled (Z is 1)
</li><li><code>NE</code> (<strong>n</strong>ot <strong>e</strong>qual). When Z is disabled. (Z is 0)
</li><li><code>GE</code> (<strong>g</strong>reater or <strong>e</strong>qual than, in two’s complement). When both V and N are enabled or disabled (V is N)
</li><li><code>LT</code> (<strong>l</strong>ower <strong>t</strong>han, in two’s complement). This is the opposite of GE, so when V and N are not both enabled or disabled (V is not N)
</li><li><code>GT</code> (<strong>g</strong>reather <strong>t</strong>han, in two’s complement). When Z is disabled and N and V are both enabled or disabled (Z is 0, N is V)
</li><li><code>LE</code> (<strong>l</strong>ower or <strong>e</strong>qual than, in two’s complement). When Z is enabled or if not that, N and V are both enabled or disabled (Z is 1. If Z is not 1 then N is V)
</li><li><code>MI</code> (<strong>mi</strong>nus/negative) When N is enabled (N is 1)
</li><li><code>PL</code> (<strong>pl</strong>us/positive or zero) When N is disabled (N is 0)
</li><li><code>VS</code> (o<strong>v</strong>erflow <strong>s</strong>et) When V is enabled (V is 1)
</li><li><code>VC</code> (o<strong>v</strong>erflow <strong>c</strong>lear) When V is disabled (V is 0)
</li><li><code>HI</code> (<strong>hi</strong>gher) When C is enabled and Z is disabled (C is 1 and Z is 0)
</li><li><code>LS</code> (<strong>l</strong>ower or <strong>s</strong>ame) When C is disabled or Z is enabled (C is 0 or Z is 1)
</li><li><code>CS</code>/<code>HS</code> (<strong>c</strong>arry <strong>s</strong>et/<strong>h</strong>igher or <strong>s</strong>ame) When C is enabled (C is 1)
</li><li><code>CC</code>/<code>LO</code> (<strong>c</strong>arry <strong>c</strong>lear/<strong>lo</strong>wer) When C is disabled (C is 0)
</li></ul>
<p>
These conditions can be combined to our <code>b</code> instruction to generate new instructions. This way, <code>beq</code> will branch only if <code>Z</code> is 1. If the condition of a conditional branch is not met, then the branch is ignored and the next instruction will be run. It is the programmer task to make sure that the condition codes are properly set prior a conditional branch.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> <span style="color: #339933;">--</span> compare01<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #0000ff; font-weight: bold;">.text</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">global</span> main
main<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span>       <span style="color: #339933;">/*</span> r1 ← <span style="color: #ff0000;">2</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span>       <span style="color: #339933;">/*</span> r2 ← <span style="color: #ff0000;">2</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">cmp</span> r1<span style="color: #339933;">,</span> r2       <span style="color: #339933;">/*</span> update cpsr condition codes with the value of r1<span style="color: #339933;">-</span>r2 <span style="color: #339933;">*/</span>
    beq case_equal   <span style="color: #339933;">/*</span> branch to case_equal only if Z = <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
case_different <span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span>       <span style="color: #339933;">/*</span> r0 ← <span style="color: #ff0000;">2</span> <span style="color: #339933;">*/</span>
    b end            <span style="color: #339933;">/*</span> branch to end <span style="color: #339933;">*/</span>
case_equal<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>       <span style="color: #339933;">/*</span> r0 ← <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
end<span style="color: #339933;">:</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* -- compare01.s */
.text
.global main
main:
    mov r1, #2       /* r1 ← 2 */
    mov r2, #2       /* r2 ← 2 */
    cmp r1, r2       /* update cpsr condition codes with the value of r1-r2 */
    beq case_equal   /* branch to case_equal only if Z = 1 */
case_different :
    mov r0, #2       /* r0 ← 2 */
    b end            /* branch to end */
case_equal:
    mov r0, #1       /* r0 ← 1 */
end:
    bx lr</p></div>

<p>
If you run this program it will return an error code of 1 because both <code>r1</code> and <code>r2</code> have the same value. Now change <code>mov r1, #2</code> in line 5 to be <code>mov r1, #3</code> and the returned error code should be 2. Note that  <code>case_different</code> we do not want to run the <code>case_equal</code> instructions, thus we have to branch to <code>end</code> (otherwise the error code would always be 1).
</p>
<p>
That’s all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/&amp;text=ARM+assembler+in+Raspberry+Pi+%E2%80%93+Chapter+5+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>				<p class="tags"><span><a href="http://thinkingeek.com/tag/arm/" rel="tag">arm</a>, <a href="http://thinkingeek.com/tag/assembler/" rel="tag">assembler</a>, <a href="http://thinkingeek.com/tag/branches/" rel="tag">branches</a>, <a href="http://thinkingeek.com/tag/pi/" rel="tag">pi</a>, <a href="http://thinkingeek.com/tag/raspberry/" rel="tag">raspberry</a></span></p>		<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2013/01/12/arm-assembler-raspberry-pi-chapter-4/" rel="prev">ARM assembler in Raspberry Pi – Chapter 4</a></span>
			<span class="next"><a href="http://thinkingeek.com/2013/01/20/arm-assembler-raspberry-pi-chapter-6/" rel="next">ARM assembler in Raspberry Pi – Chapter 6</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
			<h3>22 thoughts on “<span>ARM assembler in Raspberry Pi – Chapter 5</span>”</h3>
		<ul class="commentlist">
				<li class="comment even thread-even depth-1 parent" id="comment-888">
				<div id="div-comment-888" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/d0aa9580a37a577bb8553fb65f396456?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/d0aa9580a37a577bb8553fb65f396456?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-d0aa9580a37a577bb8553fb65f396456-0" originals="32" src-orig="http://1.gravatar.com/avatar/d0aa9580a37a577bb8553fb65f396456?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn"><a href="http://freelcs.sourceforge.net" rel="external nofollow" class="url">Mikael Hartzell</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/#comment-888">
			February 7, 2013 at 11:25 pm</a>		</div>

		<p>Hi </p>
<p>I though that you can manipulate the cpsr directly, isn’t the commands MSR and MRS meant for that ? (Source: Arm v6 reference manual: <a href="http://www.scss.tcd.ie/~waldroj/3d1/arm_arm.pdf" rel="nofollow">http://www.scss.tcd.ie/~waldroj/3d1/arm_arm.pdf</a>)</p>
<p>The text says that direct manipulation of cpsr is not possible.</p>
<p>Thanks for the great series, I have got so much out of it. I started by studying the arm instructions at <a href="http://www.davespace.co.uk/arm/introduction-to-arm/index.html" rel="nofollow">http://www.davespace.co.uk/arm/introduction-to-arm/index.html</a> and after that your article series adds to that information nicely by showing how to use the commands in real life </p>
<p>Keep up the good work </p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/?replytocom=888#respond" onclick="return addComment.moveForm( &quot;div-comment-888&quot;, &quot;888&quot;, &quot;respond&quot;, &quot;447&quot; )" aria-label="Reply to Mikael Hartzell">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-895">
				<div id="div-comment-895" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-a779b8290b1ca104fdf84d8016fd010b-0" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/#comment-895">
			February 8, 2013 at 11:27 pm</a>		</div>

		<p>Hi Mikael,</p>
<p>thanks for the comment! </p>
<p>You are right. I’ll reword the text, just to make clear that while it can be modified it is out of the scope of this chapter.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/?replytocom=895#respond" onclick="return addComment.moveForm( &quot;div-comment-895&quot;, &quot;895&quot;, &quot;respond&quot;, &quot;447&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1" id="comment-1085">
				<div id="div-comment-1085" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-9e81362ff9654533accffc53762c9520-0" originals="32" src-orig="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Fernando</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/#comment-1085">
			March 21, 2013 at 11:12 pm</a>		</div>

		<p>Still reading… very nice tutorial, I must say. I think I’m really learning a lot, just by playing with your examples and reading the text.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/?replytocom=1085#respond" onclick="return addComment.moveForm( &quot;div-comment-1085&quot;, &quot;1085&quot;, &quot;respond&quot;, &quot;447&quot; )" aria-label="Reply to Fernando">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment odd alt thread-even depth-1 parent" id="comment-1281">
				<div id="div-comment-1281" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/259bc0bc4762f6a53a9807d0289ba730?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/259bc0bc4762f6a53a9807d0289ba730?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-259bc0bc4762f6a53a9807d0289ba730-0" originals="32" src-orig="http://2.gravatar.com/avatar/259bc0bc4762f6a53a9807d0289ba730?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Damien</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/#comment-1281">
			May 12, 2013 at 1:05 am</a>		</div>

		<p>I tried modifying the program counter directly, but it doesn’t do what I expect:<br>
<code><br>
main:<br>
    mov r0, #1<br>
    add pc, pc, #4<br>
    mov r0, #2<br>
    mov r0, #3<br>
end:<br>
    bx lr<br>
</code></p>
<p>This gives me 1 (it jumped to end directly, while I was expecting that the r0←3 would get executed (I shifted the normal behavior of pc by the length of one instruction only, not two)</p>
<p>Another hypothesis I had was that since it’s modifying pc, it would not increment it, but I guess only the branching instructions have that behavior…</p>
<p>Any insights?</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/?replytocom=1281#respond" onclick="return addComment.moveForm( &quot;div-comment-1281&quot;, &quot;1281&quot;, &quot;respond&quot;, &quot;447&quot; )" aria-label="Reply to Damien">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2 parent" id="comment-1282">
				<div id="div-comment-1282" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-a779b8290b1ca104fdf84d8016fd010b-1" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/#comment-1282">
			May 12, 2013 at 8:43 am</a>		</div>

		<p>Hi Damien,</p>
<p>you are experiencing what, in my opinion, is a «quirk» in the ARM architecture (this is: the <em>contract</em> between the CPU designer and the software developer on how the CPU behaves). </p>
<p>Ideally one would expect, when reading the <code>pc</code> register in an instruction, to have the address of the current instruction. </p>
<p>Imagine that the instruction <code>add pc, pc, #4</code> is in address 0x1000. You would expect, at the end of the instruction <code>pc</code> be 0x1004. As usual in ARM, since <code>pc</code> got modified in the instruction, you would not add 4 bytes to it (as in implicit sequencing) but directly jump to 0x1004. So the next instruction run would be the one at the address 0x1004.</p>
<p>Well, this is where the ARM quirk comes into play. When you read the <code>pc</code> register in an instruction its value is the <b>current instruction plus 8 bytes</b>. </p>
<p>For instance, the following code,<br>
<code><br>
mov r1, #0<br>
current: mov pc, pc<br>
plus4: add r1, r1, #1<br>
plus8: add r1, r1, #2<br>
plus12: add r1, r1, #3<br>
end:<br>
</code></p>
<p>Here <code>r1</code> will at <code>end</code> have the value 5 (2+3) instead of 6 (1+2+3). Why? Because in instruction <code>mov pc, pc</code>, pc did not have the address <code>current</code>, it was <code>current + 8</code> which in the example is <code>plus8</code>. Since the instruction <strong>does modify <code>pc</code></strong>, the ARM processor does not do <code>pc ← pc + 4</code> before starting the next instruction but just keeps the <code>pc</code> as is. So by simply updating the <code>pc</code> to itself we were able to skip 1 instruction.</p>
<p>This is what is happening to you: in the <code>add pc, pc, #4</code> instruction you are reading a <code>pc</code> of the instruction <code>mov r0, #3</code>. If you add to it 4 more bytes, it is the address of the <code>bx lr</code>.</p>
<p>This quirk may be a bit annoying, just remember that when you directly read <code>pc</code> it will always be the current instruction plus 8. </p>
<p>Is this a problem most of the time? No, if you use labels in your branches, the assembler internally fixes everything for you.</p>
<p>I cannot explain the reason of this behaviour in ARM. I think this issue has historical roots in the earlier ARM designs where it probably happened that the <code>pc</code> was read at a point in the processor state where it had already been implicitly advanced by 8 bytes. This seems to be a very ARM specific thing (a similar sequence of code like the one above in other architectures would set <code>r1</code> to 6).</p>
<p>I hope this answers your question.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/?replytocom=1282#respond" onclick="return addComment.moveForm( &quot;div-comment-1282&quot;, &quot;1282&quot;, &quot;respond&quot;, &quot;447&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment odd alt depth-3 parent" id="comment-1349">
				<div id="div-comment-1349" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/588d8ef87925eb384a1589bc451a2690?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/588d8ef87925eb384a1589bc451a2690?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-588d8ef87925eb384a1589bc451a2690-0" originals="32" src-orig="http://2.gravatar.com/avatar/588d8ef87925eb384a1589bc451a2690?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">mirz</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/#comment-1349">
			May 26, 2013 at 10:03 pm</a>		</div>

		<p>AFAIK, this behaviour is due to pipelining, isn’t it?</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/?replytocom=1349#respond" onclick="return addComment.moveForm( &quot;div-comment-1349&quot;, &quot;1349&quot;, &quot;respond&quot;, &quot;447&quot; )" aria-label="Reply to mirz">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-4" id="comment-1358">
				<div id="div-comment-1358" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-a779b8290b1ca104fdf84d8016fd010b-2" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/#comment-1358">
			May 28, 2013 at 5:54 pm</a>		</div>

		<p>I think so. </p>
<p>My guess is that in earlier (and simpler) iterations of the ARM architecture in the <em>alu</em> stage when you read the <code>pc</code> you were reading the <code>pc</code> of the instruction-after-the-next-one (the value that the <em>physical</em> register had at that stage).</p>
<p>Probably ARM had to preserve this architectural behaviour in later versions of the architecture, so the quirk remained. Note, though, there is no technical reason that prevents the <em>alu</em> stage to have the address of the current instruction when reading the operands.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/?replytocom=1358#respond" onclick="return addComment.moveForm( &quot;div-comment-1358&quot;, &quot;1358&quot;, &quot;respond&quot;, &quot;447&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1" id="comment-1287">
				<div id="div-comment-1287" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/259bc0bc4762f6a53a9807d0289ba730?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/259bc0bc4762f6a53a9807d0289ba730?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-259bc0bc4762f6a53a9807d0289ba730-1" originals="32" src-orig="http://2.gravatar.com/avatar/259bc0bc4762f6a53a9807d0289ba730?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Damien</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/#comment-1287">
			May 13, 2013 at 4:03 pm</a>		</div>

		<p>Yup, thanks </p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/?replytocom=1287#respond" onclick="return addComment.moveForm( &quot;div-comment-1287&quot;, &quot;1287&quot;, &quot;respond&quot;, &quot;447&quot; )" aria-label="Reply to Damien">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1 parent" id="comment-110792">
				<div id="div-comment-110792" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/15bf4b40214bb6e542b3c90722ec2bd1?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/15bf4b40214bb6e542b3c90722ec2bd1?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-15bf4b40214bb6e542b3c90722ec2bd1-0" originals="32" src-orig="http://1.gravatar.com/avatar/15bf4b40214bb6e542b3c90722ec2bd1?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Randy</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/#comment-110792">
			April 2, 2014 at 1:00 pm</a>		</div>

		<p>FYI, I think there is an error in this tutorial.</p>
<p>For BGE, the branch condition is when N=V, not N=Z.</p>
<p>I looked it up in the ARM documentation when I considered that N=Z should be impossible.  Zero has a sign bit of zero, and any calculation that results in zero should not have the N bit set.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/?replytocom=110792#respond" onclick="return addComment.moveForm( &quot;div-comment-110792&quot;, &quot;110792&quot;, &quot;respond&quot;, &quot;447&quot; )" aria-label="Reply to Randy">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-110795">
				<div id="div-comment-110795" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-a779b8290b1ca104fdf84d8016fd010b-3" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/#comment-110795">
			April 5, 2014 at 9:40 pm</a>		</div>

		<p>Hi Randy,</p>
<p>you’re right. I made a typo in GE and then I propagated it to LT.</p>
<p>I fixed the post. Thanks a lot.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/?replytocom=110795#respond" onclick="return addComment.moveForm( &quot;div-comment-110795&quot;, &quot;110795&quot;, &quot;respond&quot;, &quot;447&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-150045">
				<div id="div-comment-150045" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/b642b4217b34b1e8d3bd915fc65c4452?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/b642b4217b34b1e8d3bd915fc65c4452?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-b642b4217b34b1e8d3bd915fc65c4452-0" originals="32" src-orig="http://2.gravatar.com/avatar/b642b4217b34b1e8d3bd915fc65c4452?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn"><a href="http://blahyo" rel="external nofollow" class="url">blahyo</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/#comment-150045">
			June 7, 2014 at 7:10 pm</a>		</div>

		<p>It should be “vs” instead of “os(overflow set)” I believe.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/?replytocom=150045#respond" onclick="return addComment.moveForm( &quot;div-comment-150045&quot;, &quot;150045&quot;, &quot;respond&quot;, &quot;447&quot; )" aria-label="Reply to blahyo">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-151442">
				<div id="div-comment-151442" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-a779b8290b1ca104fdf84d8016fd010b-4" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/#comment-151442">
			June 9, 2014 at 12:09 pm</a>		</div>

		<p>Yes, thanks. Another typo that I accidentally propagated. It should be right now.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/?replytocom=151442#respond" onclick="return addComment.moveForm( &quot;div-comment-151442&quot;, &quot;151442&quot;, &quot;respond&quot;, &quot;447&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1 parent" id="comment-950560">
				<div id="div-comment-950560" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/9b6de309a0c99eab0b048bf8f4408ead?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/9b6de309a0c99eab0b048bf8f4408ead?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-9b6de309a0c99eab0b048bf8f4408ead-0" originals="32" src-orig="http://0.gravatar.com/avatar/9b6de309a0c99eab0b048bf8f4408ead?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Smasher</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/#comment-950560">
			November 10, 2015 at 5:30 pm</a>		</div>

		<p>I think there is a mistake in this chapter. According to ARM Documentation the condition not-equal is NE and not NEQ.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/?replytocom=950560#respond" onclick="return addComment.moveForm( &quot;div-comment-950560&quot;, &quot;950560&quot;, &quot;respond&quot;, &quot;447&quot; )" aria-label="Reply to Smasher">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-950588">
				<div id="div-comment-950588" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-a779b8290b1ca104fdf84d8016fd010b-5" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/#comment-950588">
			November 10, 2015 at 8:15 pm</a>		</div>

		<p>Hi Smasher,</p>
<p>you are right. I’ve already fixed the post.</p>
<p>Thank you very much.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/?replytocom=950588#respond" onclick="return addComment.moveForm( &quot;div-comment-950588&quot;, &quot;950588&quot;, &quot;respond&quot;, &quot;447&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-950725">
				<div id="div-comment-950725" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/9b6de309a0c99eab0b048bf8f4408ead?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/9b6de309a0c99eab0b048bf8f4408ead?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-9b6de309a0c99eab0b048bf8f4408ead-1" originals="32" src-orig="http://0.gravatar.com/avatar/9b6de309a0c99eab0b048bf8f4408ead?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Smasher</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/#comment-950725">
			November 11, 2015 at 12:45 pm</a>		</div>

		<p>Hey Roger,</p>
<p>I noticed this because “bneq end_of_loop” threw an error. </p>
<p>I am glad to make a small contribution to your great tutorial, which I am going through with excitement and pleasure by learning asm </p>
<p>I also hope the subject will stay understandable for me to the last chapter. May I contact you If some questions will remain open after that?</p>
<p>Regards.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/?replytocom=950725#respond" onclick="return addComment.moveForm( &quot;div-comment-950725&quot;, &quot;950725&quot;, &quot;respond&quot;, &quot;447&quot; )" aria-label="Reply to Smasher">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment odd alt depth-2 parent" id="comment-950726">
				<div id="div-comment-950726" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/9b6de309a0c99eab0b048bf8f4408ead?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/9b6de309a0c99eab0b048bf8f4408ead?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-9b6de309a0c99eab0b048bf8f4408ead-2" originals="32" src-orig="http://0.gravatar.com/avatar/9b6de309a0c99eab0b048bf8f4408ead?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Smasher</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/#comment-950726">
			November 11, 2015 at 12:46 pm</a>		</div>

		<p>This should be an answer to your answer above so you may want to move that comment.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/?replytocom=950726#respond" onclick="return addComment.moveForm( &quot;div-comment-950726&quot;, &quot;950726&quot;, &quot;respond&quot;, &quot;447&quot; )" aria-label="Reply to Smasher">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-3" id="comment-950780">
				<div id="div-comment-950780" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-a779b8290b1ca104fdf84d8016fd010b-6" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/#comment-950780">
			November 11, 2015 at 7:17 pm</a>		</div>

		<p>No worries.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/?replytocom=950780#respond" onclick="return addComment.moveForm( &quot;div-comment-950780&quot;, &quot;950780&quot;, &quot;respond&quot;, &quot;447&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-950778">
				<div id="div-comment-950778" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-a779b8290b1ca104fdf84d8016fd010b-7" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/#comment-950778">
			November 11, 2015 at 7:14 pm</a>		</div>

		<p>Hi Smasher,</p>
<p>yes of course. Feel free to ask in the comments section of each chapter where you have questions.</p>
<p>I also hope next chapters are easy to understand as well </p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/?replytocom=950778#respond" onclick="return addComment.moveForm( &quot;div-comment-950778&quot;, &quot;950778&quot;, &quot;respond&quot;, &quot;447&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1 parent" id="comment-951917">
				<div id="div-comment-951917" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/ab7007220332623f94c643e0d284f1b4?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/ab7007220332623f94c643e0d284f1b4?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-ab7007220332623f94c643e0d284f1b4-0" originals="32" src-orig="http://1.gravatar.com/avatar/ab7007220332623f94c643e0d284f1b4?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Matt Miller</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/#comment-951917">
			November 17, 2015 at 4:44 am</a>		</div>

		<p>The work subtraction is spelled substraction in this post.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/?replytocom=951917#respond" onclick="return addComment.moveForm( &quot;div-comment-951917&quot;, &quot;951917&quot;, &quot;respond&quot;, &quot;447&quot; )" aria-label="Reply to Matt Miller">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-951928">
				<div id="div-comment-951928" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-a779b8290b1ca104fdf84d8016fd010b-8" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/#comment-951928">
			November 17, 2015 at 6:45 am</a>		</div>

		<p>Hi Matt,</p>
<p>Oops! I scattered this typo all over the blog.</p>
<p>Thank you for the heads-up.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/?replytocom=951928#respond" onclick="return addComment.moveForm( &quot;div-comment-951928&quot;, &quot;951928&quot;, &quot;respond&quot;, &quot;447&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-963942">
				<div id="div-comment-963942" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/e5a9109af6d59bf9d23ec3d613216623?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/e5a9109af6d59bf9d23ec3d613216623?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-e5a9109af6d59bf9d23ec3d613216623-0" originals="32" src-orig="http://2.gravatar.com/avatar/e5a9109af6d59bf9d23ec3d613216623?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">John Ganci</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/#comment-963942">
			April 5, 2016 at 4:11 am</a>		</div>

		<p>In the “Unconditional branches” section, the code sample is named branch01.s. However, the sample execution that follows the code is</p>
<p>$ ./compare01 ; echo $?</p>
<p>Should be ./branch01 ; echo $?</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/?replytocom=963942#respond" onclick="return addComment.moveForm( &quot;div-comment-963942&quot;, &quot;963942&quot;, &quot;respond&quot;, &quot;447&quot; )" aria-label="Reply to John Ganci">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-963948">
				<div id="div-comment-963948" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-a779b8290b1ca104fdf84d8016fd010b-9" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/#comment-963948">
			April 5, 2016 at 5:24 am</a>		</div>

		<p>Hi John,</p>
<p>thank you. I already fixed the post.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/19/arm-assembler-raspberry-pi-chapter-5/?replytocom=963948#respond" onclick="return addComment.moveForm( &quot;div-comment-963948&quot;, &quot;963948&quot;, &quot;respond&quot;, &quot;447&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		</ul>
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2013/01/19/arm-assembler-raspberry-pi-chapter-5/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="447" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="ce607160a6"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496597702416"></form>
			</div><!-- #respond -->
	</div>	</div>
