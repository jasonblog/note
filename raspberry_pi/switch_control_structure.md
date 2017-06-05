# Switch control structure


<div class="main">
			<h1>ARM assembler in Raspberry Pi – Chapter 16 </h1>
		<p class="post-meta"><span class="date">August 23, 2013</span> <span class="author">Roger Ferrer Ibáñez</span>, <span class="comments"><a href="http://thinkingeek.com/2013/08/23/arm-assembler-raspberry-pi-chapter-16/#comments">4</a></span> </p>
		<p>
We saw in chapters 6 and 12 several control structures but we left out a usual one: the <em>switch</em> also known as <em>select</em>/<em>case</em>. In this chapter we will see how we can implement it in ARM assembler.
</p>
<p><span id="more-1279"></span></p>
<h2>Switch control structure</h2>
<p>
A switch in C has the following structure.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #b1b100;">switch</span> <span style="color: #009900;">(</span>E<span style="color: #009900;">)</span>
<span style="color: #009900;">{</span>
   <span style="color: #b1b100;">case</span> V1<span style="color: #339933;">:</span> S1<span style="color: #339933;">;</span>
   <span style="color: #b1b100;">case</span> V2<span style="color: #339933;">:</span> S2<span style="color: #339933;">;</span>
   <span style="color: #b1b100;">default</span><span style="color: #339933;">:</span> Sdefault<span style="color: #339933;">;</span>
<span style="color: #009900;">}</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">switch (E)
{
   case V1: S1;
   case V2: S2;
   default: Sdefault;
}</p></div>

<p>
In the example above, expression <code>E</code> is evaluated and its value is used to determine the next statement executed. So if <code>E</code> evaluates to <code>V2</code>, <code>S2</code> will be the next statement executed. If no <code>case</code> matches, the whole <code>switch</code> construct is ignored unless there is a <code>default</code> case the statement of which is executed instead.
</p>
<p>
Note that, once the flow jumps to a statement, the execution continues from that point unless a <code>break</code> statement is found. The <code>break</code> statement <code>switch</code> construct. Most of the time the programmer adds a <code>break</code> to end each case. Otherwise <em>fall-through</em> cases happens. In the example above, if <code>E</code> evaluates to <code>V1</code> and there is no break in <code>S1</code>, the program would continue running <code>S2</code> and <code>Sdefault</code> unless the program encounters a <code>break</code> statement inside <code>S2</code> or <code>Sdefault</code>. Fall-through may look a bit weird and confusing but there are some cases where is useful.
</p>
<p>
That said, C is a particularly bad example to showcase this structure. The reason is that the exact language definition of a <code>switch</code> in C is as follows.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #b1b100;">switch</span> <span style="color: #009900;">(</span>E<span style="color: #009900;">)</span>
   S<span style="color: #339933;">;</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">switch (E)
   S;</p></div>

