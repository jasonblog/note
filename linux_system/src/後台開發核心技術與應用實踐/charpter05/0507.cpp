#include<stdio.h>
#include<stdlib.h>
int g1 = 0, g2 = 0, g3 = 0;
int max(int i)
{
    int m1 = 0, m2, m3 = 0, *p_max;
    static int n1_max = 0, n2_max, n3_max = 0;
    p_max = (int*)malloc(10);
    printf("打印max程序地址\n");
    printf("in max: %x\n\n", max);
    printf("打印max传入参数地址\n");
    printf("in max: %x\n\n", &i);
    printf("打印max函数中静态变量地址\n");
    printf("%x\n", &n1_max); //打印各本地变量的内存地址
    printf("%x\n", &n2_max);
    printf("%x\n\n", &n3_max);
    printf("打印max函数中局部变量地址\n");
    printf("%x\n", &m1); //打印各本地变量的内存地址
    printf("%x\n", &m2);
    printf("%x\n\n", &m3);
    printf("打印max函数中malloc分配地址\n");
    printf("%x\n\n", p_max); //打印各本地变量的内存地址

    if (i) {
        return 1;
    } else {
        return 0;
    }
}
int main(int argc, char** argv)
{
    static int s1 = 0, s2, s3 = 0;
    int v1 = 0, v2, v3 = 0;
    int* p;
    p = (int*)malloc(10);
    printf("打印各全局变量(已初始化)的内存地址\n");
    printf("%x\n", &g1); //打印各全局变量的内存地址
    printf("%x\n", &g2);
    printf("%x\n\n", &g3);
    printf("======================\n");
    printf("打印程序初始程序main地址\n");
    printf("main: %x\n\n", main);
    printf("打印主参地址\n");
    printf("argv: %x\n\n", argv);
    printf("打印各静态变量的内存地址\n");
    printf("%x\n", &s1); //打印各静态变量的内存地址
    printf("%x\n", &s2);
    printf("%x\n\n", &s3);
    printf("打印各局部变量的内存地址\n");
    printf("%x\n", &v1); //打印各本地变量的内存地址
    printf("%x\n", &v2);
    printf("%x\n\n", &v3);
    printf("打印malloc分配的堆地址\n");
    printf("malloc: %x\n\n", p);
    printf("======================\n");
    max(v1);
    printf("======================\n");
    printf("打印子函数起始地址\n");
    printf("max: %x\n\n", max);
    return 0;
}
