# 給 C++ 使用者的 Rust 簡介：參考型別與 Borrow Checker 



大多數的程式語言，為了有效率地傳遞、修改物件內容，會提供指標或參考型別 (reference type)。若某個變數是參考型別，意味著它並不直接儲存物件的內容，而是儲存記憶體位址，該位址指向的記憶體區塊才是真正儲存物件內容的地方。

除了快速傳遞大型物件外，參考型別也在一些語言功能上扮演重要角色，比如說：

- 表達像是 linked list 這樣的遞迴類別 (recursive data type)。
- 在 OOP 中操作抽象物件時，由於具體型別未知，因此必需用「指向某抽象型別的參考」來進行操作。
- 利用 copy on write 或是 flyweight 之類的共享資料節構減少記憶體浪費並提高執行效率。

作為追求執行效率的系統程式語言，Rust 也提供了參考型別，但是為了達成 memory safe 的目標，Rust 加上了一層嚴格的保護：borrow checker，讓它有著異常陡峭的學習曲線。本系列文章會試著從 borrow checker 的設計目標開始，解釋參考型別帶來的潛在危險，以及 borrow checker 如何幫我們檔下這些錯誤。



## 基本語法

Rust 中的參考型別就直接稱呼為參考 (reference)。雖然 C++ 也有參考，但 Rust 的參考在使用上其實比較接近 C++ 的指標 (pointer)。

```rust
fn main()
{
    let a: i32 = 10;
    let pa: &i32 = &a; // 相當於 int* pa = &a;
    println!("pa points to value {}", *pa);
}
```

取得物件位址的方法，是使用 `&` 運算子，這點與 C++ 是相同的。另一個相同的地方是 `*` 運算子同樣表達解參考 (deference)，可以從位址取得目標物件的值。不同的地方則在於指標型別的表示法：在 C++ 中會以 `T*` 來表示「指向 `T` 型別的指標」，但在 Rust 中的表示方法則是 `&T`。

相似之處到此為止。為了同時達到執行效率與安全的要求，Rust 對參考型別的設計和主流語言有非常大的差異。

## Null Pointer

C++、Java 與 C# 都提供 null pointer，允許指標或參考不指向任何物件。然而一旦引入了 null，那麼操作任何參考型別之前，就必需檢查它是否為 null，否則會造成存取錯誤。Java 與 C# 是 memory safe 的語言，它們會犧牲一點執行效率，在存取物件成員的時候自動檢查物件參考是否為 null。而 C++ 則讓程式設計師自行選擇：你可以像 Java 與 C# 那樣犧牲一點效率來保證安全，或是冒著 undefined behavior 的風險略過檢查以追求效率。

```cpp
#include <iostream>
using namespace std;

struct Foo {
    int data;
};

int main()
{
    Foo* obj = nullptr;
    // 在 C# 或 Java 中，compiler 會在存取成員之前檢查參考是否為 null
    // 因此相同的情況會丟出 NullPointerException
    cout << obj->data << endl; // undefined behavior
    return 0;
}
```

Rust 沒有 null pointer。在你拿一個合法物件的位址來初始化指標之前，任何操作指標的行為都會被編譯器阻止。

```rust
fn foo(ptr: &i32)
{}

fn main()
{
    let pa: &i32;
    let val = *pa;  // compile error
    let pb = pa;    // compile error
    foo(pa);        // compile error
}
```

因為 Rust 中不存在 null pointer，因此當你的函式接收到參考型別時，你可以假設它必然指向一個合法的物件，不需要進行額外檢查，編譯器也不會在執行時額外花時間去檢查參考是否合法，進而提昇執行效率。

```rust
fn dump_i32(ptr: &i32)
{
    // 呼叫 dump_i32 時，ptr 保證指向一個合法的 i32 變數
    println!("value is {}", *ptr);
}
```

## Side Effect

另一個參考型別經常造成的問題是，對變數取位址後，該變數就有了別名 (alias)。所有對別名的操作都會反應到同一個變數上，產生了某些意想不到的結果。

