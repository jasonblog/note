#include<stdio.h>
int* Address(int (*ptr)[4], int n);
void Display(int a[][4], int n, int* p);
void main()
{
    int row, n = 4;
    int* p;
    int score[3][4] = {{76, 87, 85, 81}, {67, 61, 71, 60}, {81, 89, 82, 78}};
    printf("请输入学生的编号(1或2或3).输入0退出程序.\n");
    scanf("%d", &row);

    while (row) {
        if (row == 1 || row == 2 || row == 3) {
            printf("第%d个学生的成绩4门课的成绩是：\n", row);
            p = Address(score, row - 1);
            Display(score, n, p);
            printf("请输入学生的编号(1或2或3).输入0退出程序.\n");
            scanf("%d", &row);
        } else {
            printf("输入不合法，重新输入(1或2或3).输入0退出程序.\n");
            scanf("%d", &row);
        }
    }
}
int* Address(int (*ptrScore)[4], int n)
{
    int* p;
    p = *(ptrScore + n);
    return p;
}
void Display(int a[][4], int n, int* p)
{
    int i;

    for (i = 0; i < n; i++) {
        printf("%4d", *(p + i));
    }

    printf("\n");
}
