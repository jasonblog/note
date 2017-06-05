# The operating system


<div class="main">
			<h1>ARM assembler in Raspberry Pi – Chapter 19 </h1>
		<p class="post-meta"><span class="date">May 24, 2014</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2014/05/24/arm-assembler-raspberry-pi-chapter-19/#comments">10</a></span> </p>
		<p>
So far our small assembler programs have output messages using <code>printf</code> and some of them have read input using <code>scanf</code>. These two functions are implemented in the C library, so they are more or less supported in any environment supporting the C language. But how does a program actually communicate with the world?
</p>
<p><span id="more-1518"></span></p>
<h2>The operating system</h2>
<p>
Our Raspberry Pi runs <a href="http://www.raspbian.org/">Raspbian</a>. Raspbian is an operating system based on <a href="http://www.debian.org/">Debian</a> on top of the <a href="http://www.linux.org">Linux</a> <a href="https://www.kernel.org/">kernel</a>. The operating system is a piece of software (usually a collection of pieces that together form a useful system) that enables and manages the resources required by programs to run. Which sort of resources, you may be wondering? Well, many different kinds of them: processes, files, network devices, network communications, screens, printers, terminals, timers, etc.
</p>
<p>
From the point of view of the program, the operating system is just a big servant providing lots of services to the program. But the operating system is also a caretaker, taking action when something goes wrong or programs (sometimes caused by the users of the operating system) attempt to do something that they are not authorized to do. In our case, Linux is the kernel of the Raspbian operating system. The kernel provides the most basic functionality needed to provide these services (sometimes it provides them directly, sometimes it just provides the minimal essential functionality so they can be implemented). It can be viewed as a foundational program that it is always running (or at least, always ready) so it can serve the requests of the programs run by the users. Linux is a <a href="http://en.wikipedia.org/wiki/Unix-like">UNIX®-like</a> kernel and as such shares lots of features with the long lineage of UNIX®-like operating systems.
</p>
<h3>Processes</h3>
<p>
In order to assign resources, the operating system needs an entity to which grant such resources. This entity is called a process. A process is a running program. The same program may be run several times, each time it is run it is a different process.
</p>
<h2>System calls</h2>
<p>
A process interacts with the operating system by performing <em>system calls</em>. A system call is conceptually like calling a function but more sophisticated. It is more sophisticated because now we need to satisfy some extra security requirements. An operating system is a critical part of a system and we cannot let processes <em>dodge</em> the operating system control. A usual function call offers no protection of any kind. Any strategy we could design on top of a plain function call would easily be possible to circumvent. As a consequence of this constraint, we need support from the architecture (in our case ARM) in order to safely and securely implement a system call mechanism.
</p>
<p>
In Linux ARM we can perform a system call by using the instruction <code>swi</code>. This instruction means <code>s</code>oft<code>w</code>are <code>i</code>nterruption and its sole purpose is to make a system call to the operating system. It receives a 24-bit operand that is not used at all by the processor but could be used by the the operating system to tell which service has been requested. In Linux such approach is not used and a 0 is set as the operand instead. So, in summary, in Linux we will always use <code>swi #0</code> to perform a system call.
</p>
<p>
An operating system, and particularly Linux, provides lots of services through <a href="http://man7.org/linux/man-pages/man2/syscalls.2.html#DESCRIPTION">system calls</a> so we need a way to select one of them. We will do this using the register <code>r7</code>. System calls are similar to function calls in that they receive parameters. No system call in Linux receives more than 7 arguments and the arguments are passed in registers <code>r0</code> to <code>r6</code>. If the system call returns some value it will be returned in register <code>r0</code>.
</p>
<p>
Note that the system call convention is incompatible with the convention defined by the AAPCS, so programs will need specific code that deals with a system call. In particular, it makes sense to <em>wrap</em> these system calls into normal functions, that externally, i.e. from the point of the caller, follow the AAPCS. This is precisely the main purpose of the C library. In Linux, the C library is usually <a href="http://www.gnu.org/software/libc/libc.html">GNU Libc</a> (but others can be used in Linux). These libraries hide the extra complexity of making system calls under the appearance of a normal function call.
</p>
<h2>Hello world, the system call way</h2>
<p>
As a simple illustration of calling the operating system we will write the archetypical “Hello world” program using system calls. In this case we will call the function <code>write</code>. Write receives three parameters: a file descriptor where we will write some data, a pointer to the data that will be written and the size of such data. Of these three, the most obscure may be now the file descriptor. Without entering into much details, it is just a <em>number</em> that identifies a file assigned to the process. Processes usually start with three preassigned files: the standard input, with the number 0, the standard output, with the number 1, and the standard error, with the number 2. We will write our messages to the standard output, so we will use the file descriptor 1.
</p>
<h3>The “ea-C” way</h3>
<p>
Continuing with our example, first we will call <code>write</code> through the C library. The C library follows the AAPCS convention. The prototype of the write system call can be found in the <a href="http://man7.org/linux/man-pages/man2/write.2.html">Linux man pages</a> and is as follows.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;">ssize_t write<span style="color: #009900;">(</span><span style="color: #993333;">int</span> fd<span style="color: #339933;">,</span> <span style="color: #993333;">const</span> <span style="color: #993333;">void</span> <span style="color: #339933;">*</span>buf<span style="color: #339933;">,</span> <span style="color: #993333;">size_t</span> count<span style="color: #009900;">)</span><span style="color: #339933;">;</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">ssize_t write(int fd, const void *buf, size_t count);</p></div>

