# Python技巧(漂亮又通順的程式碼)


##Looping over a range of numbers

你有可能這麼寫:

```py
for i in [0, 1, ,2, 3, 4, 5]:
    print i**2
```

比較有效率的寫法, 使用range():

```py
for i in range(6):
    print i**2
```

由於range()會傳回list, 所以當數量很大時, 最好使用xrange(), 以減少記憶體的使用量(xrange在Python 3之後已經拿掉, 原來的range改成傳回疊代).

```py
for i in xrange(6):
    print i**2
```

##Looping over a collection

當要依序取得一個list內項目的值時:

```py
colors = ['red', 'green', 'blue', 'yellow']
```

用C語言的思維, 你可能會寫這樣:

```c
for i in range(len(colors)):
    print(colors[i])
```

然而, 在Python語言, 可以直接用for迴圈來跑過list的項目:

```py
for color in colors:
    print(color)
```

##Looping backwards

上例是依正順序取得list的項目, 但如果要反向順序來取得的話呢? 
在C語言的思維, 可能會寫這樣:

```c
for i in range(len(colors), -1, -1):
    print(colors[i])
```

在Python中, 直接用reversed就可以得到反順序的list:

```py
for color in reversed(colors):
    print(color)
```

##Looping over a collection and indices

當有一個list, 我們要依序得到項目的次序以及值的時候, 我們可能會寫這樣:

```py
for i in range(len(colors)):
    print(i, '-->', colors[i])
```

其實在Python中, 我們可以使用enumerate(), 它會傳回一個次序及值成組tuple的疊代:

```py
for i, color in enumerate(colors):
    print(i, '-->', color)
```


##Looping over two collections

當我們要對兩個list依序對其值做對應的時候, 用C語言的思維, 我們可能會寫這樣:

```py
names = ['raymond', 'rachel', 'matthew']
colors = ['red', 'green', 'blue', 'yellow']

n = min(len(names), len(colors))
for i in range(n):
    print(names[i], '-->', colors[i])
```

而Python中, 我們使用zip()就可以得到想要的結果:

```py
for name, color in zip(names, colors):
    print(name, '-->', color)
```

然而, 用zip()會傳回一個新的list, 也就是會需要使用額外的記憶體. 為此, 我們可以使用izip():

```py
from itertools import izip
for name, color in izip(names, colors):
    print(name, '-->', color)
```

##Looping in sorted order

在Python中, 我們很輕易就可以得到一個排序過的list: 使用sorted(). sorted()會傳回一個新的list, 並不會改動到原來的list.

```py
colors = ['red', 'green', 'blue', 'yellow']
for color in sorted(colors):
    print(color)
```

要反向的排序:

```py
for color in sorted(colors, reverse=True):
    print(color)
```

##Custom sort oder

承上例, 如果要做自訂的排序規則, 例如用字串的長度來排序, 在傳統Python的作法, 我們可能會這樣寫:

```py
colors = ['red', 'green', 'blue', 'yellow']

def compare_length(c1, c2):
    if len(c1) < len(c2): return -1
    if len(c1) > len(c2): return 1
    return 0
print(sorted(colors, cmp=compare_length)
```

這樣的寫法, 我們要寫一個比較用的function, 它會將list的值兩兩比對, 回傳大於0, 小於0, 或等於0的值讓sorted()來做排序.

其實, 還有一個更簡便的方式:

```py
print(sorted(colors, key=len))
```

使用key, 它會將list的項目用key的函式來計算出值(在此例是len()), 再依此值來排序. 又快又簡單.

cmp在Python 3已經被拿掉, 請用key這個又快又簡單的方式.

##Call a function until a sentinel value

一直呼叫一個函式(function)直到遇到某個停止的值. 這個情況常發生在讀取檔案的時候. 如:

blocks = []
while True:
    block = f.read(32)
    if block = '':
        break
    blocks.append(block)
我們可以利用iter()以及partial()來達成, 程式碼會變得簡潔.

from functools import partial
blocks = []
for block in iter(partial(f.read, 32), ''):
    blocks.append(block)
在這個例子中, iter()會一直呼叫f.read(32), 並將回傳值産生疊代, 直到回傳值是空字串”.

iter(callback, sentinel)第一個參數是function, 它會一直呼叫這個function, 將function的回傳值變成疊代, 直到function回傳值與sentinel相等.

partial(func, *args, **keywords)産生一個新的function, 而這個function是將參數args或keywords代入func的結果. 參數可以是func所需要的部份參數.

舉例如下:

from functools import partial

def add(a, b):
    return a+b

add_by_2 = partial(add, b=2)
print(add_by_2(3))
上述的add_by_2是一個partial()新産生的function, 它利用add(a, b)而將b的值固定為2. 所以呼叫add_by_2(3)會得到5.

Distinguishing multiple exit points in loop

在for迴圈中達成某條件就跳出迴圈, 例如用for迴圈找list是否有某項值, 找到回傳1, 沒找到回傳-1.

一般情況會寫這樣:

def find(seq, target):
    found = False
    for i, value in enumerate(seq):
        if value == target:
            found = True
            break
    if not found:
        return -1
    return i
Python的for迴圈其實可以搭配else使用, 其實應該稱作no-break. 與for使用的else表示如果for迴圈被完整執行, 那就執行else的部份. 如果迴圈被中斷了(break), 那else的部份也不會被執行.

所以上述的程式可以改寫成:

