---
title: XWindow 簡介
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: XWindow,linux,圖形界面
description: XWindow 是一種以位圖方式顯示的軟件窗口系統，最初是 1984 年麻省理工學院的研究成果，之後變成類 UNIX 操作系統的標準化軟件工具包及顯示架構協議。
---

XWindow 是一種以位圖方式顯示的軟件窗口系統，最初是 1984 年麻省理工學院的研究成果，之後變成類 UNIX 操作系統的標準化軟件工具包及顯示架構協議。

這裡簡單介紹。

<!-- more -->

## 簡介

在 Linux 中最常用的是 X Window System，採用的是 server/client 架構。

#### X Server

負責屏幕畫面的繪製與顯示，以及用戶的輸入；處在用戶端。

接收來自 client 的數據，將這些數據繪製呈現為圖面在屏幕上。此外，移動鼠標、點擊、由鍵盤輸入等等，也會透過 X Server 來傳達到 X Client 端，而由 X Client 來加以運算出應繪製的數據。

#### X Client

負責數據的運算；處在服務器遠端。

在接收到 X Server 傳來的數據後 (例如移動鼠標、點擊 icon 等動作)，會經由本身的運算而得到鼠標應該要如何移動、點擊的結果應該要出現什麼樣的數據、鍵盤輸入的結果如何呈現等，然後將這些結果告知 X Server，讓他自行去繪製到屏幕上。


**注意**：這與通常理解的有些區別，Xorg 的服務端運行在本地，而客戶端運行在遠端。

### Window Manager

運行時通常只有一個 X Server，而有很多個 X Client，如果 X Client 只是根據自己的需求進行繪製，必然會導致疊圖的問題，因此需要一組特殊的 X Client 來管理其他的 X Client，這就是 Window Manager。

Window Manager (WM) 是一組控制所有 X client 的管理程序，並同時提供例如任務欄、背景桌面、虛擬桌面、窗口大小、窗口移動與重迭顯示等任務。Window manager 主要由一些大型的計劃案所開發而來，常見的有 GNOME、KDE、XFCE 等。

### Display Manager

早期的用戶在登入系統後，必須要自己先啟動 X server 程序，然後再啟動個別的 Window manager ，若有其他需求，再啟動其他額外的 X client 就是了。為了簡化啟動個人圖形接口的步驟，後來還有所謂的 Display Manager (DM) 。

Display Manager (DM): 提供使用者登入的畫面以讓用戶可以藉由圖形接口登入。在使用者登入後，可透過 display manager 的功能去呼叫其他的 Window manager ，讓用戶在圖形接口的登入過程變得更簡單，同樣可以啟動遠程桌面。

通常來說，DM 會在顯示登錄界面前啟動 X Server，當用戶登出後仍然顯示登錄界面，類似於字符界面下的 getty 和 login。

### 安裝桌面環境

對於 CentOS 來說，可以通過如下方式安裝。

{% highlight text %}
# yum groupinstall "X Window System"
# yum groupinstall "GNOME Desktop"
{% endhighlight %}

安裝完成之後，在 Windows 中，可以通過 `putty` + `Xming` 完成 XWindow 的顯示。

#### 1. 安裝 Xming

安裝時選擇 `Don't install an SSH client`，然後通過 XLaunch 啟動時，選擇 `Multiple Windows` ，其中 `Display Number` 後面鏈接時會用到，默認是 0 。

可以將配置保存，然後使用時直接雙擊即可。

#### 2. 啟動 Putty

新建一個會話，其中配置為 `Connection->SSH->X11 [Enable X11 forwarding] [MIT-Magic-Cookie-1]`；另外，其中的 `X display location` 中設置為 `localhost:0` 。

#### 3. 配置 sshd

要允許 Linux 主機上的 SSH X 轉發，修改 `/etc/ssh/sshd_config` 文件，加入如下一行。

{% highlight text %}
X11Forwrding yes
{% endhighlight %}

然後重啟 sshd 服務即可。

#### 4. 啟動程序

安裝 VIM ，並通過如下方式啟動，其中 `IP` 為 Windows 段的 IP 地址。


{% highlight text %}
# yum install vim-X11
$ set DISPLAY=10.177.219.180:0
$ gvim
{% endhighlight %}

### 實踐

在 Ubuntu 中有兩種不同的 DM：gdm 和 lightdm，其配置文件在 `/etc/init/` 中。以 gdm 為例，在 `/etc/init/gdm.conf` 中可以看到最後執行了：

{% highlight text %}
exec gdm-binary $CONFIG_FILE
{% endhighlight %}

