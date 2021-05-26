Enum, Pattern Matching
======================

列舉 (Enum)
-----------

列舉是 Rust 中的一個型態，其為多個 variant 所組成：  

```rust
enum Color {
  Red,
  Green,
  Blue,
}
```

使用時必須要加上列舉的名稱，比如 `Color::Red` ，或是你也可以像引入函式庫一樣的，把 enum 內的 variant 用 `use` 引入，比如 `use Color::*` ，就會把 `Color` 下的 variant 都引入了。

不過 Rust 的列舉的特殊之處是 variant 可以帶值：

```rust
enum StringOrInt {
  String(String),
  Int(i32),
}
```

裡面的值就像 struct 一樣，也就是你可以不只一個值，或是給它們欄位的名稱：  

```rust
enum Point {
  Point2D (i32, i32),
  Point3D {
    x: i32,
    y: i32,
    z: i32,
  }
}
```

在 Rust 中列舉除了用來表示有限的選項外，也可以用來傳遞型態不同的參數，像 `StringOrInt` 一樣。  

另一個重要的應用是 `Option` 與 `Result` ，不知道大家還記不記得之前有提到過 `Result` 這個代表結果的型態呢，它就是列舉，其定義如下：  

```rust
enum Result<T, E> {
  Ok(T),
  Err(E),
}
```

`T`, `E` 是泛型的型態變數，分別代表正確時的回傳值，與發生錯誤時的錯誤物件，它是個泛型的列舉，而它也有提供一些方便的方法，比如 `expect` ，是的 Rust 的列舉是可以幫它定義方法的，同樣的用 `impl` 就可以了，也可以幫它實作 trait ，它就像 struct 一樣。  

> `Result::expect` 其實在之前就有使用過了，它的功能是在結果為 `Err` 時印出訊息並結束程式。

再來我們剛剛還提到了 `Option` ， `Option` 是 Rust 中用來代表可能沒有值，它用來取代掉其它語言中的空指標 (`null`, `nil` ...) ，它用兩個值 `Some` 與 `None` 來代表有沒有值， Rust 中 `Result` 跟 `Option` 因為很常使用，所以它們跟它們的 variant 都已經被預先引入了，你可以不需要預先引入就可以使用。

它的定義則是這樣的：  

```rust
enum Option<T> {
  Some(T),
  None,
}
```

> 在 Rust 中之所以沒有 `null` 的，因為 Rust 認為 `null` 很容易造成錯誤，並且使用 `Option` 可以強迫使用者先檢查是否有值。  

同時 `Option` 也同樣提供了很多方法可以使用，也有不少跟 `Result` 有共通的名稱與作用：  

- `Option::unwrap`: 直接把 `Option` 內的值取出來，若 `Option` 是 `None` 則會造成程式印出錯誤訊息後直接結束
- `Option::unwrap_or`: 取出值，若沒有值則回傳使用者提供的預設值
- `Option::unwrap_or_else`: 取出值，若沒有值則呼叫與使用者提供的函式，並使用回傳值當預設值

`unwrap_or_else` 常用在建立會需要消耗資源的情況，比如當我們在沒有值時需要空的 `String` 當預設值就會寫：  

```rust
something.unwrap_or_else(String::new);
```

