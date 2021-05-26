不要用標準函式庫可不可以
========================

Rust 的基本函式庫其實包的東西算很少了，不過還是挺大的，因為 Rust 的標準函式庫並沒有預裝在各個電腦上，所以 Rust 都是直接把必要的部份也直接包含進執行檔裡 (靜態連結 static link)，如果我們看一下基本的 Hello world 有多大的話：

```plain
File  .text     Size     Crate Name
4.6%  62.7% 179.2KiB           [819 Others]
0.2%   3.3%   9.5KiB [Unknown] stats_arena_print
0.2%   3.0%   8.6KiB       std std::sys_common::backtrace::output
0.2%   2.9%   8.4KiB [Unknown] read_line_info
0.2%   2.7%   7.7KiB       std je_stats_print
0.2%   2.7%   7.7KiB       std mallocx
0.2%   2.4%   6.9KiB       std je_arena_boot
0.2%   2.3%   6.5KiB [Unknown] elf_add
0.2%   2.2%   6.4KiB       std backtrace_dwarf_add
0.2%   2.1%   6.1KiB       std je_malloc_vsnprintf
0.1%   1.5%   4.2KiB       std je_arena_palloc
0.1%   1.4%   4.0KiB [Unknown] read_attribute
0.1%   1.4%   3.9KiB       std realloc
0.1%   1.3%   3.8KiB [Unknown] malloc_init_hard_a0_locked
0.1%   1.3%   3.8KiB       std std::sys_common::backtrace::print
0.1%   1.3%   3.8KiB [Unknown] elf_zlib_inflate_and_verify
0.1%   1.1%   3.1KiB       std calloc
0.1%   1.1%   3.1KiB       std malloc
0.1%   1.1%   3.0KiB       std rallocx
0.1%   1.1%   3.0KiB       std je_arena_ralloc_no_move
0.1%   1.0%   2.9KiB       std je_arena_ralloc
7.3% 100.0% 285.6KiB           .text section size, the file size is 3.8MiB
```

