# 給 C++ 使用者的 Rust 簡介



[Rust](https://www.rust-lang.org/) 是最近受到廣泛注目的新語言。最早由 Mozilla 資助開發，後來因為 Dropbox 使用 Rust 改寫檔案系統服務[[1\]](https://electronic.blue/blog/2016/10/10-rust-an-introduction/#fn1)而聲明大噪。目前 Rust 是很活躍的開源專案，有超過一千名開發者共同開發，大約一至兩個月就會有一次 minor release。

設計程式語言最困難的地方在於選擇，沒有一個語言是上山下海無所不能的，而 Rust 也不例外。Rust 的目標是成為高效率、易於平行運算的系統程式語言，因此它選擇了以下的特性：

- 靜態型別 (static-typed)
- 區分 mutable 與 immutable，所有變數預設為 immutable，盡可能減少 mutable state
- 使用 tagged union 與 pattern matching
- 不使用動態垃圾回收 (garbage collection)，而使用靜態的 RAII
- 使用 Move semantics 避免複製物件
- 使用 borrow checker 確保 memory safety 與 thread safety

因此，對於習慣主流程式語言的使用者來說，Rust 的學習曲線非常陡峭，光是要讓程式碼通過編譯就要花上不少時間。接下來這一系列的文章，是以 C++ 使用者為對象，介紹 Rust 的各種語言功能以及背後的設計目標，希望各位可以看得很愉快。



## Hello World

先從每個語言都會有的 hello world 開始吧：

```rust
// hello.rs
fn main() {
    println!("hello world");
}
```

編譯與執行方法如下：

```bash
$ rustc -o hello hello.rs
$ ./hello
hello world
```

從這個最簡單的範例可以看出與 C++ 相同的地方：

- 註解也是 `//`，你也可以用 `/* */` 寫多行註解。
- 程式也是以 `main` 為進入點。
- 函式也同樣用大括號包起整個結構，每行敘述使用 `;` 作為結尾。

不一樣的地方則是：

- 沒有 `#include`。
- 需要用 `fn` 關鍵字來定義函式。
- `main` 沒有回傳值。
- `println!` 函式名稱多了一個驚嘆號。

`rustc` 會自動幫你引入一部份的標準函式庫 (std::prelude)，因此你不需要為了印一行字額外引入函式庫。另外 `println` 後面的驚嘆號代表它其實不是函式，而是巨集 (macro)。由於本文重點不在巨集，因此我們只要先知道 `println!` 可以拿來當 `printf` 那樣用就可以了。

## 型別與變數

宣告變數的方法是使用 `let` 關鍵字：

```rust
fn main() {
    let x = 10;
    let y: f32 = 3.14;
    println!("x = {}, y = {}", x, y); // x = 10, y = 3.14
}
```

Rust 會自動推導型別，因此 `x` 的型別是 `i32`，意指 32bit signed integer。你也可以在變數名稱後加上冒號來指定型別，因此 `y` 的型別是 32bit floating point，而不是 floating point literal 預設的 `f64`。

Rust 的內建型別及對應的 C++ 型別如下：

| Rust type | C++ type                                                     | 說明                                                         |
| :-------- | :----------------------------------------------------------- | :----------------------------------------------------------- |
| `bool`    | `bool`                                                       | 布林值                                                       |
| `i8`      | `int8_t`                                                     | 8-bit 有號整數，使用二補數表示負值                           |
| `u8`      | `uint8_t`                                                    | 8-bit 無號整數                                               |
| `i16`     | `int16_t`                                                    | 16-bit 有號整數，使用二補數表示負值                          |
| `u16`     | `uint16_t`                                                   | 16-bit 無號整數                                              |
| `i32`     | `int32_t`                                                    | 32-bit 有號整數，使用二補數表示負值                          |
| `u32`     | `uint32_t`                                                   | 32-bit 無號整數                                              |
| `i64`     | `int64_t`                                                    | 64-bit 有號整數，使用二補數表示負值                          |
| `u64`     | `uint64_t`                                                   | 64-bit 無號整數                                              |
| `usize`   | `size_t`                                                     | 可表達記憶體空間內最大物件大小的無號整數型別，常用來表示 array index |
| `isize`   | `ptrdiff_t`                                                  | 上述型別的有號版本，可用來表達兩個 array index 的差異        |
| `f32`     | `float` [[2\]](https://electronic.blue/blog/2016/10/10-rust-an-introduction/#fn2) | IEEE754 規範的 32-bit 浮點數                                 |
| `f64`     | `double` [[2\]](https://electronic.blue/blog/2016/10/10-rust-an-introduction/#fn2) | IEEE754 規範的 64-bit 浮點數                                 |
| `char`    | `char32_t` [[3\]](https://electronic.blue/blog/2016/10/10-rust-an-introduction/#fn3) | 使用 UTF-32 表達的 Unicode 字元                              |

## Mutable & Immutable

雖然講起來很矛盾，但預設情況下變數是不可變的 (immutable)：

```rust
let x = 10;
x = x + 1; // error: re-assignment of immutable variable `x`
```

若要讓變數可以重新賦值，需要使用 `mut` 關鍵字來宣告：

```rust
let mut y = 10;
y = y + 1; // ok
```

有很多理由支持讓 immutable 成為預設，比如說 compiler 的最佳化或是減少 race condition。在後續的文章中，我會更進一步討論。

## Struct & Tuple

Rust 的 struct 宣告方式與 C++ 大同小異，差別僅在於各成員型別的位置、使用逗號作為分隔、以及結尾不需要加分號：

```rust
struct Foo {
    x: i32,
    y: f64,
}

fn main() {
    let foo = Foo { x: 10, y: 2.5 };
    println!("foo.x = {}, foo.y = {}", foo.x, foo.y);
}
```

而 tuple 其實是個語法糖 (syntactic sugar)，相當於使用編號當作成員名稱的 struct：

```rust
let triple = (10, 3.14, 'x');
println!("triple = ({}, {}, {})", triple.0, triple.1, triple.2)
```

編譯器會把它轉變成這樣 (以下為示意，實際上宣告 struct 時不能拿數字當成員名稱)：

```rust
struct Triple {
    0: i32,
    1: f64,
    2: char,
}
let triple = Triple { 0: 10, 1: 3.14, 2: 'x'};
println!("triple = ({}, {}, {})", triple.0, triple.1, triple.2)
```

值得注意的是，沒有任何元素的 tuple，也就是 0-tuple，也是一個合法的型別，稱之為 *unit type*。它具有唯一一個可能的值，就是空的 tuple，稱之為 *unit*。

```rust
let unit: () = (); // 完全合法，雖然你沒辦法拿這個變數做什麼事
```

## Move Semantics

若是沒有覆載賦值運算子，C++ 的 struct 具備 value-type semantics，意即使用等號賦值或進行參數傳遞時，會複製整個物件的內容。而 Java class 則具備 reference-type semantics，使用等號僅僅複製物件的位址，它們仍然會影響同一個物件的內容。

Rust 並沒有 class，那麼 rust 的 struct 是 value-type 還是 reference-type 呢？我們試著用最簡單的做法來判定 value-type 與 reference-type：宣告一個物件，用等號賦值給另一個物件並修改其內容，然後檢查原物件的值是否變動。對 value-type 來說是不變動的，而對 reference-type 來說，因為兩個變數實際指向同一塊記憶體，因此內容會變動。然而，這兩種狀況都不會發生在 Rust 上面，因為 compiler 把它擋下來了。

```rust
struct Point {
	x: i32,
	y: i32,
}

fn main() {
	let mut foo = Point { x: 10, y: 20 };
	let mut bar = foo;
	bar.x = 30;
	println!("foo.x = {}", foo.x); // error: use of moved value `foo.x`
}
```

Rust struct 具備了 move semantics，使用等號賦值時，資料並不是「複製」，而是「移動」到左值上。右值在移動後，就會成為未初始化的物件，因此 Rust 禁止你對它進行操作。如果你還是有點難以想像，把它理解成 C++11 的 `std::move` 就可以了：

```rust
#include <utility>
#include <iostream>

struct Point {
    int x, y;
};

int main() {
    auto foo = Point { x: 10, y: 20 };
    auto bar = std::move(foo);
    bar.x = 30;
    std::cout << "foo.x = " << foo.x << std::endl;
    return 0;
}
```

這段程式碼可以通過編譯，然而如果你把 `Point` 換成其它實作 move constructor 的物件 (比如 `std::string`)，那麼在 `std::move(foo)` 之後，很可能 `foo` 會成為內容未初始化的物件，印出其內容會造成 undefined behavior。

為了使用上的方便，Rust 的基本型別，也就是上面那張表格中的所有型別，都具備可複製的特性。因此使用等號賦值時，進行的動作是「複製」，讓你可以繼續操作右值。在後續的文章中，我會更進一步講解 Rust 的 move semantics。

## 表達式

Rust 是 expression-oriented language，大部份的流程控制結構，比如說 `if`，其實都是可以求值的表達式。

```rust
let x = -10;
let abs_x = if x >= 0 {
                x
            } else {
                -x
            };
```

這看起來其實就是 C++ 的 `?:` 運算子。然而，Rust 可以在表達式中用分號進行區隔，並使用最後一個表達式當作結果，因此可以組合出複雜的表達式：

```rust
let year = 2016;
let is_leap = {
                  let div_4 = (year % 4 == 0);
                  let div_100 = (year % 100 == 0);
                  let div_400 = (year % 400 == 0);
                  if div_400 || (div_4 && !div_100) {
                      "is"
                  } else {
                      "is not"
                  }
              };
println!("Year {} {} a leap year.", year, is_leap);
```

分號可以用來分隔表達式，最後一個不帶分號的表達式會成為整個表達式的結果，因此 `is_leap` 會根據條件判斷，成為 `"is"` 或 `"is not"`。注意第七行與第九行都不能加分號，要是最後一個運算式也加上分號，那麼整個運算式的結果會變成 `()`，也就是那個沒啥用的 0-tuple。而在第 11 行的分號則用來區隔 `let` 變數宣告與 `println!`，是一定要加上去的。

## 函式

前面提到 Rust 使用 `fn` 來宣告函式，而且回傳型別寫在後面，看起來很像 C++11 裡面新的函式宣告法：

```rust
fn square(x: f64) -> f64 {
    return x * x;
}
```

函式本體也是可以使用分號區隔的表達式，最後一個不帶分號的表達式會自動成為函式的回傳值，因此上一段檢查閏年的函式可以這樣寫：

```rust
fn is_leap(year: i32) -> bool {
    let div_4 = (year % 4 == 0);
    let div_100 = (year % 100 == 0);
    let div_400 = (year % 400 == 0);

    div_400 || (div_4 && !div_100)
}
```

你可以用 tuple 輕易讓函式回傳多個值：

```rust
// 對兩個數字做排序
fn reorder(x: i32, y: i32) -> (i32, i32) {
    if x > y {
        (y, x)
    } else {
        (x, y)
    }
}
```

即使函式不回傳任何值，它還是有回傳型別，也就是上面提到那個好像沒啥用的 0-tuple。

```rust
fn say_hello() -> () { // -> () 可省略
    println!("hello world");
}

fn main() { // 若無回傳型別，rust 會自動加上 -> ()
    let result = say_hello(); // 合法，result 的值為 ()
}
```

這看起來好像沒什麼用，畢竟 0-tuple 什麼事都做不了。然而，當你要寫泛型函式 (generic function) 時，你會跪在電腦前感謝這個設計。

## 泛型

如同 C++ 那般，Rust 也可以利用模版 (template) 來達成泛型程式設計，語法也非常接近 C++：

```rust
struct Point<T> { // 相當於 template<typename T> struct Point
    x: T,
    y: T,
    z: T,
}

fn main() {
    let point_i32 = Point { x: 10, y: 20, z: 30 };
    let point_f64 = Point { x: 2.078, y: 0.454, z: 3.1415 };
}
```

與 C++ 不同的是，大部份情況下 Rust 都能藉由前後文來自動推導出正確的模版型別，因此上面的例子中並不需要特別加入 `<i32>` 或是 `<f64>`，直接用 `Point` 即可。

泛型非常適合用來實作容器型別，比如 Rust 提供的 `Vec` 泛型容器，就相當於 C++ 的 `std::vector`。

```rust
fn main() {
    let mut array = Vec::new();
    array.push(1);
    array.push(2);

    println!("{}", array[0] + array[1]);
}
```

同樣地，因為 Rust 從第三行的 `push(1)` 判斷出 array 的元素型別為 `i32`，因此在第二行就不需要寫明 `Vec<i32>::new()`，直接寫 `Vec::new()` 即可。

除了泛型類別，模版也可以用來定義泛型函式，然而與目前 C++ 不同的地方是，在 Rust 中，對泛型型別進行操作前，必需為它標上 constraint：

```rust
fn sum<T: Add>(a: T, b: T) -> T::Output {
    a + b
}
```

這邊 `Add` 意指 `T` 必需是可以使用加號相加的型別，包括整數及浮點數都包括在內。由於相加後輸出型別不一定仍然為 T，因此這個函式的回傳型別是 `T::Output`。Rust 也支援運算子覆載 (operator overloading)，只要你的自訂型別定義了加號操作以及輸出型別，那麼這個自訂型別也可以直接傳入 `sum` 進行運算。

## 總結

這篇文章中，我把重點放在 Rust 最核心的語言功能上，甚至省略了陣列與字串處理，因為講解這部份就無可避免會提到 borrow checker。在後續幾篇文章中，我將會繼續深入解釋 move semantics 與 borrow checker。

------

1. [The Epic Story of Dropbox’s Exodus From the Amazon Cloud Empire](https://www.wired.com/2016/03/epic-story-dropboxs-exodus-amazon-cloud-empire/) [↩](https://electronic.blue/blog/2016/10/10-rust-an-introduction/#fnref1)
2. 雖然大多數的 C++ compiler 都使用 IEEE754，但其實 C++ 標準沒有規範 `float` 與 `double` 一定要使用 IEEE754。 [↩](https://electronic.blue/blog/2016/10/10-rust-an-introduction/#fnref2) [↩](https://electronic.blue/blog/2016/10/10-rust-an-introduction/#fnref2:1)
3. Rust 的 `char` 固定為 32-bit，然而 C++11 規定 `char32_t` 大小與 `uint_least32_t` 相同，因此可能會超過 32-bit。 [↩](https://electronic.blue/blog/2016/10/10-rust-an-introduction/#fnref3)