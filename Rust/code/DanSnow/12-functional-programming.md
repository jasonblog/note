函數式程式設計
==============

函數式程式設計的概念是來於數學上的函數，也就是一個輸入對應一個結果，不會受到其它東西的影響，所以程式講究沒有狀態，沒有副作用 (side effect) ，而在 Rust 中也融入了很多函數式程式的概念與設計，比如預設不能修改的變數，以及能直接當成參數傳遞的函式等等。

> 所謂的副作用是對函式外部的環境或狀態造成改變，所以像 OOP 那樣的修改 struct 甚至是對螢幕輸出都被視為副作用。

Vec
---

這邊先介紹一個 Rust 中的一個集合型態， `Vec` 又稱為 vector (中文直翻為向量，但實際用途不同，所以這邊採用原文) ， `Vec` 可以想成一個可以動態成長的陣列，它必須儲存同樣類型的資料，如果你有個不確定大小的連續資料，比起使用陣列來存，用 `Vec` 來存就會方便很多。

定義一個空的 `Vec`：

```rust
// 因為我們接下來並沒有使用到，所以 Rust 這邊沒辦法自動推導裡面儲存的型態
// 所以只能用 Vec::<i32>::new 這樣的方式指定型態
let v = Vec::<i32>::new();

// 或是指定型態在變數後面
let v: Vec<i32> = Vec::new();
```

直接定義一個有值的 `Vec`：

```rust
let v = vec![1, 2, 3];

// 這會定義一個有 10 個 0 的 Vec
let v = vec![0; 10];
```

> `vec!` 也是 macro ， 這邊使用的括號是 `[]` ，事實上 Rust 中並沒有規定 macro 最外層用來包住參數的括號要使用哪一種，所以 `()` 、 `[]` 或 `{}` 都是可以用的，只要是成對的就行，這邊使用 `[]` 只是慣例，其它的單行就使用 `()` ，多行的則使用 `{}` 。

在 `Vec` 的尾端增加元素：

```rust
let mut v = Vec::new();

v.push(1);
v.push(2);
v.push(3);

println!("{:?}", v);
```

這個結果會和上面的第一個一樣。

取值：

```rust
let v = vec![1, 2, 3];

// 這邊跟陣列一樣從 0 開始，若取超過範圍會 panic
let n = v[1];

// 這邊回傳的是 Option<&i32> ，若超過範圍只會回傳 None
let n = v.get(1);
```

基本的使用大概是這樣，接下來我們要用到不少 `Vec` 。

迭代 (Iterate)
--------------

還記得我們的 for 迴圈嗎？如果今天我們把陣列的每個元素都乘 2 並把結果存回一個 `Vec` 該怎麼寫呢？

```rust
let array = [1, 2, 3];
let mut vec = Vec::new();

// 這邊的 i 型態是 &i32
for i in array.iter() {
  vec.push(i * 2);
}

println!("{:?}", vec);
```

上面的 `.iter` 會回傳一個迭代器 (iterator) ，讓你可以用 for 迴圈跑過陣列的每個元素，但迭代器所能做到的不只是這樣，以函數式的做法的話會像這樣：

```rust
let array = [1, 2, 3];
// Vec<_> 可以這樣寫是因為中間的型態可以讓編譯器自動推導
let vec = array.iter().map(|x| x * 2).collect::<Vec<_>>();
// 如果你真的不喜歡 ::<Vec<_> 的語法也可以改用型態標註
// let vec: Vec<_> = array.iter().map(|x| x * 2).collect();
println!("{:?}", vec);
```

`map` 做的事是把每個元素都用其中的函式做轉換，再產生一個新的迭代器。

`collect` 則是把迭代器的值再搜集變成某個集合型態，注意的是這邊沒辦法使用陣列，因為它的大小必須在編譯時就決定，而 `collect` 只能使用能在執行時新增值的型態。

其中的 `|x| x * 2` 是接下來要介紹的閉包 (Closure) 的語法，它做的事情就是產生一個沒有名字的函式，也稱為匿名函式，將傳進來的參數乘 2 ，等下會詳細介紹語法與怎麼使用。

你也可以寫一個函式來做這件事：

