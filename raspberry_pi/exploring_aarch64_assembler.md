# Exploring AArch64 assembler


<div class="main">
			<h1>Exploring AArch64 assembler – Chapter 1 </h1>
		<p class="post-meta"><span class="date">October 8, 2016</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/#comments">14</a></span> </p>
		<p>
AArch64 is a new 64 bit mode that is part of the ARMv8 architecture presented in 2011 by ARM. It has been progressively been deployed in smartphones and servers. So I think it is a good moment to learn a bit more about the assembler of this architecture.
</p>
<p><span id="more-3156"></span></p>
<h2>Hardware availability</h2>
<p>
Single board computers with ARMv6/ARMv7 architecture are easily available nowadays. One of the most popular choices is the <a href="https://www.raspberrypi.org/">Raspberry Pi</a>.
</p>
<p>
In contrast, single-board computers that support the 64-bit mode of ARMv8 are less common but they are slowly becoming more popular these days. For instance the <a href="https://shop.pine64.com/">Pine64</a>, the <a href="http://www.hardkernel.com/main/products/prdt_info.php?g_code=G145457216438">ODROID-C2</a>, the <a href="https://developer.qualcomm.com/hardware/dragonboard-410c">Dragonboard 410c</a>, etc. Any of them will do and in general they differ on the specific System on Chip being used.
</p>
<div style="padding: 1em; background-color: #fee">
<strong>Note</strong>: the Raspberry Pi 3 has a CPU (Cortex-A53) that implements the 64-bit mode of the ARMv8 architecture and technically could run a 64-bit system. But the software system provided by the Raspberry Foundation (<a href="https://www.raspberrypi.org/downloads/raspbian/">Raspbian</a>) is only for 32-bit and there are no official plans for a 64-bit system.
</div>
<p></p>
<div style="padding: 1em; background-color: #efe">
<strong>Update</strong>: SuSE has a <a href="https://en.opensuse.org/HCL:Raspberry_Pi3">64-bit version of its OpenSuSE distribution</a> that can run in the Raspberry Pi 3. Arch <a href="https://archlinuxarm.org/platforms/armv8/broadcom/raspberry-pi-3">also has a 64-bit distribution</a> that can be installed in the RPi3.
</div>
<h2>Software alternative</h2>
<p>
Does this mean that without hardware it is not possible to play with AArch64? No! We can still do many things using a cross-toolchain and <a href="http://wiki.qemu.org/">QEMU</a> in user mode.
</p>
<h3>Example for Ubuntu 16.04</h3>
<p>
Just install QEMU and a cross-toolchain for AArch64.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="bash" style="font-family:monospace;"><span style="color: #666666;">$ </span><span style="color: #c20cb9; font-weight: bold;">sudo</span> <span style="color: #c20cb9; font-weight: bold;">apt-get install</span> qemu-user gcc-aarch64-linux-gnu</pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ sudo apt-get install qemu-user gcc-aarch64-linux-gnu</p></div>

