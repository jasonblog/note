從 C 呼叫 Rust
==============

這次的程式碼在 https://github.com/DanSnow/rust-intro/tree/master/clib

建立可以給 C 使用的函式庫
-------------------------

Rust 跟 C 真的是個很合的語言，要從 C 呼叫 Rust 的程式也很簡單，這次我們來建立一個之前都沒有使用過的函式庫專案：

```shell
$ cargo init --lib clib
```

然後修改 `Cargo.toml` 加上一段：

```toml
[lib]
crate-type = ["staticlib"]
```

這代表我們要 cargo 建置出可以用來做靜態連結的函式庫，接著來準備個給 C 使用的函式吧，打開 `lib.rs` 輸入：

```rust
use std::{ffi::CStr, os::raw::c_char};

// 讓編譯器不要修改函式的名稱
#[no_mangle]
// 為了讓函式能夠被 C 呼叫，這邊要加上 extern "C"
pub extern "C" fn say_hello(message: *const c_char) {
  // 包裝 C 的字串成 Rust 的 CStr ，這樣才方便被 Rust 處理
  let message = unsafe { CStr::from_ptr(message) };
  // to_str 會轉換 CStr 成 str ，但如果字串不是合法的 utf-8 編碼就會回傳 Err
  println!("{} from Rust", message.to_str().unwrap());
}
```

> Rust 的編譯器會修改函式名稱，加上模組等資訊來避免出現重覆的名稱，另外 Rust 的調用約定 (calling convention) 也與 C 不同，因此必須加上 `extern "C"` 讓 Rust 使用 C 的調用約定，這樣我們才能直接在 C 使用這些函式。

接著我們來寫 C 的程式吧：

```c
void say_hello(const char *message);

int main() {
  say_hello("Hello");
}
```

這次我們一樣為了簡化編譯的過程把這部份寫成了一個 `Makefile` ，可以自己打開來看看，執行的話應該會看到：

```plain
Hello from Rust
```

傳遞 struct
-----------

若要把 Rust 的 struct 給 C 使用的話：

```rust
// 加上 repr(C) 可以讓 Rust 的型態具有跟 C 一樣的記憶體結構
#[repr(C)]
pub struct Point {
  x: i32,
  y: i32,
}

#[no_mangle]
pub extern "C" fn create_point(x: i32, y: i32) -> Point {
  Point { x, y }
}
```

C 這邊若要使用：

```c
typedef struct _Point {
  int x;
  int y;
} Point;

Point create_point(int x, int y);

create_point(10, 20);
```

分配記憶體
----------

如果要在 Rust 裡分配個記憶體並傳給 C 用的話我們可以使用 `Box` ：

```rust
#[no_mangle]
pub extern "C" fn alloc_memory() -> *mut i32 {
  // 將 Box 轉換成 C 的指標
  Box::into_raw(Box::new(42))
}

#[no_mangle]
pub extern "C" fn free_memory(x: *mut i32) {
  // 從指標建立回 Box ，這樣才能讓 Rust 知道怎麼回收這塊記憶體
  // 這邊使用 drop 明確的清掉這個 Box
  // 不過 Rust 其實也會在 Box 離開有效範圍時自動清掉，所以也不一定要這樣做
  drop(unsafe { Box::from_raw(x) });
}
```

C 的部份：

```c
// 補充一個 C 語言的小知識，若函式的宣告中沒放東西代表的是傳什麼都可以
// 所以我都會習慣在沒有參數時放 void
int *alloc_memory(void);
void free_memory(int *x);

int *x = alloc_memory();
// 這邊可以使用這個變數
*x = 123;
// 記得把空間交回給 Rust 清除
free_memory(x);
```

> 務必讓 Rust 清理記憶體， Rust 預設並不是使用 malloc 與 free ，若用 free 來清理是會出問題的，再者，只有 Rust 知道那個型態有沒有其它需要釋放的資源。

傳 Vec
------

Rust 的 `Vec` 真的很方便，可以自動的成長，做為陣列使用就不用擔心空間不夠的問題 (除非你的環境的記憶體很珍貴) ，如果要傳遞給 C 使用的話要怎麼辦呢：

```rust
use std::mem;

#[no_mangle]
// 這邊多使用了一個參數，用來回傳長度
pub extern "C" fn create_vec(size: *mut usize) -> *mut i32 {
  let mut vec = Vec::new();
  // 假設做了些工作來產生這個 Vec
  vec.push(1);
  vec.push(2);
  vec.push(3);

  // 讓 Vec 的容量與實際大小一樣
  vec.shrink_to_fit();
  // 一般來說都會一樣，不過這並沒有保證，詳細可以看一下文件
  // 這邊用 assert 確保這種情況不會出現
  assert!(vec.capacity() == vec.len());
  // 回傳大小
  unsafe { *size = vec.len() };
  // 取得指標
  let p = vec.as_mut_ptr();
  // 這讓 vec 不會被 Rust 清除
  mem::forget(vec);
  p
}

#[no_mangle]
pub extern "C" fn free_vec(vec: *mut i32, size: i32) {
  drop(unsafe { Vec::from_raw_parts(vec, size, size) });
}
```

在 C 中使用：

```c
int *create_vec(size_t *size);
void free_vec(int *vec, size_t size);

size_t size;
int *vec = create_vec(&size);
for (size_t i = 0; i < size; ++i) {
  printf("%d ", vec[i]);
}
// 換行
puts("");
free_vec(vec, size);
```

自動產生 C 的標頭檔
-------------------

前面我們都自己宣告 C 的函式，這次讓程式來自動幫我們產生標頭檔吧，首先加上 `cbindgen` ：

```shell
$ cargo add cbindgen --build
```

然後同樣的我們需要 `build.rs` ：

```rust
extern crate cbindgen;

use std::env;

fn main() {
    let crate_dir = env::var("CARGO_MANIFEST_DIR").unwrap();

    cbindgen::generate(crate_dir)
        .expect("Unable to generate bindings")
        // 寫到 bindings.h 這個檔案
        .write_to_file("bindings.h");
}
```

接著我們需要一個設定檔：

```toml
# 不設定的話預設會是 C++
language = "C"
# 設定 C 的 struct 要不要加 typedef
style = "Both"
# 設定 include guard
include_guard = "INCLUDE_BINDINGS_H"
```

再跑一次 `cargo build` 應該就會看到它產生一份這樣的標頭檔了：

```c
#ifndef INCLUDE_BINDINGS_H
#define INCLUDE_BINDINGS_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Point {
  int32_t x;
  int32_t y;
} Point;

int32_t *alloc_memory(void);

Point create_point(int32_t x, int32_t y);

int32_t *create_vec(uintptr_t *size);

void free_memory(int32_t *x);

void free_vec(int32_t *vec, uintptr_t size);

void say_hello(const char *message);

#endif /* INCLUDE_BINDINGS_H */
```

這樣產生宣告的部份就自動化了，我們的 C 的部份只要負責呼叫就好了，可喜可賀。

下一篇我們來做個 python 的 native extension 吧。
