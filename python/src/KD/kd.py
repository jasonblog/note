#!/usr/bin/env python3
# coding=UTF-8
import math
import time
import requests
import re
import os
from bs4 import BeautifulSoup
import pandas as pd

def line_nofity(token, kd_result):
    url = "https://notify-api.line.me/api/notify"
    headers = {"Authorization" : "Bearer "+ token}
    message = "【" + '月KD' + "】 \n" + kd_result
    payload = {"message" :  message}
    files = {"imageFile": open(os.getcwd() + "/kd.png", "rb")} 
    #requests.post(url ,headers = headers ,params=payload)
    requests.post(url ,headers = headers ,params=payload, files=files)
    #time.sleep(10)

# 下載網頁內容
header = {'user-agent':'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36'}

VAL_S = 8		#股本範圍起
VAL_E = 15		#股本範圍迄

url = 'https://goodinfo.tw/StockInfo/StockList.asp?MARKET_CAT=%E8%87%AA%E8%A8%82%E7%AF%A9%E9%81%B8&INDUSTRY_CAT=%E6%88%91%E7%9A%84%E6%A2%9D%E4%BB%B6&FILTER_ITEM0=K%E5%80%BC+%28%E6%9C%88%29&FILTER_VAL_S0=0&FILTER_VAL_E0=20&FILTER_ITEM1=D%E5%80%BC+%28%E6%9C%88%29&FILTER_VAL_S1=0&FILTER_VAL_E1=20&FILTER_ITEM2=%E9%80%A3%E7%BA%8C%E9%85%8D%E7%99%BC%E7%8F%BE%E9%87%91%E8%82%A1%E5%88%A9%E6%AC%A1%E6%95%B8&FILTER_VAL_S2=5&FILTER_VAL_E2=20&FILTER_ITEM3=---%E8%AB%8B%E9%81%B8%E6%93%87%E9%81%8E%E6%BF%BE%E6%A2%9D%E4%BB%B6---&FILTER_VAL_S3=&FILTER_VAL_E3=&FILTER_ITEM4=---%E8%AB%8B%E9%81%B8%E6%93%87%E9%81%8E%E6%BF%BE%E6%A2%9D%E4%BB%B6---&FILTER_VAL_S4=&FILTER_VAL_E4=&FILTER_ITEM5=---%E8%AB%8B%E9%81%B8%E6%93%87%E9%81%8E%E6%BF%BE%E6%A2%9D%E4%BB%B6---&FILTER_VAL_S5=&FILTER_VAL_E5=&FILTER_ITEM6=---%E8%AB%8B%E9%81%B8%E6%93%87%E9%81%8E%E6%BF%BE%E6%A2%9D%E4%BB%B6---&FILTER_VAL_S6=&FILTER_VAL_E6=&FILTER_ITEM7=---%E8%AB%8B%E9%81%B8%E6%93%87%E9%81%8E%E6%BF%BE%E6%A2%9D%E4%BB%B6---&FILTER_VAL_S7=&FILTER_VAL_E7=&FILTER_ITEM8=---%E8%AB%8B%E9%81%B8%E6%93%87%E9%81%8E%E6%BF%BE%E6%A2%9D%E4%BB%B6---&FILTER_VAL_S8=&FILTER_VAL_E8=&FILTER_ITEM9=---%E8%AB%8B%E9%81%B8%E6%93%87%E9%81%8E%E6%BF%BE%E6%A2%9D%E4%BB%B6---&FILTER_VAL_S9=&FILTER_VAL_E9=&FILTER_ITEM10=---%E8%AB%8B%E9%81%B8%E6%93%87%E9%81%8E%E6%BF%BE%E6%A2%9D%E4%BB%B6---&FILTER_VAL_S10=&FILTER_VAL_E10=&FILTER_ITEM11=---%E8%AB%8B%E9%81%B8%E6%93%87%E9%81%8E%E6%BF%BE%E6%A2%9D%E4%BB%B6---&FILTER_VAL_S11=&FILTER_VAL_E11=&FILTER_RULE0=---%E8%AB%8B%E6%8C%87%E5%AE%9A%E9%81%B8%E8%82%A1%E6%A2%9D%E4%BB%B6---&FILTER_RULE1=---%E8%AB%8B%E6%8C%87%E5%AE%9A%E9%81%B8%E8%82%A1%E6%A2%9D%E4%BB%B6---&FILTER_RULE2=---%E8%AB%8B%E6%8C%87%E5%AE%9A%E9%81%B8%E8%82%A1%E6%A2%9D%E4%BB%B6---&FILTER_RULE3=---%E8%AB%8B%E6%8C%87%E5%AE%9A%E9%81%B8%E8%82%A1%E6%A2%9D%E4%BB%B6---&FILTER_RULE4=---%E8%AB%8B%E6%8C%87%E5%AE%9A%E9%81%B8%E8%82%A1%E6%A2%9D%E4%BB%B6---&FILTER_RULE5=---%E8%AB%8B%E6%8C%87%E5%AE%9A%E9%81%B8%E8%82%A1%E6%A2%9D%E4%BB%B6---&FILTER_RANK0=---%E8%AB%8B%E6%8C%87%E5%AE%9A%E6%8E%92%E5%90%8D%E6%A2%9D%E4%BB%B6---&FILTER_RANK1=---%E8%AB%8B%E6%8C%87%E5%AE%9A%E6%8E%92%E5%90%8D%E6%A2%9D%E4%BB%B6---&FILTER_RANK2=---%E8%AB%8B%E6%8C%87%E5%AE%9A%E6%8E%92%E5%90%8D%E6%A2%9D%E4%BB%B6---&FILTER_SHEET=KD%E6%8C%87%E6%A8%99&FILTER_SHEET2=%E6%97%A5%2F%E9%80%B1%2F%E6%9C%88&FILTER_MARKET=%E4%B8%8A%E5%B8%82%2F%E4%B8%8A%E6%AB%83&FILTER_QUERY=%E6%9F%A5++%E8%A9%A2'
print(url)

