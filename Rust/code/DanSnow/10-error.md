Error Handle
============

現在大多的程式語言都有例外 (exception) ，這讓程式碰到錯誤時可以立即的拋出例外，拋出的例外會中斷目前整個程式的流程，並開始往上找例外處理的程式，可是 Rust 並沒有這種機制。

Rust 中主要是以回傳值 `Result` 來代表有無錯誤的，此外也有可以立即中止程式的 `panic!` 。

`panic!`
--------

先介紹之前沒用過的 `panic!` ， `panic!` 會直接終止目前的執行緒，如果你呼叫了 `Result::unwrap` 或 `Option::unwrap` ，它們也會分別在值為 `Err` 或 `None` 時發生，這用在程式碰到了無法回復的錯誤。

> `panic!` 也像 `println!` 是 macro ，所以那個驚嘆號是要加的，此外它裡面也可以放格式化字串，使用方法是一樣的。

主執行緒遇到 `panic!` 時，程式會印出 `panic!` 內的訊息，與發生位置後結束程式，你也用以下方法執行程式，你會得到更詳細的輸出：

```shell
$ RUST_BACKTRACE=1 cargo run
```

如果是子執行緒發生 panic 的話等到之後講到多執行緒時再來介紹。

> 執行緒是電腦執行的單位，如果你的 CPU 有 4 核心，那你的電腦就能一次跑 4 個執行緒，所以現在很多程式為了加速會在一個程式裡產生多個執行緒，同時執行多個工作來加速，現在我們寫的程式都只有一個執行緒，也就是主執行緒而已。

Result
------

