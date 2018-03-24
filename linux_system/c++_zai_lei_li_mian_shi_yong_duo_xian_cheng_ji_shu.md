# C++ 在類裡面使用多線程技術


## 前言
有很多時候，我們希望可以在C++類裡面對那些比較耗時的函數使用多線程技術，但是熟悉C++對象語法的人應該知道，C++類的成員函數的函數指針不能直接做為參數傳到pthread_create,**主要因為是C++成員函數指針帶有類命名空間，同時成員函數末尾是會被C++編譯器加上可以接收對象地址的this指針參數。因此需要將成員函數做一定的轉化， 將其轉化為不被編譯器加上this指針，而由我們自己來為該函數維護」this」指針即可。**  
## 舉例分析

```cpp
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
using namespace std;
class Test
{
public:
    int sum = 0;
    int cnt;
public:
    int insert();
};
int Test::insert()
{
    sleep(2);
    cnt += 1;
}
```

```sh
1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
```

如上所示，代碼聲明瞭一個類Test,假設該類有一個十分耗時的成員函數：insert()，這個求和函數每次執行需要2000ms的時間。對於如此耗時的函數，我們在設計時都會想方法將其設計為線程函數，這樣調用者才不會阻塞。 
於是我們為其加上多線程：

```cpp
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
using namespace std;

class Test
{
public:
    int sum = 0;
    int cnt;
public:
    int insert();
    void* insert_pth(void*);
    void lanch();
};

int Test::insert()
{
    sleep(2);
    sum += 1;
}

void* Test::insert_pth(void*)
{
    insert();
}

void Test::lanch()
{
    pthread_t pth;
    pthread_create(&pth, NULL, insert_pth, (void*)NULL);
}

int main()
{
    Test t;
    t.lanch();
    return 0;
}
```
```sh
1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
21
22
23
24
25
26
27
28
29
30
31
32
33
34
35
36
37
```
以上代碼通過調用lanch()來創建多線程來執行insert_pth，insert_pth 再調用insert(). 
但是 這樣的代碼在編譯時即會報錯。


```sh
pthread.cpp: In member function 『void Test::lanch()』:
pthread.cpp:30:42: error: invalid use of non-static member function
  pthread_create(&pth,NULL,insert_pth,NULL);
```

```sh
1
2
3
4
```

只需將insert_pth變化為`static`函數，同時將insert邏輯代碼轉移到insert_pth即可


```cpp
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
using namespace std;
class Test
{
public:
    int sum = 0;
    int cnt;
public:
    int insert();
    static  void* insert_pth(void*);
    void lanch();
};
int Test::insert()
{
    sleep(2);
    sum += 1;
    printf("%d insert.....\n", sum);
}
void* Test::insert_pth(void* __this)
{
    Test* _this = (Test*)__this;
    sleep(2);
    _this->sum += 1;
    printf("%d insert.....\n", _this->sum);
}
void Test::lanch()
{
    pthread_t pth;
    pthread_create(&pth, NULL, insert_pth, (void*)this);
}
int main()
{
    Test t;
    t.sum = 0;
    t.lanch();
    sleep(5);
    return 0;
}
```
```sh
1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
21
22
23
24
25
26
27
28
29
30
31
32
33
34
35
36
37
38
39
40
41
42
43
```

##總結
使用多線程處理耗時成員函數的步驟： 
1. 聲明另外一個靜態函數：`static void XXX_pth(void __this); `
該函數與目標成員函數在函數名儘量保持一致 

2. 將原成員函數的代碼拷貝至`void * XXX_pth(void * __this);` 
在 XXX_pth（）開始處將`void * __this `轉化為 對象的指針 `ObjectPoint _this; `
將拷貝下來的所有成員變量加上`_this->`

3. 編寫線程啟動代碼。 
注意`pthread_create()最後一個參數傳入this指針`


## 注意

`在 XXX_pth()函數內容不要調用類的其它成員函數，否則成員函數將無法獲取正確的this指針而操作錯誤內存,從而導致segmantation fault.`


在C++的類中，普通成員函數不能作為pthread_create的線程函數，如果要作為pthread_create中的線程函數，必須是static !

在C語言中，我們使用pthread_create創建線程，線程函數是一個`全局函數`，`所以在C++中，創建線程時，也應該使用一個全局函數。static定義的類的成員函數就是一個全局函數。`


更多 參考  http://blog.csdn.net/ksn13/article/details/40538083 
