# Linux 逆向工程的工具介绍


Linux 逆向工程主要是用要一些二进制解析工具(string/file/nm/readelf)、反汇编工具(objdump)、代码运行测试工具（ltrace/strace/gdb)、代码注入手法(LD_PRELOAD)等等，下面这篇文章简单的介绍了一些常用的工具。

另外可以参考：http://www.ouah.org/RevEng/

This article explains about the tools and commands that can be used to reverse engineer an executable in a Linux environment.

Reverse engineering is the act of figuring out what a software does, to which there is no source code available. Reverse engineering may not give you the exact details of the software. But you can understand fairly well about how a software was implemented.


The reverse engineering involves the following three basic steps:

Gathering the Info
Determining Program behavior
Intercepting the library calls

##I. Gathering the Info

The first step is to gather the information about the target program and what is does. For our example, we will take the ‘who’ command. ‘who’ command prints the list of currently logged in users.



###1. Strings Command

Strings is a command which print the strings of printable characters in files. So now let’s use this against our target (who) command.

```sh
# strings /usr/bin/who
```

Some of the important strings are,

```sh
users=%lu
EXIT
COMMENT
IDLE
TIME
LINE
NAME
/dev/
/var/log/wtmp
/var/run/utmp
/usr/share/locale
Michael Stone
David MacKenzie
Joseph Arceneaux
```

From the about output, we can know that ‘who’ is using some 3 files (/var/log/wtmp, /var/log/utmp, /usr/share/locale).

