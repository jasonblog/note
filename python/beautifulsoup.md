# BeautifulSoup


Python官方文档都说自己解析XML的方式存在漏洞了，那我也只能用他推荐的了。

这里我使用的BeautifulSoup，因为其中文文档十分完整清晰，而且相比于defusedxml，它不仅可以解析XML还可以解析HTML，非常方便。文档地址

另外，如果是简单的网页解析任务，可以直接将获取到的网页进行正则表达式匹配也可以达到效果，只是可能会出现各种编码各种错误问题



## Installation

```sh
# 直接apt安装
$ sudo apt-get install Python3-bs4  # 这将安装下面两个包
# pip方式安装
$ pip3 install beautifulsoup4   # 这样直接安装或者下载源码进行安装
$ pip3 install lxml             # 如果是解析xml必须安装一个解析器，文档里说的那个解析器只有这个好安装点，需要提前安装好依赖，apt-get install libxml2-dev, libxslt1-dev, python-dev，还可以使用html.parser这个解析器，这个解析器不会自动添加body元素,CentOS用yum -y install python-devel gcc libxml2 libxml2-devel libxslt libxslt-devel
```

## 基本概念

```sh
TAG：表示xml/html里面的一个元素(节点)，包括标签以及其里面的内容
```

## 基本使用
最简单的使用例子：

```py
import urllib.request
from bs4 import BeautifulSoup
content = "<b><!--Hey, buddy--></b>"   # 表示网页内容
content = urllib.request.urlopen(url)  # 通常做爬虫的时候html来自于网页
soup = BeautifulSoup(content)          # 解析，生成一个bs4.BeautifulSoup
comment = soup.b.string                # 获取<b>标签的内容
```


## 查找

```py
# 查找标签
soup.a             # 查找第一个a标签，返回值就是一个TAG<class 'bs4.element.Tag'>
soup.find('a')     # 同上，都只是查找满足条件的第一个
soup.find_all('a') # 查找所有的a标签，返回一个list获取内容
soup.find_all('a', class_='name')  # 根据标签的属性进行查找，比如这里查找class这个属性为name的a标签
soup.find_all(text="") # 在整个文档中查找一个字符串
soup.find_all('a', limit=3) # 限制只找三个结果
soup.find_all('a', recursive=False) # 只找直接子节点而不递归查找#
soup.find('a', {'class': 'title abc'})	# 直接指定css，适用于比较复杂的css
# CSS选择器
soup.select('a') # 查找a标签，结果是列表
soup.select('.title') # 查找类为title的标签
soup.select('#name')  # 查找id为name的标签
```

## 获取内容

```py
soup.prettify()	# 直接获取所有内容
tag.name        # 如果是Tag，那么返回它本身，例如，如果是a标签，那就返回a；如果是soup对象，那么返回[document]，返回值都是str类型
tag.attrs       # 获取该标签的属性，返回的是一个字典，例如，如果有个a标签是<a class="a" href="#"></a>那么返回{'class': 'a', 'href': '#'}
soup.a['class'] # 直接获取a标签的class属性值
soup.a.get('class'] # 同上
soup.a.string   # 获取标签内的内容，内部所有的字符串，<a>文字部分</a>
soup.a.stripped_strings	# 获取标签内所有的字符串，可以去除空白和空行
soup.a.text     # 获取标签内文字部分<span>abc<a href=""></a></span> 获取abc
soup.prettify() # 获取所有内容
soup.find_all(string=lambda text:isinstance(text, Comment))	# 获取所有的注释
```

## 操作

```py
tag.string = ''	# 修改标签内部的字符串
tag.append("hello")	# 直接在内部字符串后面添加字符串
new_tag = soup.new_tag('a', href='https://haofly.net')	# 添加一个tag
original_tag.append(new_tag)
tag.clear()		# 移除tag内部所有的内容
tag.decompose()	# 移除tag内部所有内容以及tag本身
tag.extract()	# 移除当前tag
```

## 遍历

获取tag内的字符串用tag.string，可以通过unicode方法将NavigableString对象转换成Unicode字符串，如unicode_st

```py
ring = unicode(tag.string)
```

## TroubleShooting

- 如果出现无法找到某些真的存在的标签，可能原因是选择的解析器有问题，可以将lxml换成html5lib


如果要获取xml/html中的注释使用Comment对象，如

