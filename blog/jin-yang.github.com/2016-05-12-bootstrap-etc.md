---
title: Bootstrap
layout: post
comments: true
language: chinese
category: [linux]
keywords: bootstrap,html,css,javascript
description: Bootstrap 來自 Twitter，基於 HTML、CSS、JavaScript，是目前最受歡迎的前端框架，它簡潔靈活，使得 Web 開發更加快捷。
---

Bootstrap 來自 Twitter，基於 HTML、CSS、JavaScript，是目前最受歡迎的前端框架，它簡潔靈活，使得 Web 開發更加快捷。

在這裡我們看下 bootstrap 以及一些 html、css 等相關的內容。

<!-- more -->


## 簡介

Bootstrap 是由 Twitter 的 MARK OTTO 和 Jacob Thornton 所設計和建立，在開源後，社區驚人地活躍，代碼版本進化非常快速，而且官方文檔質量極其高，並湧現了許多基於 Bootstrap 建設的網站，其界面清新、簡潔，排版利落大方。

目前的最新版本已經快到 V4 了，可以參考 [www.getbootstrap.com](https://getbootstrap.com/)，另外可以查看中文網站 [Bootstrap 中文網](http://www.bootcss.com/) ，其中後者包括了一些常用的網站，比如編碼規範、jQuery API 中文文檔、經典的網站實例等等。


### 安裝

可以直接從上述的中文網站中下載 《用於生產環境的 Bootstrap》，以 V3 為例，解壓後的目錄結構如下：

![bootstrap files](/images/webserver/bootstrap-files.png "bootstrap files"){: .pull-center}

對於 Nginx，可以直接將 root 指向該目錄即可。

### 柵格系統

也被稱為網格系統 (Grid System)，Bootstrap 提供了一套響應式、移動設備優先的流式網格系統，隨著屏幕或 viewport 尺寸的增加，系統會自動分為最多 12 列。

其在 CSS 中的定義如下，會根據屏幕的大小來決定 width 大小。

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

如下是網格系統的定義。

![bootstrap grid system](/images/webserver/bootstrap-grid-system.png "bootstrap grid system"){: .pull-center}

網格系統總共提供瞭如下的四種。

{% highlight text %}
.c0l-xs-　　無論屏幕寬度如何，單元格都在一行，寬度按照百分比設置；試用於手機；
.col-sm-　　屏幕大於768px時，單元格在一行顯示；屏幕小於768px時，獨佔一行；試用於平板；
.col-md-　　屏幕大於992px時，單元格在一行顯示；屏幕小於992px時，獨佔一行；試用於桌面顯示器；
.col-lg-　　屏幕大於1200px時，單元格在一行顯示；屏幕小於1200px時，獨佔一行；適用於大型桌面顯示器；
{% endhighlight %}

與此相關的詳細內容，也可以參考如下。

![bootstrap grid system](/images/webserver/bootstrap-grid-system-2.png "bootstrap grid system"){: .pull-center width="90%"}

假設有如下內容。

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

當不同的屏幕大小時，會有不同的表格系統，內容如下。

##### 屏幕大於 992px

.col-md-8 和.col-md-4 還處於作用範圍內。

![bootstrap grid system](/images/webserver/bootstrap-grid-system-3.png "bootstrap grid system"){: .pull-center width="95%"}

##### 屏幕範圍 769px~992px

.col-md- 已失效，而 .col-sm- 還處在作用範圍內。

![bootstrap grid system](/images/webserver/bootstrap-grid-system-4.png "bootstrap grid system"){: .pull-center width="80%"}

##### 屏幕小於 768px

 .col-sm-已失效只有 .col-xs- 不受屏幕寬度影響，這時候就由 .col-xs- 起作用。

![bootstrap grid system](/images/webserver/bootstrap-grid-system-5.png "bootstrap grid system"){: .pull-center width="70%"}

### 常用工具

除了上述的表格系統，Bootstrap 提供了一些輔助類，以便更快地實現對移動設備友好的開發。

這些可以通過合大型、小型和中型設備，實現內容對設備的顯示和隱藏，主要包括了兩類：.hidden-print、.hidden-xs、.visible-xs-* 等。




<!--

### carousel
http://blog.eol3.com/archives/938

### Modal

http://nakupanda.github.io/bootstrap3-dialog/
http://getbootstrap.com/javascript/

-->



### Affix & Scrollspy

附加導航（Affix）插件允許某個 &lt;div&gt; 固定在頁面的某個位置，也可以在打開或關閉使用該插件之間進行切換。一個常見的例子是社交圖標。它們將在某個位置開始，但當頁面點擊某個標記，該 &lt;div&gt; 會鎖定在某個位置，不會隨著頁面其他部分一起滾動。

另外，可以使用 ScrollSpy (滾動監聽)。

可參考 [Understanding Bootstrap’s Affix and ScrollSpy plugins](https://www.sitepoint.com/understanding-bootstraps-affix-scrollspy-plugins/)，其示例可以 [參考本地](/reference/linux/html_css/bootstrap/affix-scrollspy.html) 內容。


## CSS, Cascading Style Sheets

HTML 標籤原本被設計為用於定義文檔內容，例如那些是標題、各個段落的內容等。而 CSS 樣式，則定義瞭如何顯示 HTML 元素，如字體大小、顏色屬性等。樣式通常保存在外部的 .css 文件中，這樣就可以通過僅僅編輯一個簡單的 CSS 文檔，從而改變頁面的佈局和外觀。

CSS 規則由兩個主要的部分構成：選擇器 + 一條或多條聲明；選擇器指定了需要改變樣式的 HTML 元素，聲明由一個屬性和一個值組成，可以有多個聲明組成。

![css selector](/images/webserver/css-selector.gif "css selector"){: .pull-center}

屬性和值通過 ":" 分隔，各個聲明通過 ";" 分隔，為了方便閱讀，通常每行只定義了一個聲明，示例如下：

{% highlight css %}
p {
    text-align:center;   /* 這是個註釋 */
    color:black;
    font-family:arial;
}
{% endhighlight %}

### 網頁嵌入方式

假設，由如下的一段 HTML 代碼。

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

在 HTML 代碼中可以通過如下的三種方式嵌入到網頁中。

#### 1. 外鏈式CSS

編寫一個 .css 文件，假設命名為 style.css 內容如下：

{% highlight css %}
#foo {
   padding:  5px;
}
.bar {
   padding:  5px;
}
{% endhighlight %}

然後在 HTML 代碼的 head 標籤對中添加，內容如下：

{% highlight html %}
<head>
  <title>Hello World</title>
  <link rel="stylesheet" type="text/css" href="style.css">
</head>
{% endhighlight %}

#### 2. 內嵌式CSS

可以直接在 title 標籤中添加。

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

也就是直接為 DIV 標籤添加 style 屬性如下：

{% highlight html %}
<div id="foo" class="bar" style="padding:5px;"></div>
{% endhighlight %}




### id 和 class

id 和 class 的定義方式是不同的，分別通過 '#' 和 '.' 進行定義；然後在標籤中通過 id="foobar" 或者 class="foobar" 表示。

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




#### ID 使用原則

通常來說，ID 具有唯一性，不過現在的瀏覽器在多個 ID 時仍然可以保證樣式生效。通常在寫網頁時，大的結構用 ID，如：logo (#logo)、導航 (#nav)、主體內容 (#body)、版權 (#copyright) 等。

另外，還有一種情況：當通過 js 作用一個層，以實現某種效果時；這也是由 ID 的唯一性決定的。

#### Class 使用原則

在 css 定義中，實際可以無限次的重複使用 Class，常用於結構內部，這樣做的好處是有利於網站代碼的後期維護與修改。

<!--
另外，需要注意的是，若出現重複定義時，優先級是：Style ＞ ID ＞ Class ＞ 缺省的 Html 元素 。
-->


### 樣式的插入

插入樣式表的方法有三種: 外部樣式表、內部樣式表、內聯樣式。

#### 外部樣式表

當很多頁面都需要相同的樣式時，最後使用外部樣式，這樣可以直接通過改變一個文件來改變整個站點的外觀。在使用時，每個頁面使用 \<link\> 標籤鏈接到樣式表，而且 \<link\> 標籤要在文件的頭部：
{% highlight css %}
<head>
  <link rel="stylesheet" type="text/css" href="foobar.css">
</head>
{% endhighlight %}

此時，瀏覽器會從文件 foobar.css 中讀到樣式聲明，並根據它來格式文檔。如下是一個示例：

{% highlight css %}
p {margin-left:20px;}
body {background-image:url("/images/background.gif");}
{% endhighlight %}

另外，需要注意的是，不要在屬性值與單位之間有空格，例如 "margin-left: 20 px" 而不是 "margin-left: 20px"，那麼在某些瀏覽器是無法工作的。


#### 內部樣式表

常見的場景是當單個文檔需要特殊的樣式時，就應該使用內部樣式表。此時，可以使用 \<style\> 標籤在文檔頭部定義內部樣式表，例如：

{% highlight css %}
<head>
  <style>
    p {margin-left:20px;}
    body {background-image:url("images/backgroup.gif");}
  </style>
</head>
{% endhighlight %}

#### 內聯樣式

內聯樣式是在相關的標籤內使用樣式 (style) 屬性，可以包含任何 CSS 屬性。由於此時要將樣式和內容混雜在一起，從而會降低樣式表的許多優勢，因此要慎用這種方法，通常在樣式僅需要在一個元素上應用一次時才會使用。

{% highlight css %}
<p style="color:sienna;margin-left:20px">This is a paragraph.</p>
{% endhighlight %}

#### 多重樣式

樣式的屬性值會從上層的樣式表中繼承過來，一般會按照瀏覽器缺省設置、外部樣式表、內部樣式表、內聯樣式的順序，也就是越靠後的優先級越高。

{% highlight css %}
h3 { /* 外部樣式 */
    color:red;
    text-align:left;
    font-size:8pt;
}

h3 { /* 內部樣式 */
    text-align:right;
    font-size:20pt;
}

h3 { /* 最終h3得到的樣式 */
    color:red;
    text-align:right;
    font-size:20pt;
}
{% endhighlight %}

也就是說，按照上面的順序，後者會繼承前者的屬性，但是如果存在則會覆蓋，類似於 C++ Java 這種面嚮對象語言中的繼承機制。

### 分組、嵌套

通常在樣式表中有很多具有相同樣式的元素，為了儘量減少代碼，可以使用分組選擇器，每個選擇器用逗號分隔。

{% highlight css %}
h1,h2,p {
  color:green;
}
{% endhighlight %}

嵌套選擇器可以對多層的選擇器定義樣式，適用於選擇器內部的選擇器的樣式。

{% highlight css %}
p {          /* 為所有p元素指定一個樣式 */
  color:blue;
}
.marked {    /* 為所有class="marked"的元素指定一個樣式 */
  background-color:red;
}
.marked p {  /* 為所有class="marked"元素內的p元素指定一個樣式 */
  color:white;
}
{% endhighlight %}


## Javascript


### 綁定事件

在 JS 中有三種綁定事件的方法：DOM 元素中直接綁定；JS 代碼中綁定；綁定事件監聽函數。

#### 1. DOM 元素綁定

這裡的 DOM 元素可以直接理解為 HTML 標籤，其語法如下：

{% highlight text %}
onMETHOD="JavaScript Code"
{% endhighlight %}

其中，onMETHOD 為事件名稱，例如，onclick 鼠標單擊、ondouble 鼠標雙擊、onmouseover 鼠標移入、onmouseout 鼠標移出等。例如：

{% highlight html %}
<input  onclick="alert('謝謝支持')"  type="button"  value="點擊我，彈出警告框" />
{% endhighlight %}

<input  onclick="alert('謝謝支持')"  type="button"  value="點擊我，彈出警告框" />


#### 2. JavaScript 綁定事件的方法

可以使 JavaScript 代碼與 HTML 標籤分離，文檔結構清晰，便於管理和開發。

{% highlight text %}
elementObject.onXXX=function(){
    // 事件處理代碼
}
{% endhighlight %}

綁定事件的語法如上所示，其中 elementObject 為 DOM 對象、onXXX 為事件名稱。

{% highlight html %}
<input  id="demo"  type="button"  value="點擊我，顯示 type 屬性" />
<script type="text/javascript">
document.getElementById("demo").onclick=function(){
    alert(this.getAttribute("type"));  //  this 指當前發生事件的HTML元素，這裡是<div>標籤
}
</script>
{% endhighlight %}

<input id="demo"  type="button"  value="點擊我，顯示 type 屬性" />

<script type="text/javascript">
document.getElementById("demo").onclick=function(){
    alert(this.getAttribute("type"));  //  this 指當前發生事件的HTML元素，這裡是<div>標籤
}
</script>

#### 3. 綁定事件監聽函數。

綁定事件的另一種方法是用 addEventListener() 或 attachEvent() 來綁定事件監聽函數。

<!--

addEventListener()函數語法：
elementObject.addEventListener(eventName,handle,useCapture);
參數    說明
elementObject   DOM對象（即DOM元素）。
eventName   事件名稱。注意，這裡的事件名稱沒有“ on ”，如鼠標單擊事件 click ，鼠標雙擊事件 doubleclick ，鼠標移入事件 mouseover，鼠標移出事件 mouseout 等。
handle  事件句柄函數，即用來處理事件的函數。
useCapture  Boolean類型，是否使用捕獲，一般用false 。這裡涉及到JavaScript事件流的概念，後續章節將會詳細講解。

attachEvent()函數語法：
elementObject.attachEvent(eventName,handle);
參數    說明
elementObject   DOM對象（即DOM元素）。
eventName   事件名稱。注意，與addEventListener()不同，這裡的事件名稱有“ on ”，如鼠標單擊事件 onclick ，鼠標雙擊事件 ondoubleclick ，鼠標移入事件 onmouseover，鼠標移出事件 onmouseout 等。
handle  事件句柄函數，即用來處理事件的函數。

注意：事件句柄函數是指“ 函數名 ”，不能帶小括號。

addEventListener()是標準的綁定事件監聽函數的方法，是W3C所支持的，Chrome、FireFox、Opera、Safari、IE9.0及其以上版本都支持該函數；但是，IE8.0及其以下版本不支持該方法，它使用attachEvent()來綁定事件監聽函數。所以，這種綁定事件的方法必須要處理瀏覽器兼容問題。

下面綁定事件的代碼，進行了兼容性處理，能夠被所有瀏覽器支持：

    function addEvent(obj,type,handle){
        try{  // Chrome、FireFox、Opera、Safari、IE9.0及其以上版本
            obj.addEventListener(type,handle,false);
        }catch(e){
            try{  // IE8.0及其以下版本
                obj.attachEvent('on' + type,handle);
            }catch(e){  // 早期瀏覽器
                obj['on' + type] = handle;
            }
        }
    }


這裡使用 try{ ... } catch(e){ ... } 代替 if ... else... 語句，避免瀏覽器出現錯誤提示。

例如，為一個 id="demo" 的按鈕綁定事件，鼠標單擊時彈出警告框：

    addEvent(document.getElementById("demo"),"click",myAlert);
    function myAlert(){
        alert("又是一個警告框");
    }

實例演示：

-->

### 雜項

#### (function($){...})(jQuery);

JS 是支持匿名函數的，其聲明和一次調用可以使用如下方式：

{% highlight text %}
function(arg){...}             # 函數聲明
(function(arg){...})(param)    # 直接調用，定義個參數為arg的匿名函數，並將param作為參數調用
{% endhighlight %}

而 (function($){...})(jQuery) 功能相同，之所以形參使用 $，是為了不與其它庫衝突，其功能相當於：

{% highlight text %}
function outpug(s){...}; output(jQuery);    # 或者
var fn=function(s){....}; fn(jQuery);
{% endhighlight %}

另外，有如下的三種寫法，用來在文檔載入完成後執行函數。

{% highlight text %}
$(function(){...});
jQuery(function($){...});
$(document).ready(function(){...})
{% endhighlight %}




## ECharts

百度開發的 [ECharts](http://echarts.baidu.com/index.html) ，一個很不錯的圖表繪製示例，可以選擇包含那些類型的包、是否壓縮等，而且文檔、示例等都非常豐富。另外一個與 ECharts 類似的是 [www.highcharts.com](http://www.highcharts.com/) 。

如下是一個最簡單的示例，只需要通過 echarts.init() 方法初始化一個 echarts 實例並通過 setOption 方法生成一個簡單的柱狀圖。

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
    <!-- 為ECharts準備一個具備大小（寬高）的Dom -->
    <div id="main" style="width: 600px;height:400px;"></div>
    <script type="text/javascript">
        // 基於準備好的dom，初始化echarts實例
        var myChart = echarts.init(document.getElementById('main'));

        // 指定圖表的配置項和數據
        var option = {
            title: {
                text: 'ECharts 入門示例'
            },
            tooltip: {},
            legend: {
                data:['銷量']
            },
            xAxis: {
                data: ["襯衫","羊毛衫","雪紡衫","褲子","高跟鞋","襪子"]
            },
            yAxis: {},
            series: [{
                name: '銷量',
                type: 'bar',
                data: [5, 20, 36, 10, 10, 20]
            }]
        };

        // 使用剛指定的配置項和數據顯示圖表。
        myChart.setOption(option);
    </script>
</body>
</html>
{% endhighlight %}




## 參考

### 常見示例

* 彈出聯繫方式等，可以參考 [示例](/reference/linux/html_css/pop_toolbars/index.html) 。

### 網站

在此列舉一些常見的網站。

* [www.runoob.com](http://www.runoob.com/) 一個不錯的教學網站，包括 HTML、CSS、jQuery、Bootstrap 等。
* BootStrap 的源碼可以直接從 [Bootstrap on GitHub](https://github.com/twbs/bootstrap) 上下載。
* [www.fontawesome.io](http://fontawesome.io/) 包括了一些前端的圖標、字體格式等，其它的一些 SVG 素材可以直接搜索。
* [webpack](https://webpack.github.io/) 一個很不錯的打包工具。
* 一些不錯的模版，用來製作在線 PPT，可以參考 [reveal.js](https://github.com/hakimel/reveal.js)、[CSSS](https://github.com/LeaVerou/CSSS)、[Slide](https://github.com/briancavalier/slides) 。



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
