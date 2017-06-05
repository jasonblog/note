# Integer division


<div class="main">
			<h1>ARM assembler in Raspberry Pi – Chapter 15 </h1>
		<p class="post-meta"><span class="date">August 11, 2013</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2013/08/11/arm-assembler-raspberry-pi-chapter-15/#comments">10</a></span> </p>
		<p>
It may be suprising, but the ARMv6 architecture does not provide an integer division instruction while it does have a floating point instruction in VFPv2. In this chapter we will see usual ways to workaround this limitation with different techniques that can be used in specific scenarios involving divisions.
</p>
<p><span id="more-1109"></span></p>
<h2>What does integer division mean?</h2>
<p>
First we should clearly define what we understand by integer division. Given two integer numbers N (for numerator) and D (for denominator, different than zero) we define the <em>integer division</em> of N and D as the pair of integer numbers Q (for quotient) and R (for remainder) satisfying the following equality.
</p>
<p style="text-align: center;">
<code>N = D × Q + R</code> where <code>0 ≤ |R| &lt; |D|</code>
</p>
<p>
The equality implies that there are two solutions <code>0 &lt; R &lt; |D|</code> and <code>0 &lt; |-R| &lt; |D|</code>. For example, <code>N=7</code> and <code>D=3</code> has two solutions <code>(Q=2, R=1)</code> and <code>(Q=3, R=-2)</code>. While both solutions may be useful, the former one is the preferred as it is closer to our natural notion of the remainder. But what if <code>D</code> is negative? For example <code>N=7</code> and <code>D=-3</code> has two solutions as well <code>(Q=-2, R=1)</code> and <code>(Q=-3, R=-2)</code>. When negative numbers are involved the choice of the remainder is not intuitive but conventional. Many conventions can be used to choose one solution. We can always pick the solution with the positive remainder (this is called <em>euclidean division</em>), or the negative, or the solution where the sign of the remainder matches the numerator (or the denominator).
</p>
<p>
Most computers perform an integer division where the remainder has the same sign as the numerator. So for <code>N=7</code> and <code>D=3</code> the computed solution is <code>(Q=2, R=1)</code> and for <code>N=7</code> and <code>D=-3</code> the computed solution is <code>(Q=-2, R=1)</code>. We will assume such integer division convention in the remainder (no pun intended) of this post.
</p>
<h2>Unsigned division</h2>
<p>
An unsigned integer division is an integer division involving two unsigned integers N and D. This has the consequence that Q and R will always be positive. A very naive (and slow) approach for unsigned division is as follows.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">unsigned_naive_div<span style="color: #339933;">:</span>
    <span style="color: #339933;">/*</span> r0 contains N <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r1 contains D <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r2<span style="color: #339933;">,</span> r1             <span style="color: #339933;">/*</span> r2 ← r0<span style="color: #339933;">.</span> We keep D <span style="color: #00007f; font-weight: bold;">in</span> r2 <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> r0             <span style="color: #339933;">/*</span> r1 ← r0<span style="color: #339933;">.</span> We keep N <span style="color: #00007f; font-weight: bold;">in</span> r1 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>             <span style="color: #339933;">/*</span> r0 ← <span style="color: #ff0000;">0</span><span style="color: #339933;">.</span> Set Q = <span style="color: #ff0000;">0</span> initially <span style="color: #339933;">*/</span>
&nbsp;
    b <span style="color: #339933;">.</span>Lloop_check
    <span style="color: #339933;">.</span>Lloop<span style="color: #339933;">:</span>
       <span style="color: #00007f; font-weight: bold;">add</span> r0<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>      <span style="color: #339933;">/*</span> r0 ← r0 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span><span style="color: #339933;">.</span> Q = Q <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
       <span style="color: #00007f; font-weight: bold;">sub</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> r2      <span style="color: #339933;">/*</span> r1 ← r1 <span style="color: #339933;">-</span> r2 <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>Lloop_check<span style="color: #339933;">:</span>
       <span style="color: #00007f; font-weight: bold;">cmp</span> r1<span style="color: #339933;">,</span> r2          <span style="color: #339933;">/*</span> compute r1 <span style="color: #339933;">-</span> r2 <span style="color: #339933;">*/</span>
       bhs <span style="color: #339933;">.</span>Lloop            <span style="color: #339933;">/*</span> branch if r1 &gt;= r2 <span style="color: #009900; font-weight: bold;">(</span>C=<span style="color: #ff0000;">0</span> <span style="color: #00007f; font-weight: bold;">or</span> Z=<span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> r0 already contains Q <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r1 already contains R <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">unsigned_naive_div:
    /* r0 contains N */
    /* r1 contains D */
    mov r2, r1             /* r2 ← r0. We keep D in r2 */
    mov r1, r0             /* r1 ← r0. We keep N in r1 */

    mov r0, #0             /* r0 ← 0. Set Q = 0 initially */

    b .Lloop_check
    .Lloop:
       add r0, r0, #1      /* r0 ← r0 + 1. Q = Q + 1 */
       sub r1, r1, r2      /* r1 ← r1 - r2 */
    .Lloop_check:
       cmp r1, r2          /* compute r1 - r2 */
       bhs .Lloop            /* branch if r1 &gt;= r2 (C=0 or Z=1) */

    /* r0 already contains Q */
    /* r1 already contains R */
    bx lr</p></div>

<p>
This algorithm, while correct and easy to understand, is not very efficient (think on dividing a big N with a small D). Is there a way that we can compute the division in a fixed amount of time? The answer is yes, just adapt how you divide by hand but to binary numbers. We will compute a temporary remainder picking bits, from left to right, of the dividend. When the remainder is larger than the divisor, we will subtract the divisor from that remainder and set the appropiate bit in the quotient.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">unsigned_longdiv<span style="color: #339933;">:</span>
    <span style="color: #339933;">/*</span> r0 contains N <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r1 contains D <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r2 contains Q <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r3 contains R <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>                 <span style="color: #339933;">/*</span> r2 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r3<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>                 <span style="color: #339933;">/*</span> r3 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">32</span>                <span style="color: #339933;">/*</span> r4 ← <span style="color: #ff0000;">32</span> <span style="color: #339933;">*/</span>
    b <span style="color: #339933;">.</span>Lloop_check1
    <span style="color: #339933;">.</span>Lloop1<span style="color: #339933;">:</span>
        <span style="color: #00007f; font-weight: bold;">movs</span> r0<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">1</span>    <span style="color: #339933;">/*</span> r0 ← r0 &lt;&lt; <span style="color: #ff0000;">1</span> updating cpsr <span style="color: #009900; font-weight: bold;">(</span><span style="color: #00007f; font-weight: bold;">sets</span> C if 31st bit of r0 was <span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">adc</span> r3<span style="color: #339933;">,</span> r3<span style="color: #339933;">,</span> r3         <span style="color: #339933;">/*</span> r3 ← r3 <span style="color: #339933;">+</span> r3 <span style="color: #339933;">+</span> C<span style="color: #339933;">.</span> This is equivalent to r3 ← <span style="color: #009900; font-weight: bold;">(</span>r3 &lt;&lt; <span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">+</span> C <span style="color: #339933;">*/</span>
