# Python使用beautifulsoup解析HTML、XML


Python官方文檔都說自己解析XML的方式存在漏洞了，那我也只能用他推薦的了。

這裡我使用的BeautifulSoup，因為其中文文檔十分完整清晰，而且相比於defusedxml，它不僅可以解析XML還可以解析HTML，非常方便。文檔地址

另外，如果是簡單的網頁解析任務，可以直接將獲取到的網頁進行正則表達式匹配也可以達到效果，只是可能會出現各種編碼各種錯誤問題



## Installation

```sh
# 直接apt安裝
$ sudo apt-get install Python3-bs4  # 這將安裝下面兩個包
# pip方式安裝
$ pip3 install beautifulsoup4   # 這樣直接安裝或者下載源碼進行安裝
$ pip3 install lxml             # 如果是解析xml必須安裝一個解析器，文檔裡說的那個解析器只有這個好安裝點，需要提前安裝好依賴，apt-get install libxml2-dev, libxslt1-dev, python-dev，還可以使用html.parser這個解析器，這個解析器不會自動添加body元素,CentOS用yum -y install python-devel gcc libxml2 libxml2-devel libxslt libxslt-devel
```

## 基本概念

```sh
TAG：表示xml/html裡面的一個元素(節點)，包括標籤以及其裡面的內容
```

## 基本使用
最簡單的使用例子：

```py
import urllib.request
from bs4 import BeautifulSoup
content = "<b><!--Hey, buddy--></b>"   # 表示網頁內容
content = urllib.request.urlopen(url)  # 通常做爬蟲的時候html來自於網頁
soup = BeautifulSoup(content)          # 解析，生成一個bs4.BeautifulSoup
comment = soup.b.string                # 獲取<b>標籤的內容
```


## 查找

```py
# 查找標籤
soup.a             # 查找第一個a標籤，返回值就是一個TAG<class 'bs4.element.Tag'>
soup.find('a')     # 同上，都只是查找滿足條件的第一個
soup.find_all('a') # 查找所有的a標籤，返回一個list獲取內容
soup.find_all('a', class_='name')  # 根據標籤的屬性進行查找，比如這裡查找class這個屬性為name的a標籤
soup.find_all(text="") # 在整個文檔中查找一個字符串
soup.find_all('a', limit=3) # 限制只找三個結果
soup.find_all('a', recursive=False) # 只找直接子節點而不遞歸查找#
soup.find('a', {'class': 'title abc'})	# 直接指定css，適用於比較複雜的css
# CSS選擇器
soup.select('a') # 查找a標籤，結果是列表
soup.select('.title') # 查找類為title的標籤
soup.select('#name')  # 查找id為name的標籤
```

## 獲取內容

```py
soup.prettify()	# 直接獲取所有內容
tag.name        # 如果是Tag，那麼返回它本身，例如，如果是a標籤，那就返回a；如果是soup對象，那麼返回[document]，返回值都是str類型
tag.attrs       # 獲取該標籤的屬性，返回的是一個字典，例如，如果有個a標籤是<a class="a" href="#"></a>那麼返回{'class': 'a', 'href': '#'}
soup.a['class'] # 直接獲取a標籤的class屬性值
soup.a.get('class'] # 同上
soup.a.string   # 獲取標籤內的內容，內部所有的字符串，<a>文字部分</a>
soup.a.stripped_strings	# 獲取標籤內所有的字符串，可以去除空白和空行
soup.a.text     # 獲取標籤內文字部分<span>abc<a href=""></a></span> 獲取abc
soup.prettify() # 獲取所有內容
soup.find_all(string=lambda text:isinstance(text, Comment))	# 獲取所有的註釋
```

## 操作

```py
tag.string = ''	# 修改標籤內部的字符串
tag.append("hello")	# 直接在內部字符串後面添加字符串
new_tag = soup.new_tag('a', href='https://haofly.net')	# 添加一個tag
original_tag.append(new_tag)
tag.clear()		# 移除tag內部所有的內容
tag.decompose()	# 移除tag內部所有內容以及tag本身
tag.extract()	# 移除當前tag
```

## 遍歷

獲取tag內的字符串用tag.string，可以通過unicode方法將NavigableString對象轉換成Unicode字符串，如unicode_st

```py
ring = unicode(tag.string)
```

## TroubleShooting

