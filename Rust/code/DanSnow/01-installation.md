Installation
============

在正式開始教學前，我們要先把環境安裝好，請在終端機輸入以下指令：

```shell
$ curl https://sh.rustup.rs -sSf | sh -- -y
```

> `$` 是代表終端機的提示符號，你並不需要輸入這個符號

另外 Rust 需要 GCC 之類的工具，如果您的環境是 Ubuntu，可以直接用以下指令安裝：

```shell
$ sudo apt-get install -y build-essential
```

如果您的環境是 Windows，請至 https://rustup.rs 下載 rustup-init 來安裝，而因本教學會使用到相當多指令，我會建議使用 [cmder](http://cmder.net) 來取代原本 Windows 內建的 cmd，或是直接使用 Visual Studio Code（VSCode）。

完成以上步驟後，您的電腦現在應該已安裝了以下程式：

- rustup: Rust 本身的安裝軟體及版本管理以及更新系統
- rustc: Rust 的編譯器
- cargo: Rust 的套件管理器

剛安裝完，為了確保環境變數有正確設定，你需要在終端機執行下方指令：

```shell
$ source $HOME/.cargo/env
```

而在你下次登入後 Rust 應該已經幫你自動設定好環境變數了，你就不需要再執行上面那條指令。  

接著，可以使用 rustc --version 來查看已安裝的版本，如有正確顯示目前的版本，就代表安裝好囉，在本文撰寫時穩定版為 1.29.1。

一切都安裝設置好後，不免的，我們來寫個「Hello, World!」吧！開啟喜歡的編輯器並輸入以下程式碼：

```rust
fn main() {
    println!("Hello world");
}
```

> 這邊的 `!` 並不是打錯了， `println!` 是在一起的，在 Rust 裡以 `!` 結尾的東西是巨集 (macro) ，現在只需理解 `println!` 在背後會幫你產生一些程式碼，讓你可以用簡單的方式就完成印到螢幕這個動作，之後會介紹如何自己寫巨集。

完成後將程式存檔為 hello.rs ，並在終端機執行下方指令：  

```shell
$ rustc hello.rs
```

沒意外的話在同一個資料夾下將會看到名稱為 hello 的執行檔（Windows 下為 hello.exe）， 在終端機下執行：

```shell
Hello world
```

恭喜您，您已完成了第一個 Rust 程式，從下一篇開始，我們將正式開始旅程，不過在這之前，讓我們安裝幾個好東西，這會讓我們接下來更佳順利。

```shell
$ rustup component add rls-preview rustfmt-preview
```

最後的最後，Rust 有個線上的測試環境：https://play.rust-lang.org ，並且還安裝好了很多常用的套件，如果臨時有什麼想測試的可以直接在這個網站使用。

下一篇我們會介紹 Cargo 這個 Rust 的套件管理與建置工具，還有 https://crates.io ，載個套件下來玩玩，並介紹基本的輸入與輸出。