rt = requests.get(url , headers = header)

# 確認是否下載成功
if rt.status_code == requests.codes.ok:
# 以 BeautifulSoup 解析 HTML 程式碼
    rt.encoding = 'utf-8'
    #soup = BeautifulSoup(rt.text, 'html.parser')
    soup = BeautifulSoup(rt.text, "lxml")
    df = pd.read_html(str(soup))
    df = df[17] # Jupyter 測試出來是第17是我們要的 table 
    df = df.sum(level=1,axis=1) ## multiindex 問題才加
    df = df[1:] # 只要一列之後
    df = df[df['代號'].apply(lambda x: str(x).isdigit())] # 刪除非數字的列
    df = df.reset_index(col_level=1, drop=True) # 因為 drop 後需 index 重新reset 

    df['成交'] = df['成交'].astype('float') # 把 str 轉成 float 型態才能sort
    df = df.sort_values(by='成交', ascending=1)
    df = df.reset_index(col_level=1, drop=True) # 因為 sort 後需 index 重新reset 

    print(df)

    kd_result = ""
    for index, row in df.iterrows():
        k = float(re.findall(r"\d+\.?\d*", row['K值(月)'])[0])
        d = float(re.findall(r"\d+\.?\d*", row['D值(月)'])[0])
        if k > d:
            print('%s %s : K:%f, D:%f' % (row['代號'], row['名稱'], k, d))
            buf = '%s %s : K:%f, D:%f \n' % (row['代號'], row['名稱'], k, d)
            kd_result = kd_result + buf


    df.to_html('goodinfo.html')

    token = "KXwzqEGtIp1JEkS5GjqXqRAT0D4BdQQvCNcqOa7ySfz" # Jason
    line_nofity(token, kd_result)


'''
MARKET_CAT: 自訂篩選
INDUSTRY_CAT: 我的條件
FILTER_ITEM0: K值 (月)
FILTER_VAL_S0: 0
FILTER_VAL_E0: 20
FILTER_ITEM1: D值 (月)
FILTER_VAL_S1: 0
FILTER_VAL_E1: 20
FILTER_ITEM2: 連續配發現金股利次數
FILTER_VAL_S2: 5
FILTER_VAL_E2: 20
FILTER_ITEM3: 累季–EPS(元)
FILTER_VAL_S3: 2
FILTER_VAL_E3: 5
FILTER_ITEM4: ---請選擇過濾條件---
FILTER_VAL_S4:
FILTER_VAL_E4:
FILTER_ITEM5: ---請選擇過濾條件---
FILTER_VAL_S5:
FILTER_VAL_E5:
FILTER_ITEM6: ---請選擇過濾條件---
FILTER_VAL_S6:
FILTER_VAL_E6:
FILTER_ITEM7: ---請選擇過濾條件---
FILTER_VAL_S7:
FILTER_VAL_E7:
FILTER_ITEM8: ---請選擇過濾條件---
FILTER_VAL_S8:
FILTER_VAL_E8:
FILTER_RULE0: ---請指定選股條件---
FILTER_RULE1: ---請指定選股條件---
FILTER_RULE2: ---請指定選股條件---
FILTER_RULE3: ---請指定選股條件---
FILTER_RULE4: ---請指定選股條件---
FILTER_RULE5: ---請指定選股條件---
FILTER_RANK0: ---請指定排名條件---
FILTER_RANK1: ---請指定排名條件---
FILTER_RANK2: ---請指定排名條件---
FILTER_SHEET: 年獲利能力
FILTER_SHEET2: 獲利能力
FILTER_MARKET: 上市/上櫃
FILTER_QUERY: 查  詢
'''

