---
title: Linux 绘图工具
layout: post
comments: true
language: chinese
category: [linux]
keywords: gnuplot,graphviz,绘图
description: Ggnuplot 是一个命令行驱动的绘图工具，可将数学函数或数值资料以平面图或立体图的形式画在不同种类终端机或绘图输出装置上，是由 Colin Kelley 和 Thomas Williams 于 1986 年开发的绘图程序发展而来的，可以在多个平台下使用。graphviz 是贝尔实验室开发的一个开源的工具包，它使用一个特定的 DSL(领域特定语言) dot 作为脚本语言，然后使用布局引擎来解析此脚本，并完成自动布局。该软件包提供丰富的导出格式，如常用的图片格式，SVG、PDF 格式等。在此介绍一下常见的使用方法。
---

主要介绍一下 Linux 中的 Gnuplot Graphviz 套件，主要用于画图。

Ggnuplot 是一个命令行驱动的绘图工具，可将数学函数或数值资料以平面图或立体图的形式画在不同种类终端机或绘图输出装置上，是由 Colin Kelley 和 Thomas Williams 于 1986 年开发的绘图程序发展而来的，可以在多个平台下使用。

graphviz 是贝尔实验室开发的一个开源的工具包，它使用一个特定的 DSL(领域特定语言) dot 作为脚本语言，然后使用布局引擎来解析此脚本，并完成自动布局。该软件包提供丰富的导出格式，如常用的图片格式，SVG、PDF 格式等。

在此介绍一下常见的使用方法。

<!-- more -->

## Graphviz

graphviz 的设计初衷是对有向图/无向图等进行自动布局，开发人员使用 dot 脚本定义图形元素，然后选择算法进行布局，最终导出结果。

该软件中包含了众多的布局器：

* dot 默认布局方式，主要用于有向图
* neato 基于spring-model(又称force-based)算法
* twopi 径向布局
* circo 圆环布局
* fdp 用于无向图

默认 CentOS 是安装了 dot 以及 graphviz，可以直接使用。


### DOT

dot 可以用来表示有向图 (->) 以及无向图 (--)，每个节点使用不同的属性，例如颜色、形状、线形等。最简单的示例如下。

{% highlight text %}
$ cat foobar.dot                                # 查看文件
digraph G {
    hello -> world;
}
$ dot -Tpng foobar.dot -o foobar.png            # 生成png
$ eog foobar.png                                # 预览一下
{% endhighlight %}

graphviz 使用三种对象 node、edge、graph 来描述一个图，一个节点的 shape 定义了顶点的形状，代表类型有这么几种：1) 多边形类型；2) 纯文本类型； 3) 基于记录类型。

