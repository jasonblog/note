# linux系統編程之文件與IO（七）：時間函數小結


從系統時鐘獲取時間方式

time函數介紹：
- 1、函數名稱: localtime
- 2、函數名稱: asctime
- 3、函數名稱: ctime
- 4、函數名稱: difftime
- 5、函數名稱: gmtime
- 6、函數名稱: time
- 7、函數名稱: tzset



time.h是C/C++中的日期和時間頭文件。

代碼示例

```c
#include <stdio.h>
#include <time.h>

int main(void)
{
    time_t timer = time(NULL);
    printf("ctime is %s\n", ctime(&timer)); //得到日曆時間
    return 0;
}
```

從系統時鐘獲取時間方式

```c
time_t time(time_t* timer)
```

得到從標準計時點（一般是1970年1月1日午夜）到當前時間的秒數。
```c
clock_t clock(void)
```
得到從程序啟動到此次函數調用時累計的毫秒數。

### time函數介紹

- 1、函數名稱: localtime

函數原型: struct tm *localtime(const time_t *timer)<br>
函數功能: 返回一個以tm結構表達的機器時間信息<br>
函數返回: 以tm結構表達的時間，結構tm定義如下:<br>

```c
struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};
```

參數說明: timer-使用time()函數獲得的機器時間

所屬文件: `<time.h>`

```c
#include <time.h>
#include <stdio.h>
#include <dos.h>

int main()
{
    time_t timer;
    struct tm* tblock;
    timer = time(NULL);

    tblock = localtime(&timer);
    printf("Local time is: %s", asctime(tblock));
    return 0;
}
```
- 2、函數名稱: asctime

函數原型: char* asctime(struct tm * ptr)<br>
函數功能: 得到機器時間(日期時間轉換為ASCII碼)<br>
函數返回: 返回的時間字符串格式為：星期,月,日,小時：分：秒,年<br>
參數說明: 結構指針ptr應通過函數localtime()和gmtime()得到<br>

- 3、函數名稱: ctime

函數原型: char *ctime(long time)<br>
函數功能: 得到日曆時間<br>
函數返回: 返回字符串格式：星期,月,日,小時:分:秒,年<br>
參數說明: time-該參數應由函數time獲得<br>
所屬文件: `<time.h>`

```c
#include <stdio.h>
#include <time.h>

int main()
{
    time_t t;

    time(&t);
    printf("Today's date and time: %s", ctime(&t));
    return 0;
}
```
- 4、函數名稱: difftime


函數原型: double difftime(time_t time2, time_t time1)<br>
函數功能: 得到兩次機器時間差，單位為秒<br>
函數返回: 時間差，單位為秒<br>
參數說明: time1-機器時間一,time2-機器時間二.該參數應使用time函數獲得<br>
所屬文件: `<time.h>`


```c
#include <time.h>
#include <stdio.h>
#include <dos.h>
#include <conio.h>

int main()
{
    time_t first, second;

    clrscr();
    first = time(NULL);
    delay(2000);
    second = time(NULL);
    printf("The difference is: %fseconds", difftime(second, first));
    getch();
    return 0;
}
```

- 5、函數名稱: gmtime

函數原型: struct tm *gmtime(time_t *time)<br>
函數功能: 得到以結構tm表示的時間信息<br>
函數返回: 以結構tm表示的時間信息指針<br>
參數說明: time-用函數time()得到的時間信息<br>
所屬文件: `<time.h>`

```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <dos.h>

char* tzstr = "TZ=PST8PDT";

int main()
{
    time_t t;
    struct tm* gmt, *area;

    putenv(tzstr);
    tzset();
    t = time(NULL);
    area = localtime(&t);
    printf("Local time is:%s", asctime(area));
    gmt = gmtime(&t);
    printf("GMT is:%s", asctime(gmt));

    return 0;
}
```

- 6、函數名稱: time

函數原型: time_t time(time_t *timer)<br>
函數功能: 得到機器的日曆時間或者設置日曆時間<br>
函數返回: 機器日曆時間<br>
參數說明: timer=NULL時得到機器日曆時間，timer=時間數值時，用於設置日曆時間，time_t是一個long類型<br>
所屬文件: `<time.h>`

```c
#include <time.h>
#include <stdio.h>
#include <dos.h>

int main()
{
    time_t t;
    t = time();
    printf("The number of seconds since January 1,1970 is %ld", t);
    return 0;
}
```
- 7、函數名稱: tzset

函數原型: void tzset(void)<br>
函數功能: UNIX兼容函數，用於得到時區，在DOS環境下無用途<br>
函數返回:<br>
參數說明:<br>
所屬文件: `<time.h>`

```c
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
    time_t td;

    putenv("TZ=PST8PDT");
    tzset();
    time(&td);
    printf("Current time=%s", asctime(localtime(&td)));

    return 0;
}
```


