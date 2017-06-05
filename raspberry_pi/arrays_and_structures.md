# Arrays and structures


<div class="main">
			<h1>ARM assembler in Raspberry Pi – Chapter 8 </h1>
		<p class="post-meta"><span class="date">January 27, 2013</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/#comments">17</a></span> </p>
		<p>
In the previous chapter we saw that the second operand of most arithmetic instructions can use a <em>shift operator</em> which allows us to shift and rotate bits. In this chapter we will continue learning the available <em>indexing modes</em> of ARM instructions. This time we will focus on load and store instructions.
</p>
<p><span id="more-546"></span></p>
<h2>Arrays and structures</h2>
<p>
So far we have been able to move 32 bits from memory to registers (load) and back to memory (store). But working on single items of 32 bits (usually called scalars) is a bit limiting. Soon we would find ourselves working on arrays and structures, even if we did not know.
</p>
<p>
An array is a sequence of items of the same kind in memory. Arrays are a foundational data structure in almost every low level language. Every array has a base address, usually denoted by the name of the array, and contains N items. Each of these items has associated a growing index, ranging from 0 to N-1 or 1 to N. Using the base address and the index we can access an item of the array. We mentioned in chapter 3 that memory could be viewed as an array of bytes. An array in memory is the same, but an item may take more than one single byte.
</p>
<p>
A structure (or record or tuple) is a sequence of items of possibly diferent kind. Each item of a structure is usually called a field. Fields do not have an associated index but an offset respect to the beginning of the structure. Structures are laid out in memory to ensure that the proper alignment is used in every field. The base address of a structure is the address of its first field. If the base address is aligned, the structure should be laid out in a way that all the field are properly aligned as well.
</p>
<p>
What do arrays and structure have to do with <em>indexing modes</em> of load and store? Well, these indexing modes are designed to make easier accessing arrays and structs.</p>
<p></p><h2>Defining arrays and structs</h2>
<p>
To illustrate how to work with arrays and references we will use the following C declarations and implement them in assembler.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #993333;">int</span> a<span style="color: #009900;">[</span><span style="color: #0000dd;">100</span><span style="color: #009900;">]</span><span style="color: #339933;">;</span>
<span style="color: #993333;">struct</span> my_struct
<span style="color: #009900;">{</span>
  <span style="color: #993333;">char</span> f0<span style="color: #339933;">;</span>
  <span style="color: #993333;">int</span> f1<span style="color: #339933;">;</span>
<span style="color: #009900;">}</span> b<span style="color: #339933;">;</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">int a[100];
struct my_struct
{
  char f0;
  int f1;
} b;</p></div>

<p>
Let’s first define in our assembler the array ‘a’. It is just 100 integers. An integer in ARM is 32-bit wide so in our assembler code we have to make room for 400 bytes (4 * 100).
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>1
2
3
4
5
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> <span style="color: #339933;">--</span> array01<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
<span style="color: #339933;">.</span>balign <span style="color: #ff0000;">4</span>
a<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>skip <span style="color: #ff0000;">400</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* -- array01.s */
.data

.balign 4
a: .skip 400</p></div>

<p>
In line 5 we define the symbol <code>a</code> and then we make room for 400 bytes. The directive .skip tells the assembler to advance a given number of bytes before emitting the next datum. Here we are skipping 400 bytes because our array of integers takes 400 bytes (4 bytes per each of the 100 integers). Declaring a structure is not much different.</p>
<p>
</p><div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>7
8
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">.</span>balign <span style="color: #ff0000;">4</span>
b<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>skip <span style="color: #ff0000;">8</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">.balign 4
b: .skip 8</p></div>

<p>
Right now you should wonder why we skipped 8 bytes when the structure itself takes just 5 bytes. Well, it does need 5 bytes to store useful information. The first field <code>f0</code> is a <code>char</code>. A <code>char</code> takes 1 byte of storage. The next field <code>f1</code> is an integer. An integer takes 4 bytes and it must be aligned at 4 bytes as well, so we have to leave 3 unused bytes between the field <code>f0</code> and the field <code>f1</code>. This unused storage put just to fulfill alignment is called <em>padding</em>. Padding should never be used by your program.
</p>
<h2>Naive approach without indexing modes</h2>
<p>
Ok, let’s write some code to initialize every item of the array <code>a[i]</code>. We will do something equivalent to the following C code.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #b1b100;">for</span> <span style="color: #009900;">(</span>i <span style="color: #339933;">=</span> <span style="color: #0000dd;">0</span><span style="color: #339933;">;</span> i <span style="color: #339933;">&lt;</span> <span style="color: #0000dd;">100</span><span style="color: #339933;">;</span> i<span style="color: #339933;">++</span><span style="color: #009900;">)</span>
  a<span style="color: #009900;">[</span>i<span style="color: #009900;">]</span> <span style="color: #339933;">=</span> i<span style="color: #339933;">;</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">for (i = 0; i &lt; 100; i++)
  a[i] = i;</p></div>


