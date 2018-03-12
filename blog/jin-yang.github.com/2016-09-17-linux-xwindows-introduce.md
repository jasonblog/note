---
title: XWindow 简介
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: XWindow,linux,图形界面
description: XWindow 是一种以位图方式显示的软件窗口系统，最初是 1984 年麻省理工学院的研究成果，之后变成类 UNIX 操作系统的标准化软件工具包及显示架构协议。
---

XWindow 是一种以位图方式显示的软件窗口系统，最初是 1984 年麻省理工学院的研究成果，之后变成类 UNIX 操作系统的标准化软件工具包及显示架构协议。

这里简单介绍。

<!-- more -->

## 简介

在 Linux 中最常用的是 X Window System，采用的是 server/client 架构。

#### X Server

负责屏幕画面的绘制与显示，以及用户的输入；处在用户端。

接收来自 client 的数据，将这些数据绘制呈现为图面在屏幕上。此外，移动鼠标、点击、由键盘输入等等，也会透过 X Server 来传达到 X Client 端，而由 X Client 来加以运算出应绘制的数据。

#### X Client

负责数据的运算；处在服务器远端。

在接收到 X Server 传来的数据后 (例如移动鼠标、点击 icon 等动作)，会经由本身的运算而得到鼠标应该要如何移动、点击的结果应该要出现什么样的数据、键盘输入的结果如何呈现等，然后将这些结果告知 X Server，让他自行去绘制到屏幕上。


**注意**：这与通常理解的有些区别，Xorg 的服务端运行在本地，而客户端运行在远端。

### Window Manager

运行时通常只有一个 X Server，而有很多个 X Client，如果 X Client 只是根据自己的需求进行绘制，必然会导致叠图的问题，因此需要一组特殊的 X Client 来管理其他的 X Client，这就是 Window Manager。

Window Manager (WM) 是一组控制所有 X client 的管理程序，并同时提供例如任务栏、背景桌面、虚拟桌面、窗口大小、窗口移动与重迭显示等任务。Window manager 主要由一些大型的计划案所开发而来，常见的有 GNOME、KDE、XFCE 等。

### Display Manager

早期的用户在登入系统后，必须要自己先启动 X server 程序，然后再启动个别的 Window manager ，若有其他需求，再启动其他额外的 X client 就是了。为了简化启动个人图形接口的步骤，后来还有所谓的 Display Manager (DM) 。

Display Manager (DM): 提供使用者登入的画面以让用户可以藉由图形接口登入。在使用者登入后，可透过 display manager 的功能去呼叫其他的 Window manager ，让用户在图形接口的登入过程变得更简单，同样可以启动远程桌面。

通常来说，DM 会在显示登录界面前启动 X Server，当用户登出后仍然显示登录界面，类似于字符界面下的 getty 和 login。

### 安装桌面环境

对于 CentOS 来说，可以通过如下方式安装。

{% highlight text %}
# yum groupinstall "X Window System"
# yum groupinstall "GNOME Desktop"
{% endhighlight %}

安装完成之后，在 Windows 中，可以通过 `putty` + `Xming` 完成 XWindow 的显示。

#### 1. 安装 Xming

安装时选择 `Don't install an SSH client`，然后通过 XLaunch 启动时，选择 `Multiple Windows` ，其中 `Display Number` 后面链接时会用到，默认是 0 。

可以将配置保存，然后使用时直接双击即可。

#### 2. 启动 Putty

新建一个会话，其中配置为 `Connection->SSH->X11 [Enable X11 forwarding] [MIT-Magic-Cookie-1]`；另外，其中的 `X display location` 中设置为 `localhost:0` 。

#### 3. 配置 sshd

要允许 Linux 主机上的 SSH X 转发，修改 `/etc/ssh/sshd_config` 文件，加入如下一行。

{% highlight text %}
X11Forwrding yes
{% endhighlight %}

然后重启 sshd 服务即可。

#### 4. 启动程序

安装 VIM ，并通过如下方式启动，其中 `IP` 为 Windows 段的 IP 地址。


{% highlight text %}
# yum install vim-X11
$ set DISPLAY=10.177.219.180:0
$ gvim
{% endhighlight %}

### 实践

