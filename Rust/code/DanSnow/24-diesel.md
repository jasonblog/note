Diesel
======

在開始之前，我有個想講的東西，不過你也可以跳過這段直接看底下的正文。

昨天的程式碼中的第 68 行所出現的：

```rust
&matches
  .value_of("HOST")
  .map(ToOwned::to_owned)
  .or(env::var("HASTE_SERVER").ok())
  .unwrap_or_else(|| URL.to_owned())
```

老實說，後來想想我不太喜歡這個部份，昨天因為 `env::var` 回傳的是 `String` ，而另外兩個是 `&str` ，為了通過 Rust 的編譯所以必須讓型態統一，而我的做法是把全部都轉成 `String` ，雖然是在有值的時候才轉換，但傳入網址的地方所要的只是 `&str` 而已，也就是若建了 `String` 也只是存活到這個函式呼叫結束而已，如果可以我想讓 `&str` 保持 `&str` 就好，但這有可能嗎？有的，這時候就是 [`std::borrow::Cow`](https://doc.rust-lang.org/stable/std/borrow/enum.Cow.html) 出場了。

`Cow` 也算是個智慧指標，不知道各位有沒有聽過「寫入時複製 (copy on write)」，簡單來說就是個在發生修改時將內容複製一份來避免修改到原本的內容，同時又可以減少資源的消耗 (若沒發生寫入就沒必要複製)，而 `Cow` 正是這個功能在 Rust 中的實現，你可以把一個唯讀的 borrow 給它，然後在有必要修改時呼叫 `to_mut` ， `Cow` 會把 borrow 的值複製一份讓你修改，但若已經複製過了，那就不需要再複製了。

不過這有什麼關係呢？有的， `Cow` 是個列舉，它有兩個 variant 分別是 `Borrowed` 與 `Owned` ，各別代表借來的資料與自己擁有的資料，注意到了嗎？它幫 borrow 的資料與擁有的資料提供了一個統一的介面啊，於是我們把上面那份程式碼中的字串們都用 `Cow` 包裝起來：

```rust
&matches
  .value_of("HOST")
  .map(Cow::from)
  .or(env::var("HASTE_SERVER").ok().map(Cow::from))
  .unwrap_or(Cow::from(URL))
```

這樣感覺好多了， `&str` 轉換成 `String` 必須要把字串的內容複製一份，而 `Cow` 只會多出一點點的空間耗用就可以同時相容 `&str` 與 `String` 了，寫這種系統程式語言就實在是會忍不住去在意這種消耗記憶體的事啊，此外這邊的程式碼已經更新上去了。

Diesel
------

這次的程式碼在： https://github.com/DanSnow/rust-intro/tree/master/message-board

今天要介紹的是 [Diesel](http://diesel.rs/) ，這是個 Rust 的 ORM 與 Query Builder ，它支援 pgsql 、 mysql 與 sqlite ，並能在編譯時就檢查出部份的語法錯誤 (比如使用到了該資料庫不支援的功能)。

> ORM 的中文翻釋是「物件關聯對應」，原本是指將不同系統中的資料對應到程式語言中的物件，不過現在很多都已經變成指這種能連接資料庫，並把查詢結果變成物件的函式庫了。

在開始使用前要先安裝 Diesel 的工具，請輸入以下指令：

```shell
$ cargo install diesel_cli
```

> 預設它會開啟所有能支援的資料庫，若你只需要它支援部份的資料庫可以用以下指令

```shell
$ cargo install diesel_cli --no-default-features --features sqlite
```

> 本篇教學只會使用到 sqlite ，若你還想要支援 mysql 可以用逗號隔開 `--features sqlite,mysql` ，另外也有 `postgres` ，此外安裝時還會需要對應的系統函式庫，比如若要支援 sqlite 在 Ubuntu 下就要安裝 `libsqlite3-dev` 。

設定專案
--------

然後來建立一下專案，這次我們來做一個留言板，不過今天只是先介紹資料庫的使用部份：

```shell
$ cargo new message-board
$ cd message-board
$ cargo add dotenv diesel
```

接著我們需要修改一下 `Cargo.toml` ，把 diesel 的那行改成如下，開啟 sqlite 的支援：

```toml
diesel = { version = "1.3.3", features = ["sqlite"] }
```

接下來設定資料庫的位置，建一個叫 `.env` 的檔案後加入一行：

```shell
DATABASE_URL=db.sqlite
```

之後執行：

```shell
$ diesel setup
```

到這邊我們應該會看到 diesel 已經幫我們建好了資料庫的檔案 `db.sqlite` 與一個資料夾 `migrations` 還有一個設定檔 `diesel.toml` ， `migrations` 這個資料夾是用來放建立與修改資料表的檔案用的。

Migration
---------

我們先建一個存貼文的表格吧：

```shell
$ diesel migration generate create_posts
```

它會在 `migrations` 的資料夾下建立一個以日期、一組號碼與 `create_post` 命名的資料夾，在底下會有兩個檔案， `up.sql` 與 `down.sql` 分別為建立的 SQL 與撤消的 SQL ，我們先在 `up.sql` 中寫入建立資料表的指令：

```sql
CREATE TABLE posts (
  id INTEGER NOT NULL PRIMARY KEY,
  author VARCHAR NOT NULL,
  title VARCHAR NOT NULL,
  body TEXT NOT NULL
);
```

然後在 `down.sql` 中寫入刪除資料表的指令：

```sql
DROP TABLE posts;
```

接著執行：

```shell
$ diesel migration run
```

它會執行剛剛寫好的 SQL ，同時也會更新 `src/schema.rs` 這個檔案，你可以打開來，應該會看到以下內容：

```rust
table! {
  posts (id) {
    id -> Integer,
    author -> Text,
    title -> Text,
    body -> Text,
  }
}
```

這個檔案是記錄目前的資料表結構， diesel 指令會幫你維護這個檔案，在編譯時會靠這個檔案來建立關於資料庫的查詢、新增、修改等等的程式碼，我們先在 `src/main.rs` 裡把它引入吧：

```rust
#[macro_use]
extern crate diesel;

mod schema;

fn main() {}
```

到這邊程式應該可以編譯執行，雖然會有一堆警告，不過那沒關係。

連線資料庫
----------

使用資料庫的第一步當然是跟資料庫做連線，不過其實我們用的是 sqlite ，只是要開個檔案而已：

```rust
#[macro_use]
extern crate diesel;
extern crate dotenv;

use diesel::{prelude::*, sqlite::SqliteConnection};
use dotenv::dotenv;
use std::env;

mod schema;

fn establish_connection() -> SqliteConnection {
  let url = env::var("DATABASE_URL").expect("找不到資料庫位置");
  SqliteConnection::establish(&url).expect("連線失敗")
}

fn main() {
  dotenv().ok();
  establish_connection();
}
```

使用 diesel 建立連線很簡單，只要呼叫對應的連線物件的 `establish` 並傳入資料庫的位置就行了。

建立 Model
----------

所謂的 ORM 就是把資料庫的資料與這些被稱為 model 的物件對應，我們要來建立兩個 model ，一個查詢用，一個新增用，建一個 `src/models.rs` 檔案然後輸入以下內容：

```rust
// Insertable 產生的程式碼會使用到，所以必須要引入
use super::schema::posts;

// 一個可以用來查詢的 struct
#[derive(Queryable, Debug)]
pub struct Post {
  pub id: i32,
  pub author: String,
  pub title: String,
  pub body: String,
}

// 新增用的 struct ，唯一的差別是沒有 id 的欄位，以及使用的是 str
#[derive(Insertable)]
// 這邊要指定資料表的名稱，不然 diesel 會嘗試用 struct 的名稱
#[table_name = "posts"]
pub struct NewPost<'a> {
  pub author: &'a str,
  pub title: &'a str,
  pub body: &'a str,
}
```

新增資料
--------

我們先來新增資料，這樣等下才有資料可以查詢，修改 `main.rs` ，把剛剛建好的 model 引入，接著像這樣輸入 (以下程式碼省略了部份) ：

```rust
// ...
mod models;

use models::NewPost;

// ...

fn create_post(conn: &SqliteConnection, author: &str, title: &str, body: &str) {
  // 引入我們的資料表
  use self::schema::posts;

  // 建立要準備新增的資料的 struct
  let new_post = NewPost {
    author,
    title,
    body,
  };

  // 指明要新增的表與新的值
  diesel::insert_into(posts::table)
    .values(&new_post)
    // 執行
    .execute(conn)
    .expect("新增貼文失敗");
}

fn main() {
  dotenv().ok();
  let conn = establish_connection();
  // 呼叫 create_post 建立貼文
  create_post(&conn, "Anonymous", "Hello", "Hello world");
}
```

在執行後你可以找個能打開 sqlite 資料庫的軟體看一下，資料已經確實的新增進去了：

![Image](https://i.imgur.com/7gx4dxO.png)

列出資料
--------

我們使用以下程式碼來列出貼文：

```rust
// ...

fn list_posts(conn: &SqliteConnection) -> Vec<Post> {
  // 引入資料表的所有東西
  use self::schema::posts::dsl::*;

  // 載入所有的貼文
  posts.load::<Post>(conn).expect("取得貼文列表失敗")
}

fn main() {
  dotenv().ok();
  let conn = establish_connection();
  println!("{:?}", list_posts(&conn));
}
```

執行後你應該會看到這樣的結果：

```plain
[Post { id: 1, author: "Anonymous", title: "Hello", body: "Hello world" }]
```

正是我們剛剛新增的貼文。

用主鍵查詢資料
--------------

```rust
// ...
fn get_post(conn: &SqliteConnection, id: i32) -> Post {
  use self::schema::posts::dsl::*;

  // 取得指定 id 的貼文
  posts.find(id).first(conn).expect("取得貼文失敗")
}

fn main() {
  dotenv().ok();
  let conn = establish_connection();
  println!("{:?}", get_post(&conn, 1));
}
```

刪除貼文
--------

```rust
// ...
fn delete_post(conn: &SqliteConnection, id: i32) {
  use self::schema::posts::dsl::*;

  // 不知道你有沒有注意到，除了查詢外的操作都在 diesel 下
  diesel::delete(posts.find(id))
    .execute(conn)
    .expect("刪除貼文失敗");
}

fn main() {
  dotenv().ok();
  let conn = establish_connection();
  delete_post(&conn, 1);
}
```

大概的使用就這樣，這些應該夠我們建一個留言板了，先把 `main` 函式清空吧，明天我們再來繼續建立留言板的後端。
