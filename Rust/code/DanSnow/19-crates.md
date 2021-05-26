工具與 Crates
=============

這篇主要是來介紹 Rust 的社群所提供的方便的工具與 crate ，為避免重覆，若之後有打算深入介紹的我就不在這邊做介紹了。

工具
-----

Clippy
------

Clippy 是個 Rust 的 linter (程式碼的風格與錯誤檢查工具) ，可以直接用 `rustup` 安裝：

```shell
$ rustup component add clippy-preview
```

使用也非常簡單，只要在專案裡用以下指令就會做檢查了：

```shell
$ cargo clippy
```

你也可以在 Playground 的右上角找到這個工具：

![Image](https://i.imgur.com/ntFJz2W.png)

它能對像這樣的程式碼提示更好的寫法：

```rust
fn main() {
  let arr = [1, 2, 3, 4];

  if arr.iter().find(|x| x == &&3).is_some() {
    println!("Found 3");
  }

  let mut iter = arr.iter();

  loop {
    match iter.next() {
      Some(x) => println!("{}", x),
      None => break,
    }
  }
}
```

它會產生像這樣的提示：

```plain
warning: called `is_some()` after searching an `Iterator` with find. This is more succinctly expressed by calling `any()`.
 --> src/main.rs:4:8
  |
4 |     if arr.iter().find(|x| x == &&3).is_some() {
  |        ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  |
  = note: #[warn(clippy::search_is_some)] on by default
  = note: replace `find(|x| x == &&3).is_some()` with `any(|x| x == &&3)`
  = help: for further information visit https://rust-lang-nursery.github.io/rust-clippy/v0.0.212/index.html#search_is_some

warning: this loop could be written as a `while let` loop
  --> src/main.rs:10:5
   |
10 | /     loop {
11 | |         match iter.next() {
12 | |             Some(x) => println!("{}", x),
13 | |             None => break,
14 | |         }
15 | |     }
   | |_____^ help: try: `while let Some(x) = iter.next() { .. }`
   |
   = note: #[warn(clippy::while_let_loop)] on by default
   = help: for further information visit https://rust-lang-nursery.github.io/rust-clippy/v0.0.212/index.html#while_let_loop
```

它並不單單是檢查程式碼風格而已，若有更好或更快的寫法也會提供給使用者，是個很方便的工具。

evcxr
-----

evcxr 是個 Rust 的 REPL ，可以讓你方便的測試一些簡單的程式碼，它的 Github 在 https://github.com/google/evcxr ，如果要安裝的話用：

```shell
$ cargo install evcxr_repl
```

然後就可以在終端機下輸入 `evcxr` 來使用了，不過它目前很陽春， `if` 之類的必須輸入在同一樣，也只能用 `Ctrl + D` 關掉 (Windows 下是 `Ctrl + Z` ，不過作者並沒有測試過) 。

Crates
------

先說一下可以去哪邊找 crate ，首先是 [awesome-rust](https://github.com/rust-unofficial/awesome-rust) ，這邊收集了不少的 crate 並做好了分類，再來是 [ergo](https://crates.io/crates/ergo) ，這個專案原本是想收集一些常用的 crate 後整合到一個 crate 裡，不過有段時間沒維護了，但還是可以去看看他收集了哪些，大多都挺好用的。

log
---

`log` 是個由社群提出的標準的 log 介面，它在 https://crates.io/crates/log ，其實它只有 API 而已，本身沒有 log 的功能，如果要使用的話要搭配有實作它的 logger ，最簡單的就是 [env_logger](https://crates.io/crates/env_logger) ，它會根據環境變數決定要顯示哪些 log ，使用起來很簡單：

```rust
#[macro_use]
extern crate log;
extern crate env_logger;

fn main() {
  env_logger::init();

  info!("Hello");
}
```

如果想要自訂性比較高的話可以參考看看 [slog](https://crates.io/crates/slog) ，它提供了可以抽換的各個部份，也有更多的設定，顯示的 log 也比較好看，但相對的也比較複雜點。

num
---

https://crates.io/crates/num

num 是多個 crate 所組成的，它補足了 Rust 中對數字所不支援的部份，比如大數、分數與複數等等的運算，另外還有 `num-iter` 提供了支援設定間隔的數字迭代器， `num-integer` 與 `num-trait` 提供了一些運算。

regex
-----

https://crates.io/crates/regex

regex 提供了正規表示法的支援，至於正規表示法本身可以上網找一下教學，用在字串處理非常的好用。

lazy_static
-----------

https://crates.io/crates/lazy_static

lazy_static 在之前有提過，它提供的是延遲計算的變數，只要在使用到時才會初始化：

```rust
#[macro_use]
extern crate lazy_static;

use std::collections::HashMap;

lazy_static! {
  // 一定要加 ref ，因為為了做到延遲初始化， lazy_static 會用另一個型態包裝
  // 總之記得要加就對了
  static ref MAP: HashMap<&'static str, i32> = {
    // 初始化的程式碼可以不只一行
    let mut map = HashMap::new();
    map.insert("foo", 1);
    map.insert("bar", 2);
    map
  };
}

fn main() {
  lazy_static! {
    // 也可以建一個只能在 main 裡使用的變數
    static ref FOO: i32 = 1;
  }
  assert_eq!(MAP.get("foo").unwrap(), &1);
}
```

lazycell
--------

https://crates.io/crates/lazycell

提供讓你的型態擁有延遲初始化的欄位的功能：

```rust
extern crate lazycell;

use lazycell::LazyCell;

struct Foo {
  lazy_field: LazyCell<i32>,
}

impl Foo {
  pub fn new() -> Self {
    Self { lazy_field: LazyCell::new() }
  }

  pub fn get_lazy(&self) -> &i32 {
    // 若還沒初始化才會執行閉包中的內容做初始化，之後會回傳 borrow
    self.lazy_field.borrow_with(|| {
      42
    })
  }
}
```

derive_more
-----------

https://crates.io/crates/derive_more

提供更多可以用在 derive 的 trait 。

nix 與 libc
-----------

https://crates.io/crates/nix
https://crates.io/crates/libc

libc 提供直接存取底層 API ，而 nix 則是在上面做一層包裝，讓程式碼更安全，不過它們主要都是提供 Linux 下的功能。

下一篇要來介紹怎麼讓 Rust 的程式與 C 的程式做結合，不管是從 Rust 呼叫 C 的程式或是從 C 呼叫 Rust 的程式。
