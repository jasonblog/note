# goodinfo 股票篩選


```py
# coding=UTF-8
import math
import requests
import re  #正規表示式
from bs4 import BeautifulSoup
# 下載網頁內容
header = {'user-agent':'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36'}

VAL_S = 8		#股本範圍起
VAL_E = 15		#股本範圍迄

url = 'https://goodinfo.tw/StockInfo/StockList.asp?MARKET_CAT=%E8%87%AA%E8%A8%82%E7%AF%A9%E9%81%B8&INDUSTRY_CAT=%E6%88%91%E7%9A%84%E6%A2%9D%E4%BB%B6&FILTER_ITEM0=%E8%82%A1%E6%9C%AC+%28%E5%84%84%E5%85%83%29&FILTER_VAL_S0=' + str(VAL_S) + '&FILTER_VAL_E0=' + str(VAL_E)
print(url)

rt = requests.get(url , headers = header)

# 確認是否下載成功
if rt.status_code == requests.codes.ok:
    # 以 BeautifulSoup 解析 HTML 程式碼
    rt.encoding = 'utf-8'
    soup = BeautifulSoup(rt.text, 'html.parser')
    print(soup.prettify())

    #<td style="mso-number-format:\@;"><nobr><a class="link_black" href="StockDetail.asp?STOCK_ID=1101"...
    data = soup.find_all('td', style='mso-number-format:\@;')

    #使用正規表示式找出所有四位數的股票代號
    m = re.findall('\d{4}', str(data))

    #set:去除重複，sorted:排序
    STOCK_ID = sorted(set(m))

    #print(len(STOCK_ID))
```
