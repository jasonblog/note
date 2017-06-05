# Chapter 5


<div class="main">
			<h1>Exploring AArch64 assembler – Chapter 5 </h1>
		<p class="post-meta"><span class="date">November 13, 2016</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2016/11/13/exploring-aarch64-assembler-chapter-5/#respond">0</a></span> </p>
		<p>
In this chapter we will see how we can access the memory in AArch64.
</p>
<p><span id="more-3572"></span></p>
<h2>Memory</h2>
<p>
Random access memory, or simply, <em>memory</em> is an essential component of any architecture. Recall that the memory can be seen as an array of bytes each paired with a consecutive number called the address. In AArch64 the address is a 64-bit number (which does not mean all the bits are meaningful for addresses).
</p>
<h3>Algebra of addresses</h3>
<p>
Given that addresses are numbers we can operate them as such. Not all arithmetic operations make sense on addresses, though. A higher address can be subtracted to a lower address. The result is not an address but an offset. Offsets can be added to addresses to form other addresses. Many times we need to access elements of size <code>b</code> that are consecutive in memory, thus their addresses are consecutive as well. This means that it is a common operation to compute an address of the form <code>A + b*i</code> to compute the address of the <code>i</code>-th element.</p>
<p></p><p>
The occurrence of these common address operations influences the instructions of the architecture as we will see below.
</p>
<h2>Load and store</h2>
<p>
Due to its RISC heritage, AArch64 instructions do not operate with memory. Only two specific instructions can: load and store. These two instructions have in their most basic incarnation two operands: a register and an address. The address is computed based on an addressing mode, that we will see below. A load retrieves a number of bytes from the computed address and puts them in the register. A store takes some bytes from a register and puts them in a number of memory locations identified by the address.
</p>
<p>
AArch64 supports a few load/store instructions, but for the purpose of this chapter we will consider only <code>ldr</code> (load) and <code>str</code> (store). Their syntax looks like this
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">ldr Xn<span style="color: #339933;">,</span> address<span style="color: #339933;">-</span>mode  <span style="color: #339933;">//</span> Xn ← <span style="color: #ff0000;">8</span> bytes of memory <span style="color: #0000ff; font-weight: bold;">at</span> address computed by address<span style="color: #339933;">-</span>mode
ldr Wn<span style="color: #339933;">,</span> address<span style="color: #339933;">-</span>mode  <span style="color: #339933;">//</span> Wn ← <span style="color: #ff0000;">4</span> bytes of memory <span style="color: #0000ff; font-weight: bold;">at</span> address computed by address<span style="color: #339933;">-</span>mode
&nbsp;
<span style="color: #00007f; font-weight: bold;">str</span> Xn<span style="color: #339933;">,</span> address<span style="color: #339933;">-</span>mode  <span style="color: #339933;">//</span> <span style="color: #ff0000;">8</span> bytes of memory <span style="color: #0000ff; font-weight: bold;">at</span> address computed by address<span style="color: #339933;">-</span>mode ← Xn
<span style="color: #00007f; font-weight: bold;">str</span> Wn<span style="color: #339933;">,</span> address<span style="color: #339933;">-</span>mode  <span style="color: #339933;">//</span> <span style="color: #ff0000;">4</span> bytes of memory <span style="color: #0000ff; font-weight: bold;">at</span> address computed by address<span style="color: #339933;">-</span>mode ← Wn</pre></td></tr></tbody></table><p class="theCode" style="display:none;">ldr Xn, address-mode  // Xn ← 8 bytes of memory at address computed by address-mode
ldr Wn, address-mode  // Wn ← 4 bytes of memory at address computed by address-mode

str Xn, address-mode  // 8 bytes of memory at address computed by address-mode ← Xn
str Wn, address-mode  // 4 bytes of memory at address computed by address-mode ← Wn</p></div>

