# Python + Requests 編碼問題


requests 是一款非常人性化的 python http 庫，但起對網頁的編碼識別卻一直很蛋疼，時常出現亂碼。

可以從官方的 API 文檔中找出亂碼的根本原因，http://docs.python-requests.org/en/latest/api/#requests.Response.text：

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

可見，requests 是通過 http header 猜測頁面編碼，`如果 header 中不存在 charset 就認為編碼為 ISO-8859-1`。這樣的話，對於返回頭中沒有指定頁面編碼的情況，自然就出現亂碼了，如：

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
            <a href="/impression">è¡~Lä¸~Zè§~Bç~B¹</a>                  //亂碼了
            Â· <a href="/lawer">æ³~Uå¾~Ké¡¾é~W®</a>
            Â· <a href="/contactus">è~A~Tç³»æ~H~Qä»¬</a>
            Â· <a href="/help">å¸®å~J©</a>
            Â· <a href="/about">å~E³äº~N</a>
        </span>
    </div>
···SNIP···
```

文檔中提供的解決辦法：

If you can take advantage of non-HTTP knowledge to make a better guess at the encoding, you should set r.encoding appropriately before accessing this property.
即在訪問 `r.text` 屬性`前`先設置下正確的編碼，如下：


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
            <a href="/impression">行業觀點</a>              //正常了
            · <a href="/lawer">法律顧問</a>
            · <a href="/contactus">聯繫我們</a>
            · <a href="/help">幫助</a>
            · <a href="/about">關於</a>
        </span>
    </div>
···SNIP···
```

requests 也可以從內容獲取編碼，通過 chardet 庫，只是默認沒有啟用:


```py
>>> r = requests.get("http://www.wooyun.org")
>>> r.close()
>>> r.encoding
'ISO-8859-1'
>>> r.apparent_encoding
'utf-8'
```

這樣一來，我們可以直接通過設置 `r.encoding` 為 `r.apparent_encoding`，來解決這個「bug」:

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
            <a href="/impression">行業觀點</a>              //正常了
            · <a href="/lawer">法律顧問</a>
            · <a href="/contactus">聯繫我們</a>
            · <a href="/help">幫助</a>
            · <a href="/about">關於</a>
        </span>
    </div>
···SNIP···
```

問題解決。

##References：
http://liguangming.com/python-requests-ge-encoding-from-headers