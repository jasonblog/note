# 使用Pandas 函式將臺灣銀行的牌告匯率存進資料庫中


```py
#!/usr/bin/python
# -*- coding: utf-8 -*-
import sqlite3
import pandas
from datetime import datetime

dfs = pandas.read_html('http://rate.bot.com.tw/xrt?Lang=zh-TW')
currency = dfs[0]
currency = currency.ix[:,0:5]
currency.columns = [u'幣別', u'現金匯率-本行買入', u'現金匯率-本行賣出', u'即期匯率-本行買入', u'即期匯率-本行賣出']
currency[u'幣別'] = currency[u'幣別'].str.extract('\((\w+)\)', expand=True)
currency['Date'] = datetime.now().strftime('%Y-%m-%d')
currency['Date'] = pandas.to_datetime(currency['Date'])

with sqlite3.connect('currency.sqlite') as db:
    currency.to_sql('currency', con = db, if_exists='append')

with sqlite3.connect('currency.sqlite') as db:
    df = pandas.read_sql_query('select * from currency', con = db)

print df
```