<p>
AArch64 can be configured as big-endian or little-endian. This makes little difference in general but it determines where the 8/4 bytes we load/store go to/come from our register. We will assume a little-endian setting (which is the most usual). This means when doing a load/store of 8 bytes, the first byte corresponds to the lest significant byte of the register, the next byte goes to the next least significant byte, and so on. A big endian machine would work in the opposite way, the first byte would correspond to the most significant byte.
</p>
<h2>Addressing modes</h2>
<p>
An addressing mode is the process by which a load/store instruction will compute the address it will access. Instructions in AArch64 are encoded in 32-bit but we already mentioned that addresses are 64-bit. This means that the most basic addressing mode using an immediate will not be available. Some architectures are able to encode full addresses in their instructions. Programs in these architecture rarely do this since it may take a lot of space.
</p>
<h3>Base</h3>
<p>
The next simplest addressing mode we can consider is when, by some mechanism not yet discussed, we have an address in a <code>Xn</code> register. In this case the address calculation just uses the value in the register. This mode is called <em>base register</em> and the syntax for it is <code>[Xn]</code>. Only a 64-bit register can be used as base.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">ldr W2<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>X1<span style="color: #009900; font-weight: bold;">]</span>  <span style="color: #339933;">//</span> W2 ← <span style="color: #339933;">*</span>X1 <span style="color: #009900; font-weight: bold;">(</span><span style="color: #ff0000;">32</span><span style="color: #339933;">-</span>bit load<span style="color: #009900; font-weight: bold;">)</span>
ldr X2<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>X1<span style="color: #009900; font-weight: bold;">]</span>  <span style="color: #339933;">//</span> X2 ← <span style="color: #339933;">*</span>X1 <span style="color: #009900; font-weight: bold;">(</span><span style="color: #ff0000;">64</span><span style="color: #339933;">-</span>bit load<span style="color: #009900; font-weight: bold;">)</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">ldr W2, [X1]  // W2 ← *X1 (32-bit load)
ldr X2, [X1]  // X2 ← *X1 (64-bit load)</p></div>

