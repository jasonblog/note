# Python 原始碼 使用 gdb trace

- source code download

```sh
https://www.python.org/downloads/release/python-2713/
```

- build

```sh
./configure --with-pydebug
make -j8
```

- using gdb

```sh
cgdb ./python
start
```

```sh
(gdb) start
Temporary breakpoint 1 at 0x414cb5: file ./Modules/python.c, line 20.
Starting program: /home/shihyu/Python-2.7.13/python
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".

Temporary breakpoint 1, main (argc=1, argv=0x7fffffffdbe8) at ./Modules/python.c:20
```


