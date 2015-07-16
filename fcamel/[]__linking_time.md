# [轉] 加速 linking time

頻繁修改一點程式又要執行看結果時, 主要的時間都花在 linking。這時才體會到省下 linking time 也是很重要的事。

這篇提到兩個減少 linking time 的做法

- 使用 ramdisk
- 用 gold
我本來就有用 SSD, 改將編譯結果全放到 ramdisk 後, 提昇的效果不怎麼明顯。到是用 gold 後減少了一半的 linking 時間。

不過 gold 也不是那麼完美, 這篇提到一些問題。我自己用的時候發覺滿常遇到 ld 可以編, 但 gold 不行。最後的解套方式是寫個小 script 切換 /usr/bin/ld 連到的程式。不常編的東西就暫時換回 ld 連結個一次就好。

```
#!/bin/bash

LD=/usr/bin/ld
GNULD=${LD}.bfd
GOLD=${LD}.gold


current=$(\ls -l $LD | awk '{print $NF}')
if [ $current = $GNULD ]; then
    sudo ln -fs ${GOLD} ${LD}
    echo "switch ld to ${GOLD}"
else
    sudo ln -fs ${GNULD} ${LD}
    echo "switch ld to ${GNULD}"
fi
```

另外 jserv 提到 gcc 有命令列參數可直接指定用那個 linker (ref.)。wens 則查到 Debian/Ubuntu changelog 裡有寫 下可用 -B/usr/lib/compat-ld 和 -B/usr/lib/gold-ld for ld.gold。

順便 google 一下看怎麼查 changelog, 似乎是看 /usr/share/doc/binutils/changelog.Debian.gz 或用 aptitude changelog PKG, 不過這兩個作法都只有最近的 changelog, 沒有完整記錄, 像查 binutils 時就沒有找到上面提的參數說明。
