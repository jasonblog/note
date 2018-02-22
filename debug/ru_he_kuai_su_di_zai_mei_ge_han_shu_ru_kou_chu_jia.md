# 如何快速地在每個函數入口處加入相同的語句？


```sh
scj@scjComputer:~/THpro/func_trace$ tree .
.
├── func_trace.c
├── func_trace.sh
├── imple
│   ├── bar.c
│   └── bar.h
├── main.c
└── 步驟
```

- func_trace.c

```c
#include <stdio.h>

static FILE *fp_trace;

void __attribute__((constructor)) traceBegin(void) {
  fp_trace = fopen("func_trace.out", "w");
}

void __attribute__((destructor)) traceEnd(void) {
  if (fp_trace != NULL) {
    fclose(fp_trace);
  }
}

void __cyg_profile_func_enter(void *func, void *caller) {
  if (fp_trace != NULL) {
    fprintf(fp_trace, "entry %p %p\n", func, caller);
  }
}

void __cyg_profile_func_exit(void *func, void *caller) {
  if (fp_trace != NULL) {
    fprintf(fp_trace, "exit %p %p\n", func, caller);
  }
}
```

- func_trace.sh

```sh
#!/bin/bash
EXECUTABLE="$1"
TRACELOG="$2"

while read TRACEFLAG FADDR CADDR; do
FNAME="$(addr2line -f -e ${EXECUTABLE} ${FADDR}|head -1)"

if test "${TRACEFLAG}" = "entry"
then
CNAME="$(addr2line -f -e ${EXECUTABLE} ${CADDR}|head -1)"
CLINE="$(addr2line -s -e ${EXECUTABLE} ${CADDR})"
echo "Enter ${FNAME} called from ${CNAME} (${CLINE})"
fi

if test "${TRACEFLAG}" = "exit"
then
echo "Exit  ${FNAME}"
fi

done < "${TRACELOG}"
```
- bar.c

```c
#include "bar.h"

int bar(void) {
  zoo();
  return 1;
}

int foo(void) {  
  return 2; 
}

void zoo(void) { 
  foo(); 
}
```

- bar.h


```c
#ifndef bar_h
#define bar_h

int bar(void);
int foo(void);
void zoo(void);

#endif
```

- main.c

```c
#include <stdio.h>
#include "./imple/bar.h"

int main(int argc, char **argv) { 
    bar(); 
}
```


然後按照如下順序執行：

```sh
gcc func_trace.c -c
gcc main.c ./imple/*.c func_trace.o -finstrument-functions
./a.out
./func_trace.sh a.out func_trace.out
```
可以直接編譯好多源文件的程序了。
直接把main.c以外的文件放進imple文件夾裡頭就可以

如果第二步有問題，用

```sh
gcc main.c ./source/*.c func_trace.o -finstrument-functions 2>trace_log.txt
```

導出編譯信息


imple文件夾裡頭的東西封裝成so後反而看不到調用過程。