> 這是 [cargo-bloat](https://github.com/RazrFalcon/cargo-bloat) 輸出的結果，可以自己安裝來嘗試看看，另外若使用 `strip` 指令去除掉執行檔的除錯資訊，執行檔的大小會下降到 424 KB ，只是這樣 cargo-bloat 就沒辦法印出每個函式佔的大小了。

程式碼的區塊的大小在最後一行，單程式碼的部份就佔了 285.6 KB ，整個執行檔的大小有 3.8 MB ，很大嗎？如果我們用 C 寫一個 Hello world ，執行檔才 8.3 KB 而已，程式碼的部份只有 418 Byte ，程式碼的部份差了約 700 倍啊。

> 這樣比較也不完全正確，因為 C 的標準函式庫都有安裝在系統中，並不像 Rust 是編譯進執行檔中， C 的標準函式庫大小是 2 MB ，但實際上用到的也並沒有全部，若使用靜態連結的話， C 編譯出來的檔案程式的區塊佔了約 573 KB 喔

在上面的輸出可以看到比如像 `backtrace` 、 `je_*` 等等的東西， `backtrace` 是 Rust 在程式發生 panic 時可以印出程式呼叫的流程，而 je 開頭的則是 Rust 預設使用的記憶體分配函式庫，叫 [jemalloc](http://jemalloc.net/) ，就像 C 裡使用的 malloc 一樣， jemalloc 速度較快，同時它盡可能避免在多執行緒時使用鎖定，不過它還挺大的。

如果我們不要用 jemalloc 會怎麼樣呢， Rust 在 1.28 版新增了 `global_allocator` 這個 API 讓我們可以換掉預設使用的記憶體分配器：

```rust
// 系統預設的記憶分配器，簡單來說就是 malloc
use std::alloc::System;

// 設定全域的記憶體分配器，這只能設定一次，如果設定兩次以上會出錯
#[global_allocator]
static GLOBAL: System = System;
```

設定完後的結果：

```plain
File  .text     Size     Crate Name
2.2%  50.4%  62.7KiB           [434 Others]
0.3%   6.9%   8.6KiB       std std::sys_common::backtrace::output
0.3%   6.8%   8.4KiB [Unknown] read_line_info
0.2%   5.2%   6.5KiB [Unknown] elf_add
0.2%   5.1%   6.4KiB       std backtrace_dwarf_add
0.1%   3.2%   4.0KiB [Unknown] read_attribute
0.1%   3.0%   3.8KiB [Unknown] elf_zlib_inflate_and_verify
0.1%   3.0%   3.7KiB       std std::sys_common::backtrace::print
0.1%   1.8%   2.2KiB       std core::str::pattern::StrSearcher::new
0.1%   1.8%   2.2KiB [Unknown] read_function_entry
0.1%   1.6%   1.9KiB       std backtrace_qsort
0.1%   1.3%   1.6KiB [Unknown] add_function_ranges
0.1%   1.3%   1.6KiB [Unknown] dwarf_lookup_pc
0.1%   1.3%   1.6KiB [Unknown] add_unit_ranges
0.1%   1.2%   1.5KiB [Unknown] find_address_ranges
0.0%   1.1%   1.4KiB       std core::fmt::Formatter::pad
0.0%   1.0%   1.3KiB       std std::sys_common::backtrace::output_fileline
0.0%   1.0%   1.3KiB       std std::sys::unix::backtrace::printing::resolve_symname
0.0%   1.0%   1.2KiB       std core::fmt::Formatter::pad_integral
0.0%   1.0%   1.2KiB       std <str as core::fmt::Debug>::fmt
0.0%   1.0%   1.2KiB       std std::sys::unix::backtrace::printing::dladdr::resolve_symname
4.4% 100.0% 124.4KiB           .text section size, the file size is 2.8MiB
```

少了一半以上啊。

> [PR #55238](https://github.com/rust-lang/rust/pull/55238) 官方正在從 jemalloc 換回系統預設的 malloc 喔，如果你用 nightly 版的 `8b096314a 2018-11-02` ，那應該已經包含這個 PR 了，只是我實驗後似乎還是把 jemalloc 編進去了

`#![no_std]`
------------

終於進入這篇的重點了，如果我們不使用標準函式庫會怎麼樣呢？聽起來很不可思議，但其實是可以的， Rust 其實還提供了一個只有最基本的功能的函式庫，叫 [core](https://doc.rust-lang.org/stable/core/index.html) ，基本到什麼程度呢？只有最基礎的型態，所有與記憶體分配或是與系統相關的東西都沒有，比如輸入輸出。

這個函式庫主要用在一些特殊的環境，比如嵌入式系統這種系統架構可能不同的時候，這時候使用者可以自己提供操作系統的 API 綁定，只使用這個最基本的函式庫來寫 Rust 的程式。

那如果我們只使用 core 來寫 Hello world 會變怎麼樣呢？但因為 core 沒有輸入輸出，若我們要在螢幕上顯示東西就必須找其它辦法，其實很簡單，直接呼叫 C 的標準函式庫的東西就好了：

```rust
// 不要使用標準函式庫
#![no_std]
// 不要用 Rust 預設的 main
#![no_main]

// 提供 C 標準函式庫綁定的 crate
extern crate libc;

// 這邊就變的跟 C 的 main 一樣了
#[no_mangle]
pub extern "C" fn main() -> i32 {
    unsafe {
        libc::puts("Hello world\0".as_ptr() as _);
    }
    0
}
```

加入 libc 的相依性時要調一下設定，讓它不要使用到原本的 std ：

```toml
libc = { version = "0.2.43", default-features = false }
```

為什麼不要用預設的 main 呢？有興趣可以看一下這段 [程式碼](https://github.com/rust-lang/rust/blob/master/src/libstd/rt.rs#L32-L67) ， Rust 預設會設定在 panic 發生時印出訊息等等的機制，只是這些我們現在沒辦法提供，就直接用跟 C 一樣的 main 當程式的進入點吧。

若編譯上面那個程式你會看到以下兩個錯誤：

```plain
error: `#[panic_handler]` function required, but not found

error: language item required, but not found: `eh_personality`
```

我們的程式缺少了兩個函式，這兩個函式是在發生特殊情況時由編譯器產生的程式碼去呼叫的，第一個是 panic 發生時呼叫的，用來印出訊息用的，第二個則是提供 panic 發生的 unwind 機制，讓程式能從 panic 的狀態回復而繼續執行下去，先處理第二個，在 `Cargo.toml` 加上這些設定：

```toml
[profile.dev]
panic = "abort"

[profile.release]
panic = "abort"
```

這讓編譯出來的程式在 panic 時直接結束，而不要嘗試 unwind ，這樣就不需要 `eh_personality` 了，第一個要處理也很簡單，提供一個就行了：

```rust
use core::panic::PanicInfo;

#[panic_handler]
fn panic(_: &PanicInfo) -> ! {
    unsafe {
        libc::abort();
    }
}
```

在 panic 發生時直接呼叫 `abort` 結束程式，到這邊我們的程式應該能編譯執行了，來看看編譯後的大小：

```plain
File  .text Size     Crate Name
0.0%   0.0%   0B           [0 Others]
0.7%  45.1% 101B [Unknown] __libc_csu_init
0.4%  23.7%  53B [Unknown] main
0.3%  19.2%  43B [Unknown] _start
0.2%  11.2%  25B       std core::str::<impl str>::as_ptr
0.0%   0.9%   2B [Unknown] __libc_csu_fini
1.6% 100.0% 224B           .text section size, the file size is 13.5KiB
```

程式碼區塊只剩下 224 Byte ，如果用 release 模式的話：

```plain
File  .text Size     Crate Name
0.0%   0.0%   0B           [0 Others]
1.2%  61.6% 101B [Unknown] __libc_csu_init
0.5%  26.2%  43B [Unknown] _start
0.2%  11.0%  18B [Unknown] main
0.0%   1.2%   2B [Unknown] __libc_csu_fini
2.0% 100.0% 164B           .text section size, the file size is 7.9KiB
```

只剩下 164 Byte 了。

目前 `no_std` 最常被使用到的地方應該是 crate 了，在 crates.io 上還有個 [專門的分類](https://crates.io/categories/no-std) ，這些 crate 提供使用者在特殊的環境下也能使用的功能，通常也都是完全不牽涉到作業系統相關的東西的。

另外 Rust 的標準函式庫還有個叫 [alloc](https://doc.rust-lang.org/stable/alloc/index.html) ，目前還不穩定，它提供的是與記憶體分配相關的，比如像智慧指標 `Box` ，或是 Rust 中的 `Vec` 等等。

不過除非你是想在特殊的環境下使用 Rust ，或是你做了個跟作業系統完全無關的 crate ，不然還是用 std 來的方便，不需要為了想幫 Rust 的程式瘦身就不用 std 。
