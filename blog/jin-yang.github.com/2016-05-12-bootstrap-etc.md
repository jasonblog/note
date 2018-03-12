---
title: Bootstrap
layout: post
comments: true
language: chinese
category: [linux]
keywords: bootstrap,html,css,javascript
description: Bootstrap 来自 Twitter，基于 HTML、CSS、JavaScript，是目前最受欢迎的前端框架，它简洁灵活，使得 Web 开发更加快捷。
---

Bootstrap 来自 Twitter，基于 HTML、CSS、JavaScript，是目前最受欢迎的前端框架，它简洁灵活，使得 Web 开发更加快捷。

在这里我们看下 bootstrap 以及一些 html、css 等相关的内容。

<!-- more -->


## 简介

Bootstrap 是由 Twitter 的 MARK OTTO 和 Jacob Thornton 所设计和建立，在开源后，社区惊人地活跃，代码版本进化非常快速，而且官方文档质量极其高，并涌现了许多基于 Bootstrap 建设的网站，其界面清新、简洁，排版利落大方。

目前的最新版本已经快到 V4 了，可以参考 [www.getbootstrap.com](https://getbootstrap.com/)，另外可以查看中文网站 [Bootstrap 中文网](http://www.bootcss.com/) ，其中后者包括了一些常用的网站，比如编码规范、jQuery API 中文文档、经典的网站实例等等。


### 安装

可以直接从上述的中文网站中下载 《用于生产环境的 Bootstrap》，以 V3 为例，解压后的目录结构如下：

![bootstrap files](/images/webserver/bootstrap-files.png "bootstrap files"){: .pull-center}

对于 Nginx，可以直接将 root 指向该目录即可。

### 栅格系统

也被称为网格系统 (Grid System)，Bootstrap 提供了一套响应式、移动设备优先的流式网格系统，随着屏幕或 viewport 尺寸的增加，系统会自动分为最多 12 列。

其在 CSS 中的定义如下，会根据屏幕的大小来决定 width 大小。

{% highlight css %}
.container {
    padding-right: 15px;
    padding-left: 15px;
    margin-right: auto;
    margin-left: auto;
}
@media (min-width: 768px) {
    .container {
        width: 750px;
    }
}
@media (min-width: 992px) {
    .container {
        width: 970px;
    }
}
@media (min-width: 1200px) {
    .container {
        width: 1170px;
    }
}
{% endhighlight %}

如下是网格系统的定义。

![bootstrap grid system](/images/webserver/bootstrap-grid-system.png "bootstrap grid system"){: .pull-center}

网格系统总共提供了如下的四种。

{% highlight text %}
.c0l-xs-　　无论屏幕宽度如何，单元格都在一行，宽度按照百分比设置；试用于手机；
.col-sm-　　屏幕大于768px时，单元格在一行显示；屏幕小于768px时，独占一行；试用于平板；
.col-md-　　屏幕大于992px时，单元格在一行显示；屏幕小于992px时，独占一行；试用于桌面显示器；
.col-lg-　　屏幕大于1200px时，单元格在一行显示；屏幕小于1200px时，独占一行；适用于大型桌面显示器；
{% endhighlight %}

与此相关的详细内容，也可以参考如下。

![bootstrap grid system](/images/webserver/bootstrap-grid-system-2.png "bootstrap grid system"){: .pull-center width="90%"}

假设有如下内容。

{% highlight text %}
<div class="container">
    <div class="row">
        <div class="col-xs-12 col-sm-6 col-md-8">.col-xs-12 .col-sm-6 .col-md-8</div>
        <div class="col-xs-6 col-md-4">.col-xs-6 .col-md-4</div>
    </div>
    <div class="row">
        <div class="col-xs-6 col-sm-4">.col-xs-6 .col-sm-4</div>
        <div class="col-xs-6 col-sm-4">.col-xs-6 .col-sm-4</div>
        <div class="col-xs-6 col-sm-4">.col-xs-6 .col-sm-4</div>
    </div>
</div>
{% endhighlight %}

当不同的屏幕大小时，会有不同的表格系统，内容如下。

##### 屏幕大于 992px

.col-md-8 和.col-md-4 还处于作用范围内。

![bootstrap grid system](/images/webserver/bootstrap-grid-system-3.png "bootstrap grid system"){: .pull-center width="95%"}

##### 屏幕范围 769px~992px

.col-md- 已失效，而 .col-sm- 还处在作用范围内。

![bootstrap grid system](/images/webserver/bootstrap-grid-system-4.png "bootstrap grid system"){: .pull-center width="80%"}

##### 屏幕小于 768px

 .col-sm-已失效只有 .col-xs- 不受屏幕宽度影响，这时候就由 .col-xs- 起作用。

![bootstrap grid system](/images/webserver/bootstrap-grid-system-5.png "bootstrap grid system"){: .pull-center width="70%"}

### 常用工具

除了上述的表格系统，Bootstrap 提供了一些辅助类，以便更快地实现对移动设备友好的开发。

这些可以通过合大型、小型和中型设备，实现内容对设备的显示和隐藏，主要包括了两类：.hidden-print、.hidden-xs、.visible-xs-* 等。




<!--

### carousel
http://blog.eol3.com/archives/938

### Modal

http://nakupanda.github.io/bootstrap3-dialog/
http://getbootstrap.com/javascript/

-->



### Affix & Scrollspy

附加导航（Affix）插件允许某个 &lt;div&gt; 固定在页面的某个位置，也可以在打开或关闭使用该插件之间进行切换。一个常见的例子是社交图标。它们将在某个位置开始，但当页面点击某个标记，该 &lt;div&gt; 会锁定在某个位置，不会随着页面其他部分一起滚动。

另外，可以使用 ScrollSpy (滚动监听)。

可参考 [Understanding Bootstrap’s Affix and ScrollSpy plugins](https://www.sitepoint.com/understanding-bootstraps-affix-scrollspy-plugins/)，其示例可以 [参考本地](/reference/linux/html_css/bootstrap/affix-scrollspy.html) 内容。


## CSS, Cascading Style Sheets

HTML 标签原本被设计为用于定义文档内容，例如那些是标题、各个段落的内容等。而 CSS 样式，则定义了如何显示 HTML 元素，如字体大小、颜色属性等。样式通常保存在外部的 .css 文件中，这样就可以通过仅仅编辑一个简单的 CSS 文档，从而改变页面的布局和外观。

CSS 规则由两个主要的部分构成：选择器 + 一条或多条声明；选择器指定了需要改变样式的 HTML 元素，声明由一个属性和一个值组成，可以有多个声明组成。

![css selector](/images/webserver/css-selector.gif "css selector"){: .pull-center}

属性和值通过 ":" 分隔，各个声明通过 ";" 分隔，为了方便阅读，通常每行只定义了一个声明，示例如下：

{% highlight css %}
p {
    text-align:center;   /* 这是个注释 */
    color:black;
    font-family:arial;
}
{% endhighlight %}

### 网页嵌入方式

假设，由如下的一段 HTML 代码。

{% highlight html %}
<html>
  <head>
    <title>Hello World</title>
  </head>
  <body>
    <div id="foo" class="bar"></div>
  </body>
</html>
{% endhighlight %}

在 HTML 代码中可以通过如下的三种方式嵌入到网页中。

#### 1. 外链式CSS

编写一个 .css 文件，假设命名为 style.css 内容如下：

{% highlight css %}
#foo {
   padding:  5px;
}
.bar {
   padding:  5px;
}
{% endhighlight %}

然后在 HTML 代码的 head 标签对中添加，内容如下：

{% highlight html %}
<head>
  <title>Hello World</title>
  <link rel="stylesheet" type="text/css" href="style.css">
</head>
{% endhighlight %}

#### 2. 内嵌式CSS

可以直接在 title 标签中添加。

{% highlight html %}
<head>
  <title>Hello World</title>
  <style type="text/css">
    <!--
    #foo{
       padding:5px;
    }
    .bar {
       padding:  5px;
    }
    -->
  </style>
