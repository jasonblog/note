# Subword data


<div class="main">
			<h1>ARM assembler in Raspberry Pi – Chapter 21 </h1>
		<p class="post-meta"><span class="date">August 23, 2014</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2014/08/23/arm-assembler-raspberry-pi-chapter-21/#comments">6</a></span> </p>
		<p>
We already know that ARM is a 32-bit architecture: general purpose registers are 32-bit wide and addresses in memory are 32-bit numbers. The natural integer size for an architecture is usually called a <em>word</em> and in ARM is obviously 32-bit integers. Sometimes, though, we need to deal with <em>subword</em> data: integers of size smaller than 32 bits.
</p>
<p><span id="more-1609"></span></p>
<h2>Subword data</h2>
<p>
In this chapter subword data will refer either to a <em>byte</em> or to a <em>halfword</em>. A byte is an integer of 8-bit and a halfword is an integer of 16-bit. Thus, a halfword occupies 2 bytes and a word 4 bytes.
</p>
<p>
To define storage for a byte in the data section we have to use <code>.byte</code>. For a halfword the syntax is <code>.hword</code>.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
one_byte<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">byte</span> <span style="color: #ff0000;">205</span>
<span style="color: #339933;">/*</span> This number <span style="color: #00007f; font-weight: bold;">in</span> binary is <span style="color: #ff0000;">11001101</span> <span style="color: #339933;">*/</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
one_halfword<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>hword <span style="color: #ff0000;">42445</span>
<span style="color: #339933;">/*</span> This number <span style="color: #00007f; font-weight: bold;">in</span> binary is <span style="color: #ff0000;">1010010111001101</span> <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">.align 4
one_byte: .byte 205
/* This number in binary is 11001101 */

.align 4
one_halfword: .hword 42445
/* This number in binary is 1010010111001101 */</p></div>

<p>
Note that, as usual, we are aligning data to 4 bytes. Later on we will see that for subword data alignment restrictions are slightly more relaxed.
</p>
<h2>Load and store</h2>
<p>
Before we start operating a subword integer we need to get it somewhere. If we are not going to load/store it from/to memory, we may simply use a register. We may have to check that we do not overflow the range of the subword, but that’s all.
</p>
<p>
But if the data is in memory then it is important to load it properly since we do not want to read more data than actually needed. Recall that an address actually identifies a single byte of the memory: it is not possible to address anything smaller than a byte. Depending on the <em>width</em> of the load/store, the address will load/store 1 byte, 2 bytes or 4 bytes. A regular <code>ldr</code> loads a word, so we need some other instruction.
</p>
<p>
ARM provides the instructions <code>ldrb</code> and <code>ldrh</code> to load a byte and a halfword respectively. The destination is a general purpose register, of 32-bit, so this instruction must extend the value from 8 or 16 bits to 32 bits. Both <code>ldrb</code> and <code>ldrh</code> perform <em>zero-extension</em>, which means that all the extra bits, not loaded, will be set to zero.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
<span style="color: #339933;">.</span>globl main
main<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
&nbsp;
    ldr r0<span style="color: #339933;">,</span> addr_of_one_byte     <span style="color: #339933;">/*</span> r0 ← &amp;one_byte <span style="color: #339933;">*/</span>
    ldrb r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>                <span style="color: #339933;">/*</span> r0 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">{</span><span style="color: #0000ff; font-weight: bold;">byte</span><span style="color: #009900; font-weight: bold;">}</span>r0 <span style="color: #339933;">*/</span>
&nbsp;
    ldr r1<span style="color: #339933;">,</span> addr_of_one_halfword <span style="color: #339933;">/*</span> r1 ← &amp;one_halfword <span style="color: #339933;">*/</span>
    ldrh r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #009900; font-weight: bold;">]</span>                <span style="color: #339933;">/*</span> r1 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">{</span>half<span style="color: #009900; font-weight: bold;">}</span>r1 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr
&nbsp;
addr_of_one_byte<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> one_byte
addr_of_one_halfword<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> one_halfword</pre></td></tr></tbody></table><p class="theCode" style="display:none;">.text

.globl main
main:
    push {r4, lr}

    ldr r0, addr_of_one_byte     /* r0 ← &amp;one_byte */
    ldrb r0, [r0]                /* r0 ← *{byte}r0 */

    ldr r1, addr_of_one_halfword /* r1 ← &amp;one_halfword */
    ldrh r1, [r1]                /* r1 ← *{half}r1 */

    pop {r4, lr}
    mov r0, #0
    bx lr