在 Ubuntu 中有两种不同的 DM：gdm 和 lightdm，其配置文件在 `/etc/init/` 中。以 gdm 为例，在 `/etc/init/gdm.conf` 中可以看到最后执行了：

{% highlight text %}
exec gdm-binary $CONFIG_FILE
{% endhighlight %}

通过执行如下命令，可以查看启动时程序调用的顺序。

{% highlight text %}
$ pstree -p
{% endhighlight %}

可知 gdm-binary 最后唤醒了 Xorg，同样我们也会知道提供认证的图形画面应该是由 gdm-session 所提供。在 Ubuntu 中，如果想要切换到 gdm，可以使用如下的指令:

{% highlight text %}
# dpkg-reconfigure gdm
{% endhighlight %}

另外，与 X-Window 可以通过 man 查看相关的帮助，如 X、Xserver、Xorg、Xsecurity、xhost、xauth、xinit、startx。

{% highlight text %}
$ ps -e | grep X                # 查看是否启动了Xorg，启动的参数可以通过ps -ef查看
$ ps -e | grep ttyy             # 查看有那些终端启动了Xorg
{% endhighlight %}

#### 简单示例

在 X Window 中，通过 `hostname:display_number.screen_number` 指定一个屏幕，可以理解为：一台计算机有多个 display，一个 display 有多个屏幕。

可以直接运行 X/Xorg 来启动，这两个程序相同，对于本地可以直接通过 `:0`、`:1` 指定 display。

{% highlight text %}
$ X :1 -retro               # 本地再启动一个X-Server
{% endhighlight %}

## XWindow 启动

在命令行界面启动 X 时，通过 `startx` 脚本找到 `X Server` 和 `X Client` 的参数，然后通过 `xinit` 来启动 X 图形界面。

xinit 先载入 `X Server` 到 `:0(tty7:Ubuntu, tty1:CentOS)`，然后加载 X Client 到这个 X 图形界面上，而 X Client 通常是 GNOME 或 KDE ，这两个可以通过 `/etc/sysconfig/desktop` 来设置。

> TWM (TabWindow Manager for the X WindowSystem) 是一个窗口管理器，不像其他的 X 程序，它没有基于任何 GUI 组件，而是直接使用的 XLib，这样带来的好处就是占用资源小、更方便的配置。所谓窗口管理器，它是一个特殊的程序，它用来给X程序提供诸如：标题的绘制、窗口阴影、窗口图标化、用户自定义宏、鼠标点击、键盘焦点、缩放等功能。
>
> 它和 GNOME、KDE 等不同，不是一个桌面环境 (Desktop Enviroment, DE)。那些所谓的桌面环境都会有一个窗口管理器，比如 GNOME 用的就是 MetaCity，这些 DE 集成了大量的应用程序，包括一些非常便利的系统管理工具、实用小工具、游戏等，大大方便了用户。

下面举例来模拟 X Window 的启动过程。

### 1. 启动客户端

启动客户端，在 tty1 中输入：

{% highlight text %}
# X :1 &
{% endhighlight %}

执行完上述指令后会跳转到 tty8 中，此时背景为黑色，中间有一个鼠标的标志。

![xwindow startup 01]({{ site.url }}/images/linux/xwindow-startup-01.gif "xwindow startup 01"){: .pull-center }

上图就是只是启动 X Server 的情况。

### 2. 启动 XTerm

在 X 图形界面中执行虚拟终端，切换到 `tty1` 执行如下指令。

{% highlight text %}
$ xterm -display :1  &
$ xterm -display :1  &
{% endhighlight %}

启动完第一个终端后，可以看到下图所示的界面，此时没有标题，也无法移动。当启动第二个时，将第一个覆盖了，因此实际上只能看到一个。

![xwindow startup 02]({{ site.url }}/images/linux/xwindow-startup-02.gif "xwindow startup 02"){: .pull-center }

### 3. 启动 XClock 等

返回到 `tty1` 中执行如下的两条指令。

{% highlight text %}
xclock -display :1  &
xeyes -display :1  &
{% endhighlight %}

与打开 xterm 类似，此时还是重叠在一起，且不能移动。

![xwindow startup 03]({{ site.url }}/images/linux/xwindow-startup-03.gif "xwindow startup 03"){: .pull-center }

