# python爬蟲之BeautifulSoup

##簡介
Beautiful Soup提供一些簡單的、python式的函數用來處理導航、搜索、修改分析樹等功能。它是一個工具箱，通過解析文檔為用戶提供需要抓取的數據，因為簡單，所以不需要多少代碼就可以寫出一個完整的應用程序。Beautiful Soup自動將輸入文檔轉換為Unicode編碼，輸出文檔轉換為utf-8編碼。你不需要考慮編碼方式，除非文檔沒有指定一個編碼方式，這時，Beautiful Soup就不能自動識別編碼方式了。然後，你僅僅需要說明一下原始編碼方式就可以了。


Beautiful Soup已成為和lxml、html6lib一樣出色的python解釋器，為用戶靈活地提供不同的解析策略或強勁的速度。



## 安裝

```sh
pip install BeautifulSoup4
easy_install BeautifulSoup4
```

## 創建BeautifulSoup對象

- 首先應該導入BeautifulSoup類庫 from bs4 import BeautifulSoup

- 下面開始創建對像，在開始之前為了方便演示，先創建一個html文本，如下：


```html
html = """
<html><head><title>The Dormouse's story</title></head>
<body>
<p class="title" name="dromouse"><b>The Dormouse's story</b></p>
<p class="story">Once upon a time there were three little sisters; and their names were
<a href="http://example.com/elsie" class="sister" id="link1"><!-- Elsie --></a>,
<a href="http://example.com/lacie" class="sister" id="link2">Lacie</a> and
<a href="http://example.com/tillie" class="sister" id="link3">Tillie</a>;
and they lived at the bottom of a well.</p>
<p class="story">...</p>
"""
```

- 創建對象：soup=BeautifulSoup(html,'lxml'),這裡的lxml是解析的類庫，目前來說個人覺得最好的解析器了，一直在用這個，安裝方法：pip install lxml


## Tag

Tag就是html中的一個標籤，用BeautifulSoup就能解析出來Tag的具體內容，具體的格式為soup.name,其中name是html下的標籤，具體實例如下：

- print soup.title輸出title標籤下的內容，包括此標籤，這個將會輸出<title>The Dormouse's story</title>

- print soup.head

## 注意：

這裡的格式只能獲取這些標籤的第一個，後面會講到獲取多個標籤的方法。其中對於Tag有兩個重要的屬性name和attrs,分別表示名字和屬性,介紹如下：


- name:對於Tag，它的name就是其本身，如soup.p.name就是p

- attrs是一個字典類型的，對應的是屬性-值，如print soup.p.attrs,輸出的就是{'class': ['title'], 'name': 'dromouse'},當然你也可以得到具體的值，如print soup.p.attrs['class'],輸出的就是[title]是一個列表的類型，因為一個屬性可能對應多個值,當然你也可以通過get方法得到屬性的，如：print soup.p.get('class')。還可以直接使用print soup.p['class']

###get


get方法用於得到標籤下的屬性值，注意這是一個重要的方法，在許多場合都能用到，比如你要得到<img src="#">標籤下的圖像url,那麼就可以用soup.img.get('src'),具體解析如下：



```py
print soup.p.get("class")   #得到第一個p標籤下的src屬性
```

###string
得到標籤下的文本內容，只有在此標籤下沒有子標籤，或者只有一個子標籤的情況下才能返回其中的內容，否則返回的是None具體實例如下：

```py
print soup.p.string #在上面的一段文本中p標籤沒有子標籤，因此能夠正確返回文本的內容
print soup.html.string  #這裡得到的就是None,因為這裡的html中有很多的子標籤
```

### get_text()
可以獲得一個標籤中的所有文本內容，包括子孫節點的內容，這是最常用的方法


## 搜索文檔樹

```py
find_all( name , attrs , recursive , text , **kwargs )
```

find_all是用於搜索節點中所有符合過濾條件的節點

