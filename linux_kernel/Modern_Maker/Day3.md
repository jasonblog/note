## Day 3：安裝 ply 和 perf



## eBPF 極簡介

eBPF 是一個核心中的虛擬機器，有一個自己的指令集。所以最直觀的觀點來說，要使用 eBPF 就是要想辦法編出一個這個虛擬機可以執行的程式。不過，事情並沒有那麼簡單。畢竟這個虛擬機執行在核心裡面，必須審慎處理。因此，這些 eBPF 虛擬機的程式會經過一個 *verifier* 來確認有沒有問題。

除了暴力手刻 eBPF 虛擬機的組合語言之外，另外一個方法就是 (又是) 交給編譯器。比如說 [LLVM](https://qmonnet.github.io/whirl-offload/2020/04/12/llvm-ebpf-asm/) 就可以做到這件事情。把編出來的 eBPF 虛擬機可執行的程式「注入」給 eBPF 就可以執行。

而比較高階的工具有 [bcc](https://github.com/iovisor/bcc)，比較特別的地方是提供了 Python 的 frontend; 而另外一個工具是 [bpftrace](https://github.com/iovisor/bpftrace) ，他規範了另外一個腳本語言來使用 eBPF，並且有許多「一行文」式的[程式](http://www.brendangregg.com/blog/2019-01-01/learn-ebpf-tracing.html)。

上面這兩個工具都在執行時期需要 LLVM，而且目前沒有辦法在 32-bit 的 Raspberry Pi OS 上成功執行。不管是用套件管理軟體安裝，或是自行編譯 BCC，執行 `tools` 資料夾裡面的程式時都出現類似以下的錯誤訊息：

```bash
$ ./runqlen.py 
In file included from <built-in>:3:
In file included from /virtual/include/bcc/helpers.h:53:
In file included from include/linux/log2.h:12:
In file included from include/linux/bitops.h:26:
In file included from arch/arm/include/asm/bitops.h:243:
In file included from include/asm-generic/bitops/lock.h:5:
In file included from include/linux/atomic.h:7:
In file included from arch/arm/include/asm/atomic.h:16:
arch/arm/include/asm/cmpxchg.h:128:2: error: "SMP is not supported on this platform"
#error "SMP is not supported on this platform"

[...]

In file included from <built-in>:3:
In file included from /virtual/include/bcc/helpers.h:54:
In file included from arch/arm/include/asm/page.h:160:
arch/arm/include/asm/memory.h:220:26: error: value '2164260864' out of range for constraint
      'I'
                __pv_stub(x, t, "add", __PV_BITS_31_24);
                                       ^~~~~~~~~~~~~~~
arch/arm/include/asm/memory.h:175:25: note: expanded from macro '__PV_BITS_31_24'
#define __PV_BITS_31_24 0x81000000
                        ^~~~~~~~~~
arch/arm/include/asm/memory.h:193:21: note: expanded from macro '__pv_stub'
        : "r" (from), "I" (type))
                           ^~~~
arch/arm/include/asm/memory.h:222:3: error: value '129' out of range for constraint 'I'
                __pv_stub_mov_hi(t);
                ^~~~~~~~~~~~~~~~~~~
arch/arm/include/asm/memory.h:202:9: note: expanded from macro '__pv_stub_mov_hi'
        : "I" (__PV_BITS_7_0))
               ^~~~~~~~~~~~~
arch/arm/include/asm/memory.h:176:23: note: expanded from macro '__PV_BITS_7_0'
#define __PV_BITS_7_0   0x81
                        ^~~~
arch/arm/include/asm/memory.h:223:3: error: value '2164260864' out of range for constraint 'I'
                __pv_add_carry_stub(x, t);
                ^~~~~~~~~~~~~~~~~~~~~~~~~
arch/arm/include/asm/memory.h:212:18: note: expanded from macro '__pv_add_carry_stub'
        : "r" (x), "I" (__PV_BITS_31_24)                \
                        ^~~~~~~~~~~~~~~

[...]
```

而要用 `snap` 安裝 `bpftrace` 套件的時候，則會出現不支援的提示：

```bash
$ sudo snap install bpftrace
error: snap "bpftrace" is not available on stable for this architecture (armhf) but exists on
       other architectures (amd64, arm64, ppc64el, s390x).
```

另外一個比較輕量的專案是 [ply](https://github.com/iovisor/ply)。根據他的專案簡介，執行時期的函式庫只有 `glibc`，不像前面需要非常肥的 LLVM。因此下面的介紹就以 `ply` 為主。

除了 `ply` 之外，`perf` 也是觀察的好工具，但是套件管理軟體似乎僅有 `eprf_4.9`。所以以下也會把在 Raspberry Pi OS 上的 `perf` 編譯的流程。

## Step 1：編譯 ply

```bash
$ sudo apt install autogen autoconf libtool
```

接著下載 [ply](https://github.com/iovisor/ply#build-and-installation)：

```bash
$ git clone https://github.com/iovisor/ply.git
```

並且依照 [readme](https://github.com/iovisor/ply#build-and-installation) 的方式來編譯：

```bash
$ cd ply
$ ./autogen.sh 
$ ./configure
$ make
$ sudo make install
```

在安裝的過程中，可能會出現像下面這樣的輸出。意思是

```bash
[...]
libtool: finish: PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/sbin" ldconfig -n /usr/local/lib
----------------------------------------------------------------------
Libraries have been installed in:
   /usr/local/lib

If you ever happen to want to link against installed libraries
in a given directory, LIBDIR, you must either use libtool, and
specify the full pathname of the library, or use the '-LLIBDIR'
flag during linking and do at least one of the following:
   - add LIBDIR to the 'LD_LIBRARY_PATH' environment variable
     during execution
   - add LIBDIR to the 'LD_RUN_PATH' environment variable
     during linking
   - use the '-Wl,-rpath -Wl,LIBDIR' linker flag
   - have your system administrator add LIBDIR to '/etc/ld.so.conf'

See any operating system documentation about shared libraries for
more information, such as the ld(1) and ld.so(8) manual pages.
----------------------------------------------------------------------
make[4]: Nothing to be done for 'install-data-am'.
make[4]: Leaving directory '/home/pi/ply/src/libply'
[...]
```

編輯 `/etc/ld.so.conf`：

```bash
$ vim /etc/ld.so.conf
```

把裡面加上 `/usr/local/lib`：

```bash
include /etc/ld.so.conf.d/*.conf
+/usr/local/lib
```

然後執行 `ldconfig`

```bash
$ sudo ldconfig
```

## Step 2：測試 ply

```bash
$ sudo ply 'kretprobe:vfs_read { @["size"] = quantize(retval); }'
```

會出現 `ply: active` 的字樣：

```bash
$ sudo ply 'kretprobe:vfs_read { @["size"] = quantize(retval); }'
ply: active
```

等待一段時間之後，按下 ctrl + c 終止程式，就會發現對應的統計結果：

```bash
^Cply: deactivating

@:
{ size    }: 
	[   0,    1]	       3 ┤█████████▋                      │
	[   2,    3]	       1 ┤███▎                            │
	...
	[   8,   15]	       2 ┤██████▍                         │
	[  16,   31]	       1 ┤███▎                            │
	[  32,   63]	       1 ┤███▎                            │
	...
	[ 256,  511]	       1 ┤███▎                            │
	...
	[  1k,   2k)	       1 ┤███▎                            │
```

雖然沒有特別解釋這個命令的功能，但可以從他裡面的東西大致猜到：這個程式跟虛擬檔案系統 (VFS) 有關，而且他看起來他想統計的東西 `vfs_read` 這個回傳值有關。確實，這是一個「統計 `read` 系統呼叫的回傳值分佈」的一個命令。

## Step 3：編譯 perf

套件管理軟體看起來只有 `perf_4.9`，而現在的核心是 5.4，所以重新編譯。首先安裝相依套件：

```shell
$ sudo apt-get -y install flex bison libdw-dev libnewt-dev binutils-dev libaudit-dev \
  binutils-dev libssl-dev python-dev systemtap-sdt-dev libiberty-dev libperl-dev \ 
  liblzma-dev libpython-dev libunwind-* asciidoc xmlto
```

然後編譯 `perf`。進入 linux 核心的資料夾中，然後編譯：

```bash
$ make -C tools/perf/
```

## Step 4：測試 perf

編譯完之後，`perf` 會位在 `tools/perf/perf`。試著看看可不可以列出事件：

```bash
$ ./tools/perf/perf list
```

預期出現以下的輸出：

```bash
List of pre-defined events (to be used in -e):

  branch-instructions OR branches                    [Hardware event]
  branch-misses                                      [Hardware event]
  bus-cycles                                         [Hardware event]
  cache-misses                                       [Hardware event]
  cache-references                                   [Hardware event]
  cpu-cycles OR cycles                               [Hardware event]
  instructions                                       [Hardware event]

[...]
```

這個清單中，所列出的是各種 `perf` 可以統計的事件。比如說硬體的 `cache-misses`，以及各種核心中靜態的 *tracepoint* 等等。雖然滿常時候 `perf` 是作為一個觀察效能的工具，但這有時候也會對理解程式的行為有所幫助。舉例來說， `perf record`，加上一些參數，去觀察一個行程執行過程中，在哪些函數中花費的時間比較久，以及各種函數的堆疊關係 (flame graph)。在不知道該用 eBPF 觀察什麼東西時，可以用 `perf` 先對程式中各個函數之間如何相互影響有個概覽，然後再挑出有趣的地方用 eBPF 追蹤。