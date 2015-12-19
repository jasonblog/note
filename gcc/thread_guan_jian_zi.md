# __thread關鍵字


__thread是GCC內置的線程局部存儲設施，存取效率可以和全局變量相比。__thread變量每一個線程有一份獨立實體，各個線程的值互不干擾。可以用來修飾那些帶有全局性且值可能變，但是又不值得用全局變量保護的變量。


__thread使用規則：只能修飾POD類型(類似整型指針的標量，不帶自定義的構造、拷貝、賦值、析構的類型，二進制內容可以任意複製memset,memcpy,且內容可以復原)，不能修飾class類型，因為無法自動調用構造函數和析構函數，可以用於修飾全局變量，函數內的靜態變量，不能修飾函數的局部變量或者class的普通成員變量，且`__thread`變量值只能初始化為編譯器常量(值在編譯器就可以確定const int i=5,運行期常量是運行初始化後不再改變const int i=rand()).

- test.cpp

```c
#include<iostream>
#include<pthread.h>
#include<unistd.h>

using namespace std;

const int i = 5;
__thread int var = i;//两种方式效果一样
void* worker1(void* arg);
void* worker2(void* arg);

int main() {
    pthread_t pid1,pid2;

    static __thread int temp=10;//修饰函数内的static变量
    pthread_create(&pid1,NULL,worker1,NULL);
    pthread_create(&pid2,NULL,worker2,NULL);
    pthread_join(pid1,NULL);
    pthread_join(pid2,NULL);
    cout<<temp<<endl;//输出10
    return 0;
}
void* worker1(void* arg) {
    cout<<++var<<endl;//输出 6
}
void* worker2(void* arg) {
    sleep(1);//等待线程1改变var值，验证是否影响线程2
    cout<<++var<<endl;//输出6
}

```

```c
g++ test.cpp -o test_ -pthread
```

```c
./test_ 
6
6
10
```