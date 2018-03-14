---
title: Conky 配置簡介
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: conky,配置
description: Conky 是一個輕量級的桌面系統監控工具，支持多種監控項，而且支持 Python、Lua 腳本。
---

Conky 是一個輕量級的桌面系統監控工具，支持多種監控項，而且支持 Python、Lua 腳本。

<!-- more -->

## 簡介

該工具實際是 [torsmo](http://torsmo.sourceforge.net/index.php) 的一個分支，可從 [本地下載](/reference/linux/torsmo-0.18.tar.gz) ，現在已經不再維護。

![conky example]({{ site.url }}/images/linux/conky-example.png "conky example"){: .pull-center width="30%" }

關於 Conky 的詳細介紹可以參考 [官網](http://conky.sourceforge.net/) ，其中包括了參數項以及相關設置，其最新版本是 [conky-1.9.0.tar.bz2](/reference/linux/conky-1.9.0.tar.bz2)，使用 C 語言編寫。


### 編譯安裝

在 CentOS 中可以通過如下方式編譯。

{% highlight text %}
# yum install libX11-devel lua-devel libXext-devel libXdamage-devel ghc-X11-xft \
      ghc-X11-xft-devel cairo-devel tolua++-devel imlib2-devel
$ ./configure --enable-lua-cairo --enable-imlib2 \
    --enable-double-buffer                       \
    --enable-cairo                               \
    --enable-config-output                       \   可以通過-C參數輸出默認的配置文件
$ make
{% endhighlight %}

<!--
HAVE_XDBE     Double Buffer
TCP_PORT_MONITOR  支持
-->



注意，上述的 `ghc-X11-xft-devel` 依賴於 epel，需要將對應的源打開。

在 Ubuntu 中通過如下命令安裝。

{% highlight text %}
# apt-get install conky            ← 直接安裝 conky
$ conky &                          ← 加載默認配置文件
$ conky -c ~/.conky/conkyrc1 &     ← 指定配置文件
$ conky -c ~/.conky/conkyrc2 &     ← 啟動另外一個實例
{% endhighlight %}

此時運行的是默認的配置文件 `/etc/conky/conky.conf`，在設置自己的配置文件時可以將默認文件直接複製過去。在 `/usr/share/doc/conky-std` 中有各個變量的定義。

<!--
下面是一些常用的配置。

設置成真透明<br>
這需要窗口管理器支持Xorg複合特性，比如使用compiz或者啟用metacity複合特性。<br>
own_window_argb_visual：布爾值。當為true時啟用真透明特性。<br>
own_window_argb_value：當啟用真透明時設定不透明度。值的範圍是0-255。0是0%不透明度，255是100%不透明。<br>
own_window_transparent：布爾值。當該值為true，並且啟用真透明時，設置不透明度為0%。</li><br><li>

時間設置<br>
time: 變量，使用當前的時區。<br>
tztime (timzone (format)): 顯示某一個時區，可以在 /usr/share/zoneinfo 中查看，如 US/Pacific 。其中時間的格式可以通過 man date 進行查看，如 ${tztime America/Chicago $H:%M:%S} 。
-->

## 源碼解析

<!--
繪製應該是通過draw_stuff()->draw_text()->draw_line()->draw_string()函數進行的，通過for_each_line()顯示每一行的內容。<br><br>

繪製的是text_buffer指向的內容，kk

顏色的名稱採用的是X11指定的，可以通過/usr/share/X11/rgb.txt查看所支持的顏色。
-->

{% highlight text %}
main()
  |-free_templates()
  |-clear_net_stats()
  |-tcp_portmon_set_max_connections()
  |-initialisation()
  | |-set_default_configurations()
  | |-set_current_config()
  | |-load_config_file()                加載配置文件
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
  | | |-pthread_create()                創建後臺的更新線程run_update_callback()
  | |   |-ucb->func()                   調用其設置的函數
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
  | |<<<<<< while begin >>>>>>          WHILE循環
  | |-XFlush()
  | |-usleep()                          計算休眠時間，並休眠一段時間
  | |-update_text()                     更新數據
  | | |-cimlib_cleanup()
  | | |-generate_text()
  | | | |-get_time()                    更新current_update_time時間
  | | | |-update_stuff()
  | | | | |-prepare_update()
  | | | | |-sem_post()
  | | | |-generate_text_internal()      主要更新字段
  | | |-clear_text()                    如果是XWindows則先清理
  | | |-llua_update_info()
  | |
  | |-draw_stuff()                      圖片相關的繪製
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
  | |-sigprocmask()                     信號處理
  | | |-reload_config()                 SIGUSR1處理，重新加載配置文件
  | |   |-clean_up()
  | |     |-free_update_callbacks()
  | |     |-clean_up_without_threads()  清理資源
  | |   |-initialisation()
  | |-inotify_add_watch()               通過inotify監聽配置文件
  | |<<<<<<< while end >>>>>>>
  | |-clean_up()
  |
  |-curl_global_cleanup()

extract_variable_text_internal()
  |-construct_text_object()

free_text_objects()


add_update_callback()                   每個OBJ對象第二個參數為非空，都會添加一個後臺更新線程
{% endhighlight %}

在開始調試時可以設置 out_to_console 將調試信息輸出到終端。

## Conky Colors

通過 Conky Colors 可以配置出非常炫的效果，如下是一個示例。

![conky colors example]({{ site.url }}/images/linux/conky-colors-all.png "conky colors example"){: .pull-center width="90%" }

詳細的代碼可以查看 [github](https://github.com/helmuthdu/conky_colors) 。

## 其它

### 常見問題

#### 在刷新時閃爍

在 `.conkyrc` 文件中添加如下的內容。

{% highlight text %}
own_window yes
own_window_type override # 有效
double_buffer yes
{% endhighlight %}

#### 中文支持

{% highlight text %}
use_xft yes                            ＃自定義字體
xftalpha 1
xftfont Liberation Sans:size=9         # 設置字體格式
override_utf8_locale yes               # 使用UTF8而非默認的ISO8859-1
{% endhighlight %}

添加如上配置後，是可以支持中文的，如果出現問題一般可能是所使用的字符集可能不支持，可以通過類似如下的格式 `${font WenQuanYi Micro Hei:size=12}` ，進行測試。

#### 圖片顯示

當開啟了 `own_window_transparent yes` 之後，圖片顯示會有些模糊，暫時還沒有想清楚怎麼處理。

## 參考

關於 Conky 的配置項以及常用變量可以參考 [config settings](http://conky.sourceforge.net/config_settings.html) 以及 [variables](http://conky.sourceforge.net/variables.html) 。

在 [www.acme.com](http://www.acme.com/software/) 中包含了很多與 XWindows 相關的示例程序；也可以參考 [XOpenDisplay. Programming Xlib Tutorial](http://xopendisplay.hilltopia.ca/index.html)，或者本地保存文檔 [Programming_Xlib_Tutorial](/reference/linux/Programming_Xlib_Tutorial.maff) 。

[Conky Manager](https://launchpad.net/conky-manager) 是一個通過 [Gnome Vala](https://wiki.gnome.org/Projects/Vala) 語言編寫的圖形界面，用於管理 Conky 的配置文件。

<!--
feh 基於imlib的輕量級圖片查看工具
scrot 錄屏工具
-->

{% highlight text %}
{% endhighlight %}
