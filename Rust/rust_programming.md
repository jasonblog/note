# [Rust] 程式設計教學



## 試用 Rust

如果只是想先嘗試一下 Rust 的語法，可到 [Rust Playground](https://play.rust-lang.org/) 網站，即可線上撰寫 Rust 程式，不需額外安裝其他的軟體。由於安全性考量，這個網站不能使用外部套件，只能用 Rust 內建的套件，對於初期的練習這樣的環境已經足夠。



## 安裝 Rust

Rust 本身是跨平台的程式語言，同時支援 Windows、Mac、Linux、BSD 等多種平台，目前安裝和管理 Rust 主要是透過 [rustup](https://rustup.rs/) 這隻終端機程式。`rustup` 的優點在於可以很輕鬆地在不同版本和平台的 Rust 編譯器間切換，對於更換不同 Rust 版本和交叉編譯 (cross-compiling) 相當方便。

在 Windows 下安裝 Rust

在 Windows 系統中，根據不同的 C 和 C++ 編譯器，對應不同的 Rust 版本，一個是使用 Visual Studio 的 MSVC ABI (Application Binary Interface)，一個是使用 GCC 編譯器的 GNU ABI，要使用那一個版本的 Rust，要看使用者想使用那個版本的 ABI 所建立的函式庫而定。

若讀者沒特別的偏好，建議以 GNU ABI 較佳，原因在於日後需要引用第三方 C 或 C++ 函式庫時，使用 GNU ABI 可以搭配 MinGW/MSYS2 所提供的第三方 C/C++ 函式庫。



### 在類 Unix 系統下安裝 Rust

對於 Mac 和 Linux 等類 Unix 系統，rustup 提供 shell 命令稿來安裝。在終端機輸入以下命令：

```
$ curl https://sh.rustup.rs -sSf | sh
```

`rustup` 會出現提示文字，可視需求自行調整，若沒有特別需求，接受預設選項即可。 `rustup` 和 Rust 編譯器都會安裝到 *$HOME/.cargo/bin* 資料夾，再將此路徑加入 **PATH** 變數即可。

## 使用 rustup 管理 Rust

透過 `rustup` 可管理多個版本的 Rust，相當方便，本節介紹 `rustup` 的使用方式。

Rust 有三個不同的版本，分別為 stable、beta、nightly，有一些實驗性質的特性，僅放在 nightly 版本的 Rust。預設情形下，rustup 會安裝 stable 版本的 Rust。輸入 `rustup update` 指令即可更新 Rust：

```bash
$ rustup update
info: syncing channel updates for 'stable'
info: downloading component 'rustc'
info: downloading component 'rust-std'
info: downloading component 'rust-docs'
info: downloading component 'cargo'
info: installing component 'rustc'
info: installing component 'rust-std'
info: installing component 'rust-docs'
info: installing component 'cargo'
info: checking for self-updates
info: downloading self-updates

stable-x86_64-unknown-linux-gnu updated - rustc 1.14.0 (e8a012324 2016-12-16)
```

如果要更新 `rustup`，輸入 `rustup self update`。

```bash
$ rustup self update
info: checking for self-updates
info: downloading self-updates
```

### 切換 toolchain

Rust 有許多針對不同 CPU、系統、C/C++ 函式庫的版本，透過 `rustup` 可快速地切換。先以 `rustup toolchain list` 列出所有可選的版本：

```bash
$ rustup target list
# Omit some message
x86_64-apple-darwin
x86_64-apple-ios
x86_64-pc-windows-gnu
x86_64-pc-windows-msvc
x86_64-rumprun-netbsd
x86_64-unknown-freebsd
x86_64-unknown-linux-gnu (default)
x86_64-unknown-linux-musl
x86_64-unknown-netbsd
```

[musl](https://www.musl-libc.org/) 是一套小型的 C 函式庫，可用來在 Linux 下建立靜態連結的機械碼。若想安裝以 musl 為基礎的 Rust，使用 `rustup target add`：

```bash
$ rustup target add x86_64-unknown-linux-musl
```

日後在編譯專案時，可指定特定的 target：

```bash
$ cargo build --target=x86_64-unknown-linux-musl
```

### 使用實驗性質的特性

某些尚在實驗性質、不穩定的特性，會在 nightly 版本的 Rust 發布。若想使用 nightly 版本的 Rust，輸入 `rustup install nightly`：

```bash
$ rustup install nightly
info: syncing channel updates for 'nightly'
info: downloading toolchain manifest
info: downloading component 'rustc'
info: downloading component 'rust-std'
info: downloading component 'rust-docs'
info: downloading component 'cargo'
info: installing component 'rustc'
info: installing component 'rust-std'
info: installing component 'rust-docs'
info: installing component 'cargo'
 
nightly-x86_64-unknown-linux-gnu installed - rustc 1.15.0-nightly (71c06a56a 2016-12-18)
```

安裝完後，可輸入 `rustup default` 切換 Rust 版本：

```bash
# Use nightly version
$ rustup default nightly
 
# Use stable version
$ rustup default stable
```

在實務上，除非需要某個穩定版本缺乏的語言特性之外，較不建議使用 nightly 版本的 Rust，會造成專案程式碼的不穩定。

由於 Rust 的函式庫是跨平台的，理論上，交叉編譯也是可行的。假若想要在 Linux 上交叉編譯某個 Rust 專案到 Mac 平台，在該專案的根目錄輸入以下指令：

```bash
# Say that we are on Linux now
$ cargo build --target=x86_64-apple-darwin
```

*註：經筆者實際測試，交叉編譯有時仍有問題需自行排除，和該平台的 C/C++ toolchain 有關。*

若該 Rust 專案只用到 Rust 程式碼，應該都可以在不同系統間編譯。若有用到第三方 C/C++ 函式庫，則需自行建置交叉編譯的環境。可參考 [crosstool-NG](http://crosstool-ng.org/) 或其他的方案來建立環境，但交叉編譯是較進階的主題，本書不詳述，請讀者自行查閱相關資料。

### 反安裝 Rust

如果因某些因素，決定不繼續使用 Rust，可以用以下指令反安裝 Rust 和 rustup：

```bash
$ rustup self uninstall
```

---



```rust
use std::io::stdin;
fn main(){
    let mut input = String::new();
    stdin().read_line(&mut input);
    println!("Hello, World.\n{}", input);
}
```



- 首先我們可以先發現在 Rust 裡頭，分號是必須的唷！這點跟很多 Modern languages 不同，你可以說有點麻煩，但好處是讓可讀性能夠增加，表示一個表達式的結束，因為 Rust 是 expression based (雖然我已經習慣沒有打分號的日子)。

    

- Rust 透過 `use` 來將其他的 Module的 Function 引入，讓我們在使用 Function 時可以不用將整個路徑寫出來。在 Rust 定義 Function 的關鍵字是 `fn` 。`let` 是做變數綁定，為什麼我特別說是變數綁定而不是傳統的變數賦值？這裡就要講到 Rust 為了記憶體的安全性，而有一個很重要的觀念就是 Ownership (所有權)。如果有學過 C/C++，相信對 Pointer 一定不陌生 (或是頭痛？)，而 C/C++ 因為讓你有很高的自由度可以操作 Pointer，所以就會有機會產生 Dangling pointer，也就是假設 A 和 B 都是指向同一個記憶體區塊的 Pointer，但是當 A 執行釋放記憶體後，想再用 B 去存取就會產生錯誤。而 Rust 則是引入了 Ownership 的觀念來解決這個問題。簡單來說，每份資料同一時間只會有一個擁有者，因此假使今天 A 是一個 Reference (可想成是 Pointer)，而我們 `let B = A` ，此時所有權就從 A 轉移到了 B 身上，A 就不再能夠存取對應的資料了。但如果 A 是例如一個整數，就不會有這個問題，因為此時的 `let B = A` ，Rust 會拷貝一份資料給 B，所以 A 還是可以存取，與 `Reference` 的情況不同。回到 A 是 Reference 的情況，假使把 A 傳入某個 Function 作為參數，那麼當該 Function 執行完之後，A 也不能存取了，因為在傳入的那刻，所有權就轉移到了該 Function 身上。為了解決這個問題，在 Rust 又有一個 Borrowing 的概念，也就是把所有權暫時借給該 Function，等到 Function 返回之後，所有權又回到 A 的身上，而實務上就是在傳入的時候加上`stdin().read_line(&mut input)` 的 `&` 囉! 最後就是 `&mut` 的 `mut` ，因為在 Rust，如果要讓轉移後可以有權去修改，那也要明確指出，所以 `&mut` 就是說明可以修改囉！(昏頭了嗎？哈！可以參考 [這裡](https://michaelchen.tech/rust-prog/ownership/) 有更多的說明)！

    

- `String::new()`的結果，會是一個新的 Empty string ，並且綁定在 input 這個變數。接著 `stdin()` 會回傳一個 stdin 的 struct，並且我們 invoke 其方法 `read_line` ，而這裡餵入的參數就是上面說明的 `&mut input` ，因為 input 這個變數必須要讓 `read_line` 這個 Function 拿到所有權並且修改，最後就是透過 `println!` 印出來囉！

    

- `println!`出現了一個驚嘆號，這代表說這不是一般的函式，而是 Rust 中的巨集，至於 Rust 的巨集又是什麼呢？與像是 C 的 Macro 不同，Rust 的 Macro 不僅僅是文字替換。在 Rust 中， Macro 是用來讓 Compiler 替我們生出程式，Compiler 會先將 Macro 展開，生成程式，再進行編譯。而且 Macro 不會受到一般函數的限制，甚至也不一定要用 `()` 來放入參數，可以使用像是 `[]` (像是 `vec![1,2,3]`) 或是 `{}` 都可以。在 Rust 中，一般函數是無法接受任意數量的參數的，也因此 `println!`勢必要是個 Macro 囉！

---



### 學習網站

- [令狐一沖](https://github.com/anonymousGiga/learn_rust/)
- [Rust 程式設計語言](https://rust-lang.tw/book-tw/#rust-程式設計語言)
- [通過例子學 Rust 中文版](https://rustwiki.org/zh-CN/rust-by-example/)
- [RustPrimer](https://rustcc.gitbooks.io/rustprimer/content/)
- [Rust學習筆記](https://skyao.io/learning-rust/)

---

