# gdb退出時不顯示提示信息


## 技巧
gdb在退出時會提示:  

	A debugging session is active.

        Inferior 1 [process 29686    ] will be killed.

    Quit anyway? (y or n) n


如果不想顯示這個信息，則可以在gdb中使用如下命令把提示信息關掉:

	(gdb) set confirm off

也可以把這個命令加到.gdbinit文件裡。

## 貢獻者

nanxiao

