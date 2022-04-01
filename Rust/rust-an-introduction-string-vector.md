# 給 C++ 使用者的 Rust 簡介：字串與陣列

在前面幾篇與 Rust 相關的文章中，我一直還沒提到的程式語言基本功能，就是字串與陣列了。若少了這兩項功能，很難做出什麼具備複雜功能的軟體。另一方面，為了達到記憶體安全，Rust 對參考型別設下了重重限制，讓你很難在物件中直接儲存指向另一個物件的參考。對此，Rust 也提供了智慧指標 (smart pointers)，讓你可以方便地建立資料結構。

幸運的是，在字串、陣列與智慧指標的設計上，Rust 與 C++ 可說是驚人地相似。這篇文章將會從 C++ 的角度，帶你快速了解 Rust 所提供的功能。



## 字串實值

Rust 與大部份語言相同，使用雙引號 `""` 來表示字串實值 (string literal)，而它的型別是 `&'static str`。這個型別看起來很令人困惑，但其實它只是想表達這樣的特性：

1. 這是個具備靜態生命週期的字串，配置在程式的常數區段中。
2. 你不能改它的內容，它的內容也保證不會改變。

這其實就是你在 C++ 中，所有字串實值所具備的特性，因此你可以把 `&'static str` 當成是 C++ 的 `const char*`。不同的是，Rust 在型別上給了更強的保證 (內容不可改變、生命週期最長以及長度檢查)。

字串實值並不使用 null 結尾，而會另外儲存長度，你可以用 `len()` 來取得字串長度。字串永遠使用 UTF-8 編碼來儲存字元，你取得的「長度」意指字串所占的位元組總數，而非字元的數量。

```rust
fn main()
{
    println!("{}", "hello 中文".len()); // 雖然有 8 個字元，但會印出 12
}
```

因為字串使用 UTF-8，你沒辦法像在 C++ 那樣用陣列存取的方式取得某個字元，而必需使用 `chars()` 搭配 `for` 迴圈來巡訪每個字元，此時會自動進行 UTF-8 解碼。

```rust
fn main()
{
    for c in "hello 中文".chars() {
        println!("{}", c);
    }
}
```

用 `chars().count()` 可以計算出字串的字元總數，但這項操作會掃過所有字元，時間複雜度是 *O*(*n*)O(n)。

```rust
fn main()
{
    println!("{}", "hello 中文".chars().count()); // 印出 8
}
```

宣告或操作字串實值並不會動態配置記憶體 (heap allocation)，因此它非常有效率。然而 `str` 是個黑盒子型別，你只能進行一些讀取操作，沒辦法寫入或附加資料，也沒辦法創造出 `str` 物件，畢竟這些物件都是在編譯時寫死的常數。

## 字串物件

如果你要動態建立字串，那就必需使用 `String` 類別，它相當於 C++ 中的 `std::string`。`String` 會配置記憶體，並且在內容改變時自動擴充空間。

```rust
fn main()
{
    let mut s = String::new();
    s += "hello";
    s += " 中文";
    println!("{}", s); // "hello 中文"
}
```

大部份的內建型別都可以用 `to_string()` 把內容輸出成字串，你也可以用 `format!()` 巨集，相當於 C 裡面的 `sprintf()`。

```rust
fn main()
{
    let mut s = "hello".to_string(); // 相當於 C++ 的 auto s = std::string("hello");
    s = 123.to_string();      // s = "123"
    s = 456.789.to_string();  // s = "456.789"
    
    s = format!("flag: {}", true); // s = "flag: true"
}
```

`String` 具備 move semantics，除了使用等號賦值以外，當作參數傳遞時也會一併轉移所有權：

```rust
fn log_debug(msg: String)
{
    println!("[DEBUG] {}", msg);
}

fn main()
{
    let a = "some message".to_string();
    let b = a;
    println!("a = {}", a); // 錯誤：a 的內容已轉移到 b

    log_debug(b);
    println!("b = {}", b); // 錯誤：b 的內容已轉移進 log_debug 函式，並在函式結束時解構
}
```