def find(seq, target):
    for i, value in enumerate(seq):
        if value == target:
            break
    else:
        return -1
    return i
Looping over dictionary keys

在dictionary中, 我們要得到key值, 可以用下例的方式:

d = {'matthew': 'blue', 'rachel': 'green', 'raymond': 'red'}
for k in d:
    print(k)
然而, 如果會變動到dictionary (如刪除某些dictionary的項目), 應該使用d.keys(), 這個函式會産生一個由key組成的list.

for k in d.keys():
    if k.startswith('r'):
        del d[k]
當然也可以用下例的程式得到相同的結果.

d = {k:d[k] for k in d if not k.startswith('r')}
這行程式的意思是, d重設為一個新的dictionary, 而這個新的dictionary內容是k:d[k]的組合, 而k是d的key但不是’r’開頭的.

Looping over a dictionary keys and values

承上例, 要得到dictionary的key值可以用如下的程式碼:

for k in d:
    print(k)
如果要連同value一起取得, 可以用d.items(), 這會産生一個由(key, value)的tuple組成的list.

for k, v in d.items():
    print(k, '-->', v)
相同的, 産生一個新的list會佔用額外的記憶體, 所以如果只是要疊代, 可以用d.iteritems():

for k, v in d.iteritems():
    print(k, '-->', v)
Construct a dictionary from pairs

如果我們有兩個lists, 想要把它們變成key:value的dictionary, 我們可以這樣寫:

names = ['raymond', 'rachel', 'matthew']
colors = ['red', 'green', 'blue']

from itertools import izip
d = dict(izip(names, colors))

{'raymond':'red', 'rachel': 'green', 'matthew':'blue'}

d = dict(enumerate(names))
{0:'raymond', 1:'rachel', 2:'matthew'}
Counting with dictionaries

假設我們有一個list, 裡面有很多項目, 而我們要計數每個項目重複的數量有多少, 我們會這樣寫:

colors = ['red', 'green', 'red', 'blue', 'green', 'red']
d = {}
for color in colors:
    if not color in d:
        d[color] = 0
    d[color] += 1

{'blue': 1, 'green': 2, 'red': 3}
這個程式的意思是, 先設d為一個空的dictionary, 用for迴圈跑過所有colors的項目, 如果color還沒有在d的key當中, 就把d[color]設為0, 如果有color這個key, 就把d[color]加1.

上述的程式是要先確認color是否存在d的key當中, 如果沒有, 就直接用d[color]來讀取會發生錯誤例外.

我們也可以改用d.get()來得到相同的結果: 
d = {} 
for color in colors: 
d[color] = d.get(color, 0) + 1

d.get()的第二個參數是預設值, 也就是如果color這個key找不到的話, 就回傳預設值.

另外, 也可以用defaultdict來實作:

from collections import defaultdict
d = defaultdict(int)
for color in colors:
    d[color] += 1
defaultdict(default_factory)會産生一個以default_factory為基本的dictionary, 也就是key不需要存在這個dictionary當中, 它的d[key]值便會是default_factory這個type的預設值(int為0).

Grouping with dictionary

假設我們需要將list的項目, 依照它們的長度來群組起來, 我們可以這樣寫:

names = ['raymond', 'rachel', 'matthew', 'roger', 'betty', 'melissa', 'judith', 'charlie']
d = {}
for name in names:
    key = len(name)
    if not key in d:
        d[key] = []
    d[key].append(name)

{5:['roger', 'betty'], 6:['rachel', 'judith'], 7:['raymond', 'matthew', 'melissa', 'charlie']}
這段程式的意思是, 設d為一個空的dictionary, 用for跑過所有的name, 把key設為name的長度. 如果key不在d的keys中, 就把d[key]設為一個空的list, 然後把name加到d[key]這個list中.

更好的方式:

d = {}
for name in names:
    key = len(name)
    d.setdefault(key, []).append(name)
d.setdefault(key, [])就類似是d.get(key, []), 但跟get()不同的是, setdefault()如果找不到key, 就將d[key]設為預設值(此例是[]), 而get()是找不到key,就回傳預設值.

當然, 也可以用之前提過的defaultdict來實作:

from collections import defaultdict
d = defaultdict(list)
for name in names:
    key = len(name)
    d[key].append(name)
Is a dictionary popitem() atomic?

d.popitem()會刪掉(key, value)並回傳該tuple的函式. 用法如:

d = {'matthew': 'blue', 'rachel': 'green', 'raymond': 'red'}
while d:
    key, value = d.popitem()
    print(key, '-->', value)
而d.popitem()是atomic, 所以在thread的使用中, 不需要在前後做lock.

Linking dictionaries

假設我們有一堆的設定值存在dictionary中, 有一些是程式預設, 有一些在環境設定裡, 有一些是使用者輸入來設定. 我們作法可能如:

defaults = {'color': 'red', 'user': 'guest'}
parser = argparse.ArgumentParser()
parser.add_argument('-u', '--user')
parser.add_argument('-c', '--color')
namespace = parser.parse_args([])
command_line_args = {k:v for k, v in vars(namespace).items() if v}

d = defaults.copy()
d.update(os.environ)
d.update(command_line_args)
但是如果dictionary的key/value數量很多的話, copy()/update()會非常沒有效率而緩慢.

在Python 3.3加入了ChainMap來達成這個目的.

from collections import ChainMap
d = ChainMap(command_line_args, os.environ, defaults)