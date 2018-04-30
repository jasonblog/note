#include <stdio.h>
#define MaxSize 60
void Hanoi1(int n, char x, char y, char z);
void Hanoi2(int n, char x, char y, char z);
void PrintHanoi(int no, char from, char to);
typedef struct {
    char x;
    char y;
    char z;
    int flag;
    int num;
} Stack;
void main()
{
    int n;
    int i = 1;
    printf("请输入移动圆盘的个数:");
    scanf("%d", &n);
    printf("汉诺塔的递归实现:\n");
    Hanoi1(n, 'X', 'Y', 'Z');
    printf("汉诺塔的非递归实现:\n");
    Hanoi2(n, 'X', 'Y', 'Z');
}

void Hanoi1(int n, char x, char y, char z)
{
    if (n == 1) {
        PrintHanoi(n, x, z);
    } else {
        Hanoi1(n - 1, x, z, y);
        PrintHanoi(n, x, z);
        Hanoi1(n - 1, y, x, z);
    }

}
void Hanoi2(int n, char x, char y, char z)
{
    int top = 1, x1, y1, z1, m;
    Stack s[MaxSize];
    s[top].num = n;
    s[top].flag = 1;
    s[top].x = x;
    s[top].y = y;
    s[top].z = z;

    while (top > 0) {
        if (s[top].flag == 1) {
            m = s[top].num;
            x1 = s[top].x;
            y1 = s[top].y;
            z1 = s[top].z;
            top--;
            top++;
            s[top].num = m - 1;
            s[top].flag = 1;
            s[top].x = y1;
            s[top].y = x1;
            s[top].z = z1;
            top++;
            s[top].num = m;
            s[top].flag = 0;
            s[top].x = x1;
            s[top].y = z1;
            top++;
            s[top].num = m - 1;
            s[top].flag = 1;
            s[top].x = x1;
            s[top].y = z1;
            s[top].z = y1;

        }

        while (top > 0 && (s[top].flag == 0 || s[top].num == 1)) {
            if (top > 0 && s[top].flag == 0) {
                PrintHanoi(s[top].num, s[top].x, s[top].y);
                top--;
            }

            if (top > 0 && s[top].num == 1) {
                PrintHanoi(s[top].num, s[top].x, s[top].z);
                top--;
            }
        }
    }
}
void PrintHanoi(int no, char from, char to)
{
    printf("将第%d个圆盘从塔座%c移动到%c\n", no, from, to);
}