### 4. 启动 TWM

启动 `Window Manager`，这里是 TabWindow Manager for the X WindowSystem, TWM，这是一个很简单的窗口管理器。

{% highlight text %}
# twm -display :1 &
{% endhighlight %}

此时出现了标题栏，可以移动等操作。

![xwindow startup 04]({{ site.url }}/images/linux/xwindow-startup-04.gif "xwindow startup 04"){: .pull-center }

其它的窗口管理器还有 [FVWM](http://www.fvwm.org/)、[IceWM](http://www.icewm.org/)、[enlightenment](https://www.enlightenment.org/)、[TWM](http://xwinman.org/vtwm.php)、[DWM](http://dwm.suckless.org/) 等。

### 5. 关闭

最后将所建立的程序关闭。

{% highlight text %}
$ jobs              # 查看后台运行的程序
$ kill %6
$ kill %5
$ kill %4
$ kill %3
$ kill %2
$ kill %1
{% endhighlight %}

## VNC

`VNC Server` 会运行在 Linux 服务器上，一般可以使用 `tigervnc-server` 作为服务器，而客户端可以运行在 Windows/Linux，一般使用 `vncviewer` 。

大致的工作原理是：A) vnc server 在 Linux 上运行启动了一个 Xserver；B) Linux 下的 x client 通过 x11 协议连接到 vncserver，window 下的 vnc client 通过 vnc 协议连接到 vnc server。

tigervnc 会使用 5900+N 作为服务端口，启动参考如下。

{% highlight text %}
# yum install tigervnc tigervnc-server         # 安装客户端(可选)、服务端
$ vncserver :N                                 # 启动服务端，N为数字，需要提供连接时的密码
$ vncpasswd                                    # 设置密码
$ vncserver -list                              # 查看已经启动的vnc服务
$ vncserver -kill :N                           # 关闭之
{% endhighlight %}

<!--xorg-x11-server-Xorg xorg-x11-xinit xorg-x11-drv-fbdev xorg-x11-drv-mga
dbus: gnome-sesssion依赖的进程通信服务
gnome-session: 管理session，启动gnome-session会自动启动window manager
gnome-panel: 面板程序，任务列表和launcher列表都是gnome panel
gnome-terminal: 图形化终端模拟器
-->

#### CentOS 6

在 CentOS 6 中，可以通过如下方式启动。

{% highlight text %}
# yum install wqy-zenhei-fonts                      # 安装中文字体
$ fc-list :lang=zh                                  # 查看中文字体
# cat /etc/sysconfig/vncservers                     # 修改配置文件
VNCSERVERS="2:jinyang.jy 3:foobar"
VNCSERVERARGS[2]="-geometry 1920x1080 -depth 16"
$ vncpasswd                                         # 设置密码
# /sbin/service vncserver restart                   # 重启服务
# /sbin/chkconfig vncserver on                      # 设置为开机启动

$ dbus-launch --exit-with-session gnome-session &   # 需要设置export DISPLAY=:2
$ gnome-panel &
$ gnome-terminal &
{% endhighlight %}

<!--
输入法
安装ibus拼音后，用im-switcher就可以启用输入法。但是要让emacs能使用输入法，需要设置环境变量XMODIFIER="@im=ibus"
export GTK_IM_MODULE=ibus
export QT_IM_MODULE=ibus
export XMODIFIER="@im=ibus"
#ibus-daemon -xrRd
-->

第一次启动 gnome-session 的时候，只有 window-manager 没有 panel，所以需要手工启动 panel，后续就不用手工起了，gnome-session 会自动启动。

注意，vncviewer 默认根据网络连接状况选择颜色质量，所以即使 server 支持 16bit 颜色，vncviewer 也不一定显示 16bit 颜色，可能需要手工选择图像质量。

<!--
在linux上运行vncconfig才能支持剪贴板共享。要保持vncconfig不退出才能支持剪贴板共享。
  When run with no options, it runs as a kind of "helper" application for Xvnc. Its main purpose when run in this mode is to support clipboard transfer to and from the VNC viewer(s).
-->

#### CentOS 7

默认启动时采用 `/lib/systemd/system/multi-user.target` ，可以通过如下方式修改。

