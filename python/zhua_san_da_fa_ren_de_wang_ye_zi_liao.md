# 抓三大法人的網頁資料


```py
抓三大法人統計表，出處來自大數學堂http://course.largitdata.com/course/31/
只能說david是神，有機會看到要拜

第一步是抓三大法人的網頁內容
import requests
res = requests.get('http://www.twse.com.tw/ch/trading/fund/BFI82U/BFI82U.php?report1=day&input_date=104%2F07%2F22&mSubmit=%ACd%B8%DF&yr=2015&w_date=20150720&m_date=20150701')
res.encoding = 'big5'
print res.text

用beautifulsoup分析
import requests
from bs4 import BeautifulSoup
res = requests.get('http://www.twse.com.tw/ch/trading/fund/BFI82U/BFI82U.php?report1=day&input_date=104%2F07%2F22&mSubmit=%ACd%B8%DF&yr=2015&w_date=20150720&m_date=20150701')
res.encoding = 'big5'
soup = BeautifulSoup (res.text)
print soup.select('.board_trad')

只抓tr裡面的td
import requests
from bs4 import BeautifulSoup
res = requests.get('http://www.twse.com.tw/ch/trading/fund/BFI82U/BFI82U.php?report1=day&input_date=104%2F07%2F22&mSubmit=%ACd%B8%DF&yr=2015&w_date=20150720&m_date=20150701')
res.encoding = 'big5'
soup = BeautifulSoup (res.text)
for tr in soup.select('.board_trad tr')[2:]:
     for td in tr.select('td'):
        print td.text
最後的整理
import requests
from bs4 import BeautifulSoup
res = requests.get('http://www.twse.com.tw/ch/trading/fund/BFI82U/BFI82U.php?report1=day&input_date=104%2F07%2F22&mSubmit=%ACd%B8%DF&yr=2015&w_date=20150720&m_date=20150701')
res.encoding = 'big5'
soup = BeautifulSoup (res.text)
for tr in soup.select('.board_trad tr')[2:]:
    td = tr.select('td')
    print td[0].text,td[1].text,td[2].text,td[3].text

時間str是整理成字串
from datetime import date,timedelta
today = date.today()
for i in range(1,10):
    today = today + timedelta(days=-1)
    print str(today).split('-')

設定多組的時間，'-'.join代表插入'-'且只顯示括號內的值
from datetime import date,timedelta
today = date.today()
for i in range(1,10):
    today = today + timedelta(days=-1)
    dayary = str(today).split('-')
    print '-'.join([str(int(dayary[0]) - 1911), dayary[1],dayary[2]])

抓取多天的數據
import requests
from bs4 import BeautifulSoup
from datetime import date,timedelta
#res = requests.get('http://www.twse.com.tw/ch/trading/fund/BFI82U/BFI82U.php?report1=day&input_date=104%2F07%2F22&mSubmit=%ACd%B8%DF&yr=2015&w_date=20150720&m_date=20150701')
url = 'http://www.twse.com.tw/ch/trading/fund/BFI82U/BFI82U.php?report1=day&input_date={0}&mSubmit=%ACd%B8%DF&yr=2015&w_date=20150720&m_date=20150701'
def getTradeValue(dt):
    res = requests.get(url.format(dt))
    res.encoding = 'big5'
    soup = BeautifulSoup (res.text)
    for tr in soup.select('.board_trad tr')[2:]:
        td = tr.select('td')
        print td[0].text,td[1].text,td[2].text,td[3].text
       

today = date.today()
for i in range(1,10):
    today = today + timedelta(days=-1)
    dayary = str(today).split('-')
    dt = '%2F'.join([str(int(dayary[0]) - 1911), dayary[1],dayary[2]])
    getTradeValue(dt)

加上日期的資訊
import requests
from bs4 import BeautifulSoup
from datetime import date,timedelta
#res = requests.get('http://www.twse.com.tw/ch/trading/fund/BFI82U/BFI82U.php?report1=day&input_date=104%2F07%2F22&mSubmit=%ACd%B8%DF&yr=2015&w_date=20150720&m_date=20150701')
url = 'http://www.twse.com.tw/ch/trading/fund/BFI82U/BFI82U.php?report1=day&input_date={0}&mSubmit=%ACd%B8%DF&yr=2015&w_date=20150720&m_date=20150701'
def money_conversion(input_ele):
    return int(''.join(input_ele.split(',')))

def getTradeValue(today):
    dayary = str(today).split('-')
    dt = '%2F'.join([str(int(dayary[0]) - 1911), dayary[1],dayary[2]])
    res = requests.get(url.format(dt))
    res.encoding = 'big5'
    soup = BeautifulSoup (res.text)
    for tr in soup.select('.board_trad tr')[2:]:
        td = tr.select('td')
        print td[0].text, money_conversion(td[1].text),money_conversion(td[2].text),money_conversion(td[3].text),today
       

today = date.today()
for i in range(1,10):
    today = today + timedelta(days=-1)

    getTradeValue(today)

用sqlite存到檔案，檔案要寫絕對路徑
import sqlite3 as lite
con = lite.connect('c:/finace.sqlite')
cur = con.cursor()
cur.execute("select * from InvestorTradingValue")
ret = cur.fetchone()
print ret
#cur.execute("insert into InvestorTradingValue(item,total_but,total_sell,difference,date) values('foreign',20,30,-10,'2013-05-05')")
#con.commit()
con.close()   

存檔資料及取檔資料(mark處是取檔資料)
import sqlite3 as lite
con = lite.connect('c:/finace.sqlite')
cur = con.cursor()
cur.execute("select * from InvestorTradingValue")
ret = cur.fetchone()
print ret
#cur.execute("insert into InvestorTradingValue(item,total_buy,total_sell,difference,date) values('foreign',20,30,-10,'2013-05-05')")
#con.commit()
con.close()   
```

## 存到sqlite中
```py
import requests
from bs4 import BeautifulSoup
from datetime import date,timedelta
import sqlite3 as lite

#cur.execute("insert into InvestorTradingValue(item,total_buy,totol_sell,difference,date) values('foreign',20,30,-10,'2013-05-05')")
#con.commit()
con.close()   
#res = requests.get('http://www.twse.com.tw/ch/trading/fund/BFI82U/BFI82U.php?report1=day&input_date=104%2F07%2F22&mSubmit=%ACd%B8%DF&yr=2015&w_date=20150720&m_date=20150701')
url = 'http://www.twse.com.tw/ch/trading/fund/BFI82U/BFI82U.php?report1=day&input_date={0}&mSubmit=%ACd%B8%DF&yr=2015&w_date=20150720&m_date=20150701'
sql = "insert into InvestorTradingValue(item,total_buy,totol_sell,difference,date) values(?,?,?,?,?)"
def money_conversion(input_ele):
    return int(''.join(input_ele.split(',')))

def getTradeValue(cur,today):
    dayary = str(today).split('-')
    dt = '%2F'.join([str(int(dayary[0]) - 1911), dayary[1],dayary[2]])
    res = requests.get(url.format(dt))
    res.encoding = 'big5'
    soup = BeautifulSoup (res.text)
    for tr in soup.select('.board_trad tr')[2:]:
        td = tr.select('td')
        ret = [td[0].text, money_conversion(td[1].text),money_conversion(td[2].text),money_conversion(td[3].text),today]
        cur.execute(sql,ret)
con = lite.connect('c:/finace.sqlite')
cur = con.cursor()


today = date.today()
for i in range(1,10):
    today = today + timedelta(days=-1)
    getTradeValue(cur,today)
con.commit()   
con.close()    
```