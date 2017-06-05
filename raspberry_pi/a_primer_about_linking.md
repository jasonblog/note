# A primer about linking


<div class="main">
			<h1>ARM assembler in Raspberry Pi – Chapter 26 </h1>
		<p class="post-meta"><span class="date">October 30, 2016</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2016/10/30/arm-assembler-raspberry-pi-chapter-26/#comments">6</a></span> </p>
		<p>In this chapter we will talk about a fascinating step that is required to create a program, even when using assembler. Today we will talk about linking.</p>
<p><span id="more-3426"></span></p>
<h2>Linkers, the magic between symbols and addresses</h2>
<p>Linkers are an essential yet often forgotten tool. Their main job is sticking all the pieces that form our program in a way that it can be executed. The fundamental work of a link is binding symbolic names with addresses (i.e. physical names). This process is conceptually simple but it is full of interesting details. Linking is a necessary step when separate compilation is used.</p>
<h3>Separate compilation and modules</h3>
<p><em>Modules</em> are a mechanism in which programming languages let their users split programs in different logical parts. Modularization requires some amount of support from the tools that implement the programming language. <em>Separate compilation</em> is a mechanism to achieve this. In C, a program may be decomposed in several source files. Usually compiling a C source file generates an <em>object file</em>, thus several source files will lead to several object files. These object files are combined using a linker. The linker generates the final program.</p>
<h2>ELF</h2>
<p>Given that several tools manipulate object files (compilers, assemblers, linkers) a common format comes handy. There are a few formats available for this purpose like COFF, Mach-O or ELF. In the UNIX world (including Linux) the most popular format is <a href="http://www.sco.com/developers/gabi/latest/contents.html">ELF (Executable and Linking Format)</a>. This format is used for object files (called relocatable objects, we will see below why), shared objects (dynamic libraries) and executables (the program itself).</p>
<p>For a linker, an ELF relocatable file is a collection of <em>sections</em>. Sections represent a contiguous chunk of data (which can be anything: instructions, initial values of global variables, debug information, etc). Each section has a name and attributes like whether it has to be allocated in memory, loaded from the image (i.e. the file that contains the program), whether it can be executed, whether it is writable, its size and alignment, etc.</p>
<h2>Labels as symbolic names</h2>
<p>When we use global variables we have to use the following schema:</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>1
2
3
4
5
6
7
8
9
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #0000ff; font-weight: bold;">.data</span><span style="color: #339933;">:</span>
var<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">42</span>
<span style="color: #0000ff; font-weight: bold;">.text</span>
func<span style="color: #339933;">:</span>
    <span style="color: #339933;">/*</span> <span style="color: #339933;">...</span> <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> addr_of_var  <span style="color: #339933;">/*</span> r0 ← &amp;var <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>         <span style="color: #339933;">/*</span> r0 ← <span style="color: #339933;">*</span>r0 <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> <span style="color: #339933;">...</span> <span style="color: #339933;">*/</span>
addr_of_var <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> var</pre></td></tr></tbody></table><p class="theCode" style="display:none;">.data:
var: .word 42
.text
func:
    /* ... */
    ldr r0, addr_of_var  /* r0 ← &amp;var */
    ldr r0, [r0]         /* r0 ← *r0 */
    /* ... */
addr_of_var : .word var</p></div>

<p>The reason is that in ARM instructions we cannot encode the full 32-bit address of a variable inside an instruction. So it makes sense to keep the address in a place, in this case in <code>addr_of_var</code>, which is amenable for finding it from the current instruction. In the case shown above, the assembler replaces the usage of <code>addr_of_var</code> into something like this:</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>6
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">   ldr r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>pc<span style="color: #339933;">,</span> #offset<span style="color: #009900; font-weight: bold;">]</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">   ldr r0, [pc, #offset]</p></div>