</head>
{% endhighlight %}

#### 3. 直接式CSS

也就是直接为 DIV 标签添加 style 属性如下：

{% highlight html %}
<div id="foo" class="bar" style="padding:5px;"></div>
{% endhighlight %}




### id 和 class

id 和 class 的定义方式是不同的，分别通过 '#' 和 '.' 进行定义；然后在标签中通过 id="foobar" 或者 class="foobar" 表示。

{% highlight css %}
#foobar {
    text-align:center;
    color:red;
}

.foobar {
    text-align:center;
    color:red;
}
{% endhighlight %}




#### ID 使用原则

通常来说，ID 具有唯一性，不过现在的浏览器在多个 ID 时仍然可以保证样式生效。通常在写网页时，大的结构用 ID，如：logo (#logo)、导航 (#nav)、主体内容 (#body)、版权 (#copyright) 等。

另外，还有一种情况：当通过 js 作用一个层，以实现某种效果时；这也是由 ID 的唯一性决定的。

#### Class 使用原则

在 css 定义中，实际可以无限次的重复使用 Class，常用于结构内部，这样做的好处是有利于网站代码的后期维护与修改。

<!--
另外，需要注意的是，若出现重复定义时，优先级是：Style ＞ ID ＞ Class ＞ 缺省的 Html 元素 。
-->


### 样式的插入

插入样式表的方法有三种: 外部样式表、内部样式表、内联样式。

#### 外部样式表

当很多页面都需要相同的样式时，最后使用外部样式，这样可以直接通过改变一个文件来改变整个站点的外观。在使用时，每个页面使用 \<link\> 标签链接到样式表，而且 \<link\> 标签要在文件的头部：
{% highlight css %}
<head>
  <link rel="stylesheet" type="text/css" href="foobar.css">
</head>
{% endhighlight %}

此时，浏览器会从文件 foobar.css 中读到样式声明，并根据它来格式文档。如下是一个示例：

{% highlight css %}
p {margin-left:20px;}
body {background-image:url("/images/background.gif");}
{% endhighlight %}

另外，需要注意的是，不要在属性值与单位之间有空格，例如 "margin-left: 20 px" 而不是 "margin-left: 20px"，那么在某些浏览器是无法工作的。


#### 内部样式表

常见的场景是当单个文档需要特殊的样式时，就应该使用内部样式表。此时，可以使用 \<style\> 标签在文档头部定义内部样式表，例如：

{% highlight css %}
<head>
  <style>
    p {margin-left:20px;}
    body {background-image:url("images/backgroup.gif");}
  </style>
</head>
{% endhighlight %}

#### 内联样式

内联样式是在相关的标签内使用样式 (style) 属性，可以包含任何 CSS 属性。由于此时要将样式和内容混杂在一起，从而会降低样式表的许多优势，因此要慎用这种方法，通常在样式仅需要在一个元素上应用一次时才会使用。

{% highlight css %}
<p style="color:sienna;margin-left:20px">This is a paragraph.</p>
{% endhighlight %}

#### 多重样式

样式的属性值会从上层的样式表中继承过来，一般会按照浏览器缺省设置、外部样式表、内部样式表、内联样式的顺序，也就是越靠后的优先级越高。

{% highlight css %}
h3 { /* 外部样式 */
    color:red;
    text-align:left;
    font-size:8pt;
}

h3 { /* 内部样式 */
    text-align:right;
    font-size:20pt;
}

h3 { /* 最终h3得到的样式 */
    color:red;
    text-align:right;
    font-size:20pt;
}
{% endhighlight %}

也就是说，按照上面的顺序，后者会继承前者的属性，但是如果存在则会覆盖，类似于 C++ Java 这种面向对象语言中的继承机制。

### 分组、嵌套

通常在样式表中有很多具有相同样式的元素，为了尽量减少代码，可以使用分组选择器，每个选择器用逗号分隔。

{% highlight css %}
h1,h2,p {
  color:green;
}
{% endhighlight %}

嵌套选择器可以对多层的选择器定义样式，适用于选择器内部的选择器的样式。

{% highlight css %}
p {          /* 为所有p元素指定一个样式 */
  color:blue;
}
.marked {    /* 为所有class="marked"的元素指定一个样式 */
  background-color:red;
}
.marked p {  /* 为所有class="marked"元素内的p元素指定一个样式 */
  color:white;
}
{% endhighlight %}


## Javascript


### 绑定事件

在 JS 中有三种绑定事件的方法：DOM 元素中直接绑定；JS 代码中绑定；绑定事件监听函数。

#### 1. DOM 元素绑定

这里的 DOM 元素可以直接理解为 HTML 标签，其语法如下：

{% highlight text %}
onMETHOD="JavaScript Code"
{% endhighlight %}

其中，onMETHOD 为事件名称，例如，onclick 鼠标单击、ondouble 鼠标双击、onmouseover 鼠标移入、onmouseout 鼠标移出等。例如：

{% highlight html %}
<input  onclick="alert('谢谢支持')"  type="button"  value="点击我，弹出警告框" />
{% endhighlight %}

<input  onclick="alert('谢谢支持')"  type="button"  value="点击我，弹出警告框" />


#### 2. JavaScript 绑定事件的方法

可以使 JavaScript 代码与 HTML 标签分离，文档结构清晰，便于管理和开发。

{% highlight text %}
elementObject.onXXX=function(){
    // 事件处理代码
}
{% endhighlight %}

绑定事件的语法如上所示，其中 elementObject 为 DOM 对象、onXXX 为事件名称。

{% highlight html %}
<input  id="demo"  type="button"  value="点击我，显示 type 属性" />
<script type="text/javascript">
document.getElementById("demo").onclick=function(){
    alert(this.getAttribute("type"));  //  this 指当前发生事件的HTML元素，这里是<div>标签
}
</script>
{% endhighlight %}

<input id="demo"  type="button"  value="点击我，显示 type 属性" />

<script type="text/javascript">
document.getElementById("demo").onclick=function(){
    alert(this.getAttribute("type"));  //  this 指当前发生事件的HTML元素，这里是<div>标签
}
</script>

#### 3. 绑定事件监听函数。

绑定事件的另一种方法是用 addEventListener() 或 attachEvent() 来绑定事件监听函数。

<!--

addEventListener()函数语法：
elementObject.addEventListener(eventName,handle,useCapture);
参数    说明
elementObject   DOM对象（即DOM元素）。
eventName   事件名称。注意，这里的事件名称没有“ on ”，如鼠标单击事件 click ，鼠标双击事件 doubleclick ，鼠标移入事件 mouseover，鼠标移出事件 mouseout 等。
handle  事件句柄函数，即用来处理事件的函数。
useCapture  Boolean类型，是否使用捕获，一般用false 。这里涉及到JavaScript事件流的概念，后续章节将会详细讲解。

attachEvent()函数语法：
elementObject.attachEvent(eventName,handle);
参数    说明
elementObject   DOM对象（即DOM元素）。
eventName   事件名称。注意，与addEventListener()不同，这里的事件名称有“ on ”，如鼠标单击事件 onclick ，鼠标双击事件 ondoubleclick ，鼠标移入事件 onmouseover，鼠标移出事件 onmouseout 等。
handle  事件句柄函数，即用来处理事件的函数。

注意：事件句柄函数是指“ 函数名 ”，不能带小括号。

addEventListener()是标准的绑定事件监听函数的方法，是W3C所支持的，Chrome、FireFox、Opera、Safari、IE9.0及其以上版本都支持该函数；但是，IE8.0及其以下版本不支持该方法，它使用attachEvent()来绑定事件监听函数。所以，这种绑定事件的方法必须要处理浏览器兼容问题。

下面绑定事件的代码，进行了兼容性处理，能够被所有浏览器支持：

    function addEvent(obj,type,handle){
        try{  // Chrome、FireFox、Opera、Safari、IE9.0及其以上版本
            obj.addEventListener(type,handle,false);
        }catch(e){
            try{  // IE8.0及其以下版本
                obj.attachEvent('on' + type,handle);
            }catch(e){  // 早期浏览器
                obj['on' + type] = handle;
            }
        }
    }


这里使用 try{ ... } catch(e){ ... } 代替 if ... else... 语句，避免浏览器出现错误提示。

例如，为一个 id="demo" 的按钮绑定事件，鼠标单击时弹出警告框：

    addEvent(document.getElementById("demo"),"click",myAlert);
    function myAlert(){
        alert("又是一个警告框");
    }

实例演示：

-->

### 杂项

#### (function($){...})(jQuery);

JS 是支持匿名函数的，其声明和一次调用可以使用如下方式：

{% highlight text %}
function(arg){...}             # 函数声明
(function(arg){...})(param)    # 直接调用，定义个参数为arg的匿名函数，并将param作为参数调用
{% endhighlight %}

而 (function($){...})(jQuery) 功能相同，之所以形参使用 $，是为了不与其它库冲突，其功能相当于：

{% highlight text %}
function outpug(s){...}; output(jQuery);    # 或者
var fn=function(s){....}; fn(jQuery);
{% endhighlight %}

另外，有如下的三种写法，用来在文档载入完成后执行函数。

{% highlight text %}
$(function(){...});
jQuery(function($){...});
$(document).ready(function(){...})
{% endhighlight %}




## ECharts

百度开发的 [ECharts](http://echarts.baidu.com/index.html) ，一个很不错的图表绘制示例，可以选择包含那些类型的包、是否压缩等，而且文档、示例等都非常丰富。另外一个与 ECharts 类似的是 [www.highcharts.com](http://www.highcharts.com/) 。

如下是一个最简单的示例，只需要通过 echarts.init() 方法初始化一个 echarts 实例并通过 setOption 方法生成一个简单的柱状图。

{% highlight html %}
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>ECharts</title>
    <!-- 引入 echarts.js -->
    <script src="echarts.min.js"></script>
</head>
<body>
    <!-- 为ECharts准备一个具备大小（宽高）的Dom -->
    <div id="main" style="width: 600px;height:400px;"></div>
    <script type="text/javascript">
        // 基于准备好的dom，初始化echarts实例
        var myChart = echarts.init(document.getElementById('main'));

        // 指定图表的配置项和数据
        var option = {
            title: {
                text: 'ECharts 入门示例'
            },
            tooltip: {},
            legend: {
                data:['销量']
            },
            xAxis: {
                data: ["衬衫","羊毛衫","雪纺衫","裤子","高跟鞋","袜子"]
            },
            yAxis: {},
            series: [{
                name: '销量',
                type: 'bar',
                data: [5, 20, 36, 10, 10, 20]
            }]
        };

        // 使用刚指定的配置项和数据显示图表。
        myChart.setOption(option);
    </script>
</body>
</html>
{% endhighlight %}




## 参考

### 常见示例

* 弹出联系方式等，可以参考 [示例](/reference/linux/html_css/pop_toolbars/index.html) 。

### 网站

在此列举一些常见的网站。

* [www.runoob.com](http://www.runoob.com/) 一个不错的教学网站，包括 HTML、CSS、jQuery、Bootstrap 等。
* BootStrap 的源码可以直接从 [Bootstrap on GitHub](https://github.com/twbs/bootstrap) 上下载。
* [www.fontawesome.io](http://fontawesome.io/) 包括了一些前端的图标、字体格式等，其它的一些 SVG 素材可以直接搜索。
* [webpack](https://webpack.github.io/) 一个很不错的打包工具。
* 一些不错的模版，用来制作在线 PPT，可以参考 [reveal.js](https://github.com/hakimel/reveal.js)、[CSSS](https://github.com/LeaVerou/CSSS)、[Slide](https://github.com/briancavalier/slides) 。



<!--
http://www.iconfont.cn/
http://flaticons.net/
http://www.iconsvg.com/
http://www.flaticon.com/
http://fontello.com/
http://www.iconthink.com/
https://github.com/nullice/NViconsLib_Silhouette
http://www.elegantthemes.com/blog/freebie-of-the-week/beautiful-flat-icons-for-free
-->


{% highlight text %}
{% endhighlight %}
