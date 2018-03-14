---
title: TMUX 簡介
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,工具,tmux
description: TMUX 類似於 screen，是一個窗口管理器，而且功能更加強大，提供了強勁的、易於使用的命令行界面，可橫向和縱向分割窗口，支持 UTF-8 編碼及 256 色終端等特性。
---

TMUX 類似於 screen，是一個窗口管理器，而且功能更加強大，提供了強勁的、易於使用的命令行界面，可橫向和縱向分割窗口，支持 UTF-8 編碼及 256 色終端等特性。

接下來簡單介紹下 TMUX 的使用。

<!-- more -->

## 簡介

如上所述 tmux 是一個終端複用軟件，類似於 GUN Screen，tmux 可以在終端中使用，同時可以在 ssh 中使用，其中可以開啟多個控制檯。

![tmux examples]({{ site.url }}/images/linux/tmux-examples.png){: .pull-center width="95%"}

上圖是官網上的一個截圖，將屏幕分為了多個窗口，方便進行管理。

tmux 使用 C/S 模型構建，分為以下單元模塊：

* Server，服務器。一個 tmux 命令執行後啟動一個 tmux 服務；

* Session，會話。一個 tmux 服務可以擁有多個 session ，一個 session 可以看作是 tmux 管理下的偽終端的一個集合；

* Window，窗口。一個 session 可能會有多個 window 與之關聯，每個 window 都是一個偽終端，會佔據整個屏幕；

* Pane，面板。一個 window 可以被分割成多個 pane ，每個都是一個 pty ，可以通過 tty 命令查看。

基本結構如下圖所示。

![network]({{ site.url }}/images/linux/tmux-session-window-pane.jpg){: .pull-center}

tmux 所支持的特性包括了：

* 支持 UTF-8 編碼及 256 色終端。

* 可在多個緩衝區進行復制和粘貼。

* 可通過交互式菜單來選擇窗口、會話及客戶端。

* 支持跨窗口搜索。

* 支持自動及手動鎖定窗口。

另外，通過 libevent 提供高性能。


### 安裝

