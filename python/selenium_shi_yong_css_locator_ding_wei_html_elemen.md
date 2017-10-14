# Selenium 使用 CSS locator 定位 HTML element


Selenium 提供多種 locator strategy，本篇文章將說明透過 CSS locator 來定位 HTML element。將用以下幾個 Case 說明：

##Case 1. 

首先，我們先了解透過 CSS 抓取 element 的方式與透過 id、class name 與節點名稱方式來抓取 element，以下面 HTML 原始碼來說明。

```html
<input value="Google" id="igl" class="cgl">
```

要透過 id 抓取 element，呼叫 find_element_by_css_selector() 帶入的參數為 id 屬性值前面加上「#」，此方法與 find_element_by_id() 執行結果一樣。

```html
find_element_by_css_selector("#igl")
find_element_by_id("igl")
```


透過 class 抓取 element，帶入的參數為 class 屬性值前面加上「.」，結果與 find_element_by_class_name() 一樣。

```html
find_element_by_css_selector(".cgl")
find_element_by_class_name("cgl")
```

如果一個 class 裡包含多個 class 情況，HTML 原始碼如下。

```html
<input value="Google" id="igl" class="cgl_a cgl_b">
```

則不管抓取 cgl_a 或 cgl_b 的結果都會一樣，程式碼如下所示。

```html
find_element_by_css_selector(".cgl_a")
find_element_by_css_selector(".cgl_b")
```

但如果就是想抓取 class="cgl_a cgl_b" 屬性值為 “cgl_a cgl_b" 的 element 的話，無法透過 find_element_by_css_selector(“.cgl_a cgl_b") 方式抓取，則要用下面 Case 3 提到的方法來抓取 element。

透過節點名稱抓取 element，參數直接帶入節點的名稱，與 find_element_by_tag_name() 執行結果一樣。

```html
find_element_by_css_selector("input")
find_element_by_tag_name("input")
```



##Case 2.

要抓取某個節點內的 element，HTML 原始碼如下所示，我們要抓取第 5 行的 element。

```html
<input value="Google" id="igl" class="cgl">
<li class="st1">
    <h2 class="sub_title">Chrome</h2>
</li>
<li class="st2">
    <h2 class="sub_title">Firefox</h2>
<li>
```

如果我們直接抓取 class 屬性值為 sub_title 會找到兩個 element，分別會找到第 2 行與第 5 行 element，但我們只想抓到第 5 行 element。

可看到要抓取的 element 是在 li 節點內，且 li element 的 class 為 st2 ，因此可以透過 “li.st2″ 先抓到父節點，再往下找 “h2″ 子節點，就可以正確抓取到 element，程式碼如下所示。

```html
find_element_by_css_selector("li.st2 h2")
```

##Case 3.

透過 element 裡的屬性值定位 element，HTML 原始碼如下所示。

```html
<a title="yh" href="http://www.yahoo.com">Yahoo</a>
```

抓取 element 中 title 屬性值為 yh，要在 [ ] 裡代入識別的屬性值與某個值相同，程式碼如下所示。

```html
find_element_by_css_selector("a[title='yh']")
```

##Case 4.

在上一個例子，能抓取到屬性值相符的 element，但有時候我們會看到某些屬性值，會有某些字串開頭、結尾或包含有某些字串，以下面為 HTML 程式碼為範例，每個 class 屬性值都是 cls 開頭的值。

```html
<h1 class="cls_a1">A1</h1>
<h1 class="cls_b2">B2</h1>
<h1 class="cls_c3">C3</h1>
```

我們就可以透過以下程式碼，抓取到 3 個 class 屬性值為 cls 開頭的 element。

```html
driver.find_elements_by_css_selector("h1[class^='cls']")
````

```html
Tip	[class^=’cls’] 抓取 class 屬性值為 “cls" 開頭的 element
[class$=’cls’] 抓取 class 屬性值為 “cls" 結尾的 element

[class*=’cls’] 抓取 class 屬性值中包含字串 “cls" 的 element
```

Case 5.

只抓取含有特定屬性的 element，我們要抓取第 2 行超連結的element，HTML 原始碼如下所示。

1
2
<a href="http://www.yahoo.com" alt="yahoo">Yahoo!</a>
<a title="t2" href="http://www.google.com" class="gls">Google 2</a>
我們只想抓取含有 title、href 與 class 屬性的 element，程式碼如下所示。

1
find_element_by_css_selector("a[title][href][class]")
Case 6.

只抓取某節點中第幾個 element，以下面 HTML 原始碼為範例。

1
2
3
4
5
6
7
<table>
    <tr>
        <td>First</td>
        <td>Second</td>
        <td>Third</td>
    </tr>
</table>
如要抓取 td 標籤中第 1 個 element，可以在節點名稱後加入:first-child，抓到 <td>First</td>，程式碼如下所示。

1
find_element_by_css_selector("td:first-child")
如要抓取 td 標籤中第 3 個 element，可以在節點名稱後加入:nth-child(3)，括號中代入索引值，會抓到 <td>Third</td>，程式碼如下所示。

1
find_element_by_css_selector("td:nth-child(3)")
Case 7.

最後我們用一個例子，來說明抓取 element 可以透過多種方式來抓取同一個 element，HTML 原始碼如下所示。

1
2
3
<div class="dgl">
    <input value="Google" id="igl">
</div>
我們欲抓取第 2 行的 input element，可以透過下列方法抓取到 input element，使用者根據不同的結構選擇不同的定位方式。

1
2
3
4
5
6
7
8
find_element_by_css_selector("#igl")
find_element_by_css_selector("input")
find_element_by_css_selector("div input")
find_element_by_css_selector("div input#igl")
find_element_by_css_selector("div.dgl input")
find_element_by_css_selector("div.dgl input#igl")
find_element_by_css_selector("div input[value='Google']")
find_element_by_css_selector("div[class='dgl'] input[value='Google'][id='igl']")
 

以上幾個 Case 均已能滿足大部份的需求，其他更進階的應用可以參考這邊。

參考文章：

1. WebDriver Element Locators

2. selenium+python find_element_by_css_selector方法使用- 你堅持了嗎- 博客園

3. 利用 CSS Selector 定位網頁元件 | 科科和測試