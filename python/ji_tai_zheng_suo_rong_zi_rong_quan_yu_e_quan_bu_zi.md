# 集臺證所融資融券餘額全部資料


## 使用python3

- test.py

```py
#-*- coding:utf-8 -*-

import requests
from bs4 import BeautifulSoup
import random
import os
import sqlite3
import datetime
import time

# 隨機更改 headers
def head_random():
      for i in range(10):
            hs = ['Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/52.0.2743.116 Safari/537.36',
                   'Mozilla/5.0 (iPhone; U; CPU iPhone OS 3_0 like Mac OS X; en-us) AppleWebKit/528.18 (KHTML, like Gecko) Version/4.0 Mobile/7A341 Safari/528.16',
                   'Mozilla/5.0 (Linux; U; Android 4.1.2; zh-tw; GT-I9300 Build/JZO54K) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.30',
                   'Mozilla/5.0 (iPad; U; CPU OS 3_2 like Mac OS X; en-us) AppleWebKit/531.21.10 (KHTML, like Gecko) Version/4.0.4 Mobile/7B334b Safari/531.21.10'
                   'Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.1; Trident/4.0)']
            hes = {"User-Agent":random.choice(hs)}
      return hes

# 集臺證所融資融券餘額全部資料
def read_network_data(date,from_number):
      url = 'http://www.twse.com.tw/ch/trading/exchange/MI_MARGN/MI_MARGN.php'
      head =head_random()
      payload = {"download":'',
                 "qdate":date,
                 "selectType":"ALL"}
      res = requests.post(url, headers=head, data=payload)
      soup=BeautifulSoup(res.text, "lxml")
      tab = soup.select('tbody')[from_number] # 2個表格之2
      regs=[] # 初始陣列
    
      for i in range(len(tab.select('tr')[0].select('td'))): # 定義2維陣列維度
            regs.append([])
      n=0
      for tr in tab.select('tr'): #擷取表格資料
            for i in range(len(tab.select('tr')[0].select('td'))):
                  if n==0 or n==1 or n==15:      #把文字串中的空白文字去除掉
                        strs=tr.select('td')[i].text
                        strs=str(strs.replace(' ',''))
                        regs[i].append(strs)
                  else:    # 把數字中的千分符號 "," 去除掉
                        ints=tr.select('td')[i].text
                        ints=str(ints.replace(',',''))
                        regs[i].append(ints)
                  n +=1
            n=0
      return regs


def check_db_exists(regs):
      if not os.path.exists('twsedata.db'):   # 檢查twsedata.db是否存在(和此程式碼相同的資料夾)
            conn = sqlite3.connect('twsedata.db')  # 如果沒有，就建立一個空白資料庫，(sqlite3.connect) 也是連結語句，有資料庫就連結，沒有就建立
            sql = "CREATE TABLE 股票(股票代號  char(10) PRIMARY KEY,股票名稱  char(20));"
            conn.execute(sql) #在新資料庫建立空白資料表 (股票)
            sql = "CREATE TABLE  融資融券信用交易統計( \
                                                        日期  datetime, \
                                                        項目  char(20), \
                                                        買進 INTEGER, \
                                                        買出 INTEGER, \
                                                        現金（券）償還 INTEGER, \
                                                        前日餘額 INTEGER, \
                                                        今日餘額 INTEGER);"
            conn.execute(sql) #在新資料建立 空白資料表(融資融券信用交易統計)
            conn.commit() # 儲存
            conn.close() #關閉
            
      n=0
      conn = sqlite3.connect('twsedata.db') # conn和上一個 if ..中的 conn是不一樣的 ，上一個是沒有資料庫時才作用，為局部變數而此處，則為以下使用的全局變數
      for i in regs[0]:  #以爬回來的資料陣列的第一列為迴圈
            try:      #先嘗試查詢 i 變數的資料表是否存在，若是不存在會發出 Exception 的錯誤訊息
                  c = conn.cursor()
                  sql = "select * from [" + i + "]"
                  c.execute(sql) # table does not exist
                  c.close()
            except Exception as e:  #當try 發出錯誤訊息(也就是讀不到  i 資料表)，此時建立一個 i 資料表
                  c = conn.cursor()
                  sql="CREATE TABLE [" + i + "](日期  DATETIME PRIMARY KEY, \
                                                                              資買進  INTEGER, \
                                                                              資賣出 INTEGER, \
                                                                              資現金償還 INTEGER, \
                                                                              資前日餘額 INTEGER, \
                                                                              資今日餘額 INTEGER, \
                                                                              資限額 INTEGER, \
                                                                              券買進  INTEGER, \
                                                                              券賣出 INTEGER, \
                                                                              券現金償還 INTEGER, \
                                                                              券前日餘額 INTEGER, \
                                                                              券今日餘額 INTEGER, \
                                                                              券限額 INTEGER, \
                                                                              資券互抵 INTEGER, \
                                                                              註記 char(5));"
                  c.execute(sql) #建立 i 資料表
                  sql="INSERT INTO 股票(股票代號, 股票名稱) \
                  VALUES ('" + regs[0][n] + "','" + regs[1][n] + "');"
                  c.execute(sql)  # 在股票資料表中寫入股票代號、股票名稱
                  c.close() # 關閉 c 物件 
            n += 1
      conn.commit() 
      conn.close()
def add_data(regs,date):
      conn = sqlite3.connect('twsedata.db')
      for i in range(len(regs[0])):
            try: #檢查某日期是否存在於資料表中，當某日期存在時，跳離這次的循環，這裡指的僅是這次，不是跳離 for 
                  c = conn.cursor()
                  sql="SELECT * FROM [" + regs[0][i] + "] WHERE 日期 = '" + date + "';"
                  c.execute(sql)
                  c.close()
            except Exception as e: #當日期不存在時，寫入資料到資料表中
                  c = conn.cursor()
                  # 日期格式，在sqlite3中，是以文字型態存入，雖然有提供DATETIME，但非以日期型態存入，日後提出，嗯！需再參閱sqlite3的日期處理
                  
                  sql="INSERT INTO [" + regs[0][i] + "](日期,資買進,資賣出,資現金償還,資前日餘額, \
                                                                                         資今日餘額,資限額,券買進,券賣出,券現金償還, \
                                                                                         券前日餘額,券今日餘額,券限額,資券互抵,註記) \
                                                                       VALUES('" + date + "'," + regs[2][i] + "," + regs[3][i] + ", \
                                                                                          " + regs[4][i] + ",  " + regs[5][i] + "," + regs[6][i] + ", \
                                                                                          " + regs[7][i] + "," + regs[8][i] + ",  " + regs[9][i] + ", \
                                                                                          " + regs[10][i] + "," + regs[11][i] + "," + regs[12][i] + ", \
                                                                                          " + regs[13][i] + "," + regs[14][i]+ ",'" + regs[15][i] + "') ;"
                  c.execute(sql)
                  c.close()
            else:
                   break
      conn.commit()
      conn.close()

# 以下為程式的啟始點：
StartDate=input('請輸入開始擷取日期(西元日期如：2016/1/1)：')
EndDate=input('請輸入結束擷取日期(西元日期如：2016/1/31)：')
#處理日期還真的很麻煩
ts=time.time()
StartDate = time.strptime(StartDate, "%Y/%m/%d")
EndDate =time.strptime(EndDate, "%Y/%m/%d")
StartDate = datetime.date(StartDate[0], StartDate[1], StartDate[2])
EndDate =  datetime.date(EndDate[0], EndDate[1], EndDate[2])
RangeDate = datetime.timedelta(days = 1)
while StartDate <= EndDate:
      yy,mm,dd=str(StartDate).split('-')
      dat=datetime.datetime(int(yy), int(mm), int(dd))
      dd=dat.strftime('%Y/%m/%d')
      year=str(int(dd[0:4])-1911)
      date=dd.replace(dd[0:4], year)
      print('目前執行網路擷取',date ,'日的資料，請稍後...')
      re=read_network_data(date,1)
      if re[0][0]=='查無資料':
            print(date,'日，可能為休市日，查無資料。')
            StartDate = StartDate + RangeDate
            continue
      print('檢查是否有新股票加入，請稍後...')
      check_db_exists(regs=re)
      print('將資料寫入資料庫中，請稍後...')
      add_data(regs=re,date=dd)
      StartDate = StartDate + RangeDate
      te = time.time()
      print('截至目前，時間已耗費：', int(te - ts),'秒')

print('執行完畢！')

```

