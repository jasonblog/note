# Makefile  中的 $@, $^, $< , $? 符号


Makefile  $@, $^, $< 

```sh
$@  表示目标文件
$^  表示所有的依赖文件
$<  表示第一个依赖文件
$?  表示比目标还要新的依赖文件列表
```

如一个目录下有如下文件：
```sh
$ ls
hello.c  hi.c  main.c  Makefile
```

按照 Makefile 规则规规矩矩的写：

```sh
main: main.o hello.o hi.o
        gcc -o main main.o hello.o hi.o

main.o: main.c
        cc -c main.c

hello.o: hello.c
        cc -c hello.c

hi.o: hi.c
        cc -c hi.c

clean:
        rm *.o
        rm main
```
改为用上述符号进行替代：

```sh
main: main.o hello.o hi.o
        gcc -o $@ $^
main.o: main.c
        cc -c $<
hello.o: hello.c
        cc -c $<
hi.o: hi.c
        cc -c $<
clean:
        rm *.o
        rm main
```

```sh
beyes@debian:~/makefile_test/semicolon/real$ make
cc -c main.c
cc -c hello.c
cc -c hi.c
gcc -o main main.o hello.o hi.o
```

```sh
beyes@debian:~/makefile_test/semicolon/real$ ls
hello.c  hello.o  hi.c  hi.o  main  main.c  main.o  Makefile
```