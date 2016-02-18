# 打印寄存器的值




## 技巧
用gdb調試程序時，如果想查看寄存器的值，可以使用“i registers”命令（i是info命令縮寫），例如:  

    (gdb) i registers
    rax            0x7ffff7dd9f60   140737351884640
    rbx            0x0      0
    rcx            0x0      0
    rdx            0x7fffffffe608   140737488348680
    rsi            0x7fffffffe5f8   140737488348664
    rdi            0x1      1
    rbp            0x7fffffffe510   0x7fffffffe510
    rsp            0x7fffffffe4c0   0x7fffffffe4c0
    r8             0x7ffff7dd8300   140737351877376
    r9             0x7ffff7deb9e0   140737351956960
    r10            0x7fffffffe360   140737488348000
    r11            0x7ffff7a68be0   140737348275168
    r12            0x4003e0 4195296
    r13            0x7fffffffe5f0   140737488348656
    r14            0x0      0
    r15            0x0      0
    rip            0x4004cd 0x4004cd <main+9>
    eflags         0x206    [ PF IF ]
    cs             0x33     51
    ss             0x2b     43
    ds             0x0      0
    es             0x0      0
    fs             0x0      0
    gs             0x0      0
以上輸出不包括浮點寄存器和向量寄存器的內容。使用“i all-registers”命令，可以輸出所有寄存器的內容：
	

    (gdb) i all-registers
    	rax            0x7ffff7dd9f60   140737351884640
    	rbx            0x0      0
    	rcx            0x0      0
    	rdx            0x7fffffffe608   140737488348680
    	rsi            0x7fffffffe5f8   140737488348664
    	rdi            0x1      1
    	rbp            0x7fffffffe510   0x7fffffffe510
    	rsp            0x7fffffffe4c0   0x7fffffffe4c0
    	r8             0x7ffff7dd8300   140737351877376
    	r9             0x7ffff7deb9e0   140737351956960
    	r10            0x7fffffffe360   140737488348000
    	r11            0x7ffff7a68be0   140737348275168
    	r12            0x4003e0 4195296
    	r13            0x7fffffffe5f0   140737488348656
    	r14            0x0      0
    	r15            0x0      0
    	rip            0x4004cd 0x4004cd <main+9>
    	eflags         0x206    [ PF IF ]
    	cs             0x33     51
    	ss             0x2b     43
    	ds             0x0      0
    	es             0x0      0
    	fs             0x0      0
    	gs             0x0      0
    	st0            0        (raw 0x00000000000000000000)
    	st1            0        (raw 0x00000000000000000000)
    	st2            0        (raw 0x00000000000000000000)
    	st3            0        (raw 0x00000000000000000000)
    	st4            0        (raw 0x00000000000000000000)
    	st5            0        (raw 0x00000000000000000000)
    	st6            0        (raw 0x00000000000000000000)
    	st7            0        (raw 0x00000000000000000000)
    	......

要打印單個寄存器的值，可以使用“i registers regname”或者“p $regname”，例如：

    (gdb) i registers eax
    eax            0xf7dd9f60       -136470688
    (gdb) p $eax
    $1 = -136470688

參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/Registers.html).

## 貢獻者

nanxiao
