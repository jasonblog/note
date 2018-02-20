# Trace Function Calls Using GDB Revisited


In an earlier post I discussed how to trace calls using GDB so that function calls and their arguments can easily be viewed. What I neglected to mention was rbreak , a feature of GDB to be able to set breakpoints using a regular expression.

Using `rbreak` you can get the same functionality but with much less effort. For example, to get the same behavior as before (setting a breakpoint on each function call and printing a trace of the bottom most level) all you need to provide to GDB are the following commands:



```sh
set args 4 10

rbreak file.c:.
command
silent
backtrace 1
continue 
end

run
```

I have placed these commands in a file called gdb_commands. The `rbreak` command above tells GDB to set a breakpoint on any function call within file.c. With a quick call to GDB we find almost the exact same functionality as we had before.

```sh
dustymabe@laptop: gdbpost>gdb -quiet -command=gdb_commands ./a.out
Reading symbols from
/content/gdbpost/a.out...done.
Breakpoint 1 at 0x400578: file file.c, line 12.
int main(int, char **);
Breakpoint 2 at 0x400546: file file.c, line 7.
int modulo(int, int);
Breakpoint 3 at 0x40055f: file file.c, line 8.
int plus(int, int);
Breakpoint 4 at 0x400533: file file.c, line 6.
int squared(int);
#0  main (argc=3, argv=0x7fffffffe698) at file.c:12
#0  squared (x=4) at file.c:6
#0  squared (x=10) at file.c:6
#0  modulo (x=100, y=16) at file.c:7
#0  plus (x=100, y=100) at file.c:8
#0  modulo (x=200, y=16) at file.c:7
#0  plus (x=200, y=100) at file.c:8
#0  modulo (x=300, y=16) at file.c:7
#0  plus (x=300, y=100) at file.c:8
#0  modulo (x=400, y=16) at file.c:7
LCM is 400
[Inferior 1 (process 1579) exited with code 013]
Missing separate debuginfos, use: debuginfo-install
glibc-2.15-56.fc17.x86_64
(gdb) quit
```

In my previous example, if you had 50 functions that you wanted to trace, you needed 50 `break` commands as well as 50 `command` blocks.

Considering this fact, it is apparent that `rbreak` is much more efficient and friendly for when you are setting breakpoints from the GDB cli. Another extremely useful feature is that it also works for c++ classes, so you can break on any function in a class regardless of what file the function is defined in.

To show an example of this I converted all the math function calls from the program in file.c to a c++ class. I put the result in a file called file.cc (shown below).


```cpp
#include <stdio.h>
#include <stdlib.h>
// A program that will square two integers and then find the LCM
// of the resulting two integers. 
class Math {
    public:
        int squared(int x)       { return x*x; }
        int modulo(int x, int y) { return x%y; }
        int plus(int x, int y)   { return x+y; }
};

int main(int argc, char *argv[]) {
    int x, y, x2, y2, tmp;
    if (argc != 3) return 0; 

    Math* m = new Math;

    x = atoi(argv[1]);  
    y = atoi(argv[2]);
    x2       = m->squared(x);
    y2 = tmp = m->squared(y);

    while (1) {
        if (m->modulo(tmp,x2) == 0) break;
        tmp = m->plus(tmp,y2);
    }
    printf("LCM is %d\n", tmp);
}
```

I then used the `rbreak Math::` command in the following GDB commands file (I named this one gdb_commands2) to run GDB and set up the break points.


```sh
set args 4 10

rbreak Math::
command
silent
backtrace 1
continue 
end

run
```


And.. Here we are with the final result:


```sh
dustymabe@laptop: gdbpost>gdb -quiet -command=gdb_commands2 ./a.out
Reading symbols from
/content/gdbpost/a.out...done.
Breakpoint 1 at 0x400758: file file.cc, line 9.
int Math::modulo(int, int);
Breakpoint 2 at 0x400776: file file.cc, line 10.
int Math::plus(int, int);
Breakpoint 3 at 0x400741: file file.cc, line 8.
int Math::squared(int);
#0  Math::squared (this=0x601010, x=4) at file.cc:8
#0  Math::squared (this=0x601010, x=10) at file.cc:8
#0  Math::modulo (this=0x601010, x=100, y=16) at file.cc:9
#0  Math::plus (this=0x601010, x=100, y=100) at file.cc:10
#0  Math::modulo (this=0x601010, x=200, y=16) at file.cc:9
#0  Math::plus (this=0x601010, x=200, y=100) at file.cc:10
#0  Math::modulo (this=0x601010, x=300, y=16) at file.cc:9
#0  Math::plus (this=0x601010, x=300, y=100) at file.cc:10
#0  Math::modulo (this=0x601010, x=400, y=16) at file.cc:9
LCM is 400
[Inferior 1 (process 2304) exited normally]
Missing separate debuginfos, use: debuginfo-install
glibc-2.15-56.fc17.x86_64 libgcc-4.7.0-5.fc17.x86_64
libstdc++-4.7.0-5.fc17.x86_64
(gdb) quit
```
Enjoy!

Dusty

PS - If you are new to GDB I have found a great reference for beginners [here](https://betterexplained.com/articles/debugging-with-gdb/).
