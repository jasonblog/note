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