<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>10
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">global</span> main
main<span style="color: #339933;">:</span>
    ldr r1<span style="color: #339933;">,</span> addr_of_a       <span style="color: #339933;">/*</span> r1 ← &amp;a <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>              <span style="color: #339933;">/*</span> r2 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">loop</span><span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">cmp</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">100</span>            <span style="color: #339933;">/*</span> Have we reached <span style="color: #ff0000;">100</span> yet? <span style="color: #339933;">*/</span>
    beq end                 <span style="color: #339933;">/*</span> If so<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">leave</span> the <span style="color: #00007f; font-weight: bold;">loop</span><span style="color: #339933;">,</span> otherwise continue <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r3<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span>  <span style="color: #339933;">/*</span> r3 ← r1 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span>r2<span style="color: #339933;">*</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> r2<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r3<span style="color: #009900; font-weight: bold;">]</span>            <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span>r3 ← r2 <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r2<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>          <span style="color: #339933;">/*</span> r2 ← r2 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    b <span style="color: #00007f; font-weight: bold;">loop</span>                  <span style="color: #339933;">/*</span> Go to the beginning of the <span style="color: #00007f; font-weight: bold;">loop</span> <span style="color: #339933;">*/</span>
end<span style="color: #339933;">:</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr
addr_of_a<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> a</pre></td></tr></tbody></table><p class="theCode" style="display:none;">.text

.global main
main:
    ldr r1, addr_of_a       /* r1 ← &amp;a */
    mov r2, #0              /* r2 ← 0 */
loop:
    cmp r2, #100            /* Have we reached 100 yet? */
    beq end                 /* If so, leave the loop, otherwise continue */
    add r3, r1, r2, LSL #2  /* r3 ← r1 + (r2*4) */
    str r2, [r3]            /* *r3 ← r2 */
    add r2, r2, #1          /* r2 ← r2 + 1 */
    b loop                  /* Go to the beginning of the loop */
end:
    bx lr
addr_of_a: .word a</p></div>