- 如果出現無法找到某些真的存在的標籤，可能原因是選擇的解析器有問題，可以將lxml換成html5lib


如果要獲取xml/html中的註釋使用Comment對象，如

```py
markup = "<b><!--Hey, buddy. Want to buy a used parser?--></b>"
soup = BeautifulSoup(markup)
comment = soup.b.string
commment就是註釋的東西
print(comment)
>>>u'Hey, buddy. Want to buy a used parser'
可以
print(soup.b.prettify())打印全部<b>
也可以用CDATA替代註釋：如
from bs4 import CData
cdata = CData("A CDATA block")
comment.replace_with(cdata)
print(soup.b.prettify())
打印：
<b><![CDATA[A CDATA block]]></b>
```

通過點去屬性的方式只能獲得當前名字的第一個tag，如果要得到所有的就用soup.find_all(‘a’)

tag的.contents屬性可以將tag的子節點以列表的方式輸出(包括子節點的所有內容)


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

BeautifulSoup對象本身一定會包含子節點，也就是說標籤也是該對象的子節點，如 soup.contents[0].name就是html

通過tag的.children生成器，可以對tag的子節點進行循環：


```py
for child in title_tag.children:
    print(child)
    # The Dormouse's story
```

- desendants屬性可以對所有tag的子孫節點進行遞歸循環


```py
for child in head_tag.descendants:
    print(child)
    # <title>The Dormouse's story</title>
    # The Dormouse's story
字符串也是一個子節點
```

如果tag只有一個NavigableString類型的子節點，就可以用title_tag.string訪問子節點
如果tag包含多個字符串就用.strings來循環，如：

```py
for string in soup.strings:
    print(repr(string))
    # u"The Dormouse's story"
    # u'\\n\\n'
    # u"The Dormouse's story"
    # u'\\n\\n'
```

使用soup.stripped_strings代替soup.strings可以去掉空白或空行項

父節點就正好相反了，.parent得到父節點，.parents遞歸得到元素的所有父節點

兄弟節點：.next_sibling，.previous_sibling來訪問，通過.next_siblings和.previous_siblings屬性對
當前節點的兄弟節點迭代輸出for sibling in soup.a.next_siblings:這樣子

回退和前進：.next_element和.previouw_element，.next_elements和.previous_elements

查找

find和find_all，還可以傳入正則表達式，如soup.find_all(re.compile(“^b”))如果傳入的是列表，將會與列表中任一元素匹配
的內容返回，true可以匹配任何標籤，如soup.find_all(True)

檢查是否包含屬性tag.has_attr(‘class’)

find_all( name , attrs , recursive , text , **kwargs )

attr表示具有該屬性的name標籤，text可以搜索非標籤的字符串內容，如soup.find_all(text=”wang”)

soup.find_all(id="link2")[<a class="sister" href="http://example.com/lacie" id="link2">Lacie</a>]
limit參數：find_all()方法返回全部的搜索結果，如果文檔數很大那麼搜索會很慢，如果不需要全部結果，可以用limit參數，類似於SQL中的limi
t，如soup.find_all(“a”, limit=2)

如果只搜索直接子節點，就加上recursive=False參數

可以不用指明find_all，如soup.find_all(“a”)可以用soup(“a”)代替，soup.title.find_all(text=True
)可以用soup.title(text=True)代替

其他功能，按CSS搜索、支持CSS選擇器，支持修改文檔樹

find()方法至返回一個，其他的還有find_parents()和find_parent()，find_next_siblings()，find_next
_sibling()，find_previous_siblings()，find_previous_sibling()，find_all_next()，fi
nd_next()，find_all_previous()，find_previous()

如果只想得到tag中包含的文本內容，那麼就可以用get_text()方法，獲取到tag包含的所有文本內容包括子孫tag中的內容

注：beautifulsoup會自動將tag變為小寫，只有添加”xml”選項才能大小寫敏感，因為不指定就默認是html，html的標籤對大小寫不敏感，所以推
薦還是把lxml XML解析器安上，不過要先弄上什麼C語言庫


## 修改

```py
# 刪除當前節點
tag.extract()  

# 插入節點
new_tag = '<url>dagasgga</url>'
new_tag = BeautifulSoup(new_url, 'html.parser')
soup.tag.insert(位置如1, new_tag)
```