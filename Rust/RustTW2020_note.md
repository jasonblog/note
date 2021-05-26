# Rust 基本教學

[TOC]

## Hello World!

### 不使用管理工具編寫程式

1. 建立副檔名為 `.rs` 的檔案 ex: `main.rs`
2. 寫 main function，程式碼編譯過後，會以 main function 作為進入點
```rust
fn main () {
    
}
```
3. 印出 `Hello World!`
```rust
fn main () {
    println!("Hello World");
}
```
4. 編譯程式碼
```shell
rustc main.rs
```
5. 編譯完後會產生 `main`/`main.exe` 檔，執行 `main`/`main.exe` 檔
```shell=
./main
```

### 使用 Cargo 管理專案

1. 建立專案
```shell=
cargo new hello_world
```
2. 編輯 `src/main.rs` 的檔案
```rust
fn main() {
    println!("Hello, world!");
}
```
3. 檢查專案是否可以編譯得過
```shell=
cargo check
```
4. 編譯
```shell=
cargo build
```
5. 執行
```shell=
cargo run
```
6. 優化編譯
```shell=
cargo build --release
```

### Hello World 的程式碼解析
```rust
fn main () {} // fn 是 function 的關鍵字
```

```rust=
println!("Hello World!") // println 是印出資料的語法，! 是 macro 的寫法， println!  是官方的 macro ，會在編譯時期根據目標平台轉換成相應的程式碼
```

## 宣告變數

- 在 rust 中，可以不用宣告型別，也會由編譯器自行推定
```rust
fn main () {
    let x = 5; // 會被自行推定為 i32
    let y: i32 = 10; // 也可以宣告變數型別
}
```
- 預設所有變數都是不可變的
```rust
fn main () {
    let x = 5;
    x = 10; // 不可以改變 x 的值
}
```
- 若要改變變數，必須宣告 `mut`
```rust
fn main () {
    let mut x = 5;
    x = 10; // OK
}
```
- 可以用 tuple 或 struct 的方式宣告多個變數並同時賦值
```rust
fn main () {
    let (a, b) = (1, 2);
    let (mut x, mut y) = (1, 2); // 或宣告可變的變數
}
```
- 可以事先宣告變數，但若變數被宣告後沒有初始化，同時在之後被使用到，會編譯不過
```rust
fn main () {
    let x: i32;
    println!("{}", x); // use of possibly-uninitialized `x`
}
```
```rust
fn main () {
    let x: i32;
    let condition = true;
    if condition {
        x = 1; // 因為在這裡被初始化了
        println!("{}", x); // 所以可以使用
    }
    // 但在這裡沒有被初始化
    println!("{}", x); // 在這裡會出錯
}
```
- 有的時候會在接別人的 API 時，遇到用不到，但必須寫出來的變數，可以用`底線` 帶過，就可以讓編譯器~~閉嘴~~，讓編譯器忽略沒有使用到這個變數，但同時`底線`變數也被視為不能被使用的變數，所以不可以在後面的程式碼中使用到
```rust
fn main () {
    let _ = "hello";
    println!("{}", _); // expected expression
}
```

- 如果在寫程式的途中，想要命名一個跟前面名稱一模一樣的變數是可以的
```rust
fn main () {
    let x = "Hello";
    println!("{}", x);
    
    let x = 5; // 前面的變數會被 shadowing
    println!("{}", x);
}
```

- 可以用 `type` 為一個型別取新的名字
```rust
type Age = u32;
fn grow (age: Age, year: u32) -> Age {
    age + year
}
```
- 宣告靜態變數
```rust=
static GLOBAL: i32 = 0;
```

- 宣告常數
```rust=
const GLOBAL: i32 = 0;
```

## 型別種類
- bool
- char
- 數字
- array
```rust
let a = [1, 2, 3];
let first = a[0];
let second = a[1];
```
- tuple
```rust
let a = ("hello", 1)
```
- struct
```rust
struct Person {
    age: u32,
    weight: u32,
}

fn main () {
    let ballfish = Person { age: 18, weight: 40 };
    println!("{}, {}", age, weight);
}
```
- tuple struct
```rust
struct Color (i32, i32, i32);
```
- enum
```rust
enum Food {
    Noodle,
    Rice
}

enum Message {
    Quit,
    ChangeColor(i32, i32, i32),
    Move { x: i32, y: i32 },
}
```

## if/else、loop、function
- 用大括號括起來的區塊，可以放在等號後面，最後一行不寫分號，會被視為回傳直傳出去
```rust
fn main () {
    let x = { println!("喵喵喵喵"); 5 };
    println!("{}", x);
}
```
- if/else
```rust
fn main () {
    let n = 4;
    if n < 0 {
        println!("Wow");
    } else if n == 0 {
        println!("owo");
    } else {
        println!("Orz");
    }
    // Orz
}
```