addr_of_one_byte: .word one_byte
addr_of_one_halfword: .word one_halfword</p></div>

<p>
In the example above note the difference between the <code>ldr</code> and the subsequent <code>ldrb</code>/<code>ldrh</code>. The <code>ldr</code> instruction is needed to load an address into the register. Addresses in ARM are 32-bit integers so a regular <code>ldr</code> must be used here. Then, once we have the address in the register we use <code>ldrb</code> or <code>ldrh</code> to load the byte or the halfword. As stated above, the destination register is 32-bit so the loaded integer is zero-extended. The following table shows what happens with <em>zero-extension</em>.</p>
<table>
<caption>
Effect of subword loads with <code>ldrb</code> and <code>ldrh</code>.<br>
</caption>
<thead>
<tr>
<th>
</th>
<th colspan="2">
Content in memory (bytes)
</th>
<th>
Loaded in register (32-bit)
</th>
</tr>
<tr>
<th>
</th>
<th>
addr
</th>
<th>
addr+1
</th>
<th>
</th>
</tr>
</thead>
<tbody><tr>
<td>
<code>ldrb</code>
</td>
<td>
11001101
</td>
<td>
</td>
<td>
00000000 00000000 00000000 11001101
</td>
</tr>
<tr>
<td>
<code>ldrh<br>
</code></td>
<td>
11001101
</td>
<td>
10100101
</td>
<td>
00000000 00000000 10100101 11001101
</td>
</tr>
</tbody></table>
<p>
ARM in the Raspberry Pi is a <a href="http://en.wikipedia.org/wiki/Endianness">little endian architecture</a>, this means that bytes in memory are laid in memory (from lower to higher addresses) starting from the least significant byte to the most significant byte. Load and store instructions preserve this ordering. This fact is usually not important unless viewing the memory as a sequence of bytes. This the reason why in the table above 11001101 always appears in the first column even if the number 42445 is 10100101 11001101 in binary.
</p>
<p>
Ok, loading using <code>ldrb</code> and <code>ldrh</code> is fine as long as we only use natural numbers. Integral numbers include negative numbers and are commonly represented using <a href="http://en.wikipedia.org/wiki/Two%27s_complement">two’s complement</a>. If we zero-extend a negative number, the sign bit (the most significant bit of a two’s complement) will not be propagated and we will end with an unrelated positive number. When loading two’s complement subword integers we need to perform <em>sign-extension</em> using instructions <code>lsrb</code> and <code>lsrh</code>.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">    ldr r0<span style="color: #339933;">,</span> addr_of_one_byte     <span style="color: #339933;">/*</span> r0 ← &amp;one_byte <span style="color: #339933;">*/</span>
    ldrsb r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>               <span style="color: #339933;">/*</span> r0 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">{</span>signed <span style="color: #0000ff; font-weight: bold;">byte</span><span style="color: #009900; font-weight: bold;">}</span>r0 <span style="color: #339933;">*/</span>
&nbsp;
    ldr r1<span style="color: #339933;">,</span> addr_of_one_halfword <span style="color: #339933;">/*</span> r1 ← &amp;one_halfword <span style="color: #339933;">*/</span>
    ldrsh r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #009900; font-weight: bold;">]</span>               <span style="color: #339933;">/*</span> r1 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">{</span>signed half<span style="color: #009900; font-weight: bold;">}</span>r1 <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">    ldr r0, addr_of_one_byte     /* r0 ← &amp;one_byte */
    ldrsb r0, [r0]               /* r0 ← *{signed byte}r0 */

    ldr r1, addr_of_one_halfword /* r1 ← &amp;one_halfword */
    ldrsh r1, [r1]               /* r1 ← *{signed half}r1 */</p></div>

