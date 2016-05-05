# 两种将gdb的输出信息存到文件的方法


- `gdb 可以用 , cgdb 會有格式亂碼問題`

有时候输出信息太长，屏放不下，下面介绍两种将gdb的输出信息存到文件的方法。

##方法一：适合临时向文件输出些信息的情况。
比如要用info functions输出所有函数，结果往往有一大坨，所以可以将之输出到文件。
(gdb) set logging file <file name>
(gdb) set logging on
(gdb) info functions
(gdb) set logging off

http://hellogcc.blogbus.com/logs/69799908.html

##方法二：适合整个gdb会话期间都重定向输出的情况。

gdb |tee newfile 