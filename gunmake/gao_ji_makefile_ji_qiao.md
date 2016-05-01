# 高級Makefile技巧


##特殊的宏 Special Macros
如果你需要細粒度控制(fine-grained control)目標的行為，你就需要用到一些特殊的宏，他們的值基於指定的目標和依賴。所有的特殊宏都以美元符號$開始，並且不需要使用括弧。


### $@

`$@表示目標名`。通過它，不同的目標可以產生不同的輸出。例如，下面兩個目標產生的輸出文件將分別以client和server命名。

```sh
client: client.c
    $(CC) client.c -o $@

server: server.c
    $(CC) server.c -o $@
```

### $?

`$?儲存了一系列依賴文件`（通常是源文件——譯者注），這些依賴文件要比目標文件（編譯器生成的文件——譯者注）要新。通過使用這個宏，可以把上面例子中的行為變得更一般化：

```sh
client: client.c
    $(CC) $? -o $@
server: server.c
    $(CC) $? -o $@
```

### $@ and @? 應用

```sh
CC = gcc
CXX = g++
LDFLAGS = -lm
CFLAGS = -Wall -pedantic -g -O0 -std=c99
 
OBJS = main.o                       \
	   GyroscopeBiasEstimator.o     \
	   HeadTracker.o                \
	   HeadTransform.o              \
	   LowPassFilter.o              \
	   Matrix3x3d.o                 \
	   Matrix.o                     \
	   OrientationEKF.o             \
	   So3Util.o                    \
	   Vector3d.o
 
main: ${OBJS}
    #${CC} -o main ${OBJS} ${LDFLAGS} ${CFLAGS}
	${CC} -o $@ $? ${LDFLAGS} ${CFLAGS}
clean:
	rm -f main ${OBJS}
```

###$^
`$^指當前目標中的所有的依賴文件`，它並不關心這些文件是不是比目標文件新。然而，重複的依賴文件名會被移除。這會在你需要將所有的依賴文件輸出到屏幕時變得非常有用：

```sh
# print the source to the screen
viewsource: client.c server.c
    less $^
```

###$+
`$+與$^類似，但是它並不移除重複的名字`。所有依賴文件出現的順序也不改變：

```sh
# print the source to the screen
viewsource: client.c server.c
    less $+
```

### $<
當你只需要第一個依賴文件的時候，$<就變得很有用。當你的目標名稱後面只有一個依賴的時候，使用$<要比使用$^來的安全。可以想像，如果你今後加上了第二個依賴文件，使用$^可能帶來麻煩。當然，你也許就想要所有的依賴文件也說不定。所以，請務必事先仔細考慮清楚。

## 目標的通配符匹配 Wildcard Matching in Targets

百分號%可以用作通配符匹配以寫出更一般的目標。如果%出現在(makefile目標中的)依賴列表中，那麼它將由make命令行中的對應文本替換。如果你想使用目標本身的匹配文本，你需要用到特殊的$*宏。下面的例子假設你輸入make <name of .c>來用一個給定的名字建立一個可執行文件：

```sh
%.c:
    gcc -o $* $*.c
```

例如，輸入：

```sh
% make test_executable
```

將會執行：

```sh
gcc -o test_executable test_executable.c
```

## 宏修飾符 Macro Modification
使用宏是為了減少冗餘的文本，通過使用宏修飾符可以將宏從一種形態轉變為另一種形態。

## 替換文本 Replacing Text

從一個已有的宏創建一個新宏並非不可能。例如宏SRC代表一系列的源文件，你希望生成一個對應的目標文件宏OBJ。要這樣做，你只需要指定OBJ = SRC，除了擴展名不同以外：

```sh
OBJ = $(SRC:.c=.o)
```

注意，這條語句更通俗地因該說成「在宏SRC中，擴展名.c被替換成了.o」。


## References:

本文由 lesca 翻譯自cprogramming.com 的文章 Advanced Makefile Tricks
有關Makefile更多的教程，可以參考opus的The Makefile
您可以在CC許可證下自由轉載


