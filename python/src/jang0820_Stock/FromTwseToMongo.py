#將股票交易資料放進MongoDB資料庫，就不用需要每次從證交所讀取資料
import numpy as np
import requests
import pandas as pd
import datetime
import json
import matplotlib.pyplot as pp
import time
import pymongo
MONGO_HOST = 'localhost'
MONGO_DB = 'TwStock'
MONGO_COLLETION = 'twse'
from pymongo import MongoClient
#   http://www.twse.com.tw/exchangeReport/STOCK_DAY?date=20180817&stockNo=2330

def connect_mongo():  #連線資料庫
    global collection
    client = MongoClient(MONGO_HOST, 27017)
    db = client[MONGO_DB]
    collection = db[MONGO_COLLETION]

def get_stock_history(date, stock_no, retry = 5):   #從www.twse.com.tw讀取資料
    quotes = []
    url = 'http://www.twse.com.tw/exchangeReport/STOCK_DAY?date=%s&stockNo=%s' % ( date, stock_no)
    r = requests.get(url)
    data = r.json()
    return transform(data['data'])  #進行資料格式轉換

def transform_date(date):   #民國轉西元
        y, m, d = date.split('/')
        return str(int(y)+1911) + '/' + m  + '/' + d
    
def transform_data(data):   #將證交所獲得資料進行資料格式轉換
    data[0] = datetime.datetime.strptime(transform_date(data[0]), '%Y/%m/%d')
    data[1] = int(data[1].replace(',', ''))#把千進位的逗點去除
    data[2] = int(data[2].replace(',', ''))
    data[3] = float(data[3].replace(',', ''))
    data[4] = float(data[4].replace(',', ''))
    data[5] = float(data[5].replace(',', ''))
    data[6] = float(data[6].replace(',', ''))
    data[7] = float(0.0 if data[7].replace(',', '') == 'X0.00' else data[7].replace(',', ''))  # +/-/X表示漲/跌/不比價
    data[8] = int(data[8].replace(',', ''))
    return data

def transform(data):   #讀取每一個元素進行資料格式轉換，再產生新的串列
    return [transform_data(d) for d in data]

def genYM(smonth, syear, emonth, eyear):  #產生從syear年smonth月到eyear年emonth月的所有年與月的tuple
    start = 12 * syear + smonth
    end = 12 * eyear + emonth
    for num in range(int(start), int(end) + 1):
        y, m = divmod(num, 12)
        yield y, m

def fetch_data(year: int, month: int, stockno):  #擷取從year-month開始到目前為止的所有交易日資料
    raw_data = []
    today = datetime.datetime.today()
    for year, month in genYM(month, year, today.month, today.year): #產生year-month到今天的年與月份，用於查詢證交所股票資料
        if month < 10:
            date = str(year) + '0' + str(month) + '01'  #1到9月
        else:
            date = str(year) + str(month) + '01'   #10月
        data = get_stock_history(date, stockno)
        for item in data:  #取出每一天編號為stockno的股票資料
            if collection.find({    #找尋該交易資料是否不存在
                    "date": item[0],
                    "stockno": stockno
                } ).count() == 0:
                element={'date':item[0], 'stockno':stockno, 'shares':item[1], 'amount':item[2], 'open':item[3], 'close':item[4], 
                     'high':item[5], 'low':item[6], 'diff':item[7], 'turnover':item[8]};  #製作MongoDB的插入元素
                print(element)
                collection.insert_one(element)  #插入元素到MongoDB
        time.sleep(10)  #延遲5秒，證交所會根據IP進行流量統計，流量過大會斷線

connect_mongo()   #連線資料庫
fetch_data(2017, 4, '2892')   #取出編號2892的股票，從201704到今天的股價與成交量資料