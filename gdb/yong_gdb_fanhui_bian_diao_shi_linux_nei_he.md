# 用GDB反彙編調試linux內核


在搭建好linux內核調試環境之後還會遇到各種問題，比如linux內核是不允許已最低優化等級編譯的，因此有時候打印一個變量值就會顯示   <optimized out>
  這個時候就需要採用其它的方式來顯示變量值了，反彙編就是其中一種方式：

當調試linux模塊初始化函數的時候（http://blog.csdn.net/xsckernel/article/details/8159576）運行到 文件kernel/module.c ，函數add_sect_attrs 第1237行 時


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

打印sattr->address  則會顯示變量已被優化：

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

從反彙編中不難看出此時寄存器esi指向結構體sattr。
下斷：

```sh
(gdb) b *0xc107e01d  
Breakpoint 3 at 0xc107e01d: file kernel/module.c, line 1237.  
```

這個點相當於調用完kstrdup 之後執行的第一條指令，此時eax存放elf 格式的節的名字：

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

