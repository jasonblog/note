Module
======

模組架構
--------

在 Rust 中要建立一個模組其實不難，只要像這樣：

```rust
mod mymod {
  pub fn print_hello() {
    println!("Hello from mymod");
  }
}

fn main () {
  mymod::print_hello();
}
```

這樣你就有一個模組了，在裡面的東西只要沒有宣告 `pub` 的話外面是不能用的：

```rust
mod mymod {
  fn private() {
    println!("這是私有的函式");
  }
  pub fn public() {
    println!("這是公開的函式");
    private();
  }
}

fn main () {
  // mymod::private(); // 如果取消註解，這邊會出現編譯錯誤
  mymod::public();
}
```

但這樣寫還是一樣是寫在同一個檔案裡，如果要分成不同的檔案的話也很簡單，建一個 `mymod.rs` 跟你的 `main.rs` 放在一起就好了，資料夾下會像這樣：

```plain
src
├── main.rs
└── mymod.rs
```

之後在你的 `main.rs` 加上：

```rust
mod mymod;
```

跟上面不同的是這次後面沒有大括號與裡面的內容，這樣 Rust 就會去找看有沒有 `mymod.rs` 了。

如果想要更複雜一點的話，比如我想要模組下又有子模組呢？這時我們要介紹一個特殊的檔名 `mod.rs` 就像程式開始執行的地方也就是進入點是 `main.rs` 一樣，模組的進入點是 `mod.rs`，我們先在 `main.rs` 旁建一個叫 `mymod` 的資料夾，把原本的 `mymod.rs` 放進去後改名成 `mod.rs` ，於是結構會變成像這樣：

```plain
src
├── main.rs
└── mymod
    └── mod.rs
```

到這邊你可以試著執行看看，結果應該會跟剛才完全一樣。

我們現在可以在 `mymod` 的資料夾加上子模組了，這邊加上一個叫 `submod.rs` 的檔案：

```plain
src
├── main.rs
└── mymod
    ├── mod.rs
    └── submod.rs
```

然後在 `mymod/mod.rs` 中加上：

```rust
mod submod;
```

到這邊子模組已經可以使用了，如果你想讓外部也能使用你的子模組的話，就在 `mod submod` 的前面加上 `pub` 吧：

```rust
pub mod submod;
```

self & super
------------

`self` 也能被使用在引入的路徑中，它代表的是目前的模組，以上面的 `mymod` 舉例，如果要在 `mod.rs` 中使用 `submod` 中的東西可以寫成：

```rust
use self::submod::something;
// 或把完整的路徑寫出來
use mymod::submod::something;
```

`super` 則是代表上層的模組，比如：

```rust
fn hello() {
  println!("Hello");
}

fn main() {
  sub::call_hello();
}

mod sub {
  use super::hello;
  pub fn call_hello() {
    hello();
  }
}
```

這邊也可以注意到，子模組可以直接使用上層模組的東西，不論它有沒有宣告為公開的。

use
---

`use` 用在引入模組、函式、常數、或列舉中的 variant 等等，其實我們已經使用過很多次了，不過如果要從一個模組中引入很多東西時，你可以不用一行一行的寫，可以像這樣：  

```rust
use std::{
  fmt, // 引入子模組
  fs::{File}, // 引入子模組下的 struct
  ops::{Add, Deref}, // 同時引入多個
  io::{self, Read}, // 這行會引入 io 這個模組本身，與在它之下的 Read
};
```

上面的用法你也可以繼續的往下組合，至於可讀性就見仁見智了。

再匯出 (Re-export)
------------------

你可以在 `use` 前面加個 `pub` 把你引入的模組也再從你這個模組匯出：

```rust
mod mymod {
  mod submod {
    pub fn foo() {}
  }

  pub use self::submod::foo;
  
  // 這邊可以使用 foo
}

// 可以從 mymod 引入 foo;
use mymod::foo;
// use mymod::submod::foo; // 這邊拿掉註解會編譯錯誤， submod 是私有的

fn main() {
  foo();
}
```

有些 crate 所提供的 prelude 就是像這樣，把常用的東西全部在從 prelude 下重新匯出：

```rust
mod mymod {
  pub mod submod1 {
    pub fn often_use1() {}
    pub fn often_use2() {}
    pub fn rare_use() {}
  }

  pub mod submod2 {
    pub fn often_use3() {}
  }

  pub mod prelude {
    pub use super::submod1::{often_use1, often_use2};
    pub use super::submod2::often_use3;
  }
}

use mymod::prelude::*;
```

下一篇要來詳細介紹 Rust 的錯誤處理。