- run

```sh
python3 test.py
```
- 查看 sqlite 內容

```sh
sqlitebrowser twsedata.db
```


```py
#-*- coding:utf-8 -*-

'''
本程序由 Python 3.52 寫成，為作者參考麻辣家族討論區 "python上市櫃三大法人買賣超日報資料下載" 討論內容練習
其內容大部份參考 zyzzyva大大、c_c_lai 大大 、koshi0413 之討論內容，感謝！
本程序會自動生成 Access 的 twsedata.mdb資料庫，及建立各所需之資料表，但只能下載"融資融券彙總 (全部)"資料
ㄟ！沒有信用交易統計資料 ，只建立其資料表
本程序需要 pip 安裝 requests 、BeautifulSoup4、pypyodbc
以及需下載 pywin32 : https://sourceforge.net/projects/pywin32/files/pywin32/Build%20220/
使用方法：只在 input 時 ，輸入開始擷取日期及結束擷取日期即可，第一次執行時，會建構資料庫，而後只增加資料，
每日資料擷取到儲存至資料庫的時間約 21秒(依作者電腦執行計時 )
本程序僅供參考，不負責更新內容，不負責除錯(因為我也是新手，除錯對我來說太#%%$...)，搞壞使用者電腦，也不關作者的事！！
作者：麻辣家族討論區 lpk187
完成日期：2016/9/22
'''

import requests
from bs4 import BeautifulSoup
import random
import os
import win32com.client
import pypyodbc
import datetime
import time

# 隨機更改 headers
def head_random():
      for i in range(10):
            hs = ['Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/52.0.2743.116 Safari/537.36',
                   'Mozilla/5.0 (iPhone; U; CPU iPhone OS 3_0 like Mac OS X; en-us) AppleWebKit/528.18 (KHTML, like Gecko) Version/4.0 Mobile/7A341 Safari/528.16',
                   'Mozilla/5.0 (Linux; U; Android 4.1.2; zh-tw; GT-I9300 Build/JZO54K) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.30',
                   'Mozilla/5.0 (iPad; U; CPU OS 3_2 like Mac OS X; en-us) AppleWebKit/531.21.10 (KHTML, like Gecko) Version/4.0.4 Mobile/7B334b Safari/531.21.10'
                   'Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.1; Trident/4.0)']
            hes = {"User-Agent":random.choice(hs)}
      return hes

# 集臺證所融資融券餘額全部資料
def read_network_data(date,from_number):
      url = 'http://www.twse.com.tw/ch/trading/exchange/MI_MARGN/MI_MARGN.php'
      head =head_random()
      payload = {"download":'',
                 "qdate":date,
                 "selectType":"ALL"}
      res = requests.post(url, headers=head, data=payload)
      soup=BeautifulSoup(res.text, "lxml")
      tab = soup.select('tbody')[from_number] # 2個表格之2
      regs=[] # 初始陣列
    
      for i in range(len(tab.select('tr')[0].select('td'))): # 定義2維陣列維度
            regs.append([])

      for tr in tab.select('tr'): #擷取表格資料
            for i in range(len(tab.select('tr')[0].select('td'))):
                  regs[i].append(tr.select('td')[i].text)
      return regs

def conn_db():
      Conn = win32com.client.Dispatch("ADODB.Connection")
      strMDB ='Provider=Microsoft.ACE.OLEDB.12.0; Data Source=' + os.getcwd() + '\\twsedata.mdb;'
      Conn.ConnectionString = strMDB
      Conn.Open()
      return Conn

def check_db_exists(regs):
      if not os.path.exists('twsedata.mdb'):
            pypyodbc.win_create_mdb( os.getcwd() + '\\twsedata.mdb')
            Conn=conn_db()
            sql = "CREATE TABLE 股票(股票代號  char(10) PRIMARY KEY,股票名稱  char(20));"
            Conn.Execute(sql)
            sql = "CREATE TABLE  融資融券信用交易統計(日期  datetime,項目  char(20),買進 INTEGER,買出 INTEGER,現金（券）償還 INTEGER,前日餘額 INTEGER,今日餘額 INTEGER);"
            Conn.Execute(sql)
      n = 0
      Conn=conn_db()
      for i in regs[0]:
            try:
                  Conn.Execute('select * from ' + i); # table does not exist
            except Exception as e:
                  sql="CREATE TABLE " + i + "(日期  datetime,"
                  sql= sql + "資買進  INTEGER,"
                  sql= sql + "資賣出 INTEGER,"
                  sql= sql + "資現金償還 INTEGER,"
                  sql= sql + "資前日餘額 INTEGER,"
                  sql= sql + "資今日餘額 INTEGER,"
                  sql= sql + "資限額 INTEGER,"
                  sql= sql + "券買進  INTEGER,"
                  sql= sql + "券賣出 INTEGER,"
                  sql= sql + "券現金償還 INTEGER,"
                  sql= sql + "券前日餘額 INTEGER,"
                  sql= sql + "券今日餘額 INTEGER,"
                  sql= sql + "券限額 INTEGER,"
                  sql= sql + "資券互抵 INTEGER,"
                  sql= sql + "註記 char(5));"
                  Conn.Execute(sql)
                  rs = win32com.client.Dispatch('ADODB.RecordSet')
                  rs.ActiveConnection = Conn
                  rs.Open('select * from 股票', Conn, 3, 3)
                  rs.AddNew()
                  rs.Fields.Item(0).Value = regs[0][n]
                  rs.Fields.Item(1).Value = regs[1][n]
                  rs.Update()
            n += 1

def add_data(regs,date):
      Conn=conn_db()
      for i in range(len(regs[0])):
            rs = win32com.client.Dispatch('ADODB.RecordSet')
            rs.ActiveConnection = Conn
            sql='select * from ' + regs[0][i]
            rs.Open(sql, Conn, 3, 3)
            for j in range(2,len(regs)):
                  if j==2:
                        rs1 = win32com.client.Dispatch('ADODB.RecordSet')
                        rs1.ActiveConnection = Conn
                        sql="SELECT * FROM " + regs[0][i] + " WHERE 日期 = #" + date + "#;"
                        rs1.Open(sql,Conn,3,3)
                        if rs1.EOF:
                              rs.AddNew()
                              rs.Fields.Item(0).Value = date
                              rs.Fields.Item(j-1).Value = regs[j][i]
                              rs1.Close()
                        else:
                              rs1.Close()
                              break
                  rs.Fields.Item(j-1).Value = regs[j][i]
            rs.Update()

# 以下為程式的啟始點：
StartDate=input('請輸入開始擷取日期(西元日期如：2016/1/1)：')
EndDate=input('請輸入結束擷取日期(西元日期如：2016/1/31)：')
#處理日期還真的很麻煩
ts=time.time()
StartDate = time.strptime(StartDate, "%Y/%m/%d")
EndDate =time.strptime(EndDate, "%Y/%m/%d")
StartDate = datetime.date(StartDate[0], StartDate[1], StartDate[2])
EndDate =  datetime.date(EndDate[0], EndDate[1], EndDate[2])
RangeDate = datetime.timedelta(days = 1)
while StartDate <= EndDate:
      yy,mm,dd=str(StartDate).split('-')
      dat=datetime.datetime(int(yy), int(mm), int(dd))
      dd=dat.strftime('%Y/%m/%d')
      year=str(int(dd[0:4])-1911)
      date=dd.replace(dd[0:4], year)
      print('目前執行網路擷取',date ,'日的資料，請稍後...')
      re=read_network_data(date,1)
      if re[0][0]=='查無資料':
            print(date,'日，可能為休市日，查無資料。')
            StartDate = StartDate + RangeDate
            continue
      print('檢查是否有新股票加入，請稍後...')
      check_db_exists(regs=re)
      print('將資料寫入資料庫中，請稍後...')
      add_data(regs=re,date=dd)
      StartDate = StartDate + RangeDate
      te = time.time()
      print('截至目前，時間已耗費：', int(te - ts),'秒')

print('執行完畢！')
```