通過執行如下命令，可以查看啟動時程序調用的順序。

{% highlight text %}
$ pstree -p
{% endhighlight %}

可知 gdm-binary 最後喚醒了 Xorg，同樣我們也會知道提供認證的圖形畫面應該是由 gdm-session 所提供。在 Ubuntu 中，如果想要切換到 gdm，可以使用如下的指令:

{% highlight text %}
# dpkg-reconfigure gdm
{% endhighlight %}

另外，與 X-Window 可以通過 man 查看相關的幫助，如 X、Xserver、Xorg、Xsecurity、xhost、xauth、xinit、startx。

{% highlight text %}
$ ps -e | grep X                # 查看是否啟動了Xorg，啟動的參數可以通過ps -ef查看
$ ps -e | grep ttyy             # 查看有那些終端啟動了Xorg
{% endhighlight %}

#### 簡單示例

在 X Window 中，通過 `hostname:display_number.screen_number` 指定一個屏幕，可以理解為：一臺計算機有多個 display，一個 display 有多個屏幕。

可以直接運行 X/Xorg 來啟動，這兩個程序相同，對於本地可以直接通過 `:0`、`:1` 指定 display。

{% highlight text %}
$ X :1 -retro               # 本地再啟動一個X-Server
{% endhighlight %}

## XWindow 啟動

在命令行界面啟動 X 時，通過 `startx` 腳本找到 `X Server` 和 `X Client` 的參數，然後通過 `xinit` 來啟動 X 圖形界面。

xinit 先載入 `X Server` 到 `:0(tty7:Ubuntu, tty1:CentOS)`，然後加載 X Client 到這個 X 圖形界面上，而 X Client 通常是 GNOME 或 KDE ，這兩個可以通過 `/etc/sysconfig/desktop` 來設置。

> TWM (TabWindow Manager for the X WindowSystem) 是一個窗口管理器，不像其他的 X 程序，它沒有基於任何 GUI 組件，而是直接使用的 XLib，這樣帶來的好處就是佔用資源小、更方便的配置。所謂窗口管理器，它是一個特殊的程序，它用來給X程序提供諸如：標題的繪製、窗口陰影、窗口圖標化、用戶自定義宏、鼠標點擊、鍵盤焦點、縮放等功能。
>
> 它和 GNOME、KDE 等不同，不是一個桌面環境 (Desktop Enviroment, DE)。那些所謂的桌面環境都會有一個窗口管理器，比如 GNOME 用的就是 MetaCity，這些 DE 集成了大量的應用程序，包括一些非常便利的系統管理工具、實用小工具、遊戲等，大大方便了用戶。

下面舉例來模擬 X Window 的啟動過程。

### 1. 啟動客戶端

啟動客戶端，在 tty1 中輸入：

{% highlight text %}
# X :1 &
{% endhighlight %}

執行完上述指令後會跳轉到 tty8 中，此時背景為黑色，中間有一個鼠標的標誌。

![xwindow startup 01]({{ site.url }}/images/linux/xwindow-startup-01.gif "xwindow startup 01"){: .pull-center }

上圖就是隻是啟動 X Server 的情況。

### 2. 啟動 XTerm

在 X 圖形界面中執行虛擬終端，切換到 `tty1` 執行如下指令。

{% highlight text %}
$ xterm -display :1  &
$ xterm -display :1  &
{% endhighlight %}

啟動完第一個終端後，可以看到下圖所示的界面，此時沒有標題，也無法移動。當啟動第二個時，將第一個覆蓋了，因此實際上只能看到一個。

![xwindow startup 02]({{ site.url }}/images/linux/xwindow-startup-02.gif "xwindow startup 02"){: .pull-center }

### 3. 啟動 XClock 等

返回到 `tty1` 中執行如下的兩條指令。

{% highlight text %}
xclock -display :1  &
xeyes -display :1  &
{% endhighlight %}

與打開 xterm 類似，此時還是重疊在一起，且不能移動。

![xwindow startup 03]({{ site.url }}/images/linux/xwindow-startup-03.gif "xwindow startup 03"){: .pull-center }

### 4. 啟動 TWM

啟動 `Window Manager`，這裡是 TabWindow Manager for the X WindowSystem, TWM，這是一個很簡單的窗口管理器。

{% highlight text %}
# twm -display :1 &
{% endhighlight %}

此時出現了標題欄，可以移動等操作。

![xwindow startup 04]({{ site.url }}/images/linux/xwindow-startup-04.gif "xwindow startup 04"){: .pull-center }

