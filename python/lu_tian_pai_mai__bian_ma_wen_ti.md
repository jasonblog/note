# 露天拍賣 ＆ 編碼問題


```py
 #!/usr/bin/python
# -*- coding: utf-8 -*-

import sys  
import requests 

headers = {
    'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/32.0.1700.107 Safari/537.36',
}

cookies = {
    '_ts_id':  '999999999999999999', 
}
  
reload(sys)  
sys.setdefaultencoding('utf8')  

h = requests.get('http://class.ruten.com.tw/user/index00.php?s=nevereverfor', headers=headers, cookies=cookies)
wb = h.text

print type(wb)
b = wb.encode('latin-1')
print type(b)
u = b.decode('utf-8')
print type(u)
#raw_input()
print(u)
```

```py
 #!/usr/bin/python
# -*- coding: utf-8 -*-
import sys  
import requests 

headers = {
    'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/32.0.1700.107 Safari/537.36',
}

cookies = {
    '_ts_id':  '999999999999999999', 
}
  
reload(sys)  
print sys.getdefaultencoding()  

r = requests.get('http://class.ruten.com.tw/user/index00.php?s=nevereverfor', headers=headers, cookies=cookies)
r.close()

print r.headers 
print r.encoding
print r.apparent_encoding
r.encoding = r.apparent_encoding

print r.text
```


```py
>>> wb = u'\xe5\x8d\x97\xe4\xba\xac\xe5\xbe\xa9\xe8\x88\x88'
>>> b = wb.encode('latin-1')
>>> u = b.decode('utf-8')
>>> print(u)
南京復興
```