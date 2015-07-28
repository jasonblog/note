# Linux Kernel

- 內核預處理後的源文件

```
是隻關心某個特定的.c文件展開後的結果，還是所有的.c文件都關心？

如果是前者，先make V=1查看具體的編譯命令，然後手工在命令行裡執行（需要cd到內核kernel tree），當然加上-E選項了。

如果是後者，修改scripts/Makefile.build裡面的cmd_cc_o_c
cmd_cc_o_c = $(CC) $(c_flags) -c -o $(@D)/.tmp_$(@F) $<
```
