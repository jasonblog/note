# Debug Tips


```sh
scj@scjComputer:~/THpro/func_trace$ tree .
.
├── func_trace.c
├── func_trace.sh
├── imple
│   ├── bar.c
│   └── bar.h
├── main.c
└── 步骤
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
- 



