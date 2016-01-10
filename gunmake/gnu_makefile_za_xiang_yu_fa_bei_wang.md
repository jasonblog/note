# GNU Makefile 雜項語法備忘


一般的 tutorial 教得都差不多卻少了一些我想知道的語法, 以下是自己備忘用的語法, 對於讀別人的 Makefile 時有幫助

```c
$ cat Makefile
var ?= xxx         # assign var = xxx if var is not assigned
.PHONY: all        # tell make that "all" is not a file
all:               # first target is the default target
    @echo make-all # @ means do not display the cmd
    @echo all: x $(x)

all: b             # all depends on b

b:
    @echo make-b   # must use TAB to indent actions
    @echo b: var $(var)


all: a             # now all depends on a and b

a: x:= 3           # set x = 3 only in this context
a:
    @echo make-a
    @echo a: x $(x)

reverse = $(2) $(1)  # define a function
c:
    @echo c: x y
    @echo c: $(call reverse, x, y)  # use "call" to use
                                    # defined functions
```

範例輸出

```c
$ make
make-a
a: x 3
make-b
b: var xxx
make-all
all: x
$ var=ooo make b  # override var
make-b
b: var ooo
$ x=9 make  # set x = 9 globally
make-a
a: x 3   # note that x is still 3
make-b
b: var xxx
make-all
all: x 9
$ make c
c: x y
c: y x
```

注意 Makefile 自己有套變數, 想改變 g++ 的參數時, Makefile 必須寫成傳遞變數作為 g++ 參數, 比方說慣例上會用 CFLAGS, CXXFLAGS, LDFLAGS 之類的。若 Makefile 沒這麼定, 想改變 g++ 用的參數時, 就只能直接修改 Makefile 了。
