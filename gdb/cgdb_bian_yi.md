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

- cgdbrc

```sh
set arrowstyle=highlight
set autosourcereload
set shortcut
set tabstop=4
set winsplit=top_big
set showtgdbcommands
map <F2> :set<SPACE>winsplit=top_big<CR>
map <F3> :set<SPACE>winsplit=bottom_big<CR>
hi Statement cterm=bold ctermfg=6
hi PreProc cterm=bold ctermfg=4
hi IncSearch ctermfg=2
hi LineHighlight cterm=bold ctermfg=3 ctermbg=4
```
