macro 、 proc macro 與尾聲
==========================

這是最後一篇了，這篇我會補一些之前沒講到的東西，內容會比較雜一點。

目錄：

- macro
- Procedural Macro (進階)
- 權限修飾
- Rust 心得
- 鐵人賽後記

macro
-----

Rust 的 macro 同樣也是基於模式比對的，如果你發現了類似結構的程式碼重覆了，說不定你可以把它寫成 macro 來減少你的程式碼，比如像昨天使用的 `clone!` ：

```rust
macro_rules! clone {
  ($($name:ident),*) => {
    $(
      let $name = $name.clone();
    )*
  };
}
```

定義一個 macro 你需要使用 `macro_rules!` 接著你想要的 macro 名稱，名稱的規則跟變數一樣，這邊使用的是 `clone` ，之後你的 macro 就會變 `clone!` 。

接下來則是分成兩個部份，模式定義的部份與產生的程式碼，兩邊都必須使用成對的括號包住，中間使用 `=>` 分隔，最後則要用 `;` 結束 (只有一個定義時可以不用)，左邊的模式幾乎可以隨便定義，只是還是有些規則在，主要是不能造成編譯器出現岐義的情況，也就是同一段程式碼能有兩種不同的解釋，像 macro 中能用 `expr` 去比對運算式，但是運算式後就規定不能出現運算子 (+ 、 - 、 * 、 / 等等) 。

上面的 `$name:ident` 代表的是比對一個識別字 (變數或 struct 的名字等等)，前面的 `$name` 是 macro 中使用的變數，在展開後 `$name` 會被代換成傳入的東西，後面的 `ident` 則代表是要比對識別字，可以比對的東西有這些：

- item： 各式各樣的完整定義與宣告都是 item ，比如一個 struct 的定義
- block： 一個程式碼區塊，就是由一對 `{` 和 `}` 包起來的區塊
- stmt： 一個程式敘述，就是一行有加 `;` 的程式碼
- pat： 一個模式比對用的模式，如 `Point { x, y }`
- expr： 一個運算式，基本上就是一行沒加括號有回傳值的程式，這個挺常用的
- ty： 一個型態
- ident： 一個識別字，或一個關鍵字也行，這個也是常用的
- path： 比如像 `std::vec::Vec` 這樣的
- meta： `#[]` 中的內容
- lifetime： 就是 lifetime
- vis： 像 `pub` 這樣的設定可見性的東西
- tt： 可以 match 任何東西，通常是用來收集還沒比對到的部份用的

比如我可以定義：

```rust
macro_rules! foo {
  ($left:ident =>____<= $right:expr) => {
    println!("{} =>____<= {}", stringify!($left), $right);
  };
}

// 使用：

foo!(bar =>____<= 123);
// 會印出： "bar =>____<= 123"
```

其中 `stringify!` 是個內建的 macro ，可以把傳進去的東西原封不動的轉成字串，沒錯， macro 可以遞迴展開。

如果要重覆的比對的話就要用 `$()*` 或 `$()+` 了， `+` 是比對 1 個以上，而 `*` 則是可以為 0 個，如果我們把上面的範例加上 `$()*`：

```rust
macro_rules! foo {
  ($($left:ident =>____<= $right:expr)*) => {
    $(
      println!("{} =>____<= {}", stringify!($left), $right);
    )*
  };
}

// 使用：

foo!(bar =>____<= 123 baz =>____<= 456);
// 會印出：
// bar =>____<= 123
// baz =>____<= 456
```

用哪種重覆的方式就要用哪種去展開，在結尾的 `*` 或 `+` 前可以放上分隔用的符號，通常用會 `,` 或 `;`：

```rust
macro_rules! foo {
  ($($left:ident =>____<= $right:expr),*) => {
    $(
      println!("{} =>____<= {}", stringify!($left), $right);
    )*
  };
}

// 使用：

foo!(bar =>____<= 123, baz =>____<= 456);
// 會印出：
// bar =>____<= 123
// baz =>____<= 456
```

Rust 的 macro 很強大，又能遞迴展開，使用的好能做出各式各樣的效果，比如：

