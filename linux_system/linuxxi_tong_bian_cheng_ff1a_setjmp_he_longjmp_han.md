# linux系统编程：setjmp和longjmp函数用法


```c
#include <stdio.h>
#include <setjmp.h>

//jmp_buf：数组，保存栈信息即运行环境
jmp_buf buf;

double Divide(double a, double b)
{
    if (b == 0.0) {
        longjmp(buf, 1); // throw
    } else {
        return a / b;
    }
}
//setjmp保存当前栈信息，成功返回0，当执行到longjmp时，
//恢复栈信息即跳转到setjmp位置重新执行setjmp
//且此次返回值根据longjmp函数参数给定
int main(void)
{
    int ret;

    ret = setjmp(buf);

    if (ret == 0) { // try
        printf("division ...\n");
        printf("%f\n", Divide(5.0, 0.0));
    } else if (ret == 1) { // catch
        printf("divisiong by zero\n");
    }

    return 0;
}
```

运行结果：

division ...

divisiong by zero

其实上次错误处理模式已经是c++异常处理雏形

上述相当于：

```c
#include <iostream>
using namespace std;

double Divide(double a, double b)
{
    if (b == 0.0) {
        throw 1; // throw
    } else {
        return a / b;
    }
}

int main(void)
{
    try { // try
        cout << "division ..." << endl;
        cout << Divide(3.0, 0.0) << endl;
        cout << Divide(5.0, 0.0) << endl;
    } catch (int) { // catch
        cout << "divisiong by zero" << endl;
    }

    return 0;
}
```

运行结果同上