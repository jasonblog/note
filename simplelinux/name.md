[content]: https://github.com/1184893257/simplelinux/blob/master/README.md#content

[回目錄][content]

<a name="top"></a>

<h1 align="center">變量名、函數名
</h1>

　　C程序在執行的時候直接用內存地址去定位變量、函數，
而不是根據名字去搜索，所以C程序執行的速度比腳本語言要快不少。

　　對於函數中的局部變量來說，編譯為彙編的時候，
名字就已經被徹徹底底地忘記了，
因為局部變量在函數幀中，這一幀要佔多少字節，
各局部變量在幀中的相對位置，
都在編譯成彙編的時候就可以確定下來，
生成目標文件、可執行文件的時候也不需要再更改。

　　而 全局變量、static變量、函數 由於要將所有目標文件、
庫鏈接到一起之後才能最終確定它們的絕對地址，
所以在鏈接前名字還是標誌著它們的存在。
它們的信息存儲在符號表（符號數組）中，
其中每一項除了有符號名，還有符號地址（鏈接後填入），
所以 nm 命令可得到 地址-符號名 映射。
雖然程序運行時用不到符號表，
但是默認情況下可執行文件中還是存著符號表，
看下面這個程序（name.c）：

	#include <stdio.h>
	
	int globalvar;
	
	int main()
	{
		static int staticval;
		return 0;
	}

`　　`name.c 中有全局變量、static變量、函數(main)，
查看它編譯後的目標文件、可執行文件的 地址-符號 映射：

	[lqy@localhost notlong]$ gcc -c name.c
	[lqy@localhost notlong]$ nm name.o
	00000004 C globalvar
	00000000 T main
	00000000 b staticval.1672
	[lqy@localhost notlong]$ gcc -o name name.c
	[lqy@localhost notlong]$ nm name | sort
	08048274 T _init
	080482e0 T _start
	08048310 t __do_global_dtors_aux
	08048370 t frame_dummy
	08048394 T main
	...
	此處省略X行
	...
	08049604 b staticval.1672
	08049608 B globalvar
	0804960c A _end
	         U __libc_start_main@@GLIBC_2.0
	         w __gmon_start__
	         w _Jv_RegisterClasses
	[lqy@localhost notlong]$ 

`　　`可執行文件中的 地址-符號 映射還有什麼存在的意義呢？
它可用於彙編級調試的時候設置斷點，
比如linux內核編譯後就生成了 System.map 文件，
便於進行內核調試：

	00000000 A VDSO32_PRELINK
	00000040 A VDSO32_vsyscall_eh_frame_size
	000001d3 A kexec_control_code_size
	00000400 A VDSO32_sigreturn
	0000040c A VDSO32_rt_sigreturn
	00000414 A VDSO32_vsyscall
	00000424 A VDSO32_SYSENTER_RETURN
	01000000 A phys_startup_32
	c1000000 T _text
	c1000000 T startup_32
	c1000054 t default_entry
	c1001000 T wakeup_pmode_return
	c100104c t bogus_magic
	c100104e t save_registers
	c100109d t restore_registers
	c10010c0 T do_suspend_lowlevel
	c10010d6 t ret_point
	c10010e8 T _stext
	c10010e8 t cpumask_weight
	c10010f9 t run_init_process
	c1001112 t init_post
	c10011b0 T do_one_initcall
	...

[回目錄][content]