<p>
Whew! We are using lots of things we have learnt from earlier chapters. In line 14 we load the base address of the array into <code>r1</code>. The address of the array will not change so we load it once. In register <code>r2</code> we will keep the index that will range from 0 to 99. In line 17 we compare it to 100 to see if we have reached the end of the loop.
</p>
<p>
Line 19 is an important one. Here we compute the address of the item. We have in <code>r1</code> the base address and we know each item is 4 bytes wide. We know also that <code>r2</code> keeps the index of the loop which we will use to access the array element. Given an item with index <code>i</code> its address must be <code>&amp;a + 4*i</code>, since there are 4 bytes between every element of this array. So <code>r3</code> has the address of the current element in this step of the loop. In line 20 we store <code>r2</code>, this is <code>i</code>, into the memory pointed by <code>r3</code>, the <code>i</code>-th array item, this is <code>a[i]</code>.
</p>
<p>
Then we proceed to  increase <code>r2</code> and jump back for the next step of the loop.
</p>
<p>
As you can see, accessing an array involves calculating the address of the accessed item. Does the ARM instruction set provide a more compact way to do this? The answer is yes. In fact it provides several <em>indexing modes</em>.
</p>
<h2>Indexing modes</h2>
<p>
In the previous chapter the concept <em>indexing mode</em> was a bit off because we were not indexing anything. Now it makes much more sense since we are indexing an array item. ARM provides <strong>nine</strong> of these indexing modes. I will distinguish two kinds of indexing modes: non updating and updating depending on whether they feature a side-effect that we will discuss later, when dealing with updating indexing modes.
</p>
<h3>Non updating indexing modes</h3>
<ol>
<li value="1"> <code>[Rsource1, +#immediate]</code> or <code>[Rsource1, -#immediate]</code>
<p>
It justs adds (or subtracts) the immediate value to form the address. This is very useful to array items the index of which is a constant in the code or fields of a structure, since their offset is always constant. In <code>Rsource1</code> we put the base address and in <code>immediate</code> the offset we want in bytes. The immediate cannot be larger than 12 bits (0..4096). When the immediate is <code>#0</code> it is like the usual we have been using <code>[Rsource1]</code>.
</p>
<p>
For example, we can set <code>a[3]</code> to 3 this way (we assume that r1 already contans the base address of a). Note that the offset is in bytes thus we need an offset of 12 (4 bytes * 3 items skipped).
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">mov</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">3</span>          <span style="color: #339933;">/*</span> r2 ← <span style="color: #ff0000;">3</span> <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">str</span> r2<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #339933;">,</span> #<span style="color: #339933;">+</span><span style="color: #ff0000;">12</span><span style="color: #009900; font-weight: bold;">]</span>  <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r1 <span style="color: #339933;">+</span> <span style="color: #ff0000;">12</span><span style="color: #009900; font-weight: bold;">)</span> ← r2 <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">mov r2, #3          /* r2 ← 3 */
str r2, [r1, #+12]  /* *(r1 + 12) ← r2 */</p></div>

</li><li><code>[Rsource1, +Rsource2]</code> or <code>[Rsource1, -Rsource2]</code>
<p>
This is like the previous one, but the added (or subtracted) offset is the value in a register. This is useful when the offset is too big for the immediate. Note that for the <code>+Rsource2</code> case, the two registers can be swapped (as this would not affect the address computed).
</p>
<p>
Example. The same as above but using a register this time.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">mov</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">3</span>         <span style="color: #339933;">/*</span> r2 ← <span style="color: #ff0000;">3</span> <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">mov</span> r3<span style="color: #339933;">,</span> #<span style="color: #ff0000;">12</span>        <span style="color: #339933;">/*</span> r3 ← <span style="color: #ff0000;">12</span> <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">str</span> r2<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #339933;">,+</span>r3<span style="color: #009900; font-weight: bold;">]</span>   <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r1 <span style="color: #339933;">+</span> r3<span style="color: #009900; font-weight: bold;">)</span> ← r2 <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">mov r2, #3         /* r2 ← 3 */
mov r3, #12        /* r3 ← 12 */
str r2, [r1,+r3]   /* *(r1 + r3) ← r2 */</p></div>

</li><li><code>[Rsource1, +Rsource2, shift_operation #immediate]</code> or <code>[Rsource1, -Rsource2, shift_operation #immediate]</code>.
<p>
This one is similar to the usual shift operation we can do with other instructions. A shift operation (remember: <code>LSL</code>, <code>LSR</code>, <code>ASR</code> or <code>ROR</code>) is applied to <code>Rsource2</code>, <code>Rsource1</code> is then added (or subtracted) to the result of the shift operation applied to <code>Rsource2</code>. This is useful when we need to multiply the address by some fixed amount. When accessing the items of the integer array <code>a</code> we had to multiply the result by 4 to get a meaningful address.
</p>
<p>
For this example, let’s first recall how we computed above the address in the array of the item in position <code>r2</code>.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>19
20
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">add</span> r3<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span>  <span style="color: #339933;">/*</span> r3 ← r1 <span style="color: #339933;">+</span> r2<span style="color: #339933;">*</span><span style="color: #ff0000;">4</span> <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">str</span> r2<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r3<span style="color: #009900; font-weight: bold;">]</span>            <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span>r3 ← r2 <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">add r3, r1, r2, LSL #2  /* r3 ← r1 + r2*4 */
str r2, [r3]            /* *r3 ← r2 */</p></div>

<p>
We can express this in a much more compact way (without the need of the register <code>r3</code>).
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">str</span> r2<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #339933;">,</span> <span style="color: #339933;">+</span>r2<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">]</span>  <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r1 <span style="color: #339933;">+</span> r2<span style="color: #339933;">*</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span> ← r2 <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">str r2, [r1, +r2, LSL #2]  /* *(r1 + r2*4) ← r2 */</p></div>

</li></ol>
<h3>Updating indexing modes</h3>
<p>
In these indexing modes the <code>Rsource1</code> register is updated with the address synthesized by the load or store instruction. You may be wondering why one would want to do this. A bit of detour first. Recheck the code of the array load. Why do we have to keep around the base address of the array if we are always effectively moving 4 bytes away from it? Would not it make much more sense to keep the address of the current entity? So instead of
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>19
20
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">add</span> r3<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span>  <span style="color: #339933;">/*</span> r3 ← r1 <span style="color: #339933;">+</span> r2<span style="color: #339933;">*</span><span style="color: #ff0000;">4</span> <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">str</span> r2<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r3<span style="color: #009900; font-weight: bold;">]</span>            <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span>r3 ← r2 <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">add r3, r1, r2, LSL #2  /* r3 ← r1 + r2*4 */
str r2, [r3]            /* *r3 ← r2 */</p></div>

<p>
we might want to do something like
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">str</span> r2<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #009900; font-weight: bold;">]</span>        <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span>r1 ← r2 <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">add</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>      <span style="color: #339933;">/*</span> r1 ← r1 <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span> <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">str r2, [r1]        /* *r1 ← r2 */
add r1, r1, #4      /* r1 ← r1 + 4 */</p></div>

<p>
because there is no need to compute everytime from the beginning the address of the next item (as we are accessing them sequentially). Even if this looks slightly better, it still can be improved a bit more. What if our instruction were able to update <code>r1</code> for us? Something like this (obviously the exact syntax is not as shown)
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> Wrong syntax <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">str</span> r2<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #7f007f;">"and then"</span> <span style="color: #00007f; font-weight: bold;">add</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* Wrong syntax */
str r2, [r1] "and then" add r1, r1, #4</p></div>

<p>
Such indexing modes exist. There are two kinds of updating indexing modes depending on at which time <code>Rsource1</code> is updated. If <code>Rsource1</code> is updated after the load or store itself (meaning that the address to load or store is the initial <code>Rsource1</code> value) this is a <em>post-indexing</em> accessing mode. If <code>Rsource1</code> is updated before the actual load or store (meaning that the address to load or store is the final value of <code>Rsource1</code>) this is a <em>pre-indexing</em> accessing mode. In all cases, at the end of the instruction <code>Rsource1</code> will have the value of the computation of the indexing mode. Now this sounds a bit convoluted, just look in the example above: we first load using <code>r1</code> and then we do <code>r1 ← r1 + 4</code>. This is post-indexing: we first use the value of <code>r1</code> as the address where we store the value of <code>r2</code>. Then <code>r1</code> is updated with <code>r1 + 4</code>. Now consider another hypothetic syntax.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> Wrong syntax <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">str</span> r2<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #00007f; font-weight: bold;">add</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* Wrong syntax */
str r2, [add r1, r1, #4]</p></div>

<p>
This is pre-indexing: we first compute <code>r1 + 4</code> and use it as the address where we store the value of <code>r2</code>. At the end of the instruction <code>r1</code> has effectively been updated too, but the updated value has already been used as the address of the load or store.
</p>
<h4>Post-indexing modes</h4>
<ol>
<li value="4"><code>[Rsource1], #+immediate</code> or <code>[Rsource1], #-immediate</code>
<p>
The value of <code>Rsource1</code> is used as the address for the load or store. Then <code>Rsource1</code> is updated with the value of <code>immediate</code> after adding (or subtracting) it to <code>Rsource1</code>. Using this indexing mode we can rewrite the loop of our first example as follows:
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>16
17
18
19
20
21
22
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">loop</span><span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">cmp</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">100</span>            <span style="color: #339933;">/*</span> Have we reached <span style="color: #ff0000;">100</span> yet? <span style="color: #339933;">*/</span>
    beq end                 <span style="color: #339933;">/*</span> If so<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">leave</span> the <span style="color: #00007f; font-weight: bold;">loop</span><span style="color: #339933;">,</span> otherwise continue <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> r2<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>        <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span>r1 ← r2 then r1 ← r1 <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r2<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>          <span style="color: #339933;">/*</span> r2 ← r2 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    b <span style="color: #00007f; font-weight: bold;">loop</span>                  <span style="color: #339933;">/*</span> Go to the beginning of the <span style="color: #00007f; font-weight: bold;">loop</span> <span style="color: #339933;">*/</span>
end<span style="color: #339933;">:</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">loop:
    cmp r2, #100            /* Have we reached 100 yet? */
    beq end                 /* If so, leave the loop, otherwise continue */
    str r2, [r1], #4        /* *r1 ← r2 then r1 ← r1 + 4 */
    add r2, r2, #1          /* r2 ← r2 + 1 */
    b loop                  /* Go to the beginning of the loop */
end:</p></div>

</li><li><code>[Rsource1], +Rsource2</code> or <code>[Rsource1], -Rsource2</code>
<p>
Like the previous one but instead of an immediate, the value of <code>Rsource2</code> is used. As usual this can be used as a workaround when the offset is too big for the immediate value.
</p>
</li><li><code>[Rsource1], +Rsource2, shift_operation #immediate</code> or <code>[Rsource1], -Rsource2, shift_operation #immediate</code>
<p>
The value of <code>Rsource1</code> is used as the address for the load or store. Then <code>Rsource2</code> is applied a shift operation (<code>LSL</code>, <code>LSR</code>, <code>ASR</code> or <code>ROL</code>). The resulting value of that shift is added (or subtracted) to <code>Rsource1</code>. <code>Rsource1</code> is finally updated with this last value.
</p></li></ol>
<h4>Pre-indexing modes</h4>
<p>
Pre-indexing modes may look a bit weird at first but they are useful when the computed address is going to be reused soon. Instead of recomputing it we can reuse the updated <code>Rsource1</code>.<br>
Mind the <code>!</code> symbol in these indexing modes which distinguishes them from the non updating indexing modes. </p>
<ol>
<li value="7"><code>[Rsource1, #+immediate]!</code> or <code>[Rsource1, #-immediate]!</code>
<p>
It behaves like the similar non-updating indexing mode but <code>Rsource1</code> gets updated with the computed address. Imagine we want to compute <code>a[3] = a[3] + a[3]</code>. We could do this (we assume that <code>r1</code> already has the base address of the array).</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">ldr r2<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #339933;">,</span> #<span style="color: #339933;">+</span><span style="color: #ff0000;">12</span><span style="color: #009900; font-weight: bold;">]</span>!  <span style="color: #339933;">/*</span> r1 ← r1 <span style="color: #339933;">+</span> <span style="color: #ff0000;">12</span> then r2 ← <span style="color: #339933;">*</span>r1 <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">add</span> r2<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r2       <span style="color: #339933;">/*</span> r2 ← r2 <span style="color: #339933;">+</span> r2 <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">str</span> r2<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #009900; font-weight: bold;">]</span>         <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span>r1 ← r2 <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">ldr r2, [r1, #+12]!  /* r1 ← r1 + 12 then r2 ← *r1 */
add r2, r2, r2       /* r2 ← r2 + r2 */
str r2, [r1]         /* *r1 ← r2 */</p></div>

</li><li><code>[Rsource1, +Rsource2]!</code> or <code>[Rsource1, +Rsource2]!</code>
<p>
Similar to the previous one but using a register <code>Rsource2</code> instead of an immediate.</p>
</li><li><code>[Rsource1, +Rsource2, shift_operation #immediate]!</code> or <code>[Rsource1, -Rsource2, shift_operation #immediate]!</code>
<p>Like to the non-indexing equivalent but Rsource1 will be updated with the address used for the load or store instruction.</p>
</li></ol>
<h2>Back to structures</h2>
<p>
All the examples in this chapter have used an array. Structures are a bit simpler: the offset to the fields is always constant: once we have the base address of the structure (the address of the first field) accessing a field is just an indexing mode with an offset (usually an immediate). Our current structure features, on purpose, a <code>char</code> as its first field <code>f0</code>. Currently we cannot work on scalars in memory of different size than 4 bytes. So we will postpone working on that first field for a future chapter.
</p>
<p>
For instance imagine we wanted to increment the field f1 like this.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;">b.<span style="color: #202020;">f1</span> <span style="color: #339933;">=</span> b.<span style="color: #202020;">f1</span> <span style="color: #339933;">+</span> <span style="color: #0000dd;">7</span><span style="color: #339933;">;</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">b.f1 = b.f1 + 7;</p></div>

<p>
If <code>r1</code> contains the base address of our structure, accessing the field <code>f1</code> is pretty easy now that we know all the available indexing modes.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>1
2
3
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">ldr r2<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #339933;">,</span> #<span style="color: #339933;">+</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>!  <span style="color: #339933;">/*</span> r1 ← r1 <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span> then r2 ← <span style="color: #339933;">*</span>r1 <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">add</span> r2<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">7</span>     <span style="color: #339933;">/*</span> r2 ← r2 <span style="color: #339933;">+</span> <span style="color: #ff0000;">7</span> <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">str</span> r2<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #009900; font-weight: bold;">]</span>       <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span>r1 ← r2 <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">ldr r2, [r1, #+4]!  /* r1 ← r1 + 4 then r2 ← *r1 */
add r2, r2, #7     /* r2 ← r2 + 7 */
str r2, [r1]       /* *r1 ← r2 */</p></div>

<p>
Note that we use a pre-indexing mode to keep in <code>r1</code> the address of the field <code>f1</code>. This way the second store does not need to compute that address again.
</p>
<p>
That’s all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/&amp;text=ARM+assembler+in+Raspberry+Pi+%E2%80%93+Chapter+8+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>				<p class="tags"><span><a href="http://thinkingeek.com/tag/addresses/" rel="tag">addresses</a>, <a href="http://thinkingeek.com/tag/arm/" rel="tag">arm</a>, <a href="http://thinkingeek.com/tag/assembler/" rel="tag">assembler</a>, <a href="http://thinkingeek.com/tag/indexing-modes/" rel="tag">indexing modes</a>, <a href="http://thinkingeek.com/tag/pi/" rel="tag">pi</a>, <a href="http://thinkingeek.com/tag/postindex/" rel="tag">postindex</a>, <a href="http://thinkingeek.com/tag/preindex/" rel="tag">preindex</a>, <a href="http://thinkingeek.com/tag/raspberry/" rel="tag">raspberry</a></span></p>		<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2013/01/26/arm-assembler-raspberry-pi-chapter-7/" rel="prev">ARM assembler in Raspberry Pi – Chapter 7</a></span>
			<span class="next"><a href="http://thinkingeek.com/2013/02/02/arm-assembler-raspberry-pi-chapter-9/" rel="next">ARM assembler in Raspberry Pi – Chapter 9</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
			<h3>17 thoughts on “<span>ARM assembler in Raspberry Pi – Chapter 8</span>”</h3>
		<ul class="commentlist">
				<li class="comment even thread-even depth-1" id="comment-1434">
				<div id="div-comment-1434" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a16ae1e69be669d6f2c5fa0972e39c8a?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a16ae1e69be669d6f2c5fa0972e39c8a?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a16ae1e69be669d6f2c5fa0972e39c8a?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a16ae1e69be669d6f2c5fa0972e39c8a-0">			<cite class="fn"><a href="http://digilander.libero.it/zantaz/" rel="external nofollow" class="url">Mariani Antonio Mario</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/#comment-1434">
			June 29, 2013 at 5:42 am</a>		</div>

		<p>nice code …</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/?replytocom=1434#respond" onclick="return addComment.moveForm( &quot;div-comment-1434&quot;, &quot;1434&quot;, &quot;respond&quot;, &quot;546&quot; )" aria-label="Reply to Mariani Antonio Mario">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1" id="comment-3494">
				<div id="div-comment-3494" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/4eac962f395d5a867627b5c98c084a33?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/4eac962f395d5a867627b5c98c084a33?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/4eac962f395d5a867627b5c98c084a33?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-4eac962f395d5a867627b5c98c084a33-0">			<cite class="fn">Hien Nguyen</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/#comment-3494">
			January 23, 2014 at 12:58 am</a>		</div>

		<p>Your tutorial is very good. Thank you so much !</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/?replytocom=3494#respond" onclick="return addComment.moveForm( &quot;div-comment-3494&quot;, &quot;3494&quot;, &quot;respond&quot;, &quot;546&quot; )" aria-label="Reply to Hien Nguyen">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1" id="comment-3777">
				<div id="div-comment-3777" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/840007e78e078c67490e96ac2d055753?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/840007e78e078c67490e96ac2d055753?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/840007e78e078c67490e96ac2d055753?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-840007e78e078c67490e96ac2d055753-0">			<cite class="fn"><a href="http://frozentivi.com" rel="external nofollow" class="url">Amand</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/#comment-3777">
			January 28, 2014 at 11:09 pm</a>		</div>

		<p>Waouh! Your explications are incredibly good! Thanks a lot for the hard work!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/?replytocom=3777#respond" onclick="return addComment.moveForm( &quot;div-comment-3777&quot;, &quot;3777&quot;, &quot;respond&quot;, &quot;546&quot; )" aria-label="Reply to Amand">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1 parent" id="comment-157069">
				<div id="div-comment-157069" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/c5128194ae0fa12dc2c34b146c3d11a5?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/c5128194ae0fa12dc2c34b146c3d11a5?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/c5128194ae0fa12dc2c34b146c3d11a5?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-c5128194ae0fa12dc2c34b146c3d11a5-0">			<cite class="fn"><a href="http://antoniovillena.es" rel="external nofollow" class="url">Antonio Villena</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/#comment-157069">
			June 15, 2014 at 4:13 am</a>		</div>

		<p>Errata. In the text:<br>
“we can set a[4] to 3 this…”<br>
The correct sentence is:<br>
“we can set a[3] to 3 this…”</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/?replytocom=157069#respond" onclick="return addComment.moveForm( &quot;div-comment-157069&quot;, &quot;157069&quot;, &quot;respond&quot;, &quot;546&quot; )" aria-label="Reply to Antonio Villena">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2" id="comment-157243">
				<div id="div-comment-157243" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-0">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/#comment-157243">
			June 15, 2014 at 9:48 am</a>		</div>

		<p>Hi Antonio,</p>
<p>thanks. Fixed.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/?replytocom=157243#respond" onclick="return addComment.moveForm( &quot;div-comment-157243&quot;, &quot;157243&quot;, &quot;respond&quot;, &quot;546&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-even depth-1 parent" id="comment-807158">
				<div id="div-comment-807158" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/72e355463dba5d622641bf2a762899f3?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/72e355463dba5d622641bf2a762899f3?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/72e355463dba5d622641bf2a762899f3?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-72e355463dba5d622641bf2a762899f3-0">			<cite class="fn">Shuai</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/#comment-807158">
			February 22, 2015 at 10:37 pm</a>		</div>

		<p>Imagine we want to compute a[4] = a[4] + a[4].<br>
Should it really be a[3] = a[3]+a[3]?</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/?replytocom=807158#respond" onclick="return addComment.moveForm( &quot;div-comment-807158&quot;, &quot;807158&quot;, &quot;respond&quot;, &quot;546&quot; )" aria-label="Reply to Shuai">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2" id="comment-808390">
				<div id="div-comment-808390" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-1">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/#comment-808390">
			February 26, 2015 at 9:18 pm</a>		</div>

		<p>Hi Shuai,</p>
<p>yes, you are right. I already fixed the post.</p>
<p>Thanks a lot.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/?replytocom=808390#respond" onclick="return addComment.moveForm( &quot;div-comment-808390&quot;, &quot;808390&quot;, &quot;respond&quot;, &quot;546&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1 parent" id="comment-951024">
				<div id="div-comment-951024" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/9b6de309a0c99eab0b048bf8f4408ead?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/9b6de309a0c99eab0b048bf8f4408ead?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/9b6de309a0c99eab0b048bf8f4408ead?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-9b6de309a0c99eab0b048bf8f4408ead-0">			<cite class="fn">Smasher</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/#comment-951024">
			November 12, 2015 at 3:29 pm</a>		</div>

		<p>str r2, [r1, +#12]  /* *(r1 + 12) ← r2 */</p>
<p>shouldn’t it be r2 ← (r1 + 12)</p>
<p>this also applies to another comments where you use str</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/?replytocom=951024#respond" onclick="return addComment.moveForm( &quot;div-comment-951024&quot;, &quot;951024&quot;, &quot;respond&quot;, &quot;546&quot; )" aria-label="Reply to Smasher">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment even depth-2 parent" id="comment-951025">
				<div id="div-comment-951025" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/9b6de309a0c99eab0b048bf8f4408ead?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/9b6de309a0c99eab0b048bf8f4408ead?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/9b6de309a0c99eab0b048bf8f4408ead?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-9b6de309a0c99eab0b048bf8f4408ead-1">			<cite class="fn">Smasher</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/#comment-951025">
			November 12, 2015 at 3:31 pm</a>		</div>

		<p>Forget about it… that divergence of argument positions drives me mad </p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/?replytocom=951025#respond" onclick="return addComment.moveForm( &quot;div-comment-951025&quot;, &quot;951025&quot;, &quot;respond&quot;, &quot;546&quot; )" aria-label="Reply to Smasher">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-3" id="comment-951186">
				<div id="div-comment-951186" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-2">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/#comment-951186">
			November 13, 2015 at 10:23 am</a>		</div>

		<p>Yes, store instructions look weird because the <em>logical</em> destination happens to be the second operand.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/?replytocom=951186#respond" onclick="return addComment.moveForm( &quot;div-comment-951186&quot;, &quot;951186&quot;, &quot;respond&quot;, &quot;546&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1 parent" id="comment-975598">
				<div id="div-comment-975598" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/28718ba948b3c98d87dd1a872c485649?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/28718ba948b3c98d87dd1a872c485649?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/28718ba948b3c98d87dd1a872c485649?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-28718ba948b3c98d87dd1a872c485649-0">			<cite class="fn">Zack Stauber</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/#comment-975598">
			September 25, 2016 at 7:26 am</a>		</div>

		<p>Thank you so much for this tutorial, Mr. Ibáñez.  It’s really great to finally learn some Assembly.  One correction.  Under post-indexing, line 19, which is<br>
str r2, [r1], +4<br>
I think should be<br>
str r2, [r1], +#4</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/?replytocom=975598#respond" onclick="return addComment.moveForm( &quot;div-comment-975598&quot;, &quot;975598&quot;, &quot;respond&quot;, &quot;546&quot; )" aria-label="Reply to Zack Stauber">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-975952">
				<div id="div-comment-975952" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-3">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/#comment-975952">
			September 29, 2016 at 7:16 pm</a>		</div>

		<p>Hi Zack,</p>
<p>thanks! I have fixed the post.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/?replytocom=975952#respond" onclick="return addComment.moveForm( &quot;div-comment-975952&quot;, &quot;975952&quot;, &quot;respond&quot;, &quot;546&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-986666">
				<div id="div-comment-986666" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/06490ee5e3f3eff2ecc52654751d2b15?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/06490ee5e3f3eff2ecc52654751d2b15?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/06490ee5e3f3eff2ecc52654751d2b15?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-06490ee5e3f3eff2ecc52654751d2b15-0">			<cite class="fn">Marska</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/#comment-986666">
			February 14, 2017 at 7:56 pm</a>		</div>

		<p>Why have you exchanged + and # character order comparing non-updated and updated modes?<br>
str r2, [r1, +#12]<br>
ldr r2, [r1, #+4]!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/?replytocom=986666#respond" onclick="return addComment.moveForm( &quot;div-comment-986666&quot;, &quot;986666&quot;, &quot;respond&quot;, &quot;546&quot; )" aria-label="Reply to Marska">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-986906">
				<div id="div-comment-986906" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-4">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/#comment-986906">
			February 17, 2017 at 9:15 pm</a>		</div>

		<p>Hi Marska,</p>
<p>this was a mistake of mine, I’ve fixed the post to be <code>#+12</code>.</p>
<p>Thanks for the heads up!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/?replytocom=986906#respond" onclick="return addComment.moveForm( &quot;div-comment-986906&quot;, &quot;986906&quot;, &quot;respond&quot;, &quot;546&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1 parent" id="comment-986882">
				<div id="div-comment-986882" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/5301f9fac283ef87f1f4742db878c8e3?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/5301f9fac283ef87f1f4742db878c8e3?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/5301f9fac283ef87f1f4742db878c8e3?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-5301f9fac283ef87f1f4742db878c8e3-0">			<cite class="fn">Rohan</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/#comment-986882">
			February 17, 2017 at 3:31 pm</a>		</div>

		<p>Hello Mr. Ibáñez,</p>
<p>In <code>array01.s</code>, line 19 contains <code>add r3, r1, r2, LSL #2</code>. Should <code>add</code> be changed to <code>mov</code>?</p>
<p>Thank you</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/?replytocom=986882#respond" onclick="return addComment.moveForm( &quot;div-comment-986882&quot;, &quot;986882&quot;, &quot;respond&quot;, &quot;546&quot; )" aria-label="Reply to Rohan">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment odd alt depth-2 parent" id="comment-986883">
				<div id="div-comment-986883" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/5301f9fac283ef87f1f4742db878c8e3?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/5301f9fac283ef87f1f4742db878c8e3?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/5301f9fac283ef87f1f4742db878c8e3?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-5301f9fac283ef87f1f4742db878c8e3-1">			<cite class="fn">Rohan</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/#comment-986883">
			February 17, 2017 at 3:36 pm</a>		</div>

		<p>Sorry, I misread the code <img draggable="false" class="emoji" alt="😛" src="https://s.w.org/images/core/emoji/2.2.1/svg/1f61b.svg" scale="0"></p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/?replytocom=986883#respond" onclick="return addComment.moveForm( &quot;div-comment-986883&quot;, &quot;986883&quot;, &quot;respond&quot;, &quot;546&quot; )" aria-label="Reply to Rohan">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-3" id="comment-986904">
				<div id="div-comment-986904" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-5">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/#comment-986904">
			February 17, 2017 at 9:05 pm</a>		</div>

		<p>No worries </p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/01/27/arm-assembler-raspberry-pi-chapter-8/?replytocom=986904#respond" onclick="return addComment.moveForm( &quot;div-comment-986904&quot;, &quot;986904&quot;, &quot;respond&quot;, &quot;546&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		</ul>
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2013/01/27/arm-assembler-raspberry-pi-chapter-8/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="546" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="7c1d12488c"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496597950414"></form>
			</div><!-- #respond -->
	</div>	</div>
