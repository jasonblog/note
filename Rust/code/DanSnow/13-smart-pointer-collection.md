智慧指標 (Smart Pointer) 與集合型態 (Collection)
================================================

集合型態
--------

這邊介紹的集合型態只會再介紹 HashMap 與 HashSet ，不過 Rust 實際上並不只這兩種而已，詳細建議看一下 [`std::collections`](https://doc.rust-lang.org/stable/std/collections/index.html) ，這邊的東西就是常見的資料結構的實作。

HashMap
-------

HashMap 是由鍵值 (Key-Value) 對應的一個型態，給定一個鍵就能找到一個值，這用在你需要查表之類的時候很好用。

建立 HashMap ：

```rust
use std::collections::HashMap;

let mut map = HashMap::new();
map.insert(String::from("key1"), 1);
map.insert(String::from("key2"), 2);
```

也可以從鍵值對建立：

```rust
let map = vec![
  (String::from("key1"), 1),
  (String::from("key2"), 2),
].into_iter().collect::<HashMap<_, _>>();
```

另外還有 [maplit](https://github.com/bluss/maplit) 這個 crate 可以使用：

```rust
// 如果要從 crate 引入 macro 要用 #[macro_use]
#[macro_use]
extern crate maplit;

let map = hashmap! {
  String::from("key1") => 1,
  String::from("key2") => 2,
};
```

如果要用鍵取得值：

```rust
// 這邊的鍵要用 borrow 型態的，所以用 str 也是可以的
// 如果沒有這個鍵的話會 panic
println!("{}", map["key1"]);
// 這個做法的話會回傳 Option
println!("{:?}", map.get("key2"));
```

更新值只要再 insert 就行了：

```rust
map.insert(String::from("key1"), 3);
```

或是使用 entry 這個 API ：

```rust
map.entry("key1".to_owned()).and_modify(|v| { *v = 3 });
```

HashSet
-------

HashSet 就是數學上的集合，其中不會有重覆的值，很適合用來檢查一個值是否出現過。

建立一個 HashSet ：

```rust
use std::collections::HashSet;

let mut set = HashSet::new();
set.insert("foo".to_string());
set.insert("bar".to_string());
```

若用 maplit 的話：

```rust
let set = hashset! { "foo".to_string(), "bar".to_string() };
```

判斷是不是有這個值：

```rust
set.contains("foo");
```

智慧指標
--------

智慧指標是一種會自動在建立時分配一塊記憶體，並在變數消失時自動釋放空間的容器，主要就只有 `Box` 與 `Rc` ，文件分別在 [`std::boxed::Box`](https://doc.rust-lang.org/stable/std/boxed/struct.Box.html) 與 [`std::rc::Rc`](https://doc.rust-lang.org/stable/std/rc/struct.Rc.html) 。

Box
---

`Box` 可以用來裝那些無法在編譯時知道大小的型態，同時它也有指標的特性，也能把大小推遲到執行時決定，如果有些情況實在是過不了 borrow checker 的話就用 `Box` 吧。

建立一個 `Box` 很簡單：

```rust
let x = Box::new(42);
```

Rust 中其實有個有趣的問題，如果今天要建立一個連結串列 (linked-list) 要怎麼辦呢？

```rust
enum List {
  Cons(i32, List),
  Nil,
}
```

上面這個拿去編譯會得到這個錯誤：

```plain
error[E0072]: recursive type `List` has infinite size
 --> src/lib.rs:1:1
  |
1 | enum List {
  | ^^^^^^^^^ recursive type has infinite size
2 |   Cons(i32, List),
  |             ---- recursive without indirection
  |
  = help: insert indirection (e.g., a `Box`, `Rc`, or `&`) at some point to make `List` representable
```

因為編譯器必須在編譯時決定它的大小，可是 `Cons` 的大小因為會一直遞迴下去，所以無法決定，那我們改用 `Box` 看看：

```rust
enum List {
  Cons(i32, Box<List>),
  Nil,
}
```

這次就能通過編譯了，也能使用：

```rust
let list = List::Cons(1, Box::new(List::Cons(2, Box::new(List::Nil))));
```

`Box` 與 borrow 的差別：

- `Box` 本身就擁有資料，所以不會有 lifetime 的問題
- `Box` 是分配在 heap ，而 borrow 的資料位置則不一定

> 電腦主要資料儲存都是在記憶體中，根據其特性還可以再細分，其中有 stack 為存放區域變數，在上面的資料會在函式結束時釋放， heap 則為一塊空間，可讓使用者手動分配記憶體，並可自行決定何時還回去，此處為 `Box` 所使用的位置，當然分配與歸還都由 `Box` 自動的管理了。

Rc
---

`Rc` 是 reference counting 的意思， `Box` 的擁有者一次只能有一個人，然而 `Rc` 可以由多人同時持有，它像 `Box` 一樣會自動分配記憶體存放資料，並在最後一個人釋放掉 `Rc` 時將記憶體也釋放。

```rust
use std::rc::Rc;

let a = Rc::new(42);
let b = Rc::clone(&a);
```

以上的 a 與 b 同時持有同一份資料，通常還會搭配類似 `Cell` 的東西做使用，只是那不是這次的範圍，就先知道一下這個東西吧。

另外還有在 [`std::sync::Arc`](https://doc.rust-lang.org/stable/std/sync/struct.Arc.html) 的 `Arc` ，它可以讓多個執行緒同時持有同一份資料。

Weak
----

若有兩個 `Rc` 互相持有對方的話，就會因為雙方都屬於有效的持有狀態，而無法正常釋放記憶體，所以就有 `Weak` ， `Weak` 不會增加 `Rc` 的持有人數。

下一篇要來介紹 `Cell` 與 `RefCell` ，它們可以讓 Rust 的變數沒有宣告 `mut` 也能改變，同時讓 borrow check 的規則延遲到執行時才判斷。
