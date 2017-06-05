# Local data and the frame pointer


<div class="main">
			<h1>ARM assembler in Raspberry Pi – Chapter 18 </h1>
		<p class="post-meta"><span class="date">May 11, 2014</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2014/05/11/arm-assembler-raspberry-pi-chapter-18/#comments">7</a></span> </p>
		<p>
In this chapter we will delve a bit more into the stack.
</p>
<p><span id="more-1375"></span></p>
<h2>Local data</h2>
<p>
Most of our examples involving data stored in memory (in contrast to data stored in registers) have used <em>global</em> variables. Global variables are global names, i.e. addresses of the memory that we use through labels. These addresses, somehow, pre-exist before the program runs. This is because we define them when defining the program itself.
</p>
<p>
Sometimes, though, we may want data stored in memory the existence of which is not tied to the program existence but to the dynamic activation of a function. You may recall from previous chapters, that the stack allows us to store data the lifetime of which is the same as the dynamic activation of a function. This is where we will store <em>local</em> variables, which in contrast to global variables, only exist because the function they belong has been dynamically activated (i.e. called/invoked).
</p>
<p>
In chapter 17 we passed a very big array through the stack in order to pass the array by value. This will lead us to the conclusion that, somehow, parameters act as local data, in particular when they are passed through the stack.
</p>
<h2>The frame pointer</h2>
<p>
In ARM, we have plenty of general-purpose registers (up to 16, albeit some of them with very narrow semantics, so actually about 12 are actually useable as general-purpose) and the AAPCS forces us to use registers for the 4 first parameters (<code>r0</code> to <code>r3</code>, note how this is consistent with the fact that these 4 registers are caller-saved while all other registers are callee-saved). Other architectures, like 386, have a lower number of general purpose registers (about 6) and the usual approach when passing data to functions always involves the stack. This is so because with such a small number of registers, passing parameters through registers, would force the caller to save them, usually in the stack or some other memory, which in turn will usually require at least another register for indexing! By using the stack a few more registers are easily available.
</p>
<p>
Up to this point one might wonder why we don’t always pass everything through the stack and forget about registers <code>r0</code> to <code>r3</code>. Well, passing through registers is going to be faster as we do not have to mess with loads and stores in the memory. In addition, most functions receive just a few parameters, or at least not much more than 4, so it makes sense to exploit this feature.
</p>
<p>
But then a problem arises, what if we are passing parameters through the stack and at the same time we have local variables. Both entities will be stored in the stack. How can we deal with the two sources of data which happen to be stored in the same memory area?
</p>
<p>
Here is where the concept of <em>frame pointer</em> appears. A frame pointer is a sort of marker in the stack that we will use to tell apart local variables from parameters. I want to emphasize the fact that a frame register is almost always unnecessary and one can always devise ways to avoid it. That said, a frame pointer gives us a consistent solution to access local data and parameters in the stack. Of course, most good things come with a price, and the frame pointer is not an exception: we need to use a register for it. Sometimes this restriction may be unacceptable so we can, almost always, get rid of the frame pointer.
</p>
<p>
Due to its optional nature, the frame pointer is not specified nor mandated by the AAPCS. That said, the usual approach is using register <code>r11</code>. As an extension (apparently undocumented, as far as I have been able to tell) we can use the name <code>fp</code> which is far more informative than just <code>r11</code>. Nothing enforces this choice, we can use any other register as frame pointer. Since we will use <code>fp</code> (i.e. <code>r11</code>) we will have to refrain ourselves from using <code>r11</code> for any other purpose.
</p>
<h3>Dynamic link of the activation record</h3>
<p>
Activation record is a fancy name to specify the context of a called function. This is, the local data and parameters (if passed through the stack) of that function. When a function is written using a frame pointer some bookkeeping is required to correctly maintain the activation record.
</p>
<p>
First lets examine the typical structure of a function.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>1
2
3
4
5
6
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">function<span style="color: #339933;">:</span>
  <span style="color: #339933;">/*</span> Keep callee<span style="color: #339933;">-</span>saved registers <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">/*</span> Keep the callee saved registers <span style="color: #339933;">*/</span>
  <span style="color: #339933;">...</span> <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">code</span> of the function <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>  <span style="color: #339933;">/*</span> Restore the callee saved registers <span style="color: #339933;">*/</span>
  <span style="color: #46aa03; font-weight: bold;">bx</span> lr         <span style="color: #339933;">/*</span> Return from the function <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">function:
  /* Keep callee-saved registers */
  push {r4, lr} /* Keep the callee saved registers */
  ... /* code of the function */
  pop {r4, lr}  /* Restore the callee saved registers */
  bx lr         /* Return from the function */</p></div>

