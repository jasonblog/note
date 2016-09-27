
## linaro qemu 

```sh
git clone git://git.linaro.org/qemu/qemu-linaro.git

$ cd qemu-linaro
$ mkdir build
$ cd build
$ ../configure --prefix=/usr/local/qemu-linaro
$ make -j8
$ sudo make install
```

## ~/.bashrc

```sh
export PATH=/usr/local/qemu-linaro/bin:$PATH
```