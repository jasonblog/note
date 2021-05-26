設計模式
========

這是第 31 篇，因為鐵人賽已經完賽了，如果有想到什麼我才會再發一篇文章，這篇要來講的是設計模式，但不是平常 OOP 的那些，是一些在 Rust 中常用或有用的模式。

Extension
---------

如果想幫別人的東西加上一些功能，最簡單的方法就是宣告一個 trait ，然後幫那個東西實作了，比如有個 crate 叫 [void](https://github.com/reem/rust-void) 就幫 Result 定義了個 `ResultVoidExt`，我們也可以定義一個 trait 來讓原本的型態回傳我們所包裝的型態 (關於這個包裝的型態下一段會講到)：

```rust
use std::fs::File;

pub struct BetterFile {
  filename: String,
  file: File,
}

// 定義個 trait
pub trait BetterFileExt {
  fn into_better_file(self, filename: &str) -> BetterFile;
}

// 幫我們要包裝的 File 實作
impl BetterFileExt for File {
  fn into_better_file(self, filename: &str) -> BetterFile {
    BetterFile {
      filename: filename.to_owned(),
      file: self,
    }
  }
}
```

另外這也可以用在幫不同的型態提供一個統一的介面，之前也在介紹 macro 時提到過，你可以定義一個 trait 然後幫你需要的型態實作你定義的 trait 。

Deref
-----

這個其實之前有稍微介紹過，不過來講一下實際的應用吧，有時候我們想要擴充一下標準函式庫或別人寫好的東西，最簡單的方法就是上面的方法，定義個 trait 然後幫它實作，但有時候我們也有自己的資料要存，這時候我們就只好包裝原本的型態，並加上我們自己的資料了：

```rust
use std::fs::File;

pub struct BetterFile {
  filename: String,
  file: File,
}

impl BetterFile {
  fn filename(&self) -> &str {
    &self.filename
  }
}
```

這個時候若要讓使用者能使用到被包裝的 struct 所擁有的方法，你可以自己做委派 (delegate) ，或是使用 Deref 讓我們的 struct 能「繼承」原本有的方法 (再強調一次，Rust 並沒有繼承，這邊只是編譯器的自動轉型而已)：

```rust
use std::ops::{Deref, DerefMut};

impl Deref for BetterFile {
  type Target = File;

  fn deref(&self) -> &Self::Target {
    &self.file
  }
}

// 也通常會連 DerefMut 一起實作，這樣使用 &mut self 當參數的方法也能使用
impl DerefMut for BetterFile {
  // DerefMut 繼承 Deref 所以這邊的 Target 是 Deref 的 Target
  fn deref_mut(&mut self) -> &mut Self::Target {
    &mut self.file
  }
}
```

若要包裝其它型態通常也會把那個型態所實作的 trait 也一並做委派：

```rust
use std::io::{self, Write};

impl Write for BetterFile {
  // 提示編譯器可以把這個函式做 inline 來最佳作，委派的方法通常會加
  #[inline]
  fn write(&mut self, buf: &[u8]) -> io::Result<usize> {
    self.file.write(buf)
  }

  #[inline]
  fn flush(&mut self) -> io::Result<()> {
    self.file.flush()
  }
}

// 其實還有 std::io::Read 不過這邊就做個示範而已
```

> 有個 crate 叫 [delegate](https://github.com/chancancode/rust-delegate) 可以讓你在做委派時少寫一點程式碼

實作包裝的型態其實是個很強大的功能，實際上原本 Rust 的 trait 真的太強大了，所以 Rust 對這些東西做了個限制，若你要為某個型態實作 trait ，則必須這個型態是你這個 crate 定義的，或是這個 trait 是你這個 crate 定義的，簡單來說兩邊其中一邊要是屬於你的，但只要實作個包裝的型態，就能把別人的型態當作自己的了，也能隨意的實作 trait 了。

RAII
----

RAII 是 Rust 裡大量應用的一個功能，這可以確保使用者不會忘記釋放資源，這最主要是實作 [`std::ops::Drop`])(https://doc.rust-lang.org/std/ops/trait.Drop.html) 來達成，不過因為這個 trait 太常用了，所以預設都有載入：

```rust
struct Resource;

impl Drop for Resource {
  fn drop(&mut self) {
    println!("呼叫 drop");
  }
}
```

另外有個特別的地方是，大部份的函式都可以直接呼叫，但 `Drop::drop` 一定是由編譯器來呼叫的，這是因為這是個特殊的函式，使用者若要明確的 drop 掉某個變數的話要用 [`std::mem::drop`](https://doc.rust-lang.org/std/mem/fn.drop.html) ，這個函式同樣因為常用所以有預先載入：

```rust
fn main() {
  let resource = Resource;
  drop(resource);
  // 印出 "呼叫 drop"
}
```

RAII 還可以用來代替其它語言裡的 `finally` ，可以用來實作在函式結束前一定要做的操作，比如我們要在函式結束前把 borrow 來的值回復原狀：

```rust
struct Restore<'a, T: 'a + Clone> {
  data: &'a mut T,
  value: T,
}

impl<'a, T: 'a + Clone> Restore<'a, T> {
  fn new(data: &'a mut T) -> Self {
    let value = data.clone();
    Self {
      data,
      value,
    }
  }

  fn set(&mut self, new_val: T) {
    *self.data = new_val;
  }

  fn get(&self) -> &T {
    self.data
  }
}

impl<'a, T: 'a + Clone> Drop for Restore<'a, T> {
  fn drop(&mut self) {
    *self.data = self.value.clone();
  }
}

fn foo(n: &mut i32) {
  let mut n = Restore::new(n);
  n.set(123);
  println!("{}", n.get());
}

fn main() {
  let mut n = 42;
  foo(&mut n);
  println!("{}", n);
}
```

封印 (Seal)
-----------

這是我最近在一些 crate 中看到的，這個 crate 有提供一些 trait ，但作者並不希望這個 trait 被使用者實作在自己的型態上，於是：

```rust
trait Sealed {}

pub trait PublicTrait: Sealed {
  fn foo();
}

pub struct PublicStruct;

impl Sealed for PublicStruct {}

impl PublicTrait for PublicStruct {
  fn foo() {
    println!("foo");
  }
}
```

因為 `Sealed` 是私有的，外部無法存取，所以若想從外部實作 `PublicTrait` 是不可能的，只是這個方法官方目前似乎想將這做為一個編譯錯誤 ([官方 issue #34537](https://github.com/rust-lang/rust/issues/34537))。

再加上之前在第 7 篇介紹的 Builder pattern 就是 Rust 中常用的一些設計模式了。