<h3>Base plus immediate offset</h3>
<p>
As mentioned above when discussing the algebra of addresses, we can add an offset to an address to form another address. The syntax is <code>[Xn, #offset]</code>. The offset may range from -256 to 255 for 32-bit and 64-bit accesses. Larger offsets are a bit more limited: for 32-bit it must be multiple of 4 in the range 0 to 16380, for 64-bit it must be a multiple of 8 in the range of 0 to 32760.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">ldr W2<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>X1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>        <span style="color: #339933;">//</span> W2 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>X2 <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span>   <span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">32</span><span style="color: #339933;">-</span>bit load<span style="color: #009900; font-weight: bold;">]</span>
ldr W2<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>X1<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>       <span style="color: #339933;">//</span> W2 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>X2 <span style="color: #339933;">-</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span>   <span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">32</span><span style="color: #339933;">-</span>bit load<span style="color: #009900; font-weight: bold;">]</span>
ldr X2<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>X1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">240</span><span style="color: #009900; font-weight: bold;">]</span>      <span style="color: #339933;">//</span> X2 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>X2 <span style="color: #339933;">+</span> <span style="color: #ff0000;">240</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">64</span><span style="color: #339933;">-</span>bit load<span style="color: #009900; font-weight: bold;">]</span>
ldr X2<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>X1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">400</span><span style="color: #009900; font-weight: bold;">]</span>      <span style="color: #339933;">//</span> X2 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>X2 <span style="color: #339933;">+</span> <span style="color: #ff0000;">400</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">64</span><span style="color: #339933;">-</span>bit load<span style="color: #009900; font-weight: bold;">]</span>
<span style="color: #339933;">//</span> ldr X2<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>X1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">404</span><span style="color: #009900; font-weight: bold;">]</span>   <span style="color: #339933;">//</span> Invalid offset<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">not</span> multiple of <span style="color: #ff0000;">8</span>!
<span style="color: #339933;">//</span> ldr X2<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>X1<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">400</span><span style="color: #009900; font-weight: bold;">]</span>  <span style="color: #339933;">//</span> Invalid offset<span style="color: #339933;">,</span> must be positive!
<span style="color: #339933;">//</span> ldr X2<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>X1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">32768</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">//</span> Invalid offset<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">out</span> of the range!</pre></td></tr></tbody></table><p class="theCode" style="display:none;">ldr W2, [X1, #4]        // W2 ← *(X2 + 4)   [32-bit load]
ldr W2, [X1, #-4]       // W2 ← *(X2 - 4)   [32-bit load]
ldr X2, [X1, #240]      // X2 ← *(X2 + 240) [64-bit load]
ldr X2, [X1, #400]      // X2 ← *(X2 + 400) [64-bit load]
// ldr X2, [X1, #404]   // Invalid offset, not multiple of 8!
// ldr X2, [X1, #-400]  // Invalid offset, must be positive!
// ldr X2, [X1, #32768] // Invalid offset, out of the range!</p></div>

<h3>Base plus register offset</h3>
<p>
Being able to use an immediate offset is handy, but sometimes the offset cannot be encoded as an immediate or may be unknown before the program is run. For these cases a register can be<br>
used instead.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">ldr W1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>X2<span style="color: #339933;">,</span> X3<span style="color: #009900; font-weight: bold;">]</span>  <span style="color: #339933;">//</span> W1 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>X2 <span style="color: #339933;">+</span> X3<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">32</span><span style="color: #339933;">-</span>bit load<span style="color: #009900; font-weight: bold;">]</span>
ldr X1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>X2<span style="color: #339933;">,</span> X3<span style="color: #009900; font-weight: bold;">]</span>  <span style="color: #339933;">//</span> X1 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>X2 <span style="color: #339933;">+</span> X3<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">64</span><span style="color: #339933;">-</span>bit load<span style="color: #009900; font-weight: bold;">]</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">ldr W1, [X2, X3]  // W1 ← *(X2 + X3) [32-bit load]
ldr X1, [X2, X3]  // X1 ← *(X2 + X3) [64-bit load]</p></div>

<p>
It is possible to scale the value of the offset register, using a shifting operator <code>lsl</code>. This way <code>lsl #n</code> multiplies the offset by 2<sup>n</sup>.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">ldr W1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>X2<span style="color: #339933;">,</span> X3<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">lsl</span> #<span style="color: #ff0000;">3</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">//</span> W1 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>X2 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span>X3 &lt;&lt; <span style="color: #ff0000;">3</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">32</span><span style="color: #339933;">-</span>bit load<span style="color: #009900; font-weight: bold;">]</span>
                         <span style="color: #339933;">//</span> this is the same as
                         <span style="color: #339933;">//</span> W1 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>X2 <span style="color: #339933;">+</span> X3<span style="color: #339933;">*</span><span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span>      <span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">32</span><span style="color: #339933;">-</span>bit load<span style="color: #009900; font-weight: bold;">]</span>
ldr X1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>X2<span style="color: #339933;">,</span> X3<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">lsl</span> #<span style="color: #ff0000;">3</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">//</span> X1 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>X2 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span>X3 &lt;&lt; <span style="color: #ff0000;">3</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">64</span><span style="color: #339933;">-</span>bit load<span style="color: #009900; font-weight: bold;">]</span>
                         <span style="color: #339933;">//</span> this is the same as
                         <span style="color: #339933;">//</span> X1 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>X2 <span style="color: #339933;">+</span> X3<span style="color: #339933;">*</span><span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span>      <span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">64</span><span style="color: #339933;">-</span>bit load<span style="color: #009900; font-weight: bold;">]</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">ldr W1, [X2, X3, lsl #3] // W1 ← *(X2 + (X3 &lt;&lt; 3)) [32-bit load]
                         // this is the same as
                         // W1 ← *(X2 + X3*8)      [32-bit load]
ldr X1, [X2, X3, lsl #3] // X1 ← *(X2 + (X3 &lt;&lt; 3)) [64-bit load]
                         // this is the same as
                         // X1 ← *(X2 + X3*8)      [64-bit load]</p></div>

<p>
In contrast to the base register, the offset register can also be a 32-bit register but then we are forced to specify how we extend the 32-bit value to a 64-bit value. We have to use the extending operators we saw in chapter 3. Given that the source is a 32-bit value only <code>sxtw</code> and <code>uxtw</code> are allowed.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">ldr W1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>X2<span style="color: #339933;">,</span> W3<span style="color: #339933;">,</span> sxtw<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">//</span> W1 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>X2 <span style="color: #339933;">+</span> ExtendSigned32To64<span style="color: #009900; font-weight: bold;">(</span>W3<span style="color: #009900; font-weight: bold;">)</span><span style="color: #009900; font-weight: bold;">)</span>    <span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">32</span><span style="color: #339933;">-</span>bit load<span style="color: #009900; font-weight: bold;">]</span>
ldr W1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>X2<span style="color: #339933;">,</span> W3<span style="color: #339933;">,</span> uxtw<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">//</span> W1 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>X2 <span style="color: #339933;">+</span> ExtendUnsigned32To64<span style="color: #009900; font-weight: bold;">(</span>W3<span style="color: #009900; font-weight: bold;">)</span><span style="color: #009900; font-weight: bold;">)</span>  <span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">64</span><span style="color: #339933;">-</span>bit load<span style="color: #009900; font-weight: bold;">]</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">ldr W1, [X2, W3, sxtw] // W1 ← *(X2 + ExtendSigned32To64(W3))    [32-bit load]
ldr W1, [X2, W3, uxtw] // W1 ← *(X2 + ExtendUnsigned32To64(W3))  [64-bit load]</p></div>

<p>
As we already know, it is possible to combine an extending operator with a shift.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">ldr W1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>X2<span style="color: #339933;">,</span> W3<span style="color: #339933;">,</span> sxtw #<span style="color: #ff0000;">3</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">//</span> W1 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>X2 <span style="color: #339933;">+</span> ExtendSigned32To64<span style="color: #009900; font-weight: bold;">(</span>W3 &lt;&lt; <span style="color: #ff0000;">3</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">32</span><span style="color: #339933;">-</span>bit<span style="color: #339933;">-</span>load<span style="color: #009900; font-weight: bold;">]</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">ldr W1, [X2, W3, sxtw #3] // W1 ← *(X2 + ExtendSigned32To64(W3 &lt;&lt; 3)) [32-bit-load]</p></div>

<h2>Indexing modes</h2>
<p>
Sometimes it may happen that we are reading consecutive positions of memory in a way that we only care about the current element being read. In the worst of the cases, we can always compute the address using arithmetic operations in a register and then using a base indexed mode. Or, a bit better, keep the first address in a register that will act as the base and then compute the offset. If we take the latter approach most of the time our offset will be updated with a relatively simple operation like an addition or maybe an addition plus a shift (due to a multiplication with a power of two). We can exploit the fact that most of the time the address computation will have that shape. In these cases we may want to use an indexing mode.
</p>
<p>
There are two kinds of indexing modes in AArch64: pre-indexing and post-indexing. In a pre-indexing mode, the base register is added to an offset to compute the address, and then this address is written back to the base register. In a post-indexing mode, the base register is used to compute the address, as usual, but at the end of the memory access the base register is updated with the address value added to a given offset.
</p>
<p>
These two modes look pretty similar in that they update the base register with an offset. They differ at the point where the offset is accounted: a pre-indexing mode accounts it before the access, a post-indexing mode accounts it after the access. The offset we can use must be in the range -256 to 255.
</p>
<h3>Pre-index</h3>
<p>
The syntax for pre-index access mode is <code>[Xn, #offset]!</code>, mind the <code>!</code> symbol, otherwise you would be descriving a base plus offset without any indexing. In practice this acts pretty much like base plus offset but the <code>!</code> reminds us of the side effect of updating the base register.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">ldr X1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>X2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>! <span style="color: #339933;">//</span> X1 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>X2 <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span>
                  <span style="color: #339933;">//</span> X2 ← X2 <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">ldr X1, [X2, #4]! // X1 ← *(X2 + 4)
                  // X2 ← X2 + 4</p></div>

<h3>Post-index</h3>
<p>
The syntax is <code>[Xn], #offset</code>. It would be nice if there were a <code>!</code> after <code>#offset</code> in the syntax to get a visual cue similar to pre-indexing.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">ldr X1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>X2<span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>  <span style="color: #339933;">//</span> X1 ← <span style="color: #339933;">*</span>X2
                  <span style="color: #339933;">//</span> X2 ← X2 <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">ldr X1, [X2], #4  // X1 ← *X2
                  // X2 ← X2 + 4</p></div>

<h2>Load a literal address</h2>
<p>
Global objects, like global variables or functions, have addresses that are constant. This means that it should be possible to load them as literals. But as we know in AArch64 is not possible to load directly from a literal. So we need to use a two-step approach (which is by the way very common in RISC architectures). First we need to tell the assembler to put the address of the global variable in some position close to the current instruction. Then we load the address into a register using a special form of the load instruction (called load immediate).
</p>
<p>
In most of our examples it will look like this
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">ldr Xn<span style="color: #339933;">,</span> addr_of_var <span style="color: #339933;">//</span> Xn ← &amp;var
<span style="color: #339933;">...</span> 
addr_of_var <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">dword</span> variable <span style="color: #339933;">//</span> This tells the assembler that
                              <span style="color: #339933;">//</span> we want here the address of var
                              <span style="color: #339933;">//</span> <span style="color: #009900; font-weight: bold;">(</span>This is <span style="color: #00007f; font-weight: bold;">not</span> to be executed!<span style="color: #009900; font-weight: bold;">)</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">ldr Xn, addr_of_var // Xn ← &amp;var
... 
addr_of_var : .dword variable // This tells the assembler that
                              // we want here the address of var
                              // (This is not to be executed!)</p></div>

<p>
Once we have the address of the variable loaded in a register, we can do a second load to load the amount of bytes we want.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">ldr Xm<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>Xn<span style="color: #009900; font-weight: bold;">]</span>  <span style="color: #339933;">//</span> Xm ← <span style="color: #339933;">*</span>Xn    <span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">64</span><span style="color: #339933;">-</span>bit load<span style="color: #009900; font-weight: bold;">]</span>
ldr Wm<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>Xn<span style="color: #009900; font-weight: bold;">]</span>  <span style="color: #339933;">//</span> Wm ← <span style="color: #339933;">*</span>Xn    <span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">32</span><span style="color: #339933;">-</span>bit load<span style="color: #009900; font-weight: bold;">]</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">ldr Xm, [Xn]  // Xm ← *Xn    [64-bit load]
ldr Wm, [Xn]  // Wm ← *Xn    [32-bit load]</p></div>

<h3>Using 32-bit addresses</h3>
<p>
Using 64-bit addresses is correct but a bit wasteful. The reason is that many times our programs will not need more than 32-bits to encode all the addresses for code and data. The addresses of our global variables will always have the upper 32-bit as zero. So we may want to use 32-bit addresses instead.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">ldr Wn<span style="color: #339933;">,</span> addr_of_var <span style="color: #339933;">//</span> Wn ← &amp;var
<span style="color: #339933;">...</span> 
addr_of_var <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> variable <span style="color: #339933;">//</span> This tells the assembler that
                             <span style="color: #339933;">//</span> we want here the address of var
                             <span style="color: #339933;">//</span> <span style="color: #009900; font-weight: bold;">(</span>This is <span style="color: #00007f; font-weight: bold;">not</span> to be executed!<span style="color: #009900; font-weight: bold;">)</span>
                             <span style="color: #339933;">//</span> <span style="color: #ff0000;">32</span><span style="color: #339933;">-</span>bit address here</pre></td></tr></tbody></table><p class="theCode" style="display:none;">ldr Wn, addr_of_var // Wn ← &amp;var
... 
addr_of_var : .word variable // This tells the assembler that
                             // we want here the address of var
                             // (This is not to be executed!)
                             // 32-bit address here</p></div>

<p>
Recall that when writing a 32-bit value to a register, its upper 32-bits are cleared. So after the <code>ldr</code> above we can use <code>[Xn]</code> in a load or store without problems.
</p>
<h2>Global variables</h2>
<p>
As an example of today topic, we will load and store a couple of global variables. This program does not do anything useful.
</p>
<p>
Global variables are defined in the <code>.data</code> section. To do this, we just simply define their initial contents. If we want to define a 32-bit variable we will use the directive <code>.word</code>. If we want to define a 64-bit variable we use the directive <code>.dword</code>.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">//</span> globalvar<span style="color: #339933;">.</span>s
<span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
<span style="color: #339933;">.</span>balign <span style="color: #ff0000;">8</span> <span style="color: #339933;">//</span> <span style="color: #0000ff; font-weight: bold;">Align</span> to <span style="color: #ff0000;">8</span> bytes
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">byte</span> <span style="color: #ff0000;">1</span>
global_var64 <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">dword</span> <span style="color: #ff0000;">0x1234</span>  <span style="color: #339933;">//</span> a <span style="color: #ff0000;">64</span><span style="color: #339933;">-</span>bit value of <span style="color: #ff0000;">0x1234</span>
<span style="color: #339933;">//</span> alternatively<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">0x1234</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">0x0</span>
&nbsp;
<span style="color: #339933;">.</span>balign <span style="color: #ff0000;">4</span> <span style="color: #339933;">//</span> <span style="color: #0000ff; font-weight: bold;">Align</span> to <span style="color: #ff0000;">4</span> bytes
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">byte</span> <span style="color: #ff0000;">1</span>
global_var32 <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">0x5678</span>   <span style="color: #339933;">//</span> a <span style="color: #ff0000;">32</span><span style="color: #339933;">-</span>bit value of <span style="color: #ff0000;">0</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">// globalvar.s
.data

.balign 8 // Align to 8 bytes
.byte 1
global_var64 : .dword 0x1234  // a 64-bit value of 0x1234
// alternatively: .word 0x1234, 0x0

.balign 4 // Align to 4 bytes
.byte 1
global_var32 : .word 0x5678   // a 32-bit value of 0</p></div>

<p>
AArch64 in Linux does not require most memory accesses to be aligned. But if they are they can be executed faster by the hardware. So we use the directive <code>.balign</code> to align each variable to the size of the data (in bytes).
</p>
<p>
Now we can load the variables. For this example we will add 1 to each one.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>13
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
<span style="color: #339933;">.</span>globl main
main <span style="color: #339933;">:</span>
  ldr X0<span style="color: #339933;">,</span> address_of_global_var64 <span style="color: #339933;">//</span> X0 ← &amp;global_var64
  ldr X1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>X0<span style="color: #009900; font-weight: bold;">]</span>                    <span style="color: #339933;">//</span> X1 ← <span style="color: #339933;">*</span>X0
  <span style="color: #00007f; font-weight: bold;">add</span> X1<span style="color: #339933;">,</span> X1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>                  <span style="color: #339933;">//</span> X1 ← X1 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span>
  <span style="color: #00007f; font-weight: bold;">str</span> X1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>X0<span style="color: #009900; font-weight: bold;">]</span>                    <span style="color: #339933;">//</span> <span style="color: #339933;">*</span>X0 ← X1
&nbsp;
  ldr X0<span style="color: #339933;">,</span> address_of_global_var32 <span style="color: #339933;">//</span> X0 ← &amp;global_var32
  ldr W1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>X0<span style="color: #009900; font-weight: bold;">]</span>                    <span style="color: #339933;">//</span> W1 ← <span style="color: #339933;">*</span>X0
  <span style="color: #00007f; font-weight: bold;">add</span> W1<span style="color: #339933;">,</span> W1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>                  <span style="color: #339933;">//</span> W1 ← W1 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span>
  <span style="color: #00007f; font-weight: bold;">str</span> W1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>X0<span style="color: #009900; font-weight: bold;">]</span>                    <span style="color: #339933;">//</span> <span style="color: #339933;">*</span>X0 ← W1
