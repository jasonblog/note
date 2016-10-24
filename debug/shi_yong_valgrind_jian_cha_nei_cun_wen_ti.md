# 使用valgrind檢查內存問題


工欲善其事，必先利其器。Valgrind作為一個免費且優秀的工具包，平時大部分人可能都是使用valgrind檢測內存問題，如內存洩露，越界等。其實Valgrind的用途遠不止於此，其實際上為一個工具包，除了檢查內存問題以外，還有其它多項用途。我準備將其大致介紹一下。本不想再介紹Valgrind檢測內存問題的用法的，但是又一想，畢竟這是Valgrind的一個最有名的用途，如果少了它，不免有些遺憾，所以還是把檢查內存問題作為第一篇吧。

請看一下代碼：

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
這裡一共列出了七種常見的內存問題：1. 動態內存洩露；2. 資源洩露，這裡以文件描述符為例；3. 動態內存越界；4.數組內存越界；5.動態內存double free；6.使用野指針，即未初始化的指針；7.釋放野指針，即未初始化的指針；其中由於本示例代碼過於簡單，第6中情況，使用野指針會直接導致crash，所以在main中，並沒有真正的調用那個示例代碼。由於valgrind只能檢測執行到的代碼，所以在後面的報告中，不會報告第6種錯誤情況。但是，在大型的項目中，有可能使用野指針並不會導致程序crash。另外上面的7中情況，有些情況嚴格的說，實際上可以歸為一類。

下面看怎樣執行valgrind來檢測內存錯誤：

```sh
valgrind --track-fds=yes --leak-check=full --undef-value-errors=yes ./a.out
```
上面那些option的具體含義，可以參加valgrind --help，其中有些option默認就是打開的，不過我習慣於明確的使用option，以示清晰。

看執行後的報告：


