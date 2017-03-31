# python string and unicode string


檔案編碼 直譯器編碼 系統編碼

python2 string有兩種 :

- 預設的byte string 
- 另一種是unicode string。

python string是一連串的byte，`預設是用ascii`來encode。
可以載入一個超出ascii定義範圍的character。


```py
>>> w = 'abc' + chr( 0xff )
>>> w
'abc\xff'
```

我IDLE的encode方法是big5
'中'和'文'各被encode成兩個byte。


```py
coding=Big5
>>> big5_s = '中文abc'
>>> s
'\xa4\xa4\xa4\xe5abc'
>>> print big5_s
中文abc
```

轉成unicode string。
在unicode中，每個character都對應到一個code point.

```py
>>> unicode_s = big5_s.decode( 'big5' )
>>> unicode_s
u'\u4e2d\u6587abc'
>>> print unicode_s
中文abc
```