<p>Which means <em>load the value found in the given offset of the current instruction</em>. The assembler computes the right offset here so we do not have to. This is a valid approach because <code>addr_of_var</code> is found in the same section as the instruction. This means that it will for sure be located after the instructions. It also happens that it is close enough in memory. This addressing mode can encode any offset of 12-bit (plus a sign bit) so anything within 4096 bytes (i.e. within 1024 instructions) is addressable this way.</p>
<p>But the question that remains is, what does the assembler put in the that location designated by <code>addr_of_var</code>? We have written <code>.word var</code> but what does this mean? The assembler should emit the address of <code>var</code>, but at this point its address is unknown. So the assembler can only emit partial information at this point. This information will be completed later.</p>
<h2>An example</h2>
<p>Let’s consider a more complex example to see this process in action. Consider the following code that takes two global variables and adds them into a result variable. Then we call a function, that we will write in another file. This function will increment the result variable by one. The result variable has to be accessible from the other file, so we will have to mark it as global (similar to what we do with <code>main</code>).</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> main<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
one_var <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">42</span>
another_var <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">66</span>
&nbsp;
<span style="color: #339933;">.</span>globl result_var             <span style="color: #339933;">/*</span> mark result_var as <span style="color: #0000ff; font-weight: bold;">global</span> <span style="color: #339933;">*/</span>
result_var <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">0</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
<span style="color: #339933;">.</span>globl main
main<span style="color: #339933;">:</span>
    ldr r0<span style="color: #339933;">,</span> addr_one_var      <span style="color: #339933;">/*</span> r0 ← &amp;one_var <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>              <span style="color: #339933;">/*</span> r0 ← <span style="color: #339933;">*</span>r0 <span style="color: #339933;">*/</span>
    ldr r1<span style="color: #339933;">,</span> addr_another_var  <span style="color: #339933;">/*</span> r1 ← &amp;another_var <span style="color: #339933;">*/</span>
    ldr r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #009900; font-weight: bold;">]</span>              <span style="color: #339933;">/*</span> r1 ← <span style="color: #339933;">*</span>r1 <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r0<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> r1            <span style="color: #339933;">/*</span> r0 ← r0 <span style="color: #339933;">+</span> r1 <span style="color: #339933;">*/</span>
    ldr r1<span style="color: #339933;">,</span> addr_result       <span style="color: #339933;">/*</span> r1 ← &amp;result <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #009900; font-weight: bold;">]</span>              <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span>r1 ← r0 <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> inc_result             <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> to inc_result <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>                <span style="color: #339933;">/*</span> r0 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr                     <span style="color: #339933;">/*</span> return <span style="color: #339933;">*/</span>
&nbsp;
&nbsp;
addr_one_var  <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> one_var
addr_another_var  <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> another_var
addr_result  <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> result_var</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* main.s */
.data

one_var : .word 42
another_var : .word 66

.globl result_var             /* mark result_var as global */
result_var : .word 0

.text

.globl main
main:
    ldr r0, addr_one_var      /* r0 ← &amp;one_var */
    ldr r0, [r0]              /* r0 ← *r0 */
    ldr r1, addr_another_var  /* r1 ← &amp;another_var */
    ldr r1, [r1]              /* r1 ← *r1 */
    add r0, r0, r1            /* r0 ← r0 + r1 */
    ldr r1, addr_result       /* r1 ← &amp;result */
    str r0, [r1]              /* *r1 ← r0 */
    bl inc_result             /* call to inc_result */
    mov r0, #0                /* r0 ← 0 */
    bx lr                     /* return */
   

addr_one_var  : .word one_var
addr_another_var  : .word another_var
addr_result  : .word result_var</p></div>

<p>Let’s create an object file. Recall that an object file is an intermediate file that is used before we create the final program. Once created, we can use <code>objdump -d</code> to see the code contained in this object file. (The use of <code>-march=armv6</code> avoids some legacy info be emitted that would be confusing for the sake of the exposition)</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="bash" style="font-family:monospace;"><span style="color: #666666;">$ </span><span style="color: #c20cb9; font-weight: bold;">as</span> <span style="color: #660033;">-march</span>=armv6 <span style="color: #660033;">-o</span> main.o main.s      <span style="color: #666666; font-style: italic;"># creates object file main.o</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ as -march=armv6 -o main.o main.s      # creates object file main.o</p></div>

<h2>Relocations</h2>
<p>We said above that the assembler does not know the final value and instead may put some partial information (e.g. the offsets from <code>.data</code>). It also annotates that some <em>fix up</em> is required here. This <em>fix up</em> is called a <code>relocation</code>. We can read the relocations using flags <code>-dr</code> of <code>objdump</code>.</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="bash" style="font-family:monospace;"><span style="color: #666666;">$ </span>objdump <span style="color: #660033;">-dr</span> main.o</pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ objdump -dr main.o</p></div>