```rust
// 這邊就像一般函式一樣要放型態了
fn time2(x: &i32) -> i32 { x * 2 }

let array = [1, 2, 3];
let vec = array.iter().map(time2).collect::<Vec<_>>();
println!("{:?}", vec);
```

> Rust 中在函式裡定義函式並不會出錯喔

如果要計算總合你會怎麼寫呢？請你試著用 for 迴圈寫一個看看吧。

寫好了嗎？我們可以用迭代器的 `sum` 來做加總：

```rust
let array = [1, 2, 3];
println!("{}", array.iter().sum::<i32>());
```

如果要找出 1 到 10 之間的偶數，使用迭代器該怎麼做呢？

```rust
println!("{:?}", (1..=10).filter(|x| x % 2 == 0).collect::<Vec<_>>());
```

range 本身就是迭代器了，所以直接呼叫方法就行了，這邊使用了 `filter` 來過濾出符合條件的元素。

Rust 的迭代器是延遲求值的，也就是只會使用到實際使用到的部份，所以如果使用一個有無限長度的迭代器，但只要只使用到有限的部份就不會出錯，讓我們來做高斯的經典題目吧：

```rust
println!("{}", (1..).take(100).sum::<i32>());
```

`1..` 會建一個從 1 開始一直到無限的範圍，但我們之後使用了 `take` 這使得它只會取前 100 個數字，最後再加總，你應該會看到它印出了 5050 。

> 如果你把 `take` 拿掉，它也不會是無窮迴圈就是了，因為電腦整數的大小是有限的， Rust 會避免發生整數溢位，也就是當超過整數上限時，發生了數字變負數的一種情況。

這邊介紹了一些迭代器的方法， Rust 中的迭代器其實挺快的，建議去看一下迭代器的文件，了解一下有哪些方法可以用。

迭代器 (Iterator)
-----------------

所以迭代器到底是什麼，我們實際來操作一次看看：

```rust
let array = [1, 2, 3];
// iter 中必須要記錄目前跑到哪個值，所以必須是 mut
let mut iter = array.iter();

println!("{:?}", iter.next()); // => Some(&1)
println!("{:?}", iter.next()); // => Some(&2)
println!("{:?}", iter.next()); // => Some(&3)
// 已經沒有值了
println!("{:?}", iter.next()); // => None
// 沒有值後再繼續呼叫並不會錯誤，而是一直回傳 None
println!("{:?}", iter.next()); // => None
```

上面的 `=>` 後的結果是印出來的結果，簡單來說迭代器就是每次呼叫 `next` 就會回傳一個 `Option` 並包含下一個值。

我們自己來做一個迭代器，讓它從 1 開始產生到指定的數字就停止，如果要做一個迭代器就必須要實作 `Iterator` 這個 trait ：

