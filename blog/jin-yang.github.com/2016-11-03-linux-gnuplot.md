---
title: Linux 繪圖工具
layout: post
comments: true
language: chinese
category: [linux]
keywords: gnuplot,graphviz,繪圖
description: Ggnuplot 是一個命令行驅動的繪圖工具，可將數學函數或數值資料以平面圖或立體圖的形式畫在不同種類終端機或繪圖輸出裝置上，是由 Colin Kelley 和 Thomas Williams 於 1986 年開發的繪圖程序發展而來的，可以在多個平臺下使用。graphviz 是貝爾實驗室開發的一個開源的工具包，它使用一個特定的 DSL(領域特定語言) dot 作為腳本語言，然後使用佈局引擎來解析此腳本，並完成自動佈局。該軟件包提供豐富的導出格式，如常用的圖片格式，SVG、PDF 格式等。在此介紹一下常見的使用方法。
---

主要介紹一下 Linux 中的 Gnuplot Graphviz 套件，主要用於畫圖。

Ggnuplot 是一個命令行驅動的繪圖工具，可將數學函數或數值資料以平面圖或立體圖的形式畫在不同種類終端機或繪圖輸出裝置上，是由 Colin Kelley 和 Thomas Williams 於 1986 年開發的繪圖程序發展而來的，可以在多個平臺下使用。

graphviz 是貝爾實驗室開發的一個開源的工具包，它使用一個特定的 DSL(領域特定語言) dot 作為腳本語言，然後使用佈局引擎來解析此腳本，並完成自動佈局。該軟件包提供豐富的導出格式，如常用的圖片格式，SVG、PDF 格式等。

在此介紹一下常見的使用方法。

<!-- more -->

## Graphviz

graphviz 的設計初衷是對有向圖/無向圖等進行自動佈局，開發人員使用 dot 腳本定義圖形元素，然後選擇算法進行佈局，最終導出結果。

該軟件中包含了眾多的佈局器：

* dot 默認佈局方式，主要用於有向圖
* neato 基於spring-model(又稱force-based)算法
* twopi 徑向佈局
* circo 圓環佈局
* fdp 用於無向圖

默認 CentOS 是安裝了 dot 以及 graphviz，可以直接使用。


### DOT

dot 可以用來表示有向圖 (->) 以及無向圖 (--)，每個節點使用不同的屬性，例如顏色、形狀、線形等。最簡單的示例如下。

{% highlight text %}
$ cat foobar.dot                                # 查看文件
digraph G {
    hello -> world;
}
$ dot -Tpng foobar.dot -o foobar.png            # 生成png
$ eog foobar.png                                # 預覽一下
{% endhighlight %}

graphviz 使用三種對象 node、edge、graph 來描述一個圖，一個節點的 shape 定義了頂點的形狀，代表類型有這麼幾種：1) 多邊形類型；2) 純文本類型； 3) 基於記錄類型。

