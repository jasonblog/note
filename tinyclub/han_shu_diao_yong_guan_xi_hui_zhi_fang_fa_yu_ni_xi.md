# 函數調用關係繪製方法與逆向建模


  - c
  - clang
  - dot
  - fdp
  - FlameGraph
  - gcc
  - graph-easy
  - Linux
  - twopi
  - UML
  - 函數調用，tree
  - 源碼分析
categories:
  - C
  - 源碼分析
---


## 緣由

源碼分析是程序員離不開的話題。無論是研究開源項目，還是平時做各類移植、開發，都避免不了對源碼的深入解讀。

工欲善其事，必先利其器。

前三篇分別介紹瞭如何靜態/動態分析源碼的函數調用關係（流程圖），並介紹了三種不同的展示方式，這裡再來回顧並介紹更多展示方式。

這幾篇文章的思路彙總起來就是如何利用各類工具從源碼或者從程序運行的情況逆向構建源碼的模型，可以說是逆向建模，只不過這裡只是停留在函數調用關係層面，所以在本文的後面，我們抽出一個章節來初步討論如何逆向建模。

## tree

樹狀調用關係是最常見的一種展示方式，包括 `calltree`, `cflow` 等的默認輸出結果都是如此。

這裡補充介紹兩個工具，都是編譯器，一個是 `clang`，一個是 `gcc`。

### clang

