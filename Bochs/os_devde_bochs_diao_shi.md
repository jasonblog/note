# OS dev的Bochs調試


最近在弄一個自己的hobby OS，作為菜鳥在調試時候積累一些經驗，記錄一下。
##Bochs調試
Bochs自帶調試功能，但是如果你是apt裝上的是不行的，下源碼來自己編譯，編譯選項為：


```sh
./configure --enable-debugger --enable-disasm
```

這個我只是嘗試過，在OS的loader階段可能會用到，當如果進入C語言實現部分的代碼如何調試?我希望看到C的源碼級別調試，而不是彙編的。

##Bochs + gdb調試

同樣需要在編譯的時候加上選項，這個選項必須注意，否則在gdb調試的時候會出現”Cannot find bounds of current function”之類的問題。

```sh
./configure --enable-debugger --enable-disasm --enable-gdb-stub
```

詭異的是這個–enable-gdb-stub選項和上面的 –enable-debugger選項只能`二選一`。也行，編譯出來後重命名吧。編譯完成後在Bochs的配置文件.bashrc中加上這麼一行:

```sh
gdbstub: enabled=1, port=1234, text_base=0, data_base=0, bss_base=0
```

另外注意kernel的代碼也需要加入-g編譯選項。最後在編譯完成後的文件是帶調試信息的，但是我們在用Bochs啟動的img文件不需要這些，現在比如kernel.elf是帶編譯信息的kernel 文件，用下面的這個步驟去掉調試信息，據說也可以用strip來。


```sh
cmd="objcopy -R .pdr -R .comment -R .note -S -O binary kernel.elf kernel.bin"

cat boot.bin setup.bin kernel.bin > ../a.img;
Bochs 使用的是這個a.img文件， gdb載入的是kernel.elf文件。
```

啟動Bochs後會等待gdb連進來(其實Qemu也可以這樣進行調試的)，查資料過程中發現還可在調試的目錄加上.gdbinit，這樣每次啟動gdb就不那麼麻煩了：

```sh
file ./objs/kernel.elf
target remote localhost:1234
set disassembly-flavor intel
b kmain
```

## 一些有用tips

OS的代碼中經常會有內聯彙編，有的時候一條內聯過去就崩潰了，所以在gdb裡需要查看反彙編語句和registers。下面這些gdb指令比較有用：

```sh
(gdb) info line main.c:26  (查看main.c:26行在目標文件中的位置，為0x1cbc) 
Line 26 of "./kernel/main.c" starts at address 0x1cbc <kmain> and ends at 0x1cc2 <kmain+6>.

(gdb) info line *0x1cbc  (上面的反操作)
Line 26 of "./kernel/main.c" starts at address 0x1cbc <kmain> and ends at 0x1cc2 <kmain+6>.


(反彙編kmain函數，箭頭指向的是當前運行的彙編代碼)
(gdb) disas kmain  
Dump of assembler code for function kmain:
=> 0x00001cbc <+0>:	push   ebp
   0x00001cbd <+1>:	mov    ebp,esp
   0x00001cbf <+3>:	sub    esp,0x28
   0x00001cc2 <+6>:	mov    eax,DWORD PTR [ebp+0x8]
   0x00001cc5 <+9>:	mov    ds:0x5ccc,eax
   0x00001cca <+14>:	call   0x2a29 <init_video>
   0x00001ccf <+19>:	mov    DWORD PTR [esp+0x4],0xb
   0x00001cd7 <+27>:	mov    DWORD PTR [esp],0x4777
   0x00001cde <+34>:	call   0x2a40 <puts_color_str>
   0x00001ce3 <+39>:	mov    DWORD PTR [esp+0x4],0xa
   0x00001ceb <+47>:	mov    DWORD PTR [esp],0x478d
   0x00001cf2 <+54>:	call   0x2a40 <puts_color_str>
   0x00001cf7 <+59>:	cli    
   0x00001cf8 <+60>:	call   0x3876 <time_init>
   0x00001cfd <+65>:	call   0xc13 <gdt_init>
```

要正確的看到反彙編最好設置好gdb裡面的彙編指令集，對於Nasm設置”set disassembly-flavor intel”,在.gdbinit裡面弄好就行。
最後info registers查看cpu寄存器內容，info registers %eax只查看eax內容，而 info all-registers會把cpu的所有寄存器內容顯示出來，不過cr0,cr3這些貌似沒有 :(。看看這裡GDB參考。