在其它語言中的傳值 (pass-by-value)，在 Rust 中通常都是轉移所有權 (ownership) 的意思。因此上面的 `log_debug()` 函式會取得一個字串物件的所有權，意味著在函式結束時必需要呼叫物件的解構式以釋放記憶體。轉移字串物件的所有權只需要移動指向字串內容的指標及字串長度，不需要複製整個字串，因此其時間複雜度是 *O*(1)O(1)。

雖然 pass-by-move 的速度很快，但通常我們還是會希望原本的變數能保留原值。如果要避免所有權轉移，一個方法是改為傳參考 (pass-by-referenece)，或是使用 `clone()` 明確地複製物件：

```rust
let a = "some message".to_string();
log_debug(a.clone()); // 複製 a 並傳入複本
```

## `String` 與 `&str` 的使用時機

`String` 是帶有資源的物件，很自然地它可以快速轉型為 `&str`，就像 C++ 中對 `std::string` 呼叫 `c_str()` 一般。因此，所有接收字串當參數的函式，如果只需要讀取字串內容，那麼使用 `&str` 當作參數型別是最有效率的。比如說上面的 `log_debug()` 函式可以修改如下：

```rust
fn log_debug(msg: &str)
{
    println!("[DEBUG] {}", msg);
}

fn main()
{
    log_debug("starting now");

    let a = "some message".to_string();
    log_debug(&a); // &String 可以隱式轉型為 &str
}
```

反過來說，如果函式要動態建構內容不同的字串再回傳，或是要把字串儲存在 struct 內，就有必要使用 `String`：

```rust
fn make_equation(a: i32, b: i32) -> String
{
    // 輸入 a 與 b, 產生字串 "{a} x {b} = {a*b}"
    format!("{} x {} = {}", a, b, a*b)
}

struct PurchaseRecord {
    title: String,
    unit_price: i32,
    quantity: i32,
    price_detail: String
}

fn purchase_something_cost_100(t: &str, q: i32) -> PurchaseRecord
{
    PurchaseRecord {
        title: t.to_string(),
        unit_price: 100,
        quantity: q,
        price_detail: make_equation(100, q)
    }
}
```

`make_equation()` 會直接回傳一個字串物件，這樣是否會複製整個字串內容導致效率低落呢？不用擔心，`String` 具備 move semantics，只要你沒有呼叫 `clone()`，那麼傳遞時就只會移動字串指標及字串長度，絕對不會複製整個字串內容。

## 陣列

Rust 內建的陣列型別就和 C++ 一樣，大小固定並配置在 stack 上：

```rust
fn main()
{
    // 相當於 int array[5] = {1, 2, 3, 4, 5};
    let array: [i32; 5] = [1, 2, 3, 4, 5];
    for i in 0..array.len() {
        println!("{}", array[i])
    }
}
```

這邊的 `..` 是用來產生區間 (range) 的運算子，`a..b` 表示 [*a*,*b*)[a,b)，也就是 {*x*∈Z|*a*≤*x*<*b*}{x∈Z|a≤x<b} 這樣的集合。因此，我們可以用 `0..array.len()` 來表達所有陣列的合法索引值。

使用索引存取陣列元素時，Rust 一定會檢查邊界 (bound checking)，避免存取到陣列以外的記憶體。若要巡訪元素，用迭代器 (iterator) 是比較有效率也不容易出錯的做法：

```rust
fn main()
{
    let array = [1, 2, 3, 4, 5];
    for x in array.iter() { // x 的型別為 &i32，會一一掃過所有元素
        println!("{}", *x);
    }
}
```

除了使用索引或迭代器存取元素以外，內建陣列型別能做的事情不多。因為效率考量，Rust 禁止拿陣列當作參數或是函式回傳值。

```rust
fn read_array(array: [i32; 5]) // 錯誤
{ ... }

fn make_array() -> [i32; 5] // 錯誤
{ ... }
```

## 片段