```py
markup = "<b><!--Hey, buddy. Want to buy a used parser?--></b>"
soup = BeautifulSoup(markup)
comment = soup.b.string
commment就是注释的东西
print(comment)
>>>u'Hey, buddy. Want to buy a used parser'
可以
print(soup.b.prettify())打印全部<b>
也可以用CDATA替代注释：如
from bs4 import CData
cdata = CData("A CDATA block")
comment.replace_with(cdata)
print(soup.b.prettify())
打印：
<b><![CDATA[A CDATA block]]></b>
```

通过点去属性的方式只能获得当前名字的第一个tag，如果要得到所有的就用soup.find_all(‘a’)

tag的.contents属性可以将tag的子节点以列表的方式输出(包括子节点的所有内容)


```py
head_tag = soup.head
head_tag # <head><title>The Dormouse's story</title></head>  
head_tag.contents
[<title>The Dormouse's story</title>]
title_tag = head_tag.contents[0]
title_tag
#<title>The Dormouse's story</title>  
title_tag.contents
[u'The Dormouse's story']
```

BeautifulSoup对象本身一定会包含子节点，也就是说标签也是该对象的子节点，如 soup.contents[0].name就是html

通过tag的.children生成器，可以对tag的子节点进行循环：


```py
for child in title_tag.children:
    print(child)
    # The Dormouse's story
```

- desendants属性可以对所有tag的子孙节点进行递归循环


```py
for child in head_tag.descendants:
    print(child)
    # <title>The Dormouse's story</title>
    # The Dormouse's story
字符串也是一个子节点
```

如果tag只有一个NavigableString类型的子节点，就可以用title_tag.string访问子节点
如果tag包含多个字符串就用.strings来循环，如：

```py
for string in soup.strings:
    print(repr(string))
    # u"The Dormouse's story"
    # u'\\n\\n'
    # u"The Dormouse's story"
    # u'\\n\\n'
```

使用soup.stripped_strings代替soup.strings可以去掉空白或空行项

父节点就正好相反了，.parent得到父节点，.parents递归得到元素的所有父节点

兄弟节点：.next_sibling，.previous_sibling来访问，通过.next_siblings和.previous_siblings属性对
当前节点的兄弟节点迭代输出for sibling in soup.a.next_siblings:这样子

回退和前进：.next_element和.previouw_element，.next_elements和.previous_elements

查找

find和find_all，还可以传入正则表达式，如soup.find_all(re.compile(“^b”))如果传入的是列表，将会与列表中任一元素匹配
的内容返回，true可以匹配任何标签，如soup.find_all(True)

检查是否包含属性tag.has_attr(‘class’)

find_all( name , attrs , recursive , text , **kwargs )

attr表示具有该属性的name标签，text可以搜索非标签的字符串内容，如soup.find_all(text=”wang”)

soup.find_all(id="link2")[<a class="sister" href="http://example.com/lacie" id="link2">Lacie</a>]
limit参数：find_all()方法返回全部的搜索结果，如果文档数很大那么搜索会很慢，如果不需要全部结果，可以用limit参数，类似于SQL中的limi
t，如soup.find_all(“a”, limit=2)

如果只搜索直接子节点，就加上recursive=False参数

可以不用指明find_all，如soup.find_all(“a”)可以用soup(“a”)代替，soup.title.find_all(text=True
)可以用soup.title(text=True)代替

其他功能，按CSS搜索、支持CSS选择器，支持修改文档树

find()方法至返回一个，其他的还有find_parents()和find_parent()，find_next_siblings()，find_next
_sibling()，find_previous_siblings()，find_previous_sibling()，find_all_next()，fi
nd_next()，find_all_previous()，find_previous()

如果只想得到tag中包含的文本内容，那么就可以用get_text()方法，获取到tag包含的所有文本内容包括子孙tag中的内容

注：beautifulsoup会自动将tag变为小写，只有添加”xml”选项才能大小写敏感，因为不指定就默认是html，html的标签对大小写不敏感，所以推
荐还是把lxml XML解析器安上，不过要先弄上什么C语言库


## 修改

```py
# 删除当前节点
tag.extract()  

# 插入节点
new_tag = '<url>dagasgga</url>'
new_tag = BeautifulSoup(new_url, 'html.parser')
soup.tag.insert(位置如1, new_tag)
```