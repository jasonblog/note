# Chapter 6


<div class="main">
			<h1>Exploring AArch64 assembler – Chapter 6 </h1>
		<p class="post-meta"><span class="date">November 27, 2016</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2016/11/27/exploring-aarch64-assembler-chapter-6/#respond">0</a></span> </p>
		<p>
So far we know how to do some computations and access memory. Today we will learn how to alter the control flow of our program.
</p>
<p><span id="more-3605"></span></p>
<h2>Implicit sequencing</h2>
<p>
Almost everyone would expect that a sequence of instructions, like the ones we’ve seen in previous chapters, would run one after the other. This is so fundamental that we can call it <em>implicit sequencing</em>: after one instruction has been executed, the next one does. This seems very obvious.
</p>
<p>
But what if we wanted to change this implicit sequencing with something else. What if we could, selectively, execute a subset of the instructions. Even better, what if we could run a subset of instructions depending on some condition?
</p>
<h2>Where the programs live</h2>
<p>
Programs live in memory. This seems so incredibly obvious. But in fact it is not. It is relatively simple to create specific digital circuits that perform very specific tasks. Their “program” does not live in memory, instead it is encoded directly in the circuit. But, while these circuits have a place, nowadays we can use CPUs that are very sophisticated circuits that can execute programs encoded as instructions.
</p>
<p>
If programs live in memory and are at their core a bunch of instructions, it means that the instructions will be addressable (each will have an address, once in memory). Given that, from a functional point of view, a CPU is at any given time executing only a single instruction, it may make sense to know which instruction is. The instruction itself, the particular binary code associated to it, is not very useful but its address is. So maybe we want to store the address of the current instruction somewhere in the CPU. And this is what it happens.
</p>
<h2>The program counter</h2>
<p>
The piece of memory that keeps track of the current instruction is called the <em>program counter</em>. In AArch64 the program counter is stored in a register called <code>pc</code> (for program counter). It is a 64-bit register with the address of the current instruction.</p>
<p></p><p>
The <code>pc</code> tells the CPU which is the instruction that has to be executed. The CPU goes to memory, requests the 4 bytes at the address that the <code>pc</code> says. These 4 bytes are the instruction, recall that in AArch64 instructions are encoded in 32-bit. The instruction is then executed. When the execution of the instruction ends, the <code>pc</code> is incremented by 4 bytes. This is how implicit sequencing works. That’s it.
</p>
<p>
But we may wonder, what if somehow rather than doing <code>pc ← pc + 4</code> we could set the <code>pc</code> to something else when the instruction ends. Would the CPU execute some other instruction rather than the next one? The answer is yes. Except that in AArch64 we cannot write directly to the <code>pc</code>. But we can by doing branches.
</p>
<h2>Branches</h2>
<p>
Branches are instructions that are able to change the <code>pc</code>. By doing that they can alter the implicit sequencing of our program.
</p>
<p>
There are two kinds of branches: unconditional branches and conditional branches. Unconditional branches always set the <code>pc</code> to some value while conditional branches only do when some condition holds. When a branch sets the <code>pc</code> we say that the branch is <em>taken</em>. The address to where the <code>pc</code> is set is called the <em>target</em> of the branch. If a branch is not taken, then implicit sequencing applies.
</p>
<h3>Target of the branch</h3>
<p>
The target of the branch is an address. Addresses in assembler are most of the time represented using symbolic labels. So a branch will have at least one operand that is the target of the branch. Given that the address is encoded in the instruction there may be limitations of which labels we can use.
</p>
<h2>Unconditional branches</h2>
<p>
Unconditional branches are represented using the instruction <code>b</code>, for branch. The target of the branch is encoded as an offset of 27 bits, this is, an offset of ±128MiB. This offset, which is computed by the assembler tool, is added to the <code>pc</code>.
</p>
<p>
The following program will set the error condition to 3, even if there is an instruction that sets it to 4. The branch simply skips the execution of the second <code>mov</code>.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> branch <span style="color: #339933;">*/</span>
<span style="color: #0000ff; font-weight: bold;">.text</span>
<span style="color: #339933;">.</span>globl main
main<span style="color: #339933;">:</span>
  <span style="color: #00007f; font-weight: bold;">mov</span> w0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">3</span>   <span style="color: #339933;">//</span> w0 ← <span style="color: #ff0000;">3</span>
  b jump       <span style="color: #339933;">//</span> branch to label jump
  <span style="color: #00007f; font-weight: bold;">mov</span> w0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>   <span style="color: #339933;">//</span> w0 ← <span style="color: #ff0000;">4</span>
  jump<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">ret</span>        <span style="color: #339933;">//</span> end function</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* branch */
