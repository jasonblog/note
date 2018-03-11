# Manual FIle Locking


類似fputs這樣的標準文件IO函數均為線程安全，即函數內部會加鎖解鎖，但如果是大量文件讀寫，可以採用flockfile函數來進行統一加鎖減鎖，而內部採用線程不安全的文件IO。

例如

```c
fputs("XXX",stream);

fputs("YYY",stream);
fputs("ZZZ",stream);
```

可以改寫為

```c
flockfile(stream)

fputs_unlocked("XXX",stream);

fputs_unlocked("YYY",stream);
fputs_unlocked("ZZZ",stream);

funlockfile(stream)
```

這樣由三次加鎖減鎖操作，變為了一次，提高了效率。而且確保了XXX，YYY，ZZZ的順序寫入，即保證了寫入順序。



以上參考 《Linux 系統編程》 ROBERT LOVE著 P80