<pre>$ sudo apt-get install clang
$ clang -cc1 -ast-dump test.c 2>/dev/null | egrep "FunctionDecl|Function "
|-FunctionDecl 0x2eb9350 &lt;test.c:3:1, col:14> a 'int (void)'
|-FunctionDecl 0x2eb94e0 &lt;line:4:1, col:21> b 'int (int)'
|       `-DeclRefExpr 0x2eb9588 &lt;col:16> 'int (void)' Function 0x2eb9350 'a' 'int (void)'
|-FunctionDecl 0x2f027f0 &lt;line:6:1, line:15:1> main 'int (void)'
|   |   `-DeclRefExpr 0x2f02970 &lt;col:9> 'int (void)' Function 0x2eb9350 'a' 'int (void)'
|   | | `-DeclRefExpr 0x2f029d8 &lt;col:9> 'int (int)' Function 0x2eb94e0 'b' 'int (int)'
|   | | `-DeclRefExpr 0x2f02cc0 &lt;col:9> 'int (const char *restrict, ...)' Function 0x2f02b70 'scanf' 'int (const char *restrict, ...)'
`-FunctionDecl 0x2f02b70 &lt;line:12:9> scanf 'int (const char *restrict, ...)' extern
</pre>

### gcc

較新版本的 `gcc` 在編譯過程中可以直接生成流程圖，以之前用到 `fib.c` 為例：

`$ gcc fib.c -fdump-tree-ssa-graph=fib`

上述命令會導出 `fib.dot`，處理後就是流程圖，不過只是展示了函數內部的情況，函數間的關係未能體現。

另外，需要補充的是，`-fdump-tree-cfg-graph` 有 BUG，生成的 `dot` 文件缺少文件頭，轉換下查看效果。

`$ cat fib.dot | dot -Tsvg -o fib.svg`

效果如下：


![](./images/fib-gcc-dump-graph.svg)

## Graphviz: dot / twopi / fdp

`tree2dotx` 能夠將上述諸多標準的 `tree` 狀結構轉換為 `dot` 格式，並通過 Graphviz 的 `dot` 工具進一步轉換為 svg 等可以直接顯示的圖文。

實際上，除了 dot 工具，Graphviz 還提供了另外幾組類似的工具，通過 `man dot` 可以看到一堆：

  * dot &#8211; filter for drawing directed graphs
  * neato &#8211; filter for drawing undirected graphs
  * twopi &#8211; filter for radial layouts of graphs
  * circo &#8211; filter for circular layout of graphs
  * fdp &#8211; filter for drawing undirected graphs
  * sfdp &#8211; filter for drawing large undirected graphs
  * patchwork &#8211; filter for tree maps

通過驗證發現，`fdp` 針對 `tree2dotx` 的結果展示效果不錯：

`$ cd linux-0.11/
$ cflow -b -m main init/main.c | tree2dotx | fdp -Tsvg -o linux-0.11-fdp.svg`

效果如下：


![](./images/linux-0.11-fdp.svg)

## graph-easy

`graph-easy` 是另外一個展示 `dot` 圖形的方式，有點類似 Graphviz 提供的上述工具中的一種，不過它有點特別，展示的結果儘可能做到整體對齊平鋪，有點像硬件原理圖的感覺。

先安裝：

`$ sudo perl -MCPAN -e 'install Graph::Easy'
$ sudo perl -MCPAN -e 'install Graph::Easy::As_svg'`

用法：

`$ cflow -b -m main init/main.c | tree2dotx | \
        graph-easy --as=svg --output=linux-0.11-graph-easy.svg`

可以看到 `graph-easy` 的輸出結果又是另外一種風格：

![](./images/linux-0.11-graph-easy.svg)


## FlameGraph

`FlameGraph` 之前主要是用於展示程序運行時的動態數據，實際上它也可以用來展示 `dot`，我們只要把 `dot` 轉換為 `Flame` 需要的數據格式就好。

剛把 `tree2dotx` 改造了一下，添加了 `-o [dot|flame]` 參數以便支持 `FlameGraph` 採用的 `Flame` 格式。

&#8220;\` $ wget -c https://github.com/tinyclub/linux-0.11-lab/raw/master/tools/tree2dotx $ sudo cp tree2dotx /usr/local/bin/

$ cflow -b -m main init/main.c | tree2dotx -o flame | flamegraph.pl > linux-0.11-flame.svg &#8220;\`

效果如下：


![](./images/linux-0.11-flame.svg)

## 逆向建模

上述源碼分析的諸多努力其實都是希望理順源碼的結構，而逆向建模則是這類努力的更專業的做法。

### 普通繪圖工具

上述工具最終都可以轉換為一種可以編輯的 svg 圖片格式，如果想把這些圖片用到書籍或者演示文稿中，那麼可能還需要編輯或者轉換，推薦 `inkscape`。

如果要製作流程圖，比較推薦 `dia` 或者在線的工具 <https://www.draw.io/>。

另外，還有一種純文本的繪圖工具：<http://asciiflow.com/>，也非常有趣，這種圖可以直接跟文本一起貼到文檔裡頭，不過字體得用等寬字體，否則圖文排版的時候會亂掉。

### UML 建模工具

除此之外，還有一些逆向建模工具，可以根據源碼甚至二進制可執行文件直接進行逆向 UML 建模。不過暫時未能找到針對 C 語言的開源逆向建模工具。

而不支持逆向的建模工具則比較多，開源的有：

  * [yEd][6]

  * [yUML][7]

  * [OpenAmeos][8]

最後一筆工具還支持從建模語言生成代碼模板。不過 OpenAmeos 的安裝有點麻煩，而且其界面很難看。

OpenAmeos 安裝時的注意事項：

  * 需要註釋掉install: `export LD_ASSUME_KERNEL`
  * 把 `/path/to/Ameos_V10.2/bin` 路徑加入 PATH 配置
  * 安裝圖形庫 `libmotif*` 並修改 `bin/ameos` 裡頭的 `MOTIFHOME:=/usr/X11R6` 為 `MOTIFHOME:=/usr/`

不過據說有一些收費的軟件支持 C 語言逆向建模，比如 Enterprise Architect，IDA，更多的建模工具見：

  * [UML 中國收集的相關工具一覽][9]
  * [開源中國收集的 UML 工具][10]

### C 語言模塊化開發

傳統的 C 語言開發蠻多隻是注重基本的編程風格（Coding Style），不像面向對象化編程，關於結構化、模塊化的設計規範討論很少。

恰好有同學在嘗試用模塊化的方法開發 C 語言程序，是不錯的嘗試：[模塊化 C 代碼與 UML 對象模型之間的映射][11]。

## 小結

截止到該篇，整個源碼分析（函數級別）暫時告一段落。關於代碼行層面的分析，我們放到以後再做，大家也可以提前瞭解 `gcov` 和 `kgcov` 這兩個工具，它們分別針對應用程序和內核空間。





 [1]: http://tinylab.org
 [2]: /wp-content/uploads/2015/04/callgraph/fib-gcc-dump-graph.svg
 [3]: /wp-content/uploads/2015/04/callgraph/linux-0.11-fdp.svg
 [4]: /wp-content/uploads/2015/04/callgraph/linux-0.11-graph-easy.svg
 [5]: /wp-content/uploads/2015/04/callgraph/linux-0.11-flame.svg
 [6]: http://www.yworks.com/en/products_download.php
 [7]: http://www.yuml.me/diagram/scruffy/usecase/draw
 [8]: https://www.scopeforge.de/cb/project/8
 [9]: http://www.umlchina.com/Tools/Newindex1.htm
 [10]: http://www.oschina.net/project/tag/177/uml?sort=view&lang=0&os=37
 [11]: http://www.uml.org.cn/oobject/201201121.asp
