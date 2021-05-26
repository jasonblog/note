「不安全」的 Rust
=================

Rust 透過編譯器的檢查來保證記憶體的安全，然而這些檢查並不是完美的，總是有誤判的時候，所以 Rust 也提供了這些被標記為不安全的功能，讓使用者可以直接存取底層的東西，相對的編譯器無法對這個部份提供任何保證。

只要使用 `unsafe` 這個關鍵字就能使用這些不安全的功能，這些功能有：

- 直接存取指標
- 修改全域變數
- 被標記為不安全的方法與 trait

`unsafe` 實際上並沒有關掉 borrow checker ，也不代表裡面的程式碼就一定是不安全的，只是若使用到了這些功能的話，使用者要自己負責保證安全性而已。

指標
-----

指標代表的是記憶體位置，直接操作指標意謂著直接對記憶體操作，同時指標並不受到 Rust 的 borrow checker 的規則所限制，使用者可以自由的修改它所指向的值，也可以有 null 所以使用者要自己做檢查：

> 如果你有寫過 C/C++ 的話，這邊的指標跟它們的是一樣的。

```rust
use std::ptr;

fn main() {
  unsafe {
    // 取得一個可變的 null 指標，一般而言 null 是指向 0 的位置
    // 這邊還是要加 mut 不然 p 會是唯讀的，之後會無法修改 p 本身
    let mut p: *mut i32 = ptr::null_mut();
    // :p 可以用來印出指標本身
    println!("p: {:p}", p);
    // 分配一個存數字的空間後轉換成指標，這邊改了 p 本身，所以需要 mut
    // 這像是 C 裡的 malloc
    p = Box::into_raw(Box::new(42));
    println!("p: {:p}", p);
    // 直接修改指標指向的值
    *p = 123;
    // 指標不受 borrow checker 的限制，同時這邊沒有加 mut
    let q = p;
    println!("q: {:p}", q);
    let val = Box::from_raw(p);
    println!("val: {}", val);
    // 這個指標依然指向 val 的位置，所以可以修改，同時這邊沒有 mut 也能修改
    *q = 42;
    println!("val: {}", val);
  }
}
```

[![Playground][play-btn]](https://play.rust-lang.org/?version=stable&mode=debug&edition=2015&gist=12969560020c938240b570f3998ffe55)

指標的型態有兩種 `*mut` 與 `*const` ，後面則要再加上指向的型態名稱，比如 `*mut i32` ， `*mut` 所指向的內容可以修改， `*const` 則不行，另外 `*mut` 可以直接轉型為 `*const` ，但 `*const` 則要用 `as` 明確的轉型，然而它們都不受 borrow checker 的限制，所以你可以有多個 `*mut` 指向同一個位置也是可以的。

另外也可以直接把 borrow 轉型成 pointer ：

```rust
let val = 42;
let p = &val as *const _;
```

Unsafe Method
-------------

```rust
// 在函式前加上 unsafe 就可以標記這個函式是不安全的
unsafe fn foo () {}

fn main() {
  unsafe {
    // 如果不在 unsafe 裡呼叫的話就會編譯錯誤
    foo();
  }
}
```

不安全的函式或方法通常代表它們需要額外的檢查才能確保它們的使用是安全的，比如像 `Vec::set_len` 這個方法可以直接修改 `Vec` 內容的長度：

```rust
// 如果用 new 的話 Vec 是不會分配空間的
let vec = Vec::<i32>::with_capacity(1);
unsafe {
  vec.set_len(100);
}

// 這邊可以看到 vec 所分配的大小實際上還是只有 1
println!("{}", vec.capacity());
// 然而因為長度已經被設定成 100 了，所以可以看這邊印出了 100 個元素
println!("{:?}", vec);
```

上面這個範例平常可別模仿，這其實已經存取到不該存取的記憶體了，當然這也就是 unsafe 的威力。

extern
------

Rust 中可以很方便的呼叫 C 語言，之後會再來詳細的講解這部份的，不過在 Rust 中因為並不確定這些外部的函式的安全性，所以這些函式都會是 unsafe 的。

```rust
// 直接像這樣宣告外部的函式就好了
extern "C" {
  // atoi 是 C 的標準函式庫中把字串轉換成數字的函式
  fn atoi(s: *const u8) -> i32;
}

fn main() {
  let num = unsafe {
    // C 的字串必須以 0 結尾，同時這邊做轉型成指標
    atoi("42\0".as_bytes().as_ptr())
  };
  println!("{}", num);
}
```

[![Playground][play-btn]](https://play.rust-lang.org/?version=stable&mode=debug&edition=2015&gist=7e22c1d946dcb77cce1524b25ab72a69)

全域變數
--------

因為修改與讀取可修改的全域變數可能會有 data racing 等等的問題，所以 Rust 中將這個視為一個不安全的操作：

```rust
// 定義全域的變數或常數時一定要加上型態
// 全域變數使用全大寫的名稱是個慣例
static mut VAL: i32 = 42;

fn main() {
  unsafe {
    VAL = 123;
  }
  println!("{}", unsafe { VAL });
}
```

順帶介紹個 crate 叫 `lazy_static` ，之後用到時會再做詳細的介紹的，它的功能是建立一個靜態且延遲初始化的變數，也就是像全域變數一樣，但只會在第一次使用時才做初始化，它的 github 在 https://github.com/rust-lang-nursery/lazy-static.rs 。

下一篇我們要來介紹 Rust 2018 有什麼新東西，然後 Rust 的基礎大致上就介紹到這邊了，再接下就進入應用篇，我會開始介紹一些實用的 crate 與實作幾個專案，如果有什麼想要我做的也歡迎提出來。

[play-btn]: https://i.imgur.com/7F0C6a1.png