> `Iterator` 的文件在 [`std::iter::Iterator`](https://doc.rust-lang.org/stable/std/iter/trait.Iterator.html) ，在這邊還可以看到它提供了哪些方法。

```rust
use std::iter::Iterator;

#[derive(Debug, Clone, Copy, Default)]
struct UpToIterator {
  // 這邊都採用無號整數，因為要是有負數很麻煩
  current: u32,
  upper_bound: u32,
}

impl UpToIterator {
  pub fn to(upper_bound: u32) -> Self {
    UpToIterator { upper_bound, ..Default::default() }
  }
}

impl Iterator for UpToIterator {
  // 產生的值的型態
  type Item = u32;

  fn next(&mut self) -> Option<Self::Item> {
    if self.current < self.upper_bound {
      self.current += 1;
      Some(self.current)
    } else {
      None
    }
  }
}

fn main() {
  let mut iter = UpToIterator::to(10);
  for n in UpToIterator::to(10) {
    // 你應該會看到從 1 印到 10 的輸出
    println!("{}", n);
  }
}
```

重新認識 for 迴圈 (進階)
------------------------

其實 for 在 Rust 裡只是語法糖：

```rust
let array = [1, 2, 3];
for i in array.iter() {
  println!("{}", i);
}
```

這會被編譯器展開成：

```rust
let array = [1, 2, 3];
{
  // 這是一個編譯器產生的暫時的變數
  let mut _iter = array.iter().into_iter();
  while let Some(i) = _iter.next() {
    println!("{}", i);
  }
}
```

這邊可以看到我們使用了 `while let` 的語法，這跟 `if let` 很像，只是變成是如果還是 `Some` 的話就繼續執行。

`into_iter` 則是來自於 [`std::iter::IntoIterator`](https://doc.rust-lang.org/stable/std/iter/trait.IntoIterator.html) 這個 trait ， for 迴圈必須保證它後面的東西是個迭代器，所以會呼叫 `into_iter` 確保它被轉換成迭代器，相對而言，只要你的型態有實作 `IntoIterator` 就能被 for 迴圈所使用，要注意的是它會使用掉呼叫它的東西 (也就是它是使用 `self` 轉移了所有權) ，以下範例：

```rust
let v = vec![1, 2, 3];
// 這邊的 n 的型態是 i32
for n in v {
  println!("{}", n);
}
// 這邊沒辦法再使用 v
```

> Rust 中的慣例是若方法的開頭為 `into_` 則代表它會消耗掉使用它的東西。

閉包 (Closure)
--------------

以上面的例子：

```rust
|x| x * 2
```

宣告傳進來的參數 `|x|` ，在兩個 `|` 中放上參數的名字就好了，大多的情況下都不用加上型態宣告，這邊會自動推導，接下來放函式的主體，如果只有一行的話你可以不用加上大括號，或是加上大括號放進多行的程式。

閉包可以存到一個變數去：

```rust
let f = |x| x * 2;
println!("{}", f(10));
```

若你需要宣告型態的話：

```rust
let f = |x: i32| -> i32 { x * 2 };
```

只是這邊就一定要加上大括號了。

閉包可以取得區域變數：

```rust
let n = 3;
let f = |x| x * n;
println!("{}", f(10));
```

預設閉包為用唯讀 borrow 來取得外部的變數，如果加上 `mut` 宣告，則閉包會用可寫的 borrow 取得外部的變數：

```rust
let mut n = 0;
let mut counter = || {
  n += 1;
  n
};
println!("{}", counter());
println!("{}", counter());
println!("{}", counter());
```

若要讓閉包取得外部的變數的所有權，可以加上 `move` 關鍵字：

```rust
let v = vec![1, 2, 3];
let is_equal_v = move |a| v == a ;
println!("{}", is_equal_v(vec![1, 2, 3]));
println!("{}", is_equal_v(vec![4, 5, 6]));
// 這邊無法使用 v
```

至於要如何寫一個接受閉包的函式呢？

```rust
fn call_closure<F: Fn(i32) -> i32>(work: F) {
  println!("{}", work(10));
}

fn main() {
  call_closure(|x| x * 2);
}
```

Rust 中有三個代表函式與閉包有關的 trait 分別是：

- `FnOnce`: 這個代表它可能會消耗掉它取得的區域變數，所以它可能只能呼叫一次，這對應到上面使用了 `move` 的閉包
- `FnMut`： 這代表它會修改到它的環境，這對應了宣告 `mut` 的閉包
- `Fn`： 這是不會動到環境的閉包，對應到一般的閉包

這讓你可以視你的需求選擇使用哪一個，此外，位在列表上面的 trait 也可以接受位在它以下的 trait ，所以 `FnOnce` 也接受 `FnMut` 與 `Fn` ，而 `FnMut` 接受 `Fn` ，而三個也都接受一般的函式。

Rust 中也可以讓你傳進一般的函式，所以可以有以下的用法：

```rust
// 為 None 時建一個空字串
println!("{:?}", None.unwrap_or_else(String::new));

// 為 None 時用預設值
// 其實這個有 unwrap_or_default 可以用
println!("{}", Option::<i32>::unwrap_or_else(None, Default::default));

// 全部包進 Some 裡面
// Rust 中可以把 tuple struct 當函式用
println!("{:?}", vec![1, 2, 3].into_iter().map(Some).collect::<Vec<_>>());
```

上面那些大概知道就好了，主要是一些讓你可以少建立一個閉包的寫法。

下一篇來介紹智慧指標，與一些集合型態。
