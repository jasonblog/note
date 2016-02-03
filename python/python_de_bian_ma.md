# Python 的編碼


在Python 2.x，程式中所有字串，其實都是原始位元組集合。如果原始碼中寫了非ASCII字元串，必須在第一行放置編碼聲明（encoding declaration）。例如：

- main.py


```py
# coding=Big5
text = '測試'
print len(text) # 顯示 4
```


程式會顯示4而不是2，因為Big5每個中文字佔兩個位元組，而兩個中文字就佔四個位元組，所以上例中，len()實際上是計算出'測試'兩字Big5編碼實現下的位元組長度。

為了支援Unicode，Python 2.x提供了u前置字來產生unicode物件。

- main.py

```py
# coding=Big5
text = u'測試'
print type(text) # 顯示 <type 'unicode'>
print len(text)  # 顯示 2
```

chr()可取一個數字傳回對應ASCII編碼的str實例，unichr()是其Unicode對應版本，可傳回對應Unicode碼點的unicode實例。ord()可接受str或unicode實例，分別傳回其ASCII編碼或Unicode碼點。例如：

```py

>>> chr(65)
'A'
>>> unichr(65)
u'A'
>>> unichr(28204)
u'\u6e2c'
>>> ord('A')
65
>>> ord(u'A')
65
>>> ord(u'測')
28204
>>>
```


如果有一個str實例，可以使用unicode()方法並指定實現編碼，或者使用str的decode方法並指定實現編碼，將之轉換為unicode物件。unicode的encode()方法則可指定實現編碼，將之轉為代表位元組實現的str實例。例如：


```py
>>> text = u'測試'
>>> b_str = text.encode('big5')
>>> type(b_str)
<type 'str'>
>>> b_str.decode('big5')
u'\u6e2c\u8a66'
>>> unicode(b_str, "big5")
u'\u6e2c\u8a66'
>>>

```
在讀取文字檔案時，進入的資料都以代表位元組的str表示，若讀取非ASCII的文字檔案，必須自行轉換編碼。例如若有個UTF-8的文字檔案text.txt中記錄了「測試」兩字，則程式讀取時可如下：

- main.py


```py
f = open('text.txt', 'r')
b_str = f.read()
f.close()
print b_str.decode('utf-8')  # 自行判斷標準輸出編碼
print b_str.decode('utf-8').encode('big5') # 標準輸出編碼為 big5
```


以上最後兩句都會在標準輸出顯示測試兩字。類似地，如果要將unicode寫入文字檔案，必須使用encode()轉換為指定編碼實現。例如，以下程式可將「測試」以Big5編碼寫入文字檔案：


- main.py


```py
# coding=utf-8
text = u'測試'
b_str = text.encode('big5')
f = open('text.txt', 'w')
f.write(b_str)
f.close()
```
在Python 3.x中，預設.py檔案必須是UTF-8編碼。如果.py檔案想要是UTF-8以外的編碼，同樣必須在第一行放置編碼聲明。

- main.py


```py
# coding=Big5
text = '測試'
print(len(text)) # 顯示 2
```

Python 3.x中的字串都是Unicode，因此上例最後是顯示2，表示兩個字元，而不是像Python 2.x中，會顯示字串實際的位元組長度。

在Python3中，每個字串都是Unicode，不使用內部編碼表現，而使用str實例作為代表。如果想將字串轉為指定的編碼實作，可以使用encode()方法取得一個bytes實例，如果有個bytes實例，也可以使用decode()方法指定編碼取得str實例：

```py
>>> '元'.encode('big5')
b'\xa4\xb8'
>>> '元'.encode('utf-8')
b'\xe5\x85\x83'
>>> '元'.encode('big5').decode('big5')
'元'
>>>
```

如果你要將資料寫入檔案或從檔案讀出，可以使用open()函式，其中可以指定編碼：

```py
open(file,mode="r",buffering=None,encoding=None,
     errors=None,newline=None,closefd=True)

```

例如，若要讀取UTF-8文字檔案：
- main.py

```py
name = input('請輸入檔名：')
file = open(name, 'r', encoding='UTF-8')
content = file.read()
print(content)
file.close()
```

讀入的結果為str實例。若要將str實例寫入Big5編碼文字檔案可如下：
- main.py

```py
name = input('請輸入檔名：')
file = open(name, 'w', encoding = 'Big5')
file.write('test')
file.close()
```



更多有關 Python 編碼的資料，可以參考 Making Sense of Python Unicode。