你可以取得陣列的參考，甚至只參考其中某個範圍，這在多數程式語言中稱為片段 (slice)。

```rust
fn main()
{
    let array = [1, 2, 3, 4, 5];
    let slice = &array[1..4];    // slice 指向 array 中的 [2, 3, 4]

    println!("slice.len() = {}", slice.len()); // 3
    println!("slice[0]    = {}", slice[0]);    // 2
}
```

片段屬於參考型別，對它做的修改相當於對原本的陣列做修改。同時，它也可以很有效率地傳遞到函式中：

```rust
fn clear(slice: &mut [i32])
{
    // slice 也可以使用迭代器巡訪內容
    for x in slice.iter_mut() { // x 的型別為 &mut i32
        *x = 0;
    }
}

fn main()
{
    let mut array = [1, 2, 3, 4, 5];
    clear(&mut array[1..4]);

    println!("{:?}", array); // [1, 0, 0, 0, 5]
}
```

## `Vec` 物件

大部份的情況下，我們都需要一個可以動態調整大小的陣列，因此內建的陣列型別是不夠用的。對此 Rust 提供了 `Vec` 物件，用起來就像 C++ 的 `std::vector` 一樣。

```rust
fn main()
{
    let mut v = Vec::new();
    v.push(1);
    v.push(2);
    v.push(3);

    println!("v.len() = {}", v.len()); // 3
    println!("v[1]    = {}", v[1]);    // 2
}
```

在 C++ 中宣告 `std::vector` 物件時，因為它是個泛型模板，我們必需指定元素的型別才能建立出實體，比如說 `std::vector<int>`。然而因為 Rust 具備更強的型別推導能力，當它看到後面的 `v.push(1)` 後，便能正確推導出 `v` 的元素型別是 `i32`，因此前面只需要寫 `Vec::new()` 便能建立出一個元素為 `i32` 的動態陣列。

為了讓它用起來就像使用內建型別一樣方便，Rust 提供了 `vec!` 巨集讓你可以快速建立一份 `Vec` 物件：

```rust
let v = vec![1, 2, 3];
```

當然，你也可以對 `Vec` 物件取得片段。因此上面把部份陣列內容清為 0 的函式可以直接套用在 `Vec` 上面：

```rust
fn clear(slice: &mut [i32])
{
    for x in slice.iter_mut() {
        *x = 0;
    }
}

fn main()
{
    let mut v = vec![1, 2, 3, 4, 5];
    clear(&mut v[1..4]);

    println!("{:?}", v); // [1, 0, 0, 0, 5]
}
```

除了提供所有內建陣列型別的操作，`Vec` 還有使用上更方便的地方：可以直接傳進函式，或是作為函式的回傳值。`Vec` 也具備和 `String` 一樣的 move semantics，只有在你明確呼叫 `clone()` 時才會複製所有元素，因此傳遞效率是非常快的。

```rust
fn generate(from: i32, to: i32) -> Vec<i32> // 這邊需要指定元素型別
{
    let mut result = Vec::new();
    for x in from..to {
        result.push(x);
    }
    return result;
}
```

## 內建陣列與 `Vec` 的使用時機

Rust 的內建陣列無法動態調整大小，也無法傳入或傳出函式。然而它儘管缺乏彈性，卻有一項優勢：由於直接使用 stack 的空間，它不需要動態記憶體配置 (heap allocation)。在某些需要即時回應的系統中，為了避免動態配置記憶體造成額外的時間成本，就會是使用內建陣列的場合。

相反地，在絕大多數的場合中，`Vec` 兼具了彈性與效率。就如同 C++ 鼓勵大家使用 `std::vector` 那樣，Rust 鼓勵使用者在需要陣列的場合盡量使用 `Vec` 即可。

## 結語

本篇文章介紹了 Rust 當中的字串與陣列功能，若讀者原本就已經熟悉 C++ 的 `std::string` 與 `std::vector`，不需要花很多力氣就能了解 Rust 的設計。

在下一篇文章中，我會進一步介紹 Rust 的智慧指標。