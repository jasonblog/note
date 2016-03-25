# gcc -E -P

文件1"header1.h"，內容如下：
```c
#define A1 10
#define A2 0x0A
#define A3 (A1+1)
```

文件2"header2.h"，內容如下：

```c
#include "header1.h"
typedef struct st_Shit
{
  int  m1[A1];
  int  m2[A2];
  char m3[A3];
}t_Shit;
```

使用如下命令：
```c
gcc -E -P  -< header2.h > shit.h
header2.h中的宏展開之後，生成shit.h文件，內容如下：
typedef struct st_Shit
{
 int  m1[10 ];
 int  m2[0x0A ];
 char m3[(10 +1) ];
}t_Shit;
```

如果不使用-P開關，命令如下：
```c
gcc -E -< header2.h > shit.h
header2.h中的宏展開之後，生成shit.h文件，內容如下：
# 1 ""
# 1 "header1.h" 1
# 1 "" 2
typedef struct st_Shit
{
 int  m1[10 ];
 int  m2[0x0A ];
 char m3[(10 +1) ];
}t_Shit;
```

可見，`-P`選項能夠屏蔽掉這些垃圾內容