---
title: VIM 使用简介
layout: post
comments: true
language: chinese
category: [misc]
keywords: vim,编辑器
description: Vim 是一个功能强大、高度可定制的文本编辑器，在 Vi 的基础上改进和增加了很多特性。与其相匹敌的是 Emacs ，这两个都是不错的编辑器，在此不再比较两者的优劣，仅介绍 Vim 相关的内容。
---

Vim 是一个功能强大、高度可定制的文本编辑器，在 Vi 的基础上改进和增加了很多特性。与其相匹敌的是 Emacs ，这两个都是不错的编辑器，在此不再比较两者的优劣，仅介绍 Vim 相关的内容。

<!-- more -->

![vim logo]({{ site.url }}/images/misc/vim_logo.png "vim logo"){: .pull-center width="45%" }

## 简介

启动 Vim ，默认会进入 Normal 模式；按下键 i 进入 Insert 模式；此时，可以输入文本；按下 ESC 键，返回 Normal 模式。注意：在 Normal 模式下，所有的按键都是功能键。

通过 ```:help <command>``` 可以查看相关命令的帮助文件，或者直接通过 ```:help``` 查看帮助文件。

### 常用操作

首先是常见的插入、替换操作。

{% highlight text %}
===== 插入操作 :help inserting
a                   → 在当前光标字符后插入
A                   → 在当前行的末尾插入
i                   → 在当前光标字符前插入
I                   → 在当前行的第一个非空字符前插入
o                   → 在当前行后插入一个新行，也即打开新行
O                   → 在当前行前插入一个新行

===== 替换操作 :help replacing
s                   → substitute替换当前字符，删除字符+进入插入模式
S                   → 替换一行，删除行+进入插入模式
r                   → 替换一个字符，进入替换模式，自动回到normal模式
R                   → 可以替换多个连续字符，Esc退出Replace模式
c                   → change改变选中内容，通常使用Visual Mode，选中+c
cw                  → 替换从光标所在位置后到一个单词结尾的字符；首先删除，然后等待插入
{% endhighlight %}

### 源码安装

