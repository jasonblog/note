# Objects and libraries


<div class="main">
			<h1>ARM assembler in Raspberry Pi – Chapter 27 </h1>
		<p class="post-meta"><span class="date">April 17, 2017</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2017/04/17/arm-assembler-raspberry-pi-chapter-27/#respond">0</a></span> </p>
		<p>
We saw in the previous chapter what is the process required to build a program from different compilation units. This process happened before we obtained the final program. The question is, can this process happen when the program runs? This is, is it possible to dynamically link a program?
</p>
<p><span id="more-3806"></span></p>
<h2>Objects and libraries</h2>
<p>
In chapter 26 we saw the linking process, which basically combines several objects to form the final binary. If all the objects we used belonged to our program that would do. But usually programs are built on reusable pieces that are used by many programs. These reusable components are usually gathered together in what is called a library.
</p>
<p>
In the UNIX tradition such libraries have been materialized in what is called an <em>archive</em>. An archive is at its essence a collection of object files together. When linking a program we specify the archive instead of the object. The linker knows how to handle these archives and is able to determine which objects of it are used by the program. Then it behaves as if only the required objects had been specified.
</p>
<p>
The C library is an example of this. In previous examples we have called <code>printf</code>, <code>puts</code> or <code>random</code>. These functions are defined in the C library. By using the gcc driver, internally it calls the linker and it passes the C runtime library, commonly known as the libc. In Linux the most usual C library is the <a href="https://www.gnu.org/software/libc/">GNU C Library</a>. Other C libraries exist that have more specific purposes: <a href="https://sourceware.org/newlib/">newlib</a>, <a href="https://www.uclibc.org/">uClibc</a>, <a href="https://www.musl-libc.org/">musl</a>, etc.
</p>
<p>
Archives are commonly known as <em>static libraries</em> because they are just a convenient way to specify many objects at the same time. But beyond that, they do not change the fact that the final program is fully determined at link time. The linker still has all the required pieces to build the final program.
</p>
<h2>Dynamic linking</h2>
<p>
What if instead of building the whole program at link time, we just assembled the minimal pieces of it so we could <em>complete</em> it when running the program? What if instead of static libraries we used <em>dynamic libraries</em>. So the program would dynamically link to them when executing.
</p>
<p>
At first this looks a bit outlandish but has a few advantages. By delaying the link process we earn a few advantages. For instance a program that uses <code>printf</code> would not require to have the <code>printf</code> in the program file. It could use an existing dynamic C library of the system, which will also have its copy of <code>printf</code>. Also, if an error is found in the <code>printf</code> of that dynamic library, just replacing the dynamic library would be enough and our program would automatically benefit from a fixed <code>printf</code>. If we had statically linked <code>printf</code>, we would be forced to relink it again to get the correct <code>printf</code>.
</p>
<p>
Of course very few things are free in the nature, and dynamic linking and dynamic libraries require more effort. We need to talk about loading.
</p>
<h2>Loading a program</h2>
<p>
Before we can execute a program we need to put it in memory. This process is called <em>loading</em>. Usually the operating system is responsible for loading programs.
</p>
<p>
If you recall the previous chapter we had an example where we defined two variables, <code>another_var</code> and <code>result_var</code>, and a function <code>inc_result</code>. We also saw that after the linking happens, the addresses where hardcoded in the final program file. A loader task in this case is pretty straightforward, just copy the relevant bits of our program file into memory. Addresses have ben <em>fixed up</em> by the linker already, so as long as we copy (i.e. load) the program in the right memory address, we’re done.
</p>
<p>
Modern operating systems, like Linux, provide to processes (i.e. a running programs) what is called <em>virtual memory</em> thanks to specific hardware support for this. Virtual memory gives the illusion that a process can use the memory space as it wants. This mechanism also provides isolation: a process cannot write the memory of another process. Running several programs that want to be loaded at the same address is not a problem because they simply load at the same virtual address. The operating system maps these virtual addresses to different physical addresses.
</p>
<p>
In systems without virtual memory all addresses are physical. This makes impossible to load run more than one process if any of them overlaps in memory with another one.
</p>
<p><img data-attachment-id="3871" data-permalink="http://thinkingeek.com/2017/04/17/arm-assembler-raspberry-pi-chapter-27/loader-svg/" data-orig-file="images/loader.svg_.png" data-orig-size="623,709" data-comments-opened="1" data-image-meta="{&quot;aperture&quot;:&quot;0&quot;,&quot;credit&quot;:&quot;&quot;,&quot;camera&quot;:&quot;&quot;,&quot;caption&quot;:&quot;&quot;,&quot;created_timestamp&quot;:&quot;0&quot;,&quot;copyright&quot;:&quot;&quot;,&quot;focal_length&quot;:&quot;0&quot;,&quot;iso&quot;:&quot;0&quot;,&quot;shutter_speed&quot;:&quot;0&quot;,&quot;title&quot;:&quot;&quot;,&quot;orientation&quot;:&quot;0&quot;}" data-image-title="Loader" data-image-description="" data-medium-file="images/loader.svg_-264x300.png" data-large-file="images/loader.svg_.png" src="images/loader.svg_.png" alt="" width="623" height="709" class="aligncenter size-full wp-image-3871" srcset="images/loader.svg_.png 623w, images/loader.svg_-264x300.png 264w" sizes="(max-width: 623px) 100vw, 623px"></p>
<p>
To use dynamic libraries, given that the linking process happens in run time, we need a second program, called the <em>dynamic linker</em>. This is in contrast to the <em>program linker</em> or <em>static linker</em>. This dynamic linker will also act as a dynamic loader because it will be responsible to load the required dynamic libraries in memory.
</p>
<p>
We call this tool a dynamic linker because once it has loaded the code and data of the dynamic library in memory it will have to resolve some relocations. The amount of relocations it has to perform depends on whether the code is <em>position independent</em> or not.
</p>
<h3>Position independent code</h3>
<p>
Code can be position dependent or position independent.
</p>
<p>
Position dependent code assumes that it can use absolute addresses directly. This means that, if we can load the program at the address it expects, nothing else has to be done, which is great. The downside is, of course, if we cannot. In this case we need to fix all the absolute addresses to the new addresses. This means, we load the program in some address (not the one it expects) and then we <em>fix up</em> all the absolute addresses to the new locations. To make this process sensibly efficient relocations will have to be used here. These relocations occur in the code of the program. This means that every process has a slightly different version of the original code in memory. In practice this essentially the same idea as static linking but just delaying at which step the linking happens.
</p>
<p>
Position independent code (known as PIC) does not use absolute addresses. Instead some mechanism is used on which the program builds relative addresses. At run time these relative addresses can be converted, by means of some extra computation, into absolute addresses. The mechanism used in ELF uses a table called the Global Offset Table. This table contains entries, one entry per global entity we want to access. Each entry, at run time, will hold the absolute address of the object. Every program and dynamic library has its own GOT, not shared with anyone else. This GOT is located in memory in a way that is possible to access it without using an absolute address. To do this, a pc-relative address must be used. So the GOT is then located at some fixed position whose distance to it, from the instruction that refers it, can be computed at static link time.
</p>
<p>
An advantage of this technique is that there are no relocations to be done in the code at loading time. Only the GOT must be correctly relocated when dynamically loading the code. This may reduce enormously the loading time. Given that the code in memory does not have to be fixed up, all processes using the same libraries can share them. This is done in operating systems that support virtual memory: the code parts of dynamic libraries are shared between processes. This means that, while the code will still take space in the virtual memory address space of the process it will not use extra physical memory of the system. The downside is that because of the GOT, accessing to global addresses (global variables and functions) is much more complex.
</p>
<h2>Accessing a global variable</h2>
<p>
As an example of how much more complex is accessing a global variable, let’s start with a simple example. For this example we will assume our program just increments a global variable. The global variable is provided by a library. (I know this is a horrible scenario but this is just for the sake of this exposition)
</p>
<h3>Static library</h3>
<p>
Our static library will be very simple. We will have a <code>mylib.s</code> file which will only contain <code>myvar</code>.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> mylib<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
<span style="color: #339933;">.</span>balign <span style="color: #ff0000;">4</span>
<span style="color: #339933;">.</span>globl myvar
myvar <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">42</span>     <span style="color: #339933;">/*</span> <span style="color: #ff0000;">42</span> as initial value <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span>size myvar<span style="color: #339933;">,</span> <span style="color: #339933;">.-</span>myvar</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* mylib.s */
.data

