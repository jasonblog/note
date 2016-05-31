# Q Format Number in C


```c
#include <stdio.h>

double QFormatIntegerToFloat(const int x,
                             const int n)

{
    return (((double)x) / ((double)(1 << n)));
}

int FloatToQFormatInteger(const double x,
                          const int m,
                          const int n)

{
    int   i;
    int   y;
    int   mask = 0;

    y = (int)(x * ((double)(1 << n)));  // Generate the integer number in m.n format

    for (i = 0; i < (m + n);
         i++) {     // Calculate the mask to ensure we return the correct number of valid bits
        mask = (mask << 1) + 1;
    }

    return (y & mask);
}

int MpyQFormatInteger(const int x,
                      const int y,
                      const int n)

{
    long long tmp;
    tmp = ((long long)x * (long long)y) >> n;
    return ((int)tmp);
}


int main()
{
    int         a, b, c, d, e;


    printf("%f = 0x%x\n",  2.0 , FloatToQFormatInteger(2.0 , 8, 24));
    printf("%f = 0x%x\n",  1.5 , FloatToQFormatInteger(1.5 , 8, 24));
    printf("%f = 0x%x\n",  0.5 , FloatToQFormatInteger(0.5 , 8, 24));
    printf("%f = 0x%x\n",  0.25, FloatToQFormatInteger(0.25, 8, 24));
    printf("%f = 0x%x\n",  0.75, FloatToQFormatInteger(0.75, 8, 24));
    printf("\n");

#if 1
    a = FloatToQFormatInteger(2.0, 8, 24);
    printf("Result = 0x%x : %f\n", a, QFormatIntegerToFloat(a , 24));
    b = FloatToQFormatInteger(1.5, 8, 24);
    printf("Result = 0x%x : %f\n", b, QFormatIntegerToFloat(b , 24));
    c = FloatToQFormatInteger(0.5, 8, 24);
    printf("Result = 0x%x : %f\n", c, QFormatIntegerToFloat(c , 24));
    d = FloatToQFormatInteger(0.25, 8, 24);
    printf("Result = 0x%x : %f\n", d, QFormatIntegerToFloat(d , 24));
    e = FloatToQFormatInteger(0.75, 8, 24);
    printf("Result = 0x%x : %f\n", e, QFormatIntegerToFloat(e , 24));

#else
    a = FloatToQFormatInteger(0.75, 8, 24);
    b = FloatToQFormatInteger(2.0 , 8, 24);
    c = MpyQFormatInteger(a, b, 24);

    printf("Result = 0x%x : %f\n", c, QFormatIntegerToFloat(c , 24));

    d = FloatToQFormatInteger(0.5 , 8, 24);
    e = MpyQFormatInteger(c, d, 24);

    printf("Result = 0x%x : %f\n", e, QFormatIntegerToFloat(e , 24));
#endif

    return (0);
}
```

## Ref
http://blog.numerix-dsp.com/2015/01/c-code-to-implement-q-format-number.html