&nbsp;
        <span style="color: #00007f; font-weight: bold;">cmp</span> r3<span style="color: #339933;">,</span> r1             <span style="color: #339933;">/*</span> compute r3 <span style="color: #339933;">-</span> r1 <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
        subhs r3<span style="color: #339933;">,</span> r3<span style="color: #339933;">,</span> r1       <span style="color: #339933;">/*</span> if r3 &gt;= r1 <span style="color: #009900; font-weight: bold;">(</span>C=<span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">)</span> then r3 ← r3 <span style="color: #339933;">-</span> r1 <span style="color: #339933;">*/</span>
        <span style="color: #00007f; font-weight: bold;">adc</span> r2<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r2         <span style="color: #339933;">/*</span> r2 ← r2 <span style="color: #339933;">+</span> r2 <span style="color: #339933;">+</span> C<span style="color: #339933;">.</span> This is equivalent to r2 ← <span style="color: #009900; font-weight: bold;">(</span>r2 &lt;&lt; <span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">+</span> C <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>Lloop_check1<span style="color: #339933;">:</span>
        subs r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>        <span style="color: #339933;">/*</span> r4 ← r4 <span style="color: #339933;">-</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
        <span style="color: #46aa03; font-weight: bold;">bpl</span> <span style="color: #339933;">.</span>Lloop1            <span style="color: #339933;">/*</span> if r4 &gt;= <span style="color: #ff0000;">0</span> <span style="color: #009900; font-weight: bold;">(</span>N=<span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">)</span> then branch to <span style="color: #339933;">.</span>Lloop1 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">unsigned_longdiv:
    /* r0 contains N */
    /* r1 contains D */
    /* r2 contains Q */
    /* r3 contains R */
    push {r4, lr}
    mov r2, #0                 /* r2 ← 0 */
    mov r3, #0                 /* r3 ← 0 */

    mov r4, #32                /* r4 ← 32 */
    b .Lloop_check1
    .Lloop1:
        movs r0, r0, LSL #1    /* r0 ← r0 &lt;&lt; 1 updating cpsr (sets C if 31st bit of r0 was 1) */
        adc r3, r3, r3         /* r3 ← r3 + r3 + C. This is equivalent to r3 ← (r3 &lt;&lt; 1) + C */

        cmp r3, r1             /* compute r3 - r1 and update cpsr */
        subhs r3, r3, r1       /* if r3 &gt;= r1 (C=1) then r3 ← r3 - r1 */
        adc r2, r2, r2         /* r2 ← r2 + r2 + C. This is equivalent to r2 ← (r2 &lt;&lt; 1) + C */
    .Lloop_check1:
        subs r4, r4, #1        /* r4 ← r4 - 1 */
        bpl .Lloop1            /* if r4 &gt;= 0 (N=0) then branch to .Lloop1 */

    pop {r4, lr}
    bx lr</p></div>