.text
.globl main
main:
  mov w0, #3   // w0 ← 3
  b jump       // branch to label jump
  mov w0, #4   // w0 ← 4
  jump:
    ret        // end function</p></div>

<p>
I will be writing labels in their own line (check <code>main:</code> and <code>jump:</code> above) which I think is a bit easier to read. I will also indent instruction after a label (check <code>ret</code>).
</p>
<p>
If we run this program effectively it sets the error code to 3.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="bash" style="font-family:monospace;">$ .<span style="color: #000000; font-weight: bold;">/</span>branch ; <span style="color: #7a0874; font-weight: bold;">echo</span> <span style="color: #007800;">$?</span>
<span style="color: #000000;">3</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ ./branch ; echo $?
3</p></div>

<p>
Before you ask, yes, we can cause a program that never ends by jumping to ourselves. This will hang (use Ctrl-C to end it)
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> neverend<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #0000ff; font-weight: bold;">.text</span>
<span style="color: #339933;">.</span>globl main
main<span style="color: #339933;">:</span>
  noprogress<span style="color: #339933;">:</span>
    b noprogress  <span style="color: #339933;">//</span> note<span style="color: #339933;">:</span> <span style="color: #7f007f;">'main'</span> <span style="color: #00007f; font-weight: bold;">and</span> <span style="color: #7f007f;">'noprogres'</span><span style="color: #339933;">.</span> are two different
                  <span style="color: #339933;">//</span> labels to the same address<span style="color: #339933;">,</span> doing <span style="color: #7f007f;">'b main'</span> would hang
                  <span style="color: #339933;">//</span> as well
  <span style="color: #00007f; font-weight: bold;">ret</span> <span style="color: #339933;">//</span> it will never return!</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* neverend.s */
.text
.globl main
main:
  noprogress:
    b noprogress  // note: 'main' and 'noprogres'. are two different
                  // labels to the same address, doing 'b main' would hang
                  // as well
  ret // it will never return!</p></div>