&nbsp;
  <span style="color: #00007f; font-weight: bold;">mov</span> W0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>                      <span style="color: #339933;">//</span> W0 ← <span style="color: #ff0000;">0</span>
  <span style="color: #00007f; font-weight: bold;">ret</span>                             <span style="color: #339933;">//</span> exit program
address_of_global_var64 <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">dword</span> global_var64
address_of_global_var32 <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">dword</span> global_var32</pre></td></tr></tbody></table><p class="theCode" style="display:none;">.text

.globl main
main :
  ldr X0, address_of_global_var64 // X0 ← &amp;global_var64
  ldr X1, [X0]                    // X1 ← *X0
  add X1, X1, #1                  // X1 ← X1 + 1
  str X1, [X0]                    // *X0 ← X1

  ldr X0, address_of_global_var32 // X0 ← &amp;global_var32
  ldr W1, [X0]                    // W1 ← *X0
  add W1, W1, #1                  // W1 ← W1 + 1
  str W1, [X0]                    // *X0 ← W1

  mov W0, #0                      // W0 ← 0
  ret                             // exit program
address_of_global_var64 : .dword global_var64
address_of_global_var32 : .dword global_var32</p></div>

<h3>Using 32-bit addresses</h3>
<p>
As mentioned above, storing addresses of 64-bits to our variables is usually a bit wasteful. Here are the changes required to use 32-bit addresses instead.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>13
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
<span style="color: #339933;">.</span>globl main
main <span style="color: #339933;">:</span>
  ldr W0<span style="color: #339933;">,</span> address_of_global_var64 <span style="color: #339933;">//</span> W0 ← &amp;global_var64
  ldr X1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>X0<span style="color: #009900; font-weight: bold;">]</span>                    <span style="color: #339933;">//</span> X1 ← <span style="color: #339933;">*</span>X0
  <span style="color: #00007f; font-weight: bold;">add</span> X1<span style="color: #339933;">,</span> X1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>                  <span style="color: #339933;">//</span> X1 ← X1 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span>
  <span style="color: #00007f; font-weight: bold;">str</span> X1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>X0<span style="color: #009900; font-weight: bold;">]</span>                    <span style="color: #339933;">//</span> <span style="color: #339933;">*</span>X0 ← X1
