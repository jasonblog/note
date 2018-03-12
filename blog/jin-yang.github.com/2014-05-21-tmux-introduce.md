---
title: TMUX 简介
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,工具,tmux
description: TMUX 类似于 screen，是一个窗口管理器，而且功能更加强大，提供了强劲的、易于使用的命令行界面，可横向和纵向分割窗口，支持 UTF-8 编码及 256 色终端等特性。
---

TMUX 类似于 screen，是一个窗口管理器，而且功能更加强大，提供了强劲的、易于使用的命令行界面，可横向和纵向分割窗口，支持 UTF-8 编码及 256 色终端等特性。

接下来简单介绍下 TMUX 的使用。

<!-- more -->

## 简介

如上所述 tmux 是一个终端复用软件，类似于 GUN Screen，tmux 可以在终端中使用，同时可以在 ssh 中使用，其中可以开启多个控制台。

![tmux examples]({{ site.url }}/images/linux/tmux-examples.png){: .pull-center width="95%"}

上图是官网上的一个截图，将屏幕分为了多个窗口，方便进行管理。

tmux 使用 C/S 模型构建，分为以下单元模块：

* Server，服务器。一个 tmux 命令执行后启动一个 tmux 服务；

* Session，会话。一个 tmux 服务可以拥有多个 session ，一个 session 可以看作是 tmux 管理下的伪终端的一个集合；

* Window，窗口。一个 session 可能会有多个 window 与之关联，每个 window 都是一个伪终端，会占据整个屏幕；

* Pane，面板。一个 window 可以被分割成多个 pane ，每个都是一个 pty ，可以通过 tty 命令查看。

基本结构如下图所示。

![network]({{ site.url }}/images/linux/tmux-session-window-pane.jpg){: .pull-center}

tmux 所支持的特性包括了：

* 支持 UTF-8 编码及 256 色终端。

* 可在多个缓冲区进行复制和粘贴。

* 可通过交互式菜单来选择窗口、会话及客户端。

* 支持跨窗口搜索。

* 支持自动及手动锁定窗口。

另外，通过 libevent 提供高性能。


### 安装

