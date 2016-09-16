# Vim


## 編譯vim

```sh
https://github.com/vim/vim.git
```

```sh
./configure  --prefix=/root/.mybin/vim \
--enable-multibyte \
--enable-fontset \
--enable-xim  \
--enable-gui=auto \
--enable-pythoninterp=dynamic \
--enable-pythoninterp \
--enable-python3interp=dynamic \
--enable-rubyinterp=dynamic \
--enable-rubyinterp \
--enable-perlinterp \
--enable-cscope \
--enable-sniff \
--with-x  \
--with-compiledby=erocpil  \
--with-features=huge \
--enable-luainterp=dynamic  \
--with-python-config-dir=/usr/lib/python2.7/config \
--with-python3-config-dir=/usr/lib/python3.2/config
```

```sh
make && make install
```