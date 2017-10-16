# 使用BeautifulSoup


最近工作需求，須要從html的網頁抓取特定的資料，

本來想說用Regular expression來判斷過濾，但是這樣反而比較麻煩...

所已找到了一個套件，叫做 BeautifulSoup。

它可以拿來parsing html的內容，並擷取你想要的tag以及content資料，

使用上算是蠻方便，至於執行效能上...算是普通吧(?)

廢話不多說，那來開始介紹如何使用吧！


##安裝&使用
方法1: 用 pip 安裝

```sh
pip install beautifulsoup4
```

方法2: 用 easy_install 安裝

```sh
easy_install beautifulsoup4
```
方法3: 從原始碼安裝

首先要安裝setuptools

```sh
pip install -U pip setuptools
```

首先到 官方網站 下載python套件！
( https://www.crummy.com/software/BeautifulSoup/bs4/download/ )

解壓縮後 到該目錄下，會看到幾個.py檔案，其中會有一個setup.py，下安裝的指令:

```sh
python setup.py install
```

##測試是否安裝成功

```sh
python -c "import bs4; print bs4.__version__"
```

沒有任何問題就是安裝完了！

接著開始寫程式吧...要使用這個libary時，
需要在程式裡加入:

```sh
from bs4 import BeautifulSoup
```

在版本3.x以前，要用這方式

```sh
from BeautifulSoup import BeautifulSoup
```

很多使用方法，在官方的文件都有詳細記載！
英文文件


### 小範例程式1

```py
# -*- coding: UTF-8 -*-
import sys
from BeautifulSoup import BeautifulSoup
 
reload(sys)
sys.setdefaultencoding('utf8')

# HTML資料
data = [
    '<html>',
    '<head>',
    '<title>TITLE</title>',
    '</head>',
    '<body>Hi!',
    '<a href="test1.html" attr="1">Link1</a>',
    '<a href="test2.html" attr="2">Link2</a>',
    '</body>',
    '</html>'
]  

soup = BeautifulSoup(''.join(data))       # 讀進BeautifulSoup
print "[SOAP]:", soup
print "[HTML]:", soup.contents[0]         # <html>的資料
print "[TAG-NAME]", soup.contents[0].name # 該TAG的名稱
print "[HTML>HEAD]:", soup.contents[0].contents[0] # html > head

print

body = soup.contents[0].contents[1] # html > body
print "[TAG-NAME]:", body.name      # 該TAG的名稱
print "[Parent]:", body.parent      # 往上一個TAG
print "[Next]:", body.next          # 往下

print

print soup.html.head.title         # 直接用TAG的路徑找
print soup.html.head.title.string  # 取得TAG的內容
print soup.findAll('a', href=True) # 取得所有<a>
print soup.findAll('a', href=True, attr='1') # 取得<a>並且屬性attr='1'
```

上面是一些基本的用法，至於比較進階的用法...

還是去參考文件，有詳細的說明會比較好理解！


### 小範例程式2


```py
# -*- coding: UTF-8 -*-
from __future__ import print_function

import os, sys, urllib, urllib2
from urllib2 import urlopen, Request
from bs4 import BeautifulSoup
from time import strftime


reload(sys)
sys.setdefaultencoding('utf8')

constellation = [
    [u'牡羊座', 'aries'],
    [u'金牛座', 'taurus'],
    [u'雙子座', 'gemini'],
    [u'巨蟹座', 'cancer'],
    [u'獅子座', 'leo'],
    [u'處女座', 'virgo'],
    [u'天秤座', 'libra'],
    [u'天蠍座', 'scorpio'],
    [u'射手座', 'sagittarius'],
    [u'魔羯座', 'capricorn'],
    [u'水瓶座', 'aquarius'],
    [u'雙魚座', 'pisces'],
]

web_url = "http://mindcity.sina.com.tw/west/12horos/today/"

date_t = strftime("%Y%m%d") # 取得今天日期

for x_list in constellation:
    try:
        url = web_url + x_list[1]
        print(x_list[0], url)
        data = urllib2.urlopen(url)
        soup = BeautifulSoup(data, "html.parser")

        div_lotstars = soup.find('div', {'class': 'lotstars'}) # 找出 div class="lotstars"
        if div_lotstars == None:
            print("div .lotstars NOT FOUND!!")
            continue

        soup2 = BeautifulSoup(str(div_lotstars), "html.parser")
        h4 = soup2.findAll("h4")                 # 找出h4
        p = soup2.findAll("p")                   # 找出p
        for title, value in zip(h4[:9], p[:9]):  # 取前9個
            print(title.contents[0], value.contents[0])

        print("-" * 20)

    except Exception,e:
        print(e)
```

若比較在意執行效能的快慢，也許可以使用看看lxml這個套件

追記

## 露天拍賣

```py
# -*- coding: utf-8 -*-
from bs4 import BeautifulSoup

html_doc = """
<html>
<body>
<div class="section-content-row">
    <h2 class="rt-goods-list-item-name">
                            <a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21716971337096" title="前往Lowe Alpine Strike 24 運動背包/都會日用後背包 FDP55 黑商品頁面" target="_blank">
                              <span class="rt-goods-list-item-name-text" itemprop="name">Lowe Alpine Strike 24 運動背包/都會日用後背包 FDP55 黑</span>
                            </a>
                          </h2>
    <div class="rt-goods-list-item-price-sell">
        <div class="rt-goods-list-item-price">
            <strong class="item-price-symbol rt-text-larger rt-text-price" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">2,363<meta itemprop="price" content="2363"><meta itemprop="priceCurrency" content="TWD">
                                  </strong>

        </div>
    </div>
</div>
</body>
</html>
"""

soup = BeautifulSoup(html_doc, 'html.parser')
for each_div in soup.findAll('div',{'class':'section-content-row'}):
    print each_div.h2.span.text 
    print each_div.h2.a['href']
    print each_div.div.div.strong.text
    ```
