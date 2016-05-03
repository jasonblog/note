# 懶人除錯法


用法如下:
1. 將 debug.sh, gdbgen.py 跟 test.cpp 放在同一個目錄，同時 chmod +x debug.sh gdbgen.py

2. 執行 ./debug.sh
  這時候會自動編出 test.exe
  自動展出 test.gdb 和 test.bps ( break points for debug )
  還有 test.log.gdb ( 放 gdb 執行的紀錄 )

3. 執行 `grep 'CMD' test.log.gdb` 可以看到簡化的除錯紀錄如..

```sh
CMD>>===leave d()===
CMD>>$5 = 8
CMD>>===enter e()===
CMD>>===enter d()===
CMD>>===enter b()===
CMD>>===enter a()===
CMD>>===leave a()===
CMD>>count:
CMD>>===leave d()===
CMD>>===enter a()===
CMD>>===leave a()===
CMD>>$6 = 12
```

或是直接察看 test.log.gdb 裡詳細的除錯紀錄。

###程式原理:

- 希望可以類似 doxygen, 只要在程式寫上一些註解，就能自動展開 gdb debug script 並自動批次執行. 

如此一來，我們就可以不用每次費神去查出每個不斷變動的除錯斷點，同時也能用批次執行來取代每次用手動的執行 gdb 指令。

讓使用者也能自行執行 debug.sh，就能方便取得除錯紀錄轉交 developers 。過程不需要再行另外教使用者執行那些  gdb 除錯指令。

- 在程式行間加上 "// gdb: " 的標記，然後後面寫上 gdb 的 commands

```c
void b()
{
    // gdb: echo ===enter b()===
    cout << "bbbb" << endl;
    a();
    count--;
    // gdb: echo count:
    // gdb: print count
    // gdb: echo ===leave b()===
}
```

-  debug.sh 跟 gdbgen.py 會合力爬出程式裡相關的 marks 跟行號，然後輸出 gdb auto scripts, 並自動執行。其中 test.bps 就是各指定斷點自動展開的命令區塊。

-  所有的除錯資訊，包括 call stack 跟 變數資料，都是 run-time 的。用 "backtrace 2" 再加上適當的縮排跟 parsing, 可以作出 run-time function invorking tree.

大概是這樣，希望給有興趣的人參考看看，有不清楚的地方，也可以晚上一起來試玩看看 :-)

sincerely, Mat.


- debug.sh

```sh
#!/bin/bash

g++ -g test.cpp -o test.exe

grep -Hn "gdb:" test.cpp | ./gdbgen.py > test.bps

cat > ./test.gdb <<EOF
set break pending on
set pagination off
set logging on ./test.log.gdb
set verbose off

# set the most earlier entry point
break main 

run 

# read the generated breakpoints
source ./test.bps 

continue
quit
EOF

gdb -batch -x ./test.gdb ./test.exe > ./test.log  &

```

- gdbgen.py

```py
#!/usr/bin/python                                                                                 

import os
import sys
import subprocess

for line in sys.stdin:

  # get line
  d1 = line.find(":")
  bp_file= os.path.basename(line[:d1])
  d2 = line.find(":",d1+1)
  bp_line = line[d1+1:d2]

  # get commands
  bp_tag="gdb:"
  d3 = line.find( bp_tag, d2+1 )
  bp_cmds = line[d3+bp_tag.__len__():]

  # pipe out
  sys.stdout.write( "break " + bp_file + ":" + bp_line + "\n")
  sys.stdout.write( "commands" + "\n")
  sys.stdout.write( "  " + "echo CMD>>" + "\n" )
  sys.stdout.write( "  " + bp_cmds )
  sys.stdout.write( "  " + "echo \\n" + "\n" )
  sys.stdout.write( "  " + "bt 2" + "\n")
  sys.stdout.write( "  " + "continue" + "\n")
  sys.stdout.write( "end" + "\n\n")
```

- test.cpp

```cpp
#include <iostream>
#include <string>

using namespace std;

int count = 3;

void a()
{
    // gdb: echo ===enter a()===
    cout << "aaaa" << endl;
    count++;
    // gdb: echo ===leave a()===
    // gdb: print count
}
void b()
{
    // gdb: echo ===enter b()===
    cout << "bbbb" << endl;
    a();
    count--;
    // gdb: echo count:
    // gdb: print count
    // gdb: echo ===leave b()===
}
void c()
{
    // gdb: echo ===enter c()===
    cout << "cccc" << endl;
    count++;
    a();
    count++;
    b();
    // gdb: print count
    // gdb: echo ===leave c()===
}
void d()
{
    // gdb: echo ===enter d()===
    cout << "dddd" << endl;
    count++;
    b();
    // gdb: echo ===leave d()===
}
void e()
{
    // gdb: echo ===enter e()===
    cout << "eeee" << endl;
    d();
    count++;
    a();
    count++;
    // gdb: print count
    // gdb: echo ===leave e()===
}


int main()
{
    a();
    // gdb: print count
    b();
    // gdb: print count
    c();
    // gdb: print count
    d();
    // gdb: print count
    e();
}
```
