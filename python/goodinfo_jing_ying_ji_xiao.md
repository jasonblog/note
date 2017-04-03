# goodinfo 經營績效


```py
#! /usr/bin/python
# -*- coding: utf-8 -*-
import requests
import csv
import sys
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
    #'SHEET': '股利統計'}
    #'SHEET': '獲利指標'}
    'SHEET': 'PER/PBR'}

    res = requests.post('http://goodinfo.tw/StockInfo/StockBzPerformance.asp?', headers=headers, verify=False , data = payload)
    res.encoding = 'utf-8'
    soup = BeautifulSoup(res.text.replace('&nbsp;', '').replace('　',''), 'lxml')
    [s.extract() for s in soup('thead')]  # remove thead
    return soup

def main():
    # python2
    reload(sys)
    sys.setdefaultencoding('utf-8')

    page = GetHtmlcode('1301')
    df = pd.read_html(str(page))[2]
    #df = df[2:] ## 只要第二列之後
    print df

if __name__ == "__main__":
    main()
```