在 CentOS 中可以通过 yum 安装，也可以按照如下方式，采用源码安装。先从 [tmux.sourceforge.net](http://tmux.sourceforge.net/) 上下载源码。tmux 需要有 ncurses 和 libevent 的支持，需要 libevent-2.0 版本，而通过 yum 只能安装低版本的，所以使用源码编译，安装过程如下。

{% highlight text %}
# yum install ncurses-devel         # 安装 ncurses 和 libevent 库
$ cd ~/libevent2
$ ./configure --prefix=/usr/local
$ make
# make install

$ cd ~/tmux                              # 编译安装 tmux
$ LDFLAGS="-L/usr/local/lib -Wl,-rpath=/usr/local/lib" ./configure --prefix=/usr/local
$ make
# make install
{% endhighlight %}

在 Ubuntu 中可以通过 apt-get install tmux 直接安装。

### 启动

第一次会产生一个新的 session 和 window ，在下方会显示其状态，如果意外断开 (detach) 连接，session 仍会在后台运行，每个窗口可以分为多个 pane 。

启动时的参数选项有：

* -2: 强制 tmux 假设终端支持 256 色。
* -8: 类似于 -2 ，不过是强制 tmux 假设终端支持 88 色。
* -c shell-command: 使用默认的 shell 执行命令，主要用于当 tmux 作为 login shell 时使用。
* -f file: 指定配置文件，默认检查 /etc/tmux.conf、~/.tmux.conf，如果有命令错误，则会直接退出。
* -V: 查看版本号。

默认 tmux 会创建的匿名的 session ，可以通过如下命令创建一个命名的 session ， Ctrl-b 是命令前缀(Command prefix)，通过前缀告知 tmux 下面的命令是发给 tmux 的，而非终端。常见的操作如下。

{% highlight text %}
$ tmux new-session -s basic    # 创建一个名为 basic 的 session
$ tmux new -s basic -d         # 同上，但是不会连接到终端，在后台运行
$ tmux new -s basic -n win     # 同上，并将第一个窗口命令为 win
[Ctrl-b d]                     # detach

$ tmux ls                      # 列出现在的 sessions ，等同 tmux list-sessioin
$ tmux attach                  # 如果只有一个 session
$ tmux attach -t basic         # 指定名称，-t 表示 target

$ tmux kill-session -t basic   # 关闭一个 session
{% endhighlight %}

如下是各种常用的命令。


## 常用快捷键

默认采用 Ctrl-b 作为 prefix，用于激活控制台。

### 常见操作

如何在 tmux 内新建会话。

{% highlight text %}
?                                 → 列出所有快捷键，按q返回
r                                 → 强制重绘未脱离的会话口，等同于刷新
~                                 → 列出提示信息缓存；包含了tmux返回的各种提示信息
source-file ~/.tmux.conf          → 重新设置完配置文件后，使用该命令刷新
:                                 → 进入命令行模式，如kill-server可以关闭服务器

d                                 → 脱离(detach)当前会话，仍在后台运行
D                                 → 通过列表选择需要退出的会话
$ tmux attach -t work             → 通过attach命令重新连接

s                                 → 列出所有会话，选择切换会话
(                                 → 切换到前一个Session
)                                 → 切换到后一个Session

:resize-pane -L|D|R|U [NUM]       → 将pane左|下|右|上移动NUM行，默认是1行
:set-option -g status-interval 40 → 设置状态栏的更新时间，单位为秒
{% endhighlight %}

<!--
[  进入复制模式；此时的操作与vi/emacs相同，按q/Esc退出，修改为y</li><li>
:new-windos
/new-session -n name ["command"] 新建一个窗口，并在窗口中执行 command 命令；如果指定命令，当命令退出时，窗口同样关闭。</li><li>
-->

### 窗口(Windows)操作

如果窗口没有命名，会以当前命令作为窗口名。

{% highlight text %}
c                                 → 创建新窗口
&                                 → 关闭当前窗口，也可以使用exit命令
,                                 → 重命名当前窗口，这样便于识别
.                                 → 修改当前窗口编号，相当于窗口重新排序
p                                 → 切换至上一窗口
n                                 → 切换至下一窗口
<NUM>                             → 切换至指定窗口，每个窗口默认从0开始编号
f                                 → 通过名称切换窗口，窗口必须之前已经命名
w                                 → 通过窗口列表切换窗口，此时会列出所有的窗口
{% endhighlight %}

### 面板(Panes)操作

{% highlight text %}
"                                 → 将当前面板平分为上下两块，修改为-
%                                 → 将当前面板平分为左右两块，修改为|
x                                 → 关闭当前面板，会提示是否关闭

o                                 → 在当前窗口中选择下一面板
方向键                             → 移动光标以选择面板
;                                 → 切换到最后一个使用的面板
q                                 → 显示面板编号，输入编号调转到相应面板

Alt+o                             → 逆时针旋转当前窗口的面板
Ctrl+o                            → 顺时针旋转当前窗口的面板
{                                 → 向前置换当前面板
}                                 → 向后置换当前面板

!                                 → 当前面板置于新窗口，仅包含当前面板内容

Alt+方向键                         → 以5个单元格为单位移动边缘以调整当前面板大小
{% endhighlight %}

另外，可以通过 &lt;prefix&gt;+space 在预置的面板布局中循环切换；依次包括 even-horizontal (横向平铺)、even-vertical(垂直平铺)、main-horizontal(大的 panes 在顶部，底部会有很多小的 panes )、main-vertical(大的 pane 在左边，右边很多小的 panes )、tiled(平均分配各个窗口) 。

### 其它

{% highlight text %}
t                                 → 显示时间，回车返回
{% endhighlight %}



## 定制 tmux

tmux 的系统级配置文件为 /etc/tmux.conf ，用户级配置文件为 ~/.tmux.conf 。

有些资料中建议修改 Prefix 组合键，如 CapsLock 和 A 相邻，可以将 CapsLock 映射为 Ctrl （需要使用操作系统的键盘映射），并将 Prefix 的 Ctrl-b 重定义为 Ctrl-a ，这样的话可以非常方便的使用。

但是在 Linux 操作中会与 Bash 的 Ctrl-A 冲突，因此没有修改该键，主要是因为 Prefix 只有在调整 tmux 才会使用。如上的配置，可以在命令行中执行如下命令查看：

{% highlight text %}
$ tmux show-options -g
$ tmux show-window-options -g
{% endhighlight %}

如下是常用的配置文件示例。

{% highlight text %}
set -g prefix C-a                # 设置Prefix为Ctrl-a而非默认Ctrl-b，-g表示全局，set为set-option的简写
unbind C-b                       # 解除 Ctrl-b 与 Prefix 的对应关系

set-option -g display-time 5000  # 提示信息的持续时间，单位为毫秒
set-option -g repeat-time 1000   # 通过<prefix>激活控制台后持续时间，避免每次都要先激活控制台，单位为毫秒
set-option -g status-keys vi     # 操作状态栏时的默认键盘布局；可以设置为vi或emacs

set -sg escape-time 1      # tmux在发送命令时，会添加了一个延迟，可能会对其它软件如vim造成干扰
                           # 该选项可以增加其响应速度，单位毫秒

set -g base-index 1        # 将session中的窗口序列初始值设置为从1开始
setw -g pane-base-index 1  # 将panes的开始设置为1，setw是set-window-option的缩写

bind r source-file ~/.tmux.conf\; display "Reloaded!"
                           # 将PREFIX r设置为执行命令source-file ~/.tmux.conf，并回显Reload!
                           # 多条命令之间可以通过\; 进行分割。bind是bind-key的缩写

bind-key -n C-r source-file ~/.tmux.conf
                           # 不使用PREFIX，只使用Ctrl-r，可能对其它造成影响
bind C-a send-prefix       # 通过Prefix + Ctrl-a ，将Prefix发送给应用程序

bind | split-window -h     # 垂直分开panes
bind - split-window -v     # 水平，-v画个水平线，即垂直分配

bind h select-pane -L      # 切换panes，用于替换方向键，使用hjkl代替
bind j select-pane -D
bind k select-pane -U
bind l select-pane -R

bind -r C-h select-window -t :-   # 实现PREFIX Ctrl-h或PRFIX Ctrl-l的循环切换
bind -r C-l select-window -t :+

bind H resize-pane -L 5    # 调整 panes 的大小
bind J resize-pane -D 5
bind K resize-pane -U 5
bind L resize-pane -R 5

bind -r H resize-pane -L 5 # 调整 panes 的大小，可以在一定时间内连续按 H ，-r 表示 repeatable
bind -r J resize-pane -D 5 # 可以通过 repeat-time 设置时间，默认是 500 毫秒。
bind -r K resize-pane -U 5
bind -r L resize-pane -R 5

setw -g mode-mouse on            # 响应鼠标的事件，包括选中 pane ，使用滚轮查看 buffer
set -g mouse-select-pane on      # 可以选择 pane ，如果要关闭直接使用上一句即可
set -g mouse-resize-pane on      # 可以调整 pane 大小
set -g mouse-select-window on    # 可以通过窗口列表选择窗口

set -g default-terminal "screen-256color" # 设置使用 256 色
set -g status-fg white           # 设置状态栏的前景色和后景色
set -g status-bg black
setw -g window-status-fg cyan
setw -g window-status-bg default # default 表示从状态栏继承
setw -g window-status-attr dim
setw -g window-status-current-fg white   # 改变当前窗口
setw -g window-status-current-bg red
setw -g window-status-current-attr bright
set -g pane-border-fg green      # 设置 pane 的边框
set -g pane-border-bg black
set -g pane-active-border-fg white
set -g pane-active-border-bg yellow
set -g message-fg white          # 设置消息栏，包括命令栏
set -g message-bg black
set -g message-attr bright

set -g status-left-length 40     # 设置左状态栏的长度
set -g status-left "#[fg=green]#S #[fg=yellow]#I #[fg=cyan]#P"       # 设置左状态栏
set -g status-right "#[fg=cyan]%d %b %R"  # 时间格式可以通过 strftime() 设置
set -g status-utf8 on            # 设置状态栏
set -g status-interval 60        # 设置状态栏的更新间隔时间，单位为秒
set -g status-justify centre     # 将窗口列表居中显示
setw -g monitor-activity on      # 设置监控其它窗口
set -g visual-activity on        # 当其他窗口有消息时会提示

setw -g mode-keys vi                    # 使用 vi 的快捷键

bind h select-layout even-horizontal    # 将当前面板布局切换为 even-horizontal
bind v select-layout even-vertical      # 将当前面板布局切换为 even-vertical
{% endhighlight %}

### 定制 tmux 启动脚本

tmux 脚本，可以参考 [tmux_basic](/reference/linux/tmux_basic) 脚本，实际上是一个 shell 脚本，可以通过如下命令的进行不同的组合。也就是通过运行该脚本，生成最终的 tmux 初始环境。

{% highlight text %}
### 1. 创建一个可执行文件
$ touch ~/development && chmod +x ~/development

### 2. 判断是否存在basic会话，否则下面命令将会出错
tmux has-session -t basic

### 3. 创建一个新session名称为basic，新窗口为名为editor，并detach
tmux new-session -s basic -n editor -d

### 4. 向指定pane发送命令，C-m表示回车，使用session:window:pane指定特定的pane
tmux send-keys -t basic 'cd ~' C-m

### 5. 将窗口分为上下两部分，-h分为左右两部分，并通过-p指定比例，下和右所占比例
tmux split-window -v -p 10 -t basic

### 6. 也可以选择一个默认的分布
tmux select-layout -t basic main-horizontal

### 7. 创建一个新的窗口
tmux new-window -n editor -t basic

### 8. 选择一个默认窗口
tmux select-window -t basic:1

### 9. 最后连接到basic
tmux attach -t basic
{% endhighlight %}

实际是一个 Shell 脚本，当然 tmux 的配置文件也可以进行相同的设置。

可以将上述的命令放置到 tmux.conf 配置文件中，此时将会自动执行，也可以将其放置到不同的文件中，然后通过 tmux -f 指定相应的配置文件，第一行中指定需要加载默认配置文件。

{% highlight text %}
# ~/.tmux/aiduppconf
source-file ~/.tmux.conf      # 加载默认的配置选项，非强制，但是建议使用
........ your commands
{% endhighlight %}


tmux 的默认行为是新建一个 session ，因此如果没有使用 attach 选项，那么实际会再新建一个 session ，此时会导致有两个 session 。

<!--
<br><h2>复制/滚屏模式</h2><p>

    Prefix # 列出所有buffers；Prefix = 列出并选择相应的buffer

	在 tmux 中可以查看缓冲区的内容，并且可以进行复制、粘贴。可以通过yum install xclip安装xclip。<br><br>

	滚屏，即查看缓冲区的内容，需要进入 copy-mode ，即 PREFIX+[ ，然后就可以用上下键来滚动屏幕，配置了 vi 快捷键模式，就可以像操作 vi 一样来滚动屏幕，即 hjkl ，非常的方便，退出直接按 q 键或 Enter 即可。<br><br>

	之前已经设定好了使用 vi 模式，因此，可以通过 w 跳转到下一个单词，b 跳转回上一个单词；f+char 向前跳转到 char 字符处，F+char 向后跳转到 char 字符处。使用 g 跳转到 buffer 的首部，G 跳转到末尾；Ctrl-b 向后一页，Ctrl-f 向前翻页。同时可以使用 ? 和 / 向上和向下搜索，n 查找下一个，N 查找上一个。<br><br><br>


	PREFIX [ 进入复制模式，将光标移动到需要复制 buffer 的地方，按 space 开始复制，移动光标选择复制区域；按 Enter 复制并退出 copy-mode；需要粘贴时，按 PREIFX ] 粘贴。<br><br>

	可以使用 PREFIX : 并输入 capture-pane 命令复制一个 pane 的所有内容，然后再使用 Prefix-] 复制。<br><br>

	可以在终端通过 tmux show-buffer 查看 buffer 的内容，在 tmux 的命令行中通过 show-buffer 查看；当然也可以在 Shell 中通过 tmux capture-pane && tmux save buffer.txt 将 buffer 保存在 buffer.txt 文件中，在 tmux 中使用 capture-pane;save-buffer buffer.txt 保存。<br><br>

	tmux 维护了一个堆栈用于保存缓存，每次都会将新的缓存保存在栈顶。在 tmux 中通过 list-buffers 可以查看所有的 buffers 。如果有多个 buffer ，Prefix-] 默认使用第一条，可以通过 choose-buffer 选择相应的 buffer ，然后通过 Enter 插入到制定pane。<br><br>

	可以通过如下的设置，使得操作时更像 vi 。Escape 切换到复制模式，y (yank) 将内容复制到 buffers ，v 切换到选择模式，p 用于粘贴文本。
<pre>unbind [
bind Escape copy-mode
unbind p
bind p paste-buffer
bind -t vi-copy 'v' begin-selection
bind -t vi-copy 'y' copy-selection</pre><br>

	tmux 也可以通过 xclip 与系统的粘贴板进行交互，在 Linux 中通过如下命令安装 apt-get install xclip/yum install xclip 。通过第一行的命令将 tmux 当前的 buffer 输出到 xclip ；第二行将系统粘贴板的内容复制到 tmux 中。
<pre>bind C-c run "tmux save-buffer - | xclip -i -sel clipboard"<br>bind C-v run "tmux set-buffer \"$(xclip -o -sel clipboard)\"; tmux paste-buffer"</pre>

    当打开mode-mouse时，可以使用“Shift+鼠标”选中文字；Ctrl+Shift+c复制，Ctrl+Shift+v粘贴。
-->


### 颜色设置

可以从 [colortest](http://www.vim.org/scripts/download_script.php?src_id=4568) 上下载该文件，也可以使用 [本地文档](/reference/linux/tmux_colortest)，通过如下命令测试终端；并在 ~/.bashrc 添加如下内容，此时只会在 TMUX 外设置 TERM ，而 TMUX 会设置自己的终端。

{% highlight text %}
$ perl colortest -w

### 在 ~/.bashrc 中添加如下内容
[ -z "$TMUX" ] && export TERM=xterm-256color
{% endhighlight %}

tmux 支持指定特定的颜色，包括了 black, red, green, yellow, blue, magenta, cyan 和 white，也可以使用 colour0 到 colour255 ，可以简单的通过如下命令查看。

{% highlight text %}
for i in {0..255}; do printf "\x1b[38;5;${i}mcolour${i}  "; done
{% endhighlight %}

tmux 的各个组件可以通过前景色和后景色进行设置。

在 Putty 中，右击 -> Change Settings... -> Window -> Colours 选中， Allow terminal to specify ANSI colours 和 Allow terminal to use xterm 256-colour mode ，并在 Indicate bolded text by changing: The colour 。

tmux 启动时可以使用 -2 参数，此时 tmux 会强制假设终端支持 256 色。

### 设置状态栏

状态栏由三部分组成：右面板、窗口列表、左面板。左右面板可以使用文本和变量，常见变量如下所示。

{% highlight text %}
#H                主机名
#h                主机名，不含域名
#F                当前窗口的 flag
#I                当前窗口的序号
#P                当前面板的序号
#S                当前 session 名
#T                当前窗口的 title
#W                当前窗口的名称
##                字符 #
#(shell-command)  shell 命令输出的第一行
#[attributs]      改变颜色或者属性
{% endhighlight %}

### 合作

通过 tmux 可以两个人进行合作，在一个窗口上的操作可以被另一个人看到。最简单的是同一个用户 (Shared Account)，此时一个人创建一个窗口，另一个人 attach 即可。

{% highlight text %}
$ tmux new-session -s Pairing     # 一个用户
$ tmux attach -t Pairing          # 另一个用户
{% endhighlight %}

使用同一个用户 (Shared Account) 和分类会话 (Grouped Sessions) ，此时与上述的相似，不过当一个用户切换时，另一个用户不会看到。这样的话，当两个人需要使用不同的编辑器进行编译时，可以各自创建一个新的窗口，各自不会影响。

{% highlight text %}
$ tmux new-session -s Pairing     # 一个用户
$ tmux attach -t Pairing -s my    # 另一个用户
{% endhighlight %}

使用不同的用户，假设为 foo 和 bar 。这种情况下，两个人可以使用不同的配置选项，如快捷键、状态栏等属性。

{% highlight text %}
$ sudo adduser ted            # 添加用户
$ sudo adduser barney

$ sudo addgroup tmux          # 添加用户组
$ sudo mkdir /var/tmux

$ sudo chgrp tmux /var/tmux   # 修改用户组

$ sudo chmod g+ws /var/tmux   # 修改属性

$ sudo usermod -aG tmux ted   # 添加到用户组
$ sudo usermod -aG tmux barney

$ tmux -S /var/tmux/pairing   # 一个用户在创建时指定 Sockets
$ tmux -S /var/tmux/pairing attach # 另一个用户连接
{% endhighlight %}



## 其它

#### 1. pane 转为 window

通过 Prefix ! 可以将一个 pane 转作 window。

#### 2. pane 合并到 window

如果想将一个 pane 合并到 window ，可以在 tmux 中通过如下命令执行。

{% highlight text %}
join-pane -s 1.0 -t 2
{% endhighlight %}

如果没有通过 -t 指定目标，那么默认为当前的 window ，在指定 pane 时，目标通过 [session_name]:[window].[pane] 指定。


#### 3. 判断是否存在 session

在一个 Bash 脚本中，可以通过如下的命令查看是否存在一个 Session 。

{% highlight text %}
if ! tmux has-session -t remote; then
   exec tmux new-session -s development -d
   # other setup commands before attaching....
fi
exec tmux attach -t development
{% endhighlight %}



<!--
有时候我们希望将一个 pane 放大，然后查看其内容，最后将其恢复。通常可以通过 break-pane 然后使用 join-pane 将其取回，这样做会有一些麻烦，下面通过一个临时的 window 作为容器进行操作。

unbind Up        ## 将 pane 放大
bind Up new-window -d -n tmp \; swap-pane -s tmp.1 \; select-window -t tmp
unbind Down      ## 将窗口恢复
bind Down last-window \; swap-pane -s tmp.1 \; kill-window -t tmp




之前曾经通过 send-keys 执行某一个命令，同样可以通过如下的方式执行，不过当 ssh 退出时，pane 或者 window 同样会关闭。

$ tmux new-session -s servers -d "ssh deploy@burns"
$ tmux split-window -v "ssh dba@smithers"
$ tmux attach -t servers
-->


#### 4. 设置默认 bash

tmux 默认使用的是 bash ，如果要使用其它的终端，可以在 tmux.conf 中通过如下的命令指定。

{% highlight text %}
set -g default-command /bin/zsh
set -g default-shell /bin/zsh
{% endhighlight %}

#### 5. TERM设置

当 tmux 运行时，会将 TERM 变量设置为 "screen" 或者配置文件中 default-terminal 指定的值，在 .bashrc 中可以通过判断该变量来确定现在是否在 tmux 中。

{% highlight text %}
if [[ "$TERM" != "screen-256color" ]]
then
   tmux attach-session -t "$USER" || tmux new-session -s "$USER"
   exit
fi
{% endhighlight %}

如果不在 tmux 中，那么首先尝试连接 $USER ，也就是 Session 为用户名；如果链接失败则会尝试新建一个，然后退出脚本。

当启动 tmux 时，仍会执行配置文件，但是由于这次在 tmux 中，因此跳过这段指令。


#### 6. 窗口切换

将窗口在 Session 之间进行切换，使用时通过  Prefix . ，并在命令行中输入想要切换的 Session。当热也可以通过如下的命令执行

{% highlight text %}
$ tmux move-window -s processes:1 -t editor
{% endhighlight %}

将 processes 的第一个 window 移动到 editor 。



<!--
之前曾介绍过通过 capture-pane + save-buffer 的方式保存终端的输出信息。此处介绍通过 pipe-pane ，通过这个命令我们可以控制开关，可以在一个命令已经执行的时候保存输出。在配置文件中添加如下的内容。
<pre>bind P pipe-pane -o "cat >>~/#W.log" \; display "Toggled logging to ~/#W.log"</pre>
通过 Prefix P 进行切换，-o 指定输出。
-->

### putty 乱码设置

在通过 putty 链接 tmux 时，可能会出现乱码，此时需要设置字符集。[右键] => [Change Settings...] => [Windows/Translation] => [Remote character set:] 改为 UTF-8 。



## 参考

详细内容可以参考官方网站 [tmux.github.io](http://tmux.github.io/)，详细的文档可以参考 [tmux Productive Mouse-Free Development](/reference/linux/Pragmatic.tmux.Feb.2012.pdf)，版权归作者所有。

当然也可以通过 [tmux-powerline](https://github.com/erikw/tmux-powerline) 配置状态栏；关于按键的绑定可以参考 [Moving The Ctrl Key](https://www.emacswiki.org/emacs/MovingTheCtrlKey)，以及参考配置文件 [tmux.conf](/reference/linux/tmux.conf) 。

<!--
set -g status-utf8 on
Mouse Settings
set the default bash
set -g utf8
set-window-option -g utf8 on
-->

{% highlight text %}
{% endhighlight %}