- Rust 並沒有三元運算子（ex: n < 0 ? "owo" : "OAO"），但可以把 if/else 寫成下面這樣
```rust
fn main () {
    let n = 4;
    let x = if n < 4 { "owo" } else { "OAO" };
    println!("{}", x);
    // OAO
}
```

- Rust 中，若 else 沒有寫出來，視為回傳 `()`，以剛剛的例子而言，由於 `x` 必須在編譯時期就確定型態，所以 if/else 回傳的值必須一致。也因此，通常 if 會伴隨 else，除非 if 沒有回傳值
```rust
fn main () {
    let n = 4;
    let x = if n < 4 { "owo" } else { 5 }; // expected `&str`, found integer
    println!("{}", x);
}
```

```rust
fn main () {
    let n = 4;
    let x = if n < 4 { "owo" }; // expected `()`, found `&str`
}
```

```rust
fn main () {
    let n = 4;
    let x = if n < 5 { println!("OAO") };
    // OAO
}
```

- loop，是一個不帶條件的無限迴圈
```rust
fn main () {
    loop {
        print!("喵喵喵喵");
    }
}
```

- 跟其他的程式語言一樣，Rust 有 `continue` 與 `break`
```rust
fn main () {
    loop {
        print!("汪汪");
        break;
    }
    
    loop {
        print!("喵喵喵喵");
        continue;
        print!("噗伊");
    }
}
```

- 你可以在 `break` 後面接值，這個值會被作為回傳值
```rust
fn main () {
    let a = loop {
        println!("噗伊");
        break 5;
    };
    println!("{}", a);
    // 噗伊
    // 5
}
```
- while，帶有條件的迴圈
```rust
fn main () {
    let mut n = 0;
    while n < 10 {
        println!("喵喵喵喵");
        n += 1;
    }
}
```

- while 也可以接在等號後面，但因為 `break` 在 `while` 中不能接值，所以永遠會回傳 `()`
```rust
fn main () {
    let mut n = 0;
    let x = while n < 10 {
        n += 1;
        break 5; // can only break with a value inside `loop` or breakable block
    };
}
```

```rust
fn main () {
    let mut n = 0;
    // OK，但 x = ()
    let x = while n < 10 {
        n += 1;
        break;
    };
}
```

- loop 與 while 的差異
    - loop 是不帶條件一定會執行的迴圈
    - while 是帶有條件，不一定會被執行的迴圈
    - 對於編義器來說，while block 裡的程式碼不一定會被執行到，無論 while 後面接的是不是 true
    - 也是因為這個差異，`break` 在 `while` 裏面才會不能接值，因為 while 沒有被執行的情況下，回傳直永遠是 `()`，所以在 while block 裡的回傳值一定要是 `()`
```rust
fn main () {
    let x;
    loop { x = 1; break; }
    println!("{}", x); // x 一定會在 loop 中被初始化，所以編譯會過
}
```

```rust
fn main () {
    let x;
    while true { x = 1; break; }
    // 因為編譯器無法保證 while block 裡的程式碼一定會被執行到，所以無法保證 x 一定會被初始化，因此編譯不會過
    println!("{}", x); // use of possibly-uninitialized `x`
}
```

- for loop，Rust 中沒有其他語言常有的 `for (i = 0;i < 10;i++)`，Rust 中的 for loop形式跟其他語言的 for-each 視同義的
```rust
fn main () {
    let array = [1, 2, 3];
    for i in array.iter() {
        println!("{}", i);
    }
}
```
- for loop 跟 while 的特性一樣，block 中的 `break` 後不可以接值， for loop 可以接在等號後面

- function
```rust
fn add (t: (i32, i32)) -> i32 {
    t.0 + t.1
}
```

- function 的 parameter 還可以直接解構
```rust
fn add ((x, y): (i32, i32)) -> i32 {
    x + y
}
```

- function 可以作為一個普通的變數
```rust
fn add ((x, y): (i32, i32)) -> i32 {
    x + y
}
fn main () {
    let func = add;
    println!("{}", func((1, 2))); // 3
}
```

- 不會正常回傳的 function
```rust
fn amazing () -> ! {
    panic!("crash the application~~");
}
```

- const fn，加上 const 關鍵字的 function 可以在編譯時期被執行，執行完的值也可以作為常數使用
```rust
const fn add ((x, y): (i32, i32)) -> i32 {
    x + y
}

fn main () {
    const CONSTANT: i32 = add((1, 2));
    println!("{}", CONSTANT);
}
```

- Rust 的 function 可以遞迴，但跟其他語言一樣過多層的遞迴會 stack overflow

