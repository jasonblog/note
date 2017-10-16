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



```py
# -*- coding: utf-8 -*-
from bs4 import BeautifulSoup

html_doc = """
<!DOCTYPE html>

<html lang="zh-TW">
<head>
<meta charset="utf-8">
<meta content="IE=edge;" http-equiv="X-UA-Compatible"/>
<title>搜尋結果 : Lowe Alpine Strike - 露天拍賣</title>
<meta content="" name="description"/>
<meta content="Lowe Alpine Strike - 露天拍賣-台灣 NO.1 拍賣網站" property="og:title"/>
<meta content="Lowe Alpine Strike - 露天拍賣-台灣 NO.1 拍賣網站" property="og:description"/>
<meta content="露天拍賣-台灣 NO.1 拍賣網站" property="og:site_name">
<meta content="website" property="og:type"/>
<script src="http://www.ruten.com.tw/js/mobile_detect.min.js?v=20160728" type="text/javascript"></script>
<link href="http://www.ruten.com.tw/css/d/v2/style-reset.css?v=20160803" rel="stylesheet" type="text/css"/>
<link href="http://www.ruten.com.tw/css/d/v2/style-base.css?v=20171005_03c9be26ca" rel="stylesheet" type="text/css"/>
<link href="http://www.ruten.com.tw/css/d/v2/style-goods-item.css?v=20171003_974297b0e0" rel="stylesheet" type="text/css"/>
<link href="http://www.ruten.com.tw/css/d/v2/style-search.css?v=20171016_4e569c9c54" rel="stylesheet" type="text/css"/>
<script>
    var RT = window.RT || {};
    RT.context = {"g_class_val":0,"ahd_host":"http:\/\/ahd.ruten.com.tw","point_host":"https:\/\/point.ruten.com.tw","bid_img_dir":"http:\/\/www.ruten.com.tw\/images","asd_adword":"Lowe+Alpine+Strike","pager_config":{"p":1,"total":59,"perPage":30},"filter_config":{"fy":0,"h":0,"p1":"","p2":"","fx12":false,"fx14":false,"fx11":false,"fx10":false,"fx13":false,"fx9":false,"fx15":false,"fx16":false,"fx7":false,"fx8":false,"fx3":false,"fx4":false,"fx6":false,"isqty":false,"onsale":false,"ispp":false},"search_found":59,"x":"uaPOKRzLv3n93bBXVZ9r3JTwgNfDbLxsdfJCU_N4lWri5tdHR_KRIpIuWOFQgX-dja0lmZMFEAeEsJfRs36GmAU2Z-SiFfgY03rKt2S2QLYKFX8kcfBn5aic6OjrIcbG","g_no_str":"21716971337096,21716971330538,21716927114023,21715812650378,21715812650323,21715812499896,21715812397747,21715812376328,21715811754114,21715811739443,21715811707875,21643141490454,21642042865684,21642042103713,21642042004735,21642041621964,21619691674383,21619691632860,21618643483006,21618643479371,21618643470169,21612175262410,21612175204506,21537638529247,21526917670477,21525839284177,21525839264827,21525839249785,21525839238794,21525839223754","queryString":"qdcf-V6b3ljgue7eQMYqQ4KYcmnOfojiChy4PpycdcnAYfIQdh_BT2c2L7oQ4Jly5cPbgP9hdKGqHIEyYQVUMHecoVSUjzZbuMYh1g0ob0_OPa4CP5NxpumrekPmjq_CW8PjZRUyqYQdAoIJJRuBtARv12ov3QVsz0cGGETYAtdQ2OFL3m8v_c9AG5TPVSukpEIU-kuEFZE7_-hclnSwq7qPJe_zHqohUu-4BqSId12nd5GTKcT2rTcrqitCWjTexCRqt6RwGMnXeNak65-ruerCrlqjnI9w__BlCvGP7SZQNIdCrvtzOoiSwSKeYnzcOu2MX357VpztJ_WLU1e0Xh_bU-Xt6nE7KO2MkEbOJ2LKwGPvaM92fPtEmCK2IKk4CHYhP7tp6ae5kYnh1nkMUiAunA4QovyGmGg2A31axVg"};
  </script>
</meta></meta></head>
<body>
<div class="rt-page">
<div class="rt-header" data-module="rt-header">
<div class="rt-header-inner">
<div class="rt-header-part">
<h1 class="rt-heading" itemscope="" itemtype="http://schema.org/WPHeader">
<a class="rt-logo" href="http://www.ruten.com.tw" itemprop="url" title="前往露天拍賣首頁">
<span class="rt-logo-title hide" itemprop="name">露天拍賣-台灣 NO.1 拍賣網站</span>
<img alt="露天拍賣" class="rt-logo-image" src="http://www.ruten.com.tw/images/logo_s.gif"/>
</a>
</h1>
<div class="rt-header-part rt-header-login" id="header_login_info">
<div class="rt-header-loading">
<span class="rti rti-spin rti-refresh"></span>
</div>
<div class="rt-header-loaded">
<div class="rt-header-link">
<strong><a id="header_user_nick" title="前往個人賣場"></a></strong>
</div>
<div class="rt-messenger-inbox">
<div class="rt-messenger-icon-anchor">
<div :class="{ active: !inboxClose, 'im-off': !messengerOn }" @click="toggleInbox" class="rt-messenger-icon-static rt-messenger-chat-icon" v-on-clickaway="clickOutsideOfInbox">
</div>
<div :class="{ new: unreadMsgNotification }" class="rt-messenger-inbox-notification-dot"></div>
<div :class="{ close: inboxClose }" class="rt-messenger-inbox-bubble" v-cloak="">
<div class="rt-messenger-inbox-bubble-arrow-top"></div>
<div class="rt-messenger-inbox-title">對話紀錄</div>
<div :class="{ 'rt-messenger-new-message': userData.unreadMsg &gt; 0 }" @click="loadConversation(userData.userId)" class="rt-messenger-inbox-conversation" v-for="(userData, index) in inbox" v-if="index &lt; 4">
                      ${ userData.storeName }
                      <span class="rt-messenger-badge" v-show="userData.unreadMsg &gt; 0">${ userData.unreadMsg | unreadMsgCounter }</span>
</div>
<a class="rt-messenger-inbox-history-link" href="https://mybid.ruten.com.tw/message/messenger.php">全部紀錄</a>
</div>
</div>
</div>
<div class="rt-header-link">
<a data-type="logout-link" href="https://member.ruten.com.tw/user/logout.php" id="header_logout_link" title="登出頁面">登出</a>
</div>
</div>
<div class="rt-header-not-loaded">
<div class="rt-header-link">
<a href="https://member.ruten.com.tw/user/register_step1.htm" rel="nofollow" title="免費註冊">免費註冊</a>
</div>
<div class="rt-header-link">
<a href="https://member.ruten.com.tw/user/login.htm" rel="nofollow" title="登入頁面">登入</a>
</div>
</div>
<div class="rt-header-error">
<div class="rt-header-link">
<a href="https://member.ruten.com.tw/user/login.htm" rel="nofollow" title="登入頁面">登入</a>
</div>
</div>
</div>
</div>
<ul class="rt-nav rt-header-part" itemscope="" itemtype="http://schema.org/SiteNavigationElement">
<li>
<form action="http://find.ruten.com.tw/search/s000.php" class="rt-header-part rt-search-form" data-type="search-form" id="header_search_form" name="header_search_form">
<input name="enc" type="hidden" value="u"/>
<input name="searchfrom" type="hidden" value="headbar"/>
<div class="search-bar-keyword-wrap" v-on-clickaway="closeSuggestion">
<input @focus="openSuggestion" @keydown.down.prevent="selectNext" @keydown.enter="selectionSubmitting" @keydown.up.prevent="selectPrevious" autocomplete="off" class="header-search-input" data-type="keyword-input" id="keyword" name="k" placeholder="請輸入關鍵字" type="text" v-model="keywordInput">
<suggestion-keyword-template :current-selected-item-index="currentSelectedItemIndex" :is-open="isOpen" :keyword-input="keywordInput" :suggestion-keyword-array="suggestionKeywordArray"></suggestion-keyword-template>
</input></div>
<input class="header-search-submit" name="search_button" type="submit" value="搜尋"/>
</form>
</li>
<li>
<a class="rt-nav-link" href="https://mybid.ruten.com.tw/upload/step1.htm" itemprop="url" rel="nofollow" title="我要賣東西">
<span class="rt-nav-link-name" itemprop="name">賣東西</span>
</a>
</li>
<li>
<a class="rt-nav-link" href="https://mybid.ruten.com.tw/master/my.php" itemprop="url" rel="nofollow" title="進入我的拍賣管理中心">
<span class="rt-nav-link-name" itemprop="name">我的拍賣</span>
</a>
</li>
<li>
<a class="rt-nav-link" href="http://pub.ruten.com.tw/adb/3.html" itemprop="url" rel="nofollow" title="購買廣告">
<span class="rt-nav-link-name" itemprop="name">買廣告</span>
</a>
</li>
</ul>
</div>
</div>
<div class="rt-wrap rt-wrap-search">
<div class="rt-ad">
<div class="rt-ad-body">
<div class="rt-ad-mb rt-ad-leaderboard" id="search_u_970x90"></div>
<ul class="rt-ad-inline">
<li class="ad-2"></li>
<li class="ad-3"></li>
<li class="ad-4"></li>
<li class="ad-5"></li>
</ul>
</div>
</div>
<div class="rt-exclusive-store">
<h2 class="rt-exclusive-store-heading"><span>專館</span></h2>
<div class="rt-exclusive-store-list">
<style type="text/css">
.promo-bar ul,
.promo-bar h2,
.promo-bar h3 {
  margin: 0;
  padding: 0;
  border: 0;
  outline: 0;
  font-size: 13px;
  vertical-align: baseline;
  background: transparent;
}
.promo-bar ul {list-style:none outside none;}

.promo-bar a {text-decoration:none;}

.promo-bar {
  height: 32px;
  width: 1250px;
  margin-top: 0;
}
.promo-bar h2 {
  display: inline-block;
  width: 68px;
  height: 26px;
  background: url("http://www.ruten.com.tw/resource/loader.php?folder=component/theme/&file=bg-main-index.gif&v=20171016193001") -300px -360px no-repeat;
  text-indent: -9999px;
  vertical-align: top;
  margin-top: 6px;
}
.promo-bar ul {
  display: inline-block;
  width: 1200px;
  height: 27px;
  margin-top: 6px;
  overflow: hidden;
}

.promo-bar .cp {
    display: inline-block;
    height: 29px;
    min-width: 72px;
    width: auto;
    background: none;
    text-align: center;
    display: block;
    float: left;
    margin-right: 7px;
    border-width: 1px;
    border-left-width: 13px;
    border-color: #333;
    border-style: solid;
    border-radius: 5px;
    position: relative;
}

.promo-bar .cp h3:before {
    left: -8px;
    top: 14px;
    border: solid transparent;
    content: " ";
    height: 0;
    width: 0;
    position: absolute;
    border-left-color: #fff;
    border-width: 3px;
    margin-top: -3px;
}

.promo-bar .cp a {
    display: inline-block;
    vertical-align: middle;
    margin: 0 8px;
    font-weight: 400;
    padding: 7px 0;
    padding: 9px 0 5px \9;
    color: #7e7e7e;
}

.promo-bar .cp.promoAd {
    background: none;
    display: block;
    border: 0;
    min-width: auto;
}

.promo-bar .cp.promoAd h3:before {
    display: none;
}

.promo-bar .promoAd a {
    margin: 0;
    padding: 0;
}

.promo-bar .cp-female {
    border-color: #ff2888;
}

.promo-bar .cp-female a:hover {
    color: #ff2888;
}

.promo-bar .cp-male {
    border-color: #02a3ff;
}

.promo-bar .cp-male a:hover {
    color: #02a3ff;
}

.promo-bar .cp-3c {
    border-color: #7adf3e;
}

.promo-bar .cp-3c a:hover {
    color: #7adf3e;
}

.promo-bar .cp-life {
    border-color: #ff9900;
}

.promo-bar .cp-life a:hover {
    color: #ff9900;
}

.promo-bar .cp-baby {
    border-color: #bc64f9;
}

.promo-bar .cp-baby a:hover {
    color: #bc64f9;
}

.promo-bar .cp-toy {
    border-color: #ababab;
}

.promo-bar .cp-toy a:hover {
    color: #ababab;
}

.promo-bar .cp-car {
    border-color: #1ab4ab;
}

.promo-bar .cp-car a:hover {
    color: #1ab4ab;
}

.promo-bar .cp-sport {
    border-color: #6a80a7;
}

.promo-bar .cp-sport a:hover {
    color: #6a80a7;
}

.promo-bar .cp-library {
    border-color: #9e6f4a;
}

.promo-bar .cp-library a:hover {
    color: #9e6f4a;
}

.promo-bar .cp-pchomepay {
    border-color: #6EBF00
}

</style>
<ul class="promo-bar">
<li class="cp promoAd"><h3><a href="http://theme.ruten.com.tw/promo/20171011000001" target="_blank"><img src="http://www.ruten.com.tw/resource/loader.php?folder=component/theme/&amp;file=dd94b6a4fd7578c6b975218a3d78a48f.jpg&amp;v=20171016193001"/></a></h3></li>
<li class="cp cp-male"><h3><a href="http://theme.ruten.com.tw?f_tabid=R2" target="_blank">流行時尚</a></h3></li>
<li class="cp cp-3c"><h3><a href="http://theme.ruten.com.tw?f_tabid=R3" target="_blank">3C家電</a></h3></li>
<li class="cp cp-baby"><h3><a href="http://theme.ruten.com.tw?f_tabid=R5" target="_blank">生活居家</a></h3></li>
<li class="cp cp-car"><h3><a href="http://theme.ruten.com.tw?f_tabid=R7" target="_blank">戶外交通</a></h3></li>
<li class="cp cp-female"><h3><a href="http://theme.ruten.com.tw?f_tabid=R1" target="_blank">女神限定</a></h3></li>
<li class="cp cp-toy"><h3><a href="http://theme.ruten.com.tw?f_tabid=R6" target="_blank">玩具電玩</a></h3></li>
<li class="cp cp-sport"><h3><a href="http://theme.ruten.com.tw?f_tabid=R8" target="_blank">媽媽寶寶</a></h3></li>
</ul>
</div>
</div>
<div class="main-section-subcategory">
<div class="rt-subcategory">
<div class="rt-subcategory-search">
<div class="rt-category-menu"><span class="rt-category-menu-title">全部分類</span><span class="rti rti-bars-gray-darker" title="瀏覽全部分類icon"></span>
<div class="rt-category-menu-list-wrap">
<ul class="rt-category-menu-list">
<li class="rt-category-menu-list-item">
<div class="rt-category-menu-list-item-name">
<a class="rt-category-menu-list-item-link" href="http://class.ruten.com.tw/category/main?0011"> 電腦、電子、周邊 </a>
</div>
</li>
<li class="rt-category-menu-list-item">
<div class="rt-category-menu-list-item-name">
<a class="rt-category-menu-list-item-link" href="http://class.ruten.com.tw/category/main?0021"> 手機、通訊 </a>
</div>
</li>
<li class="rt-category-menu-list-item">
<div class="rt-category-menu-list-item-name">
<a class="rt-category-menu-list-item-link" href="http://class.ruten.com.tw/category/main?0008"> 相機、攝影機 </a>
</div>
</li>
<li class="rt-category-menu-list-item">
<div class="rt-category-menu-list-item-name">
<a class="rt-category-menu-list-item-link" href="http://class.ruten.com.tw/category/main?0023"> 家電、影音周邊 </a>
</div>
</li>
<li class="rt-category-menu-list-item">
<div class="rt-category-menu-list-item-name">
<a class="rt-category-menu-list-item-link" href="http://class.ruten.com.tw/category/main?0022"> 電玩、遊戲 </a>
</div>
</li>
<li class="rt-category-menu-list-item">
<div class="rt-category-menu-list-item-name">
<a class="rt-category-menu-list-item-link" href="http://class.ruten.com.tw/category/main?0019"> 交通工具 </a>
</div>
</li>
<li class="rt-category-menu-list-item">
<div class="rt-category-menu-list-item-name">
<a class="rt-category-menu-list-item-link" href="http://class.ruten.com.tw/category/main?0002"> 流行女裝、內睡衣 </a>
</div>
</li>
<li class="rt-category-menu-list-item">
<div class="rt-category-menu-list-item-name">
<a class="rt-category-menu-list-item-link" href="http://class.ruten.com.tw/category/main?0014"> 女性飾品、配件 </a>
</div>
</li>
<li class="rt-category-menu-list-item">
<div class="rt-category-menu-list-item-name">
<a class="rt-category-menu-list-item-link" href="http://class.ruten.com.tw/category/main?0015"> 女包、精品與女鞋 </a>
</div>
</li>
<li class="rt-category-menu-list-item">
<div class="rt-category-menu-list-item-name">
<a class="rt-category-menu-list-item-link" href="http://class.ruten.com.tw/category/main?0012"> 保養、彩妝 </a>
</div>
</li>
<li class="rt-category-menu-list-item">
<div class="rt-category-menu-list-item-name">
<a class="rt-category-menu-list-item-link" href="http://class.ruten.com.tw/category/main?0001"> 嬰幼童、母親 </a>
</div>
</li>
<li class="rt-category-menu-list-item">
<div class="rt-category-menu-list-item-name">
<a class="rt-category-menu-list-item-link" href="http://class.ruten.com.tw/category/main?0018"> 男裝、配件、男鞋 </a>
</div>
</li>
<li class="rt-category-menu-list-item">
<div class="rt-category-menu-list-item-name">
<a class="rt-category-menu-list-item-link" href="http://class.ruten.com.tw/category/main?0003"> 運動、健身 </a>
</div>
</li>
<li class="rt-category-menu-list-item">
<div class="rt-category-menu-list-item-name">
<a class="rt-category-menu-list-item-link" href="http://class.ruten.com.tw/category/main?0020"> 休閒、旅遊 </a>
</div>
</li>
<li class="rt-category-menu-list-item">
<div class="rt-category-menu-list-item-name">
<a class="rt-category-menu-list-item-link" href="http://class.ruten.com.tw/category/main?0009"> 生活、居家 </a>
</div>
</li>
<li class="rt-category-menu-list-item">
<div class="rt-category-menu-list-item-name">
<a class="rt-category-menu-list-item-link" href="http://class.ruten.com.tw/category/main?0024"> 名產、食品 </a>
</div>
</li>
<li class="rt-category-menu-list-item">
<div class="rt-category-menu-list-item-name">
<a class="rt-category-menu-list-item-link" href="http://class.ruten.com.tw/category/main?0007"> 不動產、百業、服務 </a>
</div>
</li>
<li class="rt-category-menu-list-item">
<div class="rt-category-menu-list-item-name">
<a class="rt-category-menu-list-item-link" href="http://class.ruten.com.tw/category/main?0005"> 玩具、公仔 </a>
</div>
</li>
<li class="rt-category-menu-list-item">
<div class="rt-category-menu-list-item-name">
<a class="rt-category-menu-list-item-link" href="http://class.ruten.com.tw/category/main?0004"> 明星、偶像 </a>
</div>
</li>
<li class="rt-category-menu-list-item">
<div class="rt-category-menu-list-item-name">
<a class="rt-category-menu-list-item-link" href="http://class.ruten.com.tw/category/main?0006"> 書籍、文創、科學 </a>
</div>
</li>
<li class="rt-category-menu-list-item">
<div class="rt-category-menu-list-item-name">
<a class="rt-category-menu-list-item-link" href="http://class.ruten.com.tw/category/main?0010"> 音樂、電影 </a>
</div>
</li>
<li class="rt-category-menu-list-item">
<div class="rt-category-menu-list-item-name">
<a class="rt-category-menu-list-item-link" href="http://class.ruten.com.tw/category/main?0013"> 古董、收藏 </a>
</div>
</li>
<li class="rt-category-menu-list-item">
<div class="rt-category-menu-list-item-name">
<a class="rt-category-menu-list-item-link" href="http://class.ruten.com.tw/category/main?0017"> 鐘錶、眼鏡 </a>
</div>
</li>
<li class="rt-category-menu-list-item">
<div class="rt-category-menu-list-item-name">
<a class="rt-category-menu-list-item-link" href="http://class.ruten.com.tw/category/main?0025"> 成人專區 </a>
</div>
</li>
</ul>
</div>
</div>
<h2 class="rt-subcategory-title">
<ul class="rt-breadcrumb rt-subcategory-breadcrumb" itemscope="" itemtype="http://schema.org/BreadcrumbList">
</ul>
</h2>
<div class="rt-site-search-wrap">
<form action="http://find.ruten.com.tw/search/s000.php" class="rt-site-search" id="result_search_form">
<input name="enc" type="hidden" value="u">
<input name="searchfrom" type="hidden" value="searchf">
<div class="search-bar-keyword-wrap" v-on-clickaway="closeSuggestion">
<input @focus="openSuggestion" @keydown.down.prevent="selectNext" @keydown.enter="selectionSubmitting" @keydown.up.prevent="selectPrevious" autocomplete="off" class="rt-filter-form-input" id="search_input" name="k" placeholder="在本分類搜尋" type="text" v-default-value="'Lowe Alpine Strike'" v-model="keywordInput">
<suggestion-keyword-template :current-selected-item-index="currentSelectedItemIndex" :is-open="isOpen" :keyword-input="keywordInput" :suggestion-keyword-array="suggestionKeywordArray"></suggestion-keyword-template>
</input></div>
<select class="rt-filter-form-select" name="t">
<option value="0">商品標題</option>
<option value="1">賣家帳號</option>
<option value="2">商品編號</option>
</select>
<button class="rt-button rt-button-gradient rt-button-small rt-site-search-submit" type="submit">
<span class="rt-site-search-button-name">搜尋</span><span class="rti rti-search-warning"></span>
</button>
</input></input></form>
</div>
</div>
</div>
<div class="search-result-summary">
            符合<strong class="rt-text-isolated rt-text-important">Lowe Alpine Strike</strong>共搜尋到<strong class="rt-text-isolated">59</strong>件商品
          </div>
</div>
<div class="clearfix">
<div class="main-section clearfix">
<div class="main-section-left">
<form action="http://find.ruten.com.tw/search/s000.php" class="rt-form" id="side_filter" method="GET" name="fm_search_sidebar">
<input name="enc" type="hidden" value="u">
<input name="k" type="hidden" value="Lowe Alpine Strike">
<input name="c" type="hidden" value="0">
<input name="ctab" type="hidden" value="1">
<input name="searchfrom" type="hidden" value="searchbars">
<input name="f" type="hidden" value="">
<input name="o" type="hidden" value="9">
<input name="m" type="hidden" value="2">
<input name="od" type="hidden" value="">
<div class="rt-filter">
<div class="rt-filter-title">縮小範圍顯示</div>
<div class="rt-filter-body">
<div class="rt-filter-form-group">
<input class="rt-filter-form-input" id="fx12" type="checkbox" value="8192">
<label class="rt-filter-form-label" for="fx12">
<span class="icon icon-pchomepay-credit"></span><span class="vmiddle">支付連 信用卡</span>
</label>
</input></div>
<div class="rt-filter-form-group">
<input class="rt-filter-form-input" id="fx14" type="checkbox" value="32768">
<label class="rt-filter-form-label" for="fx14">
<span class="icon icon-pchomepay-credit"></span><span class="vmiddle">支付連 分期0利率</span>
</label>
</input></div>
<div class="rt-filter-form-group">
<input class="rt-filter-form-input" id="fx11" type="checkbox" value="4096">
<label class="rt-filter-form-label" for="fx11">
<span class="icon icon-paylink"></span><span class="vmiddle">支付連 現金</span>
</label>
</input></div>
<div class="rt-filter-form-group">
<input class="rt-filter-form-input" id="fx16" type="checkbox" value="131072">
<label class="rt-filter-form-label" for="fx16">
<span class="icon icon-interpay"></span><span class="vmiddle">國際連儲值餘額</span>
</label>
</input></div>
<div class="rt-filter-form-group">
<input class="rt-filter-form-input" id="fx13" type="checkbox" value="16384">
<label class="rt-filter-form-label" for="fx13" title="OK、萊爾富、全家取貨付款">
<span class="icon icon-cvs-cod"></span><span class="vmiddle">取貨付款</span>
</label>
</input></div>
<div class="rt-filter-form-group">
<input class="rt-filter-form-input" id="fx9" type="checkbox" value="1024">
<label class="rt-filter-form-label" for="fx9">
<span class="icon icon-seven-cod"></span><span class="vmiddle">7-11取貨付款</span>
</label>
</input></div>
<div class="rt-filter-form-group">
<input class="rt-filter-form-input" id="fx15" type="checkbox" value="65536">
<label class="rt-filter-form-label" for="fx15" title="宅急便 - 黑貓PAY貨到付款">
<span class="icon icon-tcat-cod-s"></span><span class="vmiddle">貨到付款</span>
</label>
</input></div>
<div class="rt-filter-form-group">
<input class="rt-filter-form-input" id="fx10" type="checkbox" value="2048">
<label class="rt-filter-form-label" for="fx10" title="郵局快捷貨到付款">
<span class="icon icon-r-postcod"></span><span class="vmiddle">快捷貨到付款</span>
</label>
</input></div>
<div class="rt-filter-form-group">
<input class="rt-filter-form-input" id="ispp" name="ispp" type="checkbox" value="1">
<label class="rt-filter-form-label" for="ispp">
<span alt="支付連認證賣家標誌" class="icon icon-certified-pp-seller"></span><span class="vmiddle icon-recently-added">支付連認證賣家</span>
</label>
</input></div>
<div class="rt-filter-form-group">
<input class="rt-filter-form-input" id="onsale" name="onsale" type="checkbox" value="1">
<label class="rt-filter-form-label icon-recently-added" for="onsale">可使用折扣代碼商品</label>
</input></div>
<div class="rt-filter-form-group">
<input class="rt-filter-form-input" id="isqty" name="isqty" type="checkbox" value="1">
<label class="rt-filter-form-label icon-recently-added" for="isqty">有庫存</label>
</input></div>
<div class="rt-filter-form-inline-group">
<div class="rt-filter-form-group">
<input class="rt-filter-form-input" id="fx7" onclick="document.getElementById('fx8').checked = false;" type="checkbox" value="4">
<label class="rt-filter-form-label" for="fx7">全新品</label>
</input></div>
<div class="rt-filter-form-group">
<input class="rt-filter-form-input" id="fx8" onclick="document.getElementById('fx7').checked = false;" type="checkbox" value="512">
<label class="rt-filter-form-label" for="fx8">二手品</label>
</input></div>
</div>
<div class="rt-filter-form-inline-group">
<div class="rt-filter-form-group">
<input class="rt-filter-form-input" id="fx3" type="checkbox" value="256">
<label class="rt-filter-form-label" for="fx3">一元起標</label>
</input></div>
<div class="rt-filter-form-group">
<input class="rt-filter-form-input" id="fx4" type="checkbox" value="16">
<label class="rt-filter-form-label" for="fx4">無底價</label>
</input></div>
</div>
<div class="rt-filter-form-group rt-filter-form-mb">
<input class="rt-filter-form-input" id="fx6" type="checkbox" value="32">
<label class="rt-filter-form-label" for="fx6">已達底價</label>
</input></div>
<div class="rt-filter-form-group rt-filter-form-mb">
<label class="rt-filter-form-label" for="fy">地區：</label>
<select class="rt-filter-form-select" id="fy" name="fy">
<option value="0">不限地區</option>
<option value="1">台北市</option>
<option value="2">基隆市</option>
<option value="3">新北市</option>
<option value="4">宜蘭縣</option>
<option value="5">新竹市</option>
<option value="6">新竹縣</option>
<option value="7">桃園市</option>
<option value="8">苗栗縣</option>
<option value="9">台中市</option>
<option value="10">彰化縣</option>
<option value="11">南投縣</option>
<option value="12">嘉義市</option>
<option value="13">嘉義縣</option>
<option value="14">雲林縣</option>
<option value="15">台南市</option>
<option value="16">高雄市</option>
<option value="17">屏東縣</option>
<option value="18">台東縣</option>
<option value="19">花蓮縣</option>
<option value="20">澎湖縣</option>
<option value="21">金門縣</option>
<option value="22">連江縣</option>
<option value="23">其他地區</option>
</select>
</div>
<div class="rt-filter-form-group rt-filter-form-mb">
<label class="rt-filter-form-label" for="h">運費：</label>
<select class="rt-filter-form-select" id="h" name="h">
<option value="0">不限運費</option>
<option value="1">免運費</option>
<option value="2">30元以下</option>
<option value="3">50元以下</option>
<option value="4">80元以下</option>
<option value="5">150元以下</option>
<option value="6">500元以下</option>
</select>
</div>
<div class="rt-filter-form-group rt-filter-form-mb">
<label class="rt-filter-form-label" for="p1">價格：</label>
<input class="rt-filter-form-input rt-filter-form-input-tiny" id="p1" maxlength="10" name="p1" type="text">
                    至
                    <input class="rt-filter-form-input rt-filter-form-input-tiny" id="p2" maxlength="10" name="p2" type="text">
                    元
                  </input></input></div>
<div class="rt-filter-divider"></div>
<div class="text-center">
<input class="rt-button rt-button-default rt-button-gradient rt-button-small" id="side_filter_submit" type="button" value="確定"/>
</div>
</div>
</div>
</input></input></input></input></input></input></input></input></input></form>
<script type="text/javascript">
              /**
               * Array forEach polyfill
               * copied from
               * https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/forEach
               */
              Array.prototype.forEach||(Array.prototype.forEach=function(r,t){var o,n;if(null==this)throw new TypeError(" this is null or not defined");var e=Object(this),i=e.length>>>0;if("function"!=typeof r)throw new TypeError(r+" is not a function");for(arguments.length>1&&(o=t),n=0;i>n;){var a;n in e&&(a=e[n],r.call(o,a,n,e)),n++}});

              var filter_config = RT && RT.context && RT.context.filter_config || {}, key, dom, filter_form;
              filter_form = document.getElementById('side_filter');
              dom = filter_form.querySelectorAll('.rt-filter-form-input, .rt-filter-form-select');
              dom = dom? Array.apply(null, dom): [];
              dom.forEach(function(input) {
                var key = input.id;
                if(input.type === 'checkbox') {
                  input.checked = filter_config[key] || false;
                } else {
                  input.value = typeof filter_config[key] !== 'undefined'? filter_config[key]:(input.tagName === 'SELECT'? '0':'');
                }
              });
              </script>
<div class="rt-ad rt-ad-side">
<div class="rt-ad-title">
                推薦商品
              </div>
<div class="rt-ad-body">
<ul>
<li class="rt-ad-item rt-ad-item-recommended ad-540"></li>
<li class="rt-ad-item rt-ad-item-recommended ad-543"></li>
<li class="rt-ad-item rt-ad-item-recommended ad-544"></li>
<li class="rt-ad-item rt-ad-item-recommended ad-566"></li>
</ul>
</div>
</div>
<div class="rt-ad">
<div class="rt-ad-body">
<div class="rt-ad-item ad-28" data-width="140"></div>
<div data-width="120" id="search_l_120x600"></div>
</div>
</div>
</div>
<div class="main-section-body">
<div class="ad-focus-goods hide" id="focus_goods">
<div class="item-group-title-category-focus">
<span class="item-group-title-main">注目商品</span>
<span class="item-group-title-link">
<a href="http://pub.ruten.com.tw/adb/3.html">我要曝光</a>｜<a href="https://point.ruten.com.tw/ADI/show_imp.php?c=0&amp;k=Lowe Alpine Strike">更多商品</a>
</span>
</div>
<ul class="ad-focus-goods-content" id="focus_goods_content">
</ul>
</div>
<div class="results-listing" data-module="results-listing">
<div class="results-show-mode-control">
<div class="results-show-mode-control-item primary-row">
<ul class="results-show-mode-tab">
<li class="results-show-mode-tab-item ">
<a class="results-show-mode-tab-link " href="?enc=u&amp;searchfrom=indexbar&amp;k=Lowe+Alpine+Strike&amp;t=0&amp;ctab=0" rel="nofollow" title="顯示全部商品">全部</a>
</li>
<li class="results-show-mode-tab-item tab-item-active">
<a class="results-show-mode-tab-link view-mode-link-active" href="?enc=u&amp;searchfrom=indexbar&amp;k=Lowe+Alpine+Strike&amp;t=0&amp;ctab=1" rel="nofollow" title="顯示直購商品">直購</a>
</li>
<li class="results-show-mode-tab-item ">
<a class="results-show-mode-tab-link " href="?enc=u&amp;searchfrom=indexbar&amp;k=Lowe+Alpine+Strike&amp;t=0&amp;ctab=2" rel="nofollow" title="顯示競標商品">競標</a>
</li>
</ul>
<div class="results-head-pagination">
<span class="results-head-pagination-item pagination-info">共<strong class="rt-text-isolated">59</strong>項商品</span>
<span class="results-head-pagination-item pagination-info"><strong class="rt-text-keyword">1</strong> / 2</span>
<div class="rt-pagination-lite results-head-pagination-item" id="head_pagination"></div>
</div>
</div>
<div class="results-show-mode-control-item second-row">
<ul class="results-display-mode">
<li class="display-mode-item display-mode-item-current rt-tooltip rt-tooltip-top" data-tooltip="圖片瀏覽模式"><a href="?enc=u&amp;searchfrom=indexbar&amp;k=Lowe+Alpine+Strike&amp;t=0&amp;p=1&amp;m=2" rel="nofollow">
<span alt="圖片瀏覽模式icon" class="display-mode-icon-box rti rti-th-large-current"></span>
</a>
</li>
<li class="display-mode-item rt-tooltip rt-tooltip-top rt-ml" data-tooltip="圖文瀏覽模式">
<a href="?enc=u&amp;searchfrom=indexbar&amp;k=Lowe+Alpine+Strike&amp;t=0&amp;p=1&amp;m=1" rel="nofollow">
<span alt="圖文瀏覽模式icon" class="display-mode-icon-box rti rti-th-list-disabled"></span>
</a>
</li>
</ul>
<div class="sort-mode">
<span class="sort-mode-title">排序：</span>
<ul class="sort-mode-list">
<li class="sort-mode-content">
<a class="rt-text-keyword icon sort-bold-down" href="?enc=u&amp;searchfrom=indexbar&amp;k=Lowe+Alpine+Strike&amp;t=0&amp;p=1&amp;o=9" rel="nofollow" title="BuyRank">BuyRank</a><a class="rt-text-keyword sort-mode-buyrank" href="#" rel="nofollow"><span class="rti rti-2x rti-question-circle-icon" title="BuyRank說明"></span>
<div class="sort-mode-buyrank-content">
<div><strong>BuyRank 說明：</strong></div>
<p>BuyRank 分數，以該商品<b>熱銷度</b>與<b>新鮮度</b>等指標進行計算，若 BuyRank 積分相同者，則以商品刊登時間排序，較新的商品為先，讓買家更容易尋找到最夯、最新的商品。</p>
</div>
</a>
</li>
<li class="sort-mode-content"><a class="icon " href="?enc=u&amp;searchfrom=indexbar&amp;k=Lowe+Alpine+Strike&amp;t=0&amp;p=1&amp;o=4" rel="nofollow" title="直購價">直購價</a></li>
<li class="sort-mode-content"><a class="icon " href="?enc=u&amp;searchfrom=indexbar&amp;k=Lowe+Alpine+Strike&amp;t=0&amp;p=1&amp;o=15" rel="nofollow" title="已賣數量">已賣數量</a></li>
<li class="sort-mode-content"><a class="icon " href="?enc=u&amp;searchfrom=indexbar&amp;k=Lowe+Alpine+Strike&amp;t=0&amp;p=1&amp;o=10" rel="nofollow" title="最新刊登">最新刊登</a></li>
<li class="sort-mode-content"><a class="icon " href="?enc=u&amp;searchfrom=indexbar&amp;k=Lowe+Alpine+Strike&amp;t=0&amp;p=1&amp;o=13" rel="nofollow" title="賣家評價">賣家評價</a></li>
</ul>
</div>
</div>
</div>
<div class="rt-goods-list-wrap">
<ul class="rt-goods-list rt-goods-list-view-mode-pic">
<li class="item-group-title-default rt-goods-list-header">
<span class="item-group-title-main">全部商品</span>
</li>
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
<div class="rt-goods-list-item-image-wrap">
<a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21716971337096" target="_blank" title="前往Lowe Alpine Strike 24 運動背包/都會日用後背包 FDP55 黑商品頁面">
<img alt="Lowe Alpine Strike 24 運動背包/都會日用後背包 FDP55 黑" class="rt-goods-list-item-image" itemprop="image" src="http://b.rimg.com.tw/s2/6/29/88/21716971337096_111_m.jpg" title="Lowe Alpine Strike 24 運動背包/都會日用後背包 FDP55 黑">
</img></a>
</div>
<h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21716971337096" target="_blank" title="前往Lowe Alpine Strike 24 運動背包/都會日用後背包 FDP55 黑商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">Lowe Alpine Strike 24 運動背包/都會日用後背包 FDP55 黑</span>
</a>
</h3>
<p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<strong class="item-price-symbol rt-text-larger rt-text-price">2,363</strong>
</p>
<div class="rt-goods-list-item-info">
<div class="list-item-info-column">
<span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
</span>
</div>
<span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21716971337096" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
<span class="item-favorite-amount js-item-favorite-amount" title="已有0人將此商品加入追蹤清單">0</span>
</span>
</div>
</li>
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
<div class="rt-goods-list-item-image-wrap">
<a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21716971330538" target="_blank" title="前往Lowe Alpine Strike 24 運動背包/都會日用後背包 FDP55 浴火浩克商品頁面">
<img alt="Lowe Alpine Strike 24 運動背包/都會日用後背包 FDP55 浴火浩克" class="rt-goods-list-item-image" itemprop="image" src="http://d.rimg.com.tw/s2/6/0f/ea/21716971330538_734_m.jpg" title="Lowe Alpine Strike 24 運動背包/都會日用後背包 FDP55 浴火浩克">
</img></a>
</div>
<h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21716971330538" target="_blank" title="前往Lowe Alpine Strike 24 運動背包/都會日用後背包 FDP55 浴火浩克商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">Lowe Alpine Strike 24 運動背包/都會日用後背包 FDP55 浴火浩克</span>
</a>
</h3>
<p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<strong class="item-price-symbol rt-text-larger rt-text-price">2,363</strong>
</p>
<div class="rt-goods-list-item-info">
<div class="list-item-info-column">
<span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
</span>
</div>
<span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21716971330538" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
<span class="item-favorite-amount js-item-favorite-amount" title="已有1人將此商品加入追蹤清單">1</span>
</span>
</div>
</li>
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
<div class="rt-goods-list-item-image-wrap">
<a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21716927114023" target="_blank" title="前往【山野倉庫】歐洲-Lowe Alpine Strike 12登山多功能背包商品頁面">
<img alt="【山野倉庫】歐洲-Lowe Alpine Strike 12登山多功能背包" class="rt-goods-list-item-image" itemprop="image" src="http://f.rimg.com.tw/s1/3/5f/27/21716927114023_388_m.jpg" title="【山野倉庫】歐洲-Lowe Alpine Strike 12登山多功能背包">
</img></a>
<div class="rt-goods-list-item-similar">
<a class="rt-goods-list-item-similar-link" href="http://find.ruten.com.tw/search/alike.php?tag=fb49a0f7921eb40273b96108e726d394e3af1456&amp;g=21716927114023&amp;c=0&amp;k=Lowe Alpine Strike&amp;p=1" rel="nofollow" title="顯示多筆相似商品">多筆相似商品</a>
</div>
</div>
<h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21716927114023" target="_blank" title="前往【山野倉庫】歐洲-Lowe Alpine Strike 12登山多功能背包商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">【山野倉庫】歐洲-Lowe Alpine Strike 12登山多功能背包</span>
</a>
</h3>
<p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<strong class="item-price-symbol rt-text-larger rt-text-price">2,020</strong>
</p>
<div class="rt-goods-list-item-info">
<div class="list-item-info-column">
<span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
</span>
</div>
<span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21716927114023" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
<span class="item-favorite-amount js-item-favorite-amount" title="已有0人將此商品加入追蹤清單">0</span>
</span>
</div>
</li>
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
<div class="rt-goods-list-item-image-wrap">
<a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21715812650378" target="_blank" title="前往Lowe Alpine FDP53-M節慶紅 Strike 12L休閒登山背包/健走/登山包/多功能後背包商品頁面">
<img alt="Lowe Alpine FDP53-M節慶紅 Strike 12L休閒登山背包/健走/登山包/多功能後背包" class="rt-goods-list-item-image" itemprop="image" src="http://b.rimg.com.tw/s2/6/01/8a/21715812650378_157_m.jpg" title="Lowe Alpine FDP53-M節慶紅 Strike 12L休閒登山背包/健走/登山包/多功能後背包">
</img></a>
</div>
<h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21715812650378" target="_blank" title="前往Lowe Alpine FDP53-M節慶紅 Strike 12L休閒登山背包/健走/登山包/多功能後背包商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">Lowe Alpine FDP53-M節慶紅 Strike 12L休閒登山背包/健走/登山包/多功能後背包</span>
</a>
</h3>
<p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<strong class="item-price-symbol rt-text-larger rt-text-price">2,020</strong>
</p>
<div class="rt-goods-list-item-info">
<div class="list-item-info-column">
<span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
</span>
</div>
<span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21715812650378" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
<span class="item-favorite-amount js-item-favorite-amount" title="已有0人將此商品加入追蹤清單">0</span>
</span>
</div>
</li>
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
<div class="rt-goods-list-item-image-wrap">
<a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21715812650323" target="_blank" title="前往Lowe Alpine FDP53-M4藍燈閃電 Strike 12L休閒登山背包/健走/登山包/多功能後背包商品頁面">
<img alt="Lowe Alpine FDP53-M4藍燈閃電 Strike 12L休閒登山背包/健走/登山包/多功能後背包" class="rt-goods-list-item-image" itemprop="image" src="http://d.rimg.com.tw/s1/6/01/53/21715812650323_810_m.jpg" title="Lowe Alpine FDP53-M4藍燈閃電 Strike 12L休閒登山背包/健走/登山包/多功能後背包">
</img></a>
</div>
<h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21715812650323" target="_blank" title="前往Lowe Alpine FDP53-M4藍燈閃電 Strike 12L休閒登山背包/健走/登山包/多功能後背包商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">Lowe Alpine FDP53-M4藍燈閃電 Strike 12L休閒登山背包/健走/登山包/多功能後背包</span>
</a>
</h3>
<p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<strong class="item-price-symbol rt-text-larger rt-text-price">2,020</strong>
</p>
<div class="rt-goods-list-item-info">
<div class="list-item-info-column">
<span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
</span>
</div>
<span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21715812650323" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
<span class="item-favorite-amount js-item-favorite-amount" title="已有0人將此商品加入追蹤清單">0</span>
</span>
</div>
</li>
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
<div class="rt-goods-list-item-image-wrap">
<a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21715812499896" target="_blank" title="前往Lowe Alpine FDP53-P波斯綠 Strike 12L休閒登山背包/健走/登山包/多功能後背包商品頁面">
<img alt="Lowe Alpine FDP53-P波斯綠 Strike 12L休閒登山背包/健走/登山包/多功能後背包" class="rt-goods-list-item-image" itemprop="image" src="http://f.rimg.com.tw/s2/3/b5/b8/21715812499896_765_m.jpg" title="Lowe Alpine FDP53-P波斯綠 Strike 12L休閒登山背包/健走/登山包/多功能後背包">
</img></a>
</div>
<h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21715812499896" target="_blank" title="前往Lowe Alpine FDP53-P波斯綠 Strike 12L休閒登山背包/健走/登山包/多功能後背包商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">Lowe Alpine FDP53-P波斯綠 Strike 12L休閒登山背包/健走/登山包/多功能後背包</span>
</a>
</h3>
<p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<strong class="item-price-symbol rt-text-larger rt-text-price">2,020</strong>
</p>
<div class="rt-goods-list-item-info">
<div class="list-item-info-column">
<span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
</span>
</div>
<span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21715812499896" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
<span class="item-favorite-amount js-item-favorite-amount" title="已有0人將此商品加入追蹤清單">0</span>
</span>
</div>
</li>
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
<div class="rt-goods-list-item-image-wrap">
<a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21715812397747" target="_blank" title="前往Lowe Alpine FDP54-M4藍燈閃電Strike 18L休閒登山背包/健走/登山包/多功能後背包商品頁面">
<img alt="Lowe Alpine FDP54-M4藍燈閃電Strike 18L休閒登山背包/健走/登山包/多功能後背包" class="rt-goods-list-item-image" itemprop="image" src="http://b.rimg.com.tw/s1/2/26/b3/21715812397747_881_m.jpg" title="Lowe Alpine FDP54-M4藍燈閃電Strike 18L休閒登山背包/健走/登山包/多功能後背包">
</img></a>
</div>
<h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21715812397747" target="_blank" title="前往Lowe Alpine FDP54-M4藍燈閃電Strike 18L休閒登山背包/健走/登山包/多功能後背包商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">Lowe Alpine FDP54-M4藍燈閃電Strike 18L休閒登山背包/健走/登山包/多功能後背包</span>
</a>
</h3>
<p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<strong class="item-price-symbol rt-text-larger rt-text-price">2,190</strong>
</p>
<div class="rt-goods-list-item-info">
<div class="list-item-info-column">
<span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
</span>
</div>
<span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21715812397747" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
<span class="item-favorite-amount js-item-favorite-amount" title="已有0人將此商品加入追蹤清單">0</span>
</span>
</div>
</li>
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
<div class="rt-goods-list-item-image-wrap">
<a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21715812376328" target="_blank" title="前往Lowe Alpine FDP54-M3浴火浩克Strike 18L休閒登山背包/健走/登山包/多功能後背包商品頁面">
<img alt="Lowe Alpine FDP54-M3浴火浩克Strike 18L休閒登山背包/健走/登山包/多功能後背包" class="rt-goods-list-item-image" itemprop="image" src="http://d.rimg.com.tw/s2/1/d3/08/21715812376328_773_m.jpg" title="Lowe Alpine FDP54-M3浴火浩克Strike 18L休閒登山背包/健走/登山包/多功能後背包">
</img></a>
</div>
<h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21715812376328" target="_blank" title="前往Lowe Alpine FDP54-M3浴火浩克Strike 18L休閒登山背包/健走/登山包/多功能後背包商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">Lowe Alpine FDP54-M3浴火浩克Strike 18L休閒登山背包/健走/登山包/多功能後背包</span>
</a>
</h3>
<p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<strong class="item-price-symbol rt-text-larger rt-text-price">2,190</strong>
</p>
<div class="rt-goods-list-item-info">
<div class="list-item-info-column">
<span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
</span>
</div>
<span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21715812376328" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
<span class="item-favorite-amount js-item-favorite-amount" title="已有0人將此商品加入追蹤清單">0</span>
</span>
</div>
</li>
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
<div class="rt-goods-list-item-image-wrap">
<a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21715811754114" target="_blank" title="前往Lowe Alpine FDP54-S雲杉綠Strike 18L休閒登山背包 Lowe Alpine FD/健走/登山包商品頁面">
<img alt="Lowe Alpine FDP54-S雲杉綠Strike 18L休閒登山背包 Lowe Alpine FD/健走/登山包" class="rt-goods-list-item-image" itemprop="image" src="http://f.rimg.com.tw/s2/8/54/82/21715811754114_342_m.jpg" title="Lowe Alpine FDP54-S雲杉綠Strike 18L休閒登山背包 Lowe Alpine FD/健走/登山包">
</img></a>
</div>
<h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21715811754114" target="_blank" title="前往Lowe Alpine FDP54-S雲杉綠Strike 18L休閒登山背包 Lowe Alpine FD/健走/登山包商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">Lowe Alpine FDP54-S雲杉綠Strike 18L休閒登山背包 Lowe Alpine FD/健走/登山包</span>
</a>
</h3>
<p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<strong class="item-price-symbol rt-text-larger rt-text-price">2,190</strong>
</p>
<div class="rt-goods-list-item-info">
<div class="list-item-info-column">
<span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
</span>
</div>
<span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21715811754114" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
<span class="item-favorite-amount js-item-favorite-amount" title="已有0人將此商品加入追蹤清單">0</span>
</span>
</div>
</li>
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
<div class="rt-goods-list-item-image-wrap">
<a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21715811739443" target="_blank" title="前往Lowe Alpine FDP54-B黑Strike 18L休閒登山背包/健走/登山包/多功能後背包商品頁面">
<img alt="Lowe Alpine FDP54-B黑Strike 18L休閒登山背包/健走/登山包/多功能後背包" class="rt-goods-list-item-image" itemprop="image" src="http://b.rimg.com.tw/s1/8/1b/33/21715811739443_797_m.jpg" title="Lowe Alpine FDP54-B黑Strike 18L休閒登山背包/健走/登山包/多功能後背包">
</img></a>
</div>
<h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21715811739443" target="_blank" title="前往Lowe Alpine FDP54-B黑Strike 18L休閒登山背包/健走/登山包/多功能後背包商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">Lowe Alpine FDP54-B黑Strike 18L休閒登山背包/健走/登山包/多功能後背包</span>
</a>
</h3>
<p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<strong class="item-price-symbol rt-text-larger rt-text-price">2,190</strong>
</p>
<div class="rt-goods-list-item-info">
<div class="list-item-info-column">
<span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
</span>
</div>
<span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21715811739443" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
<span class="item-favorite-amount js-item-favorite-amount" title="已有0人將此商品加入追蹤清單">0</span>
</span>
</div>
</li>
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
<div class="rt-goods-list-item-image-wrap">
<a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21715811707875" target="_blank" title="前往Lowe Alpine FDP55-M3浴火浩克 Strike 24L休閒登山背包/健走/登山包/多功能後背包商品頁面">
<img alt="Lowe Alpine FDP55-M3浴火浩克 Strike 24L休閒登山背包/健走/登山包/多功能後背包" class="rt-goods-list-item-image" itemprop="image" src="http://d.rimg.com.tw/s1/7/9f/e3/21715811707875_367_m.jpg" title="Lowe Alpine FDP55-M3浴火浩克 Strike 24L休閒登山背包/健走/登山包/多功能後背包">
</img></a>
</div>
<h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21715811707875" target="_blank" title="前往Lowe Alpine FDP55-M3浴火浩克 Strike 24L休閒登山背包/健走/登山包/多功能後背包商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">Lowe Alpine FDP55-M3浴火浩克 Strike 24L休閒登山背包/健走/登山包/多功能後背包</span>
</a>
</h3>
<p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<strong class="item-price-symbol rt-text-larger rt-text-price">2,360</strong>
</p>
<div class="rt-goods-list-item-info">
<div class="list-item-info-column">
<span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
</span>
</div>
<span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21715811707875" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
<span class="item-favorite-amount js-item-favorite-amount" title="已有0人將此商品加入追蹤清單">0</span>
</span>
</div>
</li>
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
<div class="rt-goods-list-item-image-wrap">
<a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21643141490454" target="_blank" title="前往Lowe Alpine FDP55-24S雲杉綠 Strike 24L休閒登山背包 健行背包/多功能背包/運動旅行背包商品頁面">
<img alt="Lowe Alpine FDP55-24S雲杉綠 Strike 24L休閒登山背包 健行背包/多功能背包/運動旅行背包" class="rt-goods-list-item-image" itemprop="image" src="http://f.rimg.com.tw/s2/f/57/16/21643141490454_419_m.jpg" title="Lowe Alpine FDP55-24S雲杉綠 Strike 24L休閒登山背包 健行背包/多功能背包/運動旅行背包">
</img></a>
</div>
<h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21643141490454" target="_blank" title="前往Lowe Alpine FDP55-24S雲杉綠 Strike 24L休閒登山背包 健行背包/多功能背包/運動旅行背包商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">Lowe Alpine FDP55-24S雲杉綠 Strike 24L休閒登山背包 健行背包/多功能背包/運動旅行背包</span>
</a>
</h3>
<p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<strong class="item-price-symbol rt-text-larger rt-text-price">2,360</strong>
</p>
<div class="rt-goods-list-item-info">
<div class="list-item-info-column">
<span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
</span>
</div>
<span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21643141490454" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
<span class="item-favorite-amount js-item-favorite-amount" title="已有0人將此商品加入追蹤清單">0</span>
</span>
</div>
</li>
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
<div class="rt-goods-list-item-image-wrap">
<a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21642042865684" target="_blank" title="前往義大利 Lowe Alpine Strike 24L 氧化鉛紅 後背包 登山 | 健行 | 旅遊 | 書包商品頁面">
<img alt="義大利 Lowe Alpine Strike 24L 氧化鉛紅 後背包 登山 | 健行 | 旅遊 | 書包" class="rt-goods-list-item-image" itemprop="image" src="http://b.rimg.com.tw/s2/3/a8/14/21642042865684_340_m.jpg" title="義大利 Lowe Alpine Strike 24L 氧化鉛紅 後背包 登山 | 健行 | 旅遊 | 書包">
</img></a>
</div>
<h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21642042865684" target="_blank" title="前往義大利 Lowe Alpine Strike 24L 氧化鉛紅 後背包 登山 | 健行 | 旅遊 | 書包商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">義大利 Lowe Alpine Strike 24L 氧化鉛紅 後背包 登山 | 健行 | 旅遊 | 書包</span>
</a>
</h3>
<p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<strong class="item-price-symbol rt-text-larger rt-text-price">2,360</strong>
</p>
<div class="rt-goods-list-item-info">
<div class="list-item-info-column">
<span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
</span>
</div>
<span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21642042865684" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
<span class="item-favorite-amount js-item-favorite-amount" title="已有0人將此商品加入追蹤清單">0</span>
</span>
</div>
</li>
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
<div class="rt-goods-list-item-image-wrap">
<a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21642042103713" target="_blank" title="前往義大利 Lowe Alpine Strike 18L 氧化鉛紅 後背包 登山 | 健行 | 旅遊 | 書包商品頁面">
<img alt="義大利 Lowe Alpine Strike 18L 氧化鉛紅 後背包 登山 | 健行 | 旅遊 | 書包" class="rt-goods-list-item-image" itemprop="image" src="http://d.rimg.com.tw/s1/8/07/a1/21642042103713_857_m.jpg" title="義大利 Lowe Alpine Strike 18L 氧化鉛紅 後背包 登山 | 健行 | 旅遊 | 書包">
</img></a>
</div>
<h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21642042103713" target="_blank" title="前往義大利 Lowe Alpine Strike 18L 氧化鉛紅 後背包 登山 | 健行 | 旅遊 | 書包商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">義大利 Lowe Alpine Strike 18L 氧化鉛紅 後背包 登山 | 健行 | 旅遊 | 書包</span>
</a>
</h3>
<p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<strong class="item-price-symbol rt-text-larger rt-text-price">2,190</strong>
</p>
<div class="rt-goods-list-item-info">
<div class="list-item-info-column">
<span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
</span>
</div>
<span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21642042103713" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
<span class="item-favorite-amount js-item-favorite-amount" title="已有0人將此商品加入追蹤清單">0</span>
</span>
</div>
</li>
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
<div class="rt-goods-list-item-image-wrap">
<a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21642042004735" target="_blank" title="前往義大利 Lowe Alpine Strike 18L 雲杉綠 後背包 登山 | 健行 | 旅遊 | 書包商品頁面">
<img alt="義大利 Lowe Alpine Strike 18L 雲杉綠 後背包 登山 | 健行 | 旅遊 | 書包" class="rt-goods-list-item-image" itemprop="image" src="http://f.rimg.com.tw/s1/6/84/ff/21642042004735_682_m.jpg" title="義大利 Lowe Alpine Strike 18L 雲杉綠 後背包 登山 | 健行 | 旅遊 | 書包">
</img></a>
</div>
<h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21642042004735" target="_blank" title="前往義大利 Lowe Alpine Strike 18L 雲杉綠 後背包 登山 | 健行 | 旅遊 | 書包商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">義大利 Lowe Alpine Strike 18L 雲杉綠 後背包 登山 | 健行 | 旅遊 | 書包</span>
</a>
</h3>
<p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<strong class="item-price-symbol rt-text-larger rt-text-price">2,190</strong>
</p>
<div class="rt-goods-list-item-info">
<div class="list-item-info-column">
<span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
</span>
</div>
<span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21642042004735" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
<span class="item-favorite-amount js-item-favorite-amount" title="已有0人將此商品加入追蹤清單">0</span>
</span>
</div>
</li>
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
<div class="rt-goods-list-item-image-wrap">
<a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21642041621964" target="_blank" title="前往義大利 Lowe Alpine Strike 18L  黑橘 後背包 登山 | 健行 | 旅遊 | 書包商品頁面">
<img alt="義大利 Lowe Alpine Strike 18L  黑橘 後背包 登山 | 健行 | 旅遊 | 書包" class="rt-goods-list-item-image" itemprop="image" src="http://b.rimg.com.tw/s2/0/ad/cc/21642041621964_957_m.jpg" title="義大利 Lowe Alpine Strike 18L  黑橘 後背包 登山 | 健行 | 旅遊 | 書包">
</img></a>
</div>
<h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21642041621964" target="_blank" title="前往義大利 Lowe Alpine Strike 18L  黑橘 後背包 登山 | 健行 | 旅遊 | 書包商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">義大利 Lowe Alpine Strike 18L  黑橘 後背包 登山 | 健行 | 旅遊 | 書包</span>
</a>
</h3>
<p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<strong class="item-price-symbol rt-text-larger rt-text-price">2,190</strong>
</p>
<div class="rt-goods-list-item-info">
<div class="list-item-info-column">
<span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
</span>
</div>
<span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21642041621964" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
<span class="item-favorite-amount js-item-favorite-amount" title="已有0人將此商品加入追蹤清單">0</span>
</span>
</div>
</li>
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
<div class="rt-goods-list-item-image-wrap">
<a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21619691674383" target="_blank" title="前往【速捷戶外】英國 Lowe Alpine - Strike 24L背包(氧化鉛紅)FDP-55-24O 登山背包 旅行背商品頁面">
<img alt="【速捷戶外】英國 Lowe Alpine - Strike 24L背包(氧化鉛紅)FDP-55-24O 登山背包 旅行背" class="rt-goods-list-item-image" itemprop="image" src="http://d.rimg.com.tw/s1/b/37/0f/21619691674383_569_m.jpg" title="【速捷戶外】英國 Lowe Alpine - Strike 24L背包(氧化鉛紅)FDP-55-24O 登山背包 旅行背">
</img></a>
</div>
<h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21619691674383" target="_blank" title="前往【速捷戶外】英國 Lowe Alpine - Strike 24L背包(氧化鉛紅)FDP-55-24O 登山背包 旅行背商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">【速捷戶外】英國 Lowe Alpine - Strike 24L背包(氧化鉛紅)FDP-55-24O 登山背包 旅行背</span>
</a>
</h3>
<p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<strong class="item-price-symbol rt-text-larger rt-text-price">2,360</strong>
</p>
<div class="rt-goods-list-item-info">
<div class="list-item-info-column">
<span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
</span>
</div>
<span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21619691674383" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
<span class="item-favorite-amount js-item-favorite-amount" title="已有0人將此商品加入追蹤清單">0</span>
</span>
</div>
</li>
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
<div class="rt-goods-list-item-image-wrap">
<a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21619691632860" target="_blank" title="前往【速捷戶外】英國 Lowe Alpine - Strike 24L背包(黑)FDP-55-24B 登山背包 旅行背包 校商品頁面">
<img alt="【速捷戶外】英國 Lowe Alpine - Strike 24L背包(黑)FDP-55-24B 登山背包 旅行背包 校" class="rt-goods-list-item-image" itemprop="image" src="http://f.rimg.com.tw/s2/a/94/dc/21619691632860_474_m.jpg" title="【速捷戶外】英國 Lowe Alpine - Strike 24L背包(黑)FDP-55-24B 登山背包 旅行背包 校">
</img></a>
</div>
<h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21619691632860" target="_blank" title="前往【速捷戶外】英國 Lowe Alpine - Strike 24L背包(黑)FDP-55-24B 登山背包 旅行背包 校商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">【速捷戶外】英國 Lowe Alpine - Strike 24L背包(黑)FDP-55-24B 登山背包 旅行背包 校</span>
</a>
</h3>
<p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<strong class="item-price-symbol rt-text-larger rt-text-price">2,360</strong>
</p>
<div class="rt-goods-list-item-info">
<div class="list-item-info-column">
<span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
</span>
</div>
<span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21619691632860" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
<span class="item-favorite-amount js-item-favorite-amount" title="已有1人將此商品加入追蹤清單">1</span>
</span>
</div>
</li>
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
<div class="rt-goods-list-item-image-wrap">
<a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21618643483006" target="_blank" title="前往【速捷戶外】英國 Lowe Alpine - Strike 18L背包(黑)FDP-54-18B 登山背包 旅行背包 校商品頁面">
<img alt="【速捷戶外】英國 Lowe Alpine - Strike 18L背包(黑)FDP-54-18B 登山背包 旅行背包 校" class="rt-goods-list-item-image" itemprop="image" src="http://b.rimg.com.tw/s2/1/15/7e/21618643483006_265_m.jpg" title="【速捷戶外】英國 Lowe Alpine - Strike 18L背包(黑)FDP-54-18B 登山背包 旅行背包 校">
</img></a>
</div>
<h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21618643483006" target="_blank" title="前往【速捷戶外】英國 Lowe Alpine - Strike 18L背包(黑)FDP-54-18B 登山背包 旅行背包 校商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">【速捷戶外】英國 Lowe Alpine - Strike 18L背包(黑)FDP-54-18B 登山背包 旅行背包 校</span>
</a>
</h3>
<p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<strong class="item-price-symbol rt-text-larger rt-text-price">2,190</strong>
</p>
<div class="rt-goods-list-item-info">
<div class="list-item-info-column">
<span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
</span>
</div>
<span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21618643483006" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
<span class="item-favorite-amount js-item-favorite-amount" title="已有0人將此商品加入追蹤清單">0</span>
</span>
</div>
</li>
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
<div class="rt-goods-list-item-image-wrap">
<a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21618643479371" target="_blank" title="前往【速捷戶外】英國 Lowe Alpine - Strike 18L背包(氧化鉛紅)FDP-54-18O 登山背包 旅行背商品頁面">
<img alt="【速捷戶外】英國 Lowe Alpine - Strike 18L背包(氧化鉛紅)FDP-54-18O 登山背包 旅行背" class="rt-goods-list-item-image" itemprop="image" src="http://d.rimg.com.tw/s1/1/07/4b/21618643479371_300_m.jpg" title="【速捷戶外】英國 Lowe Alpine - Strike 18L背包(氧化鉛紅)FDP-54-18O 登山背包 旅行背">
</img></a>
</div>
<h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21618643479371" target="_blank" title="前往【速捷戶外】英國 Lowe Alpine - Strike 18L背包(氧化鉛紅)FDP-54-18O 登山背包 旅行背商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">【速捷戶外】英國 Lowe Alpine - Strike 18L背包(氧化鉛紅)FDP-54-18O 登山背包 旅行背</span>
</a>
</h3>
<p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<strong class="item-price-symbol rt-text-larger rt-text-price">2,190</strong>
</p>
<div class="rt-goods-list-item-info">
<div class="list-item-info-column">
<span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
</span>
</div>
<span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21618643479371" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
<span class="item-favorite-amount js-item-favorite-amount" title="已有0人將此商品加入追蹤清單">0</span>
</span>
</div>
</li>
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
<div class="rt-goods-list-item-image-wrap">
<a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21618643470169" target="_blank" title="前往【速捷戶外】英國 Lowe Alpine - Strike 18L背包(特攻隊紫)FDP-54-18M 登山背包 旅行背商品頁面">
<img alt="【速捷戶外】英國 Lowe Alpine - Strike 18L背包(特攻隊紫)FDP-54-18M 登山背包 旅行背" class="rt-goods-list-item-image" itemprop="image" src="http://f.rimg.com.tw/s1/0/e3/59/21618643470169_721_m.jpg" title="【速捷戶外】英國 Lowe Alpine - Strike 18L背包(特攻隊紫)FDP-54-18M 登山背包 旅行背">
</img></a>
</div>
<h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21618643470169" target="_blank" title="前往【速捷戶外】英國 Lowe Alpine - Strike 18L背包(特攻隊紫)FDP-54-18M 登山背包 旅行背商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">【速捷戶外】英國 Lowe Alpine - Strike 18L背包(特攻隊紫)FDP-54-18M 登山背包 旅行背</span>
</a>
</h3>
<p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<strong class="item-price-symbol rt-text-larger rt-text-price">2,190</strong>
</p>
<div class="rt-goods-list-item-info">
<div class="list-item-info-column">
<span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
</span>
</div>
<span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21618643470169" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
<span class="item-favorite-amount js-item-favorite-amount" title="已有0人將此商品加入追蹤清單">0</span>
</span>
</div>
</li>
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
<div class="rt-goods-list-item-image-wrap">
<a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21612175262410" target="_blank" title="前往【山野倉庫】歐洲-Lowe Alpine FDP-54  Strike 18多功能背包商品頁面">
<img alt="【山野倉庫】歐洲-Lowe Alpine FDP-54  Strike 18多功能背包" class="rt-goods-list-item-image" itemprop="image" src="http://b.rimg.com.tw/s2/7/de/ca/21612175262410_934_m.jpg" title="【山野倉庫】歐洲-Lowe Alpine FDP-54  Strike 18多功能背包">
</img></a>
<div class="rt-goods-list-item-similar">
<a class="rt-goods-list-item-similar-link" href="http://find.ruten.com.tw/search/alike.php?tag=4aa619899b28ba714d3c4c40d9fa8fc0a6766665&amp;g=21612175262410&amp;c=0&amp;k=Lowe Alpine Strike&amp;p=1" rel="nofollow" title="顯示多筆相似商品">多筆相似商品</a>
</div>
</div>
<h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21612175262410" target="_blank" title="前往【山野倉庫】歐洲-Lowe Alpine FDP-54  Strike 18多功能背包商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">【山野倉庫】歐洲-Lowe Alpine FDP-54  Strike 18多功能背包</span>
</a>
</h3>
<p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<strong class="item-price-symbol rt-text-larger rt-text-price">2,190</strong>
</p>
<div class="rt-goods-list-item-info">
<div class="list-item-info-column">
<span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
</span>
</div>
<span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21612175262410" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
<span class="item-favorite-amount js-item-favorite-amount" title="已有1人將此商品加入追蹤清單">1</span>
</span>
</div>
</li>
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
<div class="rt-goods-list-item-image-wrap">
<a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21612175204506" target="_blank" title="前往【山野倉庫】歐洲-Lowe Alpine FDP-55  Strike 24多功能背包商品頁面">
<img alt="【山野倉庫】歐洲-Lowe Alpine FDP-55  Strike 24多功能背包" class="rt-goods-list-item-image" itemprop="image" src="http://d.rimg.com.tw/s2/6/fc/9a/21612175204506_745_m.jpg" title="【山野倉庫】歐洲-Lowe Alpine FDP-55  Strike 24多功能背包">
</img></a>
<div class="rt-goods-list-item-similar">
<a class="rt-goods-list-item-similar-link" href="http://find.ruten.com.tw/search/alike.php?tag=7debeb82d85c9abc342e2408a53461eaf6a0beb2&amp;g=21612175204506&amp;c=0&amp;k=Lowe Alpine Strike&amp;p=1" rel="nofollow" title="顯示多筆相似商品">多筆相似商品</a>
</div>
</div>
<h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21612175204506" target="_blank" title="前往【山野倉庫】歐洲-Lowe Alpine FDP-55  Strike 24多功能背包商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">【山野倉庫】歐洲-Lowe Alpine FDP-55  Strike 24多功能背包</span>
</a>
</h3>
<p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<strong class="item-price-symbol rt-text-larger rt-text-price">2,360</strong>
</p>
<div class="rt-goods-list-item-info">
<div class="list-item-info-column">
<span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
</span>
</div>
<span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21612175204506" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
<span class="item-favorite-amount js-item-favorite-amount" title="已有0人將此商品加入追蹤清單">0</span>
</span>
</div>
</li>
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
<div class="rt-goods-list-item-image-wrap">
<a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21537638529247" target="_blank" title="前往Lowe Alpine Strike 18L休閒登山背包 FDP25-18TA墨西哥紅 健行旅行背包/多功能運動背包商品頁面">
<img alt="Lowe Alpine Strike 18L休閒登山背包 FDP25-18TA墨西哥紅 健行旅行背包/多功能運動背包" class="rt-goods-list-item-image" itemprop="image" src="http://f.rimg.com.tw/s1/a/b4/df/21537638529247_663_m.jpg" title="Lowe Alpine Strike 18L休閒登山背包 FDP25-18TA墨西哥紅 健行旅行背包/多功能運動背包">
</img></a>
</div>
<h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21537638529247" target="_blank" title="前往Lowe Alpine Strike 18L休閒登山背包 FDP25-18TA墨西哥紅 健行旅行背包/多功能運動背包商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">Lowe Alpine Strike 18L休閒登山背包 FDP25-18TA墨西哥紅 健行旅行背包/多功能運動背包</span>
</a>
</h3>
<p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<strong class="item-price-symbol rt-text-larger rt-text-price">1,995</strong>
</p>
<div class="rt-goods-list-item-info">
<div class="list-item-info-column">
<span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
</span>
</div>
<span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21537638529247" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
<span class="item-favorite-amount js-item-favorite-amount" title="已有0人將此商品加入追蹤清單">0</span>
</span>
</div>
</li>
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
<div class="rt-goods-list-item-image-wrap">
<a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21526917670477" target="_blank" title="前往[ Lowe Alpine ] Strike 24 運動背包/都會日用後背包 FDP25 墨西哥紅商品頁面">
<img alt="[ Lowe Alpine ] Strike 24 運動背包/都會日用後背包 FDP25 墨西哥紅" class="rt-goods-list-item-image" itemprop="image" src="http://b.rimg.com.tw/s1/3/46/4d/21526917670477_923_m.jpg" title="[ Lowe Alpine ] Strike 24 運動背包/都會日用後背包 FDP25 墨西哥紅">
</img></a>
</div>
<h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21526917670477" target="_blank" title="前往[ Lowe Alpine ] Strike 24 運動背包/都會日用後背包 FDP25 墨西哥紅商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">[ Lowe Alpine ] Strike 24 運動背包/都會日用後背包 FDP25 墨西哥紅</span>
</a>
</h3>
<p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<strong class="item-price-symbol rt-text-larger rt-text-price">2,193</strong>
</p>
<div class="rt-goods-list-item-info">
<div class="list-item-info-column">
<span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<span class="rt-goods-list-item-label">已賣</span>
<a class="rt-goods-list-item-sell-link" href="http://goods.ruten.com.tw/item/show?21526917670477#history" target="_blank" title="觀看所有紀錄">1</a>
</span>
</div>
<span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21526917670477" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
<span class="item-favorite-amount js-item-favorite-amount" title="已有2人將此商品加入追蹤清單">2</span>
</span>
</div>
</li>
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
<div class="rt-goods-list-item-image-wrap">
<a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21525839284177" target="_blank" title="前往【速捷戶外】英國 Lowe Alpine -Lowe Strike 12L背包(幻象灰) #FDP-25-12 登山背包商品頁面">
<img alt="【速捷戶外】英國 Lowe Alpine -Lowe Strike 12L背包(幻象灰) #FDP-25-12 登山背包" class="rt-goods-list-item-image" itemprop="image" src="http://d.rimg.com.tw/s1/c/67/d1/21525839284177_70_m.jpg" title="【速捷戶外】英國 Lowe Alpine -Lowe Strike 12L背包(幻象灰) #FDP-25-12 登山背包">
</img></a>
</div>
<h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21525839284177" target="_blank" title="前往【速捷戶外】英國 Lowe Alpine -Lowe Strike 12L背包(幻象灰) #FDP-25-12 登山背包商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">【速捷戶外】英國 Lowe Alpine -Lowe Strike 12L背包(幻象灰) #FDP-25-12 登山背包</span>
</a>
</h3>
<p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<strong class="item-price-symbol rt-text-larger rt-text-price">1,850</strong>
</p>
<div class="rt-goods-list-item-info">
<div class="list-item-info-column">
<span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
</span>
</div>
<span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21525839284177" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
<span class="item-favorite-amount js-item-favorite-amount" title="已有0人將此商品加入追蹤清單">0</span>
</span>
</div>
</li>
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
<div class="rt-goods-list-item-image-wrap">
<a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21525839264827" target="_blank" title="前往【速捷戶外】英國|Lowe Alpine-Lowe Strike 18背包(青鳥藍)FDP-25-18 登山背包 旅行背商品頁面">
<img alt="【速捷戶外】英國|Lowe Alpine-Lowe Strike 18背包(青鳥藍)FDP-25-18 登山背包 旅行背" class="rt-goods-list-item-image" itemprop="image" src="http://f.rimg.com.tw/s1/c/1c/3b/21525839264827_158_m.jpg" title="【速捷戶外】英國|Lowe Alpine-Lowe Strike 18背包(青鳥藍)FDP-25-18 登山背包 旅行背">
</img></a>
</div>
<h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21525839264827" target="_blank" title="前往【速捷戶外】英國|Lowe Alpine-Lowe Strike 18背包(青鳥藍)FDP-25-18 登山背包 旅行背商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">【速捷戶外】英國|Lowe Alpine-Lowe Strike 18背包(青鳥藍)FDP-25-18 登山背包 旅行背</span>
</a>
</h3>
<p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<strong class="item-price-symbol rt-text-larger rt-text-price">1,995</strong>
</p>
<div class="rt-goods-list-item-info">
<div class="list-item-info-column">
<span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
</span>
</div>
<span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21525839264827" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
<span class="item-favorite-amount js-item-favorite-amount" title="已有0人將此商品加入追蹤清單">0</span>
</span>
</div>
</li>
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
<div class="rt-goods-list-item-image-wrap">
<a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21525839249785" target="_blank" title="前往【速捷戶外】英國|Lowe Alpine-Lowe Strike 18背包(鱷魚綠)FDP-25-18 登山背包 旅行背商品頁面">
<img alt="【速捷戶外】英國|Lowe Alpine-Lowe Strike 18背包(鱷魚綠)FDP-25-18 登山背包 旅行背" class="rt-goods-list-item-image" itemprop="image" src="http://b.rimg.com.tw/s1/b/e1/79/21525839249785_15_m.jpg" title="【速捷戶外】英國|Lowe Alpine-Lowe Strike 18背包(鱷魚綠)FDP-25-18 登山背包 旅行背">
</img></a>
</div>
<h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21525839249785" target="_blank" title="前往【速捷戶外】英國|Lowe Alpine-Lowe Strike 18背包(鱷魚綠)FDP-25-18 登山背包 旅行背商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">【速捷戶外】英國|Lowe Alpine-Lowe Strike 18背包(鱷魚綠)FDP-25-18 登山背包 旅行背</span>
</a>
</h3>
<p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<strong class="item-price-symbol rt-text-larger rt-text-price">1,995</strong>
</p>
<div class="rt-goods-list-item-info">
<div class="list-item-info-column">
<span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
</span>
</div>
<span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21525839249785" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
<span class="item-favorite-amount js-item-favorite-amount" title="已有0人將此商品加入追蹤清單">0</span>
</span>
</div>
</li>
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
<div class="rt-goods-list-item-image-wrap">
<a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21525839238794" target="_blank" title="前往【速捷戶外】英國 Lowe Alpine -Lowe Strike 24L背包(幻象灰) #FDP-25-24 登山商品頁面">
<img alt="【速捷戶外】英國 Lowe Alpine -Lowe Strike 24L背包(幻象灰) #FDP-25-24 登山" class="rt-goods-list-item-image" itemprop="image" src="http://d.rimg.com.tw/s2/b/b6/8a/21525839238794_873_m.jpg" title="【速捷戶外】英國 Lowe Alpine -Lowe Strike 24L背包(幻象灰) #FDP-25-24 登山">
</img></a>
</div>
<h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21525839238794" target="_blank" title="前往【速捷戶外】英國 Lowe Alpine -Lowe Strike 24L背包(幻象灰) #FDP-25-24 登山商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">【速捷戶外】英國 Lowe Alpine -Lowe Strike 24L背包(幻象灰) #FDP-25-24 登山</span>
</a>
</h3>
<p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<strong class="item-price-symbol rt-text-larger rt-text-price">2,190</strong>
</p>
<div class="rt-goods-list-item-info">
<div class="list-item-info-column">
<span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
</span>
</div>
<span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21525839238794" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
<span class="item-favorite-amount js-item-favorite-amount" title="已有0人將此商品加入追蹤清單">0</span>
</span>
</div>
</li>
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
<div class="rt-goods-list-item-image-wrap">
<a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21525839223754" target="_blank" title="前往【速捷戶外】英國 Lowe Alpine -Lowe Strike 24L背包(單寧藍) #FDP-25-24 登山背包商品頁面">
<img alt="【速捷戶外】英國 Lowe Alpine -Lowe Strike 24L背包(單寧藍) #FDP-25-24 登山背包" class="rt-goods-list-item-image" itemprop="image" src="http://f.rimg.com.tw/s2/b/7b/ca/21525839223754_628_m.jpg" title="【速捷戶外】英國 Lowe Alpine -Lowe Strike 24L背包(單寧藍) #FDP-25-24 登山背包">
</img></a>
</div>
<h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21525839223754" target="_blank" title="前往【速捷戶外】英國 Lowe Alpine -Lowe Strike 24L背包(單寧藍) #FDP-25-24 登山背包商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">【速捷戶外】英國 Lowe Alpine -Lowe Strike 24L背包(單寧藍) #FDP-25-24 登山背包</span>
</a>
</h3>
<p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
<strong class="item-price-symbol rt-text-larger rt-text-price">2,190</strong>
</p>
<div class="rt-goods-list-item-info">
<div class="list-item-info-column">
<span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
</span>
</div>
<span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21525839223754" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
<span class="item-favorite-amount js-item-favorite-amount" title="已有0人將此商品加入追蹤清單">0</span>
</span>
</div>
</li>
</ul>
</div>
</div>
<div class="rt-ad">
<div class="ad-622" data-height="120" data-width="615" id="ad-622"></div>
</div>
<div id="foot_pagination"></div>
<div class="main-section-footer">
<div class="rt-ad">
<ul class="rt-ad-inline">
<li class="ad-16" data-height="80" data-width="420"></li>
<li class="ad-17 rt-ad-ml" data-height="80" data-width="350" id="ad-17"></li>
</ul>
</div>
</div>
</div>
</div>
<div class="side-section clearfix">
<div class="rt-ad rt-ad-side rt-ad-highlight">
<div class="rt-ad-title">搜尋精選</div>
<div class="rt-ad-body">
<div class="rt-ad-item image ad-468"></div>
</div>
</div>
<div class="rt-ad rt-ad-side rt-ad-related">
<h2 class="rt-ad-title">
              關聯廣告
              <a class="rt-ad-link float-right" href="http://pub.ruten.com.tw/adb/3.html" title="我要刊登廣告">我要刊登</a>
</h2>
<div class="rt-ad-kwad">
<ul class="ads" id="kwad">
</ul>
</div>
<div class="rt-ad-body">
<ul>
<li class="rt-ad-item ad-546-0 external-ads"></li>
<li class="rt-ad-item ad-546-1 external-ads"></li>
<li class="rt-ad-item ad-546-2 external-ads"></li>
</ul>
</div>
</div>
<div class="rt-ad">
<div class="rt-ad-body">
<div id="search_r_160x600"></div>
</div>
</div>
</div>
</div>
</div>
<div class="site-info">
<ul itemscope="" itemtype="http://schema.org/WPFooter">
<li class="about">
<a href="http://www.ruten.com.tw/about/about_us.htm" itemprop="url" rel="nofollow" title="關於露天拍賣">
<span itemprop="name">關於露天</span>
</a>
</li>
<li class="cm">
<a href="http://www.ruten.com.tw/system/index.html" itemprop="url" rel="nofollow" title="露天客服中心">
<span itemprop="name">客服中心</span>
</a>
</li>
<li class="qa">
<a href="http://www.ruten.com.tw/system/server_center.htm?000500010001" itemprop="url" rel="nofollow" title="網站常見問題">
<span itemprop="name">常見問題</span>
</a>
</li>
<li class="security">
<a href="http://www.ruten.com.tw/security/index.html" itemprop="url" rel="nofollow" title="關於交易安全說明">
<span itemprop="name">交易安全</span>
</a>
</li>
<li class="member">
<a href="http://www.ruten.com.tw/system/server_center.htm?00060001" itemprop="url" rel="nofollow" title="會員合約內容">
<span itemprop="name">會員合約</span>
</a>
</li>
<li class="privacy">
<a href="http://www.ruten.com.tw/system/server_center.htm?00060002" itemprop="url" rel="nofollow" title="隱私權聲明項目">
<span itemprop="name">隱私權聲明</span>
</a>
</li>
<li class="export">
<a href="http://twexport.ebay.com/" itemprop="url" rel="nofollow" target="_blank" title="eBay跨國交易服務">
<span itemprop="name">eBay跨國交易</span>
</a>
</li>
<li class="jobs">
<a class="bk13" href="http://www.104.com.tw/jobbank/cust_job/introduce.cfm?j=5a3a432634463e6956583a1d1d1d1d5f2443a363189j97" itemprop="url" rel="nofollow" target="_blank">
<span itemprop="name">徵人</span>
</a>
</li>
</ul>
<p>露天市集國際資訊股份有限公司版權所有，轉載必究‧Copyright©　PChome eBay Co., Ltd.</p>
</div>
<div id="rt_messenger_popup" v-cloak="" v-show="conversationLoad">
<div :class="{ 'is-hide': !showChatPopup }" class="rt-text-medium rt-messenger-popup">
<div @click="showChatPopup = !showChatPopup" class="rt-messenger-tab">
<span class="rt-messenger-text-ellipsis rt-messenger-tab-title">${ conversation.storeName }</span>
<div class="rt-messenger-icon-anchor">
<div @click.stop="closeMessenger" class="rt-messenger-icon-close" title="關閉視窗">×</div>
</div>
</div>
<div class="rt-messenger-info-block-wrap user-info">
<div class="rt-messenger-info-block">
<div class="user-info-link-wrap">
<a :href="'https://mybid.ruten.com.tw/user/' + conversation.userNick" target="_blank">
<span class="rti rti-home-rt-messenger"></span>賣場首頁
          </a>
</div>
<div class="user-info-link-wrap">
<a :href="'https://mybid.ruten.com.tw/credit/point?' + conversation.userNick" target="_blank">
<span class="rti rti-user-rt-messenger"></span>評價
          </a>
</div>
<div class="user-info-link-wrap">
<a :href="'http://class.ruten.com.tw/user/about.php?s=' + conversation.userNick" target="_blank">
<span class="rti rti-smile-o-rt-messenger"></span>關於我
          </a>
</div>
<div :class="{ 'is-online': userIsOnline }" class="rt-messenger-user-status">${ conversation.userIMStatus }</div>
</div>
</div>
<transition name="slide-up">
<div class="rt-messenger-info-block-wrap warning-msg" v-if="showChatBoxWarningMsg">
<div class="rt-messenger-info-block">
<div class="rt-messenger-icon-anchor">
<div @click="showChatBoxWarningMsg = !showChatBoxWarningMsg" class="rt-messenger-icon-close" title="關閉提示訊息">×</div>
</div>
<span class="rti rti-exclamation-triangle-rt-messenger-icon-warning"></span>
          提醒您！『賣家若要求您使用LINE帳號私下聯絡或轉帳匯款』是常見的詐騙手法，請勿上當！
        </div>
</div>
</transition>
<div class="rt-messenger-scrollbar rt-messenger-chat-box">
<transition-group class="rt-messenger-msg-list" name="rt-messenger-msg" tag="div">
<rt-messenger-msg :class="{ other: userId !== msgData.userid }" :data="msgData" :index="msgIndex" :key="msgData" :last-msg="conversation.msgs[msgIndex - 1]" :seen-msg-timestamp="conversation.seenMsgTimestamp" :user-id="userId" @show-msg-error-option-panel="showMsgErrorOptionPanel(msgData)" @show-photo-lightbox="showPhotoLightBox(msgData.msg)" class="rt-messenger-msg-wrap" v-for="(msgData, msgIndex) in conversation.msgs"></rt-messenger-msg>
</transition-group>
</div>
<div class="rt-text-medium rt-messenger-input-disabled" v-if="disableInput">
<span class="rti rti-ban-rt-messenger"></span>
      ${ disableInputErrorText }
    </div>
<div v-else="">
<div class="rt-messenger-info-msg-queue" v-if="showInfoMsgInQueue">
<div @click="resetInfoMsgQueue" class="delete-info-msg-button">×</div>
<img :src="infoMsg.info.goodsImgUrl" class="info-msg-img"/>
<div class="info-msg-info">
<span v-if="infoMsgInQueueTransLinkIsBlank">${ infoMsg.info.goodsName }</span>
<a :href="infoMsg.info.linkUrl" :title="infoMsg.info.goodsName" class="info-msg-goods-name" target="_blank" v-else="">${ infoMsg.info.goodsName }</a>
<p class="info-msg-goods-price">
<template v-if="infoMsgInQueueIsGoodsMsg">直購價</template>
<strong class="info-msg-price-value">&amp;dollar; ${ infoMsg.info.price }</strong>
</p>
</div>
</div>
<div class="rt-messenger-info-msg-queue-trans-status" v-if="showInfoMsgInQueue &amp;&amp; infoMsgInQueuehasTransDetail">
<div class="rt-messenger-text-ellipsis msg-order-status" v-if="!infoMsgInQueueTransStatusIsBlank">
<strong>狀態：</strong><span v-html="infoMsg.info.transStatus"></span>   
        </div>
<a :href="infoMsg.info.linkUrl" target="_blank" title="查看明細" v-if="!infoMsgInQueueTransLinkIsBlank">查看明細</a>
</div>
<div class="rt-messenger-quick-reply" ref="quick-reply-bar-container" v-if="quickReplyMsgs.length &gt; 0">
<span @click="slideQuickReplyBar('left')" class="rti rti-angle-left-rt-messenger rt-messenger-quick-reply-control-icon" v-if="showQuickReplyBarScrollLeftButton"></span>
<div class="rt-messenger-quick-reply-bar" ref="quick-reply-bar">
<rt-messenger-quick-reply-msg :key="quickReplyMsgText" :text="quickReplyMsgText" @quick-reply="sendQuickReplyMsg(quickReplyMsgText)" class="rt-messenger-text-ellipsis rt-messenger-quick-reply-text" ref="quick-reply-msg" v-cloak="" v-for="quickReplyMsgText in quickReplyMsgs"></rt-messenger-quick-reply-msg>
</div>
<span @click="slideQuickReplyBar('right')" class="rti rti-angle-right-rt-messenger rt-messenger-quick-reply-control-icon" v-if="showQuickReplyBarScrollRightButton"></span>
</div>
<div class="rt-messenger-input-area">
<textarea @keydown="autoResize" @keydown.ctrl.enter="addNewLine" @keypress.enter.prevent="sendMsg" @keyup="autoResize" class="rt-messenger-scrollbar rt-messenger-input" maxlength="500" placeholder="請勿隨意提供個人相關資料，如姓名、銀行帳戶…等"></textarea>
<div class="rt-messenger-toolbox">
<button @click="sendMsgButton" class="rt-button rt-button-default rt-button-mini">送出</button>
<div class="rt-messenger-icon-wrap">
<label @click="resetPhotoUploadInput" class="rt-messenger-icon rti-photo-rt-messenger-icon-photo">
<form class="rt-messenger-photo-upload-input-form">
<input @change="uploadPhoto" accept="image/png, image/jpg, image/jpeg" class="rt-messenger-transparent-file-input" multiple="" name="photo" type="file">
</input></form>
</label>
</div>
</div>
</div>
</div>
</div>
</div>
<div id="rt_messenger_option_panel" v-cloak="" v-show="showOptionPanel">
<div class="rt-messenger-option-panel-overlay">
<div class="rt-messenger-option-panel">
<div @click="hideOptionPanel" class="rt-messenger-option-panel-close-button">×</div>
<template v-if="isMsgError">
<div class="rt-text-larger rt-messenger-option-panel-content">
<span class="rti rti-large rti-exclamation-triangle-rt-messenger-error"></span>
          訊息傳送失敗！
        </div>
<div class="rt-messenger-option-panel-button-warp">
<button @click="reSendMsg" class="rt-button rt-button-default">重新傳送</button>
<button @click="deleteMsg" class="rt-button rt-button-default">刪除訊息</button>
</div>
</template>
<template v-else-if="isSetQuickReplyMsgResult">
<div class="rt-text-larger rt-messenger-option-panel-content">
<template v-if="setQuickReplyMsgsResult">
<span class="rti rti-large rti-check-circle-rt-messenger-success"></span>
            常用訊息儲存成功！
          </template>
<template v-else="">
<span class="rti rti-large rti-exclamation-triangle-rt-messenger-error"></span>
            系統忙碌中，請稍後再試！
          </template>
</div>
<div class="rt-messenger-option-panel-button-warp">
<button @click="showOptionPanel = !showOptionPanel" class="rt-button rt-button-default">確定</button>
</div>
</template>
<template v-else-if="isConfirmPopup">
<div class="rt-text-larger rt-messenger-option-panel-content">
          ${ confirmPopupMsg }
        </div>
<div class="rt-messenger-option-panel-button-warp">
<button @click="confirmPopupCallback(true)" class="rt-button rt-button-submit">確定</button>
<button @click="confirmPopupCallback(false)" class="rt-button rt-button-default">取消</button>
</div>
</template>
</div>
</div>
</div>
<div id="rt_messenger_lightbox" v-cloak="" v-show="showLightbox">
<div class="rt-messenger-lightbox-overlay">
<div class="rt-messenger-lightbox-close-button">×</div>
<img :src="lightboxImgUrl" class="rt-messenger-lightbox-upload-photo" v-on-clickaway="hidePhotoLightBox">
</img></div>
</div>
<script id="rt_messenger_msg" type="text/x-template">
  <div>
    <div class="rt-messenger-msg-time-divider-wrap"
      v-if="isFirstMsgOrNotSameDay">
      <div class="rt-messenger-msg-time-divider">${ timeDividerString }</div>
    </div>
    <div class="rt-messenger-msg"
      :class="{ other: sourceOther, goods: isGoodsInfoMsg }">
      <img class="rt-messenger-msg-upload-photo"
        v-if="isImgMsg"
        :src="msg"
        @click.stop="showPhotoLightBox">

      <div class="rt-messenger-msg-content-goods"
        v-else-if="isGoodsInfoMsg || isTransInfoMsg">
        <img class="msg-goods-img"
          :src="data.info.goodsImgUrl"
          :alt="data.info.goodsName"
          :title="data.info.goodsName">
        <div class="msg-goods-info">
          <span
            v-if="transLinkIsBlank">${ data.info.goodsName }</span>
          <a class="msg-goods-name"
            v-else
            :href="data.info.linkUrl" target="_blank"
            :title="data.info.infogoodsName">${ data.info.goodsName }</a>
          <p class="msg-goods-price">
            <template v-if="isGoodsInfoMsg">直購價</template>
            <strong class="msg-goods-price-value">&dollar;&nbsp;${ data.info.price }</strong>
          </p>
        </div>
      </div>

      <span class="rt-messenger-msg-content"
        v-else
        v-html="msg"></span>

      <div class="rt-messenger-msg-content-order"
        v-if="isTransInfoMsg && hasTransDetail">
        <div class="rt-messenger-text-ellipsis msg-order-status"
          v-if="!transStatusIsBlank">
          <strong>狀態：</strong><span v-html="data.info.transStatus"></span>&nbsp;&nbsp;&nbsp;
        </div>
        <a target="_blank" title="查看明細"
          v-if="!transLinkIsBlank"
          :href="data.info.linkUrl">查看明細</a>
      </div>

      <div class="rt-text-small rt-messenger-msg-label">
        <span class="rti rti-spin rti-refresh-rt-messenger"
          v-if="data.loading"></span>
        <span class="rti rti-exclamation-triangle-rt-messenger-error rt-messenger-msg-error-option"
          v-else-if="data.error"
          @click.stop="showMsgErrorOptionPanel"></span>
        <template v-else>
          <div class="rt-messenger-msg-seen-label"
            :class="{ seen: msgSeen }">已讀</div>
          <div>${ formatedTime }</div>
        </template>
      </div>
    </div>
  </div>
</script>
<script id="rt_messenger_quick_reply_msg" type="text/x-template">
  <span @click="addQuickReplyMsg">${ text }</span>
</script><script type="text/javascript">
  window.RT = window.RT || {};
  RT.config = {
    www_host: 'http://www.ruten.com.tw',
    search_host: 'http://find.ruten.com.tw',
    member_host: 'https://member.ruten.com.tw',
    mobile_host: 'https://m.ruten.com.tw',
    goods_host: 'http://goods.ruten.com.tw',
    mybid_host: 'https://mybid.ruten.com.tw',
    mybidu_host: 'https://mybidu.ruten.com.tw',
    point_host: 'https://point.ruten.com.tw',
    class_host: 'http://class.ruten.com.tw',
    pub_host: 'http://pub.ruten.com.tw',
    ad_host: 'https://ahd.ruten.com.tw',
    ahd_host: 'https://ahd.ruten.com.tw',
    kwad_host: 'http://kwad.ruten.com.tw',
    bid_img_dir: 'http://www.ruten.com.tw/images',
    alert_host: 'https://alert.ruten.com.tw',
    im_host: 'https://im.ruten.com.tw',
    rtapi_host: 'https://rtapi.ruten.com.tw'
  };
  RT.config.ppPayway = [
    'PAYLINK',
    'PL_ATM',
    'PL_ACCOUNT',
    'PL_POSTATM',
    'PP_CRD',
    'PP_CRD_N3',
    'PP_CRD_N6',
    'PP_CRD_N12',
    'PP_EACH'
  ];
  RT.config.ppRelatedPayway = RT.config.ppPayway.concat([
    'CVS_COD',
    'SEVEN',
    'SEVEN_COD',
    'TCAT_COD'
  ]);
  if(window.Box) {
    RT.App = Box.Application;

    RT.App.initBroadcast = function(name, obj) {
      setTimeout(function() {
        RT.App.broadcast(name, obj);
      });
    };
  }
</script><!-- ts code start -->
<script type="text/javascript">
(function(R,u,t,e,n,T,S){
t=R[t]=function(){(t.queue=t.queue||[]).push(arguments);};
t.host=n;T=u.createElement(e);T.async=1;T.src=n+'/ts_async.js?8';
S=u.getElementsByTagName(e)[0];S.parentNode.insertBefore(T,S);
}(window,document,'_ts_js_fn','script','https://ts.ruten.com.tw'));

_ts_js_fn('init');
_ts_js_fn('run');
</script>
<!-- ts code end -->
<script type="text/javascript">
    (function(i,s,o,g,r,a,m){i['GoogleAnalyticsObject']=r;i[r]=i[r]||function(){
    (i[r].q=i[r].q||[]).push(arguments)},i[r].l=1*new Date();a=s.createElement(o),
    m=s.getElementsByTagName(o)[0];a.async=1;a.src=g;m.parentNode.insertBefore(a,m)
    })(window,document,'script','//www.google-analytics.com/analytics.js','ga');

    ga('create', 'UA-40451066-2', 'ruten.com.tw', {'sampleRate' : 1});
    ga('send', 'pageview');
    </script>
<script charset="utf-8" src="http://www.ruten.com.tw/js/vue-2.1.10.min.js" type="text/javascript"></script>
<script src="http://www.ruten.com.tw/js/vuex-2.1.2.min.js" type="text/javascript"></script>
<script src="http://www.ruten.com.tw/js/vue-resource-1.2.1.min.js" type="text/javascript"></script>
<script src="http://www.ruten.com.tw/js/js.cookie-2.1.2.min.js" type="text/javascript"></script>
<script charset="utf-8" src="http://www.ruten.com.tw/js/lib/vue-clickaway.min.js" type="text/javascript"></script>
<script charset="utf-8" src="http://www.ruten.com.tw/js/d/desktop_im.js?v=20170925_5915da6812" type="text/javascript"></script>
<script charset="utf-8" src="http://www.ruten.com.tw/js/d/header_autocomplete.js?v=20171012_cde0c8ad80" type="text/javascript"></script>
<script src="http://www.ruten.com.tw/js/jquery-1.7.1.min.js" type="text/javascript"></script>
<script src="http://www.ruten.com.tw/js/t3-jquery.min.js" type="text/javascript"></script>
<script charset="utf-8" src="http://www.ruten.com.tw/js/d/desktop.js?v=20170707_a80f57d565" type="text/javascript"></script>
<div class="gotop-wrap">
<div class="gotop" id="gotop">
<p class="arrow">^</p>
<p class="text">TOP</p>
</div>
</div>
</div>
<script id="partial_search_categories" type="x-tmpl-mustache">
{{# if_show_search_categories }}
<div class="rt-filter">
  <div class="rt-filter-title">選擇分類顯示</div>
  <div class="rt-filter-body">
    <ul>
    {{# loop_category_results }}
      <li>
        <a class="rt-filter-option" href="{{ category_result_url }}">{{ category_result_name }}
          <small class="rt-text-small rt-text-notice">{{ category_result_count }}</small>
        </a>
      </li>
    {{/ loop_category_results }}
    {{^ loop_category_results }}
      <li>no item available</li> 
    {{/ loop_category_results }}
    </ul>
  </div>
</div>
{{/ if_show_search_categories }}
</script>
<script charset="UTF-8" src="http://www.ruten.com.tw/js/mustache-2.1.2.min.js?v=20150626" type="text/javascript"></script>
<script src="http://www.ruten.com.tw/js/ahd_loader_openx.js?v=20160323" type="text/javascript"></script>
<script src="http://www.ruten.com.tw/js/ahd_loader_dfp.js?v=20160714" type="text/javascript"></script>
<script src="http://www.ruten.com.tw/js/ahd_flash.js?v=20140114" type="text/javascript"></script>
<script src="http://www.ruten.com.tw/js/gads_loader.js" type="text/javascript"></script>
<script charset="utf-8" src="http://www.ruten.com.tw/js/lib/rt_utility.min.js?v=20170822_d6daac0b8c" type="text/javascript"></script>
<script charset="utf-8" src="http://www.ruten.com.tw/js/lib/rt_page.min.js?v=20161215_8b7e9fb387" type="text/javascript"></script>
<script charset="utf-8" src="http://www.ruten.com.tw/js/lib/jquery.ruten.ruten_ad.min.js?v=20161128_a55726dcf8" type="text/javascript"></script>
<script charset="utf-8" src="http://www.ruten.com.tw/js/d/find/search/s000.view.js?v=20171012_46c9782119" type="text/javascript"></script>
</body>
</html>
"""


'''
<li class="rt-goods-list-item " itemscope="" itemtype="http://schema.org/Product">
    <div class="rt-goods-list-item-image-wrap">
        <a class="rt-goods-list-item-image-link" href="http://goods.ruten.com.tw/item/show?21716971337096" target="_blank" title="前往Lowe Alpine Strike 24 運動背包/都會日用後背包 FDP55 黑商品頁面">
            <img alt="Lowe Alpine Strike 24 運動背包/都會日用後背包 FDP55 黑" class="rt-goods-list-item-image" itemprop="image" src="http://b.rimg.com.tw/s2/6/29/88/21716971337096_111_m.jpg" title="Lowe Alpine Strike 24 運動背包/都會日用後背包 FDP55 黑">

        </a>
    </div>
    <h3 class="rt-goods-list-item-name">
<a class="rt-goods-list-item-name-link" href="http://goods.ruten.com.tw/item/show?21716971337096" target="_blank" title="前往Lowe Alpine Strike 24 運動背包/都會日用後背包 FDP55 黑商品頁面">
<span class="rt-goods-list-item-name-text" itemprop="name">Lowe Alpine Strike 24 運動背包/都會日用後背包 FDP55 黑</span>
</a>
</h3>
    <p class="rt-goods-list-item-price text-right" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
        <strong class="item-price-symbol rt-text-larger rt-text-price">2,363</strong>
    </p>
    <div class="rt-goods-list-item-info">
        <div class="list-item-info-column">
            <span class="rt-goods-list-item-sell-info" itemprop="offers" itemscope="" itemtype="http://schema.org/Offer">
</span>
        </div>
        <span class="rt-goods-list-item-add-favorite">
<span class="item-add-favorite-link" data-no="21716971337096" data-type="favorite-item-link"><i class="item-favorite-icon-box rti rti-heart-o-disabled"></i></span>
        <span class="item-favorite-amount js-item-favorite-amount" title="已有0人將此商品加入追蹤清單">0</span>
        </span>
    </div>
</li>
'''

soup = BeautifulSoup(html_doc, 'html.parser')
for each_li in soup.findAll('li',{'class':'rt-goods-list-item'}):
    #print each_li
    raw_input()
    #print each_div.h2.span.text 
    print each_li.div.a['href']
    print each_li.div.a['title']
    print each_li.p.strong.text
```

