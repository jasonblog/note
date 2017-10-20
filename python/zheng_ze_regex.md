# 正則 Regex


在我之前的一篇文章 – sed與正則表達式中介紹過「正則表達式」的相關內容，以及簡單的使用sed來處理文本的方法。

最近又拾起了自己喜愛的Python（雖然一直很渣，沒有好好學習，近來想做一點東西……：）），今天來扯扯「re模塊」的蛋（說實話，我是來虛心學習的！）。

##函數

我們可以先來看看re模塊中的常用函數有哪些：

```py
compile(pattern, flags=0)
```

re.compile用來將正則表達式轉換為一個「pattern object」，我們可以稱之為「模式對象」。將正則表達式轉換為模式對象的作用就是可以將其保存下來，已備後續之用。

比如：

```py
pat = re.compile(pattern)
result = pat.match(string)
```

等同與

```py
result = re.match(pattern, string)
```

若不用`re.compile`，每次需要用到這個正則表達式的時候都需要重新寫一遍正則。

```py
escape(pattern)
```

這個函數其實很簡單，在正則表達式之中會包含很多「保留字符」，要在正則表達式之中使用這些「保留字符」就需要對其進行轉義。如果去嘗試編寫一些比較複雜的正則表達式，那麼會發現要輸入一大堆反斜線，這其實是很讓人沮喪的一件事–就像編寫LISP程序的時候去看後面要寫多少個右括號一樣。

```py
re.escape('1234@gmail.com')
# result
1234\\@gmai\\.com
```

這確實是相當實用的一個函數。

```py
findall(pattern, string, flags=0)
```

該函數可以以列表的形式返回所有匹配模式的項：

```py
pat = '[a-zA-Z]+'
text = 'Hello, hm...this is Tom speaking, who are you?'
re.findall(pat, text)
# result
['Hello', 'hm', 'this', 'is', 'Tom', 'speaking', 'who', 'are', 'you']
```

這樣就可以找出text中所有匹配pat的項（例子中為單詞）。

```py
match(pattern, string, flags=0)
```

乍看之下，會覺得match函數和findall函數是一樣的作用，但其實不然。

```py
re.match('h', 'hello')
# result
<_sre.SRE_Match at 0x22af370>
```

match函數從字符串的開始進行匹配，匹配成功則返回一個MatchObject，否則返回None。

```py
search(pattern, string, flags=0)
```

search函數和match函數很相像，不同之處就是search函數並不是從字符串的開始處進行匹配，而是會查找整個字符串。

```py
re.match('e', 'hello')
# result
<_sre.SRE_Match at 0x2482440>
```

python.org上亦有這兩個函數的對比:match vs search，不妨一看。

```py
split(pattern, string, maxsplit=0, flags=0)
```

如同常見的字符串中的split相似，不過可以用正則表達式來split。

```py
text = 'one, two...ten')
re.split('[,. ]+', text)
# result
['one', 'two', 'three']
# use 'maxsplit'
re.split('[,. ]+', text, maxsplit=1)
# result
['one', 'two...ten']
```

maxsplit參數表示的是字符串最多可以分割的次數。

```py
sub(pattern, repl, string, count=0, flags=0)
```

sub函數很像sed：


```py
pat = '_name'
text = 'my name is _name'
re.sub(pat, 'xxx', text)
# result
'my name is xxx'
```

##匹配對象

事實上我們還會經常用到re模塊中的group函數，先來看一下它的用法：

```py
m = re.match(r'www\.(.+)\.com', 'www.google.com')
m.group(0)
# result
'www.google.com'

m.group(1)
# result
'google'
```

通過group我們可以選擇匹配到的字符串中「需要」的部分。

```py
m.group(0)
```

即整個匹配到的字符串。而

```py
m.group(1)
```

則是在()中的子字符串。相同的，如果有多個子字符串的匹配模式亦可用group(n)來取出。

與group函數相配套的還有start和end:

```py
m.start(0)
# return
0
m.end(0)
# return
14
```

前者返回的是m.group(0)的組的首個位置，而後者則是返回m.group(0)的組的末尾的index+1.

```py
m.span(0)
# return
(0, 14)
```

span函數則是start和end函數的結合體，返回的整個index的範圍。

當然，以上介紹的只是re模塊中的一小部分，更多的內容可以在使用的時候查閱re模塊的doc

–EOF–