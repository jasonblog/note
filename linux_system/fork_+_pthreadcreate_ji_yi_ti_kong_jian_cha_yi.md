# fork + pthread_create 記憶體空間差異

大家知道，pthread_create()函數的線程函數必須是`靜態的函數`，`以標準的__cdecl的方式調用`的，而`C++的成員函數是以__thiscall的方式調用`的，`相當於一個普通函數有一個默認的const ClassType* this參數`。

為數據封裝的需要，我常常把線程函數封裝在一個類的內部，定義一個類的`私有靜態成員函數`來作為pthread的線程函數，通常如下：


##  pthread_create(xx,xxx,xxx,帶入main thread stack value or this class 的 instace); 

因此在子thread 可以存取共享 main thread 的 變數 or this instace

```cpp
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/syscall.h>

#define GetProcessInfo(s) do { sprintf(s, "%d %d  %s:%d %s", getpid(), gettid(), __FILE__, __LINE__, __func__);} while (0)

pid_t gettid()
{
    pid_t tid;
    tid = syscall(SYS_gettid);
    return tid;
}

class B
{
public :
    int data;

    B()
    {
        data = 100;
    }

    static void* print(void* __this)
    {
        B* _this = (B*)__this;
        char s[100];

        while (1) {
            GetProcessInfo(s);
            printf("%s, %p %d\n", s, _this,  _this->data);
            sleep(1);
        }

        pthread_exit(NULL);
    }

    static void* add(void* __this)
    {
        B* _this = (B*)__this;
        char s[100];

        while (1) {
            _this->data += 1;
            GetProcessInfo(s);
            printf("%s, %p %d\n", s, _this,  _this->data);
            sleep(2);
        }

        pthread_exit(NULL);
    }

    void start_thread()
    {
        pthread_t thread_add, thread_print;
        // 帶入 this 可以讓子thread 存取 main thread B class 的 b instace
        pthread_create(&(thread_print), NULL, print, (void*)this);
        pthread_create(&(thread_add), NULL, add, (void*)this);
    }
};

int main(int argc, char* argv[])
{
    char s[100];
    B b;
    b.start_thread();

    while (1) {
        GetProcessInfo(s);
        printf("%s,%d\n", s,  b.data);
        sleep(3);
    }




    return 0;
}

```


```c
## #define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/syscall.h>

#define GetProcessInfo(s) do { sprintf(s, "%d %d  %s:%d %s", getpid(), gettid(), __FILE__, __LINE__, __func__);} while (0)

pid_t gettid()
{
    pid_t tid;
    tid = syscall(SYS_gettid);
    return tid;
}

void* incrementA(void* value)
{
    int* val = (int*) value;
	char s[100];
	GetProcessInfo(s);
	printf("%s, val=%d\n", s, *val);	
	*val += 100;
    sleep(5);
	*val += 100;
	GetProcessInfo(s);
	printf("%s, val=%d\n", s, *val);	
    return 0;
}

void* incrementB(void* value)
{
    int* val = (int*) value;
	char s[100];
    sleep(1);
	GetProcessInfo(s);
	printf("%s, val=%d\n", s, *val);	
    sleep(10);
	*val += 10;
	GetProcessInfo(s);
	printf("%s, val=%d\n", s, *val);	
    return 0;
}

int main(int argc, char** argv)
{

    int stackvar = 0;

    pthread_t thread1, thread2;
    int iret1, iret2;

    // 帶入 stackvar 可以讓子thread 存取 main thread 資料
    iret1 = pthread_create(&thread1, NULL, incrementA, (void*) &stackvar);
    iret2 = pthread_create(&thread2, NULL, incrementB, (void*) &stackvar);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

	char s[100];
	GetProcessInfo(s);

    printf("%s, %d\n",s, stackvar);

    return 0;
}
```



---

