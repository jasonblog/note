# offsetof與container_of


###1、前言

　　今天在看代碼時，遇到offsetof和container_of兩個宏，覺得很有意思，功能很強大。offsetof是用來判斷結構體中成員的偏移位置，container_of宏用來根據成員的地址來獲取結構體的地址。兩個宏設計的很巧妙，值得學習。linux內核中有著兩個宏的定義，並在鏈表結構中得到應用。不得不提一下linux內核中的鏈表，設計的如此之妙，只需要兩個指針就搞定了。後續認真研究一下這個鏈表結構。

###2、offsetof宏

　　使用offsetof宏需要包含stddef.h頭文件，實例可以參考：http://www.cplusplus.com/reference/cstddef/offsetof/。

offsetof宏的定義如下：

```c
#define offsetof(type, member) (size_t)&(((type*)0)->member)
```

巧妙之處在於將地址0強制轉換為type類型的指針，從而定位到member在結構體中偏移位置。編譯器認為0是一個有效的地址，從而認為0是type指針的起始地址。

###3、container_of宏

使用container_of宏需要包含linux/kernel.h頭文件，container_of宏的定義如下所示：


```c
#define container_of(ptr, type, member) ({ \
     const typeof( ((type *)0)->member ) *__mptr = (ptr); \
     (type *)( (char *)__mptr - offsetof(type,member) );}) 
```


container_of宏分為兩部分，

第一部分：
```c
const typeof( ((type *)0)->member ) *__mptr = (ptr);
```
通過typeof定義一個member指針類型的指針變量__mptr，（即__mptr是指向member類型的指針），並將__mptr賦值為ptr。

第二部分：
```c
(type *)( (char *)__mptr - offsetof(type,member))
```
，通過offsetof宏計算出member在type中的偏移，然後用member的實際地址__mptr減去偏移，得到type的起始地址，即指向type類型的指針。

第一部分的目的是為了將統一轉換為member類型指針。


###4、測試程序
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
測試結果：

```sh
id offset: 0
name offset: 4
age offset: 36
age:10
stu address:0xb41010
ptr address:0xb41010
```

###5、參考網址

http://blog.csdn.net/thomas_nuaa/article/details/3542572
http://blog.chinaunix.net/uid-28489159-id-3549971.html
http://blog.csdn.net/yinkaizhong/article/details/4093795