<p>
This approach is a bit more efficient since it repeats the loop a fixed number of times (always 32). For each bit of N starting from the most significant one (line 13), we push it to the right of the current value of R (line 14). We do this by adding R to itself, this is 2*R which is actually shifting to the right R 1 bit. Then we add the carry, that will be 1 if the most significant bit of N before the shift (line 13) was 1. Then we check if the current R is already bigger than D (line 16) If so we subtract N from R, R ← R - N (line 17) and then we push a 1 to the right of Q (line 18), again by adding Q to itself plus the carry set by the comparison (if R &gt;= N then there is no borrow so C became 1 in <code>cmp</code> of line 16).
</p>
<p>
The shown code is fine but it can be improved in a number of ways. First, there is no need to check all the bits of a number (although this gives as an upper bound of the cost in the worst of the cases). Second, we should try hard to reduce the number of used registers. Here we are using 5 registers, is there a way we can use less registers? For this we will have to use a slightly different approach.
</p>
<p>
Given N and D, we will first shift D as many bits to the left as possible but always having N &gt; D. So, for instance if we divide N=1010<sub>(2</sub> by D=10<sub>(2</sub> we would adjust D until it was D<sub>0</sub>=1000<sub>(2</sub> (this is shifting twice to the left). Now we start a similar process to the one above: if N<sub>i</sub> ≥ D<sub>i</sub>, we set 1 to the lowest bit of Q and then we compute a new N<sub>i+1</sub> ← N<sub>i</sub> - D<sub>i</sub> and a new D<sub>i+1</sub> ← D<sub>i</sub>/2. If N<sub>i</sub> &lt; D<sub>i</sub> then we simply compute a new D<sub>i+1</sub> ← D<sub>i</sub>/2. We stop when the current D<sub>i</sub> is smaller than the initial D (not D<sub>0</sub>). Note that this condition is what makes dividing N=1010<sub>(2</sub> by D=10<sub>(2</sub> different that dividing N=1010<sub>(2</sub> by D=1<sub>(2</sub> although the D<sub>0</sub> of both cases is the same.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">better_unsigned_division <span style="color: #339933;">:</span>
    <span style="color: #339933;">/*</span> r0 contains N <span style="color: #00007f; font-weight: bold;">and</span> Ni <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r1 contains D <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r2 contains Q <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r3 will contain <span style="color: #46aa03; font-weight: bold;">Di</span> <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r3<span style="color: #339933;">,</span> r1                   <span style="color: #339933;">/*</span> r3 ← r1 <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">cmp</span> r3<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> LSR #<span style="color: #ff0000;">1</span>           <span style="color: #339933;">/*</span> update cpsr with r3 <span style="color: #339933;">-</span> r0<span style="color: #339933;">/</span><span style="color: #ff0000;">2</span> <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>Lloop2<span style="color: #339933;">:</span>
      movls r3<span style="color: #339933;">,</span> r3<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">1</span>       <span style="color: #339933;">/*</span> if r3 &lt;= <span style="color: #ff0000;">2</span><span style="color: #339933;">*</span>r0 <span style="color: #009900; font-weight: bold;">(</span>C=<span style="color: #ff0000;">0</span> <span style="color: #00007f; font-weight: bold;">or</span> Z=<span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">)</span> then r3 ← r3<span style="color: #339933;">*</span><span style="color: #ff0000;">2</span> <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">cmp</span> r3<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> LSR #<span style="color: #ff0000;">1</span>         <span style="color: #339933;">/*</span> update cpsr with r3 <span style="color: #339933;">-</span> <span style="color: #009900; font-weight: bold;">(</span>r0<span style="color: #339933;">/</span><span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
      bls <span style="color: #339933;">.</span>Lloop2                <span style="color: #339933;">/*</span> branch to <span style="color: #339933;">.</span>Lloop2 if r3 &lt;= <span style="color: #ff0000;">2</span><span style="color: #339933;">*</span>r0 <span style="color: #009900; font-weight: bold;">(</span>C=<span style="color: #ff0000;">0</span> <span style="color: #00007f; font-weight: bold;">or</span> Z=<span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>                   <span style="color: #339933;">/*</span> r2 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">.</span>Lloop3<span style="color: #339933;">:</span>
      <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> r3                 <span style="color: #339933;">/*</span> update cpsr with r0 <span style="color: #339933;">-</span> r3 <span style="color: #339933;">*/</span>
      subhs r0<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> r3           <span style="color: #339933;">/*</span> if r0 &gt;= r3 <span style="color: #009900; font-weight: bold;">(</span>C=<span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">)</span> then r0 ← r0 <span style="color: #339933;">-</span> r3 <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">adc</span> r2<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r2             <span style="color: #339933;">/*</span> r2 ← r2 <span style="color: #339933;">+</span> r2 <span style="color: #339933;">+</span> C<span style="color: #339933;">.</span>
                                    Note that if r0 &gt;= r3 then C=<span style="color: #ff0000;">1</span><span style="color: #339933;">,</span> C=<span style="color: #ff0000;">0</span> otherwise <span style="color: #339933;">*/</span>
&nbsp;
      <span style="color: #00007f; font-weight: bold;">mov</span> r3<span style="color: #339933;">,</span> r3<span style="color: #339933;">,</span> LSR #<span style="color: #ff0000;">1</span>         <span style="color: #339933;">/*</span> r3 ← r3<span style="color: #339933;">/</span><span style="color: #ff0000;">2</span> <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">cmp</span> r3<span style="color: #339933;">,</span> r1                 <span style="color: #339933;">/*</span> update cpsr with r3 <span style="color: #339933;">-</span> r1 <span style="color: #339933;">*/</span>
      bhs <span style="color: #339933;">.</span>Lloop3                <span style="color: #339933;">/*</span> if r3 &gt;= r1 branch to <span style="color: #339933;">.</span>Lloop3 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">better_unsigned_division :
    /* r0 contains N and Ni */
    /* r1 contains D */
    /* r2 contains Q */
    /* r3 will contain Di */

    mov r3, r1                   /* r3 ← r1 */
    cmp r3, r0, LSR #1           /* update cpsr with r3 - r0/2 */
    .Lloop2:
      movls r3, r3, LSL #1       /* if r3 &lt;= 2*r0 (C=0 or Z=1) then r3 ← r3*2 */
      cmp r3, r0, LSR #1         /* update cpsr with r3 - (r0/2) */
      bls .Lloop2                /* branch to .Lloop2 if r3 &lt;= 2*r0 (C=0 or Z=1) */

    mov r2, #0                   /* r2 ← 0 */

    .Lloop3:
      cmp r0, r3                 /* update cpsr with r0 - r3 */
      subhs r0, r0, r3           /* if r0 &gt;= r3 (C=1) then r0 ← r0 - r3 */
      adc r2, r2, r2             /* r2 ← r2 + r2 + C.
                                    Note that if r0 &gt;= r3 then C=1, C=0 otherwise */

      mov r3, r3, LSR #1         /* r3 ← r3/2 */
      cmp r3, r1                 /* update cpsr with r3 - r1 */
      bhs .Lloop3                /* if r3 &gt;= r1 branch to .Lloop3 */
   
    bx lr</p></div>

<p>
We can avoid the first loop where we shift until we exceed by counting the <em>leading zeroes</em>. By counting the leading zeroes of the dividend and the divisor we can straightforwardly compute how many bits we need to shift the divisor.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">clz_unsigned_division<span style="color: #339933;">:</span>
    clz  r3<span style="color: #339933;">,</span> r0                <span style="color: #339933;">/*</span> r3 ← CLZ<span style="color: #009900; font-weight: bold;">(</span>r0<span style="color: #009900; font-weight: bold;">)</span> Count leading zeroes of N <span style="color: #339933;">*/</span>
    clz  r2<span style="color: #339933;">,</span> r1                <span style="color: #339933;">/*</span> r2 ← CLZ<span style="color: #009900; font-weight: bold;">(</span>r1<span style="color: #009900; font-weight: bold;">)</span> Count leading zeroes of D <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">sub</span>  r3<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r3            <span style="color: #339933;">/*</span> r3 ← r2 <span style="color: #339933;">-</span> r3<span style="color: #339933;">.</span> 
                                 This is the difference of zeroes
                                 between D <span style="color: #00007f; font-weight: bold;">and</span> N<span style="color: #339933;">.</span> 
                                 Note that N &gt;= D implies CLZ<span style="color: #009900; font-weight: bold;">(</span>N<span style="color: #009900; font-weight: bold;">)</span> &lt;= CLZ<span style="color: #009900; font-weight: bold;">(</span>D<span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r3<span style="color: #339933;">,</span> r3<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>             <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Loop</span> below needs an extra iteration count <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>                 <span style="color: #339933;">/*</span> r2 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    b <span style="color: #339933;">.</span>Lloop_check4
    <span style="color: #339933;">.</span>Lloop4<span style="color: #339933;">:</span>
      <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">lsl</span> r3       <span style="color: #339933;">/*</span> Compute r0 <span style="color: #339933;">-</span> <span style="color: #009900; font-weight: bold;">(</span>r1 &lt;&lt; r3<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
      <span style="color: #00007f; font-weight: bold;">adc</span> r2<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r2           <span style="color: #339933;">/*</span> r2 ← r2 <span style="color: #339933;">+</span> r2 <span style="color: #339933;">+</span> C<span style="color: #339933;">.</span>
                                  Note that if r0 &gt;= <span style="color: #009900; font-weight: bold;">(</span>r1 &lt;&lt; r3<span style="color: #009900; font-weight: bold;">)</span> then C=<span style="color: #ff0000;">1</span><span style="color: #339933;">,</span> C=<span style="color: #ff0000;">0</span> otherwise <span style="color: #339933;">*/</span>
      subcs r0<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">lsl</span> r3 <span style="color: #339933;">/*</span> r0 ← r0 <span style="color: #339933;">-</span> <span style="color: #009900; font-weight: bold;">(</span>r1 &lt;&lt; r3<span style="color: #009900; font-weight: bold;">)</span> if C = <span style="color: #ff0000;">1</span> <span style="color: #009900; font-weight: bold;">(</span>this is<span style="color: #339933;">,</span> only if r0 &gt;= <span style="color: #009900; font-weight: bold;">(</span>r1 &lt;&lt; r3<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>Lloop_check4<span style="color: #339933;">:</span>
      subs r3<span style="color: #339933;">,</span> r3<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>          <span style="color: #339933;">/*</span> r3 ← r3 <span style="color: #339933;">-</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
      <span style="color: #46aa03; font-weight: bold;">bpl</span> <span style="color: #339933;">.</span>Lloop4              <span style="color: #339933;">/*</span> if r3 &gt;= <span style="color: #ff0000;">0</span> <span style="color: #009900; font-weight: bold;">(</span>N=<span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">)</span> then branch to <span style="color: #339933;">.</span>Lloop1 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> r2
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">clz_unsigned_division:
    clz  r3, r0                /* r3 ← CLZ(r0) Count leading zeroes of N */
    clz  r2, r1                /* r2 ← CLZ(r1) Count leading zeroes of D */
    sub  r3, r2, r3            /* r3 ← r2 - r3. 
                                 This is the difference of zeroes
                                 between D and N. 
                                 Note that N &gt;= D implies CLZ(N) &lt;= CLZ(D)*/
    add r3, r3, #1             /* Loop below needs an extra iteration count */

    mov r2, #0                 /* r2 ← 0 */
    b .Lloop_check4
    .Lloop4:
      cmp r0, r1, lsl r3       /* Compute r0 - (r1 &lt;&lt; r3) and update cpsr */
      adc r2, r2, r2           /* r2 ← r2 + r2 + C.
                                  Note that if r0 &gt;= (r1 &lt;&lt; r3) then C=1, C=0 otherwise */
      subcs r0, r0, r1, lsl r3 /* r0 ← r0 - (r1 &lt;&lt; r3) if C = 1 (this is, only if r0 &gt;= (r1 &lt;&lt; r3) ) */
    .Lloop_check4:
      subs r3, r3, #1          /* r3 ← r3 - 1 */
      bpl .Lloop4              /* if r3 &gt;= 0 (N=0) then branch to .Lloop1 */

    mov r0, r2
    bx lr</p></div>

<h2>Signed division</h2>
<p>
Signed division can be computed with an unsigned division but taking care of the signs. We can first compute |N|/|D| (this is, ignoring the signs of <code>N</code> and <code>D</code>), this will yield a quotient Q<sub>+</sub> and remainder R<sub>+</sub>. If signs of N and D are different then Q = -Q<sub>+</sub>. If N &lt; 0, then R = -R<sub>+</sub>, as we said at the beginning of the post.
</p>
<h2>Powers of two</h2>
<p>
An unsigned division by a power of two 2<sup>N</sup> is as simple as doing a logical shift right of N bits. Conversely, a signed division by a power of two 2<sup>N</sup> is as simple as doing an arithmetic shift right of N bits. We can use <code>mov</code> and the addressing modes <code>LSR</code> and <code>ASR</code> for this. This case is ideal because it is extremely fast.
</p>
<h2>Division by a constant integer</h2>
<p>
When we divide a number by a constant, we can use a multiplication by a <em>magic number</em> to compute the division. All the details and the theory of this technique is too long to write it here but you can find it in chapter 10 of <a href="http://www.amazon.com/Hackers-Delight-Edition-Henry-Warren/dp/0321842685">Hacker's Delight</a>. We can summarize it, though, into three values: a magic constant M, a shift S and an additional flag. The author set up a <a href="http://www.hackersdelight.org/magic.htm">magic number calculator</a> that computes these numbers.
</p>
<p>
It is not obvious how to properly use these magic numbers, so I crafted a <a href="https://github.com/rofirrim/raspberry-pi-assembler/blob/master/chapter15/magic.py">small Python script</a> which emits code for the signed and the unsigned case. Imagine you want to divide an unsigned number by 14. So let's ask our script.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #0000ff; font-weight: bold;">$</span> <span style="color: #339933;">./</span>magic<span style="color: #339933;">.</span>py <span style="color: #ff0000;">14</span> code_for_unsigned
u_divide_by_14<span style="color: #339933;">:</span>
   <span style="color: #339933;">/*</span> r0 contains the argument to be divided by <span style="color: #ff0000;">14</span> <span style="color: #339933;">*/</span>
   ldr r1<span style="color: #339933;">,</span> <span style="color: #339933;">.</span>Lu_magic_number_14 <span style="color: #339933;">/*</span> r1 ← magic_number <span style="color: #339933;">*/</span>
   umull r1<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> r0   <span style="color: #339933;">/*</span> r1 ← Lower32Bits<span style="color: #009900; font-weight: bold;">(</span>r1<span style="color: #339933;">*</span>r0<span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> r2 ← Upper32Bits<span style="color: #009900; font-weight: bold;">(</span>r1<span style="color: #339933;">*</span>r0<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
   adds r2<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r0        <span style="color: #339933;">/*</span> r2 ← r2 <span style="color: #339933;">+</span> r0 updating cpsr <span style="color: #339933;">*/</span>
   <span style="color: #00007f; font-weight: bold;">mov</span> r2<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">ROR</span> #<span style="color: #ff0000;">0</span>     <span style="color: #339933;">/*</span> r2 ← <span style="color: #009900; font-weight: bold;">(</span>carry_flag &lt;&lt; <span style="color: #ff0000;">31</span><span style="color: #009900; font-weight: bold;">)</span> | <span style="color: #009900; font-weight: bold;">(</span>r2 &gt;&gt; <span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
   <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> LSR #<span style="color: #ff0000;">4</span>     <span style="color: #339933;">/*</span> r0 ← r2 &gt;&gt; <span style="color: #ff0000;">4</span> <span style="color: #339933;">*/</span>
   <span style="color: #46aa03; font-weight: bold;">bx</span> lr                  <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">leave</span> function <span style="color: #339933;">*/</span>
   <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
   <span style="color: #339933;">.</span>Lu_magic_number_14<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">0x24924925</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ ./magic.py 14 code_for_unsigned
u_divide_by_14:
   /* r0 contains the argument to be divided by 14 */
   ldr r1, .Lu_magic_number_14 /* r1 ← magic_number */
   umull r1, r2, r1, r0   /* r1 ← Lower32Bits(r1*r0). r2 ← Upper32Bits(r1*r0) */
   adds r2, r2, r0        /* r2 ← r2 + r0 updating cpsr */
   mov r2, r2, ROR #0     /* r2 ← (carry_flag &lt;&lt; 31) | (r2 &gt;&gt; 1) */
   mov r0, r2, LSR #4     /* r0 ← r2 &gt;&gt; 4 */
   bx lr                  /* leave function */
   .align 4
   .Lu_magic_number_14: .word 0x24924925</p></div>

<p>
Similarly we can ask for the signed version:
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #0000ff; font-weight: bold;">$</span> <span style="color: #339933;">./</span>magic<span style="color: #339933;">.</span>py <span style="color: #ff0000;">14</span> code_for_signed
s_divide_by_14<span style="color: #339933;">:</span>
   <span style="color: #339933;">/*</span> r0 contains the argument to be divided by <span style="color: #ff0000;">14</span> <span style="color: #339933;">*/</span>
   ldr r1<span style="color: #339933;">,</span> <span style="color: #339933;">.</span>Ls_magic_number_14 <span style="color: #339933;">/*</span> r1 ← magic_number <span style="color: #339933;">*/</span>
   smull r1<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> r0   <span style="color: #339933;">/*</span> r1 ← Lower32Bits<span style="color: #009900; font-weight: bold;">(</span>r1<span style="color: #339933;">*</span>r0<span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> r2 ← Upper32Bits<span style="color: #009900; font-weight: bold;">(</span>r1<span style="color: #339933;">*</span>r0<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
   <span style="color: #00007f; font-weight: bold;">add</span> r2<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r0         <span style="color: #339933;">/*</span> r2 ← r2 <span style="color: #339933;">+</span> r0 <span style="color: #339933;">*/</span>
   <span style="color: #00007f; font-weight: bold;">mov</span> r2<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> ASR #<span style="color: #ff0000;">3</span>     <span style="color: #339933;">/*</span> r2 ← r2 &gt;&gt; <span style="color: #ff0000;">3</span> <span style="color: #339933;">*/</span>
   <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> LSR #<span style="color: #ff0000;">31</span>    <span style="color: #339933;">/*</span> r1 ← r0 &gt;&gt; <span style="color: #ff0000;">31</span> <span style="color: #339933;">*/</span>
   <span style="color: #00007f; font-weight: bold;">add</span> r0<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r1         <span style="color: #339933;">/*</span> r0 ← r2 <span style="color: #339933;">+</span> r1 <span style="color: #339933;">*/</span>
   <span style="color: #46aa03; font-weight: bold;">bx</span> lr                  <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">leave</span> function <span style="color: #339933;">*/</span>
   <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
   <span style="color: #339933;">.</span>Ls_magic_number_14<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">0x92492493</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ ./magic.py 14 code_for_signed
s_divide_by_14:
   /* r0 contains the argument to be divided by 14 */
   ldr r1, .Ls_magic_number_14 /* r1 ← magic_number */
   smull r1, r2, r1, r0   /* r1 ← Lower32Bits(r1*r0). r2 ← Upper32Bits(r1*r0) */
   add r2, r2, r0         /* r2 ← r2 + r0 */
   mov r2, r2, ASR #3     /* r2 ← r2 &gt;&gt; 3 */
   mov r1, r0, LSR #31    /* r1 ← r0 &gt;&gt; 31 */
   add r0, r2, r1         /* r0 ← r2 + r1 */
   bx lr                  /* leave function */
   .align 4
   .Ls_magic_number_14: .word 0x92492493</p></div>

<p>
As an example I have used it to implement a small program that just divides the user input by 14.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> <span style="color: #339933;">--</span> divideby14<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
read_number<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">0</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
message1 <span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"Enter an integer to divide it by 14: "</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
message2 <span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"Number %d (signed-)divided by 14 is %d\n"</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
scan_format <span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"%d"</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
<span style="color: #339933;">/*</span> This function has been generated using <span style="color: #7f007f;">"magic.py 14 code_for_signed"</span> <span style="color: #339933;">*/</span>
s_divide_by_14<span style="color: #339933;">:</span>
   <span style="color: #339933;">/*</span> r0 contains the argument to be divided by <span style="color: #ff0000;">14</span> <span style="color: #339933;">*/</span>
   ldr r1<span style="color: #339933;">,</span> <span style="color: #339933;">.</span>Ls_magic_number_14 <span style="color: #339933;">/*</span> r1 ← magic_number <span style="color: #339933;">*/</span>
   smull r1<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> r0   <span style="color: #339933;">/*</span> r1 ← Lower32Bits<span style="color: #009900; font-weight: bold;">(</span>r1<span style="color: #339933;">*</span>r0<span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> r2 ← Upper32Bits<span style="color: #009900; font-weight: bold;">(</span>r1<span style="color: #339933;">*</span>r0<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
   <span style="color: #00007f; font-weight: bold;">add</span> r2<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r0         <span style="color: #339933;">/*</span> r2 ← r2 <span style="color: #339933;">+</span> r0 <span style="color: #339933;">*/</span>
   <span style="color: #00007f; font-weight: bold;">mov</span> r2<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> ASR #<span style="color: #ff0000;">3</span>     <span style="color: #339933;">/*</span> r2 ← r2 &gt;&gt; <span style="color: #ff0000;">3</span> <span style="color: #339933;">*/</span>
   <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> LSR #<span style="color: #ff0000;">31</span>    <span style="color: #339933;">/*</span> r1 ← r0 &gt;&gt; <span style="color: #ff0000;">31</span> <span style="color: #339933;">*/</span>
   <span style="color: #00007f; font-weight: bold;">add</span> r0<span style="color: #339933;">,</span> r2<span style="color: #339933;">,</span> r1         <span style="color: #339933;">/*</span> r0 ← r2 <span style="color: #339933;">+</span> r1 <span style="color: #339933;">*/</span>
   <span style="color: #46aa03; font-weight: bold;">bx</span> lr                  <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">leave</span> function <span style="color: #339933;">*/</span>
   <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
   <span style="color: #339933;">.</span>Ls_magic_number_14<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">0x92492493</span>
&nbsp;
<span style="color: #339933;">.</span>globl main
&nbsp;
main<span style="color: #339933;">:</span>
    <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Call</span> printf <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
    ldr r0<span style="color: #339933;">,</span> addr_of_message1       <span style="color: #339933;">/*</span> r0 ← &amp;message <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> printf
&nbsp;
    <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Call</span> scanf <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> addr_of_scan_format   <span style="color: #339933;">/*</span> r0 ← &amp;scan_format <span style="color: #339933;">*/</span>
    ldr r1<span style="color: #339933;">,</span> addr_of_read_number   <span style="color: #339933;">/*</span> r1 ← &amp;read_number <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> scanf
&nbsp;
    ldr r0<span style="color: #339933;">,</span> addr_of_read_number   <span style="color: #339933;">/*</span> r1 ← &amp;read_number <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span>                  <span style="color: #339933;">/*</span> r1 ← <span style="color: #339933;">*</span>r1 <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #46aa03; font-weight: bold;">bl</span> s_divide_by_14
    <span style="color: #00007f; font-weight: bold;">mov</span> r2<span style="color: #339933;">,</span> r0
&nbsp;
    ldr r1<span style="color: #339933;">,</span> addr_of_read_number   <span style="color: #339933;">/*</span> r1 ← &amp;read_number <span style="color: #339933;">*/</span>
    ldr r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #009900; font-weight: bold;">]</span>                  <span style="color: #339933;">/*</span> r1 ← <span style="color: #339933;">*</span>r1 <span style="color: #339933;">*/</span>
&nbsp;
    ldr r0<span style="color: #339933;">,</span> addr_of_message2      <span style="color: #339933;">/*</span> r0 ← &amp;message2 <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> printf                     <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">Call</span> printf<span style="color: #339933;">,</span> r1 <span style="color: #00007f; font-weight: bold;">and</span> r2 already
                                     contain the desired values <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr
&nbsp;
addr_of_message1<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> message1
addr_of_scan_format<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> scan_format
addr_of_message2<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> message2
addr_of_read_number<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> read_number</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* -- divideby14.s */

.data

.align 4
read_number: .word 0

.align 4
message1 : .asciz "Enter an integer to divide it by 14: "

.align 4
message2 : .asciz "Number %d (signed-)divided by 14 is %d\n"

.align 4
scan_format : .asciz "%d"

.text

/* This function has been generated using "magic.py 14 code_for_signed" */
s_divide_by_14:
   /* r0 contains the argument to be divided by 14 */
   ldr r1, .Ls_magic_number_14 /* r1 ← magic_number */
   smull r1, r2, r1, r0   /* r1 ← Lower32Bits(r1*r0). r2 ← Upper32Bits(r1*r0) */
   add r2, r2, r0         /* r2 ← r2 + r0 */
   mov r2, r2, ASR #3     /* r2 ← r2 &gt;&gt; 3 */
   mov r1, r0, LSR #31    /* r1 ← r0 &gt;&gt; 31 */
   add r0, r2, r1         /* r0 ← r2 + r1 */
   bx lr                  /* leave function */
   .align 4
   .Ls_magic_number_14: .word 0x92492493

.globl main

main:
    /* Call printf */
    push {r4, lr}
    ldr r0, addr_of_message1       /* r0 ← &amp;message */
    bl printf

    /* Call scanf */
    ldr r0, addr_of_scan_format   /* r0 ← &amp;scan_format */
    ldr r1, addr_of_read_number   /* r1 ← &amp;read_number */
    bl scanf

    ldr r0, addr_of_read_number   /* r1 ← &amp;read_number */
    ldr r0, [r0]                  /* r1 ← *r1 */

    bl s_divide_by_14
    mov r2, r0

    ldr r1, addr_of_read_number   /* r1 ← &amp;read_number */
    ldr r1, [r1]                  /* r1 ← *r1 */

    ldr r0, addr_of_message2      /* r0 ← &amp;message2 */
    bl printf                     /* Call printf, r1 and r2 already
                                     contain the desired values */
    pop {r4, lr}
    mov r0, #0
    bx lr

addr_of_message1: .word message1
addr_of_scan_format: .word scan_format
addr_of_message2: .word message2
addr_of_read_number: .word read_number</p></div>

<h2>Using VFPv2</h2>
<p>
I would not recommend using this technique. I present it here for the sake of completeness. We simply convert our integers to floating point numbers, divide them as floating point numbers and convert the result back to an integer.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">vfpv2_division<span style="color: #339933;">:</span>
    <span style="color: #339933;">/*</span> r0 contains N <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> r1 contains D <span style="color: #339933;">*/</span>
    vmov s0<span style="color: #339933;">,</span> r0             <span style="color: #339933;">/*</span> s0 ← r0 <span style="color: #009900; font-weight: bold;">(</span>bit copy<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    vmov s1<span style="color: #339933;">,</span> r1             <span style="color: #339933;">/*</span> s1 ← r1 <span style="color: #009900; font-weight: bold;">(</span>bit copy<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    vcvt<span style="color: #339933;">.</span>f32<span style="color: #339933;">.</span>s32 s0<span style="color: #339933;">,</span> s0     <span style="color: #339933;">/*</span> s0 ← <span style="color: #009900; font-weight: bold;">(</span><span style="color: #0000ff; font-weight: bold;">float</span><span style="color: #009900; font-weight: bold;">)</span>s0 <span style="color: #339933;">*/</span>
    vcvt<span style="color: #339933;">.</span>f32<span style="color: #339933;">.</span>s32 s1<span style="color: #339933;">,</span> s1     <span style="color: #339933;">/*</span> s1 ← <span style="color: #009900; font-weight: bold;">(</span><span style="color: #0000ff; font-weight: bold;">float</span><span style="color: #009900; font-weight: bold;">)</span>s1 <span style="color: #339933;">*/</span>
    vdiv<span style="color: #339933;">.</span>f32 s0<span style="color: #339933;">,</span> s0<span style="color: #339933;">,</span> s1     <span style="color: #339933;">/*</span> s0 ← s0 <span style="color: #339933;">/</span> s1 <span style="color: #339933;">*/</span>
    vcvt<span style="color: #339933;">.</span>s32<span style="color: #339933;">.</span>f32 s0<span style="color: #339933;">,</span> s0     <span style="color: #339933;">/*</span> s0 ← <span style="color: #009900; font-weight: bold;">(</span><span style="color: #00007f; font-weight: bold;">int</span><span style="color: #009900; font-weight: bold;">)</span>s0 <span style="color: #339933;">*/</span>
    vmov r0<span style="color: #339933;">,</span> s0             <span style="color: #339933;">/*</span> r0 ← s0 <span style="color: #009900; font-weight: bold;">(</span>bit copy<span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> Now r0 is Q <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">vfpv2_division:
    /* r0 contains N */
    /* r1 contains D */
    vmov s0, r0             /* s0 ← r0 (bit copy) */
    vmov s1, r1             /* s1 ← r1 (bit copy) */
    vcvt.f32.s32 s0, s0     /* s0 ← (float)s0 */
    vcvt.f32.s32 s1, s1     /* s1 ← (float)s1 */
    vdiv.f32 s0, s0, s1     /* s0 ← s0 / s1 */
    vcvt.s32.f32 s0, s0     /* s0 ← (int)s0 */
    vmov r0, s0             /* r0 ← s0 (bit copy). Now r0 is Q */
    bx lr</p></div>

<h2>Comparing versions</h2>
<p>
After a comment below, I thought it would be interesting to benchmark the general division algorithm. The benchmark I used is the following:
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">.</span>set MAX<span style="color: #339933;">,</span> <span style="color: #ff0000;">16384</span>
main<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>                         <span style="color: #339933;">/*</span> r4 ← <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
&nbsp;
    b <span style="color: #339933;">.</span>Lcheck_loop_i                   <span style="color: #339933;">/*</span> branch to <span style="color: #339933;">.</span>Lcheck_loop_i <span style="color: #339933;">*/</span>
    <span style="color: #339933;">.</span>Lloop_i<span style="color: #339933;">:</span>
       <span style="color: #00007f; font-weight: bold;">mov</span> r5<span style="color: #339933;">,</span> r4                      <span style="color: #339933;">/*</span> r5 ← r4 <span style="color: #339933;">*/</span>
       b <span style="color: #339933;">.</span>Lcheck_loop_j                <span style="color: #339933;">/*</span> branch to <span style="color: #339933;">.</span>Lcheck_loop_j <span style="color: #339933;">*/</span>
       <span style="color: #339933;">.</span>Lloop_j<span style="color: #339933;">:</span>
&nbsp;
         <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> r5                    <span style="color: #339933;">/*</span> r0 ← r5<span style="color: #339933;">.</span> This is N <span style="color: #339933;">*/</span>
         <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> r4                    <span style="color: #339933;">/*</span> r1 ← r4<span style="color: #339933;">.</span> This is D <span style="color: #339933;">*/</span>
&nbsp;
         <span style="color: #46aa03; font-weight: bold;">bl</span>  &lt;your unsigned division routine here&gt;
&nbsp;
         <span style="color: #00007f; font-weight: bold;">add</span> r5<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>
       <span style="color: #339933;">.</span>Lcheck_loop_j<span style="color: #339933;">:</span>
         <span style="color: #00007f; font-weight: bold;">cmp</span> r5<span style="color: #339933;">,</span> #MAX                   <span style="color: #339933;">/*</span> compare r5 <span style="color: #00007f; font-weight: bold;">and</span> MAX <span style="color: #339933;">*/</span>
         bne <span style="color: #339933;">.</span>Lloop_j                  <span style="color: #339933;">/*</span> if r5 != <span style="color: #ff0000;">10</span> branch to <span style="color: #339933;">.</span>Lloop_j <span style="color: #339933;">*/</span>
       <span style="color: #00007f; font-weight: bold;">add</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>
    <span style="color: #339933;">.</span>Lcheck_loop_i<span style="color: #339933;">:</span>
      <span style="color: #00007f; font-weight: bold;">cmp</span> r4<span style="color: #339933;">,</span> #MAX                     <span style="color: #339933;">/*</span> compare r4 <span style="color: #00007f; font-weight: bold;">and</span> MAX <span style="color: #339933;">*/</span>
      bne <span style="color: #339933;">.</span>Lloop_i                     <span style="color: #339933;">/*</span> if r4 != <span style="color: #ff0000;">10</span> branch to <span style="color: #339933;">.</span>Lloop_i <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>               
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">.set MAX, 16384
main:
    push {r4, r5, r6, lr}

    mov r4, #1                         /* r4 ← 1 */

    b .Lcheck_loop_i                   /* branch to .Lcheck_loop_i */
    .Lloop_i:
       mov r5, r4                      /* r5 ← r4 */
       b .Lcheck_loop_j                /* branch to .Lcheck_loop_j */
       .Lloop_j:

         mov r0, r5                    /* r0 ← r5. This is N */
         mov r1, r4                    /* r1 ← r4. This is D */

         bl  &lt;your unsigned division routine here&gt;

         add r5, r5, #1
       .Lcheck_loop_j:
         cmp r5, #MAX                   /* compare r5 and MAX */
         bne .Lloop_j                  /* if r5 != 10 branch to .Lloop_j */
       add r4, r4, #1
    .Lcheck_loop_i:
      cmp r4, #MAX                     /* compare r4 and MAX */
      bne .Lloop_i                     /* if r4 != 10 branch to .Lloop_i */
       
    mov r0, #0
      
    pop {r4, r5, r6, lr}               
    bx lr</p></div>

<p>Basically it does something like this</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #b1b100;">for</span> <span style="color: #009900;">(</span>i <span style="color: #339933;">=</span> <span style="color: #0000dd;">1</span><span style="color: #339933;">;</span> i <span style="color: #339933;">&lt;</span> MAX<span style="color: #339933;">;</span> i<span style="color: #339933;">++</span><span style="color: #009900;">)</span>
 <span style="color: #b1b100;">for</span> <span style="color: #009900;">(</span>j <span style="color: #339933;">=</span> i<span style="color: #339933;">;</span> j <span style="color: #339933;">&lt;</span> MAX<span style="color: #339933;">;</span> j<span style="color: #339933;">++</span><span style="color: #009900;">)</span>
   division_function<span style="color: #009900;">(</span>j<span style="color: #339933;">,</span> i<span style="color: #009900;">)</span><span style="color: #339933;">;</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">for (i = 1; i &lt; MAX; i++)
 for (j = i; j &lt; MAX; j++)
   division_function(j, i);</p></div>

<p>
Timings have been obtained using <code>perf_3.2 stat --repeat=5 -e cpu-clock</code>. In the table below, <code>__aeabi_uidiv</code> is the function in <code>libgcc</code> that <code>gcc</code> uses to implement an unsigned integer division.
</p>
<table>
<tbody><tr>
<th>Version</th>
<th>Time (seconds)</th>
</tr>
<tr>
<td>
  unsigned_longdiv
</td>
<td>
  45,43
</td>
</tr>
<tr>
<td>
  vfpv2_division
  </td>
<td>
  9,70
  </td>
</tr>
<tr>
<td>
  clz_unsigned_longdiv
  </td>
<td>
   8,48
  </td>
</tr>
<tr>
<td>
__aeabi_uidiv
  </td>
<td>
  7,37
  </td>
</tr>
<tr>
<td>
  better_unsigned_longdiv
  </td>
<td>
  6,67
  </td>
</tr>
</tbody></table>
<p>
As you can see the performance of our unsigned long division is dismal. The reason it is that it always checks all the bits. The libgcc version is like our clz version but the loop has been fully unrolled and there is a computed branch, similar to a <a href="http://en.wikipedia.org/wiki/Duff%27s_device">Duff's device</a>. Unfortunately, I do not have a convincing explanation why <code>better_unsigned_longdiv</code> runs faster than the other versions, because the code, <em>a priori</em>, looks worse to me.
</p>
<p>
That's all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2013/08/11/arm-assembler-raspberry-pi-chapter-15/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2013/08/11/arm-assembler-raspberry-pi-chapter-15/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2013/08/11/arm-assembler-raspberry-pi-chapter-15/&amp;text=ARM+assembler+in+Raspberry+Pi+%E2%80%93+Chapter+15+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2013/08/11/arm-assembler-raspberry-pi-chapter-15/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>						<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2013/07/10/acts_as_list-gem-single-table-inheritance-rails/" rel="prev">acts_as_list gem and single table inheritance in Rails</a></span>
			<span class="next"><a href="http://thinkingeek.com/2013/08/23/arm-assembler-raspberry-pi-chapter-16/" rel="next">ARM assembler in Raspberry Pi – Chapter 16</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
			<h3>10 thoughts on “<span>ARM assembler in Raspberry Pi – Chapter 15</span>”</h3>
		<ul class="commentlist">
				<li class="comment even thread-even depth-1" id="comment-1497">
				<div id="div-comment-1497" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/d343022f7066553235d373c4014f0c7c?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/d343022f7066553235d373c4014f0c7c?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-d343022f7066553235d373c4014f0c7c-0" originals="32" src-orig="http://1.gravatar.com/avatar/d343022f7066553235d373c4014f0c7c?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn"><a href="http://www.fourtheye.org" rel="external nofollow" class="url">Bob Wilkinson</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/08/11/arm-assembler-raspberry-pi-chapter-15/#comment-1497">
			August 16, 2013 at 12:01 pm</a>		</div>

		<p>as ever – thanks for this great article!</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/08/11/arm-assembler-raspberry-pi-chapter-15/?replytocom=1497#respond" onclick="return addComment.moveForm( &quot;div-comment-1497&quot;, &quot;1497&quot;, &quot;respond&quot;, &quot;1109&quot; )" aria-label="Reply to Bob Wilkinson">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1 parent" id="comment-1499">
				<div id="div-comment-1499" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/d343022f7066553235d373c4014f0c7c?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/d343022f7066553235d373c4014f0c7c?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-d343022f7066553235d373c4014f0c7c-1" originals="32" src-orig="http://1.gravatar.com/avatar/d343022f7066553235d373c4014f0c7c?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn"><a href="http://www.fourtheye.org" rel="external nofollow" class="url">Bob Wilkinson</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/08/11/arm-assembler-raspberry-pi-chapter-15/#comment-1499">
			August 16, 2013 at 3:06 pm</a>		</div>

		<p>in better_unsigned_division lines 8 and 11 viz. </p>
<p>cmp r3, r0, LSR #1           /* update cpsr with r3 – 2*r0 */</p>
<p>a logical shift right is division not multiplication? so the comment should be r3 – (r0/2)?</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/08/11/arm-assembler-raspberry-pi-chapter-15/?replytocom=1499#respond" onclick="return addComment.moveForm( &quot;div-comment-1499&quot;, &quot;1499&quot;, &quot;respond&quot;, &quot;1109&quot; )" aria-label="Reply to Bob Wilkinson">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2" id="comment-1500">
				<div id="div-comment-1500" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-a779b8290b1ca104fdf84d8016fd010b-0" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/08/11/arm-assembler-raspberry-pi-chapter-15/#comment-1500">
			August 16, 2013 at 4:58 pm</a>		</div>

		<p>Hi Bob,</p>
<p>you are right.</p>
<p>I fixed it. Thank you very much.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/08/11/arm-assembler-raspberry-pi-chapter-15/?replytocom=1500#respond" onclick="return addComment.moveForm( &quot;div-comment-1500&quot;, &quot;1500&quot;, &quot;respond&quot;, &quot;1109&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment odd alt thread-even depth-1 parent" id="comment-1521">
				<div id="div-comment-1521" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/bc7a26a25f53e59eeaa7d5e198ccbef4?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/bc7a26a25f53e59eeaa7d5e198ccbef4?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-bc7a26a25f53e59eeaa7d5e198ccbef4-0" originals="32" src-orig="http://2.gravatar.com/avatar/bc7a26a25f53e59eeaa7d5e198ccbef4?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Ian</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/08/11/arm-assembler-raspberry-pi-chapter-15/#comment-1521">
			August 20, 2013 at 10:16 pm</a>		</div>

		<p>Thanks for the incredible blog!</p>
<p>Just one question – you say you would not recommend using VFPv2. Although the code looks very straightforward, is this approach just too slow?</p>
<p>I am curious now to see what the C compiler emits when asked to do an integer divide.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/08/11/arm-assembler-raspberry-pi-chapter-15/?replytocom=1521#respond" onclick="return addComment.moveForm( &quot;div-comment-1521&quot;, &quot;1521&quot;, &quot;respond&quot;, &quot;1109&quot; )" aria-label="Reply to Ian">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor even depth-2" id="comment-1524">
				<div id="div-comment-1524" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-a779b8290b1ca104fdf84d8016fd010b-1" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/08/11/arm-assembler-raspberry-pi-chapter-15/#comment-1524">
			August 21, 2013 at 8:54 am</a>		</div>

		<p>Hi Ian,</p>
<p>my intuition tells me that it should be slower. That said I will add some benchmarks to verify this, because I could be wrong.</p>
<p>Respect to what the C compiler emits: if the divisor is constant it uses the magic number approach (or a shift right for the case of a power of two). If the divisor is non-constant then it simply makes a call to a runtime function. That function in gcc is implemented in libgcc and it is a fully unrolled version (written in assembler) of an algorithm similar to the ones shown above.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/08/11/arm-assembler-raspberry-pi-chapter-15/?replytocom=1524#respond" onclick="return addComment.moveForm( &quot;div-comment-1524&quot;, &quot;1524&quot;, &quot;respond&quot;, &quot;1109&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-1526">
				<div id="div-comment-1526" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-a779b8290b1ca104fdf84d8016fd010b-2" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/08/11/arm-assembler-raspberry-pi-chapter-15/#comment-1526">
			August 21, 2013 at 12:55 pm</a>		</div>

		<p>Hi Ian,</p>
<p>I have updated the post with some benchmarks and another implementation.</p>
<p>The VFPv2 code is not faster than any integer counterpart. That said, it is not awfully slow, only 1.3X times compared to the libgcc version.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/08/11/arm-assembler-raspberry-pi-chapter-15/?replytocom=1526#respond" onclick="return addComment.moveForm( &quot;div-comment-1526&quot;, &quot;1526&quot;, &quot;respond&quot;, &quot;1109&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-896456">
				<div id="div-comment-896456" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/d3eedffb558ce98cffb19a03fe50966c?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/d3eedffb558ce98cffb19a03fe50966c?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-d3eedffb558ce98cffb19a03fe50966c-0" originals="32" src-orig="http://1.gravatar.com/avatar/d3eedffb558ce98cffb19a03fe50966c?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">f</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/08/11/arm-assembler-raspberry-pi-chapter-15/#comment-896456">
			July 1, 2015 at 1:37 pm</a>		</div>

		<p>if i want to use __aeabi_uidiv  function in asm, which library should i link ?</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/08/11/arm-assembler-raspberry-pi-chapter-15/?replytocom=896456#respond" onclick="return addComment.moveForm( &quot;div-comment-896456&quot;, &quot;896456&quot;, &quot;respond&quot;, &quot;1109&quot; )" aria-label="Reply to f">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-896606">
				<div id="div-comment-896606" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-a779b8290b1ca104fdf84d8016fd010b-3" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/08/11/arm-assembler-raspberry-pi-chapter-15/#comment-896606">
			July 1, 2015 at 7:40 pm</a>		</div>

		<p>Just link with <code>libgcc</code>, but this should happen automatically if you link using the <code>gcc</code> driver.</p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/08/11/arm-assembler-raspberry-pi-chapter-15/?replytocom=896606#respond" onclick="return addComment.moveForm( &quot;div-comment-896606&quot;, &quot;896606&quot;, &quot;respond&quot;, &quot;1109&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1 parent" id="comment-982993">
				<div id="div-comment-982993" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/499384e407a4d8c40b5274bb4310549d?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/499384e407a4d8c40b5274bb4310549d?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-499384e407a4d8c40b5274bb4310549d-0" originals="32" src-orig="http://1.gravatar.com/avatar/499384e407a4d8c40b5274bb4310549d?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Filippo</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/08/11/arm-assembler-raspberry-pi-chapter-15/#comment-982993">
			December 30, 2016 at 5:07 pm</a>		</div>

		<p>I think that in clz_unsigned_division you could change line 4 with subs, remove line 8 and swap line 17-18</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/08/11/arm-assembler-raspberry-pi-chapter-15/?replytocom=982993#respond" onclick="return addComment.moveForm( &quot;div-comment-982993&quot;, &quot;982993&quot;, &quot;respond&quot;, &quot;1109&quot; )" aria-label="Reply to Filippo">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-983615">
				<div id="div-comment-983615" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-a779b8290b1ca104fdf84d8016fd010b-4" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/08/11/arm-assembler-raspberry-pi-chapter-15/#comment-983615">
			January 8, 2017 at 4:25 pm</a>		</div>

		<p>Hi Filippo,</p>
<p>sounds about right I’ll try that and then update the benchmark results at the end.</p>
<p>Thanks a lot!</p>
<p>Regards</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/08/11/arm-assembler-raspberry-pi-chapter-15/?replytocom=983615#respond" onclick="return addComment.moveForm( &quot;div-comment-983615&quot;, &quot;983615&quot;, &quot;respond&quot;, &quot;1109&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		</ul>
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2013/08/11/arm-assembler-raspberry-pi-chapter-15/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="1109" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="055846f03d"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496666242832"></form>
			</div><!-- #respond -->
	</div>	</div>