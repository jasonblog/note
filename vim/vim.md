# Vim

## 編譯vim 8

### Requirement package
```sh
sudo apt-get install libacl1-dev libgpmg1-dev libgtk-3-dev libgtk2.0-dev \
liblua5.2-dev libperl-dev libselinux1-dev libtinfo-dev \
libxaw7-dev libxpm-dev libxt-dev lua5.2 python3-dev \
ruby ruby-dev tcl-dev
```

-  --with-python3-config-dir 須按照系統環境路徑設定

```sh
./configure --prefix=/home/shihyu/.mybin/vim \
--with-compiledby="OdTu" \
--with-features=huge \
--enable-python3interp=yes \
--with-python3-config-dir=/usr/lib/python3.5/config-3.5m-x86_64-linux-gnu \
--enable-cscope \
--enable-multibyte \
--enable-fail-if-missing



./configure \
--enable-perlinterp \
--enable-python3interp \
--enable-rubyinterp \
--enable-cscope \
--enable-gui=auto \
--enable-gtk2-check \
--enable-gnome-check \
--with-features=huge \
--enable-multibyte \
--with-x \
--with-compiledby="xorpd" \
--with-python3-config-dir=/usr/lib/python3.5/config-3.5m-x86_64-linux-gnu \
--prefix=/home/shihyu/.mybin/vim
```


## 編譯vim support GTK3

```sh
https://github.com/vim/vim.git
```

```sh
./configure \
--prefix=/home/shihyu/.mybin/vim \
--enable-multibyte \
--enable-perlinterp=dynamic \
--enable-rubyinterp=dynamic \
--with-ruby-command=/usr/bin/ruby \
--enable-pythoninterp=dynamic \
--with-python-config-dir=/usr/lib/python2.7/config-x86_64-linux-gnu \
--enable-python3interp \
--with-python3-config-dir=/usr/lib/python3.6/config-3.6m-x86_64-linux-gnu \
--enable-luainterp \
--with-lua-prefix=/usr/include/lua5.2
--with-luajit \
--enable-cscope \
--enable-gui=auto \
--with-features=huge \
--with-x \
--enable-fontset \
--enable-largefile \
--disable-netbeans \
--with-compiledby="Jason" \
--enable-fail-if-missing \
--enable-gui=gtk3
```

```sh
make && make install
```