<p>
Now test you can run a “Hello world” written in C. Create a <code>hello.c</code> file with the following contents.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #339933;">#include &lt;stdio.h&gt;</span>
&nbsp;
<span style="color: #993333;">int</span> main<span style="color: #009900;">(</span><span style="color: #993333;">int</span> argc<span style="color: #339933;">,</span> <span style="color: #993333;">char</span> <span style="color: #339933;">*</span>argv<span style="color: #009900;">[</span><span style="color: #009900;">]</span><span style="color: #009900;">)</span>
<span style="color: #009900;">{</span>
  <span style="color: #000066;">printf</span><span style="color: #009900;">(</span><span style="color: #ff0000;">"Hello AArch64!<span style="color: #000099; font-weight: bold;">\n</span>"</span><span style="color: #009900;">)</span><span style="color: #339933;">;</span>
  <span style="color: #b1b100;">return</span> <span style="color: #0000dd;">0</span><span style="color: #339933;">;</span>
<span style="color: #009900;">}</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">#include &lt;stdio.h&gt;

int main(int argc, char *argv[])
{
  printf("Hello AArch64!\n");
  return 0;
}</p></div>

<p>
Now compile it with the cross-compiler for AArch64 that we have installed earlier (the <strong><code>-static</code></strong> flag is important).
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="bash" style="font-family:monospace;"><span style="color: #666666;">$ </span>aarch64-linux-gnu-gcc <span style="color: #660033;">-static</span> <span style="color: #660033;">-o</span> hello hello.c</pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ aarch64-linux-gnu-gcc -static -o hello hello.c</p></div>

<p>
Check it is a AArch64 binary.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;">$ file hello
hello<span style="color: #339933;">:</span> ELF <span style="color: #0000dd;">64</span><span style="color: #339933;">-</span>bit LSB executable<span style="color: #339933;">,</span> ARM aarch64<span style="color: #339933;">,</span> version <span style="color: #0000dd;">1</span> <span style="color: #009900;">(</span>SYSV<span style="color: #009900;">)</span><span style="color: #339933;">,</span> statically linked<span style="color: #339933;">,</span> <span style="color: #b1b100;">for</span> GNU<span style="color: #339933;">/</span>Linux 3.7.0<span style="color: #339933;">,</span> BuildID<span style="color: #009900;">[</span>sha1<span style="color: #009900;">]</span><span style="color: #339933;">=</span>97c2bc66dbe4393aab9e4885df8e223a6baa235a<span style="color: #339933;">,</span> not stripped</pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ file hello
hello: ELF 64-bit LSB executable, ARM aarch64, version 1 (SYSV), statically linked, for GNU/Linux 3.7.0, BuildID[sha1]=97c2bc66dbe4393aab9e4885df8e223a6baa235a, not stripped</p></div>

<p>
Trying to run it should fail with some confusing error.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="bash" style="font-family:monospace;">$ .<span style="color: #000000; font-weight: bold;">/</span>hello 
-bash: .<span style="color: #000000; font-weight: bold;">/</span>hello: No such <span style="color: #c20cb9; font-weight: bold;">file</span> or directory</pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ ./hello 
-bash: ./hello: No such file or directory</p></div>

<p>
But we can run it using the QEMU for AArch64 that we installed earlier.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="bash" style="font-family:monospace;">$ qemu-aarch64 .<span style="color: #000000; font-weight: bold;">/</span>hello
Hello AArch64<span style="color: #000000; font-weight: bold;">!</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ qemu-aarch64 ./hello
Hello AArch64!</p></div>

<p>
Yay!
</p>
<div style="padding: 1em; background-color: #fee">
<strong>Note</strong>: If you use this option, remember always to run your programs using <code>qemu-aarch64</code>.
</div>
<h2>Our first AArch64 assembler program</h2>
<p>
Let’s write a very simple program that just returns an error code of two.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>1
2
3
4
5
6
7
8
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">//</span> first<span style="color: #339933;">.</span>s
<span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
<span style="color: #339933;">.</span>globl main
&nbsp;
main<span style="color: #339933;">:</span>
     <span style="color: #00007f; font-weight: bold;">mov</span> w0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span>
     <span style="color: #00007f; font-weight: bold;">ret</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">// first.s
.text

.globl main

main:
     mov w0, #2
     ret</p></div>

<p>
Let’s assemble it.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="bash" style="font-family:monospace;"><span style="color: #666666;">$ </span>aarch64-linux-gnu-as <span style="color: #660033;">-c</span> first.s</pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ aarch64-linux-gnu-as -c first.s</p></div>

<p>
And now link it, for convenience we will use <code>gcc</code>.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="bash" style="font-family:monospace;"><span style="color: #666666;">$ </span>aarch64-linux-gnu-gcc <span style="color: #660033;">-static</span> <span style="color: #660033;">-o</span> first first.o</pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ aarch64-linux-gnu-gcc -static -o first first.o</p></div>

<p>
Run it and check the return.</p>
<p>
</p><div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="bash" style="font-family:monospace;">$ .<span style="color: #000000; font-weight: bold;">/</span>first             <span style="color: #666666; font-style: italic;"># or use qemu-aarch64 ./first</span>
$ <span style="color: #7a0874; font-weight: bold;">echo</span> <span style="color: #007800;">$?</span>
<span style="color: #000000;">2</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ ./first             # or use qemu-aarch64 ./first
$ echo $?
2</p></div>

<p>
Yay!
</p>
<p>
Let’s go through each line of the code above.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>1
2
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">//</span> first<span style="color: #339933;">.</span>s
<span style="color: #0000ff; font-weight: bold;">.text</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">// first.s
.text</p></div>

<p>
Line 1 is just a comment with the name of the file used in this example. Any text in a line that follows a <code>//</code> is a comment and it is ignored. Line 2 is an assembler directive that means “now come instructions of the program”. This is because we can also express data in an assembler file (data goes after a <code>.data</code> directive).
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>4
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">.</span>globl main</pre></td></tr></tbody></table><p class="theCode" style="display:none;">.globl main</p></div>

<p>
This is another assembler directive that means <code>main</code> is going to be a <em>global</em> symbol. This means that when constructing the final program, this file will have the global <code>main</code> symbol that is needed by the C library to start a program.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>6
7
8
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">main<span style="color: #339933;">:</span>
     <span style="color: #00007f; font-weight: bold;">mov</span> w0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span> <span style="color: #339933;">//</span> w0 ← <span style="color: #ff0000;">2</span>
     <span style="color: #00007f; font-weight: bold;">ret</span>        <span style="color: #339933;">//</span> return</pre></td></tr></tbody></table><p class="theCode" style="display:none;">main:
     mov w0, #2 // w0 ← 2
     ret        // return</p></div>

<p>
This is the entry point of our program. Line 6 itself is just a label for the symbol <code>main</code> (that we mentioned above it was a global symbol). Lines 7 and 8 are two instructions. The first one just sets the register <code>w0</code> to be 2 (we will see what registers are in the next chapter). The second one returns from the <code>main</code>, effectively finishing our program.
</p>
<p>
When finishing a program, the contents of the register <code>w0</code> are used to determine the error code of the program. This is the reason why <code>echo $?</code> above prints <code>2</code>.
</p>
<h2>Reference documentation</h2>
<p>
Documentation for the AArch64 instruction set can be found in the <a href="https://developer.arm.com/docs/ddi0487/a/arm-architecture-reference-manual-armv8-for-armv8-a-architecture-profile">ARM® Architecture Reference Manual ARMv8, for ARMv8-A architecture profile</a></p>
<p>
That’s all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/&amp;text=Exploring+AArch64+assembler+%E2%80%93+Chapter+1+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>						<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2016/09/05/tiny-gcc-front-part-11/" rel="prev">A tiny GCC front end – Part 11</a></span>
			<span class="next"><a href="http://thinkingeek.com/2016/10/08/not-write-c-parser-part-1/" rel="next">How (not) to write a C++ front end – Part 1</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
			<h3>14 thoughts on “<span>Exploring AArch64 assembler – Chapter 1</span>”</h3>
		<ul class="commentlist">
				<li class="comment even thread-even depth-1 parent" id="comment-976674">
				<div id="div-comment-976674" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/b06feeb5f27ff3b8ffbb008fae5b304a?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/b06feeb5f27ff3b8ffbb008fae5b304a?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-b06feeb5f27ff3b8ffbb008fae5b304a-0" originals="32" src-orig="http://2.gravatar.com/avatar/b06feeb5f27ff3b8ffbb008fae5b304a?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn"><a href="https://github.com/Lichtso/UnikernelExperiments" rel="external nofollow" class="url">Alexander Meißner</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/#comment-976674">
			October 10, 2016 at 6:35 pm</a>		</div>

		<p>For those who are interested in running your software on AArch64 hardware directly, without any additional firmware or operating system in between, I’ve developed a boot loader and tool chain to test and execute unikernels on a Pine64.</p>
<p>Nice to see there are other people who are into the “deeper” layers and ARM as well.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/?replytocom=976674#respond" onclick="return addComment.moveForm( &quot;div-comment-976674&quot;, &quot;976674&quot;, &quot;respond&quot;, &quot;3156&quot; )" aria-label="Reply to Alexander Meißner">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-976980">
				<div id="div-comment-976980" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-0">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/#comment-976980">
			October 15, 2016 at 12:01 pm</a>		</div>

		<p>Hi Alexander,</p>
<p>good to know! Thanks.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/?replytocom=976980#respond" onclick="return addComment.moveForm( &quot;div-comment-976980&quot;, &quot;976980&quot;, &quot;respond&quot;, &quot;3156&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-976686">
				<div id="div-comment-976686" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/098597ee293c3628de503d0a2c11da57?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/098597ee293c3628de503d0a2c11da57?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/098597ee293c3628de503d0a2c11da57?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-098597ee293c3628de503d0a2c11da57-0">			<cite class="fn">B2B</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/#comment-976686">
			October 10, 2016 at 10:45 pm</a>		</div>

		<p>Very nice. Looking forward for more to come.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/?replytocom=976686#respond" onclick="return addComment.moveForm( &quot;div-comment-976686&quot;, &quot;976686&quot;, &quot;respond&quot;, &quot;3156&quot; )" aria-label="Reply to B2B">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-976984">
				<div id="div-comment-976984" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-1">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/#comment-976984">
			October 15, 2016 at 12:38 pm</a>		</div>

		<p>Thanks!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/?replytocom=976984#respond" onclick="return addComment.moveForm( &quot;div-comment-976984&quot;, &quot;976984&quot;, &quot;respond&quot;, &quot;3156&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1 parent" id="comment-980162">
				<div id="div-comment-980162" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/1567d9eec2b29932cd4e1be2c5f64dac?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/1567d9eec2b29932cd4e1be2c5f64dac?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/1567d9eec2b29932cd4e1be2c5f64dac?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-1567d9eec2b29932cd4e1be2c5f64dac-0">			<cite class="fn">Luis Guilherme</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/#comment-980162">
			November 23, 2016 at 7:46 pm</a>		</div>

		<p>Hope to get all the way to interface with a framebuffer (maybe Raspberry PI) and i/o other than GPIOs :o)</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/?replytocom=980162#respond" onclick="return addComment.moveForm( &quot;div-comment-980162&quot;, &quot;980162&quot;, &quot;respond&quot;, &quot;3156&quot; )" aria-label="Reply to Luis Guilherme">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-980593">
				<div id="div-comment-980593" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-2">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/#comment-980593">
			November 27, 2016 at 1:02 pm</a>		</div>

		<p>So cool <img draggable="false" class="emoji" alt="🙂" src="https://s.w.org/images/core/emoji/2.2.1/svg/1f642.svg" scale="0"> Let us know if you succeed!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/?replytocom=980593#respond" onclick="return addComment.moveForm( &quot;div-comment-980593&quot;, &quot;980593&quot;, &quot;respond&quot;, &quot;3156&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-983990">
				<div id="div-comment-983990" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/d847bfb8fd192ef711ae2daf48ac49be?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/d847bfb8fd192ef711ae2daf48ac49be?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-d847bfb8fd192ef711ae2daf48ac49be-0" originals="32" src-orig="http://1.gravatar.com/avatar/d847bfb8fd192ef711ae2daf48ac49be?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Javi</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/#comment-983990">
			January 12, 2017 at 11:42 pm</a>		</div>

		<p>Hi, i’m interested in code an operating system from scratch for my Raspberry Pi 3, the idea is starting doing something like Baking Pi. Due to your experience in this topic, do you see this project feasible? Should I use AArch64 assembly for program my PI? What are resources should I look for (because baking pi is for Raspberry Pi 1 and not very extensive). I hope for your answer, very nice tutorial BTW.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/?replytocom=983990#respond" onclick="return addComment.moveForm( &quot;div-comment-983990&quot;, &quot;983990&quot;, &quot;respond&quot;, &quot;3156&quot; )" aria-label="Reply to Javi">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-984082">
				<div id="div-comment-984082" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-3">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/#comment-984082">
			January 14, 2017 at 4:12 pm</a>		</div>

		<p>Hi Javi,</p>
