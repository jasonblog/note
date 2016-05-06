# 兩種將gdb的輸出信息存到文件的方法


- `gdb 可以用 , cgdb 會有格式亂碼問題`

有時候輸出信息太長，屏放不下，下面介紹兩種將gdb的輸出信息存到文件的方法。

##方法一：適合臨時向文件輸出些信息的情況。
比如要用info functions輸出所有函數，結果往往有一大坨，所以可以將之輸出到文件。
(gdb) set logging file <file name>
(gdb) set logging on
(gdb) info functions
(gdb) set logging off

http://hellogcc.blogbus.com/logs/69799908.html

##方法二：適合整個gdb會話期間都重定向輸出的情況。

gdb |tee newfile 