```rust
macro_rules! foo {
  // 遞迴的終止條件
  ($left:ident -> $right:expr) => {
    println!("{} -> {}", stringify!($left), $right);
  };

  // 遞迴的終止條件
  ($left:ident => $right:expr) => {
    println!("{} => {}", stringify!($left), $right);
  };

  ($left:ident -> $right:expr, $($rest:tt)+) => {
    println!("{} -> {}", stringify!($left), $right);
    foo!($($rest)+);
  };

  ($left:ident => $right:expr, $($rest:tt)+) => {
    println!("{} => {}", stringify!($left), $right);
    foo!($($rest)+);
  };
}
```

這樣就能支援兩種不同的分隔。

也有人在 macro 裡做出狀態機：

```rust
macro_rules! foo {
  ($left:ident => $right:expr) => {
    foo!(@end $left => $right);
  };

  ($left:ident => $right:expr, $($rest:tt)+) => {
    println!("{} => {}", stringify!($left), $right);
    foo!(@second $($rest)+);
  };

  (@second $left:ident => $right:expr, $($rest:tt)+) => {
    println!("{} => {}", stringify!($left), $right);
    foo!(@end $($rest)+);
  };
  
  (@end $left:ident => $right:expr) => {
    println!("{} => {}", stringify!($left), $right);
  };

  (@end $left:ident => $right:expr, $($rest:tt)*) => {
    println!("{} => {}", stringify!($left), $right);
  };
}
```

這會讓只能比對到 1 組或 3 組以上，而且超過第 3 組後都會被忽略，至於使用 `@` 只是個慣例，加上基本上程式裡不會出現。

另外還可以搭配 trait 來實作一些針對型態的特化等等的。

Procedural Macro (進階)
-----------------------

