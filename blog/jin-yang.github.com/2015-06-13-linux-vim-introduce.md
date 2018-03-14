---
title: VIM 使用簡介
layout: post
comments: true
language: chinese
category: [misc]
keywords: vim,編輯器
description: Vim 是一個功能強大、高度可定製的文本編輯器，在 Vi 的基礎上改進和增加了很多特性。與其相匹敵的是 Emacs ，這兩個都是不錯的編輯器，在此不再比較兩者的優劣，僅介紹 Vim 相關的內容。
---

Vim 是一個功能強大、高度可定製的文本編輯器，在 Vi 的基礎上改進和增加了很多特性。與其相匹敵的是 Emacs ，這兩個都是不錯的編輯器，在此不再比較兩者的優劣，僅介紹 Vim 相關的內容。

<!-- more -->

![vim logo]({{ site.url }}/images/misc/vim_logo.png "vim logo"){: .pull-center width="45%" }

## 簡介

啟動 Vim ，默認會進入 Normal 模式；按下鍵 i 進入 Insert 模式；此時，可以輸入文本；按下 ESC 鍵，返回 Normal 模式。注意：在 Normal 模式下，所有的按鍵都是功能鍵。

通過 ```:help <command>``` 可以查看相關命令的幫助文件，或者直接通過 ```:help``` 查看幫助文件。

### 常用操作

首先是常見的插入、替換操作。

{% highlight text %}
===== 插入操作 :help inserting
a                   → 在當前光標字符後插入
A                   → 在當前行的末尾插入
i                   → 在當前光標字符前插入
I                   → 在當前行的第一個非空字符前插入
o                   → 在當前行後插入一個新行，也即打開新行
O                   → 在當前行前插入一個新行

===== 替換操作 :help replacing
s                   → substitute替換當前字符，刪除字符+進入插入模式
S                   → 替換一行，刪除行+進入插入模式
r                   → 替換一個字符，進入替換模式，自動回到normal模式
R                   → 可以替換多個連續字符，Esc退出Replace模式
c                   → change改變選中內容，通常使用Visual Mode，選中+c
cw                  → 替換從光標所在位置後到一個單詞結尾的字符；首先刪除，然後等待插入
{% endhighlight %}

### 源碼安裝

