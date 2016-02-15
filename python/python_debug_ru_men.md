# python debug 入門


記得剛學寫 C 時, Scott 一再告誡我們不要依賴 printf 或 debugger, 要自己能掌握全局, 想清楚整個邏輯是怎麼一回事再動手。矯枉過正的結果, 就是幾乎沒什麼在用 debugger。偶而用一下不順, 又放棄使用, 覺得還是配合 logger、讀通原始碼和 unit test, 才是正規解法。不過有時候還是用 debugger 比較省事, 最近開始加減練習用 debugger 來 trace code。

pdb 入門方式滿簡單的, 兩種執行方法：

- python -m pdb YOUR_MODULE
- 在 YOUR_MODULE 開頭寫


```py
import pdb
pdb.set_trace()
```

這篇用一個簡單的例子說明 pdb 有多方便, 一邊執行一邊觀察, 還可以順便試看看怎麼改才對, 再繼續跑, 看即時寫入的修正碼是否有效。反正是 interpreter, 可在執行期間隨意插入程式碼。

[這篇說明常用指令](https://pythonconquerstheuniverse.wordpress.com/2009/09/10/debugging-in-python/), 比官方死版版的文件清楚多了。

若希望看到彩色的輸出, 可以用 ipdb, 站在 ipython 的肩膀上, ipython 有的好處, ipdb 都有!! 不過python -m ipdb YOUR_MODULE 的用法, 只有在 python2.7 開始才有效。