這個功能就是像 `derive` 所使用的 trait ，或是可以自訂屬性 (像 `#[foo]`) ，社群有提供兩個很好用的 crate 可以幫忙實作，分別是 [`syn`](https://github.com/dtolnay/syn) 與 [`quote`](https://github.com/dtolnay/quote)。

這個專案在： https://github.com/DanSnow/rust-intro/tree/master/proc-macro-demo

假設我們有個 trait 是定義一個函式回傳 struct 的名字：

```rust
trait Name {
  fn name() -> &'static str;
}
```

然後我們把它變成可以 derive 的，於是我們必須建一個額外的函式庫專案，並在 `Cargo.toml` 中加上：

```toml
[lib]
proc-macro = true
```

這樣編譯器才會知道這個 crate 是 proc macro 然後實作的程式碼如下：

```rust
#[macro_use]
extern crate quote;
extern crate proc_macro;
extern crate proc_macro2;
extern crate syn;

use self::proc_macro::TokenStream;

// 定義一個 derive 名稱為 Name
#[proc_macro_derive(Name)]
// 這個函式的輸入輸出是規定的
pub fn name_derive(input: TokenStream) -> TokenStream {
    let ast = syn::parse(input).unwrap();
    impl_name(&ast).into()
}

fn impl_name(ast: &syn::DeriveInput) -> proc_macro2::TokenStream {
  // 取得 struct 的名稱
  let name = &ast.ident;
  // 轉成字串，這樣才會在 quote 裡是 "<名稱>" 的型式
  let name_str = name.to_string();
  quote! {
    impl ::name::Name for #name {
      fn name() -> &'static str {
        #name_str
      }
    }
  }
}
```

使用像這樣：

```rust
#[macro_use]
extern crate name_derive;
extern crate name;

use name::Name;

#[derive(Name)]
struct Foo;

fn main() {
  println!("{}", Foo::name());
}
```

權限修飾
--------

之前提過用 `pub` 可以公開模組中的東西，事實上並不單只有這樣的用法：

- `pub(crate)`： 讓這個東西能在這個 crate 中使用，但離開這個 crate 就不能
- `pub(in <模組路徑>)`： 只再開放給指定的模組，路徑也可以用 `self` 與 `super`

此外也可以把 `pub(in super)` 寫成 `pub(super)` ， `self` 也同樣，不過其實寫 `pub(self)` 就相當於是預設的，還有路徑只能是上層的路徑，下層的原本就都可以存取的。

使用 Rust 的心得
----------------

當初知道 Rust 這個語言是因為火狐說要用這個語言重寫他們的引擎，那時就去查了一下，記得那個時候的第一印像是：「這什麼鬼， struct 跟 impl 的定義是分開的？！」，不過後來嘗試後慢慢發現，這個語言雖然是個系統程式語言，但寫起來卻很方便，我在那之前主要使用的是 C++ ，接觸 Rust 後我最喜歡的是它的型態推導 (這在 C++ 裡可以用 auto)，跟它的模式比對 (這遠遠超過了 C++ 的 switch 了)，還有一些 FP 的特性，這是在 C++ 中比較缺乏，最重要的是 Rust 有強大的社群支援，還有 cargo 這個套件管理工具， C++ 若想用一些第三方的東西真的會比較麻煩，而且在 Rust 中大量的使用了 RAII ，我可以不用擔心忘記釋放任何東西 (反倒要擔心東西不小心被釋放了，但「基本上」也不會發生) ，雖然剛開始被 lifetime 搞的很頭痛，但習慣後效率真的很高。

鐵人賽後記
----------

之前就一直想參加鐵人賽了，但是要寫什麼好呢？一直拿不定主意，介紹前端的 React 與 Vue 已經一堆了，競爭感覺很激烈，雖然我對於自己對這兩個東西的熟悉程度有自信，包含對它們底層的實作多多少少都有了解，我不只對新的東西感興趣，還對它們背後怎麼實現的感興趣，所以我覺得開源程式真是個好東西，多虧了開源，我去翻了 Vue 的 observer 的實作，去翻了 React 怎麼處理 event ，去翻了 ptrace 怎麼 trace system call 之類的，稍微扯遠了，但我對寫好文章沒什麼自信，我沒什麼寫文章的習慣的，那就挑個冷門點的題目吧， Rust 這個語言也急速的在發展，或許以後會有不少人來用它寫些需要速度，或是偏底層的程式吧。

第一篇文章出來後，如果有編輯紀錄你應該會看到我反覆修了好幾次，那時貼出第一篇後拿給我朋友看，朋友：「我還以為你會先排版再貼上去」，於是我反覆修正了排版與用詞，雖然我從一開始就知道我的文章不適合初學者當教材了，我自己是覺得我的我文章像 Rust By Examples ，之前有位教授這樣說：「學生總是很困惑，老師在講什麼怎麼都聽不懂。老師也很困惑，學生到底為什麼不懂。」，一旦學會了什麼東西就會覺得很簡單，要再來教人就不容易了，雖然我也挺喜歡教人的，但有時我不知道自己的教法是不是正確的。

鐵人賽真的給了我個不錯的機會，有理由讓自己去寫文章，而且到後面還每天寫個專案(X ，其實除了網頁後端與資料庫那兩篇的專案都有事先準備外，其它的專案都是當天或前一天開始趕工做出來的，至於文章除了前幾篇外也都是當天新鮮的，所以那天如果到 10 點以後才發文大概是我那之前在除錯，這週花在鐵人賽的文章與相關的專案上的時數接近完整的 1 天啊，這次鐵人賽也讓我接觸到了兩樣之前沒碰過的，一個是 WebAssembly ，另一個是用 Rust 寫 GTK 程式 (之前只有用 C 寫過)。

最後一篇好像大多數人都會直接寫心得了，不過我還是有附上一點教學，因為理論上我的心得通常不會寫太多字，不過這個後記居然有將近 800 字其實我也是有點驚訝，要是我以前寫讀書心得也能寫的這麼順就好了。

總之謝謝正在看文章的你，以及之前提供我建議的讀者，拿 GTK 做井字棋可說完全是因為那則留言而產生的。

對了，原本我還想在這篇示範怎麼發佈個 crate ，然後把之前寫的那個 hastebin-client 發上去的，不過我還是先把程式碼整理一下吧。
