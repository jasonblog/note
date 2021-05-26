單元測試
========

寫程式難免會出錯，所以我們要寫點程式來測試我們的程式，這邊的程式並不難，你可以試著打開 Rust Playground 一起操作。

```rust
fn add(a: i32, b: i32) -> i32 {
  a + b
}

// 只在測試模式時編譯這個模組
#[cfg(test)]
mod tests {
  // 將上層的東西全部引入
  use super::*;

  // 標記這個是一個測試
  #[test]
  fn test_add_should_work_correctly() {
    // assert_eq! 會確定兩邊是相等的，若不是就會 panic
    assert_eq!(add(1, 1), 2);
    assert_eq!(add(2, 3), 5);
  }
}
```

上面的程式碼當你在 Rust Playground 中輸入後，你應該會注意到上面原本通常都是 `RUN` 的按鈕變成了 `TEST` ，按下去就對了，你應該會看到以下結果：  

```plain
running 1 test
test tests::test_add_should_work_correctly ... ok

test result: ok. 1 passed; 0 failed; 0 ignored; 0 measured; 0 filtered out
```

這代表我們的測試通過了，於是我們來加個不會通過的測試試試，請加上面的程式碼中的 tests 模組中：  

```rust
#[test]
fn test_this_should_fail() {
  assert_eq!(add(2, 2), 5);
}
```

這次應該會是這樣的輸出：

```plain
running 2 tests
test tests::test_add_should_work_correctly ... ok
test tests::test_this_should_fail ... FAILED

failures:

---- tests::test_this_should_fail stdout ----
thread 'tests::test_this_should_fail' panicked at 'assertion failed: `(left == right)`
  left: `4`,
 right: `5`', src/lib.rs:21:7
note: Run with `RUST_BACKTRACE=1` for a backtrace.


failures:
    tests::test_this_should_fail

test result: FAILED. 1 passed; 1 failed; 0 ignored; 0 measured; 0 filtered out
```

這應該要是個錯誤的範例，然而它真的「錯誤」了，我們來修正它，在  `#[test]` 的底下加上 `#[should_panic]` ，像這樣：  

```rust
#[test]
#[should_panic] // 標記測試應該要失敗
fn test_this_should_fail() {
  assert_eq!(add(2, 2), 5);
}
```

再按一次測試，通過了吧。

你也可以使用回傳 `Result` 的方式來測試：

```rust
#[test]
fn test_use_result() -> Result<(), &'static str> {
  if add(1, 1) == 2 {
    Ok(())
  } else {
    Err("1 + 1 != 2")
  }
}
```

只是這個方法就沒辦法使用 `#[should_panic]` 了，看哪個比較方便了，這邊的 `Result` 的 `Err` 中的值只要能用 `{:?}` 印得出來就行了 (以 Rust 的說法就是有實作 `Debug`)，此外事實上這邊的函式名稱也沒有規定一定要什麼格式，只要有標記 `#[test]` 就行了，分到另一個模組也是個慣例。

斷言 (Assert)
-------------

程式語言常提供的一個功能就是斷言，它可以幫助你測試某些情況是不是真的成立，通常不成立時就會結束程式，上面所使用的 `assert_eq!` 就是其中一個，它們不只可以被使用在測試中，也可以用在一般程式，確認環境或參數符合假設的必要條件，比如不該用任何數除以 0 。

Rust 中一共提供了三種斷言：

- `assert!`: 最基本款的，測試裡面的條件為真，如果裡面的條件為假 (`false`) 則引發 panic

```rust
assert!(false);
```

```plain
thread 'tests::demo' panicked at 'assertion failed: false', src/lib.rs:27:9
```

- `assert_eq!`: 測試兩個參數是相等的，這個上面已經用過了
- `assert_ne!`: 測試兩個參數是不相等的

```rust
assert_ne!(1, 1);
```

```plain
thread 'tests::demo' panicked at 'assertion failed: `(left != right)`
  left: `1`,
 right: `1`', src/lib.rs:27:9
```

它們的用法都大同小異，同時它們還支援自訂訊息：

```rust
assert!(false, "是 false");
```

```plain
thread 'tests::demo' panicked at '是 false', src/lib.rs:27:9
```

後面也可以放格式化字串跟參數。

有人可能會擔心這些斷言會不會影響程式的效能，所以上面的斷言也都提供了除錯的版本，分別為： `debug_assert!` 、 `debug_assert_eq!` 、 `debug_assert_ne!` ，它們只在除錯模式時有作用， release 時是沒作用的，除錯模式就是平常使用 `cargo build` 產生的版本，那 release 版本的怎麼產生呢？使用 `cargo build --release` 就好了，這會花比較長的時間編譯，此外 `cargo run` 也可以使用 `cargo run --release` 執行 release 版本的程式。

下一篇我們要來介紹 Rust 中的函數式程式設計，以及 Rust 中的閉包 (Closure) 。
