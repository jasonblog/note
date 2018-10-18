# python爬虫之BeautifulSoup

##简介
Beautiful Soup提供一些简单的、python式的函数用来处理导航、搜索、修改分析树等功能。它是一个工具箱，通过解析文档为用户提供需要抓取的数据，因为简单，所以不需要多少代码就可以写出一个完整的应用程序。Beautiful Soup自动将输入文档转换为Unicode编码，输出文档转换为utf-8编码。你不需要考虑编码方式，除非文档没有指定一个编码方式，这时，Beautiful Soup就不能自动识别编码方式了。然后，你仅仅需要说明一下原始编码方式就可以了。


Beautiful Soup已成为和lxml、html6lib一样出色的python解释器，为用户灵活地提供不同的解析策略或强劲的速度。



## 安装

```sh
pip install BeautifulSoup4
easy_install BeautifulSoup4
```

## 创建BeautifulSoup对象

- 首先应该导入BeautifulSoup类库 from bs4 import BeautifulSoup

- 下面开始创建对像，在开始之前为了方便演示，先创建一个html文本，如下：


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

- 创建对象：soup=BeautifulSoup(html,'lxml'),这里的lxml是解析的类库，目前来说个人觉得最好的解析器了，一直在用这个，安装方法：pip install lxml


## Tag

Tag就是html中的一个标签，用BeautifulSoup就能解析出来Tag的具体内容，具体的格式为soup.name,其中name是html下的标签，具体实例如下：

- print soup.title输出title标签下的内容，包括此标签，这个将会输出<title>The Dormouse's story</title>

- print soup.head

## 注意：

这里的格式只能获取这些标签的第一个，后面会讲到获取多个标签的方法。其中对于Tag有两个重要的属性name和attrs,分别表示名字和属性,介绍如下：


- name:对于Tag，它的name就是其本身，如soup.p.name就是p

- attrs是一个字典类型的，对应的是属性-值，如print soup.p.attrs,输出的就是{'class': ['title'], 'name': 'dromouse'},当然你也可以得到具体的值，如print soup.p.attrs['class'],输出的就是[title]是一个列表的类型，因为一个属性可能对应多个值,当然你也可以通过get方法得到属性的，如：print soup.p.get('class')。还可以直接使用print soup.p['class']

###get


get方法用于得到标签下的属性值，注意这是一个重要的方法，在许多场合都能用到，比如你要得到<img src="#">标签下的图像url,那么就可以用soup.img.get('src'),具体解析如下：



```py
print soup.p.get("class")   #得到第一个p标签下的src属性
```

###string
得到标签下的文本内容，只有在此标签下没有子标签，或者只有一个子标签的情况下才能返回其中的内容，否则返回的是None具体实例如下：

```py
print soup.p.string #在上面的一段文本中p标签没有子标签，因此能够正确返回文本的内容
print soup.html.string  #这里得到的就是None,因为这里的html中有很多的子标签
```

### get_text()
可以获得一个标签中的所有文本内容，包括子孙节点的内容，这是最常用的方法


## 搜索文档树

```py
find_all( name , attrs , recursive , text , **kwargs )
```

find_all是用于搜索节点中所有符合过滤条件的节点

