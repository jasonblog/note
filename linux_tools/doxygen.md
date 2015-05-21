# Doxygen


### 編譯少了 liconv
```
src/Makefile.doxygen加上 -liconv

LIBS    =   -L../lib -ldoxygen -ldoxycfg -lqtools -lmd5 -lpthread -liconv
```

### 畫 Structure
```
HAVE_DOT               = YES
UML_LOOK               = YES
RECURSIVE = YES
```

### Call Graph

```
RECURSIVE = YES
HAVE_DOT               = YES
EXTRACT_ALL            = YES
EXTRACT_PRIVATE        = YES
EXTRACT_STATIC         = YES
CALL_GRAPH             = YES
```