```c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/syscall.h>

/**************************************
 * fork-pthread.c
 *
 * pthread_join一般是主線程來調用，用來等待子線程退出，因為是等待，所以是阻塞的，一般主線程會依次join所有它創建的子線程。
 * pthread_exit一般是子線程調用，用來結束當前線程。
 * 一個線程對應一個pthread_join()調用，對同一個線程進行多次pthread_join()調用是邏輯錯誤。
 * -1: fork() 發生錯誤, 無 child process 產生.
 *  0: 在child process裡
 * >0: 在parent process裡, fork() 回傳值, 即是 child process 的 PID
 *************************************/

#define DELAY 10

int g = 100;

pid_t gettid()
{
    pid_t tid;
    tid = syscall(SYS_gettid);
    return tid;
}

class B
{
public :
    int a;

    B()
    {
        a = 100;
    }

    static void* print(void* __this)
    {
        B * _this =(B *)__this;
        while (1) {
            printf("print pid=%d, tid=%d, a=%d\n", getpid(), gettid(), _this->a);
            sleep(1);
        }

        pthread_exit(NULL);
    }

    static void* add(void* __this)
    {
        B * _this =(B *)__this;
        while (1) {
            _this->a += 1;
            printf("add pid=%d, tid=%d, a=%d\n", getpid(), gettid(), _this->a);
            sleep(2);
        }

        pthread_exit(NULL);
    }

    void start_thread()
    {
        int err;
        pthread_t thread_add, thread_print;
        err = pthread_create(&(thread_print), NULL, print, (void*)this);
        err = pthread_create(&(thread_add), NULL, add, (void*)this);
    }
};


void* func(void* param)
{
    int slept = DELAY;

    B b;
    b.start_thread();

    printf("-------------------------------------------------------------------\n");
    printf("func pid=%d, tid=%d, a=%d\n", getpid(), gettid(), b.a);

    printf("thread pid=%d, tid=%d, g=%d, g address=%p[+]\n", getpid(), gettid(), g,
           (void*)&g);
    g = 88;
    printf("thread pid=%d, tid=%d, g=%d, g address=%p[-]\n", getpid(), gettid(), g,
           (void*)&g);
    fflush(stdout);


    while ((slept = sleep(slept)) != 0) ;

    pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
    int err;
    pid_t pid;
    int status;
    pthread_t thread;
    int slept = DELAY;
    printf("main pid=%d tid=%d\n", getpid(), gettid());
    fflush(stdout);

    g = 100;

    pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // fils
        printf("child pid=%d tid=%d, g=%d, g address=%p[+]\n", getpid(), gettid(), g,
               (void*)&g);
        g = 50;
        printf("child pid=%d tid=%d, g=%d, g address=%p[-]\n", getpid(), gettid(), g,
               (void*)&g);

        fflush(stdout);

        while ((slept = sleep(slept)) != 0) ;

        return (EXIT_SUCCESS);
    } else {
        // père
        err = pthread_create(&(thread), NULL, &func, NULL);

        if (err != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }

        printf("parent pid=%d tid=%d, g=%d, g address=%p[+]\n", getpid(), gettid(), g,
               (void*)&g);

        // 主thread等到子thread結束
        err = pthread_join(thread, NULL);
        printf("parent pid=%d tid=%d, g=%d, g address=%p[-]\n", getpid(), gettid(), g,
               (void*)&g);

        if (err != 0) {
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }

        printf("waitpid=%d\n", pid);

        // 父行程等子行程結束
        int fils = waitpid(pid, &status, 0);

        if (fils == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }

        if (!WIFEXITED(status)) {
            fprintf(stderr, "Erreur de waitpid\n");
            exit(EXIT_FAILURE);
        }
    }

    return (EXIT_SUCCESS);
}
```

## 函數模版不單可以替換類型本身，還能替換類型的成員函數。
```cpp
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/syscall.h>

using namespace std;

#define GetProcessInfo(ss) do { sprintf(ss, "%d %d  %p  %s:%d %s", getpid(), gettid(),(void*)this, __FILE__, __LINE__, __func__);} while (0)

pid_t gettid()
{
    pid_t tid;
    tid = syscall(SYS_gettid);
    return tid;
}

template <typename TYPE, void (TYPE::*_RunThread)() >
void* _thread_t(void* param)
{
    TYPE* This = (TYPE*)param;
    // 多 wrapper 一層讓_RunThread 不需要是static 函數
    This->_RunThread();
    return NULL;
}

class MyClass
{
public:
    MyClass()
    {
        data = 66;
        char s[100];
        GetProcessInfo(s);
        printf("%s, data=%d\n", s, data);
        pthread_create(&tid, NULL, _thread_t<MyClass, &MyClass::_RunThread>, this);
    }

    ~MyClass()
    {
        char s[100];
        GetProcessInfo(s);
        printf("%s, data=%d\n", s, data);

        pthread_cancel(tid);
        pthread_join(tid, NULL);
    }

    void _RunThread()
    {
        char s[100];
        GetProcessInfo(s);

        while (1) {
            printf("%s, data=%d\n", s, data);
            data+= 3;
            sleep(1);
            //this->DoSomeThing();
            //...
        }
    }

    int data;

private:
    pthread_t tid;
};

int main(int argc, char* argv[])
{
    MyClass a;

    while (1) {
        printf("main data=%d\n", a.data);
        sleep(2);
        //this->DoSomeThing();
        //...
    }

    return 0;
}
```

## C++ 建構解構都由產生 instance 的pid 解構

```cpp
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/syscall.h>

pid_t gettid()
{
    pid_t tid;
    tid = syscall(SYS_gettid);
    return tid;
}

void get_pid_tid(char s[]) {
	sprintf(s, "pid=%d, tid=%d\n", getpid(), gettid());
}

class testClass
{
public:
    testClass();
    ~testClass();
private:
    static void* thread_func(void*);
    pthread_t thrid;
};

testClass::testClass()
{
	char s[50];
	get_pid_tid(s);
    printf("[%s:%d,%s,%s]\n", __FILE__, __LINE__, __func__, s);
    pthread_create(&thrid, NULL, thread_func, this);
}

testClass::~testClass()
{
	char s[50];
	get_pid_tid(s);
    printf("[%s:%d,%s,%s]\n", __FILE__, __LINE__, __func__, s);
    pthread_cancel(thrid);
    pthread_join(thrid, NULL);
}

void* testClass::thread_func(void* param)
{
    int i = 0;
    testClass* self = (testClass*)param;
	char s[50];
	get_pid_tid(s);

    printf("[%s:%d,%s,%s]thread start\n", __FILE__, __LINE__, __func__,s);

    while (1) {
        printf("[%s:%d,%s]%d,%s\n", __FILE__, __LINE__, __func__, i++,s);
        sleep(1);
    }

    printf("[%s:%d,%s,%s]thread stop\n", __FILE__, __LINE__, __func__,s);
    return (void*)0;
}

int main(int argc, char** argv)
{
    testClass* test;
    test = new testClass();
    sleep(10);
    delete test;
    return 0;
}
```

