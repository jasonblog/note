# Manual FIle Locking


类似fputs这样的标准文件IO函数均为线程安全，即函数内部会加锁解锁，但如果是大量文件读写，可以采用flockfile函数来进行统一加锁减锁，而内部采用线程不安全的文件IO。

例如

```c
fputs("XXX",stream);

fputs("YYY",stream);
fputs("ZZZ",stream);
```

可以改写为

```c
flockfile(stream)

fputs_unlocked("XXX",stream);

fputs_unlocked("YYY",stream);
fputs_unlocked("ZZZ",stream);

funlockfile(stream)
```

这样由三次加锁减锁操作，变为了一次，提高了效率。而且确保了XXX，YYY，ZZZ的顺序写入，即保证了写入顺序。



以上参考 《Linux 系统编程》 ROBERT LOVE著 P80