<p>
At first it seems that unconditional branches are not very useful. They have their usefulness but we are still missing some details to be able to appreciate them. In a next chapter we will see another unconditional branch instruction called <code>bl</code>. That one is much more interesting, I promise.
</p>
<h2>Conditional branches</h2>
<p>
Conditional branches are a bit more interesting because the branch is taken only if some condition holds. Which kind of conditions?
</p>
<p>
AArch64 has a four <em>flags</em>. Each flag means some condition resulting from the execution of some instruction. Just a few instructions set these flags. For the current chapter we will consider only <code>cmp</code>. Also just a few instructions use these flags: among them conditional branch instructions.
</p>
<h3>Flags</h3>
<p>
As mentioned above there are 4 flags. They are <code>N</code>, <code>Z</code>, <code>C</code>, <code>V</code>. They are set as a result of executing one instruction that updates the flags. They have very specific meanings:
</p>
<ul>
<li><code>N</code> (<strong>n</strong>egative) is set if the result of the instruction yields a value that can be understood as a negative number. So positive and zero will have this flag unset.
</li><li><code>Z</code> (<strong>z</strong>ero) is set if the result of the instruction yields a value that is zero.
</li><li><code>C</code> (<strong>c</strong>arry) is set if the result of the instruction cannot be represented as an unsigned integer.
</li><li><code>V</code> (o<strong>v</strong>erflow) is set if the result of the instruction cannot be represented as a signed integer.
</li></ul>
<div style="background-color: #efe; padding: 0.3em; padding-left: 1em; padding-right: 1em;">
<p>
The difference between flag <code>C</code> and <code>V</code> lies on the interpretation of the result. If we add two large positive integers and the result cannot be encoded as an unsigned integer, then C will be set. This happens, for instance, if we compute 2<sup>31</sup> + 2<sup>31</sup> and we want to keep the result in a 32-bit register: 2<sup>32</sup> cannot be represented with 32 bits. In this case C is set.
</p>
<p>
If we add two large positive integers or two large negative integers, it may happen that the result is too big (in absolute terms) to be encoded as a signed integer. For instance if we compute 2<sup>30</sup> + 2<sup>30</sup> the result is 2<sup>31</sup>. If, again, we want to keep the result in a 32-bit register as a signed integer (i.e. two’s complement) it simply cannot be encoded as such, so in this case <code>V</code> will be set but not <code>C</code>.
</p>
<p>
You may be wondering whether there is a case where <code>C</code> is set but not <code>V</code>. In fact there are many, when adding two signed integers, if one of them is negative and the other one is positive, <code>C</code> may be set but V will never be set (this operation cannot overflow in a signed integer interpretation). Consider for instance computing -1 + 2. The result is 1, so it fits in the range of signed integers, thus V is not enabled. But the encoding of the signed integer -1 interpreted as an unsigned integer is the value where all bits are 1. For a 32-bit operation this is 2<sup>32</sup>-1. So, when we compute -1 + 2 we are actually doing 2<sup>32</sup>-1 + 2, which would be 2<sup>32</sup>+1. This number cannot be encoded in 32-bit as an unsigned integer, so the flag <code>C</code> would be enabled.
</p>
<p></p><div id="attachment_3677" style="width: 661px" class="wp-caption aligncenter"><img data-attachment-id="3677" data-permalink="http://thinkingeek.com/2016/11/27/exploring-aarch64-assembler-chapter-6/integers/" data-orig-file="http://thinkingeek.com/wp-content/uploads/2016/11/integers.png" data-orig-size="651,260" data-comments-opened="1" data-image-meta="{&quot;aperture&quot;:&quot;0&quot;,&quot;credit&quot;:&quot;&quot;,&quot;camera&quot;:&quot;&quot;,&quot;caption&quot;:&quot;&quot;,&quot;created_timestamp&quot;:&quot;0&quot;,&quot;copyright&quot;:&quot;&quot;,&quot;focal_length&quot;:&quot;0&quot;,&quot;iso&quot;:&quot;0&quot;,&quot;shutter_speed&quot;:&quot;0&quot;,&quot;title&quot;:&quot;&quot;,&quot;orientation&quot;:&quot;0&quot;}" data-image-title="integers" data-image-description="" data-medium-file="http://thinkingeek.com/wp-content/uploads/2016/11/integers-300x120.png" data-large-file="http://thinkingeek.com/wp-content/uploads/2016/11/integers.png" src="http://thinkingeek.com/wp-content/uploads/2016/11/integers.png" alt="Unsigned and signed integers and how they are represented." width="651" height="260" class="size-full wp-image-3677" srcset="http://thinkingeek.com/wp-content/uploads/2016/11/integers.png 651w, http://thinkingeek.com/wp-content/uploads/2016/11/integers-300x120.png 300w" sizes="(max-width: 651px) 100vw, 651px"><p class="wp-caption-text">Unsigned and signed integers encoded in N-bits</p></div>
</div>
<h3>Utility of flags</h3>
<p>
Now that we see what the four flags mean, we can see some instructions that set them. Note that flags are set according to the result of some operation. Recall that for the purpose of the scope of this post we will consider only instruction <code>cmp</code>.</p>
<p></p><p>
Instruction <code>cmp</code> does a comparison of the two operands. To do this it subtracts the second operand to the first operand (i.e. first – second) and updates the flags with the result of that subtraction. This subtraction is discarded, so the whole purpose of this operation is to update the flags.
</p>
<p>
Now consider several non obvious consequences of the comparison itself and how they are materialized in the flags. If we compare two registers that contain the same value, the subtraction will be zero, so the flag Z will be set. If the numbers are different, the result will be nonzero so Z will not be set. So the flag Z can be used to tell whether two values are the same or not.
</p>
<p>
If we are comparing integers we have to be more careful here. Let’s start first with unsigned integers. In principle, subtracting one unsigned integer to another would only be well defined if the second is lower than the first. To avoid this problem, a two’s complement subtraction is used instead. In two’s complement a subtraction is just the usual addition where the second operand has been negated. If the first number is greater than the second, the result will be a positive number so the flag C will be set. If the first number is lower than the second, the addition will not overflow as it will stay as a negative number, so C will not be set.
</p>
<p>
If we are comparing two signed integers, the whole thing is a bit more complicated. If the first integer is larger than the second, a similar thing happens to unsigned integers, but now the result is interpreted as a signed number. This means that N should not be set in this case. And V should not be set either. But there are cases that the subtraction can cause a signed integer overflow: for instance if we subtract -1 to the largest positive representable integer. This is like adding 1 to that number. This causes an overflow and also the result happens to be interpreted as a negative, so N will be set. The reasoning here is that, if no overflow happens, the subtraction will look like a positive number. If overflow happens, the number must have become negative. So we can tell if the first number is larger than the second when N<em> and V have the same value. Now consider the case where the first number is lower than the second, here the subtraction will always yield a negative number, except possibly when overflow happens. In this case the overflown number will be a positive one, consider for instance subtracting 1 to the smallest positive number, this is like adding -1. This will cause a signed integer overflow and the resulting number will be positive. So a way to tell whether a signed integer is lower than another one is that N and V have different values.</em></p><em>
<p></p><h3>Condition codes</h3>
</em><p><em>
Now that we have some examples of the utility of the flags, we can talk about </em>condition codes. A condition code is true when flags have some specific values. Condition codes are used by branching instructions, to determine if they are taken or not. Note that not all condition codes will be useful for a <code>cmp</code> operation (but they are for other instructions that set flags). I have marked in blue shade the most common condition codes.
</p>
<table>
<thead>
<tr>
<th>Name</th>
<th>Meaning</th>
<th>Condition flag</th>
</tr>
</thead>
<tbody>
<tr style="background: #def;">
<td>EQ</td>
<td>Values compare equal or the result of the operation is zero.</td>
<td>Z is set</td>
</tr>
<tr style="background: #def;">
<td>NE</td>
<td>Values compare different or the result of the operation is non-zero.</td>
<td>Z is not set</td>
</tr>
<tr style="background: #def;">
<td>GE</td>
<td>Greater or equal. Comparison of signed integers, the first operand is greater or equal than the second.</td>
<td>N and V are both set or both not set</td>
</tr>
<tr style="background: #def;">
<td>GT</td>
<td>Greater. Comparison of signed integers, the first operand is greater than the second.</td>
<td>Z is not set. N and V are both set or both not set</td>
</tr>
<tr style="background: #def;">
<td>LE</td>
<td>Lower or equal. Comparison of signed integers, the first operand is lower or equal than the second.</td>
<td>N is set and V is not set or the opposite</td>
</tr>
<tr style="background: #def;">
<td>LT</td>
<td>Lower. Comparison of signed integers, the first operand is lower than the second.</td>
<td>Z is not set. N is set and V is not set or the opposite</td>
</tr>
<tr>
<td>CS</td>
<td>Carry set. For instance, an unsigned addition overflows. Not very useful with <code>cmp</code></td>
<td>C is set</td>
</tr>
<tr>
<td>CC</td>
<td>Carry clear. For instance, an unsigned addition does not overflow. Not very useful with <code>cmp</code></td>
<td>C is not set</td>
</tr>
<tr>
<td>MI</td>
<td>Minus. For instance, an addition or subtraction results a negative number. Not very useful with <code>cmp</code></td>
<td>N is set</td>
</tr>
<tr>
<td>PL</td>
<td>Positive or zero. For instance, an addition or subtraction results in a non negative number. Not very useful with <code>cmp</code></td>
<td>N is not set</td>
</tr>
<tr>
<td>VS</td>
<td>Overflow of signed integers. Not very useful with <code>cmp</code></td>
<td>V is set</td>
</tr>
<tr>
<td>VC</td>
<td>No overflow of signed integers. Not very useful with <code>cmp</code></td>
<td>V is not set</td>
</tr>
<tr>
<td>HI</td>
<td>Higher. Comparison of unsigned integers, the first operand is strictly greater than the second</td>
<td>C is set and Z is not set</td>
</tr>
<tr>
<td>HS</td>
<td>Higher or same. Comparison of unsigned integers, the first operand is greater or the same. Note that this is an alias of CS above.</td>
<td>C is set</td>
</tr>
<tr>
<td>LO</td>
<td>Lower. Comparison of unsigned integers, the first operand is strictly lower than the second. Note that this is an alias of CC above.</td>
<td>C is not set</td>
</tr>
<tr>
<td>LS</td>
<td>Lower or same. Comparison of unsigned integers, the first operand is lower or equal than the second.</td>
<td>Either C is not set or Z is set</td>
</tr>
</tbody>
</table>
<h3>Conditional branch instruction</h3>
<p>
Ok, after all this prologue, we can now introduce the conditional branch instruction: <code>b.<em>cond</em></code></p>
<p></p><p>
This instruction is a bit special in that one of its operands, the condition code, is represented in the name of the instruction. The <code>cond</code> part is the condition code and has to be one of the condition codes describe above.
</p>
<h2>Fortran arithmetic if</h2>
<p>
As an example of branches, let’s see how we could implement a high-level programming language construct that alters the control flow of our program.
</p>
<p>
Fortran is a very old language invented in 1956. This means that has some odd constructions (in current practice, I guess at that moment they probably made sense). One of those constructions is the <code>arithmetic if</code> statement. The arithmetic if had this form
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>1
</pre></td><td class="code"><pre class="fortran" style="font-family:monospace;"><span style="color: #b1b100;">IF</span> expression, label1, label2, label3</pre></td></tr></tbody></table><p class="theCode" style="display:none;">IF expression, label1, label2, label3</p></div>