.balign 4
.globl myvar
myvar : .word 42     /* 42 as initial value */
.size myvar, .-myvar</p></div>

<p>
The <code>.size</code> directive will be required for the case of the dynamic library. It states the size of a symbol, in this case <code>myvar</code>. We could have hard coded the value (4) but here we are making the assembler compute it for us. The expression subtracts the current address (denoted by a dot, <code>.</code>) with the address of <code>myvar</code>. Due to the <code>.word</code> directive inbetween, these two addresses are 4 bytes apart.
</p>
<p>
Our program will be just a <code>main.s</code> file which accesses the variable and increments it. Nothing interesting, just to show that this is not different to what we have been doing.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> main<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #0000ff; font-weight: bold;">.data</span>
<span style="color: #339933;">.</span>globl myvar
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
<span style="color: #339933;">.</span>globl main
&nbsp;
<span style="color: #339933;">.</span>balign <span style="color: #ff0000;">4</span>
main<span style="color: #339933;">:</span>
    ldr r0<span style="color: #339933;">,</span> addr_myvar  <span style="color: #339933;">/*</span> r0 ← &amp;myvar <span style="color: #339933;">*/</span>
    ldr r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-wehttp://thinkingeek.com/wp-content/uploads/2015/01ight: bold;">]</span>        <span style="color: #339933;">/*</span> r1 ← <span style="color: #339933;">*</span>r0 <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>      <span style="color: #339933;">/*</span> r1 ← r1 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>        <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span>r0 ← r1 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>          <span style="color: #339933;">/*</span> end as usual <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr
&nbsp;
addr_myvar<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> myvar</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* main.s */
.data
.globl myvar

.text
.globl main

.balign 4
main:
    ldr r0, addr_myvar  /* r0 ← &amp;myvar */
    ldr r1, [r0]        /* r1 ← *r0 */
    add r1, r1, #1      /* r1 ← r1 + 1 */
    str r1, [r0]        /* *r0 ← r1 */

    mov r0, #0          /* end as usual */
    bx lr

addr_myvar: .word myvar</p></div>