<p>
Note that sign-extension is the same as zero-extension when the sign bit is zero, as it happens in the two last rows of the following table that shows the effect of <code>ldrsb</code> and <code>ldrsh</code>.
</p>
<table>
<caption>
Effect of subword loads with <code>ldrsb</code> and <code>ldrsh</code>.<br>
</caption>
<thead>
<tr>
<th>
</th>
<th colspan="2">
Content in memory (bytes)
</th>
<th>
Loaded in register (32-bit)
</th>
</tr>
<tr>
<th>
</th>
<th>
addr
</th>
<th>
addr+1
</th>
<th>
</th>
</tr>
</thead>
<tbody><tr>
<td>
<code>ldrsb</code>
</td>
<td>
11001101
</td>
<td>
</td>
<td>
11111111 11111111 11111111 11001101
</td>
</tr>
<tr style="border-bottom: double #CACACA;">
<td>
<code>ldrsh<br>
</code></td>
<td>
11001101
</td>
<td>
10100101
</td>
<td>
11111111 11111111 10100101 11001101
</td>
</tr>
<tr>
<td>
<code>ldrsb</code>
</td>
<td>
01001101
</td>
<td>
</td>
<td>
00000000 00000000 00000000 01001101
</td>
</tr>
<tr>
<td>
<code>ldrsh<br>
</code></td>
<td>
11001101
</td>
<td>
00100101
</td>
<td>
00000000 00000000 00100101 11001101
</td>
</tr>
</tbody></table>
<p>
It is very important not to mix both instructions when loading subword data. When loading natural numbers, <code>lrb</code> and <code>lrh</code> are the correct choice. If the number is an integer that could be negative always use <code>ldrsb</code> and <code>ldrsh</code>. The following table summarizes what happens when you mix interpretations and the different load instructions.
</p>
<table style="font-size: 90%;">
<caption>
Patterns of bits interpreted as (natural) binary or two’s complement.<br>
</caption>
<thead>
<tr>
<th></th>
<th></th>
<th colspan="2">Interpretation of bits</th>
</tr>
<tr>
<th>Width</th>
<th>Bits</th>
<th>Binary</th>
<th>Two’s complement</th>
</tr>
</thead>
<tbody>
<tr>
<td>8-bit</td>
<td>11001101</td>
<td>205</td>
<td>-51</td>
</tr>
<tr>
<td>32-bit after <code>ldrb</code></td>
<td>00000000000000000000000011001101</td>
<td>205</td>
<td>205</td>
</tr>
<tr style="border-bottom: double #CACACA;">
<td>32-bit after <code>ldrsb</code></td>
<td>11111111111111111111111111001101</td>
<td>4294967245</td>
<td>-51</td>
</tr>
<tr>
<td>16-bit</td>
<td>1010010111001101</td>
<td>42445</td>
<td>-23091</td>
</tr>
<tr>
<td>32-bit after <code>ldrh</code></td>
<td>00000000000000001010010111001101</td>
<td>42445</td>
<td>42445</td>
</tr>
<tr>
<td>32-bit after <code>ldrsh</code></td>
<td>11111111111111111010010111001101</td>
<td>4294944205</td>
<td>-23091</td>
</tr>
</tbody>
</table>
<h3>Store</h3>
<p>
While load requires to take care whether the loaded subword is a binary or a two’s complement encoded number, a store instruction does not require any of this consideration. The reason is that the corresponding <code>strb</code> and <code>strh</code> instructions will simply take the least significant 8 or 16 bits of the register and store it in memory.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">    ldr r1<span style="color: #339933;">,</span> addr_of_one_byte     <span style="color: #339933;">/*</span> r0 ← &amp;one_byte <span style="color: #339933;">*/</span>
    ldrsb r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #009900; font-weight: bold;">]</span>               <span style="color: #339933;">/*</span> r0 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">{</span>signed <span style="color: #0000ff; font-weight: bold;">byte</span><span style="color: #009900; font-weight: bold;">}</span>r1 <span style="color: #339933;">*/</span>
    strb r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #009900; font-weight: bold;">]</span>                <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">{</span><span style="color: #0000ff; font-weight: bold;">byte</span><span style="color: #009900; font-weight: bold;">}</span>r1 ← r0 <span style="color: #339933;">*/</span>
&nbsp;
    ldr r0<span style="color: #339933;">,</span> addr_of_one_halfword <span style="color: #339933;">/*</span> r0 ← &amp;one_halfword <span style="color: #339933;">*/</span>
    ldrsh r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>               <span style="color: #339933;">/*</span> r1 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">{</span>signed half<span style="color: #009900; font-weight: bold;">}</span>r0 <span style="color: #339933;">*/</span>
    strh r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>                <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">{</span>half<span style="color: #009900; font-weight: bold;">}</span>r0 ← r1 <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">    ldr r1, addr_of_one_byte     /* r0 ← &amp;one_byte */
    ldrsb r0, [r1]               /* r0 ← *{signed byte}r1 */
    strb r0, [r1]                /* *{byte}r1 ← r0 */

    ldr r0, addr_of_one_halfword /* r0 ← &amp;one_halfword */
    ldrsh r1, [r0]               /* r1 ← *{signed half}r0 */
    strh r1, [r0]                /* *{half}r0 ← r1 */</p></div>