<p>
In Fortran the labels were represented with numbers from 0 to 99999 but we will use symbolic names instead. A Fortran program, when encounters an arithmetic if statement, evaluates the expression. The value of the expression is then compared to zero. If the expression is lower than zero, then the <code>label1</code> is taken. If the expression evaluates to zero, the <code>label2</code> is taken. Otherwise, if the expression evaluates to a value greater than zero the <code>label3</code> is taken.
</p>
<p>
Let’s assume, for simplicity, that the value of our expression is in <code>w0</code>. So the first thing we have to do is to compare it with zero.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>1
2
3
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">arithmetic_if<span style="color: #339933;">:</span>
  <span style="color: #339933;">//</span> <span style="color: #0000ff; font-weight: bold;">code</span> that evaluates the expression <span style="color: #00007f; font-weight: bold;">and</span> <span style="color: #00007f; font-weight: bold;">sets</span> its value <span style="color: #00007f; font-weight: bold;">in</span> w0
  <span style="color: #00007f; font-weight: bold;">cmp</span> w0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span> <span style="color: #339933;">//</span> compares w0 with <span style="color: #ff0000;">0</span> <span style="color: #00007f; font-weight: bold;">and</span> updates the flags</pre></td></tr></tbody></table><p class="theCode" style="display:none;">arithmetic_if:
  // code that evaluates the expression and sets its value in w0
  cmp w0, #0 // compares w0 with 0 and updates the flags</p></div>