相關示例可以參考 [Drawing graphs with dot](http://www.graphviz.org/pdf/dotguide.pdf)，或者 [本地文檔](/reference/linux/dotguide.pdf) 。



## Gunplot

現在有很多的科學作圖軟件，如 Windows 平臺上的 Origin、Tecplot、SigmaPlot 等，*NIX 平臺上的 LabPlot、gnuplot 等，其它的計算軟件如 Matlab、Mathematica、Maple、Scilab、IDL、Maxima 等對科學作圖也有很好的支持。

而 gnuplot 是典型的 UNIX 哲學的產物，小巧、靈活、擅長與其他工具協同；既支持命令行交互模式，也支持腳本。

如下兩個就是通過 gnuplot 繪製的圖片。

![gnuplot examples]({{ site.url }}/images/linux/gnuplot-examples-1.png "an example picture")
![gnuplot examples]({{ site.url }}/images/linux/gnuplot-examples-2.png "an example picture")

在 CentOS 中，可以直接通過 yum install gnuplot 安裝即可。

{% highlight text %}
gnuplot> plot sin(x)                              # 繪製sin(x)曲線
gnuplot> set xrange [-pi:pi]                      # 設置X的取值範圍
gnuplot> replot cos(x) with points pointtype 2    # 用點圖重繪

----- 效果同上
gnuplot> plot [-pi:pi] sin(x), cos(x) with points pointtype 2
{% endhighlight %}

## 常見示例

### 設置輸出

包括了輸出文件的格式，輸出的步驟。

{% highlight text %}
set terminal wxt                # 輸出到電腦屏幕
set terminal postscript eps color solid linewidth 2 font "Helvetica,20"  # 輸出為eps格式
set terminal pdfcairo linewidth 2 font "Times New Roman,8"
set terminal pngcairo linewidth 2 font "AR PL UKai CN, 14"

set output "hello.eps"
set title 'Hello World Gnuplot'
plot [-pi:pi] sin(x), cos(x) with points pointtype 2
set output                      # 標示輸出結束
{% endhighlight %}

eps 為 LaTeX 的常用圖片格式，支持高質量的矢量圖形，並且可以方便轉換為 pdf、svg 等其它常用格式。如上是將輸出設置為有色、採用實線、設置為 2 倍線寬、且字體採用 "Helvetica" 20 號字體。

另外，對於字體常用的還有 Times-Roman、Arial、Courier (等寬字體)、Symbol (希臘字母)。

### 命名圖和座標軸

{% highlight text %}
set key top left                  # 設置圖標
set key box                       # 圖標外添加一個框
set title 'Hello World Gnuplot'
set xlabel 'Angle, in degrees'
set ylabel 'sin(angle)'
set xrange [-pi:pi]             　# 設置X軸的取之範圍，並設置角度顯示
set xtics ('0' 0, '90' pi/2, '-90' -pi/2, '45' pi/4,'-45' -pi/4,'135' 3*pi/4,'-135' -3*pi/4)
set grid                          # 顯示網格
plot sin(x)
{% endhighlight %}

![gnuplot multi lines]({{ site.url }}/images/linux/gnuplot-keys-grids.png "gnuplot multi lines"){: .pull-center }

### 多條曲線

with 子句可以詳細而精確地指定線的樣式。

{% highlight text %}
plot sin(x) with linespoints pointtype 5, cos(x) w boxes lt 4

       with/w    指定繪製線的格式
  linespoints    在每個數據點處標記一個點
        boxes    適合繪製直方圖數據
 pointtype/pt    點類型為5
  linetype/lt    繪製採用第4種線
{% endhighlight %}

![gnuplot multi lines]({{ site.url }}/images/linux/gnuplot-multi-lines.png "gnuplot multi lines"){: .pull-center }









<!--

例 5 將數據文件中的數據畫出：

    plot sin(x), ‘1.dat’

其中1.dat 為一數據文件，每一行描述一點座標位置。 內容如下，其中 # 後面的內容為註釋：

    # $Id: 1.dat,v 1.1.1.1 1998/04/15 19:16:40 lhecking Exp $
    -20.000000 -3.041676
    -19.000000 -3.036427
    -18.000000 -3.030596
    -17.000000 -3.024081
    -16.000000 -3.016755
    -15.000000 -3.008456
    ……

圖 2 Plotting sin(x), data file – 1.dat




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

## 參考

[Graphviz - Graph Visualization Software](http://www.graphviz.org/) 官方網站，其中的文檔中包括了在 C 語言中使用。

詳細可以參考官方文檔 [gnuplot homepage](http://www.gnuplot.info/)，其示例可以查看 [www.gnuplotting.org](http://www.gnuplotting.org/)，其中的動態圖片示例可以參考 [www.gnuplotting.org](http://www.gnuplotting.org/tag/animation/) 。

{% highlight python %}
{% endhighlight %}
