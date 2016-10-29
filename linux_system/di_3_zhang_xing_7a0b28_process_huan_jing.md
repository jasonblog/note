# 第3章 行程(process)環境

===
:::info
環境設定

Host :
Distributor ID:    Ubuntu
Description:    Ubuntu 15.10
Release:    15.10
Codename:    wily

Target : i386
kernel : 4.8.1
glibc : 2.23
:::
**1.從官網抓buildroot 下來**
###### git clone git://git.buildroot.net/buildroot
**2.推到自己的github**
**2.1 先在自己的github帳號建立一個repository (wayling/buildroot)**
**2.2 改變remote url**
###### git remote set-url origin https://github.com/wayling/buildroot.git
###### git remote -v (檢查一下)
###### git push -u origin master (上傳)
**2.3去github 檢查一下**
https://github.com/wayling/buildroot

**3.使用buildroot**
###### make qemu_x86_defconfig (或是使用我修改過的qemu_study_x86_defconfig)
**3.1 選擇自己喜歡的選項**
###### make menuconfig
gdb/gdbserver/strace 應該是基本款,蠻好用的,可以追glibc實際呼叫的syscall
debug選項記得要開
```
[Build options]->[build packages with debugging symbols]
[Toolchain]->[glibc (2.23)]
[Toolchain] -> [GCC compiler Version (gcc 4.8.x)]
[Build options]-> [strip command for binaries on target (none)]
[Build options]->[build packages with debugging symbols]
[Target packages] -> [Debugging, profiling and benchmark]->[gdb]
[Target packages] -> [Debugging, profiling and benchmark]->[gdbserver]
[Target packages] -> [Debugging, profiling and benchmark]->[full debugger]
[Target packages] -> [Debugging, profiling and benchmark]->[strace]
[Target packages] -> [System tools] -> [htop]
```
**3.2 選擇**
###### make linux-menuconfig
```
[Kernel hacking]->[Kernel debugging]
[Compile-time checks and compiler options]->[Compile the kernel with debug info]
```

**3.3**
###### make all
**3.3.1 自己寫buildroot package 方便自己寫測試code**
可以在buildroot/package/helloworld/HELLOWORLD.mk 看到範例(自己寫的)
只要在自己的PC放置自己的測試code(路徑參考HELLOWORLD_SITE,我寫的範例 https://github.com/wayling/ch3.1-start )
執行build package就會去編譯然後產生到buildroot target
######make helloworld

**3.4 debug userspace 程式需要使用gdbserver**
###### qemu-system-i386 --kernel output/images/bzImage --hda output/images/rootfs.ext2 --append "root=/dev/sda" -net nic,model=virtio -net user  -redir tcp:5556:10.0.2.15:5566
**3.4.1**
登入qemu 裡的帳號 (root)
###### gdbserver 10:0.2.15:5556 ./helloworld

開啟另一個terminal
進入 buildroot/target/root
###### gdb ./helloworld
```
target remote 127.0.0.1:5566
b _start
b main
info sharelibrary (確認share library 的symbol是否載入)
From        To          Syms Read   Shared Object Library
0xb7fdd820  0xb7ff6089  Yes (*)     target:/lib/ld-linux.so.2
c
可以開始debug helloworld
Reading symbols from ./helloworld...done.
(gdb) list
1    #include <stdio.h>
2    
3    int main()
4    {
5            printf("\nMain entry.\n");
6            return 0;
7    }
(gdb) info sharedlibrary
From        To          Syms Read   Shared Object Library
0xb7fdd820  0xb7ff6089  Yes         target:/lib/ld-linux.so.2
(gdb) b _start
Breakpoint 1 at 0x80482e0: file ../sysdeps/i386/start.S, line 61.
(gdb) b main
Breakpoint 2 at 0x80483ec: file main.c, line 5.
(gdb) c
Continuing.
Reading /lib/libc.so.6 from remote target...

Breakpoint 1, _start () at ../sysdeps/i386/start.S:61
61        xorl %ebp, %ebp
(gdb) list
56        .globl _start
57        .type _start,@function
58    _start:
59        /* Clear the frame pointer.  The ABI suggests this be done, to mark
60           the outermost frame obviously.  */
61        xorl %ebp, %ebp
62    
63        /* Extract the arguments as encoded on the stack and set up
64           the arguments for `main': argc, argv.  envp will be determined
65           later in __libc_start_main.  */
(gdb)

```
**3.5 debug kernel ,結合自己寫的範例**
###### qemu-system-i386 --kernel output/images/bzImage --hda output/images/rootfs.ext2 --append "root=/dev/sda" -S -s

開啟另一個terminal
進入buildroot/output/linux-4.8.1
###### gdb ./vmlinux
```
target remote 127.0.0.1:1234
開完機,登入buildroot qemu 
b do_group_exit
c
回到qemu執行helloworld
###### ./helloworld
可以再回到 gdb畫面,應該會停do_group_exit,可以開始動態追蹤了
(gdb) target remote 127.0.0.1:1234
Remote debugging using 127.0.0.1:1234
0x0000fff0 in ?? ()
(gdb) c
Continuing.
^C
Program received signal SIGINT, Interrupt.
native_safe_halt () at ./arch/x86/include/asm/irqflags.h:50
50    }
(gdb) b do_group_exit
Breakpoint 1 at 0xc10486a0: file kernel/exit.c, line 931.
(gdb) c
Continuing.