<p>
Let’s assume that <code>w0</code> is a signed integer. The order of the comparisons is not important at first, so we can follow the description above.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>4
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">  b<span style="color: #339933;">.</span>lt label1 <span style="color: #339933;">//</span> if w0 &lt; <span style="color: #ff0000;">0</span> then branch to label1
  b<span style="color: #339933;">.</span>eq label2 <span style="color: #339933;">//</span> if w0 == <span style="color: #ff0000;">0</span> then branch to label2
  b<span style="color: #339933;">.</span>gt label3 <span style="color: #339933;">//</span> if w0 &gt; <span style="color: #ff0000;">1</span> then branch to label3
label1<span style="color: #339933;">:</span>
  <span style="color: #339933;">//</span> <span style="color: #0000ff; font-weight: bold;">code</span> for label1
  b end_of_arithmetic_if <span style="color: #339933;">//</span> branch to end_of_arithmetic_if
label2<span style="color: #339933;">:</span>
  <span style="color: #339933;">//</span> <span style="color: #0000ff; font-weight: bold;">code</span> for label2
  b end_of_arithmetic_if <span style="color: #339933;">//</span> branch to end_of_arithmetic_if
label3<span style="color: #339933;">:</span>
  <span style="color: #339933;">//</span> <span style="color: #0000ff; font-weight: bold;">code</span> for label3
  b end_of_arithmetic_if <span style="color: #339933;">//</span> branch to end_of_arithmetic_if