###1.name參數：是Tag的名字，如p,div,title …..
soup.find_all("p") 查找所有的p標籤，返回的是[<b>The Dormouse's story</b>]，可以通過遍歷獲取每一個節點，如下：


```py
ps=soup.find_all("p")
for p in ps:
    print p.get('class')   #得到p標籤下的class屬性
```

傳入正則表達式：soup.find_all(re.compile(r'^b')查找以b開頭的所有標籤，這裡的body和b標籤都會被查到

傳入類列表：如果傳入列表參數,BeautifulSoup會將與列表中任一元素匹配的內容返回.下面代碼找到文檔中所有<a>標籤和<b>標籤

```py
soup.find_all(["a", "b"])
```

### 2. KeyWords參數，就是傳入屬性和對應的屬性值，或者一些其他的表達式

- soup.find_all(id='link2'),這個將會搜索找到所有的id屬性為link2的標籤。傳入正則表達式soup.find_all(href=re.compile("elsie")),這個將會查找所有href屬性滿足正則表達式的標籤

- 傳入多個值：soup.find_all(id='link2',class_='title') ,這個將會查找到同時滿足這兩個屬性的標籤，這裡的class必須用class_傳入參數，因為class是python中的關鍵詞

- 有些屬性不能通過以上方法直接搜索，比如html5中的data-*屬性，不過可以通過attrs參數指定一個字典參數來搜索包含特殊屬性的標籤，如下：


```py
# [<div data-foo="value">foo!</div>]
data_soup.find_all(attrs={"data-foo": "value"})   #注意這裡的atts不僅能夠搜索特殊屬性，亦可以搜索普通屬性
soup.find_all("p",attrs={'class':'title','id':'value'})  #相當與soup.find_all('p',class_='title',id='value')
```


### 3. text參數：通過 text 參數可以搜搜文檔中的字符串內容.與 name 參數的可選值一樣, text 參數接受 字符串 , 正則表達式 , 列表, True

```py
soup.find_all(text="Elsie")
# [u'Elsie']
 
soup.find_all(text=["Tillie", "Elsie", "Lacie"])
# [u'Elsie', u'Lacie', u'Tillie']
 
soup.find_all(text=re.compile("Dormouse"))
[u"The Dormouse's story", u"The Dormouse's story"]
```

### 4. limit參數：find_all() 方法返回全部的搜索結構,如果文檔樹很大那麼搜索會很慢.如果我們不需要全部結果,可以使用 limit 參數限制返回結果的數量.效果與SQL中的limit關鍵字類似,當搜索到的結果數量達到 limit 的限制時,就停止搜索返回結果.

文檔樹中有3個tag符合搜索條件,但結果只返回了2個,因為我們限制了返回數量,代碼如下：


```py
soup.find_all("a", limit=2)
# [<a class="sister" href="http://example.com/elsie" id="link1">Elsie</a>,
#  <a class="sister" href="http://example.com/lacie" id="link2">Lacie</a>]
```

### 5.recursive 參數:調用tag的 find_all() 方法時,BeautifulSoup會檢索當前tag的所有子孫節點,如果只想搜索tag的直接子節點,可以使用參數 recursive=False

```py
find( name , attrs , recursive , text , **kwargs )
```

它與 find_all() 方法唯一的區別是 find_all() 方法的返回結果是值包含一個元素的列表,而 find() 方法直接返回結果,就是直接返回第一匹配到的元素，不是列表，不用遍歷，如soup.find("p").get("class")


# css選擇器

我們在寫 CSS 時，標籤名不加任何修飾，類名前加點，id名前加#，在這裡我們也可以利用類似的方法來篩選元素，用到的方法是 soup.select()，返回類型是 list


### 通過標籤名查找

```py
print soup.select('title') 
#[<title>The Dormouse's story</title>]
print soup.select('a')
#[<a class="sister" href="http://example.com/elsie" id="link1"><!-- Elsie --></a>, <a class="sister" href="http://example.com/lacie" id="link2">Lacie</a>, <a class="sister" href="http://example.com/tillie" id="link3">Tillie</a>]
```

### 通過類名查找

```py
print soup.select('.sister')
#[<a class="sister" href="http://example.com/elsie" id="link1"><!-- Elsie --></a>, <a class="sister" href="http://example.com/lacie" id="link2">Lacie</a>, <a class="sister" href="http://example.com/tillie" id="link3">Tillie</a>]
```

### 通過id名查找

```py
print soup.select('#link1')
#[<a class="sister" href="http://example.com/elsie" id="link1"><!-- Elsie --></a>]
```

### 組合查找

學過css的都知道css選擇器，如p #link1是查找p標籤下的id屬性為link1的標籤

```py
print soup.select('p #link1')    #查找p標籤中內容為id屬性為link1的標籤
#[<a class="sister" href="http://example.com/elsie" id="link1"><!-- Elsie --></a>]
print soup.select("head > title")   #直接查找子標籤
#[<title>The Dormouse's story</title>]
```

### 屬性查找

查找時還可以加入屬性元素，屬性需要用中括號括起來，注意屬性和標籤屬於同一節點，所以中間不能加空格，否則會無法匹配到。

```py
print soup.select('a[class="sister"]')
#[<a class="sister" href="http://example.com/elsie" id="link1"><!-- Elsie --></a>, <a class="sister" href="http://example.com/lacie" id="link2">Lacie</a>, <a class="sister" href="http://example.com/tillie" id="link3">Tillie</a>]
print soup.select('a[href="http://example.com/elsie"]')
#[<a class="sister" href="http://example.com/elsie" id="link1"><!-- Elsie --></a>]
```

同樣，屬性仍然可以與上述查找方式組合，不在同一節點的空格隔開，同一節點的不加空格,代碼如下：


```py
print soup.select('p a[href="http://example.com/elsie"]')
#[<a class="sister" href="http://example.com/elsie" id="link1"><!-- Elsie --></a>]
```

以上的 select 方法返回的結果都是列表形式，可以遍歷形式輸出，然後用 get_text() 方法來獲取它的內容

```py
soup = BeautifulSoup(html, 'lxml')
print type(soup.select('title'))
print soup.select('title')[0].get_text()
for title in soup.select('title'):
    print title.get_text()
```

## 修改文檔樹

Beautiful Soup的強項是文檔樹的搜索,但同時也可以方便的修改文檔樹,這個雖說對於一些其他的爬蟲並不適用，因為他們都是爬文章的內容的，並不需要網頁的源碼並且修改它們，但是在我後續更新的文章中有用python製作pdf電子書的,這個就需要用到修改文檔樹的功能了，詳情請見本人博客

### 修改tag的名稱和屬性

```py
html="""
<p><a href='#'>修改文檔樹</a></p>
"""
soup=BeautifulSoup(html,'lxml')
tag=soup.a    #得到標籤a，可以使用print tag.name輸出標籤
tag['class']='content'    #修改標籤a的屬性class和div
tag['div']='nav'
```

### 修改.string

注意這裡如果標籤的中還嵌套了子孫標籤，那麼如果直接使用string這個屬性會將這裡的所有的子孫標籤都覆蓋掉

```py
html="""
<p><a href='#'>修改文檔樹</a></p>
"""
soup=BeautifulSoup(html,'lxml')
tag=soup.a
tag.string='陳加兵的博客'   #這裡會將修改文檔樹變成修改的內容
print  tag
soup.p.string='陳加兵的博客'   #這裡修改了p標籤的內容，那麼就會覆蓋掉a標籤，直接變成的修改後的文本
print soup
```

### append

append的方法的作用是在在原本標籤文本後面附加文本，就像python中列表的append方法


```py
html="""
<p><a href='#'>修改文檔樹</a></p>
"""
soup=BeautifulSoup(html,'lxml')
soup.a.append("陳加兵的博客")    #在a標籤和麵添加文本，這裡的文本內容將會變成修改文檔樹陳加兵的博客
print soup
print soup.a.contents    #這裡輸出a標籤的內容，這裡的必定是一個帶有兩個元素的列表
```

注意這裡的append方法也可以將一個新的標籤插入到文本的後面，下面將會講到


### new_tag

相信學過js的朋友都知道怎樣創建一個新的標籤，這裡的方法和js中的大同小異，使用的new_tag


```py
html="""
<p><p>
"""
soup=BeautifulSoup(html,'lxml')
tag=soup.p
new_tag=soup.new_tag('a')    #創建一個新的標籤a
new_tag['href']='#'    #添加屬性
new_tag.string='陳加兵的博客'  #添加文本
print new_tag      
tag.append(new_tag)    #將新添加的標籤寫入到p標籤中
print tag
```

### insert

Tag.insert() 方法與 Tag.append() 方法類似,區別是不會把新元素添加到父節點 .contents 屬性的最後,而是把元素插入到指定的位置.與Python列表總的 .insert() 方法的用法下同:


```py
html="""
<p><p>
"""
soup=BeautifulSoup(html,'lxml')
tag=soup.p
new_tag=soup.new_tag('a')
new_tag['href']='#'
new_tag.string='陳加兵的博客'
tag.append("歡迎來到")  #這裡向p標籤中插入文本，這個文本在contents下的序號為0
tag.insert(1,new_tag)   #在contents序號為1的位置插入新的標籤，如果這裡修改成0，那麼將會出現a標籤將會出現在歡飲來到的前面
print tag
```

注意這的1是標籤的內容在contents中的序號，可以用print tag.contents查看當前的內容


### insert_before() 和 insert_after()

insert_before() 方法在當前tag或文本節點前插入內容,insert_after() 方法在當前tag或文本節點後插入內容:

```py
soup = BeautifulSoup("<b>stop</b>")
tag = soup.new_tag("i")
tag.string = "Don't"
soup.b.string.insert_before(tag)
soup.b
# <b><i>Don't</i>stop</b>
soup.b.i.insert_after(soup.new_string(" ever "))
soup.b
# <b><i>Don't</i> ever stop</b>
soup.b.contents
# [<i>Don't</i>, u' ever ', u'stop']
```

### clear

clear用來移除當前節點的所有的內容，包括其中的子孫節點和文本內容

```py
html="""
<p><p>
"""
soup=BeautifulSoup(html,'lxml')
tag=soup.p
new_tag=soup.new_tag('a')
new_tag['href']='#'
new_tag.string='陳加兵的博客'
tag.append("歡迎來到")
tag.insert(1,new_tag)
tag.clear()    #這裡將會移除所有內容
print tag
```


