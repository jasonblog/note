# Predication


<div class="main">
			<h1>ARM assembler in Raspberry Pi – Chapter 11 </h1>
		<p class="post-meta"><span class="date">March 16, 2013</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2013/03/16/arm-assembler-raspberry-pi-chapter-11/#comments">8</a></span> </p>
		<p>
Several times, in earlier chapters, I stated that the ARM architecture was designed with the embedded world in mind. Although the cost of the memory is everyday lower, it still may account as an important part of the budget of an embedded system. The ARM instruction set has several features meant to reduce the impact of code size. One of the features which helps in such approach is <strong>predication</strong>.
</p>
<p><span id="more-772"></span></p>
<h2>Predication</h2>
<p>
We saw in chapters 6 and 7 how to use branches in our program in order to modify the execution flow of instructions and implement useful control structures. Branches can be unconditional, for instance when calling a function as we did in chapters 9 and 10, or conditional when we want to jump to some part of the code only when a previously tested condition is met.
</p>
<p>
Predication is related to conditional branches. What if, instead of branching to some part of code meant to be executed only when a condition <code>C</code> holds, we were able to <em>turn</em> some instructions <em>off</em> when that <code>C</code> condition does not hold?. Consider some case like this.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #b1b100;">if</span> <span style="color: #009900;">(</span>C<span style="color: #009900;">)</span>
  T<span style="color: #009900;">(</span><span style="color: #009900;">)</span><span style="color: #339933;">;</span>
<span style="color: #b1b100;">else</span>
  E<span style="color: #009900;">(</span><span style="color: #009900;">)</span><span style="color: #339933;">;</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">if (C)
  T();
else
  E();</p></div>

<p>
Using predication (and with some invented syntax to express it) we could write the above if as follows.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;">P <span style="color: #339933;">=</span> C<span style="color: #339933;">;</span>
<span style="color: #009900;">[</span>P<span style="color: #009900;">]</span>  T<span style="color: #009900;">(</span><span style="color: #009900;">)</span><span style="color: #339933;">;</span>
<span style="color: #009900;">[</span><span style="color: #339933;">!</span>P<span style="color: #009900;">]</span> E<span style="color: #009900;">(</span><span style="color: #009900;">)</span><span style="color: #339933;">;</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">P = C;
[P]  T();
[!P] E();</p></div>

