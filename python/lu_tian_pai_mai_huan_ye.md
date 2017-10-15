# 露天拍賣 換頁

```py
#!/usr/bin/python
# -*- coding: utf-8 -*-
import re
import sys  
import requests 
from bs4 import BeautifulSoup

headers = {
    'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/32.0.1700.107 Safari/537.36',
}

cookies = {
    '_ts_id':  '999999999999999999', 
}
  
reload(sys)  
sys.setdefaultencoding('utf8')  

def PageCount(keyword) :
    h = requests.get('http://search.ruten.com.tw/search/s000.php?enc=u&searchfrom=indexbar&k='+keyword+'&t=0', headers=headers, cookies=cookies)
    soup = BeautifulSoup(h.text,'html.parser')
    pattern = re.compile(r'total"\:(\d+)\,\"perPage"\:(\d+)')
    script = soup.find("script", text=pattern)
    script = script.text.strip().split('\n')

    #print pattern.findall(script[1])[0][0]
    if int(pattern.findall(script[1])[0][0]) / int(pattern.findall(script[1])[0][1]) > 100:
        return 100
    else:
        return int(pattern.findall(script[1])[0][0]) / int(pattern.findall(script[1])[0][1])

def crawler(keyword):
    ALLpage = PageCount(keyword)
    print ALLpage

    for number  in range(ALLpage, 0, -1):
        #url = 'http://search.ruten.com.tw/search/s000.php?enc=u&searchfrom=indexbar&k='+keyword+'&t=0&p='+str(number)
        #print url
        #raw_input()
        h = requests.get('http://search.ruten.com.tw/search/s000.php?enc=u&searchfrom=indexbar&k='+keyword+'&t=0&p='+str(number), headers=headers, cookies=cookies)
        soup = BeautifulSoup(h.text,'html.parser')
        wb = h.text
        b = wb.encode('latin-1')
        u = b.decode('utf-8')
        print u

if __name__ == "__main__":  
    keyword = str(sys.argv[1])
    print keyword
    #raw_input()
    crawler(keyword)


```

```sh
python test.py 'Lowe Alpine Strike'
```