很多的一些插件依賴一些高版本的特性，所以需要手動編譯安裝，直接從 [www.vim.org](http://www.vim.org/) 或者 [github](https://github.com/vim/vim) 上下載相關的版本。

{% highlight text %}
# yum remove vim-common vim-enhanced vim-filesystem vim-minimal
$ git clone https://github.com/vim/vim.git
$ cd vim
$ ./configure -h
$ ./configure --with-features=huge                                      \
    --enable-pythoninterp                                               \
    --with-python-config-dir=/usr/lib/python2.7/config-x86_64-linux-gnu \
    --enable-multibyte                                                  \   多字節，支持UTF8
    --enable-rubyinterp                                                 \
    --enable-perlinterp                                                 \
    --enable-luainterp                                                  \
    --enable-cscope --prefix=/usr                                       \
    --disable-selinux                                                   \
    --enable-gui=auto --enable-xim  --with-x --enable-fontset               也可以使用gnome或者gtk2
$ make VIMRUNTIMEDIR=/usr/share/vim/vim80
# make install
# checkinstall
$ vim --version


{% endhighlight %}

### 其它

如果有多個配置，則可以通過 ```source ~/.vim/default.vim``` 加載。

#### mapleader

可以通過 ```let mapleader=','``` 設置命令的前綴，然後直接使用即可。

#### 啟動時間

可以通過 `vim --startuptime tmp.txt` 命令將啟動時間信息保存在 `tmp.txt` 文件中，然後通過 `sort -nrk 2` 排序即可。

另外，可以通過 `vim --noplugin` 取消插件加載。

#### 加載插件

通過 `:scriptnames` 命令查看發現沒有加載相應的插件。

## 基本配置

vim 含有多種變量，`$HOME` 表示環境變量；`&options` 表示選項；`@a` 表示寄存器；可以通過 `:help function-list` 查看所支持的函數列表；`has()` 用於判斷 vim 是否支持這一特性。

如果不知道 vim 配置文件的位置，可以通過 ```:version``` 查詢，不同的平臺其配置文件名、配置文件順序會有所區別；還可以通過 ```:help vimrc``` 來查看配置文件在所有平臺上的加載位置。

在 Windows 平臺下，通常為 `_vimrc` (也即 `$VIM/_vimrc` )，可以通過 `source` 指令包含其它的配置文件，如與 `_vimrc` 同目錄下 `source $VIM/myvim.vim` 。

在配置文件中，可以指定變量，而對應的變量可通過 ```:echo $VIM``` 查看。

### Tab 操作

{% highlight text %}
:help tabpage

:tabnew file :tabe file      → 新建或打開某一文件並開啟新標籤頁
:tab split                   → 用標籤頁打開當前編輯的文件
:tabf filename_re            → 基於正則表達式遞歸遍歷當前工作目錄查找名稱匹配的文件併為其建立新標籤頁

:tabs                        → 顯示已打開標籤頁的列表，&lt;指示當前頁，+顯示修改未保存
:tabc    :tabnew             → 關閉當前標籤頁等價與:q，新建
:tabn    :tabp               → 移動到下/上一個標籤頁
:tablast :tabfirst           → 移動到最後/第一個標籤頁
gt                           → 切換到下一個Tab
gT                           → 反向切換

:tabmove 0                   → 將當前tab移動到第一個位置，位置編號從0開始
:tabdo %s/aaa/bbb/g          → 在每個打開的Tab上執行操作

:tab help tabpage            → 使用Tab而非Windows打開幫助窗口
:help setting-guitablabel    → 自己配置tab標題
{% endhighlight %}

`:tabr` 跳轉第一個標籤頁 gvim 提供了 remote-tab 的功能。

在標籤欄中，各標籤頁的默認名稱是對應文件所在路徑全稱的簡寫，如 `/usr/share/doc/test.txt` 文件所對應的標籤頁名默認是 `/u/s/d/test.txt`，這樣的標籤頁名看上去有些詭異。可以在配置文件中添加如下內容，在標籤頁欄中去除當前所編輯文件的路徑信息，只保留文件名。

{% highlight text %}
function ShortTabLabel ()
    let bufnrlist = tabpagebuflist (v:lnum)
    let label = bufname (bufnrlist[tabpagewinnr (v:lnum) -1])
    let filename = fnamemodify (label, ':t')
    return filename
endfunction

set guitablabel=%{ShortTabLabel()}
{% endhighlight %}

### 緩衝區

{% highlight text %}
:help buffer|buffer-list
{% endhighlight %}

緩衝區是一塊內存區域，裡面存儲著正在編輯的文件，如果沒有把緩衝區裡的文件存盤，那麼原始文件不會被更改。

{% highlight text %}
:e <path/to/file>            → 打開一個文件，保存到緩衝區列表，e是edit縮寫
:badd <path/to/file>         → 添加到緩衝區列表中
:saveas <path/to/file>       → 另存為 &lt;path/to/file&gt;
:wq                          → 存盤並退出
:x                           → 表示僅在需要時保存
ZZ                           → 不需要輸入冒號並回車
:q!                          → 退出不保存
:qa!                         → 強行退出所有的正在編輯的文件，就算別的文件有更改

:[buffers|ls]                → 列出當前編輯中所有的緩衝區狀態
:buffer number|filename      → 切換緩衝區，可通過緩衝區的序號或者打開文件的名字選擇緩衝區
:sbuffer number|filename     → 水平分割窗口
:ball                        → 為每個緩衝區打開一個窗口，默認按順序水平切分
:bnext :bprevious            → 調轉到下/上一個緩衝區，簡寫為:bn/:bp，:n 調轉到下個文件
:blast :bfirst               → 調轉到最後/第一個緩衝區
:bd(elete) number|filename   → 刪除緩衝區
:3 bdelete :1,3 bdelete      → 刪除或者指定範圍的緩衝區
{% endhighlight %}

查看緩衝區時，在這個狀態列表中，前面的數字是緩衝區的數字標記，第二個標記就是緩衝區當前的狀態，緊接著是與緩衝區所關聯的文件名。有如下幾種狀態：

{% highlight text %}
    - （非活動的緩衝區）
    a （激活緩衝區）
    h （隱藏的緩衝區）
    % （當前的緩衝區）
    # （交換緩衝區）
    = （只讀緩衝區）
    + （已經更改的緩衝區）
{% endhighlight %}

在刪除緩衝區時，如果緩衝區被改動過，那麼該命令將失敗，除非使用 `!` 選項。如果使用了帶 `!` 選項的 `:bdelete! filename` 命令，那麼在緩衝區中的所有改動都會被放棄。

### 摺疊

{% highlight text %}
: help folding
: help usr_28

: set foldenable
{% endhighlight %}

當進入插入模式，將會自動打開摺疊，也可通過 ```:nnoremap <space> za``` 定義通過空格展開/關閉。

主要有如下幾種方式，當進入非 manual 時，所有的摺疊將會刪除重建，反之則不會。

#### Marker Fold 標記摺疊

{% highlight text %}
:set foldmethod=marker
vim: foldmarker={,} foldlevel=0 foldmethod=marker :
{% endhighlight %}

可以精確地定義摺疊，摺疊的標記通過 ```foldmaker``` 定義，默認以 \{\{\{ 開始，以 \}\}\} 結束，同時在標記後面可以添加數字表示摺疊的層級，通過 ```foldlevel``` 設置級別，超過默認值則會摺疊。


<!--
#### Manual Fold 手動摺疊

{% highlight text %}
:set foldmethod=manual
{% endhighlight %}

在可視化模式下，通過命令 zf ，將摺疊選中的文本；通過組合使用移動命令摺疊指定行，如，zf70j(摺疊光標之後的70行)、5zF(當前行及隨後 4 行摺疊)；zf7G(當前行至全文第 7 行摺疊)、zfa[(摺疊括號(如()、[]、{}、><等)包圍的區域)。<br><br>

vim 不會自動記憶手工摺疊，可以通過命令 :mkview 來保存當前的摺疊狀態；下次打開文檔時，使用命令 :loadview 來載入記憶的摺疊信息；使用命令 :help fold-manual 查看關於手工摺疊的幫助信息。

#### Indent Fold 縮進摺疊

{% highlight text %}
:set foldmethod=indent
{% endhighlight %}

，相同縮進距離的行構成摺疊。<br>

對於已經格式化好的語言，如 Python 比較有用。此時所有文本將按照（選項shiftwidth定義的）縮進層次自動摺疊，使用zr打開，zm關閉。zm和zr實際控制的是 set foldlevel=N，zr增加，zm減小，0時關閉所有摺疊，1時打開一層摺疊。可以設置foldignore='#'來忽略對一些行的摺疊，僅用於該選項。可以使用以下命令，查看關於縮進摺疊的幫助信息：:help fold-indent</li><br><li>

#### Syntax Fold 語法摺疊

:set foldmethod=syntax


所有文本將按照語法結構自動摺疊。可以使用以下命令，查看關於語法摺疊的幫助信息：:help fold-syntax</li><br><li>

Expression Fold, :set foldmethod=expr，表達式摺疊<br>
通過 'foldexpr' 給出每行的摺疊級別。如<br>
set foldexpr=strlen(substitute(substitute(getline(v:lnum),'\\s','',\"g\"),'[^>].*','',''))<br>
上面是對>開頭（如郵件）的摺疊，其中 getline(v:lnum)：獲得當前行；substitute(...,'\\s','','g')：刪除所有的空白字符；substitute(...,'[^>].*','','')：刪除>之後的所有內容；strlen(...)：計算長度，也就是>的個數。


zfap 創建一個摺疊。當我們在摺疊處左右移動或者跳轉(如0)時，摺疊會自動打開； set foldopen=all 使光標在當前時自動打開，注意此時光標不能移動到關閉的摺疊，因此經常臨時使用，可以使用 set foldopen& 恢復默認；使用 set foldclose=all 當光標移動之後自動關閉。<br><br>

注意摺疊只對本窗口有效，因此可以對同一個緩衝區打開兩個窗口，一個帶有摺疊，另一個沒有。<br><br>

當關閉時摺疊不會保存，因此應該在關閉之前使用 mkview 此時會保存包括 folder 在內的影響 view 的所有內容；再次打開時使用 loadview 加載。最多可以保存10個，可以使用 mkview 3 或 loadview 2 。
-->

#### 常用命令

{% highlight text %}
zo  打開當前的摺疊，O-pen a fold.
zO  打開當前所有的摺疊，O-pen all fold.
zr  打開所有摺疊，R-educe the folding.
zR  打開所有摺疊及其嵌套的摺疊

zc  關閉當前打開的摺疊，C-lose a fold.
zC  關閉當前所有打開的摺疊，C-lose all fold.
zm  關閉所有摺疊，folds M-ore.
zM  關閉所有摺疊及其嵌套的摺疊

za  關閉、打開相互切換

zd  刪除當前摺疊，對於標記則會自動刪除，D-elete a fold
zD  刪除當前所有摺疊，D-elete all fold

zj  移動至下一個摺疊
zk  移動至上一個摺疊

zn  禁用摺疊
zN  啟用摺疊
zi  在上述兩者之間切換
{% endhighlight %}

<!--
zf  關閉所有摺疊，F-old creation.
zE  刪除所有摺疊
-->


### 高亮顯示

{% highlight text %}
:help syntax
:help usr_44.txt
{% endhighlight %}

Vim 會根據 ```$VIMRUNTIME/syntax/language.vim``` 中的配置內容，自動識別關鍵字、字符串以及其他語法元素，並以不同的顏色顯示出來。

{% highlight text %}
:syntax [enable|clear]          → 啟用|關閉語法高亮度，只在當前文件中有效
:syntax [off|on]                → 同上，會對所有緩衝區中的文件立刻生效
:set filetype=c                 → 如果VIM無法識別，則設置文件類型
{% endhighlight %}

高亮顯示主要通過兩步來實現：A) 首先，確定需要格式化的字符；B) 然後，根據配色方案決定如何顯示這些字符。

其中經典的是 [solarized](http://ethanschoonover.com/solarized) ，一個配色方案在 github 就有 4K+ 的 Star ，可通過如下方式配置：

{% highlight text %}
syntax enable
if has('gui_running')
    set background=light
else
    set background=dark
endif
colorscheme solarized
{% endhighlight %}

還有一種很受歡迎的配色方案 [Molokai](https://github.com/tomasr/molokai)，它是 Mac 上 TextMate 編輯器的一種經典配色。

<!--
如可使用以下命令，將所有 FIX 和 ENDFIX 關鍵字顯示為特定顏色。

:syntax match cscFix "FIX\|ENDFIX"              // 創建名為 cscFix 的匹配模式
:highlight cscFix ctermfg=cyan guifg=#00FFFF    // 青色顯示匹配的文本

註釋的開頭到結尾，可用於識別代碼註釋。

:syntax region myComments start=/\/\*/ end=/\*\//
:syntax keyword myToDo FIXME TODO
:syntax region myComments start=/\/\*/ end=/\*\// contains=myToDo

https://github.com/altercation/vim-colors-solarized
-->

### 自動縮進和對齊

可以在配置文件中添加如下內容。

{% highlight text %}
set autoindent         " 設置自動縮進
set smartindent        " 對autoindent進行了一些改進

set shiftwidth=4       " 自動縮進所使用的空白長度
set tabstop=4          " 定義tab所等同的空格長度
set softtabstop=4      " 詳見如下的解釋

set expandtab          " 將TAB自動替換為空格

set listchars=tab:▸\ ,trail:-,extends:>,precedes:<,eol:¬    " 設置不可見字符的顯示方式
set nolist             " 不顯示TAB、空格、回車等不可見字符

filetype indent on     " 可以通過如下的設置，根據文件類型自動進行設置
autocmd FileType python setlocal expandtab smarttab shiftwidth=4 softtabstop=4
{% endhighlight %}

#### autoindent/smartindent/cindent

當在 ```insert``` 狀態用回車新增一個新行，或者在 ```normal``` 狀態用 ```o/O``` 插入一個新行時，會根據配置的不同模式，進行不同的縮進。

在 ```autoindent``` 模式中，會自動將當前行的縮進拷貝到新行，也就是 "自動對齊"，當然了，如果你在新行沒有輸入任何字符，那麼這個縮進將自動刪除。

```smartindent``` 對 ```autoindent``` 進行了一些改進，可以識別一些基本的語法。

```cindent``` 會按照 C 語言的語法，自動地調整縮進的長度，可以與上述配置共存。比如，當輸入了半條語句然後回車時，縮進會自動增加一個 ```TABSTOP``` 值，當你鍵入了一個右花括號時，會自動減少一個 ```TABSTOP``` 值。

另外，可以通過 ```indentexpr``` 設置不同的模式，在此不詳述，詳見 [vim reference manual](http://man.chinaunix.net/newsoft/vi/doc/indent.html) 。

#### 縮進寬度設置

如上配置的第二部分，就是所使用的縮進模式，如下主要介紹下 ```softtabstop``` 的含義。

當 ```shiftwidth/tabstop``` 不同時，會導致程序對齊很難看，這時可以使用 ```softtabstop```；此時，當按下 ```TAB``` 鍵，插入的是空格和 ```TAB``` 製表符的混合，具體如何混合取決於你設定的 ```softtabstop``` 。

舉個例子，如果設定 ```softtabstop=4``` ，那麼按下 ```tab``` 鍵，插入的就是正常的一個製表符；如果設定 ```softtabstop=8``` ，那麼插入的就是兩個製表符；如果 ```softtabstop=10``` ，那麼插入的就是一個製表符加上 2 個空格；如果 ```softtabstop=2``` 呢？

開始插入的就是 2 個空格，此時一旦你再按下一次 tab ，這次的 2 個空格就會和上次的 2 個空格組合起來變成一個製表符。換句話說，```softtabstop``` 是 “逢 4 空格進 1 製表符” ，前提是 ```tabstop=4``` 。

#### TAB 和空格替換

```:set expandtab/noexpandtab``` 使用空格替換TAB/不進行替換。對於已保存的文件，可以使用下面的方法進行空格和TAB的替換。

{% highlight text %}
----- TAB替換為空格
:set ts=4
:set expandtab
:%retab!

----- 空格替換為TAB
:set ts=4
:set noexpandtab
:%retab!
{% endhighlight %}

加 ```!``` 用於處理非空白字符之後的 ```TAB```，即所有的 ```TAB```，若不加 ```!```，則只處理行首的 ```TAB``` 。

假設配置文件中使用了 ```set expandtab```，如果想要輸入 ```TAB```，Linux 下使用 ```Ctrl-V + TAB```，Win 下使用 ```Ctrl-Q + TAB``` 。

#### 設置對齊方式

通過如下方式設置 C 語言的縮進方式，具體配置可查看 [Vim documentation: indent](http://vimdoc.sourceforge.net/htmldoc/indent.html) 。

{% highlight text %}
set cinoptions={0,1s,t0,n-2,p2s,(03s,=.5s,>;1s,=1s,:1s
{% endhighlight %}

#### 常用命令

{% highlight text %}
----- 執行縮進，前面可以加數字進行多節縮進
>>               # Normal模式下，增加當前行的縮進
<<               # Normal模式下，減少當前行的縮進
CTRL+SHIFT+T     # Insert模式下，增加當前行縮進
CTRL+SHIFT+D     # Insert模式下，減小當前行縮進
=                # Visual模式下，對選中的部分進行自動縮進

: set list       # 查看不可見字符，包括TAB、空格、回車等
{% endhighlight %}

<!-- ]p可以實現p的粘貼功能，並自動縮進。 -->

### 文件類型檢測

Vim 會根據不同的文件類型，分別設置高亮、縮進等功能。可以通過 ```:filetype``` 查看 Vim 的文件類型檢測功能是否已打開，通常有三個選項 ```detection:ON plugin:ON indent:ON``` 。

* detection，是否自動檢測文件類型。
* plugin，是否自動加載該類型相關的插件。
* indent，根據文件類型定義縮進方式，通常在安裝目錄的 indent 目錄下定義縮進相關腳本。

當配置文件中添加了 ```filetype on``` 命令時，實際上會調用 ```$VIMRUNTIME/filetype.vim``` 腳本檢測文件的類型。

{% highlight text %}
:filetype [detection|plugin|indent] on  → 設置開關
:filetype plugin indent on              → 同上，也可以在一行中設置
:filetype [off|on]                      → 關閉/打開
:set filetype                           → 查看當前文件類型
:set filetype=python                    → 設置類型
{% endhighlight %}

在設置文件類型時，會同時觸發 `FileType` 自動命令；從而，可以用下面的命令實現，根據不同文件類型設置不同的配置項。

{% highlight text %}
autocmd FileType c,cpp set shiftwidth=4 | set expandtab
{% endhighlight %}

## 高級進階

### 區域選擇

{% highlight text %}
:help object-motions
:help text-objects
{% endhighlight %}

`Text Objects Commands` 或許這個是最強大的命令了。

{% highlight text %}
<number><command><text object or motion>
  <number>     操作的重複次數；
  <command>    命令，如chang(c), delete(d), yank(y)，默認是選擇，如果不指定則只移動；
  <text object or motion>
               可以是指定文本，例如word、sentence、paragraph；或者是動作，如前進一行、行尾等。

Old text                  Command     New text
the t*ext object          daw         the object     刪除Word，同時刪除空格
the t*ext object          diw         the  object    刪除Word，不刪除空格
the t*ext object          dw          the tobject    刪除Word，從開始位置刪除，含空格
<h2>Sa*mple</h2>          dit         <h2></h2>      刪除tag之間的內容
<div *id="cont"></div>    di>         <></div>       刪除單個tag的內容
{% endhighlight %}

除了上面的 `Word` 外，還可以通過如下方式指定範圍。

{% highlight text %}
aw(A Word)        iw(Inner Word)        前者包含了區分子的空格，後者不包含；
as(A Sentence)    is(Inner Sentence)    通過.分割，作為一個句子；
as(A Block)       is(Inner Block)       也就是括號
ap(A Paragraph)   ip(Inner Paragraph)   通過空白行分割；
at(A Tag Block)   it(Inner Tag Block)   使用tag，例如HTML中的tag
a>(A Single Tag)  i>(Inner Single Tag)  在單個的tag內
{% endhighlight %}

除了上述的定義外，還可以通過 `a)` `i)` `a]` `i]` `a}` `i}` `a'` `i'` `a"` `i"` 還有反問號 ( \` ) ，這類操作與上述的 `Word` 操作相同。另外，`%` 可以在括號之間切換，其中 `c%` 和 `ca)` 等價。

需要注意 `Motion Commands` 和 `Text Objects Commands` 的區別，前者如 `cw` 是從當前的位置操作，而像 `ciw` 則處理的是光標所在的整個 `Word` 。

<!--
在 visual 模式下，這些命令很強大，其命令格式為 ```<action>a<object>``` 和 ```<action>i<object>``` ，其中 a 含有空格，i 不含。在 Normal 模式下按 'v' 進入自由選擇，'V' 行選擇，Ctrl-v 塊選擇。

{% highlight text %}
Old text                  Command     New text ~
 "Hello *world!"           ds"         Hello world!
 [123+4*56]/2              cs])        (123+456)/2
 "Look ma, I'm *HTML!"     cs"<q>      <q>Look ma, I'm HTML!</q>
 if *x>3 {                 ysW(        if ( x>3 ) {
 my $str = *whee!;         vlllls'     my $str = 'whee!';
 <div>Yo!*</div>           dst         Yo!
 <div>Yo!*</div>           cst<p>      <p>Yo!</p>
{% endhighlight %}
-->

常用操作。


{% highlight text %}
ci[ ci( ci< ci{      刪除一對 [], (), <>, 或{} 中的所有字符並進入插入模式
ci" ci' ci`          刪除一對引號字符 " ' 或 ` 中所有字符並進入插入模式
cit                  刪除一對 HTML/XML 的標籤內部的所有字符並進入插入模式

ci                   如 ci(，或者 ci)，將會修改 () 之間的文本；
di                   刪除配對符號之間文本；
yi                   複製；
ca                   同ci，但修改內容包括配對符號本身；
da                   同di，但剪切內容包括配對符號本身；
ya                   同yi，但複製內容包括配對符號本身。
{% endhighlight %}

另外，`dib` 等同於 `di(`，`diB` 等同於 `di{` 。

### 宏錄製

通過 `q` 進入宏錄製模式，操作為 A) 錄製，`qa do_something q`；B) 使用 `@a`, `@@`。`qa` 會把操作記錄在寄存器 `a`；`@a` 會重做被錄製的宏，就好象自己在輸入一樣；`@@` 是一個快捷鍵，用來執行最近被執行的宏。

如，在一個只有一行且這一行只有 `"1"` 的文本中，鍵入如下命令:

{% highlight text %}
qaYp<C-a>     → qa開始錄製；Yp複製行；<C-a>增加1；q停止錄製
@a            → 在1下面寫下2
@@            → 在2正面寫下3
100@@         → 會創建新的100行，並把數據增加到103
{% endhighlight %}

### 塊操作

在 Normal 模式下，通過 `Ctrl-v` 執行操作，如在指定的行首添加 `"--"` 字符串，其執行命令順序為 `0 <C-v> <C-d> I-- [ESC]`，解釋如下：

{% highlight text %}
0             → 到行頭
<C-v>         → 開始塊操作
<C-d>         → 向下移動，也可以使用hjkl來移動光標
I-- [ESC]     → I是插入，插入"--"，按ESC鍵來為每一行生效
{% endhighlight %}

行後添加字符的執行順序如下。

{% highlight text %}
<C-v>       → 視圖模式
<C-d>       → 選中相關的行，也可以使用j，或是/pattern或是%等
$           → 到行最後
A           → 輸入字符串，按ESC
{% endhighlight %}

在 Windows 下的 vim ，需要使用 `<C-q>` 而不是 `<C-v>`，`<C-v>` 是拷貝剪貼板。

### 可視化選擇

常見的操作包括了 `v`、`V`、`<C-v>` 等視圖模式下，在 Windows 下應該是 `<C-q>`，選好之後，可以做下面的事。

{% highlight text %}
J           → 把所有的行連接起來，變成一行
< 或 >      → 左右縮進
=           → 自動縮進
{% endhighlight %}

### 其它

#### 重複執行命令

{% highlight text %}
.                     → 重複上一次的命令
<NUM><command>        → 重複某個命令N次
2dd                   → 刪除2行
100idesu [ESC]        → 會寫下100個"desu "
{% endhighlight %}

#### 模式行

可以通過 `:help modeline` 查看幫助，在文件首、尾的若干行 (modelines默認5行) 添加的配置內容，可以用來指明這個文件的類型，以及一些其它的相關配置項。

{% highlight text %}
# vim: filetype=python
/* vim: filetype=java */
# vim: foldmarker={,} foldlevel=0 foldmethod=marker :
{% endhighlight %}

#### 其它

{% highlight text %}
:set cc=80         → 高亮顯示第80列
: shell            → 切換到shell執行命令，退出後返回到VIM
: messages         → 出現錯誤時可以通過該命令查看錯誤信息
{% endhighlight %}

## 常用插件

<!--
導航與搜索，taglist 和 tagbar 類似，不過其關注點有所區別，後者比較適合面向對象
  1. NERDTree - file navigation
  2. CtrlP    - fast file finder
  3. Taglist  - source code browser
  4. Tagbar   - tag generation and navigation

自動補全
  1. YouCompleteMe - visual assist for vim
  2. UltiSnips     - ultimate snippets
  3. Zen Coding    - hi-speed coding for html/css

語法
  1. Syntastic   - integrated syntax checking
  2. Python-mode - Python in VIM

其它
  1. Tabularize    - align everything
  2. Easymotion    - jump anywhere
  3. NERDCommenter - comment++
  4. Surround      - managing all the "'[{}]'" etc
  5. Gundo         - time machine
  6. Sessionman    - session manager
  7. Powerline     - ultimate statusline utility
  8. vim-powerline/vim-airline
-->

通常可以使用 vundle 管理所有的插件，通常插件為了防止多次加載，會在開始的時候檢測是否已經加載。

一些經典的配置可以參考 [vim.spf13.com](http://vim.spf13.com/)，該 vim 配置，在 [github](https://github.com/spf13/spf13-vim) 中有 1W+ 的 Star，可以通過如下方式進行配置：

{% highlight text %}
----- 直接下載安裝，實際上就是github中的bootstrap.sh腳本
$ curl https://j.mp/spf13-vim3 -L -o - | sh
{% endhighlight %}



### Vundle

<!-- 可以使用 vim plug -->

vim 缺少默認的插件管理器，所有插件的文件都散佈在 ```~/.vim``` 下的幾個文件夾中，無論是配置、更新、刪除，都需要手動配置，很容易出錯。

vundle 把 git 操作整合進去，用戶需要做的只是去 Github 上找到自己想要的插件的名字，安裝，更新和卸載都可有 vundle 來完成了，可以查看 [github](https://github.com/gmarik/vundle)，可以通過如下方式安裝。

{% highlight text %}
$ git clone https://github.com/gmarik/vundle.git ~/.vim/bundle/vundle
{% endhighlight %}

vim-scripts 在配置時，可以直接打倉庫名，相關的插件可以從 [官網](http://vim-scripts.org/index.html) 查看，或查看 [Github](https://github.com/vim-scripts)，此時可以直接使用 ```Bundle 'L9'``` 。在配置文件中，主要是配置安裝哪些插件，Vundle 支持如下的格式：

{% highlight text %}
Bundle 'gmarik/vundle'          " 使用Vundle來管理Vundle，這個必須要有
Bundle 'tpope/vim-fugitive'     " Github上其他用戶的倉庫，非vim-scripts賬戶裡的倉庫
Bundle 'file:///path/to/plugin' " 使用自己的插件
Bundle 'git://vim-latex.git.sourceforge.net/gitroot/vim-latex/vim-latex'
{% endhighlight %}

自己寫的插件也可以通過如上方式管理，如自己寫了一個 test 插件，並放到了 ```.vim/myplugin``` 目錄中，那麼可以在 vim 的配置文件中加入下面的命令 ```set rtp+=~/.vim/myplugin/``` 。

Vundle 常用的命令如下。

{% highlight text %}
BundleList             列出所有已配置的插件
BundleInstall(!)       下載(更新)插件
BundleUpdate           更新插件
BundleClean            清除不再使用的插件
BundleSearch(!) foo    查找(先刷新cache)foo
{% endhighlight %}

對於 Vundle 插件，如果使用 `call vundle#begin()` 時發現很多插件無法使用，可以使用 `call vundle#rc()` ，暫時不確認為什麼。

### Tagbar

源碼可以從 [github tagbar](http://majutsushi.github.io/tagbar/) 中查看，幫助文檔可以查看 [doc/tagbar.txt](https://github.com/majutsushi/tagbar/blob/master/doc/tagbar.txt) 。

### Tabular

一個不錯的對齊用的插件，可以從 [tabular](http://www.vim.org/scripts/script.php?script_id=3464) 下載，然後通過如下的方式安裝，詳細的資料可以參考 [aligning text with tabular vim](http://vimcasts.org/episodes/aligning-text-with-tabular-vim/) 。

{% highlight text %}
$ git clone https://github.com/godlygeek/tabular.git
$ cd tabular/
$ mv after autoload doc plugin ~/.vim/
{% endhighlight %}

首先，通過 V v Ctrl-v 選取需要對其的內容，然後直接輸入如下的命令即可。注意，在 Visual 模式中輸入 ```:``` 後，會顯示 ```:'<,'>``` 直接輸入命令即可。

{% highlight text %}
:Tabularize /=           使用=進行分割
:Tabularize /:\zs        ':'符號不動，只對其':'後面的字符
{% endhighlight %}

其中 Tabularize 可以簡寫為 Tab ；另外，tabular 可以自動識別，尤其是含有 {}()[] 的，因此可以不選擇字符，也就是在某行中直接輸入 ```:Tabularize /|``` 即可。

可以通過如下的示例進行測試。

{% highlight text %}
|start|eat|left|
|12|5|7|
|20|5|15|

var video = {
    metadata: {
        title: "Aligning assignments"
        h264Src: "/media/alignment.mov",
        oggSrc: "/media/alignment.ogv"
        posterSrc: "/media/alignment.png"
        duration: 320,
    }
}
{% endhighlight %}

對於第二部分，執行 ```vi}``` 選擇區域，執行 ```:Tab /:``` ，執行 ```:Tab /:\zs``` 則會使 : 不變。

{% highlight text %}
$ vim ~/.vimrc                              # 在最後添加如下的內容
let mapleader=','
if exists(":Tabularize")
  nmap <Leader>a= :Tabularize /=<CR>
  vmap <Leader>a= :Tabularize /=<CR>
  nmap <Leader>a: :Tabularize /:\zs<CR>
  vmap <Leader>a: :Tabularize /:\zs<CR>
endif

inoremap <silent> <Bar>   <Bar><Esc>:call <SID>align()<CR>a
function! s:align()
  let p = '^\s*|\s.*\s|\s*$'
  if exists(':Tabularize')&&getline('.') =~# '^\s*|'&&(getline(line('.')-1) =~# p || getline(line('.')+1) =~# p)
    let column = strlen(substitute(getline('.')[0:col('.')],'[^|]','','g'))
    let position = strlen(matchstr(getline('.')[0:col('.')],'.*|\s*\zs.*'))
    Tabularize/|/l1
    normal! 0
    call search(repeat('[^|]*|',column).'\s\{-\}'.repeat('.',position),'ce',line('.'))
  endif
endfunction
{% endhighlight %}

上述前一部分表示綁定的快捷鍵<!--，if語句使用時出錯?????????????-->；後一部分表示在輸入時會自動檢測，並設置格式。

### CtrlP

![vim CtrlP]({{ site.url }}/images/misc/vim-ctrl-p-buttons.png "vim CtrlP"){: .pull-center}

一個強大的搜索插件，可以模糊查詢定位，包括了工程下的所有文件、打開的 Buffer、Buffer 內的 tag、最近訪問的文件等，極大了方便了大規模工程代碼的瀏覽。

<!-- 據說還有一個 CopyCat ，暫時沒有找到怎麼使用 -->

### UltiSnips

一個和牛擺的模版，在寫代碼時經常需要在文件開頭加一個版權聲明之類的註釋，又或者在頭文件中要需要 `#ifndef... #def... #endif` 這樣的宏，亦或寫一個 `for` `switch` 等很固定的代碼片段。

該工具和 YouCompleteMe 以及 neocomplete 都很好的整合在一起了，不過需要編寫模版，很多模版可以參考 [honza/vim-snippets](https://github.com/honza/vim-snippets)。

<!-- http://vimcasts.org/episodes/meet-ultisnips/ -->

### YouCompleteMe

當通過 `Plugin 'Valloric/YouCompleteMe'` 安裝 YCM 後，經常會出現 `no module named future` 的報錯，可以通過如下方式進行安裝。

{% highlight text %}
----- 可以通過Vundle安裝，或使用如下方式下載，後者用於下載相關的依賴
$ git clone --recursive https://github.com/Valloric/YouCompleteMe.git ~/.vim/bundle/YouCompleteMe
$ git submodule update --init --recursive

----- 通過該腳本安裝，支持C，如果要支持所有語言通過--all安裝
$ ./install.py --clang-completer
{% endhighlight %}

在安裝時，可以通過 `--system-clang` 指定使用系統的 Clang 。 <!-- --system-libclang -->

另外，UltiSnips 與 YCM 有按鍵衝突，很多都建議將 UltiSnips 的自動填充快捷鍵更換，不過仍無效，可以通過如下方式修改，此時 `<C-N>` 和 `<C-P>` 仍然有效。

{% highlight text %}
let g:ycm_key_list_select_completion=[]
let g:ycm_key_list_previous_completion=[]
{% endhighlight %}

常用命令。

{% highlight text %}
:YcmDiags          通過location-list顯示診斷信息
{% endhighlight %}

<!--
在 Centos7 中可以通過 `yum install clang` 安裝，不過依賴 epel 鏡像庫
使用 vundle 下載源碼，在 vimrc 文件中加入 Bundle 'Valloric/YouCompleteMe'，並通過如下命令安裝。
-->

<!--
    " 自動補全配置
    autocmd InsertLeave * if pumvisible() == 0|pclose|endif "離開插入模式後自動關閉預覽窗口
    inoremap <expr> <CR>       pumvisible() ? "\<C-y>" : "\<CR>"    "回車即選中當前項
    "上下左右鍵的行為 會顯示其他信息
    inoremap <expr> <Down>     pumvisible() ? "\<C-n>" : "\<Down>"
    inoremap <expr> <Up>       pumvisible() ? "\<C-p>" : "\<Up>"
    inoremap <expr> <PageDown> pumvisible() ? "\<PageDown>\<C-p>\<C-n>" : "\<PageDown>"
    inoremap <expr> <PageUp>   pumvisible() ? "\<PageUp>\<C-p>\<C-n>" : "\<PageUp>"

    "youcompleteme  默認tab  s-tab 和自動補全衝突
    "let g:ycm_key_list_select_completion=['<c-n>']
    let g:ycm_key_list_select_completion = ['<Down>']
    "let g:ycm_key_list_previous_completion=['<c-p>']
    let g:ycm_key_list_previous_completion = ['<Up>']
    let g:ycm_confirm_extra_conf=0 "關閉加載.ycm_extra_conf.py提示

    let g:ycm_collect_identifiers_from_tags_files=1 " 開啟 YCM 基於標籤引擎
    let g:ycm_min_num_of_chars_for_completion=2 " 從第2個鍵入字符就開始羅列匹配項
    let g:ycm_cache_omnifunc=0  " 禁止緩存匹配項,每次都重新生成匹配項
    let g:ycm_seed_identifiers_with_syntax=1    " 語法關鍵字補全
    "nnoremap <leader>lo :lopen<CR> "open locationlist
    "nnoremap <leader>lc :lclose<CR>    "close locationlist
    inoremap <leader><leader> <C-x><C-o>
    let g:ycm_collect_identifiers_from_comments_and_strings = 0

    nnoremap <leader>jd :YcmCompleter GoToDefinitionElseDeclaration<CR> " 跳轉到定義處-->

#### 配置文件

YCM 中需要在 flags 中添加編譯時的參數定義，例如 `-Wall`、`-Wextra` 等，然後通過 `-isystem` 指定系統的頭文件，通過 `-I` 指定自定義的頭文件。

注意，在使用相對路徑時是 YCM 的安裝路徑。

#### YCM錯誤日誌及調試信息

在 vim 配置文件中加上下面的選項，打開調試信息。

{% highlight text %}
let g:ycm_server_keep_logfiles = 1
let g:ycm_server_log_level = 'debug'
{% endhighlight %}

幾個常用命令:

{% highlight text %}
:YcmToggleLogs   查看到錯誤日誌及調試信息，輸出到哪些文件中
:YcmDebugInfo    可以查看ycm相關的信息,包括編譯標誌,版本等等
:YcmDiags        查看當前文件中的錯誤信息
{% endhighlight %}

然後調試時就可以通過 print 打印，然後輸出到 stdout 指定的文件中。

#### 常見問題

對於 `Your C++ compiler does NOT support C++11` 錯誤，是由於要求最低 `gcc 4.9` 版本，該功能會在 `third_party/ycmd/cpp/CMakeLists.txt` 文件中進行檢查，也就是 `set( CPP11_AVAILABLE false )` 定義。

<!--
配置文件參考示例
https://github.com/rasendubi/dotfiles/blob/master/.vim/.ycm_extra_conf.py
https://github.com/robturtle/newycm_extra_conf.py/blob/master/ycm.cpp.py
-->

### Syntastic

一個語法檢查工具，支持多種語言，提供了基本的補全功能、自動提示錯誤的功能外，還提供了 tags 的功能；採用 C/S 模式，當 vim 關閉時，ycmd 會自動關閉。

不過對於不同的語言需要安裝相應的插件，詳細內容可以查看 [doc/syntastic-checkers.txt](https://raw.githubusercontent.com/vim-syntastic/syntastic/master/doc/syntastic-checkers.txt)，安裝方法可以參考 README.md 文件。

![vim Syntastic]({{ site.url }}/images/misc/vim-syntastic-screenshot.png "vim Syntastic"){: .pull-center width='90%' }

能夠實時的進行語法和編碼風格的檢查，還集成了靜態檢查工具，支持近百種編程語言，像是一個集大成的實時編譯器，出現錯誤之後，可以非常方便的跳轉到出錯處。

另外，是一個 [Asynchronous Lint Engine](https://github.com/w0rp/ale) 一個異步的檢查引擎。

### NERDCommenter

用於快速，批量註釋與反註釋，適用於任何你能想到的語言，會根據不同的語言選擇不同的註釋方式，方便快捷。

### NERDTree

[NERDTree](https://github.com/scrooloose/nerdtree) 的作用就是列出當前路徑的目錄樹，類似與一般 IDE，可以方便的瀏覽項目的總體的目錄結構和創建刪除重命名文件或文件名。

另外還有一個 NERDTreeTabs 插件，可以以 tab 形式顯示窗口，用於方便瀏覽目錄以及文件。

{% highlight text %}
map <F2> :NERDTreeToggle<CR>          " 使用F2鍵快速調出和隱藏它
autocmd vimenter * NERDTree           " 打開vim時自動打開NERDTree
{% endhighlight %}

如下是一些常用的命令：

{% highlight text %}
t   以Tab形式打開文件
i   分割成上下兩個窗口顯示文件
s   分割成左右兩個窗口顯示文件

C   將該目錄設置為根目錄
P   把有表移動到該目錄的根目錄
K   把有表移動到該目錄的第一個
J   把有表移動到該目錄的最後一個

p   切換到目錄的下一層
u   切換到目錄的上一層

m   通過NERDTree的選擇菜單進行操作
I   是否顯示隱藏文件切換
q   關閉NERDTree窗口
?   打開/關閉幫助指令
{% endhighlight %}

另外，比較好用的是提供的標籤操作。

<!--
書籤操作，以下命令只在在Nerdtree的buffer中有效。

:Bookmark name 將選中結點添加到書籤列表中，並命名為name（書籤名不可包含空格），如與現有書籤重名，則覆蓋現有書籤。</li><li>
:OpenBookmark name  打開指定的文件。</li><li>
:ClearBookmarks name 清除指定書籤；如未指定參數，則清除所有書籤。</li><li>
:ClearAllBookmarks 清除所有書籤。</li><li>
:ReadBookmarks 重新讀入'NERDTreeBookmarksFile'中的所有書籤。</li></ul>
:BookmarkToRoot 以指定目錄書籤或文件書籤的父目錄作為根結點顯示NerdTree
:RevealBookmark
如果指定書籤已經存在於當前目錄樹下，打開它的上層結點並選中該書籤

下面總結一些命令，原文地址<a href="http://yang3wei.github.io/blog/2013/01/29/nerdtree-kuai-jie-jian-ji-lu/">http://yang3wei.github.io/blog/2013/01/29/nerdtree-kuai-jie-jian-ji-lu/</a>
<pre style="font-size:0.8em; face:arial;">
o       在已有窗口中打開文件、目錄或書籤，並跳到該窗口
go      在已有窗口中打開文件、目錄或書籤，但不跳到該窗口
i       split 一個新窗口打開選中文件，並跳到該窗口
gi      split 一個新窗口打開選中文件，但不跳到該窗口
s       vsplit 一個新窗口打開選中文件，並跳到該窗口
gs      vsplit 一個新 窗口打開選中文件，但不跳到該窗口
O       遞歸打開選中 結點下的所有目錄
x       合攏選中結點的父目錄
X       遞歸 合攏選中結點下的所有目錄
t       在新 Tab 中打開選中文件/書籤，並跳到新 Tab
T       在新 Tab 中打開選中文件/書籤，但不跳到新 Tab
</pre>
Nerdtree當打開多個標籤時，會導致新打開的標籤沒有nerdtree窗口，此時可以使用nerdtree-tabs，將上述定義的快捷鍵設置為NERDTreeTabsToggle即可。
!       執行當前文件
e       Edit the current dif

雙擊    相當於 NERDTree-o
中鍵    對文件相當於 NERDTree-i，對目錄相當於 NERDTree-e

D       刪除當前書籤

P       跳到根結點
p       跳到父結點
K       跳到當前目錄下同級的第一個結點
J       跳到當前目錄下同級的最後一個結點
k       跳到當前目錄下同級的前一個結點
j       跳到當前目錄下同級的後一個結點

C       將選中目錄或選中文件的父目錄設為根結點
u       將當前根結點的父目錄設為根目錄，並變成合攏原根結點
U       將當前根結點的父目錄設為根目錄，但保持展開原根結點
r       遞歸刷新選中目錄
R       遞歸刷新根結點
m       顯示文件系統菜單 #！！！然後根據提示進行文件的操作如新建，重命名等
cd      將 CWD 設為選中目錄

I       切換是否顯示隱藏文件
f       切換是否使用文件過濾器
F       切換是否顯示文件
B       切換是否顯示書籤

:tabnew [++opt選項] ［＋cmd］ 文件      建立對指定文件新的tab
:tabc   關閉當前的 tab
:tabo   關閉所有其他的 tab
:tabs   查看所有打開的 tab
:tabp   前一個 tab
:tabn   後一個 tab

標準模式下：
gT      前一個 tab
gt      後一個 tab

MacVim 還可以藉助快捷鍵來完成 tab 的關閉、切換
cmd+w   關閉當前的 tab
cmd+{   前一個 tab
cmd+}   後一個 tab
-->

### Surround

一個專門用來處理這種配對符號的插件，它可以非常高效快速的修改、刪除及增加一個配對符號，對於前端工程師非常有用。

通常可以和 [repeat.vim](http://www.vim.org/scripts/script.php?script_id=2136) 配合使用，如下是常見的操作示例。

{% highlight text %}
Old text                  Command     New text ~
 "Hello *world!"           ds"         Hello world!
 [123+4*56]/2              cs])        (123+456)/2
 "Look ma, I'm *HTML!"     cs"<q>      <q>Look ma, I'm HTML!</q>
 if *x>3 {                 ysW(        if ( x>3 ) {
 my $str = *whee!;         vlllls'     my $str = 'whee!';
 <div>Yo!*</div>           dst         Yo!
 <div>Yo!*</div>           cst<p>      <p>Yo!</p>
{% endhighlight %}

如下是一個命令列表。

{% highlight text %}
 Normal mode
 ds  - delete a surrounding
 cs  - change a surrounding
 ys  - add a surrounding
 yS  - add a surrounding and place the surrounded text on a new line + indent it
 yss - add a surrounding to the whole line
 ySs - add a surrounding to the whole line, place it on a new line + indent it
 ySS - same as ySs

 Visual mode
 s   - in visual mode, add a surrounding
 S   - in visual mode, add a surrounding but place text on new line + indent it

 Insert mode
 <CTRL-s> - in insert mode, add a surrounding
 <CTRL-s><CTRL-s> - in insert mode, add a new line + surrounding + indent
 <CTRL-g>s - same as <CTRL-s>
 <CTRL-g>S - same as <CTRL-s><CTRL-s>
{% endhighlight %}

### vim-powerline/vim-airline

主題在目錄 `autoload/Powerline/Themes` 下，配色在 `autoload/Powerline/Colorschemes`，`vim-airline` 與 `vim-powerline` 類似但是更小，更輕便。

推薦使用後者，可以參考 [vim-airline](https://github.com/vim-airline/vim-airline)，其中主題 themes 已經單獨拆出來了。

### tags

tags 記錄了關於一個標識符在哪裡被定義的信息，比如 C/C++ 程序中的一個函數定義。vim 默認是支持 tags 的，那麼可以讓 vim 從任何位置跳轉到相應的標示位置。

{% highlight text %}
ctags -R --c++-kinds=+px --fields=+iaS --extra=+q .

常用參數：
-R
   遍歷循環子目錄生成tags；
--fields=+iaS
  將可用擴展域添加到tags中，
    i) 如有繼承，則標識出父類；
    a) 標明類成員的權限，如public、private等；
    S) 函數的信息，如原型、參數列表等；
-I identifier-list
   通常用於處理一些宏，如果只列出了那麼則會忽略；
--c++-kinds=+px
   記錄c++文件中的函數聲明和各種外部和前向聲明，使用p時同時也會添加extern的聲明；
--extra=+q
  是否將特定信息添加到tags中，q) 類成員信息，包括結構體；

其它常用命令：
----- 列舉出當前支持的語言，也可以自定義，具體沒研究過
$ ctags --list-languages
----- 查看擴展名跟語言的映射關係，從而可以使用正確的分析器
$ ctags --list-maps
----- 可識別的語法元素，默認打印所有，在生成時可以通過--c-kinds指定
$ ctags --list-kinds=c

生成的文件格式如下：
{tagname} {TAB} {tagfile} {TAB} {tagaddress} {term} {field} ..
   {tagname}     標識符名字，例如函數名、類名、結構名、宏等，不能含TAB符。
   {tagfile}     包含 {tagname} 的文件。
   {tagaddress}  可以定位到 {tagname} 光標位置的 Ex 命令，通常只包含行號或搜索命令。
                 出於安全的考慮，會限制其中某些命令的執行。
   {term}        設為 ;” ，主要是為了兼容vi編輯器，使vi忽略後面的{field}字段。
   {field} ..    也就是擴展字段，可選，用於表示此 {tagname} 的類型是函數、類、宏或是其它。

常見快捷鍵如下：
Ctrl+]       跳轉到定義處；
Ctrl+T       跳轉到上次tags處；
Ctrl+i       (in)跳轉下一個；
Ctrl+o       (out)退回原來的地方；
gd           轉到當前光標所指的局部變量的定義；
gf           打開頭文件；
:ju          顯示所有可以跳轉的地方；
:set tags    查看加載的tags；
:tag name    調轉到name處；
:stag name   等價於split+tag name
:ta XXX      跳轉到符號XXX定義處，如果有多個符號，直接跳轉到第一處；
:ts XXX      列出符號XXX的定義；
:tj XXX      可看做上面兩個命令的合併，如果只找到一個符號定義，那麼直接跳轉，有多個，則讓用戶自行選擇；
:ptag name   預覽窗口顯示name標籤，光標跳到標籤處；
Ctrl-W + }   預覽窗口顯示當前光標下單詞的標籤，光標跳到標籤處；
:pclose      關閉預覽窗口；
:pedit file.h 在預覽窗口中編輯文件file.h，在編輯頭文件時很有用；
:psearch atoi 查找當前文件和頭文件中的單詞並在預覽窗口中顯示匹配，在使用沒有標籤文件的庫函數時十分有用。
{% endhighlight %}

如果有多個可以使用 `tfirst` `tlast` `tprevious` `tnext` `tselect` 選擇，也可以 `:tag name_<TAB>` 自動補全，或者使用 `tselect /^write` 正則表達式。

#### 生成系統tags

{% highlight text %}
----- 添加系統的tags
$ ctags --fields=+iaS --extra=+q -R -f ~/.vim/systags /usr/include /usr/local/include
:set tags+=~/.vim/systags
{% endhighlight %}

此時，基本可以跳轉到系統函數，不過仍有部分函數未添加到tags中，常見的有如下的示例。

{% highlight c %}
extern int listen (int __fd, int __n) __THROW;
extern int strcmp (__const char *__s1, __const char *__s2)
     __THROW __attribute_pure__ __nonnull ((1, 2));
{% endhighlight %}

也就是因為存在 `__THROW` `attribute_pure` `nonull` 等屬性，導致認為上述的聲明不是函數，都需要忽略。如果需要 `#if 0` 裡面的定義，可以使用 `-if0=yes` 來忽略 `#if 0` 這樣的定義。

{% highlight text %}
$ ctags -I __THROW -I __attribute_pure__ -I __nonnull -I __attribute__ \   忽略這裡的定義
    --file-scope=yes              \     例如對於static聲明只在一個文件中可見
    --langmap=c:+.h               \     定義擴展名和語言的映射關係，可以通過--list-maps查看
    --languages=c,c++             \     使能哪些語言
    --links=yes                   \     是否跟蹤符號鏈接指向的文件
    --c-kinds=+p --c++-kinds=+p   \     指定生成哪些C語言的tag信息
    --fields=+iaS --extra=+q -R -f ~/.vim/systags /usr/include /usr/local/include
{% endhighlight %}

可以在配置文件中添加如下的內容，然後在源碼目錄內可以通過 `Ctrl-F12` 生成 tags 文件。

{% highlight text %}
map <C-F12> :!ctags -R --c-kinds=+px --fields=+iaS --extra=+q <CR>
{% endhighlight %}

### cscope

{% highlight text %}
:help if_cscop
:cscope show       查看已經加載的數據庫
:cscope add        添加數據庫
:cscope kill       關閉已加載的數據庫
:cscope reset      重新初始化所有連接
{% endhighlight %}

可以將 cscope 看做是 ctags 的升級版本，提供交互式查詢語言符號功能，如查詢哪些地方使用某個變量或調用某個函數。

和 tags 一樣，默認支持，可以通過 `:version` 查看，沒有則增加 `--enable-cscope` 編譯選項，詳細可查看官方網址為 [cscope.sourceforge.net](http://cscope.sourceforge.net/) 。

確定 Vim 已支持 Cscope 後，將文件 
[cscope_maps.vim](http://cscope.sourceforge.net/cscope_maps.vim) 下載到 `~/.vim/plugin` 目錄下；也可以將其內容添加到 `~/.vimrc` 配置文件中。

在 CentOS 中，可以通過 `yum install cscope` 安裝，在第一次解析時掃描全部文件，以後再調用 cscope 只會掃描那些改動過的文件。

進入項目代碼根目錄運行命令生成 cscope 數據庫文件：

{% highlight text %}
cscope -Rbq -f cscope.out

常用參數：
  -R     表示遞歸操作，無該參數則進入基於cureses的GUI界面，通過方向鍵選擇查找類
           型、TAB鍵搜索結果和搜索類型中選擇、Ctrl-D退出；
  -b     生成數據庫文件後直接退出，否則生成數據庫之後會進入 cscope 界面；
  -q     表示生成 cscope.in.out 和 cscope.po.out 文件，加快 cscope 的索引速度；
  -C     在搜索時忽略大小寫；
  -Ppath 默認是相對路徑，可以在相對路徑的文件前加上path。
  -Idir  在指定的目錄中查找頭文件；
{% endhighlight %}

<!--
-k 在生成數據庫時如果在當前項目目錄下沒有找到頭文件，則會自動到/usr/include目錄下查找，通過-k選項關閉；
-i 如果不是cscope.files，則通過該選項指定源文件列表，-表示標準輸入；
-u 掃描所有文件，重新生成交叉索引文件；
-->

默認只解析 C、lex 和 yacc 文件，如果希望解析 C++ 和 Java 文件，可以通過指定 cscope.files 文件，此時不再需要 -R 選項。

<!--
  然後可以通過 cscope find 進行查找，vim支持8中cscope的查詢功能，如下：<ul><li>
     0/s, symbol: 查找C語言符號，即查找函數名、宏、枚舉值等出現的地方。</li><li>
     1/g, global: 查找函數、宏、枚舉等定義的位置，通常為全局變量，類似ctags所提供的功能。</li><li>
     2/d, called: 查找本函數所調用的函數。</li><li>
     3/c, calls: 查找所有調用本函數的函數。</li><li>
     4/t, text: 查找指定的字符串。</li><li>
     6/e, egrep: 查找egrep模式，相當於egrep功能，但查找速度快多了。</li><li>
     7/f, file: 查找並打開文件，類似vim的find功能。</li><li>
     8/i, includes: 查找包含當前cursor的文件。</li></ul>
 如查找調用do_cscope()函數的函數，可以輸入 :cs find c do_cscope 或者使用 :cs find s do_cscope 。</li><br><li>

    </li><br><li>
 Cscope 常用快捷鍵，此處是通過vimrc配置的<br>
   Ctrl-\ s 查找所有當前光標所在符號出現過位置。<br>
   Ctrl-\ c 查找所有調用當前光標所在函數的函數。<br><br>


find $PROJECT_HOME -name *.c -o -name "*.cpp" -o -name "*.cc" -o -name "*.h" -o -name "*.hpp" > cscope.files
cscope -qbR -i cscope.files


    vim提供了一些選項可以調整它的cscope功能：<ul><li>
        cscopeprg選項用於設置cscope程序的位置。</li><br><li>

        cscopequickfix設定是否使用quickfix窗口來顯示cscope的結果，這是一組用逗號分隔的值，每項都包含於csope-find命令（s, g, d, c, t, e, f, 或者i）和旗標（+, -或者0）。<!-- ‘+’預示著顯示結果必須追加到quickfix窗口。‘-’隱含著清空先前的的顯示結果，’0’或者不設置表示不使用quickfix窗口。查找會從開始直到第一條命令出現。默認的值是””（不使用quickfix窗口）。下面的值似乎會很有用：”s-,c-,d-,i-,t-,e-”。 詳情請”:help cscopequickfix“。</li><br><li>

        如果你想vim同時搜索tag文件以及cscope數據庫，設置cscopetag選項，此時使用的是cstag而不是tag，默認是關閉的set cst/nocst。</li><br><li>

        cscopetagorder選項決定:cstag是先查找tag文件還是先查找cscope數據庫。設置為0則先查找cscope數據庫，設置為1先查找tag文件。默認是0，set csto=0/1。</li><br><li>

        cscopeverbose是否顯示數據庫增加/失敗，如 set csverb/nocsverb。</li><br><li>

       ‘cspc’的值決定了一個文件的路徑的多少部分被顯示。默認值是0，所以整個路徑都會被顯示。值為1的話，那麼就只會顯示文件名，不帶路徑。其他值就會顯示不同的部分。例如 :set cspc=3 將會顯示文件路徑的最後3個部分，包含這個文件名本身。 </li></ul>

    vim的手冊中給出了使用cscope的建議方法，使用命令”:help cscope-suggestions“查看。
 </p>
-->

#### 查看Linux Kernel

對於內核中感興趣的文件通常需要去除文本、非x86源碼、非驅動。

{% highlight text %}
$ tar -Jxf linux-kernel.tar.xz
$ LNX=/home/andy/linux-kernel
$ find  $LNX                                                              \
    -path "$LNX/arch/*" ! -path "$LNX/arch/i386*" -prune -o               \
    -path "$LNX/include/asm-*" ! -path "$LNX/include/asm-i386*" -prune -o \
    -path "$LNX/tmp*" -prune -o                                           \
    -path "$LNX/Documentation*" -prune -o                                 \
    -path "$LNX/scripts*" -prune -o                                       \
    -path "$LNX/drivers*" -prune -o                                       \
    -name "*.[chxsS]" -print >/home/andy/cscope/cscope.files
$ cd /home/andy/cscope
$ cscope -b -q -k                             生成Cscope數據庫文件
$ add some files and 'cscope -b -q -k'        添加新文件重新生成數據庫
{% endhighlight %}

在 find 命令中 `-o` 表示 `or`，`-prune` 表示不包含該目錄，如果前面又添加了 `!` 號，則表示只包含該目錄。

<!--
duplicate cscope database not added
使用Cscope時兩次加載cscope，可以通過 grep -rne "cscope.out" 檢查 ~/.vim/ 目錄和 /etc/ 目錄，通常時由於 ~/.vim/plugin
-->

### 其它插件

簡單介紹一些常見的插件。

#### xterm-color-table

查看顏色列表，可從 [color-table](http://www.vim.org/scripts/script.php?script_id=3412) 下載，下載解壓後可以放置到 ```$VIMRUNTIME/colors``` 目錄下，通過 ```:XtermColorTable``` 命令即可查看。

其它可以查看 doc 目錄下的幫助文件，也可以查看如下的圖片 [xterm color](/images/linux/vim_xterm_color_chart.png) 。

#### number.vim

更好的顯示行號，在 Normal 狀態時會在主窗口中顯示絕對/相對行號，插入狀態時顯示絕對行號，不需要進行任何配置。

{% highlight text %}
let g:numbers_exclude = ['tagbar', 'gundo', 'minibufexpl', 'nerdtree']
nnoremap <F3> :NumbersToggle<CR>
nnoremap <F4> :NumbersOnOff<CR>
{% endhighlight %}

#### vim-startify

一個啟動界面替換默認的 vim 啟動界面，會顯示一些最近打開的文件等信息。

#### fencview

對於 vim 打開文件時如何自動識別，可以使用如下設置：

{% highlight text %}
set fileencodings=utf-8,gb2312,ucs-bom,euc-cn,euc-tw,gb18030,gbk,cp936
{% endhighlight %}

或者通過 fencview 插件，該插件主要用於自動識別漢字、日文等，主要有如下的命令。

{% highlight text %}
:FencAutoDetect                  " 自動識別文件編碼
:FencView                        " 打開一個編碼列表窗口，用戶選擇編碼reload文件
{% endhighlight %}

可以在 vimrc 中設置如下選項：

{% highlight text %}
let g:fencview_autodetect = 1    " 打開文件時自動識別編碼
let g:fencview_checklines = 10   " 檢查前後10行來判斷編碼，或者'*'檢查全文
{% endhighlight %}

可以將 `autodetect` 設置為 0，當發現為亂碼時，那麼可以使用 `:FencAutoDetect` 命令查看，然後通過 `:set fileencoding` 查看當前編碼，並將其編碼放置到 `fileencodings` 中。

可以參考 [VIM 文件編碼識別與亂碼處理](http://edyfox.codecarver.org/html/vim_fileencodings_detection.html) 。

<!-- /reference/linux/vim_fileencoding.maff -->

<!--
Adds file type glyphs/icons to popular Vim plugins
https://github.com/ryanoasis/vim-devicons
-->


#### 常用快捷鍵

{% highlight text %}
F2    替換末尾的空格
F3    Tagbar(左側)
F4    Nerdtree(右側)
{% endhighlight %}


<!--
##############################################################################

### 移動光標 :help motion

其中幫助還可以查看 :help word/cursor-motions/various-motions 命令。

在 VIM 中，光標的移動非常重要，很多命令可以和其進行組合，常見的如 &lt;start position&gt; &lt;command&gt; &lt;end position&gt; ，如果沒有指定 &lt;start position&gt; 則默認為當前光標所在位置。

如 0y$ 複製從 0 開始到 $ 結尾，也就是行首到行尾的數據；ye 複製從當前光標到詞的結尾的數據；y2/foo 複製當前光標到第二次出現 foo 的數據。不僅 y(yank) 其它的操作如 d(delete)、v(visual select)、c(change)、gU (uppercase，變為大寫)、gu (lowercase，變為小寫) 等。

### 常規方式

{% highlight text %}
[h|j|k|l]           → 等價於光標鍵(←↓↑→)
35j                 → 向下調轉35行

0                   → 數字零，到行頭
^/Home              → 到本行第一個不是空字符的位置(所謂空字符就是空格、tab、換行、回車等)
<NUM>|              → 跳轉到某一列
$/End               → 到本行行尾
g_                  → 到本行最後一個不是空字符的位置

<NUM>G              → 跳轉到第<NUM>行，默認到最後一行
:<NUM>              → 同上
<NUM>gg             → 到第N行，默認第一行

H                   → 跳轉到窗口的第一行
M                   → 跳轉到窗口的中間
L                   → 跳轉到窗口的最後一行
w                   → 跳轉到下一個單詞的開始處，用標點符號和空格分開
W                   → 跳轉到下一個單詞的開始處，用空格分開
b                   → 跳轉到上一個單詞的開始處，用標點符號和空格分開
B                   → 跳轉到上一個單詞開始處，用空格分開
e                   → 跳轉到單詞的末尾，使用標點符號和空格分開
E                   → 跳轉到單詞的末尾，使用空格分開
(                   → 跳轉到上一句
)                   → 跳轉到下一句
{                   → 調轉到上一段
}                   → 調轉到下一段
{% endhighlight %}

### 查找調轉

{% highlight text %}
%                   → 匹配括號移動，包括 (, {, [，需要在當前行
[*|#]               → 匹配光標當前所在的單詞，移動光標到下一個或上一個匹配單詞

3fh                 → 調轉到第3次出現h字符的地方
t,                  → 到逗號前的第一個字符，逗號可以變成其它字符，T反向
3fa                 → 在當前行查找第三個出現的a，F反向

Ctrl-o              → 跳回到前一個位置
Ctrl-i              → 跳轉到後一個位置

/pattern            → 搜索pattern的字符串，通過 n N 來查找下一個或上一個
{% endhighlight %}

![network]({{ site.url }}/images/vim/vim_word_moves.jpg){: .pull-center}
![network]({{ site.url }}/images/vim/vim_line_moves.jpg){: .pull-center}

### Marks :help mark-motions

在 VIM 中可以自己定義標記，用來調轉。

{% highlight text %}
''(雙單引號)              → 在最近的兩個標記的地方相互切換，回到第一個非空字符上
``(雙反引號)              → 與上述的功能相同，只是該命令會回到之前的列
:jumps                   → 列出可以跳轉的位置列表，當前的用>標記
m[char]                  → 使用26個字符(a-z)標記當前位置，如ma
`[char]                  → 跳轉到標記處，如`a
'[char]                  → 同上，跳轉到所在行的行首，如'a
:marks                   → 列出所有的標記
:delmarks markname       → 刪除標記。
{% endhighlight %}

## 查找 :help pattern

通過* # 可以查找當前光標所在單詞位置。

{% highlight text %}
/\<step\>                → 只查找step，其中\<和\>表示一個單詞的起始和結尾
/\d                      → 查找一個數字
/\d\+                    → 查找一個或兩個數字
/\d\*                    → 查找0個或多個數字
{% endhighlight %}

## 替換，h :substite

vim 中替換命令方式如下。

{% highlight text %}
[ADDR]s/src/dest/[OPTION]
      ADDR: %(1,$)整個文件；1,20第1行到第20行；.,$當前行到行尾
    OPTION: g全局替換；c每次需要確認；i忽略大小寫；p不清楚作用
{% endhighlight %}

可以參考如下的示例。

{% highlight text %}
:%s/\(That\) \(this\)/\u\2 \l\1/       將That this 換成This that
:%s/child\([ ,.;!:?]\)/children\1/g    將非句尾的child換成children
:%s/  */ /g                            將多個空格換成一個空格
:%s/\([:.]\)  */\1 /g                  使用空格替換句號或者冒號後面的一個或者多個空格
:g/^$/d                                刪除所有空行
:%s/^/  /                              在每行的開始插入兩個空白
:%s/$/hello/g                          在每行的莫為插入兩個空白
:s/hi/hh/g                             將本行的hi替換為hh
:.,5/$/./                              在接下來的6行末尾加入.
:%s/\r//g                              刪除DOS的回車符(^M)
:%s/\r/\r/g                            將DOS回車符，替換為回車
:%s/^\(.*\)\n\1$/\1/g                  刪除重複的行
{% endhighlight %}

<!--
http://www.guckes.net/vi/substitute.html<br>
<a href="reference/vim/vi_substitution_guide.html">Vi Pages - Substitution Guide </a>


:set ignorecase     Ignore case in searches
:set smartcase  Ignore case in searches excepted if an uppercase letter is used
:%s/\<./\u&/g   Sets first letter of each word to uppercase
:%s/\<./\l&/g   Sets first letter of each word to lowercase
:%s/.*/\u&  Sets first letter of each line to uppercase
:%s/.*/\l&  Sets first letter of each line to lowercase
Read/Write files
:1,10 w outfile     Saves lines 1 to 10 in outfile
:1,10 w >> outfile  Appends lines 1 to 10 to outfile
:r infile   Insert the content of infile
:23r infile     Insert the content of infile under line 23
File explorer
:e .    Open integrated file explorer
:Sex    Split window and open integrated file explorer
:browse e   Graphical file explorer
:ls     List buffers
:cd ..  Move to parent directory
:args   List files
:args *.php     Open file list
:grep expression *.php  Returns a list of .php files contening expression
gf  Open file name under cursor
Interact with Unix
:!pwd   Execute the pwd unix command, then returns to Vi
!!pwd   Execute the pwd unix command and insert output in file

Alignment
:%!fmt  Align all lines
!}fmt   Align all lines at the current position
5!!fmt  Align the next 5 lines
Tabs
:tabnew     Creates a new tab
gt  Show next tab
:tabfirst   Show first tab
:tablast    Show last tab
:tabm n(position)   Rearrange tabs
:tabdo %s/foo/bar/g     Execute a command in all tabs
:tab ball   Puts all open files in tabs
Window spliting
:e filename     Edit filename in current window
:split filename     Split the window and open filename
ctrl-w up arrow     Puts cursor in top window
ctrl-w ctrl-w   Puts cursor in next window
ctrl-w_     Maximise current window
ctrl-w=     Gives the same size to all windows
10 ctrl-w+  Add 10 lines to current window
:vsplit file    Split window vertically
:sview file     Same as :split in readonly mode
:hide   Close current window
:­nly   Close all windows, excepted current
:b 2    Open #2 in this window
Auto-completion
Ctrl+n Ctrl+p (in insert mode)  Complete word
Ctrl+x Ctrl+l   Complete line
:set dictionary=dict    Define dict as a dictionnary
Ctrl+x Ctrl+k   Complete with dictionnary
Marks
mk  Marks current position as k
˜k  Moves cursor to mark k
d™k     Delete all until mark k
Abbreviations
:ab mail mail@provider.org  Define mail as abbreviation of mail@provider.org
Text indent
:set autoindent     Turn on auto-indent
:set smartindent    Turn on intelligent auto-indent
:set shiftwidth=4   Defines 4 spaces as indent size
ctrl-t, ctrl-d  Indent/un-indent in insert mode

## 撤銷/重做 help undo-todo

幫助文檔，也可以通過 help usr_32.txt 或者 help undolist 查看。

{% highlight text %}
u                        → 撤銷
Ctrl-r                   → 重做
:earlier 4m              → 回到4分鐘前
:later 45s               → 前進45s鍾
:undo 5                  → 撤銷5次
:undolist                → 查看undo列表
{% endhighlight %}

## 多窗口操作 :help split

{% highlight text %}
new filename             → 水平打開新窗口
<NUM>sp(lit) filename    → 水平窗口打開文件，默認當前窗口文件，NUM指定新窗口的大小
Ctrl+W s                 → 同上

vnew filename            → 垂直打開新窗口
<NUM>vsp(lit) filename   → 垂直窗口打開文件，默認當前窗口文件，NUM指定新窗口的大小
Ctrl+W v                 → 同上

close                    → 關閉窗口，內容仍然保存，會保留最後一個窗口
Ctrl+W c                 → 同上
quit/ZZ/Ctrl+W q         → 可以關閉最後一個窗口，當多於一個窗口時上述的命令等效
only                     → 只保留當前窗口，如果其它窗口有未保存的修改，則會提示錯誤

CTRL-W w                 → 切換窗口
CTRL-W h                 → 到左邊的窗口
CTRL-W j                 → 到下面的窗口
CTRL-W k                 → 到上面的窗口
CTRL-W l                 → 到右邊的窗口
CTRL-W t                 → 到頂部窗口
CTRL-W r                 → 各個窗口旋轉
CTRL-W b                 → 到底部窗口

<NUM>Ctrl-w =|+|-        → 均分/增加/減小窗口的大小(NUM行)
<NUM>Ctrl-w [><]         → 寬度設置
<NUM>Ctrl-w _            → 設置當前窗口行數，默認最大化
<NUM>Ctrl-w |            → 垂直設置
:[vertical] res(ize) num → 指定窗口的行數
:[vertical] res(ize)+num → 增大num行
:[vertical] res(ize)-num → 減小num行
{% endhighlight %}

也可以啟動 VIM 時，使用 -[O|o]NUM 參數來垂直或水平分屏，例如，vim -O2 file1 file2。


![network]({{ site.url }}/images/vim/vim_split.gif){: .pull-center}

VIM 支持文件瀏覽，可以通過 :Ex 在當前窗口開啟目錄瀏覽器，:Sex 水平分割當前窗口，並在一個窗口中開啟目錄瀏覽器。
## 拷貝/粘貼以及粘貼板配置

{% highlight text %}
d    → delete刪除
y    → yank複製
P    → 在當前位置之後粘貼
p    → 在當前位置之前粘貼

yap  → 複製整個段，注意段後的空白行也會被複制
yw   → 也會複製詞後的空格
yy   → 複製當前行當行到 ddP
{% endhighlight %}

常用的一些技巧包括：xp (交換兩個字符)，dwwP (交換兩個詞，刪除一個 word，跳轉到下一個，複製) 。


vim 中有十幾個粘貼板，分別是 [0-9]、a、"、+ 等，可以直接通過 :reg 命令查看各個粘貼板裡的內容，如果只是簡單用 y 命令，只會複製到 " 粘貼板裡，同樣 p 粘貼的也是這個粘貼板裡的內容。

\+ 為系統粘貼板，用 "+y 將內容複製到該粘貼板後可以使用 Ctrl＋V 將其粘貼到其他文檔，反之則可以通過 "+p 複製到 VIM 。另外，對於系統粘貼板比較有效的是 Shift+Ctrl+v，以及 Shift+Insert 即可，實踐中實際要比 "*p、"+p 有用！

要將 vim 的內容複製到某個粘貼板，需要在 Normal 模式中，選擇要複製的內容，然後按 "Ny 完成複製，其中 N 為如上所述的粘貼板號。反之，複製到 VIM 時，也要在 Normal 模式下，通過 "Np 粘貼。

另需注意，默認 vim.basic 是沒有 + 號寄存器的，需要單獨編譯，或者 vim.gtk、vim.gnome 才會有。
-->

## 雜項

簡單記錄下雜七雜八。

### 常用技巧

#### 大小些操作

按 `v` 進入 `visual` 模式，`ap` paragraph 選擇整段，`aw` word 選擇一個詞，通過 `~` 大小寫取反；通過 `gU` 更改為大些，`gu` 改為小寫。

#### 取消備份

Windows 的配置文件在安裝目錄下，為 `_vimrc` 。可以添加 `set nobackup | backup` ，如果只需要當前文件不保存可以 `:set nobackup` ，或者保存在固定目錄下 `set backupdir=dir` 。

默認會將文件保存為 `filename~` ，也可以通過 `:set backupext=.bak` 指定後綴名，當然這樣只能保存最後的一個版本。

另外，在 VIM 中，還提供了 patchmode 把第一次的原始文件備份下來，不會改動，可以直接通過 `:set patchmode=.orig` 命令開啟。

#### 沒有使用 root

如果由於權限不足導致無法保存，可以通過如下方式保存 `:w !sudo tee %` 。

#### 二進制文件

通常使用外部程序 `xxd` 達到這樣的效果，通過 `vim -b file` 命令以二進制格式打開, `:%xxd` 將二進制轉換為文本，然後通過 `:%xxd -r` 重新保存為二進制，詳細參數可以參考 `man xxd` 。

另外，可以通過 `:set display=uhex` 以 uhex 模式顯示，用來顯示一些無法顯示的字符，例如控制字符之類。

#### 外部修改

可以通過 `:e` 或 `:e!` 加載，後者會放棄之前的修改。

#### 輸入特殊字符

通過 `:digraphs` 可以查看特殊字符，在插入模式下或輸入命令時可以通過 `Ctrl-k XX` ，其中 `XX` 為 digraphs 前面指定的兩個字符；也可以通過 `Ctrl-v NUM` 對應的數字輸入特殊字符。

#### 替換^M字符

在 Linux 下查看一些 Windows 下創建的文本文件時，經常會發現在行尾有一些 `^M` 字符，有幾種方法可以處理。

##### 1. dos2unix

使用 `dos2unix` 命令，發佈版本一般都帶這個工具，通過 `dos2unix myfile.txt` 修改即可。

##### 2. vim

使用 vim 的替換功能，當啟動 vi 後，進入命令模式，輸入以下命令:

{% highlight text %}
:%s/^M$//g                      # 去掉行尾的^M
:%s/^M//g                       # 去掉所有的^M
:%s/^M/[ctrl-v]+[enter]/g       # 將^M替換成回車
:%s/^M/\r/g                     # 將^M替換成回車
{% endhighlight %}

##### 3. sed

使用 sed 命令，和 vi 的用法相似 `sed -e 's/^M/\n/g' myfile.txt` 。

注意：這裡的 `^M` 要使用 `CTRL-V CTRL-M` 生成，而不是直接鍵入 `^M` 。

#### 光標閃爍

可以通過 `set gcr=a:block-blinkon0` 設置，其中 `gcr` 是 `guicursor` 的簡寫，`a` 表示所有模式，冒號後面是對應模式下的行為參數，每個參數用 `-` 分隔，`block` 說明用塊狀光標，`blinkon` 表示亮起光標時長，時長為零表示禁用閃爍。

<!--
也可以是 blinkoff0 或者 blinkwait0。具體參考 :help gcr。
Applications->System Tools->Settings->Keyboard->[Cursor Blinking]
-->

### 異常處理

如果發現某個插件無法使用，可以通過 `:scriptnames` 命令查看當前已經加載的腳本程序。

### 鍵盤圖

![vim keyboard 1]({{ site.url }}/images/misc/vim-keyboard-1.gif "vim keyboard 1"){: .pull-center width="90%" }

也可以參考英文版的 [vim Keyboard en]({{ site.url }}/images/misc/vim-keyboard-en.gif) ，或者 [參考下圖](http://michael.peopleofhonoronly.com/vim/) 。

![vim keyboard 2]({{ site.url }}/images/misc/vim-keyboard-2.png "vim keyboard 2"){: .pull-center width="90%" }

另外，關於按鍵列表可以參考 [vim keys]({{ site.url }}/reference/linux/vim-keys.pdf) 。

<!-- http://jrmiii.com/attachments/Vim.pdf -->

## 參考

如果要編寫 VIM 腳本，可以參考 Trinity 這個簡單的插件，常見的語法可以參考 Vim 腳本語言官方文檔 [Write a Vim script](http://vimdoc.sourceforge.net/htmldoc/usr_41.html) 。

可以查看中文文檔 [VIM 中文幫助文檔](http://vimcdoc.sourceforge.net/doc/) ；另外一個很經典的文檔 [Learn Vim Progressively](http://yannesposito.com/Scratch/en/blog/Learn-Vim-Progressively/)，以及 [中文版](http://coolshell.cn/articles/5426.html)； 還有就是一個 PDF 教程，也就是 [Practical Vim.pdf](/reference/linux/Practical_Vim.pdf) 。

一個 vim 的遊戲，用於熟悉各種操作，可以參考 [vim adventures](http://vim-adventures.com/) 。

關於如何針對新的語言添加 tags 可以參考 [How to Add Support for a New Language to Exuberant Ctags](http://ctags.sourceforge.net/EXTENDING.html) 。

<!--
https://raw.githubusercontent.com/nvie/vimrc/master/vimrc
https://github.com/amix/vimrc
https://github.com/vgod/vimrc
https://github.com/humiaozuzu/dot-vimrc
https://github.com/xolox/vim-easytags
https://github.com/ruchee/vimrc/blob/master/_vimrc
http://www.cnblogs.com/ma6174/archive/2011/12/10/2283393.html
https://www.zhihu.com/question/26713049

vim 配置文件，將 F2 等功能添加到 default 文件中。

http://www.yeolar.com/note/2010/01/27/vim/
https://github.com/vim-airline/vim-airline/wiki/FAQ

<a href="http://python.42qu.com/11180003">使用Vim打造現代化的Python IDE</a>。<br><br>
<a href="http://python.42qu.com/11165602">如何用python寫vim插件</a>

http://ju.outofmemory.cn/entry/79671                      經典插件的介紹
http://blog.chinaunix.net/uid-24118190-id-4077308.html    VIM終極配置
/reference/linux/{default.vim, plugin.vim}                本地保存的版本
-->

{% highlight text %}
{% endhighlight %}
