#include <setjmp.h>
#include <stdio.h>

jmp_buf buf;

int b() {
    puts("stat of b");
    //回傳值可以是任意數字，
    //例如5，但請不要回傳0以免造成混淆
    longjmp(buf, 5);
    puts("end of b");
}

int a() {
    puts("stat of a");
    b();
    puts("end of a");
}

int main(int argc, char** argv) {
    int ret;
    register int p1=11;
    volatile int p2=22;
    int p3=33;
    p1=1;
    p2=2;
    p3=3;
    //回傳值0有特別用途，代表setjmp成功
    if ((ret=setjmp(buf)) == 0)
        a();
    else {
        printf("return form longjmp."
            " the return value is %d\n", ret);
        printf("p1 = %d, p2 = %d, p3 = %d\n", p1, p2, p3);
    }
}