Read more: [Linux Strings Command Examples (Search Text in UNIX Binary Files)](http://www.thegeekstuff.com/2010/11/strings-command-examples/)



###2. nm Command

nm command, is used to list the symbols from the target program. By using nm, we can get to know the local and library functions and also the global variables used. nm cannot work on a program which is striped using ‘strip’ command.

Note: By default ‘who’ command is stripped. For this example, I compiled the ‘who’ command once again.

```sh
# nm /usr/bin/who
```

This will list the following:

```sh
08049110 t print_line
08049320 t time_string
08049390 t print_user
08049820 t make_id_equals_comment
080498b0 t who
0804a170 T usage
0804a4e0 T main
0804a900 T set_program_name
08051ddc b need_runlevel
08051ddd b need_users
08051dde b my_line_only
08051de0 b time_format
08051de4 b time_format_width
08051de8 B program_name
08051d24 D Version
08051d28 D exit_failure
```

In the above output:

>t|T – The symbol is present in the .text code section
b|B – The symbol is in UN-initialized .data section
D|d – The symbol is in Initialized .data section.
The Capital or Small letter, determines whether the symbol is local or global.

From the about output, we can know the following,

>It has the global function (main,set_program_name,usage,etc..)
It has some local functions (print_user,time_string etc..)
It has global initialized variables (Version,exit_failure)
It has the UN-initialized variables (time_format, time_format_width, etc..)
Sometimes, by using the function names we can guess what the functions will do.

Read more: [10 Practical Linux nm Command Examples](http://www.thegeekstuff.com/2012/03/linux-nm-command/)

The other commands that can be used to get information are

```sh
ldd command
fuser command
lsof command
/proc file system
```

## II. Determining Program Behavior

###3. ltrace Command

It traces the calls to the library function. It executes the program in that process.

```sh
# ltrace /usr/bin/who
```

The output is shown below.

```sh
utmpxname(0x8050c6c, 0xb77068f8, 0, 0xbfc5cdc0, 0xbfc5cd78)          = 0
setutxent(0x8050c6c, 0xb77068f8, 0, 0xbfc5cdc0, 0xbfc5cd78)          = 1
getutxent(0x8050c6c, 0xb77068f8, 0, 0xbfc5cdc0, 0xbfc5cd78)          = 0x9ed5860
realloc(NULL, 384)                                                   = 0x09ed59e8
getutxent(0, 384, 0, 0xbfc5cdc0, 0xbfc5cd78)                         = 0x9ed5860
realloc(0x09ed59e8, 768)                                             = 0x09ed59e8
getutxent(0x9ed59e8, 768, 0, 0xbfc5cdc0, 0xbfc5cd78)                 = 0x9ed5860
realloc(0x09ed59e8, 1152)                                            = 0x09ed59e8
getutxent(0x9ed59e8, 1152, 0, 0xbfc5cdc0, 0xbfc5cd78)                = 0x9ed5860
realloc(0x09ed59e8, 1920)                                            = 0x09ed59e8
getutxent(0x9ed59e8, 1920, 0, 0xbfc5cdc0, 0xbfc5cd78)                = 0x9ed5860
getutxent(0x9ed59e8, 1920, 0, 0xbfc5cdc0, 0xbfc5cd78)                = 0x9ed5860
realloc(0x09ed59e8, 3072)                                            = 0x09ed59e8
getutxent(0x9ed59e8, 3072, 0, 0xbfc5cdc0, 0xbfc5cd78)                = 0x9ed5860
getutxent(0x9ed59e8, 3072, 0, 0xbfc5cdc0, 0xbfc5cd78)                = 0x9ed5860
getutxent(0x9ed59e8, 3072, 0, 0xbfc5cdc0, 0xbfc5cd78)
```

You can observe that there is a set of calls to getutxent and its family of library function. You can also note that ltrace gives the results in the order the functions are called in the program.

Now we know that ‘who’ command works by calling the getutxent and its family of function to get the logged in users.

### 4. strace Command

strace command is used to trace the system calls made by the program. If a program is not using any library function, and it uses only system calls, then using plain ltrace, we cannot trace the program execution.

```sh
# strace /usr/bin/who
```

```sh
[b76e7424] brk(0x887d000)               = 0x887d000
[b76e7424] access("/var/run/utmpx", F_OK) = -1 ENOENT (No such file or directory)
[b76e7424] open("/var/run/utmp", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = 3
.
.
.
[b76e7424] fcntl64(3, F_SETLKW, {type=F_RDLCK, whence=SEEK_SET, start=0, len=0}) = 0
[b76e7424] read(3, "\10\325"..., 384) = 384
[b76e7424] fcntl64(3, F_SETLKW, {type=F_UNLCK, whence=SEEK_SET, start=0, len=0}) = 0
```

You can observe that whenever malloc function is called, it calls brk() system call. The getutxent library function actually calls the ‘open’ system call to open ‘/var/run/utmp’ and it put’s a read lock and read the contents then release the locks.

Now we confirmed that who command read the utmp file to display the output.

Both ‘strace’ and ‘ltrace’ has a set of good options which can be used.

> -p pid – Attaches to the specified pid. Useful if the program is already running and you want to know its behavior.
-n 2 – Indent each nested call by 2 spaces.
-f – Follow fork
Read more: 7 Strace Examples to Debug the Execution of a Program in Linux


## III. Intercepting the library calls

###5. LD_PRELOAD & LD_LIBRARY_PATH

LD_PRELOAD allows us to add a library to a particular execution of the program. The function in this library will overwrite the actual library function.

Note: We can’t use this with programs set with ‘suid’ bit.

Let’s take the following program.


```c
#include <stdio.h>
int main()
{
    char str1[] = "TGS";
    char str2[] = "tgs";

    if (strcmp(str1, str2)) {
        printf("String are not matched\n");
    } else {
        printf("Strings are matched\n");
    }
}
```

Compile and execute the program.

```sh
# cc -o my_prg my_prg.c
# ./my_prg
```

It will print “Strings are not matched”.

Now we will write our own library and we will see how we can intercept the library function.


```c
#include <stdio.h>
int strcmp(const char* s1, const char* s2)
{
    // Always return 0.
    return 0;
}
```


Compile and set the LD_LIBRARY_PATH variable to current directory.

```sh
# cc -o mylibrary.so -shared library.c -ldl
# LD_LIBRARY_PATH=./:$LD_LIBRARY_PATH
```

Now a file named ‘library.so’ will be created.
Set the LD_PRELOAD variable to this file and execute the string comparison program.

```sh
# LD_PRELOAD=mylibrary.so ./my_prg
```

Now it will print ‘Strings are matched’ because it uses our version of strcmp function.

Note: If you want to intercept any library function, then your own library function should have the same prototype as the original library function.

We have just covered the very basic things needed to reverse engineer a program.

For those who would like to take next step in reverse engineering, understanding the ELF file format and Assembly Language Program will help to a greater extent.