<p>
This way we avoid branches. But, why would be want to avoid branches? Well, executing a conditional branch involves a bit of uncertainty. But this deserves a bit of explanation.
</p>
<h3>The assembly line of instructions</h3>
<p>
Imagine an assembly line. In that assembly line there are 5 workers, each one fully specialized in a single task. That assembly line <em>executes</em> instructions. Every instruction enters the assembly line from the left and leaves it at the right. Each worker does some task on the instruction and passes to the next worker to the right. Also, imagine all workers are more or less synchronized, each one ends the task in as much <code>6</code> seconds. This means that at every 6 seconds there is an instruction leaving the assembly line, an instruction fully executed. It also means that at any given time there may be up to 5 instructions being processed (although not fully executed, we only have one fully executed instruction at every 6 seconds).
</p>
<p><img data-attachment-id="803" data-permalink="http://thinkingeek.com/2013/03/16/arm-assembler-raspberry-pi-chapter-11/pipeline/" data-orig-file="http://thinkingeek.com/wp-content/uploads/2013/03/pipeline.png" data-orig-size="537,124" data-comments-opened="1" data-image-meta="{&quot;aperture&quot;:&quot;0&quot;,&quot;credit&quot;:&quot;&quot;,&quot;camera&quot;:&quot;&quot;,&quot;caption&quot;:&quot;&quot;,&quot;created_timestamp&quot;:&quot;0&quot;,&quot;copyright&quot;:&quot;&quot;,&quot;focal_length&quot;:&quot;0&quot;,&quot;iso&quot;:&quot;0&quot;,&quot;shutter_speed&quot;:&quot;0&quot;,&quot;title&quot;:&quot;&quot;}" data-image-title="pipeline" data-image-description="" data-medium-file="http://thinkingeek.com/wp-content/uploads/2013/03/pipeline-300x69.png" data-large-file="http://thinkingeek.com/wp-content/uploads/2013/03/pipeline.png" src="http://thinkingeek.com/wp-content/uploads/2013/03/pipeline.png" alt="The assembly line of instructions" width="537" height="124" class="aligncenter size-full wp-image-803" srcset="http://thinkingeek.com/wp-content/uploads/2013/03/pipeline.png 537w, http://thinkingeek.com/wp-content/uploads/2013/03/pipeline-300x69.png 300w" sizes="(max-width: 537px) 100vw, 537px"></p>
<p>
The first worker <em>fetches</em> instructions and puts them in the assembly line. It fetches the instruction at the address specified by the register <code>pc</code>. By default, unless told, this worker <em>fetches</em> the instruction physically following the one he previously fetched (this is <em>implicit sequencing</em>).
</p>
<p>
In this assembly line, the worker that checks the condition of a conditional branch is not the first one but the third one. Now consider what happens when the first worker fetches a conditional branch and puts it in the assembly line. The second worker will process it and pass it to the third one. The third one will process it by checking the condition of the conditional branch. If it does not hold, nothing happens, the branch has no effect. But if the condition holds, the third worker must notify the first one that the next instruction fetched should be the instruction at the address of the branch.
</p>
<p>
But now there are two instructions in the assembly line that should not be fully executed (the ones that were physically after the conditional branch). There are several options here. The third worker may pick two stickers labeled as <span style="font-variant: small-caps;">do nothing</span>, and stick them to the two next instructions. Another approach would be the third worker to tell the first and second workers «hey guys, stick a <span style="font-variant: small-caps;">do nothing</span> to your current instruction». Later workers, when they see these <span style="font-variant: small-caps;">do nothing</span> stickers will do, huh, nothing. This way each <span style="font-variant: small-caps;">do nothing</span> instruction will never be fully executed.
</p>
<p><img data-attachment-id="821" data-permalink="http://thinkingeek.com/2013/03/16/arm-assembler-raspberry-pi-chapter-11/bombolla/" data-orig-file="http://thinkingeek.com/wp-content/uploads/2013/03/bombolla.png" data-orig-size="535,556" data-comments-opened="1" data-image-meta="{&quot;aperture&quot;:&quot;0&quot;,&quot;credit&quot;:&quot;&quot;,&quot;camera&quot;:&quot;&quot;,&quot;caption&quot;:&quot;&quot;,&quot;created_timestamp&quot;:&quot;0&quot;,&quot;copyright&quot;:&quot;&quot;,&quot;focal_length&quot;:&quot;0&quot;,&quot;iso&quot;:&quot;0&quot;,&quot;shutter_speed&quot;:&quot;0&quot;,&quot;title&quot;:&quot;&quot;}" data-image-title="bombolla" data-image-description="" data-medium-file="http://thinkingeek.com/wp-content/uploads/2013/03/bombolla-288x300.png" data-large-file="http://thinkingeek.com/wp-content/uploads/2013/03/bombolla.png" src="http://thinkingeek.com/wp-content/uploads/2013/03/bombolla.png" alt="The third worker realizes that a branch is taken. Next two instructions will get a DO NOTHING sticker" width="535" height="556" class="aligncenter size-full wp-image-821" srcset="http://thinkingeek.com/wp-content/uploads/2013/03/bombolla.png 535w, http://thinkingeek.com/wp-content/uploads/2013/03/bombolla-288x300.png 288w" sizes="(max-width: 535px) 100vw, 535px"></p>
<p>
But by doing this, that nice property of our assembly line is gone: now we do not have a fully executed instruction every 6 seconds. In fact, after the conditional branch there are two <span style="font-variant: small-caps;">do nothing</span> instructions. A program that is constantly doing branches may well reduce the performance of our assembly line from one (useful) instruction each 6 seconds to one instruction each 18 seconds. This is three times slower!
</p>
<p>
Truth is that modern processors, including the one in the Raspberry Pi, have <em>branch predictors</em> which are able to mitigate these problems: they try to predict whether the condition will hold, so the branch is taken or not. Branch predictors, though, predict the future like stock brokers, using the past and, when there is no past information, using some sensible assumptions. So branch predictors may work very well with relatively predictable codes but may work not so well if the code has unpredictable behaviour. Such behaviour, for instance, is observed when running decompressors. A compressor reduces the size of your files removing the redundancy. Redundant stuff is predictable and can be omitted (for instance in “he is wearing his coat” you could ommit “he” or replace “his” by “its”, regardless of whether doing this is rude, because you know you are talking about a male). So a decompressor will have to decompress a file which has very little redundancy, driving nuts the predictor.
</p>
<p>
Back to the assembly line example, it would be the first worker who attempts to predict where the branch will be taken or not. It is the third worker who verifies if the first worker did the right prediction. If the first worker mispredicted the branch, then we have to apply two stickers again and notify the first worker which is the right address of the next instruction. If the first worker predicted the branch right, nothing special has to be done, which is great.
</p>
<p>
If we avoid branches, we avoid the uncertainty of whether the branch is taken or not. So it looks like that predication is the way to go. Not so fast. Processing a bunch of instructions that are actually turned off is not an efficient usage of a processor.
</p>
<p>
Back to our assembly line, the third worker will check the predicate. If it does not hold, the current instruction will get a <span style="font-variant: small-caps;">do nothing</span> sticker but in contrast to a branch, it does not notify the first worker.
</p>
<p>
So it ends, as usually, that no approach is perfect on its own.
</p>
<h2>Predication in ARM</h2>
<p>
In ARM, predication is very simple to use: almost all instructions can be predicated. The predicate is specified as a suffix to the instruction name. The suffix is exactly the same as those used in branches in the chapter 5: <code>eq</code>, <code>neq</code>, <code>le</code>, <code>lt</code>, <code>ge</code> and <code>gt</code>. Instructions that are not predicated are assumed to have a suffix <code>al</code> standing for <em><strong>al</strong>ways</em>. That predicate always holds and we do not write it for economy (it is valid though). You can understand conditional branches as predicated branches if you feel like.
</p>
<h2>Collatz conjecture revisited</h2>
<p>
In chapter 6 we implementd an algorithm that computed the length of the sequence of Hailstone of a given number. Though not proved yet, no number has been found that has an infinite Hailstone sequence. Given our knowledge of functions we learnt in chapters 9 and 10, I encapsulated the code that computes the length of the sequence of Hailstone in a function.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> <span style="color: #339933;">--</span> collatz02<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
message<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"Type a number: "</span>
scan_format <span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"%d"</span>
message2<span style="color: #339933;">:</span> <span style="color: #339933;">.</span>asciz <span style="color: #7f007f;">"Length of the Hailstone sequence for %d is %d\n"</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
collatz<span style="color: #339933;">:</span>
    <span style="color: #339933;">/*</span> r0 contains the first argument <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> Only r0<span style="color: #339933;">,</span> r1 <span style="color: #00007f; font-weight: bold;">and</span> r2 are modified<span style="color: #339933;">,</span> 
       so we <span style="color: #0000ff; font-weight: bold;">do</span> <span style="color: #00007f; font-weight: bold;">not</span> need to keep anything
       <span style="color: #00007f; font-weight: bold;">in</span> the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
    <span style="color: #339933;">/*</span> Since we <span style="color: #0000ff; font-weight: bold;">do</span> <span style="color: #00007f; font-weight: bold;">not</span> <span style="color: #0000ff; font-weight: bold;">do</span> any <span style="color: #00007f; font-weight: bold;">call</span><span style="color: #339933;">,</span> we <span style="color: #0000ff; font-weight: bold;">do</span>
       <span style="color: #00007f; font-weight: bold;">not</span> have to keep lr either <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> r0                 <span style="color: #339933;">/*</span> r1 ← r0 <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>                 <span style="color: #339933;">/*</span> r0 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
  collatz_loop<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">cmp</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>                 <span style="color: #339933;">/*</span> compare r1 <span style="color: #00007f; font-weight: bold;">and</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    beq collatz_end            <span style="color: #339933;">/*</span> if r1 == <span style="color: #ff0000;">1</span> branch to collatz_end <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">and</span> r2<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>             <span style="color: #339933;">/*</span> r2 ← r1 &amp; <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">cmp</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>                 <span style="color: #339933;">/*</span> compare r2 <span style="color: #00007f; font-weight: bold;">and</span> <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    bne collatz_odd            <span style="color: #339933;">/*</span> if r2 != <span style="color: #ff0000;">0</span> <span style="color: #009900; font-weight: bold;">(</span>this is r1 <span style="color: #339933;">%</span> <span style="color: #ff0000;">2</span> != <span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">)</span> branch to collatz_odd <span style="color: #339933;">*/</span>
  collatz_even<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> ASR #<span style="color: #ff0000;">1</span>         <span style="color: #339933;">/*</span> r1 ← r1 &gt;&gt; <span style="color: #ff0000;">1</span><span style="color: #339933;">.</span> This is r1 ← r1<span style="color: #339933;">/</span><span style="color: #ff0000;">2</span> <span style="color: #339933;">*/</span>
    b collatz_end_loop         <span style="color: #339933;">/*</span> branch to collatz_end_loop <span style="color: #339933;">*/</span>
  collatz_odd<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">1</span>     <span style="color: #339933;">/*</span> r1 ← r1 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span>r1 &lt;&lt; <span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> This is r1 ← <span style="color: #ff0000;">3</span><span style="color: #339933;">*</span>r1 <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>             <span style="color: #339933;">/*</span> r1 ← r1 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span><span style="color: #339933;">.</span> <span style="color: #339933;">*/</span>
  collatz_end_loop<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r0<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>             <span style="color: #339933;">/*</span> r0 ← r0 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    b collatz_loop             <span style="color: #339933;">/*</span> branch back to collatz_loop <span style="color: #339933;">*/</span>
  collatz_end<span style="color: #339933;">:</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">global</span> main
main<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>lr<span style="color: #009900; font-weight: bold;">}</span>                       <span style="color: #339933;">/*</span> keep lr <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">sub</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>                  <span style="color: #339933;">/*</span> make room for <span style="color: #ff0000;">4</span> bytes <span style="color: #00007f; font-weight: bold;">in</span> the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
                                    <span style="color: #339933;">/*</span> The <span style="color: #0000ff; font-weight: bold;">stack</span> is already <span style="color: #ff0000;">8</span> <span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
&nbsp;
    ldr r0<span style="color: #339933;">,</span> address_of_message      <span style="color: #339933;">/*</span> first parameter of printf<span style="color: #339933;">:</span> &amp;message <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> printf                       <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> printf <span style="color: #339933;">*/</span>
