# pandas create dataframe example


```py
#! /usr/bin/python
# -*- coding: utf-8 -*-
import numpy as np
import pandas as pd
import sys

reload(sys)
sys.setdefaultencoding('utf-8')

HEADER = '''
<html>
    <head>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
    </head>
    <body>
'''
FOOTER = '''
    </body>
</html>
'''

labels = ['年度',
          '股本',
          '營收(億)',
          '稅後淨利(億)',
          '毛利(%)',
          '營益(%)',
          'ROE',
          'EPS',
          '獲利矩陣',
          '現金',
          '股票',
          '股利合計',
          '高低價差',
          '最高本益比',
          '日期',
          '最低本益比',
          '日期',
          '收盤平均價',
          '平均本益比']

w, h = 19, 10
raw_data = [['-' for x in range(w)] for y in range(h)] 

df = pd.DataFrame(raw_data, columns = labels)

print df.index
print(df.columns) 
print(df.values)
print(df.describe())

df['年度'] = ['2016','2015', '2014', '2013', '2012', '2011', '2010', '2009', '2008', '2007']
print(df['年度']) 
print(type(df['年度'])) 

with open('test.html', 'w') as f:
    f.write(HEADER)
    f.write(df.to_html(classes='df'))
    f.write(FOOTER)
```