&nbsp;
  ldr W0<span style="color: #339933;">,</span> address_of_global_var32 <span style="color: #339933;">//</span> W0 ← &amp;global_var32
  ldr W1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>X0<span style="color: #009900; font-weight: bold;">]</span>                    <span style="color: #339933;">//</span> W1 ← <span style="color: #339933;">*</span>X0
  <span style="color: #00007f; font-weight: bold;">add</span> W1<span style="color: #339933;">,</span> W1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>                  <span style="color: #339933;">//</span> W1 ← W1 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span>
  <span style="color: #00007f; font-weight: bold;">str</span> W1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>X0<span style="color: #009900; font-weight: bold;">]</span>                    <span style="color: #339933;">//</span> <span style="color: #339933;">*</span>X0 ← W1
&nbsp;
  <span style="color: #00007f; font-weight: bold;">mov</span> W0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>                      <span style="color: #339933;">//</span> W0 ← <span style="color: #ff0000;">0</span>
  <span style="color: #00007f; font-weight: bold;">ret</span>                             <span style="color: #339933;">//</span> exit program
address_of_global_var64 <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> global_var64 <span style="color: #339933;">//</span> note the usage of <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> here
address_of_global_var32 <span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> global_var32 <span style="color: #339933;">//</span> note the usage of <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> here</pre></td></tr></tbody></table><p class="theCode" style="display:none;">.text

