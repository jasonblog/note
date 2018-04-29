# Vim

## 編譯vim 8

### Requirement package
```sh
sudo apt-get install libncurses5-dev \
                     libgnome2-dev libgnomeui-dev libgtk2.0-dev \
                     libatk1.0-dev libbonoboui2-dev libcairo2-dev \
                     libx11-dev libxpm-dev libxt-dev python-dev \
                     python3-dev git
                     
                     

sudo apt-get install lua5.1 liblua5.1-dev \
                     luajit libluajit-5.1 \
                     python-dev python3-dev ruby-dev \
                     libperl-dev libncurses5-dev \
                     libatk1.0-dev libx11-dev \
                     libxpm-dev libxt-dev


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


## 編譯vim

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
--with-luajit \
--enable-cscope \
--enable-gui=auto \
--with-features=huge \
--with-x \
--enable-fontset \
--enable-largefile \
--disable-netbeans \
--with-compiledby="Jason" \
--enable-gtk3-check \
--enable-fail-if-missing
```

```sh
make && make install
```