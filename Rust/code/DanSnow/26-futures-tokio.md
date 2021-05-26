Futures & Tokio
===============

Async IO
---------

在開始之前要先來介紹一下非同步的 IO 是怎麼回事，如果你有碰過比較底層的系統程式，你可能會知道在 Linux 下用來開啟檔案的 [`open`](http://man7.org/linux/man-pages/man2/open.2.html) 有個選項是 `O_NONBLOCK` ，它會做什麼？它做的事就是當你讀取資料時，若資料還沒準備好，讀取的操作就會直接以失敗返回，而不會在那邊等到有資料，於是在這段時間內程式就能去做其它的事，晚點再回來試著讀看看，而且這並不只可以用在檔案上， Linux 系統把非常多的東西都視為檔案去操作，這包含網路連線，與硬體的溝通等等，當然 Windows 下也有類似的機制。

當然，盲目的嘗試其實很沒效率，所以作業系統也提供了方法可以讓我們偵測是不是可以讀取了，我們可以開啟多個檔案、連線或裝置，等到系統通知我們其中有某個已經準備好能讀取了再去處理，這就是所謂的事件驅動 (event driven) ，同時也是像 Nodejs 內部的處理方式。

這樣的方式能以單一的執行緒處理所有的讀寫操作，比起原本的同步 IO 必須使用多個執行緒或處理序省下了切換行程等等的成本，但相對的會讓程式的設計變得複雜，不過理想上這些處理非同步 IO 的框架可以幫我們把程式碼變的像同步一樣。

Futures
-------

[futures](https://rust-lang-nursery.github.io/futures-rs/) 是 Rust 社群所提出的一個統一非同步操作的一個介面，大家可以實作一個統一的介面去描述一個尚未完成的事物，之前也有說過它像非同步的 `Result` ，它用起來像這樣：

```rust
extern crate futures;

use futures::{future, Future};

fn main() {
  // future::ok 會直接建一個完成，並且有結果的 Future
  let fut = future::ok::<i32, ()>(111).map(|x| x * 3);
  // wait 會執行一個 Future ，並等到它完成
  assert_eq!(fut.wait().unwrap(), 333);
}
```

> 如果你用過 Javascript 你可以想像 `future::ok` 就是 `Promise.resolve` ，不過它還有附很多操作的方法

其實目前在網站上看到的會是 0.3 的 alpha 版，但常用的卻是 0.1 版的，目前 futures 將會有部份整合進 Rust 的標準函式庫中，而之後 Rust 預計要支援 async 與 await ， 0.3 版的則是要跟 async 與 await 搭配使用的，若支援的話就能把非同步的程式碼寫的像同步的一樣：

```rust
#![feature(async_await, await_macro)]
extern crate futures;

use futures::future;
use futures::executor::block_on;

fn main() {
  block_on(async {
    let n = await!(future::ok::<i32, ()>(111)).unwrap();
    assert_eq!(n, 111);
  });
}
```

這邊要使用的 futures 是 `futures-preview = "0.3.0-alpha.4"` 。

你可以在 [`std::future`](https://doc.rust-lang.org/stable/std/future/index.html) 與 [`std::pin`](https://doc.rust-lang.org/stable/std/pin/index.html) 找到那些整合進來的 API ，當然目前都還是不穩定的。

Tokio
-----

Tokio 則是實作了非同步 IO 的框架，它提供了經過包裝的非同步的檔案與網路操作，同時還提供了執行 `Future` 的功能。

說來執行 `Future` 是怎麼一回事，事實上 `Future` 的實作就是透過輪詢 (poll) 來確定 `Future` 完成了沒，若還沒就讓它繼續跑，好了的話就把結果拿出來，所以若要自己來做這個工作就會變成：

```rust
extern crate futures;

use futures::{future, Async, Future};

fn main() {
    let mut fut = future::ok::<i32, ()>(111).map(|x| x * 3);

    loop {
      match fut.poll() {
        Ok(Async::Ready(res)) => {
          println!("{}", res);
          break;
        }
        Ok(Async::NotReady) => {
          println!("Not ready");
        }
        Err(e) => panic!(e),
      }
    }
}
```

> 記得把 futures 換回原本的 0.1 版

不要懷疑，這個是可以跑的，當然上面這個情況不可能會出現 `Not ready` 就是了。

所以事實上如果要執行一個 `Future` 問題就變成了，如果遇到了 `Async::NotReady` 程式是不是可以等一下，等到有結果了再做一次輪詢，而這個等一下又要等多久，而 tokio 的解決辦法就是使用系統底層的 API 實作 IO 的通知機制，若 futures 是從 IO 操作建立的就向系統註冊需要取得狀態改變的通知，而在有通知時再做一次輪詢。

> 這部份可能比較不好懂，但我想避免直接講到系統底層的 API ，如果有興趣可以看看 Linux 下的 [`epoll`](https://zh.wikipedia.org/zh-tw/Epoll)，還有 Rust 的 [mio](https://github.com/carllerche/mio) ， mio 正是 tokio 的核心， tokio 使用 mio 來包裝這些底層 API ，而 mio 則在 Linux 下使用 epoll 實作通知機制

若我們用 tokio 寫一個 echo server 的話會像這樣：

```rust
extern crate futures;
extern crate tokio;

use futures::{Future, Stream};
use std::net::SocketAddr;
use tokio::{
    io::{self, AsyncRead},
    net::TcpListener,
    runtime::current_thread::Runtime,
};

fn main() {
  let mut runtime = Runtime::new().unwrap();
  let handle = runtime.handle();
  let listener = TcpListener::bind(&SocketAddr::new("127.0.0.1".parse().unwrap(), 1234)).unwrap();
  let fut = listener
    .incoming()
    .for_each(|stream| {
      let (read, write) = stream.split();
      // 若要用 tokio::run 這邊要換成 tokio::spawn
      handle
        .spawn(
          io::copy(read, write)
            .map(|_| ())
            .map_err(|e| println!("{:?}", e)),
        )
        .unwrap();
      Ok(())
    })
    .map_err(|e| println!("{:?}", e));
  runtime.block_on(fut).unwrap();
  // 這邊其實使用 tokio 讓它用預設的方法執行 Future 會比較方便
  // 只是預設的方法會產生一些 thread ，我希望這邊可以用單執行緒完成
  // tokio::run(fut);
}
```

在 Linux 下你可以用 nc ， Windows 下可以用 telnet 來連線到 localhost:1234 ，你輸入什麼伺服器就會回應你什麼，這樣程式看起來其實不難，底層也變成使用事件驅動的方式執行。

上面的程式碼我放在： https://github.com/DanSnow/rust-intro/tree/master/echo-server

我個人是認為 tokio 與 futures 它們的 API 都很抽像，老實說不好理解，不過也有很多建立在這兩個函式庫上的東西幫它們做了很好的包裝，讓你可以解決特定領域的問題，比如像昨天所介紹的 tower-web ，它就讓你不需要去碰到 tokio 的細節部份。

另外如果想了解更多關於 tokio 與 mio 的底層的人可以參考這篇翻譯文章： [【譯】Tokio 內部機制：從頭理解 Rust 非同步 I/O 框架
](https://blog.techbridge.cc/2018/01/05/tokio-internal/)。

下一篇我們就來介紹 WebAssembly 。
