# 查看Linux內核編譯配置


有時候我們需要查看Linux系統的內核是否在編譯的時候的編譯選項，從而確定某個模塊是否已經加入到內核以及參數設置等等。

方法有兩種：

```sh
# zcat /proc/config.gz
# cat /boot/config-$(uname -r)
```

第一種方法要求在內核編譯時增減相應的選項才會生成，很多系統找不到/proc/config.gz這個文件。不過第二種方法通常不會遇到什麼問題。準確的說，這兩個文件就是內核編譯使使用的config文件。

比如查看bcache是否編譯進了內核：

```sh
# cat /boot/config-$(uname -r) | grep BCACHE
```