<p>this is not my field of expertise so I’m afraid I cannot give you any useful pointers or resources for an AArch64-based OS.</p>
<p>I’ll let you know if I learn of any project like this.</p>
<p>Kind regards,<br>
Roger</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/?replytocom=984082#respond" onclick="return addComment.moveForm( &quot;div-comment-984082&quot;, &quot;984082&quot;, &quot;respond&quot;, &quot;3156&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1 parent" id="comment-988233">
				<div id="div-comment-988233" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/3e8f190363cf6f1f7d4813c8e8b8c9e9?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/3e8f190363cf6f1f7d4813c8e8b8c9e9?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/3e8f190363cf6f1f7d4813c8e8b8c9e9?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-3e8f190363cf6f1f7d4813c8e8b8c9e9-0">			<cite class="fn">David Todd</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/#comment-988233">
			March 9, 2017 at 5:52 pm</a>		</div>

		<p>Roger, I worked through your ARMv7 tutorials on my Pi-1 several years ago and found them to be excellent, so I was very excited to see this series now that I have a Pi-3!  I’ve done three tutorials so far this morning!  Thank you SOOO much for doing these.  And I’ve enjoyed your other tutorials along the way.  Excellent work!  And much appreciated.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/?replytocom=988233#respond" onclick="return addComment.moveForm( &quot;div-comment-988233&quot;, &quot;988233&quot;, &quot;respond&quot;, &quot;3156&quot; )" aria-label="Reply to David Todd">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-988445">
				<div id="div-comment-988445" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-4">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/#comment-988445">
			March 12, 2017 at 1:43 pm</a>		</div>

		<p>Thanks David. I will be a bit slower with this one because I have a strong feeling of “explaining again the same just with 64-bit”. So I try to spend more time being a bit more creative introducing the material. Apologies if I’m slower with new chapters.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/?replytocom=988445#respond" onclick="return addComment.moveForm( &quot;div-comment-988445&quot;, &quot;988445&quot;, &quot;respond&quot;, &quot;3156&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-988239">
				<div id="div-comment-988239" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/3e8f190363cf6f1f7d4813c8e8b8c9e9?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/3e8f190363cf6f1f7d4813c8e8b8c9e9?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/3e8f190363cf6f1f7d4813c8e8b8c9e9?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-3e8f190363cf6f1f7d4813c8e8b8c9e9-1">			<cite class="fn">David Todd</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/#comment-988239">
			March 9, 2017 at 7:47 pm</a>		</div>

		<p>Roger, For those wanting to follow your tutorial under an OS running native on the RPi-3 hardware, it is possible now to do so.  A month after you published chapter 1, SUSE announced that they had ported SUSE Linux Enterprise Server (SLES 12.2) to the Raspberry Pi-3 as an aarch64 implementation [ <a href="https://www.suse.com/communities/blog/suse-linux-enterprise-server-raspberry-pi/" rel="nofollow">https://www.suse.com/communities/blog/suse-linux-enterprise-server-raspberry-pi/</a> ].  A variety of openSUSE aarch64 implementations for RPi-3 are also available [ <a href="https://en.opensuse.org/HCL:Raspberry_Pi3" rel="nofollow">https://en.opensuse.org/HCL:Raspberry_Pi3</a> ].   To test this out in a low-overhead manner, it is also possible to copy the image to a USB thumbdrive (or harddrive or SSD) and boot the RPi-3 from that USB device [  <a href="https://en.opensuse.org/HCL:Raspberry_Pi3" rel="nofollow">https://en.opensuse.org/HCL:Raspberry_Pi3</a> , see comments].</p>
<p>Some components haven’t been ported yet, and some don’t have full functionality.  But through your chapter 3, at least, the code compiles with gcc and runs, and gdb works (though without full debug info).  Though “perf” works, it doesn’t have all the counters implemented — just gives timings, at least on the openSUSE I’m running.</p>
<p>It appears that Fedora 26 will also have an aarch64 Raspberry Pi-3 implementation; last I saw, it was tentatively expected in June, 2017.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/?replytocom=988239#respond" onclick="return addComment.moveForm( &quot;div-comment-988239&quot;, &quot;988239&quot;, &quot;respond&quot;, &quot;3156&quot; )" aria-label="Reply to David Todd">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-988446">
				<div id="div-comment-988446" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-5">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/#comment-988446">
			March 12, 2017 at 1:45 pm</a>		</div>

		<p>Thanks David!</p>
<p>Looking forward better support of 64-bit software in the Raspberry Pi 3.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/?replytocom=988446#respond" onclick="return addComment.moveForm( &quot;div-comment-988446&quot;, &quot;988446&quot;, &quot;respond&quot;, &quot;3156&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1 parent" id="comment-991689">
				<div id="div-comment-991689" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/2f687f8751a41faab65bc0d669dae1cc?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/2f687f8751a41faab65bc0d669dae1cc?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-2f687f8751a41faab65bc0d669dae1cc-0" originals="32" src-orig="http://2.gravatar.com/avatar/2f687f8751a41faab65bc0d669dae1cc?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Chris Sears</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/#comment-991689">
			May 1, 2017 at 6:52 pm</a>		</div>

		<p>You can install a 64b Arch Linux on a Raspberry PI 3.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/?replytocom=991689#respond" onclick="return addComment.moveForm( &quot;div-comment-991689&quot;, &quot;991689&quot;, &quot;respond&quot;, &quot;3156&quot; )" aria-label="Reply to Chris Sears">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-992005">
				<div id="div-comment-992005" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-6">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/#comment-992005">
			May 7, 2017 at 9:02 am</a>		</div>

		<p>Hi Chris,</p>
<p>this is now true but it wasn’t when I wrote that post. I will amend it, though.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/08/exploring-aarch64-assembler-chapter1/?replytocom=992005#respond" onclick="return addComment.moveForm( &quot;div-comment-992005&quot;, &quot;992005&quot;, &quot;respond&quot;, &quot;3156&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		</ul>
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2016/10/08/exploring-aarch64-assembler-chapter1/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="3156" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="9bda82d449"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496668637597"></form>
			</div><!-- #respond -->
	</div>	</div>