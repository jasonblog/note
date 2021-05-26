Trait 與泛型 (Generic)
======================

Trait
------

Trait 本身同時提供兩個功能，一是讓不同的型態去實作同樣的功能，再來就是提供實作來共用程式碼了，這同時也是 Rust 泛型的基礎。

```rust
trait Movable {
  fn move(&self);
}

struct Human;

impl Movable for Human {
  fn move(&self) {
    println!("Human walk");
  }
}

struct Rabbit;

impl Movable for Rabbit {
  fn move(&self) {
    println!("Rabbit jump");
  }
}
```

於是不同的型別就能各自實作 trait 並提供自己專屬的實作，另外要注意的是： trait 的方法一定都是公開的。

trait 也可以提供預設的實作，與在使用者實作了特定的方法後提供像 mixin 的功能：  

```rust
trait Greeter {
  fn greet(&self) {
    println!("{}", self.message());
  }

  fn greet_to(&self, name: &str) {
    println!("{} {}", self.message(), name);
  }

  fn message(&self) -> &'static str;
}

struct Someone;

impl Greeter for Someone {
  // 提供必要的方法
  fn message(&self) -> &'static str {
    "Hello"
  }

  // 覆寫 (override) 預設實作
  fn greet_to(&self, name: &str) {
    println!("Yo {}", name);
  }
}
```

也可以指定要實作這個 trait 的同時要實作另一個 trait： 

```rust
trait HasName: Greeter {
    fn name(&self) -> &'static str;

    fn greet_with_name(&self) {
        println!("{} my name is {}", self.message(), self.name());
    }
}
```

trait 中也可以宣告型態別名 (type alias)，這樣就能讓方法能輸入或回傳不同型態：

```rust
trait Foo {
  type Item;
  fn foo(&self) -> Self::Item;
}

struct Bar;

impl Foo for Bar {
  type Item = i32;

  fn foo(&self) -> Self::Item { 42 }
}
```

另外這邊我們都使用 `Self` ，因為你無法知道是誰會實作這個 trait。

內建的 Trait
------------

Rust 內建了很多的 trait ，只要實作了這些 trait 就能讓 Rust 知道你的型態能提供哪些功能，也能被標準函式庫或第三方的函式庫使用了，以下會介紹幾個比較重要的。

### Display

