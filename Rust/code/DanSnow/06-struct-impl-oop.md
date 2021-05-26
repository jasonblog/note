Struct, Impl & OOP
=================

各位有在 C 裡實作過物件導向程式設計 (OOP) 嗎？  
這篇要來介紹 Rust 中的 struct 以及 OOP。

Struct
------

首先來介紹一下結構 (structure) ， Rust 中宣告結構是用 `struct` 關鍵字，這跟 C/C++ 很像，不過不太一樣的是， Rust 中預設所有的成員 (member) 都是私有的 (private)。

```rust
struct Foo {
  pub public_member: i32,
  private_member: i32, // 這個結尾的逗號可加可不加，但是加上去是官方推薦的
}
```

結構讓你可以組合不同的型態，給每個欄位名字，最後組合出一個新的型態，在上面我們宣告了一個有公開的 `public_member` 與私有的 `private_member` 的 struct ，事不遲疑，我們寫個 `main` 來試用一下：

```rust
fn main() {
  let foo = Foo {
    public_member: 123,
    private_member: 456
  };
}
```

Rust 裡建立一個結構就像這樣，在前面放上結構名稱，後面則是各個欄位以及它們的初始值，如果你有接觸過其它的 OOP 程式語言，你應該有注意到哪裡不太對勁了，為什麼我們可以直接使用應該是私有的 `private_member`？這不是違反封裝了嗎？在 Rust 中私有的成員代表的是只能在這個 `package` 中存取 (你可以先理解為這個檔案，之後會詳細解釋 Rust 的模組架構) ，這就像 Java 的 `package` 修飾或 Kotlin 的 `internal` 或 C 中使用 `static` 的全域變數。

> 封裝是把資料對外部隱藏起來，只允許使用方法存取，避免外部直接存取資料而導致程式的邏輯錯誤

More Struct
-----------

如果欄位的名稱不重要，或是只是想建一個型態把其它的型態包裝起來的話  
我們可以建一個 **"tuple struct"**：

```rust
struct Foo(i32, i32);
```

這像 tuple 與 struct 的融合體，如果要存取資料的話也跟 tuple 一樣：  

```rust
let foo = Foo(1, 2);
foo.0;
foo.1;
```

另外還有完全沒有資料的 "unit-like struct"：  

```rust
struct Bar;
```

這個比較少用到，通常是配合 trait 來使用的。

Impl
----

OOP 是把資料與操作資料的方法結合，借此模擬現實的東西，於是少不了的，有了資料自然要有操作資料的方法。  

```rust
struct Person {
  name: String
}

impl Person {
  pub fn new(name: String) -> Self {
    Person { name }
  }

  pub fn say_hello(&self) {
    println!("Hello, my name is {}", self.name);
  }
}

fn main() {
  let john = Person::new("John".to_string());
  john.say_hello();
}
```

Rust 使用一個分開的 `impl` 區塊來幫 struct 實作方法，這邊建立了一個 `Person` 的 struct ，與建構子 `new` 還有一個方法 `say_hello`，方法可以設定可見度，在前面加上 `pub` 就會變公開的方法了，在 Rust 中並沒有指定建構子的名字，所以稱為 `new` 只是一個慣例，因為 Rust 不允許多型 (Polymorphism)，也不允許函式的參數有預設值，所以在 Rust 中一個 struct 有多個不同的建構子也是常有的事，建構子所回傳的 `Self` 是一個特殊的型態，代表的是自己所實作的 struct ，在這邊你也可以寫 `Person`，不過如果要回傳自身都建議寫 `Self` ，因為它有一些好用的特性，在下一章會提到。  

其中使用的 `Person { name }` 是個語法糖，當變數名稱與欄位名稱一致時，就可以把 `Person { name: name }` 簡寫。

若實作的 method 沒有 `self` 變數則代表這個方法是個 associated method ，它只是個跟 struct 搭配的方法而已，就像其它語言的 static method ，使用時要用 `Person::new` 來呼叫。  

> 多型是指函式可以有同一個名稱，但使用不同的參數，這在 OOP 也是很常見的一個特性，比如在 C++ 可以建立 `Persion()` 與 `Persion(string)` 兩個建構子，但接受不同的參數  

`say_hello` 則是 `Persion` 的方法，而 `self` 是個特殊的變數，只會在實作時出現  
它代表的是自己，如果要變成 struct 的方法則第一個參數必須是 `self`，同時宣告 `self` 時要決定使用什麼方式宣告，一般最常用的是唯讀的 borrow `&self` 這代表你會用唯讀的方式存取自己，另外還有 `&mut self` 這時候你就可以修改自己了，最後 `self` 這個比較少用， 若使用 `self` 代表你將會取得自身的所有權，呼叫這個方法將不再能存取這個 struct ，一般用在要將自身轉換成另一個型態，或是呼叫這個方法後自己就不再有效的情況。

大部份情況下還是使用 `&self` 居多，呼叫方法則是用 `john.say_hello()` ，Rust 在這邊會自動的把 struct 轉成 borrow 的型態。

> Rust 中的所有的 function, method 都可以直接的存取與當成參數傳遞，比如在上面的例子，你可以使用 `Person::say_hello(&john)` 來呼叫方法，這其實有一種像在 C 中實作 OOP 的感覺，而上面的呼叫方式反而像是語法糖一樣。  

建造模式 Builder Pattern
------------------------

Rust 不支援多型，也不支援預設參數，所以當建構子的參數多時就顯的很麻煩，把所有成員都設成公開的又破壞封裝，所以在 Rust 中常會看到建造模式，建造模式是個設計模式，其將會使用另一個 struct 以一系列的方法來建造我們要的目標，以 Rust 的標準函式庫中的 [`std::process::Command`](https://doc.rust-lang.org/stable/std/process/struct.Command.html) 為例： 

```rust
let output = Command::new("echo")
  .arg("Hello world")
  .output();
println!("{}", String::from_utf8_lossy(&output.stdout));
```

像這樣的方式就是建造模式，最後我們要拿到的目標是那個 `output` 變數，底下我們自己實做一個：

```rust
#[derive(Debug)]
struct Point {
    x: i32,
    y: i32,
    z: i32,
}

impl Point {
    fn new(x: i32, y: i32, z: i32) -> Self {
        Point { x, y, z }
    }
}

struct Builder {
  x: i32,
  y: i32,
  z: i32,
}

impl Builder {
  fn new(x: i32) -> Self {
    Builder {
      x,
      y: 0,
      z: 0,
    }
  }

  fn y(&mut self, y: i32) -> &mut Self {
    self.y = y;
    self
  }

  fn z(&mut self, z: i32) -> &mut Self {
    self.z = z;
    self
  }
  
  fn build(&self) -> Point {
      Point::new(self.x, self.y, self.z)
  }
}

fn main() {
    let point = Builder::new(1)
        .y(2)
        .z(3)
        .build();
    println!("{:?}", point);
}
```

[![Playground][play-btn]](https://play.rust-lang.org/?gist=5e7b08e230a379f9c05571f8bcb66d1b&version=stable&mode=debug&edition=2015)

這邊假設了 `x` 的值是必要的，另外兩個都可以用 0 當預設值，這樣就可以只指定想要的東西了。  

另外這邊還出現了 `#[derive(Debug)]` ，這是什麼呢？  
下一篇我們要來介紹 trait ，與泛型 (generic) 。

[play-btn]: https://i.imgur.com/7F0C6a1.png
