# cgdb 編譯

```sh
git clone https://github.com/cgdb/cgdb
cd cgdb
./autogen.sh
./configure --prefix=/usr/local/cgdb
make -j8

sudo make install
```

- ~/.bashrc

```sh
export PATH=/usr/local/cgdb/bin:$PATH
```

你用的是 cross compiler tool chain 也沒關係，`加個參數 -d` 就可以用了。

```sh
cgdb -d arm-linux-androideabi-gdb
```