[`std::fmt::Display`](https://doc.rust-lang.org/stable/std/fmt/trait.Display.html) 是讓你的型態能被 `println!` 印出來  

```rust
struct Point(i32, i32);

// 當然這邊你可以先用 use std::fmt::Display; 這樣這邊就只需要使用 Display
impl std::fmt::Display for Point {
  fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
    write!(f, "({}, {})", self.0, self.1)
  }
}
```

實作了這個 trait 還會自動實作 [`std::string::ToString`](https://doc.rust-lang.org/stable/std/string/trait.ToString.html) ，這是讓你的型態能轉換成字串。

### From

[`std::convert::From`](https://doc.rust-lang.org/stable/std/convert/trait.From.html) 代表你的型態能從另一個型態轉換，之前所使用的 `String::from` 就是從這裡來的，同時若你實作了 `From` ，編譯器就會自動幫你實作 `Into`，`Into` 則是這個型態可以被轉換成某個固定的型態。  

```rust
struct Foo;
struct Bar;

// Foo 是來源的型態，這是等下要講的泛型
impl From<Foo> for Bar {
  fn from(_: Foo) -> Self {
    Bar
  }
}

// 相對的你可以使用 let bar: Bar = Foo.into();
// 這裡也是少數要標記型態的，因為編譯器沒辦法自動推導
```

### Add

[`std::ops::Add`](https://doc.rust-lang.org/stable/std/ops/trait.Add.html) 可以讓你的型態與別的東西做加法運算，同時這也是 Rust 的運算子重載， Rust 的所有運算子都有個 trait 在 [`std::ops`](https://doc.rust-lang.org/stable/std/ops/index.html)，只要實作了你就能使用那個運算子做運算了。

(這邊不提供範例，請去看文件裡的範例)  

### Deref

[`std::ops::Deref`](https://doc.rust-lang.org/stable/std/ops/trait.Deref.html) 這是 Rust 裡一個很重要的運算子，就是取值的操作，只是這個取值也可以取得其它的型態，這代表著你可以用自己定義的型態去包裝不是由你建立的型態，並擴充它的功能，同時還能自動的「繼承」原先的型態所擁有的方法。

> 這邊的繼承並不像其它語言的繼承，它只是在呼叫方法時透過 `Deref` 轉換成需要的型態而已。

```rust
use std::{ops::Deref, fmt};

#[derive(Copy, Clone)]
struct Num(i32);

impl fmt::Display for Num {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        // 直接呼叫被包裝的 i32 所實作的 fmt::Display
        fmt::Display::fmt(&self.0, f)
    }
}

impl Num {
  fn inc_one(self) -> Self {
    Num(self.0 + 1)
  }
}

impl Deref for Num {
  type Target = i32;

  fn deref(&self) -> &Self::Target {
    &self.0
  }
}

fn main() {
  let n = Num(42);
  println!("{}", n.inc_one()); // n 可以有新定義的方法
  println!("{}", n.abs()); // n 也可以有原本定義的方法
}
```

Derivable Trait
---------------

Derivable trait 是一些能自動 **產生** 實作的 trait ，如果要讓編譯器產生實作的話，就只要在你的型態上加上 `derive` 的標記：

```rust
#[derive(Debug, Default, Clone, Copy)]
struct Foo;
```

> 請注意，它們是 **產生** 實作，這代表它們還是要有程式碼來實作，只是能自動產生而已，通常而言 derivable trait 會要求你的 struct 中每個欄位也都要實作同樣的 trait ，這樣才能遞迴下去

也有不少內建的這樣的 trait ，以下也是介紹重要的

### Default

[`std::default::Default`](https://doc.rust-lang.org/stable/std/default/trait.Default.html) 代表你的型態有預設值，Rust 裡的慣例也有如果實作了無參數的建構子，則也要實作 `Default` ，你也可以在建構子使用實作的 `Default` ，另外 `Default` 也還有個用途：

```rust
#[derive(Default)]
struct Point { x: i32, y: i32, z: i32 };

Point {
  x: 1,
  ..Default::default() // 剩下的值直接使用預設值
}
```

### Debug

[`std::fmt::Debug`](https://doc.rust-lang.org/stable/std/fmt/trait.Debug.html) 是用來印出 debug 資訊的，也就是 `println!` 使用 `{:?}` 印出來的結果。

### Copy

> `Copy` 是個 marker trait ，這類的 trait 其實並沒有任何實作，它們的用途是讓編譯器知道這個型態的一些特性，以及在什麼情況下該怎麼處理。

`Copy` 是代表這個型態可以被簡單的複製，這通常代表你的型態裡只有包含像數字或是布林等型態的資料，如果包含了 `String` 或 `Vec` 就沒辦法實作這個 trait，另外如果有實作 `Copy` 則一定要實作 `Clone`。

### Clone

[`std::clone::Clone`](https://doc.rust-lang.org/stable/std/clone/trait.Clone.html) 是可被複製的型態，如果一個型態只有 `Clone` 而沒有 `Copy` 則通常代表這個型態的複製是需要成本的，比如 `String`，大部份的型態也都有實作 `Clone` ，如果你的型態允許複製也請務必實作 `Clone`，至於沒有實作 `Clone` 的型態基本上就是像 `File` 之類的因為它是對應到了一個實際存在的檔案。  

泛型
----

如果沒有泛型實際上 trait 也沒什麼作用，泛型可以讓一個函式接受不同型態的參數，同時透過指定要實作的 trait 來確保傳進來的參數一定滿足某些必要的條件，比如我想要傳進來的數字可以跟數字相加，而且回傳數字：

```rust
use std::ops::Add;

fn print_add_one<T: Add<i32, Output = i32>>(n: T) {
  println!("{}", n + 1);
}
```

`Add` 本身也是一個泛型的 trait 它的參數是用泛型，並且還帶有一個型態別名，我們可以在 `<>` 中指定泛型的參數，以及型態的別名，第一個 `i32` 指定的是泛型，而 `Output = i32` 指定的則是別名，於是這邊我們就能傳進去任何與 `i32` 相加後會回傳 `i32` 的東西了，你可以傳入數字，也可以試著把上面的 `Num` 加上 `Add` 的定義後傳進去試試， Rust 的編譯器在碰到泛型時會各別的幫出現的每個型態產生程式碼，所以是沒有任何額外的執行消耗的，這也是 Rust 所推的 zero-cost abstract。

泛型還有其它不同的寫法，比如你的型態太長了，那你可以先宣告，再補上 trait 的限制：  

```rust
fn print_add_one<T>(n: T)
  where T: Add<i32, Output = i32> {
  println!("{}", n + 1);
}
```

也可以直接寫在參數的宣告那邊，我比較喜歡這樣寫，這是在 Rust 1.27 後新增的語法：

```rust
fn print_add_one(n: impl Add<i32, Output = i32>) {
  println!("{}", n + 1);
}
```

另外你可以回傳實作了某種 trait 的回傳值，同樣是 1.27 的語法：

```rust
fn return_addable() -> impl Add<i32, Output = i32>) {
  42
}
```

收到這個回傳值的使用者只會知道這個型態支援什麼東西，不會知道實際的型態  

struct 或 trait 也可以使用泛型：

```rust
struct Wrapper<T>{
  inner: T
}

impl<T> Deref for Wrapper<T> {
  type Target = T;
  fn deref(&self) -> &T {
    &self.inner
  }
}

fn main() {
  let n = Wrapper { inner: 42 };
  println!("{}", *n);
}
```

泛型 & 型態別名
--------------------

之前一直沒介紹 type alias 的語法，這語法其實並不是只有在 trait 裡可以使用的，若你覺得某個型態你很常用到但太長了打起來很麻煩時你可以用這個語法來建立一個別名，也可以加上 pub 讓你的別名可以被外部使用：

```rust
pub type MyInt = i32;
```

像標準函式庫中的 [`std::io::Result`](https://doc.rust-lang.org/stable/std/io/type.Result.html) 就是一個很好的例子，它的定義如下：

```rust
type Result<T> = Result<T, Error>;
```

這邊定義了有一個泛型的參數的 `Result` 做為原本的 `Result` 的別名，之後錯誤的型態則是使用 `Error` ，於是程式碼裡就不需要到處都是 `Result<T, Error>` 而只要寫 `Result<T>` 就可以了。

至於在 trait 中何時該用泛型，何時又該用型態別名呢？

大部份的情況下你應該使用型態別名，不過如果你的 trait 要可以針對不同的型態有不同的處理方式：

```rust
struct Handler;

trait Handle<T> {
  fn handle(input: T);
}

impl Handle<i32> for Handler {
  fn handle(input: i32) {
    println!("This is i32: {}", input);
  }
}

impl Handle<f64> for Handler {
  fn handle(input: f64) {
    println!("This is f64: {}", input);
  }
}
```

這時你該使用的是泛型。

下一篇要介紹的是 enum 與模式比對，我個人覺得 Rust 的 enum 很有趣，畢竟和其它語言的不太一樣啊，模式比對也是 Functional Programming 一個重要的特性。