end_of_arithmetic_if<span style="color: #339933;">:</span>
  <span style="color: #339933;">//</span> <span style="color: #0000ff; font-weight: bold;">rest</span> ouf our Fortran program <span style="color: #339933;">:</span><span style="color: #009900; font-weight: bold;">)</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">  b.lt label1 // if w0 &lt; 0 then branch to label1
  b.eq label2 // if w0 == 0 then branch to label2
  b.gt label3 // if w0 &gt; 1 then branch to label3
label1:
  // code for label1
  b end_of_arithmetic_if // branch to end_of_arithmetic_if
label2:
  // code for label2
  b end_of_arithmetic_if // branch to end_of_arithmetic_if
label3:
  // code for label3
  b end_of_arithmetic_if // branch to end_of_arithmetic_if
end_of_arithmetic_if:
  // rest ouf our Fortran program :)</p></div>

<p>
Of course we can save some branches if we make use of the layout of the instructions of our program.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">arithmetic_if<span style="color: #339933;">:</span>
  <span style="color: #339933;">//</span> <span style="color: #0000ff; font-weight: bold;">code</span> that evaluates the expression <span style="color: #00007f; font-weight: bold;">and</span> <span style="color: #00007f; font-weight: bold;">sets</span> its value <span style="color: #00007f; font-weight: bold;">in</span> w0
  <span style="color: #00007f; font-weight: bold;">cmp</span> w0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span> <span style="color: #339933;">//</span> compares w0 with <span style="color: #ff0000;">0</span> <span style="color: #00007f; font-weight: bold;">and</span> updates the flags
  b<span style="color: #339933;">.</span>lt label1 <span style="color: #339933;">//</span> if w0 &lt; <span style="color: #ff0000;">0</span> then branch to label1
  b<span style="color: #339933;">.</span>eq label2 <span style="color: #339933;">//</span> if w0 == <span style="color: #ff0000;">0</span> then branch to label2
  <span style="color: #339933;">//</span> <span style="color: #0000ff; font-weight: bold;">code</span> for label3
  b end_of_arithmetic_if <span style="color: #339933;">//</span> branch to end_of_arithmetic_if
label1<span style="color: #339933;">:</span>
  <span style="color: #339933;">//</span> <span style="color: #0000ff; font-weight: bold;">code</span> for label1
  b end_of_arithmetic_if <span style="color: #339933;">//</span> branch to end_of_arithmetic_if
label2<span style="color: #339933;">:</span>
  <span style="color: #339933;">//</span> <span style="color: #0000ff; font-weight: bold;">code</span> for label2
end_of_arithmetic_if<span style="color: #339933;">:</span>
  <span style="color: #339933;">//</span> <span style="color: #0000ff; font-weight: bold;">rest</span> ouf our Fortran program <span style="color: #339933;">:</span><span style="color: #009900; font-weight: bold;">)</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">arithmetic_if:
  // code that evaluates the expression and sets its value in w0
  cmp w0, #0 // compares w0 with 0 and updates the flags
  b.lt label1 // if w0 &lt; 0 then branch to label1
  b.eq label2 // if w0 == 0 then branch to label2
  // code for label3
  b end_of_arithmetic_if // branch to end_of_arithmetic_if
label1:
  // code for label1
  b end_of_arithmetic_if // branch to end_of_arithmetic_if
label2:
  // code for label2
end_of_arithmetic_if:
  // rest ouf our Fortran program :)</p></div>

<p>
Ok, this post has been too long already. So let’s stop here. In a next chapter we will put conditional branches at work by implementing a few common high level constructs.
</p>
<p>
That’s all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2016/11/27/exploring-aarch64-assembler-chapter-6/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2016/11/27/exploring-aarch64-assembler-chapter-6/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2016/11/27/exploring-aarch64-assembler-chapter-6/&amp;text=Exploring+AArch64+assembler+%E2%80%93+Chapter+6+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2016/11/27/exploring-aarch64-assembler-chapter-6/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>						<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2016/11/13/exploring-aarch64-assembler-chapter-5/" rel="prev">Exploring AArch64 assembler – Chapter 5</a></span>
			<span class="next"><a href="http://thinkingeek.com/2016/12/03/not-write-c-front-part-3/" rel="next">How (not) to write a C++ front end – Part 3</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2016/11/27/exploring-aarch64-assembler-chapter-6/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="3605" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="851632febe"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496668950907"></form>
			</div><!-- #respond -->
	</div>	</div>