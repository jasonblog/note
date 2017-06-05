# Chapter 8


<div class="main">
			<h1>Exploring AArch64 assembler – Chapter 8 </h1>
		<p class="post-meta"><span class="date">May 29, 2017</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2017/05/29/exploring-aarch64-assembler-chapter-8/#respond">0</a></span> </p>
		<p>In the last chapter we saw how to call a function. We mentioned a special memory called the stack but we did not delve into it. Let’s see in this chapter how we can use the stack and why it is important in function calls.</p>
<p><span id="more-3925"></span></p>
<h2>Function calls</h2>
<p>
When we call a function, that same function can call other functions and so on. It is a bit like if we suspended the execution of a function to execute another one. The constraint here is that we always resume the execution when the called function ends. This is, we return always in the inverse order of the function calls. As a consequence, we can see the number of function calls <em>active</em> at some point as a stack (like a stack of plates).
</p>
<p>
This property is important because a function, say <code>F</code>, may need to keep some temporary memory. When <code>F</code> calls another function <code>G</code>, we want this memory be preserved when we return from <code>G</code>. Given that the <em>activations</em> of the functions follow a stack-like pattern, it seems natural that the temporary memory used by functions also follows this schema. This schema is so common that most architectures provide a specialized mechanism for this kind of «temporary memory associated to the activation of functions». That memory receives the name of <em>stack</em> basically because it follows a stack discipline: an element that is in the stack can only be removed when all the elements added after that first element have been removed.
</p>
<h2>The stack and convention call</h2>
<p>
Since the stack memory behaves like a stack, the only interesting thing we care about is its top element as in practice the stack is never empty. Since the stack is memory and memory is accessed using addresses, the top of the stack is an address. This address is stored in a special register called <code>sp</code> for <em>stack pointer</em>. Changing the value of <code>sp</code> changes the size of the stack. The whole stack memory ranges from the <em>stack base</em>, which is not kept anywhere and is usually conventional, and the stack pointer.
</p>
<p>
The stack has, then, two basic operations: it grows and it shrinks. Growing is done when we need to add new elements to the stack. Shrinking is done when we want to remove such elements. A function will typically grow the stack to keep temporary memory and it will shrink, by the same amount of elements, the stack before the function returns. This way, when returning from a call, the caller will have the stack as it was right before the call.
</p>
<p>
We have not specified how the grow and shrink operations are actually implemented. An architecture can decide make the stack grow towards higher addresses (and shrink towards lower addresses) or make the stack grow towards lower addresses (and, hence, shrink toward higher addresses). AArch64 chooses the latter, so to grow the stack we will just reduce the value in <code>sp</code> and to shrink it we will increase it.
</p>
<p><img data-attachment-id="3964" data-permalink="http://thinkingeek.com/2017/05/29/exploring-aarch64-assembler-chapter-8/stack-size/" data-orig-file="images/stack-size.png" data-orig-size="682,492" data-comments-opened="1" data-image-meta="{&quot;aperture&quot;:&quot;0&quot;,&quot;credit&quot;:&quot;&quot;,&quot;camera&quot;:&quot;&quot;,&quot;caption&quot;:&quot;&quot;,&quot;created_timestamp&quot;:&quot;0&quot;,&quot;copyright&quot;:&quot;&quot;,&quot;focal_length&quot;:&quot;0&quot;,&quot;iso&quot;:&quot;0&quot;,&quot;shutter_speed&quot;:&quot;0&quot;,&quot;title&quot;:&quot;&quot;,&quot;orientation&quot;:&quot;0&quot;}" data-image-title="stack-size" data-image-description="" data-medium-file="images/stack-size-300x216.png" data-large-file="images/stack-size.png" src="images/stack-size.png" alt="" width="682" height="492" class="aligncenter size-full wp-image-3964" srcset="images/stack-size.png 682w, images/stack-size-300x216.png 300w" sizes="(max-width: 682px) 100vw, 682px"></p>
<p>
The <a href="https://developer.arm.com/docs/ihi0055/latest/procedure-call-standard-for-the-arm-64-bit-architecture-aarch64">convention call of functions in AArch64</a> also dictates an additional constraint on the values that <code>sp</code> can take. Without too much details, at any point we use the <code>sp</code> (except for strictly growing or shrinking it) its value must be a multiple of 16. This means that the addresses kept in <code>sp</code> will be always aligned to 16-bytes.
</p>
<h2>Operating the stack</h2>
<p>
The operation of adding an element of the stack is commonly known as a <em>push</em>. A push does two things: it first grows the stack as many bytes as the size of the element and then does a store to the top of the stack. The inverse operation, removing something from the stack, is known as a <em>pop</em>. In this case first a load from the top is done to retrieve the value and then the stack is shrunk as many bytes as the size of the element removed. Recall that grow will mean subtracting the number of bytes from <code>sp</code> and shrinking it will mean adding the number of bytes to <code>sp</code>. Sometimes we do not want to retrieve the value, in this case we simply shrink the stack.
</p>
<p>
We could implement push and pop using a combination of <code>sub</code>/<code>str</code> and <code>add</code>/<code>ldr</code> instructions. And it could work but these operations happen very frequently so a combination of two instructions for each looks inefficient. Luckily we can cleverly use addressing modes to grow and shrink the stack at the same time that we perform a store or a load respectively.
</p>
<p>
If you recall chapter 5, we saw two addressing modes called pre-indexing and post-indexing. In these modes there is a base register plus an offset. In a pre-index mode the computed address is the offset added to the value of the base register. In a post-index mode the computed addres is just the value of the base register. Both modes update the base register with the value of the offset added to the value of the base register. These modes are useful when accessing contiguous memory, and the stack memory elements are a kind of contiguous memory.
</p>
<p>
With this insight, now we can implement a push using a store that uses pre-indexed mode with <code>sp</code> as the base register. This works because we want the address used for the stored be the new storage and then we want to update <code>sp</code> to point to that new memory. For instance, we can preserve the value of <code>x8</code> in the stack doing this:
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">//</span> Keep register x8 <span style="color: #00007f; font-weight: bold;">in</span> the <span style="color: #0000ff; font-weight: bold;">stack</span>
<span style="color: #00007f; font-weight: bold;">str</span> x8<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span>!  <span style="color: #339933;">//</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">-</span> <span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span> ← x8
                    <span style="color: #339933;">//</span> <span style="color: #46aa03; font-weight: bold;">sp</span> ← <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">-</span> <span style="color: #ff0000;">8</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">// Keep register x8 in the stack