.globl main
main :
  ldr W0, address_of_global_var64 // W0 ← &amp;global_var64
  ldr X1, [X0]                    // X1 ← *X0
  add X1, X1, #1                  // X1 ← X1 + 1
  str X1, [X0]                    // *X0 ← X1

  ldr W0, address_of_global_var32 // W0 ← &amp;global_var32
  ldr W1, [X0]                    // W1 ← *X0
  add W1, W1, #1                  // W1 ← W1 + 1
  str W1, [X0]                    // *X0 ← W1

  mov W0, #0                      // W0 ← 0
  ret                             // exit program
address_of_global_var64 : .word global_var64 // note the usage of .word here
address_of_global_var32 : .word global_var32 // note the usage of .word here</p></div>

<p>
There are other, slightly better ways, to get the address of global variables but this will be enough for now. Maybe in a later chapter we will revisit this.
</p>
<p>
This is all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2016/11/13/exploring-aarch64-assembler-chapter-5/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2016/11/13/exploring-aarch64-assembler-chapter-5/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2016/11/13/exploring-aarch64-assembler-chapter-5/&amp;text=Exploring+AArch64+assembler+%E2%80%93+Chapter+5+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2016/11/13/exploring-aarch64-assembler-chapter-5/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>						<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2016/10/30/arm-assembler-raspberry-pi-chapter-26/" rel="prev">ARM assembler in Raspberry Pi – Chapter 26</a></span>
			<span class="next"><a href="http://thinkingeek.com/2016/11/27/exploring-aarch64-assembler-chapter-6/" rel="next">Exploring AArch64 assembler – Chapter 6</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2016/11/13/exploring-aarch64-assembler-chapter-5/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="3572" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="892214a2ae"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496668919033"></form>
			</div><!-- #respond -->
	</div>	</div>