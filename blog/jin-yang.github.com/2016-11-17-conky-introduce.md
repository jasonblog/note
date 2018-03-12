---
title: Conky 配置简介
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: conky,配置
description: Conky 是一个轻量级的桌面系统监控工具，支持多种监控项，而且支持 Python、Lua 脚本。
---

Conky 是一个轻量级的桌面系统监控工具，支持多种监控项，而且支持 Python、Lua 脚本。

<!-- more -->

## 简介

该工具实际是 [torsmo](http://torsmo.sourceforge.net/index.php) 的一个分支，可从 [本地下载](/reference/linux/torsmo-0.18.tar.gz) ，现在已经不再维护。

![conky example]({{ site.url }}/images/linux/conky-example.png "conky example"){: .pull-center width="30%" }

关于 Conky 的详细介绍可以参考 [官网](http://conky.sourceforge.net/) ，其中包括了参数项以及相关设置，其最新版本是 [conky-1.9.0.tar.bz2](/reference/linux/conky-1.9.0.tar.bz2)，使用 C 语言编写。


### 编译安装

在 CentOS 中可以通过如下方式编译。

{% highlight text %}
# yum install libX11-devel lua-devel libXext-devel libXdamage-devel ghc-X11-xft \
      ghc-X11-xft-devel cairo-devel tolua++-devel imlib2-devel
$ ./configure --enable-lua-cairo --enable-imlib2 \
    --enable-double-buffer                       \
    --enable-cairo                               \
    --enable-config-output                       \   可以通过-C参数输出默认的配置文件
$ make
{% endhighlight %}

<!--
HAVE_XDBE     Double Buffer
TCP_PORT_MONITOR  支持
-->



注意，上述的 `ghc-X11-xft-devel` 依赖于 epel，需要将对应的源打开。

在 Ubuntu 中通过如下命令安装。

{% highlight text %}
# apt-get install conky            ← 直接安装 conky
$ conky &                          ← 加载默认配置文件
$ conky -c ~/.conky/conkyrc1 &     ← 指定配置文件
$ conky -c ~/.conky/conkyrc2 &     ← 启动另外一个实例
{% endhighlight %}

此时运行的是默认的配置文件 `/etc/conky/conky.conf`，在设置自己的配置文件时可以将默认文件直接复制过去。在 `/usr/share/doc/conky-std` 中有各个变量的定义。

<!--
下面是一些常用的配置。

设置成真透明<br>
这需要窗口管理器支持Xorg复合特性，比如使用compiz或者启用metacity复合特性。<br>
own_window_argb_visual：布尔值。当为true时启用真透明特性。<br>
own_window_argb_value：当启用真透明时设定不透明度。值的范围是0-255。0是0%不透明度，255是100%不透明。<br>
own_window_transparent：布尔值。当该值为true，并且启用真透明时，设置不透明度为0%。</li><br><li>

时间设置<br>
time: 变量，使用当前的时区。<br>
tztime (timzone (format)): 显示某一个时区，可以在 /usr/share/zoneinfo 中查看，如 US/Pacific 。其中时间的格式可以通过 man date 进行查看，如 ${tztime America/Chicago $H:%M:%S} 。
-->

## 源码解析

<!--
绘制应该是通过draw_stuff()->draw_text()->draw_line()->draw_string()函数进行的，通过for_each_line()显示每一行的内容。<br><br>

绘制的是text_buffer指向的内容，kk

颜色的名称采用的是X11指定的，可以通过/usr/share/X11/rgb.txt查看所支持的颜色。
-->

{% highlight text %}
main()
  |-free_templates()
  |-clear_net_stats()
  |-tcp_portmon_set_max_connections()
  |-initialisation()
  | |-set_default_configurations()
  | |-set_current_config()
  | |-load_config_file()                加载配置文件
  |   |-open_config_file()
  |   |-do_config_step()
  | |-conftree_add()
  | |-load_config_file_x11()
  | | |-X11_initialisation()
  | |   |-init_X11()
  | |   | |-XOpenDisplay()
  | |   | |-DefaultScreen()
  | |   | |-DisplayWidth()
  | |   | |-DisplayHeight()
  | |   | |-get_x11_desktop_info()
  | |   | |-update_workarea()
  | |   |-set_default_configurations_for_x()
  | |-start_update_threading()
  | | |-pthread_create()                创建后台的更新线程run_update_callback()
  | |   |-ucb->func()                   调用其设置的函数
  | |-X11_create_window()
  | | |-init_window()
  | |   |-find_desktop_window()
  | |   |-cimlib_init()
  | |   |-XFlush()
  | |   |-XSelectInput()
  | |-llua_setup_info()
  | |-xmlInitParser()
  | |-llua_startup_hook()
  |
  |-main_loop()
  | |<<<<<< while begin >>>>>>          WHILE循环
  | |-XFlush()
  | |-usleep()                          计算休眠时间，并休眠一段时间
  | |-update_text()                     更新数据
  | | |-cimlib_cleanup()
  | | |-generate_text()
  | | | |-get_time()                    更新current_update_time时间
  | | | |-update_stuff()
  | | | | |-prepare_update()
  | | | | |-sem_post()
  | | | |-generate_text_internal()      主要更新字段
  | | |-clear_text()                    如果是XWindows则先清理
  | | |-llua_update_info()
  | |
  | |-draw_stuff()                      图片相关的绘制
  | | |-cimlib_render()
  | | | |-cimlib_draw_all()
  | | |   |-cimlib_draw_image()
  | | |     |-imlib_load_image()
  | | |     |-imlib_context_set_image()
  | | |     |-imlib_image_set_has_alpha()
  | | |     |-imlib_image_get_width()
  | | |     |-imlib_image_get_height()
  | | |     |-imlib_context_set_image()
  | | |     |-imlib_blend_image_onto_image()
  | | |     |-imlib_free_image()
  | | |-draw_text()
  | |
  | |-sigprocmask()                     信号处理
  | | |-reload_config()                 SIGUSR1处理，重新加载配置文件
  | |   |-clean_up()
  | |     |-free_update_callbacks()
  | |     |-clean_up_without_threads()  清理资源
  | |   |-initialisation()
  | |-inotify_add_watch()               通过inotify监听配置文件
  | |<<<<<<< while end >>>>>>>
  | |-clean_up()
  |
  |-curl_global_cleanup()

extract_variable_text_internal()
  |-construct_text_object()

free_text_objects()


add_update_callback()                   每个OBJ对象第二个参数为非空，都会添加一个后台更新线程
{% endhighlight %}

在开始调试时可以设置 out_to_console 将调试信息输出到终端。

## Conky Colors

通过 Conky Colors 可以配置出非常炫的效果，如下是一个示例。

![conky colors example]({{ site.url }}/images/linux/conky-colors-all.png "conky colors example"){: .pull-center width="90%" }

详细的代码可以查看 [github](https://github.com/helmuthdu/conky_colors) 。

## 其它

### 常见问题

#### 在刷新时闪烁

在 `.conkyrc` 文件中添加如下的内容。

{% highlight text %}
own_window yes
own_window_type override # 有效
double_buffer yes
{% endhighlight %}

#### 中文支持

{% highlight text %}
use_xft yes                            ＃自定义字体
xftalpha 1
xftfont Liberation Sans:size=9         # 设置字体格式
override_utf8_locale yes               # 使用UTF8而非默认的ISO8859-1
{% endhighlight %}

添加如上配置后，是可以支持中文的，如果出现问题一般可能是所使用的字符集可能不支持，可以通过类似如下的格式 `${font WenQuanYi Micro Hei:size=12}` ，进行测试。

#### 图片显示

当开启了 `own_window_transparent yes` 之后，图片显示会有些模糊，暂时还没有想清楚怎么处理。

## 参考

关于 Conky 的配置项以及常用变量可以参考 [config settings](http://conky.sourceforge.net/config_settings.html) 以及 [variables](http://conky.sourceforge.net/variables.html) 。

在 [www.acme.com](http://www.acme.com/software/) 中包含了很多与 XWindows 相关的示例程序；也可以参考 [XOpenDisplay. Programming Xlib Tutorial](http://xopendisplay.hilltopia.ca/index.html)，或者本地保存文档 [Programming_Xlib_Tutorial](/reference/linux/Programming_Xlib_Tutorial.maff) 。

[Conky Manager](https://launchpad.net/conky-manager) 是一个通过 [Gnome Vala](https://wiki.gnome.org/Projects/Vala) 语言编写的图形界面，用于管理 Conky 的配置文件。

<!--
feh 基于imlib的轻量级图片查看工具
scrot 录屏工具
-->

{% highlight text %}
{% endhighlight %}