其它的窗口管理器還有 [FVWM](http://www.fvwm.org/)、[IceWM](http://www.icewm.org/)、[enlightenment](https://www.enlightenment.org/)、[TWM](http://xwinman.org/vtwm.php)、[DWM](http://dwm.suckless.org/) 等。

### 5. 關閉

最後將所建立的程序關閉。

{% highlight text %}
$ jobs              # 查看後臺運行的程序
$ kill %6
$ kill %5
$ kill %4
$ kill %3
$ kill %2
$ kill %1
{% endhighlight %}

## VNC

`VNC Server` 會運行在 Linux 服務器上，一般可以使用 `tigervnc-server` 作為服務器，而客戶端可以運行在 Windows/Linux，一般使用 `vncviewer` 。

大致的工作原理是：A) vnc server 在 Linux 上運行啟動了一個 Xserver；B) Linux 下的 x client 通過 x11 協議連接到 vncserver，window 下的 vnc client 通過 vnc 協議連接到 vnc server。

tigervnc 會使用 5900+N 作為服務端口，啟動參考如下。

{% highlight text %}
# yum install tigervnc tigervnc-server         # 安裝客戶端(可選)、服務端
$ vncserver :N                                 # 啟動服務端，N為數字，需要提供連接時的密碼
$ vncpasswd                                    # 設置密碼
$ vncserver -list                              # 查看已經啟動的vnc服務
$ vncserver -kill :N                           # 關閉之
{% endhighlight %}

<!--xorg-x11-server-Xorg xorg-x11-xinit xorg-x11-drv-fbdev xorg-x11-drv-mga
dbus: gnome-sesssion依賴的進程通信服務
gnome-session: 管理session，啟動gnome-session會自動啟動window manager
gnome-panel: 面板程序，任務列表和launcher列表都是gnome panel
gnome-terminal: 圖形化終端模擬器
-->

#### CentOS 6

在 CentOS 6 中，可以通過如下方式啟動。

{% highlight text %}
# yum install wqy-zenhei-fonts                      # 安裝中文字體
$ fc-list :lang=zh                                  # 查看中文字體
# cat /etc/sysconfig/vncservers                     # 修改配置文件
VNCSERVERS="2:jinyang.jy 3:foobar"
VNCSERVERARGS[2]="-geometry 1920x1080 -depth 16"
$ vncpasswd                                         # 設置密碼
# /sbin/service vncserver restart                   # 重啟服務
# /sbin/chkconfig vncserver on                      # 設置為開機啟動

$ dbus-launch --exit-with-session gnome-session &   # 需要設置export DISPLAY=:2
$ gnome-panel &
$ gnome-terminal &
{% endhighlight %}

<!--
輸入法
安裝ibus拼音後，用im-switcher就可以啟用輸入法。但是要讓emacs能使用輸入法，需要設置環境變量XMODIFIER="@im=ibus"
export GTK_IM_MODULE=ibus
export QT_IM_MODULE=ibus
export XMODIFIER="@im=ibus"
#ibus-daemon -xrRd
-->

第一次啟動 gnome-session 的時候，只有 window-manager 沒有 panel，所以需要手工啟動 panel，後續就不用手工起了，gnome-session 會自動啟動。

注意，vncviewer 默認根據網絡連接狀況選擇顏色質量，所以即使 server 支持 16bit 顏色，vncviewer 也不一定顯示 16bit 顏色，可能需要手工選擇圖像質量。

<!--
在linux上運行vncconfig才能支持剪貼板共享。要保持vncconfig不退出才能支持剪貼板共享。
  When run with no options, it runs as a kind of "helper" application for Xvnc. Its main purpose when run in this mode is to support clipboard transfer to and from the VNC viewer(s).
-->

#### CentOS 7

默認啟動時採用 `/lib/systemd/system/multi-user.target` ，可以通過如下方式修改。

{% highlight text %}
# unlink /etc/systemd/system/default.target
# ln -sf /lib/systemd/system/graphical.target /etc/systemd/system/default.target
# reboot
{% endhighlight %}

然後修改如下內容，可以通過 systemctl 啟動。

{% highlight text %}
# cp /lib/systemd/system/vncserver@.service /etc/systemd/system/vncserver@:1.service
# systemctl daemon-reload
# systemctl enable vncserver@:1.service
# systemctl start vncserver@:1.service
# systemctl disable vncserver@:1.service
$ vncpasswd
{% endhighlight %}

### 問題

黑屏出現 `Accept clipboard from viewer` `Send clipboard to viewer` `Send premary seletion to viewer` 選項，此時需要修改 `~/.vnc/xstartup` 文件。

