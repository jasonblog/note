與 Python 綁定
==============

Python 是個廣泛使用的腳本語言，想必或多或少都有聽過這個名字吧，人工智慧、科學計算、統計、應用程式、爬蟲等等的領域無不存在，非常的實用，如果想學個腳本語言的話非常推薦，雖然我個人是 Ruby 派的。

在這方便的程式語言中若偶爾碰到了需要對底層的東西做操作又找不到相關的函式庫時，就只好自己來寫綁定啦，或是有某部份需要加速等等的， Rust 的社群提供了一個很棒的 Python 綁定： [PyO3](https://github.com/PyO3/pyo3)

本次的程式碼在 https://github.com/DanSnow/rust-intro/tree/master/python-binding

PyO3 需要使用到 nightly 版本的 Rust ，因為它目前使用到了一個還未穩定的功能 `specialization` ，中文翻譯叫「特化」，它可以讓你為特定的型態提供不同的 trait 的實作，實際上標準函式庫中也有因為效能原因而為 `&str` 提供特化的 `to_string` 實作。

> Rust 只有 nightly 的編譯器支援開啟這些不穩定的功能，但是為了要編譯標準函式庫， Rust 的編譯器使用了個特殊的環境變數允許在穩定版開啟這些功能。

我們先來設定我們的專案與安裝 PyO3 吧，這次比較特別點，我們必須手動修改 `Cargo.toml` ，在 `[dependencies]` 後加上：

```toml
pyo3 = { version = "0.4.1", features = ["extension-module"] }
```

`features` 是代表我們需要的功能， Rust 支援使用者在程式裡設定一些只在某些功能開啟或關閉的情況下才編譯的程式碼，而我們這邊需要開啟 `extension-module` 這個功能才有辦法把 Rust 的程式編譯成 Python 的擴充模組。

> 這有點像在 C 裡使用 `define` 定義常數加上 `ifdef` 等等來開關功能。

接著我們再加入一段設定：

```toml
[lib]
name = "binding"
crate-type = ["cdylib"]
```

這邊是設定編譯完的輸出檔名與我們要編譯成動態函式庫，這樣才能讓 Python 載入。

總之先像往常一樣寫個 Hello world ：

```rust
// 開啟 specialization 這個功能，這只能在 nightly 版才能使用
#![feature(specialization)]

#[macro_use]
extern crate pyo3;

// 直接引入 prelude
use pyo3::prelude::*;

// 標記這是個給 python 的函式，這是 PyO3 提供的功能
// 這些函式必須回傳 PyResult
#[pyfunction]
fn hello() -> PyResult<()> {
  println!("Hello from Rust");
  Ok(())
}

// 標記這是 python 模組初始化用的函式
#[pymodinit]
fn binding(_py: Python, m: &PyModule) -> PyResult<()> {
  // 將我們的函式匯出給 python 使用
  m.add_function(wrap_function!(hello))?;

  Ok(())
}
```

然後我們編譯我們的函式庫：

```shell
$ cargo build
```

> 如果你編譯失敗的話你會需要安裝 `python3-dev`

編譯完後你應該看到我們編譯好的檔案在 `target/debug/libbinding.so` (如果你是用 Windows 的話會是 `binding.dll` ， Mac 會是 `libbinding.dylib`) ，我們把它複製出來，然後改名成 `binding.so` (Mac 也一樣， Windows 要改成 `binding.pyd`) ，接著你可以在終端機下輸入 `python3` (或 `python`) 來打開 Python 的 repl ，或你也可以選擇使用 `ipython`：

```python
>>> import binding
>>> binding.hello()
Hello from Rust
```

> `>>>` 是 Python 的提示字元，後面的內容是你要輸入的。

這次我們來計算 π (PI) 吧，也就是圓周率，我們用的是蒙地卡羅的計算方法，總之大概就是正方型與其內切圓的面積為 4:PI ，所以我們隨機的打點，然後計算落在圓中的次數來反算 PI ，用 Python 是這樣的：

```python
from random import random

TIMES = 1000_0000


def calculate_pi():
    hit = 0

    for _i in range(0, TIMES):
        x = random()
        y = random()
        if x * x + y * y <= 1.0:
            hit += 1
    return hit * 4 / TIMES
```

在上面隨機的產生了 1 千萬次的點，然後最後回傳算出 PI 的近似值，接著我們用 timeit 這個 Python 的 module 來算耗時：

```python
print("Python:", timeit.timeit("calculate_pi()", number=3, globals=globals()))
```

重覆執行三次算平均，在我的電腦上大概花了 4 秒。

接著我們用 Rust 實現同樣的演算法：

```rust
#[pyfunction]
fn calculate_pi() -> PyResult<f64> {
  let mut hit = 0;
  for _ in 0..TIMES {
    let x = rand::random::<f64>();
    let y = rand::random::<f64>();
    if x * x + y * y <= 1.0 {
      hit += 1;
    }
  }
  Ok(f64::from(hit * 4) / f64::from(TIMES))
}
```

一樣在 Python 用 timeit 計算時間，這次只花了 0.7 秒，快了大約 5 倍啊。

最後若我們搭配之前介紹的 rayon 做平行化：

```rust
#[pyfunction]
fn calculate_pi_parallel(py: Python) -> PyResult<f64> {
  let hit: i32 = py.allow_threads(|| {
    (0..TIMES)
      .into_par_iter()
      .map(|_| {
        let x = rand::random::<f64>();
        let y = rand::random::<f64>();
        if x * x + y * y <= 1.0 {
          1
        } else {
          0
        }
      })
      .sum()
  });
  Ok(f64::from(hit * 4) / f64::from(TIMES))
}
```

使用到執行緒的部份必須包在 `py.allow_threads` 裡，不然會出問題，這次只花了 0.3 秒，底下是在我電腦上執行的完整結果：

```plain
Python: 4.45852969000407
Rust: 0.7022144030052004
Rust parallel: 0.30857402100082254
```

PyO3 雖然需要使用到 nightly 版的 Rust ，但它真的很方便，實際上 PyO3 的 macro 幫我們產生了很多在 Rust 與 Python 之間轉換資料型態的程式碼，我們才能這麼簡單的寫與 Python 的綁定，如果有興趣的話有另一個 crate 叫 cpython ，它是 PyO3 的前身，但它並沒有提供這些自動產生的程式碼，所以可以看到這中間做了哪些轉換。

條件編譯
--------

因為上面正好提到了，就順便來講一下，條件編譯不只可以做出可以開關的功能，也能在不同系統或架構下提供不同的功能。

```rust
#[cfg(unix)]
fn print_os() {
  println!("你在使用類 Unix 的系統");
}

#[cfg(windows)]
fn print_os() {
  println!("你在使用 Windows 系統");
}

fn main() {
  print_os();
}
```

上面的程式在 Windows 下與 Linux 下會印出不同的東西，因為 Rust 的編譯器會根據不同的作業系統而選擇編譯不同的程式，這也不只可以用在函式上而已，還可以用在模組的引入：

```rust
#[cfg(unix)]
mod unix_imp;
#[cfg(unix)]
use unix_imp as imp;

// Use imp::...
```

這很類似在標準函式庫中提供在不同系統下不同實作的方法。

另外也可以在 `Cargo.toml` 中宣告 `[features]` ：

```toml
[features]
default = [] # 預設開啟的功能
foo = [] # 一個叫 foo 的功能，後面可以放相依的功能或是支援這個功能所額外需要的 crate
```

然後在程式碼中使用：

```rust
#[cfg(feature = "foo")]
fn foo() {
  // 實作 foo 功能等等
}
```

這篇展示了怎麼建立 Python 的綁定，大部份的腳本語言都有支援用 C 擴充它的功能 (但像 Node.js 是用 C++) ，這使得這些腳本語言能透過這些原生的擴充模組存取系統底層或是加速，而只要能夠支援 C 就能透過 Rust 實作，有著接近 C 的速度，卻有如此大量的社群套件支援，用 Rust 來實作這些原生套件真的不失一個選擇。

下一篇來寫點應用程式吧，我們試著用 clap 來建一個指令列的程式。
