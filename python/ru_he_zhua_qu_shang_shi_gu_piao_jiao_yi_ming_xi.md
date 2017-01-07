# 如何抓取上市股票交易明細


```py
# -*- coding: utf-8 -*-
from urllib import urlencode
import httplib2
import pandas as pd

url = 'http://www.tse.com.tw/ch/trading/exchange/STOCK_DAY/STOCK_DAYMAIN.php'
data = { 'download': 'html','query_year': '2017', 'query_month': '01',
'CO_ID': '2330'}
agent = 'Mozilla/5.0 (Windows NT 6.3; WOW64; rv:34.0) Gecko/20100101Firefox/34.0' 

httplib2.debuglevel = 1
conn = httplib2.Http('.cache')
headers = {'Content-type': 'application/x-www-form-urlencoded',
           'Accept':
'text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8',
           'User-Agent': agent}
resp, content = conn.request(url, 'POST', urlencode(data), headers)

df=pd.read_html(content)
df1=pd.DataFrame(df[0])

print df1
```