<h2>Alignment restrictions</h2>
<p>
When loading or storing 32-bit integer from memory, the address must be 4 byte aligned, this means that the two least significant bits of the address must be 0. Such restriction is relaxed if the memory operation (load or store) is a subword one. For halfwords the address must be 2 byte aligned. For bytes, no restriction applies. This way we can <em>reinterpret</em> words and halfwords as either halfwords and bytes if we want.
</p>
<p>
Consider the following example, where we traverse a single word reinterpreting its bytes and halfwords (and finally the word itself).
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
32
33
34
35
36
37
38
39
40
41
42
43
44
45
46
47
48
49
50
51
52
53
54
55
56
57
58
59
60
61
62
63
64
65
66
67
68
69
70
71
72
73
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
a_word<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">0x11223344</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
message_bytes <span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"byte #%d is 0x%x\n"</span>
message_halfwords <span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"halfword #%d is 0x%x\n"</span>
message_words <span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"word #%d is 0x%x\n"</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
<span style="color: #339933;">.</span>globl main
main<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>  <span style="color: #339933;">/*</span> keep callee saved registers <span style="color: #339933;">*/</span>
&nbsp;
    ldr r4<span style="color: #339933;">,</span> addr_a_word    <span style="color: #339933;">/*</span> r4 ← &amp;a_word <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>             <span style="color: #339933;">/*</span> r5 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    b check_loop_bytes     <span style="color: #339933;">/*</span> branch to check_loop_bytes <span style="color: #339933;">*/</span>
&nbsp;
    loop_bytes<span style="color: #339933;">:</span>
        <span style="color: #339933;">/*</span> prepare <span style="color: #00007f; font-weight: bold;">call</span> to printf <span style="color: #339933;">*/</span>
        ldr r0<span style="color: #339933;">,</span> addr_message_bytes
                           <span style="color: #339933;">/*</span> r0 ← &amp;message_bytes
                              first parameter of printf <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> r5         <span style="color: #339933;">/*</span> r1 ← r5
                              second parameter of printf <span style="color: #339933;">*/</span>
        ldrb r2<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #339933;">,</span> r5<span style="color: #009900; font-weight: bold;">]</span>  <span style="color: #339933;">/*</span> r2 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">{</span><span style="color: #0000ff; font-weight: bold;">byte</span><span style="color: #009900; font-weight: bold;">}</span><span style="color: #009900; font-weight: bold;">(</span>r4 <span style="color: #339933;">+</span> r5<span style="color: #009900; font-weight: bold;">)</span>
                              third parameter of printf <span style="color: #339933;">*/</span>
        <span style="color: #46aa03; font-weight: bold;">bl</span> printf          <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> printf <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">add</span> r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>     <span style="color: #339933;">/*</span> r5 ← r5 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    check_loop_bytes<span style="color: #339933;">:</span>
        <span style="color: #00007f; font-weight: bold;">cmp</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>         <span style="color: #339933;">/*</span> compute r5 <span style="color: #339933;">-</span> <span style="color: #ff0000;">4</span> <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
        bne loop_bytes     <span style="color: #339933;">/*</span> if r5 != <span style="color: #ff0000;">4</span> branch to loop_bytes <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>             <span style="color: #339933;">/*</span> r5 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    b check_loop_halfwords <span style="color: #339933;">/*</span> branch to check_loop_halfwords <span style="color: #339933;">*/</span>
