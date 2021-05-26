Move, Borrow & Ownership
========================

這篇與下一篇要介紹 Rust 中可說是最複雜，卻也是最重要的一個觀念，變數的所有權 (ownership) ，在 Rust 中每個變數都有其所屬的範圍 (scope) ，在變數的有效的範圍中，可以選擇將變數「借 (borrow)」給其它的 scope ，也可以將所有權整個轉移 (move) 出去，送給別人喔，當然，送出去的東西如果別人不還你的話是拿不回來的，但借出去的就只是暫時的給別人使用而已。

Move
----

```rust
fn main() {
  let message = String::from("Hello");
  {
    message;
  }
  println!("{}", message);
}
```

[![Playground][play-btn]](https://play.rust-lang.org/?gist=2e8d72baf064c582fcd1d7563b0ed682&version=stable&mode=debug&edition=2015)

> 範例的下方若有個 ![Playground](https://i.imgur.com/7F0C6a1.png) 的連結，按下去就會連到 Rust Playground ，讓你可以直接執行範例。

> 補充一個之前忘了提的東西， `!` 在這邊並不是打錯了， `println!` 是一起的

```rust
fn greet(message: String) {
  println!("{}", message);
}

fn main() {
  let message = String::from("Hello");
  greet(message);
  println!("{}", message);
}
```

猜看看上面的兩段程式碼的執行結果是什麼，猜到了嗎，答案都是無法編譯，編譯器會出現：

```plain
error[E0382]: use of moved value: `message`
```

意思是使用了已經送給別人的變數，在 Rust 中一個程式碼的區塊， 也就是由 `{` 與 `}` 包圍的區域都是一個 scope ，這也包含了函式、迴圈的括號等等，只要你把變數傳給了其它區塊，都會把變數送出去，所以在上面的範例中， `message` 這個變數已經送出去，並且在接下來的 `println!` 無法使用了，另外在底下的情況也會送出變數：

```rust
let a = String::from("a");
let b = a;
println!("{}", a); // 這邊也同樣不能編譯
```

> 或許你已經注意到了，這邊使用的都是 `String::from` ，都是在建立字串，如果把上面的例子都換成數字的話，你會發現不會出現任何錯誤，而能順利的執行，因為數字可以 **複製** ，字串不能複製嗎？也可以，只是字串的大小並不固定，有可能是很長的一篇文章，也有可能是一個空字串， Rust 並不允許在沒有明確的說要複製的情況下複製這種不知道會花費多少成本的型態，如果要改寫上面的範例，複製一個字串的話，可以使用 `clone`：

```rust
let a = String::from("a");
let b = a.clone();
println!("{}", a);
```

[![Playground][play-btn]](https://play.rust-lang.org/?gist=bbd5cea5645d6f17a52376a1105d2ef6&version=stable&mode=debug&edition=2015)

> 數字的大小則是固定的，於是在發生把變數送出去的情況時， Rust 會使用複製一份的方式給別人，所以就變成了兩個人都擁有，不會發生錯誤的情況。
>> 如果你想知道哪個型態可以被複製，可以參考文件的 [`std::marker::Copy`](https://doc.rust-lang.org/stable/std/marker/trait.Copy.html) ，你會在底下看到如 `impl Copy for i32` 這就代表 `i32` 可以被複製

拿走的東西主動的還回去也是可以的：

```rust
// 我要拿走整個 message 變數
fn greet(message: String) -> String {
  println!("{}", message);
  message // 之後再還回去
}

fn main() {
  let message = String::from("Hello");

  // 這邊變數被拿走了，但是又還了回來，於是我們需要一個變數代表它
  // 當然你也可以使用同樣的名稱 message
  let msg = greet(message);

  println!("{}", msg); // 又拿回來了，於是可以使用
}
```

[![Playground][play-btn]](https://play.rust-lang.org/?gist=0ef153bf8dae80d3d812b57dd934e6c9&version=stable&mode=debug&edition=2015)

Borrow
------

Rust 中把出借變數直接稱為 borrow ， Rust 中使用在變數前面加一個 `&` 來代表出借變數，borrow 的用途是當你不想把變數送出去時，你就可以把你的變數 **借** 出去，但還有個前提是對方要 **願意跟你借** ，底下是個借出變數給函式的範例：

```rust
// 這邊在 String 的前面加上了 & 代表我可以跟別人用借的
fn greet(message: &String) {
  println!("{}", message);
}

fn main() {
  let message = String::from("Hello");
  greet(&message); // 這邊加上了 & 來表示借出去
  println!("{}", message); // 借出去的東西只是暫時給別人而已，自己還可以使用
}
```

[![Playground][play-btn]](https://play.rust-lang.org/?gist=a1740013bff718d8141cec6cba465392&version=stable&mode=debug&edition=2015)

```rust
// 這邊沒有加上 & 代表我想要整個拿走
fn greet(message: String) {
  println!("{}", message);
}

fn main() {
  let message = String::from("Hello");
  // greet(&message); // 這邊就算加上了 & 也沒辦法把變數用借的借出去
  greet(message); // 一定要整個給它
  // println!("{}", message); // 因為被整個拿走了，所以這邊已經沒辦法使用了
}
```

Rust 預設借給別人的東西別人必須原封不動的還回來，也就是借出去的變數是沒辦法被修改的，如果你想允許別人修改的話，你就必須使用 `&mut` 對方也必須明確的使用 `&mut` 來代表我要借到一個可以修改的變數：  

```rust
fn combine_string(target: &mut String, source: &String) {
  // push_str 會把傳進去的字串接到字串的後面
  target.push_str(source);
}

fn main() {
  // 這邊一定要加 mut ，因為這個變數會被修改，就算不是你自己改的也一樣
  let mut message = String::from("Hello, ");
  let world = String::from("World");
  // 借給 combine_string 一個可以改的變數 message ，與一個不能改的 world
  combine_string(&mut message, &world);
  println!("{}", message); // 這邊就會印出 Hello, World
}
```

[![Playground][play-btn]](https://play.rust-lang.org/?gist=845ebd1e9abf0c3cdf0def4f5dbcad1e&version=stable&mode=debug&edition=2015)

> 還記得前一篇的猜數字裡有 `stdin().read_line(&mut input)` 嗎？  

Borrow 的規則
-------------

Rust 的出借變數是有其規則在的：  

1. 所有的變數一次都只能用可以修改的方式 (`&mut`) 出借一次

```rust
let mut n = 42;
let a = &mut n;
let b = &mut n; // 這裡用可以修改的方式總共借出去兩次了，這是不可以的
```

2. 可以無限的用唯讀的方式借出去

```rust
let n = 42;
let a = &n;
let b = &n;
```

3. 一旦用可以修改的方式 (`&mut`) 出借，那你就不能用任何其它的方式存取變數了

```rust
let mut n = 42;
{
  let a = &mut n;
  // println!("{}", n); // 你不可以使用原本的 n
  // let b = &n; // 你也不可以再用任何方式借走 n
}
println!("{}", n); // 我們離開了 a 借走 n 的範圍了，於是 n 又可以用了
```

4. 一旦你用唯讀的方式借出了變數，你就不可以修改變數

```rust
let mut n = 42;
{
  let a = &n;
  // n = 123; // 又不可以了，有夠煩的(X
}
n = 123; // 這邊才可以修改
```

> 這些規則是用來確保多執行緒時不會有資料競爭用的，也就是同時有兩個人修改了同一個變數，於是一次只允許有一個變數的擁有者能修改變數的值，同時一但借出了變數就不能隨意修改，因為別人不一定會知道變數被修改了。雖然有點麻煩 (也真的很麻煩) ，但往好處想，變數不再會被隨意的修改了。

有點可惜的是目前的 `borrow checker` ，也就是檢查，並執行上面這些規則的功能，它並不是很完善，比如：  

```rust
let mut array = [123, 456];
let a = &mut array[0];
let b = &mut array[1];
```

兩個變數分別借走了不相干的兩個部份，但這沒辦法通過檢查，不過這在 Rust 2018 將會有所改善，敬請期待。  

> Q： Rust 2018 是什麼？  
> A： 在今年的年底 Rust 將要推出 2018 年版，版本號會是 1.30 ，將會有不少的改進以及部份的語法的變更。  
> Q： 什麼！那我現在學的這些東西到年底就都沒辦法用了？  
> A： 放心好了，大部份的是功能的增強與新的語法，只有一小部份的修改，之後會有一篇來討論這些修改，與看看有哪些新功能。
> Q： 那我不想更新可以嗎？  
> A： 可以，你可以設定使用現在的語法版本，也就是 Rust 2015 版。  
> Q： 那我要怎麼設定？  
> A： 這個之後再說。  

String & str, Array & Slice
---------------------------

我們之前應該有提到過 Rust 有兩種字串 `String` 與 `str` ，可是一直沒有詳細說明這兩個的差別，這邊我們要提到 Rust 的一個東西「切片 (slice)」，切片可以理解為一次出借如陣列或字串這類的連續的資料型態的一部份：

> 如果你有寫過 Python 你可能知道 Python 的切片 `array[1:3]` ，只是這邊把 `:` 換成了 `..` 而已。  

```rust
let mut array = [0, 1, 2, 3, 4, 5];
{ // 建立一個區塊，不然我們等下沒辦法使用原本的 array
  let slice: &mut [i32] = &mut array[1..3]; // 這邊一次的借走了 array 的第 2 跟第 3 個元素

  // 然後我們修改了切片的第 1 個元素，對應到原本的 array 則是第二個元素
  slice[0] = 42;

  println!("{:?}", slice); // 會印出 [42, 2]
}
println!("{:?}", array); // 印出 [0, 42, 2, 3, 4, 5]
```

> Rust 的切片會知道自己借走了多少長度的東西，而且跟原本的變數 **會共用同一塊空間** ，建立切片是不會複製任何資料的。  

你可以看到這邊的印出來的結果很明顯的修改了原本的資料，同時很重要的一點，切片 **只能有 borrow 的型態** ，因為切片的本質就是出借資料，切片能把資料出借一小段，而使用者可以把這段資料當成像陣列一樣使用。

> `{:?}` 是把資料以 debug 的方式印出來，內建的型態不一定能直接印出來，但大部份都能用這種方式印出來，如果不能使用 `{}` 印出來時 `{:?}` 通常能派上用場。  

上面的 `slice` 的型態是 `&mut [i32]` ，這就是切片型態的寫法，一般如果需要借走一個陣列都會使用切片型態，這樣可以給予使用者更大的彈性，比如決定要不要把整個陣列都借出去，或是只借出一部份。

那終於可以來講 `str` 了， `str` 事實上就是字串的切片，而 `String` 則是一個可以在執行時改變大小的字串：

```rust
// 直接使用雙引號 (") 的字串都是字串的切片，它們都被 Rust 放在某個地方並且借給使用者使用而已
let hello: &str = "Hello";
// 建立一個 String
let string: String = String::from(hello);
// 借走字串的一部份，產生一個字串切片
let part_of_string: &str = string[1..3];
```

同樣的 `str` 也只能有 borrow 的型態。

下一篇要來介紹一下 borrow 的存活時間 (lifetime) 同樣也是重要觀念，這兩篇都是在講觀念可能比較無聊，不過接下來我們就會繼續介紹程式的語法了。

(希望你也跟我一樣喜歡寫 code)

[play-btn]: https://i.imgur.com/7F0C6a1.png