<p>
Now let’s modify the function to use a frame pointer (in the code snippet below do not mind the <code>r5</code> register that only appears here to keep the stack 8-byte aligned).
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">function<span style="color: #339933;">:</span>
  <span style="color: #339933;">/*</span> Keep callee<span style="color: #339933;">-</span>saved registers <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> fp<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">/*</span> Keep the callee saved registers<span style="color: #339933;">.</span>
                           We added r5 to keep the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #ff0000;">8</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned
                           but the important thing here is fp <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">mov</span> fp<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span>            <span style="color: #339933;">/*</span> fp ← <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">.</span> Keep dynamic link <span style="color: #00007f; font-weight: bold;">in</span> fp <span style="color: #339933;">*/</span>
  <span style="color: #339933;">...</span> <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">code</span> of the function <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">mov</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> fp            <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">sp</span> ← fp<span style="color: #339933;">.</span> Restore dynamic link <span style="color: #00007f; font-weight: bold;">in</span> fp <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> fp<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>  <span style="color: #339933;">/*</span> Restore the callee saved registers<span style="color: #339933;">.</span>
                           This will restore fp as well <span style="color: #339933;">*/</span>
  <span style="color: #46aa03; font-weight: bold;">bx</span> lr         <span style="color: #339933;">/*</span> Return from the function <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">function:
  /* Keep callee-saved registers */
  push {r4, r5, fp, lr} /* Keep the callee saved registers.
                           We added r5 to keep the stack 8-byte aligned
                           but the important thing here is fp */
  mov fp, sp            /* fp ← sp. Keep dynamic link in fp */
  ... /* code of the function */
  mov sp, fp            /* sp ← fp. Restore dynamic link in fp */
  pop {r4, r5, fp, lr}  /* Restore the callee saved registers.
                           This will restore fp as well */
  bx lr         /* Return from the function */</p></div>

<p>
Focus on instructions at line 6 and 8. In line 6 we keep the address of the top of the stack in <code>fp</code>. In line 8 we restore the value of the stack using the value kept in <code>fp</code>. Now you should see why I said that the frame pointer is usually unnecessary: if the <code>sp</code> register does not change between lines 6 and 8, having a frame pointer will be pointless, why should we restore a register that didn’t change?
</p>
<p>
Let’s assume for now that the frame pointer is going to be useful. What we did in instruction line 6 is setting the <em>dynamic link</em>. The stack and registers will look like this after we have set it.
</p>
<p><img data-attachment-id="1469" data-permalink="http://thinkingeek.com/2014/05/11/arm-assembler-raspberry-pi-chapter-18/path4864/" data-orig-file="images/path4864.png" data-orig-size="387,420" data-comments-opened="1" data-image-meta="{&quot;aperture&quot;:&quot;0&quot;,&quot;credit&quot;:&quot;&quot;,&quot;camera&quot;:&quot;&quot;,&quot;caption&quot;:&quot;&quot;,&quot;created_timestamp&quot;:&quot;0&quot;,&quot;copyright&quot;:&quot;&quot;,&quot;focal_length&quot;:&quot;0&quot;,&quot;iso&quot;:&quot;0&quot;,&quot;shutter_speed&quot;:&quot;0&quot;,&quot;title&quot;:&quot;&quot;}" data-image-title="" data-image-description="" data-medium-file="images/path4864-276x300.png" data-large-file="images/path4864.png" src="images/path4864.png" alt="" width="387" height="420" class="aligncenter size-full wp-image-1469" srcset="images/path4864.png 387w, images/path4864-276x300.png 276w" sizes="(max-width: 387px) 100vw, 387px"></p>
<p>
As you can see, the <code>fp</code> register will point to the top of the stack. But note that in the stack we have the value of the <em>old</em> <code>fp</code> (the value of the <code>fp</code> in the function that called us). If we assume that our caller also uses a frame pointer, then the <code>fp</code> we kept in the stack of the callee points to the top of the stack when our caller was called.
</p>
<p>
But still this looks useless because both registers <code>fp</code> and <code>sp</code> in the current function point to the same position in the stack.
</p>
<p>
Let’s proceed with the example, make sure you check line 7.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">function<span style="color: #339933;">:</span>
  <span style="color: #339933;">/*</span> Keep callee<span style="color: #339933;">-</span>saved registers <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> fp<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">/*</span> Keep the callee saved registers<span style="color: #339933;">.</span>
                           We added r5 to keep the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #ff0000;">8</span><span style="color: #339933;">-</span><span style="color: #0000ff; font-weight: bold;">byte</span> aligned
                           but the important thing here is fp <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">mov</span> fp<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span>            <span style="color: #339933;">/*</span> fp ← <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">.</span> Keep dynamic link <span style="color: #00007f; font-weight: bold;">in</span> fp <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">sub</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span>        <span style="color: #339933;">/*</span> Enlarge the <span style="color: #0000ff; font-weight: bold;">stack</span> by <span style="color: #ff0000;">8</span> bytes <span style="color: #339933;">*/</span>
  <span style="color: #339933;">...</span> <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">code</span> of the function <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">mov</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> fp            <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">sp</span> ← fp<span style="color: #339933;">.</span> Restore dynamic link <span style="color: #00007f; font-weight: bold;">in</span> fp <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> fp<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>  <span style="color: #339933;">/*</span> Restore the callee saved registers<span style="color: #339933;">.</span>
                           This will restore fp as well <span style="color: #339933;">*/</span>
  <span style="color: #46aa03; font-weight: bold;">bx</span> lr         <span style="color: #339933;">/*</span> Return from the function <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">function:
  /* Keep callee-saved registers */
  push {r4, r5, fp, lr} /* Keep the callee saved registers.
                           We added r5 to keep the stack 8-byte aligned
                           but the important thing here is fp */
  mov fp, sp            /* fp ← sp. Keep dynamic link in fp */
  sub sp, sp, #8        /* Enlarge the stack by 8 bytes */
  ... /* code of the function */
  mov sp, fp            /* sp ← fp. Restore dynamic link in fp */
  pop {r4, r5, fp, lr}  /* Restore the callee saved registers.
                           This will restore fp as well */
  bx lr         /* Return from the function */</p></div>