str x8, [sp, #-8]!  // *(sp - 8) ← x8
                    // sp ← sp - 8</p></div>

<p>
Note that we use an offset of 8 because <code>x8</code> is a 64-bit register. The offset is negative because in AArch64 the convention tells us to use a downwards growing stack.
</p>
<p>
A pop, conversely, is implemented using a pre-indexed address mode. For instance, we can restore the preserved value of <code>x8</code> doing this:
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">//</span> Restore register x8
ldr x8<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span>   <span style="color: #339933;">//</span> x8 ← <span style="color: #339933;">*</span><span style="color: #46aa03; font-weight: bold;">sp</span>
                   <span style="color: #339933;">//</span> <span style="color: #46aa03; font-weight: bold;">sp</span> ← <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">// Restore register x8
ldr x8, [sp], #8   // x8 ← *sp
                   // sp ← sp + 8</p></div>

<p>
Note that the offset in this case is positive, because now we are shrinking the stack.
</p>
<p>
Well, this could work but we’re not quite there yet. The reason is that the convention tells us to keep the stack aligned to a multiple of 16. Assuming it was originally like this, just doing a single <code>ldr</code>/<code>str</code> will break this property very easily. One option is making sure the stack is still aligned by using additional <code>add</code>/<code>sub</code> instructions. Another option is making sure we push and pop pairs of 64-bit registers. As each register takes 8-bytes, two of them will take obviously 16-bytes. If we push in pairs the stack remains aligned in a single instruction. To do this, AArch64 provides special load/store pair instructions called <code>ldp</code> and <code>stp</code>. These instruction receive two registers and a single addressing mode. The registers are loaded/stored as consecutive elements of the address computed by the addressing mode.
</p>
<p>
For instance, the following sequence:
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #00007f; font-weight: bold;">str</span> x11<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span>!  <span style="color: #339933;">//</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">-</span> <span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span> ← x11
                     <span style="color: #339933;">//</span> <span style="color: #46aa03; font-weight: bold;">sp</span> ← <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">-</span> <span style="color: #ff0000;">8</span>
<span style="color: #00007f; font-weight: bold;">str</span> x8<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span>!   <span style="color: #339933;">//</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">-</span> <span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span> ← x8
                     <span style="color: #339933;">//</span> <span style="color: #46aa03; font-weight: bold;">sp</span> ← <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">-</span> <span style="color: #ff0000;">8</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">str x11, [sp, #-8]!  // *(sp - 8) ← x11
                     // sp ← sp - 8
str x8, [sp, #-8]!   // *(sp - 8) ← x8
                     // sp ← sp - 8</p></div>

<p>
can be rewritten as
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">stp x8<span style="color: #339933;">,</span> x11<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">]</span>!  <span style="color: #339933;">//</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">-</span> <span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">)</span> ← x8
                          <span style="color: #339933;">//</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">-</span> <span style="color: #ff0000;">16</span> <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span> ← x11
                          <span style="color: #339933;">//</span> <span style="color: #46aa03; font-weight: bold;">sp</span> ← <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">-</span> <span style="color: #ff0000;">16</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">stp x8, x11, [sp, #-16]!  // *(sp - 16) ← x8
                          // *(sp - 16 + 8) ← x11
                          // sp ← sp - 16</p></div>

<p>
As you can see the first register in the instruction will be the one in the top of the stack. The second register is just stored contiguously after that (towards the bottom of the stack). A similar thing happens with the corresponding <code>ldp</code> implementing a pop of these two registers.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">ldp x8<span style="color: #339933;">,</span> x11<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">16</span>  <span style="color: #339933;">//</span> x8 ← <span style="color: #339933;">*</span><span style="color: #46aa03; font-weight: bold;">sp</span>
                        <span style="color: #339933;">//</span> x11 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span>
                        <span style="color: #339933;">//</span> <span style="color: #46aa03; font-weight: bold;">sp</span> ← <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">+</span> <span style="color: #ff0000;">16</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">ldp x8, x11, [sp], #16  // x8 ← *sp
                        // x11 ← *(sp + 8)
                        // sp ← sp + 16</p></div>

<p>
As long as we use the same order of registers inside <code>ldp</code> and <code>stp</code> we are fine. Note that this is a stack, so the first elements we put must be the last to leave. This means that if we want to keep and restore, say, <code>x8</code>, <code>x9</code>, <code>x10</code> and <code>x11</code>, the right order is the following.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">//</span> <span style="color: #00007f; font-weight: bold;">push</span> x8<span style="color: #339933;">,</span> x9
stp x8<span style="color: #339933;">,</span> x9<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">]</span>!    <span style="color: #339933;">//</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">-</span> <span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">)</span> ← x8
                           <span style="color: #339933;">//</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">-</span> <span style="color: #ff0000;">16</span> <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span> ← x9
                           <span style="color: #339933;">//</span> <span style="color: #46aa03; font-weight: bold;">sp</span> ← <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">-</span> <span style="color: #ff0000;">16</span>
<span style="color: #339933;">//</span> <span style="color: #00007f; font-weight: bold;">push</span> x10<span style="color: #339933;">,</span> x11
stp x10<span style="color: #339933;">,</span> x11<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">]</span>!  <span style="color: #339933;">//</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">-</span> <span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">)</span> ← x10
                           <span style="color: #339933;">//</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">-</span> <span style="color: #ff0000;">16</span> <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span> ← x11
                           <span style="color: #339933;">//</span> <span style="color: #46aa03; font-weight: bold;">sp</span> ← <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">-</span> <span style="color: #ff0000;">16</span>
<span style="color: #339933;">...</span>
<span style="color: #339933;">//</span> <span style="color: #00007f; font-weight: bold;">pop</span> x10<span style="color: #339933;">,</span> x11
ldp x10<span style="color: #339933;">,</span> x11<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">16</span>    <span style="color: #339933;">//</span> x10 ← <span style="color: #339933;">*</span><span style="color: #46aa03; font-weight: bold;">sp</span>
                           <span style="color: #339933;">//</span> x11 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span>
                           <span style="color: #339933;">//</span> <span style="color: #46aa03; font-weight: bold;">sp</span> ← <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">+</span> <span style="color: #ff0000;">16</span>
<span style="color: #339933;">//</span> <span style="color: #00007f; font-weight: bold;">pop</span> x8<span style="color: #339933;">,</span> x9
ldp x8<span style="color: #339933;">,</span> x9<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">16</span>     <span style="color: #339933;">//</span> x8 ← <span style="color: #339933;">*</span><span style="color: #46aa03; font-weight: bold;">sp</span>
                           <span style="color: #339933;">//</span> x9 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span><span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span>
                           <span style="color: #339933;">//</span> <span style="color: #46aa03; font-weight: bold;">sp</span> ← <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">+</span> <span style="color: #ff0000;">16</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">// push x8, x9
stp x8, x9, [sp, #-16]!    // *(sp - 16) ← x8
                           // *(sp - 16 + 8) ← x9
                           // sp ← sp - 16
// push x10, x11
stp x10, x11, [sp, #-16]!  // *(sp - 16) ← x10
                           // *(sp - 16 + 8) ← x11
                           // sp ← sp - 16
...
// pop x10, x11
ldp x10, x11, [sp], #16    // x10 ← *sp
                           // x11 ← *(sp + 8)
                           // sp ← sp + 16
// pop x8, x9
ldp x8, x9, [sp], #16     // x8 ← *sp
                           // x9 ← *(sp + 8)
                           // sp ← sp + 16</p></div>

<p><img data-attachment-id="3973" data-permalink="http://thinkingeek.com/2017/05/29/exploring-aarch64-assembler-chapter-8/stack-operation/" data-orig-file="images/stack-operation.png" data-orig-size="330,486" data-comments-opened="1" data-image-meta="{&quot;aperture&quot;:&quot;0&quot;,&quot;credit&quot;:&quot;&quot;,&quot;camera&quot;:&quot;&quot;,&quot;caption&quot;:&quot;&quot;,&quot;created_timestamp&quot;:&quot;0&quot;,&quot;copyright&quot;:&quot;&quot;,&quot;focal_length&quot;:&quot;0&quot;,&quot;iso&quot;:&quot;0&quot;,&quot;shutter_speed&quot;:&quot;0&quot;,&quot;title&quot;:&quot;&quot;,&quot;orientation&quot;:&quot;0&quot;}" data-image-title="stack-operation" data-image-description="" data-medium-file="images/stack-operation-204x300.png" data-large-file="images/stack-operation.png" src="images/stack-operation.png" alt="" width="330" height="486" class="aligncenter size-full wp-image-3973" srcset="images/stack-operation.png 330w, images/stack-operation-204x300.png 204w" sizes="(max-width: 330px) 100vw, 330px"></p>
<p>
So basically what is pushed the first is popped the last. Or similarly: the order of pops must be the opposite as the order of pushes.
</p>
<h2>Fibonacci</h2>
<p>
The archetypical example when explaining recursion is computing the Fibonacci numbers doing a direct translation of the formula. This is not an efficient way to compute such numbers (in the rare case you actually need them) but makes for a simple example involving recursion. Recursion to work many times requires a stack (except for a subset of specially recursive functions that don’t) so this will showcase how to manipulate it.
</p>
<p>
We will write a program that will ask for a number <code>n</code> and will compute the <code>n</code>-th Fibonacci number. Let’s write the <code>main</code> function first.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
msg_input<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"Please type a number: "</span>
scanf_fmt <span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"%d"</span>
msg_output<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"Fibonacci number %d is %ld\n"</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">global</span> main
main<span style="color: #339933;">:</span>
    stp x19<span style="color: #339933;">,</span> x30<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">]</span>! <span style="color: #339933;">//</span> Keep x19 <span style="color: #00007f; font-weight: bold;">and</span> x30 <span style="color: #009900; font-weight: bold;">(</span>link register<span style="color: #009900; font-weight: bold;">)</span>
    <span style="color: #00007f; font-weight: bold;">sub</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">16</span>           <span style="color: #339933;">//</span> Grow the <span style="color: #0000ff; font-weight: bold;">stack</span> because for a local
                              <span style="color: #339933;">//</span> variable used by scanf<span style="color: #339933;">.</span>
    <span style="color: #339933;">/*</span>
      Our <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #0000ff; font-weight: bold;">at</span> this point will look like this
      Contents  Address
      | var |    <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #009900; font-weight: bold;">]</span>       We will use the first <span style="color: #ff0000;">4</span> bytes for scanf
      |     |    <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span>
      | x19 |    <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">]</span>
      | x30 |    <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">24</span><span style="color: #009900; font-weight: bold;">]</span>
     <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">//</span> Set up first <span style="color: #00007f; font-weight: bold;">call</span> to printf
    <span style="color: #339933;">//</span>   printf<span style="color: #009900; font-weight: bold;">(</span><span style="color: #7f007f;">"Please type a number: "</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #666666; font-style: italic;">;</span>
    ldr w0<span style="color: #339933;">,</span> addr_msg_input   <span style="color: #339933;">//</span> w0 ← &amp;msg_input  <span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">32</span><span style="color: #339933;">-</span>bit<span style="color: #009900; font-weight: bold;">]</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> printf                <span style="color: #339933;">//</span> <span style="color: #00007f; font-weight: bold;">call</span> printf
&nbsp;
    <span style="color: #339933;">//</span> Set up <span style="color: #00007f; font-weight: bold;">call</span> to scanf
    <span style="color: #339933;">//</span>   scanf<span style="color: #009900; font-weight: bold;">(</span><span style="color: #7f007f;">"%d"</span><span style="color: #339933;">,</span> &amp;var<span style="color: #009900; font-weight: bold;">)</span><span style="color: #666666; font-style: italic;">;</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> x1<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span>               <span style="color: #339933;">//</span> x1 ← <span style="color: #46aa03; font-weight: bold;">sp</span>
                             <span style="color: #339933;">//</span> the first <span style="color: #ff0000;">4</span> bytes pointed by <span style="color: #46aa03; font-weight: bold;">sp</span> will be <span style="color: #7f007f;">'var'</span>
    ldr w0<span style="color: #339933;">,</span> addr_scanf_fmt   <span style="color: #339933;">//</span> w0 ← &amp;scanf_fmt  <span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">32</span><span style="color: #339933;">-</span>bit<span style="color: #009900; font-weight: bold;">]</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> scanf                 <span style="color: #339933;">//</span> <span style="color: #00007f; font-weight: bold;">call</span> scanf
&nbsp;
    <span style="color: #339933;">//</span> Set up <span style="color: #00007f; font-weight: bold;">call</span> to fibonacci
    <span style="color: #339933;">//</span>   res = fibonacci<span style="color: #009900; font-weight: bold;">(</span>var<span style="color: #009900; font-weight: bold;">)</span><span style="color: #666666; font-style: italic;">;</span>
    ldr w0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #009900; font-weight: bold;">]</span>             <span style="color: #339933;">//</span> w0 ← <span style="color: #339933;">*</span><span style="color: #46aa03; font-weight: bold;">sp</span>   <span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">32</span><span style="color: #339933;">-</span>bit<span style="color: #009900; font-weight: bold;">]</span>
                             <span style="color: #339933;">//</span> this is var <span style="color: #00007f; font-weight: bold;">in</span> the <span style="color: #0000ff; font-weight: bold;">stack</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> fibonacci             <span style="color: #339933;">//</span> <span style="color: #00007f; font-weight: bold;">call</span> fibonacci
&nbsp;
    <span style="color: #339933;">//</span> Setup <span style="color: #00007f; font-weight: bold;">call</span> to printf
    <span style="color: #339933;">//</span>   printf<span style="color: #009900; font-weight: bold;">(</span><span style="color: #7f007f;">"Fibonacci number %d is %ld\n"</span><span style="color: #339933;">,</span> var<span style="color: #339933;">,</span> res<span style="color: #009900; font-weight: bold;">)</span><span style="color: #666666; font-style: italic;">;</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> x2<span style="color: #339933;">,</span> x0               <span style="color: #339933;">//</span> x2 ← x0
                             <span style="color: #339933;">//</span> this is <span style="color: #7f007f;">'res'</span> <span style="color: #00007f; font-weight: bold;">in</span> the <span style="color: #00007f; font-weight: bold;">call</span> to fibonacci
    ldr w1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #009900; font-weight: bold;">]</span>             <span style="color: #339933;">//</span> w1 ← <span style="color: #339933;">*</span><span style="color: #46aa03; font-weight: bold;">sp</span>    <span style="color: #009900; font-weight: bold;">[</span><span style="color: #ff0000;">32</span><span style="color: #339933;">-</span>bit<span style="color: #009900; font-weight: bold;">]</span>
    ldr w0<span style="color: #339933;">,</span> addr_msg_output  <span style="color: #339933;">//</span> w0 ← &amp;msg_output
    <span style="color: #46aa03; font-weight: bold;">bl</span> printf                <span style="color: #339933;">//</span> <span style="color: #00007f; font-weight: bold;">call</span> printf
&nbsp;
    <span style="color: #00007f; font-weight: bold;">add</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">16</span>          <span style="color: #339933;">//</span> Shrink the <span style="color: #0000ff; font-weight: bold;">stack</span><span style="color: #339933;">.</span>
    ldp x19<span style="color: #339933;">,</span> x30<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">16</span>  <span style="color: #339933;">//</span> Restore x19 <span style="color: #00007f; font-weight: bold;">and</span> x30 <span style="color: #009900; font-weight: bold;">(</span>link register<span style="color: #009900; font-weight: bold;">)</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> w0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>               <span style="color: #339933;">//</span> w0 ← <span style="color: #ff0000;">0</span>
    <span style="color: #00007f; font-weight: bold;">ret</span>                      <span style="color: #339933;">//</span> <span style="color: #00007f; font-weight: bold;">Leave</span> the function
&nbsp;
addr_msg_input<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> msg_input
addr_msg_output<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> msg_output
addr_scanf_fmt<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> scanf_fmt</pre></td></tr></tbody></table><p class="theCode" style="display:none;">.data

msg_input: .asciz "Please type a number: "
scanf_fmt : .asciz "%d"
msg_output: .asciz "Fibonacci number %d is %ld\n"

.text

.global main
main:
    stp x19, x30, [sp, #-16]! // Keep x19 and x30 (link register)
    sub sp, sp, #16           // Grow the stack because for a local
                              // variable used by scanf.
    /*
      Our stack at this point will look like this
      Contents  Address
      | var |    [sp]       We will use the first 4 bytes for scanf
      |     |    [sp, #8]
      | x19 |    [sp, #16]
      | x30 |    [sp, #24]
     */

    // Set up first call to printf
    //   printf("Please type a number: ");
    ldr w0, addr_msg_input   // w0 ← &amp;msg_input  [32-bit]
    bl printf                // call printf

    // Set up call to scanf
    //   scanf("%d", &amp;var);
    mov x1, sp               // x1 ← sp
                             // the first 4 bytes pointed by sp will be 'var'
    ldr w0, addr_scanf_fmt   // w0 ← &amp;scanf_fmt  [32-bit]
    bl scanf                 // call scanf

    // Set up call to fibonacci
    //   res = fibonacci(var);
    ldr w0, [sp]             // w0 ← *sp   [32-bit]
                             // this is var in the stack
    bl fibonacci             // call fibonacci

    // Setup call to printf
    //   printf("Fibonacci number %d is %ld\n", var, res);
    mov x2, x0               // x2 ← x0
                             // this is 'res' in the call to fibonacci
    ldr w1, [sp]             // w1 ← *sp    [32-bit]
    ldr w0, addr_msg_output  // w0 ← &amp;msg_output
    bl printf                // call printf

    add sp, sp, #16          // Shrink the stack.
    ldp x19, x30, [sp], #16  // Restore x19 and x30 (link register)
    mov w0, #0               // w0 ← 0
    ret                      // Leave the function

addr_msg_input: .word msg_input
addr_msg_output: .word msg_output
addr_scanf_fmt: .word scanf_fmt</p></div>

<p>
The main program first asks the user for a number using <code>printf</code>, reads a 32-bit integer from the input using scanf. With that number we call <code>fibonacci</code> (we will see its code later) and then just print the result for the given number, again using <code>printf</code>.
</p>
<p>
In lines 3 to 5 we define a few strings that we will need for the <code>printf</code> and <code>scanf</code> calls. The directive .asciz means “emit these characters as ASCII bytes and add a zero byte at the end”. The zero byte is required by C routines that use it to tell where the string ends. The marker <code>%d</code> in <code>scanf_fmt</code> means “read an integer and store its value as a 32-bit signed number in the given address” (you will see this in the call of scanf below). The markers <code>%d</code> and <code>%ld</code> in <code>msg_output</code> mean “print an integer as a decimal number of 32-bit/64-bit” respectively.
</p>
<p>
As this function calls another function it has top keep the value of <code>x30</code>, line 11. Recall that executing a <code>bl</code> instruction will change <code>x30</code> to be the address of the instruction after that <code>bl</code>. Also, <code>ret</code> instruction uses <code>x30</code> to know where to return. So if our function calls another function it <strong>must keep</strong> <code>x30</code>. As we have to keep the stack aligned to 16-bytes, storing <code>x30</code> is not enough so we will also keep another register even if we do not use it. Conventionally we will use <code>x19</code> as it is the first callee-saved register
</p>
<p>
As mentioned above, <code>scanf</code> reads an integer from the input and stores it in some memory. We could use a global variable for that but we can also use the stack, but first we need to make room in the stack. So we grow it by 16-bytes, line 12. Actually we only need 4 bytes but recall, the stack must be kept 16-byte aligned, so yes, we will waste 12 bytes in this case.
</p>
<p>
Then we do the first call to <code>printf</code>, lines 25 to 26. This call only receives a parameter, which is the address of the string <code>msg_input</code>. So we simply load the address to <code>w0</code> and then <code>bl</code> to <code>printf</code>.
</p>
<p>
Now we do the call to <code>scanf</code>. This call receives first the format of the input read (the string <code>%d</code> that we have in <code>scanf_fmt</code>) and the address of the memory where we will store the result of the scanf. The first parameter is the address of <code>scanf_fmt</code> so we load it in <code>w0</code>, line 32. The second parameter is the address where we want <code>scanf</code> to store the read integer. In this case we will use the first 4-bytes in the address pointed by <code>sp</code>. So we simply copy the value of <code>sp</code> in <code>x1</code> using <code>mov</code>, line 30. Now with everything in place we can do the call to <code>scanf</code>, line 33. Note: the order in which we set up the registers for parameters is usually not very relevant (except for those cases where it might make things easier, of course).
</p>
<p>
Now after <code>scanf</code> has returned in the top of the stack, pointed by <code>sp</code>, we have an integer that we can pass to <code>fibonacci</code>. So we load it, line 37, in <code>w0</code>. And we’re now ready for the call to <code>fibonacci</code>, line 39.
</p>
<p>
Our <code>fibonacci</code> function receives a 32-bit integer as a parameter and returns a 64-bit integer as the result. The calling convention of AArch64 says that for the parameter we have to use <code>w0</code> and return it in <code>x0</code>. So to prepare the call to the final <code>printf</code>, we have to make sure our 64-bit number is in the right register, so we copy <code>x0</code> (set by the fibonacci call) to <code>x2</code>, line 43, and then we load again from the stack the value we passed to fibonacci, but this time we load it into <code>w1</code>, line 45. Lastly we load into <code>w0</code> the address of <code>msg_output</code>. Now we can call <code>printf</code> to show the results of our computation.
</p>
<p>
Now the only thing that remains to do in this function is to do clean up. We shrink the stack that we grew for the local variable, line 49, and then we restore the values of <code>x19</code> and <code>x30</code> (the link register, which was modified by the <code>bl</code> instructions), line 50. Now we can return, line 52, but since this is the main and returns an integer, we just make sure <code>w0</code> is set to zero just before returning, line 51.
</p>
<p>
Now we can see the code of the fibonacci function.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>59
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
74
75
76
77
78
79
80
81
82
83
84
85
86
87
88
89
90
91
92
93
94
95
96
97
98
99
100
101
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">fibonacci<span style="color: #339933;">:</span>
    <span style="color: #339933;">//</span> fibonacci<span style="color: #009900; font-weight: bold;">(</span>n<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">-</span>&gt; result
    <span style="color: #339933;">//</span>   n is <span style="color: #ff0000;">32</span><span style="color: #339933;">-</span>bit <span style="color: #00007f; font-weight: bold;">and</span> will be passed <span style="color: #00007f; font-weight: bold;">in</span> w0
    <span style="color: #339933;">//</span>   result is <span style="color: #ff0000;">64</span><span style="color: #339933;">-</span>bit <span style="color: #00007f; font-weight: bold;">and</span> will be returned <span style="color: #00007f; font-weight: bold;">in</span> x0
&nbsp;
    stp x19<span style="color: #339933;">,</span> x30<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">]</span>!  <span style="color: #339933;">//</span> Keep x19 <span style="color: #00007f; font-weight: bold;">and</span> x30 <span style="color: #009900; font-weight: bold;">(</span>link register<span style="color: #009900; font-weight: bold;">)</span>
    stp x20<span style="color: #339933;">,</span> x21<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">]</span>!  <span style="color: #339933;">//</span> Keep x20 <span style="color: #00007f; font-weight: bold;">and</span> x21
    <span style="color: #339933;">/*</span>
      Our <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #0000ff; font-weight: bold;">at</span> this point will look like this
      Contents  Address
      | x20 |    <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #009900; font-weight: bold;">]</span>
      | x21 |    <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span>
      | x19 |    <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">]</span>
      | x30 |    <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">24</span><span style="color: #009900; font-weight: bold;">]</span>
     <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">cmp</span> w0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>                <span style="color: #339933;">//</span> Compare w0 with <span style="color: #ff0000;">1</span> <span style="color: #00007f; font-weight: bold;">and</span> update the flags
    ble simple_case           <span style="color: #339933;">//</span> if w0 &lt;= <span style="color: #ff0000;">1</span> branch to simple_case
                              <span style="color: #339933;">//</span> <span style="color: #009900; font-weight: bold;">(</span>otherwise continue to recursive_case<span style="color: #009900; font-weight: bold;">)</span>
&nbsp;
    recursive_case<span style="color: #339933;">:</span>           <span style="color: #339933;">//</span> recursive case
                              <span style="color: #339933;">//</span> <span style="color: #009900; font-weight: bold;">(</span>this label is <span style="color: #00007f; font-weight: bold;">not</span> used<span style="color: #339933;">,</span> added for clarity<span style="color: #009900; font-weight: bold;">)</span>
      <span style="color: #00007f; font-weight: bold;">mov</span> w19<span style="color: #339933;">,</span> w0             <span style="color: #339933;">//</span> w19 ← w0
      <span style="color: #339933;">//</span> Set up <span style="color: #00007f; font-weight: bold;">call</span> to fibonacci
      <span style="color: #339933;">//</span>     fibonacci<span style="color: #009900; font-weight: bold;">(</span>n<span style="color: #339933;">-</span><span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #666666; font-style: italic;">;</span>
      <span style="color: #00007f; font-weight: bold;">sub</span> w0<span style="color: #339933;">,</span> w0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>          <span style="color: #339933;">//</span> w0 ← w0 <span style="color: #339933;">-</span> <span style="color: #ff0000;">1</span>
      <span style="color: #46aa03; font-weight: bold;">bl</span> fibonacci            <span style="color: #339933;">//</span> <span style="color: #00007f; font-weight: bold;">call</span> fibonacci
      <span style="color: #00007f; font-weight: bold;">mov</span> x20<span style="color: #339933;">,</span> x0             <span style="color: #339933;">//</span> x20 ← x0
&nbsp;
      <span style="color: #00007f; font-weight: bold;">sub</span> w0<span style="color: #339933;">,</span> w19<span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span>         <span style="color: #339933;">//</span> w0 ← w19 <span style="color: #339933;">-</span> <span style="color: #ff0000;">2</span>
      <span style="color: #46aa03; font-weight: bold;">bl</span> fibonacci            <span style="color: #339933;">//</span> <span style="color: #00007f; font-weight: bold;">call</span> fibonacci
      <span style="color: #00007f; font-weight: bold;">mov</span> x21<span style="color: #339933;">,</span> x0             <span style="color: #339933;">//</span> x21 ← x0
&nbsp;
      <span style="color: #00007f; font-weight: bold;">add</span> x0<span style="color: #339933;">,</span> x20<span style="color: #339933;">,</span> x21        <span style="color: #339933;">//</span> x0 ← x20 <span style="color: #339933;">+</span> x21
      b end                   <span style="color: #339933;">//</span> <span style="color: #009900; font-weight: bold;">(</span>unconditional<span style="color: #009900; font-weight: bold;">)</span> branch to end
&nbsp;
    simple_case<span style="color: #339933;">:</span>
      sxtw x0<span style="color: #339933;">,</span> w0             <span style="color: #339933;">//</span> x0 ← ExtendSigned32To64<span style="color: #009900; font-weight: bold;">(</span>w0<span style="color: #009900; font-weight: bold;">)</span>
&nbsp;
    end<span style="color: #339933;">:</span>
      ldp x20<span style="color: #339933;">,</span> x21<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">16</span>  <span style="color: #339933;">//</span> Restore x20 <span style="color: #00007f; font-weight: bold;">and</span> x21
      ldp x19<span style="color: #339933;">,</span> x30<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">16</span>  <span style="color: #339933;">//</span> Restore x19 <span style="color: #00007f; font-weight: bold;">and</span> x30 <span style="color: #009900; font-weight: bold;">(</span>link register<span style="color: #009900; font-weight: bold;">)</span>
    <span style="color: #00007f; font-weight: bold;">ret</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">fibonacci:
    // fibonacci(n) -&gt; result
    //   n is 32-bit and will be passed in w0
    //   result is 64-bit and will be returned in x0

    stp x19, x30, [sp, #-16]!  // Keep x19 and x30 (link register)
    stp x20, x21, [sp, #-16]!  // Keep x20 and x21
    /*
      Our stack at this point will look like this
      Contents  Address
      | x20 |    [sp]
      | x21 |    [sp, #8]
      | x19 |    [sp, #16]
      | x30 |    [sp, #24]
     */

    cmp w0, #1                // Compare w0 with 1 and update the flags
    ble simple_case           // if w0 &lt;= 1 branch to simple_case
                              // (otherwise continue to recursive_case)

    recursive_case:           // recursive case
                              // (this label is not used, added for clarity)
      mov w19, w0             // w19 ← w0
      // Set up call to fibonacci
      //     fibonacci(n-1);
      sub w0, w0, #1          // w0 ← w0 - 1
      bl fibonacci            // call fibonacci
      mov x20, x0             // x20 ← x0

      sub w0, w19, #2         // w0 ← w19 - 2
      bl fibonacci            // call fibonacci
      mov x21, x0             // x21 ← x0
 
      add x0, x20, x21        // x0 ← x20 + x21
      b end                   // (unconditional) branch to end

    simple_case:
      sxtw x0, w0             // x0 ← ExtendSigned32To64(w0)

    end:
      ldp x20, x21, [sp], #16  // Restore x20 and x21
      ldp x19, x30, [sp], #16  // Restore x19 and x30 (link register)
    ret</p></div>

<p>
Similar to the main function we first start by keeping all that must be restored upon leaving the function. So we keep <code>x19</code>, <code>x30</code> and <code>x20</code> and <code>x21</code>, lines 64-65. In contrast to the <code>main</code> function, where <code>x19</code> was not used but we kept it and restored it to keep the stack 16-byte aligned, we will use all the preserved registers this time.
</p>
<p>
The fibonacci function is actually a sequence of numbers F<sub>i</sub> defined by the following recurrence:
</p>
<ul>
<li>F<sub>0</sub> = 0
</li><li>F<sub>1</sub> = 1
</li><li>F<sub>n</sub> = F<sub>n-1</sub> + F<sub>n-2</sub>, where n &gt; 1
</li></ul>
<p>
This means that <code>fibonacci(0)</code> and <code>fibonacci(1)</code> just return the parameter 0 and 1 respectively, the simple case. Otherwise <code>fibonacci(n)</code> has to add <code>fibonacci(n-1)</code> and <code>fibonacci(n-2)</code> to compute the result.
</p>
<p>
In order to tell whether this is the simple case or not, we just compare <code>w0</code> with 1, line 75. If <code>w0 ≤ 1</code> then this is the simple case and we branch to it, line 96. In the simple case we simply extend <code>w0</code> from 32 to 64 bits, line 96. The instruction <code>sxtw</code> is in practice like a move that also extends (and the name of the instruction is deliberately the same as the extending operators we saw in chapter 3).
</p>
<p>
For the recursive case, we first need to make sure we will not lose the value of <code>w0</code> because this is a caller-saved register meaning that its content will be lost after a function call. So we keep it in register <code>w19</code>, line 81, which is a callee saved register, and we already preserved it at the beginning of the function. That said we have not lost the value of <code>x0</code> already, so we can still use it to compute the parameter for <code>fibonacci(n-1)</code>, so we subtract 1 from <code>x0</code>, line 84. Then we do the call, line 85. After the call we want to gather the result, so we will keep it in a callee-saved register, this time <code>x20</code>, line 86.
</p>
<p>
For the second call to fibonacci, the original value of x0 from which we could compute n-2 has been lost already. However we kept in <code>w19</code> so we can compute <code>n-2</code> and store it in <code>w0</code>, line 88. Now we call <code>fibonacci</code>, line 89, and similarly we keep the result in another callee-saved register, this time <code>x21</code>, line 90.
</p>
<p>
Finally we compute the result of this fibonacci as the sum of <code>x20</code>, which contains the value of <code>fibonacci(n-1)</code>, and <code>x21</code>, which contains the value of <code>fibonacci(n-2)</code>, line 92. Note that we could have coalesced lines 90 and 92 in a single <code>add x0, x0, x20</code>, but I did in two steps for clarity.
</p>
<p>
Since we do not want to run the simple case, we just branch to the end of the function, line 93. Like any other function, clean up must be in order, so we restore the registers we kept at the beginning, lines 99 and 100.
</p>
<p>
OK, let’s try our program.
</p>
<pre escaped="1">$ ./fibo 
Please type a number: <span style="color: blue;">12↴</span>
Fibonacci number 12 is 144
$ ./fibo 
Please type a number: <span style="color: blue;">25↴</span>
Fibonacci number 25 is 75025
$ ./fibo 
Please type a number: <span style="color: blue;">40↴</span>
Fibonacci number 40 is 102334155
</pre>
<p>
Yay! Note that this algorithm is very inefficient so the Fibonacci number 40 will be really slow to calculate. A big number will also overflow 64-bit numbers.
</p>
<p>
That’s all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2017/05/29/exploring-aarch64-assembler-chapter-8/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2017/05/29/exploring-aarch64-assembler-chapter-8/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2017/05/29/exploring-aarch64-assembler-chapter-8/&amp;text=Exploring+AArch64+assembler+%E2%80%93+Chapter+8+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2017/05/29/exploring-aarch64-assembler-chapter-8/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>						<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2017/05/21/small-telegram-bot/" rel="prev">A small Telegram Bot in Go</a></span>
			<span class="next"><a href="http://thinkingeek.com/2017/05/31/compilers-memory-error-detectors/" rel="next">Compilers as a memory error detectors</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2017/05/29/exploring-aarch64-assembler-chapter-8/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="3925" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="3ff618c508"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496669082228"></form>
			</div><!-- #respond -->
	</div>	</div>