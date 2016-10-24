# 使用valgrind检查内存问题


工欲善其事，必先利其器。Valgrind作为一个免费且优秀的工具包，平时大部分人可能都是使用valgrind检测内存问题，如内存泄露，越界等。其实Valgrind的用途远不止于此，其实际上为一个工具包，除了检查内存问题以外，还有其它多项用途。我准备将其大致介绍一下。本不想再介绍Valgrind检测内存问题的用法的，但是又一想，毕竟这是Valgrind的一个最有名的用途，如果少了它，不免有些遗憾，所以还是把检查内存问题作为第一篇吧。

请看一下代码：

```c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void mem_leak1(void)
{
    char *p = malloc(1);
}

static void mem_leak2(void)
{
    FILE *fp = fopen("test.txt", "w");
}

static void mem_overrun1(void)
{
    char *p = malloc(1);
    *(short*)p = 2;

    free(p);
}

static void mem_overrun2(void)
{
    char array[5];
    strcpy(array, "hello");
}

static void mem_double_free(void)
{
    char *p = malloc(1);
    free(p);
    free(p);
}

static void mem_use_wild_pointer(void)
{
    char *p = (void*)0x80184800;
    *p = 1;
}

static void mem_free_wild_pointer(void)
{
    char *p;
    free(p);
}


int main()
{
    mem_leak1();
    mem_leak2();
    mem_overrun1();
    mem_overrun2();
    mem_double_free();
    //mem_use_wild_pointer();
    mem_free_wild_pointer();

    return 0;
}
```
这里一共列出了七种常见的内存问题：1. 动态内存泄露；2. 资源泄露，这里以文件描述符为例；3. 动态内存越界；4.数组内存越界；5.动态内存double free；6.使用野指针，即未初始化的指针；7.释放野指针，即未初始化的指针；其中由于本示例代码过于简单，第6中情况，使用野指针会直接导致crash，所以在main中，并没有真正的调用那个示例代码。由于valgrind只能检测执行到的代码，所以在后面的报告中，不会报告第6种错误情况。但是，在大型的项目中，有可能使用野指针并不会导致程序crash。另外上面的7中情况，有些情况严格的说，实际上可以归为一类。

下面看怎样执行valgrind来检测内存错误：

```sh
valgrind --track-fds=yes --leak-check=full --undef-value-errors=yes ./a.out
```
上面那些option的具体含义，可以参加valgrind --help，其中有些option默认就是打开的，不过我习惯于明确的使用option，以示清晰。

看执行后的报告：


