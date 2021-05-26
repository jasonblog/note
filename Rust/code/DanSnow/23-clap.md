Clap
=====

本次的程式碼在 https://github.com/DanSnow/rust-intro/tree/master/hastebin-client

這次來寫個在指令列下使用的小工具吧，各位知道 [hastebin](https://hastebin.com/about.md) 嗎？是個開源，而且很陽春的 pastebin ，今天的目標是來寫個它的用戶端，首先先建立專案並安裝幾個相依套件吧：

```shell
$ cargo init hastebin-client
$ cd hastebin-client
$ cargo add atty clap reqwest serde serde_derive
```

那幾個 crate 功能分別是：

- [atty](https://github.com/softprops/atty)： 偵測是不是終端機
- [clap](https://github.com/clap-rs/clap)： 分析指令列的參數
- [reqwest](https://github.com/seanmonstar/reqwest)： Http 的用戶端
- [serde](https://github.com/serde-rs/serde) 與 serde_derive ： 序列化與反序列化的函式庫，這在 Rust 的生態系中很常用

我們先試著用 reqwest 送出一份文件吧：

```rust
#[macro_use]
extern crate serde_derive;
extern crate reqwest;
extern crate serde;

use reqwest::{Client, Url};

const URL: &str = "https://hastebin.com";
// 這邊會把我們的程式碼以字串的型式引入進來
const SOURCE: &str = include_str!("main.rs");

// Deserialize 是 serde 提供的，只要加上去就能從資料反序列化回 Rust 的這個型態
#[derive(Clone, Debug, Deserialize)]
struct Data {
    key: String,
    // 因為這個欄位不一定會有，所以讓 serde 在沒有這個欄位時用預設值
    #[serde(default)]
    message: Option<String>,
}

fn main() -> reqwest::Result<()> {
  // 把網址 parse 成 Url 的型態
  let base = Url::parse(URL).unwrap();
  // 建一個 reqwest 的用戶端
  let client = Client::new();
  let res = client
    // 建立 post 的請求，並在網址後附上 documents
    // 完整的網址就變成 https://hastebin.com/documents 這正是 hastebin 的 api
    .post(base.join("documents").unwrap())
    // body 為我們的原始碼
    .body(SOURCE)
    // 送出請求
    .send()?
    // 將回傳的資料做為 json 反序列化為 data
    .json::<Data>()?;
  // 印出來
  println!("{:?}", res);
  Ok(())
}
```

實際執行一次會看到這樣的輸出：

```plain
Data { key: "avamicupez", message: None }
```

這邊的 key 是文件的代碼，你的應該不會跟我的一樣，這代表我們的文件在 https://hastebin.com/avamicupez 。

接著我們來加上更多功能吧，我希望這個程式可以讓我接上檔名就可以上傳該文件，若沒有接上檔名時則從標準輸入讀進來，還要顯示上傳後的網址，底下只有大概列一下改變的部份：

```rust
// ...
extern crate clap;

use clap::{App, Arg};
use reqwest::{Body, Client, Url};
use std::{
    fs::File,
    io::{stdin, Read},
    process,
};

// ...

fn main() -> reqwest::Result<()> {
  // 設定 clap
  let matches = App::new("haste-client")
    .author("DanSnow")
    .version("0.1.0")
    // 定義一個可選的位置的參數
    .arg(Arg::with_name("FILE").index(1))
    // 取得 parse 的結果
    .get_matches();
  // 決定要怎麼取得 body ，在有給檔名時使用檔名
  let body = if let Some(file) = matches.value_of("FILE") {
    // 開檔
    match File::open(file) {
      // 轉換成 Body
      Ok(f) => Body::from(f),
      Err(err) => {
        eprintln!("開啟檔案失敗： {}", err);
        process::exit(1);
      }
    }
  } else {
    // 這邊是讀標準輸入
    let mut buf = String::new();
    stdin()
      .lock()
      .read_to_string(&mut buf)
      .expect("讀輸入失敗");
    Body::from(buf)
  };
  // ...
  let res = client
    .post(base.join("documents").unwrap())
    .body(body)
    .send()?
    .json::<Data>()?;
  println!("{}", base.join(&res.key).unwrap());
  Ok(())
}
```

到這邊就完成了一個很陽春的用戶端了，很簡單吧，我們再多加一點功能好了，如果加上參數 `--raw` 就顯示另一個 `raw` 版本的網址，如果標準輸入是終端機的話就打開 vim (Linux 中在終端機下的一個老牌的文字編輯器)，讓使用者編輯檔案，編輯完再上傳，同樣的，這次只列出修改的部份：

```rust
// ...
extern crate atty;

use atty::Stream;
use std::{
  env,
  fs::{self, File},
  io::{stdin, Read},
  process::{self, Command},
};

// ...

fn main() -> reqwest::Result<()> {
  let matches = App::new("haste-client")
    .author("DanSnow")
    .version("0.1.0")
    // 增加 RAW 這個參數，設定參數使用的是 --raw
    .arg(Arg::with_name("RAW").long("raw"))
    .arg(Arg::with_name("FILE").index(1))
    .get_matches();
  let body = if let Some(file) = matches.value_of("FILE") {
    // ...
    // 判斷是不是終端機
  } else if atty::is(Stream::Stdin) {
    // 建個放暫存檔的位置
    let path = env::temp_dir().join("haste-client-tempfile");
    // 啟動 vim 打開這個暫存檔
    let mut child = Command::new("vim")
      .arg(path.as_os_str())
      .spawn()
      .expect("開啟 vim 失敗");
    child.wait().expect("等待 vim 結束失敗");
    // 開啟暫存檔
    let file = File::open(&path).expect("開啟暫存檔失敗");
    // 刪除暫存檔，在 Linux 就算刪除檔案，已經開啟的人還是可以正常讀寫檔案
    // 這招在 Windows 下不適用
    fs::remove_file(path).expect("刪除暫存檔失敗");
    Body::from(file)
  } else {
    // ...
  };
  // 檢查有沒有 RAW 參數，若有則在網址加上 raw
  let mut url = base;
  if matches.is_present("RAW") {
      url = url.join("raw/").unwrap();
  }
  println!("{}", url.join(&res.key).unwrap());
  Ok(())
}
```

這樣子已經挺好用的了，不過我們再加個參數好了，因為 hastebin 可以自己架設，所以我們加個 `--host` 的參數讓使用者決定要不要用自己的伺服器，同時也讀取 `HASTE_SERVER` 這個環境變數來找伺服器，順序是 `--host` > `HASTE_SERVER` > 預設值，以下是程式碼：

```rust
// ...
fn main() -> reqwest::Result<()> {
  let matches = App::new("haste-client")
    .author("DanSnow")
    .version("0.1.0")
    .arg(
      // 定義一個叫 HOST 的參數，它需要值，這次支援使用 -h 或 --host
      Arg::with_name("HOST")
        .short("h")
        .long("host")
        .takes_value(true),
    )
    .arg(Arg::with_name("RAW").long("raw"))
    .arg(Arg::with_name("FILE").index(1))
    .get_matches();
  // ...
  let base = Url::parse(
    // 先嘗試取得 HOST 的內容並轉換成 String
    &matches
      .value_of("HOST")
      .map(ToOwned::to_owned)
      // 取得環境變數的值，並用 ok 將 Result 轉換成 Option
      .or(env::var("HASTE_SERVER").ok())
      // unwrap ，若沒有值則用預設值
      .unwrap_or_else(|| URL.to_owned()),
  )
  .unwrap();
  // ...
}
```

另外你也可以對這個程式使用 `--help` 或 `--version` 看看，這兩個是 clap 自動幫我們加上去的 clap 還有其它的使用方法，有興趣可以去看文件，還有 serde 系列的還有個叫 serde_json 的函式庫，可以把 Rust 的值轉換成 json 或是從 json 轉換回來，也建議參考看看。

這次實作的 hastebin 的用戶端，我覺得做起來還挺簡單的，你覺得呢？ Rust 的社群提供了這些強大的 crate 讓這些功能的實作都變的很容易，雖然同樣的功能用 Python 等等的腳本語言做起來肯定更簡單，但 Rust 的程式碼我並沒有覺得有複雜到哪裡去，而且執行速度又快又是原生的程式，如果你會寫 Go 的話要不要試著寫一個來比較看看呢？

這次的用戶端其實也還有些可以改進的地方，比如：

- 錯誤處理的部份，像 `--host` 亂輸入
- 使用 `EDITOR` 環境變數來打開使用者偏好的編輯器

或許你也可以想想看有什麼地方能改進的。

下一篇我們來玩玩資料庫吧。