在 CentOS 中可以通過 yum 安裝，也可以按照如下方式，採用源碼安裝。先從 [tmux.sourceforge.net](http://tmux.sourceforge.net/) 上下載源碼。tmux 需要有 ncurses 和 libevent 的支持，需要 libevent-2.0 版本，而通過 yum 只能安裝低版本的，所以使用源碼編譯，安裝過程如下。

{% highlight text %}
# yum install ncurses-devel         # 安裝 ncurses 和 libevent 庫
$ cd ~/libevent2
$ ./configure --prefix=/usr/local
$ make
# make install

$ cd ~/tmux                              # 編譯安裝 tmux
$ LDFLAGS="-L/usr/local/lib -Wl,-rpath=/usr/local/lib" ./configure --prefix=/usr/local
$ make
# make install
{% endhighlight %}

在 Ubuntu 中可以通過 apt-get install tmux 直接安裝。

### 啟動

第一次會產生一個新的 session 和 window ，在下方會顯示其狀態，如果意外斷開 (detach) 連接，session 仍會在後臺運行，每個窗口可以分為多個 pane 。

啟動時的參數選項有：

* -2: 強制 tmux 假設終端支持 256 色。
* -8: 類似於 -2 ，不過是強制 tmux 假設終端支持 88 色。
* -c shell-command: 使用默認的 shell 執行命令，主要用於當 tmux 作為 login shell 時使用。
* -f file: 指定配置文件，默認檢查 /etc/tmux.conf、~/.tmux.conf，如果有命令錯誤，則會直接退出。
* -V: 查看版本號。

默認 tmux 會創建的匿名的 session ，可以通過如下命令創建一個命名的 session ， Ctrl-b 是命令前綴(Command prefix)，通過前綴告知 tmux 下面的命令是發給 tmux 的，而非終端。常見的操作如下。

{% highlight text %}
$ tmux new-session -s basic    # 創建一個名為 basic 的 session
$ tmux new -s basic -d         # 同上，但是不會連接到終端，在後臺運行
$ tmux new -s basic -n win     # 同上，並將第一個窗口命令為 win
[Ctrl-b d]                     # detach

$ tmux ls                      # 列出現在的 sessions ，等同 tmux list-sessioin
$ tmux attach                  # 如果只有一個 session
$ tmux attach -t basic         # 指定名稱，-t 表示 target

$ tmux kill-session -t basic   # 關閉一個 session
{% endhighlight %}

如下是各種常用的命令。


## 常用快捷鍵

默認採用 Ctrl-b 作為 prefix，用於激活控制檯。

### 常見操作

如何在 tmux 內新建會話。

{% highlight text %}
?                                 → 列出所有快捷鍵，按q返回
r                                 → 強制重繪未脫離的會話口，等同於刷新
~                                 → 列出提示信息緩存；包含了tmux返回的各種提示信息
source-file ~/.tmux.conf          → 重新設置完配置文件後，使用該命令刷新
:                                 → 進入命令行模式，如kill-server可以關閉服務器

d                                 → 脫離(detach)當前會話，仍在後臺運行
D                                 → 通過列表選擇需要退出的會話
$ tmux attach -t work             → 通過attach命令重新連接

s                                 → 列出所有會話，選擇切換會話
(                                 → 切換到前一個Session
)                                 → 切換到後一個Session

:resize-pane -L|D|R|U [NUM]       → 將pane左|下|右|上移動NUM行，默認是1行
:set-option -g status-interval 40 → 設置狀態欄的更新時間，單位為秒
{% endhighlight %}

<!--
[  進入複製模式；此時的操作與vi/emacs相同，按q/Esc退出，修改為y</li><li>
:new-windos
/new-session -n name ["command"] 新建一個窗口，並在窗口中執行 command 命令；如果指定命令，當命令退出時，窗口同樣關閉。</li><li>
-->

### 窗口(Windows)操作

如果窗口沒有命名，會以當前命令作為窗口名。

{% highlight text %}
c                                 → 創建新窗口
&                                 → 關閉當前窗口，也可以使用exit命令
,                                 → 重命名當前窗口，這樣便於識別
.                                 → 修改當前窗口編號，相當於窗口重新排序
p                                 → 切換至上一窗口
n                                 → 切換至下一窗口
<NUM>                             → 切換至指定窗口，每個窗口默認從0開始編號
f                                 → 通過名稱切換窗口，窗口必須之前已經命名
w                                 → 通過窗口列表切換窗口，此時會列出所有的窗口
{% endhighlight %}

### 面板(Panes)操作

{% highlight text %}
"                                 → 將當前面板平分為上下兩塊，修改為-
%                                 → 將當前面板平分為左右兩塊，修改為|
x                                 → 關閉當前面板，會提示是否關閉

o                                 → 在當前窗口中選擇下一面板
方向鍵                             → 移動光標以選擇面板
;                                 → 切換到最後一個使用的面板
q                                 → 顯示面板編號，輸入編號調轉到相應面板

Alt+o                             → 逆時針旋轉當前窗口的面板
Ctrl+o                            → 順時針旋轉當前窗口的面板
{                                 → 向前置換當前面板
}                                 → 向後置換當前面板

!                                 → 當前面板置於新窗口，僅包含當前面板內容

Alt+方向鍵                         → 以5個單元格為單位移動邊緣以調整當前面板大小
{% endhighlight %}

另外，可以通過 &lt;prefix&gt;+space 在預置的面板佈局中循環切換；依次包括 even-horizontal (橫向平鋪)、even-vertical(垂直平鋪)、main-horizontal(大的 panes 在頂部，底部會有很多小的 panes )、main-vertical(大的 pane 在左邊，右邊很多小的 panes )、tiled(平均分配各個窗口) 。

### 其它

{% highlight text %}
t                                 → 顯示時間，回車返回
{% endhighlight %}



## 定製 tmux

tmux 的系統級配置文件為 /etc/tmux.conf ，用戶級配置文件為 ~/.tmux.conf 。

有些資料中建議修改 Prefix 組合鍵，如 CapsLock 和 A 相鄰，可以將 CapsLock 映射為 Ctrl （需要使用操作系統的鍵盤映射），並將 Prefix 的 Ctrl-b 重定義為 Ctrl-a ，這樣的話可以非常方便的使用。

但是在 Linux 操作中會與 Bash 的 Ctrl-A 衝突，因此沒有修改該鍵，主要是因為 Prefix 只有在調整 tmux 才會使用。如上的配置，可以在命令行中執行如下命令查看：

{% highlight text %}
$ tmux show-options -g
$ tmux show-window-options -g
{% endhighlight %}

如下是常用的配置文件示例。

{% highlight text %}
set -g prefix C-a                # 設置Prefix為Ctrl-a而非默認Ctrl-b，-g表示全局，set為set-option的簡寫
unbind C-b                       # 解除 Ctrl-b 與 Prefix 的對應關係

set-option -g display-time 5000  # 提示信息的持續時間，單位為毫秒
set-option -g repeat-time 1000   # 通過<prefix>激活控制檯後持續時間，避免每次都要先激活控制檯，單位為毫秒
set-option -g status-keys vi     # 操作狀態欄時的默認鍵盤佈局；可以設置為vi或emacs

set -sg escape-time 1      # tmux在發送命令時，會添加了一個延遲，可能會對其它軟件如vim造成干擾
                           # 該選項可以增加其響應速度，單位毫秒

set -g base-index 1        # 將session中的窗口序列初始值設置為從1開始
setw -g pane-base-index 1  # 將panes的開始設置為1，setw是set-window-option的縮寫

bind r source-file ~/.tmux.conf\; display "Reloaded!"
                           # 將PREFIX r設置為執行命令source-file ~/.tmux.conf，並回顯Reload!
                           # 多條命令之間可以通過\; 進行分割。bind是bind-key的縮寫

bind-key -n C-r source-file ~/.tmux.conf
                           # 不使用PREFIX，只使用Ctrl-r，可能對其它造成影響
bind C-a send-prefix       # 通過Prefix + Ctrl-a ，將Prefix發送給應用程序

bind | split-window -h     # 垂直分開panes
bind - split-window -v     # 水平，-v畫個水平線，即垂直分配

bind h select-pane -L      # 切換panes，用於替換方向鍵，使用hjkl代替
bind j select-pane -D
bind k select-pane -U
bind l select-pane -R

bind -r C-h select-window -t :-   # 實現PREFIX Ctrl-h或PRFIX Ctrl-l的循環切換
bind -r C-l select-window -t :+

bind H resize-pane -L 5    # 調整 panes 的大小
bind J resize-pane -D 5
bind K resize-pane -U 5
bind L resize-pane -R 5

bind -r H resize-pane -L 5 # 調整 panes 的大小，可以在一定時間內連續按 H ，-r 表示 repeatable
bind -r J resize-pane -D 5 # 可以通過 repeat-time 設置時間，默認是 500 毫秒。
bind -r K resize-pane -U 5
bind -r L resize-pane -R 5

setw -g mode-mouse on            # 響應鼠標的事件，包括選中 pane ，使用滾輪查看 buffer
set -g mouse-select-pane on      # 可以選擇 pane ，如果要關閉直接使用上一句即可
set -g mouse-resize-pane on      # 可以調整 pane 大小
set -g mouse-select-window on    # 可以通過窗口列表選擇窗口

set -g default-terminal "screen-256color" # 設置使用 256 色
set -g status-fg white           # 設置狀態欄的前景色和後景色
set -g status-bg black
setw -g window-status-fg cyan
setw -g window-status-bg default # default 表示從狀態欄繼承
setw -g window-status-attr dim
setw -g window-status-current-fg white   # 改變當前窗口
setw -g window-status-current-bg red
setw -g window-status-current-attr bright
set -g pane-border-fg green      # 設置 pane 的邊框
set -g pane-border-bg black
set -g pane-active-border-fg white
set -g pane-active-border-bg yellow
set -g message-fg white          # 設置消息欄，包括命令欄
set -g message-bg black
set -g message-attr bright

set -g status-left-length 40     # 設置左狀態欄的長度
set -g status-left "#[fg=green]#S #[fg=yellow]#I #[fg=cyan]#P"       # 設置左狀態欄
set -g status-right "#[fg=cyan]%d %b %R"  # 時間格式可以通過 strftime() 設置
set -g status-utf8 on            # 設置狀態欄
set -g status-interval 60        # 設置狀態欄的更新間隔時間，單位為秒
set -g status-justify centre     # 將窗口列表居中顯示
setw -g monitor-activity on      # 設置監控其它窗口
set -g visual-activity on        # 當其他窗口有消息時會提示

setw -g mode-keys vi                    # 使用 vi 的快捷鍵

bind h select-layout even-horizontal    # 將當前面板佈局切換為 even-horizontal
bind v select-layout even-vertical      # 將當前面板佈局切換為 even-vertical
{% endhighlight %}

### 定製 tmux 啟動腳本

tmux 腳本，可以參考 [tmux_basic](/reference/linux/tmux_basic) 腳本，實際上是一個 shell 腳本，可以通過如下命令的進行不同的組合。也就是通過運行該腳本，生成最終的 tmux 初始環境。

{% highlight text %}
### 1. 創建一個可執行文件
$ touch ~/development && chmod +x ~/development

### 2. 判斷是否存在basic會話，否則下面命令將會出錯
tmux has-session -t basic

### 3. 創建一個新session名稱為basic，新窗口為名為editor，並detach
tmux new-session -s basic -n editor -d

### 4. 向指定pane發送命令，C-m表示回車，使用session:window:pane指定特定的pane
tmux send-keys -t basic 'cd ~' C-m

### 5. 將窗口分為上下兩部分，-h分為左右兩部分，並通過-p指定比例，下和右所佔比例
tmux split-window -v -p 10 -t basic

### 6. 也可以選擇一個默認的分佈
tmux select-layout -t basic main-horizontal

### 7. 創建一個新的窗口
tmux new-window -n editor -t basic

### 8. 選擇一個默認窗口
tmux select-window -t basic:1

### 9. 最後連接到basic
tmux attach -t basic
{% endhighlight %}

實際是一個 Shell 腳本，當然 tmux 的配置文件也可以進行相同的設置。

可以將上述的命令放置到 tmux.conf 配置文件中，此時將會自動執行，也可以將其放置到不同的文件中，然後通過 tmux -f 指定相應的配置文件，第一行中指定需要加載默認配置文件。

{% highlight text %}
# ~/.tmux/aiduppconf
source-file ~/.tmux.conf      # 加載默認的配置選項，非強制，但是建議使用
........ your commands
{% endhighlight %}


tmux 的默認行為是新建一個 session ，因此如果沒有使用 attach 選項，那麼實際會再新建一個 session ，此時會導致有兩個 session 。

<!--
<br><h2>複製/滾屏模式</h2><p>

    Prefix # 列出所有buffers；Prefix = 列出並選擇相應的buffer

	在 tmux 中可以查看緩衝區的內容，並且可以進行復制、粘貼。可以通過yum install xclip安裝xclip。<br><br>

	滾屏，即查看緩衝區的內容，需要進入 copy-mode ，即 PREFIX+[ ，然後就可以用上下鍵來滾動屏幕，配置了 vi 快捷鍵模式，就可以像操作 vi 一樣來滾動屏幕，即 hjkl ，非常的方便，退出直接按 q 鍵或 Enter 即可。<br><br>

	之前已經設定好了使用 vi 模式，因此，可以通過 w 跳轉到下一個單詞，b 跳轉回上一個單詞；f+char 向前跳轉到 char 字符處，F+char 向後跳轉到 char 字符處。使用 g 跳轉到 buffer 的首部，G 跳轉到末尾；Ctrl-b 向後一頁，Ctrl-f 向前翻頁。同時可以使用 ? 和 / 向上和向下搜索，n 查找下一個，N 查找上一個。<br><br><br>


	PREFIX [ 進入複製模式，將光標移動到需要複製 buffer 的地方，按 space 開始複製，移動光標選擇複製區域；按 Enter 複製並退出 copy-mode；需要粘貼時，按 PREIFX ] 粘貼。<br><br>

	可以使用 PREFIX : 並輸入 capture-pane 命令複製一個 pane 的所有內容，然後再使用 Prefix-] 複製。<br><br>

	可以在終端通過 tmux show-buffer 查看 buffer 的內容，在 tmux 的命令行中通過 show-buffer 查看；當然也可以在 Shell 中通過 tmux capture-pane && tmux save buffer.txt 將 buffer 保存在 buffer.txt 文件中，在 tmux 中使用 capture-pane;save-buffer buffer.txt 保存。<br><br>

	tmux 維護了一個堆棧用於保存緩存，每次都會將新的緩存保存在棧頂。在 tmux 中通過 list-buffers 可以查看所有的 buffers 。如果有多個 buffer ，Prefix-] 默認使用第一條，可以通過 choose-buffer 選擇相應的 buffer ，然後通過 Enter 插入到制定pane。<br><br>

	可以通過如下的設置，使得操作時更像 vi 。Escape 切換到複製模式，y (yank) 將內容複製到 buffers ，v 切換到選擇模式，p 用於粘貼文本。
<pre>unbind [
bind Escape copy-mode
unbind p
bind p paste-buffer
bind -t vi-copy 'v' begin-selection
bind -t vi-copy 'y' copy-selection</pre><br>

	tmux 也可以通過 xclip 與系統的粘貼板進行交互，在 Linux 中通過如下命令安裝 apt-get install xclip/yum install xclip 。通過第一行的命令將 tmux 當前的 buffer 輸出到 xclip ；第二行將系統粘貼板的內容複製到 tmux 中。
<pre>bind C-c run "tmux save-buffer - | xclip -i -sel clipboard"<br>bind C-v run "tmux set-buffer \"$(xclip -o -sel clipboard)\"; tmux paste-buffer"</pre>

    當打開mode-mouse時，可以使用“Shift+鼠標”選中文字；Ctrl+Shift+c複製，Ctrl+Shift+v粘貼。
-->


### 顏色設置

可以從 [colortest](http://www.vim.org/scripts/download_script.php?src_id=4568) 上下載該文件，也可以使用 [本地文檔](/reference/linux/tmux_colortest)，通過如下命令測試終端；並在 ~/.bashrc 添加如下內容，此時只會在 TMUX 外設置 TERM ，而 TMUX 會設置自己的終端。

{% highlight text %}
$ perl colortest -w

### 在 ~/.bashrc 中添加如下內容
[ -z "$TMUX" ] && export TERM=xterm-256color
{% endhighlight %}

tmux 支持指定特定的顏色，包括了 black, red, green, yellow, blue, magenta, cyan 和 white，也可以使用 colour0 到 colour255 ，可以簡單的通過如下命令查看。

{% highlight text %}
for i in {0..255}; do printf "\x1b[38;5;${i}mcolour${i}  "; done
{% endhighlight %}

tmux 的各個組件可以通過前景色和後景色進行設置。

在 Putty 中，右擊 -> Change Settings... -> Window -> Colours 選中， Allow terminal to specify ANSI colours 和 Allow terminal to use xterm 256-colour mode ，並在 Indicate bolded text by changing: The colour 。

tmux 啟動時可以使用 -2 參數，此時 tmux 會強制假設終端支持 256 色。

### 設置狀態欄

狀態欄由三部分組成：右面板、窗口列表、左面板。左右面板可以使用文本和變量，常見變量如下所示。

{% highlight text %}
#H                主機名
#h                主機名，不含域名
#F                當前窗口的 flag
#I                當前窗口的序號
#P                當前面板的序號
#S                當前 session 名
#T                當前窗口的 title
#W                當前窗口的名稱
##                字符 #
#(shell-command)  shell 命令輸出的第一行
#[attributs]      改變顏色或者屬性
{% endhighlight %}

### 合作

通過 tmux 可以兩個人進行合作，在一個窗口上的操作可以被另一個人看到。最簡單的是同一個用戶 (Shared Account)，此時一個人創建一個窗口，另一個人 attach 即可。

{% highlight text %}
$ tmux new-session -s Pairing     # 一個用戶
$ tmux attach -t Pairing          # 另一個用戶
{% endhighlight %}

使用同一個用戶 (Shared Account) 和分類會話 (Grouped Sessions) ，此時與上述的相似，不過當一個用戶切換時，另一個用戶不會看到。這樣的話，當兩個人需要使用不同的編輯器進行編譯時，可以各自創建一個新的窗口，各自不會影響。

{% highlight text %}
$ tmux new-session -s Pairing     # 一個用戶
$ tmux attach -t Pairing -s my    # 另一個用戶
{% endhighlight %}

使用不同的用戶，假設為 foo 和 bar 。這種情況下，兩個人可以使用不同的配置選項，如快捷鍵、狀態欄等屬性。

{% highlight text %}
$ sudo adduser ted            # 添加用戶
$ sudo adduser barney

$ sudo addgroup tmux          # 添加用戶組
$ sudo mkdir /var/tmux

$ sudo chgrp tmux /var/tmux   # 修改用戶組

$ sudo chmod g+ws /var/tmux   # 修改屬性

$ sudo usermod -aG tmux ted   # 添加到用戶組
$ sudo usermod -aG tmux barney

$ tmux -S /var/tmux/pairing   # 一個用戶在創建時指定 Sockets
$ tmux -S /var/tmux/pairing attach # 另一個用戶連接
{% endhighlight %}



## 其它

#### 1. pane 轉為 window

通過 Prefix ! 可以將一個 pane 轉作 window。

#### 2. pane 合併到 window

如果想將一個 pane 合併到 window ，可以在 tmux 中通過如下命令執行。

{% highlight text %}
join-pane -s 1.0 -t 2
{% endhighlight %}

如果沒有通過 -t 指定目標，那麼默認為當前的 window ，在指定 pane 時，目標通過 [session_name]:[window].[pane] 指定。


#### 3. 判斷是否存在 session

在一個 Bash 腳本中，可以通過如下的命令查看是否存在一個 Session 。

{% highlight text %}
if ! tmux has-session -t remote; then
   exec tmux new-session -s development -d
   # other setup commands before attaching....
fi
exec tmux attach -t development
{% endhighlight %}



<!--
有時候我們希望將一個 pane 放大，然後查看其內容，最後將其恢復。通常可以通過 break-pane 然後使用 join-pane 將其取回，這樣做會有一些麻煩，下面通過一個臨時的 window 作為容器進行操作。

unbind Up        ## 將 pane 放大
bind Up new-window -d -n tmp \; swap-pane -s tmp.1 \; select-window -t tmp
unbind Down      ## 將窗口恢復
bind Down last-window \; swap-pane -s tmp.1 \; kill-window -t tmp




之前曾經通過 send-keys 執行某一個命令，同樣可以通過如下的方式執行，不過當 ssh 退出時，pane 或者 window 同樣會關閉。

$ tmux new-session -s servers -d "ssh deploy@burns"
$ tmux split-window -v "ssh dba@smithers"
$ tmux attach -t servers
-->


#### 4. 設置默認 bash

tmux 默認使用的是 bash ，如果要使用其它的終端，可以在 tmux.conf 中通過如下的命令指定。

{% highlight text %}
set -g default-command /bin/zsh
set -g default-shell /bin/zsh
{% endhighlight %}

#### 5. TERM設置

當 tmux 運行時，會將 TERM 變量設置為 "screen" 或者配置文件中 default-terminal 指定的值，在 .bashrc 中可以通過判斷該變量來確定現在是否在 tmux 中。

{% highlight text %}
if [[ "$TERM" != "screen-256color" ]]
then
   tmux attach-session -t "$USER" || tmux new-session -s "$USER"
   exit
fi
{% endhighlight %}

如果不在 tmux 中，那麼首先嚐試連接 $USER ，也就是 Session 為用戶名；如果鏈接失敗則會嘗試新建一個，然後退出腳本。

當啟動 tmux 時，仍會執行配置文件，但是由於這次在 tmux 中，因此跳過這段指令。


#### 6. 窗口切換

將窗口在 Session 之間進行切換，使用時通過  Prefix . ，並在命令行中輸入想要切換的 Session。當熱也可以通過如下的命令執行

{% highlight text %}
$ tmux move-window -s processes:1 -t editor
{% endhighlight %}

將 processes 的第一個 window 移動到 editor 。



<!--
之前曾介紹過通過 capture-pane + save-buffer 的方式保存終端的輸出信息。此處介紹通過 pipe-pane ，通過這個命令我們可以控制開關，可以在一個命令已經執行的時候保存輸出。在配置文件中添加如下的內容。
<pre>bind P pipe-pane -o "cat >>~/#W.log" \; display "Toggled logging to ~/#W.log"</pre>
通過 Prefix P 進行切換，-o 指定輸出。
-->

### putty 亂碼設置

在通過 putty 鏈接 tmux 時，可能會出現亂碼，此時需要設置字符集。[右鍵] => [Change Settings...] => [Windows/Translation] => [Remote character set:] 改為 UTF-8 。



## 參考

詳細內容可以參考官方網站 [tmux.github.io](http://tmux.github.io/)，詳細的文檔可以參考 [tmux Productive Mouse-Free Development](/reference/linux/Pragmatic.tmux.Feb.2012.pdf)，版權歸作者所有。

當然也可以通過 [tmux-powerline](https://github.com/erikw/tmux-powerline) 配置狀態欄；關於按鍵的綁定可以參考 [Moving The Ctrl Key](https://www.emacswiki.org/emacs/MovingTheCtrlKey)，以及參考配置文件 [tmux.conf](/reference/linux/tmux.conf) 。

<!--
set -g status-utf8 on
Mouse Settings
set the default bash
set -g utf8
set-window-option -g utf8 on
-->

{% highlight text %}
{% endhighlight %}