&nbsp;
    loop_halfwords<span style="color: #339933;">:</span>
        <span style="color: #339933;">/*</span> prepare <span style="color: #00007f; font-weight: bold;">call</span> to printf <span style="color: #339933;">*/</span>
        ldr r0<span style="color: #339933;">,</span> addr_message_halfwords
                           <span style="color: #339933;">/*</span> r0 ← &amp;message_halfwords
                              first parameter of printf <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> r5         <span style="color: #339933;">/*</span> r1 ← r5
                              second parameter of printf <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">mov</span> r6<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">1</span> <span style="color: #339933;">/*</span> r6 ← r5 <span style="color: #339933;">*</span> <span style="color: #ff0000;">2</span> <span style="color: #339933;">*/</span>
        ldrh r2<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #339933;">,</span> r6<span style="color: #009900; font-weight: bold;">]</span>  <span style="color: #339933;">/*</span> r2 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">{</span>half<span style="color: #009900; font-weight: bold;">}</span><span style="color: #009900; font-weight: bold;">(</span>r4 <span style="color: #339933;">+</span> r6<span style="color: #009900; font-weight: bold;">)</span>
                              this is r2 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">{</span>half<span style="color: #009900; font-weight: bold;">}</span><span style="color: #009900; font-weight: bold;">(</span>r4 <span style="color: #339933;">+</span> r5 <span style="color: #339933;">*</span> <span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">)</span>
                              third parameter of printf <span style="color: #339933;">*/</span>
        <span style="color: #46aa03; font-weight: bold;">bl</span> printf          <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> printf <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">add</span> r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>     <span style="color: #339933;">/*</span> r5 ← r5 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    check_loop_halfwords<span style="color: #339933;">:</span>
        <span style="color: #00007f; font-weight: bold;">cmp</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span>         <span style="color: #339933;">/*</span> compute r5 <span style="color: #339933;">-</span> <span style="color: #ff0000;">2</span> <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
        bne loop_halfwords <span style="color: #339933;">/*</span> if r5 != <span style="color: #ff0000;">2</span> branch to loop_halfwords <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> prepare <span style="color: #00007f; font-weight: bold;">call</span> to printf <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> addr_message_words <span style="color: #339933;">/*</span> r0 ← &amp;message_words
                                  first parameter of printf <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>                 <span style="color: #339933;">/*</span> r1 ← <span style="color: #ff0000;">0</span>
                                  second parameter of printf <span style="color: #339933;">*/</span>
    ldr r2<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #009900; font-weight: bold;">]</span>               <span style="color: #339933;">/*</span> r1 ← <span style="color: #339933;">*</span>r4
                                  third parameter of printf <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> printf                  <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> printf <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>   <span style="color: #339933;">/*</span> restore callee saved registers <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>             <span style="color: #339933;">/*</span> set error <span style="color: #0000ff; font-weight: bold;">code</span> <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr                  <span style="color: #339933;">/*</span> return to system <span style="color: #339933;">*/</span>
&nbsp;
addr_a_word <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> a_word
addr_message_bytes <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> message_bytes
addr_message_halfwords <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> message_halfwords
addr_message_words <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> message_words</pre></td></tr></tbody></table><p class="theCode" style="display:none;">.data

.align 4
a_word: .word 0x11223344

.align 4
message_bytes : .asciz "byte #%d is 0x%x\n"
message_halfwords : .asciz "halfword #%d is 0x%x\n"
message_words : .asciz "word #%d is 0x%x\n"

.text

.globl main
main:
    push {r4, r5, r6, lr}  /* keep callee saved registers */

    ldr r4, addr_a_word    /* r4 ← &amp;a_word */

    mov r5, #0             /* r5 ← 0 */
    b check_loop_bytes     /* branch to check_loop_bytes */

    loop_bytes:
        /* prepare call to printf */
        ldr r0, addr_message_bytes
                           /* r0 ← &amp;message_bytes
                              first parameter of printf */
        mov r1, r5         /* r1 ← r5
                              second parameter of printf */
        ldrb r2, [r4, r5]  /* r2 ← *{byte}(r4 + r5)
                              third parameter of printf */
        bl printf          /* call printf */
        add r5, r5, #1     /* r5 ← r5 + 1 */
    check_loop_bytes:
        cmp r5, #4         /* compute r5 - 4 and update cpsr */
        bne loop_bytes     /* if r5 != 4 branch to loop_bytes */

    mov r5, #0             /* r5 ← 0 */
    b check_loop_halfwords /* branch to check_loop_halfwords */

    loop_halfwords:
        /* prepare call to printf */
        ldr r0, addr_message_halfwords
                           /* r0 ← &amp;message_halfwords
                              first parameter of printf */
        mov r1, r5         /* r1 ← r5
                              second parameter of printf */
        mov r6, r5, LSL #1 /* r6 ← r5 * 2 */
        ldrh r2, [r4, r6]  /* r2 ← *{half}(r4 + r6)
                              this is r2 ← *{half}(r4 + r5 * 2)
                              third parameter of printf */
        bl printf          /* call printf */
        add r5, r5, #1     /* r5 ← r5 + 1 */
    check_loop_halfwords:
        cmp r5, #2         /* compute r5 - 2 and update cpsr */
        bne loop_halfwords /* if r5 != 2 branch to loop_halfwords */

    /* prepare call to printf */
    ldr r0, addr_message_words /* r0 ← &amp;message_words
                                  first parameter of printf */
    mov r1, #0                 /* r1 ← 0
                                  second parameter of printf */
    ldr r2, [r4]               /* r1 ← *r4
                                  third parameter of printf */
    bl printf                  /* call printf */

    pop {r4, r5, r6, lr}   /* restore callee saved registers */
    mov r0, #0             /* set error code */
    bx lr                  /* return to system */

