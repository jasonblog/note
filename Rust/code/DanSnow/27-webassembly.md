WebAssembly
===========

什麼是 WebAssembly
------------------

WebAssembly 是個實驗性的 Web 標準，其是由其它程式語言如 C/C++ ，目前 Go 也支援了，當然還有我們的 Rust ，由這些語言來編譯成位元組碼 (bytecode) 來執行，並提供比 js 還要來的更快的，接近原生程式的效能。

Rust 與 WebAssembly
-------------------

Rust 原生支援編譯成 WebAssembly，你只需要底下這行指令就能安裝這項功能：

```shell
$ rustup target add wasm32-unknown-unknown
```

接著我們來安裝 `wasm-bindgen` 這個工具：

```shell
$ cargo install wasm-bindgen-cli
```

這個工具是之後來幫我們最佳化編譯出來的 `wasm` 檔與產生對應的 js 包裝用的。

這次的專案在： https://github.com/DanSnow/rust-intro/tree/master/wasm-demo

我們可以來開個小專案測試一下了：

```shell
$ cargo init --lib wasm-demo
$ cd wasm-demo
$ cargo add wasm-bindgen web-sys
```

- [wasm-bindgen](https://github.com/rustwasm/wasm-bindgen)： 自動產生 JS 與 Rust 的綁定
- js-sys： js 的基本型態等等，這邊並沒有用到
- web-sys： 網頁的 API 綁定

然後我們需要編輯一下 `Cargo.toml` ，首先加上這段：

```toml
[lib]
crate-type = ["cdylib"]
```

然後編輯 `web-sys` 的相依性：

```toml
web-sys = { version = "0.3.2", features = ["Window", "Document", "Node", "HtmlElement", "Element"] }
```

這個有點長啊，我建議你改寫成這個型式：

```toml
[dependencies.web-sys]
version = "0.3.2"
features = ["Window", "Document", "Node", "HtmlElement", "Element"]
```

`web-sys` 為了避免編譯不需要的功能，所以用了一大堆的 `feature` ，使用的時候最好看一下 [文件](https://rustwasm.github.io/wasm-bindgen/api/web_sys/index.html) 確定一下自己要的功能要開啟哪個 `feature` 。

之後還要加上一個設定檔在 `.cargo/config` 內容是：

```toml
[build]
target = "wasm32-unknown-unknown"
```

這讓 cargo 預設把這個專案編譯成 WebAssembly 。

接著我們會需要 webpack 來幫我們打包 wasm 與 js 的 code ，所以這邊就假設你已經有 Node.js 的環境了，如果沒有的話要去安裝。

```shell
$ npm init --yes
$ npm install --dev webpack webpack-cli webpack-dev-server html-webpack-plugin shelljs
```

或是像我一樣使用 yarn 也行：

```shell
$ yarn init --yes
$ yarn add -D webpack webpack-cli webpack-dev-server html-webpack-plugin shelljs
```

我們先開始寫程式碼吧，修改 `src/lib.rs` 如下：

```rust
extern crate wasm_bindgen;
extern crate web_sys;

use wasm_bindgen::prelude::*;

#[wasm_bindgen]
pub fn main() -> Result<(), JsValue> {
  // 取得 window 物件
  let window = web_sys::window().unwrap();
  // 取得 document 物件，以下類推
  let document = window.document().unwrap();
  let body = document.body().unwrap();
  // 建立 HTML 元素
  let el = document.create_element("p")?;
  // 設定裡面的字
  el.set_inner_html("Hello from Rust");
  // 這邊必須要使用 AsRef::<web_sys::Node>::as_ref(&body) 這種寫法
  // 因為這邊還沒辦法很好的做型態的轉換
  // 轉成 Node 型態才有 append_child 能用
  // 而 el.as_ref() 也是在做型態轉換，只是這邊就能自動推導
  AsRef::<web_sys::Node>::as_ref(&body).append_child(el.as_ref())?;
  Ok(())
}
```

然後再寫個 `index.js` ：

```js
// wasm 的模組不允許同步的載入，所以要用 import()
const wasm = import('./wasm_demo')

wasm
  .then(m => {
    // 執行上面的 main
    m.main()
  })
  .catch(console.error)
```

然後接下來就是需要編譯與打包了，編譯的指令如下：

```shell
$ cargo build --release
$ wasm-bindgen target/wasm32-unknown-unknown/release/wasm_demo.wasm --out-dir .
```

在 build 完後你會發現檔案是被放在 `wasm32-unknown-unknown/release` 下，這是因為我們設定編譯成 wasm ，而因為這並不是預設的就被放到別的資料夾了。

接著的 `wasm-bindgen` 指令幫我們最佳化 wasm 檔案並產生一份 js 檔，輸出到現在的資料夾下。

我也把這個步驟寫成了 `build.js` ，你可以使用 `node build.js` 執行。

之後就是要打包了，請你直接把 `webpack.config.js` 複製過去，然後執行：

```shell
$ npm run webpack-dev-server
```

或用 `yarn` ：

```shell
$ yarn webpack-dev-server
```

這會開啟開發用的伺服器，你可以打開瀏覽器連上 `http://localhost:8080` 應該會看到 `Hello from Rust` 。

Port message-board 到 WebAssembly
---------------------------------

這個專案在： https://github.com/DanSnow/rust-intro/tree/master/message-board-wasm

再來個比較複雜點的專案，這次挑戰把之前的留言板的 js 部份全部用 Rust 改寫，先開個新專案：

```shell
$ cargo init --lib message-board-wasm
$ cd message-board-wasm
$ cargo add wasm-bindgen web-sys js-sys
```

一些細部的設定與重覆的部份我就省略了，底下我也只挑重點節錄，有興趣自己開專案來看：

```rust
// ...
fn render_posts(posts: Posts) -> Result<JsValue, JsValue> {
    let window = web_sys::window().unwrap();
    let document = window.document().unwrap();
    let fragment = document.create_document_fragment();
    let frag_node = AsRef::<web_sys::Node>::as_ref(&fragment);
    for post in posts.posts.iter().rev() {
        let card = document.create_element("div").unwrap();
        card.set_class_name("card");
        card.set_inner_html(&format!(
            r#"<div class="card-content">
            <p class="title is-5">{}</p>
            <p class="subtitle is-5">{}</p>
            <div class="content">
              {}
            </div>
          </div>"#,
            post.title, post.author, post.body
        ));
        frag_node.append_child(card.as_ref())?;
    }
    let container = document.query_selector(".posts")?.unwrap();
    container.set_inner_html("");
    AsRef::<web_sys::Node>::as_ref(&container).append_child(fragment.as_ref())?;
    Ok(JsValue::UNDEFINED)
}

fn fetch_posts(window: &web_sys::Window) {
  // JsFuture 會把 Promise 轉成 Future
  // future_to_promise 則是做相反的事
  future_to_promise(
    JsFuture::from(window.fetch_with_str("/api/posts"))
      .and_then(|res: JsValue| res.dyn_into::<web_sys::Response>().unwrap().json())
      // 這邊要再轉換 json 所回傳的 Promise
      .and_then(|json: js_sys::Promise| JsFuture::from(json))
      // parse json 後顯示
      .and_then(|json| render_posts(json.into_serde::<Posts>().unwrap())),
  );
}

fn handle_submit(event: web_sys::Event) {
  // 前面都在取表單的值
  // ...
  let data = JsValue::from_serde(&Post {
    author,
    title,
    body,
  })
  .unwrap();
  web_sys::console::log_1(&data);
  let req = web_sys::Request::new_with_str_and_init(
    "/api/posts/create",
    web_sys::RequestInit::new()
      // 或許這邊用 serde_json 會比較好點
      .body(Some(js_sys::JSON::stringify(&data).unwrap().as_ref()))
      .method("POST"),
  )
  .unwrap();
  req.headers()
    .set("Content-Type", "application/json")
    .unwrap();
  let window = web_sys::window().unwrap();
  let cb = Closure::wrap(Box::new(|_| {
    let window = web_sys::window().unwrap();
    fetch_posts(&window);
  }) as Box<FnMut(_)>);
  window.fetch_with_request(&req).then(&cb);
  // 同下
  cb.forget();
}

#[wasm_bindgen]
pub fn main() -> Result<(), JsValue> {
  // 這個會讓 debug 方便點，它讓 panic 時印出 stack trace
  console_error_panic_hook::set_once();
  let window = web_sys::window().unwrap();
  let document = window.document().unwrap();
  let create_post = document.query_selector("#create-post")?.unwrap();
  // 這是 console.log
  web_sys::console::log_1(create_post.as_ref());

  // 這邊一定要做轉型， Closure::wrap 要的是一個內部的型態
  let handler = Closure::wrap(Box::new(handle_submit) as Box<Fn(_)>);
  AsRef::<web_sys::EventTarget>::as_ref(&create_post)
    // 這邊要做兩次轉型，一次 as_ref 一次 unchecked_ref 才能拿到 js_sys::Function
    .add_event_listener_with_callback("submit", handler.as_ref().unchecked_ref())?;
  // 一定要呼叫 Closure 的 forget ，或是把它回傳，不然 Rust 這邊會把它釋放掉
  handler.forget();
  fetch_posts(&window);
  Ok(())
}
```

這樣就完成了一個前端是 Rust 後端還是 Rust 的網站了，可惜的是我還沒找到方法讓之前做的後端能夠送出正確的 mime 的方法，而 Chrome 會因為 mime 不對而拒絕執行 wasm ，所以我是用 [http-server](https://www.npmjs.com/package/http-server) 當伺服器的部份， API 的部份則用 Proxy 轉發給原本的後端處理。

Raw String
----------

上面使用到了一個語法：

```rust
r#"有 "" 也沒問題"#
```

這是 Rust 裡的 raw string ，它的語法是 `r` 後跟著 1 個以上的 `#` 再接 `"` ，之後若不是碰到 `"` 後接著同樣數量的 `#` 都不會當做結束，這中間出現 `"` 也沒問題，所以底下的也是可以的：

```rust
r##" 這次出現 #""# 也行"##
```

使用者可以根據情況調整 `#` 的數量。

後記
----

事實上像留言板的程式改寫成 WebAssembly 不一定會比較快，我個人是覺得 WebAssembly 目前應該著重於運算量大的，或是像 js 所不擅長處理的 64 位元整數，目前的 WebAssembly 其實很多東西都沒辦法直接操作，這中間靠了很多 webpack 與 wasm-bindgen 的處理把 js 與 wasm 整合起來，不然正常情況下 wasm 這邊是沒辦法直接處理 dom 的，這次算一個挑戰，除錯到很崩潰，差點要放棄，我一開始並沒有用 console error 的那個 crate ，結果沒想到真的遇到了 panic ，後來查了才發現，我把型態記錯了，我把 HtmlTextAreaElement 轉型成 HtmlInputElement，不過還是順利的把這篇文章生出來了。

下一篇來做個小遊戲。