{% highlight text %}
# unlink /etc/systemd/system/default.target
# ln -sf /lib/systemd/system/graphical.target /etc/systemd/system/default.target
# reboot
{% endhighlight %}

然后修改如下内容，可以通过 systemctl 启动。

{% highlight text %}
# cp /lib/systemd/system/vncserver@.service /etc/systemd/system/vncserver@:1.service
# systemctl daemon-reload
# systemctl enable vncserver@:1.service
# systemctl start vncserver@:1.service
# systemctl disable vncserver@:1.service
$ vncpasswd
{% endhighlight %}

### 问题

黑屏出现 `Accept clipboard from viewer` `Send clipboard to viewer` `Send premary seletion to viewer` 选项，此时需要修改 `~/.vnc/xstartup` 文件。

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

从出现时间上看，window manager的演进历史是: tab -> stacking -> composite
1tab window manager

tab WM把所有的窗口不重叠地绘制在显示器上，一般窗口不需要过多装饰
不符合普通人的习惯

2stacking window manager

X11缺省的WM是twm，twm就属于此类。
stacking WM的操作方式是最符合普通人直觉的，它把窗口类比于放在桌面上的很多纸片，纸片堆叠起来。窗口一般都有边框装饰
stacking WM的绘制方法是按窗口的z-order, 先绘制最底层的窗口，然后在绘制上层的窗口。

3composite

compsite WM给每个窗口一个buffer，每个窗口在自己的buffer绘制好，然后由composite WM把多个窗口的buffer再合成起来
因为最后合成，那么composite WM可以实现各种特效

4stacking WM vs composite WM

实际上大部分composite WM的操作方式也是stacking的，只不过绘制窗口的实现方法不一样。
composite WM是各个窗口独立绘图，所以可以并行绘图
因为各个窗口有自己的buffer，所以和stacking方式不同，不会因为某个窗口移动了位置就要通知别的窗口程序重新绘图。



http://blog.csdn.net/fyh2003/article/details/49253713           Wayland与Weston简介
https://www.tizen.org/blogs/tsg/2015/tizen-3.0-milestones-source-code-release
-->









## 其它

### Frame Buffer

FrameBuffer 要比 X-Windows 更趋于底层，通过 FB 把屏幕上的每个点映射成一段线性内存空间，程序可以简单的改变这段内存的值来改变屏幕上某一点的颜色。

X 的高度可移植性就是来自于这种机制，不管是在那种图形环境下，只要有这种机制的实现就可以运行 X。通过如下命令可以查看相关的 FB 设备。

{% highlight text %}
$ ls -l  /dev/fb*
$ cat /dev/fb0 > sreensnap
{% endhighlight %}

通过下面的一个小程序，读取屏幕的属性。

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

    printf("The memory is      :%d\n", finfo.smem_len);        // 内存大小
    printf("The line_length is :%d\n", finfo.line_length);     // 屏幕上一行所占的内存
    printf("The xres is        :%d\n", vinfo.xres);            // 分辨率
    printf("The yres is        :%d\n", vinfo.yres);
    printf("bits_per_pixel is  :%d\n", vinfo.bits_per_pixel);  // 每一点所占内存
    close (fp);
}
{% endhighlight %}

通常上述的计算会有两倍的大小，这是由于缓冲的原因。

### Windows RDP

Remote Desktop Protocol, RDP 通常采用 3389 端口，在 CentOS7 中可以通过 freerdp 链接。

{% highlight text %}
# yum install freerdp                                          # 安装
$ xfreerdp -g 1366x768 -u tsg50 -p tsg000000 223.72.237.133    # 登陆
{% endhighlight %}

## 参考

关于 VNC 的启动可以参考 [VNC](https://wiki.centos.org/zh/HowTos/VNC-Server)，或者参考 [本地文档](/reference/linux/VNC-Server_HOWTO.maff) 。

XWindow 或者 startx 启动过程可以参考 [探究X Window System运行原理与启动过程](http://www.chinaunix.net/old_jh/4/474678.html) 。

<!--
也可以参考本地文档
/reference/linux/XWindow-System-Startup.maff
重启Gnome可以通过 /etc/init.d/gdm restart，重新加载桌面 Alt-F2 ，输入 r 命令。
-->

{% highlight text %}
{% endhighlight %}
