# Floating point numbers


<div class="main">
			<h1>ARM assembler in Raspberry Pi – Chapter 13 </h1>
		<p class="post-meta"><span class="date">May 12, 2013</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/#comments">22</a></span> </p>
		<p>
So far, all examples have dealt with integer values. But processors would be rather limited if they were only able to work with integer values. Fortunately they can work with floating point numbers. In this chapter we will see how we can use the floating point facilities of our Raspberry Pi.
</p>
<p><span id="more-911"></span></p>
<h2>Floating point numbers</h2>
<p>
Following is a quick recap of what is a floating point number.
</p>
<p>
A <em>binary floating point number</em> is an approximate representation of a real number with three parts: <em>sign</em>, <em>mantissa</em> and <em>exponent</em>. The <em>sign</em> may be just 0 or 1, meaning 1 a negative number, positive otherwise. The <em>mantissa</em> represents a fractional magnitude. Similarly to 1.2345 we can have a binary <code>1.01110</code> where every digit is just a bit. The dot means where the integer part ends and the fractional part starts. Note that there is nothing special in binary fractional numbers: <code>1.01110</code> is just 2<sup>0</sup> + 2<sup>-2</sup> + 2<sup>-3</sup> + 2<sup>-4</sup> = 1.43750<sub>(10</sub>. Usually numbers are normalized, this means that the mantissa is adjusted so the integer part is always 1, so instead of <em>0.00110101</em> we would represent <em>1.101101</em> (in fact a floating point may be a <em>denormal</em> if this property does not hold, but such numbers lie in a very specific range so we can ignore them here). If the mantissa is adjusted so it always has a single 1 as the integer part two things happen. First, we do not represent the integer part (as it is always 1 in normalized numbers). Second, to make things sound we need an <em>exponent</em> which compensates the mantissa being normalized. This means that the number -101.110111 (remember that it is a binary real number) will be represented by a sign = 1, mantissa = 1.01110111 and exponent = 2 (because we moved the dot 2 digits to the left). Similarly, number 0.0010110111 is represented with a sign = 0, mantissa = 1.0110111 and exponent = -3 (we moved the dot 3 digits to the right).
</p>
<p>
In order for different computers to be able to share floating point numbers, IEEE 754 standardizes the format of a floating point number. VFPv2 supports two of the IEEE 754 numbers: Binary32 and Binary64, usually known by their C types, <code>float</code> and <code>double</code>, or by single- and double-precision, respectively. In a <a href="http://en.wikipedia.org/wiki/Single_precision_floating-point_format" title="Single-precision floating-point format">single-precision floating point</a> the mantissa is 23 bits (+1 of the integer one for normalized numbers) and the exponent is 8 bits (so the exponent ranges from -126 to 127). In a<a href="http://en.wikipedia.org/wiki/Double_precision_floating-point_format" title="Double-precision floating-point format"> double-precision floating point</a> the mantissa is 52 bits (+1) and the exponent is 11 bits (so the exponent ranges from -1022 to 1023). A single-precision floating point number occupies 32 bit and a double-precision floating point number occupies 64 bits. Operating double-precision numbers is in average one and a half to twice slower than single-precision.
</p>
<p>
<a href="http://cr.yp.to/2005-590/goldberg.pdf" title="What Every Computer Scientist Should Know About Floating Point Arithmetic">Goldberg’s famous paper</a> is a classical reference that should be read by anyone serious when using floating point numbers.
</p>
<h2>Coprocessors</h2>
<p>
As I stated several times in earlier chapters, ARM was designed to be very flexible. We can see this in the fact that ARM architecture provides a generic coprocessor interface. Manufacturers of system-on-chips may bundle additional coprocessors. Each coprocessor is identified by a number and provides specific instructions. For instance the Raspberry Pi SoC is a BCM2835 which provides a multimedia coprocessor (which we will not discuss here).
</p>
<p>
That said, there are two standard coprocessors in the ARMv6 architecture: 10 and 11. These two coprocessors provide floating point support for single and double precision, respectively. Although the floating point instructions have their own specific names, they are actually mapped to generic coprocessor instructions targeting coprocessor 10 and 11.
</p>
<h2>Vector Floating-point v2</h2>
<p>
ARMv6 defines a floating point subarchitecture called the Vector Floating-point v2 (VFPv2). Version 2 because earlier ARM architectures supported a simpler form called now v1. As stated above, the VFP is implemented on top of two standarized coprocessors 10 and 11. ARMv6 does not require VFPv2 be implemented in hardware (one can always resort to a slower software implementation). Fortunately, the Raspberry Pi does provide a hardware implementation of VFPv2.
</p>
<h2>VFPv2 Registers</h2>
<p>
We already know that the ARM architecture provides 16 general purpose registers <code>r0</code> to <code>r15</code>, where some of them play special roles: <code>r13</code>, <code>r14</code> and <code>r15</code>. Despite their name, these general purpose registers do not allow operating floating point numbers in them, so VFPv2 provides us with some specific registers. These registers are named <code>s0</code> to <code>s31</code>, for single-precision, and <code>d0</code> to <code>d15</code> for double precision. These are not 48 different registers. Instead every <code>d<sub>n</sub></code> is mapped to two (consecutive) registers <code>s<sub>2n</sub></code> and <code>s<sub>2n+1</sub></code>, where 0 ≤ <code>n</code> ≤ 15.
</p>
<p>
These registers are structured in 4 banks: <code>s0</code>–<code>s7</code> (<code>d0</code>–<code>d3</code>), <code>s8</code>–<code>s15</code> (<code>d4</code>–<code>d7</code>), <code>s16</code>–<code>s23</code> (<code>d8</code>–<code>d11</code>) and <code>s24</code>–<code>s31</code> (<code>d12</code>–<code>d15</code>). We will call the first bank (bank 0, <code>s0</code>–<code>s7</code>, <code>d0</code>–<code>d3</code>) the <em>scalar</em> bank, while the remaining three are <em>vectorial</em> banks (below we will see why).
</p>
<p><img data-attachment-id="976" data-permalink="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/vfp-registers/" data-orig-file="images/vfp-registers.png" data-orig-size="558,387" data-comments-opened="1" data-image-meta="{&quot;aperture&quot;:&quot;0&quot;,&quot;credit&quot;:&quot;&quot;,&quot;camera&quot;:&quot;&quot;,&quot;caption&quot;:&quot;&quot;,&quot;created_timestamp&quot;:&quot;0&quot;,&quot;copyright&quot;:&quot;&quot;,&quot;focal_length&quot;:&quot;0&quot;,&quot;iso&quot;:&quot;0&quot;,&quot;shutter_speed&quot;:&quot;0&quot;,&quot;title&quot;:&quot;&quot;}" data-image-title="vfp-registers" data-image-description="" data-medium-file="images/vfp-registers-300x208.png" data-large-file="images/vfp-registers.png" src="images/vfp-registers.png" alt="vfp-registers" width="558" height="387" class="aligncenter size-full wp-image-976" srcset="images/vfp-registers.png 558w, images/vfp-registers-300x208.png 300w" sizes="(max-width: 558px) 100vw, 558px"></p>
<p>
VFPv2 provides three control registers but we will only be interested in one called <code>fpscr</code>. This register is similar to the <code>cpsr</code> as it keeps the usual comparison flags <code>N</code>, <code>Z</code>, <code>C</code> and <code>V</code>. It also stores two fields that are very useful, <code>len</code> and <code>stride</code>. These two fields control how floating point instructions behave. We will not care very much of the remaining information in this register: status information of the floating point exceptions, the current rounding mode and whether denormal numbers are flushed to zero.
</p>
<h2>Arithmetic operations</h2>
<p>
Most VFPv2 instructions are of the form <code>v<em>name</em> Rdest, Rsource1, Rsource2</code> or <code>f<em>name</em> Rdest, Rsource1</code>. They have three modes of operation.
</p>
<ul>
<li>Scalar. This mode is used when the destination register is in bank 0 (<code>s0</code>–<code>s7</code> or <code>d0</code>–<code>d3</code>). In this case, the instruction operates only with <code>Rsource1</code> and <code>Rsource2</code>. No other registers are involved.
</li><li>Vectorial. This mode is used when the destination register and Rsource2 (or Rsource1 for instructions with only one source register) are not in the bank 0. In this case the instruction will operate as many registers (starting from the given register in the instruction and wrapping around the bank of the register) as defined in field <code>len</code> of the <code>fpscr</code> (at least 1). The next register operated is defined by the <code>stride</code> field of the <code>fpscr</code> (at least 1). If wrap-around happens, no register can be operated twice.
</li><li>Scalar expanded (also called <em>mixed vector/scalar</em>). This mode is used if Rsource2 (or Rsource1 if the instruction only has one source register) is in the bank0, but the destination is not. In this case Rsource2 (or Rsource1 for instructions with only one source) is left fixed as the source. The remaining registers are operated as in the vectorial case (this is, using <code>len</code> and <code>stride</code> from the <code>fpscr</code>).
</li></ul>
<p>
Ok, this looks pretty complicated, so let’s see some examples. Most instructions end in <code>.f32</code> if they operate on single-precision and in <code>.f64</code> if they operate in double-precision. We can add two single-precision numbers using <code>vadd.f32 Rdest, Rsource1, Rsource2</code> and double-precision using <code>vadd.f64 Rdest, Rsource1, Rsource2</code>. Note also that we can use predication in these instructions (but be aware that, as usual, predication uses the flags in <code>cpsr</code> not in <code>fpscr</code>). Predication would be specified before the suffix like in <code>vaddne.f32</code>.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">//</span> For this example assume that len = <span style="color: #ff0000;">4</span><span style="color: #339933;">,</span> stride = <span style="color: #ff0000;">2</span>
vadd<span style="color: #339933;">.</span>f32 s1<span style="color: #339933;">,</span> s2<span style="color: #339933;">,</span> s3  <span style="color: #339933;">/*</span> s1 ← s2 <span style="color: #339933;">+</span> s3<span style="color: #339933;">.</span> Scalar operation because Rdest = s1 <span style="color: #00007f; font-weight: bold;">in</span> the bank <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
vadd<span style="color: #339933;">.</span>f32 s1<span style="color: #339933;">,</span> s8<span style="color: #339933;">,</span> s15 <span style="color: #339933;">/*</span> s1 ← s8 <span style="color: #339933;">+</span> s15<span style="color: #339933;">.</span> ditto <span style="color: #339933;">*/</span>
vadd<span style="color: #339933;">.</span>f32 s8<span style="color: #339933;">,</span> s16<span style="color: #339933;">,</span> s24 <span style="color: #339933;">/*</span> s8  ← s16 <span style="color: #339933;">+</span> s24
                      s10 ← s18 <span style="color: #339933;">+</span> s26
                      s12 ← s20 <span style="color: #339933;">+</span> s28
                      s14 ← s22 <span style="color: #339933;">+</span> s30
                      <span style="color: #00007f; font-weight: bold;">or</span> more compactly <span style="color: #009900; font-weight: bold;">{</span>s8<span style="color: #339933;">,</span>s10<span style="color: #339933;">,</span>s12<span style="color: #339933;">,</span>s14<span style="color: #009900; font-weight: bold;">}</span> ← <span style="color: #009900; font-weight: bold;">{</span>s16<span style="color: #339933;">,</span>s18<span style="color: #339933;">,</span>s20<span style="color: #339933;">,</span>s22<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">{</span>s24<span style="color: #339933;">,</span>s26<span style="color: #339933;">,</span>s28<span style="color: #339933;">,</span>s30<span style="color: #009900; font-weight: bold;">}</span>
                      Vectorial<span style="color: #339933;">,</span> since Rdest <span style="color: #00007f; font-weight: bold;">and</span> Rsource2 are <span style="color: #00007f; font-weight: bold;">not</span> <span style="color: #00007f; font-weight: bold;">in</span> bank <span style="color: #ff0000;">0</span>
                   <span style="color: #339933;">*/</span>
vadd<span style="color: #339933;">.</span>f32 s10<span style="color: #339933;">,</span> s16<span style="color: #339933;">,</span> s24 <span style="color: #339933;">/*</span> <span style="color: #009900; font-weight: bold;">{</span>s10<span style="color: #339933;">,</span>s12<span style="color: #339933;">,</span>s14<span style="color: #339933;">,</span>s8<span style="color: #009900; font-weight: bold;">}</span> ← <span style="color: #009900; font-weight: bold;">{</span>s16<span style="color: #339933;">,</span>s18<span style="color: #339933;">,</span>s20<span style="color: #339933;">,</span>s22<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">{</span>s24<span style="color: #339933;">,</span>s26<span style="color: #339933;">,</span>s28<span style="color: #339933;">,</span>s30<span style="color: #009900; font-weight: bold;">}</span><span style="color: #339933;">.</span>
                       Vectorial<span style="color: #339933;">,</span> but note the wraparound inside the bank after s14<span style="color: #339933;">.</span>
                     <span style="color: #339933;">*/</span>
vadd<span style="color: #339933;">.</span>f32 s8<span style="color: #339933;">,</span> s16<span style="color: #339933;">,</span> s3 <span style="color: #339933;">/*</span> <span style="color: #009900; font-weight: bold;">{</span>s8<span style="color: #339933;">,</span>s10<span style="color: #339933;">,</span>s12<span style="color: #339933;">,</span>s14<span style="color: #009900; font-weight: bold;">}</span> ← <span style="color: #009900; font-weight: bold;">{</span>s16<span style="color: #339933;">,</span>s18<span style="color: #339933;">,</span>s20<span style="color: #339933;">,</span>s22<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">{</span>s3<span style="color: #339933;">,</span>s3<span style="color: #339933;">,</span>s3<span style="color: #339933;">,</span>s3<span style="color: #009900; font-weight: bold;">}</span>
                     Scalar expanded since Rsource2 is <span style="color: #00007f; font-weight: bold;">in</span> the bank <span style="color: #ff0000;">0</span>
                   <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">// For this example assume that len = 4, stride = 2
vadd.f32 s1, s2, s3  /* s1 ← s2 + s3. Scalar operation because Rdest = s1 in the bank 0 */
vadd.f32 s1, s8, s15 /* s1 ← s8 + s15. ditto */
vadd.f32 s8, s16, s24 /* s8  ← s16 + s24
                      s10 ← s18 + s26
                      s12 ← s20 + s28
                      s14 ← s22 + s30
                      or more compactly {s8,s10,s12,s14} ← {s16,s18,s20,s22} + {s24,s26,s28,s30}
                      Vectorial, since Rdest and Rsource2 are not in bank 0
                   */
vadd.f32 s10, s16, s24 /* {s10,s12,s14,s8} ← {s16,s18,s20,s22} + {s24,s26,s28,s30}.
                       Vectorial, but note the wraparound inside the bank after s14.
                     */
vadd.f32 s8, s16, s3 /* {s8,s10,s12,s14} ← {s16,s18,s20,s22} + {s3,s3,s3,s3}
                     Scalar expanded since Rsource2 is in the bank 0
                   */</p></div>

<h2>Load and store</h2>
<p>
Once we have a rough idea of how we can operate floating points in VFPv2, a question remains: how do we load/store floating point values from/to memory? VFPv2 provides several specific load/store instructions.
</p>
<p>
We load/store one single-precision floating point using <code>vldr</code>/<code>vstr</code>. The address of the load/store must be already in a general purpose register, although we can apply an offset in bytes which must be a multiple of 4 (this applies to double-precision as well).
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">vldr s1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r3<span style="color: #009900; font-weight: bold;">]</span>         <span style="color: #339933;">/*</span> s1 ← <span style="color: #339933;">*</span>r3 <span style="color: #339933;">*/</span>
vldr s2<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r3<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>     <span style="color: #339933;">/*</span> s2 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r3 <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
vldr s3<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r3<span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span>     <span style="color: #339933;">/*</span> s3 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r3 <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
vldr s4<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r3<span style="color: #339933;">,</span> #<span style="color: #ff0000;">12</span><span style="color: #009900; font-weight: bold;">]</span>     <span style="color: #339933;">/*</span> s3 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r3 <span style="color: #339933;">+</span> <span style="color: #ff0000;">12</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
&nbsp;
vstr s10<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #009900; font-weight: bold;">]</span>        <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span>r4 ← s10 <span style="color: #339933;">*/</span>
vstr s11<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">]</span>     <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span> ← s11 <span style="color: #339933;">*/</span>
vstr s12<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">]</span>     <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span> ← s12 <span style="color: #339933;">*/</span>
vstr s13<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">12</span><span style="color: #009900; font-weight: bold;">]</span>      <span style="color: #339933;">/*</span> <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">12</span><span style="color: #009900; font-weight: bold;">)</span> ← s13 <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">vldr s1, [r3]         /* s1 ← *r3 */
vldr s2, [r3, #4]     /* s2 ← *(r3 + 4) */
vldr s3, [r3, #8]     /* s3 ← *(r3 + 8) */
vldr s4, [r3, #12]     /* s3 ← *(r3 + 12) */

vstr s10, [r4]        /* *r4 ← s10 */
vstr s11, [r4, #4]     /* *(r4 + 4) ← s11 */
vstr s12, [r4, #8]     /* *(r4 + 8) ← s12 */
vstr s13, [r4, #12]      /* *(r4 + 12) ← s13 */</p></div>

<p>
We can load/store several registers with a single instruction. In contrast to general load/store, we cannot load an arbitrary set of registers but instead they must be a sequential set of registers.
</p>
<pre>// Here precision can be s or d for single-precision and double-precision
// floating-point-register-set is {sFirst-sLast} for single-precision 
// and {dFirst-dLast} for double-precision
vldm indexing-mode precision Rbase{!}, floating-point-register-set
vstm indexing-mode precision Rbase{!}, floating-point-register-set
</pre>
<p>
The behaviour is similar to the indexing modes we saw in chapter 10. There is a Rbase register used as the base address of several load/store to/from floating point registers. There are only two indexing modes: increment after and decrement before. When using increment after, the address used to load/store the floating point value register is increased by 4 after the load/store has happened. When using decrement before, the base address is first subtracted as many bytes as foating point values are going to be loaded/stored. Rbase is always updated in decrement before but it is optional to update it in increment after.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">vldmias r4<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">{</span>s3<span style="color: #339933;">-</span>s8<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">/*</span> s3 ← <span style="color: #339933;">*</span>r4
                       s4 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span>
                       s5 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">8</span><span style="color: #009900; font-weight: bold;">)</span>
                       s6 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">12</span><span style="color: #009900; font-weight: bold;">)</span>
                       s7 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">16</span><span style="color: #009900; font-weight: bold;">)</span>
                       s8 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">20</span><span style="color: #009900; font-weight: bold;">)</span>
                     <span style="color: #339933;">*/</span>
vldmias r4!<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">{</span>s3<span style="color: #339933;">-</span>s8<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">/*</span> Like the previous instruction
                        but <span style="color: #0000ff; font-weight: bold;">at</span> the end r4 ← r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">24</span> 
                      <span style="color: #339933;">*/</span>
vstmdbs r5!<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">{</span>s12<span style="color: #339933;">-</span>s13<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">/*</span>  <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r5 <span style="color: #339933;">-</span> <span style="color: #ff0000;">4</span> <span style="color: #339933;">*</span> <span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">)</span> ← s12
                           <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r5 <span style="color: #339933;">-</span> <span style="color: #ff0000;">4</span> <span style="color: #339933;">*</span> <span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">)</span> ← s13
                           r5 ← r5 <span style="color: #339933;">-</span> <span style="color: #ff0000;">4</span><span style="color: #339933;">*</span><span style="color: #ff0000;">2</span>
                       <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">vldmias r4, {s3-s8} /* s3 ← *r4
                       s4 ← *(r4 + 4)
                       s5 ← *(r4 + 8)
                       s6 ← *(r4 + 12)
                       s7 ← *(r4 + 16)
                       s8 ← *(r4 + 20)
                     */
vldmias r4!, {s3-s8} /* Like the previous instruction
                        but at the end r4 ← r4 + 24 
                      */
vstmdbs r5!, {s12-s13} /*  *(r5 - 4 * 2) ← s12
                           *(r5 - 4 * 1) ← s13
                           r5 ← r5 - 4*2
                       */</p></div>

<p>
For the usual stack operations when we push onto the stack several floating point registers we will use <code>vstmdb</code> with <code>sp!</code> as the base register. To pop from the stack we will use <code>vldmia</code> again with <code>sp!</code> as the base register. Given that these instructions names are very hard to remember we can use the mnemonics <code>vpush</code> and <code>vpop</code>, respectively.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">vpush <span style="color: #009900; font-weight: bold;">{</span>s0<span style="color: #339933;">-</span>s5<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">/*</span> Equivalent to vstmdb <span style="color: #46aa03; font-weight: bold;">sp</span>!<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">{</span>s0<span style="color: #339933;">-</span>s5<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">*/</span>
vpop <span style="color: #009900; font-weight: bold;">{</span>s0<span style="color: #339933;">-</span>s5<span style="color: #009900; font-weight: bold;">}</span>  <span style="color: #339933;">/*</span> Equivalent to vldmia <span style="color: #46aa03; font-weight: bold;">sp</span>!<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">{</span>s0<span style="color: #339933;">-</span>s5<span style="color: #009900; font-weight: bold;">}</span> <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">vpush {s0-s5} /* Equivalent to vstmdb sp!, {s0-s5} */
vpop {s0-s5}  /* Equivalent to vldmia sp!, {s0-s5} */</p></div>

<h2>Movements between registers</h2>
<p>
Another operation that may be required sometimes is moving among registers. Similar to the <code>mov</code> instruction for general purpose registers there is the <code>vmov</code> instruction. Several movements are possible.
</p>
<p>We can move floating point values between two floating point registers of the same precision</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">vmov s2<span style="color: #339933;">,</span> s3  <span style="color: #339933;">/*</span> s2 ← s3 <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">vmov s2, s3  /* s2 ← s3 */</p></div>

<p>
Between one general purpose register and one single-precision register. But note that data is not converted. Only bits are copied around, so be aware of not mixing floating point values with integer instructions or the other way round.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">vmov s2<span style="color: #339933;">,</span> r3  <span style="color: #339933;">/*</span> s2 ← r3 <span style="color: #339933;">*/</span>
vmov r4<span style="color: #339933;">,</span> s5  <span style="color: #339933;">/*</span> r4 ← s5 <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">vmov s2, r3  /* s2 ← r3 */
vmov r4, s5  /* r4 ← s5 */</p></div>

<p>
Like the previous case but between two general purpose registers and two consecutive single-precision registers.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">vmov s2<span style="color: #339933;">,</span> s3<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">r10</span> <span style="color: #339933;">/*</span> s2 ← r4
                        s3 ← <span style="color: #46aa03; font-weight: bold;">r10</span> <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">vmov s2, s3, r4, r10 /* s2 ← r4
                        s3 ← r10 */</p></div>

<p>
Between two general purpose registers and one double-precision register. Again, note that data is not converted.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">vmov d3<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> r6  <span style="color: #339933;">/*</span> Lower32BitsOf<span style="color: #009900; font-weight: bold;">(</span>d3<span style="color: #009900; font-weight: bold;">)</span> ← r4
                    Higher32BitsOf<span style="color: #009900; font-weight: bold;">(</span>d3<span style="color: #009900; font-weight: bold;">)</span> ← r6
                 <span style="color: #339933;">*/</span>
vmov r5<span style="color: #339933;">,</span> r7<span style="color: #339933;">,</span> d4 <span style="color: #339933;">/*</span> r5 ← Lower32BitsOf<span style="color: #009900; font-weight: bold;">(</span>d4<span style="color: #009900; font-weight: bold;">)</span>
                   r7 ← Higher32BitsOf<span style="color: #009900; font-weight: bold;">(</span>d4<span style="color: #009900; font-weight: bold;">)</span>
                 <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">vmov d3, r4, r6  /* Lower32BitsOf(d3) ← r4
                    Higher32BitsOf(d3) ← r6
                 */
vmov r5, r7, d4 /* r5 ← Lower32BitsOf(d4)
                   r7 ← Higher32BitsOf(d4)
                 */</p></div>

<h2>Conversions</h2>
<p>
Sometimes we need to convert from an integer to a floating-point and the opposite. Note that some conversions may potentially lose precision, in particular when a floating point is converted to an integer. There is a single instruction <code>vcvt</code> with a suffix <code>.T.S</code> where <code>T</code> (target) and <code>S</code> (source) can be <code>u32</code>, <code>s32</code>, <code>f32</code> and <code>f64</code> (<code>S</code> must be different to <code>T</code>). Both registers must be floating point registers, so in order to convert integers to floating point or floating point to an integer value an extra <code>vmov</code> instruction will be required from or to an integer register before or after the conversion. Because of this, for a moment (between the two instructions) a floating point register will contain a value which is not a IEEE 754 value, bear this in mind.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">vcvt<span style="color: #339933;">.</span>f64<span style="color: #339933;">.</span>f32 d0<span style="color: #339933;">,</span> s0  <span style="color: #339933;">/*</span> Converts s0 single<span style="color: #339933;">-</span>precision value 
                        to a double<span style="color: #339933;">-</span>precision value <span style="color: #00007f; font-weight: bold;">and</span> stores it <span style="color: #00007f; font-weight: bold;">in</span> d0 <span style="color: #339933;">*/</span>
&nbsp;
vcvt<span style="color: #339933;">.</span>f32<span style="color: #339933;">.</span>f64 s0<span style="color: #339933;">,</span> d0  <span style="color: #339933;">/*</span> Converts d0 double<span style="color: #339933;">-</span>precision value 
                        to a single<span style="color: #339933;">-</span>precision value  <span style="color: #00007f; font-weight: bold;">and</span> stores it <span style="color: #00007f; font-weight: bold;">in</span> s0 <span style="color: #339933;">*/</span>
&nbsp;
vmov s0<span style="color: #339933;">,</span> r0          <span style="color: #339933;">/*</span> Bit copy from integer register r0 to s0 <span style="color: #339933;">*/</span>
vcvt<span style="color: #339933;">.</span>f32<span style="color: #339933;">.</span>s32 s0<span style="color: #339933;">,</span> s0  <span style="color: #339933;">/*</span> Converts s0 signed integer value 
                        to a single<span style="color: #339933;">-</span>precision value <span style="color: #00007f; font-weight: bold;">and</span> stores it <span style="color: #00007f; font-weight: bold;">in</span> s0 <span style="color: #339933;">*/</span>
&nbsp;
vmov s0<span style="color: #339933;">,</span> r0          <span style="color: #339933;">/*</span> Bit copy from integer register r0 to s0 <span style="color: #339933;">*/</span>
vcvt<span style="color: #339933;">.</span>f32<span style="color: #339933;">.</span>u32 s0<span style="color: #339933;">,</span> s0  <span style="color: #339933;">/*</span> Converts s0 unsigned integer value 
                        to a single<span style="color: #339933;">-</span>precision value <span style="color: #00007f; font-weight: bold;">and</span> stores <span style="color: #00007f; font-weight: bold;">in</span> s0 <span style="color: #339933;">*/</span>
&nbsp;
vmov s0<span style="color: #339933;">,</span> r0          <span style="color: #339933;">/*</span> Bit copy from integer register r0 to s0 <span style="color: #339933;">*/</span>
vcvt<span style="color: #339933;">.</span>f64<span style="color: #339933;">.</span>s32 d0<span style="color: #339933;">,</span> s0  <span style="color: #339933;">/*</span> Converts r0 signed integer value 
                        to a double<span style="color: #339933;">-</span>precision value <span style="color: #00007f; font-weight: bold;">and</span> stores <span style="color: #00007f; font-weight: bold;">in</span> d0 <span style="color: #339933;">*/</span>
&nbsp;
vmov s0<span style="color: #339933;">,</span> r0          <span style="color: #339933;">/*</span> Bit copy from integer register r0 to s0 <span style="color: #339933;">*/</span>
vcvt<span style="color: #339933;">.</span>f64<span style="color: #339933;">.</span>u32 d0<span style="color: #339933;">,</span> s0  <span style="color: #339933;">/*</span> Converts s0 unsigned integer value 
                        to a double<span style="color: #339933;">-</span>precision value <span style="color: #00007f; font-weight: bold;">and</span> stores <span style="color: #00007f; font-weight: bold;">in</span> d0 <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">vcvt.f64.f32 d0, s0  /* Converts s0 single-precision value 
                        to a double-precision value and stores it in d0 */

vcvt.f32.f64 s0, d0  /* Converts d0 double-precision value 
                        to a single-precision value  and stores it in s0 */

vmov s0, r0          /* Bit copy from integer register r0 to s0 */
vcvt.f32.s32 s0, s0  /* Converts s0 signed integer value 
                        to a single-precision value and stores it in s0 */

vmov s0, r0          /* Bit copy from integer register r0 to s0 */
vcvt.f32.u32 s0, s0  /* Converts s0 unsigned integer value 
                        to a single-precision value and stores in s0 */

vmov s0, r0          /* Bit copy from integer register r0 to s0 */
vcvt.f64.s32 d0, s0  /* Converts r0 signed integer value 
                        to a double-precision value and stores in d0 */

vmov s0, r0          /* Bit copy from integer register r0 to s0 */
vcvt.f64.u32 d0, s0  /* Converts s0 unsigned integer value 
                        to a double-precision value and stores in d0 */</p></div>

<h2>Modifying fpscr</h2>
<p>
The special register fpscr, where <code>len</code> and <code>stride</code> are set, cannot be modified directly. Instead we have to load fpscr into a general purpose register using <code>vmrs</code> instruction. Then we operate on the register and move it back to the <code>fpscr</code>, using the <code>vmsr</code> instruction.
</p>
<p>
The value of <code>len</code> is stored in bits 16 to 18 of <code>fpscr</code>. The value of <code>len</code> is not directly stored directly in these bits. Instead, we have to subtract 1 before setting the bits. This is because <code>len</code> cannot be 0 (it does not make sense to operate 0 floating points). This way the value <code>000</code> in these bits means <code>len</code> = 1, <code>001</code> means <code>len</code> = 2, …, <code>111</code> means <code>len</code> = 8. The following is a code that sets <code>len</code> to 8.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> Set the len field of fpscr to be <span style="color: #ff0000;">8</span> <span style="color: #009900; font-weight: bold;">(</span><span style="color: #0000ff; font-weight: bold;">bits</span><span style="color: #339933;">:</span> <span style="color: #ff0000;">111</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">7</span>                            <span style="color: #339933;">/*</span> r5 ← <span style="color: #ff0000;">7</span><span style="color: #339933;">.</span> <span style="color: #ff0000;">7</span> is <span style="color: #ff0000;">111</span> <span style="color: #00007f; font-weight: bold;">in</span> binary <span style="color: #339933;">*/</span>
<span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">16</span>                   <span style="color: #339933;">/*</span> r5 ← r5 &lt;&lt; <span style="color: #ff0000;">16</span> <span style="color: #339933;">*/</span>
vmrs r4<span style="color: #339933;">,</span> fpscr                        <span style="color: #339933;">/*</span> r4 ← fpscr <span style="color: #339933;">*/</span>
orr r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> r5                        <span style="color: #339933;">/*</span> r4 ← r4 | r5<span style="color: #339933;">.</span> Bitwise <span style="color: #00007f; font-weight: bold;">OR</span> <span style="color: #339933;">*/</span>
vmsr fpscr<span style="color: #339933;">,</span> r4                        <span style="color: #339933;">/*</span> fpscr ← r4 <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* Set the len field of fpscr to be 8 (bits: 111) */
mov r5, #7                            /* r5 ← 7. 7 is 111 in binary */
mov r5, r5, LSL #16                   /* r5 ← r5 &lt;&lt; 16 */
vmrs r4, fpscr                        /* r4 ← fpscr */
orr r4, r4, r5                        /* r4 ← r4 | r5. Bitwise OR */
vmsr fpscr, r4                        /* fpscr ← r4 */</p></div>

<p>
<code>stride</code> is stored in bits 20 to 21 of <code>fpscr</code>. Similar to <code>len</code>, a value of <code>00</code> in these bits means <code>stride</code> = 1, <code>01</code> means <code>stride</code> = 2, <code>10</code> means <code>stride</code> = 3 and <code>11</code> means <code>stride</code> = 4.
</p>
<h2>Function call convention and floating-point registers</h2>
<p>
Since we have introduced new registers we should state how to use them when calling functions. The following rules apply for VFPv2 registers.
</p>
<ul>
<li>Fields <code>len</code> and <code>stride</code> of <code>fpscr</code> have all their bits as zero at the entry of a function and those bits must be zero when leaving it.
</li><li>We can pass floating point parameters using registers <code>s0</code>–<code>s15</code> and <code>d0</code>–<code>d7</code>. Note that passing a double-precision after a single-precision may involve discarding an odd-numbered single-precision register (for instance we can use <code>s0</code>, and <code>d1</code> but note that <code>s1</code> will be unused).
</li><li>All other floating point registers (<code>s16</code>–<code>s31</code> and <code>d8</code>–<code>d15</code>) must have their values preserved upon leaving the function. Instructions <code>vpush</code> and <code>vpop</code> can be used for that.
</li><li>If a function returns a floating-point value, the return register will be <code>s0</code> or <code>d0</code>.
</li></ul>
<p>
Finally a note about variadic functions like printf: you cannot pass a single-precision floating point to one of such functions. Only doubles can be passed. So you will need to convert the single-precision values into double-precision values. Note also that usual integer registers are used (<code>r0</code>–<code>r3</code>), so you will only be able to pass up to 2 double-precision values, the remaining must be passed on the stack. In particular for <code>printf</code>, since <code>r0</code> contains the address of the string format, you will only be able to pass a double-precision in <code>{r2,r3}</code>.
</p>
<h2>Assembler</h2>
<p>
Make sure you pass the flag <code>-mfpu=vfpv2</code> to <code>as</code>, otherwise it will not recognize the VFPv2 instructions.
</p>
<h2>Colophon</h2>
<p>
You may want to check this official <a href="http://infocenter.arm.com/help/topic/com.arm.doc.qrc0007e/QRC0007_VFP.pdf">quick reference card of VFP</a>. Note that it includes also VFPv3 not available in the Raspberry Pi processor. Most of what is there has already been presented here although some minor details may have been omitted.
</p>
<p>
In the next chapter we will use these instructions in a full example.
</p>
<p>
That’s all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/&amp;text=ARM+assembler+in+Raspberry+Pi+%E2%80%93+Chapter+13+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>						<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2013/04/27/capybara-pop-windows-paypal-sandbox/" rel="prev">Capybara, pop up windows and the new PayPal sandbox</a></span>
			<span class="next"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-14/" rel="next">ARM assembler in Raspberry Pi – Chapter 14</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
			<h3>22 thoughts on “<span>ARM assembler in Raspberry Pi – Chapter 13</span>”</h3>
		<ul class="commentlist">
				<li class="comment even thread-even depth-1" id="comment-1292">
				<div id="div-comment-1292" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/d343022f7066553235d373c4014f0c7c?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/d343022f7066553235d373c4014f0c7c?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/d343022f7066553235d373c4014f0c7c?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-d343022f7066553235d373c4014f0c7c-0">			<cite class="fn"><a href="http://www.fourtheye.org" rel="external nofollow" class="url">Bob Wilkinson</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/#comment-1292">
			May 14, 2013 at 12:20 pm</a>		</div>

		<p>I am enjoying the series – I seem to read about things here just after I have been looking into them 
        </p>
<p>” The address of the load/store must be already in a general purpose register, although we can apply an offset in bytes which must be a multiple of 4 (this applies to double-precision as well). ”</p>
<p>Would the offset not need to be a multiple of 8 for double-precision?</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/?replytocom=1292#respond" onclick="return addComment.moveForm( &quot;div-comment-1292&quot;, &quot;1292&quot;, &quot;respond&quot;, &quot;911&quot; )" aria-label="Reply to Bob Wilkinson">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1 parent" id="comment-1293">
				<div id="div-comment-1293" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/d343022f7066553235d373c4014f0c7c?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/d343022f7066553235d373c4014f0c7c?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/d343022f7066553235d373c4014f0c7c?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-d343022f7066553235d373c4014f0c7c-1">			<cite class="fn"><a href="http://www.fourtheye.org" rel="external nofollow" class="url">Bob Wilkinson</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/#comment-1293">
			May 14, 2013 at 12:33 pm</a>		</div>

		<p>111 means lean = 8 – should be len=8.</p>
<p>If I can answer my previous question, I guess that the *address* of the double is a 32-bit int, so an offset of 4 is valid?</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/?replytocom=1293#respond" onclick="return addComment.moveForm( &quot;div-comment-1293&quot;, &quot;1293&quot;, &quot;respond&quot;, &quot;911&quot; )" aria-label="Reply to Bob Wilkinson">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2" id="comment-1294">
				<div id="div-comment-1294" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-0">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/#comment-1294">
			May 14, 2013 at 2:32 pm</a>		</div>

		<p>Thanks, I fixed that lean that slipped in.</p>
<p>Regarding to your question: a double-precision should be 8-byte aligned per the AAPCS but, as far I’ve tested, a <code>vldr</code> or <code>vstr</code> do not seem to care very much about this constraint. If we follow the AAPCS our double-precision would always be 8-bytes aligned.</p>
<p>That alignment issue, though, is orthogonal to the offset itself. <code>vstr</code> and <code>vldr</code> are actually ARM generic coprocessor instructions with an appropiate 10 or 11 identifier for the coprocessor. Such generic instructions define an offset that must be a multiple of 4. Imagine we have <code>r1 ← 0x104</code> and the instruction is <code>vldr d0, [r1, #4]</code>, the effect will be <code>d0 ← *(r1 + #4)</code> so <code>d0 ← *(0x108)</code> which is 8-byte aligned. This would still be compliant with the AAPCS.</p>
<p>Hope this answers your question </p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/?replytocom=1294#respond" onclick="return addComment.moveForm( &quot;div-comment-1294&quot;, &quot;1294&quot;, &quot;respond&quot;, &quot;911&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-even depth-1 parent" id="comment-1493">
				<div id="div-comment-1493" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/3232b400aed767dbd68054fca2148bb9?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/3232b400aed767dbd68054fca2148bb9?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/3232b400aed767dbd68054fca2148bb9?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-3232b400aed767dbd68054fca2148bb9-0">			<cite class="fn">Elie De Brauwer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/#comment-1493">
			August 15, 2013 at 6:16 pm</a>		</div>

		<p>One comment, you are giving the following example: “vcvt.f32.u32 s4, r3” however the assembler says:</p>
<p>Error: VFP single, double or Neon quad precision register expected — `vcvt.f32.u32 s4,r3′</p>
<p>Also  vcvt is specified as in the VFP Quick Reference cad as VCVT{C}.U32 Fd, Sm  so I think that the example of converting u32 to f32 should be a two step thing like:<br>
vmov s4, r4<br>
vcvt.f32.u32 s4, s4</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/?replytocom=1493#respond" onclick="return addComment.moveForm( &quot;div-comment-1493&quot;, &quot;1493&quot;, &quot;respond&quot;, &quot;911&quot; )" aria-label="Reply to Elie De Brauwer">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2" id="comment-1494">
				<div id="div-comment-1494" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-1">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/#comment-1494">
			August 15, 2013 at 9:00 pm</a>		</div>

		<p>Hi Elie,</p>
<p>I fixed that in the post.</p>
<p>Thank you very much!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/?replytocom=1494#respond" onclick="return addComment.moveForm( &quot;div-comment-1494&quot;, &quot;1494&quot;, &quot;respond&quot;, &quot;911&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1 parent" id="comment-2815">
				<div id="div-comment-2815" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/aa649450777ff2f5dbf16b0a2734321c?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/aa649450777ff2f5dbf16b0a2734321c?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/aa649450777ff2f5dbf16b0a2734321c?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-aa649450777ff2f5dbf16b0a2734321c-0">			<cite class="fn">Henryk Sarat</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/#comment-2815">
			January 7, 2014 at 9:11 am</a>		</div>

		<p>Double point precision numbers consist of:<br>
1 bit for the sign, 11 bits for the exponent, and 52 bits for the mantissa.</p>
<p>This tripped me up for a bit. Please correct so no one else is confused!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/?replytocom=2815#respond" onclick="return addComment.moveForm( &quot;div-comment-2815&quot;, &quot;2815&quot;, &quot;respond&quot;, &quot;911&quot; )" aria-label="Reply to Henryk Sarat">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2" id="comment-2816">
				<div id="div-comment-2816" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-2">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/#comment-2816">
			January 7, 2014 at 9:21 am</a>		</div>

		<p>Oops. Fixed. Thanks!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/?replytocom=2816#respond" onclick="return addComment.moveForm( &quot;div-comment-2816&quot;, &quot;2816&quot;, &quot;respond&quot;, &quot;911&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-even depth-1 parent" id="comment-3488">
				<div id="div-comment-3488" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/9805f4a2be4e3be4c489773f4e52b2e6?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/9805f4a2be4e3be4c489773f4e52b2e6?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/9805f4a2be4e3be4c489773f4e52b2e6?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-9805f4a2be4e3be4c489773f4e52b2e6-0">			<cite class="fn">RobG</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/#comment-3488">
			January 22, 2014 at 11:49 pm</a>		</div>

		<p>A great series of posts, has been very helpful understanding my new RPi. </p>
<p>It would be really helpful to newcomers for each post to include links to the next and previous – this works well for 1-12, but I only just realised there were more!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/?replytocom=3488#respond" onclick="return addComment.moveForm( &quot;div-comment-3488&quot;, &quot;3488&quot;, &quot;respond&quot;, &quot;911&quot; )" aria-label="Reply to RobG">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2 parent" id="comment-3522">
				<div id="div-comment-3522" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-3">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/#comment-3522">
			January 23, 2014 at 10:56 am</a>		</div>

		<p>Hi RobG,</p>
<p>I’ll try to fix though I’m not an ace in this wordpress thingy! </p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/?replytocom=3522#respond" onclick="return addComment.moveForm( &quot;div-comment-3522&quot;, &quot;3522&quot;, &quot;respond&quot;, &quot;911&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment odd alt depth-3" id="comment-3536">
				<div id="div-comment-3536" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/9805f4a2be4e3be4c489773f4e52b2e6?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/9805f4a2be4e3be4c489773f4e52b2e6?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/9805f4a2be4e3be4c489773f4e52b2e6?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-9805f4a2be4e3be4c489773f4e52b2e6-1">			<cite class="fn">RobG</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/#comment-3536">
			January 23, 2014 at 11:39 pm</a>		</div>

		<p>Excellent.</p>
<p>With help from this series and Cambridge tutorials, I’ve just managed to boot my PI into custom code. (I’d hesitate to call it an O/S yet, but I’ve got as far as a (very) basic CLI. </p>
<p>Many thanks – I found it hard getting useful info from the ARM documentation – had to keep cross-referencing the ARMv7 and ARM11 documents. This brings it all together nicely and reminded me of some stuff I’ve not had do manually for decades – (ZX Spectrum – I used to literally hand code z80 assembly to hex for that – this is much less painful).</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/?replytocom=3536#respond" onclick="return addComment.moveForm( &quot;div-comment-3536&quot;, &quot;3536&quot;, &quot;respond&quot;, &quot;911&quot; )" aria-label="Reply to RobG">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-902586">
				<div id="div-comment-902586" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/924103b0b5766856300afb21d73b6656?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/924103b0b5766856300afb21d73b6656?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/924103b0b5766856300afb21d73b6656?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-924103b0b5766856300afb21d73b6656-0">			<cite class="fn">janan</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/#comment-902586">
			July 16, 2015 at 2:58 pm</a>		</div>

		<p>thanks you , please can you tell me what is the diference between Vector Floating-point v2 (VFPv2) , and floating point unit …<br>
and how to use the floating point support …<br>
i’m using raspberry pi 2 under raspbian</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/?replytocom=902586#respond" onclick="return addComment.moveForm( &quot;div-comment-902586&quot;, &quot;902586&quot;, &quot;respond&quot;, &quot;911&quot; )" aria-label="Reply to janan">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-914391">
				<div id="div-comment-914391" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-4">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/#comment-914391">
			August 17, 2015 at 7:17 am</a>		</div>

		<p>Hi janan,</p>
<p>a floating point unit (FPU) is a generic name for a coprocessor or (more commonly nowadays) a functional unit inside a processor that performs floating point operations.</p>
<p>VFPv2 is the name ARM gives to an extension to the ARM architecture that provides floating point support in hardware. Note that, while the architectural “interface” to the VFPv2 is via the coprocessor mechanism, this does not preclude that it can be integrated in a single chip.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/?replytocom=914391#respond" onclick="return addComment.moveForm( &quot;div-comment-914391&quot;, &quot;914391&quot;, &quot;respond&quot;, &quot;911&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1 parent" id="comment-955370">
				<div id="div-comment-955370" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/8e02ba12a3b8f9d19306c56fbde0d455?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/8e02ba12a3b8f9d19306c56fbde0d455?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/8e02ba12a3b8f9d19306c56fbde0d455?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-8e02ba12a3b8f9d19306c56fbde0d455-0">			<cite class="fn"><a href="http://www.utdallas.edu/~pervin" rel="external nofollow" class="url">William Pervin</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/#comment-955370">
			December 18, 2015 at 10:43 pm</a>		</div>

		<p>“These are not 48 different registers. Instead every dn is mapped to two consecutive sn and sn+1, where n is an even number lower than 31.” </p>
<p>This should be corrected to: “every d_n is mapped to two consecutive registers s_2n and s_2n+1, where 0 &lt;= n &lt;= 15."</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/?replytocom=955370#respond" onclick="return addComment.moveForm( &quot;div-comment-955370&quot;, &quot;955370&quot;, &quot;respond&quot;, &quot;911&quot; )" aria-label="Reply to William Pervin">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-955420">
				<div id="div-comment-955420" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-5">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/#comment-955420">
			December 19, 2015 at 10:27 am</a>		</div>

		<p>Hi William,</p>
<p>thanks a lot for the suggestion. I applied it to the text.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/?replytocom=955420#respond" onclick="return addComment.moveForm( &quot;div-comment-955420&quot;, &quot;955420&quot;, &quot;respond&quot;, &quot;911&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-965532">
				<div id="div-comment-965532" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/4608f7354ffe7781d0ed871964fd0366?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/4608f7354ffe7781d0ed871964fd0366?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/4608f7354ffe7781d0ed871964fd0366?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-4608f7354ffe7781d0ed871964fd0366-0">			<cite class="fn">Sam Ferguson</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/#comment-965532">
			April 26, 2016 at 11:28 pm</a>		</div>

		<p>Will VFP instructions assemble from the terminal like earlier examples? I am getting an Error: selected processor does not support ARM mode `add.f32 s1, s2, s2′<br>
Just wondering if this is a limitation or something I have not setup correctly.</p>
<p>Really enjoying the tutorials, thank you very much!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/?replytocom=965532#respond" onclick="return addComment.moveForm( &quot;div-comment-965532&quot;, &quot;965532&quot;, &quot;respond&quot;, &quot;911&quot; )" aria-label="Reply to Sam Ferguson">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-969148">
				<div id="div-comment-969148" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-6">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/#comment-969148">
			June 4, 2016 at 7:34 pm</a>		</div>

		<p>Hi Sam,</p>
<p>the instruction is called <code>vadd.f32</code> but likely you forgot to pass <code>-mfpu=vfpv2</code>.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/?replytocom=969148#respond" onclick="return addComment.moveForm( &quot;div-comment-969148&quot;, &quot;969148&quot;, &quot;respond&quot;, &quot;911&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1 parent" id="comment-971875">
				<div id="div-comment-971875" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/e6a7ed778a0e8d9f284c28e119f1d212?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/e6a7ed778a0e8d9f284c28e119f1d212?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/e6a7ed778a0e8d9f284c28e119f1d212?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-e6a7ed778a0e8d9f284c28e119f1d212-0">			<cite class="fn">Andreas Madsen</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/#comment-971875">
			July 24, 2016 at 2:27 pm</a>		</div>

		<p>“Most VFPv2 instructions are of the form fname Rdest, Rsource1, Rsource2 or fname Rdest, Rsource1.”</p>
<p>but it appears that the instructions are prefixed with “v” and not “f”. Did I misunderstand the notation?</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/?replytocom=971875#respond" onclick="return addComment.moveForm( &quot;div-comment-971875&quot;, &quot;971875&quot;, &quot;respond&quot;, &quot;911&quot; )" aria-label="Reply to Andreas Madsen">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-972057">
				<div id="div-comment-972057" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-7">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/#comment-972057">
			July 29, 2016 at 7:02 pm</a>		</div>

		<p>Hi Andreas,</p>
<p>no you did not. It was a mistake of mine. I have already fixed the post.</p>
<p>Thank you!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/?replytocom=972057#respond" onclick="return addComment.moveForm( &quot;div-comment-972057&quot;, &quot;972057&quot;, &quot;respond&quot;, &quot;911&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-984244">
				<div id="div-comment-984244" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/65783b7260e0e49601d1f864eb668fba?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/65783b7260e0e49601d1f864eb668fba?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/65783b7260e0e49601d1f864eb668fba?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-65783b7260e0e49601d1f864eb668fba-0">			<cite class="fn">zair</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/#comment-984244">
			January 16, 2017 at 4:17 pm</a>		</div>

		<p>Great work of sharing things, Was able to follow up and experiment.<br>
I was experimenting the code with intrinsics/ACLE,<br>
vcvt_u32_f32 did not do the intended job, maybe my understanding of vcvt for converting floating point to fixed point is not clear.</p>
<p>— snip—<br>
const float temp[2] = {2.84};<br>
float32x2_t z = vld1_f32(temp);<br>
uint32x2_t in = vcvt_u32_f32(z);</p>
<p>y[0] = vget_lane_u32(in, 0);<br>
y[1] = vget_lane_u32(in, 1);</p>
<p>— end of snip —</p>
<p>It would be a great help if i get through getting 3 floating point parts in separate 3 integers.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/?replytocom=984244#respond" onclick="return addComment.moveForm( &quot;div-comment-984244&quot;, &quot;984244&quot;, &quot;respond&quot;, &quot;911&quot; )" aria-label="Reply to zair">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-985259">
				<div id="div-comment-985259" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-8">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/#comment-985259">
			January 28, 2017 at 7:38 pm</a>		</div>

		<p>Hi,</p>
<p>thanks!</p>
<p>I’m not expert in ACLE and I’m also not sure what you mean by “getting 3 floating point parts in separate 3 integers”? Is there a reason you are using types that apparently only encode 2 floats? And finally, these intrinsics I think they are only for NEON. But the Raspberry Pi model used in this tutorial does not support NEON.</p>
<p>Kind regards,<br>
Roger</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/?replytocom=985259#respond" onclick="return addComment.moveForm( &quot;div-comment-985259&quot;, &quot;985259&quot;, &quot;respond&quot;, &quot;911&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1 parent" id="comment-987355">
				<div id="div-comment-987355" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/5301f9fac283ef87f1f4742db878c8e3?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/5301f9fac283ef87f1f4742db878c8e3?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-5301f9fac283ef87f1f4742db878c8e3-0" originals="32" src-orig="http://2.gravatar.com/avatar/5301f9fac283ef87f1f4742db878c8e3?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Rohan</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/#comment-987355">
			February 24, 2017 at 12:11 am</a>		</div>

		<p>Hello Mr. Ibáñez,</p>
<blockquote><p>Fields <code>len</code> and <code>stride</code> of <code>fpscr</code> are zero at the entry of a function and must be zero when leaving it.</p></blockquote>
<p><code>len</code> and <code>stride</code> cannot be zero, so did you mean to write one instead?</p>
<p>Thank you</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/?replytocom=987355#respond" onclick="return addComment.moveForm( &quot;div-comment-987355&quot;, &quot;987355&quot;, &quot;respond&quot;, &quot;911&quot; )" aria-label="Reply to Rohan">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-987489">
				<div id="div-comment-987489" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-9">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/#comment-987489">
			February 25, 2017 at 10:40 pm</a>		</div>

		<p>Hi Rohan,</p>
<p>I was a bit unclear here: you’re right in that they cannot be logically zero. But because of that a physical zero means one. I will update the text.</p>
<p>Thank you.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/05/12/arm-assembler-raspberry-pi-chapter-13/?replytocom=987489#respond" onclick="return addComment.moveForm( &quot;div-comment-987489&quot;, &quot;987489&quot;, &quot;respond&quot;, &quot;911&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		</ul>
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2013/05/12/arm-assembler-raspberry-pi-chapter-13/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="911" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="3e63a950bf"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496665760240"></form>
			</div><!-- #respond -->
	</div>	</div>
