番外 3： Rust 2018, const fn & inline asm
==========================================

就在昨天 Rust 2018 第一個正式版出來了，距離上次發文也有點時間了，其它功能大部份都在之前已經先介紹過了，不過這次的 `1.31` 版還有個重要的功能，那就是 const fn ，這個 const fn 實際上還不是完整版的，核心團隊決定把目前比較沒爭議，已經可以穩定使用的版本加進來而已，所以不支援的東西其實還挺多的。

const fn 是在編譯時就能預先執行並給出執行結果的函式，這些函式能使用在需要使用常數值的地方，並提供直接寫上常數值一樣的效果，比如在以下情況：

```rust
const fn ret_5() -> usize { 5 }

static FOO: [i32; ret_5()] = [0, 1, 2, 3, 4];

fn main() {
  assert_eq!(FOO.len(), 5);
}
```

陣列的大小必須為一個常數，但因為 `ret_5` 被標記為 `const fn` ，所以它可以在編譯時就被求值，並且當成常數來使用。

> 這就像是 C++ 中的 `constexpr` 一樣

這還有些應用，比如有些 struct 需要在全域時初始化，可是又不想曝露私有的欄位時，原本的作法可能會像這樣：

```rust
mod mymod {
  pub struct Foo {
    inner: i32
  }

  impl Foo {
    #[inline]
    pub fn value(&self) -> i32 {
      self.inner
    }
  }

  pub const FOO_INIT: Foo = Foo { inner: 42 };
}

static FOO: mymod::Foo = mymod::FOO_INIT;

fn main() {
  assert_eq!(FOO.value(), 42);
}
```

這樣雖然解決了問題，但卻也限制了欄位的初始值一定是 42 ，如果有 const fn 的話就可以讓使用者自己決定了：

```rust
mod mymod {
  // ...

  impl Foo {
    pub const fn new(val: i32) -> Self {
      Foo { inner: val }
    }

    // ...
  }
}

static FOO: mymod::Foo = mymod::Foo::new(123);

fn main() {
  assert_eq!(FOO.value(), 123);
}
```

這兩種作法實際上早就被用在標準函式庫中了，比如在 [`std::sync::atomic::AtomicUsize`](https://doc.rust-lang.org/stable/std/sync/atomic/struct.AtomicUsize.html) ，原本設計是使用 [`std::sync::atomic::ATOMIC_USIZE_INIT`](https://doc.rust-lang.org/stable/std/sync/atomic/constant.ATOMIC_USIZE_INIT.html) 來初始化的，不過現在 `AtomicUsize::new` 也是 const fn 了，所以可以直接在全域使用。

> 實際上早在更早之前的版本 `AtomicUsize::new` 就已經是 const fn 了，標準函式庫實際上可以使用那些還沒穩定的功能的

目前的 const fn 支援的東西還不多，基本上只有算數運算、邏輯判斷 (但是沒有 `&&` 與 `||`) 等等的，反正就是沒有條件判斷 (`if`) 與迴圈 (`loop`, `for`, `while`)，之後的版本說不定會有。

另外目前 Rust 的[官網](https://www.rust-lang.org/)換新設計了，請務必去看看。

inline asm
----------

底下的都跟 Rust 2018 沒關係，只是我想談談而已，這是目前還沒穩定的功能，可以讓你直接在程式裡面內嵌組語 (底下的範例都需要使用 nightly 版) ：

```rust
#![feature(asm)]

fn main() {
  let foo: i32;
  unsafe {
    asm! {
      "mov $0, 42"
      : "=r"(foo)
      ::: "intel"
    }
  }
  println!("foo is {}", foo);
}
```

畢竟怎麼想都是不安全的功能，所以包在 `unsafe` 裡是必要的， `asm!` 裡被用 `:` 分成 5 個部份，第一個是組合語言的樣版，為什麼說是樣版是因為像 `$0` 這種東西是會在編譯時被編譯器取代掉的，用什麼取代就是接下來要指定的了， `asm!` 完整的語法是：

```rust
asm! {
  "組語樣版"
  : "要寫入的變數"
  : "要讀取的變數"
  : "被修改的暫存器"
  : "選項"
}
```

其中除了組語樣版是必要的外，其它的部份都可以省略，只是若中間的部份被省略，你依然要用正確數量的 `:` 做區隔，尤於我習慣使用 intel 的語法，因此我在選項的部份都會寫上 `"intel"` 。

> 如果你有在 C\C++ 中使用過內嵌組語，你應該會發現這語法非常的像，因為實際上 Rust 提供的內嵌組語是由 llvm 所提供的功能

寫入與讀取的變數它們的語法都是像 `"[<選項>]<存取方式>"(變數名)`，存取方式的部份一定要指定，其中常用的有：

- r ： 使用暫存器 (register)
- m ： 使用記憶體位置

而選項的部份是可選的，常用的有：

- = ： 代表要寫入

另外這些變數會從 0 開始依序編號，所以像上面的 `foo` 就被給了編號 0 ，並在樣版中由 `$0` 代表，存取方式指定為暫存器，之後才由編譯器取代成實際使用的暫存器。

如果要同時讀寫同一個變數的話，存取方式可以指定為數字，代表跟第幾個變數使用同一種存取方式，像：

```rust
#![feature(asm)]

fn main() {
  let mut foo: i32 = 42;
  let bar: i32;
  unsafe {
    asm! {
      r#"
      mov $1, $0
      mov $0, 123
      "#
      : "=r"(foo), "=r"(bar)
      : "0"(foo)
      :: "intel"
    }
  }
  println!("foo is {}, bar is {}", foo, bar);
}
```

另外變數一樣要遵守 Rust 對於唯讀與可寫的限制，所以 `foo` 還是要加上 `mut` 。

naked_functions
---------------

naked function 是指函式的開頭不會有編譯器產生的程式碼，這在英文中被稱為 prologue (中文是「序」)，這可以讓你完全控制一個函式的組語：

```rust
#![feature(naked_functions, asm)]

#[naked]
fn foo() {
  unsafe {
    asm! {
      "jmp bar"
      :::: "intel"
    }
  }
}

// 我們需要在上面使用 bar 所以不能讓編譯器調整函式的名稱
#[no_mangle]
fn bar() {
  println!("This is bar");
}

fn main() {
  foo();
}
```

不過這功能目前還有些問題，比如不能傳參數，也不能使用堆疊變數 (比如 `let foo = 42`) ，不然就會產生有問題的組語，基本上在裡面除了寫內嵌組語外並不建議做其它事。