<div id="codeText" class="codeText"><ol start="1" class="dp-css"><li>==2326== Memcheck, a memory error detector</li><li>
==2326== Copyright (C) 2002-2009, and GNU GPL'd, by Julian Seward et al.</li><li>
==2326== Using Valgrind-3.5.0 and LibVEX; rerun with -h for copyright info</li><li>
==2326== Command: ./a.out</li><li>
==2326==</li><li><font class="Apple-style-span" color="#008000">/*&nbsp;這裡檢測到了動態內存的越界，提示Invalid write。*/</font></li><li>
==2326== Invalid write of size 2</li><li>
==2326==    at 0x80484B4: mem_overrun1 (in /home/fgao/works/test/a.out)</li><li>
==2326==    by 0x8048553: main (in /home/fgao/works/test/a.out)</li><li>
==2326==  Address 0x40211f0 is 0 bytes inside a block of size 1 alloc'd</li><li>
==2326==    at 0x4005BDC: malloc (vg_replace_malloc.c:195)</li><li>
==2326==    by 0x80484AD: mem_overrun1 (in /home/fgao/works/test/a.out)</li><li>
==2326==    by 0x8048553: main (in /home/fgao/works/test/a.out)</li><li>
==2326==</li></ol><div><span class="Apple-style-span" style="line-height: 15px; "><font class="Apple-style-span" color="#5c5c5c">&nbsp;</font><font class="Apple-style-span" color="#008000">/* 這裡檢測到了double free問題，提示Invalid Free&nbsp;</font></span><span class="Apple-style-span" style="line-height: 15px; "><font class="Apple-style-span" color="#008000">*/</font></span></div><ol start="1" class="dp-css"><li>
==2326== Invalid free() / delete / delete[]</li><li>
==2326==    at 0x40057F6: free (vg_replace_malloc.c:325)</li><li>
==2326==    by 0x8048514: mem_double_free (in /home/fgao/works/test/a.out)</li><li>
==2326==    by 0x804855D: main (in /home/fgao/works/test/a.out)</li><li>
==2326==  Address 0x4021228 is 0 bytes inside a block of size 1 free'd</li><li>
==2326==    at 0x40057F6: free (vg_replace_malloc.c:325)</li><li>
==2326==    by 0x8048509: mem_double_free (in /home/fgao/works/test/a.out)</li><li>
==2326==    by 0x804855D: main (in /home/fgao/works/test/a.out)</li><li>
==2326==</li><li>
</li><li><font class="Apple-style-span" color="#008000">/* 這裡檢測到了未初始化變量 */</font></li><li>
==2326== Conditional jump or move depends on uninitialised value(s)</li><li>
==2326==    at 0x40057B6: free (vg_replace_malloc.c:325)</li><li>
==2326==    by 0x804853C: mem_free_wild_pointer (in /home/fgao/works/test/a.out)</li><li>
==2326==    by 0x8048562: main (in /home/fgao/works/test/a.out)</li><li>
==2326==</li></ol><div><span class="Apple-style-span" style="line-height: 15px; "><font class="Apple-style-span" color="#5c5c5c">&nbsp;</font><font class="Apple-style-span" color="#008000">/* 這裡檢測到了非法是否野指針 */</font></span></div><ol start="1" class="dp-css"><li>
==2326== Invalid free() / delete / delete[]</li><li>
==2326==    at 0x40057F6: free (vg_replace_malloc.c:325)</li><li>
==2326==    by 0x804853C: mem_free_wild_pointer (in /home/fgao/works/test/a.out)</li><li>
==2326==    by 0x8048562: main (in /home/fgao/works/test/a.out)</li><li>
==2326==  Address 0x4021228 is 0 bytes inside a block of size 1 free'd</li><li>
==2326==    at 0x40057F6: free (vg_replace_malloc.c:325)</li><li>
==2326==    by 0x8048509: mem_double_free (in /home/fgao/works/test/a.out)</li><li>
==2326==    by 0x804855D: main (in /home/fgao/works/test/a.out)</li><li>
==2326==</li><li>
==2326==</li><li><font class="Apple-style-span" color="#008000">/*&nbsp;</font></li><li><font class="Apple-style-span" color="#008000">這裡檢測到了文件指針資源的洩露，下面提示說有4個文件描述符在退出時仍是打開的。</font></li><li><font class="Apple-style-span" color="#008000">描述符0,1,2無需關心，通過報告，可以發現程序中自己明確打開的文件描述符沒有關閉。</font></li><li><font class="Apple-style-span" color="#008000">*/</font></li><li>
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
==2326==</li><li><font class="Apple-style-span" color="#008000">/* 堆信息的總結：一共調用4次alloc，4次free。之所以正好相等，因為上面有一函數少了free，有一個函數多了一個free */</font></li><li>
==2326== HEAP SUMMARY:</li><li>
==2326==     in use at exit: 353 bytes in 2 blocks</li><li>
==2326==   total heap usage: 4 allocs, 4 frees, 355 bytes allocated</li><li>
==2326==</li><li><font class="Apple-style-span" color="#008000">/* 檢測到一個字節的內存洩露 */</font></li><li>
==2326== 1 bytes in 1 blocks are definitely lost in loss record 1 of 2</li><li>
==2326==    at 0x4005BDC: malloc (vg_replace_malloc.c:195)</li><li>
==2326==    by 0x8048475: mem_leak1 (in /home/fgao/works/test/a.out)</li><li>
==2326==    by 0x8048549: main (in /home/fgao/works/test/a.out)</li><li>
==2326==</li><li><font class="Apple-style-span" color="#008000">/* 內存洩露的總結 */</font></li><li>
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



這個只是一個簡單的示例程序，即使沒有Valgrind，我們也可以很輕易的發現問題。但是在真實的項目中，當代碼量達到萬行，十萬行，甚至百萬行時。由於申請的內存可能不是在一個地方使用，不可避免的被傳來傳去。這時，如果光是看review代碼來檢查問題，可能很難找到根本原因。這時，使用Valgrind則可以很容易的發現問題所在。

當然，Valgrind也不是萬能的。我也遇到過Valgrind無法找到問題，反而我通過不斷的review代碼找到了癥結。發現問題，解決問題，畢竟是末流。最好的方法，就是不引入內存問題。這可以通過良好的代碼風格和設計來實現的。

寫代碼不是那麼容易的。要用心，把代碼當作自己的作品，真心的去寫好它。這樣，自然而然的就會把代碼寫好。

Note：這裡我還要再次鄙視GFW，整個兒一個瞎搞！我連Valgrind的主站都打不開了。請問，Valgrind有什麼敏感詞！閉關鎖國！難道讓我成天的翻牆！！！