<p>
Here both <code>size_t</code> and <code>ssize_t</code> are 32-bit integers, where the former is unsigned and the latter signed. Equipped with our knowledge of the AAPCS and ARM assembler it should not be hard for us to perform a call like the following
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #993333;">const</span> <span style="color: #993333;">char</span> greeting<span style="color: #009900;">[</span><span style="color: #0000dd;">13</span><span style="color: #009900;">]</span> <span style="color: #339933;">=</span> <span style="color: #ff0000;">"Hello world<span style="color: #000099; font-weight: bold;">\n</span>"</span><span style="color: #339933;">;</span>
write<span style="color: #009900;">(</span><span style="color: #0000dd;">1</span><span style="color: #339933;">,</span> greeting<span style="color: #339933;">,</span> <span style="color: #993333;">sizeof</span><span style="color: #009900;">(</span>greeting<span style="color: #009900;">)</span><span style="color: #009900;">)</span><span style="color: #339933;">;</span> <span style="color: #666666; font-style: italic;">// Here sizeof(greeting) is 13</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">const char greeting[13] = "Hello world\n";
write(1, greeting, sizeof(greeting)); // Here sizeof(greeting) is 13</p></div>

<p>
Here is the code
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> write_c<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
greeting<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"Hello world\n"</span>
after_greeting<span style="color: #339933;">:</span>
&nbsp;
<span style="color: #339933;">/*</span> This is an assembler constant<span style="color: #339933;">:</span> the assembler will compute it<span style="color: #339933;">.</span> Needless to say
   that this must evaluate to a constant value<span style="color: #339933;">.</span> <span style="color: #00007f; font-weight: bold;">In</span> this case we are computing the
   difference of addresses between the address after_greeting <span style="color: #00007f; font-weight: bold;">and</span> greeting<span style="color: #339933;">.</span> <span style="color: #00007f; font-weight: bold;">In</span> this
   case it will be <span style="color: #ff0000;">13</span> <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span>set size_of_greeting<span style="color: #339933;">,</span> after_greeting <span style="color: #339933;">-</span> greeting
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
<span style="color: #339933;">.</span>globl main
&nbsp;
main<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
&nbsp;
    <span style="color: #339933;">/*</span> Prepare the <span style="color: #00007f; font-weight: bold;">call</span> to <span style="color: #0000ff; font-weight: bold;">write</span> <span style="color: #339933;">*/</span>  
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>                <span style="color: #339933;">/*</span> First argument<span style="color: #339933;">:</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    ldr r1<span style="color: #339933;">,</span> addr_of_greeting  <span style="color: #339933;">/*</span> Second argument<span style="color: #339933;">:</span> &amp;greeting <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r2<span style="color: #339933;">,</span> #size_of_greeting <span style="color: #339933;">/*</span> Third argument<span style="color: #339933;">:</span> sizeof<span style="color: #009900; font-weight: bold;">(</span>greeting<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> <span style="color: #0000ff; font-weight: bold;">write</span>                  <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">write</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #ff0000;">1</span><span style="color: #339933;">,</span> greeting<span style="color: #339933;">,</span> sizeof<span style="color: #009900; font-weight: bold;">(</span>greeting<span style="color: #009900; font-weight: bold;">)</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #666666; font-style: italic;">;</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr
&nbsp;
addr_of_greeting <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> greeting</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* write_c.s */

.data

greeting: .asciz "Hello world\n"
after_greeting:

/* This is an assembler constant: the assembler will compute it. Needless to say
   that this must evaluate to a constant value. In this case we are computing the
   difference of addresses between the address after_greeting and greeting. In this
   case it will be 13 */
.set size_of_greeting, after_greeting - greeting

.text

.globl main

main:
    push {r4, lr}

    /* Prepare the call to write */  
    mov r0, #1                /* First argument: 1 */
    ldr r1, addr_of_greeting  /* Second argument: &amp;greeting */
    mov r2, #size_of_greeting /* Third argument: sizeof(greeting) */
    bl write                  /* write(1, greeting, sizeof(greeting));

    mov r0, #0
    pop {r4, lr}
    bx lr

addr_of_greeting : .word greeting</p></div>

<h3>The system call way</h3>
<p>
Ok, calling the system call through the C library was not harder than calling a normal function. Let’s try the same directly performing a Linux system call. First we have to identify the number of the system call and put it in <code>r7</code>. The call <code>write</code> has the number 4 (you can see the numbers in the file <code>/usr/include/arm-linux-gnueabihf/asm/unistd.h</code>). The parameters are usually the same as in the C function, so we will use registers <code>r0</code>, <code>r1</code> and <code>r2</code> likewise.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> write_sys<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
&nbsp;
greeting<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"Hello world\n"</span>
after_greeting<span style="color: #339933;">:</span>
&nbsp;
<span style="color: #339933;">.</span>set size_of_greeting<span style="color: #339933;">,</span> after_greeting <span style="color: #339933;">-</span> greeting
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
<span style="color: #339933;">.</span>globl main
&nbsp;
main<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r7<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
&nbsp;
    <span style="color: #339933;">/*</span> Prepare the system <span style="color: #00007f; font-weight: bold;">call</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>                  <span style="color: #339933;">/*</span> r0 ← <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    ldr r1<span style="color: #339933;">,</span> addr_of_greeting    <span style="color: #339933;">/*</span> r1 ← &amp;greeting <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r2<span style="color: #339933;">,</span> #size_of_greeting   <span style="color: #339933;">/*</span> r2 ← sizeof<span style="color: #009900; font-weight: bold;">(</span>greeting<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r7<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>                  <span style="color: #339933;">/*</span> select system <span style="color: #00007f; font-weight: bold;">call</span> <span style="color: #7f007f;">'write'</span> <span style="color: #339933;">*/</span>
    swi #<span style="color: #ff0000;">0</span>                      <span style="color: #339933;">/*</span> perform the system <span style="color: #00007f; font-weight: bold;">call</span> <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r7<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr
&nbsp;
addr_of_greeting <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> greeting</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* write_sys.s */

.data


greeting: .asciz "Hello world\n"
after_greeting:

.set size_of_greeting, after_greeting - greeting

.text

.globl main

main:
    push {r7, lr}

    /* Prepare the system call */
    mov r0, #1                  /* r0 ← 1 */
    ldr r1, addr_of_greeting    /* r1 ← &amp;greeting */
    mov r2, #size_of_greeting   /* r2 ← sizeof(greeting) */

    mov r7, #4                  /* select system call 'write' */
    swi #0                      /* perform the system call */

    mov r0, #0
    pop {r7, lr}
    bx lr

addr_of_greeting : .word greeting</p></div>

<p>
As you can see it is not that different to a function call but instead of branching to a specific address of code using <code>bl</code> we use <code>swi #0</code>. Truth be told, it is rather unusual to perform system calls directly. It is almost always preferable to call the C library instead.
</p>
<p>
That’s all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2014/05/24/arm-assembler-raspberry-pi-chapter-19/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2014/05/24/arm-assembler-raspberry-pi-chapter-19/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2014/05/24/arm-assembler-raspberry-pi-chapter-19/&amp;text=ARM+assembler+in+Raspberry+Pi+%E2%80%93+Chapter+19+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2014/05/24/arm-assembler-raspberry-pi-chapter-19/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>						<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2014/05/11/arm-assembler-raspberry-pi-chapter-18/" rel="prev">ARM assembler in Raspberry Pi – Chapter 18</a></span>
			<span class="next"><a href="http://thinkingeek.com/2014/06/09/tinymce-checkbox-toggler-jquery/" rel="next">TinyMCE checkbox toggler for jQuery</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
			<h3>10 thoughts on “<span>ARM assembler in Raspberry Pi – Chapter 19</span>”</h3>
		<ul class="commentlist">
				<li class="comment even thread-even depth-1 parent" id="comment-172790">
				<div id="div-comment-172790" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/9188c3fc6f4eb57e20b2cf3237f01bf7?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/9188c3fc6f4eb57e20b2cf3237f01bf7?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-9188c3fc6f4eb57e20b2cf3237f01bf7-0" originals="32" src-orig="http://0.gravatar.com/avatar/9188c3fc6f4eb57e20b2cf3237f01bf7?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Guojian</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/05/24/arm-assembler-raspberry-pi-chapter-19/#comment-172790">
			July 4, 2014 at 11:04 pm</a>		</div>

		<p>this is really great, I learned a lot from this serial. now would you continue to write something about the stack information for a system call? how a system call function allocate stack for itself ?</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/05/24/arm-assembler-raspberry-pi-chapter-19/?replytocom=172790#respond" onclick="return addComment.moveForm( &quot;div-comment-172790&quot;, &quot;172790&quot;, &quot;respond&quot;, &quot;1518&quot; )" aria-label="Reply to Guojian">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2 parent" id="comment-175299">
				<div id="div-comment-175299" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-a779b8290b1ca104fdf84d8016fd010b-0" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/05/24/arm-assembler-raspberry-pi-chapter-19/#comment-175299">
			July 8, 2014 at 9:34 am</a>		</div>

		<p>Hi Guojian,</p>