<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="text" style="font-family:monospace;">main.o:     file format elf32-littlearm
&nbsp;
Disassembly of section .text:
&nbsp;
00000000 &lt;main&gt;:
   0:	e59f0020 	ldr	r0, [pc, #32]	; 28 &lt;addr_one_var&gt;
   4:	e5900000 	ldr	r0, [r0]
   8:	e59f101c 	ldr	r1, [pc, #28]	; 2c &lt;addr_another_var&gt;
   c:	e5911000 	ldr	r1, [r1]
  10:	e0800001 	add	r0, r0, r1
  14:	e59f1014 	ldr	r1, [pc, #20]	; 30 &lt;addr_result&gt;
  18:	e5810000 	str	r0, [r1]
  1c:	ebfffffe 	bl	0 &lt;inc_result&gt;
			1c: R_ARM_CALL	inc_result
  20:	e3a00000 	mov	r0, #0
  24:	e12fff1e 	bx	lr
&nbsp;
00000028 &lt;addr_one_var&gt;:
  28:	00000000 	.word	0x00000000
			28: R_ARM_ABS32	.data
&nbsp;
0000002c &lt;addr_another_var&gt;:
  2c:	00000004 	.word	0x00000004
			2c: R_ARM_ABS32	.data
&nbsp;
00000030 &lt;addr_result&gt;:
  30:	00000000 	.word	0x00000000
			30: R_ARM_ABS32	result_var</pre></td></tr></tbody></table><p class="theCode" style="display:none;">main.o:     file format elf32-littlearm

Disassembly of section .text:

00000000 &lt;main&gt;:
   0:	e59f0020 	ldr	r0, [pc, #32]	; 28 &lt;addr_one_var&gt;
   4:	e5900000 	ldr	r0, [r0]
   8:	e59f101c 	ldr	r1, [pc, #28]	; 2c &lt;addr_another_var&gt;
   c:	e5911000 	ldr	r1, [r1]
  10:	e0800001 	add	r0, r0, r1
  14:	e59f1014 	ldr	r1, [pc, #20]	; 30 &lt;addr_result&gt;
  18:	e5810000 	str	r0, [r1]
  1c:	ebfffffe 	bl	0 &lt;inc_result&gt;
			1c: R_ARM_CALL	inc_result
  20:	e3a00000 	mov	r0, #0
  24:	e12fff1e 	bx	lr

00000028 &lt;addr_one_var&gt;:
  28:	00000000 	.word	0x00000000
			28: R_ARM_ABS32	.data

0000002c &lt;addr_another_var&gt;:
  2c:	00000004 	.word	0x00000004
			2c: R_ARM_ABS32	.data

00000030 &lt;addr_result&gt;:
  30:	00000000 	.word	0x00000000
			30: R_ARM_ABS32	result_var</p></div>

<p>Relocations are rendered the output above like</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="text" style="font-family:monospace;">			OFFSET: TYPE	VALUE</pre></td></tr></tbody></table><p class="theCode" style="display:none;">			OFFSET: TYPE	VALUE</p></div>

<p>
They are also printed right after the point they affect.
</p>
<p><code>OFFSET</code> is the offset inside the section for the bytes that will need fixing up (in this case all of them inside <code>.text</code>). <code>TYPE</code> is the kind of relocation. The kind of relocation determines which and how bytes are fixed up. <code>VALUE</code> is a symbolic entity for which we have to figure the physical address. It can be a real symbol, like <code>inc_result</code> and <code>result_var</code>, or a section name like <code>.data</code>.</p>
<p>In the current list, there is a relocation at <code>.text+1c</code> so we can call the actual <code>inc_result</code>. The other two relocations in <code>.text+28</code>, <code>.text+2c</code> are the relocations required to access <code>.data</code>. These relocations could have as <code>VALUE</code> the symbols <code>one_var</code> and <code>another_var</code> respectively but GNU as seems to prefer to represent them as offsets relative to <code>.data</code> section. Finally <code>.text+30</code> refers to the global symbol <code>result_var</code>.</p>
<p>Every relocation kind is defined in terms of a few parameters: <code>S</code> is the address of the symbol referred by the relocation (the <code>VALUE</code> above), <code>P</code> is the address of the <em>place</em> (the <code>OFFSET</code> plus the address of the section itself), <code>A</code> (for addenda) is the value that the assembler has left in place. In our example, <code>R_ARM_ABS32</code> it is the value of the <code>.word</code>, for <code>R_ARM_CALL</code> it is a set of bits in the <code>bl</code> instruction itself. Using these parameters, earch relocation has a related operation. Relocations of kind <code>R_ARM_ABS32</code> do an operation <code>S + A</code>. Relocations of kind <code>R_ARM_CALL</code> do an operation <code>(S + A) – P</code>.</p>
<div style="padding: 1em; background-color: #fee;">Due to Thumb, ARM relocations have an extra parameter <code>T</code> that has the value <code>1</code> if the symbol <code>S</code> is a Thumb function, <code>0</code> otherwise. This is not the case for our examples, so I have omitted <code>T</code> in the description of the relocations above</div>
<p>Before we can see the result computed by the linker, we will define <code>inc_result</code> otherwise linking will fail. This function will increment the value of <code>addr_result</code> (whose storage is defined in the first file <code>main.s</code>).</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> inc_result<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
<span style="color: #339933;">.</span>globl inc_result
inc_result<span style="color: #339933;">:</span>
    ldr r1<span style="color: #339933;">,</span> addr_result  <span style="color: #339933;">/*</span> r1 ← &amp;result <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #009900; font-weight: bold;">]</span>         <span style="color: #339933;">/*</span> r0 ← <span style="color: #339933;">*</span>r1 <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r0<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>       <span style="color: #339933;">/*</span> r0 ← r0 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #009900; font-weight: bold;">]</span>         <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span>r1 ← r0 <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr                <span style="color: #339933;">/*</span> return <span style="color: #339933;">*/</span>
&nbsp;
addr_result  <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> result_var</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* inc_result.s */
.text

.globl inc_result
inc_result:
    ldr r1, addr_result  /* r1 ← &amp;result */
    ldr r0, [r1]         /* r0 ← *r1 */
    add r0, r0, #1       /* r0 ← r0 + 1 */
    str r0, [r1]         /* *r1 ← r0 */
    bx lr                /* return */

addr_result  : .word result_var</p></div>

<p>Let’s check the relocations as well.</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="bash" style="font-family:monospace;">$ <span style="color: #c20cb9; font-weight: bold;">as</span> <span style="color: #660033;">-march</span>=armv6 <span style="color: #660033;">-o</span> inc_result.o inc_result.s
$ objdump <span style="color: #660033;">-dr</span> inc_result.o</pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ as -march=armv6 -o inc_result.o inc_result.s
$ objdump -dr inc_result.o</p></div>


<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="text" style="font-family:monospace;">inc_result.o:     file format elf32-littlearm
&nbsp;
Disassembly of section .text:
&nbsp;
00000000 &lt;inc_result&gt;:
   0:	e59f100c 	ldr	r1, [pc, #12]	; 14 &lt;addr_result&gt;
   4:	e5910000 	ldr	r0, [r1]
   8:	e2800001 	add	r0, r0, #1
   c:	e5810000 	str	r0, [r1]
  10:	e12fff1e 	bx	lr
&nbsp;
00000014 &lt;addr_result&gt;:
  14:	00000000 	.word	0x00000000
			14: R_ARM_ABS32	result_var</pre></td></tr></tbody></table><p class="theCode" style="display:none;">inc_result.o:     file format elf32-littlearm

Disassembly of section .text:

00000000 &lt;inc_result&gt;:
   0:	e59f100c 	ldr	r1, [pc, #12]	; 14 &lt;addr_result&gt;
   4:	e5910000 	ldr	r0, [r1]
   8:	e2800001 	add	r0, r0, #1
   c:	e5810000 	str	r0, [r1]
  10:	e12fff1e 	bx	lr

00000014 &lt;addr_result&gt;:
  14:	00000000 	.word	0x00000000
			14: R_ARM_ABS32	result_var</p></div>

<p>We can see that it has a relocation for <code>result_var</code> as expected.</p>
<p>Now we can combine the two object files to generate an executable binary.</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="bash" style="font-family:monospace;"><span style="color: #666666;">$ </span><span style="color: #c20cb9; font-weight: bold;">gcc</span> <span style="color: #660033;">-o</span> test.exe print_float.o reloc.o</pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ gcc -o test.exe print_float.o reloc.o</p></div>

<p>And check the contents of the file. Our program will include a few functions from the C library that we can ignore.</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="bash" style="font-family:monospace;"><span style="color: #666666;">$ </span>objdump <span style="color: #660033;">-d</span> test.exe</pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ objdump -d test.exe</p></div>


<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="text" style="font-family:monospace;">...
00008390 &lt;main&gt;:
    8390:       e59f0020        ldr     r0, [pc, #32]   ; 83b8 &lt;addr_one_var&gt;
    8394:       e5900000        ldr     r0, [r0]
    8398:       e59f101c        ldr     r1, [pc, #28]   ; 83bc &lt;addr_another_var&gt;
    839c:       e5911000        ldr     r1, [r1]
    83a0:       e0800001        add     r0, r0, r1
    83a4:       e59f1014        ldr     r1, [pc, #20]   ; 83c0 &lt;addr_result&gt;
    83a8:       e5810000        str     r0, [r1]
    83ac:       eb000004        bl      83c4 &lt;inc_result&gt;
    83b0:       e3a00000        mov     r0, #0
    83b4:       e12fff1e        bx      lr
&nbsp;
000083b8 &lt;addr_one_var&gt;:
    83b8:       00010578        .word   0x00010578
&nbsp;
000083bc &lt;addr_another_var&gt;:
    83bc:       0001057c        .word   0x0001057c
&nbsp;
000083c0 &lt;addr_result&gt;:
    83c0:       00010580        .word   0x00010580
&nbsp;
000083c4 &lt;inc_result&gt;:
    83c4:       e59f100c        ldr     r1, [pc, #12]   ; 83d8 &lt;addr_result&gt;
    83c8:       e5910000        ldr     r0, [r1]
    83cc:       e2800001        add     r0, r0, #1
    83d0:       e5810000        str     r0, [r1]
    83d4:       e12fff1e        bx      lr
&nbsp;
000083d8 &lt;addr_result&gt;:
    83d8:       00010580        .word   0x00010580
&nbsp;
...</pre></td></tr></tbody></table><p class="theCode" style="display:none;">...
00008390 &lt;main&gt;:
    8390:       e59f0020        ldr     r0, [pc, #32]   ; 83b8 &lt;addr_one_var&gt;
    8394:       e5900000        ldr     r0, [r0]
    8398:       e59f101c        ldr     r1, [pc, #28]   ; 83bc &lt;addr_another_var&gt;
    839c:       e5911000        ldr     r1, [r1]
    83a0:       e0800001        add     r0, r0, r1
    83a4:       e59f1014        ldr     r1, [pc, #20]   ; 83c0 &lt;addr_result&gt;
    83a8:       e5810000        str     r0, [r1]
    83ac:       eb000004        bl      83c4 &lt;inc_result&gt;
    83b0:       e3a00000        mov     r0, #0
    83b4:       e12fff1e        bx      lr

000083b8 &lt;addr_one_var&gt;:
    83b8:       00010578        .word   0x00010578

000083bc &lt;addr_another_var&gt;:
    83bc:       0001057c        .word   0x0001057c

000083c0 &lt;addr_result&gt;:
    83c0:       00010580        .word   0x00010580

000083c4 &lt;inc_result&gt;:
    83c4:       e59f100c        ldr     r1, [pc, #12]   ; 83d8 &lt;addr_result&gt;
    83c8:       e5910000        ldr     r0, [r1]
    83cc:       e2800001        add     r0, r0, #1
    83d0:       e5810000        str     r0, [r1]
    83d4:       e12fff1e        bx      lr

000083d8 &lt;addr_result&gt;:
    83d8:       00010580        .word   0x00010580

...</p></div>

<p>From the output above we can observe that <code>addr_one_var</code> is in address <code>0x00010578</code>, <code>addr_another_var</code> is in address <code>0x0001057c</code> and <code>addr_result</code> is in address <code>0x00010580</code>. The last one appears repeated, but this is because both files <code>main.s</code> and <code>inc_result.s</code> refer to it so they need to keep the address somewhere. Note that in both cases it contains the same address.</p>
<p>Let’s start with the relocations of <code>addr_one_var</code>, <code>addr_another_var</code> and <code>addr_result</code>. These three relocations were <code>R_ARM_ABS32</code> so their operation is <code>S + A</code>. <code>S</code> is the address of section <code>.data</code> whose address can be determined also with <code>objdump -h</code> (plus flag <code>-w</code> to make it a bit more readable). A file may contain many sections so I will omit the uninteresting ones.</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="bash" style="font-family:monospace;"><span style="color: #666666;">$ </span>objdump <span style="color: #660033;">-hw</span> test.exe</pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ objdump -hw test.exe</p></div>


<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="text" style="font-family:monospace;">test.exe:     file format elf32-littlearm
&nbsp;
Sections:
Idx Name          Size      VMA       LMA       File off  Algn  Flags
...
 13 .text         0000015c  000082e4  000082e4  000002e4  2**2  CONTENTS, ALLOC, LOAD, READONLY, CODE
...
 23 .data         00000014  00010570  00010570  00000570  2**2  CONTENTS, ALLOC, LOAD, DATA
...</pre></td></tr></tbody></table><p class="theCode" style="display:none;">test.exe:     file format elf32-littlearm

Sections:
Idx Name          Size      VMA       LMA       File off  Algn  Flags
...
 13 .text         0000015c  000082e4  000082e4  000002e4  2**2  CONTENTS, ALLOC, LOAD, READONLY, CODE
...
 23 .data         00000014  00010570  00010570  00000570  2**2  CONTENTS, ALLOC, LOAD, DATA
...</p></div>

<p>Column <code>VMA</code> defines the address of the section. In our case <code>.data</code> is located at <code>00010570</code>. And our variables are found in <code>0x00010578</code>, 0x0001057c and <code>0x00010580</code>. These are offsets 8, 12 and 16 respectively from the beginning of <code>.data</code>. The linker has laid some other variables in this section before ours. We can see this asking the linker to print a map of the generated executable.</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="bash" style="font-family:monospace;">$ <span style="color: #c20cb9; font-weight: bold;">gcc</span> <span style="color: #660033;">-o</span> test.exe main.o inc_result.o -Wl,--print-map <span style="color: #000000; font-weight: bold;">&gt;</span> map.txt
$ <span style="color: #c20cb9; font-weight: bold;">cat</span> map.txt</pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ gcc -o test.exe main.o inc_result.o -Wl,--print-map &gt; map.txt
$ cat map.txt</p></div>


<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>314
315
316
317
318
319
320
321
322
323
324
325
326
327
328
</pre></td><td class="code"><pre class="text" style="font-family:monospace;">.data           0x00010570       0x14
                0x00010570                PROVIDE (__data_start, .)
 *(.data .data.* .gnu.linkonce.d.*)
 .data          0x00010570        0x4 /usr/lib/gcc/arm-linux-gnueabihf/4.6/../../../arm-linux-gnueabihf/crt1.o
                0x00010570                data_start
                0x00010570                __data_start
 .data          0x00010574        0x0 /usr/lib/gcc/arm-linux-gnueabihf/4.6/../../../arm-linux-gnueabihf/crti.o
 .data          0x00010574        0x4 /usr/lib/gcc/arm-linux-gnueabihf/4.6/crtbegin.o
                0x00010574                __dso_handle
 .data          0x00010578        0xc main.o
                0x00010580                result_var
 .data          0x00010584        0x0 inc_result.o
 .data          0x00010584        0x0 /usr/lib/arm-linux-gnueabihf/libc_nonshared.a(elf-init.oS)
 .data          0x00010584        0x0 /usr/lib/gcc/arm-linux-gnueabihf/4.6/crtend.o
 .data          0x00010584        0x0 /usr/lib/gcc/arm-linux-gnueabihf/4.6/../../../arm-linux-gnueabihf/cr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">.data           0x00010570       0x14
                0x00010570                PROVIDE (__data_start, .)
 *(.data .data.* .gnu.linkonce.d.*)
 .data          0x00010570        0x4 /usr/lib/gcc/arm-linux-gnueabihf/4.6/../../../arm-linux-gnueabihf/crt1.o
                0x00010570                data_start
                0x00010570                __data_start
 .data          0x00010574        0x0 /usr/lib/gcc/arm-linux-gnueabihf/4.6/../../../arm-linux-gnueabihf/crti.o
 .data          0x00010574        0x4 /usr/lib/gcc/arm-linux-gnueabihf/4.6/crtbegin.o
                0x00010574                __dso_handle
 .data          0x00010578        0xc main.o
                0x00010580                result_var
 .data          0x00010584        0x0 inc_result.o
 .data          0x00010584        0x0 /usr/lib/arm-linux-gnueabihf/libc_nonshared.a(elf-init.oS)
 .data          0x00010584        0x0 /usr/lib/gcc/arm-linux-gnueabihf/4.6/crtend.o
 .data          0x00010584        0x0 /usr/lib/gcc/arm-linux-gnueabihf/4.6/../../../arm-linux-gnueabihf/cr</p></div>

<p>If you check lines 317 to 322, you will see that that the final <code>.data</code> section (that effectively starts <code>0x00010570</code> as we checked above) of our program includes 4 bytes from <code>crt1.o</code> for the symbols <code>data_start</code> (and its alias <code>__data_start</code>). File <code>crtbegin.o</code> also has contributed a symbol <code>__dso_handle</code>. These global symbols come from the C library. Only symbol <code>result_var</code> appears here because is a global symbol, all other global variables are not global symbols. The storage, though, is accounted for all of them in line 323. They take 0xc bytes (i.e. 12 bytes because of 3 variables each one of 4 bytes).</p>
<p>So with this info we can infer what has happened: variable <code>one_var</code> is in address 0x00010570, variable <code>another_var</code> is in 0x00010574 and variable result_var is in 0x00010578. If you check the result of <code>objdump -d test.exe </code>above you will see that</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="text" style="font-family:monospace;">000083b8 &lt;addr_one_var&gt;:
    83b8:       00010578        .word   0x00010578
&nbsp;
000083bc &lt;addr_another_var&gt;:
    83bc:       0001057c        .word   0x0001057c
&nbsp;
000083c0 &lt;addr_result&gt;:
    83c0:       00010580        .word   0x00010580
...
000083d8 &lt;addr_result&gt;:
    83d8:       00010580        .word   0x00010580</pre></td></tr></tbody></table><p class="theCode" style="display:none;">000083b8 &lt;addr_one_var&gt;:
    83b8:       00010578        .word   0x00010578

000083bc &lt;addr_another_var&gt;:
    83bc:       0001057c        .word   0x0001057c

000083c0 &lt;addr_result&gt;:
    83c0:       00010580        .word   0x00010580
...
000083d8 &lt;addr_result&gt;:
    83d8:       00010580        .word   0x00010580</p></div>

<p>What about the call to <code>inc_result</code>?</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="text" style="font-family:monospace;">    83ac:       eb000004        bl      83c4</pre></td></tr></tbody></table><p class="theCode" style="display:none;">    83ac:       eb000004        bl      83c4</p></div>

<p>This one is a bit more involved. Recall that the relocation operation is <code>(S + A) - P</code>. Here <code>A</code> is <code>0</code> and <code>P</code> is <code>0x000083ac</code>, S is <code>0x000083c4</code>. So the relocation has to define an offset of 24 bytes (83c4 – 83ac is 24<sub>(10</sub>). Instruction <code>bl</code> encodes the offset by shifting it 2 bits to the right. So the current offset encoded in <code>eb000004</code> is 16. Recall that the current <code>pc</code> points to the current instruction plus 8 bytes, so this instruction is exactly telling us to jump to an offset + 24 bytes. Exactly what we wanted.</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="text" style="font-family:monospace;">...
    83ac:       eb000004        bl      83c4 &lt;inc_result&gt;
    83b0:       e3a00000        mov     r0, #0
    83b4:       e12fff1e        bx      lr
&nbsp;
000083b8 &lt;addr_one_var&gt;:
    83b8:       00010578        .word   0x00010578
&nbsp;
000083bc &lt;addr_another_var&gt;:
    83bc:       0001057c        .word   0x0001057c
&nbsp;
000083c0 &lt;addr_result&gt;:
    83c0:       00010580        .word   0x00010580
&nbsp;
000083c4 &lt;inc_result&gt;:
    83c4:       e59f100c        ldr     r1, [pc, #12]   ; 83d8 &lt;addr_result&gt;
&nbsp;
...</pre></td></tr></tbody></table><p class="theCode" style="display:none;">...
    83ac:       eb000004        bl      83c4 &lt;inc_result&gt;
    83b0:       e3a00000        mov     r0, #0
    83b4:       e12fff1e        bx      lr

000083b8 &lt;addr_one_var&gt;:
    83b8:       00010578        .word   0x00010578

000083bc &lt;addr_another_var&gt;:
    83bc:       0001057c        .word   0x0001057c

000083c0 &lt;addr_result&gt;:
    83c0:       00010580        .word   0x00010580

000083c4 &lt;inc_result&gt;:
    83c4:       e59f100c        ldr     r1, [pc, #12]   ; 83d8 &lt;addr_result&gt;

...</p></div>

<h2>More information</h2>
<p>
Linkers are a bit of arcana because they must handle with the lowest level parts of code. So sometimes it is hard to find good resources on them.
</p>
<p>
Ian Lance Taylor, author of <code>gold</code>, made a very nice <a href="https://lwn.net/Articles/276782/">linker essay in 20 chapters</a>. If you want a book, <a href="https://www.amazon.com/Linkers-Kaufmann-Software-Engineering-Programming/dp/1558604960">Linkers &amp; Loaders</a> is not a bad one. The ELF standard is actually defined in two parts, a <a href="http://www.sco.com/developers/gabi/latest/contents.html">generic</a> one and a processor specific one, including <a href="http://infocenter.arm.com/help/topic/com.arm.doc.ihi0044f/IHI0044F_aaelf.pdf">one for ARM</a>.
</p>
<p>
That’s all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2016/10/30/arm-assembler-raspberry-pi-chapter-26/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2016/10/30/arm-assembler-raspberry-pi-chapter-26/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2016/10/30/arm-assembler-raspberry-pi-chapter-26/&amp;text=ARM+assembler+in+Raspberry+Pi+%E2%80%93+Chapter+26+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2016/10/30/arm-assembler-raspberry-pi-chapter-26/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>						<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2016/10/23/exploring-aarch64-assembler-chapter-4/" rel="prev">Exploring AArch64 assembler – Chapter 4</a></span>
			<span class="next"><a href="http://thinkingeek.com/2016/11/13/exploring-aarch64-assembler-chapter-5/" rel="next">Exploring AArch64 assembler – Chapter 5</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
			<h3>6 thoughts on “<span>ARM assembler in Raspberry Pi – Chapter 26</span>”</h3>
		<ul class="commentlist">
				<li class="comment even thread-even depth-1 parent" id="comment-978457">
				<div id="div-comment-978457" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/8e02ba12a3b8f9d19306c56fbde0d455?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/8e02ba12a3b8f9d19306c56fbde0d455?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/8e02ba12a3b8f9d19306c56fbde0d455?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-8e02ba12a3b8f9d19306c56fbde0d455-0">			<cite class="fn"><a href="http://www.utdallas.edu/~pervin" rel="external nofollow" class="url">William J. Pervin</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/30/arm-assembler-raspberry-pi-chapter-26/#comment-978457">
			November 1, 2016 at 6:23 pm</a>		</div>

		<p>Nice to see you back and giving everybody a very interesting article. I have not given up on finishing the Assembler book but am waiting for some feedback from colleagues here at my University. We will use it next Fall semester. Question: I’d really like to put your current company (******) under your name on the book. Could you let me do that?</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/30/arm-assembler-raspberry-pi-chapter-26/?replytocom=978457#respond" onclick="return addComment.moveForm( &quot;div-comment-978457&quot;, &quot;978457&quot;, &quot;respond&quot;, &quot;3426&quot; )" aria-label="Reply to William J. Pervin">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-978468">
				<div id="div-comment-978468" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-0">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/30/arm-assembler-raspberry-pi-chapter-26/#comment-978468">
			November 1, 2016 at 9:17 pm</a>		</div>

		<p>Hi William, </p>
<p>no, please do not put the name of my company in the book. This blog is completely unrelated to it.</p>
<p>Hope you can understand.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/30/arm-assembler-raspberry-pi-chapter-26/?replytocom=978468#respond" onclick="return addComment.moveForm( &quot;div-comment-978468&quot;, &quot;978468&quot;, &quot;respond&quot;, &quot;3426&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-982624">
				<div id="div-comment-982624" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/c95e80f39257c3d9bb517ebd61b3ea9f?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/c95e80f39257c3d9bb517ebd61b3ea9f?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/c95e80f39257c3d9bb517ebd61b3ea9f?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-c95e80f39257c3d9bb517ebd61b3ea9f-0">			<cite class="fn">loveWangkaka</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/30/arm-assembler-raspberry-pi-chapter-26/#comment-982624">
			December 26, 2016 at 1:29 pm</a>		</div>

		<p>excellent article!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/30/arm-assembler-raspberry-pi-chapter-26/?replytocom=982624#respond" onclick="return addComment.moveForm( &quot;div-comment-982624&quot;, &quot;982624&quot;, &quot;respond&quot;, &quot;3426&quot; )" aria-label="Reply to loveWangkaka">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-983618">
				<div id="div-comment-983618" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-1">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/30/arm-assembler-raspberry-pi-chapter-26/#comment-983618">
			January 8, 2017 at 4:35 pm</a>		</div>

		<p>Thanks!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/30/arm-assembler-raspberry-pi-chapter-26/?replytocom=983618#respond" onclick="return addComment.moveForm( &quot;div-comment-983618&quot;, &quot;983618&quot;, &quot;respond&quot;, &quot;3426&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1 parent" id="comment-988807">
				<div id="div-comment-988807" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a7643c7c06f839fdbdd0a31be4f5b999?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a7643c7c06f839fdbdd0a31be4f5b999?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a7643c7c06f839fdbdd0a31be4f5b999?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a7643c7c06f839fdbdd0a31be4f5b999-0">			<cite class="fn">DavidS</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/30/arm-assembler-raspberry-pi-chapter-26/#comment-988807">
			March 19, 2017 at 1:05 am</a>		</div>

		<p>Nice tutorial.  I am looking at it to get into assembly on Linux, i have long done ARM Assembly in RISC OS.</p>
<p>Why do you use bx lr to return?   The cannicle way is to use MOV R15,R14 (or MOV PC,LR if you like the APCS-R register names).</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/30/arm-assembler-raspberry-pi-chapter-26/?replytocom=988807#respond" onclick="return addComment.moveForm( &quot;div-comment-988807&quot;, &quot;988807&quot;, &quot;respond&quot;, &quot;3426&quot; )" aria-label="Reply to DavidS">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-988846">
				<div id="div-comment-988846" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-2">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2016/10/30/arm-assembler-raspberry-pi-chapter-26/#comment-988846">
			March 19, 2017 at 4:28 pm</a>		</div>

		<p>Hi David,</p>
<p>I thought it was clearer for the sake of the exposition to avoid using updates to the <code>pc</code> for control flow. For the purposes of the tutorial it would not make much difference to use <code>mov pc, lr</code> in most places. It would only make a difference if our ARM routine were called from a Thumb routine.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2016/10/30/arm-assembler-raspberry-pi-chapter-26/?replytocom=988846#respond" onclick="return addComment.moveForm( &quot;div-comment-988846&quot;, &quot;988846&quot;, &quot;respond&quot;, &quot;3426&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		</ul>
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2016/10/30/arm-assembler-raspberry-pi-chapter-26/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="3426" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="687c86db96"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496667483651"></form>
			</div><!-- #respond -->
	</div>	</div>