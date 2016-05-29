# 使用 gdb 研究 libevent


```
http://libevent.org/
```

```sh
./configure --enable-debug-mode --enable-static --enable-thread-support
```

- mytimer.c  測試例子

```c
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "event2/event-config.h"
#include "event2/event.h"
#include "event2/event_struct.h"
#include "event2/util.h"


static void handle_timer(evutil_socket_t fd, short event, void* arg)
{
    printf("handle_timer function is called \n");
    fflush(stdout);
}

int main(int argc, char** argv)
{
    /* Initalize the event library */
    struct event_base* base = event_base_new();

    if (NULL == base) {
        return -1;
    }

    /* Initalize one timeout event */
    struct event timeout = {0};
    event_assign(&timeout, base, -1, EV_PERSIST, handle_timer, (void*)&timeout);

    /* Register the event */
    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = 2;
    event_add(&timeout, &tv);

    /*event dispatch*/
    event_base_dispatch(base);

    event_base_free(base);

    return 0;
}

```

```sh
gcc -g -I include/ -o mytimer mytimer.c -L./.libs -levent
```



```sh
ldd mytimer 
```

```sh
linux-vdso.so.1 =>  (0x00007ffc41f33000)
libevent-2.0.so.5 => /usr/lib/x86_64-linux-gnu/libevent-2.0.so.5 (0x00007ffb250a1000)
libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007ffb24cdc000)
libpthread.so.0 => /lib/x86_64-linux-gnu/libpthread.so.0 (0x00007ffb24abd000)
/lib64/ld-linux-x86-64.so.2 (0x0000559c1b26f000)
```

- libevent  指定編譯出來的 .libs/libevent.so 才可以使用 gdb 

```sh
export LD_PRELOAD=.libs/libevent.so
```

- ldd mytimer 

```
linux-vdso.so.2 =>  (0x00007fff65714000)
.libs/libevent.so (0x00007f65044d8000)
libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f65040f2000)
libpthread.so.0 => /lib/x86_64-linux-gnu/libpthread.so.0 (0x00007f6503ed4000)
/lib64/ld-linux-x86-64.so.2 (0x0000559012306000)
```

- gdb test

```
cgdb ./mytimer
```