<p>while the precise stack construction inside the operating system is related to assembler, it is closer to a systems programming topic, which I am not going to cover in these tutorials.</p>
<p>Kind regards</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/05/24/arm-assembler-raspberry-pi-chapter-19/?replytocom=175299#respond" onclick="return addComment.moveForm( &quot;div-comment-175299&quot;, &quot;175299&quot;, &quot;respond&quot;, &quot;1518&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment even depth-3 parent" id="comment-205116">
				<div id="div-comment-205116" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/e1d78cf1c6802a32ecb8f36aeff2591b?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/e1d78cf1c6802a32ecb8f36aeff2591b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-e1d78cf1c6802a32ecb8f36aeff2591b-0" originals="32" src-orig="http://2.gravatar.com/avatar/e1d78cf1c6802a32ecb8f36aeff2591b?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">dummys</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/05/24/arm-assembler-raspberry-pi-chapter-19/#comment-205116">
			August 8, 2014 at 8:32 am</a>		</div>

		<p>Hi, I’ve learned so many cool things with your tutorial, may you continue on how we can interact with the hardware component of the raspberry pi ?<br>
Thanks<br>
Keep the good works<br>
dummys</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/05/24/arm-assembler-raspberry-pi-chapter-19/?replytocom=205116#respond" onclick="return addComment.moveForm( &quot;div-comment-205116&quot;, &quot;205116&quot;, &quot;respond&quot;, &quot;1518&quot; )" aria-label="Reply to dummys">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-4" id="comment-217536">
				<div id="div-comment-217536" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-a779b8290b1ca104fdf84d8016fd010b-1" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/05/24/arm-assembler-raspberry-pi-chapter-19/#comment-217536">
			August 20, 2014 at 9:54 am</a>		</div>

		<p>Hi dummys,</p>
<p>my intent was to gently introduce assembler, rather than interacting with hardware.</p>
<p>In my opinion it is much better to do this with a higher level language, even if it is C.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/05/24/arm-assembler-raspberry-pi-chapter-19/?replytocom=217536#respond" onclick="return addComment.moveForm( &quot;div-comment-217536&quot;, &quot;217536&quot;, &quot;respond&quot;, &quot;1518&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-226103">
				<div id="div-comment-226103" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a56adeb0c6952a08b8e2ee63e83eb50a?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a56adeb0c6952a08b8e2ee63e83eb50a?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-a56adeb0c6952a08b8e2ee63e83eb50a-0" originals="32" src-orig="http://1.gravatar.com/avatar/a56adeb0c6952a08b8e2ee63e83eb50a?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">claes</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/05/24/arm-assembler-raspberry-pi-chapter-19/#comment-226103">
			August 27, 2014 at 9:13 pm</a>		</div>

		<p>I think this lesson, the calling convention with “swi” was very good.</p>
<p>In the last example, shouldn’t it be register r7 that is pushed and poped instead of r4?</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/05/24/arm-assembler-raspberry-pi-chapter-19/?replytocom=226103#respond" onclick="return addComment.moveForm( &quot;div-comment-226103&quot;, &quot;226103&quot;, &quot;respond&quot;, &quot;1518&quot; )" aria-label="Reply to claes">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-226138">
				<div id="div-comment-226138" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-a779b8290b1ca104fdf84d8016fd010b-2" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/05/24/arm-assembler-raspberry-pi-chapter-19/#comment-226138">
			August 27, 2014 at 9:52 pm</a>		</div>

		<p>Hi claes,</p>