&nbsp;
    ldr r0<span style="color: #339933;">,</span> address_of_scan_format  <span style="color: #339933;">/*</span> first parameter of scanf<span style="color: #339933;">:</span> &amp;scan_format <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span>                      <span style="color: #339933;">/*</span> second parameter of scanf<span style="color: #339933;">:</span> 
                                       address of the top of the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> scanf                        <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> scanf <span style="color: #339933;">*/</span>
&nbsp;
    ldr r0<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #009900; font-weight: bold;">]</span>                    <span style="color: #339933;">/*</span> first parameter of collatz<span style="color: #339933;">:</span>
                                       the value stored <span style="color: #009900; font-weight: bold;">(</span>by scanf<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #00007f; font-weight: bold;">in</span> the top of the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> collatz                      <span style="color: #339933;">/*</span> <span style="color: #00007f; font-weight: bold;">call</span> collatz <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #00007f; font-weight: bold;">mov</span> r2<span style="color: #339933;">,</span> r0                      <span style="color: #339933;">/*</span> third parameter of printf<span style="color: #339933;">:</span> 
                                       the result of collatz <span style="color: #339933;">*/</span>
    ldr r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span><span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #009900; font-weight: bold;">]</span>                    <span style="color: #339933;">/*</span> second parameter of printf<span style="color: #339933;">:</span>
                                       the value stored <span style="color: #009900; font-weight: bold;">(</span>by scanf<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #00007f; font-weight: bold;">in</span> the top of the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
    ldr r0<span style="color: #339933;">,</span> address_of_message2     <span style="color: #339933;">/*</span> first parameter of printf<span style="color: #339933;">:</span> &amp;address_of_message2 <span style="color: #339933;">*/</span>
    <span style="color: #46aa03; font-weight: bold;">bl</span> printf
&nbsp;
    <span style="color: #00007f; font-weight: bold;">add</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>lr<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr
&nbsp;
&nbsp;
address_of_message<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> message
address_of_scan_format<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> scan_format
address_of_message2<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> message2</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* -- collatz02.s */
.data

message: .asciz "Type a number: "
scan_format : .asciz "%d"
message2: .asciz "Length of the Hailstone sequence for %d is %d\n"

.text

collatz:
    /* r0 contains the first argument */
    /* Only r0, r1 and r2 are modified, 
       so we do not need to keep anything
       in the stack */
    /* Since we do not do any call, we do
       not have to keep lr either */
    mov r1, r0                 /* r1 ← r0 */
    mov r0, #0                 /* r0 ← 0 */
  collatz_loop:
    cmp r1, #1                 /* compare r1 and 1 */
    beq collatz_end            /* if r1 == 1 branch to collatz_end */
    and r2, r1, #1             /* r2 ← r1 &amp; 1 */
    cmp r2, #0                 /* compare r2 and 0 */
    bne collatz_odd            /* if r2 != 0 (this is r1 % 2 != 0) branch to collatz_odd */
  collatz_even:
    mov r1, r1, ASR #1         /* r1 ← r1 &gt;&gt; 1. This is r1 ← r1/2 */
    b collatz_end_loop         /* branch to collatz_end_loop */
  collatz_odd:
    add r1, r1, r1, LSL #1     /* r1 ← r1 + (r1 &lt;&lt; 1). This is r1 ← 3*r1 */
    add r1, r1, #1             /* r1 ← r1 + 1. */
  collatz_end_loop:
    add r0, r0, #1             /* r0 ← r0 + 1 */
    b collatz_loop             /* branch back to collatz_loop */
  collatz_end:
    bx lr

.global main
main:
    push {lr}                       /* keep lr */
    sub sp, sp, #4                  /* make room for 4 bytes in the stack */
                                    /* The stack is already 8 byte aligned */

    ldr r0, address_of_message      /* first parameter of printf: &amp;message */
    bl printf                       /* call printf */

    ldr r0, address_of_scan_format  /* first parameter of scanf: &amp;scan_format */
    mov r1, sp                      /* second parameter of scanf: 
                                       address of the top of the stack */
    bl scanf                        /* call scanf */

    ldr r0, [sp]                    /* first parameter of collatz:
                                       the value stored (by scanf) in the top of the stack */
    bl collatz                      /* call collatz */
    
    mov r2, r0                      /* third parameter of printf: 
                                       the result of collatz */
    ldr r1, [sp]                    /* second parameter of printf:
                                       the value stored (by scanf) in the top of the stack */
    ldr r0, address_of_message2     /* first parameter of printf: &amp;address_of_message2 */
    bl printf

    add sp, sp, #4
    pop {lr}
    bx lr
    

address_of_message: .word message
address_of_scan_format: .word scan_format
address_of_message2: .word message2</p></div>

<h2>Adding predication</h2>
<p>
Ok, let’s add some predication. There is an <em>if-then-else</em> construct in lines 22 to 31. There we check if the number is even or odd. If even we divide it by 2, if even we multiply it by 3 and add 1.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="line_numbers"><pre>22
23
24
25
26
27
28
29
30
31
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">    <span style="color: #00007f; font-weight: bold;">and</span> r2<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>             <span style="color: #339933;">/*</span> r2 ← r1 &amp; <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">cmp</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>                 <span style="color: #339933;">/*</span> compare r2 <span style="color: #00007f; font-weight: bold;">and</span> <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    bne collatz_odd            <span style="color: #339933;">/*</span> if r2 != <span style="color: #ff0000;">0</span> <span style="color: #009900; font-weight: bold;">(</span>this is r1 <span style="color: #339933;">%</span> <span style="color: #ff0000;">2</span> != <span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">)</span> branch to collatz_odd <span style="color: #339933;">*/</span>
  collatz_even<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> ASR #<span style="color: #ff0000;">1</span>         <span style="color: #339933;">/*</span> r1 ← r1 &gt;&gt; <span style="color: #ff0000;">1</span><span style="color: #339933;">.</span> This is r1 ← r1<span style="color: #339933;">/</span><span style="color: #ff0000;">2</span> <span style="color: #339933;">*/</span>
    b collatz_end_loop         <span style="color: #339933;">/*</span> branch to collatz_end_loop <span style="color: #339933;">*/</span>
  collatz_odd<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">1</span>     <span style="color: #339933;">/*</span> r1 ← r1 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span>r1 &lt;&lt; <span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> This is r1 ← <span style="color: #ff0000;">3</span><span style="color: #339933;">*</span>r1 <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>             <span style="color: #339933;">/*</span> r1 ← r1 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span><span style="color: #339933;">.</span> <span style="color: #339933;">*/</span>
  collatz_end_loop<span style="color: #339933;">:</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">    and r2, r1, #1             /* r2 ← r1 &amp; 1 */
    cmp r2, #0                 /* compare r2 and 0 */
    bne collatz_odd            /* if r2 != 0 (this is r1 % 2 != 0) branch to collatz_odd */
  collatz_even:
    mov r1, r1, ASR #1         /* r1 ← r1 &gt;&gt; 1. This is r1 ← r1/2 */
    b collatz_end_loop         /* branch to collatz_end_loop */
  collatz_odd:
    add r1, r1, r1, LSL #1     /* r1 ← r1 + (r1 &lt;&lt; 1). This is r1 ← 3*r1 */
    add r1, r1, #1             /* r1 ← r1 + 1. */
  collatz_end_loop:</p></div>

<p>
Note in line 24 that there is a <code>bne</code> (<strong>b</strong>ranch if <strong>n</strong>ot <strong>e</strong>qual). We can use this condition (and its opposite <code>eq</code>) to predicate this <em>if-then-else</em> construct. Instructions in the <em>then</em> part will be predicated using <code>eq</code>, instructions in the <em>else</em> part will be predicated using <code>ne</code>. The resulting code is shown below.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">    <span style="color: #00007f; font-weight: bold;">cmp</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>                 <span style="color: #339933;">/*</span> compare r2 <span style="color: #00007f; font-weight: bold;">and</span> <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    moveq r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> ASR #<span style="color: #ff0000;">1</span>       <span style="color: #339933;">/*</span> if r2 == <span style="color: #ff0000;">0</span><span style="color: #339933;">,</span> r1 ← r1 &gt;&gt; <span style="color: #ff0000;">1</span><span style="color: #339933;">.</span> This is r1 ← r1<span style="color: #339933;">/</span><span style="color: #ff0000;">2</span> <span style="color: #339933;">*/</span>
    addne r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">1</span>   <span style="color: #339933;">/*</span> if r2 != <span style="color: #ff0000;">0</span><span style="color: #339933;">,</span> r1 ← r1 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span>r1 &lt;&lt; <span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> This is r1 ← <span style="color: #ff0000;">3</span><span style="color: #339933;">*</span>r1 <span style="color: #339933;">*/</span>
    addne r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>           <span style="color: #339933;">/*</span> if r2 != <span style="color: #ff0000;">0</span><span style="color: #339933;">,</span> r1 ← r1 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span><span style="color: #339933;">.</span> <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">    cmp r2, #0                 /* compare r2 and 0 */
    moveq r1, r1, ASR #1       /* if r2 == 0, r1 ← r1 &gt;&gt; 1. This is r1 ← r1/2 */
    addne r1, r1, r1, LSL #1   /* if r2 != 0, r1 ← r1 + (r1 &lt;&lt; 1). This is r1 ← 3*r1 */
    addne r1, r1, #1           /* if r2 != 0, r1 ← r1 + 1. */</p></div>

<p>
As you can se there are no labels in the predicated version. We do not branch now so they are not needed anymore. Note also that we actually removed two branches: the one that branches from the condition test code to the <em>else</em> part and the one that branches from the end of the <em>then</em> part to the instruction after the whole <em>if-then-else</em>. This leads to a more compact code.
</p>
<h2>Does it make any difference in performance?</h2>
<p>
Taken as is, this program is very small to be accountable for time, so I modified it to run the same calculation inside the collatz function 4194304 (this is 2<sup>22</sup>) times. I chose the number after some tests, so the execution did not take too much time to be a tedium.
</p>
<p>
Sadly, while the Raspberry Pi processor provides some hardware performance counters I have not been able to use any of them. <code>perf</code> tool (from the package <code>linux-tools-3.2</code>) complains that the counter cannot be opened.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="shell" style="font-family:monospace;">$ perf_3.2 stat -e cpu-cycles ./collatz02
  Error: open_counter returned with 19 (No such device). /bin/dmesg may provide additional information.
&nbsp;
  Fatal: Not all events could be opened</pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ perf_3.2 stat -e cpu-cycles ./collatz02
  Error: open_counter returned with 19 (No such device). /bin/dmesg may provide additional information.

  Fatal: Not all events could be opened</p></div>

<p>
<code>dmesg</code> does not provide any additional information. We can see, though, that the performance counters was loaded by the kernel.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="shell" style="font-family:monospace;">$ dmesg | grep perf
[    0.061722] hw perfevents: enabled with v6 PMU driver, 3 counters available</pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ dmesg | grep perf
[    0.061722] hw perfevents: enabled with v6 PMU driver, 3 counters available</p></div>

<p>
Supposedly I should be able to measure up to 3 hardware events at the same time. I think the Raspberry Pi processor, packaged in the BCM2835 SoC does not provide a PMU (Performance Monitoring Unit) which is required for performance counters. Nevertheless we can use <code>cpu-clock</code> to measure the time.
</p>
<p>
Below are the versions I used for this comparison. First is the branches version, second the predication version.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">collatz<span style="color: #339933;">:</span>
    <span style="color: #339933;">/*</span> r0 contains the first argument <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #00007f; font-weight: bold;">sub</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> Make sure the <span style="color: #0000ff; font-weight: bold;">stack</span> is <span style="color: #ff0000;">8</span> <span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> r0
    <span style="color: #00007f; font-weight: bold;">mov</span> r3<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4194304</span>
  collatz_repeat<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> r4                 <span style="color: #339933;">/*</span> r1 ← r0 <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>                 <span style="color: #339933;">/*</span> r0 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
  collatz_loop<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">cmp</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>                 <span style="color: #339933;">/*</span> compare r1 <span style="color: #00007f; font-weight: bold;">and</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    beq collatz_end            <span style="color: #339933;">/*</span> if r1 == <span style="color: #ff0000;">1</span> branch to collatz_end <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">and</span> r2<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>             <span style="color: #339933;">/*</span> r2 ← r1 &amp; <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">cmp</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>                 <span style="color: #339933;">/*</span> compare r2 <span style="color: #00007f; font-weight: bold;">and</span> <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    bne collatz_odd            <span style="color: #339933;">/*</span> if r2 != <span style="color: #ff0000;">0</span> <span style="color: #009900; font-weight: bold;">(</span>this is r1 <span style="color: #339933;">%</span> <span style="color: #ff0000;">2</span> != <span style="color: #ff0000;">0</span><span style="color: #009900; font-weight: bold;">)</span> branch to collatz_odd <span style="color: #339933;">*/</span>
  collatz_even<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> ASR #<span style="color: #ff0000;">1</span>         <span style="color: #339933;">/*</span> r1 ← r1 &gt;&gt; <span style="color: #ff0000;">1</span><span style="color: #339933;">.</span> This is r1 ← r1<span style="color: #339933;">/</span><span style="color: #ff0000;">2</span> <span style="color: #339933;">*/</span>
    b collatz_end_loop         <span style="color: #339933;">/*</span> branch to collatz_end_loop <span style="color: #339933;">*/</span>
  collatz_odd<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">1</span>     <span style="color: #339933;">/*</span> r1 ← r1 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span>r1 &lt;&lt; <span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> This is r1 ← <span style="color: #ff0000;">3</span><span style="color: #339933;">*</span>r1 <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>             <span style="color: #339933;">/*</span> r1 ← r1 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span><span style="color: #339933;">.</span> <span style="color: #339933;">*/</span>
  collatz_end_loop<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r0<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>             <span style="color: #339933;">/*</span> r0 ← r0 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    b collatz_loop             <span style="color: #339933;">/*</span> branch back to collatz_loop <span style="color: #339933;">*/</span>
  collatz_end<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">sub</span> r3<span style="color: #339933;">,</span> r3<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>
    <span style="color: #00007f; font-weight: bold;">cmp</span> r3<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>
    bne collatz_repeat
    <span style="color: #00007f; font-weight: bold;">add</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> Make sure the <span style="color: #0000ff; font-weight: bold;">stack</span> is <span style="color: #ff0000;">8</span> <span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">collatz:
    /* r0 contains the first argument */
    push {r4}
    sub sp, sp, #4  /* Make sure the stack is 8 byte aligned */
    mov r4, r0
    mov r3, #4194304
  collatz_repeat:
    mov r1, r4                 /* r1 ← r0 */
    mov r0, #0                 /* r0 ← 0 */
  collatz_loop:
    cmp r1, #1                 /* compare r1 and 1 */
    beq collatz_end            /* if r1 == 1 branch to collatz_end */
    and r2, r1, #1             /* r2 ← r1 &amp; 1 */
    cmp r2, #0                 /* compare r2 and 0 */
    bne collatz_odd            /* if r2 != 0 (this is r1 % 2 != 0) branch to collatz_odd */
  collatz_even:
    mov r1, r1, ASR #1         /* r1 ← r1 &gt;&gt; 1. This is r1 ← r1/2 */
    b collatz_end_loop         /* branch to collatz_end_loop */
  collatz_odd:
    add r1, r1, r1, LSL #1     /* r1 ← r1 + (r1 &lt;&lt; 1). This is r1 ← 3*r1 */
    add r1, r1, #1             /* r1 ← r1 + 1. */
  collatz_end_loop:
    add r0, r0, #1             /* r0 ← r0 + 1 */
    b collatz_loop             /* branch back to collatz_loop */
  collatz_end:
    sub r3, r3, #1
    cmp r3, #0
    bne collatz_repeat
    add sp, sp, #4  /* Make sure the stack is 8 byte aligned */
    pop {r4}
    bx lr</p></div>


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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;">collatz2<span style="color: #339933;">:</span>
    <span style="color: #339933;">/*</span> r0 contains the first argument <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #00007f; font-weight: bold;">sub</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>  <span style="color: #339933;">/*</span> Make sure the <span style="color: #0000ff; font-weight: bold;">stack</span> is <span style="color: #ff0000;">8</span> <span style="color: #0000ff; font-weight: bold;">byte</span> aligned <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> r0
    <span style="color: #00007f; font-weight: bold;">mov</span> r3<span style="color: #339933;">,</span> #<span style="color: #ff0000;">4194304</span>
  collatz_repeat<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> r4                 <span style="color: #339933;">/*</span> r1 ← r0 <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>                 <span style="color: #339933;">/*</span> r0 ← <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
  collatz2_loop<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">cmp</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>                 <span style="color: #339933;">/*</span> compare r1 <span style="color: #00007f; font-weight: bold;">and</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    beq collatz2_end           <span style="color: #339933;">/*</span> if r1 == <span style="color: #ff0000;">1</span> branch to collatz2_end <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">and</span> r2<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>             <span style="color: #339933;">/*</span> r2 ← r1 &amp; <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">cmp</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>                 <span style="color: #339933;">/*</span> compare r2 <span style="color: #00007f; font-weight: bold;">and</span> <span style="color: #ff0000;">0</span> <span style="color: #339933;">*/</span>
    moveq r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> ASR #<span style="color: #ff0000;">1</span>       <span style="color: #339933;">/*</span> if r2 == <span style="color: #ff0000;">0</span><span style="color: #339933;">,</span> r1 ← r1 &gt;&gt; <span style="color: #ff0000;">1</span><span style="color: #339933;">.</span> This is r1 ← r1<span style="color: #339933;">/</span><span style="color: #ff0000;">2</span> <span style="color: #339933;">*/</span>
    addne r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">1</span>   <span style="color: #339933;">/*</span> if r2 != <span style="color: #ff0000;">0</span><span style="color: #339933;">,</span> r1 ← r1 <span style="color: #339933;">+</span> <span style="color: #009900; font-weight: bold;">(</span>r1 &lt;&lt; <span style="color: #ff0000;">1</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> This is r1 ← <span style="color: #ff0000;">3</span><span style="color: #339933;">*</span>r1 <span style="color: #339933;">*/</span>
    addne r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>           <span style="color: #339933;">/*</span> if r2 != <span style="color: #ff0000;">0</span><span style="color: #339933;">,</span> r1 ← r1 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span><span style="color: #339933;">.</span> <span style="color: #339933;">*/</span>
  collatz2_end_loop<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r0<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>             <span style="color: #339933;">/*</span> r0 ← r0 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    b collatz2_loop            <span style="color: #339933;">/*</span> branch back to collatz2_loop <span style="color: #339933;">*/</span>
  collatz2_end<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">sub</span> r3<span style="color: #339933;">,</span> r3<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>
    <span style="color: #00007f; font-weight: bold;">cmp</span> r3<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>
    bne collatz_repeat
    <span style="color: #00007f; font-weight: bold;">add</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> <span style="color: #46aa03; font-weight: bold;">sp</span><span style="color: #339933;">,</span> #<span style="color: #ff0000;">4</span>             <span style="color: #339933;">/*</span> Restore the <span style="color: #0000ff; font-weight: bold;">stack</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #009900; font-weight: bold;">}</span>
    <span style="color: #46aa03; font-weight: bold;">bx</span> lr</pre></td></tr></tbody></table><p class="theCode" style="display:none;">collatz2:
    /* r0 contains the first argument */
    push {r4}
    sub sp, sp, #4  /* Make sure the stack is 8 byte aligned */
    mov r4, r0
    mov r3, #4194304
  collatz_repeat:
    mov r1, r4                 /* r1 ← r0 */
    mov r0, #0                 /* r0 ← 0 */
  collatz2_loop:
    cmp r1, #1                 /* compare r1 and 1 */
    beq collatz2_end           /* if r1 == 1 branch to collatz2_end */
    and r2, r1, #1             /* r2 ← r1 &amp; 1 */
    cmp r2, #0                 /* compare r2 and 0 */
    moveq r1, r1, ASR #1       /* if r2 == 0, r1 ← r1 &gt;&gt; 1. This is r1 ← r1/2 */
    addne r1, r1, r1, LSL #1   /* if r2 != 0, r1 ← r1 + (r1 &lt;&lt; 1). This is r1 ← 3*r1 */
    addne r1, r1, #1           /* if r2 != 0, r1 ← r1 + 1. */
  collatz2_end_loop:
    add r0, r0, #1             /* r0 ← r0 + 1 */
    b collatz2_loop            /* branch back to collatz2_loop */
  collatz2_end:
    sub r3, r3, #1
    cmp r3, #0
    bne collatz_repeat
    add sp, sp, #4             /* Restore the stack */
    pop {r4}
    bx lr</p></div>

<p>
The tool <code>perf</code> can be used to gather performance counters. We will run 5 times each version. We will use number 123. We redirect the output of <code>yes 123</code> to the standard input of our tested program. This way we do not have to type it (which may affect the timing of the comparison).
</p>
<p>
The version with branches gives the following results:
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="shell" style="font-family:monospace;">$ yes 123 | perf_3.2 stat --log-fd=3 --repeat=5 -e cpu-clock ./collatz_branches 3&gt;&amp;1
Type a number: Length of the Hailstone sequence for 123 is 46
Type a number: Length of the Hailstone sequence for 123 is 46
Type a number: Length of the Hailstone sequence for 123 is 46
Type a number: Length of the Hailstone sequence for 123 is 46
Type a number: Length of the Hailstone sequence for 123 is 46
&nbsp;
 Performance counter stats for './collatz_branches' (5 runs):
&nbsp;
       3359,953200 cpu-clock                  ( +-  0,01% )
&nbsp;
       3,365263737 seconds time elapsed                                          ( +-  0,01% )</pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ yes 123 | perf_3.2 stat --log-fd=3 --repeat=5 -e cpu-clock ./collatz_branches 3&gt;&amp;1
Type a number: Length of the Hailstone sequence for 123 is 46
Type a number: Length of the Hailstone sequence for 123 is 46
Type a number: Length of the Hailstone sequence for 123 is 46
Type a number: Length of the Hailstone sequence for 123 is 46
Type a number: Length of the Hailstone sequence for 123 is 46

 Performance counter stats for './collatz_branches' (5 runs):

       3359,953200 cpu-clock                  ( +-  0,01% )

       3,365263737 seconds time elapsed                                          ( +-  0,01% )</p></div>

<p>
(When redirecting the input of <code>perf</code> one must specify the file descriptor for the output of <code>perf stat</code> itself. In this case we have used the file descriptor number 3 and then told the shell to redirect the file descriptor number 3 to the standard output, which is the file descriptor number 1).
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="shell" style="font-family:monospace;">$ yes 123 | perf_3.2 stat --log-fd=3 --repeat=5 -e cpu-clock ./collatz_predication 3&gt;&amp;1
Type a number: Length of the Hailstone sequence for 123 is 46
Type a number: Length of the Hailstone sequence for 123 is 46
Type a number: Length of the Hailstone sequence for 123 is 46
Type a number: Length of the Hailstone sequence for 123 is 46
Type a number: Length of the Hailstone sequence for 123 is 46
&nbsp;
 Performance counter stats for './collatz_predication' (5 runs):
&nbsp;
       2318,217200 cpu-clock                  ( +-  0,01% )
&nbsp;
       2,322732232 seconds time elapsed                                          ( +-  0,01% )</pre></td></tr></tbody></table><p class="theCode" style="display:none;">$ yes 123 | perf_3.2 stat --log-fd=3 --repeat=5 -e cpu-clock ./collatz_predication 3&gt;&amp;1
Type a number: Length of the Hailstone sequence for 123 is 46
Type a number: Length of the Hailstone sequence for 123 is 46
Type a number: Length of the Hailstone sequence for 123 is 46
Type a number: Length of the Hailstone sequence for 123 is 46
Type a number: Length of the Hailstone sequence for 123 is 46

 Performance counter stats for './collatz_predication' (5 runs):

       2318,217200 cpu-clock                  ( +-  0,01% )

       2,322732232 seconds time elapsed                                          ( +-  0,01% )</p></div>

<p>
So the answer is, yes. In <strong>this case</strong> it does make a difference. The predicated version runs 1,44 times faster than the version using branches. It would be bold, though, to assume that in general predication outperforms branches. Always measure your time.
</p>
<p>
That’s all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2013/03/16/arm-assembler-raspberry-pi-chapter-11/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2013/03/16/arm-assembler-raspberry-pi-chapter-11/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2013/03/16/arm-assembler-raspberry-pi-chapter-11/&amp;text=ARM+assembler+in+Raspberry+Pi+%E2%80%93+Chapter+11+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2013/03/16/arm-assembler-raspberry-pi-chapter-11/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>				<p class="tags"><span><a href="http://thinkingeek.com/tag/arm/" rel="tag">arm</a>, <a href="http://thinkingeek.com/tag/assembler/" rel="tag">assembler</a>, <a href="http://thinkingeek.com/tag/branches/" rel="tag">branches</a>, <a href="http://thinkingeek.com/tag/function/" rel="tag">function</a>, <a href="http://thinkingeek.com/tag/function-call/" rel="tag">function call</a>, <a href="http://thinkingeek.com/tag/functions/" rel="tag">functions</a>, <a href="http://thinkingeek.com/tag/pi/" rel="tag">pi</a>, <a href="http://thinkingeek.com/tag/predication/" rel="tag">predication</a>, <a href="http://thinkingeek.com/tag/raspberry/" rel="tag">raspberry</a></span></p>		<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2013/02/07/arm-assembler-raspberry-pi-chapter-10/" rel="prev">ARM assembler in Raspberry Pi – Chapter 10</a></span>
			<span class="next"><a href="http://thinkingeek.com/2013/03/28/arm-assembler-raspberry-pi-chapter-12/" rel="next">ARM assembler in Raspberry Pi – Chapter 12</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
			<h3>8 thoughts on “<span>ARM assembler in Raspberry Pi – Chapter 11</span>”</h3>
		<ul class="commentlist">
				<li class="comment even thread-even depth-1 parent" id="comment-1130">
				<div id="div-comment-1130" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/9e81362ff9654533accffc53762c9520?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-9e81362ff9654533accffc53762c9520-0">			<cite class="fn">Fernando</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/03/16/arm-assembler-raspberry-pi-chapter-11/#comment-1130">
			April 2, 2013 at 8:42 am</a>		</div>

		<p>Hi, why is predication supposed to be faster? As far as I can see, both ways there is a number of instructions that will be “noped”.</p>
<p>Even more, if the if…then…else is ‘big’ (let’s say 10 or more instructions in each case), there will be much more instructions wasted than with a typical branch.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/03/16/arm-assembler-raspberry-pi-chapter-11/?replytocom=1130#respond" onclick="return addComment.moveForm( &quot;div-comment-1130&quot;, &quot;1130&quot;, &quot;respond&quot;, &quot;772&quot; )" aria-label="Reply to Fernando">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-1140">
				<div id="div-comment-1140" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-0">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/03/16/arm-assembler-raspberry-pi-chapter-11/#comment-1140">
			April 6, 2013 at 9:28 pm</a>		</div>

		<p>It is not supposed to be always faster. It just happens that in some cases, like in this 3n+1 example, a small predicated part may beat branching if the branches are hard to predict.</p>
<p>Of course, predicating big chunks of instructions is not beneficial, as the processor will be doing nothing for them. In such cases branching is the right choice.</p>
<p>No solution is always better. Sometimes predication is better, sometimes branching is the way to go.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/03/16/arm-assembler-raspberry-pi-chapter-11/?replytocom=1140#respond" onclick="return addComment.moveForm( &quot;div-comment-1140&quot;, &quot;1140&quot;, &quot;respond&quot;, &quot;772&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-odd thread-alt depth-1 parent" id="comment-1145">
				<div id="div-comment-1145" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://2.gravatar.com/avatar/84c0d08c07046bcb1196822a9feef78d?s=54&amp;d=mm&amp;r=g" srcset="http://2.gravatar.com/avatar/84c0d08c07046bcb1196822a9feef78d?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://2.gravatar.com/avatar/84c0d08c07046bcb1196822a9feef78d?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-84c0d08c07046bcb1196822a9feef78d-0">			<cite class="fn"><a href="http://xpl0.org/" rel="external nofollow" class="url">Loren Blaney</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/03/16/arm-assembler-raspberry-pi-chapter-11/#comment-1145">
			April 7, 2013 at 6:50 pm</a>		</div>

		<p>These lessons have been very helpful. Thanks!</p>
<p>I’ve just gotten my XPL0 compiler to generate enough arm assembly code to run the classic Sieve of Eratosthenes benchmark. I’m really tired of seeing “Segmentation fault.” I haven’t found the manual that explains how the gcc assembler works.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/03/16/arm-assembler-raspberry-pi-chapter-11/?replytocom=1145#respond" onclick="return addComment.moveForm( &quot;div-comment-1145&quot;, &quot;1145&quot;, &quot;respond&quot;, &quot;772&quot; )" aria-label="Reply to Loren Blaney">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-1171">
				<div id="div-comment-1171" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-1">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/03/16/arm-assembler-raspberry-pi-chapter-11/#comment-1171">
			April 11, 2013 at 8:51 pm</a>		</div>

		<p>Glad to know your XPL0 compiler is starting to be useful!</p>
<p>Regarding the “gcc assembler” issue you had, you’ll never find a manual. GCC targets many architectures (arm, x86, powerpc, etc), so they generate different assembler for each one. Every architecture is different, so you first learn assembler and then you map your programming language to that assembler. Of course, the intermediate steps are more complicated than that, but you get the idea <img draggable="false" class="emoji" alt="🙂" src="https://s.w.org/images/core/emoji/2.2.1/svg/1f642.svg" scale="0"></p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/03/16/arm-assembler-raspberry-pi-chapter-11/?replytocom=1171#respond" onclick="return addComment.moveForm( &quot;div-comment-1171&quot;, &quot;1171&quot;, &quot;respond&quot;, &quot;772&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1" id="comment-1440">
				<div id="div-comment-1440" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a16ae1e69be669d6f2c5fa0972e39c8a?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a16ae1e69be669d6f2c5fa0972e39c8a?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a16ae1e69be669d6f2c5fa0972e39c8a?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a16ae1e69be669d6f2c5fa0972e39c8a-0">			<cite class="fn"><a href="http://digilander.libero.it/zantaz/" rel="external nofollow" class="url">Mariani Antonio Mario</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/03/16/arm-assembler-raspberry-pi-chapter-11/#comment-1440">
			July 1, 2013 at 5:28 am</a>		</div>

		<p>very very very nice code … i loved it …</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/03/16/arm-assembler-raspberry-pi-chapter-11/?replytocom=1440#respond" onclick="return addComment.moveForm( &quot;div-comment-1440&quot;, &quot;1440&quot;, &quot;respond&quot;, &quot;772&quot; )" aria-label="Reply to Mariani Antonio Mario">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment odd alt thread-odd thread-alt depth-1" id="comment-157446">
				<div id="div-comment-157446" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/c5128194ae0fa12dc2c34b146c3d11a5?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/c5128194ae0fa12dc2c34b146c3d11a5?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://0.gravatar.com/avatar/c5128194ae0fa12dc2c34b146c3d11a5?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-c5128194ae0fa12dc2c34b146c3d11a5-0">			<cite class="fn"><a href="http://antoniovillena.es" rel="external nofollow" class="url">Antonio Villena</a></cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/03/16/arm-assembler-raspberry-pi-chapter-11/#comment-157446">
			June 15, 2014 at 3:49 pm</a>		</div>

		<p>I have optimized a little:</p>
<p><code><br>
main:   mov     r1, #123            /* r1 &lt;- 123 */<br>
        mov     r0, #0              /* r0 &lt;- 0 */<br>
loop:   add     r0, r0, #1          /* r0 &lt;- r0 + 1 */<br>
        movs    r1, r1, ASR #1      /* r1 &gt; 1) */<br>
        bxeq    lr<br>
        bcc     loop<br>
        adc     r1, r1, r1<br>
        adc     r1, r1, r1, LSL #1  /* r1 &lt;- r1 + (r1 &lt;&lt; 1) */<br>
        b       loop<br>
</code></p>
<p>Or predicated version:</p>
<p><code><br>
main:   mov     r1, #123            /* r1 &lt;- 123 */<br>
        mov     r0, #0              /* r0 &lt;- 0 */<br>
loop:   add     r0, r0, #1          /* r0 &lt;- r0 + 1 */<br>
        movs    r1, r1, ASR #1      /* r1 &gt; 1) */<br>
        bxeq    lr<br>
        adccs   r1, r1, r1<br>
        adccs   r1, r1, r1, LSL #1  /* r1 &lt;- r1 + (r1 &lt;&lt; 1) */<br>
        b       loop<br>
</code></p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/03/16/arm-assembler-raspberry-pi-chapter-11/?replytocom=157446#respond" onclick="return addComment.moveForm( &quot;div-comment-157446&quot;, &quot;157446&quot;, &quot;respond&quot;, &quot;772&quot; )" aria-label="Reply to Antonio Villena">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
		<li class="comment even thread-even depth-1 parent" id="comment-969498">
				<div id="div-comment-969498" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/7c2d18fc22887437f09f99f2c60ab15d?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/7c2d18fc22887437f09f99f2c60ab15d?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/7c2d18fc22887437f09f99f2c60ab15d?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-7c2d18fc22887437f09f99f2c60ab15d-0">			<cite class="fn">Dennis Ng</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/03/16/arm-assembler-raspberry-pi-chapter-11/#comment-969498">
			June 11, 2016 at 4:58 am</a>		</div>

		<p>119.247.241.235	</p>
<p>It seemed it is not a hardware issue as the latest Jessie seemed to be able to run the cpu-cycles pref command (need to get to linux-tools 4.4 using upgrade/install/install 4.4 works). The error below is about Make which I after cutting everything still cannot make your command work under Make: (The last command is by cut-and-past and it works.)</p>
<p><code></code></p><code>
<p>Type a number: Length of the Hailstone sequence for 123 is 46<br>
Type a number: Length of the Hailstone sequence for 123 is 46<br>
Type a number: Length of the Hailstone sequence for 123 is 46<br>
Type a number: Length of the Hailstone sequence for 123 is 46<br>
Type a number: Length of the Hailstone sequence for 123 is 46</p>
<p>Performance counter stats for ‘./collatz1’ (5 runs):</p>
<p>6.189200 cpu-clock (msec) ( +- 1.52% )</p>
<p>0.011474382 seconds time elapsed ( +- 0.40% )</p>
<p>Makefile:42: recipe for target ‘perf1’ failed<br>
make: *** [perf1] Error 47<br>
d@raspberrypi:~/ch11 $ yes 123 | perf stat –repeat=5 -e cpu-clock ./collatz1<br>
Type a number: Length of the Hailstone sequence for 123 is 46<br>
Type a number: Length of the Hailstone sequence for 123 is 46<br>
Type a number: Length of the Hailstone sequence for 123 is 46<br>
Type a number: Length of the Hailstone sequence for 123 is 46<br>
Type a number: Length of the Hailstone sequence for 123 is 46</p>
<p>Performance counter stats for ‘./collatz1’ (5 runs):</p>
<p>6.151000 cpu-clock (msec) ( +- 1.57% )</p>
<p>0.011320386 seconds time elapsed ( +- 0.37% )</p>
<p>d@raspberrypi:~/ch11 $ yes 123 | perf stat –repeat=5 -e cpu-cycles ./collatz1<br>
Type a number: Length of the Hailstone sequence for 123 is 46<br>
Type a number: Length of the Hailstone sequence for 123 is 46<br>
Type a number: Length of the Hailstone sequence for 123 is 46<br>
Type a number: Length of the Hailstone sequence for 123 is 46<br>
Type a number: Length of the Hailstone sequence for 123 is 46</p>
<p>Performance counter stats for ‘./collatz1’ (5 runs):</p>
<p>4258777 cpu-cycles ( +- 1.08% )</p>
<p>0.012091358 seconds time elapsed ( +- 5.45% )</p>
<p>d@raspberrypi:~/ch11 $</p>
</code><p><code></code></p>
<p>Not sure what is #3&gt;&amp;1 and as Make cannot handle the –log-fd=3, I delete that phase.</p>
<p>Strangely the speed of 2 is slower than 1 i.e. prediction does not work:</p>
<p><code></code></p><code>
<p>model name : ARMv6-compatible processor rev 7 (v6l)<br>
BogoMIPS : 697.95<br>
Features : half thumb fastmult vfp edsp java tls<br>
CPU implementer : 0x41<br>
CPU architecture: 7<br>
CPU variant : 0x0<br>
CPU part : 0xb76<br>
CPU revision : 7</p>
<p>Hardware : BCM2708<br>
Revision : 0002<br>
Serial : 000000002ba0c3d4<br>
#perf stat -e cpu-cycles ./collatz2<br>
yes 123 | perf stat –repeat=5 -e cpu-clock ./collatz2 #3&gt;&amp;1<br>
Type a number: Length of the Hailstone sequence for 123 is 46<br>
Type a number: Length of the Hailstone sequence for 123 is 46<br>
Type a number: Length of the Hailstone sequence for 123 is 46<br>
Type a number: Length of the Hailstone sequence for 123 is 46<br>
Type a number: Length of the Hailstone sequence for 123 is 46</p>
<p>Performance counter stats for ‘./collatz2’ (5 runs):</p>
<p>2327.793800 cpu-clock (msec) ( +- 0.01% )</p>
<p>2.344818639 seconds time elapsed ( +- 0.07% )</p>
<p>Makefile:60: recipe for target ‘perf2’ failed<br>
make: *** [perf2] Error 47<br>
d@raspberrypi:~/ch11 $ yes 123 | perf stat –repeat=5 -e cpu-cycles ./collatz2 #3&gt;&amp;1<br>
Type a number: Length of the Hailstone sequence for 123 is 46<br>
Type a number: Length of the Hailstone sequence for 123 is 46<br>
Type a number: Length of the Hailstone sequence for 123 is 46<br>
Type a number: Length of the Hailstone sequence for 123 is 46<br>
Type a number: Length of the Hailstone sequence for 123 is 46</p>
<p>Performance counter stats for ‘./collatz2’ (5 runs):</p>
<p>1629471236 cpu-cycles ( +- 0.00% )</p>
<p>2.345886541 seconds time elapsed ( +- 0.07% )</p>
<p>d@raspberrypi:~/ch11 $</p>
</code><p><code></code></p>
<p>The result is the same under rasp pi 2 (the previous one are all pi 1 or B):</p>
<p><code><br>
d@raspberrypi:~/ch11 $ make perf2<br>
uname -a<br>
Linux raspberrypi 4.4.11-v7+ #888 SMP Mon May 23 20:10:33 BST 2016 armv7l GNU/Linux<br>
cat /proc/cpuinfo<br>
processor : 0<br>
model name : ARMv7 Processor rev 5 (v7l)<br>
BogoMIPS : 38.40<br>
Features : half thumb fastmult vfp edsp neon vfpv3 tls vfpv4 idiva idivt vfpd32 lpae evtstrm<br>
CPU implementer : 0x41<br>
CPU architecture: 7<br>
CPU variant : 0x0<br>
CPU part : 0xc07<br>
CPU revision : 5</code></p><code>
<p>processor : 1<br>
model name : ARMv7 Processor rev 5 (v7l)<br>
BogoMIPS : 38.40<br>
Features : half thumb fastmult vfp edsp neon vfpv3 tls vfpv4 idiva idivt vfpd32 lpae evtstrm<br>
CPU implementer : 0x41<br>
CPU architecture: 7<br>
CPU variant : 0x0<br>
CPU part : 0xc07<br>
CPU revision : 5</p>
<p>processor : 2<br>
model name : ARMv7 Processor rev 5 (v7l)<br>
BogoMIPS : 38.40<br>
Features : half thumb fastmult vfp edsp neon vfpv3 tls vfpv4 idiva idivt vfpd32 lpae evtstrm<br>
CPU implementer : 0x41<br>
CPU architecture: 7<br>
CPU variant : 0x0<br>
CPU part : 0xc07<br>
CPU revision : 5</p>
<p>processor : 3<br>
model name : ARMv7 Processor rev 5 (v7l)<br>
BogoMIPS : 38.40<br>
Features : half thumb fastmult vfp edsp neon vfpv3 tls vfpv4 idiva idivt vfpd32 lpae evtstrm<br>
CPU implementer : 0x41<br>
CPU architecture: 7<br>
CPU variant : 0x0<br>
CPU part : 0xc07<br>
CPU revision : 5</p>
<p>Hardware : BCM2709<br>
Revision : a21041<br>
Serial : 000000000644243a<br>
#perf stat -e cpu-cycles ./collatz2<br>
yes 123 | perf stat –repeat=5 -e cpu-clock ./collatz2 #3&gt;&amp;1<br>
Type a number: Length of the Hailstone sequence for 123 is 46<br>
Type a number: Length of the Hailstone sequence for 123 is 46<br>
Type a number: Length of the Hailstone sequence for 123 is 46<br>
Type a number: Length of the Hailstone sequence for 123 is 46<br>
Type a number: Length of the Hailstone sequence for 123 is 46</p>
<p>Performance counter stats for ‘./collatz2’ (5 runs):</p>
<p>1560.273734 cpu-clock (msec) ( +- 0.14% )</p>
<p>1.562665742 seconds time elapsed ( +- 0.16% )</p>
</code><p><code>Makefile:61: recipe for target ‘perf2’ failed<br>
make: *** [perf2] Error 47<br>
d@raspberrypi:~/ch11 $<br>
</code></p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/03/16/arm-assembler-raspberry-pi-chapter-11/?replytocom=969498#respond" onclick="return addComment.moveForm( &quot;div-comment-969498&quot;, &quot;969498&quot;, &quot;respond&quot;, &quot;772&quot; )" aria-label="Reply to Dennis Ng">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2" id="comment-970003">
				<div id="div-comment-970003" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-2">			<cite class="fn">Roger Ferrer Ibáñez</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/03/16/arm-assembler-raspberry-pi-chapter-11/#comment-970003">
			June 19, 2016 at 4:58 pm</a>		</div>

		<p>Hi Dennis,</p>
<p>I’m not sure what is <code>collatz1</code> or <code>collatz2</code> in your case, but it seems that there is a difference indeed of performance in the two versions.</p>
<p>I assume your results do not match with mine, do they?</p>
<p>The difference on the <code>perf</code> command is likely due to the Raspbian support of linux-tools at the time of writing this article in 2013. Glad to see <code>linux-tools</code> is better supported.</p>
<p>Regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/03/16/arm-assembler-raspberry-pi-chapter-11/?replytocom=970003#respond" onclick="return addComment.moveForm( &quot;div-comment-970003&quot;, &quot;970003&quot;, &quot;respond&quot;, &quot;772&quot; )" aria-label="Reply to Roger Ferrer Ibáñez">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		</ul>
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2013/03/16/arm-assembler-raspberry-pi-chapter-11/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="772" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="3a8120d290"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496664643149"></form>
			</div><!-- #respond -->
	</div>	</div>