addr_a_word : .word a_word
addr_message_bytes : .word message_bytes
addr_message_halfwords : .word message_halfwords
addr_message_words : .word message_words</p></div>

<p>
Our word is the number 11223344<sub>16</sub> (this is 287454020<sub>10</sub>). We load the address of the word, line 17, as usual with a <code>ldr</code> and then we perform different sized loads. The first loop, lines 19 to 35, loads each byte and prints it. Note that the <code>ldrb</code>, line 29, just adds the current byte (in <code>r5</code>) to the address of the word (in <code>r4</code>). We do not have to multiply <code>r5</code> by anything. In fact <code>ldrb</code> and <code>ldrh</code>, unlike <code>ldr</code>, do not allow a shift operand of the form <code>LSL #x</code>. You can see how to dodge this restriction in the loop that prints halfwords, lines 37 to 55. The instruction <code>ldrh</code>, line 48, we use <code>r6</code> that is just <code>r4 + r5*2</code>, computed in line 47. Since the original word was 4 byte aligned, we can read its two halfwords because they will be 2-byte aligned. It would be an error to attempt to load a halfword using the address of the byte 1, only the halfwords starting at bytes 0 and 2 can be loaded as a halfword.
</p>
<p>This is the output of the program</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="shell" style="font-family:monospace;">$ ./reinterpret 
byte #0 is 0x44
byte #1 is 0x33
byte #2 is 0x22
byte #3 is 0x11
halfword #0 is 0x3344
halfword #1 is 0x1122
word #0 is 0x11223344</pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ ./reinterpret 
byte #0 is 0x44
byte #1 is 0x33
byte #2 is 0x22
byte #3 is 0x11
halfword #0 is 0x3344
halfword #1 is 0x1122
word #0 is 0x11223344</p></div>

<p>
As we stated above, ARM in the Raspberry Pi is a little endian architecture, so for integers of more than one byte, they are laid out (from lower addresses to higher addresses) starting from the less significant bytes, this is why the first byte is 44<sub>16</sub> and not 11<sub>16</sub>. Similarly for halfwords, the first halfword will be 3344<sub>16</sub> instead of 1122<sub>16</sub>.
</p>
<p>
Thats all for today</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2014/08/23/arm-assembler-raspberry-pi-chapter-21/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2014/08/23/arm-assembler-raspberry-pi-chapter-21/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2014/08/23/arm-assembler-raspberry-pi-chapter-21/&amp;text=ARM+assembler+in+Raspberry+Pi+%E2%80%93+Chapter+21+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2014/08/23/arm-assembler-raspberry-pi-chapter-21/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>						<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2014/08/20/arm-assembler-raspberry-pi-chapter-20/" rel="prev">ARM assembler in Raspberry Pi – Chapter 20</a></span>
			<span class="next"><a href="http://thinkingeek.com/2014/12/20/arm-assembler-raspberry-pi-chapter-22/" rel="next">ARM assembler in Raspberry Pi – Chapter 22</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
			<h3>6 thoughts on “<span>ARM assembler in Raspberry Pi – Chapter 21</span>”</h3>
		<ul class="commentlist">
				<li class="comment even thread-even depth-1 parent" id="comment-531537">
				<div id="div-comment-531537" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/52f6d538cf402fc8cab00eccd21a911b?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/52f6d538cf402fc8cab00eccd21a911b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/52f6d538cf402fc8cab00eccd21a911b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-52f6d538cf402fc8cab00eccd21a911b-0">			<cite class="fn">Nilesh</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/08/23/arm-assembler-raspberry-pi-chapter-21/#comment-531537">
			October 29, 2014 at 11:49 am</a>		</div>

		<p>Thanks a lot for fantastic tutorial. It helped me to learn ARM assembly quickly.<br>