<div id="codeText" class="codeText"><ol start="1" class="dp-css"><li>==2326== Memcheck, a memory error detector</li><li>
==2326== Copyright (C) 2002-2009, and GNU GPL'd, by Julian Seward et al.</li><li>
==2326== Using Valgrind-3.5.0 and LibVEX; rerun with -h for copyright info</li><li>
==2326== Command: ./a.out</li><li>
==2326==</li><li><font class="Apple-style-span" color="#008000">/*&nbsp;这里检测到了动态内存的越界，提示Invalid write。*/</font></li><li>
==2326== Invalid write of size 2</li><li>
==2326==    at 0x80484B4: mem_overrun1 (in /home/fgao/works/test/a.out)</li><li>
==2326==    by 0x8048553: main (in /home/fgao/works/test/a.out)</li><li>
==2326==  Address 0x40211f0 is 0 bytes inside a block of size 1 alloc'd</li><li>
==2326==    at 0x4005BDC: malloc (vg_replace_malloc.c:195)</li><li>
==2326==    by 0x80484AD: mem_overrun1 (in /home/fgao/works/test/a.out)</li><li>
==2326==    by 0x8048553: main (in /home/fgao/works/test/a.out)</li><li>
==2326==</li></ol><div><span class="Apple-style-span" style="line-height: 15px; "><font class="Apple-style-span" color="#5c5c5c">&nbsp;</font><font class="Apple-style-span" color="#008000">/* 这里检测到了double free问题，提示Invalid Free&nbsp;</font></span><span class="Apple-style-span" style="line-height: 15px; "><font class="Apple-style-span" color="#008000">*/</font></span></div><ol start="1" class="dp-css"><li>
==2326== Invalid free() / delete / delete[]</li><li>
==2326==    at 0x40057F6: free (vg_replace_malloc.c:325)</li><li>
==2326==    by 0x8048514: mem_double_free (in /home/fgao/works/test/a.out)</li><li>
==2326==    by 0x804855D: main (in /home/fgao/works/test/a.out)</li><li>
==2326==  Address 0x4021228 is 0 bytes inside a block of size 1 free'd</li><li>
==2326==    at 0x40057F6: free (vg_replace_malloc.c:325)</li><li>
==2326==    by 0x8048509: mem_double_free (in /home/fgao/works/test/a.out)</li><li>
==2326==    by 0x804855D: main (in /home/fgao/works/test/a.out)</li><li>
==2326==</li><li>
</li><li><font class="Apple-style-span" color="#008000">/* 这里检测到了未初始化变量 */</font></li><li>
==2326== Conditional jump or move depends on uninitialised value(s)</li><li>
==2326==    at 0x40057B6: free (vg_replace_malloc.c:325)</li><li>
==2326==    by 0x804853C: mem_free_wild_pointer (in /home/fgao/works/test/a.out)</li><li>
==2326==    by 0x8048562: main (in /home/fgao/works/test/a.out)</li><li>
==2326==</li></ol><div><span class="Apple-style-span" style="line-height: 15px; "><font class="Apple-style-span" color="#5c5c5c">&nbsp;</font><font class="Apple-style-span" color="#008000">/* 这里检测到了非法是否野指针 */</font></span></div><ol start="1" class="dp-css"><li>
==2326== Invalid free() / delete / delete[]</li><li>
==2326==    at 0x40057F6: free (vg_replace_malloc.c:325)</li><li>
==2326==    by 0x804853C: mem_free_wild_pointer (in /home/fgao/works/test/a.out)</li><li>
==2326==    by 0x8048562: main (in /home/fgao/works/test/a.out)</li><li>
==2326==  Address 0x4021228 is 0 bytes inside a block of size 1 free'd</li><li>
==2326==    at 0x40057F6: free (vg_replace_malloc.c:325)</li><li>
==2326==    by 0x8048509: mem_double_free (in /home/fgao/works/test/a.out)</li><li>
==2326==    by 0x804855D: main (in /home/fgao/works/test/a.out)</li><li>
==2326==</li><li>
==2326==</li><li><font class="Apple-style-span" color="#008000">/*&nbsp;</font></li><li><font class="Apple-style-span" color="#008000">这里检测到了文件指针资源的泄露，下面提示说有4个文件描述符在退出时仍是打开的。</font></li><li><font class="Apple-style-span" color="#008000">描述符0,1,2无需关心，通过报告，可以发现程序中自己明确打开的文件描述符没有关闭。</font></li><li><font class="Apple-style-span" color="#008000">*/</font></li><li>
==2326== FILE DESCRIPTORS: 4 open at exit.</li><li>
==2326== Open file descriptor 3: test.txt</li><li>
==2326==    at 0x68D613: __open_nocancel (in /lib/libc-2.12.so)</li><li>
==2326==    by 0x61F8EC: __fopen_internal (in /lib/libc-2.12.so)</li><li>
==2326==    by 0x61F94B: fopen@@GLIBC_2.1 (in /lib/libc-2.12.so)</li><li>
==2326==    by 0x8048496: mem_leak2 (in /home/fgao/works/test/a.out)</li><li>
==2326==    by 0x804854E: main (in /home/fgao/works/test/a.out)</li><li>
==2326==</li><li>
==2326== Open file descriptor 2: /dev/pts/4</li><li>
==2326==    <inherited from="" parent=""></inherited></li><li>
==2326==</li><li>
==2326== Open file descriptor 1: /dev/pts/4</li><li>
==2326==    <inherited from="" parent=""></inherited></li><li>
==2326==</li><li>
==2326== Open file descriptor 0: /dev/pts/4</li><li>
==2326==    <inherited from="" parent=""></inherited></li><li>
==2326==</li><li>
==2326==</li><li><font class="Apple-style-span" color="#008000">/* 堆信息的总结：一共调用4次alloc，4次free。之所以正好相等，因为上面有一函数少了free，有一个函数多了一个free */</font></li><li>
==2326== HEAP SUMMARY:</li><li>
==2326==     in use at exit: 353 bytes in 2 blocks</li><li>
==2326==   total heap usage: 4 allocs, 4 frees, 355 bytes allocated</li><li>
==2326==</li><li><font class="Apple-style-span" color="#008000">/* 检测到一个字节的内存泄露 */</font></li><li>
==2326== 1 bytes in 1 blocks are definitely lost in loss record 1 of 2</li><li>
==2326==    at 0x4005BDC: malloc (vg_replace_malloc.c:195)</li><li>
==2326==    by 0x8048475: mem_leak1 (in /home/fgao/works/test/a.out)</li><li>
==2326==    by 0x8048549: main (in /home/fgao/works/test/a.out)</li><li>
==2326==</li><li><font class="Apple-style-span" color="#008000">/* 内存泄露的总结 */</font></li><li>
==2326== LEAK SUMMARY:</li><li>
==2326==    definitely lost: 1 bytes in 1 blocks</li><li>
==2326==    indirectly lost: 0 bytes in 0 blocks</li><li>
==2326==      possibly lost: 0 bytes in 0 blocks</li><li>
==2326==    still reachable: 352 bytes in 1 blocks</li><li>
==2326==         suppressed: 0 bytes in 0 blocks</li><li>
==2326== Reachable blocks (those to which a pointer was found) are not shown.</li><li>
==2326== To see them, rerun with: --leak-check=full --show-reachable=yes</li><li>
==2326==</li><li>
==2326== For counts of detected and suppressed errors, rerun with: -v</li><li>
==2326== Use --track-origins=yes to see where uninitialised values come from</li><li>
==2326== ERROR SUMMARY: 5 errors from 5 contexts (suppressed: 12 from 8)</li></ol></div>



这个只是一个简单的示例程序，即使没有Valgrind，我们也可以很轻易的发现问题。但是在真实的项目中，当代码量达到万行，十万行，甚至百万行时。由于申请的内存可能不是在一个地方使用，不可避免的被传来传去。这时，如果光是看review代码来检查问题，可能很难找到根本原因。这时，使用Valgrind则可以很容易的发现问题所在。

当然，Valgrind也不是万能的。我也遇到过Valgrind无法找到问题，反而我通过不断的review代码找到了症结。发现问题，解决问题，毕竟是末流。最好的方法，就是不引入内存问题。这可以通过良好的代码风格和设计来实现的。

写代码不是那么容易的。要用心，把代码当作自己的作品，真心的去写好它。这样，自然而然的就会把代码写好。

Note：这里我还要再次鄙视GFW，整个儿一个瞎搞！我连Valgrind的主站都打不开了。请问，Valgrind有什么敏感词！闭关锁国！难道让我成天的翻墙！！！

