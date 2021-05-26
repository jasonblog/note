Cell 與 RefCell
===============

`Cell` 與 `RefCell` 能讓變數沒宣告 `mut` 也能修改，因為有時你還是需要在有多個 borrow 的情況下能修改變數，它們的文件都在 [`std::cell`](https://doc.rust-lang.org/stable/std/cell/index.html) 底下。

Cell
----

`Cell` 使用在能 `Copy` 的型態，因為它是使用取值與設定值的，在取值時會發生複製，若型態無法複製的話就無法取值。

```rust
use std::cell::Cell;

let num = Cell::new(42);
num.set(123);
assert_eq!(num.get(), 123);
```

RefCell
-------

`RefCell` 則能使用 borrow 的方式取值，就可以使用在其它的型態上了，它依然會檢查 Rust 的那些 borrow 的規則，只是是在執行時檢查。

```rust
use std::cell::RefCell;

let s = RefCell::new(String::from("Hello, "));

// 以可寫的方式 borrow
s.borrow_mut().push_str("World");

// 唯讀的 borrow
println!("{}", s.borrow());
```

搭配 Rc
=======

用 `RefCell` 搭配 `Rc` 使用就能做出同時有多個參照，卻還能視情況修改值了，我們試著做一個 double linked-list 吧：

```rust
use std::cell::RefCell;
use std::rc::{Rc, Weak};

#[derive(Debug)]
struct Node {
  data: i32,
  next: Option<Rc<RefCell<Node>>>,
  prev: Option<Weak<RefCell<Node>>>,
}

type Link = Option<Rc<RefCell<Node>>>;

#[derive(Debug, Default)]
struct DoubleList {
  head: Link,
}

impl DoubleList {
  fn new() -> Self {
    Default::default()
  }

  fn push(&mut self, val: i32) {
    match self.head {
      Some(ref head) => {
        let mut cursor = Rc::clone(head);
        let mut next = None;
        let mut prev = Weak::new();
        loop {
          match cursor.borrow().next {
            Some(ref node) => {
              // 先把下一個節點存起來
              next = Some(Rc::clone(node));
            }
            None => {
              // 存前一點節點
              prev = Rc::clone(&cursor);
              break;
            }
          }
          // 這邊 cursor 的 borrow 才結束，我們才能修改 cursor 的值
          cursor = next.unwrap();
        }
      cursor.borrow_mut().next = Some(Rc::new(RefCell::new(Node {
        data: val,
        next: None,
        prev: Some(prev),
      })));
      },
      None => {
        self.head = Some(Rc::new(RefCell::new(Node {
          data: val,
          next: None,
          prev: None,
        })));
      }
    }
  }
}

fn main() {
  let mut list = DoubleList::new();
  list.push(1);
  list.push(2);
  println!("{:?}", list);
}
```
