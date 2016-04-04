# C-compiler-optimizations

##Branch Optimization

###If optimization

```c
void f (int *p)
{
  if (p) g(1);
  if (p) g(2);
  return;
}
```

Can be simply replaced by

```c
void f (int *p) 
{
    if (p) {
        g(1);
        g(2);

    }
    return;
}
```


###Value Range Optimization
```c
for(int i = 1; i < 100; i++) {
    if (i)
        g();
}
```

The ```if``` expression can be eliminated since it is already known that ```i``` is a positive integer.

```c

for(int i = 1; i < 100; i++) {
    g();
}
```

###Branch elimination
```c
goto L1;
  //do something

L1:
  goto L2 //L1 branch is unnecessary 
```



###Unswitching

As opposed to checking if some condition or the other is true inside of a loop, you can take the ```if``` out of the loop and then loop.

```c
for (int i = 0; i < N; i++) 
    if (x)
        a[i] = 0;
    else
        b[i] = 0;
```

```c
if (x)
    for (int i = 0; i < N; i++)
        a[i] = 0
else
    for (int i = 0; i < N; i++)
        b[i] = 0;
```


###Tail Recursion

A tail recursive call can be replaced by a ```goto``` statement which avoids keeping track of the stack frame.

Let's take a simple recursive function

```c
int f(int i) {
    if (i > 0) {
        g(i);
        return f(i - 1)
    }
    else
        return 0;
}
```

Now let's optimize it

```c
int f (int i) {
    entry:

    if (i > 0) {
        g(i);
        i--;
        goto entry;
    }
    else
        return 0;
}

```

###Try/Catch block optimization

Try/Catch blocks that never throw an exception can be optimized

```c
try {
    int i = 1;
} catch (Exception e) {
    //some code
}
```

Can be turned into ```int i = 1;```

##Loop Optimizations

###Loop unrolling

When the different iterations of a loop are independent

```c
for (int i = 0; i < 100; i++) {
    g();
}
```

The loop can be optimized

```c
for (int i = 0; i < 100; i += 2) {
    g();
    g();
}
```

This can of course be done even more aggressively

###Loop Collapsing

```c
int a[100][300];

for (i = 0; i < 300; i++)
  for (j = 0; j < 100; j++)
    a[j][i] = 0;
```

Nested loops can be collapsed into a single loop where the index iterates over range(0,\product_j index_j)
```c
int a[100][300];
int *p = &a[0][0];

for (i = 0; i < 30000; i++)
  *p++ = 0;

```

###Loop fusion
Two separate loops can be fused together

```c
for (i = 0; i < 300; i++)
  a[i] = a[i] + 3;

for (i = 0; i < 300; i++)
  b[i] = b[i] + 4;
```

```c
for (i = 0; i < 300; i++) {
  a[i] = a[i] + 3;
  b[i] = b[i] + 4;
}
```

###Forward store
Stores to global variables in loops can be moved out of the loop
```c
int sum;

void f (void)
{
  int i;

  sum = 0;
  for (i = 0; i < 100; i++)
    sum += a[i];
}
```

```c
int sum;

void f (void)
{
  int i;
  register int t;

  t = 0;
  for (i = 0; i < 100; i++)
    t += a[i];
  sum = t;
}
```

##Access pattern optimization

###Volative Optimization
```volatile``` keyword is used to declare objects that may have unintended side effects.

```c
volatile int x,y;
int a[SIZE];

void f (void) {
    int i;
    for (i = 0;  i < SIZE; i++)
        a[i] = x + y;
}
```

You would think that you could hoist the computation of ```x+y``` outside of the loop 

```c
volatile int x,y;
int a[SIZE];

void f (void) {
    int i;
    int temp = x + y;
    for (i = 0;  i < SIZE; i++)
        a[i] = temp;
}

```

However if ```x``` and ```y``` are volatile then this optimization might in fact be incorrect which is why compilers will not perform it.


###Quick Optimization

Accessed objects can be cached into a temporary variable
```java
{
    for(i = 0; i < 10; i++)
          arr[i] = obj.i + volatile_var;
}
```

Below is the code fragment after Quick Optimization.

```java
{
         t = obj.i;
         for(i = 0; i < 10; i++)
           arr[i] = t + volatile_var;
}
```
  
###printf Optimization
Calling ```printf()``` invokes the external library function ```printf()```

```
#include <stdio.h>

void f (char *s)
{
  printf ("%s", s);
}
```

The string can be formatted at compile time using
```c
#include <stdio.h>

void f (char *s)
{
  fputs (s, stdout);
}
```

###Dead code elimination

Unused code is removed
```c
int i = 1;
return //something else
```

###Constant Propagation/Constant folding
```c
int x = 3;
int y = 4 + x; //replaced by y = 7

return (x + y) //replaced by 10
```

###Instruction combining
Below is a simple case of this, loop unrolling can reveal instances where instruction combining is possible
```c
i++;
i++;

i += 2
```


###Narrowing

```c
unsigned short int s;

(s >> 20)      /* all bits of precision have been shifted out, thus 0 */
(s > 0x10000)  /* 16 bit value can't be greater than 17 bit, thus 0 */
(s == -1)      /* can't be negative, thus 0 */
```

###Integer Multiply

This a well known one, given an expression 

```c
int f (int i,int n)
{
  return i * n;
}  
```

Multiplication can be replaced by leftwise bitwise shifting

```c
int f (int i)
{
  return i << (n-1);
}
```

###Integer mod optimization
Another known one, integer divide is really expensive on hardware.
```c
int f (int x,int y)
{
  return x % y;
}
```

Hacker's delight is a wonderful book that's encyclopedic in its treatment of cool bit tricks such as the one below.

```c

int f (int x)
{
  int temp = x & (y-1);
  return (x < 0) ? ((temp == 0) ? 0 : (temp | ~(y-1))) : temp;
}
```

###Block Merging

Suppose you had the following code fragment

```
int a;
int b;

void f (int x, int y)
{
  goto L1;                   /* basic block 1 */

L2:                          /* basic block 2 */
  b = x + y;
  goto L3;

L1:                          /* basic block 3 */
  a = x + y;
  goto L2;

L3:                          /* basic block 4 */
  return;
}
```


The different blocks will be optimized into one

```c
int a;
int b;

void f (int x, int y)
{
  a = x + y;                 /* basic block 1 */
  b = x + y;
  return;
}

```

###Common SubExpression
The second code fragment above can further be optimzed into 

```c
tmp = x + y
a   = tmp
b   = tmp
return;
```

###Function inlining

A lot of optimizations can be discovered if a function call is replaced by the body of the function

Suppose we wish to implement a substraction function given a working addition function

```c

int add (int x, int y)
{
  return x + y;
}

int sub (int x, int y)
{
  return add (x, -y);
}
```
  
Expanding add() at the call site in sub() yields:

```c
int sub (int x, int y)
{
  return x + -y;
}
```
  
which can be further optimized to:

```c
int sub (int x, int y)
{
  return x - y;
}
```





  