迭代器失效 (iterator invalidation) 可以說是這方面最經典的示範：

```cpp
void duplicate_key(std::vector<int>& data, int key)
{
    for(auto it = data.begin(); it != data.end(); ++it){
        if(key == *it) {
            data.insert(it, key);
        }
    }
}
```

這段程式碼會尋訪 vector 中的所有元素，並且把所有符合條件的元素在相同位置複製一份。不幸的是，呼叫 `vector::insert` 時，有可能會因為預留空間不足而造成 vector 重新配置記憶體以容納更多元素。一旦這件事發生，所有參考到這個 vector 迭代器都會失效，繼續用它來尋訪 vector 是未定義行為。

這個問題的根源來自於對變數的寫入行為，會破壞所有指向該變數的參考。因此 Rust 做了這樣的設計：

1. 使用 `&` 取得變數的參考後，你只能透過參考讀取內容，而不能寫入資料。這樣的取址行為，Rust 稱之為 immutable borrow。
2. 若你想要透過參考寫入資料，必需透過 `&mut` 來取得位址。這樣的取址稱為 mutable borrow。
3. Rust 在編譯時會保證，任何變數經過取址後，要嘛同時有許多個 immutable borrow，或是只存在唯一一個 mutable borrow，不允許兩種取址方法同時存在，也不允許有多個 mutable borrow。

馬上來看一個例子：

```rust
fn main()
{
    let mut a = 10;
    {
        let pa = &a;     // OK
        let pb = &a;     // OK: 多個 immutable borrow 可同時存在
        let pc = &mut a; // Error: mutable borrow 與 immutable borrow 不可同時存在
    }
    let pd = &mut a;     // OK: 已脫離 pa 與 pb 的 scope，因此可以進行 mutable borrow
}
```

Rust 希望達成的目標是，當你對某一塊記憶體寫入資料時，編譯器可以保證這塊記憶體沒有其它的別名 (alias)，從而避免前述的迭代器失效問題。當然，Rust 的變數本身也是別名之一，因此除了上述三個條款之外，對被取址的變數還有以下條款：

1. 原本宣告為 mutable 的變數，經過 `&` 取址後，會暫時變為 immutable，直到所有的 immutable borrow 消滅為止。
2. 原本宣告為 mutable 的變數，經過 `&mut` 取址後，會暫時無法存取，直到該 mutable borrow 消滅為止。

以下的程式碼示範這兩個條款的作用：

```rust
fn main()
{
    let mut a = 10;
    {
        let pa = &a;           // OK: immutable borrow
        println!("a = {}", a); // OK: a 變成 immutable
        a = 20;                // Error: a 變成 immutable
    }

    a = 20;                    // OK: pa 已消滅，a 不存在 immutable borrow

    {
        let pb = &mut a;       // OK: mutable borrow
        println!("a = {}", a); // Error: 此時無法存取 a
        a = 30;                // Error: 此時無法存取 a
    }

    a = 30;                    // OK: pb 已消減，a 不存在 mutable borrow
}
```

這樣的設計，除了安全性的考量外，對於執行效率也有許多好處。考慮以下的 C++ 程式碼，以及使用 GCC 6.1 加上 `-O2` 編譯出來的結果：

```cpp
void foo(int* x, int* y, const int* z)
{
               //  movl    (%rdx), %eax
    *x += *z;  //  addl    %eax, (%rdi)
               //  movl    (%rdx), %eax
    *y += *z;  //  addl    %eax, (%rsi)
}
```