相关示例可以参考 [Drawing graphs with dot](http://www.graphviz.org/pdf/dotguide.pdf)，或者 [本地文档](/reference/linux/dotguide.pdf) 。



## Gunplot

现在有很多的科学作图软件，如 Windows 平台上的 Origin、Tecplot、SigmaPlot 等，*NIX 平台上的 LabPlot、gnuplot 等，其它的计算软件如 Matlab、Mathematica、Maple、Scilab、IDL、Maxima 等对科学作图也有很好的支持。

而 gnuplot 是典型的 UNIX 哲学的产物，小巧、灵活、擅长与其他工具协同；既支持命令行交互模式，也支持脚本。

如下两个就是通过 gnuplot 绘制的图片。

![gnuplot examples]({{ site.url }}/images/linux/gnuplot-examples-1.png "an example picture")
![gnuplot examples]({{ site.url }}/images/linux/gnuplot-examples-2.png "an example picture")

在 CentOS 中，可以直接通过 yum install gnuplot 安装即可。

{% highlight text %}
gnuplot> plot sin(x)                              # 绘制sin(x)曲线
gnuplot> set xrange [-pi:pi]                      # 设置X的取值范围
gnuplot> replot cos(x) with points pointtype 2    # 用点图重绘

----- 效果同上
gnuplot> plot [-pi:pi] sin(x), cos(x) with points pointtype 2
{% endhighlight %}

## 常见示例

### 设置输出

包括了输出文件的格式，输出的步骤。

{% highlight text %}
set terminal wxt                # 输出到电脑屏幕
set terminal postscript eps color solid linewidth 2 font "Helvetica,20"  # 输出为eps格式
set terminal pdfcairo linewidth 2 font "Times New Roman,8"
set terminal pngcairo linewidth 2 font "AR PL UKai CN, 14"

set output "hello.eps"
set title 'Hello World Gnuplot'
plot [-pi:pi] sin(x), cos(x) with points pointtype 2
set output                      # 标示输出结束
{% endhighlight %}

eps 为 LaTeX 的常用图片格式，支持高质量的矢量图形，并且可以方便转换为 pdf、svg 等其它常用格式。如上是将输出设置为有色、采用实线、设置为 2 倍线宽、且字体采用 "Helvetica" 20 号字体。

另外，对于字体常用的还有 Times-Roman、Arial、Courier (等宽字体)、Symbol (希腊字母)。

### 命名图和坐标轴

{% highlight text %}
set key top left                  # 设置图标
set key box                       # 图标外添加一个框
set title 'Hello World Gnuplot'
set xlabel 'Angle, in degrees'
set ylabel 'sin(angle)'
set xrange [-pi:pi]             　# 设置X轴的取之范围，并设置角度显示
set xtics ('0' 0, '90' pi/2, '-90' -pi/2, '45' pi/4,'-45' -pi/4,'135' 3*pi/4,'-135' -3*pi/4)
set grid                          # 显示网格
plot sin(x)
{% endhighlight %}

![gnuplot multi lines]({{ site.url }}/images/linux/gnuplot-keys-grids.png "gnuplot multi lines"){: .pull-center }

### 多条曲线

with 子句可以详细而精确地指定线的样式。

{% highlight text %}
plot sin(x) with linespoints pointtype 5, cos(x) w boxes lt 4

       with/w    指定绘制线的格式
  linespoints    在每个数据点处标记一个点
        boxes    适合绘制直方图数据
 pointtype/pt    点类型为5
  linetype/lt    绘制采用第4种线
{% endhighlight %}

![gnuplot multi lines]({{ site.url }}/images/linux/gnuplot-multi-lines.png "gnuplot multi lines"){: .pull-center }









<!--

例 5 将数据文件中的数据画出：

    plot sin(x), ‘1.dat’

其中1.dat 为一数据文件，每一行描述一点坐标位置。 内容如下，其中 # 后面的内容为注释：

    # $Id: 1.dat,v 1.1.1.1 1998/04/15 19:16:40 lhecking Exp $
    -20.000000 -3.041676
    -19.000000 -3.036427
    -18.000000 -3.030596
    -17.000000 -3.024081
    -16.000000 -3.016755
    -15.000000 -3.008456
    ……

图 2 Plotting sin(x), data file – 1.dat




例 10 Multiplot 示例：
[plain] view plain copy
    set xrange [-pi:pi]
    # gnuplot recommends setting the size and origin before going to multiplot mode
    # This sets up bounding boxes and may be required on some terminals
    set size 1,1
    set origin 0,0

    # Done interactively, this takes gnuplot into multiplot mode
    set multiplot

    # plot the first graph so that it takes a quarter of the screen
    set size 0.5,0.5
    set origin 0,0.5
    plot sin(x)

    # plot the second graph so that it takes a quarter of the screen
    set size 0.5,0.5
    set origin 0,0
    plot 1/sin(x)

    # plot the third graph so that it takes a quarter of the screen
    set size 0.5,0.5
    set origin 0.5,0.5
    plot cos(x)

    # plot the fourth graph so that it takes a quarter of the screen
    set size 0.5,0.5
    set origin 0.5,0
    plot 1/cos(x)

    # On some terminals, nothing gets plotted until this command is issued
    unset multiplot
    # remove all customization
    reset
-->

## 参考

[Graphviz - Graph Visualization Software](http://www.graphviz.org/) 官方网站，其中的文档中包括了在 C 语言中使用。

详细可以参考官方文档 [gnuplot homepage](http://www.gnuplot.info/)，其示例可以查看 [www.gnuplotting.org](http://www.gnuplotting.org/)，其中的动态图片示例可以参考 [www.gnuplotting.org](http://www.gnuplotting.org/tag/animation/) 。

{% highlight python %}
{% endhighlight %}