<p>
We can build and link the library and the program as usual.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="bash" style="font-family:monospace;"><span style="color: #666666; font-style: italic;"># (static) library</span>
<span style="color: #c20cb9; font-weight: bold;">as</span> <span style="color: #660033;">-o</span> mylib.o mylib.s
<span style="color: #c20cb9; font-weight: bold;">ar</span> cru mylib.a mylib.o
<span style="color: #666666; font-style: italic;"># program</span>
<span style="color: #c20cb9; font-weight: bold;">as</span> <span style="color: #660033;">-o</span> main.o main.s
<span style="color: #c20cb9; font-weight: bold;">gcc</span> <span style="color: #660033;">-o</span> main main.o -L. -l:mylib.a</pre></td></tr></tbody></table><p class="theCode" style="display:none;"># (static) library
as -o mylib.o mylib.s
ar cru mylib.a mylib.o
# program
as -o main.o main.s
gcc -o main main.o -L. -l:mylib.a</p></div>

<p>
<code>ar</code>, the archiver, is the tool that creates a static library, an <code>.a</code> file, from a set of object files (just a single one in this example). Then we link the final main specifying <code>mylib</code> as a library (the colon in the <code>-l</code> flag is required because the library file name does not start with the customary <code><em>lib</em></code> prefix).
</p>
<p>
Nothing special so far, actually.
</p>
<h3>Dynamic library</h3>
<p>
In order to generate a dynamic library, we need to tell the linker that we do not want a program but a dynamic library. In ELF dynamic libraries are called shared objects thus their extension <code>.so</code>. For this purpose we will use gcc which provides a handy flag <code>-shared</code> which takes care of all the flags that ld will need to create a dynamic library.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="bash" style="font-family:monospace;"><span style="color: #666666; font-style: italic;"># dynamic library</span>
<span style="color: #c20cb9; font-weight: bold;">as</span> <span style="color: #660033;">-o</span> mylib.o mylib.s
<span style="color: #c20cb9; font-weight: bold;">gcc</span> <span style="color: #660033;">-shared</span> <span style="color: #660033;">-o</span> mylib.so mylib.o</pre></td></tr></tbody></table><p class="theCode" style="display:none;"># dynamic library
as -o mylib.o mylib.s
gcc -shared -o mylib.so mylib.o</p></div>

<p>
Now we want to access from our program to the variable <code>myvar</code> using a position independent access.
</p>
<p style="background-color: lightyellow; padding: 1em;">
Actually, the position independent code is only required for dynamic libraries. Our main program could still use non-PIC accesses and it would work for variables in libraries, the linker would take care of this case. But nothing prevents us from using PIC code in the main program. A <em>position independent executable</em> (PIE) needs to do all accesses through the GOT.
</p>
<p>
Recall, we cannot use a mechanism that forces the code to be relocated (i.e. have its addresses fixed up). Only the GOT can be fixed (it is not code, after all). The address of <code>myvar</code> will be in some entry in the GOT. We do not know which one, exactly, this is a concern of the linker. We still need to get the base address of the GOT first, though.
</p>
<p>
We saw above, that a PIC access is going to be <code>pc</code>-relative. Given that the program and library will be loaded as a single piece in memory, we can ask the static linker to put the exact offset to the GOT for us. We can do this by just doing
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">...</span>
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> _GLOBAL_OFFSET_TABLE_
<span style="color: #339933;">...</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">...
.word _GLOBAL_OFFSET_TABLE_
...</p></div>

<p>
Unfortunately this will be a relative offset from the current position in the code. Ideally we’d want to write this
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">add</span> r0<span style="color: #339933;">,</span> pc<span style="color: #339933;">,</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> _GLOBAL_OFFSET_TABLE_  <span style="color: #339933;">/*</span> r0 ← pc <span style="color: #339933;">+</span> <span style="color: #7f007f;">"offset-to-GOT"</span> <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">add r0, pc, .word _GLOBAL_OFFSET_TABLE_  /* r0 ← pc + "offset-to-GOT" */</p></div>

<p>
It is not possible to encode an instruction like this in the 32-bit of an ARM instruction. So we will need to use the typical approach.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">ldr r0<span style="color: #339933;">,</span> offset_of_GOT    <span style="color: #339933;">/*</span> r0 ← <span style="color: #7f007f;">"offset-to-GOT"</span> <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">add</span> r0<span style="color: #339933;">,</span> pc<span style="color: #339933;">,</span> r0           <span style="color: #339933;">/*</span> r0 ← pc <span style="color: #339933;">+</span> r0 <span style="color: #339933;">*/</span> 
<span style="color: #339933;">...</span>
offset_of_GOT<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> _GLOBAL_OFFSET_TABLE_</pre></td></tr></tbody></table><p class="theCode" style="display:none;">ldr r0, offset_of_GOT    /* r0 ← "offset-to-GOT" */
add r0, pc, r0           /* r0 ← pc + r0 */ 
...
offset_of_GOT: .word _GLOBAL_OFFSET_TABLE_</p></div>

<p>
But that “offset-to-got” must be the offset to the GOT at the point where we are actually adding the <code>pc</code>, this is, in the second instruction. This means that we need to ask the linker to adjust it so the offset make sense for the instruction that adds that offset to the pc. We can do this using an additional label.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">ldr r0<span style="color: #339933;">,</span> offset_of_GOT           <span style="color: #339933;">/*</span> r0 ← <span style="color: #7f007f;">"offset-to-GOT"</span> <span style="color: #339933;">*/</span>
got_address<span style="color: #339933;">:</span> <span style="color: #00007f; font-weight: bold;">add</span> r0<span style="color: #339933;">,</span> pc<span style="color: #339933;">,</span> r0     <span style="color: #339933;">/*</span> r0 ← pc <span style="color: #339933;">+</span> r0 <span style="color: #339933;">*/</span> 
<span style="color: #339933;">...</span>
offset_of_GOT<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> _GLOBAL_OFFSET_TABLE_  <span style="color: #339933;">-</span> got_address</pre></td></tr></tbody></table><p class="theCode" style="display:none;">ldr r0, offset_of_GOT           /* r0 ← "offset-to-GOT" */
got_address: add r0, pc, r0     /* r0 ← pc + r0 */ 
...
offset_of_GOT: .word _GLOBAL_OFFSET_TABLE_  - got_address</p></div>

