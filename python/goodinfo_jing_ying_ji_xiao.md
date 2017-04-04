# goodinfo 經營績效


```py
#! /usr/bin/python
# -*- coding: utf-8 -*-
import requests
import sys
import os
from bs4 import BeautifulSoup
import pandas as pd

def GetHtmlcode(ID):
    # Get the webpage's source html code
    source = 'http://goodinfo.tw/StockInfo/StockBzPerformance.asp?STOCK_ID='
    url = source + ID
    print url

    # Header
    headers = { 'User-Agent' : 'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/31.0.1650.63 Safari/537.36',
                'Accept' : 'text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8',
                'Accept-Charset' : 'Big5,utf-8;q=0.7,*;q=0.3',
                #'Accept-Encoding' : 'gzip,deflate,sdch',
                'Accept-Language' : 'zh-TW,zh;q=0.8,en-US;q=0.6,en;q=0.4,ja;q=0.2' ,
                'Cache-Control' : 'max-age=0',
                'Connection' : 'keep-alive',
                'Cookie' : '__gads=ID=a9b4db3df9875a51:T=1491062185:S=ALNI_MahBgxXa6B0VuRdoVYVNbFF3PYmbQ; GOOD%5FINFO%5FSTOCK%5FBROWSE%5FLIST=2%7C1301%7C2311; ASPSESSIONIDSCADSSBR=BJJLMMCAPPLDLCHKCNDJIBCG; ASPSESSIONIDQCCCQSCR=CGPPIMCAPHAOLODAHJLPKAIG; ASPSESSIONIDSAACQRDR=LMENOMCAGIHGEEIDJNCJDKJL; ASPSESSIONIDSABARTCR=DDFPGMCALMLHHEJPNJDIBCGC; _ga=GA1.2.1896088280.1491062185; _gat=1; SCREEN_SIZE=WIDTH=1920&HEIGHT=1080',
                'Host' : 'www.goodinfo.tw',
                'Referer' : url }

    # 'PER/PBR' '獲利指標' '股利統計'
    payload = {
    'STOCK_ID':ID,
    'YEAR_PERIOD':'9999',
    'RPT_CAT':'M_YEAR',
    'STEP':'DATA',
    'SHEET': '股利統計'}
    #'SHEET': '獲利指標'}
    #'SHEET': 'PER/PBR'}

    SHEETS = ['獲利指標','年增統計','股利統計', 'PER/PBR']
    columns = {'獲利指標' : [u'年度', u'股本(億)', u'財報評分', u'股價收盤', u'股價平均', u'股價漲跌', u'股價漲跌(%)', u'營業收入(億)', u'營業毛利(億)',
			     u'營業利益(億)', u'業外損益(億)', u'稅後淨利(億)',u'營業收入(%)', u'營業毛利(%)',u'營業利益(%)', u'業外損益(%)', u'ROE(%)', 
                             u'ROA(%)', u'稅後EPS', u'成長(元)', u'BPS(元)'],
			     
               '年增統計' : [u'年度', u'營業收入金額(億)',  u'營業收入增減(億)',    u'營業收入增減(%)', u'營業毛利金額(億)', u'營業毛利增減(億)',u'營業毛利增減(%)',    
                             u'毛利(%)', u'毛利增減數(億)', u'稅後淨利金額(億)',  u'稅後淨利增減(億)', u'稅後淨利增減(%)',    u'稅後淨利(%)', u'稅後增減數(億)', u'EPS(元)',  
                             u'每股盈餘增減(元)', u'ROE(%)',  u'ROE增減數', u'ROA(%)', u'ROA增減數'],


	       '股利統計' : [u'年度', u'股本(億)', u'財報評分', u'股價最高', u'股價最低', u'股價收盤', u'股價平均', u'股價漲跌', u'股價漲跌(%)', u'去年EPS(元)', u'股利現金',
			     u'股利股票', u'股利合計', u'殖利率最高', u'殖利率最低', u'殖利率平均', u'盈餘分配率配息', u'盈餘分配率配股', u'盈餘分配率合計'],

	       'PER/PBR' : [u'年度', u'股本(億)', u'財報評分', u'股價最高', u'股價最低', u'股價收盤', u'股價平均', u'股價漲跌', u'股價漲跌(%)', u'EPS(元)', u'最高PER',
			     u'最低PER', u'平均PER', u'BPS(元)', u'最高PBR', u'最低PBR', u'平均PBR']}

    for key in SHEETS:
        payload['SHEET'] = key
        res = requests.post('http://goodinfo.tw/StockInfo/StockBzPerformance.asp?', headers=headers, verify=False , data = payload)
        res.encoding = 'utf-8'
        soup = BeautifulSoup(res.text.replace('&nbsp;', '').replace('　',''), 'lxml')
        [s.extract() for s in soup('thead')]  # remove thead

        df = pd.read_html(str(soup))[2]
        df.columns = columns[key]
        print df

        key = key.replace('/', '_')

	if not os.path.exists(ID):
	    os.makedirs(ID)

        key = str(ID) + '/' + key +'.html' 
        #print key
        #raw_input()
        df.to_html(str(key))

    return soup

def main():
    # python2
    reload(sys)
    sys.setdefaultencoding('utf-8')

    fin = open('StockCode', 'r+')
    StockCodeList = [str(i)for i in fin.read().splitlines()]
    fin.close()

    for ID in StockCodeList:
        page = GetHtmlcode(ID)


if __name__ == "__main__":
    main()
```


```sh
1101
1102
1216
1301
1303
1326
1402
1476
2002
2105
2207
2301
2303
2308
2311
2317
2324
2325
2330
2354
2357
2382
2395
2408
2409
2412
2454
2474
2801
2912
3008
3045
3481
4904
4938
6505
9904
1227
1232
1319
1434
1477
1504
1536
1537
1707
1717
1722
1909
2015
2101
2103
2106
2201
2204
2227
2313
2345
2347
2355
2356
2360
2376
2377
2379
2383
2385
2441
2449
2451
2501
2542
2606
2615
2903
2915
3010
3030
3034
3044
3189
3213
3231
3702
3706
5434
5471
5522
6115
6146
6176
6189
6192
6206
6224
6239
6269
6277
6279
6285
6286
8050
8255
8299
9907
9910
9914
9917
9921
9925
9930
9933
9938
9941
9942
9945
```
