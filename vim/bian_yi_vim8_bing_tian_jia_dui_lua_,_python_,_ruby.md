# 編譯vim8並添加對lua,python,ruby的支持


##卸載原有vim

```sh
sudo apt-get remove --purge vim vim-runtime vim-gnome vim-tiny vim-gui-common
sudo rm -rf /usr/local/share/vim /usr/bin/vim
```

建議卸載原有 vim 版本


##下載並解壓源碼包


```sh
git clone https://github.com/vim/vim

https://github.com/vim/vim/releases

```

對vim8的開發還在火熱進行中，在我下載該源碼包後的9小時內又產生了3個新版本。


##安裝相關依賴

```sh
sudo apt-get install lua5.1 liblua5.1-dev \
                     luajit libluajit-5.1 \
                     python-dev python3-dev ruby-dev \
                     libperl-dev libncurses5-dev \
                     libatk1.0-dev libx11-dev \
                     libxpm-dev libxt-dev
```

## 編譯vim8

生成Makefile :

```sh
./configure \
--enable-multibyte \
--enable-perlinterp=dynamic \
--enable-rubyinterp=dynamic \
--with-ruby-command=/usr/bin/ruby \
--enable-pythoninterp=dynamic \
--with-python-config-dir=/usr/lib/python2.7/config-x86_64-linux-gnu \
--enable-python3interp \
--with-python3-config-dir=/usr/lib/python3.5/config-3.5m-x86_64-linux-gnu \
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
--enable-fail-if-missing
```

建 議：將上面的命令添加到目錄下的 configure 文件中，替換掉文件中原有的 ./configure ?@。


## 使用make進行編譯：

```sh
make
```

`注意`：無需添加--with-lua-prefix=/usr/include/lua5.1選項，只需存在--enable-luainterp選項即可為vim8添加lua支持。


編譯失敗的腳本

```sh
./configure --with-vim-name=vim \
--with-features=huge \
--enable-gui=auto \
--with-x \
--enable-rubyinterp \
--with-ruby-command=/usr/bin/ruby \
--enable-luainterp \
--with-lua-prefix=/usr/include/lua5.3 \
--enable-perlinterp \
--enable-pythoninterp  \
--with-python-config-dir=$(bash /usr/bin/python2.7-config --configdir)  \
--enable-python3interp \
--with-python3-config-dir=$(bash /usr/bin/python3.5-config --configdir) \
--enable-fontset --enable-gtk2-check \
--enable-gnome-check \
--enable-multibyte \
--with-compiledby="YourName"  \
--enable-fail-if-missing

# 遇到的幾個錯誤：
# 腳本中的 \ 後不能有空格，而因在\後直接換行
# bash: /usr/bin/python3.5-config: 沒有那個文件或目錄    原因是沒有安裝：python3.5-dev
# checking if lua.h can be found in /usr/include/lua5.3/include... no
# checking if lua.h can be found in /usr/include/lua5.3/include/lua... no
```

## 安裝vim8

###生成 deb 包

```sh
sudo checkinstall --install=no
# ... 對deb進行描述
```

###安裝

```sh
sudo dpkg -i vim-***.deb
```

### 查看安裝：

```sh
[fan 12:28:37]/tmp/vim-8.0.0768$ vim --version
VIM - Vi IMproved 8.0 (2016 Sep 12, compiled Jul 25 2017 12:21:19)
包含補丁: 1-768
編譯者 fan
巨型版本 無圖形界面。  可使用(+)與不可使用(-)的功能:
+acl             +file_in_path    +mouse_sgr       +tag_old_static
+arabic          +find_in_path    -mouse_sysmouse  -tag_any_white
+autocmd         +float           +mouse_urxvt     -tcl
-balloon_eval    +folding         +mouse_xterm     +termguicolors
-browse          -footer          +multi_byte      -terminal
++builtin_terms  +fork()          +multi_lang      +terminfo
+byte_offset     +gettext         -mzscheme        +termresponse
+channel         -hangul_input    -netbeans_intg   +textobjects
+cindent         +iconv           +num64           +timers
+clientserver    +insert_expand   +packages        +title
+clipboard       +job             +path_extra      -toolbar
+cmdline_compl   +jumplist        +perl/dyn        +user_commands
+cmdline_hist    +keymap          +persistent_undo +vertsplit
+cmdline_info    +lambda          +postscript      +virtualedit
+comments        +langmap         +printer         +visual
+conceal         +libcall         +profile         +visualextra
+cryptv          +linebreak       +python/dyn      +viminfo
+cscope          +lispindent      +python3/dyn     +vreplace
+cursorbind      +listcmds        +quickfix        +wildignore
+cursorshape     +localmap        +reltime         +wildmenu
+dialog_con      +lua             +rightleft       +windows
+diff            +menu            +ruby/dyn        +writebackup
+digraphs        +mksession       +scrollbind      +X11
-dnd             +modify_fname    +signs           +xfontset
-ebcdic          +mouse           +smartindent     -xim
+emacs_tags      -mouseshape      +startuptime     -xpm
+eval            +mouse_dec       +statusline      +xsmp_interact
+ex_extra        -mouse_gpm       -sun_workshop    +xterm_clipboard
+extra_search    -mouse_jsbterm   +syntax          -xterm_save
+farsi           +mouse_netterm   +tag_binary      
     系統 vimrc 文件: "$VIM/vimrc"
     用戶 vimrc 文件: "$HOME/.vimrc"
 第二用戶 vimrc 文件: "~/.vim/vimrc"
      用戶 exrc 文件: "$HOME/.exrc"
       defaults file: "$VIMRUNTIME/defaults.vim"
         $VIM 預設值: "/usr/local/share/vim"
```

##參考

Google搜索： Installing vim 8 with lua on Ubuntu 
Install Vim 8 with Python, Python 3, Ruby and Lua support on Ubuntu 16.04 
Install Vim 8 with Python, Python 3, Ruby (2.4) and Lua support on Ubuntu