Breakpoint 1, do_group_exit (exit_code=0) at kernel/exit.c:931
931    {
(gdb) list
926     * Take down every thread in the group.  This is called by fatal signals
927     * as well as by sys_exit_group (below).
928     */
929    void
930    do_group_exit(int exit_code)
931    {
932        struct signal_struct *sig = current->signal;
933    
934        BUG_ON(exit_code & 0x80); /* core dumps don't get here */
935    
(gdb)
```
3.1 main是C程式的開始嗎?
===
Linux上程式執行有很多行為可以探討,要完整串起來不是一件簡單的事
- #### 執行檔程生

編譯 -> 組譯 -> 連結 -> 執行檔(ELF)|
|-

編譯器課本
- #### 作業系統

user space -> kernel space|
|-
可以期待之後的讀書會分享
1. Understanding the Linux Kernel, Third Edition
2. Professional Linux Kernel Architecture
 
- #### 執行檔執行
loader  -> 執行檔(ELF)|
|-

1.程式設計師的自我修煉
2.可以參考jserv的很多課程
https://hackmd.io/s/rJARrHa2

⇒ main function 在user space的執行流程 

何謂程式 ? 
+ 1.binary -> raw,ELF,PE format...
+ 2.code/data segment

第1個範例


手動編譯一下

###### /home/wayling/kernel_study/study/buildroot/output/host/usr/bin/i686-buildroot-linux-gnu-gcc -g -v -o before.exe before.c

:::warning
collect2(gcc tool)
dynamic-linker /lib/ld-linux.so.2(glibc)
crt1.o(glibc) -> (/sysdeps/i386/start.S) 
crti.o(glibc) -> /sysdeps/i386/crti.S
crtbegin.o(gcc[__do_global_dtors_aux])
crtend.o(gcc[__do_global_ctors_aux])
crtn.o(glibc) -> (/sysdeps/i386/crtn.S
ldscript -> (gcc ,output/host/usr/lib/ldscripts/elf_i386.xdw)
:::

我們來檢查一下segment
###### objdump -l ./before.exe
```
Section to Segment mapping:
  Segment Sections...
   00     
   01     .interp 
   02     .interp .note.ABI-tag .hash .dynsym .dynstr .gnu.version .gnu.version_
r .rel.dyn .rel.plt .init .plt .plt.got .text .fini .rodata .eh_frame_hdr .eh_fr
ame 
   03     .ctors .dtors .jcr .dynamic .got .got.plt .data .bss 
   04     .dynamic 
   05     .note.ABI-tag 
   06     .eh_frame_hdr 
   07     
```

###### objdump -s -j .ctors ./before.exe
```
Contents of section .ctors:
 804967c ffffffff e3830408 f7830408 0b840408  ................
 804968c 00000000                             ....            
```
.ctors section 示意圖:

![](http://i.imgur.com/qlABJPO.png)



#### 執行流程

_start () at ../sysdeps/i386/start.S
__libc_start_main at ../csu/libc-start.c
__libc_csu_init ../csu/libc-start.c
_init ../sysdeps/i386/crti.S
__do_global_ctors_aux  at /root/before.exe (from gcc)
_init () at ../sysdeps/i386/crtn.S

![](http://i.imgur.com/YzMKccX.png)

額外補充:
1.compile option有加 "-gp" 就會跑進__gmon_start__

2.frame_dummy是用來傳入ELF的eh_frame

在沒有 Frame Pointer 的情況下進行 Stack Unwind (scott文章)


#### Linux x86 Program Start Up
http://dbp-consulting.com/tutorials/debugging/linuxProgramStartup.html

PEFA -- debug tool
http://www.ropshell.com/peda/Linux_Interactive_Exploit_Development_with_GDB_and_PEDA_Slides.pdf


3.2 "猴賽雷" exit
===

![](http://i.imgur.com/wVXt5gx.png)

寫demo程式去追code
1.test_fork.exe
2.test_pthread.exe
3.test_single.exe


```clike=
void
exit (int status)
{
  __run_exit_handlers (status, &__exit_funcs, true);
}

/* Call all functions registered with `atexit' and `on_exit',
   in the reverse of the order in which they were registered
   perform stdio cleanup, and terminate program execution with STATUS.  */
void
attribute_hidden
__run_exit_handlers (int status, struct exit_function_list **listp,
             bool run_list_atexit)
{
  /* First, call the TLS destructors.  */
#ifndef SHARED
  if (&__call_tls_dtors != NULL)
#endif
    __call_tls_dtors ();

  /* We do it this way to handle recursive calls to exit () made by
     the functions registered with `atexit' and `on_exit'. We call
     everyone on the list and use the status value in the last
     exit (). */
  while (*listp != NULL)
    {
      struct exit_function_list *cur = *listp;

      while (cur->idx > 0)
    {
      const struct exit_function *const f =
        &cur->fns[--cur->idx];
      switch (f->flavor)
        {
          void (*atfct) (void);
          void (*onfct) (int status, void *arg);
          void (*cxafct) (void *arg, int status);

        case ef_free:
        case ef_us:
          break;
        case ef_on:
          onfct = f->func.on.fn;
#ifdef PTR_DEMANGLE
          PTR_DEMANGLE (onfct);
#endif
          onfct (status, f->func.on.arg);
          break;
        case ef_at:
          atfct = f->func.at;
#ifdef PTR_DEMANGLE
          PTR_DEMANGLE (atfct);
#endif
          atfct ();
          break;
        case ef_cxa:
          cxafct = f->func.cxa.fn;
#ifdef PTR_DEMANGLE
          PTR_DEMANGLE (cxafct);
#endif
          cxafct (f->func.cxa.arg, status);
          break;
        }
    }

      *listp = cur->next;
      if (*listp != NULL)
    /* Don't free the last element in the chain, this is the statically
       allocate element.  */
    free (cur);
    }

  if (run_list_atexit)
    RUN_HOOK (__libc_atexit, ());

  _exit (status);
}
```

3.3 atexit介紹
===
3.3.1 使用atexit 
===
3.3.2 atexit的侷限性
===
3.3.3 atexit的實現機制 
===

![](http://i.imgur.com/j027Vjo.png)
```clike=
int
#ifndef atexit
attribute_hidden
#endif
atexit (void (*func) (void))
{
  return __cxa_atexit ((void (*) (void *)) func, NULL,
               &__dso_handle == NULL ? NULL : __dso_handle);
}

int
__cxa_atexit (void (*func) (void *), void *arg, void *d)
{
  return __internal_atexit (func, arg, d, &__exit_funcs);
}
int
attribute_hidden
__internal_atexit (void (*func) (void *), void *arg, void *d,
           struct exit_function_list **listp)
{
  struct exit_function *new = __new_exitfn (listp);

  if (new == NULL)
    return -1;

#ifdef PTR_MANGLE
  PTR_MANGLE (func);
#endif
  new->func.cxa.fn = (void (*) (void *, int)) func;
  new->func.cxa.arg = arg;
  new->func.cxa.dso_handle = d;
  atomic_write_barrier ();
  new->flavor = ef_cxa;
  return 0;
}

struct exit_function *
__new_exitfn (struct exit_function_list **listp)
{
  struct exit_function_list *p = NULL;
  struct exit_function_list *l;
  struct exit_function *r = NULL;
  size_t i = 0;

  __libc_lock_lock (lock);

  for (l = *listp; l != NULL; p = l, l = l->next)
    {
      for (i = l->idx; i > 0; --i)
    if (l->fns[i - 1].flavor != ef_free)
      break;

      if (i > 0)
    break;

      /* This block is completely unused.  */
      l->idx = 0;
    }

  if (l == NULL || i == sizeof (l->fns) / sizeof (l->fns[0]))
    {
      /* The last entry in a block is used.  Use the first entry in
     the previous block if it exists.  Otherwise create a new one.  */
      if (p == NULL)
    {
      assert (l != NULL);
      p = (struct exit_function_list *)
        calloc (1, sizeof (struct exit_function_list));
      if (p != NULL)
        {
          p->next = *listp;
          *listp = p;
        }
    }

      if (p != NULL)
    {
      r = &p->fns[0];
      p->idx = 1;
    }
    }
  else
    {
      /* There is more room in the block.  */
      r = &l->fns[i];
      l->idx = i + 1;
    }

  /* Mark entry as used, but we don't know the flavor now.  */
  if (r != NULL)
    {
      r->flavor = ef_us;
      ++__new_exitfn_called;
    }

  __libc_lock_unlock (lock);

  return r;
}
```
3.4 小心使用環境變數
===
```c=
int setenv(const char *name, const char *value, int overwrite);
int putenv (char *string);

int
putenv (char *string)
{
  const char *const name_end = strchr (string, '=');

  if (name_end != NULL)
    {
      char *name;
#ifdef _LIBC
      int use_malloc = !__libc_use_alloca (name_end - string + 1);
      if (__builtin_expect (use_malloc, 0))
	{
	  name = strndup (string, name_end - string);
	  if (name == NULL)
	    return -1;
	}
      else
	name = strndupa (string, name_end - string);
#else
# define use_malloc 1
      name = malloc (name_end - string + 1);
      if (name == NULL)
	return -1;
      memcpy (name, string, name_end - string);
      name[name_end - string] = '\0';
#endif
      int result = __add_to_environ (name, NULL, string, 1);

      if (__glibc_unlikely (use_malloc))
	free (name);

      return result;
    }

  __unsetenv (string);
  return 0;
}

int
setenv (const char *name, const char *value, int replace)
{
  if (name == NULL || *name == '\0' || strchr (name, '=') != NULL)
    {
      __set_errno (EINVAL);
      return -1;
    }

  return __add_to_environ (name, value, NULL, replace);
}

/* This function is used by `setenv' and `putenv'.  The difference between
   the two functions is that for the former must create a new string which
   is then placed in the environment, while the argument of `putenv'
   must be used directly.  This is all complicated by the fact that we try
   to reuse values once generated for a `setenv' call since we can never
   free the strings.  */
int
__add_to_environ (const char *name, const char *value, const char *combined,
          int replace)
{
  char **ep;
  size_t size;

  /* Compute lengths before locking, so that the critical section is
     less of a performance bottleneck.  VALLEN is needed only if
     COMBINED is null (unfortunately GCC is not smart enough to deduce
     this; see the #pragma at the start of this file).  Testing
     COMBINED instead of VALUE causes setenv (..., NULL, ...)  to dump
     core now instead of corrupting memory later.  */
  const size_t namelen = strlen (name);
  size_t vallen;
  if (combined == NULL)
    vallen = strlen (value) + 1;

  LOCK;

  /* We have to get the pointer now that we have the lock and not earlier
     since another thread might have created a new environment.  */
  ep = __environ;

  size = 0;
  if (ep != NULL)
    {
      for (; *ep != NULL; ++ep)
    if (!strncmp (*ep, name, namelen) && (*ep)[namelen] == '=')
      break;
    else
      ++size;
    }

  if (ep == NULL || __builtin_expect (*ep == NULL, 1))
    {
      char **new_environ;

      /* We allocated this space; we can extend it.  */
      new_environ = (char **) realloc (last_environ,
                       (size + 2) * sizeof (char *));
      if (new_environ == NULL)
    {
      UNLOCK;
      return -1;
    }

      if (__environ != last_environ)
    memcpy ((char *) new_environ, (char *) __environ,
        size * sizeof (char *));

      new_environ[size] = NULL;
      new_environ[size + 1] = NULL;
      ep = new_environ + size;

      last_environ = __environ = new_environ;
    }
  if (*ep == NULL || replace)
    { //putenv
      char *np;

      /* Use the user string if given.  */
      if (combined != NULL)
    np = (char *) combined;
      else
    {
      const size_t varlen = namelen + 1 + vallen;
#ifdef USE_TSEARCH
      char *new_value;
      int use_alloca = __libc_use_alloca (varlen);
      if (__builtin_expect (use_alloca, 1))
        new_value = (char *) alloca (varlen);
      else
        {
          new_value = malloc (varlen);
          if (new_value == NULL)
        {
          UNLOCK;
          return -1;
        }
        }
# ifdef _LIBC
      __mempcpy (__mempcpy (__mempcpy (new_value, name, namelen), "=", 1),
             value, vallen);
# else
      memcpy (new_value, name, namelen);
      new_value[namelen] = '=';
      memcpy (&new_value[namelen + 1], value, vallen);
# endif

      np = KNOWN_VALUE (new_value);
      if (__glibc_likely (np == NULL))
#endif
        {
#ifdef USE_TSEARCH
          if (__glibc_unlikely (! use_alloca))
        np = new_value;
          else
#endif
        {
          np = malloc (varlen);
          if (__glibc_unlikely (np == NULL))
            {
              UNLOCK;
              return -1;
            }

#ifdef USE_TSEARCH
          memcpy (np, new_value, varlen);
#else
          memcpy (np, name, namelen);
          np[namelen] = '=';
          memcpy (&np[namelen + 1], value, vallen);
#endif
        }
          /* And remember the value.  */
          STORE_VALUE (np);
        }
#ifdef USE_TSEARCH
      else
        {
          if (__glibc_unlikely (! use_alloca))
        free (new_value);
        }
#endif
    }

      *ep = np;
    }

  UNLOCK;

  return 0;
}
```
3.5 使用動態函式庫
===
	$(CC) -Wall -g -shared -fPIC dlib.c -o libdlib.so
	$(CC) -Wall -g main.c -o example -L./ -ldlib
	$(CC) -Wall -g main_1.c -o example_dl -L./ -ldlib -ldl
3.5.1 動態與靜態函式庫
===

編譯選項 : -shared -fPIC

3.5.2 編譯與使用動態函式庫 
===
3.5.3 程式的平滑無縫升級 
===
手動載入share object

3.6 避免記憶體問題
===
3.6.1 尷尬的realloc
===
3.6.2 如何防止記憶體越界
===

1. char *strncat(char *dest,const char *src, size_t n);
2. char *strncpy(char *dest, const char *src, size_t n);
3. int snprintf(char *str, size_t size, const char *format, ...);
5. char *fgets(char *s, int size, FILE *stream);

3.6.3 如何定位記憶體問題
===

記憶體問題檢測工具 - valgrind
http://valgrind.org/

3.7 "長轉跳" longjmp
===
3.7.1 setjmp與longjmp的使用
===
3.7.2 "長轉跳"的實現機制
===
glibc/sysdeps/i386

setjmp -> (setjmp.S)
{%gist wayling/8c28dfa15b4c5a9ba08685a18bdec688%}
longjmp -> (__longjmp.S)
{%gist wayling/e3222f707baa46dd7eab345c59b2b994%}
3.7.3 "長轉跳"的限制
===

Bug 1:
{%gist wayling/6015d38a4883625c4bdbcdb056b4242d%}
Bug 2:
{%gist wayling/7fad6f7d24de1740ea04258d780b2059%}

**setjmp/longjmp 應用**
1.coroutine
2.c語言的exception handler
