# 如何正確地使用設計模式？


除非你還在寫 Java，否則設計模式真沒多大用處。

---- 補充幾句 ----

首先明確一點，這裡說的「設計模式」，是「面向對象設計模式」的簡稱，以 GoF 書中列出的 23 個模式為代表。
不要把什麼都歸為模式，比如 「if-then-else 模式」、「for each 模式」就不是（面向對象設計）模式。Like, the inverse, reverse, double-back-flip pattern—whatever. Oh, you mean a loop? OK. --- Jamie Zawinski
類似的，thread pool、thread-per-connection、process-per-connection、thread local storage、scoped locking、readers-writer lock 等等也不是面向對象設計模式。

贊同 @winter 說的 「建議不要泛泛地討論和學習『設計模式』，而是具體到每一個模式。」

- ，很多創建型模式就沒用了。
- 如果你用的語言能把函數像變數一樣賦值並傳來傳去，很多行為模式就沒用了。
- 如果你用的語言 style 反對疊床架屋的 class hierarchy，很多結構模式就沒用了。


幾個簡單具體的代碼示例：

1. C# 內置了 delegate/event，Observer 模式就沒用了。

2. 如果你的語言的 typeof 操作符能得到 runtime type，那麼 Prototype 模式就沒用了。比如

```java
class Base(object):
  pass

class Derived(Base):
  pass

def clone(x):
  return type(x)()

if __name__ == '__main__':
  d = Derived()
  print type(d)  # <class '__main__.Derived'>
  c = clone(d)
  print type(c)  # <class '__main__.Derived'>
  b = Base()
  print type(b)  # <class '__main__.Base'>
  a = clone(b)
  print type(a)  # <class '__main__.Base'>
```