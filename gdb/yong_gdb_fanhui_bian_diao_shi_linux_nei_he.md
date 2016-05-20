# 用GDB反汇编调试linux内核


在搭建好linux内核调试环境之后还会遇到各种问题，比如linux内核是不允许已最低优化等级编译的，因此有时候打印一个变量值就会显示   <optimized out>
  这个时候就需要采用其它的方式来显示变量值了，反汇编就是其中一种方式：

当调试linux模块初始化函数的时候（http://blog.csdn.net/xsckernel/article/details/8159576）运行到 文件kernel/module.c ，函数add_sect_attrs 第1237行 时


```c

for (i = 0; i < info->hdr->e_shnum; i++)
{
    Elf_Shdr* sec = &info->sechdrs[i];

    if (sect_empty(sec)) {
        continue;
    }

    sattr->address = sec->sh_addr;
    sattr->name = kstrdup(info->secstrings + sec->sh_name,
                          GFP_KERNEL);

    if (sattr->name == NULL) {
        goto out;
    }

    sect_attrs->nsections++;
    sysfs_attr_init(&sattr->mattr.attr);
    sattr->mattr.show = module_sect_show;
    sattr->mattr.store = NULL;
    sattr->mattr.attr.name = sattr->name;
    sattr->mattr.attr.mode = S_IRUGO;
    *(gattr++) = &(sattr++)->mattr.attr;
}
```

打印sattr->address  则会显示变量已被优化：

```sh
(gdb) p sattr ->address   
value has been optimized out
```

```sh
(gdb) p $eip
$2 = (void (*)()) 0xc107e00e <sys_init_module+5089>

(gdb) disassemble /r 0xc107e00e,0xc107e04e
Dump of assembler code from 0xc107e00e to 0xc107e04e:
=> 0xc107e00e <sys_init_module+5089>:	8b 45 bc	mov    -0x44(%ebp),%eax
   0xc107e011 <sys_init_module+5092>:	03 02	add    (%edx),%eax
   0xc107e013 <sys_init_module+5094>:	ba d0 00 00 00	mov    $0xd0,%edx
   0xc107e018 <sys_init_module+5099>:	e8 50 9a 06 00	call   0xc10e7a6d <kstrdup>
   0xc107e01d <sys_init_module+5104>:	89 46 1c	mov    %eax,0x1c(%esi)
   0xc107e020 <sys_init_module+5107>:	85 c0	test   %eax,%eax
   0xc107e022 <sys_init_module+5109>:	74 5e	je     0xc107e082 <sys_init_module+5205>
   0xc107e024 <sys_init_module+5111>:	83 47 0c 01	addl   $0x1,0xc(%edi)
   0xc107e028 <sys_init_module+5115>:	c7 46 08 95 b6 07 c1	movl   $0xc107b695,0x8(%esi)
   0xc107e02f <sys_init_module+5122>:	c7 46 0c 00 00 00 00	movl   $0x0,0xc(%esi)
   0xc107e036 <sys_init_module+5129>:	8b 46 1c	mov    0x1c(%esi),%eax
   0xc107e039 <sys_init_module+5132>:	89 06	mov    %eax,(%esi)
   0xc107e03b <sys_init_module+5134>:	66 c7 46 04 24 01	movw   $0x124,0x4(%esi)
   0xc107e041 <sys_init_module+5140>:	8b 45 ac	mov    -0x54(%ebp),%eax
   0xc107e044 <sys_init_module+5143>:	89 30	mov    %esi,(%eax)
   0xc107e046 <sys_init_module+5145>:	83 c0 04	add    $0x4,%eax
   0xc107e049 <sys_init_module+5148>:	89 45 ac	mov    %eax,-0x54(%ebp)
   0xc107e04c <sys_init_module+5151>:	83 c6 24	add    $0x24,%esi
End of assembler dump.
```

从反汇编中不难看出此时寄存器esi指向结构体sattr。
下断：

```sh
(gdb) b *0xc107e01d  
Breakpoint 3 at 0xc107e01d: file kernel/module.c, line 1237.  
```

这个点相当于调用完kstrdup 之后执行的第一条指令，此时eax存放elf 格式的节的名字：

```sh
(gdb) b *0xc107e01d  
Breakpoint 3 at 0xc107e01d: file kernel/module.c, line 1237.  
(gdb) c  
Continuing.  
  
Breakpoint 3, 0xc107e01d in add_sect_attrs (info=0xdf705f5c, mod=0xe0839060)  
    at kernel/module.c:1237  
1237            sattr->name = kstrdup(info->secstrings + sec->sh_name,  
(gdb) p $eip  
$3 = (void (*)()) 0xc107e01d <sys_init_module+5104>  
(gdb) p (char *)$eax  
$5 = 0xdf690840 ".note.gnu.build-id" 
```

```sh
(gdb) ni  
1239            if (sattr->name == NULL)  
(gdb) p $eip  
$12 = (void (*)()) 0xc107e020 <sys_init_module+5107>  
(gdb) p ((struct module_sect_attr *)$esi)->name  
$15 = 0xdf690840 ".note.gnu.build-id"  
(gdb) p /x ((struct module_sect_attr *)$esi)->address  
$19 = 0xe0838000  
```

