# Python + Requests 编码问题


requests 是一款非常人性化的 python http 库，但起对网页的编码识别却一直很蛋疼，时常出现乱码。

可以从官方的 API 文档中找出乱码的根本原因，http://docs.python-requests.org/en/latest/api/#requests.Response.text：

class requests.Response
The Response object, which contains a server’s response to an HTTP request.
···SNIP···
text
Content of the response, in unicode.
If Response.encoding is None, encoding will be guessed using chardet.
The encoding of the response content is determined based solely on HTTP head
ers, following RFC 2616 to the letter. If you can take advantage of non-HTTP
knowledge to make a better guess at the encoding, you should set r.encoding
appropriately before accessing this property.
···SNIP···

可见，requests 是通过 http header 猜测页面编码，`如果 header 中不存在 charset 就认为编码为 ISO-8859-1`。这样的话，对于返回头中没有指定页面编码的情况，自然就出现乱码了，如：

```py
>>> import requests
>>> r = requests.get("http://www.wooyun.org")
>>> r.close()
>>> r.headers
CaseInsensitiveDict({'via': '10.67.15.68', 'content-encoding': 'gzip', 'transfer
-encoding': 'chunked', 'set-cookie': 'saeut=123.138.79.30.1403078425607937; 
path=/; max-age=311040000, PHPSESSID=ead28aadbfdaa8aa3cc2d7c9bf184f67; path=/; 
HttpOnly', 'expires': 'Thu, 19 Nov 1981 08:52:00 GMT', 'server': 'nginx/1.4.4', 
'connection': 'keep-alive', 'pragma': 'no-cache', 'cache-control': 'no-store, 
no-cache, must-revalidate, post-check=0, pre-check=0', 'date': 'Wed, 18 Jun 2014
08:00:25 GMT', 'x-powered-by': 'PHP/5.3.27', 'content-type': 'text/html'})
>>> r.encoding
'ISO-8859-1'
>>> print r.text
···SNIP···
        <span class="other fright">
            <a href="/impression">è¡~Lä¸~Zè§~Bç~B¹</a>                  //乱码了
            Â· <a href="/lawer">æ³~Uå¾~Ké¡¾é~W®</a>
            Â· <a href="/contactus">è~A~Tç³»æ~H~Qä»¬</a>
            Â· <a href="/help">å¸®å~J©</a>
            Â· <a href="/about">å~E³äº~N</a>
        </span>
    </div>
···SNIP···
```

文档中提供的解决办法：

If you can take advantage of non-HTTP knowledge to make a better guess at the encoding, you should set r.encoding appropriately before accessing this property.
即在访问 `r.text` 属性`前`先设置下正确的编码，如下：


```py
>>> import requests
>>> r = requests.get("http://www.wooyun.org")
>>> r.close()
>>> r.headers
CaseInsensitiveDict({'via': '10.67.15.68', 'content-encoding': 'gzip', 'transfer
-encoding': 'chunked', 'set-cookie': 'saeut=123.138.79.30.1403078425607937; path
=/; max-age=311040000, PHPSESSID=ead28aadbfdaa8aa3cc2d7c9bf184f67; path=/; Http
Only', 'expires': 'Thu, 19 Nov 1981 08:52:00 GMT', 'server': 'nginx/1.4.4', 'con
nection': 'keep-alive', 'pragma': 'no-cache', 'cache-control': 'no-store, no-cac
he, must-revalidate, post-check=0, pre-check=0', 'date': 'Wed, 18 Jun 2014 08:00
:25 GMT', 'x-powered-by': 'PHP/5.3.27', 'content-type': 'text/html'})
>>> r.encoding = "utf-8"
>>> print r.text
···SNIP···
        <span class="other fright">
            <a href="/impression">行业观点</a>              //正常了
            · <a href="/lawer">法律顾问</a>
            · <a href="/contactus">联系我们</a>
            · <a href="/help">帮助</a>
            · <a href="/about">关于</a>
        </span>
    </div>
···SNIP···
```

requests 也可以从内容获取编码，通过 chardet 库，只是默认没有启用:


```py
>>> r = requests.get("http://www.wooyun.org")
>>> r.close()
>>> r.encoding
'ISO-8859-1'
>>> r.apparent_encoding
'utf-8'
```

这样一来，我们可以直接通过设置 `r.encoding` 为 `r.apparent_encoding`，来解决这个「bug」:

- r.encoding =  r.apparent_encoding

```py
>>> r = requests.get("http://www.wooyun.org")
>>> r.close()
>>> r.encoding
'ISO-8859-1'
>>> r.apparent_encoding
'utf-8'
>>> r.encoding =  r.apparent_encoding
>>> print r.text
···SNIP···
        <span class="other fright">
            <a href="/impression">行业观点</a>              //正常了
            · <a href="/lawer">法律顾问</a>
            · <a href="/contactus">联系我们</a>
            · <a href="/help">帮助</a>
            · <a href="/about">关于</a>
        </span>
    </div>
···SNIP···
```

问题解决。

##References：
http://liguangming.com/python-requests-ge-encoding-from-headers