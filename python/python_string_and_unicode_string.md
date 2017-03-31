# python string and unicode string


`檔案編碼 直譯器編碼 系統編碼要一致`

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


`vim : set fileencoding=Big5 or gedit 另存big5`

```py
#!/usr/bin/env python     
 # -*- coding: big5 -*-

big5_s = '中文abc'
print repr(big5_s)
print big5_s
```

轉成unicode string。
在unicode中，每個character都對應到一個code point.


`vim : set fileencoding=Big5 or gedit 另存big5`

```py
#!/usr/bin/env python     
 # -*- coding: big5 -*-

big5_s = 'いゅabc'
print repr(big5_s)
print big5_s

unicode_s = big5_s.decode('big5')
print repr(unicode_s)
print unicode_s
```

```sh
'\xa4\xa4\xa4\xe5abc'
����abc                               // 亂碼可能是終端機是支援utf-8
u'\u4e2d\u6587abc'
中文abc
```


`Unicode所需的儲存空間大`，為了`節省空間`，就有了`UTF ( Unicode Transformation Format )` 。

從unicode轉成utf8
'中'和'文'各被轉成3byte。
英文字母則各被轉成1byte

`vim : set fileencoding=Big5 or gedit 另存big5`

```py
#!/usr/bin/env python     
 # -*- coding: big5 -*-

big5_s = 'いゅabc'
print repr(big5_s)
print big5_s

unicode_s = big5_s.decode('big5')
print repr(unicode_s)
print unicode_s

utf8_s = unicode_s.encode('utf-8')
print repr(utf8_s)
print utf8_s
```

```sh
'\xa4\xa4\xa4\xe5abc'
����abc
u'\u4e2d\u6587abc'
中文abc
'\xe4\xb8\xad\xe6\x96\x87abc'
中文abc
```

如果是寫在text file中，則可以指定encoding。
下面的例子是用utf-8。

可以直接在字串前加u'，直接轉成unicode。


```py
# -*- coding: utf-8 -*- 

utf8_s = '中文abc'
print utf8_s , ":" , repr( utf8_s )

unicode_s = u'中文abc'
print unicode_s , ":" , repr( unicode_s )

utf8_to_unicode_s = utf8_s.decode( 'utf-8' )
print utf8_to_unicode_s , ":" , repr( utf8_to_unicode_s )
```

```sh
中文abc : '\xe4\xb8\xad\xe6\x96\x87abc'
中文abc : u'\u4e2d\u6587abc'
中文abc : u'\u4e2d\u6587abc'
```