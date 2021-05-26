Cargo
=====

這篇要介紹的是 Rust 的套件管理工具 Cargo 以及套件倉庫 [crates.io](https://crates.io) 目前 crates.io 上有一萬八千多個套件，很多功能你都可以在上面找到別人幫你寫好的套件。

在 Rust 要使用別人寫好的套件你需要直接修改 `Cargo.toml` 這個檔案，把套件的名字以及你需要的版本加進去，但無需擔心，它的寫法很簡單的，以下是個範例：  

```toml
[package]
name = "guess"
version = "0.1.0"
authors = ["DanSnow"]

[dependencies]
rand = "0.5.5"
```

> `Cargo.toml` 跟 Node.js 的 `package.json` 的用途很像。

我們來建立我們的第一個專案吧，請打開終端機輸入以下的指令：

```shell
$ cargo init guess
```

這時你的目錄下應該有個名稱為 `guess` 的資料夾並且你可以看到底下的檔案與資料夾：

```plain
guess
├── Cargo.toml
├── .gitignore
└── src
    └── main.rs
```

> `.` 開頭的檔案或目錄在類 Unix 系統下是隱藏檔，如果你使用的是 Ubuntu 預設的檔案管理員的話你可以按 `Ctrl+H` 顯示所有檔案。你也可以使用指令 `ls -a` 顯示檔案。

其中 `src/main.rs` 就是我們接下來要修改的專案主程式的程式碼了，你會看到裡面已經有 Hello world， `Cargo.toml` 也已經幫你填好了基本的設定， 我們現在試著輸入 `cargo run` 來執行看看：

```shell
$ cargo run
   Compiling guess v0.1.0 (guess)
    Finished dev [unoptimized + debuginfo] target(s) in 0.52s
     Running `target/debug/guess`
Hello, world!
```

這次要做的是個終極密碼的小遊戲，主要遊戲方式是由使用者輸入數字，電腦回答是比答案大還是小，若沒猜中則猜到中為止的遊戲，接下來我們會一邊介紹基本語法一邊完成這個遊戲，不過我們先幫我們的專案加上一個套件吧，打開 `Cargo.toml` ，找到有一行為 `[depeendencies]` ，在它底下加上：

```toml
rand = "0.5.5"
```

這個套件的功能是產生隨機的數字，如果沒有它，我們的小遊戲每次的答案就要一樣了。

不過像這樣手動編輯檔案也挺容易出錯的，我們有個更好的方法，請你輸入底下的指令：  

```shell
$ cargo install cargo-edit
```

這會幫 cargo 擴充新的功能，現在我們可以用底下的指令來加上套件了：  

```shell
$ cargo add rand
```

是不是方便多了？

下一篇要來快速的講解 Rust 的基本語法