I want to know one more thing. Is it possible to learn thumb set using such simple tutorial. Can you give a simple example?</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/08/23/arm-assembler-raspberry-pi-chapter-21/?replytocom=531537#respond" onclick="return addComment.moveForm( &quot;div-comment-531537&quot;, &quot;531537&quot;, &quot;respond&quot;, &quot;1609&quot; )" aria-label="Reply to Nilesh">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-533949">
				<div id="div-comment-533949" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-0">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/08/23/arm-assembler-raspberry-pi-chapter-21/#comment-533949">
			October 29, 2014 at 10:10 pm</a>		</div>

		<p>Hi Nilesh,</p>
<p>thanks for your comments.</p>
<p>Although it is not the goal of this tutorial maybe I will devote some article to Thumb.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/08/23/arm-assembler-raspberry-pi-chapter-21/?replytocom=533949#respond" onclick="return addComment.moveForm( &quot;div-comment-533949&quot;, &quot;533949&quot;, &quot;respond&quot;, &quot;1609&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-582936">
				<div id="div-comment-582936" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/7a33dfef5460bfa20067b7622dfd9371?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/7a33dfef5460bfa20067b7622dfd9371?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/7a33dfef5460bfa20067b7622dfd9371?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-7a33dfef5460bfa20067b7622dfd9371-0">			<cite class="fn">Yen</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/08/23/arm-assembler-raspberry-pi-chapter-21/#comment-582936">
			November 6, 2014 at 11:47 pm</a>		</div>

		<p>This is an awesome series of tutorials, Thankyou so much for spending the time going through them rferrer!</p>
<p>For anyone that wants extra information i highly recommend the free course texas university has put up online <a href="http://users.ece.utexas.edu/~valvano/Volume1/" rel="nofollow">http://users.ece.utexas.edu/~valvano/Volume1/</a></p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/08/23/arm-assembler-raspberry-pi-chapter-21/?replytocom=582936#respond" onclick="return addComment.moveForm( &quot;div-comment-582936&quot;, &quot;582936&quot;, &quot;respond&quot;, &quot;1609&quot; )" aria-label="Reply to Yen">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-582953">
				<div id="div-comment-582953" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-1">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/08/23/arm-assembler-raspberry-pi-chapter-21/#comment-582953">
			November 6, 2014 at 11:52 pm</a>		</div>

		<p>Hi Yen, thanks for the online resource!</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/08/23/arm-assembler-raspberry-pi-chapter-21/?replytocom=582953#respond" onclick="return addComment.moveForm( &quot;div-comment-582953&quot;, &quot;582953&quot;, &quot;respond&quot;, &quot;1609&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1 parent" id="comment-721923">
				<div id="div-comment-721923" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/71fdea1ec822d5321bd95f5aca8284c2?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/71fdea1ec822d5321bd95f5aca8284c2?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/71fdea1ec822d5321bd95f5aca8284c2?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-71fdea1ec822d5321bd95f5aca8284c2-0">			<cite class="fn">Cor Massar</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/08/23/arm-assembler-raspberry-pi-chapter-21/#comment-721923">
			December 23, 2014 at 10:42 pm</a>		</div>

		<p>Love the dedication you’ve shown. My RasPi will march with your instructions. All kidding aside, thanks for a very clear explanation of how how assembler influences the ARM processor. Will be using it after New Year. Keep up the quality. Wish all readers fine Holidays !?</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/08/23/arm-assembler-raspberry-pi-chapter-21/?replytocom=721923#respond" onclick="return addComment.moveForm( &quot;div-comment-721923&quot;, &quot;721923&quot;, &quot;respond&quot;, &quot;1609&quot; )" aria-label="Reply to Cor Massar">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-734811">
				<div id="div-comment-734811" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-2">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/08/23/arm-assembler-raspberry-pi-chapter-21/#comment-734811">
			December 30, 2014 at 3:52 pm</a>		</div>

		<p>Thanks Cor! Happy holidays.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/08/23/arm-assembler-raspberry-pi-chapter-21/?replytocom=734811#respond" onclick="return addComment.moveForm( &quot;div-comment-734811&quot;, &quot;734811&quot;, &quot;respond&quot;, &quot;1609&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		</ul>
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2014/08/23/arm-assembler-raspberry-pi-chapter-21/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="1609" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="737395950e"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496666724405"></form>
			</div><!-- #respond -->
	</div>	</div>