<p>
A peculiarity of ARM is that reading the <code>pc</code> in an instruction, gives us the value of the <code>pc</code> offset 8 bytes. So we may have to subtract 8 to the <code>r0</code> above or just make sure the relocation is already doing that for us. The second approach is actually better because avoids an instruction.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">ldr r0<span style="color: #339933;">,</span> offset_of_GOT           <span style="color: #339933;">/*</span> r0 ← <span style="color: #7f007f;">"offset-to-GOT"</span> <span style="color: #339933;">*/</span>
got_address<span style="color: #339933;">:</span> <span style="color: #00007f; font-weight: bold;">add</span> r0<span style="color: #339933;">,</span> pc<span style="color: #339933;">,</span> r0     <span style="color: #339933;">/*</span> r0 ← pc <span style="color: #339933;">+</span> r0 <span style="color: #339933;">*/</span> 
<span style="color: #339933;">...</span>
offset_of_GOT<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> _GLOBAL_OFFSET_TABLE_  <span style="color: #339933;">-</span> <span style="color: #009900; font-weight: bold;">(</span>got_address <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">ldr r0, offset_of_GOT           /* r0 ← "offset-to-GOT" */
got_address: add r0, pc, r0     /* r0 ← pc + r0 */ 
...
offset_of_GOT: .word _GLOBAL_OFFSET_TABLE_  - (got_address + 8)</p></div>

<p>
And now in <code>r0</code> we have the absolute address of the GOT. But we want to access <code>myvar</code>. We can ask the static linker to tell use the offset (in bytes) in the GOT for a symbol using the syntax below.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> myvar<span style="color: #009900; font-weight: bold;">(</span>GOT<span style="color: #009900; font-weight: bold;">)</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">.word myvar(GOT)</p></div>

<p>
Now we have all the necessary ingredients to access <code>myvar</code> in a position-independent way.
</p>
<p><img data-attachment-id="3916" data-permalink="http://thinkingeek.com/2017/04/17/arm-assembler-raspberry-pi-chapter-27/got-svg/" data-orig-file="images/got.svg_.png" data-orig-size="532,470" data-comments-opened="1" data-image-meta="{&quot;aperture&quot;:&quot;0&quot;,&quot;credit&quot;:&quot;&quot;,&quot;camera&quot;:&quot;&quot;,&quot;caption&quot;:&quot;&quot;,&quot;created_timestamp&quot;:&quot;0&quot;,&quot;copyright&quot;:&quot;&quot;,&quot;focal_length&quot;:&quot;0&quot;,&quot;iso&quot;:&quot;0&quot;,&quot;shutter_speed&quot;:&quot;0&quot;,&quot;title&quot;:&quot;&quot;,&quot;orientation&quot;:&quot;0&quot;}" data-image-title="got.svg" data-image-description="" data-medium-file="images/got.svg_-300x265.png" data-large-file="images/got.svg_.png" src="images/got.svg_.png" alt="" width="532" height="470" class="aligncenter size-full wp-image-3916" srcset="images/got.svg_.png 532w, images/got.svg_-300x265.png 300w" sizes="(max-width: 532px) 100vw, 532px"></p>

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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> main<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
<span style="color: #339933;">.</span>globl main
&nbsp;
<span style="color: #339933;">.</span>balign <span style="color: #ff0000;">4</span>
&nbsp;
main<span style="color: #339933;">:</span>
  ldr r0<span style="color: #339933;">,</span> offset_of_GOT        <span style="color: #339933;">/*</span> r0 ← offset<span style="color: #339933;">-</span>to<span style="color: #339933;">-</span>GOT
                                  <span style="color: #009900; font-weight: bold;">(</span>respect to got_address<span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">*/</span>
  got_address<span style="color: #339933;">:</span> <span style="color: #00007f; font-weight: bold;">add</span> r0<span style="color: #339933;">,</span> pc<span style="color: #339933;">,</span> r0  <span style="color: #339933;">/*</span> r0 ← pc <span style="color: #339933;">+</span> r0 
                                  this is
                                    r0 ← &amp;GOT <span style="color: #339933;">*/</span>
  ldr r1<span style="color: #339933;">,</span> myvar_in_GOT         <span style="color: #339933;">/*</span> r1 ← offset<span style="color: #339933;">-</span>of<span style="color: #339933;">-</span>myvar<span style="color: #339933;">-</span>inside<span style="color: #339933;">-</span>GOT <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">add</span> r0<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> r1               <span style="color: #339933;">/*</span> r0 ← r0 <span style="color: #339933;">+</span> r1 
                                  this is
                                    r0 ← &amp;GOT <span style="color: #339933;">+</span> offset<span style="color: #339933;">-</span>of<span style="color: #339933;">-</span>myvar<span style="color: #339933;">-</span>inside<span style="color: #339933;">-</span>GOT <span style="color: #339933;">*/</span>
  ldr r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>                 <span style="color: #339933;">/*</span> r0 ← <span style="color: #339933;">*</span>r0
                                  this is
                                    r0 ← &amp;myvar
                                <span style="color: #339933;">*/</span>
  ldr r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>                  <span style="color: #339933;">/*</span> r0 ← <span style="color: #339933;">*</span>r1 <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">add</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>                <span style="color: #339933;">/*</span> r1 ← r1 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">str</span> r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>                  <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span>r0 ← r1 <span style="color: #339933;">*/</span>
&nbsp;
  <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>          <span style="color: #339933;">/*</span> end as usual <span style="color: #339933;">*/</span>
  <span style="color: #46aa03; font-weight: bold;">bx</span> lr
&nbsp;
offset_of_GOT<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> _GLOBAL_OFFSET_TABLE_  <span style="color: #339933;">-</span> <span style="color: #009900; font-weight: bold;">(</span>got_address <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span>
myvar_in_GOT <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> myvar<span style="color: #009900; font-weight: bold;">(</span>GOT<span style="color: #009900; font-weight: bold;">)</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* main.s */
.data

.text
.globl main

.balign 4

main:
  ldr r0, offset_of_GOT        /* r0 ← offset-to-GOT
                                  (respect to got_address)*/
  got_address: add r0, pc, r0  /* r0 ← pc + r0 
                                  this is
                                    r0 ← &amp;GOT */
  ldr r1, myvar_in_GOT         /* r1 ← offset-of-myvar-inside-GOT */
  add r0, r0, r1               /* r0 ← r0 + r1 
                                  this is
                                    r0 ← &amp;GOT + offset-of-myvar-inside-GOT */
  ldr r0, [r0]                 /* r0 ← *r0
                                  this is
                                    r0 ← &amp;myvar
                                */
  ldr r1, [r0]                  /* r0 ← *r1 */
  add r1, r1, #1                /* r1 ← r1 + 1 */
  str r1, [r0]                  /* *r0 ← r1 */

  mov r0, #0          /* end as usual */
  bx lr

offset_of_GOT: .word _GLOBAL_OFFSET_TABLE_  - (got_address + 8)
myvar_in_GOT : .word myvar(GOT)</p></div>

<p>
We can replace the <code>add</code> (line 16) and the <code>ldr</code> (line 19) with a more elaborated memory access.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>16
17
18
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">  ldr r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #339933;">,</span> r1<span style="color: #009900; font-weight: bold;">]</span>     <span style="color: #339933;">/*</span> r0 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r0 <span style="color: #339933;">+</span> r1<span style="color: #009900; font-weight: bold;">)</span>
                          this is
                            r0 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>&amp;GOT <span style="color: #339933;">+</span> offset<span style="color: #339933;">-</span>of<span style="color: #339933;">-</span>my<span style="color: #339933;">-</span>var<span style="color: #339933;">-</span>inside<span style="color: #339933;">-</span>GOT<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">  ldr r0, [r0, r1]     /* r0 ← *(r0 + r1)
                          this is
                            r0 ← *(&amp;GOT + offset-of-my-var-inside-GOT) */</p></div>

<p>
Now we can build the program.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="bash" style="font-family:monospace;"><span style="color: #666666; font-style: italic;"># program</span>
<span style="color: #c20cb9; font-weight: bold;">as</span> <span style="color: #660033;">-o</span> main.o main.s
<span style="color: #c20cb9; font-weight: bold;">gcc</span> <span style="color: #660033;">-o</span> main main.o -L. -l:mylib.so -Wl,-rpath,$<span style="color: #7a0874; font-weight: bold;">(</span><span style="color: #7a0874; font-weight: bold;">pwd</span><span style="color: #7a0874; font-weight: bold;">)</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;"># program
as -o main.o main.s
gcc -o main main.o -L. -l:mylib.so -Wl,-rpath,$(pwd)</p></div>

<p>
The <code>-Wl,-rpath,$(pwd)</code> option tells the dynamic linker to use the current directory, <code>$(pwd)</code>, to find the library. This is because if we don’t do this we will not be able to run the program as the dynamic loader will not be able to find it.
</p>
<h2>Calling a function</h2>
<p>
Calling a function from a dynamic library is slightly more involved than just accessing it in the GOT. Because of a feature of ELF called <em>lazy binding</em>, functions may be loaded in a lazy fashion. The reason is that a library may provide many functions but only a few may be required at runtime. When using static linking this is rarely a problem because the linker will use only those object files that define the symbols potentially used by the program. But we cannot do this for a dynamic library because it must be handled as a whole.
</p>
<p>
Thus, under lazy loading, the first time that we call a function it has to be loaded. Further calls will use the previously loaded function. This is efficient but requires a bit more of machinery. In ELF this is achieved by using an extra table called the Procedure Linkage Table (PLT). There is an entry for each, potentially, used function by the program. These entries are also replicated in the GOT. In contrast to the GOT, the PLT is code and we do not want to modify it. Entries in the PLT are small sequences of instructions that just branch to the entry in the GOT. The GOT entries for functions are initialized by the dynamic linker with the address to an internal function of the dynamic linker which retrieves the address of the function, updates the GOT with that address and branches to it. Because the dynamic linker updated the GOT table, the next call through the PLT (that recall simply branches to the GOT) will directly go to the function.
</p>
<p>
One may wonder why not directly calling the address in the GOT or why using a PLT. The reason is that the dynamic linker must know which function we want to load the first time, if we directly call the address in the GOT we need to devise a mechanism to be able to tell which function must be loaded. A way could be initalizing the GOT entries for functions to a table that prepares everything so the the dynamic loader knows the exact function that needs to be loaded. But this is in practice equivalent to the PLT!
</p>
<p>
All at this point looks overly complicated but the good news are that it is the linker who creates these PLT entries and they can be used as regular function calls. No need to get the address of the GOT entry and all that we had to do for a variable (we still have to do this if we will be using the address of the function!). We could always do that but this would bloat the code as every function call would require a complex indexing in the GOT table. This mechanism works both for PIC and non-PIC, and it is the reason we have been able to call C library functions like <code>printf</code> without having to worry whether it came from a dynamic library (and they do unless we use <code>-static</code> to generate a fully static executable) or not. That said, we can explicitly use the suffix <code>@PLT</code> to state that we want to call a function through the PLT. This is mandatory for calls done inside a library.
</p>
<h3>Complete example</h3>
<p>
Let’s extend now our library with a function that prints the value of <code>myvar</code>. Given that it is code in the library it must be PIC code: accesses to variables through the GOT and calls to functions via PLT. Our function is called <code>myfun</code>. It is pretty similar to what we did in the main, except for the increment.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> mylib<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
<span style="color: #339933;">.</span>balign <span style="color: #ff0000;">4</span>
<span style="color: #339933;">.</span>globl myvar
myvar <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">42</span> <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">global</span> variable <span style="color: #7f007f;">"myvar"</span> <span style="color: #339933;">*/</span>
<span style="color: #339933;">.</span>size myvar<span style="color: #339933;">,</span> <span style="color: #339933;">.-</span>myvar
&nbsp;
message<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"Value of 'myvar' is %d\n"</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
<span style="color: #339933;">.</span>balign <span style="color: #ff0000;">4</span>
<span style="color: #339933;">.</span>globl myfun
myfun<span style="color: #339933;">:</span>
  <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>                <span style="color: #339933;">/*</span> we are going to <span style="color: #0000ff; font-weight: bold;">do</span> a
                                  <span style="color: #00007f; font-weight: bold;">call</span> so keep lr<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">and</span> also r4
                                  for a <span style="color: #ff0000;">8</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
  ldr r0<span style="color: #339933;">,</span> offset_of_GOT        <span style="color: #339933;">/*</span> r0 ← offset<span style="color: #339933;">-</span>to<span style="color: #339933;">-</span>GOT
                                  <span style="color: #009900; font-weight: bold;">(</span>respect to got_address<span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">*/</span>
  got_address<span style="color: #339933;">:</span> <span style="color: #00007f; font-weight: bold;">add</span> r0<span style="color: #339933;">,</span> pc<span style="color: #339933;">,</span> r0  <span style="color: #339933;">/*</span> r0 ← pc <span style="color: #339933;">+</span> r0 
                                  this is
                                    r0 ← &amp;GOT <span style="color: #339933;">*/</span>
  ldr r1<span style="color: #339933;">,</span> myvar_in_GOT         <span style="color: #339933;">/*</span> r1 ← offset<span style="color: #339933;">-</span>of<span style="color: #339933;">-</span>myvar<span style="color: #339933;">-</span>inside<span style="color: #339933;">-</span>GOT <span style="color: #339933;">*/</span>
  ldr r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #339933;">,</span> r1<span style="color: #009900; font-weight: bold;">]</span>             <span style="color: #339933;">/*</span> r0 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r0 <span style="color: #339933;">+</span> r1<span style="color: #009900; font-weight: bold;">)</span>
                                  this is
                                    r0 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>&amp;GOT <span style="color: #339933;">+</span> offset<span style="color: #339933;">-</span>of<span style="color: #339933;">-</span>myvar<span style="color: #339933;">-</span>inside<span style="color: #339933;">-</span>GOT<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
  ldr r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>                 <span style="color: #339933;">/*</span> r0 ← <span style="color: #339933;">*</span>r1 <span style="color: #339933;">*/</span>
&nbsp;
  ldr r0<span style="color: #339933;">,</span> addr_of_message      <span style="color: #339933;">/*</span> r0 ← &amp;message <span style="color: #339933;">*/</span>
  <span style="color: #339933;">/*</span> r1 already contains the value we want <span style="color: #339933;">*/</span>
  <span style="color: #46aa03; font-weight: bold;">bl</span> printf@PLT                <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> to printf via the PLT <span style="color: #339933;">*/</span>
&nbsp;
  <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>                 <span style="color: #339933;">/*</span> restore registers <span style="color: #339933;">*/</span>
  <span style="color: #46aa03; font-weight: bold;">bx</span> lr
offset_of_GOT<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> _GLOBAL_OFFSET_TABLE_  <span style="color: #339933;">-</span> <span style="color: #009900; font-weight: bold;">(</span>got_address <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span>
myvar_in_GOT <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> myvar<span style="color: #009900; font-weight: bold;">(</span>GOT<span style="color: #009900; font-weight: bold;">)</span>
addr_of_message<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> message</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* mylib.s */
.data

.balign 4
.globl myvar
myvar : .word 42 /* global variable "myvar" */
.size myvar, .-myvar

message: .asciz "Value of 'myvar' is %d\n"

.text

.balign 4
.globl myfun
myfun:
  push {r4, lr}                /* we are going to do a
                                  call so keep lr, and also r4
                                  for a 8-byte aligned stack */
  ldr r0, offset_of_GOT        /* r0 ← offset-to-GOT
                                  (respect to got_address)*/
  got_address: add r0, pc, r0  /* r0 ← pc + r0 
                                  this is
                                    r0 ← &amp;GOT */
  ldr r1, myvar_in_GOT         /* r1 ← offset-of-myvar-inside-GOT */
  ldr r0, [r0, r1]             /* r0 ← *(r0 + r1)
                                  this is
                                    r0 ← *(&amp;GOT + offset-of-myvar-inside-GOT) */
  ldr r1, [r0]                 /* r0 ← *r1 */

  ldr r0, addr_of_message      /* r0 ← &amp;message */
  /* r1 already contains the value we want */
  bl printf@PLT                /* call to printf via the PLT */

  pop {r4, lr}                 /* restore registers */
  bx lr
offset_of_GOT: .word _GLOBAL_OFFSET_TABLE_  - (got_address + 8)
myvar_in_GOT : .word myvar(GOT)
addr_of_message: .word message</p></div>

<p>
Now let’s change the main program so it first calls <code>myfun</code>, increments <code>myvar</code> and calls <code>myfun</code> again.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> main<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
<span style="color: #339933;">.</span>globl main
&nbsp;
<span style="color: #339933;">.</span>balign <span style="color: #ff0000;">4</span>
&nbsp;
main<span style="color: #339933;">:</span>
  <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>                <span style="color: #339933;">/*</span> we are going to <span style="color: #0000ff; font-weight: bold;">do</span> a
                                  <span style="color: #00007f; font-weight: bold;">call</span> so keep lr<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">and</span> also r4
                                  for a <span style="color: #ff0000;">8</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
  <span style="color: #46aa03; font-weight: bold;">bl</span> myfun@PLT                 <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> function <span style="color: #00007f; font-weight: bold;">in</span> library <span style="color: #339933;">*/</span>
&nbsp;
  ldr r0<span style="color: #339933;">,</span> offset_of_GOT        <span style="color: #339933;">/*</span> r0 ← offset<span style="color: #339933;">-</span>to<span style="color: #339933;">-</span>GOT
                                  <span style="color: #009900; font-weight: bold;">(</span>respect to got_address<span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">*/</span>
  got_address<span style="color: #339933;">:</span> <span style="color: #00007f; font-weight: bold;">add</span> r0<span style="color: #339933;">,</span> pc<span style="color: #339933;">,</span> r0  <span style="color: #339933;">/*</span> r0 ← pc <span style="color: #339933;">+</span> r0 
                                  this is
                                    r0 ← &amp;GOT <span style="color: #339933;">*/</span>
  ldr r1<span style="color: #339933;">,</span> myvar_in_GOT         <span style="color: #339933;">/*</span> r1 ← offset<span style="color: #339933;">-</span>of<span style="color: #339933;">-</span>myvar<span style="color: #339933;">-</span>inside<span style="color: #339933;">-</span>GOT <span style="color: #339933;">*/</span>
  ldr r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #339933;">,</span> r1<span style="color: #009900; font-weight: bold;">]</span>             <span style="color: #339933;">/*</span> r0 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r0 <span style="color: #339933;">+</span> r1<span style="color: #009900; font-weight: bold;">)</span>
                                  this is
                                    r0 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>&amp;GOT <span style="color: #339933;">+</span> offset<span style="color: #339933;">-</span>of<span style="color: #339933;">-</span>myvar<span style="color: #339933;">-</span>inside<span style="color: #339933;">-</span>GOT<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
  ldr r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>                 <span style="color: #339933;">/*</span> r0 ← <span style="color: #339933;">*</span>r1 <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">add</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>               <span style="color: #339933;">/*</span> r1 ← r1 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">str</span> r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>                 <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span>r0 ← r1 <span style="color: #339933;">*/</span>
&nbsp;
  <span style="color: #46aa03; font-weight: bold;">bl</span> myfun@PLT                 <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> function <span style="color: #00007f; font-weight: bold;">in</span> library a second time <span style="color: #339933;">*/</span>
&nbsp;
  <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>                 <span style="color: #339933;">/*</span> restore registers <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>                   <span style="color: #339933;">/*</span> end as usual <span style="color: #339933;">*/</span>
  <span style="color: #46aa03; font-weight: bold;">bx</span> lr
&nbsp;
offset_of_GOT<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> _GLOBAL_OFFSET_TABLE_  <span style="color: #339933;">-</span> <span style="color: #009900; font-weight: bold;">(</span>got_address <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span>
myvar_in_GOT <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> myvar<span style="color: #009900; font-weight: bold;">(</span>GOT<span style="color: #009900; font-weight: bold;">)</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* main.s */
.data

.text
.globl main

.balign 4

main:
  push {r4, lr}                /* we are going to do a
                                  call so keep lr, and also r4
                                  for a 8-byte aligned stack */
  bl myfun@PLT                 /* call function in library */

  ldr r0, offset_of_GOT        /* r0 ← offset-to-GOT
                                  (respect to got_address)*/
  got_address: add r0, pc, r0  /* r0 ← pc + r0 
                                  this is
                                    r0 ← &amp;GOT */
  ldr r1, myvar_in_GOT         /* r1 ← offset-of-myvar-inside-GOT */
  ldr r0, [r0, r1]             /* r0 ← *(r0 + r1)
                                  this is
                                    r0 ← *(&amp;GOT + offset-of-myvar-inside-GOT) */
  ldr r1, [r0]                 /* r0 ← *r1 */
  add r1, r1, #1               /* r1 ← r1 + 1 */
  str r1, [r0]                 /* *r0 ← r1 */

  bl myfun@PLT                 /* call function in library a second time */

  pop {r4, lr}                 /* restore registers */
  mov r0, #0                   /* end as usual */
  bx lr

offset_of_GOT: .word _GLOBAL_OFFSET_TABLE_  - (got_address + 8)
myvar_in_GOT : .word myvar(GOT)</p></div>

<p>
Let’s build it.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="bash" style="font-family:monospace;"><span style="color: #666666; font-style: italic;"># Dynamic library</span>
<span style="color: #c20cb9; font-weight: bold;">as</span> <span style="color: #660033;">-o</span> mylib.o mylib.s
<span style="color: #c20cb9; font-weight: bold;">gcc</span> <span style="color: #660033;">-shared</span> <span style="color: #660033;">-o</span> mylib.so mylib.o
<span style="color: #666666; font-style: italic;"># Program</span>
<span style="color: #c20cb9; font-weight: bold;">as</span> <span style="color: #660033;">-o</span> main.o main.s
<span style="color: #c20cb9; font-weight: bold;">gcc</span> <span style="color: #660033;">-o</span> main main.o -L. -l:mylib.so -Wl,-rpath,$<span style="color: #7a0874; font-weight: bold;">(</span><span style="color: #7a0874; font-weight: bold;">pwd</span><span style="color: #7a0874; font-weight: bold;">)</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;"># Dynamic library
as -o mylib.o mylib.s
gcc -shared -o mylib.so mylib.o
# Program
as -o main.o main.s
gcc -o main main.o -L. -l:mylib.so -Wl,-rpath,$(pwd)</p></div>

<p>
We can check it is using our library.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="bash" style="font-family:monospace;">$ <span style="color: #c20cb9; font-weight: bold;">ldd</span> main
	<span style="color: #000000; font-weight: bold;">/</span>usr<span style="color: #000000; font-weight: bold;">/</span>lib<span style="color: #000000; font-weight: bold;">/</span>arm-linux-gnueabihf<span style="color: #000000; font-weight: bold;">/</span>libcofi_rpi.so <span style="color: #7a0874; font-weight: bold;">(</span>0xb6f3b000<span style="color: #7a0874; font-weight: bold;">)</span>
	mylib.so =<span style="color: #000000; font-weight: bold;">&gt;</span> $<span style="color: #7a0874; font-weight: bold;">(</span><span style="color: #7a0874; font-weight: bold;">pwd</span><span style="color: #7a0874; font-weight: bold;">)</span><span style="color: #000000; font-weight: bold;">/</span>mylib.so <span style="color: #7a0874; font-weight: bold;">(</span>0xb6f32000<span style="color: #7a0874; font-weight: bold;">)</span>
	libc.so.6 =<span style="color: #000000; font-weight: bold;">&gt;</span> <span style="color: #000000; font-weight: bold;">/</span>lib<span style="color: #000000; font-weight: bold;">/</span>arm-linux-gnueabihf<span style="color: #000000; font-weight: bold;">/</span>libc.so.6 <span style="color: #7a0874; font-weight: bold;">(</span>0xb6df0000<span style="color: #7a0874; font-weight: bold;">)</span>
	<span style="color: #000000; font-weight: bold;">/</span>lib<span style="color: #000000; font-weight: bold;">/</span>ld-linux-armhf.so.3 <span style="color: #7a0874; font-weight: bold;">(</span>0x7f5cb000<span style="color: #7a0874; font-weight: bold;">)</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ ldd main
	/usr/lib/arm-linux-gnueabihf/libcofi_rpi.so (0xb6f3b000)
	mylib.so =&gt; $(pwd)/mylib.so (0xb6f32000)
	libc.so.6 =&gt; /lib/arm-linux-gnueabihf/libc.so.6 (0xb6df0000)
	/lib/ld-linux-armhf.so.3 (0x7f5cb000)</p></div>

<p>
And run it.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="text" style="font-family:monospace;">$ ./main 
Value of 'myvar' is 42
Value of 'myvar' is 43</pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ ./main 
Value of 'myvar' is 42
Value of 'myvar' is 43</p></div>

<p>
Yay! <img draggable="false" class="emoji" alt="🙂" src="https://s.w.org/images/core/emoji/2.2.1/svg/1f642.svg" scale="0">
</p>
<p>
That’s all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2017/04/17/arm-assembler-raspberry-pi-chapter-27/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2017/04/17/arm-assembler-raspberry-pi-chapter-27/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2017/04/17/arm-assembler-raspberry-pi-chapter-27/&amp;text=ARM+assembler+in+Raspberry+Pi+%E2%80%93+Chapter+27+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2017/04/17/arm-assembler-raspberry-pi-chapter-27/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>						<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2017/04/02/optimization/" rel="prev">Whose is this optimization?</a></span>
			<span class="next"><a href="http://thinkingeek.com/2017/05/21/small-telegram-bot/" rel="next">A small Telegram Bot in Go</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2017/04/17/arm-assembler-raspberry-pi-chapter-27/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="3806" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="fffb045d69"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496667722125"></form>
			</div><!-- #respond -->
	</div>	</div>