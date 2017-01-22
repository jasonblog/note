# goodinfo


```py
#! /usr/bin/python
# -*- coding: utf-8 -*-
import re
import urllib2
import csv
import pandas as pd

def GetHtmlcode(ID):
    # Get the webpage's source html code
    source = 'http://goodinfo.tw/StockInfo/StockDetail.asp?STOCK_ID='
    url = source + ID
    #print url

    # Header
    headers = { 'User-Agent' : 'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/31.0.1650.63 Safari/537.36',
                'Accept' : 'text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8',
                'Accept-Charset' : 'Big5,utf-8;q=0.7,*;q=0.3',
                #'Accept-Encoding' : 'gzip,deflate,sdch',
                'Accept-Language' : 'zh-TW,zh;q=0.8,en-US;q=0.6,en;q=0.4,ja;q=0.2' ,
                'Cache-Control' : 'max-age=0',
                'Connection' : 'keep-alive',
                'Cookie' : '427 bytes were stripped',
                'Host' : 'www.goodinfo.tw',
                'Referer' : url }

    # 連到網頁抓取資料
    req= urllib2.Request(url,"",headers)
    response = urllib2.urlopen(req)
    result = response.read().decode('utf-8')
    #print result
    return result

def main():
    page = GetHtmlcode('2103')
    df=pd.read_html(page)
    df2=pd.DataFrame(df[41])
    df2 = df2[2:] ## 只要第二列之後
    df2.columns = df2.iloc[0] # 把第一列當作 columns 名稱
    df2 = df2.drop(df2.index[0]) # 刪除第一列 
    df2.columns.name = ''
    df2.index = range(len(df2)) # 重新產生 index 
    print df2

if __name__ == "__main__":
    main()
```