儘管這邊 `z` 的型別是 `const int*`，但在 C++ 中 `int*` 可以安全轉型為 `const int*`，導致 `x`, `y` 與 `z` 可能指向同一塊記憶體位址，因此在做完第一次加法後，編譯器需要再度把 `*z` 的值讀取到暫存器，才能進行加法。但在 Rust 中，由於有上述條款護身，編譯器看到 `x` 與 `y` 都是 mutable reference，便能假設它們指向不同的空間，而且寫入時完全不影響到 `*z` 的內容，因此產生更有效率的程式碼。[[1\]](https://electronic.blue/blog/2016/12/02-rust-an-introduction-reference-and-borrow-checker-1/#fn1)

```rust
fn foo(x: &mut i32, y: &mut i32, z: &i32)
{
               //  movl    (%rdx), %eax
    *x += *z;  //  addl    %eax, (%rdi)
    *y += *z;  //  addl    %eax, (%rsi)
}
```

## Lifetime

所有的變數都有生命週期 (lifetime)，而參考型別帶來最困難的問題，就是如何避免懸置參考 (dangling reference)，亦即參考所指向的物件已經消失，但是參考本身仍然可以存取的現象。

```
fn foo() -> &mut i32
{
    let mut data = 10;
    return &mut data;
}

fn main()
{
    let ptr = foo();
    *ptr = 20; // 應視為錯誤
}
```

`Java 與 C# 為了避免這種情況，嚴格限制了參考型別指向的目標。因此，這兩個語言都限制 class 僅能使用 heap 配置記憶體空間，並且運用 GC 來保證這些物件的生命週期比所有指向它的參考還要長`。然而 Rust 是系統程式語言，為了儘可能達到最高的執行效率，`Rust 允許使用者在 stack 上配置物件，並且取得它們的參考`。為了確保記憶體安全，Rust 設計了一套非常精細的規則來限制參考型別，保證在程式碼中所有能使用的參考，都必然指向一個有效的物件。

由於 lifetime borrow checker 是 Rust 中最複雜的功能，同時在其它程式語言中也非常罕見，因此我會在下一篇文章中講解這套規則，敬請期待。

------

1. 在 C99 標準中的 `restrict` 關鍵字可以用來宣告某個指標沒有別名，進而達到與 Rust 相同的最佳化效果。然而編譯器並不幫使用者檢查 `restrict` 的條件是否真正成立，使用者仍有可能不慎傳入指向相同位址的指標，這時候會導致未定義行為。 [↩](https://electronic.blue/blog/2016/12/02-rust-an-introduction-reference-and-borrow-checker-1/#fnref1)

我們看到了在 Rust 中使用參考的方法，以及 mutable / immutable borrow 的規則。在這篇文章中，我會說明 Rust 如何避免懸置參考造成的記憶體存取錯誤。

```rust
fn foo() -> &mut i32
{
    let mut data = 10;
    return &mut data;
}

fn main()
{
    let ptr = foo();
    *ptr = 20; // 應視為嚴重錯誤
}
```

在詳細說明這套規則之前，我想要先提出幾個案例進行思考：有些是我們想允許的功能，有些是我們想禁止的危險動作。在看過這些案例後，我們會比較容易理解為何 Rust 會設計出這套規則。



## 思考案例

在上面的程式碼中，若我們直接回傳參考，就會有傳回懸置參考的風險。但這是否表示所有函式都不應該回傳參考型別呢？

考慮以下的程式碼：

```rust
struct Point {
    x: i32,
    y: i32
}

fn test_1(p: &Point) -> &i32
{
    return &p.x;        // 應該合法
}

// 宣告全域常數，相當於 C++ 中的
// const static int SOME_CONST = 20;
static SOME_CONST: i32 = 20;

fn test_2() -> &i32
{
    return &SOME_CONST; // 應該合法
}
```

`test_1` 回傳的參考來自於參數，這件事應該是可行的。`test_2` 回傳指向全域常數的參考，這也是安全的。

我們可以嘗試讓函式僅能回傳來自全域常數或參數的參考，而禁止回傳其它參考。然而，若參考來自另一個內層函式的回傳值，我們沒有辦法可以簡單判斷這個值是否可以當作外層函式的回傳值。

```rust
fn test_3() -> &i32
{
    let p = Point { x: 10, y: 20 };
    return test_1(&p); // 應視為錯誤
}

fn test_4() -> &i32
{
    return test_2();   // 應該合法
}
```

另一方面，回傳參考只是眾多危險行為之一。考慮以下程式碼：

```rust
struct Packet {
    ptr: &i32
}

fn test_5() -> Packet
{
    let x = 10;
    let pak = Packet { ptr: &x }; // 只在函式內使用應該合法
    return pak;                   // 應視為錯誤
}
```

任何傳遞或儲存參考的行為都有風險，因此 Rust 使用型別系統 (type system) 來解決這個問題，其最核心的想法是：參考所指向的物件，其生命週期的長短也是參考型別的一部份。對生命週期很短的物件取址後，這個位址不能儲存在生命週期比它長的參考變數中。

## 生命週期規則

上面那句話聽起來很饒口，但說穿了也只是要符合以下的規則：

1. 若變數 x 的型別為 T，則對 x 取址後得到的參考型別為 `&'x T`，其中 `'x` 代表 x 的生命週期 (lifetime)。
2. 若 r 是指向某個 T 型別的參考，則 r 的型別為 `&'r T`，其中 `'r` 代表 r 的生命週期。
3. `&'x T` 與 `&'r T` 雖然都是指向 T 的參考型別，但因為對象的生命週期不同，因此不能被視為相同的型別。
4. 唯有當 `'x` 包含 `'r`，亦即 x 的生命週期完全涵蓋 r 的生命週期時，`&'x T` 才能被安全地轉型為 `&'r T`。

我們來看幾個範例：

```rust
fn main()
{
    let x = 10;
    {
        let r = &x;
        println!("*r = {}", *r);
    }
}
```

Rust 會把它轉變成以下的樣子：(示意用，並非合法 Rust code)

```rust
fn main()
{
    'x: {
        let x: i32 = 10;
        'r: {
            let r: &'r i32 = &x;
            println!("*r = {}", *r);
        }
    }
}
```

對 x 取址所得到的型別是 `&'x i32`，由於 x 的生命週期完全涵蓋 r 的生命週期，因此這個參考可以安全轉型為 `&'r i32`，可以儲存在參考 r 裡面。

我們來看看反例：

```rust
fn main()
{
    let r: &i32;
    {
        let x = 10;
        r = &x;
    }
    println!("*r = {}", *r);
}
```

Rust 允許先宣告變數，稍後再賦予初始值。然而在這個例子中，明顯地 x 的生命週期比 r 的生命週期還短，因此這樣的賦值行為會因為型別不符合而被編譯器報錯。

## 函式呼叫

不同的變數，即使屬於相同型別，也因為有不同的生命週期，取址後得到的參考型別也不同。因此若某個函式接受參考型別作為參數，該如何指定這個參考的生命週期就成為難題。

```rust
fn main()
{
    let x: i32 = 10;
    foo(&x);        // 此時參數型別是 &'x i32

    {
        let y: i32 = 20;
        foo(&y);    // 此時參數型別是 &'y i32
    }

    {
        let z: i32 = 30;
        foo(&z);    // 此時參數型別是 &'z i32
    }
}
```

`&x`、`&y` 與 `&z` 都是不同的型別，因此我們希望 `foo` 可以接收不同型別的參數，但使用一致的邏輯來進行處理。C++ 的泛型 (generics) 為這道難題帶來解答：

```rust
fn foo<'a>(a: &'a i32)
{
    let b = a; // OK: b 的生命週期保證被 'a 所涵蓋
    println!("*b = {}", *b);
}
```

這邊的 `'a` 代表外層在呼叫 `foo` 的時候，傳入參考所具備的生命週期。我們不知道 `'a` 究竟是哪個變數的生命週期，可能是 x、可能是 y 也可能是 z，但可以保證的是，因為它指向一個更外層的變數，因此生命週期 `'a` 一定可以涵蓋 `foo` 裡面任何內層變數的生命週期。

我們來試著實作前面的幾個案例：

```rust
struct Point {
    x: i32,
    y: i32
}

fn test_1<'a>(p: &'a Point) -> &'a i32
{
    return &p.x;        // OK
}

static SOME_CONST: i32 = 20;

fn test_2<'a>() -> &'a i32
{
    return &SOME_CONST; // OK
}
```

`test_1` 的參數及回傳型別看起來好像很複雜，但說穿了只是想表達這樣的規格：

> `test_1` 接受一個指向 `Point` 的參考，並回傳另一個**生命週期相同**，但指向 `i32` 的參考。

由於 `p` 指向一個 struct，很自然地，該 struct 的所有成員與 struct 本身具備了相同的生命週期。因此對 `p.x` 取址時，其型別會是 `&'a i32`。

`test_2` 的寫法則是這樣的意思：

> `test_2` 不接受參數，而會回傳一個生命週期任君挑選，但指向 `i32` 的參考。

由於 `SOME_CONST` 是個全域常數，它擁有整個程式中最長的生命週期，因此它的位址可以安全轉型為具備任意生命週期、且指向 `i32` 的參考 (`&'a i32`)。

所謂「最長的生命週期」帶有其特殊涵意，因此 Rust 使用 `'static` 這個符號來代表它。上面的 `test_2` 可以做這樣的改寫：

```rust
fn test_2() -> &'static i32
{
    return &SOME_CONST;
}
```

這樣寫的意思就很清楚：`test_2` 會回傳一個指向全域常數的參考。由於回傳值所指向的物件具有最長的生命週期，因此任何指向 `i32` 的參考都可以安全地儲存這個回傳值。

我們繼續實作前面的範例：

```rust
fn test_3<'a>() -> &'a i32
{
    let p = Point { x: 10, y: 20 };
    return test_1(&p); // Error
}

fn test_4<'a>() -> &'a i32
{
    return test_2();   // OK
}
```

在 `test_3` 當中，呼叫 `test_1(&p)` 是合法的，它會回傳一個生命週期與 p 相同的參考。然而回傳型別 `&'a i32` 代表回傳參考的生命週期必需要涵蓋外層某個更長的生命週期，而 p 只是內層的區域變數，其生命週期顯然比 `'a` 還要短，因此我們不能拿 `test_1(&p)` 的結果當回傳值。

而在 `test_4` 中，因為我們知道 `test_2` 回傳值的生命週期為 `'static`，因此外層的 `test_4` 可以將它的結果轉為任意生命週期的參考並作為回傳值。當然，你也可以直接讓 `test_4` 回傳 `&'static i32`。

## Struct 中的參考

我們可以用泛型函式來處理生命週期不同的參考，當然也可以用泛型來宣告 struct 中的參考成員：

```rust
struct Packet<'a> {
    ptr: &'a i32
}

fn test_5<'a>() -> Packet<'a>
{
    let x = 10;
    let pak = Packet { ptr: &x }; // OK
    return pak;                   // Error
}
```

`Packet<'a>` 的意思是：這個 struct 裡面帶有一個以上的參考型別，指向某生命週期為 `'a` 的變數。因此這個 struct 的生命週期必需**被** `'a` 所涵蓋，以免發生懸置參考。

在 `test_5` 中，由於 `pak` 的生命週期被 `x` 的生命週期所涵蓋，所以第 8 行的宣告可通過編譯。然而因為 `'a` 代表外層另一個更長的生命週期，比 `pak` 的生命週期更長，因此編譯器阻止你回傳 `pak`。

## 生命週期與 borrow checker

Rust 讓生命週期成為型別的一部份，除了有助於消除懸置參考，還能夠協助上一篇文章中提到的 borrow checker：

> Rust 在編譯時會保證，任何變數經過取址後，要嘛同時有許多個 immutable borrow，或是只存在唯一一個 mutable borrow，不允許兩種取址方法同時存在，也不允許有多個 mutable borrow。

考慮以下程式碼：

```rust
struct Point {
    x: i32,
    y: i32
}

fn test_6<'a>(p: &'a Point) -> &'a i32
{
    // ...
}

fn main()
{
    let mut p = Point { x: 10, y: 20 };
    let r1 = test_6(&p);
    let r2 = &mut p;
}
```

我們不知道 `test_6` 做了什麼事情，然而他的參數接受一個指向 `Point` 參考，卻能回傳另一個生命週期相同，但指向 `i32` 的參考，因此讓編譯器做出這樣的推論：

> 若函式的回傳值帶有與輸入參數相同的生命週期，我們可以推論回傳值就是參數本身，或是參數底下的成員。

這個推論雖然大膽，但也有其道理。因為輸入參數的生命週期 `'a` 可以代表任何變數的生命週期，你要怎麼找到另一個生命週期與 `'a` 相同、或比它更長的變數呢？當然，你可以對全域常數取出生命週期為 `'static` 的參考作為合法回傳值。但既然 `test_6` 回傳型別的生命週期不是 `'static` 而是 `'a`，編譯器就可以認定它的回傳值**可能**來自於 `p` 的成員。

既然 `r1` 可能指向 `p` 的某個成員，因此 `r1` 就符合 immutable borrow 的條件。在 `r1` 尚未消滅前，對 `p` 進行 mutable borrow 的動作就會被編譯器擋下來。

這個推導規則對任何包含生命週期的型別都適用，自然對包含參考的 struct 也適用：

```rust
struct Container {
    // 容器定義
}

struct Iterator<'a> {
    // 迭代器定義
}

fn iterate<'a>(c: &'a Container) -> Iterator<'a>
{
    // 產生一個可巡訪容器的迭代器
}

fn push_back<'a>(c: &mut 'a Container, e: i32)
{
    // 在容器中加入新的元素
}

fn main()
{
    let mut c = Container { ... };
    let it = iterate(&c);
    loop {
        // 操作迭代器
        push_back(&mut c, 10); // Error
    }
}
```

當我們呼叫 `iterate(&c)` 時，由於回傳的型別是個生命週期與 `c` 相同的 struct，因此編譯器判定這個函式產生了 immutable borrow。這個迭代器本身雖然是 struct 而不是參考，但只要它尚未消滅，後續對 `c` 的 mutable borrow 就就會被編譯器阻止。

## 簡寫泛型涵式

若每個有接收參考或回傳參考的函式都要寫成泛型，對於撰寫或閱讀上都不太方便，因此 Rust 提供了省略生命週期的簡單寫法：

```rust
fn test_1(p: &Point) -> &i32
{ /* ... */ }
```

當 Rust 發現函式的輸入參數只有其中一項是參考，而回傳型別也需要標注生命週期時，會自動假設回傳值的生命週期等於參數的生命週期，因此上面的函式宣告與以下效果完全相同：

```rust
fn test_1<'a>(p: &'a Point) -> &'a i32
{ /* ... */ }
```

若參數有兩個以上的參考型別，或完全沒有參考型別時，編譯器無法自動推導回傳型別的生命週期。你得寫出完整的泛型宣告，或是不回傳任何需要生命週期的型別。

```rust
fn test_3() -> &i32 // Error: 沒有參數，不可省略 lifetime
{ /* ... */ }

struct Container
{ /* some members... */ }

struct Iterator<'a> {
{ /* some reference to a Container object */ }

fn test_5() -> Iterator // Error: 沒有參數，不可省略 lifetime
{ /* ... */ }

fn iterate(c: &Container) -> Iterator // OK: 回傳的 Iterator 生命週期與 c 相同
{ /* ... */ }

fn test_7(x: &i32, y: &i32) -> &i32 // Error: 兩個以上的輸入參考
{ /* ... */ }

fn test_8(x: &i32, y: &i32) -> i32 // OK: 不回傳參考，不需指定生命週期
{ /* ... */ }
```

## 結語

為了追求效率，Rust 允許在 stack 上配置物件並操作參考，並且在編譯時期就盡可能阻檔一切可能造成記憶體錯誤的行為。在這兩篇文章中，我詳細介紹了這套最複雜也最獨特的參考型別系統。實務上由於參考有生命週期的限制，因此在大型資料結構中，通常會使用智慧指標 (smart pointer) 來指向其它物件。在後續文章中，我會介紹這些標準函式庫提供的工具。