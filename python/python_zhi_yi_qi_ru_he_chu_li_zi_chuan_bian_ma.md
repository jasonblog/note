# Python 直譯器如何處理字串編碼


首先，進入 python 直譯器，輸入下列指令：

以我所使用的電腦 ( Ubuntu )為例 [1] ，

```py
>>> import sys
>>> sys.stdin.encoding
'UTF-8'
>>> sys.stdout.encoding
'UTF-8'
```

我們可以看到標準輸入與標準輸出採用的編碼方式是 UTF-8。

這代表 當我在 python 直譯器輸入(sys.stdin)的任何字元 (包含 input() 或是 raw_input() 所輸入的字元 ) 都是以 UTF-8 編碼 。

而顯示到到標準輸出 [2] 的字元則是 UTF-8 編碼。

由於我們已經知道 stdin 的編碼是使用 UTF-8　， 因此對於型態是 str 的字串物件，就可以透過 decode 方法得到該字串的 unicode string。

```py
>>> '你'.decode("utf-8") or '你'.decode("UTF-8")
u'\u4f60'
```

對於一個 unicode string ，透過指定編碼，可以得到 8-bit string。

```py
>>> u'\u4f60'.encode("utf-8")
你
```

當我們沒有指定編碼的時候， 則 python 會使用預設的編碼，通常這個預設的編碼是 ascii 。
```py
>>> import sys
>>> sys.getdefaultencoding()
'ascii'
```

當我們將 8-bit string 轉換成 unicode string 時，沒有特別指出編碼的方式時， python 就會採用預設的編碼。

由於 ascii 編碼所允許的值是 0-127。 有時候會讓我們遇到一些麻煩。

```py
>>> u = u'你'
>>> u
u'\u4f60'
>>> print u
你
>>> str(u)
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
UnicodeEncodeError: 'ascii' codec can't encode character u'\u4f60' in position 0: ordinal not in range(128)
>>> a = '哈'
>>> u + a
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
UnicodeDecodeError: 'ascii' codec can't decode byte 0xe5 in position 0: ordinal not in range(128)
```

如何修改系統預設的編碼表示呢？

在安裝 python 的 site-packages 下，創造一個名為 sitecustomize.py，內容是

```py
import sys
sys.setdefaultencoding('utf-8')
```

當你找不到要放在哪裡或者是沒有更改它的權限時，則可以

```py
import sys
reload(sys)
sys.setdefaultencoding('utf-8')
```

因為 python(2.6)在初始化之後會刪除 sys.setdefaultencoding 這個方法，因此需要重新載入 reload(sys)。

[1]	Windows 在 IDLE 以及 DOS console 下的 sys.stdout.encoding 與 sys.stdin.encoding　很可能不會相同 。比如 IDLE 用的編碼是 cp1252，DOS console 用的編碼是 cp437。
[2]	通常是螢幕。

##Python str unicode
- 瞭解 Unicode

http://python.ez2learn.com/basic/unicode.html

該網頁介紹了字元編碼，以及 python2 裡頭 str 與 unicode 的關係。