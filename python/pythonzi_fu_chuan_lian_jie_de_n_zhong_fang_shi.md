# python字符串連接的N種方式


python中有很多字符串連接方式，今天在寫代碼，順便總結一下：

- 最原始的字符串連接方式：str1 + str2
- python 新字符串連接語法：str1, str2
- 奇怪的字符串方式：str1 str2
- % 連接字符串：‘name:%s; sex: ’ % ('tom', 'male')
- 字符串列表連接：str.join(some_list)

###第一種，想必只要是有編程經驗的人，估計都知道，直接用 “+” 來連接兩個字符串：

```py
'Jim' + 'Green' = 'JimGreen'
```


###第二種比較特殊，如果兩個字符串用“逗號”隔開，那麼這兩個字符串將被連接，但是，字符串之間會多出一個空格：
```py
'Jim', 'Green' = 'Jim Green'
```
###第三種也是 python 獨有的，只要把兩個字符串放在一起，中間有空白或者沒有空白：兩個字符串自動連接為一個字符串：

```py
'Jim''Green' = 'JimGreen'
'Jim'  'Green' = 'JimGreen'
```

###第四種功能比較強大，借鑑了C語言中 printf 函數的功能，如果你有C語言基礎，看下文檔就知道了。這種方式用符號“%”連接一個字符串和一組變量，字符串中的特殊標記會被自動用右邊變量組中的變量替換：
```py
'%s, %s' % ('Jim', 'Green') = 'Jim, Green'
```

###第五種就屬於技巧了，利用字符串的函數 join 。這個函數接受一個列表，然後用字符串依次連接列表中每一個元素：
```py
var_list = ['tom', 'david', 'john']
a = '###'
a.join(var_list) =  'tom###david###john'
```

其實，python 中還有一種字符串連接方式，不過用的不多，就是字符串乘法，如：
```py
a = 'abc'
a * 3 = 'abcabcabc'
```