> 建議可以看一下 [`Result`](https://doc.rust-lang.org/stable/std/result/enum.Result.html) 和 [`Option`](https://doc.rust-lang.org/stable/std/option/enum.Option.html) 的文件，畢竟這兩個型態可說是一定會在 Rust 中接觸到，說不定它已經寫好函式提供你所要的功能了，上面列出的三個函式也都有 `Result` 的版本。

而 Rust 也幫所有的型態實作了 `impl From<T> for Option<T>` 可以直接把任何型態的值轉換成 `Some`。  

之前雖說沒有預設參數，不過搭配的泛型使用還是可以寫出像這樣的程式碼：  

```rust
// 這邊的 i 用的是任何可以被轉換成 Option<i32> 的型態
fn print_number(i: impl Into<Option<i32>>) {
  println!("{}", i.into().unwrap_or(42));
}

fn main() {
  print_number(123);
  // 沒有值時還是需要明確傳 None 進去，這邊會印出 42
  print_number(None);
}
```

解構賦值 (Destructuring)
------------------------

Rust 中的複合的型態 (陣列、元組、結構) 都可以做解構：  

```rust
struct Point {
  x: i32,
  y: i32,
}

let [a, b] = [1, 2];
let (num, msg) = (123, "foo");
let Point { x, y } = Point { x: 10, y: 10 };
```

需要注意的是，解構是轉移所有權的操作，也就是說如果使用到了無法複製的型態，則它的值就會被移動，所以這邊要再來介紹一個關鍵字 `ref`：  

```rust
let msg = "Hello world".to_string();

// 底下這兩句的意思是一樣的
let borrowed_msg = &msg;
let ref borrowed_msg = msg;
```

簡單來說 `ref` 代表要使用 borrow 來取得在右邊的變數，這在解構時非常有用，你可以這樣寫：  

```rust
struct Person {
  name: String,
}

let person = Person { name: "John".to_string() };
let Person { ref name } = person;
```

這時候 `name` 就會用 borrow 的方式取得，而不會把原本的 `name` 值移出來。

同樣的也有 `ref mut`：

```rust
let Persion { ref mut name } = person;
```

這邊的 `name` 就會以可寫的方式 borrow，當然這邊也同樣的要套上之前介紹的 borrow 的規則，在 `name` 的 borrow 結束前你沒辦法使用 `person`。  

如果想要在解構時忽略掉某一部份的的值的話怎麼辦呢，如果只想忽略掉某幾個值的話，你可以使用 `_`， `_` 是個特殊的變數名稱，Rust 不會把任何東西賦值給 `_` ，而會直接忽略，你可以想像它就是個黑洞：  

```rust
let [_, b, _] = [1, 2, 3];
let Point { x, y: _ } = Point { x: 1, y: 2 }; // 如果 y 的值是不能 copy 的，這邊並不會發生所有權轉移
```

如果想把其它的值都忽略掉呢，你可以使用 `..` ，這個目前只支援 struct 與 tuple：  

```rust
let (a, ..) = (1, 2, 3);
let Point { x, .. } = Point { x: 1, y: 2 };
```

要注意的是 `..` 只能在解構時出現一次  

```rust
let (.., x, ..) = (1, 2, 3, 4); // 這裡的 x 應該要是多少呢
```

`..` 也可以在你想要忽略掉 struct 中的私有成員時：

```rust
pub struct Person {
  pub name: String,
  age: i32,
}

// 假設這邊是另一個模組，也就是無法取得私有成員的
// 因為你並不知道私有成員的名稱，所以是無法用 _ 的
let { ref name, .. } = person;
```

模式比對 (match)
----------------

模式比對是 FP (Functional Programming) 裡一個重要的操作，它的語法如下：

```rust
// 若數字是 1 就印出「數字是 1」，以此類推
match 2 {
  1 => println!("數字是 1"),
  2 => println!("數字是 2"),
  3 => println!("數字是 3"),
}
```

> `match` ，這樣寫的話很像 C 的 `switch` ，除了它可以有回傳值以及沒有 `break` 的這點外  

那麼它強在哪邊呢，你可以把它搭配上面的解構使用，同時再加上可以帶值的列舉，就能寫出更複雜的判斷，像在 Rust 很常出現的一種錯誤處理方式：  

```rust
match result {
  Ok(val) => {
    // 這邊就有成功的值可以用
  }
  Err(err) => {
    // 這邊可以做錯誤處理
  }
}
```

> 如果你 `match` 裡放的是大括號的區塊，那可以不用加逗號，怕搞錯的話還是都加吧。  

你也可以比對一部份的值比如：

```rust
match [1, 2] {
  [1, _] => println!("陣列的開頭是 1"),
  _ => println!("陣列的開頭不是 1"),
}
```

或是：

```rust
match Point { x: 10, y: 20 } {
  Point { x: 10, .. } => println!("x 是 10"),
  _ => println!("x 不是 10"),
}
```

還可以比對數字是不是在一個範圍內

```rust
match 3 {
  1...5 => println!("x 在 1~5"),
  6...10 => println!("x 在 6~10"),
  _ => println!("x 不在 1~10")
}
```

那個 `...` 只有在 match 時支援而已，是代表包含上下界的範圍，如果在平常需要用到包含上下界的範圍，比如在做切片時要用 `..=`：  

```rust
let array = [1, 2, 3, 4, 5];
let slice = &array[0..=2];
```

還可以加上條件判斷：  

```rust
match Some(3) {
  Some(x) if x < 5 => println!("x < 5: {}", x),
  Some(x) => println!("x > 5: {}", x),
  None => println!("None"),
}
```

你可以在一行裡比對數個情況

```rust
match 3 {
  1 | 2 | 3 => println!("是 1 ， 2 或 3"),
  _ => println!("不是 1 ， 2 ， 3"),
}
```

你還可以用 `@` 來給比對成功的值一個變數  

```rust
match 3 {
  // 若這邊比對 1...5 成功，則值會被放到 x 這個變數
  x @ 1...5 => println!("x 是 {}", x),
  6...10 => println!("x 在 6~10"),
  _ => println!("x 不在 1~10")
}
```

Rust 裡的模式比對要求要把所有可能出現的值都比對一次，如果沒有的話會是編譯錯誤， 所以你可以在最後用一個變數，或是不需要變數的話用 `_` 當預設的情況，你也可以使用 `if` ，只在碰到某種情況時處理，而忽略另一些情況：

```rust
// 這邊一定要有 let
if let Some(x) = Some(42) {
  println!("x 是 {}", x);
}
```

下一篇要來講 Rust 的模組架構，這樣就可以好好的組織程式碼，不用在全部都寫在 `main.rs` 裡了。