<p>yes, you are right. <code>r7</code> is a callee-saved register so we must preserve its value upon returning the function. I already updated the code.</p>
<p>Thanks a lot,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/05/24/arm-assembler-raspberry-pi-chapter-19/?replytocom=226138#respond" onclick="return addComment.moveForm( &quot;div-comment-226138&quot;, &quot;226138&quot;, &quot;respond&quot;, &quot;1518&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1 parent" id="comment-817231">
				<div id="div-comment-817231" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/8e02ba12a3b8f9d19306c56fbde0d455?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/8e02ba12a3b8f9d19306c56fbde0d455?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-8e02ba12a3b8f9d19306c56fbde0d455-0" originals="32" src-orig="http://2.gravatar.com/avatar/8e02ba12a3b8f9d19306c56fbde0d455?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn"><a href="http://pervin@utdallas.edu" rel="external nofollow" class="url">William Pervin</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/05/24/arm-assembler-raspberry-pi-chapter-19/#comment-817231">
			March 11, 2015 at 2:14 am</a>		</div>

		<p>I see that SoftWare Interrupt (SWI) is now replaced by SerVice Call (SVC) in ARM documentation but works the same and the old name is still accepted. I’d really like to find detailed information of what should be in the registers for READ (3), WRITE (4), OPEN (5), CLOSE (6), CREAT (7), etc. Since it’s OS dependent, where do I look?</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/05/24/arm-assembler-raspberry-pi-chapter-19/?replytocom=817231#respond" onclick="return addComment.moveForm( &quot;div-comment-817231&quot;, &quot;817231&quot;, &quot;respond&quot;, &quot;1518&quot; )" aria-label="Reply to William Pervin">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-818009">
				<div id="div-comment-818009" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-a779b8290b1ca104fdf84d8016fd010b-3" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/05/24/arm-assembler-raspberry-pi-chapter-19/#comment-818009">
			March 11, 2015 at 8:47 pm</a>		</div>

		<p>Hi William,</p>
<p>this is indeed OS dependent. In the case of Raspberry Pi under Raspbian, the underlying operating system is a modified Debian with a Linux kernel.</p>
<p>You can see the prototypes (in C) of the Linux system calls in the Linux Cross Reference in the following URL: <a href="http://lxr.free-electrons.com/source/include/linux/syscalls.h" rel="nofollow">http://lxr.free-electrons.com/source/include/linux/syscalls.h</a> </p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/05/24/arm-assembler-raspberry-pi-chapter-19/?replytocom=818009#respond" onclick="return addComment.moveForm( &quot;div-comment-818009&quot;, &quot;818009&quot;, &quot;respond&quot;, &quot;1518&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-981580">
				<div id="div-comment-981580" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/982226093c9bd26e1a93673ec9d736c5?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/982226093c9bd26e1a93673ec9d736c5?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-982226093c9bd26e1a93673ec9d736c5-0" originals="32" src-orig="http://0.gravatar.com/avatar/982226093c9bd26e1a93673ec9d736c5?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Denis</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/05/24/arm-assembler-raspberry-pi-chapter-19/#comment-981580">
			December 10, 2016 at 11:52 am</a>		</div>

		<p>ok. but i did’t understand how can i output content of registet to terminal.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/05/24/arm-assembler-raspberry-pi-chapter-19/?replytocom=981580#respond" onclick="return addComment.moveForm( &quot;div-comment-981580&quot;, &quot;981580&quot;, &quot;respond&quot;, &quot;1518&quot; )" aria-label="Reply to Denis">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-981866">
				<div id="div-comment-981866" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-a779b8290b1ca104fdf84d8016fd010b-4" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/05/24/arm-assembler-raspberry-pi-chapter-19/#comment-981866">
			December 15, 2016 at 9:21 pm</a>		</div>

		<p>Ho Denis,</p>
<p>calling printf or a similar function should do. In other chapters we have called printf. You may want to revisit them.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/05/24/arm-assembler-raspberry-pi-chapter-19/?replytocom=981866#respond" onclick="return addComment.moveForm( &quot;div-comment-981866&quot;, &quot;981866&quot;, &quot;respond&quot;, &quot;1518&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		</ul>
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2014/05/24/arm-assembler-raspberry-pi-chapter-19/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="1518" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="fe7ee45aed"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496666613607"></form>
			</div><!-- #respond -->
	</div>	</div>