# C 的 Thread Pool 筆記


最近被丟到 FreeBSD 跟 C 的世界裡面，沒有 Scala 的 Actor 可以用。 所以只好參考別人的 Thread Pool 來看一下，在 C 的世界裡面 Thread Pool 是怎麼實作的。

首先先去 StackOverFlow 中尋找是否有人問過類似的問題

Existing threadpool C implementation

裡面有提到幾個 C Thread Pool 的實作範例與可參考的文件

- [threadpool-mbrossard](https://github.com/mbrossard/threadpool)
- [threadpool-jmatthew](http://people.clarkson.edu/~jmatthew/cs644.archive/cs644.fa2001/proj/locksmith/code/ExampleTest/)
- [cthreadpool](http://sourceforge.net/projects/cthpool/)
- [C-Thread-Pool](https://github.com/Pithikos/C-Thread-Pool)

我這邊是直接應該是會以 threadpool-mbrossard 作為第一個研究的版本，因為他一直有在維護。 而且作者就是 [Existing threadpool C implementation](http://stackoverflow.com/questions/6297428/existing-threadpool-c-implementation) 的發文者，感覺他還蠻熱心的。


##threadpool
`A simple C thread pool implementation`

Currently, the implementation:

- Works with pthreads only, but API is intentionally opaque to allow other implementations (Windows for instance).
- Starts all threads on creation of the thread pool.
- Reserves one task for signaling the queue is full.
- Stops and joins all worker threads on destroy.



![](./images/20140124164010281.png)