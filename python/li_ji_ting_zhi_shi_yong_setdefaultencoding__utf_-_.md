# 立即停止使用 setdefaultencoding('utf-8')， 以及為什麼



## 最壞實踐

```py
import sys
reload(sys)
sys.setdefaultencoding('utf-8')
```

上面這種代碼曾經（現在依然）是解決中文編碼的萬能鑰匙。解決編碼錯誤問題一勞永逸，從此和 `UnicodeEncodeError: 'ascii' codec can't encode characters in position 0-5: ordinal not in range(128) 說 byebye 。`

那麼現在，對於那些想解決 UnicodeEncodeError 問題而搜索到這篇文章的讀者，我要說的是，不要用以上的代碼片段。下面我來說說為什麼，以及我們應該怎麼做。

###sys.setdefaultencoding('utf-8') 會導致的兩個大問題

簡單來說這麼做將會使得一些代碼行為變得怪異，而這怪異還不好修復，以一個不可見的 bug 存在著。下面我們舉兩個例子。


## 1. 編碼錯誤

```py
import chardet
def print_string(string):
    try:
        print(u"%s" % string)
    except UnicodeError:
        print u"%s" % unicode(byte_string, encoding=chardet.detect(string)['encoding'])

print_string(u"Ã¾".encode("latin-1"))

import sys
reload(sys)
sys.setdefaultencoding('utf-8')

print(key_in_dict('Ã¾'))
```

輸出：

```py
$~ Ã¾
$~ þ
```

在上面的代碼中，默認的 ascii 編碼無法解碼，Ã¾ latin-1 編碼 hex 表示是 c3 be ，顯然是超出了只有128個字符的 ascii 碼集的，引發 UnicodeError 異常，進入異常處理。異常處理則會根據編碼探測，用最可能的編碼來解碼，會比較靠譜地輸出 Ã¾ 。
而一旦我們將 defaultencoding 設置為 utf-8，因為 utf-8 的字符範圍是完全覆蓋 latin-1，因此，會直接使用 utf-8 進行解碼。c3 be 在 utf-8 中，是 þ。於是我們打印出了完全不同的字符。
可能你們會說我們不會寫這樣的代碼。如果我們寫了也會做修正。但如果是第三方庫這麼寫了呢？項目依賴的第三方庫就這麼 bug 了。如果你不依賴第三方庫，那麼下面這個 bug，還是逃不過。


## 2. dictionray 行為異常

假設我們要從一個 dictionary 裡查找一個 key 是否存在，通常來說，有兩種可行方法。


```py
#-*- coding: utf-8 -*-

d = {1:2, '1':'2', '你好': 'hello'}
def key_in_dict(key)
    if key in d:
        return True
    return False

def key_found_in_dict(key):
    for _key in d:
        if _key == key:
            return True
    return False
```

我們對比下改變系統默認編碼前後這倆函數的輸出有什麼不同。


```py
#-*- coding: utf-8 -*-

print(key_in_dict('你好'))
print(key_found_dict('你好'))
print(key_in_dict(u'你好'))
print(key_found_in_dict(u'你好'))

print('------utf-8------')

import sys
reload(sys)
sys.setdefaultencoding('utf-8')

print(key_in_dict('你好'))
print(key_found_dict('你好'))
print(key_in_dict(u'你好'))
print(key_found_in_dict(u'你好'))
```

輸出：

```py
$~ True
$~ True
$~ False
$~ False
$~ ------utf-8------
$~ True
$~ True
$~ False
$~ True
```

可以看到，當默認編碼改了之後，兩個函數的輸出不再一致。
dict 的 in 操作符將鍵做哈希，並比較哈希值判斷是否相等。對於 ascii 集合內的字符來說，不管是字節字符類型還是還是 unicode 類型，其哈希值是一樣的，如 u'1' in {'1':1} 會返回 True，而超出 ascii 碼集的字符，如上例中的 '你好'，它的字節字符類型的哈希與 unicode 類型的哈希是不一樣的。
而 == 操作符則是做了一次轉換，將字節字符（byte string，上面的 '你好'）轉換成 unicode（u'你好'） 類型，然後對轉換後的結果做比較。在 ascii 系統默認編碼中，'你好'轉換成 Unicode 會產生 Warning: UnicodeWarning: Unicode equal comparison failed to convert both arguments to Unicode - interpreting them as being unequal，因為超出碼集無法轉換，系統會默認其不相等。當系統編碼被我們手動改為 utf-8 後，這個禁忌則被解除，'你好' 能夠順利被轉換成 unicode，最後的結果就是，in 和 == 行為不再一致。


## 問題的根源：Python2 中的 string

Python 為了讓其語法看上去簡潔好用，做了很多 tricky 的事情，混淆 byte string 和 text string 就是其中一例。

在 Python 裡，有三大類 string 類型，unicode（text string），str（byte string，二進制數據），basestring，是前兩者的父類。

其實，在語言設計領域，一串字節（sequences of bytes）是否應該當做字符串（string）一直是存在爭議的。我們熟知的 Java 和 C# 投了反對票，而 Python 則站在了支持者的陣營裡。其實我們在很多情況下，給文本做的操作，比如正則匹配、字符替換等，對於字節來說是用不著的。而 Python 認為字節就是字符，所以他們倆的操作集合是一致的。
然後進一步的，Python 會在必要的情況下，嘗試對字節做自動類型轉換，例如，在上文中的 ==，或者字節和文本拼接時。如果沒有一個編碼（encoding），兩個不同類型之間的轉換是無法進行的，於是，Python 需要一個默認編碼。在 Python2 誕生的年代，ASCII 是最流行的（可以這麼說吧），於是 Python2 選擇了 ASCII。然而，眾所周知，在需要需要轉換的場景，ASCII 都是沒用的（128個字符，夠什麼吃）。

在歷經這麼多年吐槽後，Python 3 終於學乖了。默認編碼是 Unicode，這也就意味著，做所有需要轉換的場合，都能正確併成功的轉換。



## 最佳實踐

說了這麼多，如果不遷移到 Python 3，能怎麼做呢？
有這麼幾個建議：

- 所有 text string 都應該是 unicode 類型，而不是 str，如果你在操作 text，而類型卻是 str，那就是在製造 bug。

- 在需要轉換的時候，顯式轉換。從字節解碼成文本，用 var.decode(encoding)，從文本編碼成字節，用 var.encode(encoding)。

- 從外部讀取數據時，默認它是字節，然後 decode 成需要的文本；同樣的，當需要向外部發送文本時，encode 成字節再發送。

##References

- UTF-8 codes
- LATIN-1 codes
- ASCII wiki
- Why sys.setdefaultencoding() will break code
- Getting unicode right in Python
