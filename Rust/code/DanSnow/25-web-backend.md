Web Backend
===========

這次的程式碼一樣在： https://github.com/DanSnow/rust-intro/tree/master/message-board

今天要來把昨天說好的留言板完成，昨天我們已經建立好了那些用來從資料庫建立、查詢、刪除資料的函式，今天要把它變成網頁後端，請打開昨天的專案，這次要安裝的東西有點多：

```shell
$ cargo add serde serde_derive serde_json futures tokio tower-web
```

- [futures](https://github.com/rust-lang-nursery/futures-rs): 提供非同步操作的一個介面
- [tokio](https://github.com/tokio-rs/tokio)： 實作非同步 IO 的函式庫
- [tower-web](https://github.com/carllerche/tower-web)： 這次要使用的網頁後端框架，它是建立在 tokio 之上的，其實這套是個比較新的框架，在 Rust 的生態系中比較有名的是 [rocket](https://rocket.rs/) ，不過它一定要用到 nightly 的功能，加上我覺得 tower 設計的也挺不錯的。

我們先讓這個後端能送出我們前端的 HTML 吧，你說那個 HTML 嗎？我已經先寫好了，大概的用 [bulma](https://bulma.io/) 排一下版面而已，沒說很好看，也沒用 React 或 Vue 是用 Vanilla JS 寫，總之你就複製一份到 `static/index.html` 吧。

總之我們需要一個 struct ，並幫它加上我們的路由：

```rust
// ...
#[macro_use]
extern crate tower_web;
extern crate futures;
extern crate tokio;
use futures::Future;
use std::{env, io, path::PathBuf};
use tokio::fs::File;
// ...
// Clone 是必要的
#[derive(Clone, Copy, Debug)]
struct Service;

// 我們必須把 impl 包在 impl_web 這個 macro 裡
// 這樣 tower-web 才會幫我們產生必要程式碼
impl_web! {
  impl Service {
    // 這代表 get 根目錄的路由
    #[get("/")]
    // 回傳的是 html
    #[content_type("html")]
    // 傳回值是個 Future ，這是由 futures 提供的介面
    // 你可以想像它是個非同步的 Result
    // 所謂的非同步就是程式能先去做別的事，等到這邊好了再回來執行
    // 至於這詳細要怎麼處理，現在不需要去擔心
    fn index(&self) -> impl Future<Item = File, Error = io::Error> + Send {
      // 取專案的根目錄
      let mut path = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
      // 加上我們的路徑 static/index.html
      path.push("static");
      path.push("index.html");
      // 打開檔案，這邊使用的是 tokio 提供的 File 不是內建的，要注意喔
      File::open(path)
    }
}
```

然後修改 main 讓它執行：

```rust
use tower_web::ServiceBuilder;

fn main() {
  dotenv().ok();

  let addr = "127.0.0.1:8080".parse().unwrap();
  println!("Listen on {}", addr);
  ServiceBuilder::new()
    // 這邊加上剛剛建立的 Service
    .resource(Service)
    .run(&addr)
    .expect("啟動 Server 失敗");
}
```

編譯執行，在瀏覽器打開 http://localhost:8080 ，應該可以看到網頁顯示出來了，雖然網頁的 console 會顯示錯誤，因為我們還沒實作 API ，如果要關掉 Server 就直接按 Ctrl + C。

接著先來實作取得貼文列表：

```rust
// ...
// 做為回應的型態需要加上 Response
#[derive(Response, Debug)]
struct PostsResponse {
  // 我們的文章列表
  posts: Vec<Post>,
}

impl_web! {
  impl Service {
    // ...
    // 這次是 get /api/posts
    #[get("/api/posts")]
    // 回傳的資料是 json
    #[content_type("json")]
    // 回傳是個 Result ，因為我們沒做錯誤處理
    // 所以 Err 的型態就直接用 () 了
    fn list_posts(&self) -> Result<PostsResponse, ()> {
      // 建立資料庫連線，正常其實不該這樣
      let conn = establish_connection();
      // 取得文章列表後回傳
      Ok(PostsResponse {
        posts: list_posts(&conn)
      })
    }
  }
}
// ...
```

編譯後跑跑看，糟糕，出錯了，我們到 `models.rs` ，幫我們的 `Post` 加上 `Serialize`：

```rust
#[derive(Queryable, Serialize, Debug)]
pub struct Post {
    pub id: i32,
    pub author: String,
    pub title: String,
    pub body: String,
}
```

再編譯，這次應該沒錯了，重整網頁應該也不會有錯誤訊息了。

接著來實作新增貼文的 API：

```rust
// ...
// 從用戶端取得資料的 struct 都要實作 Extract
// 另外實作 Extract 的型態裡不能有 borrow
// 這是目前比較可惜的部份
#[derive(Extract, Debug)]
struct CreatePostRequest {
  author: String,
  title: String,
  body: String,
}

#[derive(Response, Debug)]
// 這個是設定當回傳這個型態時 http status code 用 201
#[web(status = "201")]
struct CreatePostResponse {
  ok: bool,
}

impl_web! {
  impl Service {
    // ...

    #[post("/api/posts/create")]
    #[content_type("json")]
    // body 是從 post body 取得的
    // 另外也可以把變數取名叫 query_string 就可以從 query string 拿到資料
    // 當然這都是 impl_web 這個 macro 提供的功能
    fn create_post(&self, body: CreatePostRequest) -> Result<CreatePostResponse, ()> {
      let conn = establish_connection();
      create_post(&conn, &body.author, &body.title, &body.body);
      Ok(CreatePostResponse { ok: true })
    }
  }
}
```

重新編譯執行，我們應該要能發佈貼文了，同時發佈後貼文就會更新，到這邊就完成了一個不能修改也不能刪除的留言板了，我們再來把剩下的 CRUD (Create、Retrieve、Update、Delete 建立、取得、更新、刪除) …，沒有 U ，來給完成吧。

```rust
#[macro_use]
extern crate serde_json;
// ...
#[derive(Response, Debug)]
struct PostResponse {
    post: Post,
}

impl_web! {
  impl Service {
    // ...
    // 網址中的變數是使用 : 來宣告的
    #[get("/api/posts/:id")]
    #[content_type("json")]
    // 這邊的變數要跟上面的同名
    fn get_post(&self, id: i32) -> Result<PostResponse, ()>{
      let conn = establish_connection();
      Ok(PostResponse { post: get_post(&conn, id) })
    }

    #[delete("/api/posts/:id")]
    // 回傳的是 json 型態，如果回傳的是 serde_json::Value
    // 就會自動用 json 回傳了
    fn delete_post(&self, id: i32) -> Result<serde_json::Value, ()>{
      let conn = establish_connection();
      delete_post(&conn, id);
      // serde_json 有提供 json 這個 macro 來建立 json
      // 只是用這個方法沒辦法自訂 http status code
      Ok(json!({
        "ok": true
      }))
    }
  }
}
// ...
```

好了，然後前端再加個刪除的按鈕就行了，但是那個我就不做了，用 Rust 寫網頁後端最大的好處應該就是執行速度快了，每次修改都要編譯再執行其實挺麻煩的，如果不是真的有很嚴重的效能問題不然還是用其它的語言寫比較方便，這個就當一個嘗試吧。

明天就來介紹一下 tokio 與 futures 是怎麼一回事。
