# 动态链接库的搜索路径


man ld.so(8)说，如果库依赖不包括“/”，那么它将按照下面的规则按顺序搜索：

- （仅对ELF格式）如果可执行文件包含DT_RPATH标签，并且不包含DT_RUNPATH标签，将从DT_RPATH列出的路径搜索。（DT_RPATH已经被废弃，请用DT_RUNPATH）
- 如果LD_LIBRARY_PATH在程序运行时被定义，那么将从它包含的路径开始。安全起见，对于set-user-ID或者set-group-ID的程序，这个变量将被忽略。
- （仅对ELF格式）如果可执行文件包含DT_RUNPATH标签，将从这个标签列出的路径开始搜索。
- 从 /etc/ld.so.cache（运行ldconfig产生）中查找文件
- 从/lib以及/urs/lib，按顺序搜索。如果链接时指定-z nodefaultlib,这个步骤将被忽略。

看起来够简洁的，当做休闲，写个程序验证一下。但在这之前，先介绍一个Glibc扩展的函数（POSIX中没有）

```c
#define _GNU_SOURCE
#include <dlfcn.h>

int dladdr(void* addr, Dl_info *info);
```

这个函数解析传入的函数指针（第一个参数），将信息填充到Dl_info的结构体

```c
typedef struct {
    const char *dli_fname;  /* Pathname of shared object that contains address */
    void       *dli_fbase;  /* Address at which shared object  is loaded */
    const char *dli_sname;  /* Name of nearest symbol with addresslower than addr */
    void       *dli_saddr;  /* Exact address of symbol named in dli_sname */
} Dl_info;
```


下面是程序以及需要加载的动态库的代码：

- ld_main.c

```c
int main()
{
	lib_fun();
	return 0;
}
```

- ld_lib.c

```c
#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
int lib_fun()
{
	Dl_info dl_info;
	dladdr((void*)lib_fun, &dl_info);
	fprintf(stdout, ".so@ %s.\n", dl_info.dli_fname);
	return 0;
}
```
编译这两个文件：
```sh
1、动态库：gcc --shared -fPIC ld_lib.c -o libld_lib.so -ldl
2、主程序：gcc ld_main.c -o ld_main -Wl,-rpath,./  -ldl -lld_lib -L./
```
-Wl,-rpath编译选项将在程序中生成DT_RPATH节点，使用readelf会看到Library rpath被设为当前目录：

![](./images/20131105101102375)

接下来将生成的libld_lib.so拷贝到前面介绍到的搜索路径:<br>
对于LD_LIBRARY_PATH，随便设置：export LD_LIBRARY_PATH=../<br>
对于ld.so.conf提到的路径，在/etc/ld.so.conf.d/下面随便找一个，或者自己建立一个，这里用系统自带的libc.conf<br>
中提到的路径：/usr/local/lib<br>

![](./images/20131105102103015)

然后运行（每次都删除程序优先加载的so文件）：

![](./images/20131105102749656)

（ld.so.conf路径更新文件后需要运行ldconfig更新cache，否则会找不到文件，如上图）。

关于-z nodefaultlib链接选项：


![](./images/20131105103331156)

看来它真起作用了
关于DT_RUNPATH，需要用到--enable-new-dtags链接选项：

![](./images/20131105103810531)

（`Linux下程序默认不会从当前路径搜索.so文件`，这对于自行开发的分为很多模块，要安装在同一目录的“程序”来说不是个优点。还好可以用DT_RUNPATH指定.so的加载路径）