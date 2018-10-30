import pandas as pd
import datetime
import matplotlib.pyplot as pp
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

connect_mongo()  #呼叫連線資料庫函式
query = { 'stockno' : '2892' }
cursor = collection.find(query)  #依query查詢資料
stock =  pd.DataFrame(list(cursor))  #轉換成DataFrame
del stock['_id']  #刪除欄位_id
print(stock)
indexlist = []
for i in range(len(stock)):  #建立日期串列
    indexlist.append(stock['date'][i])  #stock['date'][i]為datetime.date物件
stock.index = indexlist  #索引值改成日期
stock = stock.drop(['date'],axis = 1)  #刪除日期欄位
mlist = []
for item in stock.index:   #建立月份串列
    mlist.append(item.month)
stock['month'] = mlist  #新增月份欄位
#print(stock)
result = stock
for item in result[result.close > 21]:  #收盤價大於21元
    print(item)
    
print(result[(result.index >= "2018-06-01") & (result.index <= "2018-06-30") & (result.close >= 21)])  #六月份大於21元

tmp = result.sort_values(by = 'close', ascending=False)   #依照收盤價排序
print(tmp[:3])  #取收盤價前三高

print(result.loc["2018-06-01":"2018-06-30"])  #只顯示2018六月份

print(result.loc["2018-01-01":"2018-12-31"].groupby('month').close.count())  #2018每個月幾個營業日

print(result.loc["2018-01-01":"2018-12-31"].groupby('month').shares.sum())  #2018每個月累計成交股數

result.loc["2018-01-01":"2018-12-31"].groupby('month').shares.sum().plot()  #2018月累計成交股數圖
pp.ylabel('shares')
pp.title('month of shares')