###1.name参数：是Tag的名字，如p,div,title …..
soup.find_all("p") 查找所有的p标签，返回的是[<b>The Dormouse's story</b>]，可以通过遍历获取每一个节点，如下：


```py
ps=soup.find_all("p")
for p in ps:
    print p.get('class')   #得到p标签下的class属性
```

传入正则表达式：soup.find_all(re.compile(r'^b')查找以b开头的所有标签，这里的body和b标签都会被查到

传入类列表：如果传入列表参数,BeautifulSoup会将与列表中任一元素匹配的内容返回.下面代码找到文档中所有<a>标签和<b>标签

```py
soup.find_all(["a", "b"])
```

### 2. KeyWords参数，就是传入属性和对应的属性值，或者一些其他的表达式

- soup.find_all(id='link2'),这个将会搜索找到所有的id属性为link2的标签。传入正则表达式soup.find_all(href=re.compile("elsie")),这个将会查找所有href属性满足正则表达式的标签

- 传入多个值：soup.find_all(id='link2',class_='title') ,这个将会查找到同时满足这两个属性的标签，这里的class必须用class_传入参数，因为class是python中的关键词

- 有些属性不能通过以上方法直接搜索，比如html5中的data-*属性，不过可以通过attrs参数指定一个字典参数来搜索包含特殊属性的标签，如下：


```py
# [<div data-foo="value">foo!</div>]
data_soup.find_all(attrs={"data-foo": "value"})   #注意这里的atts不仅能够搜索特殊属性，亦可以搜索普通属性
soup.find_all("p",attrs={'class':'title','id':'value'})  #相当与soup.find_all('p',class_='title',id='value')
```


### 3. text参数：通过 text 参数可以搜搜文档中的字符串内容.与 name 参数的可选值一样, text 参数接受 字符串 , 正则表达式 , 列表, True

```py
soup.find_all(text="Elsie")
# [u'Elsie']
 
soup.find_all(text=["Tillie", "Elsie", "Lacie"])
# [u'Elsie', u'Lacie', u'Tillie']
 
soup.find_all(text=re.compile("Dormouse"))
[u"The Dormouse's story", u"The Dormouse's story"]
```

### 4. limit参数：find_all() 方法返回全部的搜索结构,如果文档树很大那么搜索会很慢.如果我们不需要全部结果,可以使用 limit 参数限制返回结果的数量.效果与SQL中的limit关键字类似,当搜索到的结果数量达到 limit 的限制时,就停止搜索返回结果.

文档树中有3个tag符合搜索条件,但结果只返回了2个,因为我们限制了返回数量,代码如下：


```py
soup.find_all("a", limit=2)
# [<a class="sister" href="http://example.com/elsie" id="link1">Elsie</a>,
#  <a class="sister" href="http://example.com/lacie" id="link2">Lacie</a>]
```

### 5.recursive 参数:调用tag的 find_all() 方法时,BeautifulSoup会检索当前tag的所有子孙节点,如果只想搜索tag的直接子节点,可以使用参数 recursive=False

```py
find( name , attrs , recursive , text , **kwargs )
```

它与 find_all() 方法唯一的区别是 find_all() 方法的返回结果是值包含一个元素的列表,而 find() 方法直接返回结果,就是直接返回第一匹配到的元素，不是列表，不用遍历，如soup.find("p").get("class")


# css选择器

我们在写 CSS 时，标签名不加任何修饰，类名前加点，id名前加#，在这里我们也可以利用类似的方法来筛选元素，用到的方法是 soup.select()，返回类型是 list


### 通过标签名查找

```py
print soup.select('title') 
#[<title>The Dormouse's story</title>]
print soup.select('a')
#[<a class="sister" href="http://example.com/elsie" id="link1"><!-- Elsie --></a>, <a class="sister" href="http://example.com/lacie" id="link2">Lacie</a>, <a class="sister" href="http://example.com/tillie" id="link3">Tillie</a>]
```

### 通过类名查找

```py
print soup.select('.sister')
#[<a class="sister" href="http://example.com/elsie" id="link1"><!-- Elsie --></a>, <a class="sister" href="http://example.com/lacie" id="link2">Lacie</a>, <a class="sister" href="http://example.com/tillie" id="link3">Tillie</a>]
```

### 通过id名查找

```py
print soup.select('#link1')
#[<a class="sister" href="http://example.com/elsie" id="link1"><!-- Elsie --></a>]
```

### 组合查找

学过css的都知道css选择器，如p #link1是查找p标签下的id属性为link1的标签

```py
print soup.select('p #link1')    #查找p标签中内容为id属性为link1的标签
#[<a class="sister" href="http://example.com/elsie" id="link1"><!-- Elsie --></a>]
print soup.select("head > title")   #直接查找子标签
#[<title>The Dormouse's story</title>]
```

### 属性查找

查找时还可以加入属性元素，属性需要用中括号括起来，注意属性和标签属于同一节点，所以中间不能加空格，否则会无法匹配到。

```py
print soup.select('a[class="sister"]')
#[<a class="sister" href="http://example.com/elsie" id="link1"><!-- Elsie --></a>, <a class="sister" href="http://example.com/lacie" id="link2">Lacie</a>, <a class="sister" href="http://example.com/tillie" id="link3">Tillie</a>]
print soup.select('a[href="http://example.com/elsie"]')
#[<a class="sister" href="http://example.com/elsie" id="link1"><!-- Elsie --></a>]
```

同样，属性仍然可以与上述查找方式组合，不在同一节点的空格隔开，同一节点的不加空格,代码如下：


```py
print soup.select('p a[href="http://example.com/elsie"]')
#[<a class="sister" href="http://example.com/elsie" id="link1"><!-- Elsie --></a>]
```

以上的 select 方法返回的结果都是列表形式，可以遍历形式输出，然后用 get_text() 方法来获取它的内容

```py
soup = BeautifulSoup(html, 'lxml')
print type(soup.select('title'))
print soup.select('title')[0].get_text()
for title in soup.select('title'):
    print title.get_text()
```

## 修改文档树

Beautiful Soup的强项是文档树的搜索,但同时也可以方便的修改文档树,这个虽说对于一些其他的爬虫并不适用，因为他们都是爬文章的内容的，并不需要网页的源码并且修改它们，但是在我后续更新的文章中有用python制作pdf电子书的,这个就需要用到修改文档树的功能了，详情请见本人博客

### 修改tag的名称和属性

```py
html="""
<p><a href='#'>修改文档树</a></p>
"""
soup=BeautifulSoup(html,'lxml')
tag=soup.a    #得到标签a，可以使用print tag.name输出标签
tag['class']='content'    #修改标签a的属性class和div
tag['div']='nav'
```

### 修改.string

注意这里如果标签的中还嵌套了子孙标签，那么如果直接使用string这个属性会将这里的所有的子孙标签都覆盖掉

```py
html="""
<p><a href='#'>修改文档树</a></p>
"""
soup=BeautifulSoup(html,'lxml')
tag=soup.a
tag.string='陈加兵的博客'   #这里会将修改文档树变成修改的内容
print  tag
soup.p.string='陈加兵的博客'   #这里修改了p标签的内容，那么就会覆盖掉a标签，直接变成的修改后的文本
print soup
```

### append

append的方法的作用是在在原本标签文本后面附加文本，就像python中列表的append方法


```py
html="""
<p><a href='#'>修改文档树</a></p>
"""
soup=BeautifulSoup(html,'lxml')
soup.a.append("陈加兵的博客")    #在a标签和面添加文本，这里的文本内容将会变成修改文档树陈加兵的博客
print soup
print soup.a.contents    #这里输出a标签的内容，这里的必定是一个带有两个元素的列表
```

注意这里的append方法也可以将一个新的标签插入到文本的后面，下面将会讲到


### new_tag

相信学过js的朋友都知道怎样创建一个新的标签，这里的方法和js中的大同小异，使用的new_tag


```py
html="""
<p><p>
"""
soup=BeautifulSoup(html,'lxml')
tag=soup.p
new_tag=soup.new_tag('a')    #创建一个新的标签a
new_tag['href']='#'    #添加属性
new_tag.string='陈加兵的博客'  #添加文本
print new_tag      
tag.append(new_tag)    #将新添加的标签写入到p标签中
print tag
```

### insert

Tag.insert() 方法与 Tag.append() 方法类似,区别是不会把新元素添加到父节点 .contents 属性的最后,而是把元素插入到指定的位置.与Python列表总的 .insert() 方法的用法下同:


```py
html="""
<p><p>
"""
soup=BeautifulSoup(html,'lxml')
tag=soup.p
new_tag=soup.new_tag('a')
new_tag['href']='#'
new_tag.string='陈加兵的博客'
tag.append("欢迎来到")  #这里向p标签中插入文本，这个文本在contents下的序号为0
tag.insert(1,new_tag)   #在contents序号为1的位置插入新的标签，如果这里修改成0，那么将会出现a标签将会出现在欢饮来到的前面
print tag
```

注意这的1是标签的内容在contents中的序号，可以用print tag.contents查看当前的内容


### insert_before() 和 insert_after()

insert_before() 方法在当前tag或文本节点前插入内容,insert_after() 方法在当前tag或文本节点后插入内容:

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

clear用来移除当前节点的所有的内容，包括其中的子孙节点和文本内容


