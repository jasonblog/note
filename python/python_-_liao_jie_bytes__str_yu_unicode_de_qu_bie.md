# Python - 瞭解bytes、str與unicode的區別


 Python3有兩種表示字符序列的類型：bytes和str。前者的實例包含原始的8位值，後者的實例包含Unicode字符。
 
Python2也有兩種表示字符序列的類型，分別叫做str和Unicode。與Python3不同的是，str實例包含原始的8位值；而unicode的實例，則包含Unicode字符。


把Unicode字符表示為二進制數據（也就是原始8位值）有許多種辦法。最常見的編碼方式就是UTF-8。但是，Python3的str實例和Python2的unicode實例都沒有和特定的二進制編碼形式相關聯。要想把Unicode字符轉換成二進制數據，就必須使用encode方法。要想把二進制數據轉換成Unicode字符，則必須使用decode方法。

編寫Python程序的時候，一定要把編碼和解碼操作放在界面最外圍來做。程序的核心部分應該使用Unicode字符類型（也就是Python3中的str、Python2中的unicode），而且不要對字符編碼做任何假設。這種辦法既可以令程序接受多種類型的文本編碼（如Latin-1、Shift JIS和Big5），又可以保證輸出的文本信息只採用一種編碼形式（最好是UTF-8）。

由於字符類型有別，所以Python代碼中經常會出現兩種常見的使用情境：
開發者需要原始8位值，這些8位值表示以UTF-8格式（或其他編碼形式）來編碼的字符。

開發者需要操作沒有特定編碼形式的Unicode字符。
所以，我們需要編寫兩個輔助（helper）函數，以便在這兩種情況之間轉換，使得轉換後的輸入數據能夠符合開發者的預期。


```py
#在Python3中，我們需要編寫接受str或bytes，並總是返回str的方法：
def to_str(bytes_or_str):
  if isinstance(bytes_or_str, bytes):
    value = bytes_or_str.decode('utf-8')
  else:
    value = bytes_or_str
  return value # Instance of str
   
#另外，還需要編寫接受str或bytes，並總是返回bytes的方法：
def to_bytes(bytes_or_str):
  if isinstance(bytes_or_str, str):
    value = bytes_or_str.encode('utf-8')
  else:
    value = bytes_or_str
  return value # Instance of bytes

#在Python2中，需要編寫接受str或unicode，並總是返回unicode的方法：
#python2
def to_unicode(unicode_or_str):
  if isinstance(unicode_or_str, str):
    value = unicode_or_str.decode('utf-8')
  else:
    value = unicode_or_str
  return value # Instance of unicode
   
#另外，還需要編寫接受str或unicode，並總是返回str的方法：
#Python2
def to_str(unicode_or_str):
  if isinstance(unicode_or_str, unicode):
    value = unicode_or_str.encode('utf-8')
  else:
    value = unicode_or_str
  reutrn vlaue # Instance of str
  
```



在Python中使用原始8位值與Unicode字符時，有兩個問題需要注意。
第一個問題可能會出現在Python2裡面。如果str只包含7位ASCII字符，那麼unicode和str實例似乎就成了同一種類型。

* 可以用+操作符把這種str與unicode連接起來。
* 可以用等價於不等價操作符，在這種str實例與unicode實例之間進行比較。
* 在格式字符串中，可以用』%s'等形式來代表unicode實例。
 
這些行為意味著，在只處理7位ASCII的情境下，如果某函數接受str，那麼可以給它傳入unicode；如果某函數接受unicode，那麼也可以給它傳入str。而在Python3中，bytes與str實例則絕對不會等價，即使是空字符串也不行。所以，在傳入字符序列時必須留意其類型。

第二個問題可能會出現在Python3裡面。如果通過內置的open函數獲取了文件句柄，那麼請注意，該句柄默認會採用UTF-8編碼格式來操作文件。而在Python2中，文件操作的默認編碼格式則是二進制形式。這可能會導致程序出現怪的錯誤，對習慣了Python2的程序員來說更是如此。

例如，現在要向文件中隨機寫入一些二進制數據。下面這種用法在Python2中可以正常運作，但在Python3中不行。

```py
with open('/tmp/random.bin','w') as f:
  f.write(os.urandom(10))
>>>
TypeError: must be str, not bytes
``` 


發生上述異常的原因在於，Python3給open函數添加了名為encoding的新參數，而這個新參數的默認值是』utf-8』。這樣在文件句柄上進行read和write操作時，系統就要求開發者必須傳入包含Unicode字符的str實例，而不接受包含二進制數據的bytes實例。



為瞭解決這個問題，我們必須用二進制寫入模式（』wb'）來開啟待操作的文件，而不能像原來那樣，採用字符寫入模式(『w')。按照下面這種方式來使用open函數，即可同時適配Python2與Python3：


```py
with open('/tmp/ramdom.bin','wb') as f:
    f.write(os.urandom(10))
```


要點：
- 在Python3中，bytes是一種包含8位值的序列，str是一種包含Unicode字符的序列。開發者不能以>或+等操作符來混同操作bytes和str實例。

- 在Python2中，str是一種包含8位值的序列，unicode是一種包含Unicode字符的序列。如果str只含有7位ASCII字符，那麼可以通過相關的操作來同時使用str和unicode。

- 在對輸入的進行操作之前，使用輔助函數來保證字符序列的類型與開發者的期望相符（有的時候，開發者想操作以UTF-8格式來編碼的8位值，有的時候，則想操作Unicode字符）。

- 從文件中讀取二進制數據，或向其中寫入二進制數據時，總應該以』rb'或』wb'等二進制模式來開啟文件。

摘自《編寫高質量Python代碼的59個有效方法》--第三條：瞭解bytes、str與unicode的區別