<p>
<code>S</code> can be anything but the flow will always jump to a <code>case</code> or a <code>default</code> inside <code>S</code>, so if <code>S</code> does not contain any of these statements, nothing happens.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #b1b100;">switch</span> <span style="color: #009900;">(</span>E<span style="color: #009900;">)</span>
  <span style="color: #000066;">printf</span><span style="color: #009900;">(</span><span style="color: #ff0000;">"This will never be printed<span style="color: #000099; font-weight: bold;">\n</span>"</span><span style="color: #009900;">)</span><span style="color: #339933;">;</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">switch (E)
  printf("This will never be printed\n");</p></div>

<p>
So for a <code>switch</code> to be useful we will need at least one <code>case</code> or <code>default</code> statement. If more than one is needed, then we can use a compound statement (a list of statements enclosed in side <code>{</code> and <code>}</code> as shown in the first example above.
</p>
<p>
Note also, that <code>case</code> and <code>default</code> statements are only valid inside the <code>S</code> of a <code>switch</code> but this does not mean that they have to be immediately nested inside them.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #b1b100;">switch</span> <span style="color: #009900;">(</span>E<span style="color: #009900;">)</span>
<span style="color: #009900;">{</span>
  <span style="color: #b1b100;">if</span> <span style="color: #009900;">(</span>X<span style="color: #009900;">)</span> <span style="color: #666666; font-style: italic;">// Note that the check of the truth value of X will be never run!</span>
  <span style="color: #009900;">{</span>
     <span style="color: #b1b100;">default</span><span style="color: #339933;">:</span> <span style="color: #000066;">printf</span> <span style="color: #009900;">(</span><span style="color: #ff0000;">"Hi!<span style="color: #000099; font-weight: bold;">\n</span>"</span><span style="color: #009900;">)</span><span style="color: #339933;">;</span>
  <span style="color: #009900;">}</span>
  <span style="color: #b1b100;">else</span>
  <span style="color: #009900;">{</span>
     <span style="color: #b1b100;">case</span> <span style="color: #0000dd;">10</span><span style="color: #339933;">:</span> <span style="color: #000066;">printf</span> <span style="color: #009900;">(</span><span style="color: #ff0000;">"Howdy stranger!<span style="color: #000099; font-weight: bold;">\n</span>"</span><span style="color: #009900;">)</span><span style="color: #339933;">;</span>
  <span style="color: #009900;">}</span>
<span style="color: #009900;">}</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">switch (E)
{
  if (X) // Note that the check of the truth value of X will be never run!
  {
     default: printf ("Hi!\n");
  }
  else
  {
     case 10: printf ("Howdy stranger!\n");
  }
}</p></div>

<p>
As you can see, the <code>switch</code> statement in C is pretty liberal. Other languages, like <a href="http://www.freepascal.org/docs-html/ref/refsu50.html#x143-15300013.2.2">Pascal</a> or <a href="http://www.cs.mtu.edu/~shene/COURSES/cs201/NOTES/chap03/select.html">Fortran</a>, have stricter syntaxes that do not allow fall-through nor loose positioning of case/default.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="pascal" style="font-family:monospace;"><span style="color: #808080; font-style: italic;">{ Case statement in Pascal }</span>
<span style="color: #000000; font-weight: bold;">Case</span> Number <span style="color: #000000; font-weight: bold;">of</span>  
 <span style="color: #cc66cc;">1</span> <span style="color: #000066;">:</span> <span style="color: #000066;">WriteLn</span> <span style="color: #009900;">(</span><span style="color: #ff0000;">'One'</span><span style="color: #009900;">)</span><span style="color: #000066;">;</span>  
 <span style="color: #cc66cc;">2</span> <span style="color: #000066;">:</span> <span style="color: #000066;">WriteLn</span> <span style="color: #009900;">(</span><span style="color: #ff0000;">'Two'</span><span style="color: #009900;">)</span><span style="color: #000066;">;</span>  
<span style="color: #000000; font-weight: bold;">Else</span>  
 <span style="color: #000066;">WriteLn</span> <span style="color: #009900;">(</span><span style="color: #ff0000;">'Other than one or two'</span><span style="color: #009900;">)</span><span style="color: #000066;">;</span>  
<span style="color: #000000; font-weight: bold;">End</span><span style="color: #000066;">;</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">{ Case statement in Pascal }
Case Number of  
 1 : WriteLn ('One');  
 2 : WriteLn ('Two');  
Else  
 WriteLn ('Other than one or two');  
End;</p></div>

<p>
In this post, we will not care about these strange cases of <code>switch</code> although we will allow fall-through.
</p>
<h2>Implementing switch</h2>
<p>
Probably you already have figured that a switch not involving fall-through in any of its cases is equivalent to a sequence of if-else blocks. The following <code>switch</code>,</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #b1b100;">switch</span> <span style="color: #009900;">(</span>x<span style="color: #009900;">)</span>
<span style="color: #009900;">{</span>
  <span style="color: #b1b100;">case</span> <span style="color: #0000dd;">5</span><span style="color: #339933;">:</span> code_for_case5<span style="color: #339933;">;</span> <span style="color: #000000; font-weight: bold;">break</span><span style="color: #339933;">;</span>
  <span style="color: #b1b100;">case</span> <span style="color: #0000dd;">10</span><span style="color: #339933;">:</span> code_for_case10<span style="color: #339933;">;</span> <span style="color: #000000; font-weight: bold;">break</span><span style="color: #339933;">;</span>
  <span style="color: #b1b100;">default</span><span style="color: #339933;">:</span> code_for_default<span style="color: #339933;">;</span> <span style="color: #000000; font-weight: bold;">break</span><span style="color: #339933;">;</span> 
  <span style="color: #666666; font-style: italic;">// break would not be required here as this is the last case</span>
<span style="color: #009900;">}</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">switch (x)
{
  case 5: code_for_case5; break;
  case 10: code_for_case10; break;
  default: code_for_default; break; 
  // break would not be required here as this is the last case
}</p></div>

<p>
can be implemented as
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #b1b100;">if</span> <span style="color: #009900;">(</span>x <span style="color: #339933;">==</span> <span style="color: #0000dd;">5</span><span style="color: #009900;">)</span>
  code_for_case5<span style="color: #339933;">;</span>
<span style="color: #b1b100;">else</span> <span style="color: #b1b100;">if</span> <span style="color: #009900;">(</span>x <span style="color: #339933;">==</span> <span style="color: #0000dd;">10</span><span style="color: #009900;">)</span>
  code_for_case10<span style="color: #339933;">;</span>
<span style="color: #b1b100;">else</span> <span style="color: #808080; font-style: italic;">/* default */</span>
  code_for_default<span style="color: #339933;">;</span>
&nbsp;
code_after<span style="color: #339933;">;</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">if (x == 5)
  code_for_case5;
else if (x == 10)
  code_for_case10;
else /* default */
  code_for_default;

code_after;</p></div>

<p>
In contrast to the usual if-else statement, there need not be a branch that goes after the if-statement once the if branch has been executed. This is, in the example above, it is optional to have a branch after <code>code_for_case5</code> that goes to <code>code_after</code>. If such branch is omitted, then a fall-through to <code>code_for_case10</code> happens naturally. So the <code>break</code> statement inside a <code>switch</code> is simply that unconditional branch.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">  <span style="color: #339933;">/*</span> Here we evaluate x <span style="color: #00007f; font-weight: bold;">and</span> keep it <span style="color: #00007f; font-weight: bold;">in</span> r0 <span style="color: #339933;">*/</span>
  case_5<span style="color: #339933;">:</span>             <span style="color: #339933;">/*</span> case <span style="color: #ff0000;">5</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">5</span>        <span style="color: #339933;">/*</span> Compute r0 <span style="color: #339933;">-</span> <span style="color: #ff0000;">5</span> <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
    bne case_10       <span style="color: #339933;">/*</span> if r0 != <span style="color: #ff0000;">5</span> branch to case_10 <span style="color: #339933;">*/</span>
    code_for_case5
    b after_switch    <span style="color: #339933;">/*</span> break <span style="color: #339933;">*/</span>
&nbsp;
  case_10<span style="color: #339933;">:</span>            <span style="color: #339933;">/*</span> case <span style="color: #ff0000;">10</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">10</span>       <span style="color: #339933;">/*</span> Compute r0 <span style="color: #339933;">-</span> <span style="color: #ff0000;">10</span> <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
    bne case_default  <span style="color: #339933;">/*</span> If r0 != <span style="color: #ff0000;">10</span> branch to case_default <span style="color: #339933;">*/</span>
    code_for_case10
    b after_switch    <span style="color: #339933;">/*</span> break <span style="color: #339933;">*/</span>
&nbsp;
  case_default<span style="color: #339933;">:</span>
    code_for_default 
    <span style="color: #339933;">/*</span> Note that if <span style="color: #0000ff; font-weight: bold;">default</span> is <span style="color: #00007f; font-weight: bold;">not</span> the last case
       we need a branch to after_switch here <span style="color: #339933;">*/</span>
&nbsp;
  after_switch<span style="color: #339933;">:</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">  /* Here we evaluate x and keep it in r0 */
  case_5:             /* case 5 */
    cmp r0, #5        /* Compute r0 - 5 and update cpsr */
    bne case_10       /* if r0 != 5 branch to case_10 */
    code_for_case5
    b after_switch    /* break */

  case_10:            /* case 10 */
    cmp r0, #10       /* Compute r0 - 10 and update cpsr */
    bne case_default  /* If r0 != 10 branch to case_default */
    code_for_case10
    b after_switch    /* break */

  case_default:
    code_for_default 
    /* Note that if default is not the last case
       we need a branch to after_switch here */

  after_switch:</p></div>

<p>
We can put all the checks at the beginning, as long as we preserve the order of the cases (so fall-through works if <code>break</code> is omitted).
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">  <span style="color: #339933;">/*</span> Here we evaluate x <span style="color: #00007f; font-weight: bold;">and</span> keep it <span style="color: #00007f; font-weight: bold;">in</span> r0 <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">5</span>        <span style="color: #339933;">/*</span> Compute r0 <span style="color: #339933;">-</span> <span style="color: #ff0000;">5</span> <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
  beq case_5        <span style="color: #339933;">/*</span> if r0 == <span style="color: #ff0000;">5</span> branch to case_5 <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">10</span>       <span style="color: #339933;">/*</span> Compute r0 <span style="color: #339933;">-</span> <span style="color: #ff0000;">10</span> <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
  beq case_10       <span style="color: #339933;">/*</span> if r0 == <span style="color: #ff0000;">10</span> branch to case_10 <span style="color: #339933;">*/</span>
  b case_default    <span style="color: #339933;">/*</span> branch to <span style="color: #0000ff; font-weight: bold;">default</span> case
                       Note that there is no <span style="color: #0000ff; font-weight: bold;">default</span> case
                       we would branch to after_switch <span style="color: #339933;">*/</span>
&nbsp;
  case_5<span style="color: #339933;">:</span>             <span style="color: #339933;">/*</span> case <span style="color: #ff0000;">5</span> <span style="color: #339933;">*/</span>
    code_for_case5
    b after_switch    <span style="color: #339933;">/*</span> break <span style="color: #339933;">*/</span>
&nbsp;
  case_10<span style="color: #339933;">:</span>            <span style="color: #339933;">/*</span> case <span style="color: #ff0000;">10</span> <span style="color: #339933;">*/</span>
    code_for_case10
    b after_switch    <span style="color: #339933;">/*</span> break <span style="color: #339933;">*/</span>
&nbsp;
  case_default<span style="color: #339933;">:</span>
    code_for_default 
    <span style="color: #339933;">/*</span> Note that if <span style="color: #0000ff; font-weight: bold;">default</span> is <span style="color: #00007f; font-weight: bold;">not</span> the last case
       we need a branch to after_switch here <span style="color: #339933;">*/</span>
&nbsp;
  after_switch<span style="color: #339933;">:</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">  /* Here we evaluate x and keep it in r0 */
  cmp r0, #5        /* Compute r0 - 5 and update cpsr */
  beq case_5        /* if r0 == 5 branch to case_5 */
  cmp r0, #10       /* Compute r0 - 10 and update cpsr */
  beq case_10       /* if r0 == 10 branch to case_10 */
  b case_default    /* branch to default case
                       Note that there is no default case
                       we would branch to after_switch */

  case_5:             /* case 5 */
    code_for_case5
    b after_switch    /* break */

  case_10:            /* case 10 */
    code_for_case10
    b after_switch    /* break */

  case_default:
    code_for_default 
    /* Note that if default is not the last case
       we need a branch to after_switch here */

  after_switch:</p></div>

<p>
This approach is sensible if the number of cases is low. Here “low” is not very well defined, let’s say 10 or less. What if we have lots of cases? A sequence of if-else checks will make as many comparisons as cases. If the values of the N cases are uniformly spread during the execution of the program, this means that in average we will have to do N/2 checks. If the values are not uniformly spread, then it is obvious that we should check the common values first and the rare last (sadly, most of the times we have no idea of their frequency).
</p>
<p>
There are a number of ways of reducing the cost of checking the cases: tables and binary search.
</p>
<h2>Jump tables</h2>
<p>
Imagine we have a <code>switch</code> like this one
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #b1b100;">switch</span> <span style="color: #009900;">(</span>x<span style="color: #009900;">)</span>
<span style="color: #009900;">{</span>
  <span style="color: #b1b100;">case</span> <span style="color: #0000dd;">1</span><span style="color: #339933;">:</span> do_something_1<span style="color: #339933;">;</span>
  <span style="color: #b1b100;">case</span> <span style="color: #0000dd;">2</span><span style="color: #339933;">:</span> do_something_2<span style="color: #339933;">;</span>
  ...
  <span style="color: #b1b100;">case</span> <span style="color: #0000dd;">100</span><span style="color: #339933;">:</span> do_something_100<span style="color: #339933;">;</span>
<span style="color: #009900;">}</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">switch (x)
{
  case 1: do_something_1;
  case 2: do_something_2;
  ...
  case 100: do_something_100;
}</p></div>

<p>
If we implement it the way shown above we will make in average (for an uniformly distributed set of values of <code>x</code>) 50 comparisons. We can improve this if we simply use the value <code>c</code> to index a table of addresses to the instructions of the case instructions.
</p>
<p>
Consider the program below where we use the value of <code>argc</code> of a C program. In C, the <code>main</code> function receives two parameters, <code>argc</code> and <code>argv</code>: <code>argc</code> is just an integer, in the register <code>r0</code> as usual; <code>argv</code> is an address, in the register <code>r1</code> as usual, to an array of the arguments passed in the command-line. There are as many elements in <code>argv</code> as the value of <code>argc</code>, at least one. We will not use <code>argv</code> today, only <code>argc</code>.
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="c" style="font-family:monospace;"><span style="color: #993333;">int</span> main<span style="color: #009900;">(</span><span style="color: #993333;">int</span> argc<span style="color: #339933;">,</span> <span style="color: #993333;">char</span> <span style="color: #339933;">*</span>argv<span style="color: #009900;">[</span><span style="color: #009900;">]</span><span style="color: #009900;">)</span>
<span style="color: #009900;">{</span>
   <span style="color: #993333;">int</span> x<span style="color: #339933;">;</span>
   <span style="color: #b1b100;">switch</span> <span style="color: #009900;">(</span>argc<span style="color: #009900;">)</span>
   <span style="color: #009900;">{</span>
     <span style="color: #b1b100;">case</span> <span style="color: #0000dd;">1</span><span style="color: #339933;">:</span> x <span style="color: #339933;">=</span> <span style="color: #0000dd;">1</span><span style="color: #339933;">;</span> <span style="color: #000000; font-weight: bold;">break</span><span style="color: #339933;">;</span>
     <span style="color: #b1b100;">case</span> <span style="color: #0000dd;">2</span><span style="color: #339933;">:</span> x <span style="color: #339933;">=</span> <span style="color: #0000dd;">2</span><span style="color: #339933;">;</span> <span style="color: #000000; font-weight: bold;">break</span><span style="color: #339933;">;</span>
     <span style="color: #b1b100;">case</span> <span style="color: #0000dd;">3</span><span style="color: #339933;">:</span> x <span style="color: #339933;">=</span> <span style="color: #0000dd;">3</span><span style="color: #339933;">;</span> <span style="color: #000000; font-weight: bold;">break</span><span style="color: #339933;">;</span>
     <span style="color: #b1b100;">default</span><span style="color: #339933;">:</span> x <span style="color: #339933;">=</span> <span style="color: #0000dd;">42</span><span style="color: #339933;">;</span> <span style="color: #000000; font-weight: bold;">break</span><span style="color: #339933;">;</span>
   <span style="color: #009900;">}</span>
   <span style="color: #b1b100;">return</span> x<span style="color: #339933;">;</span>
<span style="color: #009900;">}</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">int main(int argc, char *argv[])
{
   int x;
   switch (argc)
   {
     case 1: x = 1; break;
     case 2: x = 2; break;
     case 3: x = 3; break;
     default: x = 42; break;
   }
   return x;
}</p></div>

<p>
We are using just 3 cases plus the default one, but it would not be complex (yet cumbersome) to extend it to 100 cases.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> jumptable<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
<span style="color: #339933;">.</span>globl main
&nbsp;
main<span style="color: #339933;">:</span>
  <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>                  <span style="color: #339933;">/*</span> r0 <span style="color: #339933;">-</span> <span style="color: #ff0000;">1</span> <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
  blt case_default            <span style="color: #339933;">/*</span> branch to case_default if r0 &lt; <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">3</span>                  <span style="color: #339933;">/*</span> r0 <span style="color: #339933;">-</span> <span style="color: #ff0000;">3</span> <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
  bgt case_default            <span style="color: #339933;">/*</span> branch to case_default if r0 &gt; <span style="color: #ff0000;">3</span> <span style="color: #339933;">*/</span>
&nbsp;
  <span style="color: #00007f; font-weight: bold;">sub</span> r0<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>              <span style="color: #339933;">/*</span> r0 ← r0 <span style="color: #339933;">-</span> <span style="color: #ff0000;">1</span><span style="color: #339933;">.</span> Required to index the table <span style="color: #339933;">*/</span>
  ldr r1<span style="color: #339933;">,</span> addr_of_jump_table  <span style="color: #339933;">/*</span> r1 ← &amp;jump_table <span style="color: #339933;">*/</span>
  ldr r1<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r1<span style="color: #339933;">,</span> <span style="color: #339933;">+</span>r0<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">]</span>   <span style="color: #339933;">/*</span> r1 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r1 <span style="color: #339933;">+</span> r0<span style="color: #339933;">*</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span>
                                 This is r1 ← jump_table<span style="color: #009900; font-weight: bold;">[</span>r0<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
&nbsp;
  <span style="color: #00007f; font-weight: bold;">mov</span> pc<span style="color: #339933;">,</span> r1                  <span style="color: #339933;">/*</span> pc ← r1
                                 This will cause a branch to the
                                 computed address <span style="color: #339933;">*/</span>
&nbsp;
  case_1<span style="color: #339933;">:</span>
   <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>                 <span style="color: #339933;">/*</span> r0 ← <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span> 
   b after_switch             <span style="color: #339933;">/*</span> break <span style="color: #339933;">*/</span>
&nbsp;
  case_2<span style="color: #339933;">:</span>
   <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span>                 <span style="color: #339933;">/*</span> r0 ← <span style="color: #ff0000;">2</span> <span style="color: #339933;">*/</span>
   b after_switch             <span style="color: #339933;">/*</span> break <span style="color: #339933;">*/</span>
&nbsp;
  case_3<span style="color: #339933;">:</span>
   <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">3</span>                 <span style="color: #339933;">/*</span> r0 ← <span style="color: #ff0000;">3</span> <span style="color: #339933;">*/</span>
   b after_switch             <span style="color: #339933;">/*</span> break <span style="color: #339933;">*/</span>
&nbsp;
  case_default<span style="color: #339933;">:</span>
   <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">42</span>                <span style="color: #339933;">/*</span> r0 ← <span style="color: #ff0000;">42</span> <span style="color: #339933;">*/</span>
   b after_switch             <span style="color: #339933;">/*</span> break <span style="color: #009900; font-weight: bold;">(</span>unnecessary<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
&nbsp;
  after_switch<span style="color: #339933;">:</span>
&nbsp;
  <span style="color: #46aa03; font-weight: bold;">bx</span> lr                       <span style="color: #339933;">/*</span> Return from main <span style="color: #339933;">*/</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
jump_table<span style="color: #339933;">:</span>
   <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> case_1
   <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> case_2
   <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> case_3
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
addr_of_jump_table<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> jump_table</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* jumptable.s */
.data

.text

.globl main

main:
  cmp r0, #1                  /* r0 - 1 and update cpsr */
  blt case_default            /* branch to case_default if r0 &lt; 1 */
  cmp r0, #3                  /* r0 - 3 and update cpsr */
  bgt case_default            /* branch to case_default if r0 &gt; 3 */

  sub r0, r0, #1              /* r0 ← r0 - 1. Required to index the table */
  ldr r1, addr_of_jump_table  /* r1 ← &amp;jump_table */
  ldr r1, [r1, +r0, LSL #2]   /* r1 ← *(r1 + r0*4).
                                 This is r1 ← jump_table[r0] */

  mov pc, r1                  /* pc ← r1
                                 This will cause a branch to the
                                 computed address */

  case_1:
   mov r0, #1                 /* r0 ← 1 */ 
   b after_switch             /* break */

  case_2:
   mov r0, #2                 /* r0 ← 2 */
   b after_switch             /* break */

  case_3:
   mov r0, #3                 /* r0 ← 3 */
   b after_switch             /* break */

  case_default:
   mov r0, #42                /* r0 ← 42 */
   b after_switch             /* break (unnecessary) */

  after_switch:

  bx lr                       /* Return from main */

.align 4
jump_table:
   .word case_1
   .word case_2
   .word case_3

.align 4
addr_of_jump_table: .word jump_table</p></div>

<p>
As you can see in line 43 we define a jump table, whose elements are the addresses of the labels of each case (in order). In lines 14 to 16 we load the appropiate value from that table after we are sure that the value of argc is between 1 and 3, checked in lines 9 to 12. Finally, we load the address to <code>pc</code>. This will effectively do a branch to the proper case.
</p>
<p>
If you run the program you will see diferent exit codes returned (remember that they are retuned through <code>r0</code> in <code>main</code>). The program only counts the arguments, if instead of “a b” you use “one two” it will return 3 as well. More than two arguments and it will return 42.
</p>
<pre>$ ./jumptable ; echo $?
1
$ ./jumptable a ; echo $?
2
$ ./jumptable a b  ; echo $?
3
$ ./jumptable a b c ; echo $?
42
</pre>
<p>
To safely use the jump table we have to make sure the value of the case lies in the bounds of the table. If m is the minimum case value and M is the maximum case value, our table will have M – m + 1 entries. In the example above m = 1 and M = 3 so we have 3 entries in the table. We have to make sure that the value used to index is m ≤ x ≤ M, otherwise we would be accessing incorrect memory locations. Also remember that to properly index the jump table we will have to subtract m to the case value.
</p>
<p>
Jump tables are great, once we have checked that the case value is in the proper range (these are two comparisons) then we do not have to compare anything else. So basically the cost of comparisons in this approach is constant (i.e. it does not grow if the number of cases grow).
</p>
<p>
There are two big downsides to this approach which prevent us from always using it. The first one happens when the difference between M and m is large, our jump table will be large. This enlarges the code size. We have, basically, traded time by space. Now our code size will add 4 bytes per case handled in a jump table. A table of 256 entries will take up to 1 Kbyte (1024 bytes) of memory in our executable program. To be fair, this is the amount of space taken by 256 instructions. So if code size is a concern for you (and usually in the embedded world is), this approach may not be suitable. The second big downside happens when there are “holes” in the cases. Imagine our cases are just 1, 3 and 100. The table will be 100 items long but only 1, 3 and 100 will have useful entries: all the remaining entries will have the address of the default case (or the address after the switch if the default case is omitted). In this case we are not just taking 400 bytes, we are wasting 388 bytes (97% of entries would be useless!). So if the number of cases is low and the values are scattered in a large range, jump tables are not a good choice.
</p>
<h2>Compute the case address</h2>
<p>
This strategy is a bit complicated and has more constraints than a jump table, so it is less general. If all the cases are ordered and they take the same amount of instructions, we can compute the address of the case without using a jump table. This is risky because we have to be careful when computing the address of the branch using the current value (otherwise we will jump to a wrong address and bad things will happen for sure).
</p>
<p>
If not all the cases take the same amount of instructions, we can compensate them to take as many instructions as the case with the biggest number of instructions. We can do that using the <code>nop</code> instruction that does nothing but occupy space. If the variance of the number of instructions among cases is small we will just end adding some <code>nop</code>s to a few cases. If the variance is large, we may end with <em>code bloat</em>, something we wanted to avoid when using this technique.
</p>
<p>
If there are holes, we can just branch them to the default case and fill the remaining instructions with <code>nop</code>s. Again if the number of holes is large this is prone to code bloat as well.
</p>
<p>
In our example of the jump table, each case takes just two instructions. So we can get the address of the first case and use it as a base address to compute the branch.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> calcjump<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
<span style="color: #339933;">.</span>globl main
&nbsp;
main<span style="color: #339933;">:</span>
  <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>                  <span style="color: #339933;">/*</span> r0 <span style="color: #339933;">-</span> <span style="color: #ff0000;">1</span> <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
  blt case_default            <span style="color: #339933;">/*</span> branch to case_default if r0 &lt; <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">3</span>                  <span style="color: #339933;">/*</span> r0 <span style="color: #339933;">-</span> <span style="color: #ff0000;">3</span> <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
  bgt case_default            <span style="color: #339933;">/*</span> branch to case_default if r0 &gt; <span style="color: #ff0000;">3</span> <span style="color: #339933;">*/</span>
&nbsp;
  <span style="color: #00007f; font-weight: bold;">sub</span> r0<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>              <span style="color: #339933;">/*</span> r0 ← r0 <span style="color: #339933;">-</span> <span style="color: #ff0000;">1</span><span style="color: #339933;">.</span> Required to index the table <span style="color: #339933;">*/</span>
  ldr r1<span style="color: #339933;">,</span> addr_of_case_1      <span style="color: #339933;">/*</span> r1 ← &amp;case_1 <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">add</span> r1<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> r0<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">3</span>      <span style="color: #339933;">/*</span> r1 ← r1 <span style="color: #339933;">+</span> r0 <span style="color: #339933;">*</span> <span style="color: #ff0000;">8</span>
                                 Each instruction is <span style="color: #ff0000;">4</span> bytes
                                 Each case takes <span style="color: #ff0000;">2</span> instructions
                                 Thus<span style="color: #339933;">,</span> each case is <span style="color: #ff0000;">8</span> bytes <span style="color: #009900; font-weight: bold;">(</span><span style="color: #ff0000;">4</span> <span style="color: #339933;">*</span> <span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">)</span>
                                 <span style="color: #339933;">*/</span>
&nbsp;
  <span style="color: #00007f; font-weight: bold;">mov</span> pc<span style="color: #339933;">,</span> r1                  <span style="color: #339933;">/*</span> pc ← r1
                                 This will cause a branch to the
                                 computed address <span style="color: #339933;">*/</span>
&nbsp;
  case_1<span style="color: #339933;">:</span>
   <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>                 <span style="color: #339933;">/*</span> r0 ← <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span> 
   b after_switch             <span style="color: #339933;">/*</span> break <span style="color: #339933;">*/</span>
&nbsp;
  case_2<span style="color: #339933;">:</span>
   <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span>                 <span style="color: #339933;">/*</span> r0 ← <span style="color: #ff0000;">2</span> <span style="color: #339933;">*/</span>
   b after_switch             <span style="color: #339933;">/*</span> break <span style="color: #339933;">*/</span>
&nbsp;
  case_3<span style="color: #339933;">:</span>
   <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">3</span>                 <span style="color: #339933;">/*</span> r0 ← <span style="color: #ff0000;">3</span> <span style="color: #339933;">*/</span>
   b after_switch             <span style="color: #339933;">/*</span> break <span style="color: #339933;">*/</span>
&nbsp;
  case_default<span style="color: #339933;">:</span>
   <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">42</span>                <span style="color: #339933;">/*</span> r0 ← <span style="color: #ff0000;">42</span> <span style="color: #339933;">*/</span>
   b after_switch             <span style="color: #339933;">/*</span> break <span style="color: #009900; font-weight: bold;">(</span>unnecessary<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>  
&nbsp;
  after_switch<span style="color: #339933;">:</span>
&nbsp;
  <span style="color: #46aa03; font-weight: bold;">bx</span> lr                       <span style="color: #339933;">/*</span> Return from main <span style="color: #339933;">*/</span>
&nbsp;
<span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">align</span> <span style="color: #ff0000;">4</span>
addr_of_case_1<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> case_1</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* calcjump.s */
.data

.text

.globl main

main:
  cmp r0, #1                  /* r0 - 1 and update cpsr */
  blt case_default            /* branch to case_default if r0 &lt; 1 */
  cmp r0, #3                  /* r0 - 3 and update cpsr */
  bgt case_default            /* branch to case_default if r0 &gt; 3 */

  sub r0, r0, #1              /* r0 ← r0 - 1. Required to index the table */
  ldr r1, addr_of_case_1      /* r1 ← &amp;case_1 */
  add r1, r1, r0, LSL #3      /* r1 ← r1 + r0 * 8
                                 Each instruction is 4 bytes
                                 Each case takes 2 instructions
                                 Thus, each case is 8 bytes (4 * 2)
                                 */

  mov pc, r1                  /* pc ← r1
                                 This will cause a branch to the
                                 computed address */

  case_1:
   mov r0, #1                 /* r0 ← 1 */ 
   b after_switch             /* break */
 
  case_2:
   mov r0, #2                 /* r0 ← 2 */
   b after_switch             /* break */

  case_3:
   mov r0, #3                 /* r0 ← 3 */
   b after_switch             /* break */

  case_default:
   mov r0, #42                /* r0 ← 42 */
   b after_switch             /* break (unnecessary) */  

  after_switch:

  bx lr                       /* Return from main */

.align 4
addr_of_case_1: .word case_1</p></div>

<h2>Binary search</h2>
<p>
Consider again our example with 100 cases. A string of if-else will require in average 50 comparisons. Can we reduce the number of comparisons? Well, the answer is yes. Perform a binary search of the case.
</p>
<p>
A binary search will discard half of the case set each time. This will allow us to dramatically reduce the amount of comparisons. The following example implements the same code in the jump table but with cases 1 to 10.
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
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> binsearch<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
<span style="color: #339933;">.</span>globl main
&nbsp;
main<span style="color: #339933;">:</span>
&nbsp;
  <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>              <span style="color: #339933;">/*</span> r0 <span style="color: #339933;">-</span> <span style="color: #ff0000;">1</span> <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
  blt case_default        <span style="color: #339933;">/*</span> if r0 &lt; <span style="color: #ff0000;">1</span> then branch to case_default <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">10</span>             <span style="color: #339933;">/*</span> r0 <span style="color: #339933;">-</span> <span style="color: #ff0000;">10</span> <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
  bgt case_default        <span style="color: #339933;">/*</span> if r0 &gt; <span style="color: #ff0000;">10</span> then branch to case <span style="color: #0000ff; font-weight: bold;">default</span> <span style="color: #339933;">*/</span>
&nbsp;
  case_1_to_10<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">5</span>            <span style="color: #339933;">/*</span> r0 <span style="color: #339933;">-</span> <span style="color: #ff0000;">5</span> <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
    beq case_5            <span style="color: #339933;">/*</span> if r0 == <span style="color: #ff0000;">5</span> branch to case_5 <span style="color: #339933;">*/</span>
    blt case_1_to_4       <span style="color: #339933;">/*</span> if r0 &lt; <span style="color: #ff0000;">5</span> branch to case_1_to_4 <span style="color: #339933;">*/</span>
    bgt case_6_to_10      <span style="color: #339933;">/*</span> if r0 &gt; <span style="color: #ff0000;">5</span> branch to case_6_to_4 <span style="color: #339933;">*/</span>
&nbsp;
  case_1_to_4<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span>            <span style="color: #339933;">/*</span> r0 <span style="color: #339933;">-</span> <span style="color: #ff0000;">2</span> <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
    beq case_2            <span style="color: #339933;">/*</span> if r0 == <span style="color: #ff0000;">2</span> branch to case_2 <span style="color: #339933;">*/</span>
    blt case_1            <span style="color: #339933;">/*</span> if r0 &lt; <span style="color: #ff0000;">2</span> branch to case_1 
                             <span style="color: #009900; font-weight: bold;">(</span>case_1_to_1 does <span style="color: #00007f; font-weight: bold;">not</span> make sense<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
    bgt case_3_to_4       <span style="color: #339933;">/*</span> if r0 &gt; <span style="color: #ff0000;">2</span> branch to case_3_to_4 <span style="color: #339933;">*/</span>
&nbsp;
  case_3_to_4<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">3</span>            <span style="color: #339933;">/*</span> r0 <span style="color: #339933;">-</span> <span style="color: #ff0000;">3</span> <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
    beq case_3            <span style="color: #339933;">/*</span> if r0 == <span style="color: #ff0000;">3</span> branch to case_3 <span style="color: #339933;">*/</span>
    b case_4              <span style="color: #339933;">/*</span> otherwise it must be r0 == <span style="color: #ff0000;">4</span><span style="color: #339933;">,</span>
                             branch to case_4 <span style="color: #339933;">*/</span>
&nbsp;
  case_6_to_10<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">8</span>            <span style="color: #339933;">/*</span> r0 <span style="color: #339933;">-</span> <span style="color: #ff0000;">8</span> <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
    beq case_8            <span style="color: #339933;">/*</span> if r0 == <span style="color: #ff0000;">8</span> branch to case_8 <span style="color: #339933;">*/</span>
    blt case_6_to_7       <span style="color: #339933;">/*</span> if r0 &lt; <span style="color: #ff0000;">8</span> then branch to case_6_to_7 <span style="color: #339933;">*/</span>
    bgt case_9_to_10      <span style="color: #339933;">/*</span> if r0 &gt; <span style="color: #ff0000;">8</span> then branch to case_9_to_10 <span style="color: #339933;">*/</span>
&nbsp;
  case_6_to_7<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">6</span>            <span style="color: #339933;">/*</span> r0 <span style="color: #339933;">-</span> <span style="color: #ff0000;">6</span> <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
    beq case_6            <span style="color: #339933;">/*</span> if r0 == <span style="color: #ff0000;">6</span> branch to case_6 <span style="color: #339933;">*/</span>
    b case_7              <span style="color: #339933;">/*</span> otherwise it must be r0 == <span style="color: #ff0000;">7</span><span style="color: #339933;">,</span>
                             branch to case <span style="color: #ff0000;">7</span> <span style="color: #339933;">*/</span>
&nbsp;
  case_9_to_10<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">9</span>            <span style="color: #339933;">/*</span> r0 <span style="color: #339933;">-</span> <span style="color: #ff0000;">9</span> <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
    beq case_9            <span style="color: #339933;">/*</span> if r0 == <span style="color: #ff0000;">9</span> branch to case_9 <span style="color: #339933;">*/</span>
    b case_10             <span style="color: #339933;">/*</span> otherwise it must be r0 == <span style="color: #ff0000;">10</span><span style="color: #339933;">,</span>
                             branch to case <span style="color: #ff0000;">10</span> <span style="color: #339933;">*/</span>
&nbsp;
  case_1<span style="color: #339933;">:</span>
     <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>
     b after_switch
  case_2<span style="color: #339933;">:</span>
     <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span>
     b after_switch
  <span style="color: #339933;">.</span>
  <span style="color: #339933;">.</span>  <span style="color: #339933;">/*</span> Cases from <span style="color: #ff0000;">3</span> to <span style="color: #ff0000;">9</span> omitted <span style="color: #339933;">*/</span>
  <span style="color: #339933;">.</span>
  case_10<span style="color: #339933;">:</span>
     <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">10</span>
     b after_switch
&nbsp;
  case_default<span style="color: #339933;">:</span>
   <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">42</span>                <span style="color: #339933;">/*</span> r0 ← <span style="color: #ff0000;">42</span> <span style="color: #339933;">*/</span>
   b after_switch             <span style="color: #339933;">/*</span> break <span style="color: #009900; font-weight: bold;">(</span>unnecessary<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
&nbsp;
  after_switch<span style="color: #339933;">:</span>
&nbsp;
  <span style="color: #46aa03; font-weight: bold;">bx</span> lr                       <span style="color: #339933;">/*</span> Return from main <span style="color: #339933;">*/</span></pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* binsearch.s */
.data

.text

.globl main

main:

  cmp r0, #1              /* r0 - 1 and update cpsr */
  blt case_default        /* if r0 &lt; 1 then branch to case_default */
  cmp r0, #10             /* r0 - 10 and update cpsr */
  bgt case_default        /* if r0 &gt; 10 then branch to case default */

  case_1_to_10:
    cmp r0, #5            /* r0 - 5 and update cpsr */
    beq case_5            /* if r0 == 5 branch to case_5 */
    blt case_1_to_4       /* if r0 &lt; 5 branch to case_1_to_4 */
    bgt case_6_to_10      /* if r0 &gt; 5 branch to case_6_to_4 */

  case_1_to_4:
    cmp r0, #2            /* r0 - 2 and update cpsr */
    beq case_2            /* if r0 == 2 branch to case_2 */
    blt case_1            /* if r0 &lt; 2 branch to case_1 
                             (case_1_to_1 does not make sense) */
    bgt case_3_to_4       /* if r0 &gt; 2 branch to case_3_to_4 */

  case_3_to_4:
    cmp r0, #3            /* r0 - 3 and update cpsr */
    beq case_3            /* if r0 == 3 branch to case_3 */
    b case_4              /* otherwise it must be r0 == 4,
                             branch to case_4 */

  case_6_to_10:
    cmp r0, #8            /* r0 - 8 and update cpsr */
    beq case_8            /* if r0 == 8 branch to case_8 */
    blt case_6_to_7       /* if r0 &lt; 8 then branch to case_6_to_7 */
    bgt case_9_to_10      /* if r0 &gt; 8 then branch to case_9_to_10 */

  case_6_to_7:
    cmp r0, #6            /* r0 - 6 and update cpsr */
    beq case_6            /* if r0 == 6 branch to case_6 */
    b case_7              /* otherwise it must be r0 == 7,
                             branch to case 7 */

  case_9_to_10:
    cmp r0, #9            /* r0 - 9 and update cpsr */
    beq case_9            /* if r0 == 9 branch to case_9 */
    b case_10             /* otherwise it must be r0 == 10,
                             branch to case 10 */

  case_1:
     mov r0, #1
     b after_switch
  case_2:
     mov r0, #2
     b after_switch
  .
  .  /* Cases from 3 to 9 omitted */
  .
  case_10:
     mov r0, #10
     b after_switch

  case_default:
   mov r0, #42                /* r0 ← 42 */
   b after_switch             /* break (unnecessary) */

  after_switch:

  bx lr                       /* Return from main */</p></div>

<p>
This strategy is able to determine the case value in just only 3 comparisons (if we ignore the mandated two comparisons for range checking). What we do is we check the compare the case value with the middle one in the current range. This way we can discard half of the sets at every comparison step.
</p>
<p>
This strategy works well also for scattered case sets like [1, 2, 3, 24, 25, 26, 97, 98, 99, 300]. In this case the comparisons would be
</p>

<div class="wp_syntax" style="position:relative;"><table><tbody><tr><td class="code"><pre class="asm" style="font-family:monospace;">case_1_to_300<span style="color: #339933;">:</span>
  <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">25</span>
  beq case_25
  blt case_1_to_24
  bgt case_26_to_300
case_1_to_24<span style="color: #339933;">:</span>
  <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span>
  beq case_2
  blt case_1
  bgt case_3_to_24
case_3_to_24<span style="color: #339933;">:</span>
  <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">3</span>
  beq case_3
  b case_24
case_26_to_300<span style="color: #339933;">:</span>
  <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">98</span>
  beq case_98
  blt case_26_to_97
  bgt case_99_to_300
case_26_to_97<span style="color: #339933;">:</span>
  <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">26</span>
  beq case_26
  b case_97
case_99_to_300<span style="color: #339933;">:</span>
  <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">99</span>
  beq case_99
  b case_300</pre></td></tr></tbody></table><p class="theCode" style="display:none;">case_1_to_300:
  cmp r0, #25
  beq case_25
  blt case_1_to_24
  bgt case_26_to_300
case_1_to_24:
  cmp r0, #2
  beq case_2
  blt case_1
  bgt case_3_to_24
case_3_to_24:
  cmp r0, #3
  beq case_3
  b case_24
case_26_to_300:
  cmp r0, #98
  beq case_98
  blt case_26_to_97
  bgt case_99_to_300
case_26_to_97:
  cmp r0, #26
  beq case_26
  b case_97
case_99_to_300:
  cmp r0, #99
  beq case_99
  b case_300</p></div>

<p>
which is 3 comparisons at most also.
</p>
<p>
Using this strategy the number of comparisons is log(N), where N is the number of elements in the case set. So for 10 elements, in the worst of the cases, we will have to do 3 comparisons, for 20 at most 4, for 40 at most 5, etc.
</p>
<p>
Let’s retake the code bloat issue that arised with jump tables. If you check, every comparison requires 3 or 4 instructions, this is about 12 to 16 bytes per comparison. If we have a case set of 256 elements, the generated code will require 128 comparisons blocks in total. While the number of comparisons performed at runtime, 8 in the worst of the cases, we still need 128 <code>case_x_to_y</code> comparison blocks to perform the binary search. If we pessimistically assume that all comparison blocks take 4 instructions this will be 4*128*4 = 2048 bytes in instructions. Compare that to a jump table of 256 positions, each position takes 4 bytes: 256 * 4 = 1024 bytes. So, binary search is not so competitive in terms of code size.
</p>
<p>
Binary search, thus, is useful for large scattered sets. Recall that if-else strings are not efficient for large sets of cases and jump tables waste space if the case range lacks lots of cases.
</p>
<h2>Hybrid approach</h2>
<p>
Is it possible to combine the two strategies? The answer is yes. We will use two tables: a case values table (sorted, usually in ascending order) and addresses for each case in another table, in the same order as the case values table.
</p>
<p>
We will make a binary search inside the case value set. When the value is found we will use the index of the match to calculate a jump. For the example below we will use the case set<br>
[1, 2, 3, 24, 25, 26, 97, 98, 99, 300].
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
102
103
104
</pre></td><td class="code"><pre class="asm" style="font-family:monospace;"><span style="color: #339933;">/*</span> hybrid<span style="color: #339933;">.</span>s <span style="color: #339933;">*/</span>
<span style="color: #0000ff; font-weight: bold;">.data</span>
&nbsp;
<span style="color: #0000ff; font-weight: bold;">.text</span>
&nbsp;
<span style="color: #339933;">.</span>globl main
&nbsp;
main<span style="color: #339933;">:</span>
  <span style="color: #00007f; font-weight: bold;">push</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span> r5<span style="color: #339933;">,</span> r6<span style="color: #339933;">,</span> lr<span style="color: #009900; font-weight: bold;">}</span>
&nbsp;
  <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>                <span style="color: #339933;">/*</span> r0 <span style="color: #339933;">-</span> <span style="color: #ff0000;">1</span> <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
  blt case_default          <span style="color: #339933;">/*</span> if r0 &lt; <span style="color: #ff0000;">1</span> then branch to case_default <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">300</span>              <span style="color: #339933;">/*</span> r0 <span style="color: #339933;">-</span> <span style="color: #ff0000;">300</span> <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
  bgt case_default          <span style="color: #339933;">/*</span> if r0 &gt; <span style="color: #ff0000;">300</span> then branch to case <span style="color: #0000ff; font-weight: bold;">default</span> <span style="color: #339933;">*/</span>
&nbsp;
  <span style="color: #339933;">/*</span> prepare the binary search<span style="color: #339933;">.</span> 
     r1 will hold the lower index
     r2 will hold the upper index
     r3 the base address of the case_value_table
  <span style="color: #339933;">*/</span>
  <span style="color: #00007f; font-weight: bold;">mov</span> r1<span style="color: #339933;">,</span> #<span style="color: #ff0000;">0</span>
  <span style="color: #00007f; font-weight: bold;">mov</span> r2<span style="color: #339933;">,</span> #<span style="color: #ff0000;">9</span>
  ldr r3<span style="color: #339933;">,</span> addr_case_value_table <span style="color: #339933;">/*</span> r3 ← &amp;case_value_table <span style="color: #339933;">*/</span>
&nbsp;
  b check_binary_search
  binary_search<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">add</span> r4<span style="color: #339933;">,</span> r1<span style="color: #339933;">,</span> r2          <span style="color: #339933;">/*</span> r4 ← r1 <span style="color: #339933;">+</span> r2 <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> r4<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> ASR #<span style="color: #ff0000;">1</span>      <span style="color: #339933;">/*</span> r4 ← r4 <span style="color: #339933;">/</span> <span style="color: #ff0000;">2</span> <span style="color: #339933;">*/</span>
    ldr r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r3<span style="color: #339933;">,</span> <span style="color: #339933;">+</span>r4<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">]</span>   <span style="color: #339933;">/*</span> r5 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r3 <span style="color: #339933;">+</span> r4 <span style="color: #339933;">*</span> <span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span><span style="color: #339933;">.</span> 
                               This is r5 ← case_value_table<span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">cmp</span> r0<span style="color: #339933;">,</span> r5              <span style="color: #339933;">/*</span> r0 <span style="color: #339933;">-</span> r5 <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
    sublt r2<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>        <span style="color: #339933;">/*</span> if r0 &lt; r5 then r2 ← r4 <span style="color: #339933;">-</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    addgt r1<span style="color: #339933;">,</span> r4<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>        <span style="color: #339933;">/*</span> if r0 &gt; r5 then r1 ← r4 <span style="color: #339933;">+</span> <span style="color: #ff0000;">1</span> <span style="color: #339933;">*/</span>
    bne check_binary_search <span style="color: #339933;">/*</span> if r0 != r5 branch to binary_search <span style="color: #339933;">*/</span>
&nbsp;
    <span style="color: #339933;">/*</span> if we reach here it means that r0 == r5 <span style="color: #339933;">*/</span>
    ldr r5<span style="color: #339933;">,</span> addr_case_addresses_table <span style="color: #339933;">/*</span> r5 ← &amp;addr_case_value_table <span style="color: #339933;">*/</span>
    ldr r5<span style="color: #339933;">,</span> <span style="color: #009900; font-weight: bold;">[</span>r5<span style="color: #339933;">,</span> <span style="color: #339933;">+</span>r4<span style="color: #339933;">,</span> <span style="color: #00007f; font-weight: bold;">LSL</span> #<span style="color: #ff0000;">2</span><span style="color: #009900; font-weight: bold;">]</span>   <span style="color: #339933;">/*</span> r5 ← <span style="color: #339933;">*</span><span style="color: #009900; font-weight: bold;">(</span>r5 <span style="color: #339933;">+</span> r4<span style="color: #339933;">*</span><span style="color: #ff0000;">4</span><span style="color: #009900; font-weight: bold;">)</span> 
                               This is r5 ← case_addresses_table<span style="color: #009900; font-weight: bold;">[</span>r4<span style="color: #009900; font-weight: bold;">]</span> <span style="color: #339933;">*/</span>
    <span style="color: #00007f; font-weight: bold;">mov</span> pc<span style="color: #339933;">,</span> r5              <span style="color: #339933;">/*</span> branch to the proper case <span style="color: #339933;">*/</span>
&nbsp;
  check_binary_search<span style="color: #339933;">:</span>
    <span style="color: #00007f; font-weight: bold;">cmp</span> r1<span style="color: #339933;">,</span> r2              <span style="color: #339933;">/*</span> r1 <span style="color: #339933;">-</span> r2 <span style="color: #00007f; font-weight: bold;">and</span> update cpsr <span style="color: #339933;">*/</span>
    ble binary_search       <span style="color: #339933;">/*</span> if r1 &lt;= r2 branch to binary_search <span style="color: #339933;">*/</span>
&nbsp;
  <span style="color: #339933;">/*</span> if we reach here it means the case value
     was <span style="color: #00007f; font-weight: bold;">not</span> found<span style="color: #339933;">.</span> branch to <span style="color: #0000ff; font-weight: bold;">default</span> case <span style="color: #339933;">*/</span>
  b case_default
&nbsp;
    case_1<span style="color: #339933;">:</span>
     <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">1</span>
     b after_switch
  case_2<span style="color: #339933;">:</span>
     <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">2</span>
     b after_switch
  case_3<span style="color: #339933;">:</span>
     <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">3</span>
     b after_switch
  case_24<span style="color: #339933;">:</span>
     <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">24</span>
     b after_switch
  case_25<span style="color: #339933;">:</span>
     <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">95</span>
     b after_switch
  case_26<span style="color: #339933;">:</span>
     <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">96</span>
     b after_switch
  case_97<span style="color: #339933;">:</span>
     <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">97</span>
     b after_switch
  case_98<span style="color: #339933;">:</span>
     <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">98</span>
     b after_switch
  case_99<span style="color: #339933;">:</span>
     <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">99</span>
     b after_switch
  case_300<span style="color: #339933;">:</span>
     <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">300</span>    <span style="color: #339933;">/*</span> The error <span style="color: #0000ff; font-weight: bold;">code</span> will be <span style="color: #ff0000;">44</span> <span style="color: #339933;">*/</span>
     b after_switch
&nbsp;
  case_default<span style="color: #339933;">:</span>
   <span style="color: #00007f; font-weight: bold;">mov</span> r0<span style="color: #339933;">,</span> #<span style="color: #ff0000;">42</span>       <span style="color: #339933;">/*</span> r0 ← <span style="color: #ff0000;">42</span> <span style="color: #339933;">*/</span>
   b after_switch    <span style="color: #339933;">/*</span> break <span style="color: #009900; font-weight: bold;">(</span>unnecessary<span style="color: #009900; font-weight: bold;">)</span> <span style="color: #339933;">*/</span>
&nbsp;
  after_switch<span style="color: #339933;">:</span>
&nbsp;
  <span style="color: #00007f; font-weight: bold;">pop</span> <span style="color: #009900; font-weight: bold;">{</span>r4<span style="color: #339933;">,</span>r5<span style="color: #339933;">,</span>r6<span style="color: #339933;">,</span>lr<span style="color: #009900; font-weight: bold;">}</span>
  <span style="color: #46aa03; font-weight: bold;">bx</span> lr              <span style="color: #339933;">/*</span> Return from main <span style="color: #339933;">*/</span>
&nbsp;
case_value_table<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> <span style="color: #ff0000;">1</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">2</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">3</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">24</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">25</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">26</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">97</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">98</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">99</span><span style="color: #339933;">,</span> <span style="color: #ff0000;">300</span>
addr_case_value_table<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> case_value_table
&nbsp;
case_addresses_table<span style="color: #339933;">:</span>
    <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> case_1
    <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> case_2
    <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> case_3
    <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> case_24
    <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> case_25
    <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> case_26
    <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> case_97
    <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> case_98
    <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> case_99
    <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> case_300
addr_case_addresses_table<span style="color: #339933;">:</span> <span style="color: #339933;">.</span><span style="color: #0000ff; font-weight: bold;">word</span> case_addresses_table</pre></td></tr></tbody></table><p class="theCode" style="display:none;">/* hybrid.s */
.data

.text

.globl main

main:
  push {r4, r5, r6, lr}

  cmp r0, #1                /* r0 - 1 and update cpsr */
  blt case_default          /* if r0 &lt; 1 then branch to case_default */
  cmp r0, #300              /* r0 - 300 and update cpsr */
  bgt case_default          /* if r0 &gt; 300 then branch to case default */

  /* prepare the binary search. 
     r1 will hold the lower index
     r2 will hold the upper index
     r3 the base address of the case_value_table
  */
  mov r1, #0
  mov r2, #9
  ldr r3, addr_case_value_table /* r3 ← &amp;case_value_table */

  b check_binary_search
  binary_search:
    add r4, r1, r2          /* r4 ← r1 + r2 */
    mov r4, r4, ASR #1      /* r4 ← r4 / 2 */
    ldr r5, [r3, +r4, LSL #2]   /* r5 ← *(r3 + r4 * 4). 
                               This is r5 ← case_value_table[r4] */
    cmp r0, r5              /* r0 - r5 and update cpsr */
    sublt r2, r4, #1        /* if r0 &lt; r5 then r2 ← r4 - 1 */
    addgt r1, r4, #1        /* if r0 &gt; r5 then r1 ← r4 + 1 */
    bne check_binary_search /* if r0 != r5 branch to binary_search */

    /* if we reach here it means that r0 == r5 */
    ldr r5, addr_case_addresses_table /* r5 ← &amp;addr_case_value_table */
    ldr r5, [r5, +r4, LSL #2]   /* r5 ← *(r5 + r4*4) 
                               This is r5 ← case_addresses_table[r4] */
    mov pc, r5              /* branch to the proper case */

  check_binary_search:
    cmp r1, r2              /* r1 - r2 and update cpsr */
    ble binary_search       /* if r1 &lt;= r2 branch to binary_search */

  /* if we reach here it means the case value
     was not found. branch to default case */
  b case_default

    case_1:
     mov r0, #1
     b after_switch
  case_2:
     mov r0, #2
     b after_switch
  case_3:
     mov r0, #3
     b after_switch
  case_24:
     mov r0, #24
     b after_switch
  case_25:
     mov r0, #95
     b after_switch
  case_26:
     mov r0, #96
     b after_switch
  case_97:
     mov r0, #97
     b after_switch
  case_98:
     mov r0, #98
     b after_switch
  case_99:
     mov r0, #99
     b after_switch
  case_300:
     mov r0, #300    /* The error code will be 44 */
     b after_switch

  case_default:
   mov r0, #42       /* r0 ← 42 */
   b after_switch    /* break (unnecessary) */

  after_switch:

  pop {r4,r5,r6,lr}
  bx lr              /* Return from main */

case_value_table: .word 1, 2, 3, 24, 25, 26, 97, 98, 99, 300
addr_case_value_table: .word case_value_table

case_addresses_table:
    .word case_1
    .word case_2
    .word case_3
    .word case_24
    .word case_25
    .word case_26
    .word case_97
    .word case_98
    .word case_99
    .word case_300
addr_case_addresses_table: .word case_addresses_table</p></div>

<p>
In lines 21 to 44 we implement the binary search. This implementation is an <a href="http://en.wikipedia.org/wiki/Binary_search_algorithm#Iterative">iterative binary search</a> where <code>r1</code> and <code>r2</code> keep the lower and upper indexes of the table that is currently searched. We will leave the search if the lower index becomes larger than the upper, lines 42 to 44. When searching the range given by <code>r1</code> and <code>r2</code>, we will compute <code>r4</code> as the middle index <code>(r1+r2)/2</code>, lines 27 to 28. We will compare it to the current case value being searched, in <code>r0</code>, line 31. If the value, <code>r5</code>, in the case value table (which must be in ascending order) is lower than the current case value being searched, then we shrink the range from <code>r1</code> to <code>r4-1</code>, so we update <code>r2</code> only if <code>r0 &lt; r5</code>, line 32. Conversely if <code>r0 &gt; r5</code> then we shrink the range from <code>r4+1</code> to <code>r2</code>, line 33. If the value of <code>r5</code> matches, then we use the index <code>r4</code> to load the case address and branch to it, lines 37 to 40. Note that if <code>r0</code> is different to <code>r5</code>, we have to omit this step so we branch to the check of the loop, line 34.
</p>
<p>
You can check that this works.
</p>
<pre>$ ./hybrid ; echo $?
1
$ ./hybrid 2 ; echo $?
2
$ ./hybrid 2 3 ; echo $?
3
$ ./hybrid 2 3 4 ; echo $?
42
$ ./hybrid 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 ; echo $?
24
</pre>
<p>
This approach has several interesting properties. It reduces the number of comparisons (we will make the same number of comparisons as in the binary search) and avoids the code bloat due to big jump tables (avoiding useless entries) and comparison blocks (by using a loop). As a drawback, this approach, requires two tables.
</p>
<p>
That's all for today.</p>
<!-- Simple Share Buttons Adder (6.3.4) simplesharebuttons.com --><div class="ssba ssba-wrap"><div style="text-align:left"><a data-site="" class="ssba_facebook_share" href="http://www.facebook.com/sharer.php?u=http://thinkingeek.com/2013/08/23/arm-assembler-raspberry-pi-chapter-16/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/facebook.png" title="Facebook" class="ssba ssba-img" alt="Share on Facebook" scale="0"></a><a data-site="" class="ssba_google_share" href="https://plus.google.com/share?url=http://thinkingeek.com/2013/08/23/arm-assembler-raspberry-pi-chapter-16/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/google.png" title="Google+" class="ssba ssba-img" alt="Share on Google+" scale="0"></a><a data-site="" class="ssba_twitter_share" href="http://twitter.com/share?url=http://thinkingeek.com/2013/08/23/arm-assembler-raspberry-pi-chapter-16/&amp;text=ARM+assembler+in+Raspberry+Pi+%E2%80%93+Chapter+16+" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/twitter.png" title="Twitter" class="ssba ssba-img" alt="Tweet about this on Twitter" scale="0"></a><a data-site="linkedin" class="ssba_linkedin_share ssba_share_link" href="http://www.linkedin.com/shareArticle?mini=true&amp;url=http://thinkingeek.com/2013/08/23/arm-assembler-raspberry-pi-chapter-16/" target="_blank"><img src="http://thinkingeek.com/wp-content/plugins/simple-share-buttons-adder/buttons/somacro/linkedin.png" title="LinkedIn" class="ssba ssba-img" alt="Share on LinkedIn" scale="0"></a></div></div><div class="ccg-banner"><a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" scale="0"></a><br><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">This work</span> by <a target="_blank" xmlns:cc="http://creativecommons.org/ns#" href="http://thinkingeek.com/author/rferrer/" property="cc:attributionName" rel="cc:attributionURL nofollow">Roger Ferrer Ibáñez</a> is licensed under a <a target="_blank" rel="license nofollow" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International</a><br></div>						<p></p>
		<p class="pagination">
			<span class="prev"><a href="http://thinkingeek.com/2013/08/11/arm-assembler-raspberry-pi-chapter-15/" rel="prev">ARM assembler in Raspberry Pi – Chapter 15</a></span>
			<span class="next"><a href="http://thinkingeek.com/2013/11/15/create-temporary-zip-file-send-response-rails/" rel="next">Create a temporary zip file to send as response in Rails</a></span>
		</p>
		

<!-- You can start editing here. -->
<div class="comments">
	
			<h3>4 thoughts on “<span>ARM assembler in Raspberry Pi – Chapter 16</span>”</h3>
		<ul class="commentlist">
				<li class="comment even thread-even depth-1 parent" id="comment-1763">
				<div id="div-comment-1763" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/439111e97c6c8689c4bbdb224095b667?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/439111e97c6c8689c4bbdb224095b667?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-439111e97c6c8689c4bbdb224095b667-0" originals="32" src-orig="http://1.gravatar.com/avatar/439111e97c6c8689c4bbdb224095b667?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">shahrukh</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/08/23/arm-assembler-raspberry-pi-chapter-16/#comment-1763">
			October 18, 2013 at 8:40 pm</a>		</div>

		<p>You are totally awesome. I have somehow finished your tutorial 1-16. I mean to say it more of programming best practices applied in context of embedded programming, rather than just mere documenting assembly operations.<br>
Thanks for much help, and a great introduction to arm assembly using raspberry pi.</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/08/23/arm-assembler-raspberry-pi-chapter-16/?replytocom=1763#respond" onclick="return addComment.moveForm( &quot;div-comment-1763&quot;, &quot;1763&quot;, &quot;respond&quot;, &quot;1279&quot; )" aria-label="Reply to shahrukh">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-2 parent" id="comment-1766">
				<div id="div-comment-1766" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-0">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/08/23/arm-assembler-raspberry-pi-chapter-16/#comment-1766">
			October 20, 2013 at 5:34 pm</a>		</div>

		<p>Thank you very much! </p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/08/23/arm-assembler-raspberry-pi-chapter-16/?replytocom=1766#respond" onclick="return addComment.moveForm( &quot;div-comment-1766&quot;, &quot;1766&quot;, &quot;respond&quot;, &quot;1279&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment even depth-3 parent" id="comment-110794">
				<div id="div-comment-110794" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://0.gravatar.com/avatar/cd8f1ac6d11141da3f51bb1f30bc5f9e?s=54&amp;d=mm&amp;r=g" srcset="http://0.gravatar.com/avatar/cd8f1ac6d11141da3f51bb1f30bc5f9e?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" id="grav-cd8f1ac6d11141da3f51bb1f30bc5f9e-0" originals="32" src-orig="http://0.gravatar.com/avatar/cd8f1ac6d11141da3f51bb1f30bc5f9e?s=32&amp;d=mm&amp;r=g" scale="1.5">			<cite class="fn">Benny Boo</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/08/23/arm-assembler-raspberry-pi-chapter-16/#comment-110794">
			April 3, 2014 at 4:31 pm</a>		</div>

		<p>Do you by  any chance have a book because I would buy it in a second</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/08/23/arm-assembler-raspberry-pi-chapter-16/?replytocom=110794#respond" onclick="return addComment.moveForm( &quot;div-comment-110794&quot;, &quot;110794&quot;, &quot;respond&quot;, &quot;1279&quot; )" aria-label="Reply to Benny Boo">Reply</a></div>
				</div>
		<ul class="children">
		<li class="comment byuser comment-author-rferrer bypostauthor odd alt depth-4" id="comment-110796">
				<div id="div-comment-110796" class="comment-body">
				<div class="comment-author vcard">
			<img alt="" src="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=54&amp;d=mm&amp;r=g" srcset="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=64&amp;d=mm&amp;r=g 2x" class="avatar avatar-32 photo grav-hashed grav-hijack" height="32" width="32" originals="32" src-orig="http://1.gravatar.com/avatar/a779b8290b1ca104fdf84d8016fd010b?s=32&amp;d=mm&amp;r=g" scale="1.5" id="grav-a779b8290b1ca104fdf84d8016fd010b-1">			<cite class="fn">rferrer</cite> <span class="says">says:</span>		</div>
		
		<div class="comment-meta commentmetadata"><a href="http://thinkingeek.com/2013/08/23/arm-assembler-raspberry-pi-chapter-16/#comment-110796">
			April 5, 2014 at 9:40 pm</a>		</div>

		<p>Hi Benny Boo,</p>
<p>no, but thanks anyway </p>
<p>Kind regards,</p>

		<div class="reply"><a rel="nofollow" class="comment-reply-link" href="http://thinkingeek.com/2013/08/23/arm-assembler-raspberry-pi-chapter-16/?replytocom=110796#respond" onclick="return addComment.moveForm( &quot;div-comment-110796&quot;, &quot;110796&quot;, &quot;respond&quot;, &quot;1279&quot; )" aria-label="Reply to rferrer">Reply</a></div>
				</div>
		</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
</ul><!-- .children -->
</li><!-- #comment-## -->
		</ul>
	
	<p></p>
		<div id="respond" class="comment-respond">
		<h3 id="reply-title" class="comment-reply-title">Leave a Reply <small><a rel="nofollow" id="cancel-comment-reply-link" href="/2013/08/23/arm-assembler-raspberry-pi-chapter-16/#respond" style="display:none;">Cancel reply</a></small></h3>			<form action="http://thinkingeek.com/wp-comments-post.php" method="post" id="commentform" class="comment-form">
				<p class="comment-notes"><span id="email-notes">Your email address will not be published.</span> Required fields are marked <span class="required">*</span></p><p class="comment-form-comment"><label for="comment">Comment</label> <textarea id="comment" name="comment" cols="45" rows="8" maxlength="65525" aria-required="true" required="required"></textarea></p><p class="comment-form-author"><label for="author">Name <span class="required">*</span></label> <input id="author" name="author" type="text" value="" size="30" maxlength="245" aria-required="true" required="required"></p>
<p class="comment-form-email"><label for="email">Email <span class="required">*</span></label> <input id="email" name="email" type="text" value="" size="30" maxlength="100" aria-describedby="email-notes" aria-required="true" required="required"></p>
<p class="comment-form-url"><label for="url">Website</label> <input id="url" name="url" type="text" value="" size="30" maxlength="200"></p>
<p class="form-submit"><input name="submit" type="submit" id="submit" class="submit" value="Post Comment"> <input type="hidden" name="comment_post_ID" value="1279" id="comment_post_ID">
<input type="hidden" name="comment_parent" id="comment_parent" value="0">
</p><p style="display: none;"><input type="hidden" id="akismet_comment_nonce" name="akismet_comment_nonce" value="ea05b428de"></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_comments" id="subscribe_comments" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-label" for="subscribe_comments">Notify me of follow-up comments by email.</label></p><p class="comment-subscription-form"><input type="checkbox" name="subscribe_blog" id="subscribe_blog" value="subscribe" style="width: auto; -moz-appearance: checkbox; -webkit-appearance: checkbox;"> <label class="subscribe-label" id="subscribe-blog-label" for="subscribe_blog">Notify me of new posts by email.</label></p><p style="display: none;"></p>			<input type="hidden" id="ak_js" name="ak_js" value="1496666327297"></form>
			</div><!-- #respond -->
	</div>	</div>