{% highlight text %}
#!/bin/sh
unset SESSION_MANAGER
unset DBUS_SESSION_BUS_ADDRESS
#exec /etc/X11/xinit/xinitrc
[ -x /etc/vnc/xstartup ] && exec /etc/vnc/xstartup
[ -r $HOME/.Xresources ] && xrdb $HOME/.Xresources
xsetroot -solid grey
vncconfig -iconic -nowin &
gnome-session &
{% endhighlight %}



<!--
tab vs stacking vs composite

從出現時間上看，window manager的演進歷史是: tab -> stacking -> composite
1tab window manager

tab WM把所有的窗口不重疊地繪製在顯示器上，一般窗口不需要過多裝飾
不符合普通人的習慣

2stacking window manager

X11缺省的WM是twm，twm就屬於此類。
stacking WM的操作方式是最符合普通人直覺的，它把窗口類比於放在桌面上的很多紙片，紙片堆疊起來。窗口一般都有邊框裝飾
stacking WM的繪製方法是按窗口的z-order, 先繪製最底層的窗口，然後在繪製上層的窗口。

3composite

compsite WM給每個窗口一個buffer，每個窗口在自己的buffer繪製好，然後由composite WM把多個窗口的buffer再合成起來
因為最後合成，那麼composite WM可以實現各種特效

4stacking WM vs composite WM

實際上大部分composite WM的操作方式也是stacking的，只不過繪製窗口的實現方法不一樣。
composite WM是各個窗口獨立繪圖，所以可以並行繪圖
因為各個窗口有自己的buffer，所以和stacking方式不同，不會因為某個窗口移動了位置就要通知別的窗口程序重新繪圖。



http://blog.csdn.net/fyh2003/article/details/49253713           Wayland與Weston簡介
https://www.tizen.org/blogs/tsg/2015/tizen-3.0-milestones-source-code-release
-->









## 其它

### Frame Buffer

FrameBuffer 要比 X-Windows 更趨於底層，通過 FB 把屏幕上的每個點映射成一段線性內存空間，程序可以簡單的改變這段內存的值來改變屏幕上某一點的顏色。

X 的高度可移植性就是來自於這種機制，不管是在那種圖形環境下，只要有這種機制的實現就可以運行 X。通過如下命令可以查看相關的 FB 設備。

{% highlight text %}
$ ls -l  /dev/fb*
$ cat /dev/fb0 > sreensnap
{% endhighlight %}

通過下面的一個小程序，讀取屏幕的屬性。

{% highlight c %}
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/mman.h>

int main ()
{
    int    fp=0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;

    fp = open("/dev/fb0", O_RDWR);
    if (fp < 0){
        printf("Error: Can not open framebuffer device\n");
        exit(EXIT_FAILURE);
    }

    if (ioctl(fp, FBIOGET_FSCREENINFO, &finfo)) {
        printf("Error reading fixed information\n");
        exit(EXIT_FAILURE);
    }

    if (ioctl(fp, FBIOGET_VSCREENINFO, &vinfo)){
        printf("Error reading variable information\n");
        exit(EXIT_FAILURE);
    }

    printf("The memory is      :%d\n", finfo.smem_len);        // 內存大小
    printf("The line_length is :%d\n", finfo.line_length);     // 屏幕上一行所佔的內存
    printf("The xres is        :%d\n", vinfo.xres);            // 分辨率
    printf("The yres is        :%d\n", vinfo.yres);
    printf("bits_per_pixel is  :%d\n", vinfo.bits_per_pixel);  // 每一點所佔內存
    close (fp);
}
{% endhighlight %}

通常上述的計算會有兩倍的大小，這是由於緩衝的原因。

### Windows RDP

Remote Desktop Protocol, RDP 通常採用 3389 端口，在 CentOS7 中可以通過 freerdp 鏈接。

{% highlight text %}
# yum install freerdp                                          # 安裝
$ xfreerdp -g 1366x768 -u tsg50 -p tsg000000 223.72.237.133    # 登陸
{% endhighlight %}

## 參考

關於 VNC 的啟動可以參考 [VNC](https://wiki.centos.org/zh/HowTos/VNC-Server)，或者參考 [本地文檔](/reference/linux/VNC-Server_HOWTO.maff) 。

XWindow 或者 startx 啟動過程可以參考 [探究X Window System運行原理與啟動過程](http://www.chinaunix.net/old_jh/4/474678.html) 。

<!--
也可以參考本地文檔
/reference/linux/XWindow-System-Startup.maff
重啟Gnome可以通過 /etc/init.d/gdm restart，重新加載桌面 Alt-F2 ，輸入 r 命令。
-->

{% highlight text %}
{% endhighlight %}