<p>
Now, after line 7, the stack and registers will look like this.
</p>
<p><img data-attachment-id="1473" data-permalink="http://thinkingeek.com/2014/05/11/arm-assembler-raspberry-pi-chapter-18/path4864-2/" data-orig-file="images/path48641.png" data-orig-size="412,514" data-comments-opened="1" data-image-meta="{&quot;aperture&quot;:&quot;0&quot;,&quot;credit&quot;:&quot;&quot;,&quot;camera&quot;:&quot;&quot;,&quot;caption&quot;:&quot;&quot;,&quot;created_timestamp&quot;:&quot;0&quot;,&quot;copyright&quot;:&quot;&quot;,&quot;focal_length&quot;:&quot;0&quot;,&quot;iso&quot;:&quot;0&quot;,&quot;shutter_speed&quot;:&quot;0&quot;,&quot;title&quot;:&quot;&quot;}" data-image-title="" data-image-description="" data-medium-file="images/path48641-240x300.png" data-large-file="images/path48641.png" src="images/path48641.png" alt="" width="412" height="514" class="aligncenter size-full wp-image-1473" srcset="images/path48641.png 412w, images/path48641-240x300.png 240w" sizes="(max-width: 412px) 100vw, 412px"></p>
<p>
Can you see the range of data from <code>sp</code> to <code>fp</code>? This is the local data of our function. We will keep local variables of a function in this space when using a frame pointer. We simply have to allocate stack space by decreasing the value of <code>sp</code> (and ensuring it is 8-byte aligned per AAPCS requirements).
</p>
<p>
Now consider the instruction <code>mov sp, fp</code> near the end of the function. What it does is leaving the state of the registers just like before we enlarged the stack (before the <code>sub sp, sp, #8</code>). And voilà, we have freed all the stack our function was using. A bonus of this approach is that it does not require keeping anywhere the amount of bytes we reserved in the stack. Neat, isn’t it?
</p>
<h3>What about parameters passed in the stack?</h3>
<p>
A player is still missing in our frame pointer approach: parameters passed through the stack. Let’s assume that our function may receive parameters in the stack and we have enlarged the stack by subtracting <code>sp</code>. The whole picture looks like this.
</p>
<p><img data-attachment-id="1482" data-permalink="http://thinkingeek.com/2014/05/11/arm-assembler-raspberry-pi-chapter-18/path4864-4/" data-orig-file="images/path48643.png" data-orig-size="412,513" data-comments-opened="1" data-image-meta="{&quot;aperture&quot;:&quot;0&quot;,&quot;credit&quot;:&quot;&quot;,&quot;camera&quot;:&quot;&quot;,&quot;caption&quot;:&quot;&quot;,&quot;created_timestamp&quot;:&quot;0&quot;,&quot;copyright&quot;:&quot;&quot;,&quot;focal_length&quot;:&quot;0&quot;,&quot;iso&quot;:&quot;0&quot;,&quot;shutter_speed&quot;:&quot;0&quot;,&quot;title&quot;:&quot;&quot;}" data-image-title="path4864" data-image-description="" data-medium-file="images/path48643-240x300.png" data-large-file="images/path48643.png" src="images/path48643.png" alt="path4864" width="412" height="513" class="aligncenter size-full wp-image-1482" srcset="images/path48643.png 412w, images/path48643-240x300.png 240w" sizes="(max-width: 412px) 100vw, 412px"></p>
<p>
I want you to note that I just lied a bit in the two first figures. In them, the <em>old</em> <code>fp</code> pointer kept in the stack pointed to the top of the stack of the caller. Not exactly, it will point to the <em>base</em> of the local data of the caller, exactly like happens with the <code>fp</code> register in the current function.
</p>
<h2>Indexing through the frame pointer</h2>
<p>
When we are using a frame pointer a nice property (that maybe you have already deduced from the figures above) holds: local data is always at lower addresses than the address pointed by <code>fp</code> while parameters passed in the stack (if any) will always be at higher addresses than the one pointed by <code>fp</code>. It must be possible to access both kinds of local data through <code>fp</code>.
</p>
<p>
In the following example we will use a function that receives an integer by reference (i.e. an address to an integer) and then squares that integer.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #993333;">void</span> sq<span style="color: #009900;">(</span><span style="color: #993333;">int</span> <span style="color: #339933;">*</span>c<span style="color: #009900;">)</span>
<span style="color: #009900;">{</span>
  <span style="color: #009900;">(</span><span style="color: #339933;">*</span>c<span style="color: #009900;">)</span> <span style="color: #339933;">=</span> <span style="color: #009900;">(</span><span style="color: #339933;">*</span>c<span style="color: #009900;">)</span> <span style="color: #339933;">*</span> <span style="color: #009900;">(</span><span style="color: #339933;">*</span>c<span style="color: #009900;">)</span><span style="color: #339933;">;</span>
<span style="color: #009900;">}</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">void sq(int *c)
{
  (*c) = (*c) * (*c);
}</p></div>

<p>
You may be wondering why the function <code>sq</code> has a parameter by reference (should not it be easier to return a value?), but bear with me for now. We can (should?) implement <code>sq</code> without using a frame pointer due to its simplicity.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>1
2
3
4
5
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">sq<span style="color: #339933;">:</span> 
  ldr r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>   <span style="color: #339933;">/*</span> r1 ← <span style="color: #009900; font-weight: bold;">(</span><span style="color: #339933;">*</span>r0<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">mul</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> r1 <span style="color: #339933;">/*</span> r1 ← r1 <span style="color: #339933;">*</span> r1 <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">str</span> r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>   <span style="color: #339933;">/*</span> <span style="color: #009900; font-weight: bold;">(</span><span style="color: #339933;">*</span>r0<span style="color: #009900; font-weight: bold;">)</span> ← r1 <span style="color: #339933;">*/</span>
  <span style="color: #46aa03; font-weight: bold;">bx</span> lr          <span style="color: #339933;">/*</span> Return from the function <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">sq: 
  ldr r1, [r0]   /* r1 ← (*r0) */
  mul r1, r1, r1 /* r1 ← r1 * r1 */
  str r1, [r0]   /* (*r0) ← r1 */
  bx lr          /* Return from the function */</p></div>

<p>
Now consider the following function that returns the sum of the squares of its five parameters. It uses the function <code>sq</code> defined above.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #993333;">int</span> sq_sum5<span style="color: #009900;">(</span><span style="color: #993333;">int</span> a<span style="color: #339933;">,</span> <span style="color: #993333;">int</span> b<span style="color: #339933;">,</span> <span style="color: #993333;">int</span> c<span style="color: #339933;">,</span> <span style="color: #993333;">int</span> d<span style="color: #339933;">,</span> <span style="color: #993333;">int</span> e<span style="color: #009900;">)</span>
<span style="color: #009900;">{</span>
  sq<span style="color: #009900;">(</span><span style="color: #339933;">&amp;</span>a<span style="color: #009900;">)</span><span style="color: #339933;">;</span>
  sq<span style="color: #009900;">(</span><span style="color: #339933;">&amp;</span>b<span style="color: #009900;">)</span><span style="color: #339933;">;</span>
  sq<span style="color: #009900;">(</span><span style="color: #339933;">&amp;</span>c<span style="color: #009900;">)</span><span style="color: #339933;">;</span>
  sq<span style="color: #009900;">(</span><span style="color: #339933;">&amp;</span>d<span style="color: #009900;">)</span><span style="color: #339933;">;</span>
  sq<span style="color: #009900;">(</span><span style="color: #339933;">&amp;</span>e<span style="color: #009900;">)</span><span style="color: #339933;">;</span>
  <span style="color: #b1b100;">return</span> a <span style="color: #339933;">+</span> b <span style="color: #339933;">+</span> c <span style="color: #339933;">+</span> d <span style="color: #339933;">+</span> e<span style="color: #339933;">;</span>
<span style="color: #009900;">}</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">int sq_sum5(int a, int b, int c, int d, int e)
{
  sq(&amp;a);
  sq(&amp;b);
  sq(&amp;c);
  sq(&amp;d);
  sq(&amp;e);
  return a + b + c + d + e;
}</p></div>

<p>
Parameters <code>a</code>, <code>b</code>, <code>c</code> and <code>d</code> will be passed through registers <code>r0</code>, <code>r1</code>, <code>r2</code>, and <code>r3</code> respectively. The parameter <code>e</code> will be passed through the stack. The function <code>sq</code>, though, expects a reference, i.e. an address, to an integer and registers do not have an address. This means we will have to allocate temporary local storage for these registers. At least one integer will have to be allocated in the stack in order to be able to call <code>sq</code> but for simplicity we will allocate four of them.
</p>
<p>
This time we will use a frame pointer to access both the local storage and the parameter <code>e</code>.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">sq_sum5<span style="color: #339933;">:</span>
  <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>fp<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>         <span style="color: #339933;">/*</span> Keep fp <span style="color: #00007f; font-weight: bold;">and</span> all callee<span style="color: #339933;">-</span>saved registers<span style="color: #339933;">.</span> <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">mov</span> fp<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span>            <span style="color: #339933;">/*</span> Set the dynamic link <span style="color: #339933;">*/</span>
&nbsp;
  <span style="color: #00007f; font-weight: bold;">sub</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">16</span>      <span style="color: #339933;">/*</span> <span style="color: #46aa03; font-weight: bold;">sp</span> ← <span style="color: #46aa03; font-weight: bold;">sp</span> <span style="color: #339933;">-</span> <span style="color: #ff0000;">16</span><span style="color: #339933;">.</span> Allocate space for <span style="color: #ff0000;">4</span> integers <span style="color: #00007f; font-weight: bold;">in</span> the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
  <span style="color: #339933;">/*</span> Keep parameters <span style="color: #00007f; font-weight: bold;">in</span> the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">str</span> r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">]</span>    <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>fp <span style="color: #339933;">-</span> <span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">)</span> ← r0 <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">str</span> r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">12</span><span style="color: #009900; font-weight: bold;">]</span>    <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>fp <span style="color: #339933;">-</span> <span style="color: #ff0000;">12</span><span style="color: #009900; font-weight: bold;">)</span> ← r1 <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">str</span> r2<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span>     <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>fp <span style="color: #339933;">-</span> <span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span> ← r2 <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">str</span> r3<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>     <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>fp <span style="color: #339933;">-</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span> ← r3 <span style="color: #339933;">*/</span>
&nbsp;
  <span style="color: #339933;">/*</span> <span style="color: #0000ff; font-weight: bold;">At</span> this point the <span style="color: #0000ff; font-weight: bold;">stack</span> looks like this
     | Value  |  Address<span style="color: #009900; font-weight: bold;">(</span><span style="color: #46aa03; font-weight: bold;">es</span><span style="color: #009900; font-weight: bold;">)</span>
     <span style="color: #339933;">+--------+-----------------------</span>
     |   r0   |  <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #009900; font-weight: bold;">]</span>
     |   r1   |  <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">12</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>
     |   r2   |  <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span>  <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span>
     |   r3   |  <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span>  <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">12</span><span style="color: #009900; font-weight: bold;">]</span>
     |   fp   |  <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span>       <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">]</span>
     |   lr   |  <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span>   <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">20</span><span style="color: #009900; font-weight: bold;">]</span>
     |   e    |  <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span><span style="color: #339933;">,</span>   <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">24</span><span style="color: #009900; font-weight: bold;">]</span>
     v
   Higher
   addresses
  <span style="color: #339933;">*/</span>
&nbsp;
  <span style="color: #00007f; font-weight: bold;">sub</span> r0<span style="color: #339933;">,</span> fp<span style="color: #339933;">,</span> #<span style="color: #ff0000;">16</span>    <span style="color: #339933;">/*</span> r0 ← fp <span style="color: #339933;">-</span> <span style="color: #ff0000;">16</span> <span style="color: #339933;">*/</span>
  <span style="color: #46aa03; font-weight: bold;">bl</span> sq              <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> sq<span style="color: #009900; font-weight: bold;">(</span>&amp;a<span style="color: #009900; font-weight: bold;">)</span><span style="color: #666666; font-style: italic;">; */</span>
  <span style="color: #00007f; font-weight: bold;">sub</span> r0<span style="color: #339933;">,</span> fp<span style="color: #339933;">,</span> #<span style="color: #ff0000;">12</span>    <span style="color: #339933;">/*</span> r0 ← fp <span style="color: #339933;">-</span> <span style="color: #ff0000;">12</span> <span style="color: #339933;">*/</span>
  <span style="color: #46aa03; font-weight: bold;">bl</span> sq              <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> sq<span style="color: #009900; font-weight: bold;">(</span>&amp;b<span style="color: #009900; font-weight: bold;">)</span><span style="color: #666666; font-style: italic;">; */</span>
  <span style="color: #00007f; font-weight: bold;">sub</span> r0<span style="color: #339933;">,</span> fp<span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span>     <span style="color: #339933;">/*</span> r0 ← fp <span style="color: #339933;">-</span> <span style="color: #ff0000;">8</span> <span style="color: #339933;">*/</span>
  <span style="color: #46aa03; font-weight: bold;">bl</span> sq              <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> sq<span style="color: #009900; font-weight: bold;">(</span>&amp;c<span style="color: #009900; font-weight: bold;">)</span><span style="color: #666666; font-style: italic;">; */</span>
  <span style="color: #00007f; font-weight: bold;">sub</span> r0<span style="color: #339933;">,</span> fp<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>     <span style="color: #339933;">/*</span> r0 ← fp <span style="color: #339933;">-</span> <span style="color: #ff0000;">4</span> <span style="color: #339933;">*/</span>
  <span style="color: #46aa03; font-weight: bold;">bl</span> sq              <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> sq<span style="color: #009900; font-weight: bold;">(</span>&amp;d<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">add</span> r0<span style="color: #339933;">,</span> fp<span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span>     <span style="color: #339933;">/*</span> r0 ← fp <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span> <span style="color: #339933;">*/</span>
  <span style="color: #46aa03; font-weight: bold;">bl</span> sq              <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> sq<span style="color: #009900; font-weight: bold;">(</span>&amp;e<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
&nbsp;
  ldr r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">/*</span> r0 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>fp <span style="color: #339933;">-</span> <span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> Loads a <span style="color: #00007f; font-weight: bold;">into</span> r0 <span style="color: #339933;">*/</span>
  ldr r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">12</span><span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">/*</span> r1 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>fp <span style="color: #339933;">-</span> <span style="color: #ff0000;">12</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> Loads b <span style="color: #00007f; font-weight: bold;">into</span> r1 <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">add</span> r0<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> r1     <span style="color: #339933;">/*</span> r0 ← r0 <span style="color: #339933;">+</span> r1 <span style="color: #339933;">*/</span>
  ldr r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span>  <span style="color: #339933;">/*</span> r1 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>fp <span style="color: #339933;">-</span> <span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> Loads c <span style="color: #00007f; font-weight: bold;">into</span> r1 <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">add</span> r0<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> r1     <span style="color: #339933;">/*</span> r0 ← r0 <span style="color: #339933;">+</span> r1 <span style="color: #339933;">*/</span>
  ldr r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #<span style="color: #339933;">-</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>  <span style="color: #339933;">/*</span> r1 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>fp <span style="color: #339933;">-</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> Loads d <span style="color: #00007f; font-weight: bold;">into</span> r1 <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">add</span> r0<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> r1     <span style="color: #339933;">/*</span> r0 ← r0 <span style="color: #339933;">+</span> r1 <span style="color: #339933;">*/</span>
  ldr r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>fp<span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span>   <span style="color: #339933;">/*</span> r1 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>fp <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> Loads e <span style="color: #00007f; font-weight: bold;">into</span> r1 <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">add</span> r0<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> r1     <span style="color: #339933;">/*</span> r0 ← r0 <span style="color: #339933;">+</span> r1 <span style="color: #339933;">*/</span>
&nbsp;
  <span style="color: #00007f; font-weight: bold;">mov</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> fp         <span style="color: #339933;">/*</span> Undo the dynamic link <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>fp<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>       <span style="color: #339933;">/*</span> Restore fp <span style="color: #00007f; font-weight: bold;">and</span> callee<span style="color: #339933;">-</span>saved registers <span style="color: #339933;">*/</span>
  <span style="color: #46aa03; font-weight: bold;">bx</span> lr              <span style="color: #339933;">/*</span> Return from the function <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">sq_sum5:
  push {fp, lr}         /* Keep fp and all callee-saved registers. */
  mov fp, sp            /* Set the dynamic link */

  sub sp, sp, #16      /* sp ← sp - 16. Allocate space for 4 integers in the stack */
  /* Keep parameters in the stack */
  str r0, [fp, #-16]    /* *(fp - 16) ← r0 */
  str r1, [fp, #-12]    /* *(fp - 12) ← r1 */
  str r2, [fp, #-8]     /* *(fp - 8) ← r2 */
  str r3, [fp, #-4]     /* *(fp - 4) ← r3 */

  /* At this point the stack looks like this
     | Value  |  Address(es)
     +--------+-----------------------
     |   r0   |  [fp, #-16], [sp]
     |   r1   |  [fp, #-12], [sp, #4]
     |   r2   |  [fp, #-8],  [sp, #8]
     |   r3   |  [fp, #-4],  [sp, #12]
     |   fp   |  [fp],       [sp, #16]
     |   lr   |  [fp, #4],   [sp, #20]
     |   e    |  [fp, #8],   [sp, #24]
     v
   Higher
   addresses
  */

  sub r0, fp, #16    /* r0 ← fp - 16 */
  bl sq              /* call sq(&amp;a); */
  sub r0, fp, #12    /* r0 ← fp - 12 */
  bl sq              /* call sq(&amp;b); */
  sub r0, fp, #8     /* r0 ← fp - 8 */
  bl sq              /* call sq(&amp;c); */
  sub r0, fp, #4     /* r0 ← fp - 4 */
  bl sq              /* call sq(&amp;d) */
  add r0, fp, #8     /* r0 ← fp + 8 */
  bl sq              /* call sq(&amp;e) */

  ldr r0, [fp, #-16] /* r0 ← *(fp - 16). Loads a into r0 */
  ldr r1, [fp, #-12] /* r1 ← *(fp - 12). Loads b into r1 */
  add r0, r0, r1     /* r0 ← r0 + r1 */
  ldr r1, [fp, #-8]  /* r1 ← *(fp - 8). Loads c into r1 */
  add r0, r0, r1     /* r0 ← r0 + r1 */
  ldr r1, [fp, #-4]  /* r1 ← *(fp - 4). Loads d into r1 */
  add r0, r0, r1     /* r0 ← r0 + r1 */
  ldr r1, [fp, #8]   /* r1 ← *(fp + 8). Loads e into r1 */
  add r0, r0, r1     /* r0 ← r0 + r1 */

  mov sp, fp         /* Undo the dynamic link */
  pop {fp, lr}       /* Restore fp and callee-saved registers */
  bx lr              /* Return from the function */</p></div>

<p>
As you can see, we first store all parameters (but <code>e</code>) in the local storage. This means that we need to enlarge the stack enough, as usual, by subtracting <code>sp</code> (line 5). Once we have the storage then we can do the actual store by using the <code>fp</code> register (lines 7 to 10). Note the usage of negative offsets, because local data will always be in lower addresses than the address in <code>fp</code>. As mentioned above, the parameter <code>e</code> does not have to be stored because it is already in the stack, in a positive offset from <code>fp</code> (i.e. at a higher address than the address in <code>fp</code>).
</p>
<p>
Note that, in this example, the frame pointer is not indispensable as we could have used <code>sp</code> to access all the required data (see the representation of the stack in lines 12 to 21).
</p>
<p>
In order to call <code>sq</code> we have to pass the addresses of the several integers, so we compute the address by subtracting <code>fp</code> the proper offset and storing it in <code>r0</code>, which will be used for passing the first (and only) parameter of <code>sq</code> (lines 27 to 36). See how, to pass the address of <code>e</code>, we just compute an address with a positive offset (line 35). Finally we add the values by loading them again in <code>r0</code> and <code>r1</code> and using <code>r0</code> to accumulate the additions (lines 38 to 46).
</p>
<p>
An example program that calls <code>sq_sum5(1, 2, 3, 4, 5)</code> looks like this.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> squares<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
message<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"Sum of 1^2 + 2^2 + 3^2 + 4^2 + 5^2 is %d\n"</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
sq<span style="color: #339933;">:</span>
  &lt;&lt;defined above&gt;&gt;
&nbsp;
sq_sum5<span style="color: #339933;">:</span>
  &lt;&lt;defined above&gt;&gt;
&nbsp;
<span style="color: #339933;">.</span>globl main
&nbsp;
main<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>          <span style="color: #339933;">/*</span> Keep callee<span style="color: #339933;">-</span>saved registers <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> Prepare the <span style="color: #00007f; font-weight: bold;">call</span> to sq_sum5 <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>             <span style="color: #339933;">/*</span> Parameter a ← <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span>             <span style="color: #339933;">/*</span> Parameter b ← <span style="color: #ff0000;">2</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">3</span>             <span style="color: #339933;">/*</span> Parameter c ← <span style="color: #ff0000;">3</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r3<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>             <span style="color: #339933;">/*</span> Parameter d ← <span style="color: #ff0000;">4</span> <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> Parameter e goes through the <span style="color: #0000ff; font-weight: bold;">stack</span><span style="color: #339933;">,</span>
       so it requires enlarging the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">5</span>             <span style="color: #339933;">/*</span> r4 ← <span style="color: #ff0000;">5</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">sub</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span>         <span style="color: #339933;">/*</span> Enlarge the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #ff0000;">8</span> bytes<span style="color: #339933;">,</span>
                              we will use only the
                              topmost <span style="color: #ff0000;">4</span> bytes <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">str</span> r4<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #009900; font-weight: bold;">]</span>           <span style="color: #339933;">/*</span> Parameter e ← <span style="color: #ff0000;">5</span> <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> sq_sum5             <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> sq_sum5<span style="color: #009900; font-weight: bold;">(</span><span style="color: #ff0000;">1</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">2</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">3</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">5</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">add</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span>         <span style="color: #339933;">/*</span> Shrink back the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> Prepare the <span style="color: #00007f; font-weight: bold;">call</span> to printf <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> r0             <span style="color: #339933;">/*</span> The result of sq_sum5 <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> address_of_message
    <span style="color: #46aa03; font-weight: bold;">bl</span> printf              <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Call</span> printf <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>           <span style="color: #339933;">/*</span> Restore callee<span style="color: #339933;">-</span>saved registers <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr
&nbsp;
&nbsp;
address_of_message<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> message</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* squares.s */
.data

.align 4
message: .asciz "Sum of 1^2 + 2^2 + 3^2 + 4^2 + 5^2 is %d\n"

.text

sq:
  &lt;&lt;defined above&gt;&gt;

sq_sum5:
  &lt;&lt;defined above&gt;&gt;

.globl main

main:
    push {r4, lr}          /* Keep callee-saved registers */

    /* Prepare the call to sq_sum5 */
    mov r0, #1             /* Parameter a ← 1 */
    mov r1, #2             /* Parameter b ← 2 */
    mov r2, #3             /* Parameter c ← 3 */
    mov r3, #4             /* Parameter d ← 4 */

    /* Parameter e goes through the stack,
       so it requires enlarging the stack */
    mov r4, #5             /* r4 ← 5 */
    sub sp, sp, #8         /* Enlarge the stack 8 bytes,
                              we will use only the
                              topmost 4 bytes */
    str r4, [sp]           /* Parameter e ← 5 */
    bl sq_sum5             /* call sq_sum5(1, 2, 3, 4, 5) */
    add sp, sp, #8         /* Shrink back the stack */

    /* Prepare the call to printf */
    mov r1, r0             /* The result of sq_sum5 */
    ldr r0, address_of_message
    bl printf              /* Call printf */

    pop {r4, lr}           /* Restore callee-saved registers */
    bx lr


address_of_message: .word message</p></div>

<pre>$ ./square 
Sum of 1^2 + 2^2 + 3^2 + 4^2 + 5^2 is 55
</pre>
<p>
That’s all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2014/05/11/arm-assembler-raspberry-pi-chapter-18/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2014/05/11/arm-assembler-raspberry-pi-chapter-18/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2014/05/11/arm-assembler-raspberry-pi-chapter-18/&amp;text=ARM+assembler+in+Raspberry+Pi+%E2%80%93+Chapter+18+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2014/05/11/arm-assembler-raspberry-pi-chapter-18/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>						<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2014/05/10/create-configure-ec2-instances-rails-deployment-ansible/" rel="prev">How to create and configure EC2 instances for Rails hosting with CentOS using Ansible</a></span>
			<span class="next"><a href="http://thinkingeek.com/2014/05/24/arm-assembler-raspberry-pi-chapter-19/" rel="next">ARM assembler in Raspberry Pi – Chapter 19</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
			<h3>7 thoughts on “<span>ARM assembler in Raspberry Pi – Chapter 18</span>”</h3>
		<ul class="commentlist">
				<li class="comment even thread-even depth-1 parent" id="comment-129136">
				<div id="div-comment-129136" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-9e81362ff9654533accffc53762c9520-0">			<cite class="fn"><a href="http://devel.meruelo.com.es/blog/" rel="external nofollow" class="url">Fernando</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/05/11/arm-assembler-raspberry-pi-chapter-18/#comment-129136">
			May 14, 2014 at 9:21 pm</a>		</div>

		<p>Nice entry! Keep up the great work!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/05/11/arm-assembler-raspberry-pi-chapter-18/?replytocom=129136#respond" onclick="return addComment.moveForm( &quot;div-comment-129136&quot;, &quot;129136&quot;, &quot;respond&quot;, &quot;1375&quot; )" aria-label="Reply to Fernando">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-129811">
				<div id="div-comment-129811" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-0">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/05/11/arm-assembler-raspberry-pi-chapter-18/#comment-129811">
			May 15, 2014 at 7:59 pm</a>		</div>

		<p>Thanks Fernando! </p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/05/11/arm-assembler-raspberry-pi-chapter-18/?replytocom=129811#respond" onclick="return addComment.moveForm( &quot;div-comment-129811&quot;, &quot;129811&quot;, &quot;respond&quot;, &quot;1375&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-968472">
				<div id="div-comment-968472" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/b113497a7ade2a288e43f792650fe82c?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/b113497a7ade2a288e43f792650fe82c?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-b113497a7ade2a288e43f792650fe82c-0" originals="32" src-orig="http://2.gravatar.com/avatar/b113497a7ade2a288e43f792650fe82c?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Gael</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/05/11/arm-assembler-raspberry-pi-chapter-18/#comment-968472">
			May 27, 2016 at 11:52 pm</a>		</div>

		<p>These tutorials are really great…the size of the chapters is perfect ! For this chapter I think the diagrams could be clearer if the saved fp was pointing to the beginning of the caller stack frame.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/05/11/arm-assembler-raspberry-pi-chapter-18/?replytocom=968472#respond" onclick="return addComment.moveForm( &quot;div-comment-968472&quot;, &quot;968472&quot;, &quot;respond&quot;, &quot;1375&quot; )" aria-label="Reply to Gael">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-969150">
				<div id="div-comment-969150" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-1">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/05/11/arm-assembler-raspberry-pi-chapter-18/#comment-969150">
			June 4, 2016 at 7:38 pm</a>		</div>

		<p>Hi Gael,</p>
<p>thanks for the nice comments. I’m not sure what you mean by <em>«the saved fp»</em>.</p>
<p>I guess you mean the contents of the memory in the stack where we store the old <code>fp</code>, right? The current <code>fp</code> points to the frame pointer of the callee, but you are right that the address pointed by <code>fp</code> (in the stack) contains the <code>fp</code> of the caller.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/05/11/arm-assembler-raspberry-pi-chapter-18/?replytocom=969150#respond" onclick="return addComment.moveForm( &quot;div-comment-969150&quot;, &quot;969150&quot;, &quot;respond&quot;, &quot;1375&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1" id="comment-969684">
				<div id="div-comment-969684" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/b113497a7ade2a288e43f792650fe82c?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/b113497a7ade2a288e43f792650fe82c?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-b113497a7ade2a288e43f792650fe82c-1" originals="32" src-orig="http://2.gravatar.com/avatar/b113497a7ade2a288e43f792650fe82c?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Gael</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/05/11/arm-assembler-raspberry-pi-chapter-18/#comment-969684">
			June 14, 2016 at 8:44 am</a>		</div>

		<p>Hi Roger,</p>
<p>This is it ! In fact when I wrote the comment I hadn’t finished the whole chapter and this is explained in the last stack diagram. But I was wondering if it shouldn’t have been presented right from the first diagram. For the first time in the tutorials, I had to stop and reread the previous paragraph to try to figure out why the stack-saved fp was pointing to this spot…but it can possibly be explained by the fact that I already knew where it was supposed to be pointing. Anyway I am not sure my suggestion is the right answer for everyone, pedagogy is hard…you sometimes have to lie a little to get to the point !</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/05/11/arm-assembler-raspberry-pi-chapter-18/?replytocom=969684#respond" onclick="return addComment.moveForm( &quot;div-comment-969684&quot;, &quot;969684&quot;, &quot;respond&quot;, &quot;1375&quot; )" aria-label="Reply to Gael">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1 parent" id="comment-981193">
				<div id="div-comment-981193" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/3675444e34d113abf6eb1978eea4c8ed?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/3675444e34d113abf6eb1978eea4c8ed?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-3675444e34d113abf6eb1978eea4c8ed-0" originals="32" src-orig="http://0.gravatar.com/avatar/3675444e34d113abf6eb1978eea4c8ed?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Shawn Mason</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/05/11/arm-assembler-raspberry-pi-chapter-18/#comment-981193">
			December 5, 2016 at 3:41 pm</a>		</div>

		<p>Excellent tutorial.  Thank you for putting this together.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/05/11/arm-assembler-raspberry-pi-chapter-18/?replytocom=981193#respond" onclick="return addComment.moveForm( &quot;div-comment-981193&quot;, &quot;981193&quot;, &quot;respond&quot;, &quot;1375&quot; )" aria-label="Reply to Shawn Mason">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2" id="comment-981867">
				<div id="div-comment-981867" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-2">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2014/05/11/arm-assembler-raspberry-pi-chapter-18/#comment-981867">
			December 15, 2016 at 9:23 pm</a>		</div>

		<p>Thanks Shawn!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2014/05/11/arm-assembler-raspberry-pi-chapter-18/?replytocom=981867#respond" onclick="return addComment.moveForm( &quot;div-comment-981867&quot;, &quot;981867&quot;, &quot;respond&quot;, &quot;1375&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		</ul>
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2014/05/11/arm-assembler-raspberry-pi-chapter-18/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="1375" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="2380b49084"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496666454570"></form>
			</div><!-- #respond -->
	</div>	</div>
