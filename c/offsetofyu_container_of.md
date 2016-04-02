# offsetof与container_of


###1、前言

　　今天在看代码时，遇到offsetof和container_of两个宏，觉得很有意思，功能很强大。offsetof是用来判断结构体中成员的偏移位置，container_of宏用来根据成员的地址来获取结构体的地址。两个宏设计的很巧妙，值得学习。linux内核中有着两个宏的定义，并在链表结构中得到应用。不得不提一下linux内核中的链表，设计的如此之妙，只需要两个指针就搞定了。后续认真研究一下这个链表结构。

###2、offsetof宏

　　使用offsetof宏需要包含stddef.h头文件，实例可以参考：http://www.cplusplus.com/reference/cstddef/offsetof/。

offsetof宏的定义如下：

```c
#define offsetof(type, member) (size_t)&(((type*)0)->member)
```

巧妙之处在于将地址0强制转换为type类型的指针，从而定位到member在结构体中偏移位置。编译器认为0是一个有效的地址，从而认为0是type指针的起始地址。

###3、container_of宏

使用container_of宏需要包含linux/kernel.h头文件，container_of宏的定义如下所示：


```c
#define container_of(ptr, type, member) ({ \
     const typeof( ((type *)0)->member ) *__mptr = (ptr); \
     (type *)( (char *)__mptr - offsetof(type,member) );}) 
```


container_of宏分为两部分，

第一部分：
```c
const typeof( ((type *)0)->member ) *__mptr = (ptr);
```
通过typeof定义一个member指针类型的指针变量__mptr，（即__mptr是指向member类型的指针），并将__mptr赋值为ptr。

第二部分：
```c
(type *)( (char *)__mptr - offsetof(type,member))
```
，通过offsetof宏计算出member在type中的偏移，然后用member的实际地址__mptr减去偏移，得到type的起始地址，即指向type类型的指针。

第一部分的目的是为了将统一转换为member类型指针。


###4、测试程序
```c
#include <stdio.h>
#include <stdlib.h>

#define NAME_STR_LEN  32

#define offsetof(type, member) (size_t)&(((type*)0)->member)

#define container_of(ptr, type, member) ({ \
        const typeof( ((type *)0)->member ) *__mptr = (ptr); \
        (type *)( (char *)__mptr - offsetof(type,member) );})

typedef struct student_info {
    int  id;
    char name[NAME_STR_LEN];
    int  age;
} student_info;


int main()
{
    size_t off_set = 0;
    off_set = offsetof(student_info, id);
    printf("id offset: %u\n", off_set);
    off_set = offsetof(student_info, name);
    printf("name offset: %u\n", off_set);
    off_set = offsetof(student_info, age);
    printf("age offset: %u\n", off_set);
    student_info* stu = (student_info*)malloc(sizeof(student_info));
    stu->age = 10;
    student_info* ptr = container_of(&(stu->age), student_info, age);
    printf("age:%d\n", ptr->age);
    printf("stu address:%p\n", stu);
    printf("ptr address:%p\n", ptr);
    return 0;
}
```
测试结果：

```sh
id offset: 0
name offset: 4
age offset: 36
age:10
stu address:0xb41010
ptr address:0xb41010
```

###5、参考网址

http://blog.csdn.net/thomas_nuaa/article/details/3542572
http://blog.chinaunix.net/uid-28489159-id-3549971.html
http://blog.csdn.net/yinkaizhong/article/details/4093795



