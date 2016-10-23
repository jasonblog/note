# 立即停止使用 setdefaultencoding('utf-8')， 以及为什么



## 最坏实践

```py
import sys
reload(sys)
sys.setdefaultencoding('utf-8')
```

上面这种代码曾经（现在依然）是解决中文编码的万能钥匙。解决编码错误问题一劳永逸，从此和 `UnicodeEncodeError: 'ascii' codec can't encode characters in position 0-5: ordinal not in range(128) 说 byebye 。`

那么现在，对于那些想解决 UnicodeEncodeError 问题而搜索到这篇文章的读者，我要说的是，不要用以上的代码片段。下面我来说说为什么，以及我们应该怎么做。

###sys.setdefaultencoding('utf-8') 会导致的两个大问题

简单来说这么做将会使得一些代码行为变得怪异，而这怪异还不好修复，以一个不可见的 bug 存在着。下面我们举两个例子。


## 1. 编码错误

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

输出：

```py
$~ Ã¾
$~ þ
```

在上面的代码中，默认的 ascii 编码无法解码，Ã¾ latin-1 编码 hex 表示是 c3 be ，显然是超出了只有128个字符的 ascii 码集的，引发 UnicodeError 异常，进入异常处理。异常处理则会根据编码探测，用最可能的编码来解码，会比较靠谱地输出 Ã¾ 。
而一旦我们将 defaultencoding 设置为 utf-8，因为 utf-8 的字符范围是完全覆盖 latin-1，因此，会直接使用 utf-8 进行解码。c3 be 在 utf-8 中，是 þ。于是我们打印出了完全不同的字符。
可能你们会说我们不会写这样的代码。如果我们写了也会做修正。但如果是第三方库这么写了呢？项目依赖的第三方库就这么 bug 了。如果你不依赖第三方库，那么下面这个 bug，还是逃不过。


## 2. dictionray 行为异常

假设我们要从一个 dictionary 里查找一个 key 是否存在，通常来说，有两种可行方法。


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

我们对比下改变系统默认编码前后这俩函数的输出有什么不同。


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

输出：

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

可以看到，当默认编码改了之后，两个函数的输出不再一致。
dict 的 in 操作符将键做哈希，并比较哈希值判断是否相等。对于 ascii 集合内的字符来说，不管是字节字符类型还是还是 unicode 类型，其哈希值是一样的，如 u'1' in {'1':1} 会返回 True，而超出 ascii 码集的字符，如上例中的 '你好'，它的字节字符类型的哈希与 unicode 类型的哈希是不一样的。
而 == 操作符则是做了一次转换，将字节字符（byte string，上面的 '你好'）转换成 unicode（u'你好'） 类型，然后对转换后的结果做比较。在 ascii 系统默认编码中，'你好'转换成 Unicode 会产生 Warning: UnicodeWarning: Unicode equal comparison failed to convert both arguments to Unicode - interpreting them as being unequal，因为超出码集无法转换，系统会默认其不相等。当系统编码被我们手动改为 utf-8 后，这个禁忌则被解除，'你好' 能够顺利被转换成 unicode，最后的结果就是，in 和 == 行为不再一致。


## 问题的根源：Python2 中的 string

Python 为了让其语法看上去简洁好用，做了很多 tricky 的事情，混淆 byte string 和 text string 就是其中一例。

在 Python 里，有三大类 string 类型，unicode（text string），str（byte string，二进制数据），basestring，是前两者的父类。

其实，在语言设计领域，一串字节（sequences of bytes）是否应该当做字符串（string）一直是存在争议的。我们熟知的 Java 和 C# 投了反对票，而 Python 则站在了支持者的阵营里。其实我们在很多情况下，给文本做的操作，比如正则匹配、字符替换等，对于字节来说是用不着的。而 Python 认为字节就是字符，所以他们俩的操作集合是一致的。
然后进一步的，Python 会在必要的情况下，尝试对字节做自动类型转换，例如，在上文中的 ==，或者字节和文本拼接时。如果没有一个编码（encoding），两个不同类型之间的转换是无法进行的，于是，Python 需要一个默认编码。在 Python2 诞生的年代，ASCII 是最流行的（可以这么说吧），于是 Python2 选择了 ASCII。然而，众所周知，在需要需要转换的场景，ASCII 都是没用的（128个字符，够什么吃）。

在历经这么多年吐槽后，Python 3 终于学乖了。默认编码是 Unicode，这也就意味着，做所有需要转换的场合，都能正确并成功的转换。



## 最佳实践

说了这么多，如果不迁移到 Python 3，能怎么做呢？
有这么几个建议：

- 所有 text string 都应该是 unicode 类型，而不是 str，如果你在操作 text，而类型却是 str，那就是在制造 bug。

- 在需要转换的时候，显式转换。从字节解码成文本，用 var.decode(encoding)，从文本编码成字节，用 var.encode(encoding)。

- 从外部读取数据时，默认它是字节，然后 decode 成需要的文本；同样的，当需要向外部发送文本时，encode 成字节再发送。

##References

- UTF-8 codes
- LATIN-1 codes
- ASCII wiki
- Why sys.setdefaultencoding() will break code
- Getting unicode right in Python
