#include <stdio.h>          /*包含输入输出函数*/
#define N 10
/*结构体类型及变量定义、初始化*/
struct student {
    char* number;
    char* name;
    char sex;
    int age;
    float score;
} stu[3] = {{"06001", "Wang Chong", 'm', 22, 78.5},
    {"06002", "Li Hua", 'f', 21, 87.0},
    {"06003", "Zhang Yang", 'm', 22, 90.0}
};
void main()
{
    struct student* p;
    printf("学生基本情况表:\n");
    printf("编号          姓名       性别    年龄   成绩\n");

    for (p = stu; p < stu + 3; p++) {   /*通过指向结构体的指针输出学生信息*/
        printf("%-8s%12s%8c%8d%6d\n", p->number, p->name, p->sex, p->age, p->score);
    }
}
