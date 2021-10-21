## Day 5：ply 語法簡介



之前只示範了如何用 `ply` 追蹤函數呼叫的堆疊。接下來講解更詳敘的語法，以達成更有效的追蹤。 `ply` 的語法在這個專案的[文件](https://wkz.github.io/ply/ply.1.html) 中可以找到。在這個專案的 [README](https://github.com/iovisor/ply/blob/master/README.md) 中也有一些例子。

## 前言

雖然有文件可以參考，但要注意的是文件有的地方是過時的。比如說裡面有用到 `SyS` 來表示追蹤的是系統呼叫，像是文件中的第一個例子：

```c
kprobe:SyS_*
{
    @syscalls[caller] = count();
}
```

假定這個檔案叫做 `syscall_count.ply`，如果用最新版的 `ply` 執行的話，就會發現下面的錯誤：

```bash
$ sudo ply syscall_count.ply 
ERR:-22 
```

相關的議題可以在這個 [issue#59](https://github.com/iovisor/ply/issues/59) 中看到。除此之外，tracepoint 目前看來也有些問題，可以參考[issue#46](https://github.com/iovisor/ply/issues/46)。不過 kprobe 看起來沒有什麼問題，所以接下來的例子會以 kprobe 為主。

`ply` 執行腳本方式可以直接在命令列用 `ply` 執行，比如說 `ply 'kretprobe:vfs_read { @["size"] = quantize(retval); }`，或是把後面 `'...'` 的部分獨立成一個檔案：

```c
kretprobe:vfs_read
{
    @["size"] = quantize(retval);
}
```

然後叫 `ply` 執行他。比如說假定上面這個檔案叫做 `vfs.ply`，那就直接執行：

```c=
$ ply vfs.ply
```

## 基本架構

基本的架構跟 `awk` 有點像。最基本的樣子是：

```c
kprobe:<name>
{
    /* stuffs to do */
}
```

其中，`<name>` 是想要放 kprobe 的地方，而大括號中間是要做的事情。比如說這個 README 中的例子：

```c
kprobe:do_sys_open
{
    printf("%v(%v): %s\n", comm, uid, str(arg1));
}
```

這個例子會追蹤所有的 `open` 系統呼叫。如果行程呼叫了這個系統呼叫，就會把行程的名稱 (`comm`)、uid (`uid`)，以及這個系統呼叫的第 1 個參數 (按：參數由編號 0 開始，依次往上。也就是 `arg0`、`arg1`、`arg2` ... `argN` 一直下去。為了方便，這邊就直接用「第 N 個參數」來稱呼 `argN`)。

另外一個例子是原來的 [execsnoop.ply](https://github.com/iovisor/ply/blob/master/scripts/execsnoop.ply) 。可以把這個例子稍微修改，使得可以在目前的 Raspberry Pi OS 上運作：

```c=
kprobe:sys_execve {
	execs[kpid] = str(arg0, 48);
}

kretprobe:sys_execve {
	printf("(%4u) %v %3ld\n", uid, execs[kpid], retval);
}
```

這個例子中的 `exec[]` 稱為一個 *map*，是一個 key-value pair，會以 `[]` 中的東西為 key，`=` 右側的東西為 value 儲存。而 *kretprobe* 則是在對應的函數「回傳」的時候被觸發。在使用 kretprobe 時，可以用 `retval` 這個變數來得到回傳值。

## 內建變數

有一些內建的變數可以使用。比如說 `comm` 表示現在這個行程的名稱、`pid` 就表示行程 id、`stack` 表示目前的呼叫堆疊等等。除此之外，可以利用 `arg0`、`arg1` ... `argN` 來表示這個函數中的第 `N` 個參數的值 (`argN` 就表示第 `N` 個參數)。`time` 則是 timestamp，在計算延遲時也滿方便的。

## Aggregations

用來統計結果。用法是 `@[expr]`，其中 `<name>` 是這個統計結果的名稱。這個東西右邊只能有放兩種東西：一個是 `count()`，另外一個是 `quantize(scalar-expr)`。前者就是統計數字，統計每個 `expr` 的結果出現了幾次。比如說統計呼叫某個函數的路徑：

```c=
kprobe:gpiod_*
{
    @[stack] = count();
}
```

這個統計的是「所有名字是 `gpiod_` 開頭的函數被呼叫時，那個堆疊的路徑出現了幾次」：

```c
@:
{ 
	gpiod_set_value_nocheck
	gpio_led_set+120
	gpio_led_set_blocking+24
	set_brightness_delayed+140
	process_one_work+592
	worker_thread+96
	kthread+368
	ret_from_fork+20
 }: 10
{ 
	gpiod_set_raw_value_commit
	gpiod_set_value_cansleep+56
	gpio_led_set+120
	gpio_led_set_blocking+24
	set_brightness_delayed+140
	process_one_work+592
	worker_thread+96
	kthread+368
	ret_from_fork+20
 }: 10
{ 
	gpiod_set_value_cansleep
	gpio_led_set_blocking+24
	set_brightness_delayed+140
	process_one_work+592
	worker_thread+96
	kthread+368
	ret_from_fork+20
 }: 10
```

而後者則是畫出區間分佈圖。比如說下面這個程式

```c
kretprobe:vfs_read
{
    @["size"] = quantize(retval);
}
```

這個就是在統計「`vfs_read` 的回傳值區間分佈圖」。執行完之後結果像下面這樣：

```bash
@:
{ size    }: 
	         < 0	       1 ┤▏                               │
	[   0,    1]	      36 ┤██▌                             │
	[   2,    3]	       3 ┤▎                               │
	[   4,    7]	      15 ┤█                               │
	[   8,   15]	       4 ┤▎                               │
	[  16,   31]	       4 ┤▎                               │
	[  32,   63]	     192 ┤█████████████▎                  │
	[  64,  127]	       1 ┤▏                               │
	[ 128,  255]	       6 ┤▍                               │
	[ 256,  511]	       5 ┤▍                               │
	[ 512,   1k)	      89 ┤██████▏                         │
	[  1k,   2k)	      94 ┤██████▌                         │
	[  2k,   4k)	      14 ┤█                               │
```

## Filter

除了這個之外，也可以只「過濾」出滿足特定條件的事件。這個用法就是在 kprobe 的最後面加上 / ... /：

```c
kprobe:<name>/filter_expr/
{
    /* stuffs to do */
}
```

當 `filter_expr` 為真時，才會觸發接下來的程式片段。這在追蹤特定的可執行檔的行為時很有用。比如說下面這個程式統計名稱為 `hello` 的可執行檔在執行時，進行了哪些系統呼叫：

```c=
kprobe:__se_sys_* / !strcmp(execname, "hello") /
{
    @[caller] = count();
}
```

結果類似以下：

```c
@:
{ __se_sys_exit_group }: 1
{ __se_sys_newuname }: 1
{ __se_sys_access }: 1
{ __se_sys_write }: 1
{ __se_sys_readlink }: 1
{ __se_sys_munmap }: 2
{ __se_sys_brk }: 3
{ __se_sys_openat }: 4
{ __se_sys_close }: 4
{ __se_sys_fstat64 }: 5
{ __se_sys_mprotect }: 6
{ __se_sys_mmap_pgoff }: 8
{ __se_sys_llseek }: 24
{ __se_sys_read }: 26
```

而 `filtr_expr` 也可以是其他 `map`，這樣就可以有「傳接球」的方式來統計特定資料傳遞需要花費的時間。比如說可以拿 `skb` 當作 key，紀錄他到達某一個函數的時間; 然後再去另外一個參數有 `skb` 的函數，看他什麼時候收到這個被其他人收過的 `skb`：

```c
kprobe:__netif_receive_skb_core
{
    rx[arg0] = time;
}

kprobe:ip_rcv / rx[arg0] /
{
    @["diff"] = quantize(time - rx[arg0]);
}
```