很多的一些插件依赖一些高版本的特性，所以需要手动编译安装，直接从 [www.vim.org](http://www.vim.org/) 或者 [github](https://github.com/vim/vim) 上下载相关的版本。

{% highlight text %}
# yum remove vim-common vim-enhanced vim-filesystem vim-minimal
$ git clone https://github.com/vim/vim.git
$ cd vim
$ ./configure -h
$ ./configure --with-features=huge                                      \
    --enable-pythoninterp                                               \
    --with-python-config-dir=/usr/lib/python2.7/config-x86_64-linux-gnu \
    --enable-multibyte                                                  \   多字节，支持UTF8
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

如果有多个配置，则可以通过 ```source ~/.vim/default.vim``` 加载。

#### mapleader

可以通过 ```let mapleader=','``` 设置命令的前缀，然后直接使用即可。

#### 启动时间

可以通过 `vim --startuptime tmp.txt` 命令将启动时间信息保存在 `tmp.txt` 文件中，然后通过 `sort -nrk 2` 排序即可。

另外，可以通过 `vim --noplugin` 取消插件加载。

#### 加载插件

通过 `:scriptnames` 命令查看发现没有加载相应的插件。

## 基本配置

vim 含有多种变量，`$HOME` 表示环境变量；`&options` 表示选项；`@a` 表示寄存器；可以通过 `:help function-list` 查看所支持的函数列表；`has()` 用于判断 vim 是否支持这一特性。

如果不知道 vim 配置文件的位置，可以通过 ```:version``` 查询，不同的平台其配置文件名、配置文件顺序会有所区别；还可以通过 ```:help vimrc``` 来查看配置文件在所有平台上的加载位置。

在 Windows 平台下，通常为 `_vimrc` (也即 `$VIM/_vimrc` )，可以通过 `source` 指令包含其它的配置文件，如与 `_vimrc` 同目录下 `source $VIM/myvim.vim` 。

在配置文件中，可以指定变量，而对应的变量可通过 ```:echo $VIM``` 查看。

### Tab 操作

{% highlight text %}
:help tabpage

:tabnew file :tabe file      → 新建或打开某一文件并开启新标签页
:tab split                   → 用标签页打开当前编辑的文件
:tabf filename_re            → 基于正则表达式递归遍历当前工作目录查找名称匹配的文件并为其建立新标签页

:tabs                        → 显示已打开标签页的列表，&lt;指示当前页，+显示修改未保存
:tabc    :tabnew             → 关闭当前标签页等价与:q，新建
:tabn    :tabp               → 移动到下/上一个标签页
:tablast :tabfirst           → 移动到最后/第一个标签页
gt                           → 切换到下一个Tab
gT                           → 反向切换

:tabmove 0                   → 将当前tab移动到第一个位置，位置编号从0开始
:tabdo %s/aaa/bbb/g          → 在每个打开的Tab上执行操作

:tab help tabpage            → 使用Tab而非Windows打开帮助窗口
:help setting-guitablabel    → 自己配置tab标题
{% endhighlight %}

`:tabr` 跳转第一个标签页 gvim 提供了 remote-tab 的功能。

在标签栏中，各标签页的默认名称是对应文件所在路径全称的简写，如 `/usr/share/doc/test.txt` 文件所对应的标签页名默认是 `/u/s/d/test.txt`，这样的标签页名看上去有些诡异。可以在配置文件中添加如下内容，在标签页栏中去除当前所编辑文件的路径信息，只保留文件名。

{% highlight text %}
function ShortTabLabel ()
    let bufnrlist = tabpagebuflist (v:lnum)
    let label = bufname (bufnrlist[tabpagewinnr (v:lnum) -1])
    let filename = fnamemodify (label, ':t')
    return filename
endfunction

set guitablabel=%{ShortTabLabel()}
{% endhighlight %}

### 缓冲区

{% highlight text %}
:help buffer|buffer-list
{% endhighlight %}

缓冲区是一块内存区域，里面存储着正在编辑的文件，如果没有把缓冲区里的文件存盘，那么原始文件不会被更改。

{% highlight text %}
:e <path/to/file>            → 打开一个文件，保存到缓冲区列表，e是edit缩写
:badd <path/to/file>         → 添加到缓冲区列表中
:saveas <path/to/file>       → 另存为 &lt;path/to/file&gt;
:wq                          → 存盘并退出
:x                           → 表示仅在需要时保存
ZZ                           → 不需要输入冒号并回车
:q!                          → 退出不保存
:qa!                         → 强行退出所有的正在编辑的文件，就算别的文件有更改

:[buffers|ls]                → 列出当前编辑中所有的缓冲区状态
:buffer number|filename      → 切换缓冲区，可通过缓冲区的序号或者打开文件的名字选择缓冲区
:sbuffer number|filename     → 水平分割窗口
:ball                        → 为每个缓冲区打开一个窗口，默认按顺序水平切分
:bnext :bprevious            → 调转到下/上一个缓冲区，简写为:bn/:bp，:n 调转到下个文件
:blast :bfirst               → 调转到最后/第一个缓冲区
:bd(elete) number|filename   → 删除缓冲区
:3 bdelete :1,3 bdelete      → 删除或者指定范围的缓冲区
{% endhighlight %}

查看缓冲区时，在这个状态列表中，前面的数字是缓冲区的数字标记，第二个标记就是缓冲区当前的状态，紧接着是与缓冲区所关联的文件名。有如下几种状态：

{% highlight text %}
    - （非活动的缓冲区）
    a （激活缓冲区）
    h （隐藏的缓冲区）
    % （当前的缓冲区）
    # （交换缓冲区）
    = （只读缓冲区）
    + （已经更改的缓冲区）
{% endhighlight %}

在删除缓冲区时，如果缓冲区被改动过，那么该命令将失败，除非使用 `!` 选项。如果使用了带 `!` 选项的 `:bdelete! filename` 命令，那么在缓冲区中的所有改动都会被放弃。

### 折叠

{% highlight text %}
: help folding
: help usr_28

: set foldenable
{% endhighlight %}

当进入插入模式，将会自动打开折叠，也可通过 ```:nnoremap <space> za``` 定义通过空格展开/关闭。

主要有如下几种方式，当进入非 manual 时，所有的折叠将会删除重建，反之则不会。

#### Marker Fold 标记折叠

{% highlight text %}
:set foldmethod=marker
vim: foldmarker={,} foldlevel=0 foldmethod=marker :
{% endhighlight %}

可以精确地定义折叠，折叠的标记通过 ```foldmaker``` 定义，默认以 \{\{\{ 开始，以 \}\}\} 结束，同时在标记后面可以添加数字表示折叠的层级，通过 ```foldlevel``` 设置级别，超过默认值则会折叠。


<!--
#### Manual Fold 手动折叠

{% highlight text %}
:set foldmethod=manual
{% endhighlight %}

在可视化模式下，通过命令 zf ，将折叠选中的文本；通过组合使用移动命令折叠指定行，如，zf70j(折叠光标之后的70行)、5zF(当前行及随后 4 行折叠)；zf7G(当前行至全文第 7 行折叠)、zfa[(折叠括号(如()、[]、{}、><等)包围的区域)。<br><br>

vim 不会自动记忆手工折叠，可以通过命令 :mkview 来保存当前的折叠状态；下次打开文档时，使用命令 :loadview 来载入记忆的折叠信息；使用命令 :help fold-manual 查看关于手工折叠的帮助信息。

#### Indent Fold 缩进折叠

{% highlight text %}
:set foldmethod=indent
{% endhighlight %}

，相同缩进距离的行构成折叠。<br>

对于已经格式化好的语言，如 Python 比较有用。此时所有文本将按照（选项shiftwidth定义的）缩进层次自动折叠，使用zr打开，zm关闭。zm和zr实际控制的是 set foldlevel=N，zr增加，zm减小，0时关闭所有折叠，1时打开一层折叠。可以设置foldignore='#'来忽略对一些行的折叠，仅用于该选项。可以使用以下命令，查看关于缩进折叠的帮助信息：:help fold-indent</li><br><li>

#### Syntax Fold 语法折叠

:set foldmethod=syntax


所有文本将按照语法结构自动折叠。可以使用以下命令，查看关于语法折叠的帮助信息：:help fold-syntax</li><br><li>

Expression Fold, :set foldmethod=expr，表达式折叠<br>
通过 'foldexpr' 给出每行的折叠级别。如<br>
set foldexpr=strlen(substitute(substitute(getline(v:lnum),'\\s','',\"g\"),'[^>].*','',''))<br>
上面是对>开头（如邮件）的折叠，其中 getline(v:lnum)：获得当前行；substitute(...,'\\s','','g')：删除所有的空白字符；substitute(...,'[^>].*','','')：删除>之后的所有内容；strlen(...)：计算长度，也就是>的个数。


zfap 创建一个折叠。当我们在折叠处左右移动或者跳转(如0)时，折叠会自动打开； set foldopen=all 使光标在当前时自动打开，注意此时光标不能移动到关闭的折叠，因此经常临时使用，可以使用 set foldopen& 恢复默认；使用 set foldclose=all 当光标移动之后自动关闭。<br><br>

注意折叠只对本窗口有效，因此可以对同一个缓冲区打开两个窗口，一个带有折叠，另一个没有。<br><br>

当关闭时折叠不会保存，因此应该在关闭之前使用 mkview 此时会保存包括 folder 在内的影响 view 的所有内容；再次打开时使用 loadview 加载。最多可以保存10个，可以使用 mkview 3 或 loadview 2 。
-->

#### 常用命令

{% highlight text %}
zo  打开当前的折叠，O-pen a fold.
zO  打开当前所有的折叠，O-pen all fold.
zr  打开所有折叠，R-educe the folding.
zR  打开所有折叠及其嵌套的折叠

zc  关闭当前打开的折叠，C-lose a fold.
zC  关闭当前所有打开的折叠，C-lose all fold.
zm  关闭所有折叠，folds M-ore.
zM  关闭所有折叠及其嵌套的折叠

za  关闭、打开相互切换

zd  删除当前折叠，对于标记则会自动删除，D-elete a fold
zD  删除当前所有折叠，D-elete all fold

zj  移动至下一个折叠
zk  移动至上一个折叠

zn  禁用折叠
zN  启用折叠
zi  在上述两者之间切换
{% endhighlight %}

<!--
zf  关闭所有折叠，F-old creation.
zE  删除所有折叠
-->


### 高亮显示

{% highlight text %}
:help syntax
:help usr_44.txt
{% endhighlight %}

Vim 会根据 ```$VIMRUNTIME/syntax/language.vim``` 中的配置内容，自动识别关键字、字符串以及其他语法元素，并以不同的颜色显示出来。

{% highlight text %}
:syntax [enable|clear]          → 启用|关闭语法高亮度，只在当前文件中有效
:syntax [off|on]                → 同上，会对所有缓冲区中的文件立刻生效
:set filetype=c                 → 如果VIM无法识别，则设置文件类型
{% endhighlight %}

高亮显示主要通过两步来实现：A) 首先，确定需要格式化的字符；B) 然后，根据配色方案决定如何显示这些字符。

其中经典的是 [solarized](http://ethanschoonover.com/solarized) ，一个配色方案在 github 就有 4K+ 的 Star ，可通过如下方式配置：

{% highlight text %}
syntax enable
if has('gui_running')
    set background=light
else
    set background=dark
endif
colorscheme solarized
{% endhighlight %}

还有一种很受欢迎的配色方案 [Molokai](https://github.com/tomasr/molokai)，它是 Mac 上 TextMate 编辑器的一种经典配色。

<!--
如可使用以下命令，将所有 FIX 和 ENDFIX 关键字显示为特定颜色。

:syntax match cscFix "FIX\|ENDFIX"              // 创建名为 cscFix 的匹配模式
:highlight cscFix ctermfg=cyan guifg=#00FFFF    // 青色显示匹配的文本

注释的开头到结尾，可用于识别代码注释。

:syntax region myComments start=/\/\*/ end=/\*\//
:syntax keyword myToDo FIXME TODO
:syntax region myComments start=/\/\*/ end=/\*\// contains=myToDo

https://github.com/altercation/vim-colors-solarized
-->

### 自动缩进和对齐

可以在配置文件中添加如下内容。

{% highlight text %}
set autoindent         " 设置自动缩进
set smartindent        " 对autoindent进行了一些改进

set shiftwidth=4       " 自动缩进所使用的空白长度
set tabstop=4          " 定义tab所等同的空格长度
set softtabstop=4      " 详见如下的解释

set expandtab          " 将TAB自动替换为空格

set listchars=tab:▸\ ,trail:-,extends:>,precedes:<,eol:¬    " 设置不可见字符的显示方式
set nolist             " 不显示TAB、空格、回车等不可见字符

filetype indent on     " 可以通过如下的设置，根据文件类型自动进行设置
autocmd FileType python setlocal expandtab smarttab shiftwidth=4 softtabstop=4
{% endhighlight %}

#### autoindent/smartindent/cindent

当在 ```insert``` 状态用回车新增一个新行，或者在 ```normal``` 状态用 ```o/O``` 插入一个新行时，会根据配置的不同模式，进行不同的缩进。

在 ```autoindent``` 模式中，会自动将当前行的缩进拷贝到新行，也就是 "自动对齐"，当然了，如果你在新行没有输入任何字符，那么这个缩进将自动删除。

```smartindent``` 对 ```autoindent``` 进行了一些改进，可以识别一些基本的语法。

```cindent``` 会按照 C 语言的语法，自动地调整缩进的长度，可以与上述配置共存。比如，当输入了半条语句然后回车时，缩进会自动增加一个 ```TABSTOP``` 值，当你键入了一个右花括号时，会自动减少一个 ```TABSTOP``` 值。

另外，可以通过 ```indentexpr``` 设置不同的模式，在此不详述，详见 [vim reference manual](http://man.chinaunix.net/newsoft/vi/doc/indent.html) 。

#### 缩进宽度设置

如上配置的第二部分，就是所使用的缩进模式，如下主要介绍下 ```softtabstop``` 的含义。

当 ```shiftwidth/tabstop``` 不同时，会导致程序对齐很难看，这时可以使用 ```softtabstop```；此时，当按下 ```TAB``` 键，插入的是空格和 ```TAB``` 制表符的混合，具体如何混合取决于你设定的 ```softtabstop``` 。

举个例子，如果设定 ```softtabstop=4``` ，那么按下 ```tab``` 键，插入的就是正常的一个制表符；如果设定 ```softtabstop=8``` ，那么插入的就是两个制表符；如果 ```softtabstop=10``` ，那么插入的就是一个制表符加上 2 个空格；如果 ```softtabstop=2``` 呢？

开始插入的就是 2 个空格，此时一旦你再按下一次 tab ，这次的 2 个空格就会和上次的 2 个空格组合起来变成一个制表符。换句话说，```softtabstop``` 是 “逢 4 空格进 1 制表符” ，前提是 ```tabstop=4``` 。

#### TAB 和空格替换

```:set expandtab/noexpandtab``` 使用空格替换TAB/不进行替换。对于已保存的文件，可以使用下面的方法进行空格和TAB的替换。

{% highlight text %}
----- TAB替换为空格
:set ts=4
:set expandtab
:%retab!

----- 空格替换为TAB
:set ts=4
:set noexpandtab
:%retab!
{% endhighlight %}

加 ```!``` 用于处理非空白字符之后的 ```TAB```，即所有的 ```TAB```，若不加 ```!```，则只处理行首的 ```TAB``` 。

假设配置文件中使用了 ```set expandtab```，如果想要输入 ```TAB```，Linux 下使用 ```Ctrl-V + TAB```，Win 下使用 ```Ctrl-Q + TAB``` 。

#### 设置对齐方式

通过如下方式设置 C 语言的缩进方式，具体配置可查看 [Vim documentation: indent](http://vimdoc.sourceforge.net/htmldoc/indent.html) 。

{% highlight text %}
set cinoptions={0,1s,t0,n-2,p2s,(03s,=.5s,>;1s,=1s,:1s
{% endhighlight %}

#### 常用命令

{% highlight text %}
----- 执行缩进，前面可以加数字进行多节缩进
>>               # Normal模式下，增加当前行的缩进
<<               # Normal模式下，减少当前行的缩进
CTRL+SHIFT+T     # Insert模式下，增加当前行缩进
CTRL+SHIFT+D     # Insert模式下，减小当前行缩进
=                # Visual模式下，对选中的部分进行自动缩进

: set list       # 查看不可见字符，包括TAB、空格、回车等
{% endhighlight %}

<!-- ]p可以实现p的粘贴功能，并自动缩进。 -->

### 文件类型检测

Vim 会根据不同的文件类型，分别设置高亮、缩进等功能。可以通过 ```:filetype``` 查看 Vim 的文件类型检测功能是否已打开，通常有三个选项 ```detection:ON plugin:ON indent:ON``` 。

* detection，是否自动检测文件类型。
* plugin，是否自动加载该类型相关的插件。
* indent，根据文件类型定义缩进方式，通常在安装目录的 indent 目录下定义缩进相关脚本。

当配置文件中添加了 ```filetype on``` 命令时，实际上会调用 ```$VIMRUNTIME/filetype.vim``` 脚本检测文件的类型。

{% highlight text %}
:filetype [detection|plugin|indent] on  → 设置开关
:filetype plugin indent on              → 同上，也可以在一行中设置
:filetype [off|on]                      → 关闭/打开
:set filetype                           → 查看当前文件类型
:set filetype=python                    → 设置类型
{% endhighlight %}

在设置文件类型时，会同时触发 `FileType` 自动命令；从而，可以用下面的命令实现，根据不同文件类型设置不同的配置项。

{% highlight text %}
autocmd FileType c,cpp set shiftwidth=4 | set expandtab
{% endhighlight %}

## 高级进阶

### 区域选择

{% highlight text %}
:help object-motions
:help text-objects
{% endhighlight %}

`Text Objects Commands` 或许这个是最强大的命令了。

{% highlight text %}
<number><command><text object or motion>
  <number>     操作的重复次数；
  <command>    命令，如chang(c), delete(d), yank(y)，默认是选择，如果不指定则只移动；
  <text object or motion>
               可以是指定文本，例如word、sentence、paragraph；或者是动作，如前进一行、行尾等。

Old text                  Command     New text
the t*ext object          daw         the object     删除Word，同时删除空格
the t*ext object          diw         the  object    删除Word，不删除空格
the t*ext object          dw          the tobject    删除Word，从开始位置删除，含空格
<h2>Sa*mple</h2>          dit         <h2></h2>      删除tag之间的内容
<div *id="cont"></div>    di>         <></div>       删除单个tag的内容
{% endhighlight %}

除了上面的 `Word` 外，还可以通过如下方式指定范围。

{% highlight text %}
aw(A Word)        iw(Inner Word)        前者包含了区分子的空格，后者不包含；
as(A Sentence)    is(Inner Sentence)    通过.分割，作为一个句子；
as(A Block)       is(Inner Block)       也就是括号
ap(A Paragraph)   ip(Inner Paragraph)   通过空白行分割；
at(A Tag Block)   it(Inner Tag Block)   使用tag，例如HTML中的tag
a>(A Single Tag)  i>(Inner Single Tag)  在单个的tag内
{% endhighlight %}

除了上述的定义外，还可以通过 `a)` `i)` `a]` `i]` `a}` `i}` `a'` `i'` `a"` `i"` 还有反问号 ( \` ) ，这类操作与上述的 `Word` 操作相同。另外，`%` 可以在括号之间切换，其中 `c%` 和 `ca)` 等价。

需要注意 `Motion Commands` 和 `Text Objects Commands` 的区别，前者如 `cw` 是从当前的位置操作，而像 `ciw` 则处理的是光标所在的整个 `Word` 。

<!--
在 visual 模式下，这些命令很强大，其命令格式为 ```<action>a<object>``` 和 ```<action>i<object>``` ，其中 a 含有空格，i 不含。在 Normal 模式下按 'v' 进入自由选择，'V' 行选择，Ctrl-v 块选择。

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
ci[ ci( ci< ci{      删除一对 [], (), <>, 或{} 中的所有字符并进入插入模式
ci" ci' ci`          删除一对引号字符 " ' 或 ` 中所有字符并进入插入模式
cit                  删除一对 HTML/XML 的标签内部的所有字符并进入插入模式

ci                   如 ci(，或者 ci)，将会修改 () 之间的文本；
di                   删除配对符号之间文本；
yi                   复制；
ca                   同ci，但修改内容包括配对符号本身；
da                   同di，但剪切内容包括配对符号本身；
ya                   同yi，但复制内容包括配对符号本身。
{% endhighlight %}

另外，`dib` 等同于 `di(`，`diB` 等同于 `di{` 。

### 宏录制

通过 `q` 进入宏录制模式，操作为 A) 录制，`qa do_something q`；B) 使用 `@a`, `@@`。`qa` 会把操作记录在寄存器 `a`；`@a` 会重做被录制的宏，就好象自己在输入一样；`@@` 是一个快捷键，用来执行最近被执行的宏。

如，在一个只有一行且这一行只有 `"1"` 的文本中，键入如下命令:

{% highlight text %}
qaYp<C-a>     → qa开始录制；Yp复制行；<C-a>增加1；q停止录制
@a            → 在1下面写下2
@@            → 在2正面写下3
100@@         → 会创建新的100行，并把数据增加到103
{% endhighlight %}

### 块操作

在 Normal 模式下，通过 `Ctrl-v` 执行操作，如在指定的行首添加 `"--"` 字符串，其执行命令顺序为 `0 <C-v> <C-d> I-- [ESC]`，解释如下：

{% highlight text %}
0             → 到行头
<C-v>         → 开始块操作
<C-d>         → 向下移动，也可以使用hjkl来移动光标
I-- [ESC]     → I是插入，插入"--"，按ESC键来为每一行生效
{% endhighlight %}

行后添加字符的执行顺序如下。

{% highlight text %}
<C-v>       → 视图模式
<C-d>       → 选中相关的行，也可以使用j，或是/pattern或是%等
$           → 到行最后
A           → 输入字符串，按ESC
{% endhighlight %}

在 Windows 下的 vim ，需要使用 `<C-q>` 而不是 `<C-v>`，`<C-v>` 是拷贝剪贴板。

### 可视化选择

常见的操作包括了 `v`、`V`、`<C-v>` 等视图模式下，在 Windows 下应该是 `<C-q>`，选好之后，可以做下面的事。

{% highlight text %}
J           → 把所有的行连接起来，变成一行
< 或 >      → 左右缩进
=           → 自动缩进
{% endhighlight %}

### 其它

#### 重复执行命令

{% highlight text %}
.                     → 重复上一次的命令
<NUM><command>        → 重复某个命令N次
2dd                   → 删除2行
100idesu [ESC]        → 会写下100个"desu "
{% endhighlight %}

#### 模式行

可以通过 `:help modeline` 查看帮助，在文件首、尾的若干行 (modelines默认5行) 添加的配置内容，可以用来指明这个文件的类型，以及一些其它的相关配置项。

{% highlight text %}
# vim: filetype=python
/* vim: filetype=java */
# vim: foldmarker={,} foldlevel=0 foldmethod=marker :
{% endhighlight %}

#### 其它

{% highlight text %}
:set cc=80         → 高亮显示第80列
: shell            → 切换到shell执行命令，退出后返回到VIM
: messages         → 出现错误时可以通过该命令查看错误信息
{% endhighlight %}

## 常用插件

<!--
导航与搜索，taglist 和 tagbar 类似，不过其关注点有所区别，后者比较适合面向对象
  1. NERDTree - file navigation
  2. CtrlP    - fast file finder
  3. Taglist  - source code browser
  4. Tagbar   - tag generation and navigation

自动补全
  1. YouCompleteMe - visual assist for vim
  2. UltiSnips     - ultimate snippets
  3. Zen Coding    - hi-speed coding for html/css

语法
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

通常可以使用 vundle 管理所有的插件，通常插件为了防止多次加载，会在开始的时候检测是否已经加载。

一些经典的配置可以参考 [vim.spf13.com](http://vim.spf13.com/)，该 vim 配置，在 [github](https://github.com/spf13/spf13-vim) 中有 1W+ 的 Star，可以通过如下方式进行配置：

{% highlight text %}
----- 直接下载安装，实际上就是github中的bootstrap.sh脚本
$ curl https://j.mp/spf13-vim3 -L -o - | sh
{% endhighlight %}



### Vundle

<!-- 可以使用 vim plug -->

vim 缺少默认的插件管理器，所有插件的文件都散布在 ```~/.vim``` 下的几个文件夹中，无论是配置、更新、删除，都需要手动配置，很容易出错。

vundle 把 git 操作整合进去，用户需要做的只是去 Github 上找到自己想要的插件的名字，安装，更新和卸载都可有 vundle 来完成了，可以查看 [github](https://github.com/gmarik/vundle)，可以通过如下方式安装。

{% highlight text %}
$ git clone https://github.com/gmarik/vundle.git ~/.vim/bundle/vundle
{% endhighlight %}

vim-scripts 在配置时，可以直接打仓库名，相关的插件可以从 [官网](http://vim-scripts.org/index.html) 查看，或查看 [Github](https://github.com/vim-scripts)，此时可以直接使用 ```Bundle 'L9'``` 。在配置文件中，主要是配置安装哪些插件，Vundle 支持如下的格式：

{% highlight text %}
Bundle 'gmarik/vundle'          " 使用Vundle来管理Vundle，这个必须要有
Bundle 'tpope/vim-fugitive'     " Github上其他用户的仓库，非vim-scripts账户里的仓库
Bundle 'file:///path/to/plugin' " 使用自己的插件
Bundle 'git://vim-latex.git.sourceforge.net/gitroot/vim-latex/vim-latex'
{% endhighlight %}

自己写的插件也可以通过如上方式管理，如自己写了一个 test 插件，并放到了 ```.vim/myplugin``` 目录中，那么可以在 vim 的配置文件中加入下面的命令 ```set rtp+=~/.vim/myplugin/``` 。

Vundle 常用的命令如下。

{% highlight text %}
BundleList             列出所有已配置的插件
BundleInstall(!)       下载(更新)插件
BundleUpdate           更新插件
BundleClean            清除不再使用的插件
BundleSearch(!) foo    查找(先刷新cache)foo
{% endhighlight %}

对于 Vundle 插件，如果使用 `call vundle#begin()` 时发现很多插件无法使用，可以使用 `call vundle#rc()` ，暂时不确认为什么。

### Tagbar

源码可以从 [github tagbar](http://majutsushi.github.io/tagbar/) 中查看，帮助文档可以查看 [doc/tagbar.txt](https://github.com/majutsushi/tagbar/blob/master/doc/tagbar.txt) 。

### Tabular

一个不错的对齐用的插件，可以从 [tabular](http://www.vim.org/scripts/script.php?script_id=3464) 下载，然后通过如下的方式安装，详细的资料可以参考 [aligning text with tabular vim](http://vimcasts.org/episodes/aligning-text-with-tabular-vim/) 。

{% highlight text %}
$ git clone https://github.com/godlygeek/tabular.git
$ cd tabular/
$ mv after autoload doc plugin ~/.vim/
{% endhighlight %}

首先，通过 V v Ctrl-v 选取需要对其的内容，然后直接输入如下的命令即可。注意，在 Visual 模式中输入 ```:``` 后，会显示 ```:'<,'>``` 直接输入命令即可。

{% highlight text %}
:Tabularize /=           使用=进行分割
:Tabularize /:\zs        ':'符号不动，只对其':'后面的字符
{% endhighlight %}

其中 Tabularize 可以简写为 Tab ；另外，tabular 可以自动识别，尤其是含有 {}()[] 的，因此可以不选择字符，也就是在某行中直接输入 ```:Tabularize /|``` 即可。

可以通过如下的示例进行测试。

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

对于第二部分，执行 ```vi}``` 选择区域，执行 ```:Tab /:``` ，执行 ```:Tab /:\zs``` 则会使 : 不变。

{% highlight text %}
$ vim ~/.vimrc                              # 在最后添加如下的内容
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

上述前一部分表示绑定的快捷键<!--，if语句使用时出错?????????????-->；后一部分表示在输入时会自动检测，并设置格式。

### CtrlP

![vim CtrlP]({{ site.url }}/images/misc/vim-ctrl-p-buttons.png "vim CtrlP"){: .pull-center}

一个强大的搜索插件，可以模糊查询定位，包括了工程下的所有文件、打开的 Buffer、Buffer 内的 tag、最近访问的文件等，极大了方便了大规模工程代码的浏览。

<!-- 据说还有一个 CopyCat ，暂时没有找到怎么使用 -->

### UltiSnips

一个和牛摆的模版，在写代码时经常需要在文件开头加一个版权声明之类的注释，又或者在头文件中要需要 `#ifndef... #def... #endif` 这样的宏，亦或写一个 `for` `switch` 等很固定的代码片段。

该工具和 YouCompleteMe 以及 neocomplete 都很好的整合在一起了，不过需要编写模版，很多模版可以参考 [honza/vim-snippets](https://github.com/honza/vim-snippets)。

<!-- http://vimcasts.org/episodes/meet-ultisnips/ -->

### YouCompleteMe

当通过 `Plugin 'Valloric/YouCompleteMe'` 安装 YCM 后，经常会出现 `no module named future` 的报错，可以通过如下方式进行安装。

{% highlight text %}
----- 可以通过Vundle安装，或使用如下方式下载，后者用于下载相关的依赖
$ git clone --recursive https://github.com/Valloric/YouCompleteMe.git ~/.vim/bundle/YouCompleteMe
$ git submodule update --init --recursive

----- 通过该脚本安装，支持C，如果要支持所有语言通过--all安装
$ ./install.py --clang-completer
{% endhighlight %}

在安装时，可以通过 `--system-clang` 指定使用系统的 Clang 。 <!-- --system-libclang -->

另外，UltiSnips 与 YCM 有按键冲突，很多都建议将 UltiSnips 的自动填充快捷键更换，不过仍无效，可以通过如下方式修改，此时 `<C-N>` 和 `<C-P>` 仍然有效。

{% highlight text %}
let g:ycm_key_list_select_completion=[]
let g:ycm_key_list_previous_completion=[]
{% endhighlight %}

常用命令。

{% highlight text %}
:YcmDiags          通过location-list显示诊断信息
{% endhighlight %}

<!--
在 Centos7 中可以通过 `yum install clang` 安装，不过依赖 epel 镜像库
使用 vundle 下载源码，在 vimrc 文件中加入 Bundle 'Valloric/YouCompleteMe'，并通过如下命令安装。
-->

<!--
    " 自动补全配置
    autocmd InsertLeave * if pumvisible() == 0|pclose|endif "离开插入模式后自动关闭预览窗口
    inoremap <expr> <CR>       pumvisible() ? "\<C-y>" : "\<CR>"    "回车即选中当前项
    "上下左右键的行为 会显示其他信息
    inoremap <expr> <Down>     pumvisible() ? "\<C-n>" : "\<Down>"
    inoremap <expr> <Up>       pumvisible() ? "\<C-p>" : "\<Up>"
    inoremap <expr> <PageDown> pumvisible() ? "\<PageDown>\<C-p>\<C-n>" : "\<PageDown>"
    inoremap <expr> <PageUp>   pumvisible() ? "\<PageUp>\<C-p>\<C-n>" : "\<PageUp>"

    "youcompleteme  默认tab  s-tab 和自动补全冲突
    "let g:ycm_key_list_select_completion=['<c-n>']
    let g:ycm_key_list_select_completion = ['<Down>']
    "let g:ycm_key_list_previous_completion=['<c-p>']
    let g:ycm_key_list_previous_completion = ['<Up>']
    let g:ycm_confirm_extra_conf=0 "关闭加载.ycm_extra_conf.py提示

    let g:ycm_collect_identifiers_from_tags_files=1 " 开启 YCM 基于标签引擎
    let g:ycm_min_num_of_chars_for_completion=2 " 从第2个键入字符就开始罗列匹配项
    let g:ycm_cache_omnifunc=0  " 禁止缓存匹配项,每次都重新生成匹配项
    let g:ycm_seed_identifiers_with_syntax=1    " 语法关键字补全
    "nnoremap <leader>lo :lopen<CR> "open locationlist
    "nnoremap <leader>lc :lclose<CR>    "close locationlist
    inoremap <leader><leader> <C-x><C-o>
    let g:ycm_collect_identifiers_from_comments_and_strings = 0

    nnoremap <leader>jd :YcmCompleter GoToDefinitionElseDeclaration<CR> " 跳转到定义处-->

#### 配置文件

YCM 中需要在 flags 中添加编译时的参数定义，例如 `-Wall`、`-Wextra` 等，然后通过 `-isystem` 指定系统的头文件，通过 `-I` 指定自定义的头文件。

注意，在使用相对路径时是 YCM 的安装路径。

#### YCM错误日志及调试信息

在 vim 配置文件中加上下面的选项，打开调试信息。

{% highlight text %}
let g:ycm_server_keep_logfiles = 1
let g:ycm_server_log_level = 'debug'
{% endhighlight %}

几个常用命令:

{% highlight text %}
:YcmToggleLogs   查看到错误日志及调试信息，输出到哪些文件中
:YcmDebugInfo    可以查看ycm相关的信息,包括编译标志,版本等等
:YcmDiags        查看当前文件中的错误信息
{% endhighlight %}

然后调试时就可以通过 print 打印，然后输出到 stdout 指定的文件中。

#### 常见问题

对于 `Your C++ compiler does NOT support C++11` 错误，是由于要求最低 `gcc 4.9` 版本，该功能会在 `third_party/ycmd/cpp/CMakeLists.txt` 文件中进行检查，也就是 `set( CPP11_AVAILABLE false )` 定义。

<!--
配置文件参考示例
https://github.com/rasendubi/dotfiles/blob/master/.vim/.ycm_extra_conf.py
https://github.com/robturtle/newycm_extra_conf.py/blob/master/ycm.cpp.py
-->

### Syntastic

一个语法检查工具，支持多种语言，提供了基本的补全功能、自动提示错误的功能外，还提供了 tags 的功能；采用 C/S 模式，当 vim 关闭时，ycmd 会自动关闭。

不过对于不同的语言需要安装相应的插件，详细内容可以查看 [doc/syntastic-checkers.txt](https://raw.githubusercontent.com/vim-syntastic/syntastic/master/doc/syntastic-checkers.txt)，安装方法可以参考 README.md 文件。

![vim Syntastic]({{ site.url }}/images/misc/vim-syntastic-screenshot.png "vim Syntastic"){: .pull-center width='90%' }

能够实时的进行语法和编码风格的检查，还集成了静态检查工具，支持近百种编程语言，像是一个集大成的实时编译器，出现错误之后，可以非常方便的跳转到出错处。

另外，是一个 [Asynchronous Lint Engine](https://github.com/w0rp/ale) 一个异步的检查引擎。

### NERDCommenter

用于快速，批量注释与反注释，适用于任何你能想到的语言，会根据不同的语言选择不同的注释方式，方便快捷。

### NERDTree

[NERDTree](https://github.com/scrooloose/nerdtree) 的作用就是列出当前路径的目录树，类似与一般 IDE，可以方便的浏览项目的总体的目录结构和创建删除重命名文件或文件名。

另外还有一个 NERDTreeTabs 插件，可以以 tab 形式显示窗口，用于方便浏览目录以及文件。

{% highlight text %}
map <F2> :NERDTreeToggle<CR>          " 使用F2键快速调出和隐藏它
autocmd vimenter * NERDTree           " 打开vim时自动打开NERDTree
{% endhighlight %}

如下是一些常用的命令：

{% highlight text %}
t   以Tab形式打开文件
i   分割成上下两个窗口显示文件
s   分割成左右两个窗口显示文件

C   将该目录设置为根目录
P   把有表移动到该目录的根目录
K   把有表移动到该目录的第一个
J   把有表移动到该目录的最后一个

p   切换到目录的下一层
u   切换到目录的上一层

m   通过NERDTree的选择菜单进行操作
I   是否显示隐藏文件切换
q   关闭NERDTree窗口
?   打开/关闭帮助指令
{% endhighlight %}

另外，比较好用的是提供的标签操作。

<!--
书签操作，以下命令只在在Nerdtree的buffer中有效。

:Bookmark name 将选中结点添加到书签列表中，并命名为name（书签名不可包含空格），如与现有书签重名，则覆盖现有书签。</li><li>
:OpenBookmark name  打开指定的文件。</li><li>
:ClearBookmarks name 清除指定书签；如未指定参数，则清除所有书签。</li><li>
:ClearAllBookmarks 清除所有书签。</li><li>
:ReadBookmarks 重新读入'NERDTreeBookmarksFile'中的所有书签。</li></ul>
:BookmarkToRoot 以指定目录书签或文件书签的父目录作为根结点显示NerdTree
:RevealBookmark
如果指定书签已经存在于当前目录树下，打开它的上层结点并选中该书签

下面总结一些命令，原文地址<a href="http://yang3wei.github.io/blog/2013/01/29/nerdtree-kuai-jie-jian-ji-lu/">http://yang3wei.github.io/blog/2013/01/29/nerdtree-kuai-jie-jian-ji-lu/</a>
<pre style="font-size:0.8em; face:arial;">
o       在已有窗口中打开文件、目录或书签，并跳到该窗口
go      在已有窗口中打开文件、目录或书签，但不跳到该窗口
i       split 一个新窗口打开选中文件，并跳到该窗口
gi      split 一个新窗口打开选中文件，但不跳到该窗口
s       vsplit 一个新窗口打开选中文件，并跳到该窗口
gs      vsplit 一个新 窗口打开选中文件，但不跳到该窗口
O       递归打开选中 结点下的所有目录
x       合拢选中结点的父目录
X       递归 合拢选中结点下的所有目录
t       在新 Tab 中打开选中文件/书签，并跳到新 Tab
T       在新 Tab 中打开选中文件/书签，但不跳到新 Tab
</pre>
Nerdtree当打开多个标签时，会导致新打开的标签没有nerdtree窗口，此时可以使用nerdtree-tabs，将上述定义的快捷键设置为NERDTreeTabsToggle即可。
!       执行当前文件
e       Edit the current dif

双击    相当于 NERDTree-o
中键    对文件相当于 NERDTree-i，对目录相当于 NERDTree-e

D       删除当前书签

P       跳到根结点
p       跳到父结点
K       跳到当前目录下同级的第一个结点
J       跳到当前目录下同级的最后一个结点
k       跳到当前目录下同级的前一个结点
j       跳到当前目录下同级的后一个结点

C       将选中目录或选中文件的父目录设为根结点
u       将当前根结点的父目录设为根目录，并变成合拢原根结点
U       将当前根结点的父目录设为根目录，但保持展开原根结点
r       递归刷新选中目录
R       递归刷新根结点
m       显示文件系统菜单 #！！！然后根据提示进行文件的操作如新建，重命名等
cd      将 CWD 设为选中目录

I       切换是否显示隐藏文件
f       切换是否使用文件过滤器
F       切换是否显示文件
B       切换是否显示书签

:tabnew [++opt选项] ［＋cmd］ 文件      建立对指定文件新的tab
:tabc   关闭当前的 tab
:tabo   关闭所有其他的 tab
:tabs   查看所有打开的 tab
:tabp   前一个 tab
:tabn   后一个 tab

标准模式下：
gT      前一个 tab
gt      后一个 tab

MacVim 还可以借助快捷键来完成 tab 的关闭、切换
cmd+w   关闭当前的 tab
cmd+{   前一个 tab
cmd+}   后一个 tab
-->

### Surround

一个专门用来处理这种配对符号的插件，它可以非常高效快速的修改、删除及增加一个配对符号，对于前端工程师非常有用。

通常可以和 [repeat.vim](http://www.vim.org/scripts/script.php?script_id=2136) 配合使用，如下是常见的操作示例。

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

如下是一个命令列表。

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

主题在目录 `autoload/Powerline/Themes` 下，配色在 `autoload/Powerline/Colorschemes`，`vim-airline` 与 `vim-powerline` 类似但是更小，更轻便。

推荐使用后者，可以参考 [vim-airline](https://github.com/vim-airline/vim-airline)，其中主题 themes 已经单独拆出来了。

### tags

tags 记录了关于一个标识符在哪里被定义的信息，比如 C/C++ 程序中的一个函数定义。vim 默认是支持 tags 的，那么可以让 vim 从任何位置跳转到相应的标示位置。

{% highlight text %}
ctags -R --c++-kinds=+px --fields=+iaS --extra=+q .

常用参数：
-R
   遍历循环子目录生成tags；
--fields=+iaS
  将可用扩展域添加到tags中，
    i) 如有继承，则标识出父类；
    a) 标明类成员的权限，如public、private等；
    S) 函数的信息，如原型、参数列表等；
-I identifier-list
   通常用于处理一些宏，如果只列出了那么则会忽略；
--c++-kinds=+px
   记录c++文件中的函数声明和各种外部和前向声明，使用p时同时也会添加extern的声明；
--extra=+q
  是否将特定信息添加到tags中，q) 类成员信息，包括结构体；

其它常用命令：
----- 列举出当前支持的语言，也可以自定义，具体没研究过
$ ctags --list-languages
----- 查看扩展名跟语言的映射关系，从而可以使用正确的分析器
$ ctags --list-maps
----- 可识别的语法元素，默认打印所有，在生成时可以通过--c-kinds指定
$ ctags --list-kinds=c

生成的文件格式如下：
{tagname} {TAB} {tagfile} {TAB} {tagaddress} {term} {field} ..
   {tagname}     标识符名字，例如函数名、类名、结构名、宏等，不能含TAB符。
   {tagfile}     包含 {tagname} 的文件。
   {tagaddress}  可以定位到 {tagname} 光标位置的 Ex 命令，通常只包含行号或搜索命令。
                 出于安全的考虑，会限制其中某些命令的执行。
   {term}        设为 ;” ，主要是为了兼容vi编辑器，使vi忽略后面的{field}字段。
   {field} ..    也就是扩展字段，可选，用于表示此 {tagname} 的类型是函数、类、宏或是其它。

常见快捷键如下：
Ctrl+]       跳转到定义处；
Ctrl+T       跳转到上次tags处；
Ctrl+i       (in)跳转下一个；
Ctrl+o       (out)退回原来的地方；
gd           转到当前光标所指的局部变量的定义；
gf           打开头文件；
:ju          显示所有可以跳转的地方；
:set tags    查看加载的tags；
:tag name    调转到name处；
:stag name   等价于split+tag name
:ta XXX      跳转到符号XXX定义处，如果有多个符号，直接跳转到第一处；
:ts XXX      列出符号XXX的定义；
:tj XXX      可看做上面两个命令的合并，如果只找到一个符号定义，那么直接跳转，有多个，则让用户自行选择；
:ptag name   预览窗口显示name标签，光标跳到标签处；
Ctrl-W + }   预览窗口显示当前光标下单词的标签，光标跳到标签处；
:pclose      关闭预览窗口；
:pedit file.h 在预览窗口中编辑文件file.h，在编辑头文件时很有用；
:psearch atoi 查找当前文件和头文件中的单词并在预览窗口中显示匹配，在使用没有标签文件的库函数时十分有用。
{% endhighlight %}

如果有多个可以使用 `tfirst` `tlast` `tprevious` `tnext` `tselect` 选择，也可以 `:tag name_<TAB>` 自动补全，或者使用 `tselect /^write` 正则表达式。

#### 生成系统tags

{% highlight text %}
----- 添加系统的tags
$ ctags --fields=+iaS --extra=+q -R -f ~/.vim/systags /usr/include /usr/local/include
:set tags+=~/.vim/systags
{% endhighlight %}

此时，基本可以跳转到系统函数，不过仍有部分函数未添加到tags中，常见的有如下的示例。

{% highlight c %}
extern int listen (int __fd, int __n) __THROW;
extern int strcmp (__const char *__s1, __const char *__s2)
     __THROW __attribute_pure__ __nonnull ((1, 2));
{% endhighlight %}

也就是因为存在 `__THROW` `attribute_pure` `nonull` 等属性，导致认为上述的声明不是函数，都需要忽略。如果需要 `#if 0` 里面的定义，可以使用 `-if0=yes` 来忽略 `#if 0` 这样的定义。

{% highlight text %}
$ ctags -I __THROW -I __attribute_pure__ -I __nonnull -I __attribute__ \   忽略这里的定义
    --file-scope=yes              \     例如对于static声明只在一个文件中可见
    --langmap=c:+.h               \     定义扩展名和语言的映射关系，可以通过--list-maps查看
    --languages=c,c++             \     使能哪些语言
    --links=yes                   \     是否跟踪符号链接指向的文件
    --c-kinds=+p --c++-kinds=+p   \     指定生成哪些C语言的tag信息
    --fields=+iaS --extra=+q -R -f ~/.vim/systags /usr/include /usr/local/include
{% endhighlight %}

可以在配置文件中添加如下的内容，然后在源码目录内可以通过 `Ctrl-F12` 生成 tags 文件。

{% highlight text %}
map <C-F12> :!ctags -R --c-kinds=+px --fields=+iaS --extra=+q <CR>
{% endhighlight %}

### cscope

{% highlight text %}
:help if_cscop
:cscope show       查看已经加载的数据库
:cscope add        添加数据库
:cscope kill       关闭已加载的数据库
:cscope reset      重新初始化所有连接
{% endhighlight %}

可以将 cscope 看做是 ctags 的升级版本，提供交互式查询语言符号功能，如查询哪些地方使用某个变量或调用某个函数。

和 tags 一样，默认支持，可以通过 `:version` 查看，没有则增加 `--enable-cscope` 编译选项，详细可查看官方网址为 [cscope.sourceforge.net](http://cscope.sourceforge.net/) 。

确定 Vim 已支持 Cscope 后，将文件 
[cscope_maps.vim](http://cscope.sourceforge.net/cscope_maps.vim) 下载到 `~/.vim/plugin` 目录下；也可以将其内容添加到 `~/.vimrc` 配置文件中。

在 CentOS 中，可以通过 `yum install cscope` 安装，在第一次解析时扫描全部文件，以后再调用 cscope 只会扫描那些改动过的文件。

进入项目代码根目录运行命令生成 cscope 数据库文件：

{% highlight text %}
cscope -Rbq -f cscope.out

常用参数：
  -R     表示递归操作，无该参数则进入基于cureses的GUI界面，通过方向键选择查找类
           型、TAB键搜索结果和搜索类型中选择、Ctrl-D退出；
  -b     生成数据库文件后直接退出，否则生成数据库之后会进入 cscope 界面；
  -q     表示生成 cscope.in.out 和 cscope.po.out 文件，加快 cscope 的索引速度；
  -C     在搜索时忽略大小写；
  -Ppath 默认是相对路径，可以在相对路径的文件前加上path。
  -Idir  在指定的目录中查找头文件；
{% endhighlight %}

<!--
-k 在生成数据库时如果在当前项目目录下没有找到头文件，则会自动到/usr/include目录下查找，通过-k选项关闭；
-i 如果不是cscope.files，则通过该选项指定源文件列表，-表示标准输入；
-u 扫描所有文件，重新生成交叉索引文件；
-->

默认只解析 C、lex 和 yacc 文件，如果希望解析 C++ 和 Java 文件，可以通过指定 cscope.files 文件，此时不再需要 -R 选项。

<!--
  然后可以通过 cscope find 进行查找，vim支持8中cscope的查询功能，如下：<ul><li>
     0/s, symbol: 查找C语言符号，即查找函数名、宏、枚举值等出现的地方。</li><li>
     1/g, global: 查找函数、宏、枚举等定义的位置，通常为全局变量，类似ctags所提供的功能。</li><li>
     2/d, called: 查找本函数所调用的函数。</li><li>
     3/c, calls: 查找所有调用本函数的函数。</li><li>
     4/t, text: 查找指定的字符串。</li><li>
     6/e, egrep: 查找egrep模式，相当于egrep功能，但查找速度快多了。</li><li>
     7/f, file: 查找并打开文件，类似vim的find功能。</li><li>
     8/i, includes: 查找包含当前cursor的文件。</li></ul>
 如查找调用do_cscope()函数的函数，可以输入 :cs find c do_cscope 或者使用 :cs find s do_cscope 。</li><br><li>

    </li><br><li>
 Cscope 常用快捷键，此处是通过vimrc配置的<br>
   Ctrl-\ s 查找所有当前光标所在符号出现过位置。<br>
   Ctrl-\ c 查找所有调用当前光标所在函数的函数。<br><br>


find $PROJECT_HOME -name *.c -o -name "*.cpp" -o -name "*.cc" -o -name "*.h" -o -name "*.hpp" > cscope.files
cscope -qbR -i cscope.files


    vim提供了一些选项可以调整它的cscope功能：<ul><li>
        cscopeprg选项用于设置cscope程序的位置。</li><br><li>

        cscopequickfix设定是否使用quickfix窗口来显示cscope的结果，这是一组用逗号分隔的值，每项都包含于csope-find命令（s, g, d, c, t, e, f, 或者i）和旗标（+, -或者0）。<!-- ‘+’预示着显示结果必须追加到quickfix窗口。‘-’隐含着清空先前的的显示结果，’0’或者不设置表示不使用quickfix窗口。查找会从开始直到第一条命令出现。默认的值是””（不使用quickfix窗口）。下面的值似乎会很有用：”s-,c-,d-,i-,t-,e-”。 详情请”:help cscopequickfix“。</li><br><li>

        如果你想vim同时搜索tag文件以及cscope数据库，设置cscopetag选项，此时使用的是cstag而不是tag，默认是关闭的set cst/nocst。</li><br><li>

        cscopetagorder选项决定:cstag是先查找tag文件还是先查找cscope数据库。设置为0则先查找cscope数据库，设置为1先查找tag文件。默认是0，set csto=0/1。</li><br><li>

        cscopeverbose是否显示数据库增加/失败，如 set csverb/nocsverb。</li><br><li>

       ‘cspc’的值决定了一个文件的路径的多少部分被显示。默认值是0，所以整个路径都会被显示。值为1的话，那么就只会显示文件名，不带路径。其他值就会显示不同的部分。例如 :set cspc=3 将会显示文件路径的最后3个部分，包含这个文件名本身。 </li></ul>

    vim的手册中给出了使用cscope的建议方法，使用命令”:help cscope-suggestions“查看。
 </p>
-->

#### 查看Linux Kernel

对于内核中感兴趣的文件通常需要去除文本、非x86源码、非驱动。

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
$ cscope -b -q -k                             生成Cscope数据库文件
$ add some files and 'cscope -b -q -k'        添加新文件重新生成数据库
{% endhighlight %}

在 find 命令中 `-o` 表示 `or`，`-prune` 表示不包含该目录，如果前面又添加了 `!` 号，则表示只包含该目录。

<!--
duplicate cscope database not added
使用Cscope时两次加载cscope，可以通过 grep -rne "cscope.out" 检查 ~/.vim/ 目录和 /etc/ 目录，通常时由于 ~/.vim/plugin
-->

### 其它插件

简单介绍一些常见的插件。

#### xterm-color-table

查看颜色列表，可从 [color-table](http://www.vim.org/scripts/script.php?script_id=3412) 下载，下载解压后可以放置到 ```$VIMRUNTIME/colors``` 目录下，通过 ```:XtermColorTable``` 命令即可查看。

其它可以查看 doc 目录下的帮助文件，也可以查看如下的图片 [xterm color](/images/linux/vim_xterm_color_chart.png) 。

#### number.vim

更好的显示行号，在 Normal 状态时会在主窗口中显示绝对/相对行号，插入状态时显示绝对行号，不需要进行任何配置。

{% highlight text %}
let g:numbers_exclude = ['tagbar', 'gundo', 'minibufexpl', 'nerdtree']
nnoremap <F3> :NumbersToggle<CR>
nnoremap <F4> :NumbersOnOff<CR>
{% endhighlight %}

#### vim-startify

一个启动界面替换默认的 vim 启动界面，会显示一些最近打开的文件等信息。

#### fencview

对于 vim 打开文件时如何自动识别，可以使用如下设置：

{% highlight text %}
set fileencodings=utf-8,gb2312,ucs-bom,euc-cn,euc-tw,gb18030,gbk,cp936
{% endhighlight %}

或者通过 fencview 插件，该插件主要用于自动识别汉字、日文等，主要有如下的命令。

{% highlight text %}
:FencAutoDetect                  " 自动识别文件编码
:FencView                        " 打开一个编码列表窗口，用户选择编码reload文件
{% endhighlight %}

可以在 vimrc 中设置如下选项：

{% highlight text %}
let g:fencview_autodetect = 1    " 打开文件时自动识别编码
let g:fencview_checklines = 10   " 检查前后10行来判断编码，或者'*'检查全文
{% endhighlight %}

可以将 `autodetect` 设置为 0，当发现为乱码时，那么可以使用 `:FencAutoDetect` 命令查看，然后通过 `:set fileencoding` 查看当前编码，并将其编码放置到 `fileencodings` 中。

可以参考 [VIM 文件编码识别与乱码处理](http://edyfox.codecarver.org/html/vim_fileencodings_detection.html) 。

<!-- /reference/linux/vim_fileencoding.maff -->

<!--
Adds file type glyphs/icons to popular Vim plugins
https://github.com/ryanoasis/vim-devicons
-->


#### 常用快捷键

{% highlight text %}
F2    替换末尾的空格
F3    Tagbar(左侧)
F4    Nerdtree(右侧)
{% endhighlight %}


<!--
##############################################################################

### 移动光标 :help motion

其中帮助还可以查看 :help word/cursor-motions/various-motions 命令。

在 VIM 中，光标的移动非常重要，很多命令可以和其进行组合，常见的如 &lt;start position&gt; &lt;command&gt; &lt;end position&gt; ，如果没有指定 &lt;start position&gt; 则默认为当前光标所在位置。

如 0y$ 复制从 0 开始到 $ 结尾，也就是行首到行尾的数据；ye 复制从当前光标到词的结尾的数据；y2/foo 复制当前光标到第二次出现 foo 的数据。不仅 y(yank) 其它的操作如 d(delete)、v(visual select)、c(change)、gU (uppercase，变为大写)、gu (lowercase，变为小写) 等。

### 常规方式

{% highlight text %}
[h|j|k|l]           → 等价于光标键(←↓↑→)
35j                 → 向下调转35行

0                   → 数字零，到行头
^/Home              → 到本行第一个不是空字符的位置(所谓空字符就是空格、tab、换行、回车等)
<NUM>|              → 跳转到某一列
$/End               → 到本行行尾
g_                  → 到本行最后一个不是空字符的位置

<NUM>G              → 跳转到第<NUM>行，默认到最后一行
:<NUM>              → 同上
<NUM>gg             → 到第N行，默认第一行

H                   → 跳转到窗口的第一行
M                   → 跳转到窗口的中间
L                   → 跳转到窗口的最后一行
w                   → 跳转到下一个单词的开始处，用标点符号和空格分开
W                   → 跳转到下一个单词的开始处，用空格分开
b                   → 跳转到上一个单词的开始处，用标点符号和空格分开
B                   → 跳转到上一个单词开始处，用空格分开
e                   → 跳转到单词的末尾，使用标点符号和空格分开
E                   → 跳转到单词的末尾，使用空格分开
(                   → 跳转到上一句
)                   → 跳转到下一句
{                   → 调转到上一段
}                   → 调转到下一段
{% endhighlight %}

### 查找调转

{% highlight text %}
%                   → 匹配括号移动，包括 (, {, [，需要在当前行
[*|#]               → 匹配光标当前所在的单词，移动光标到下一个或上一个匹配单词

3fh                 → 调转到第3次出现h字符的地方
t,                  → 到逗号前的第一个字符，逗号可以变成其它字符，T反向
3fa                 → 在当前行查找第三个出现的a，F反向

Ctrl-o              → 跳回到前一个位置
Ctrl-i              → 跳转到后一个位置

/pattern            → 搜索pattern的字符串，通过 n N 来查找下一个或上一个
{% endhighlight %}

![network]({{ site.url }}/images/vim/vim_word_moves.jpg){: .pull-center}
![network]({{ site.url }}/images/vim/vim_line_moves.jpg){: .pull-center}

### Marks :help mark-motions

在 VIM 中可以自己定义标记，用来调转。

{% highlight text %}
''(双单引号)              → 在最近的两个标记的地方相互切换，回到第一个非空字符上
``(双反引号)              → 与上述的功能相同，只是该命令会回到之前的列
:jumps                   → 列出可以跳转的位置列表，当前的用>标记
m[char]                  → 使用26个字符(a-z)标记当前位置，如ma
`[char]                  → 跳转到标记处，如`a
'[char]                  → 同上，跳转到所在行的行首，如'a
:marks                   → 列出所有的标记
:delmarks markname       → 删除标记。
{% endhighlight %}

## 查找 :help pattern

通过* # 可以查找当前光标所在单词位置。

{% highlight text %}
/\<step\>                → 只查找step，其中\<和\>表示一个单词的起始和结尾
/\d                      → 查找一个数字
/\d\+                    → 查找一个或两个数字
/\d\*                    → 查找0个或多个数字
{% endhighlight %}

## 替换，h :substite

vim 中替换命令方式如下。

{% highlight text %}
[ADDR]s/src/dest/[OPTION]
      ADDR: %(1,$)整个文件；1,20第1行到第20行；.,$当前行到行尾
    OPTION: g全局替换；c每次需要确认；i忽略大小写；p不清楚作用
{% endhighlight %}

可以参考如下的示例。

{% highlight text %}
:%s/\(That\) \(this\)/\u\2 \l\1/       将That this 换成This that
:%s/child\([ ,.;!:?]\)/children\1/g    将非句尾的child换成children
:%s/  */ /g                            将多个空格换成一个空格
:%s/\([:.]\)  */\1 /g                  使用空格替换句号或者冒号后面的一个或者多个空格
:g/^$/d                                删除所有空行
:%s/^/  /                              在每行的开始插入两个空白
:%s/$/hello/g                          在每行的莫为插入两个空白
:s/hi/hh/g                             将本行的hi替换为hh
:.,5/$/./                              在接下来的6行末尾加入.
:%s/\r//g                              删除DOS的回车符(^M)
:%s/\r/\r/g                            将DOS回车符，替换为回车
:%s/^\(.*\)\n\1$/\1/g                  删除重复的行
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

## 撤销/重做 help undo-todo

帮助文档，也可以通过 help usr_32.txt 或者 help undolist 查看。

{% highlight text %}
u                        → 撤销
Ctrl-r                   → 重做
:earlier 4m              → 回到4分钟前
:later 45s               → 前进45s钟
:undo 5                  → 撤销5次
:undolist                → 查看undo列表
{% endhighlight %}

## 多窗口操作 :help split

{% highlight text %}
new filename             → 水平打开新窗口
<NUM>sp(lit) filename    → 水平窗口打开文件，默认当前窗口文件，NUM指定新窗口的大小
Ctrl+W s                 → 同上

vnew filename            → 垂直打开新窗口
<NUM>vsp(lit) filename   → 垂直窗口打开文件，默认当前窗口文件，NUM指定新窗口的大小
Ctrl+W v                 → 同上

close                    → 关闭窗口，内容仍然保存，会保留最后一个窗口
Ctrl+W c                 → 同上
quit/ZZ/Ctrl+W q         → 可以关闭最后一个窗口，当多于一个窗口时上述的命令等效
only                     → 只保留当前窗口，如果其它窗口有未保存的修改，则会提示错误

CTRL-W w                 → 切换窗口
CTRL-W h                 → 到左边的窗口
CTRL-W j                 → 到下面的窗口
CTRL-W k                 → 到上面的窗口
CTRL-W l                 → 到右边的窗口
CTRL-W t                 → 到顶部窗口
CTRL-W r                 → 各个窗口旋转
CTRL-W b                 → 到底部窗口

<NUM>Ctrl-w =|+|-        → 均分/增加/减小窗口的大小(NUM行)
<NUM>Ctrl-w [><]         → 宽度设置
<NUM>Ctrl-w _            → 设置当前窗口行数，默认最大化
<NUM>Ctrl-w |            → 垂直设置
:[vertical] res(ize) num → 指定窗口的行数
:[vertical] res(ize)+num → 增大num行
:[vertical] res(ize)-num → 减小num行
{% endhighlight %}

也可以启动 VIM 时，使用 -[O|o]NUM 参数来垂直或水平分屏，例如，vim -O2 file1 file2。


![network]({{ site.url }}/images/vim/vim_split.gif){: .pull-center}

VIM 支持文件浏览，可以通过 :Ex 在当前窗口开启目录浏览器，:Sex 水平分割当前窗口，并在一个窗口中开启目录浏览器。
## 拷贝/粘贴以及粘贴板配置

{% highlight text %}
d    → delete删除
y    → yank复制
P    → 在当前位置之后粘贴
p    → 在当前位置之前粘贴

yap  → 复制整个段，注意段后的空白行也会被复制
yw   → 也会复制词后的空格
yy   → 复制当前行当行到 ddP
{% endhighlight %}

常用的一些技巧包括：xp (交换两个字符)，dwwP (交换两个词，删除一个 word，跳转到下一个，复制) 。


vim 中有十几个粘贴板，分别是 [0-9]、a、"、+ 等，可以直接通过 :reg 命令查看各个粘贴板里的内容，如果只是简单用 y 命令，只会复制到 " 粘贴板里，同样 p 粘贴的也是这个粘贴板里的内容。

\+ 为系统粘贴板，用 "+y 将内容复制到该粘贴板后可以使用 Ctrl＋V 将其粘贴到其他文档，反之则可以通过 "+p 复制到 VIM 。另外，对于系统粘贴板比较有效的是 Shift+Ctrl+v，以及 Shift+Insert 即可，实践中实际要比 "*p、"+p 有用！

要将 vim 的内容复制到某个粘贴板，需要在 Normal 模式中，选择要复制的内容，然后按 "Ny 完成复制，其中 N 为如上所述的粘贴板号。反之，复制到 VIM 时，也要在 Normal 模式下，通过 "Np 粘贴。

另需注意，默认 vim.basic 是没有 + 号寄存器的，需要单独编译，或者 vim.gtk、vim.gnome 才会有。
-->

## 杂项

简单记录下杂七杂八。

### 常用技巧

#### 大小些操作

按 `v` 进入 `visual` 模式，`ap` paragraph 选择整段，`aw` word 选择一个词，通过 `~` 大小写取反；通过 `gU` 更改为大些，`gu` 改为小写。

#### 取消备份

Windows 的配置文件在安装目录下，为 `_vimrc` 。可以添加 `set nobackup | backup` ，如果只需要当前文件不保存可以 `:set nobackup` ，或者保存在固定目录下 `set backupdir=dir` 。

默认会将文件保存为 `filename~` ，也可以通过 `:set backupext=.bak` 指定后缀名，当然这样只能保存最后的一个版本。

另外，在 VIM 中，还提供了 patchmode 把第一次的原始文件备份下来，不会改动，可以直接通过 `:set patchmode=.orig` 命令开启。

#### 没有使用 root

如果由于权限不足导致无法保存，可以通过如下方式保存 `:w !sudo tee %` 。

#### 二进制文件

通常使用外部程序 `xxd` 达到这样的效果，通过 `vim -b file` 命令以二进制格式打开, `:%xxd` 将二进制转换为文本，然后通过 `:%xxd -r` 重新保存为二进制，详细参数可以参考 `man xxd` 。

另外，可以通过 `:set display=uhex` 以 uhex 模式显示，用来显示一些无法显示的字符，例如控制字符之类。

#### 外部修改

可以通过 `:e` 或 `:e!` 加载，后者会放弃之前的修改。

#### 输入特殊字符

通过 `:digraphs` 可以查看特殊字符，在插入模式下或输入命令时可以通过 `Ctrl-k XX` ，其中 `XX` 为 digraphs 前面指定的两个字符；也可以通过 `Ctrl-v NUM` 对应的数字输入特殊字符。

#### 替换^M字符

在 Linux 下查看一些 Windows 下创建的文本文件时，经常会发现在行尾有一些 `^M` 字符，有几种方法可以处理。

##### 1. dos2unix

使用 `dos2unix` 命令，发布版本一般都带这个工具，通过 `dos2unix myfile.txt` 修改即可。

##### 2. vim

使用 vim 的替换功能，当启动 vi 后，进入命令模式，输入以下命令:

{% highlight text %}
:%s/^M$//g                      # 去掉行尾的^M
:%s/^M//g                       # 去掉所有的^M
:%s/^M/[ctrl-v]+[enter]/g       # 将^M替换成回车
:%s/^M/\r/g                     # 将^M替换成回车
{% endhighlight %}

##### 3. sed

使用 sed 命令，和 vi 的用法相似 `sed -e 's/^M/\n/g' myfile.txt` 。

注意：这里的 `^M` 要使用 `CTRL-V CTRL-M` 生成，而不是直接键入 `^M` 。

#### 光标闪烁

可以通过 `set gcr=a:block-blinkon0` 设置，其中 `gcr` 是 `guicursor` 的简写，`a` 表示所有模式，冒号后面是对应模式下的行为参数，每个参数用 `-` 分隔，`block` 说明用块状光标，`blinkon` 表示亮起光标时长，时长为零表示禁用闪烁。

<!--
也可以是 blinkoff0 或者 blinkwait0。具体参考 :help gcr。
Applications->System Tools->Settings->Keyboard->[Cursor Blinking]
-->

### 异常处理

如果发现某个插件无法使用，可以通过 `:scriptnames` 命令查看当前已经加载的脚本程序。

### 键盘图

![vim keyboard 1]({{ site.url }}/images/misc/vim-keyboard-1.gif "vim keyboard 1"){: .pull-center width="90%" }

也可以参考英文版的 [vim Keyboard en]({{ site.url }}/images/misc/vim-keyboard-en.gif) ，或者 [参考下图](http://michael.peopleofhonoronly.com/vim/) 。

![vim keyboard 2]({{ site.url }}/images/misc/vim-keyboard-2.png "vim keyboard 2"){: .pull-center width="90%" }

另外，关于按键列表可以参考 [vim keys]({{ site.url }}/reference/linux/vim-keys.pdf) 。

<!-- http://jrmiii.com/attachments/Vim.pdf -->

## 参考

如果要编写 VIM 脚本，可以参考 Trinity 这个简单的插件，常见的语法可以参考 Vim 脚本语言官方文档 [Write a Vim script](http://vimdoc.sourceforge.net/htmldoc/usr_41.html) 。

可以查看中文文档 [VIM 中文帮助文档](http://vimcdoc.sourceforge.net/doc/) ；另外一个很经典的文档 [Learn Vim Progressively](http://yannesposito.com/Scratch/en/blog/Learn-Vim-Progressively/)，以及 [中文版](http://coolshell.cn/articles/5426.html)； 还有就是一个 PDF 教程，也就是 [Practical Vim.pdf](/reference/linux/Practical_Vim.pdf) 。

一个 vim 的游戏，用于熟悉各种操作，可以参考 [vim adventures](http://vim-adventures.com/) 。

关于如何针对新的语言添加 tags 可以参考 [How to Add Support for a New Language to Exuberant Ctags](http://ctags.sourceforge.net/EXTENDING.html) 。

<!--
https://raw.githubusercontent.com/nvie/vimrc/master/vimrc
https://github.com/amix/vimrc
https://github.com/vgod/vimrc
https://github.com/humiaozuzu/dot-vimrc
https://github.com/xolox/vim-easytags
https://github.com/ruchee/vimrc/blob/master/_vimrc
http://www.cnblogs.com/ma6174/archive/2011/12/10/2283393.html
https://www.zhihu.com/question/26713049

vim 配置文件，将 F2 等功能添加到 default 文件中。

http://www.yeolar.com/note/2010/01/27/vim/
https://github.com/vim-airline/vim-airline/wiki/FAQ

<a href="http://python.42qu.com/11180003">使用Vim打造现代化的Python IDE</a>。<br><br>
<a href="http://python.42qu.com/11165602">如何用python写vim插件</a>

http://ju.outofmemory.cn/entry/79671                      经典插件的介绍
http://blog.chinaunix.net/uid-24118190-id-4077308.html    VIM终极配置
/reference/linux/{default.vim, plugin.vim}                本地保存的版本
-->

{% highlight text %}
{% endhighlight %}
