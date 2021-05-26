Channel & Crossbeam
===================

今天要來介紹 Atomic 與 Channel ，另外還會介紹 crossbeam 這個 crate 。

> 這篇的範例也都請在自己的電腦上測試。

Atomic
------

還記得我們在上一篇時使用了 mutex 來保護我們的數字的讀寫嗎？今天要介紹的是 atomic ，它保證操作不會因為多執行緒中斷，所以可以安全的讀寫，而不需要 mutex ，它的文件在 [`std::sync::atomic`](https://doc.rust-lang.org/std/sync/atomic/index.html) ，我們把昨天的範例用 atomic 重寫一份看看：  

```rust
use std::thread;
use std::sync::{Arc, atomic::{AtomicUsize, Ordering}};
use std::time::Duration;

fn main() {
  let data = Arc::new(AtomicUsize::new(0));
  let mut children = Vec::new();
  let one_sec = Duration::from_secs(1);
  for i in 0..4 {
    let data = data.clone();
    children.push(thread::spawn(move || loop {
      let n = data.fetch_add(1, Ordering::SeqCst);
      // 如果大於 10 就結束
      if n >= 10 {
        println!("Thread[{}] exit", i);
        break;
      }
      println!("Thread[{}] data: {}", i, n);
      // 模擬處理的耗時
      thread::sleep(one_sec);
    }));
  }
  // 等所有執行緒結束
  for child in children {
    child.join().unwrap();
  }
}
```

這個範例其實和昨天的執行結果不太一樣，首先，我們資料的值是有可能超過 10 的，再來它不會像昨天的一樣照著順序了，因為昨天的輸出也在 mutex 的保護範圍內，但這次有保證的只有數值的增加而已，輸出的順序是沒有任何保證的，關於 atomic 的 ordering 建議可以自己再上網找相關的資料，畢竟這個還挺複雜的，怕用錯的話還是用 mutex 就好了。

Channel
-------

Channel 可以跨執行緒傳遞資料，大多的用途是主執行緒用來分配工作給子執行緒，文件在 [`std::sync::mpsc`](https://doc.rust-lang.org/std/sync/mpsc/index.html) ， Rust 中內建的 channel 是支援多個發送端，但只能有單個接收端：  

```rust
use std::{
    io::{stdin, BufRead},
    sync::mpsc::channel,
    thread,
};

fn main() {
  // tx 是發送端， rx 是接收端
  let (tx, rx) = channel();
  let handle = thread::spawn(move || loop {
    match rx.recv() {
      Ok(val) => {
        println!("收到 {:?}", val);
      }
      // 出錯時離開迴圈
      Err(_) => break,
    }
  });
  for line in stdin().lock().lines() {
    let line = line.unwrap();
    // 把輸入送過去
    tx.send(line).unwrap();
  }
  // 關掉發送端，這會讓接收端的 recv 得到 Err
  drop(tx);
  // 等待子執行緒結束
  handle.join().unwrap();
}
```

> 如果你有用過 Go 的話你應該知道 Go 內建的 channel ， Rust 的跟 Go 的 channel 的也挺像的，只是並不像 Go 的一樣可以有多個接收端，所以用 mutex 保護接收端也是有的。

Channel 在多執行緒上非常的方便，可是 Rust 的標準函式庫所提供的 channel 只能支援單個接收端，也不支援同時處理多個接收端，看哪個的訊息先到 (目前這個功能還沒穩定) ，所以就有人做了 crossbeam 這個 crate ，它提供很多多執行緒下會使用到的東西，可以說是補足了 Rust 標準函式庫不足的部份。

crossbeam
---------

crossbeam 實際上不只是一個 crate ，其底下還分成 `crossbeam-epoch` 、 `crossbeam-deque` 、 `crossbeam-channel` 、 `crossbeam-utils` ，這次主要要介紹的東西在 `crossbeam-channel` 與 `crossbeam-utils` ，不過為了方便，我們還是使用 `crossbeam` 這個 crate 吧。

> 以下的程式使用的是 crossbeam `0.5`

crossbeam channel
-----------------

```rust
extern crate crossbeam;

use std::{
  io::{stdin, BufRead},
  thread,
};
use crossbeam::channel::unbounded;

fn main() {
  // 建一個沒有大小限制的 channel
  let (tx, rx) = unbounded();
  let mut children = Vec::new();
  // 這次建立了 4 個執行緒來展示 crossbeam 能支援多個接收端
  for i in  0..4 {
    let rx = rx.clone();
    children.push(thread::spawn(move || loop {
      match rx.recv() {
        Ok(val) => {
          println!("Thread[{}]: 收到 {:?}", i, val);
        }
        Err(_) => break,
      }
    }));
  }
  let stdin = stdin();
  for line in stdin.lock().lines() {
    let line = line.unwrap();
    tx.send(line).unwrap();
  }
  drop(tx);
  for handle in children {
    handle.join().unwrap();
  }
}
```

crossbeam 的 channel 比起標準函式庫裡的要強大的多了。

> 如果你用過 Go 的話， crossbeam 的 channel 比較像 Go 的 channel 。

Scoped Thread
-------------

一般而言 thread 可以在背景執行，只要主執行緒沒有結束，子執行緒也可以繼續執行下去，在 Rust 裡要是弄丟了 JoinHandle (`thread::spawn` 的傳回值) ， 執行緒就會脫離掌握了，除非主執行緒結束不然是不會停止的，也代表 Rust 的執行緒可以離開建立它的函式繼續執行，因此 Rust 中的執行緒若要使用 borrow 就必須要有 `'static` 的 lifetime ，若要使用函式中的 borrow 就要用到 `Box` 或 `Arc` 來確保子執行緒能拿到合法的 borrow ，或著，如果有種執行緒能夠保證在函式結束時一起結束，而能拿到函式中的 borrow 就好了。

```rust
extern crate crossbeam;

use std::io::{stdin, BufRead};
use crossbeam::{thread, channel::unbounded};

fn main() {
  let (tx, rx) = unbounded();
  thread::scope(|scope| {
    for i in  0..4 {
      let rx = rx.clone();
      // 改呼叫 scope 上的 spawn
      scope.spawn(move |_| loop {
        match rx.recv() {
          Ok(val) => {
            println!("Thread[{}]: 收到 {:?}", i, val);
          }
          Err(_) => break,
        }
      });
    }
    // 要把外面的讀 stdin 移進來，不然不會被執行到而導致程式卡住
    for line in stdin().lock().lines() {
      let line = line.unwrap();
      tx.send(line).unwrap();
    }
    drop(tx);
    // 所有的 thread 會在離開 scope 時 join
    // 所以 lifetime 只需要在這個範圍有效就行了
  }).unwrap();
}
```

這兩章的內容需要你對執行緒有點基本的了解，希望你還能夠理解，這已經是第 17 篇了， Rust 做為一門系統程式語言，接觸到一些電腦、作業系統的基本概念也是不可免的，雖然這兩章的內容對初學程式來說並不是那麼的必要，到這邊我有點好奇各位為什麼想學習 Rust 這門程式語言，如果你完全沒有電腦的基礎概念就來學這門語言我想應該會很辛苦，如果你單純用過 C/C++ 這類的程式語言的話或許會好一點，不知道各位在讀這兩章之前，知不知道 data racing 是什麼呢？

下一篇我們來講所謂「不安全」的 Rust ，在保證安全的 Rust 中為了安全總是犧牲掉了點彈性，而我們要來使用那些不安全的功能，當然，使用了這些功能 Rust 就沒辦法保證你的程式是安全不會有記憶體錯誤，就跟你拆開保固中的東西一樣，要自己負責。