之前有介紹過 `Result` 是一個列舉，其由兩個 variant 組成，分別是 `Ok` 與 `Err` ，之前我們有在猜數字的遊戲中使用過 [`str::parse`](https://doc.rust-lang.org/stable/std/primitive.str.html#method.parse) 來把字串轉換成數字，它的回傳值的型態就是 `Result` ，但要怎麼知道是 `Result` 呢？除了看文件也有個簡單的辦法是像這樣：

```rust
fn main() {
  let res: i32 = "123".parse::<i32>();
}
```

拿去編譯的話你就會看到像這樣的錯誤訊息：

```plain
error[E0308]: mismatched types
 --> src/main.rs:2:18
  |
2 |   let res: i32 = "123".parse::<i32>();
  |                  ^^^^^^^^^^^^^^^^^^^^ expected i32, found enum `std::result::Result`
  |
  = note: expected type `i32`
             found type `std::result::Result<i32, std::num::ParseIntError>`
```

或是如果你有裝好 VSCode 的 Rust 的外掛的話應該也可以在滑鼠移上去後看到像這樣的提示：

![editor-hint](https://i.imgur.com/K1QYzIj.png)

不過這邊的錯誤也只有可能是字串中有非數字的字元而已，另一個比較複雜的範例是開啟檔案：

```rust
let f = match File::open("myfile") {
  Ok(f) => f,
  Err(err) => {
    // ...
  }
};
```

> 關於 `File` 的文件在 [`std::fs::File`](https://doc.rust-lang.org/stable/std/fs/struct.File.html)

這邊的 `err` 是 [`std::io::Error`](https://doc.rust-lang.org/stable/std/io/struct.Error.html) ，這是在有讀寫，或是比較跟系統底層有關時， Rust 的標準函式庫常回傳的錯誤型態，同時它還有個與之搭配的列舉 [`std::io::ErrorKind`](https://doc.rust-lang.org/stable/std/io/enum.ErrorKind.html)，用來代表錯誤的類別，於是我們可以像這樣使用：  

```rust
use std::io::ErrorKind;

let f = match File::open("myfile") {
  Ok(f) => f,
  // kind 是 std::io::Error 才有的方法，將會傳回代表錯誤類型的 ErrorKind
  Err(err) => match err.kind() {
    ErrorKind::NotFound => panic!("找不到檔案"),
    ErrorKind::PermissionDenied => panic!("權限不足"),
    err => panic!("開檔錯誤 {:?}", err),
  }
};
```

像這樣子進行更複雜的處理，也可以在找不到時建立一個檔案也是行的：

```rust
use std::io::ErrorKind;

let f = match File::open("myfile") {
  Ok(f) => f,
  // kind 是 std::io::Error 才有的方法，將會傳回代表錯誤類型的 ErrorKind
  Err(err) => match err.kind() {
    ErrorKind::NotFound => {
      match File::create("myfile") {
        // 檔案建立成功
        Ok(f) => f,
        Err(err) => panic!("無法建立檔案 {:?}", err),
      }
    }
    err => panic!("開檔錯誤 {:?}", err),
  }
};
```

`?` 運算子
----------

不要懷疑，這個運算子就是 `?` ，如果有個函式在它呼叫其它函式時發生了錯誤的情況，它，它就把錯誤往上回傳：

```rust
use std::io::{self, Read};

fn read_and_append<R: Read>(reader: R) -> io::Result<String> {
  let mut buf = String::new();
  match reader.read_to_string(&mut buf) {
    // 成功的話什麼都不用做
    Ok(_) => {}
    // 失敗的話直接回傳錯誤
    err => return err,
  }
  // 假設這邊還要做些處理後才回傳
  buf.push_str("END");
  // 回傳成功的結果
  Ok(buf)
}
```

> `Read` 是所有可讀取的東西會實作的一個 trait ，這包含檔案，或是標準輸入等等關於它的文件在 [`std::io::Read`](https://doc.rust-lang.org/stable/std/io/trait.Read.html)

其中的判斷錯誤，如果是錯誤就回傳的這段因為太常用到了，所以 Rust 就提供了個簡寫的方法，我們可以直接把上面的 `match` 那段改寫成：

```rust
reader.read_to_string(&mut buf)?;
```

如果它在成功時是會有回傳值的，比如 `File::open` 成功會回傳 `File` ，一個代表檔案的 struct ，那你也可以使用 `?` ：

```rust
let f = File::open("filename")?;
```

`?` 只能在會回傳 `Result` 的函式中使用，不過因為它實在是太方便了，所以後來 Rust 的 `main` 函式也支援回傳 `Result` 了：  

```rust
use std::fs::File;
use std::io;

fn main() -> Result<(), io::Error> {
  let f = File::open("filename")?;
  // 因為回傳值變 Result 了，所以這邊要回傳 Ok
  Ok(())
}
```

該用 `panic!` 還是回傳  `Result`
--------------------------------

一般的規則就是，能被處理的就用 `Result` ，嚴重的錯誤才用 `panic!` 。

自訂 Error
----------

在 [`std::error::Error`](https://doc.rust-lang.org/stable/std/error/trait.Error.html) 中定義了一個代表 Error 的 struct 應該要支援的兩個方法 `description` 與 `cause` ，此外同時還要實作 `Debug` 與 `Display` ，不過實際上 `description` 與 `cause` 都有提供預設的實作，於是這些之中一定要實作的就只有 `Display` 了，此外也可以實作 `cause` 用來指明發生這個錯誤的原因：

```rust
use std::io;
use std::fmt;
use std::error::Error;

// 建一個能包裝 io::Error 的 struct
#[derive(Debug)] // 實作 Debug
struct MyError(Option<io::Error>);

impl fmt::Display for MyError {
  fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
    // 寫出自訂的錯誤訊息
    f.write_str("這是自訂的錯誤: ")?;
    match self.0 {
      Some(ref err) => {
        // 如果有包裝的 io::Error 就把它的訊息印出來
        write!(f, "{}", err)
      }
      None => {
        write!(f, "沒有包裝的 io::Error")
      }
    }
  }
}

impl Error for MyError {
  // 覆寫原本的 cause ，在如果有原本的 io::Error 時傳回去
  fn cause(&self) -> Option<&Error> {
    // 這邊很可惜沒辦法用 Option::as_ref
    match self.0 {
      Some(ref err) => Some(err),
      None => None,
    }
  }
}

// 從 io::Error 轉換成 MyError
impl From<io::Error> for MyError {
  fn from(err: io::Error) -> Self {
    MyError(Some(err))
  }
}

fn main() {
  let err = MyError(None);
  println!("{}", err);
  let err = MyError(Some(io::Error::new(io::ErrorKind::Other, "Demo")));
  println!("{}", err);
}
```

> `write!` 的用法也和 `println!` 很像，只是第一個參數必須是可以寫入的，也就是有實作 [`std::io::Write`](https://doc.rust-lang.org/stable/std/fmt/trait.Write.html) 的物件，所以也可以用在 `File` 上，而第二個開始才是原本的格式化字串，它不像 `println!` 一樣會自動加換行。

這邊做了一個我們自己的 Error ，並還包裝了原本的 `io::Error` ，最後一個 `From` 的實作其實並不是必要的，只是實作了會很有用：

```rust
fn foo() -> Result<(), MyError> {
  Err(io::Error::new(io::ErrorKind::Other, "Demo"))?;
  unreachable!("這邊永遠不會執行到");
}
```

> `unreachable!` 同樣也是個 macro 它的功能在提示編譯器這種情況不該發生，否則編譯器會認為你的程式可能沒有回傳值，那如果真的執行到了呢？答案是會 panic

這邊可以看到我們用 `?` 運算子在碰到 `Err(io::Error)` 時提早回傳了，只是我們的回傳值明明是寫 `MyError` 呀，事實上用 `?` 運算子回傳時會使用 `MyError::from` 去轉換回傳的錯誤，當我們有幫 `MyError` 定義 `From<io::Error>` 時就能被自動轉換。

當你使用多個第三方的套件時，可能大家都會定義自己的錯誤型態，這時你可以嘗試使用列舉來包裝不同的錯誤型態，同時定義 `From` 來做轉換，這樣你就能在程式裡使用一個統一的錯誤型態了，因為這件事情太常用了，所以有個叫 [failure](https://github.com/rust-lang-nursery/failure) 的套件就把這件事用比較簡單的方式完成了，可惜因為再介紹下去篇幅會有點長，所以到後面實作專案時再來介紹吧。

> 題外話，實際上 `Result` 中代表錯誤的型態並沒有必要實作 `Error` ，只是一般還是會用實作了 `Error` 的型態來代表錯誤。

自訂 panic 的訊息 (進階)
------------------------

你可以在程式開始時註冊一個處理 panic 的函式：

```rust
use std::panic;

fn handle_panic<'a>(_info: &panic::PanicInfo<'a>) {
  println!("天啊，程式爆炸了");
}

fn main() {
  panic::set_hook(Box::new(handle_panic));
  panic!("Boom");
}
```

在這個函式裡你還可以拿到 panic 發生時的位置，與傳給 panic 的訊息：

```rust
use std::panic;

fn handle_panic<'a>(info: &panic::PanicInfo<'a>) {
  if let Some(loc) = info.location() {
    println!("在 {} 的第 {} 行", loc.file(), loc.line())
  } else {
    println!("不知道在哪邊");
  }

  // 這邊的 payload 的回傳值是 Any
  // downcast_ref 是嘗試把 Any 這個型態轉換成指定的型態
  // 如果轉換不成功就會回傳 None
  if let Some(msg) = info.payload().downcast_ref::<&str>() {
    println!("訊息： {}", msg);
  } else {
     println!("沒有訊息或訊息不是 str");
  }
  println!("總之爆炸了");
}

fn main() {
  panic::set_hook(Box::new(handle_panic));
  panic!("Boom");
}
```

> Any 是個特殊的 trait ，它幫大部份型態都實作過了一遍，透過編譯器的協助，幫每個型態都分配了一個代碼，要使用時你要使用 downcast_ref 或 downcast_mut ，只要你要求轉換的型態與原本的型態符合就會轉換成功，詳細可以參考文件的 [`std::any::Any`](https://doc.rust-lang.org/stable/std/any/trait.Any.html)

也有人使用 `set_hook` 的功能實作了一個會在 panic 時寫出紀錄檔的功能，有興趣可以看看這個專案 [human-panic](https://github.com/rust-clique/human-panic) 。

> 最開頭的地方說 Rust 沒有例外處理的機制也不是完全正確的， Rust 現在有能力捕捉 panic 了，只是這並沒有保證一定能捕捉到 panic 還要看編譯時的設定…等等的條件，有興趣可以看看 [`std::panic::catch_unwind`](https://doc.rust-lang.org/stable/std/panic/fn.catch_unwind.html) ，這功能主要的目的是當你使用其它語言呼叫 Rust 的程式時，讓你可以避免 Rust 的 panic 影響到其它的程式語言，平常如果要回傳錯誤的話還是請用 `Result` ，不要依賴這個。

下一篇要來介紹如何在 Rust 中寫測試。
