extern crate gdk;
extern crate gio;
extern crate gtk;

use gio::prelude::*;
use gtk::prelude::*;
use std::{cell::RefCell, env::args, rc::Rc};

macro_rules! clone {
    ($($name:ident),*) => {
        $(
            let $name = $name.clone();
        )*
    };
}

#[derive(Copy, Clone, Debug, PartialEq, Eq)]
enum Player {
    Circle,
    Cross,
}

impl Player {
    fn opponent(&self) -> Self {
        match self {
            Player::Circle => Player::Cross,
            Player::Cross => Player::Circle,
        }
    }
    fn label(&self) -> &'static str {
        match self {
            Player::Circle => "O",
            Player::Cross => "X",
        }
    }

    fn markup(&self) -> &'static str {
        match self {
            Player::Circle => r#"<span color="white">O</span>"#,
            Player::Cross => "X",
        }
    }
}

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
    fn new() -> Self {
        Self::default()
    }

    pub fn push_button(&mut self, btn: gtk::ToggleButton) {
        self.buttons.push(btn);
    }

    pub fn place_and_check(&mut self, target: (i32, i32)) -> bool {
        self.place(self.player, target);
        let res = self.check(target);
        self.player = self.player.opponent();
        if res {
            self.disable_all();
        }
        res
    }

    pub fn reset(&mut self) {
        self.player = Player::Circle;
        for row in self.board.iter_mut() {
            for col in row.iter_mut() {
                *col = None;
            }
        }
        for btn in self.buttons.iter() {
            btn.set_sensitive(true);
            btn.set_active(false);
            for widget in btn.get_children() {
                btn.remove(&widget);
            }
        }
    }

    fn check(&self, target: (i32, i32)) -> bool {
        let (px, py) = target;
        let (px, py) = (px as usize, py as usize);
        if self.board[0][px] == self.board[1][px] && self.board[0][px] == self.board[2][px] {
            return true;
        }
        if self.board[py][0] == self.board[py][1] && self.board[py][0] == self.board[py][2] {
            return true;
        }
        if self.board[1][1].is_none() {
            return false;
        }
        if self.board[0][0] == self.board[1][1] && self.board[0][0] == self.board[2][2] {
            return true;
        }
        if self.board[0][2] == self.board[1][1] && self.board[0][2] == self.board[2][0] {
            return true;
        }
        false
    }

    fn place(&mut self, player: Player, target: (i32, i32)) {
        if self.board[target.1 as usize][target.0 as usize].is_some() {
            panic!("{:?} 已經有棋了", target);
        }
        self.board[target.1 as usize][target.0 as usize] = Some(player);
    }

    fn disable_all(&self) {
        for btn in self.buttons.iter() {
            btn.set_sensitive(false);
        }
    }
}

const STYLE: &[u8] = include_bytes!("style.css");

fn show_message(window: &gtk::ApplicationWindow, winner: &str) {
    let dialog = gtk::MessageDialog::new(
        Some(window),
        gtk::DialogFlags::MODAL | gtk::DialogFlags::DESTROY_WITH_PARENT,
        gtk::MessageType::Info,
        gtk::ButtonsType::Close,
        &format!("{} Win", winner),
    );
    dialog.connect_response(|dialog, _| {
        dialog.destroy();
    });
    dialog.show_all();
}

fn setup_ui(app: &gtk::Application) {
    let screen = gdk::Screen::get_default().unwrap();
    let provider = gtk::CssProvider::new();
    provider.load_from_data(STYLE).expect("載入 css 失敗");
    gtk::StyleContext::add_provider_for_screen(
        &screen,
        &provider,
        gtk::STYLE_PROVIDER_PRIORITY_APPLICATION,
    );

    let window = gtk::ApplicationWindow::new(app);
    window.set_title("Tic Tac Toe");
    window.set_default_size(300, 300);
    window.connect_delete_event(move |win, _| {
        win.destroy();
        Inhibit(false)
    });

    let game = Rc::new(RefCell::new(Game::new()));
    let outer_box = gtk::Box::new(gtk::Orientation::Vertical, 0);
    let icon = gtk::Image::new_from_icon_name("restart", 16);
    let toolbar = gtk::Toolbar::new();
    let tool_button = gtk::ToolButton::new(Some(&icon), None);
    toolbar.insert(&tool_button, 0);
    {
        let game = game.clone();
        tool_button.connect_clicked(move |_| {
            game.borrow_mut().reset();
        });
    }
    outer_box.pack_start(&toolbar, true, false, 0);
    for y in 0..3 {
        let inner_box = gtk::Box::new(gtk::Orientation::Horizontal, 0);
        for x in 0..3 {
            let button = gtk::ToggleButton::new();
            game.borrow_mut().push_button(button.clone());
            clone!(game, window);
            button.connect_toggled(move |btn| {
                if !btn.get_active() {
                    return;
                }
                btn.set_sensitive(false);
                let p = game.borrow().player;
                let label = gtk::Label::new(None);
                label.set_markup(p.markup());
                btn.add(&label);
                label.show();
                if game.borrow_mut().place_and_check((x, y)) {
                    show_message(&window, p.label());
                }
            });
            inner_box.pack_start(&button, true, true, 0);
        }
        outer_box.pack_start(&inner_box, true, true, 0);
    }
    window.add(&outer_box);
    window.show_all();
}

fn main() {
    let application = gtk::Application::new(
        "io.github.dansnow.tic-tac-toe",
        gio::ApplicationFlags::empty(),
    )
    .expect("建立 APP 失敗");
    application.connect_startup(setup_ui);
    application.connect_activate(|_| {});
    application.run(&args().collect::<Vec<_>>());
}
