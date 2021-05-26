用 Rust + GTK 做個井字棋
========================

前幾天有看到別人鐵人賽在做井字棋，我也來做一個吧，用 Rust ，加上 GTK ，做成原生的視窗程式。

但在開始前 Rust 最近發佈了 `1.30` 版，這版裡有些有趣的功能像 `procedural macro` ，它是讓你可以用 Rust 的程式處理 Rust 的抽像語法樹 (AST) ，然後去改變或產生程式，這其實已經不是 macro 了，而是 meta programming 了，然後還有 crate 的 macro 可以用 `use` 載入了，而不用 `#[macro_use]` (意思是自己寫的還是要)， Rust 2018 的功能開始出現了，下一版就會是第一版的 Rust 2018 了，想看詳細內容可以去看 [官方 blog](https://blog.rust-lang.org/2018/10/25/Rust-1.30.0.html)。

GTK 是什麼
----------

GTK 是個在類 Unix 系統下的一個主流的圖形介面程式開發的函式庫，可以讓你做出有視窗的程式，而不是終端機那黑黑的畫面，雖然它是用 C 寫的，但非常的 OOP ，而提供 Rust 綁定的函式庫： [gtk-rs](http://gtk-rs.org/) ，也很好的把原本的繼承關係等等的用 Rust 的 trait 實作出來了。

它實際上由數個部份組成：

- gtk： 最上層的函式庫，提供圖形的元件
- gdk： gtk 的繪圖函式庫，也處理字型、游標等等
- gio： 處理 IO 與檔案等等的操作
- glib： 最低層的函式庫，提供資料結構、常用函式與物件系統

另外還有 cario 與 pango 等。

建立專案
--------

這次的專案在： https://github.com/DanSnow/rust-intro/tree/master/gtk-tic-tac-toe

不多說了：

```shell
$ cargo init gtk-tic-tac-toe
$ cd gtk-tic-tac-toe
$ cargo add gtk gio gdk
```

然後我們要修改一下 `Cargo.toml`：

```toml
gtk = { version = "0.5.0", features = ["v3_22"] }
```

後面的 `v3_22` 要配合電腦上有的 gtk 函式庫的版本調整，我所使用的是 3.22 ，另外電腦上也要裝 gtk 的開發用函式庫， Ubuntu 下套件的名字是 `libgtk-3-dev` 。

Hello in GTK
------------

修改 `src/main.rs`：

```rust
extern crate gdk;
extern crate gio;
extern crate gtk;

use gio::prelude::*;
use gtk::prelude::*;

fn main() {
  let application = gtk::Application::new(
    // 這邊要放個 id
    "io.github.dansnow.tic-tac-toe",
    // 不需要什麼選項
    gio::ApplicationFlags::empty(),
  )
  .expect("建立 APP 失敗");
  // 設定程式啟動時的動作，這邊的 app 實際上是 application 的 borrow
  application.connect_startup(|app| {
    // 建立視窗
    let window = gtk::ApplicationWindow::new(app);
    // 設定標題
    window.set_title("Tic Tac Toe");
    // 預設視窗大小
    window.set_default_size(300, 200);
    // 設定按叉叉時的動作
    window.connect_delete_event(move |win, _| {
      // 關掉視窗
      win.destroy();
      Inhibit(false)
    });
    let label = gtk::Label::new("Hello");
    window.add(&label);
    // 顯示視窗
    window.show_all();
  });
  application.connect_activate(|_| {});
  // 程式啟動
  application.run(&args().collect::<Vec<_>>());
}
```

執行看看，應該會有個視窗出現，想當初第一次接觸視窗程式，第一次脫離那黑色的視窗時有多感動 (然後我就寫了個全螢幕但沒有離開的方法的程式丟在別人的電腦上) 。

建立程式畫面
------------

這次的目標長這樣：

![Image](https://i.imgur.com/V0DAyi0.png)

其實沒很好看，我盡力了。

盤面的部份每個都是一個按鈕，用的是 `GtkToggleButton`，三個包進一個橫的 `GtkBox` (這是 GTK 裡很常用的一個排版元件) ，最上面是 `GtkToolbar` ，最後再全部包進一個直的 `GtkBox`，總之程式碼是這樣子的：

```rust
fn setup_ui(app: &gtk::Application) {
  // 建立 window
  // ...

  // 這是遊戲的邏輯部份，等下再講
  let game = Rc::new(RefCell::new(Game::new()));
  // 最外層的 Box
  let outer_box = gtk::Box::new(gtk::Orientation::Vertical, 0);
  // 按鈕的圖示
  let icon = gtk::Image::new_from_icon_name("restart", 16);
  // 工具列
  let toolbar = gtk::Toolbar::new();
  // 工具列的按鈕
  let tool_button = gtk::ToolButton::new(Some(&icon), None);
  // 把按鈕加到工具列
  toolbar.insert(&tool_button, 0);
  {
    let game = game.clone();
    // 設定重置按鍵按下時重置遊戲
    tool_button.connect_clicked(move |_| {
        game.borrow_mut().reset();
    });
  }
  // 把 Toolbar 加進去 outer_box
  outer_box.pack_start(&toolbar, true, false, 0);
  for y in 0..3 {
    let inner_box = gtk::Box::new(gtk::Orientation::Horizontal, 0);
    for x in 0..3 {
      let button = gtk::ToggleButton::new();
      // 我們之後需要重置這些按鈕的狀態，所以把它存起來
      game.borrow_mut().push_button(button.clone());
      // 這是自訂的 macro
      clone!(game, window);
      // 設定按鈕狀態改變時的動作
      button.connect_toggled(move |btn| {
        // 因為重置按鈕時也會觸發，所以要檢查
        if !btn.get_active() {
          return;
        }
        // 停用按鈕，這樣使用者就不能再按了
        btn.set_sensitive(false);
        let p = game.borrow().player;
        // 插入 O 或 X 的符號
        let label = gtk::Label::new(None);
        label.set_markup(p.markup());
        btn.add(&label);
        label.show();
        // 放棋與檢查
        if game.borrow_mut().place_and_check((x, y)) {
          show_message(&window, p.label());
        }
      });
      // 把按鈕加建 box
      inner_box.pack_start(&button, true, true, 0);
    }
    outer_box.pack_start(&inner_box, true, true, 0);
  }
  window.add(&outer_box);
  window.show_all();
}
```

建立 UI 就是這麼麻煩，雖然有個工具叫 [Glade](https://glade.gnome.org/) 可以用拉的，不過這次我並沒有使用。

修改元件的外觀
--------------

如果程式碼是像上面那樣，那建出來的畫面會是：

![Image](https://i.imgur.com/d6J6vlg.png)

這配色是我電腦的主題，平常很好看，但在這邊不適合，所以我們來寫點 CSS 修改它，建一個 `src/style.css`：

```css
.toggle {
  background-color: lightseagreen;
  background-image: none;
}

.toggle label {
  color: black;
  font-size: 1.5em;
}
```

對，別懷疑，就是寫網頁用的 CSS ， GTK 的主題設定是使用 CSS 的，順帶一提，你可以用以下指令：

```shell
$ GTK_DEBUG=interactive cargo run
```

程式打開始還會開啟一個圖形介面用的除錯器：

![Image](https://i.imgur.com/euWmHej.png)

就像 Chrome 的 F12 一樣，而且 `GTK_DEBUG=interactive` 可以用在所有使用 gtk3 的程式上。

> 準確來說 gtk3 的主題才是使用 CSS ， gtk2 是使用自訂的語言，另外比較早的 gtk3 版本是使用元件名稱當成 css selector 的，像 `GtkButton` ，現在則是改用跟網頁元素類似的名稱，或是使用 class 了，像 `button.toggle` 就是指 `GtkToggleButton`

接著我們要載入 CSS ：

```rust
// include_bytes! 可以把相對於原始碼目錄的檔案以 &[u8] 載入，並編譯到程式裡
// 另外還有個 include_str! 則是載入成字串
const STYLE: &[u8] = include_bytes!("style.css");

fn setup_ui(app: &gtk::Application) {
  let screen = gdk::Screen::get_default().unwrap();
  let provider = gtk::CssProvider::new();
  provider.load_from_data(STYLE).expect("載入 css 失敗");
  gtk::StyleContext::add_provider_for_screen(
    &screen,
    &provider,
    gtk::STYLE_PROVIDER_PRIORITY_APPLICATION,
  );
  // ...
}
```

顯示訊息
--------

這邊很簡單的顯示了個對話框做為輸贏的提示：

![Image](https://i.imgur.com/azJQmD1.png)

```rust
fn show_message(window: &gtk::ApplicationWindow, winner: &str) {
  let dialog = gtk::MessageDialog::new(
    Some(window),
    // 對話框選項
    gtk::DialogFlags::MODAL | gtk::DialogFlags::DESTROY_WITH_PARENT,
    // 對話框類型
    gtk::MessageType::Info,
    // 對話框上的按鈕
    gtk::ButtonsType::Close,
    // 格式化訊息
    &format!("{} Win", winner),
  );
  // 這邊設定按下按鈕後做的動作
  dialog.connect_response(|dialog, _| {
    dialog.destroy();
  });
  dialog.show_all();
}
```

遊戲邏輯
--------

```rust
// PartialEq 與 Eq 是讓它可以判斷是否相等
#[derive(Copy, Clone, Debug, PartialEq, Eq)]
enum Player {
  // 圈
  Circle,
  // 叉
  Cross,
}

impl Player {
  // 取得對手
  fn opponent(&self) -> Self {
    match self {
      Player::Circle => Player::Cross,
      Player::Cross => Player::Circle,
    }
  }

  // 取得文字的表示
  fn label(&self) -> &'static str {
    match self {
      Player::Circle => "O",
      Player::Cross => "X",
    }
  }

  // 給 Label 用的
  fn markup(&self) -> &'static str {
    match self {
      Player::Circle => r#"<span color="white">O</span>"#,
      Player::Cross => "X",
    }
  }
}

// 一開始從圈圈開始
impl Default for Player {
    fn default() -> Self {
        Player::Circle
    }
}

#[derive(Default, Debug)]
struct Game {
    pub player: Player,
    board: [[Option<Player>; 3]; 3],
    buttons: Vec<gtk::ToggleButton>,
}

impl Game {
  // ...

  // 檢查輸贏的函式，只需要檢查放棋的點的上下左右與鈄的就行了
  fn check(&self, target: (i32, i32)) -> bool {
    let (px, py) = target;
    let (px, py) = (px as usize, py as usize);
    // 檢查直的
    if self.board[0][px] == self.board[1][px] && self.board[0][px] == self.board[2][px] {
      return true;
    }
    // 橫的
    if self.board[py][0] == self.board[py][1] && self.board[py][0] == self.board[py][2] {
      return true;
    }
    // 中間沒放就不會有鈄的情況了
    if self.board[1][1].is_none() {
      return false;
    }
    // 左上到右下鈄
    if self.board[0][0] == self.board[1][1] && self.board[0][0] == self.board[2][2] {
      return true;
    }
    // 右上到左下鈄
    if self.board[0][2] == self.board[1][1] && self.board[0][2] == self.board[2][0] {
      return true;
    }
    false
  }

  // ...
}
```

你說沒有 AI 嗎？對，這是個單機雙人遊戲，你可以找朋友一起玩，或是自己跟自己玩。

說來寫到這段時我真的覺得 Rust 能幫列舉加上方法的這個功能真的很好用。

macro
-----

這次用了個自訂的 macro：

```rust
macro_rules! clone {
  ($($name:ident),*) => {
    $(
      let $name = $name.clone();
    )*
  };
}
```

這個 macro 會把傳進去的變數都 clone 一份後再賦值給同名的變數， macro 的詳細的寫法請讓我留到明天再講，我有記得我說過要講解這個喔，絕對沒忘喔。

下一篇我想來講一些我覺得值得提的部份，當然包括 macro 的寫法，另外還會談談我對於 Rust 的心得，跟鐵人賽的心得，畢竟是達標的最後一篇了，至於如果之後又有什麼有趣的東西，說不定我還是會寫成文章的。
